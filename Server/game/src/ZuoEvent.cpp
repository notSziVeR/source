#include "stdafx.h"
#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
#include <numeric>
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_manager.h"
#include "utils.h"
#include "ZuoEvent.hpp"
#include "mob_manager.h"

CZuoEventManager::CZuoEventManager() : iEventStatus(EEventType::TYPE_STOP), st_max_player_count(0), tt_start_time(0)
{}

CZuoEventManager::~CZuoEventManager()
{
	Clear();
}

void CZuoEventManager::Clear()
{
	iEventStatus = EEventType::TYPE_STOP;
	st_max_player_count = 0;
	tt_start_time = 0;
	KillAll();
	us_player_count.clear();
	us_monster_count.clear();
	us_dead_monster_count.clear();
}

void CZuoEventManager::ChangeEventStatus(const int & iStatus)
{
	if (iStatus == -1)
	{
		iEventStatus = (iEventStatus == EEventType::TYPE_STOP) ? EEventType::TYPE_START : EEventType::TYPE_STOP;
	}
	else
	{
		iEventStatus = static_cast<EEventType>(iStatus);
	}

	//BroadcastNotice((iEventStatus == EEventType::TYPE_STOP) ? "ZUO_EVENT_STOP_INFO" : "ZUO_EVENT_START_INFO", CHAT_TYPE_BIG_NOTICE, true);
	BroadcastNotice((iEventStatus == EEventType::TYPE_STOP) ? LC_TEXT("ZUO_EVENT_STOP_INFO") : LC_TEXT("ZUO_EVENT_START_INFO"));

	// Warping out
	if (GetEventStatus() == EEventType::TYPE_STOP)
		std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this](LPDESC d)
	{
		if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == GetEventMapIndex())
		{
			d->GetCharacter()->GoHome();
		}
	});
	else
	{
		Clear();
		tt_start_time = get_global_time() - 1; // -1 for handicap
		iEventStatus = EEventType::TYPE_START;
	}

	BroadcastEventInfo(EBroadcastType::BROADCAST_TIME);
}

EMonsterType CZuoEventManager::GetMonsterType(LPCHARACTER ch)
{
	if (ch->IsMonster() && ch->GetMobRank() >= MOB_RANK_BOSS)
	{
		return EMonsterType::MONSTER_BOSS;
	}
	else if (ch->IsStone())
	{
		return EMonsterType::MONSTER_STONE;
	}

	return EMonsterType::MONSTER_UNKNOWN;
}

EMonsterType CZuoEventManager::GetMonsterType(const DWORD & dwVnum)
{
	const CMob * pkMob = CMobManager::instance().Get(dwVnum);
	if (!pkMob)
	{
		return EMonsterType::MONSTER_UNKNOWN;
	}

	if ((pkMob->m_table).bType == CHAR_TYPE_MONSTER && (pkMob->m_table).bRank >= MOB_RANK_BOSS)
	{
		return EMonsterType::MONSTER_BOSS;
	}
	else if ((pkMob->m_table).bType == CHAR_TYPE_STONE)
	{
		return EMonsterType::MONSTER_STONE;
	}

	return EMonsterType::MONSTER_UNKNOWN;
}

void CZuoEventManager::SpawnMonster(const DWORD & dwVnum, const WORD & wCount)
{
	auto pMonsterType = std::move(std::make_pair(-1, GetMonsterType(dwVnum)));
	if (pMonsterType.second == EMonsterType::MONSTER_UNKNOWN)
	{
		sys_err("Cannot spawn this kind of monster (it's not boss neither stone). Vnum: %d", dwVnum);
		return;
	}

	pMonsterType.first = static_cast<int>(GetMonsterType(dwVnum));
	auto unsetIt = us_monster_count.find(pMonsterType.first);
	if (unsetIt == us_monster_count.end())
	{
		unsetIt = us_monster_count.emplace(std::piecewise_construct, std::forward_as_tuple(pMonsterType.first), std::forward_as_tuple()).first;
	}

	auto & usmonster_count = unsetIt->second;
	for (WORD i = 0; i < wCount; ++i)
	{
		LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(dwVnum, GetEventMapIndex());
		if (ch)
		{
			usmonster_count.insert(ch->GetVID());
			BroadcastEventInfo((pMonsterType.second == EMonsterType::MONSTER_BOSS) ? EBroadcastType::BROADCAST_BOSS : EBroadcastType::BROADCAST_STONE);
		}
	}
}

void CZuoEventManager::DeadMonster(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	if (ch->GetMapIndex() != GetEventMapIndex())
	{
		return;
	}

	auto pMonsterType = std::move(std::make_pair(-1, GetMonsterType(ch->GetRaceNum())));
	pMonsterType.first = static_cast<int>(GetMonsterType(ch->GetRaceNum()));

	auto unsetIt = us_monster_count.find(pMonsterType.first);
	if (unsetIt == us_monster_count.end())
	{
		return;
	}

	auto & usmonster_count = unsetIt->second;
	auto fIt = usmonster_count.find(ch->GetVID());

	if (fIt != usmonster_count.end())
	{
		// Reusing iterator
		unsetIt = us_dead_monster_count.find(pMonsterType.first);
		if (unsetIt == us_dead_monster_count.end())
		{
			unsetIt = us_dead_monster_count.emplace(std::piecewise_construct, std::forward_as_tuple(pMonsterType.first), std::forward_as_tuple()).first;
		}

		(unsetIt->second).insert(*fIt);
		usmonster_count.erase(fIt);

		BroadcastEventInfo((pMonsterType.second == EMonsterType::MONSTER_BOSS) ? EBroadcastType::BROADCAST_BOSS : EBroadcastType::BROADCAST_STONE);
	}
}

