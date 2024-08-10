#include "stdafx.h"
#ifdef __MONSTER_HELPER__
#include "utils.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "MonsterHelper.hpp"

#ifdef __ENABLE_NEW_LOGS_CHAT__
	#include "ChatLogsHelper.hpp"
#endif

#include "ItemUtils.h"

namespace MonsterHelper
{
std::map<ESpecialInstance, std::pair<std::vector<DWORD>, std::function<bool(LPCHARACTER, LPCHARACTER, DWORD, BYTE)>>> m_special_instance =
{
	{ESpecialInstance::TYPE_UNMOUNT, {std::vector<DWORD>{}, &MonsterHelper::SpecialInstanceUnmount}},
	{ESpecialInstance::TYPE_SKILL, {std::vector<DWORD>{}, &MonsterHelper::SpecialInstanceSkill}},
	{ESpecialInstance::TYPE_POLY, {std::vector<DWORD>{}, &MonsterHelper::SpecialInstancePoly}},
	{ESpecialInstance::TYPE_FIRST_HIT, {std::vector<DWORD>{}, &MonsterHelper::SpecialInstanceFirstHit}},
	{ESpecialInstance::TYPE_CRITICAL, {std::vector<DWORD>{}, &MonsterHelper::SpecialInstanceCritical}},
};
std::unordered_map<DWORD, DWORD> um_first_hit;

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE = (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
	DAMAGE_FIRE 	= (1 << 6),
};

const std::set<DWORD> s_pickup_items{ };

// Special Instance //
bool SpecialInstanceUnmount(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	return (ch->GetMountVnum() > 0);
}

bool SpecialInstanceSkill(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	return (eType != DAMAGE_TYPE_MAGIC);
}

bool SpecialInstancePoly(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	return (ch->GetPolymorphVnum() == 0);
}

bool SpecialInstanceFirstHit(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	auto fIt = um_first_hit.find(pVictim->GetVID());
	if (fIt != um_first_hit.end())
	{
		return (fIt->second != ch->GetPlayerID());
	}

	um_first_hit[pVictim->GetVID()] = ch->GetPlayerID();
	return true;
}

bool SpecialInstanceCritical(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	return (!IS_SET(bDamageFlag, DAMAGE_CRITICAL));
}

bool CanAttackSpecialInstance(LPCHARACTER ch, LPCHARACTER pVictim, DWORD eType, BYTE bDamageFlag)
{
	if (!ch || !ch->IsPC())
	{
		return true;
	}

	if (!pVictim || pVictim->IsPC())
	{
		return true;
	}

	return std::find_if(m_special_instance.begin(), m_special_instance.end(), [&ch, &pVictim, &eType, &bDamageFlag] (const std::pair<ESpecialInstance, std::pair<std::vector<DWORD>, std::function<bool(LPCHARACTER, LPCHARACTER, DWORD, BYTE)>>> & pRef) -> bool
	{
		auto & pSec = pRef.second;
		if (std::find(pSec.first.begin(), pSec.first.end(), pVictim->GetRaceNum()) != pSec.first.end())
			return pSec.second(ch, pVictim, eType, bDamageFlag);

		return false;
	}) == m_special_instance.end();
}

void AddSpecialInstance(ESpecialInstance eType, DWORD dwVnum)
{
	(m_special_instance[eType].first).push_back(dwVnum);
}

void RemoveSpecialInstance(DWORD dwVID)
{
	um_first_hit.erase(dwVID);
}

// Pickuper
bool PerformPickup(LPCHARACTER ch, LPITEM item)
{
	if (!ch)
	{
		return false;
	}

	if (s_pickup_items.find(item->GetVnum()) != s_pickup_items.end())
	{
		return false;
	}

	auto pkItem = FindToggleItem(ch, true, TOGGLE_PICKUPER);
	if (!pkItem)
		return false;

	if (ch->GetEmptyInventory(item->GetSize()) == -1)
	{
		return false;
	}

#ifdef __ENABLE_NEW_LOGS_CHAT__
	CChatLogs::SendChatLogInformation(ch, ELogsType::LOG_TYPE_PICKUP, static_cast<int>(item->GetCount()), static_cast<int>(item->GetVnum()));
#endif

	ch->AutoGiveItem(item->GetVnum(), item->GetCount(), 1, false);
	return true;
}
}
#endif

