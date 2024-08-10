#include "stdafx.h"
#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
#include "questmanager.h"
#include "char.h"
#include "config.h"
#include "ZuoEvent.hpp"

#undef sys_err
#ifndef __WIN32__
	#define sys_err(fmt, args...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, ##args)
#else
	#define sys_err(fmt, ...) quest::CQuestManager::instance().QuestError(__FUNCTION__, __LINE__, fmt, __VA_ARGS__)
#endif

namespace quest
{
int zuo_event_can_enter(lua_State * L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	if (!ch)
	{
		lua_pushnumber(L, -1);
		return 1;
	}

	if (g_bChannel != 1)
	{
		lua_pushnumber(L, 1);
		return 1;
	}

	if (ch->GetLevel() < CZuoEventManager::instance().GetMinimalRequiredLevel())
	{
		lua_pushnumber(L, 2);
		return 1;
	}

	if (CZuoEventManager::instance().GetEventStatus() != EEventType::TYPE_START)
	{
		lua_pushnumber(L, 3);
		return 1;
	}

	lua_pushnumber(L, 0);
	return 1;
}

int zuo_event_enter(lua_State * L)
{
	LPCHARACTER ch = CQuestManager::instance().GetCurrentCharacterPtr();
	if (!ch)
	{
		return 0;
	}

	ch->WarpSet(CZuoEventManager::instance().GetMapXWarp(), CZuoEventManager::instance().GetMapYWarp());
	return 0;
}

void RegisterZuoEventFunctionTable()
{
	luaL_reg zuo_event_functions[] =
	{
		{ "can_enter",			zuo_event_can_enter	},
		{ "enter",			zuo_event_enter	},
		{ NULL,				NULL			}
	};

	CQuestManager::instance().AddLuaFunctionTable("zuo_event", zuo_event_functions);
}
}
#endif

