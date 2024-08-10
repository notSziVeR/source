#include "stdafx.h"
#ifdef __ENABLE_MISSION_MANAGER__
#include "char.h"
#include "utils.h"

#include <boost/noncopyable.hpp>
#include <fstream>

#include "MissionManager.hpp"

#include "../../libxml/Document.hpp"

namespace
{
	const std::string sMissionType("mission_manager.num_{}");
	const std::string sMissionLimit("mission_manager.num_{}_limit");
	const std::string sMissionFlag("mission_manager.num_{}_task_{}");

	const std::string sBroadcastClearInterface("MissionManager_ClearInterface");
	const std::string sBroadcastTaskInfo("MissionManager_TaskInfo {} {} {} {} {} {}");
	const std::string sBroadcastTaskRefresh("MissionManager_TaskRefresh {} {} {}");
	const std::string sBroadcastReward("MissionManager_RewardInfo {} {} {} {} {}");
	const std::string sBroadcastRefresh("MissionManager_Refresh");

	const std::string sConfigFile("locale/germany/missions-manager.xml");
}

// SMissionObject definitions
BYTE CMissionManager::SMissionObject::GetTargetPerc(LPCHARACTER ch, const DWORD& dwVnum) const
{
	int iValue = ch->GetQuestFlag(fmt::format(sMissionFlag, dwVnum, iTaskNum));
	return MINMAX(0, (iValue * 100 / sObjective.sObjectiveCount), 100);
}

void CMissionManager::SMissionObject::GiveReward(LPCHARACTER ch)
{
	for (const auto& rReward : v_rewards)
	{
		ch->AutoGiveItem(rReward.dwItemVnum, rReward.wItemCount);
	}
}

void CMissionManager::SMissionObject::UpdateObjective(LPCHARACTER ch, const DWORD& dwVnum)
{
	int iValue = ch->GetQuestFlag(fmt::format(sMissionFlag, dwVnum, iTaskNum));

	ch->SetQuestFlag(fmt::format(sMissionFlag, dwVnum, iTaskNum), iValue + 1);
}

void CMissionManager::SMissionObject::ClearObjective(LPCHARACTER ch, const DWORD& dwVnum)
{
	if (ch->GetQuestFlag(fmt::format(sMissionFlag, dwVnum, iTaskNum)))
		ch->SetQuestFlag(fmt::format(sMissionFlag, dwVnum, iTaskNum), 0);
}

void CMissionManager::Create()
{
	Clear();
	Load(sConfigFile);
}

bool CMissionManager::Load(const std::string& fileName) {
	try {
		xml::Document document(fileName);

		auto rootNode = xml::TryGetNode(&document, "Missions");
		if (!rootNode)
		{
			sys_err(fmt::format("Root node <Missions/> in {} not found.", fileName).c_str());
			return false;
		}

		// Load Items
		for (auto node = rootNode->first_node();
			node;
			node = node->next_sibling()) {
			const auto& name = xml::GetName(node);
			if (name == "Mission") {
				// Register mission!
				RegisterPool(node);
			}
		}
	}
	catch (const std::exception& e)
	{
		sys_err(e.what());
		return false;
	}

	return true;
}

void CMissionManager::Clear()
{
	m_task_pool.clear();
}

void CMissionManager::RegisterPool(const xml::Node* root)
{
	BOOST_ASSERT_MSG(root, "Node nullptr.");

	uint32_t id;
	xml::GetAttribute(root, "id", id);

	std::string name;
	xml::GetAttribute(root, "name", name);

	uint32_t time;
	xml::GetAttribute(root, "time", time);

	auto iFind = m_task_pool.find(id);
	if (iFind != m_task_pool.end())
	{
		sys_err(fmt::format("Mission Manager: Pool already exist! Pool vnum: {}", id).c_str());
		return;
	}

	SQuestsList missions = {};
	for (auto node = root->first_node();
		node;
		node = node->next_sibling()) {
		if (xml::GetName(node) == "Target") {
			ParseMissions(node, missions);
		}
	}

	m_task_pool[id] = sMissionsPool{ time, missions };
	sys_log(0, fmt::format("Mission Manager: Pool registred sucessfully! Vnum: {}", id).c_str());
}

void CMissionManager::ParseMissions(const xml::Node* root, SQuestsList& missions)
{
	BOOST_ASSERT_MSG(root, "Node nullptr.");

	SMissionObject rMission = {};
	rMission.iTaskNum = static_cast<int>(missions.size());

	xml::GetAttribute(root, "id", rMission.sObjective.dwObjectiveVnum);
	xml::GetAttribute(root, "count", rMission.sObjective.sObjectiveCount);

	MissionNS::SReward reward = {};
	for (auto node = root->first_node();
		node;
		node = node->next_sibling()) {
		if (xml::GetName(node) == "Reward") {
			ParseRewards(node, reward);
			rMission.AddReward(reward);
		}
	}

	missions.push_back(rMission);
	sys_log(0, fmt::format("Mission Manager: New mission added sucessfully! Vnum: {}, List count: {}", rMission.iTaskNum, static_cast<int>(missions.size())).c_str());
}

void CMissionManager::ParseRewards(const xml::Node* root, MissionNS::SReward& rewards)
{
	BOOST_ASSERT_MSG(root, "Node nullptr.");
	
	xml::GetAttribute(root, "vnum", rewards.dwItemVnum);
	xml::GetAttribute(root, "count", rewards.wItemCount);

	sys_log(0, fmt::format("Missions Manager: Reward was parsed: {} | {}", rewards.dwItemVnum, rewards.wItemCount).c_str());
}

