#pragma once
#ifdef __MONSTER_HELPER__

namespace MonsterHelper
{
	enum class ESpecialInstance : int
	{
		TYPE_UNMOUNT,
		TYPE_SKILL,
		TYPE_POLY,
		TYPE_FIRST_HIT,
		TYPE_CRITICAL,
	};

	// Special Instance //
	bool CanAttackSpecialInstance(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);
	bool SpecialInstanceUnmount(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);
	bool SpecialInstanceSkill(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);
	bool SpecialInstancePoly(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);
	bool SpecialInstanceFirstHit(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);
	bool SpecialInstanceCritical(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag);

	void AddSpecialInstance(ESpecialInstance eType, DWORD dwVnum);
	void RemoveSpecialInstance(DWORD dwVID);

	bool PerformPickup(LPCHARACTER ch, LPITEM item);
}
#endif

