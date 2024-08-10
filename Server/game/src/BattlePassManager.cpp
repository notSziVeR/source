#include "stdafx.h"
#ifdef __BATTLE_PASS_ENABLE__
#include "char.h"
#include "desc_client.h"
#include "utils.h"
#include "BattlePassManager.hpp"
#include "config.h"

#include <boost/format.hpp>

namespace
{
const std::string sActiveYear("battle_pass.active_year");
const std::string sActiveMonth("battle_pass.active_month");
const std::string sGrantedReward("battle_pass.granted_reward");
const std::string sDiffLevel("battle_pass.diff_level");
const std::string sTaskFlag("battle_pass.task_%d_num_%d");
const std::string sFinishDT("battle_pass.finish_dt");

const std::string sBroadcastClearInterface("BattlePass_ClearInterface");
const std::string sBroadcastBasic("BattlePass_BasicInfo %d %d %d %d");
const std::string sBroadcastRewards("BattlePass_MajorReward %u %u");
const std::string sBroadcastTaskInfo("BattlePass_TaskInfo %s %s %u %d");
const std::string sBroadcastTaskRewards("BattlePass_TaskReward %d %u %u");

const std::string sBroadcastHighScore("BattlePass_HighScore %d %s %u");
const size_t stHighScoreCount = 10;

std::map<std::string, std::string> m_translation_unit =
{
	{"TASK_COMPLETED", "BATTLE_PASS_TASK_COMPLETED"},
	{"CAN_COLLECT", "BATTLE_PASS_CAN_COLLECT_REWARD"},
	{"REWARD_COLLECTED", "BATTLE_PASS_REWARD_COLLECTED"},
};
}

// SQuestObject definitions
BYTE CBattlePassManager::SQuestObject::GetTargetPerc(LPCHARACTER ch) const
{
	int iValue = ch->GetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str());
	return MINMAX(0, (iValue * 100 / sObjective.sObjectiveCount), 100);
}

void CBattlePassManager::SQuestObject::GiveReward(LPCHARACTER ch)
{
	for (const auto & rReward : v_rewards)
	{
		ch->AutoGiveItem(rReward.dwItemVnum, rReward.wItemCount);
	}
}

void CBattlePassManager::SQuestObject::UpdateObjective(LPCHARACTER ch, const uint64_t& dwObjectiveCount)
{
	int iValue = ch->GetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str());

	// Support for custom behaviour
	switch (sObjective.eObjective)
	{
	case BattlePassNS::EObjectives::TYPE_KILL:
	case BattlePassNS::EObjectives::TYPE_USE:
	case BattlePassNS::EObjectives::TYPE_REFINE:
	case BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY:
	case BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY:
	case BattlePassNS::EObjectives::TYPE_NPC_GET_MONEY:
	case BattlePassNS::EObjectives::TYPE_NPC_SPEND_MONEY:
	case BattlePassNS::EObjectives::TYPE_NPC_SELL_ITEM:
	case BattlePassNS::EObjectives::TYPE_NPC_SELL_ALL:
	case BattlePassNS::EObjectives::TYPE_DESTROY_ITEM:
		ch->SetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str(), iValue + dwObjectiveCount);
		break;
	default:
		ch->SetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str(), iValue + 1);
		break;
	}
}

void CBattlePassManager::SQuestObject::ClearObjective(LPCHARACTER ch)
{
	if (ch->GetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str()))
		ch->SetQuestFlag((boost::format{sTaskFlag} % ch->GetQuestFlag(sDiffLevel) % iTaskNum).str(), 0);
}

void CBattlePassManager::SQuestObject::PrintData()
{
	sys_log(0, "Printing data for task: %s", sTaskName.c_str());
	// Objective
	sys_log(0, "Objective number: %d, objective vnum: %u, objective count: %u", static_cast<int>(sObjective.eObjective), sObjective.dwObjectiveVnum, sObjective.sObjectiveCount);
	// Rewards
	int i = 1;
	for (const auto & rReward : v_rewards)
	{
		sys_log(0, "Reward nr %d: Vnum: %u, Count: %u", i++, rReward.dwItemVnum, rReward.wItemCount);
	}
}
//

CBattlePassManager::CBattlePassManager()
{
	for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
		m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)] = std::vector<TBattlePassHighScore>(stHighScoreCount, TBattlePassHighScore{});

	TBattlePassHighScore pack{};
	pack.bSubHeader = SUBHEADER_BATTLEPASS_GD_REQUEST_ITEMS;

	db_clientdesc->DBPacket(HEADER_GD_BATTLEPASS_RANKING, 0, &pack, sizeof(pack));
}