void CMissionManager::TriggerEvent(LPCHARACTER ch, const MissionNS::SObjective& eType)
{
	if (!ch || !ch->IsPC()) return;

	// Checking if PC has mission on
	if (!HasEnabled(ch)) return;

	for (const auto& [iKey, rInformation] : m_task_pool)
	{
		if (!ch->GetQuestFlag(fmt::format(sMissionType, iKey))) continue;
		
		if (get_global_time() > ch->GetQuestFlag(fmt::format(sMissionLimit, iKey))) continue;

		SQuestsList* rRef = nullptr;
		if (!GetPoolByTask(iKey, &rRef))
		{
			sys_err(fmt::format("Missions Manager: Mission for provided difficulity not found. Num: {}", iKey).c_str());
			return;
		}

		for (auto& rElement : *(rRef))
		{
			if (rElement == eType)
			{
				if (rElement.GetTargetPerc(ch, iKey) == 100) continue;

				rElement.UpdateObjective(ch, iKey);

				if (rElement.GetTargetPerc(ch, iKey) == 100)
				{
					// Add reward
					rElement.GiveReward(ch);

					// Chyecking player's total progress
					if (CalculateTotalProgress(*rRef, ch, iKey))
					{
						ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface);
						ch->SetQuestFlag(fmt::format(sMissionType, iKey), 0);
						return;
					}
				}
			}

			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastTaskRefresh, iKey, rElement.iTaskNum, rElement.GetTargetPerc(ch, iKey)).c_str());
		}
	}
}

bool CMissionManager::HasEnabled(LPCHARACTER ch)
{
	if (!ch) return false;

	for (const auto& [iKey, rElement] : m_task_pool)
	{
		if (ch->GetQuestFlag(fmt::format(sMissionType, iKey)) && (get_global_time() < ch->GetQuestFlag(fmt::format(sMissionLimit, iKey))))
			return true;
	}

	return false;
}

bool CMissionManager::CalculateTotalProgress(const SQuestsList& rPool, LPCHARACTER ch, const DWORD& dwVnum)
{
	bool bProgress = true;
	std::for_each(rPool.begin(), rPool.end(), [&bProgress, &ch, &dwVnum](const SMissionObject& rObj) { if (rObj.GetTargetPerc(ch, dwVnum) < 100) bProgress = false; });

	return bProgress;
}

void CMissionManager::BroadcastMission(LPCHARACTER ch, bool bClear, const DWORD iElement, const int iNum)
{
	if (!ch || !HasEnabled(ch)) return;

	if (bClear)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface);
	}

	for (const auto& [iKey, rInformation] : m_task_pool)
	{
		if (!ch->GetQuestFlag(fmt::format(sMissionType, iKey))) continue;

		if (get_global_time() > ch->GetQuestFlag(fmt::format(sMissionLimit, iKey))) continue;

		SQuestsList* rRef_Pool = nullptr;
		if (!GetPoolByTask(iKey, &rRef_Pool))
		{
			sys_err(fmt::format("Missions Manager: Mission for provided difficulity not found. Num: {}", iKey).c_str());
			return;
		}

		for (const auto& rElement : *rRef_Pool)
		{
			// Task Info
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastTaskInfo, iKey, std::max<time_t>(0, ch->GetQuestFlag(fmt::format(sMissionLimit, iKey)) - get_global_time()), rElement.iTaskNum, rElement.GetTargetPerc(ch, iKey), rElement.sObjective.dwObjectiveVnum, rElement.sObjective.sObjectiveCount).c_str());

			// Rewards
			int i = 0;
			for (const auto& rReward : rElement.v_rewards)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastReward, iKey, rElement.iTaskNum, i, rReward.dwItemVnum, rReward.wItemCount).c_str());
				i++;
			}
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastRefresh);
	}
}

bool CMissionManager::RegisterMission(LPCHARACTER ch, const DWORD& dwVnum)
{
	if (!ch) return false;

	// User cannot register mission if is already in
	if (ch->GetQuestFlag(fmt::format(sMissionType, dwVnum)) && (get_global_time() < ch->GetQuestFlag(fmt::format(sMissionLimit, dwVnum))))
	{
		return false;
	}

	SQuestsList* rRef = nullptr;
	if (!GetPoolByTask(dwVnum, &rRef))
	{
		sys_err(fmt::format("Missions Manager: Mission for provided difficulity not found. Num: {}", dwVnum).c_str());
		return false;
	}

	ch->SetQuestFlag(fmt::format(sMissionType, dwVnum), 1);
	auto fIt = m_task_pool.find(dwVnum);
	if (fIt != m_task_pool.end())
		ch->SetQuestFlag(fmt::format(sMissionLimit, dwVnum), get_global_time() + fIt->second.first);

	// Lets clear current progress!
	for (auto& rElement : *rRef)
	{
		rElement.ClearObjective(ch, dwVnum);
	}

	//Broadcast info
	BroadcastMission(ch, true);
	return true;
}

/// Find ///
bool CMissionManager::GetPoolByTask(const DWORD& dwVnum, SQuestsList** rRef)
{
	auto fIt = m_task_pool.find(dwVnum);
	if (fIt == m_task_pool.end())
	{
		return false;
	}

	*rRef = &(fIt->second).second;
	return true;
}
/// ///
#endif