void CZuoEventManager::RegisterNewPlayer(LPCHARACTER ch)
{
	// Don't count implementor
	if (ch->GetGMLevel() == GM_IMPLEMENTOR)
	{
		return;
	}

	if (ch->GetMapIndex() != GetEventMapIndex())
	{
		return;
	}

	if (GetEventStatus() == EEventType::TYPE_STOP)
	{
		return;
	}

	auto fIt = us_player_count.find(ch->GetPlayerID());
	if (fIt == us_player_count.end())
	{
		us_player_count.emplace(ch->GetPlayerID(), true);
		st_max_player_count++;
	}
	else
	{
		fIt->second = true;
	}

	BroadcastEventInfo(EBroadcastType::BROADCAST_ATTENDERS);
}

void CZuoEventManager::UnregisterPlayer(LPCHARACTER ch)
{
	// Don't count implementor
	if (ch->GetGMLevel() == GM_IMPLEMENTOR)
	{
		return;
	}

	if (ch->GetMapIndex() != GetEventMapIndex())
	{
		return;
	}

	if (GetEventStatus() == EEventType::TYPE_STOP)
	{
		return;
	}

	auto fIt = us_player_count.find(ch->GetPlayerID());
	if (fIt != us_player_count.end())
	{
		fIt->second = false;
	}

	BroadcastEventInfo(EBroadcastType::BROADCAST_ATTENDERS);
}

void CZuoEventManager::KillAll()
{
	for (const auto & pr : us_monster_count)
	{
		for (const auto & dwVID : pr.second)
		{
			auto chPtr = CHARACTER_MANAGER::instance().Find(dwVID);
			if (chPtr)
			{
				M2_DESTROY_CHARACTER(chPtr);
			}
		}
	}
}

void CZuoEventManager::BroadcastEventInfo(const EBroadcastType & eBroadcastType, std::unordered_set<LPCHARACTER> s_residents)
{
	if (s_residents.empty() && !FindMapResident(s_residents))
	{
		return;
	}

	// Broadcasting lambda
	auto br_lmbd = [&s_residents] (const std::string & s_txt, const size_t & arg)
	{
		std::for_each(s_residents.begin(), s_residents.end(), [&s_txt, &arg] (LPCHARACTER chPtr)
		{
			chPtr->ChatPacket(CHAT_TYPE_COMMAND, s_txt.c_str(), arg);
		});
	};

	decltype(us_monster_count)::iterator usIter;
	switch (eBroadcastType)
	{
	case EBroadcastType::BROADCAST_BOSS:
	{
		// Boss
		usIter = us_monster_count.find(static_cast<int>(EMonsterType::MONSTER_BOSS));
		if (usIter != us_monster_count.end())
		{
			br_lmbd("UpdateZuoDialogElement BossCount %u", (usIter->second).size());
		}
		else
		{
			br_lmbd("UpdateZuoDialogElement BossCount %u", 0);
		}

		usIter = us_dead_monster_count.find(static_cast<int>(EMonsterType::MONSTER_BOSS));
		if (usIter != us_dead_monster_count.end())
		{
			br_lmbd("UpdateZuoDialogElement BossCountDead %u", (usIter->second).size());
		}
		else
		{
			br_lmbd("UpdateZuoDialogElement BossCountDead %u", 0);
		}
	}
	break;
	case EBroadcastType::BROADCAST_STONE:
	{
		// Stones
		usIter = us_monster_count.find(static_cast<int>(EMonsterType::MONSTER_STONE));
		if (usIter != us_monster_count.end())
		{
			br_lmbd("UpdateZuoDialogElement MetinCount %u", (usIter->second).size());
		}
		else
		{
			br_lmbd("UpdateZuoDialogElement MetinCount %u", 0);
		}

		usIter = us_dead_monster_count.find(static_cast<int>(EMonsterType::MONSTER_STONE));
		if (usIter != us_dead_monster_count.end())
		{
			br_lmbd("UpdateZuoDialogElement MetinCountDead %u", (usIter->second).size());
		}
		else
		{
			br_lmbd("UpdateZuoDialogElement MetinCountDead %u", 0);
		}
	}
	break;
	case EBroadcastType::BROADCAST_ATTENDERS:
	{
		// Attenders
		br_lmbd("UpdateZuoDialogElement OnlineAttenders %u", std::accumulate(us_player_count.begin(), us_player_count.end(), 0, [] ( const size_t & left, const std::pair<const DWORD, bool> & right) -> size_t { return left + right.second; }));
		br_lmbd("UpdateZuoDialogElement MaxAttenders %u", st_max_player_count);
	}
	break;
	case EBroadcastType::BROADCAST_TIME:
	{
		// TimeGone
		br_lmbd("UpdateZuoDialogElement TimeGone %d", !tt_start_time ? 0 : std::max((int) 0, static_cast<int>(get_global_time() - tt_start_time)));
	}
	break;
	case EBroadcastType::BROADCAST_ALL:
	{
		for (auto eType = 0; eType < static_cast<int>(EBroadcastType::BROADCAST_ALL); ++eType)
		{
			BroadcastEventInfo(static_cast<EBroadcastType>(eType), s_residents);
		}
	}
	break;
	}
}

bool CZuoEventManager::FindMapResident(std::unordered_set<LPCHARACTER> & s_residents)
{
	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &s_residents] (LPDESC d)
	{
		if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == GetEventMapIndex() && d->GetCharacter()->GetGMLevel() == GM_IMPLEMENTOR)
		{
			s_residents.insert(d->GetCharacter());
		}
	});

	return s_residents.size();
}
#endif