void CBattlePassManager::RegisterPool(const BattlePassNS::EDiffLevel & ePoolNum)
{
	auto iFind = m_task_pool.find(ePoolNum);
	if (iFind != m_task_pool.end())
	{
		sys_err("Pool already exists! Pool num: %d", static_cast<int>(ePoolNum));
		return;
	}

	m_task_pool[ePoolNum] = SQuestsPool{};
	sys_log(0, "Pool registered sucessfully! Num: %d", static_cast<int>(ePoolNum));
}

void CBattlePassManager::RegisterMonth(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth)
{
	auto iFind = m_task_pool.find(ePoolNum);
	if (iFind == m_task_pool.end())
	{
		sys_err("Pool doesn't exist! Pool num: %d", static_cast<int>(ePoolNum));
		return;
	}

	m_task_pool[ePoolNum][eMonth] = {SRewards{}, sQuestsList{}};
	sys_log(0, "Month registered sucessfully! Pool: %d, Num: %d",  static_cast<int>(ePoolNum), static_cast<int>(eMonth));
}

void CBattlePassManager::AddMonthReward(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const BattlePassNS::SReward & rReward)
{
	SRewards * rRef = nullptr;
	if (!GetRewardByMonth(ePoolNum, eMonth, &rRef))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", static_cast<int>(ePoolNum), static_cast<int>(eMonth));
		return;
	}

	rRef->push_back(rReward);
	sys_log(0, "Reward added sucessfully! Num: %d, Vnum: %u, Count: %u", static_cast<int>(ePoolNum), rReward.dwItemVnum, rReward.wItemCount);
}

void CBattlePassManager::AddNewTask(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const std::string & sTaskName, const std::string & sTaskDesc, const BattlePassNS::SObjective & rObjective)
{
	auto iFind = m_task_pool.find(ePoolNum);
	if (iFind == m_task_pool.end())
	{
		sys_err("Pool doesnt't exist! Pool num: %d", static_cast<int>(ePoolNum));
		return;
	}

	auto iFindDeep = (iFind->second).find(eMonth);
	if (iFindDeep == (iFind->second).end())
	{
		sys_err("Month doesnt't exist! Pool num: %d, Month: %u", static_cast<int>(ePoolNum), static_cast<size_t>(eMonth));
		return;
	}

	auto & rQuestList = (iFindDeep->second).second;
	rQuestList.push_back(SQuestObject(static_cast<int>(rQuestList.size()), sTaskName, sTaskDesc, rObjective));

	sys_log(0, "New task added sucessfully! Num: %u, List count: %u", static_cast<size_t>(ePoolNum), rQuestList.size());
}

void CBattlePassManager::AddTaskReward(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const size_t ttTaskNum, const BattlePassNS::SReward & rReward)
{
	sQuestsList * rRef = nullptr;
	if (!GetPoolByMonth(ePoolNum, eMonth, &rRef))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", static_cast<int>(ePoolNum), static_cast<int>(eMonth));
		return;
	}

	if (ttTaskNum >= rRef->size())
	{
		sys_err("List overflow. Diff level: %d, Month: %d, List size: %u, Provided index: %u", static_cast<int>(ePoolNum), static_cast<int>(eMonth), rRef->size(), ttTaskNum);
		return;
	}

	auto & rTask = (*rRef)[ttTaskNum];
	rTask.AddReward(rReward);
	sys_log(0, "Reward to task %u added sucessfully! Num: %u, List count: %u", ttTaskNum, static_cast<size_t>(ePoolNum), rRef->size());
}

void CBattlePassManager::PrintTask(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const size_t ttTaskNum)
{
	if (!test_server)
	{
		return;
	}

	sQuestsList * rRef = nullptr;
	if (!GetPoolByMonth(ePoolNum, eMonth, &rRef))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", static_cast<int>(ePoolNum), static_cast<int>(eMonth));
		return;
	}

	if (ttTaskNum >= rRef->size())
	{
		sys_err("List overflow. Diff level: %d, Month: %d, List size: %u, Provided index: %u", static_cast<int>(ePoolNum), static_cast<int>(eMonth), rRef->size(), ttTaskNum);
		return;
	}

	auto & rTask = (*rRef)[ttTaskNum];
	rTask.PrintData();
}

