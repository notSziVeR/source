#include "stdafx.h"
#ifdef __CYCLIC_MONSTER_RESPAWNER__
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "desc.h"
#include "desc_manager.h"
#include "locale.hpp"
#include "p2p.h"
#include "utils.h"
#include "MonsterRespawner.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

#define __DEBUG_PRINT
#ifdef __DEBUG_PRINT
	#include <iostream>

	using std::cerr;
	using std::endl;
#endif

namespace MonsterRespawner
{
static const long lHoursBase = 24;
std::unordered_map<std::string, TRespawnPair> m_respawn_times = {{"MONSTER", {0, ESpotRefreshType::TYPE_MONSTER}}, {"BOSS", {0, ESpotRefreshType::TYPE_BOSS}}, {"STONE", {0, ESpotRefreshType::TYPE_STONE}}};

inline static long ComputeHourDistance(const long & lCurrentHour, const long & lFurtherHour)
{
	return (lCurrentHour >= lFurtherHour) ? (lHoursBase - lCurrentHour) + lFurtherHour : (lFurtherHour - lCurrentHour);
}

const std::set<long> v_lRespawnMonsterDelay = {3, 15}; // Respawn hours
const long lRespawnBossDelay = 60 * 60 * 10; // 10h, starting from beginning of current hour
const long lRespawnStoneDelay = 60 * 60 * 5; // 5h, starting from beginning of current hour

const std::unordered_set<ESpotRefreshType> s_type_blacklist = {}; // Blacklist for selected types
const std::unordered_set<DWORD> s_respawn_blacklist = {1192}; // Blacklist (convergent for all types)


const std::unordered_map<ESpotRefreshType, std::string, EnumClassHash> m_say_dictionary =
{
	{ESpotRefreshType::TYPE_MONSTER, "MONSTER_RESPAWNER_RESET_MONSTER_NOTICE %s"},
	{ESpotRefreshType::TYPE_BOSS, "MONSTER_RESPAWNER_RESET_BOSS_NOTICE %s"},
	{ESpotRefreshType::TYPE_STONE, "MONSTER_RESPAWNER_RESET_STONE_NOTICE %s"},
};

static void BroadcastNotice(const std::string & s_notice, const time_t & ttRespawnTime)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_ref_set.begin(), c_ref_set.end(),
	[&s_notice, &ttRespawnTime](LPDESC d) { if (d->GetCharacter() == NULL) return; d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT(s_notice.c_str()), GetFullDateFromTime(ttRespawnTime).c_str()); });
}

void Initialize()
{
	// Current time
	auto cur_time = boost::posix_time::second_clock::local_time();
	long lHours = cur_time.time_of_day().hours(), lMinutes = cur_time.time_of_day().minutes();

	// Computing further restart dates
	long lNextH = -1;
	std::for_each(v_lRespawnMonsterDelay.begin(), v_lRespawnMonsterDelay.end(), [&lNextH, &lHours] (const long & lItHour)
	{
		long lHourDiff = ComputeHourDistance(lHours, lItHour);
		if (lNextH == -1 || lHourDiff < lNextH)
		{
			lNextH = lHourDiff;
		}
	});

	// Counting next update time
	auto subMin = [&lMinutes] (time_t && ttCurTime) -> time_t { return (ttCurTime - (lMinutes * 60)); };
	auto ttCurTime = get_global_time();
	m_respawn_times["MONSTER"].first = subMin(ttCurTime + (lNextH * 60 * 60));
	m_respawn_times["BOSS"].first = subMin(ttCurTime + lRespawnBossDelay);
	m_respawn_times["STONE"].first = subMin(ttCurTime + lRespawnStoneDelay);

#ifdef __DEBUG_PRINT
	cerr << "Current time: " << cur_time << endl;
	for (const auto & rec : m_respawn_times)
	{
		cerr << "Monster Respawner. Key: " << rec.first << ", Respawn at: " << GetFullDateFromTime(rec.second.first - 0) << endl;
	}
#endif
}

void RespawnSingleMonster(std::set<DWORD> && s_respawn, const long & lMapIndex, const bool & bBroadcast)
{
	for (const auto & dwMonster : s_respawn)
	{
		CHARACTER_MANAGER::instance().EraseMonsterByValue(dwMonster, lMapIndex);

		// Broadcasting erase command to other cores
		if (bBroadcast)
		{
			TPacketGGClearSpots p;
			memset(&p, 0, sizeof(p));
			p.header = HEADER_GG_RESET_SPOTS;
			p.lMapIndex = lMapIndex;
			p.dwMonster = dwMonster;
			P2P_MANAGER::instance().Send(&p, sizeof(p));

			sys_log(0, "Respawn packet has been sent. MapIndex: %ld, Monster Vnum: %u", lMapIndex, dwMonster);
		}
	}
}

const long GetRespawnDelayByType(const ESpotRefreshType & eType)
{
	const static long lEmpty = 0;
	switch (eType)
	{
	case ESpotRefreshType::TYPE_MONSTER:
	{
		long lHours = boost::posix_time::second_clock::local_time().time_of_day().hours();
		auto it = v_lRespawnMonsterDelay.find(lHours);
		if (it != v_lRespawnMonsterDelay.end() && (++it) == v_lRespawnMonsterDelay.end())
		{
			it = v_lRespawnMonsterDelay.begin();
		}

		return (it != v_lRespawnMonsterDelay.end()) ? ComputeHourDistance(lHours, *it) * 60 * 60 : lHoursBase * 60 * 60;
	}
	break;
	case ESpotRefreshType::TYPE_BOSS:
		return lRespawnBossDelay;
		break;
	case ESpotRefreshType::TYPE_STONE:
		return lRespawnStoneDelay;
		break;
	}

	return lEmpty;
}

void ProcessRespawn()
{
	for (auto & rec : m_respawn_times)
	{
		if (get_global_time() >= rec.second.first && s_type_blacklist.find(rec.second.second) == s_type_blacklist.end())
		{
			CHARACTER_MANAGER::instance().EraseMonsterByType(rec.second.second, s_respawn_blacklist);
			rec.second.first = get_global_time() + GetRespawnDelayByType(rec.second.second);

			if (m_say_dictionary.find(rec.second.second) != m_say_dictionary.end())
			{
				BroadcastNotice(m_say_dictionary.find(rec.second.second)->second, rec.second.first);
			}

#ifdef __DEBUG_PRINT
			cerr << "Respawn had been proceeded! Type: " << static_cast<int>(rec.second.second) << ", Time: "
				 << GetFullDateFromTime(get_global_time()) << ", Next Respawn: " << GetFullDateFromTime(rec.second.first - 0) << endl;
#endif
		}
	}
}
}
#endif

