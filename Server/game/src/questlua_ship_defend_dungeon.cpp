#include "stdafx.h"
#ifdef __SHIP_DUNGEON_ENABLE__
#include "questmanager.h"
#include "char.h"
#include "dungeon.h"
#include "party.h"
#include "sectree_manager.h"
#include "sectree.h"
#include "ShipDefendDungeonManager.hpp"
#include "ShipDefendDungeon.hpp"

#undef sys_err
#ifndef __WIN32__
	#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
	#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{
int ship_defend_dungeon_check_status(lua_State * L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	if (!ch)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	if (CShipDefendDungeonManager::instance().FindDungeonByPID(ch))
	{
		lua_pushnumber(L, CShipDefendDungeonManager::instance().FindDungeonByPID(ch)->IsEnded() ? false : true);
	}
	else
	{
		lua_pushnumber(L, 0);
	}

	return 1;
}

int ship_defend_dungeon_join(lua_State * L)
{
	// Make sure all requirements are conducted by lua
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	if (!ch)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	LPDUNGEON pDungeon = CDungeonManager::instance().Create(lua_tonumber(L, 1));
	if (!pDungeon)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
	auto lX = pkSectreeMap->m_setting.posSpawn.x;
	auto lY = pkSectreeMap->m_setting.posSpawn.y;

	if (!CShipDefendDungeonManager::instance().RegisterAttender(ch, pDungeon))
	{
		CDungeonManager::instance().Destroy(pDungeon->GetId());
		lua_pushnumber(L, 0);
	}
	else
	{
		lua_pushnumber(L, 1);
		if (ch->GetParty() && ch->GetParty()->GetLeaderPID() == ch->GetPlayerID())
		{
			pDungeon->JumpParty(ch->GetParty(), ch->GetMapIndex(), lX, lY);
		}
		else if (!ch->GetParty())
		{
			pDungeon->Join(ch);
		}
	}

	return 1;
}

int ship_defend_dungeon_rejoin(lua_State * L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	if (!ch)
	{
		return 0;
	}

	CShipDefendDungeon * pDungeon;
	if ((pDungeon = CShipDefendDungeonManager::instance().FindDungeonByPID(ch)))
	{
		lua_pushnumber(L, 0);
		pDungeon->GetDungeon()->Join(ch);
	}
	else
	{
		lua_pushnumber(L, 0);
	}

	return 1;
}

void RegisterShipDefendDungeonFunctionTable()
{
	luaL_reg ship_defend_dungeon_functions[] =
	{
		{ "check_status",			ship_defend_dungeon_check_status	},
		{ "join",			ship_defend_dungeon_join	},
		{ "rejoin",			ship_defend_dungeon_rejoin	},
		{ NULL,				NULL			}
	};

	CQuestManager::instance().AddLuaFunctionTable("sdd", ship_defend_dungeon_functions);
}
}
#endif