void CBattlePassManager::TriggerEvent(LPCHARACTER ch, const BattlePassNS::SObjective & eType)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	// Checking if PC has battlepass on
	if (!HasEnabled(ch))
	{
		return;
	}

	// Checking if PC has all tasks completed
	if (ch->GetQuestFlag(sFinishDT) > 0)
	{
		return;
	}

	BattlePassNS::EDiffLevel eDiffLevel = static_cast<BattlePassNS::EDiffLevel>(ch->GetQuestFlag(sDiffLevel));
	BattlePassNS::EMonths eMonth = static_cast<BattlePassNS::EMonths>(ch->GetQuestFlag(sActiveMonth));

	sQuestsList * rRef = nullptr;
	if (!GetPoolByMonth(eDiffLevel, eMonth, &rRef))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", ch->GetQuestFlag(sDiffLevel), ch->GetQuestFlag(sActiveMonth));
		return;
	}

	// Mismatching events excluding inappropriates
	auto vMisMatchEvents = MisMatchEvents(eType, eDiffLevel);
	if (!vMisMatchEvents.size())
	{
		return;
	}

	// Looping through
	for (const auto & rElementPtr : vMisMatchEvents)
	{
		auto rElement = *rElementPtr;
		if (rElement.GetTargetPerc(ch) == 100)
		{
			continue;
		}

		// Update task data for user
		rElement.UpdateObjective(ch, eType.sObjectiveCount);

		if (rElement.GetTargetPerc(ch) == 100)
		{
			// Add reward
			rElement.GiveReward(ch);
			ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT(m_translation_unit["TASK_COMPLETED"].c_str()));

			// Checking player's total progress
			if (CalculateTotalProgress(*rRef, ch))
			{
				ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT(m_translation_unit["CAN_COLLECT"].c_str()));

				time_t dtFinishTime = get_global_time();
				ch->SetQuestFlag(sFinishDT, dtFinishTime);

				// Register highscore
				RegisterHighScore(ch, eDiffLevel, dtFinishTime);
			}
		}

		BroadcastBattlePass(ch, eDiffLevel, false, rElement.iTaskNum);
	}
}

void CBattlePassManager::BroadcastBattlePass(LPCHARACTER ch, BattlePassNS::EDiffLevel eDiffLevel, bool bClear, const int iNum)
{
	if (!ch || !HasEnabled(ch))
	{
		return;
	}

	if (eDiffLevel == BattlePassNS::EDiffLevel::TYPE_NONE)
	{
		eDiffLevel = static_cast<BattlePassNS::EDiffLevel>(ch->GetQuestFlag(sDiffLevel));
	}

	SRewards * rRef_Rew = nullptr;
	if (!GetRewardByMonth(eDiffLevel, GetCurrentMonth(), &rRef_Rew))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", ch->GetQuestFlag(sDiffLevel), ch->GetQuestFlag(sActiveMonth));
		return;
	}

	sQuestsList * rRef_Pool = nullptr;
	if (!GetPoolByMonth(eDiffLevel, GetCurrentMonth(), &rRef_Pool))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", ch->GetQuestFlag(sDiffLevel), ch->GetQuestFlag(sActiveMonth));
		return;
	}

	// Clear interface before broadcasting
	if (bClear)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface.c_str());
	}

	if (iNum > -1)
	{
		auto & rPool = *rRef_Pool;
		if (static_cast<size_t>(iNum) >= rPool.size())
		{
			sys_err("Index overflow! Pool size: %u, index: %d", rPool.size(), iNum);
			return;
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastTaskInfo.c_str(), rPool[iNum].sTaskName.c_str(), rPool[iNum].sTaskDesc.c_str(), rPool[iNum].GetTargetPerc(ch), rPool[iNum].iTaskNum);
		return;
	}

	for (const auto & rElement : *rRef_Pool)
	{
		// Task Info
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastTaskInfo.c_str(), rElement.sTaskName.c_str(), rElement.sTaskDesc.c_str(), rElement.GetTargetPerc(ch), rElement.iTaskNum);

		// Rewards
		for (const auto & rReward : rElement.v_rewards)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastTaskRewards.c_str(), rElement.iTaskNum, rReward.dwItemVnum, rReward.wItemCount);
		}
	}

	// Basic Info
	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastBasic.c_str(), static_cast<int>(eDiffLevel), static_cast<int>(get_global_time()), ch->GetQuestFlag(sFinishDT), ch->GetQuestFlag(sGrantedReward));

	// Major Rewards
	for (const auto & rElement : *rRef_Rew)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastRewards.c_str(), rElement.dwItemVnum, rElement.wItemCount);
	}

	// Ranking
	auto & rRank = m_highscores[eDiffLevel];
	for (const auto & rElement : rRank)
	{
		// If record is empty, stop it
		if (!rElement.ttTime)
		{
			break;
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastHighScore.c_str(), rElement.stPos, rElement.sName, rElement.ttTime);
	}
}

