#include "stdafx.h"
#ifdef __LEGENDARY_MONSTER_HELPER_ENABLE__
#include "char.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "sectree.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "utils.h"
#include "config.h"
#include "MonsterSpecialHelper.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

namespace MonsterSpecialHelper
{
	// Legendary Bosses
	struct SLegendaryBossesData
	{
		DWORD dwVnum;
		long lMapIndex;
		BYTE bChannelID;
		long lSpawnTime; // In hours
		std::vector<std::tuple<long, long>> v_monster_spawn;
	};

	std::vector<SLegendaryBossesData> v_legendary_bosses = {
																	{1192, 1, 3, 23, {{622, 664}}},
															};

	const std::string sLegendaryFlag("legendary_boss_flag_");
	const std::string sLegendaryFlagKilled("legendary_boss_flag_killed_");
	const int iLegendaryBossMapIndex = 83;
	const int iLegendaryBossRespawn = 60*60*24;
	bool bLegendaryInit = false;

	// Legendary Boss
	void UpdateEventFlag(const std::string & sFlag, const int & iValue)
	{
		quest::CQuestManager::instance().RequestSetEventFlag(sFlag, iValue);
		quest::CQuestManager::instance().SetEventFlag(sFlag, iValue);
	}

	void InitBossRespawn()
	{
		bLegendaryInit = true;

		for (const auto & rMobRecord : v_legendary_bosses)
		{
			const std::string sMobFlag(std::to_string(rMobRecord.dwVnum) + "_" + std::to_string(rMobRecord.lMapIndex) + "_" + std::to_string(rMobRecord.bChannelID));
			// Check if channel requirement is matched
			if (rMobRecord.bChannelID != g_bChannel)
				continue;

			// Check if map exists here
			LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(rMobRecord.lMapIndex);
			if (!pMap)
				continue;

			// Check if respawn time comes
			if (get_global_time() < quest::CQuestManager::instance().GetEventFlag(sLegendaryFlag + sMobFlag))
				continue;

			// Check if legend has been killed recently
			if (!quest::CQuestManager::instance().GetEventFlag(sLegendaryFlagKilled + sMobFlag))
			{
				int iRandom = number(0, rMobRecord.v_monster_spawn.size()-1);
				auto pChar = CHARACTER_MANAGER::instance().SpawnMob(rMobRecord.dwVnum, rMobRecord.lMapIndex, 
														pMap->m_setting.iBaseX + std::get<0>(rMobRecord.v_monster_spawn[iRandom])*100, pMap->m_setting.iBaseY + std::get<1>(rMobRecord.v_monster_spawn[iRandom])*100, 
														0, false, 0);

				// Broadcast notice
				if (pChar)
					BroadcastNotice(LC_TEXT("SPECIAL_MONSTER_SPAWN_TEXT"));

				sys_log(0, "Legend has been respawned! %d. Next respawn: %d", rMobRecord.dwVnum, get_global_time()+(rMobRecord.lSpawnTime*60*60));
			}
		}
	}

	void LegendaryBossSpawn()
	{
		if (!bLegendaryInit)
			return;

		for (const auto & rMobRecord : v_legendary_bosses)
		{
			const std::string sMobFlag(std::to_string(rMobRecord.dwVnum) + "_" + std::to_string(rMobRecord.lMapIndex) + "_" + std::to_string(rMobRecord.bChannelID));
			// Check if channel requirement is matched
			if (rMobRecord.bChannelID != g_bChannel)
				continue;

			// Check if map exists here
			LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(rMobRecord.lMapIndex);
			if (!pMap)
				continue;

			// Check if respawn time comes
			if (get_global_time() < quest::CQuestManager::instance().GetEventFlag(sLegendaryFlag + sMobFlag))
				continue;

			// Check if one killed boss
			if (!quest::CQuestManager::instance().GetEventFlag(sLegendaryFlagKilled + sMobFlag))
				continue;

			// Respawn legend
			int iRandom = number(0, rMobRecord.v_monster_spawn.size()-1);
			auto pChar = CHARACTER_MANAGER::instance().SpawnMob(rMobRecord.dwVnum, rMobRecord.lMapIndex, 
													pMap->m_setting.iBaseX + std::get<0>(rMobRecord.v_monster_spawn[iRandom])*100, pMap->m_setting.iBaseY + std::get<1>(rMobRecord.v_monster_spawn[iRandom])*100, 
													0, false, 0);
			UpdateEventFlag(sLegendaryFlagKilled + sMobFlag, 0);
			UpdateEventFlag(sLegendaryFlag + sMobFlag, get_global_time()+(rMobRecord.lSpawnTime*60*60));

			// Broadcast notice
			if (pChar)
				BroadcastNotice(LC_TEXT("SPECIAL_MONSTER_SPAWN_TEXT"));

			sys_log(0, "Legend has been respawned! %d. Next respawn: %d", rMobRecord.dwVnum, get_global_time()+(rMobRecord.lSpawnTime*60*60));
		}
	}

	void RegisterLegendaryBossKill(LPCHARACTER killer, LPCHARACTER boss)
	{
		auto fIt = std::find_if(v_legendary_bosses.begin(), v_legendary_bosses.end(), [&boss] (const SLegendaryBossesData & rRecord) { return (rRecord.dwVnum == boss->GetRaceNum() && rRecord.lMapIndex == boss->GetMapIndex() && rRecord.bChannelID == g_bChannel); } );
		if (fIt == v_legendary_bosses.end())
			return;

		const std::string sMobFlag(std::to_string(boss->GetRaceNum()) + "_" + std::to_string(boss->GetMapIndex()) + "_" + std::to_string(g_bChannel));
		UpdateEventFlag(sLegendaryFlagKilled + sMobFlag, 1);
		UpdateEventFlag(sLegendaryFlag + sMobFlag, get_global_time()+(fIt->lSpawnTime*60*60));

		// Broadcast notice
		BroadcastNotice(LC_TEXT("SPECIAL_MONSTER_KILL_TEXT"));
	}
}
#endif

