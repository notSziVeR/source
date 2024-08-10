#pragma once

#ifdef __LEGENDARY_MONSTER_HELPER_ENABLE__
namespace MonsterSpecialHelper
{
	// Legendary Boss
	void InitBossRespawn();
	void LegendaryBossSpawn();
	void RegisterLegendaryBossKill(LPCHARACTER killer, LPCHARACTER boss);
}
#endif