void CBattlePassManager::CollectReward(LPCHARACTER ch)
{
	if (!ch || !HasEnabled(ch))
	{
		return;
	}

	// Checking if reward was already granted
	if (ch->GetQuestFlag(sGrantedReward) > 0)
	{
		return;
	}

	// Checking out progress
	BattlePassNS::EDiffLevel eDiffLevel = static_cast<BattlePassNS::EDiffLevel>(ch->GetQuestFlag(sDiffLevel));
	BattlePassNS::EMonths eMonth = static_cast<BattlePassNS::EMonths>(ch->GetQuestFlag(sActiveMonth));

	SRewards * rRef_Rew = nullptr;
	if (!GetRewardByMonth(eDiffLevel, eMonth, &rRef_Rew))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", ch->GetQuestFlag(sDiffLevel), ch->GetQuestFlag(sActiveMonth));
		return;
	}

	sQuestsList * rRef_Pool = nullptr;
	if (!GetPoolByMonth(eDiffLevel, eMonth, &rRef_Pool))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", ch->GetQuestFlag(sDiffLevel), ch->GetQuestFlag(sActiveMonth));
		return;
	}

	if (CalculateTotalProgress(*rRef_Pool, ch))
	{
		// Grant reward
		for (const auto & rReward : *rRef_Rew)
		{
			ch->AutoGiveItem(rReward.dwItemVnum, rReward.wItemCount);
		}

		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT(m_translation_unit["REWARD_COLLECTED"].c_str()));

		// Set reward flag
		ch->SetQuestFlag(sGrantedReward, 1);

		BroadcastBattlePass(ch, eDiffLevel, true);
	}
}

bool CBattlePassManager::RegisterBattlePass(LPCHARACTER ch, const BattlePassNS::EDiffLevel & eDiffLevel)
{
	if (!ch)
	{
		return false;
	}

	// User cannot register to battlepass if is already in
	if (static_cast<BattlePassNS::EDiffLevel>(ch->GetQuestFlag(sDiffLevel)) != BattlePassNS::EDiffLevel::TYPE_NONE &&
			(static_cast<DWORD>(ch->GetQuestFlag(sActiveYear)) == GetCurrentYear() && static_cast<BattlePassNS::EMonths>(ch->GetQuestFlag(sActiveMonth)) == GetCurrentMonth()))
	{
		return false;
	}

	sQuestsList * rRef_Pool = nullptr;
	if (!GetPoolByMonth(eDiffLevel, GetCurrentMonth(), &rRef_Pool))
	{
		sys_err("BattlePass for this diff level is not configured. Diff level: %d, Month: %d", static_cast<int>(eDiffLevel), static_cast<int>(GetCurrentMonth()));
		return false;
	}

	ch->SetQuestFlag(sActiveYear, static_cast<int>(GetCurrentYear()));
	ch->SetQuestFlag(sActiveMonth, static_cast<int>(GetCurrentMonth()));
	ch->SetQuestFlag(sDiffLevel, static_cast<int>(eDiffLevel));
	ch->SetQuestFlag(sGrantedReward, 0);
	ch->SetQuestFlag(sFinishDT, 0);

	for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
	{
		for (auto j = static_cast<int>(BattlePassNS::EMonths::Jan); j <= static_cast<int>(BattlePassNS::EMonths::Dec); ++j)
		{
			if (GetPoolByMonth(static_cast<BattlePassNS::EDiffLevel>(i), static_cast<BattlePassNS::EMonths>(j), &rRef_Pool))
			{
				for (auto & rElement : *rRef_Pool)
				{
					rElement.ClearObjective(ch);
				}
			}
		}
	}

	// Broadcast info
	BroadcastBattlePass(ch, eDiffLevel, true);
	return true;
}

