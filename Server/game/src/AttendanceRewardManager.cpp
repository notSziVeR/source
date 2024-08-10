#include "stdafx.h"
#ifdef __ENABLE_ATTENDANCE_EVENT__
#include "char.h"
#include "questmanager.h"

#include <boost/noncopyable.hpp>
#include <fstream>

#include "../../libxml/Document.hpp"

#include "AttendanceRewardManager.hpp"

namespace
{
	const std::string sEventFlag("Attendance_Event");

	const std::string sAttendanceFlag("attendance_manager.y_{}_m_{}");
	const std::string sActiveYearFlag("attendance_manager.active_year");

	const std::string sBroadcastStatus("Attendance_Status {}");
	const std::string sBroadcastClearInterface("Attendance_ClearInterface");
	const std::string sBroadcastBasicInformation("Attendance_BasicInfo {} {}");
	const std::string sBroadcastCreateInterface("Attendance_Item {} {} {} {} {}");
	const std::string sBroadcastRefreshItem("Attendance_RefreshItem {} {}");
	const std::string sBroadcastRefresh("Attendance_Refresh");

	const std::string sConfigFile("locale/germany/attendance-manager.xml");
}

void CAttendanceRewardManager::Create()
{
	Clear();
	Load(sConfigFile);
}

void CAttendanceRewardManager::Clear()
{
	m_att_pool.clear();
}

bool CAttendanceRewardManager::Load(const std::string& fileName)
{
	try {
		xml::Document document(fileName);

		auto rootNode = xml::TryGetNode(&document, "Attendance-Manager");
		if (!rootNode)
		{
			sys_err(fmt::format("Root node <Attendance-Manager/> in {} not found.", fileName).c_str());
			return false;
		}

		// Load Items
		for (auto node = rootNode->first_node();
			node;
			node = node->next_sibling()) {
			const auto& name = xml::GetName(node);
			if (name == "Month") {
				// Register month!
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

void CAttendanceRewardManager::RegisterPool(const xml::Node* root)
{
	BOOST_ASSERT_MSG(root, "Node nullptr.");

	uint8_t iMonth;
	xml::GetAttribute<>(root, "month", iMonth);

	auto iFind = m_att_pool.find(static_cast<AttendanceNS::EMonths>(iMonth));
	if (iFind != m_att_pool.end())
	{
		sys_err(fmt::format("Attendance Manager: Pool already exist! Pool vnum: {}", iMonth).c_str());
		return;
	}

	AttendanceNS::SReward sReward = {};

	xml::GetAttribute(root, "reward_vnum", sReward.dwItemVnum);
	xml::GetAttribute(root, "reward_count", sReward.wItemCount);

	sAttPool rPair = { sReward, {} };

	for (auto node = root->first_node();
		node;
		node = node->next_sibling()) {
		if (xml::GetName(node) == "Day") {
			ParseDays(node, rPair);
		}
	}

	m_att_pool[static_cast<AttendanceNS::EMonths>(iMonth)] = sAttPool{ rPair };
	sys_log(0, fmt::format("Attendance Manager: Pool registred sucessfully! Vnum: {}", iMonth).c_str());
}

void CAttendanceRewardManager::ParseDays(const xml::Node* root, sAttPool& rRewards)
{
	BOOST_ASSERT_MSG(root, "Node nullptr.");

	AttendanceNS::SReward reward = {};

	xml::GetAttribute(root, "reward_vnum", reward.dwItemVnum);
	xml::GetAttribute(root, "reward_count", reward.wItemCount);

	rRewards.second.push_back(reward);
	sys_log(0, fmt::format("Attendance Manager: Reward to day {} added sucessfully!", rRewards.second.size()).c_str());
}

void CAttendanceRewardManager::Broadcast(LPCHARACTER ch, bool bClear)
{
	if (!ch) return;

	// Checking if event is active
	if (!quest::CQuestManager::Instance().GetEventFlag(sEventFlag)) return;

	if (bClear)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface);
	}

	auto iFit = m_att_pool.find(GetCurrentMonth());
	if (iFit != m_att_pool.end())  
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBasicInformation, iFit->first, GetCurrentDay()));

		int i = 1;
		for (const auto& rDays : iFit->second.second)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastCreateInterface, i, static_cast<int>(IsCollected(ch, i)), rDays.dwItemVnum, rDays.wItemCount, 0));
			i++;
		}

		// Broadcast Special Reward
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastCreateInterface, i++, static_cast<int>(CalculateTotalCollected(ch)), iFit->second.first.dwItemVnum, iFit->second.first.wItemCount, 1));
		
		// Broadcast Status
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastStatus, quest::CQuestManager::Instance().GetEventFlag(sEventFlag)));

		// Refresh Interface
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastRefresh);
	}
}

void CAttendanceRewardManager::CollectReward(LPCHARACTER ch)
{
	if (!ch) return;

	// Checking if event is active
	if (!quest::CQuestManager::Instance().GetEventFlag(sEventFlag)) return;

	if (IsCollected(ch)) return;

	auto iFit = m_att_pool.find(GetCurrentMonth());
	if (iFit != m_att_pool.end())
	{
		auto& vRewards = iFit->second.second;

		if (GetCurrentDay() > vRewards.size()) return;

		ch->AutoGiveItem(vRewards[GetCurrentDay() - 1].dwItemVnum, vRewards[GetCurrentDay() - 1].wItemCount);

		ch->SetQuestFlag(fmt::format(sAttendanceFlag, GetCurrentYear(), GetCurrentMonth()),
			ch->GetQuestFlag(fmt::format(sAttendanceFlag, GetCurrentYear(), GetCurrentMonth())) | (1 << (GetCurrentDay()))
			);

		// We have to send an update!
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRefreshItem, GetCurrentDay(), static_cast<int>(IsCollected(ch))));

		// Lets check if we can get special reward!
		if (CalculateTotalCollected(ch))
		{
			ch->AutoGiveItem(iFit->second.first.dwItemVnum, iFit->second.first.wItemCount);

			// We have to send an update!
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRefreshItem, GetCurrentDay() + 1, 1));
		}
	}

	// Refresh Interface
	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastRefresh);
}

bool CAttendanceRewardManager::IsCollected(LPCHARACTER ch, int iNum)
{
	int iValue = ch->GetQuestFlag(fmt::format(sAttendanceFlag, GetCurrentYear(), GetCurrentMonth()));
	
	iNum = iNum == -1 ? GetCurrentDay(): iNum;
	return (iValue & (1 << (iNum))) != 0;
}

bool CAttendanceRewardManager::CalculateTotalCollected(LPCHARACTER ch)
{
	auto iFit = m_att_pool.find(GetCurrentMonth());
	if (iFit != m_att_pool.end())
	{
		auto& vRewards = iFit->second.second;

		auto fIte = std::find_if(vRewards.begin(), vRewards.end(), [&, iNum = 1](const auto& rVal) mutable { return !IsCollected(ch, iNum++); });
		return (fIte == vRewards.end());
	}

	return false;
}
#endif
