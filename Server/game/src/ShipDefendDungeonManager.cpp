#include "stdafx.h"
#ifdef __SHIP_DUNGEON_ENABLE__
#include <tuple>
#include "char.h"
#include "party.h"
#include "dungeon.h"
#include "ShipDefendDungeon.hpp"
#include "ShipDefendDungeonManager.hpp"

CShipDefendDungeonManager::CShipDefendDungeonManager()
{
}

CShipDefendDungeonManager::~CShipDefendDungeonManager()
{
	m_attenders_list.clear();
}

void CShipDefendDungeonManager::Initialize()
{
	ShipDefendHelper::Initialize();
}

bool CShipDefendDungeonManager::RegisterAttender(LPCHARACTER ch, LPDUNGEON pDungeon)
{
	if (!ch || !pDungeon)
	{
		return false;
	}

	auto it = m_attenders_list.find(ch->GetPlayerID());
	if (it == m_attenders_list.end())
	{
		auto pDefend = new CShipDefendDungeon(pDungeon->GetMapIndex(), pDungeon);
		if (ch->GetParty() && ch->GetParty()->GetLeaderPID() == ch->GetPlayerID())
		{
			m_attenders_list.emplace(std::piecewise_construct, std::forward_as_tuple(ch->GetPlayerID()), std::forward_as_tuple(true, pDefend));
		}
		else
		{
			m_attenders_list.emplace(std::piecewise_construct, std::forward_as_tuple(ch->GetPlayerID()), std::forward_as_tuple(false, pDefend));
		}
	}
	else
	{
		auto pParty = ch->GetParty();
		// For those who tried to cheat system
		if (((it->second).bParty && (!pParty || m_attenders_list.find(pParty->GetLeaderPID()) == m_attenders_list.end()))
				|| (!(it->second).bParty && pParty))
		{
			return false;
		}
	}

	return true;
}

void CShipDefendDungeonManager::EraseAttender(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	auto it = m_attenders_list.find(ch->GetPlayerID());
	if (it != m_attenders_list.end())
	{
		// Dungeon is over. We can eaisly delete it
		if ((it->second).pDungeon.get()->IsEnded())
		{
			m_attenders_list.erase(ch->GetPlayerID());
		}
	}
}

CShipDefendDungeon * CShipDefendDungeonManager::FindDungeonByPID(LPCHARACTER ch)
{
	DWORD s_val = ch->GetParty() ? ch->GetParty()->GetLeaderPID() : ch->GetPlayerID();
	auto fIt = m_attenders_list.find(s_val);
	return (fIt == m_attenders_list.end()) ? nullptr : fIt->second.pDungeon.get();
}

CShipDefendDungeon * CShipDefendDungeonManager::FindDungeonByVID(LPCHARACTER ch)
{
	for (auto const & rec : m_attenders_list)
	{
		if ((rec.second).pDungeon.get()->FindFieldMonster(ch))
		{
			return (rec.second).pDungeon.get();
		}
	}

	return nullptr;
}

// Callback sent from dungeon manager
void CShipDefendDungeonManager::EraseDungeon(LPDUNGEON pDungeon)
{
	auto it = std::find_if(m_attenders_list.begin(), m_attenders_list.end(), [&](const std::pair<const DWORD, SShipDefendStruct> & val) { return val.second.pDungeon.get()->GetDungeon() == pDungeon; });
	if (it != m_attenders_list.end())
	{
		m_attenders_list.erase(it);
	}
}

// Only for non-pc
// Nevermind
void CShipDefendDungeonManager::RegisterHitRecord(LPCHARACTER pAttacker, LPCHARACTER pVictim)
{
	if (!pAttacker || !pVictim)
	{
		return;
	}

	if (pVictim->IsPC())
	{
		return;
	}

	auto pVic = FindDungeonByVID(pVictim);
	if (pVic)
	{
		auto pAttack = pAttacker->IsPC() ? FindDungeonByPID(pAttacker) : FindDungeonByVID(pAttacker);
		if (pAttack && pVic && pAttack == pVic)
		{
			pAttack->RegisterHitRecord(pAttacker, pVictim);
		}
	}
}

void CShipDefendDungeonManager::MoveInitialState(LPCHARACTER pPC, LPCHARACTER pAllied)
{
	if (CheckForAllie(pPC, pAllied))
	{
		FindDungeonByPID(pPC)->MoveInitialState();
	}
}

bool CShipDefendDungeonManager::CheckForAllie(LPCHARACTER pAttacker, LPCHARACTER pVictim)
{
	using namespace ShipDefendHelper;

	if (!pAttacker || !pVictim)
	{
		return false;
	}

	if (pVictim->IsPC() || !pAttacker->IsPC())
	{
		return false;
	}

	return (pVictim->GetRaceNum() == ALLIED_MAST_VNUM && FindDungeonByPID(pAttacker) == FindDungeonByVID(pVictim));
}
#endif