CBattlePassManager::VQuestsPtrPool CBattlePassManager::MisMatchEvents(const BattlePassNS::SObjective & eType, const BattlePassNS::EDiffLevel & eDiffLevel)
{
	std::vector<SQuestObject*> v_ret;
	sQuestsList * rRef_Pool = nullptr;

	if (!GetPoolByMonth(eDiffLevel, GetCurrentMonth(), &rRef_Pool))
	{
		sys_err("Month for provided difficulity not found. Diff level: %d, Month: %d", static_cast<int>(eDiffLevel), static_cast<int>(GetCurrentMonth()));
		return v_ret;
	}

	for (auto & rElement : *rRef_Pool)
	{
		if (rElement == eType)
		{
			v_ret.push_back(&rElement);
		}
	}

	return v_ret;
}

bool CBattlePassManager::HasEnabled(LPCHARACTER ch)
{
	if (!ch)
	{
		return false;
	}

	if (static_cast<BattlePassNS::EDiffLevel>(ch->GetQuestFlag(sDiffLevel)) == BattlePassNS::EDiffLevel::TYPE_NONE)
	{
		return false;
	}

	if (static_cast<DWORD>(ch->GetQuestFlag(sActiveYear)) != GetCurrentYear())
	{
		return false;
	}

	if (static_cast<BattlePassNS::EMonths>(ch->GetQuestFlag(sActiveMonth)) != GetCurrentMonth())
	{
		return false;
	}

	return true;
}

bool CBattlePassManager::CalculateTotalProgress(const sQuestsList & rPool, LPCHARACTER ch)
{
	bool bProgress = true;
	std::for_each(rPool.begin(), rPool.end(), [&bProgress, &ch] (const SQuestObject & rObj) { if (rObj.GetTargetPerc(ch) < 100) bProgress = false; });

	return bProgress;
}

/// Find ///
bool CBattlePassManager::GetQuestObject(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, SQuestsPool::iterator & it)
{
	auto fIt = m_task_pool.find(eDiffLevel);
	if (fIt == m_task_pool.end())
	{
		return false;
	}

	auto fItDeep = (fIt->second).find(eMonth);
	if (fItDeep == (fIt->second).end())
	{
		return false;
	}

	it = fItDeep;
	return true;
}

bool CBattlePassManager::GetPoolByMonth(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, sQuestsList ** rRef)
{
	SQuestsPool::iterator it;
	if (!GetQuestObject(eDiffLevel, eMonth, it))
	{
		return false;
	}

	*rRef = &((it->second).second);
	return true;
}

bool CBattlePassManager::GetRewardByMonth(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, SRewards ** rRef)
{
	SQuestsPool::iterator it;
	if (!GetQuestObject(eDiffLevel, eMonth, it))
	{
		return false;
	}

	*rRef = &((it->second).first);
	return true;
}
/// ///

/// HighScores ///
void CBattlePassManager::RegisterHighScore(LPCHARACTER ch, const BattlePassNS::EDiffLevel & eDiffLevel, const time_t ttTime)
{
	auto & rRank = m_highscores[eDiffLevel];
	auto fIt = std::find_if(rRank.begin(), rRank.end(), [&ttTime] (const TBattlePassHighScore & rElement) { return (rElement.ttTime > ttTime || !rElement.ttTime); });

	if (fIt != rRank.end())
	{
		TBattlePassHighScore pack{};
		pack.bSubHeader = SUBHEADER_BATTLEPASS_GD_ADD_ITEM;
		pack.iDiffLevel = static_cast<int>(eDiffLevel);
		pack.iMonth = static_cast<int>(GetCurrentMonth());
		strlcpy(pack.sName, ch->GetName(), sizeof(pack.sName));
		pack.ttTime = ttTime;

		db_clientdesc->DBPacket(HEADER_GD_BATTLEPASS_RANKING, 0, &pack, sizeof(pack));
	}
}

void CBattlePassManager::RecvHighScoreUpdate(const TBattlePassHighScore * pPack)
{
	switch (pPack->bSubHeader)
	{
	case SUBHEADER_BATTLEPASS_DG_WIPE_RANKING:
	{
		for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
			m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)] = std::vector<TBattlePassHighScore>(stHighScoreCount, TBattlePassHighScore{});
	}
	break;
	case SUBHEADER_BATTLEPASS_DG_ADD_ELEMENT:
	{
		auto & rRank = m_highscores[static_cast<BattlePassNS::EDiffLevel>(pPack->iDiffLevel)];
		if (pPack->stPos >= rRank.size())
		{
			sys_err("Ranking overflow! Size: %u, Element: %d", rRank.size(), pPack->stPos);
			return;
		}

		rRank[pPack->stPos] = *pPack;
	}
	break;
	default:
		sys_err("Unknown subheader: %d", pPack->bSubHeader);
		break;
	}
}
/// ///
#endif

