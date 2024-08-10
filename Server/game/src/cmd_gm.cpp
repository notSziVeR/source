#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "sectree_manager.h"
#include "mob_manager.h"
#include "packet.h"
#include "cmd.h"
#include "regen.h"
#include "guild.h"
#include "guild_manager.h"
#include "p2p.h"
#include "buffer_manager.h"
#include "fishing.h"
#include "mining.h"
#include "questmanager.h"
#include "vector.h"
#include "affect.h"
#include "db.h"
#include "priv_manager.h"
#include "building.h"
#include "battle.h"
#include "arena.h"
#include "start_position.h"
#include "party.h"
#include "castle.h"
#include "BattleArena.h"
#include "xmas_event.h"
#include "log.h"
#include "pcbang.h"
#include "threeway_war.h"
#include "unique_item.h"
#include "DragonSoul.h"
#include "../../common/CommonDefines.h"

#include "OXEvent.h"

#include "shop_manager.h"

#include "../../common/billing.h"

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	#include "TechnicalMaintenance.hpp"
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	#include "ZuoEvent.hpp"
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	#include "EventFunctionHandler.hpp"
	#include "EventManager.hpp"
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	#include "NotificatonSender.hpp"
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	#include "MonsterRespawner.hpp"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
	#include "gm.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __SHOP_SEARCH__
	#include "ShopSearchManager.h"
#endif

#ifdef __ITEM_SHOP_ENABLE__
	#include "ItemShopManager.hpp"
#endif

#ifdef __ENABLE_MISSION_MANAGER__
	#include "MissionManager.hpp"
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	#include "TeleportManagerSystem.hpp"
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	#include "AttendanceRewardManager.hpp"
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	#include "BeginnerHelper.hpp"
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	#include "BiologSystemManager.hpp"
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	#include "MarbleCreatorSystem.hpp"
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	#include "MobTrackerManager.hpp"
#endif

#include "EquipmentSet.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

// ston<int32_t>, ston<uint32_t>, etc.
#include <sstream>
template <typename T>
T ston(const std::string& str)
{
	T value;

	std::istringstream input(str);
	input >> value;

	return value;
}

extern bool DropEvent_RefineBox_SetValue(const std::string& name, int value);

// ADD_COMMAND_SLOW_STUN
enum
{
	COMMANDAFFECT_STUN,
	COMMANDAFFECT_SLOW,
};

void Command_ApplyAffect(LPCHARACTER ch, const char* argument, const char* affectName, int cmdAffect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	sys_log(0, arg1);

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: %s <name>", affectName);
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s is not in same map", arg1);
		return;
	}

	switch (cmdAffect)
	{
	case COMMANDAFFECT_STUN:
		SkillAttackAffect(tch, 1000, IMMUNE_STUN, AFFECT_STUN, POINT_NONE, 0, AFF_STUN, 30, "GM_STUN");
		break;
	case COMMANDAFFECT_SLOW:
		SkillAttackAffect(tch, 1000, IMMUNE_SLOW, AFFECT_SLOW, POINT_MOV_SPEED, -30, AFF_SLOW, 30, "GM_SLOW");
		break;
	}

	sys_log(0, "%s %s", arg1, affectName);

	ch->ChatPacket(CHAT_TYPE_INFO, "%s %s", arg1, affectName);
}
// END_OF_ADD_COMMAND_SLOW_STUN

ACMD(do_pcbang_update)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	unsigned long PCBangID = 0;

	if (*arg1 == '\0')
	{
		PCBangID = 0;
	}
	else
	{
		str_to_number(PCBangID, arg1);
	}

	if (PCBangID == 0)
	{
		CPCBangManager::instance().RequestUpdateIPList(0);
		ch->ChatPacket(CHAT_TYPE_INFO, "PCBang Info Update For All");
	}
	else
	{
		CPCBangManager::instance().RequestUpdateIPList(PCBangID);
		ch->ChatPacket(CHAT_TYPE_INFO, "PCBang Info Update For %u", PCBangID);
	}

	TPacketPCBangUpdate packet;
	packet.bHeader = HEADER_GG_PCBANG_UPDATE;
	packet.ulPCBangID = PCBangID;

	P2P_MANAGER::instance().Send(&packet, sizeof(TPacketPCBangUpdate));

}

ACMD(do_pcbang_check)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (CPCBangManager::instance().IsPCBangIP(arg1) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s is a PCBang IP", arg1);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s is not a PCBang IP", arg1);
	}
}

ACMD(do_stun)
{
	Command_ApplyAffect(ch, argument, "stun", COMMANDAFFECT_STUN);
}

ACMD(do_slow)
{
	Command_ApplyAffect(ch, argument, "slow", COMMANDAFFECT_SLOW);
}

ACMD(do_transfer)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: transfer <name>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(arg1);

		if (pkCCI)
		{
			if (pkCCI->bChannel != g_bChannel)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Target(%s) is in %d channel (my channel %d)", arg1, pkCCI->bChannel, g_bChannel);
				return;
			}

			TPacketGGTransfer pgg;

			pgg.bHeader = HEADER_GG_TRANSFER;
			strlcpy(pgg.szName, arg1, sizeof(pgg.szName));
			pgg.lX = ch->GetX();
			pgg.lY = ch->GetY();

			P2P_MANAGER::instance().Send(&pgg, sizeof(TPacketGGTransfer));
			ch->ChatPacket(CHAT_TYPE_INFO, "Transfer requested.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no character(%s) by that name", arg1);
		}

		return;
	}

	if (ch == tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Transfer me?!?");
		return;
	}

	//tch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
	tch->WarpSet(ch->GetX(), ch->GetY(), ch->GetMapIndex());
}

// LUA_ADD_GOTO_INFO
struct GotoInfo
{
	std::string 	st_name;

	BYTE 	empire;
	int 	mapIndex;
	DWORD 	x, y;

	GotoInfo()
	{
		st_name 	= "";
		empire 		= 0;
		mapIndex 	= 0;

		x = 0;
		y = 0;
	}
	GotoInfo(const GotoInfo& c_src)
	{
		__copy__(c_src);
	}
	void operator = (const GotoInfo& c_src)
	{
		__copy__(c_src);
	}
	void __copy__(const GotoInfo& c_src)
	{
		st_name 	= c_src.st_name;
		empire 		= c_src.empire;
		mapIndex 	= c_src.mapIndex;

		x = c_src.x;
		y = c_src.y;
	}
};

static std::vector<GotoInfo> gs_vec_gotoInfo;

void CHARACTER_AddGotoInfo(const std::string& c_st_name, BYTE empire, int mapIndex, DWORD x, DWORD y)
{
	GotoInfo newGotoInfo;
	newGotoInfo.st_name = c_st_name;
	newGotoInfo.empire = empire;
	newGotoInfo.mapIndex = mapIndex;
	newGotoInfo.x = x;
	newGotoInfo.y = y;
	gs_vec_gotoInfo.push_back(newGotoInfo);

	sys_log(0, "AddGotoInfo(name=%s, empire=%d, mapIndex=%d, pos=(%d, %d))", c_st_name.c_str(), empire, mapIndex, x, y);
}

bool FindInString(const char * c_pszFind, const char * c_pszIn)
{
	const char * c = c_pszIn;
	const char * p;

	p = strchr(c, '|');

	if (!p)
	{
		return (0 == strncasecmp(c_pszFind, c_pszIn, strlen(c_pszFind)));
	}
	else
	{
		char sz[64 + 1];

		do
		{
			strlcpy(sz, c, MIN(sizeof(sz), (p - c) + 1));

			if (!strncasecmp(c_pszFind, sz, strlen(c_pszFind)))
			{
				return true;
			}

			c = p + 1;
		} while ((p = strchr(c, '|')));

		strlcpy(sz, c, sizeof(sz));

		if (!strncasecmp(c_pszFind, sz, strlen(c_pszFind)))
		{
			return true;
		}
	}

	return false;
}

bool CHARACTER_GoToName(LPCHARACTER ch, BYTE empire, int mapIndex, const char* gotoName)
{
	std::vector<GotoInfo>::iterator i;
	for (i = gs_vec_gotoInfo.begin(); i != gs_vec_gotoInfo.end(); ++i)
	{
		const GotoInfo& c_eachGotoInfo = *i;

		if (mapIndex != 0)
		{
			if (mapIndex != c_eachGotoInfo.mapIndex)
			{
				continue;
			}
		}
		else if (!FindInString(gotoName, c_eachGotoInfo.st_name.c_str()))
		{
			continue;
		}

		if (c_eachGotoInfo.empire == 0 || c_eachGotoInfo.empire == empire)
		{
			int x = c_eachGotoInfo.x * 100;
			int y = c_eachGotoInfo.y * 100;

			ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", x, y);
			ch->WarpSet(x, y);
			ch->Stop();
			return true;
		}
	}
	return false;
}

// END_OF_LUA_ADD_GOTO_INFO




ACMD(do_goto)
{
	char arg1[256], arg2[256];
	int x = 0, y = 0, z = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 && !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: goto <x meter> <y meter>");
		return;
	}

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(x, arg1);
		str_to_number(y, arg2);

		PIXEL_POSITION p;

		if (SECTREE_MANAGER::instance().GetMapBasePosition(ch->GetX(), ch->GetY(), p))
		{
			x += p.x / 100;
			y += p.y / 100;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "You goto ( %d, %d )", x, y);
	}
	else
	{
		int mapIndex = 0;
		BYTE empire = 0;

		if (*arg1 == '#')
		{
			str_to_number(mapIndex,  (arg1 + 1));
		}

		if (*arg2 && isnhdigit(*arg2))
		{
			str_to_number(empire, arg2);
			empire = MINMAX(1, empire, 3);
		}
		else
		{
			empire = ch->GetEmpire();
		}

		if (CHARACTER_GoToName(ch, empire, mapIndex, arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Cannot find map command syntax: /goto <mapname> [empire]");
			return;
		}

		return;

		/*
		   int iMapIndex = 0;
		   for (int i = 0; aWarpInfo[i].c_pszName != NULL; ++i)
		   {
		   if (iMapIndex != 0)
		   {
		   if (iMapIndex != aWarpInfo[i].iMapIndex)
		   continue;
		   }
		   else if (!FindInString(arg1, aWarpInfo[i].c_pszName))
		   continue;

		   if (aWarpInfo[i].bEmpire == 0 || aWarpInfo[i].bEmpire == bEmpire)
		   {
		   x = aWarpInfo[i].x * 100;
		   y = aWarpInfo[i].y * 100;

		   ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", x, y);
		   ch->WarpSet(x, y);
		   ch->Stop();
		   return;
		   }
		   }
		 */

	}

	x *= 100;
	y *= 100;

	ch->Show(ch->GetMapIndex(), x, y, z);
	ch->Stop();
}

ACMD(do_warp)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: warp <character name> | <x meter> <y meter>");
		return;
	}

	int x = 0, y = 0;

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(x, arg1);
		str_to_number(y, arg2);
	}
	else
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

		if (NULL == tch)
		{
			const CCI* pkCCI = P2P_MANAGER::instance().Find(arg1);

			if (NULL != pkCCI)
			{
				//if (pkCCI->bChannel != g_bChannel)
				//{
				//	ch->ChatPacket(CHAT_TYPE_INFO, "Target(%s) is in %d channel (my channel %d)", arg1, pkCCI->bChannel, g_bChannel);
				//	return;
				//}

				ch->WarpToPID( pkCCI->dwPID );
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "There is no one(%s) by that name", arg1);
			}

			return;
		}
		else
		{
			x = tch->GetX() / 100;
			y = tch->GetY() / 100;
		}
	}

	x *= 100;
	y *= 100;

	ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", x, y);
	ch->WarpSet(x, y);
	ch->Stop();
}

#ifdef __ENABLE_NEWSTUFF__
ACMD(do_rewarp)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "You warp to ( %d, %d )", ch->GetX(), ch->GetY());
	ch->WarpSet(ch->GetX(), ch->GetY());
	ch->Stop();
}
#endif

ACMD(do_item)
{
	auto args = split_string(argument);

	if (args.size() < 1)
	{
		ch->RawChatPacket(CHAT_TYPE_INFO, "Usage: item <vnum / name> [<count>] [<locale>]");
		return;
	}

	DWORD vnum = 0;
	if (str_is_int(args[0]))
	{
		vnum = ston<uint32_t>(args[0]);
	}
	else
	{
		if (args.size() >= 3 && str_is_int(args[2]))
		{
			vnum = LC::FindItemVnumByLocalization(args[0], ston<int32_t>(args[2]));
		}
		else
		{
			vnum = LC::FindItemVnumByLocalization(args[0], ch->GetLocale());
		}
	}

	if (vnum == 0)
	{
		ch->RawChatPacket(CHAT_TYPE_INFO, "No vnum found for %s.", args[0].c_str());
		return;
	}

	int count = 1;
	if (args.size() >= 2 && str_is_int(args[1]))
	{
		count = ston<int32_t>(args[1]);
	}

	if (!ch->AutoGiveItem(vnum, count, -1, true, false, true))
	{
		ch->RawChatPacket(CHAT_TYPE_INFO, "No item found by #%u.", vnum);
	}
}

ACMD(do_group_random)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: grrandom <group vnum>");
		return;
	}

	DWORD dwVnum = 0;
	str_to_number(dwVnum, arg1);
	CHARACTER_MANAGER::instance().SpawnGroupGroup(dwVnum, ch->GetMapIndex(), ch->GetX() - 500, ch->GetY() - 500, ch->GetX() + 500, ch->GetY() + 500);
}

ACMD(do_group)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: group <group vnum>");
		return;
	}

	DWORD dwVnum = 0;
	str_to_number(dwVnum, arg1);

	if (test_server)
	{
		sys_log(0, "COMMAND GROUP SPAWN %u at %u %u %u", dwVnum, ch->GetMapIndex(), ch->GetX(), ch->GetY());
	}

	CHARACTER_MANAGER::instance().SpawnGroup(dwVnum, ch->GetMapIndex(), ch->GetX() - 500, ch->GetY() - 500, ch->GetX() + 500, ch->GetY() + 500);
}

ACMD(do_mob_coward)
{
	char	arg1[256], arg2[256];
	DWORD	vnum = 0;
	LPCHARACTER	tch;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mc <vnum>");
		return;
	}

	const CMob * pkMob;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
		{
			vnum = 0;
		}
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
		{
			vnum = pkMob->m_table.dwVnum;
		}
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No such mob(%s) by that vnum", arg1);
		return;
	}

	int iCount = 0;

	if (*arg2)
	{
		str_to_number(iCount, arg2);
	}
	else
	{
		iCount = 1;
	}

	iCount = MIN(20, iCount);

	while (iCount--)
	{
		tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
				ch->GetMapIndex(),
				ch->GetX() - number(200, 750),
				ch->GetY() - number(200, 750),
				ch->GetX() + number(200, 750),
				ch->GetY() + number(200, 750),
				true,
				pkMob->m_table.bType == CHAR_TYPE_STONE);
		if (tch)
		{
			tch->SetCoward();
		}
	}
}
#ifndef __MM_COMMAND_FEATURE__
ACMD(do_mob_map)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: mm <vnum>");
		return;
	}

	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(vnum, ch->GetMapIndex());

	if (tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s spawned in %dx%d", tch->GetName(), tch->GetX(), tch->GetY());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Spawn failed.");
	}
}
#else
ACMD(do_mob_map)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: mm <vnum> <count>");
		return;
	}

	DWORD vnum = 0;
	str_to_number(vnum, arg1);
	BYTE count = 0;
	str_to_number(count, arg2);

	count = MINMAX(1, count, 50);

	while (count--)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRandomPosition(vnum, ch->GetMapIndex());

		if (tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s spawned in %dx%d", tch->GetName(), tch->GetX(), tch->GetY());
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Spawn failed.");
		}
	}
}
#endif
ACMD(do_mob_aggresive)
{
	char	arg1[256], arg2[256];
	DWORD	vnum = 0;
	LPCHARACTER	tch;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mob <mob vnum>");
		return;
	}

	const CMob * pkMob;

	if (isdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
		{
			vnum = 0;
		}
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
		{
			vnum = pkMob->m_table.dwVnum;
		}
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No such mob(%s) by that vnum", arg1);
		return;
	}

	int iCount = 0;

	if (*arg2)
	{
		str_to_number(iCount, arg2);
	}
	else
	{
		iCount = 1;
	}

	iCount = MIN(20, iCount);

	while (iCount--)
	{
		tch = CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
				ch->GetMapIndex(),
				ch->GetX() - number(200, 750),
				ch->GetY() - number(200, 750),
				ch->GetX() + number(200, 750),
				ch->GetY() + number(200, 750),
				true,
				pkMob->m_table.bType == CHAR_TYPE_STONE);
		if (tch)
		{
			tch->SetAggressive();
		}
	}
}

ACMD(do_mob)
{
	auto args = split_string(argument);

	if (args.size() < 1)
	{
		ch->RawChatPacket(CHAT_TYPE_INFO, "Usage: mob <vnum / name> [<count>] [<locale>]");
		return;
	}

	DWORD vnum = 0;
	if (str_is_int(args[0]))
	{
		vnum = ston<uint32_t>(args[0]);
	}
	else
	{
		if (args.size() >= 3 && str_is_int(args[2]))
		{
			vnum = LC::FindMobVnumByLocalization(args[0], ston<int32_t>(args[2]));
		}
		else
		{
			vnum = LC::FindMobVnumByLocalization(args[0], ch->GetLocale());
		}
	}

	auto mob = CMobManager::instance().Get(vnum);
	if (!mob)
	{
		ch->RawChatPacket(CHAT_TYPE_INFO, "No vnum found for %s.", args[0].c_str());
		return;
	}

	int count = 1;
	if (args.size() >= 2 && str_is_int(args[1]))
	{
		count = ston<int32_t>(args[1]);
	}

	while (count--)
	{
		CHARACTER_MANAGER::instance().SpawnMobRange(vnum,
			ch->GetMapIndex(),
			ch->GetX() - number(200, 750),
			ch->GetY() - number(200, 750),
			ch->GetX() + number(200, 750),
			ch->GetY() + number(200, 750),
			true,
			mob->m_table.bType == CHAR_TYPE_STONE);
	}
}

ACMD(do_mob_ld)
{
	char	arg1[256], arg2[256], arg3[256], arg4[256];
	DWORD	vnum = 0;

	two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: mob <mob vnum>");
		return;
	}

	const CMob* pkMob = NULL;

	if (isnhdigit(*arg1))
	{
		str_to_number(vnum, arg1);

		if ((pkMob = CMobManager::instance().Get(vnum)) == NULL)
		{
			vnum = 0;
		}
	}
	else
	{
		pkMob = CMobManager::Instance().Get(arg1, true);

		if (pkMob)
		{
			vnum = pkMob->m_table.dwVnum;
		}
	}

	if (vnum == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No such mob(%s) by that vnum", arg1);
		return;
	}

	int dir = 1;
	long x = 0, y = 0;

	if (*arg2)
	{
		str_to_number(x, arg2);
	}
	if (*arg3)
	{
		str_to_number(y, arg3);
	}
	if (*arg4)
	{
		str_to_number(dir, arg4);
	}


	CHARACTER_MANAGER::instance().SpawnMob(vnum,
										   ch->GetMapIndex(),
										   x * 100,
										   y * 100,
										   ch->GetZ(),
										   pkMob->m_table.bType == CHAR_TYPE_STONE,
										   dir);
}

struct FuncPurge
{
	LPCHARACTER m_pkGM;
	bool	m_bAll;

	FuncPurge(LPCHARACTER ch) : m_pkGM(ch), m_bAll(false)
	{
	}

	void operator () (LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
		{
			return;
		}

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		if (!pkChr->IsNPC() || pkChr->IsPet() || pkChr->GetRider()
#ifdef __ENABLE_OFFLINE_SHOP__
				|| pkChr->GetKeepingOfflineShop()
#endif
#ifdef __ENABLE_SHAMAN_SYSTEM__
				|| pkChr->IsAutoShaman()
#endif
		   )
		{
			return;
		}

		int iDist = DISTANCE_APPROX(pkChr->GetX() - m_pkGM->GetX(), pkChr->GetY() - m_pkGM->GetY());

		if (!m_bAll && iDist >= 1000)
		{
			return;
		}

		sys_log(0, "PURGE: %s %d", pkChr->GetName(), iDist);
		M2_DESTROY_CHARACTER(pkChr);
	}
};

ACMD(do_purge)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	FuncPurge func(ch);

	if (*arg1 && !strcmp(arg1, "all"))
	{
		func.m_bAll = true;
	}

	LPSECTREE sectree = ch->GetSectree();
	if (sectree) // #431
	{
		sectree->ForEachAround(func);
	}
	else
	{
		sys_err("PURGE_ERROR.NULL_SECTREE(mapIndex=%d, pos=(%d, %d)", ch->GetMapIndex(), ch->GetX(), ch->GetY());
	}
}

#define ENABLE_CMD_IPURGE_EX
ACMD(do_item_purge)
{
#ifdef ENABLE_CMD_IPURGE_EX
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: ipurge <window>");
		ch->ChatPacket(CHAT_TYPE_INFO, "List of the available windows:");
		ch->ChatPacket(CHAT_TYPE_INFO, " all");
		ch->ChatPacket(CHAT_TYPE_INFO, " inventory or inv");
		ch->ChatPacket(CHAT_TYPE_INFO, " equipment or equip");
		ch->ChatPacket(CHAT_TYPE_INFO, " dragonsoul or ds");
		ch->ChatPacket(CHAT_TYPE_INFO, " belt");
		return;
	}

	int         i;
	LPITEM      item;

	std::string strArg(arg1);
	if (!strArg.compare(0, 3, "all"))
	{
		for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		{
			if ((item = ch->GetInventoryItem(i)))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
				ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
			}
		}
		for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
		{
			if ((item = ch->GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i ))))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
			}
		}
	}
	else if (!strArg.compare(0, 3, "inv"))
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		for (i = 0; i < INVENTORY_MAX_NUM; ++i)
#else
		for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#endif
		{
#ifdef __SPECIAL_STORAGE_ENABLE__
			// We ought to skip equipped items
			if (ch->GetInventoryItem(i) && (ch->GetInventoryItem(i)->GetCell() >= INVENTORY_MAX_NUM && ch->GetInventoryItem(i)->GetCell() < SPECIAL_STORAGE_START_CELL))
			{
				continue;
			}
#endif

			if ((item = ch->GetInventoryItem(i)))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
				ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
			}
		}
	}
	else if (!strArg.compare(0, 5, "equip"))
	{
		for (i = 0; i < WEAR_MAX_NUM; ++i)
		{
			if ((item = ch->GetInventoryItem(INVENTORY_MAX_NUM + i)))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
				ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, INVENTORY_MAX_NUM + i, 255);
			}
		}
	}
	else if (!strArg.compare(0, 6, "dragon") || !strArg.compare(0, 2, "ds"))
	{
		for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
		{
			if ((item = ch->GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i ))))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
			}
		}
	}
	else if (!strArg.compare(0, 4, "belt"))
	{
		for (i = 0; i < BELT_INVENTORY_SLOT_COUNT; ++i)
		{
			if ((item = ch->GetInventoryItem(BELT_INVENTORY_SLOT_START + i)))
			{
				ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
				ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, BELT_INVENTORY_SLOT_START + i, 255);
			}
		}
	}
#else
	int         i;
	LPITEM      item;

	for (i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
	{
		if ((item = ch->GetInventoryItem(i)))
		{
			ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
			ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = ch->GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i ))))
		{
			ITEM_MANAGER::instance().RemoveItem(item, "PURGE");
		}
	}
#endif
}

ACMD(do_state)
{
	char arg1[256];
	LPCHARACTER tch;

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		if (arg1[0] == '#')
		{
			tch = CHARACTER_MANAGER::instance().Find(strtoul(arg1 + 1, NULL, 10));
		}
		else
		{
			LPDESC d = DESC_MANAGER::instance().FindByCharacterName(arg1);

			if (!d)
			{
				tch = NULL;
			}
			else
			{
				tch = d->GetCharacter();
			}
		}
	}
	else
	{
		tch = ch;
	}

	if (!tch)
	{
		return;
	}

	char buf[256];

	snprintf(buf, sizeof(buf), "%s's State: ", tch->GetName());

	if (tch->IsPosition(POS_FIGHTING))
	{
		strlcat(buf, "Battle", sizeof(buf));
	}
	else if (tch->IsPosition(POS_DEAD))
	{
		strlcat(buf, "Dead", sizeof(buf));
	}
	else
	{
		strlcat(buf, "Standing", sizeof(buf));
	}

	if (ch->GetShop())
	{
		strlcat(buf, ", Shop", sizeof(buf));
	}

	if (ch->GetExchange())
	{
		strlcat(buf, ", Exchange", sizeof(buf));
	}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	ch->RawChatPacket(CHAT_TYPE_INFO, "%s", buf);
#else
	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);
#endif

	int len;
	len = snprintf(buf, sizeof(buf), "Coordinate %ldx%ld (%ldx%ld)",
				   tch->GetX(), tch->GetY(), tch->GetX() / 100, tch->GetY() / 100);

	if (len < 0 || len >= (int) sizeof(buf))
	{
		len = sizeof(buf) - 1;
	}

	LPSECTREE pSec = SECTREE_MANAGER::instance().Get(tch->GetMapIndex(), tch->GetX(), tch->GetY());

	if (pSec)
	{
		TMapSetting& map_setting = SECTREE_MANAGER::instance().GetMap(tch->GetMapIndex())->m_setting;
		snprintf(buf + len, sizeof(buf) - len, " MapIndex %ld Attribute %08X Local Position (%ld x %ld)",
				 tch->GetMapIndex(), pSec->GetAttribute(tch->GetX(), tch->GetY()), (tch->GetX() - map_setting.iBaseX) / 100, (tch->GetY() - map_setting.iBaseY) / 100);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "%s", buf);

	ch->ChatPacket(CHAT_TYPE_INFO, "LEV %d", tch->GetLevel());
	ch->ChatPacket(CHAT_TYPE_INFO, "HP %d/%d", tch->GetHP(), tch->GetMaxHP());
	ch->ChatPacket(CHAT_TYPE_INFO, "SP %d/%d", tch->GetSP(), tch->GetMaxSP());
	ch->ChatPacket(CHAT_TYPE_INFO, "ATT %d MAGIC_ATT %d SPD %d CRIT %d%% PENE %d%% ATT_BONUS %d%%",
				   tch->GetPoint(POINT_ATT_GRADE),
				   tch->GetPoint(POINT_MAGIC_ATT_GRADE),
				   tch->GetPoint(POINT_ATT_SPEED),
				   tch->GetPoint(POINT_CRITICAL_PCT),
				   tch->GetPoint(POINT_PENETRATE_PCT),
				   tch->GetPoint(POINT_ATT_BONUS));
	ch->ChatPacket(CHAT_TYPE_INFO, "DEF %d MAGIC_DEF %d BLOCK %d%% DODGE %d%% DEF_BONUS %d%%",
				   tch->GetPoint(POINT_DEF_GRADE),
				   tch->GetPoint(POINT_MAGIC_DEF_GRADE),
				   tch->GetPoint(POINT_BLOCK),
				   tch->GetPoint(POINT_DODGE),
				   tch->GetPoint(POINT_DEF_BONUS));
	ch->ChatPacket(CHAT_TYPE_INFO, "RESISTANCES:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   WARR:%3d%% ASAS:%3d%% SURA:%3d%% SHAM:%3d%%"
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   " WOLF:%3d%%"
#endif
				   ,
				   tch->GetPoint(POINT_RESIST_WARRIOR),
				   tch->GetPoint(POINT_RESIST_ASSASSIN),
				   tch->GetPoint(POINT_RESIST_SURA),
				   tch->GetPoint(POINT_RESIST_SHAMAN)
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   , tch->GetPoint(POINT_RESIST_WOLFMAN)
#endif
				  );
	ch->ChatPacket(CHAT_TYPE_INFO, "   SWORD:%3d%% THSWORD:%3d%% DAGGER:%3d%% BELL:%3d%% FAN:%3d%% BOW:%3d%%"
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   " CLAW:%3d%%"
#endif
				   ,
				   tch->GetPoint(POINT_RESIST_SWORD),
				   tch->GetPoint(POINT_RESIST_TWOHAND),
				   tch->GetPoint(POINT_RESIST_DAGGER),
				   tch->GetPoint(POINT_RESIST_BELL),
				   tch->GetPoint(POINT_RESIST_FAN),
				   tch->GetPoint(POINT_RESIST_BOW)
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   , tch->GetPoint(POINT_RESIST_CLAW)
#endif
				  );
	ch->ChatPacket(CHAT_TYPE_INFO, "   FIRE:%3d%% ELEC:%3d%% MAGIC:%3d%% WIND:%3d%% CRIT:%3d%% PENE:%3d%%",
				   tch->GetPoint(POINT_RESIST_FIRE),
				   tch->GetPoint(POINT_RESIST_ELEC),
				   tch->GetPoint(POINT_RESIST_MAGIC),
				   tch->GetPoint(POINT_RESIST_WIND),
				   tch->GetPoint(POINT_RESIST_CRITICAL),
				   tch->GetPoint(POINT_RESIST_PENETRATE));
	ch->ChatPacket(CHAT_TYPE_INFO, "   ICE:%3d%% EARTH:%3d%% DARK:%3d%%",
				   tch->GetPoint(POINT_RESIST_ICE),
				   tch->GetPoint(POINT_RESIST_EARTH),
				   tch->GetPoint(POINT_RESIST_DARK));

#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	ch->ChatPacket(CHAT_TYPE_INFO, "   MAGICREDUCT:%3d%%", tch->GetPoint(POINT_RESIST_MAGIC_REDUCTION));
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "MALL:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   ATT:%3d%% DEF:%3d%% EXP:%3d%% ITEMx%d GOLDx%d",
				   tch->GetPoint(POINT_MALL_ATTBONUS),
				   tch->GetPoint(POINT_MALL_DEFBONUS),
				   tch->GetPoint(POINT_MALL_EXPBONUS),
				   tch->GetPoint(POINT_MALL_ITEMBONUS) / 10,
				   tch->GetPoint(POINT_MALL_GOLDBONUS) / 10);

	ch->ChatPacket(CHAT_TYPE_INFO, "BONUS:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   SKILL:%3d%% NORMAL:%3d%% SKILL_DEF:%3d%% NORMAL_DEF:%3d%%",
				   tch->GetPoint(POINT_SKILL_DAMAGE_BONUS),
				   tch->GetPoint(POINT_NORMAL_HIT_DAMAGE_BONUS),
				   tch->GetPoint(POINT_SKILL_DEFEND_BONUS),
				   tch->GetPoint(POINT_NORMAL_HIT_DEFEND_BONUS));

	ch->ChatPacket(CHAT_TYPE_INFO, "   HUMAN:%3d%% ANIMAL:%3d%% ORC:%3d%% MILGYO:%3d%% UNDEAD:%3d%%",
				   tch->GetPoint(POINT_ATTBONUS_HUMAN),
				   tch->GetPoint(POINT_ATTBONUS_ANIMAL),
				   tch->GetPoint(POINT_ATTBONUS_ORC),
				   tch->GetPoint(POINT_ATTBONUS_MILGYO),
				   tch->GetPoint(POINT_ATTBONUS_UNDEAD));

	ch->ChatPacket(CHAT_TYPE_INFO, "   DEVIL:%3d%% INSECT:%3d%% FIRE:%3d%% ICE:%3d%% DESERT:%3d%%",
				   tch->GetPoint(POINT_ATTBONUS_DEVIL),
				   tch->GetPoint(POINT_ATTBONUS_INSECT),
				   tch->GetPoint(POINT_ATTBONUS_FIRE),
				   tch->GetPoint(POINT_ATTBONUS_ICE),
				   tch->GetPoint(POINT_ATTBONUS_DESERT));

	ch->ChatPacket(CHAT_TYPE_INFO, "   TREE:%3d%% MONSTER:%3d%% METIN:%3d%% BOSS:%3d%%",
				   tch->GetPoint(POINT_ATTBONUS_TREE),
				   tch->GetPoint(POINT_ATTBONUS_MONSTER),
				   tch->GetPoint(POINT_ATTBONUS_METIN),
				   tch->GetPoint(POINT_ATTBONUS_BOSS));

	ch->ChatPacket(CHAT_TYPE_INFO, "   WARR:%3d%% ASAS:%3d%% SURA:%3d%% SHAM:%3d%%"
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   " WOLF:%3d%%"
#endif
				   ,
				   tch->GetPoint(POINT_ATTBONUS_WARRIOR),
				   tch->GetPoint(POINT_ATTBONUS_ASSASSIN),
				   tch->GetPoint(POINT_ATTBONUS_SURA),
				   tch->GetPoint(POINT_ATTBONUS_SHAMAN)
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				   , tch->GetPoint(POINT_ATTBONUS_WOLFMAN)
#endif
				  );

#ifdef __ENABLE_12ZI_ELEMENT_ADD__
	ch->ChatPacket(CHAT_TYPE_INFO, "Bonus elementales:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   ATT_ELECT:%3d%% ATT_FIRE:%3d%% ATT_ICE:%3d%% ATT_WIND:%3d%% ATT_EARTH:%3d%% ATT_DARK:%3d%%",
				   tch->GetPoint(POINT_ATTBONUS_ELEC),
				   tch->GetPoint(POINT_ATTBONUS_FIRE),
				   tch->GetPoint(POINT_ATTBONUS_ICE),
				   tch->GetPoint(POINT_ATTBONUS_WIND),
				   tch->GetPoint(POINT_ATTBONUS_EARTH),
				   tch->GetPoint(POINT_ATTBONUS_DARK));

	ch->ChatPacket(CHAT_TYPE_INFO, "Bonus nuevos:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   ATT_CZ:%3d%%", tch->GetPoint(POINT_ATTBONUS_CZ));
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "Nowe odpornościowe: HUMAN:%3d%% MONSTER: %3d%%, BOSS: %3d%%",
				   tch->GetPoint(POINT_RESIST_ALL),
				   tch->GetPoint(POINT_RESIST_MONSTER),
				   tch->GetPoint(POINT_RESIST_BOSS));

	ch->ChatPacket(CHAT_TYPE_INFO, "IMMUNE:");
	ch->ChatPacket(CHAT_TYPE_INFO, "   STUN:%d SLOW:%d FALL:%d",
				   tch->GetPoint(POINT_IMMUNE_STUN),
				   tch->GetPoint(POINT_IMMUNE_SLOW),
				   tch->GetPoint(POINT_IMMUNE_FALL));

	for (int i = 0; i < MAX_PRIV_NUM; ++i)
		if (CPrivManager::instance().GetPriv(tch, i))
		{
			int iByEmpire = CPrivManager::instance().GetPrivByEmpire(tch->GetEmpire(), i);
			int iByGuild = 0;

			if (tch->GetGuild())
			{
				iByGuild = CPrivManager::instance().GetPrivByGuild(tch->GetGuild()->GetID(), i);
			}

			int iByPlayer = CPrivManager::instance().GetPrivByCharacter(tch->GetPlayerID(), i);

			if (iByEmpire)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s for empire : %d", LC_TEXT(c_apszPrivNames[i]), iByEmpire);
			}

			if (iByGuild)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s for guild : %d", LC_TEXT(c_apszPrivNames[i]), iByGuild);
			}

			if (iByPlayer)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s for player : %d", LC_TEXT(c_apszPrivNames[i]), iByPlayer);
			}
		}
}

struct notice_packet_func
{
	const char * m_str;
	notice_packet_func(const char* str) : m_str(str)
	{
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		d->GetCharacter()->RawChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
#else
		d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
#endif
	}
};

struct big_notice_packet_func
{
	const char* m_str;

	big_notice_packet_func(const char* str) : m_str(str)
	{
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		d->GetCharacter()->RawChatPacket(CHAT_TYPE_BIG_NOTICE, "%s", m_str);
#else
		d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, "%s", m_str);
#endif
	}
};

void SendNotice(const char* c_pszBuf)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_packet_func(c_pszBuf));
}

void SendBigNotice(const char* c_pszBuf)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), big_notice_packet_func(c_pszBuf));

}

struct notice_map_packet_func
{
	const char* m_str;
	int m_mapIndex;
	bool m_bBigFont;

	notice_map_packet_func(const char* str, int idx, bool bBigFont) : m_str(str), m_mapIndex(idx), m_bBigFont(bBigFont)
	{
	}

	void operator() (LPDESC d)
	{
		if (d->GetCharacter() == NULL) { return; }
		if (d->GetCharacter()->GetMapIndex() != m_mapIndex) { return; }

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		d->GetCharacter()->RawChatPacket(m_bBigFont == true ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", LC::TranslateText(d->GetCharacter(), LC::TRANSLATION_TYPE_GAME, m_str));
#else
		d->GetCharacter()->ChatPacket(m_bBigFont == true ? CHAT_TYPE_BIG_NOTICE : CHAT_TYPE_NOTICE, "%s", m_str);
#endif
	}
};

void SendNoticeMap(const char* c_pszBuf, int nMapIndex, bool bBigFont)
{
	const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_ref_set.begin(), c_ref_set.end(), notice_map_packet_func(c_pszBuf, nMapIndex, bBigFont));
}

struct log_packet_func
{
	const char * m_str;

	log_packet_func(const char * str) : m_str(str)
	{
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		if (d->GetCharacter()->GetGMLevel() > GM_PLAYER)
		{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
			d->GetCharacter()->RawChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
#else
			d->GetCharacter()->ChatPacket(CHAT_TYPE_NOTICE, "%s", m_str);
#endif
		}
	}
};


void SendLog(const char * c_pszBuf)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), log_packet_func(c_pszBuf));
}

void BroadcastNotice(const char * c_pszBuf)
{
	TPacketGGNotice p;
	p.bHeader = HEADER_GG_NOTICE;
	p.lSize = strlen(c_pszBuf) + 1;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	buf.write(c_pszBuf, p.lSize);

	P2P_MANAGER::instance().Send(buf.read_peek(), buf.size()); // HEADER_GG_NOTICE

	SendNotice(c_pszBuf);
}

void BroadcastBigNotice(const char* c_pszBuf)
{
	TPacketGGBigNotice p;
	p.bHeader = HEADER_GG_BIG_NOTICE;
	p.lSize = strlen(c_pszBuf) + 1;

	TEMP_BUFFER buf;
	buf.write(&p, sizeof(p));
	buf.write(c_pszBuf, p.lSize);

	P2P_MANAGER::instance().Send(buf.read_peek(), buf.size()); // HEADER_GG_BIG_NOTICE

	SendBigNotice(c_pszBuf);
}

ACMD(do_notice)
{
	BroadcastNotice(argument);
}

ACMD(do_map_notice)
{
	SendNoticeMap(argument, ch->GetMapIndex(), false);
}

ACMD(do_big_notice)
{
	//ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, "%s", argument);
	BroadcastBigNotice(argument);
}

#ifdef __ENABLE_FULL_NOTICE__
ACMD(do_map_big_notice)
{
	SendNoticeMap(argument, ch->GetMapIndex(), true);
}

ACMD(do_notice_test)
{
	ch->ChatPacket(CHAT_TYPE_NOTICE, "%s", argument);
}

ACMD(do_big_notice_test)
{
	ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, "%s", argument);
}
#endif

ACMD(do_who)
{
	int iTotal;
	int * paiEmpireUserCount;
	int iLocal;

	DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

	ch->ChatPacket(CHAT_TYPE_INFO, "Total [%d] %d / %d / %d (this server %d)",
				   iTotal, paiEmpireUserCount[1], paiEmpireUserCount[2], paiEmpireUserCount[3], iLocal);
}

class user_func
{
public:
	LPCHARACTER	m_ch;
	static int count;
	static char str[128];
	static int str_len;

	user_func()
		: m_ch(NULL)
	{}

	void initialize(LPCHARACTER ch)
	{
		m_ch = ch;
		str_len = 0;
		count = 0;
		str[0] = '\0';
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		int len = snprintf(str + str_len, sizeof(str) - str_len, "%-16s ", d->GetCharacter()->GetName());

		if (len < 0 || len >= (int) sizeof(str) - str_len)
		{
			len = (sizeof(str) - str_len) - 1;
		}

		str_len += len;
		++count;

		if (!(count % 4))
		{
			m_ch->ChatPacket(CHAT_TYPE_INFO, str);

			str[0] = '\0';
			str_len = 0;
		}
	}
};

int	user_func::count = 0;
char user_func::str[128] = { 0, };
int	user_func::str_len = 0;

ACMD(do_user)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	user_func func;

	func.initialize(ch);
	std::for_each(c_ref_set.begin(), c_ref_set.end(), func);

	if (func.count % 4)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, func.str);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Total %d", func.count);
}

ACMD(do_disconnect)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /dc <player name>");
		return;
	}

	LPDESC d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER	tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player.", arg1);
		return;
	}

	if (tch == ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "cannot disconnect myself");
		return;
	}

#ifdef __ENABLE_ANTY_AUTO_LOGIN__
	//delete login key
	if (d)
	{
		TPacketDC p;
		strlcpy(p.login, d->GetAccountTable().login, sizeof(p.login));
		db_clientdesc->DBPacket(HEADER_GD_DC, 0, &p, sizeof(p));
	}
#endif

	DESC_MANAGER::instance().DestroyDesc(d);
}

ACMD(do_kill)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /kill <player name>");
		return;
	}

	LPDESC	d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player", arg1);
		return;
	}

	tch->Dead();
}

#ifdef __ENABLE_NEWSTUFF__
ACMD(do_poison)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /poison <player name>");
		return;
	}

	LPDESC	d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player", arg1);
		return;
	}

	tch->AttackedByPoison(NULL);
}
#endif
#ifdef __ENABLE_WOLFMAN_CHARACTER__
ACMD(do_bleeding)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ex) /bleeding <player name>");
		return;
	}

	LPDESC	d = DESC_MANAGER::instance().FindByCharacterName(arg1);
	LPCHARACTER tch = d ? d->GetCharacter() : NULL;

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s: no such a player", arg1);
		return;
	}

	tch->AttackedByBleeding(NULL);
}
#endif

#define MISC    0
#define BINARY  1
#define NUMBER  2

namespace DoSetTypes
{
typedef enum do_set_types_s {GOLD, RACE, SEX, JOB, EXP, MAX_HP, MAX_SP, SKILL, ALIGNMENT, ALIGN} do_set_types_t;
}

const struct set_struct
{
	const char *cmd;
	const char type;
	const char * help;
} set_fields[] =
{
	{ "gold",		NUMBER,	NULL	},
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	{ "race",		NUMBER,	"0. Warrior, 1. Ninja, 2. Sura, 3. Shaman, 4. Lycan"		},
#else
	{ "race",		NUMBER,	"0. Warrior, 1. Ninja, 2. Sura, 3. Shaman"		},
#endif
	{ "sex",		NUMBER,	"0. Male, 1. Female"	},
	{ "job",		NUMBER,	"0. None, 1. First, 2. Second"	},
	{ "exp",		NUMBER,	NULL	},
	{ "max_hp",		NUMBER,	NULL	},
	{ "max_sp",		NUMBER,	NULL	},
	{ "skill",		NUMBER,	NULL	},
	{ "alignment",	NUMBER,	NULL	},
	{ "align",		NUMBER,	NULL	},
	{ "\n",			MISC,	NULL	}
};

ACMD(do_set)
{
	char arg1[256], arg2[256], arg3[256];

	LPCHARACTER tch = NULL;

	int i, len;
	const char* line;

	line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: set <name> <field> <value>");
#ifdef __ENABLE_NEWSTUFF__
		ch->ChatPacket(CHAT_TYPE_INFO, "List of the fields available:");
		for (i = 0; * (set_fields[i].cmd) != '\n'; i++)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, " %d. %s", i + 1, set_fields[i].cmd);
			if (set_fields[i].help != NULL)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "  Help: %s", set_fields[i].help);
			}
		}
#endif
		return;
	}

	tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s not exist", arg1);
		return;
	}

#ifdef __ADMIN_MANAGER__
	if (tch != ch && !GM::check_allow(ch->GetGMLevel(), GM_ALLOW_MODIFY_OTHERS))
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have no rights to modify other players.");
		}
		return;
	}
#endif

	len = strlen(arg2);

	for (i = 0; * (set_fields[i].cmd) != '\n'; i++)
		if (!strncmp(arg2, set_fields[i].cmd, len))
		{
			break;
		}

	switch (i)
	{
	case DoSetTypes::GOLD:	// gold
	{
#ifdef __EXTANDED_GOLD_AMOUNT__
		int64_t gold = 0;
#else
		int gold = 0;
#endif
		str_to_number(gold, arg3);
		DBManager::instance().SendMoneyLog(MONEY_LOG_MISC, 3, gold);
		tch->PointChange(POINT_GOLD, gold, true);
	}
	break;

	case DoSetTypes::RACE: // race
#ifdef __ENABLE_NEWSTUFF__
	{
		int amount = 0;
		str_to_number(amount, arg3);
		amount = MINMAX(0, amount, JOB_MAX_NUM);
		ESex mySex = GET_SEX(tch);
		DWORD dwRace = MAIN_RACE_WARRIOR_M;
		switch (amount)
		{
		case JOB_WARRIOR:
			dwRace = (mySex == SEX_MALE) ? MAIN_RACE_WARRIOR_M : MAIN_RACE_WARRIOR_W;
			break;
		case JOB_ASSASSIN:
			dwRace = (mySex == SEX_MALE) ? MAIN_RACE_ASSASSIN_M : MAIN_RACE_ASSASSIN_W;
			break;
		case JOB_SURA:
			dwRace = (mySex == SEX_MALE) ? MAIN_RACE_SURA_M : MAIN_RACE_SURA_W;
			break;
		case JOB_SHAMAN:
			dwRace = (mySex == SEX_MALE) ? MAIN_RACE_SHAMAN_M : MAIN_RACE_SHAMAN_W;
			break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		case JOB_WOLFMAN:
			dwRace = (mySex == SEX_MALE) ? MAIN_RACE_WOLFMAN_M : MAIN_RACE_WOLFMAN_M;
			break;
#endif
		}
		if (dwRace != tch->GetRaceNum())
		{
			tch->SetRace(dwRace);
			tch->ClearSkill();
			tch->SetSkillGroup(0);
			// quick mesh change workaround begin
			tch->SetPolymorph(101);
			tch->SetPolymorph(0);
			// quick mesh change workaround end
		}
	}
#endif
	break;

	case DoSetTypes::SEX: // sex
#ifdef __ENABLE_NEWSTUFF__
	{
		int amount = 0;
		str_to_number(amount, arg3);
		amount = MINMAX(SEX_MALE, amount, SEX_FEMALE);
		if (amount != GET_SEX(tch))
		{
			tch->ChangeSex();
			// quick mesh change workaround begin
			tch->SetPolymorph(101);
			tch->SetPolymorph(0);
			// quick mesh change workaround end
		}
	}
#endif
	break;

	case DoSetTypes::JOB: // job
#ifdef __ENABLE_NEWSTUFF__
	{
		int amount = 0;
		str_to_number(amount, arg3);
		amount = MINMAX(0, amount, 2);
		if (amount != tch->GetSkillGroup())
		{
			tch->ClearSkill();
			tch->SetSkillGroup(amount);
		}
	}
#endif
	break;

	case DoSetTypes::EXP: // exp
	{
		int amount = 0;
		str_to_number(amount, arg3);
		tch->PointChange(POINT_EXP, amount, true);
	}
	break;

	case DoSetTypes::MAX_HP: // max_hp
	{
		int amount = 0;
		str_to_number(amount, arg3);
		tch->PointChange(POINT_MAX_HP, amount, true);
	}
	break;

	case DoSetTypes::MAX_SP: // max_sp
	{
		int amount = 0;
		str_to_number(amount, arg3);
		tch->PointChange(POINT_MAX_SP, amount, true);
	}
	break;

	case DoSetTypes::SKILL: // active skill point
	{
		int amount = 0;
		str_to_number(amount, arg3);
		tch->PointChange(POINT_SKILL, amount, true);
	}
	break;

	case DoSetTypes::ALIGN: // alignment
	case DoSetTypes::ALIGNMENT: // alignment
	{
		int	amount = 0;
		str_to_number(amount, arg3);
		tch->UpdateAlignment(amount - ch->GetRealAlignment());
	}
	break;
	}

	if (set_fields[i].type == NUMBER)
	{
#ifdef __EXTANDED_GOLD_AMOUNT__
		int64_t amount = 0;
#else
		int	amount = 0;
#endif
		str_to_number(amount, arg3);
#ifdef __EXTANDED_GOLD_AMOUNT__
		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s set to [%lld]", tch->GetName(), set_fields[i].cmd, amount);
#else
		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s set to [%d]", tch->GetName(), set_fields[i].cmd, amount);
#endif
	}
}

ACMD(do_reset)
{
	ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
	ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
	ch->Save();
}

ACMD(do_advance)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: advance <name> <level>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%s not exist", arg1);
		return;
	}

	int level = 0;
	str_to_number(level, arg2);

	tch->ResetPoint(MINMAX(0, level, gPlayerMaxLevel));

	tch->ClearSkill();
	tch->ClearSubSkill();
}

ACMD(do_respawn)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1 && !strcasecmp(arg1, "all"))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Respaw everywhere");
		regen_reset(0, 0);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Respaw around");
		regen_reset(ch->GetX(), ch->GetY());
	}
}

ACMD(do_safebox_size)
{

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int size = 0;

	if (*arg1)
	{
		str_to_number(size, arg1);
	}

	if (size > 3 || size < 0)
	{
		size = 0;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Safebox size set to %d", size);
	ch->ChangeSafeboxSize(size);
}

ACMD(do_makeguild)
{
	if (ch->GetGuild())
	{
		return;
	}

	CGuildManager& gm = CGuildManager::instance();

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, arg1, sizeof(cp.name));

	if (!check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ŔűÇŐÇĎÁö ľĘŔş ±ćµĺ ŔĚ¸§ ŔÔ´Ď´Ů."));
		return;
	}

	gm.CreateGuild(cp);
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("(%s) ±ćµĺ°ˇ »ýĽşµÇľú˝Ŕ´Ď´Ů. [ŔÓ˝Ă]"), cp.name);
}

ACMD(do_deleteguild)
{
	if (ch->GetGuild())
	{
		ch->GetGuild()->RequestDisband(ch->GetPlayerID());
	}
}

ACMD(do_greset)
{
	if (ch->GetGuild())
	{
		ch->GetGuild()->Reset();
	}
}

// REFINE_ROD_HACK_BUG_FIX
ACMD(do_refine_rod)
{
	//char arg1[256];
	//one_argument(argument, arg1, sizeof(arg1));
	//
	//BYTE cell = 0;
	//str_to_number(cell, arg1);
	//LPITEM item = ch->GetInventoryItem(cell);
	//if (item)
	//	CFishing::RealRefineRod(ch, item);
}
// END_OF_REFINE_ROD_HACK_BUG_FIX

// REFINE_PICK
ACMD(do_refine_pick)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	BYTE cell = 0;
	str_to_number(cell, arg1);
	LPITEM item = ch->GetInventoryItem(cell);
	if (item)
	{
		mining::CHEAT_MAX_PICK(ch, item);
		mining::RealRefinePick(ch, item);
	}
}

ACMD(do_max_pick)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	BYTE cell = 0;
	str_to_number(cell, arg1);
	LPITEM item = ch->GetInventoryItem(cell);
	if (item)
	{
		mining::CHEAT_MAX_PICK(ch, item);
	}
}
// END_OF_REFINE_PICK


ACMD(do_fishing_simul)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];
	argument = one_argument(argument, arg1, sizeof(arg1));
	two_arguments(argument, arg2, sizeof(arg2), arg3, sizeof(arg3));

	int count = 1000;
	int prob_idx = 0;
	int level = 100;

	ch->ChatPacket(CHAT_TYPE_INFO, "Usage: fishing_simul <level> <prob index> <count>");

	if (*arg1)
	{
		str_to_number(level, arg1);
	}

	if (*arg2)
	{
		str_to_number(prob_idx, arg2);
	}

	if (*arg3)
	{
		str_to_number(count, arg3);
	}

	// fishing::Simulation(level, count, prob_idx, ch);
}

ACMD(do_invisibility)
{
	if (ch->IsAffectFlag(AFF_INVISIBILITY))
	{
		ch->RemoveAffect(AFFECT_INVISIBILITY);
	}
	else
	{
		ch->AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, INFINITE_AFFECT_DURATION, 0, true);
	}
}

ACMD(do_event_flag)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!(*arg1) || !(*arg2))
	{
		return;
	}

	int value = 0;
	str_to_number(value, arg2);

	if (!strcmp(arg1, "mob_item") ||
			!strcmp(arg1, "mob_exp") ||
			!strcmp(arg1, "mob_gold") ||
			!strcmp(arg1, "mob_dam") ||
			!strcmp(arg1, "mob_gold_pct") ||
			!strcmp(arg1, "mob_item_buyer") ||
			!strcmp(arg1, "mob_exp_buyer") ||
			!strcmp(arg1, "mob_gold_buyer") ||
			!strcmp(arg1, "mob_gold_pct_buyer")
	   )
	{
		value = MINMAX(0, value, 1000);
	}

#ifdef __ADMIN_MANAGER__
	if (!strcasecmp(arg1, EVENT_FLAG_GM_ITEM_TRADE_BLOCK))
	{
		if (ch && !CAdminManager::instance().HasAllow(ch, CAdminManager::ALLOW_GM_TRADE_BLOCK_OPTION))
		{
			return;
		}
	}
#endif

	//quest::CQuestManager::instance().SetEventFlag(arg1, atoi(arg2));
	quest::CQuestManager::instance().RequestSetEventFlag(arg1, value);
	ch->ChatPacket(CHAT_TYPE_INFO, "RequestSetEventFlag %s %d", arg1, value);
	sys_log(0, "RequestSetEventFlag %s %d", arg1, value);
}

ACMD(do_get_event_flag)
{
	quest::CQuestManager::instance().SendEventFlagList(ch);
}

ACMD(do_private)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: private <map index>");
		return;
	}

	long lMapIndex;
	long map_index = 0;
	str_to_number(map_index, arg1);
	if ((lMapIndex = SECTREE_MANAGER::instance().CreatePrivateMap(map_index)))
	{
		ch->SaveExitLocation();

		LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
		ch->WarpSet(pkSectreeMap->m_setting.posSpawn.x, pkSectreeMap->m_setting.posSpawn.y, lMapIndex);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Can't find map by index %d", map_index);
	}
}

ACMD(do_qf)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	std::string questname = pPC->GetCurrentQuestName();

	if (!questname.empty())
	{
		int value = quest::CQuestManager::Instance().GetQuestStateIndex(questname, arg1);

		pPC->SetFlag(questname + ".__status", value);
		pPC->ClearTimer();

		quest::PC::QuestInfoIterator it = pPC->quest_begin();
		unsigned int questindex = quest::CQuestManager::instance().GetQuestIndexByName(questname);

		while (it != pPC->quest_end())
		{
			if (it->first == questindex)
			{
				it->second.st = value;
				break;
			}

			++it;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "setting quest state flag %s %s %d", questname.c_str(), arg1, value);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "setting quest state flag failed");
	}
}

LPCHARACTER chHori, chForge, chLib, chTemple, chTraining, chTree, chPortal, chBall;

ACMD(do_b1)
{

	chHori = CHARACTER_MANAGER::instance().SpawnMobRange(14017, ch->GetMapIndex(), 304222, 742858, 304222, 742858, true, false);
	chHori->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_CONSTRUCTION_SMALL, 65535, 0, true);
	chHori->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	for (int i = 0; i < 30; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 800, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(number(701, 706),
						  ch->GetMapIndex(),
						  304222 + (int)fx,
						  742858 + (int)fy,
						  304222 + (int)fx,
						  742858 + (int)fy,
						  true,
						  false);
		tch->SetAggressive();
	}

	for (int i = 0; i < 5; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 800, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(8009,
						  ch->GetMapIndex(),
						  304222 + (int)fx,
						  742858 + (int)fy,
						  304222 + (int)fx,
						  742858 + (int)fy,
						  true,
						  false);
		tch->SetAggressive();
	}
}

ACMD(do_b2)
{
	chHori->RemoveAffect(AFFECT_DUNGEON_UNIQUE);
}

ACMD(do_b3)
{
	chForge = CHARACTER_MANAGER::instance().SpawnMobRange(14003, ch->GetMapIndex(), 307500, 746300, 307500, 746300, true, false);
	chForge->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14007, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chTemple = CHARACTER_MANAGER::instance().SpawnMobRange(14004, ch->GetMapIndex(), 307700, 741600, 307700, 741600, true, false);
	chTemple->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);

	chTraining = CHARACTER_MANAGER::instance().SpawnMobRange(14010, ch->GetMapIndex(), 307100, 739500, 307100, 739500, true, false);
	chTraining->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
	chTree = CHARACTER_MANAGER::instance().SpawnMobRange(14013, ch->GetMapIndex(), 300800, 741600, 300800, 741600, true, false);
	chTree->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
	chPortal = CHARACTER_MANAGER::instance().SpawnMobRange(14001, ch->GetMapIndex(), 300900, 744500, 300900, 744500, true, false);
	chPortal->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
	chBall = CHARACTER_MANAGER::instance().SpawnMobRange(14012, ch->GetMapIndex(), 302500, 746600, 302500, 746600, true, false);
	chBall->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_b4)
{
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_UPGRADE, 65535, 0, true);

	for (int i = 0; i < 30; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 1200, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(number(701, 706),
						  ch->GetMapIndex(),
						  307900 + (int)fx,
						  744500 + (int)fy,
						  307900 + (int)fx,
						  744500 + (int)fy,
						  true,
						  false);
		tch->SetAggressive();
	}

	for (int i = 0; i < 5; ++i)
	{
		int rot = number(0, 359);
		float fx, fy;
		GetDeltaByDegree(rot, 1200, &fx, &fy);

		LPCHARACTER tch = CHARACTER_MANAGER::instance().SpawnMobRange(8009,
						  ch->GetMapIndex(),
						  307900 + (int)fx,
						  744500 + (int)fy,
						  307900 + (int)fx,
						  744500 + (int)fy,
						  true,
						  false);
		tch->SetAggressive();
	}

}

ACMD(do_b5)
{
	M2_DESTROY_CHARACTER(chLib);
	//chHori->RemoveAffect(AFFECT_DUNGEON_UNIQUE);
	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14008, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_b6)
{
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_BUILDING_UPGRADE, 65535, 0, true);
}
ACMD(do_b7)
{
	M2_DESTROY_CHARACTER(chLib);
	//chHori->RemoveAffect(AFFECT_DUNGEON_UNIQUE);
	chLib = CHARACTER_MANAGER::instance().SpawnMobRange(14009, ch->GetMapIndex(), 307900, 744500, 307900, 744500, true, false);
	chLib->AddAffect(AFFECT_DUNGEON_UNIQUE, POINT_NONE, 0, AFF_DUNGEON_UNIQUE, 65535, 0, true);
}

ACMD(do_book)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	CSkillProto * pkProto;

	if (isnhdigit(*arg1))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		pkProto = CSkillManager::instance().Get(vnum);
	}
	else
	{
		pkProto = CSkillManager::instance().Get(arg1);
	}

	if (!pkProto)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no such a skill.");
		return;
	}

	LPITEM item = ch->AutoGiveItem(50300);
	item->SetSocket(0, pkProto->dwVnum);
}

ACMD(do_setskillother)
{
	char arg1[256], arg2[256], arg3[256];
	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(argument, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2 || !*arg3 || !isdigit(*arg3))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setskillother <target> <skillname> <lev>");
		return;
	}

	LPCHARACTER tch;

	tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no such character.");
		return;
	}

	CSkillProto * pk;

	if (isdigit(*arg2))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg2);
		pk = CSkillManager::instance().Get(vnum);
	}
	else
	{
		pk = CSkillManager::instance().Get(arg2);
	}

	if (!pk)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No such a skill by that name.");
		return;
	}

	BYTE level = 0;
	str_to_number(level, arg3);
	tch->SetSkillLevel(pk->dwVnum, level);
	tch->ComputePoints();
	tch->SkillLevelPacket();
}

ACMD(do_setskill)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2 || !isdigit(*arg2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setskill <name> <lev>");
		return;
	}

	CSkillProto * pk;

	if (isdigit(*arg1))
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		pk = CSkillManager::instance().Get(vnum);
	}

	else
	{
		pk = CSkillManager::instance().Get(arg1);
	}

	if (!pk)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No such a skill by that name.");
		return;
	}

	BYTE level = 0;
	str_to_number(level, arg2);
	ch->SetSkillLevel(pk->dwVnum, level);
	ch->ComputePoints();
	ch->SkillLevelPacket();
}

ACMD(do_set_skill_point)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int skill_point = 0;
	if (*arg1)
	{
		str_to_number(skill_point, arg1);
	}

	ch->SetRealPoint(POINT_SKILL, skill_point);
	ch->SetPoint(POINT_SKILL, ch->GetRealPoint(POINT_SKILL));
	ch->PointChange(POINT_SKILL, 0);
}

ACMD(do_set_skill_group)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int skill_group = 0;
	if (*arg1)
	{
		str_to_number(skill_group, arg1);
	}

	ch->SetSkillGroup(skill_group);

	ch->ClearSkill();
	ch->ChatPacket(CHAT_TYPE_INFO, "skill group to %d.", skill_group);
}

ACMD(do_reload)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	enum EReloadCMD
	{
		RELOAD_STATE_USER_COUNT,
		RELOAD_PROTOTYPE,
		RELOAD_NOTICE_STRING,
		RELOAD_QUEST,
		//RELOAD_FISHING,
		RELOAD_ADMIN_INFO,
		RELOAD_CUBE,
		RELOAD_SHOP_EX,
#ifdef __SHOP_SEARCH__
		RELOAD_SHOP_SEARCH_AVERAGE,
#endif
#ifdef __TRANSMUTE__
		RELOAD_TRANSMUTE,
#endif
		RELOAD_EQUIPMENT_SETS,
#ifdef __ENABLE_MISSION_MANAGER__
		RELOAD_MISSIONS_MANAGER,
#endif
#ifdef __ENABLE_TELEPORT_SYSTEM__
		RELOAD_TELEPORT_MANAGER,
#endif
#ifdef __ENABLE_ATTENDANCE_EVENT__
		RELOAD_ATTENDANCE_MANAGER,
#endif
#ifdef __ENABLE_BEGINNER_MANAGER__
		RELOAD_BEGINNER_MANAGER,
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
		RELOAD_BIOLOG_MANAGER,
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
		RELOAD_MARBLE_MANAGER,
#endif
#ifdef __ENABLE_MOB_TRAKCER__
		RELOAD_MOB_TRACKER,
#endif
		RELOAD_MAX_NUM,
	};

	const struct
	{
		const char* szKey;
		EReloadCMD reloadType;
	} cmdKeyList[RELOAD_MAX_NUM] =
	{
		{ "user_count", RELOAD_STATE_USER_COUNT },
		{ "proto", RELOAD_PROTOTYPE },
		{ "notice_string", RELOAD_NOTICE_STRING },
		{ "quest", RELOAD_QUEST },
		//{ "fishing", RELOAD_FISHING },
		{ "admin", RELOAD_ADMIN_INFO },
		{ "cube", RELOAD_CUBE },
		{ "shop_ex", RELOAD_SHOP_EX },
#ifdef __SHOP_SEARCH__
		{ "shop_search", RELOAD_SHOP_SEARCH_AVERAGE },
#endif
#ifdef __TRANSMUTE__
		{ "transmute", RELOAD_TRANSMUTE },
#endif
		{ "equipment_sets", RELOAD_EQUIPMENT_SETS },
#ifdef __ENABLE_MISSION_MANAGER__
		{ "mission_manager", RELOAD_MISSIONS_MANAGER },
#endif
#ifdef __ENABLE_TELEPORT_SYSTEM__
		{ "teleport_manager", RELOAD_TELEPORT_MANAGER },
#endif
#ifdef __ENABLE_ATTENDANCE_EVENT__
		{ "attendance_manager", RELOAD_ATTENDANCE_MANAGER },
#endif
#ifdef __ENABLE_BEGINNER_MANAGER__
		{ "beginner_manager", RELOAD_BEGINNER_MANAGER },
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
		{ "biolog_manager", RELOAD_BIOLOG_MANAGER },
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
		{ "marble_manager", RELOAD_MARBLE_MANAGER },
#endif
#ifdef __ENABLE_MOB_TRAKCER__
		{ "mob_trakcer", RELOAD_MOB_TRACKER },
#endif
	};

	const char* fullCmdName = NULL;
	EReloadCMD reloadCmd = RELOAD_MAX_NUM;
	for (int i = 0; i < RELOAD_MAX_NUM; ++i)
	{
		if (!strncmp(arg1, cmdKeyList[i].szKey, strlen(arg1)))
		{
			if (fullCmdName && strlen(fullCmdName) <= strlen(cmdKeyList[i].szKey))
			{
				continue;
			}

			fullCmdName = cmdKeyList[i].szKey;
			reloadCmd = cmdKeyList[i].reloadType;

			if (strlen(fullCmdName) == strlen(arg1))
			{
				break;
			}
		}
	}

	if (reloadCmd == RELOAD_MAX_NUM)
	{
		if (*arg1)
		{
			if (ch)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "invalid reload argument : %s", arg1);
			}
			return;
		}

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Reloading state_user_count.");
		}
		LoadStateUserCount();

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Reloading prototype tables,");
		}
		db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Reloading notice string.");
		}
		DBManager::instance().LoadDBString();
	}
	else
	{
		bool sendP2P = false;

		switch (reloadCmd)
		{
			case RELOAD_STATE_USER_COUNT:
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Reloading state_user_count.");
				}
				LoadStateUserCount();
				break;

			case RELOAD_PROTOTYPE:
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Reloading prototype tables,");
				}
				db_clientdesc->DBPacket(HEADER_GD_RELOAD_PROTO, 0, NULL, 0);
				break;

			case RELOAD_NOTICE_STRING:
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Reloading notice string.");
				}
				DBManager::instance().LoadDBString();
				break;

			case RELOAD_QUEST:
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Reloading quest.");
				}
				quest::CQuestManager::instance().Reload();
				break;

			case RELOAD_ADMIN_INFO:
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "Reloading Admin infomation.");
				}
				db_clientdesc->DBPacket(HEADER_GD_RELOAD_ADMIN, 0, NULL, 0);
				sys_log(0, "Reloading admin infomation.");
				break;

			case RELOAD_CUBE:
	#ifdef __ENABLE_CUBE_RENEWAL__
				CCubeRenewal::instance().Initialize(true);
	#else
				Cube_init();
	#endif
				sendP2P = true;
				break;

			case RELOAD_SHOP_EX:
			{
				bool ret = CShopManager::Instance().InitializeShopEx();
				if (ch)
				{
					if (!ret)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Failed to initialize shop ex table.");
					}
					else
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "Initialized shop ex table.");
					}
				}
				sendP2P = true;
			}
			break;

	#ifdef __SHOP_SEARCH__
			case RELOAD_SHOP_SEARCH_AVERAGE:
				CShopSearchManager::ReloadAverageData();
				break;
	#endif

			case RELOAD_EQUIPMENT_SETS: {
				EquipmentSetSettings::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading equipment set settings...");
			} break;

#ifdef __ENABLE_MISSION_MANAGER__
			case RELOAD_MISSIONS_MANAGER: {
				CMissionManager::Instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading missions manager...");
			} break;
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
			case RELOAD_TELEPORT_MANAGER: {
				BYTE bHeader = HEADER_GG_TELEPORT_RELOAD;
				P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				CTeleportManager::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading teleport manager...");
			} break;
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
			case RELOAD_ATTENDANCE_MANAGER: {
				BYTE bHeader = HEADER_GG_ATTENDANCE_RELOAD;
				P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				CAttendanceRewardManager::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading attendance manager...");
			} break;
#endif
#ifdef __ENABLE_BEGINNER_MANAGER__
			case RELOAD_BEGINNER_MANAGER: {
				BYTE bHeader = HEADER_GG_BEGINNER_RELOAD;
				P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				BeginnerHelper::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading beginner manager...");
			} break;
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
			case RELOAD_BIOLOG_MANAGER: {
				BYTE bHeader = HEADER_GG_BIOLOG_RELOAD;
				P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				CBiologSystemManager::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading biolog manager...");
			} break;
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
			case RELOAD_MARBLE_MANAGER: {
				BYTE bHeader = HEADER_GG_MARBLE_RELOAD;
				P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				CMarbleSystemManager::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading marble manager...");
			} break;
#endif
#ifdef __ENABLE_MOB_TRAKCER__
			case RELOAD_MOB_TRACKER: {
				//BYTE bHeader = HEADER_GG_MARBLE_RELOAD;
				//P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
				CMarbleSystemManager::instance().Create();
				ch->ChatPacket(CHAT_TYPE_INFO, "Reloading monster tracker...");
			} break;
#endif
			default:
				break;
		}

		if (sendP2P && !subcmd)
		{
			TPacketGGExecReloadCommand pack;
			pack.header = HEADER_GG_EXEC_RELOAD_COMMAND;
			strcpy(pack.arg1, arg1);

			P2P_MANAGER::instance().Send(&pack, sizeof(pack));
		}
	}
}

ACMD(do_cooltime)
{
	ch->DisableCooltime();
}

ACMD(do_level)
{
	char arg2[256];
	one_argument(argument, arg2, sizeof(arg2));

	if (!*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: level <level>");
		return;
	}

	int	level = 0;
	str_to_number(level, arg2);

	ch->ResetPoint(MINMAX(1, level, gPlayerMaxLevel));

	ch->ClearSkill();
	ch->ClearSubSkill();
}

ACMD(do_gwlist)
{
	ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("ÇöŔç ŔüŔďÁßŔÎ ±ćµĺ ŔÔ´Ď´Ů"));
	CGuildManager::instance().ShowGuildWarList(ch);
}

ACMD(do_stop_guild_war)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	int id1 = 0, id2 = 0;

	str_to_number(id1, arg1);
	str_to_number(id2, arg2);

	if (!id1 || !id2)
	{
		return;
	}

	if (id1 > id2)
	{
		std::swap(id1, id2);
	}

	ch->ChatPacket(CHAT_TYPE_TALKING, "%d %d", id1, id2);
	CGuildManager::instance().RequestEndWar(id1, id2);
}

ACMD(do_cancel_guild_war)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int id1 = 0, id2 = 0;
	str_to_number(id1, arg1);
	str_to_number(id2, arg2);

	if (id1 > id2)
	{
		std::swap(id1, id2);
	}

	CGuildManager::instance().RequestCancelWar(id1, id2);
}

ACMD(do_guild_state)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CGuild* pGuild = CGuildManager::instance().FindGuildByName(arg1);
	if (pGuild != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "GuildID: %d", pGuild->GetID());
		ch->ChatPacket(CHAT_TYPE_INFO, "GuildMasterPID: %d", pGuild->GetMasterPID());
		ch->ChatPacket(CHAT_TYPE_INFO, "IsInWar: %d", pGuild->UnderAnyWar());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s: Á¸ŔçÇĎÁö ľĘ´Â ±ćµĺ ŔÔ´Ď´Ů."), arg1);
	}
}

struct FuncWeaken
{
	LPCHARACTER m_pkGM;
	bool	m_bAll;

	FuncWeaken(LPCHARACTER ch) : m_pkGM(ch), m_bAll(false)
	{
	}

	void operator () (LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
		{
			return;
		}

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		int iDist = DISTANCE_APPROX(pkChr->GetX() - m_pkGM->GetX(), pkChr->GetY() - m_pkGM->GetY());

		if (!m_bAll && iDist >= 1000)
		{
			return;
		}

		if (pkChr->IsNPC())
		{
			pkChr->PointChange(POINT_HP, (10 - pkChr->GetHP()));
		}
	}
};

ACMD(do_weaken)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	FuncWeaken func(ch);

	if (*arg1 && !strcmp(arg1, "all"))
	{
		func.m_bAll = true;
	}

	ch->GetSectree()->ForEachAround(func);
}

ACMD(do_getqf)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	LPCHARACTER tch;

	if (!*arg1)
	{
		tch = ch;
	}
	else
	{
		tch = CHARACTER_MANAGER::instance().FindPC(arg1);

		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no such character.");
			return;
		}
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
	{
		pPC->SendFlagList(ch);
	}
}

#define ENABLE_SET_STATE_WITH_TARGET
ACMD(do_set_state)
{
	char arg1[256];
	char arg2[256];

	argument = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO,
					   "Syntax: set_state <questname> <statename>"
#ifdef ENABLE_SET_STATE_WITH_TARGET
					   " [<character name>]"
#endif
					  );
		return;
	}

#ifdef ENABLE_SET_STATE_WITH_TARGET
	LPCHARACTER tch = ch;
	char arg3[256];
	argument = one_argument(argument, arg3, sizeof(arg3));
	if (*arg3)
	{
		tch = CHARACTER_MANAGER::instance().FindPC(arg3);
		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "There is no such character.");
			return;
		}
	}

#ifdef __ADMIN_MANAGER__
	if (tch != ch && !GM::check_allow(ch->GetGMLevel(), GM_ALLOW_MODIFY_OTHERS))
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have no rights to modify other players.");
		}
		return;
	}
#endif

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(tch->GetPlayerID());
#else
	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
#endif
	std::string questname = arg1;
	std::string statename = arg2;

	if (!questname.empty())
	{
		int value = quest::CQuestManager::Instance().GetQuestStateIndex(questname, statename);

		pPC->SetFlag(questname + ".__status", value);
		pPC->ClearTimer();

		quest::PC::QuestInfoIterator it = pPC->quest_begin();
		unsigned int questindex = quest::CQuestManager::instance().GetQuestIndexByName(questname);

		while (it != pPC->quest_end())
		{
			if (it->first == questindex)
			{
				it->second.st = value;
				break;
			}

			++it;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "setting quest state flag %s %s %d", questname.c_str(), arg1, value);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "setting quest state flag failed");
	}
}

ACMD(do_setqf)
{
	char arg1[256];
	char arg2[256];
	char arg3[256];

	one_argument(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: setqf <flagname> <value> [<character name>]");
		return;
	}

	LPCHARACTER tch = ch;

	if (*arg3)
	{
		tch = CHARACTER_MANAGER::instance().FindPC(arg3);
	}

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no such character.");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
	{
		int value = 0;
		str_to_number(value, arg2);
		pPC->SetFlag(arg1, value);
		ch->ChatPacket(CHAT_TYPE_INFO, "Quest flag set: %s %d", arg1, value);
	}
}

ACMD(do_delqf)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: delqf <flagname> [<character name>]");
		return;
	}

	LPCHARACTER tch = ch;

	if (*arg2)
	{
		tch = CHARACTER_MANAGER::instance().FindPC(arg2);
	}

	if (!tch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no such character.");
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPC(tch->GetPlayerID());

	if (pPC)
	{
		if (pPC->DeleteFlag(arg1))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Delete success.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Delete failed. Quest flag does not exist.");
		}
	}
}

ACMD(do_forgetme)
{
	ch->ForgetMyAttacker();
}

ACMD(do_aggregate)
{
	ch->AggregateMonster();
}

ACMD(do_attract_ranger)
{
	ch->AttractRanger();
}

ACMD(do_pull_monster)
{
	ch->PullMonster();
}

ACMD(do_polymorph)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);
		bool bMaintainStat = false;
		if (*arg2)
		{
			int value = 0;
			str_to_number(value, arg2);
			bMaintainStat = (value > 0);
		}

		ch->SetPolymorph(dwVnum, bMaintainStat);
	}
}

ACMD(do_polymorph_item)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);

		LPITEM item = ITEM_MANAGER::instance().CreateItem(70104, 1, 0, true);
		if (item)
		{
			item->SetSocket(0, dwVnum);
#ifndef __SPECIAL_STORAGE_ENABLE__
			int iEmptyPos = ch->GetEmptyInventory(item->GetSize());
#else
			int iEmptyPos = ch->GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

			if (iEmptyPos != -1)
			{
				item->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				LogManager::instance().ItemLog(ch, item, "GM", item->GetName());
			}
			else
			{
				M2_DESTROY_ITEM(item);
				ch->ChatPacket(CHAT_TYPE_INFO, "Not enough inventory space.");
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "#%d item not exist by that vnum.", 70103);
		}
		//ch->SetPolymorph(dwVnum, bMaintainStat);
	}
}

ACMD(do_priv_empire)
{
	char arg1[256] = {0};
	char arg2[256] = {0};
	char arg3[256] = {0};
	char arg4[256] = {0};
	int empire = 0;
	int type = 0;
	int value = 0;
	int duration = 0;

	const char* line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		goto USAGE;
	}

	if (!line)
	{
		goto USAGE;
	}

	two_arguments(line, arg3, sizeof(arg3), arg4, sizeof(arg4));

	if (!*arg3 || !*arg4)
	{
		goto USAGE;
	}

	str_to_number(empire, arg1);
	str_to_number(type,	arg2);
	str_to_number(value,	arg3);
	value = MINMAX(0, value, 1000);
	str_to_number(duration, arg4);

	if (empire < 0 || 3 < empire)
	{
		goto USAGE;
	}

	if (type < 1 || 4 < type)
	{
		goto USAGE;
	}

	if (value < 0)
	{
		goto USAGE;
	}

	if (duration < 0)
	{
		goto USAGE;
	}


	duration = duration * (60 * 60);

	sys_log(0, "_give_empire_privileage(empire=%d, type=%d, value=%d, duration=%d) by command",
			empire, type, value, duration);
	CPrivManager::instance().RequestGiveEmpirePriv(empire, type, value, duration);
	return;

USAGE:
	ch->ChatPacket(CHAT_TYPE_INFO, "usage : priv_empire <empire> <type> <value> <duration>");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <empire>    0 - 3 (0==all)");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <type>      1:item_drop, 2:gold_drop, 3:gold10_drop, 4:exp");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <value>     percent");
	ch->ChatPacket(CHAT_TYPE_INFO, "  <duration>  hour");
}


ACMD(do_priv_guild)
{
	static const char msg[] = { '\0' };

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		CGuild * g = CGuildManager::instance().FindGuildByName(arg1);

		if (!g)
		{
			DWORD guild_id = 0;
			str_to_number(guild_id, arg1);
			g = CGuildManager::instance().FindGuild(guild_id);
		}

		if (!g)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A Guild with this name or number does not exist."));
		}
		else
		{
			char buf[1024 + 1];
			snprintf(buf, sizeof(buf), msg, g->GetID());

			using namespace quest;
			PC * pc = CQuestManager::instance().GetPC(ch->GetPlayerID());
			QuestState qs = CQuestManager::instance().OpenState("ADMIN_QUEST", QUEST_FISH_REFINE_STATE_INDEX);
			luaL_loadbuffer(qs.co, buf, strlen(buf), "ADMIN_QUEST");
			pc->SetQuest("ADMIN_QUEST", qs);

			QuestState & rqs = *pc->GetRunningQuestState();

			if (!CQuestManager::instance().RunState(rqs))
			{
				CQuestManager::instance().CloseState(rqs);
				pc->EndRunning();
				return;
			}
		}
	}
}

ACMD(do_mount_test)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ignore polymorph block...");
	}
	if (*arg1)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		ch->MountVnum(vnum);
	}
}

ACMD(do_observer)
{
	ch->SetObserverMode(!ch->IsObserverMode());
}

ACMD(do_socket_item)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1)
	{
		DWORD dwVnum = 0;
		str_to_number(dwVnum, arg1);

		int iSocketCount = 0;
		str_to_number(iSocketCount, arg2);

		if (!iSocketCount || iSocketCount >= ITEM_SOCKET_MAX_NUM)
		{
			iSocketCount = 3;
		}

		if (!dwVnum)
		{
			if (!ITEM_MANAGER::instance().GetVnum(arg1, dwVnum))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "#%d item not exist by that vnum.", dwVnum);
				return;
			}
		}

		LPITEM item = ch->AutoGiveItem(dwVnum);

		if (item)
		{
			for (int i = 0; i < iSocketCount; ++i)
			{
				item->SetSocket(i, 1);
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "#%d cannot create item.", dwVnum);
		}
	}
}

ACMD(do_xmas)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	int flag = 0;

	if (*arg1)
	{
		str_to_number(flag, arg1);
	}

	switch (subcmd)
	{
	case SCMD_XMAS_SNOW:
		quest::CQuestManager::instance().RequestSetEventFlag("xmas_snow", flag);
		break;

	case SCMD_XMAS_BOOM:
		quest::CQuestManager::instance().RequestSetEventFlag("xmas_boom", flag);
		break;

	case SCMD_XMAS_SANTA:
		quest::CQuestManager::instance().RequestSetEventFlag("xmas_santa", flag);
		break;
	}
}


// BLOCK_CHAT
ACMD(do_block_chat_list)
{

	if (!ch || (ch->GetGMLevel() < GM_HIGH_WIZARD && ch->GetQuestFlag("chat_privilege.block") <= 0))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Order does not exist."));
		return;
	}

#ifdef __ADMIN_MANAGER__
	if (!CAdminManager::Instance().HasAllow(ch, CAdminManager::ALLOW_VIEW_BAN_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You are not allowed to view blocked chats.");
		return;
	}
#endif

	DBManager::instance().ReturnQuery(QID_BLOCK_CHAT_LIST, ch->GetPlayerID(), NULL,
									  "SELECT p.name, a.lDuration FROM affect%s as a, player%s as p WHERE a.bType = %d AND a.dwPID = p.id",
									  get_table_postfix(), get_table_postfix(), AFFECT_BLOCK_CHAT);
}

ACMD(do_vote_block_chat)
{
	return;

	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: vote_block_chat <name>");
		return;
	}

	const char* name = arg1;
	long lBlockDuration = 10;
	sys_log(0, "vote_block_chat %s %d", name, lBlockDuration);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(name);

		if (pkCCI)
		{
			TPacketGGBlockChat p;

			p.bHeader = HEADER_GG_BLOCK_CHAT;
			strlcpy(p.szName, name, sizeof(p.szName));
			p.lBlockDuration = lBlockDuration;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGBlockChat));
		}
		else
		{
			TPacketBlockChat p;

			strlcpy(p.szName, name, sizeof(p.szName));
			p.lDuration = lBlockDuration;
			db_clientdesc->DBPacket(HEADER_GD_BLOCK_CHAT, ch ? ch->GetDesc()->GetHandle() : 0, &p, sizeof(p));

		}

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block requested.");
		}

		return;
	}

	if (tch && ch != tch)
	{
		tch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
	}
}

#ifdef __ADMIN_MANAGER__
ACMD(do_block_chat)
{
#ifdef __ADMIN_MANAGER__
	if (ch && !CAdminManager::Instance().HasAllow(ch, CAdminManager::ALLOW_BAN_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You are not allowed to block chats.");
		return;
	}
#endif

	char arg1[256], arg2[256], arg3[256], arg4[256], arg5[256];
	argument = one_argument(two_arguments(two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3), arg4, sizeof(arg4)),
							arg5, sizeof(arg5));

	if (!*arg1 || !*arg2 || !*arg3 || !*arg4 || !*arg5)
	{
		return;
	}

	const char* name = arg1;
	long lBlockDuration = parse_time_str(arg2);
	bool bIncreaseBanCounter = *arg3 == '1';

	if (lBlockDuration < 0)
	{
		return;
	}

	sys_log(0, "BLOCK CHAT %s %d", name, lBlockDuration);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

	if (!tch)
	{
		CCI* pkCCI = P2P_MANAGER::instance().Find(name);

		if (pkCCI)
		{
			TPacketGGBlockChat p;

			p.bHeader = HEADER_GG_BLOCK_CHAT;
			strlcpy(p.szName, name, sizeof(p.szName));
			if (ch)
			{
				p.dwGMPid = ch->GetPlayerID();
				strlcpy(p.szGMName, ch->GetName(), sizeof(p.szGMName));
			}
			else
			{
				p.dwGMPid = 0;
				*p.szGMName = '\0';
			}
			p.lBlockDuration = lBlockDuration;
			strlcpy(p.szDesc, arg4, sizeof(p.szDesc));
			strlcpy(p.szProof, arg5, sizeof(p.szProof));
			p.bIncreaseBanCounter = bIncreaseBanCounter;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGBlockChat));
		}
		else
		{
			int nameLen = strlen(name);
			if (nameLen > CHARACTER_NAME_MAX_LEN)
			{
				nameLen = CHARACTER_NAME_MAX_LEN;
			}

			char szEscapedName[CHARACTER_NAME_MAX_LEN * 2 + 1];
			DBManager::instance().EscapeString(szEscapedName, sizeof(szEscapedName), name, nameLen);
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT id, account_id FROM player WHERE name = '%s'", szEscapedName));
			if (pMsg->Get()->uiNumRows == 0)
			{
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "There is no character by name %s.", name);
				}
				return;
			}

			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			DWORD dwPID = 0;
			str_to_number(dwPID, row[0]);
			DWORD dwAID = 0;
			str_to_number(dwAID, row[1]);

			TPacketBlockChat p;

			p.dwPID = dwPID;
			strlcpy(p.szName, name, sizeof(p.szName));
			p.lDuration = lBlockDuration;
			p.bIncreaseBanCounter = bIncreaseBanCounter;
			db_clientdesc->DBPacket(HEADER_GD_BLOCK_CHAT, ch ? ch->GetDesc()->GetHandle() : 0, &p, sizeof(p));

			int iOldDuration = 0;
			{
				std::unique_ptr<SQLMsg> pMsg2(DBManager::instance().DirectQuery("SELECT lDuration FROM affect WHERE dwPID = %u AND bType = %u",
											  dwPID, AFFECT_BLOCK_CHAT));
				if (pMsg2->Get()->uiNumRows > 0)
				{
					str_to_number(iOldDuration, mysql_fetch_row(pMsg2->Get()->pSQLResult)[0]);
				}
			}
			
			LogManager::instance().ChatBanLog(dwPID, dwAID, name, ch, lBlockDuration - iOldDuration, lBlockDuration, arg4, arg5, bIncreaseBanCounter);
		}

#ifdef __ADMIN_MANAGER__
		CAdminManager::instance().OnPlayerChatban(name, lBlockDuration, bIncreaseBanCounter);
#endif

		if (ch && lBlockDuration)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block requested.");
		}
		else if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block remove requested.");
		}

		return;
	}

	if (tch && (ch != tch || lBlockDuration == 0))
	{
		int iOldDuration = 0;
		if (CAffect* pAff = tch->FindAffect(AFFECT_BLOCK_CHAT))
		{
			iOldDuration = pAff->lDuration;
		}
		LogManager::instance().ChatBanLog(tch, ch, lBlockDuration - iOldDuration, lBlockDuration, arg4, arg5, bIncreaseBanCounter);

		tch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
		if (bIncreaseBanCounter)
		{
			tch->SetChatBanCount(tch->GetChatBanCount() + 1);
		}

#ifdef __ADMIN_MANAGER__
		CAdminManager::instance().OnPlayerChatban(tch, lBlockDuration);
#endif

		if (ch && lBlockDuration)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block set.");
		}
		else if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block removed.");
		}
	}
}
// END_OF_BLOCK_CHAT
#else
ACMD(do_block_chat)
{

	if (ch && (ch->GetGMLevel() < GM_HIGH_WIZARD && ch->GetQuestFlag("chat_privilege.block") <= 0))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Order does not exist."));
		return;
	}

	char arg1[256];
	argument = one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Usage: block_chat <name> <time> (0 to off)");
		}

		return;
	}

	const char* name = arg1;
	long lBlockDuration = parse_time_str(argument);

	if (lBlockDuration < 0)
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Ŕß¸řµČ Çü˝ÄŔÇ ˝Ă°ŁŔÔ´Ď´Ů. h, m, s¸¦ şŮż©Ľ­ ÁöÁ¤ÇŘ ÁÖ˝Ę˝ĂżŔ.");
			ch->ChatPacket(CHAT_TYPE_INFO, "żą) 10s, 10m, 1m 30s");
		}
		return;
	}

	sys_log(0, "BLOCK CHAT %s %d", name, lBlockDuration);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(name);

		if (pkCCI)
		{
			TPacketGGBlockChat p;

			p.bHeader = HEADER_GG_BLOCK_CHAT;
			strlcpy(p.szName, name, sizeof(p.szName));
			p.lBlockDuration = lBlockDuration;
			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGBlockChat));
		}
		else
		{
			TPacketBlockChat p;

			strlcpy(p.szName, name, sizeof(p.szName));
			p.lDuration = lBlockDuration;
			db_clientdesc->DBPacket(HEADER_GD_BLOCK_CHAT, ch ? ch->GetDesc()->GetHandle() : 0, &p, sizeof(p));
		}

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Chat block requested.");
		}

		return;
	}

	if (tch && ch != tch)
	{
		tch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, lBlockDuration, 0, true);
	}
}
// END_OF_BLOCK_CHAT
#endif

// BUILD_BUILDING
ACMD(do_build)
{
	using namespace building;

	char arg1[256], arg2[256], arg3[256], arg4[256];
	const char * line = one_argument(argument, arg1, sizeof(arg1));
	BYTE GMLevel = ch->GetGMLevel();

	CLand * pkLand = CManager::instance().FindLand(ch->GetMapIndex(), ch->GetX(), ch->GetY());



	if (!pkLand)
	{
		sys_err("%s trying to build on not buildable area.", ch->GetName());
		return;
	}

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax: no command");
		return;
	}


	if (GMLevel == GM_PLAYER)
	{

		if ((!ch->GetGuild() || ch->GetGuild()->GetID() != pkLand->GetOwner()))
		{
			sys_err("%s trying to build on not owned land.", ch->GetName());
			return;
		}


		if (ch->GetGuild()->GetMasterPID() != ch->GetPlayerID())
		{
			sys_err("%s trying to build while not the guild master.", ch->GetName());
			return;
		}
	}

	switch (LOWER(*arg1))
	{
	case 'c':
	{
		// /build c vnum x y x_rot y_rot z_rot
		char arg5[256], arg6[256];
		line = one_argument(two_arguments(line, arg1, sizeof(arg1), arg2, sizeof(arg2)), arg3, sizeof(arg3)); // vnum x y
		one_argument(two_arguments(line, arg4, sizeof(arg4), arg5, sizeof(arg5)), arg6, sizeof(arg6)); // x_rot y_rot z_rot

		if (!*arg1 || !*arg2 || !*arg3 || !*arg4 || !*arg5 || !*arg6)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax");
			return;
		}

		DWORD dwVnum = 0;
		str_to_number(dwVnum,  arg1);

		using namespace building;

		const TObjectProto * t = CManager::instance().GetObjectProto(dwVnum);
		if (!t)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Building does not exist."));
			return;
		}

#ifdef __EXTANDED_GOLD_AMOUNT__
		const int64_t BUILDING_MAX_PRICE = 100000000LL;
#else
		const DWORD BUILDING_MAX_PRICE = 100000000;
#endif

		if (t->dwGroupVnum)
		{
			if (pkLand->FindObjectByGroup(t->dwGroupVnum))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Type of Building can be erected only once."));
				return;
			}
		}

		if (t->dwDependOnGroupVnum)
		{
			{

				if (!pkLand->FindObjectByGroup(t->dwDependOnGroupVnum))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Base Building has to be made first."));
					return;
				}
			}
		}

		if (test_server || GMLevel == GM_PLAYER)
		{
			if (t->dwPrice > BUILDING_MAX_PRICE)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Building failed because of an incorrect pricing."));
				return;
			}

			if (ch->GetGold() < t->dwPrice)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough Gold to erect this Building."));
				return;
			}

			int i;
			for (i = 0; i < OBJECT_MATERIAL_MAX_NUM; ++i)
			{
				DWORD dwItemVnum = t->kMaterials[i].dwItemVnum;
				DWORD dwItemCount = t->kMaterials[i].dwCount;

				if (dwItemVnum == 0)
				{
					break;
				}

				if ((int) dwItemCount > ch->CountSpecifyItem(dwItemVnum))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough resources for this Building."));
					return;
				}
			}
		}

		float x_rot = atof(arg4);
		float y_rot = atof(arg5);
		float z_rot = atof(arg6);

		long map_x = 0;
		str_to_number(map_x, arg2);
		long map_y = 0;
		str_to_number(map_y, arg3);

		bool isSuccess = pkLand->RequestCreateObject(dwVnum,
						 ch->GetMapIndex(),
						 map_x,
						 map_y,
						 x_rot,
						 y_rot,
						 z_rot, true);

		if (!isSuccess)
		{
			if (test_server)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot erect a Building at this place."));
			}
			return;
		}

		if (test_server || GMLevel == GM_PLAYER)
		{
			ch->PointChange(POINT_GOLD, -t->dwPrice);
			{
				int i;
				for (i = 0; i < OBJECT_MATERIAL_MAX_NUM; ++i)
				{
					DWORD dwItemVnum = t->kMaterials[i].dwItemVnum;
					DWORD dwItemCount = t->kMaterials[i].dwCount;

					if (dwItemVnum == 0)
					{
						break;
					}

					sys_log(0, "BUILD: material %d %u %u", i, dwItemVnum, dwItemCount);
					ch->RemoveSpecifyItem(dwItemVnum, dwItemCount);
				}
			}
		}
	}
	break;

	case 'd' :
		// build (d)elete ObjectID
	{
		one_argument(line, arg1, sizeof(arg1));

		if (!*arg1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Invalid syntax");
			return;
		}

		DWORD vid = 0;
		str_to_number(vid, arg1);
		pkLand->RequestDeleteObjectByVID(vid);
	}
	break;

	// BUILD_WALL

	// build w n/e/w/s
	case 'w' :
		if (GMLevel > GM_PLAYER)
		{
			int mapIndex = ch->GetMapIndex();

			one_argument(line, arg1, sizeof(arg1));

			sys_log(0, "guild.wall.build map[%d] direction[%s]", mapIndex, arg1);

			switch (arg1[0])
			{
			case 's':
				pkLand->RequestCreateWall(mapIndex,   0.0f);
				break;
			case 'n':
				pkLand->RequestCreateWall(mapIndex, 180.0f);
				break;
			case 'e':
				pkLand->RequestCreateWall(mapIndex,  90.0f);
				break;
			case 'w':
				pkLand->RequestCreateWall(mapIndex, 270.0f);
				break;
			default:
				ch->ChatPacket(CHAT_TYPE_INFO, "guild.wall.build unknown_direction[%s]", arg1);
				sys_err("guild.wall.build unknown_direction[%s]", arg1);
				break;
			}

		}
		break;

	case 'e':
		if (GMLevel > GM_PLAYER)
		{
			pkLand->RequestDeleteWall();
		}
		break;

	case 'W' :



		if (GMLevel >  GM_PLAYER)
		{
			int setID = 0, wallSize = 0;
			char arg5[256], arg6[256];
			line = two_arguments(line, arg1, sizeof(arg1), arg2, sizeof(arg2));
			line = two_arguments(line, arg3, sizeof(arg3), arg4, sizeof(arg4));
			two_arguments(line, arg5, sizeof(arg5), arg6, sizeof(arg6));

			str_to_number(setID, arg1);
			str_to_number(wallSize, arg2);

			if (setID != 14105 && setID != 14115 && setID != 14125)
			{
				sys_log(0, "BUILD_WALL: wrong wall set id %d", setID);
				break;
			}
			else
			{
				bool door_east = false;
				str_to_number(door_east, arg3);
				bool door_west = false;
				str_to_number(door_west, arg4);
				bool door_south = false;
				str_to_number(door_south, arg5);
				bool door_north = false;
				str_to_number(door_north, arg6);
				pkLand->RequestCreateWallBlocks(setID, ch->GetMapIndex(), wallSize, door_east, door_west, door_south, door_north);
			}
		}
		break;

	case 'E' :

		if (GMLevel > GM_PLAYER)
		{
			one_argument(line, arg1, sizeof(arg1));
			DWORD id = 0;
			str_to_number(id, arg1);
			pkLand->RequestDeleteWallBlocks(id);
		}
		break;

	default:
		ch->ChatPacket(CHAT_TYPE_INFO, "Invalid command %s", arg1);
		break;
	}
}
// END_OF_BUILD_BUILDING

ACMD(do_clear_quest)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	quest::PC* pPC = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	pPC->ClearQuest(arg1);
}

ACMD(do_horse_state)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "Horse Information:");
	ch->ChatPacket(CHAT_TYPE_INFO, "    Level  %d", ch->GetHorseLevel());
	ch->ChatPacket(CHAT_TYPE_INFO, "    Health %d/%d (%d%%)", ch->GetHorseHealth(), ch->GetHorseMaxHealth(), ch->GetHorseHealth() * 100 / ch->GetHorseMaxHealth());
	ch->ChatPacket(CHAT_TYPE_INFO, "    Stam   %d/%d (%d%%)", ch->GetHorseStamina(), ch->GetHorseMaxStamina(), ch->GetHorseStamina() * 100 / ch->GetHorseMaxStamina());
}

ACMD(do_horse_level)
{
	char arg1[256] = {0};
	char arg2[256] = {0};
	LPCHARACTER victim;
	int	level = 0;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage : /horse_level <name> <level>");
		return;
	}

	victim = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (NULL == victim)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This character does not exist."));
		return;
	}

	str_to_number(level, arg2);
	level = MINMAX(0, level, HORSE_MAX_LEVEL);

	ch->ChatPacket(CHAT_TYPE_INFO, "horse level set (%s: %d)", victim->GetName(), level);

	victim->SetHorseLevel(level);
	victim->ComputePoints();
	victim->SkillLevelPacket();
	return;
}

ACMD(do_horse_ride)
{
	if (ch->IsHorseRiding())
	{
		ch->StopRiding();
	}
	else
	{
		ch->StartRiding();
	}
}

ACMD(do_horse_summon)
{
	ch->HorseSummon(true, true);
}

ACMD(do_horse_unsummon)
{
	ch->HorseSummon(false, true);
}

ACMD(do_horse_set_stat)
{
	char arg1[256], arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		int hp = 0;
		str_to_number(hp, arg1);
		int stam = 0;
		str_to_number(stam, arg2);
		ch->UpdateHorseHealth(hp - ch->GetHorseHealth());
		ch->UpdateHorseStamina(stam - ch->GetHorseStamina());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage : /horse_set_stat <hp> <stamina>");
	}
}

ACMD(do_save_attribute_to_image) // command "/saveati" for alias
{
	char szFileName[256];
	char szMapIndex[256];

	two_arguments(argument, szMapIndex, sizeof(szMapIndex), szFileName, sizeof(szFileName));

	if (!*szMapIndex || !*szFileName)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /saveati <map_index> <filename>");
		return;
	}

	long lMapIndex = 0;
	str_to_number(lMapIndex, szMapIndex);

	if (SECTREE_MANAGER::instance().SaveAttributeToImage(lMapIndex, szFileName))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Save done.");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Save failed.");
	}
}

ACMD(do_affect_remove)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /affect_remove <player name>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: /affect_remove <type> <point>");

		LPCHARACTER tch = ch;

		if (*arg1)
			if (!(tch = CHARACTER_MANAGER::instance().FindPC(arg1)))
			{
				tch = ch;
			}

		ch->ChatPacket(CHAT_TYPE_INFO, "-- Affect List of %s -------------------------------", tch->GetName());
		ch->ChatPacket(CHAT_TYPE_INFO, "Type Point Modif Duration Flag");

		const std::list<CAffect *> & cont = tch->GetAffectContainer();

		itertype(cont) it = cont.begin();

		while (it != cont.end())
		{
			CAffect * pkAff = *it++;

			ch->ChatPacket(CHAT_TYPE_INFO, "%4d %5d %5d %8d %u",
						   pkAff->dwType, pkAff->bApplyOn, pkAff->lApplyValue, pkAff->lDuration, pkAff->dwFlag);
		}
		return;
	}

	bool removed = false;

	CAffect * af;

	DWORD	type = 0;
	str_to_number(type, arg1);
	BYTE	point = 0;
	str_to_number(point, arg2);
	while ((af = ch->FindAffect(type, point)))
	{
		ch->RemoveAffect(af);
		removed = true;
	}

	if (removed)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Affect successfully removed.");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Not affected by that type and point.");
	}
}

ACMD(do_change_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
	{
		weapon->ChangeAttribute();
	}
}

ACMD(do_add_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
	{
		weapon->AddAttribute();
	}
}

ACMD(do_add_socket)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
	{
		weapon->AddSocket();
	}
}

#ifdef __ENABLE_NEWSTUFF__
ACMD(do_change_rare_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
	{
		weapon->ChangeRareAttribute();
	}
}

ACMD(do_add_rare_attr)
{
	LPITEM weapon = ch->GetWear(WEAR_WEAPON);
	if (weapon)
	{
		weapon->AddRareAttribute();
	}
}
#endif

ACMD(do_show_arena_list)
{
	CArenaManager::instance().SendArenaMapListTo(ch);
}

ACMD(do_end_all_duel)
{
	CArenaManager::instance().EndAllDuel();
}

ACMD(do_end_duel)
{
	char szName[256];

	one_argument(argument, szName, sizeof(szName));

	LPCHARACTER pChar = CHARACTER_MANAGER::instance().FindPC(szName);
	if (pChar == NULL)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This character does not exist."));
		return;
	}

	if (CArenaManager::instance().EndDuel(pChar->GetPlayerID()) == false)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Duel not cancelled."));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Duel cancelled successfully."));
	}
}

ACMD(do_duel)
{
	char szName1[256];
	char szName2[256];
	char szSet[256];
	char szMinute[256];
	int set = 0;
	int minute = 0;

	argument = two_arguments(argument, szName1, sizeof(szName1), szName2, sizeof(szName2));
	two_arguments(argument, szSet, sizeof(szSet), szMinute, sizeof(szMinute));

	str_to_number(set, szSet);

	if (set < 0) { set = 1; }
	if (set > 5) { set = 5; }

	if (!str_to_number(minute, szMinute))
	{
		minute = 5;
	}

	if (minute < 5)
	{
		minute = 5;
	}

	LPCHARACTER pChar1 = CHARACTER_MANAGER::instance().FindPC(szName1);
	LPCHARACTER pChar2 = CHARACTER_MANAGER::instance().FindPC(szName2);

	if (pChar1 != NULL && pChar2 != NULL)
	{
		pChar1->RemoveGoodAffect();
		pChar2->RemoveGoodAffect();

		pChar1->RemoveBadAffect();
		pChar2->RemoveBadAffect();

		LPPARTY pParty = pChar1->GetParty();
		if (pParty != NULL)
		{
			if (pParty->GetMemberCount() == 2)
			{
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				pChar1->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You left the Group."));
				pParty->Quit(pChar1->GetPlayerID());
			}
		}

		pParty = pChar2->GetParty();
		if (pParty != NULL)
		{
			if (pParty->GetMemberCount() == 2)
			{
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				pChar2->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<Group> You left the Group."));
				pParty->Quit(pChar2->GetPlayerID());
			}
		}

		if (CArenaManager::instance().StartDuel(pChar1, pChar2, set, minute) == true)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The start of the duel was successful."));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Is a problem with the start of the duel."));
		}
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No Combatants there."));
	}
}

#define ENABLE_STATPLUS_NOLIMIT
ACMD(do_stat_plus_amount)
{
	char szPoint[256];

	one_argument(argument, szPoint, sizeof(szPoint));

	if (*szPoint == '\0')
	{
		return;
	}

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
		return;
	}

	int nRemainPoint = ch->GetPoint(POINT_STAT);

	if (nRemainPoint <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No State Points left."));
		return;
	}

	int nPoint = 0;
	str_to_number(nPoint, szPoint);

	if (nRemainPoint < nPoint)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Left State Points are too low."));
		return;
	}

	if (nPoint < 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You entered a wrong value."));
		return;
	}

#ifndef ENABLE_STATPLUS_NOLIMIT
	switch (subcmd)
	{
	case POINT_HT :
		if (nPoint + ch->GetPoint(POINT_HT) > 90)
		{
			nPoint = 90 - ch->GetPoint(POINT_HT);
		}
		break;

	case POINT_IQ :
		if (nPoint + ch->GetPoint(POINT_IQ) > 90)
		{
			nPoint = 90 - ch->GetPoint(POINT_IQ);
		}
		break;

	case POINT_ST :
		if (nPoint + ch->GetPoint(POINT_ST) > 90)
		{
			nPoint = 90 - ch->GetPoint(POINT_ST);
		}
		break;

	case POINT_DX :
		if (nPoint + ch->GetPoint(POINT_DX) > 90)
		{
			nPoint = 90 - ch->GetPoint(POINT_DX);
		}
		break;

	default :
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Suborder or the Order is incorrect."));
		return;
		break;
	}
#endif

	if (nPoint != 0)
	{
		ch->SetRealPoint(subcmd, ch->GetRealPoint(subcmd) + nPoint);
		ch->SetPoint(subcmd, ch->GetPoint(subcmd) + nPoint);
		ch->ComputePoints();
		ch->PointChange(subcmd, 0);

		ch->PointChange(POINT_STAT, -nPoint);
		ch->ComputePoints();
	}
}

struct tTwoPID
{
	int pid1;
	int pid2;
};

ACMD(do_break_marriage)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	tTwoPID pids = { 0, 0 };

	str_to_number(pids.pid1, arg1);
	str_to_number(pids.pid2, arg2);

	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Broken Contract between player %d and player %d."), pids.pid1, pids.pid2);
	db_clientdesc->DBPacket(HEADER_GD_BREAK_MARRIAGE, 0, &pids, sizeof(pids));
}

ACMD(do_effect)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	int	effect_type = 0;
	str_to_number(effect_type, arg1);
	ch->EffectPacket(effect_type);
}


struct FCountInMap
{
	int m_Count[4];
	FCountInMap() { memset(m_Count, 0, sizeof(int) * 4); }
	void operator()(LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER) ent;
			if (ch && ch->IsPC())
			{
				++m_Count[ch->GetEmpire()];
			}
		}
	}
	int GetCount(BYTE bEmpire) { return m_Count[bEmpire]; }
};

ACMD(do_threeway_war_info)
{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Information for the Kingdoms"));
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Choose the Map Information of the Holy Land %d Entrance %d %d %d"), GetSungziMapIndex(), GetPassMapIndex(1), GetPassMapIndex(2), GetPassMapIndex(3));
	ch->ChatPacket(CHAT_TYPE_INFO, "ThreewayPhase %d", CThreeWayWar::instance().GetRegenFlag());

	for (int n = 1; n < 4; ++n)
	{
		LPSECTREE_MAP pSecMap = SECTREE_MANAGER::instance().GetMap(GetSungziMapIndex());

		FCountInMap c;

		if (pSecMap)
		{
			pSecMap->for_each(c);
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "%s killscore %d usercount %d",
					   EMPIRE_NAME(n),
					   CThreeWayWar::instance().GetKillScore(n),
					   c.GetCount(n));
	}
}

ACMD(do_threeway_war_myinfo)
{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Information about the Kingdom Battle"));
	ch->ChatPacket(CHAT_TYPE_INFO, "Deadcount %d",
				   CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()));
}

ACMD(do_reset_subskill)
{
	char arg1[256];

	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: reset_subskill <name>");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);

	if (tch == NULL)
	{
		return;
	}

	tch->ClearSubSkill();
	ch->ChatPacket(CHAT_TYPE_INFO, "Subskill of [%s] was reset", tch->GetName());
}

ACMD(do_siege)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	int	empire = strtol(arg1, NULL, 10);
	int tower_count = strtol(arg2, NULL, 10);

	if (empire == 0) { empire = number(1, 3); }
	if (tower_count < 5 || tower_count > 10) { tower_count = number(5, 10); }

	TPacketGGSiege packet;
	packet.bHeader = HEADER_GG_SIEGE;
	packet.bEmpire = empire;
	packet.bTowerCount = tower_count;

	P2P_MANAGER::instance().Send(&packet, sizeof(TPacketGGSiege));

	switch (castle_siege(empire, tower_count))
	{
	case 0 :
		ch->ChatPacket(CHAT_TYPE_INFO, "SIEGE FAILED");
		break;
	case 1 :
		ch->ChatPacket(CHAT_TYPE_INFO, "SIEGE START Empire(%d) Tower(%d)", empire, tower_count);
		break;
	case 2 :
		ch->ChatPacket(CHAT_TYPE_INFO, "SIEGE END");
		break;
	}
}

ACMD(do_frog)
{
	char	arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: empire(1, 2, 3)");
		return;
	}

	int	empire = 0;
	str_to_number(empire, arg1);

	switch (empire)
	{
	case 1:
	case 2:
	case 3:
		if (IS_CASTLE_MAP(ch->GetMapIndex()))
		{
			castle_spawn_frog(empire);
			castle_save();
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You must spawn frog in castle");
		}
		break;

	default:
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: empire(1, 2, 3)");
		break;
	}

}

ACMD(do_flush)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (0 == arg1[0])
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "usage : /flush player_id");
		return;
	}

	DWORD pid = (DWORD) strtoul(arg1, NULL, 10);

	db_clientdesc->DBPacketHeader(HEADER_GD_FLUSH_CACHE, 0, sizeof(DWORD));
	db_clientdesc->Packet(&pid, sizeof(DWORD));
}

ACMD(do_eclipse)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (strtol(arg1, NULL, 10) == 1)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("eclipse", 1);
	}
	else
	{
		quest::CQuestManager::instance().RequestSetEventFlag("eclipse", 0);
	}
}

ACMD(do_weeklyevent)
{
	char arg1[256];
	int empire = 0;

	if (CBattleArena::instance().IsRunning() == false)
	{
		one_argument(argument, arg1, sizeof(arg1));

		empire = strtol(arg1, NULL, 10);

		if (empire == 1 || empire == 2 || empire == 3)
		{
			CBattleArena::instance().Start(empire);
		}
		else
		{
			CBattleArena::instance().Start(rand() % 3 + 1);
		}
		ch->ChatPacket(CHAT_TYPE_INFO, "Weekly Event Start");
	}
	else
	{
		CBattleArena::instance().ForceEnd();
		ch->ChatPacket(CHAT_TYPE_INFO, "Weekly Event End");
	}
}

ACMD(do_event_helper)
{
	char arg1[256];
	int mode = 0;

	one_argument(argument, arg1, sizeof(arg1));
	str_to_number(mode, arg1);

	if (mode == 1)
	{
		xmas::SpawnEventHelper(true);
		ch->ChatPacket(CHAT_TYPE_INFO, "Event Helper Spawn");
	}
	else
	{
		xmas::SpawnEventHelper(false);
		ch->ChatPacket(CHAT_TYPE_INFO, "Event Helper Delete");
	}
}

struct FMobCounter
{
	int nCount;

	void operator () (LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);

			if (pChar->IsMonster() == true || pChar->IsStone())
			{
				nCount++;
			}
		}
	}
};

ACMD(do_get_mob_count)
{
	LPSECTREE_MAP pSectree = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());

	if (pSectree == NULL)
	{
		return;
	}

	FMobCounter f;
	f.nCount = 0;

	pSectree->for_each(f);

	ch->ChatPacket(CHAT_TYPE_INFO, "MapIndex: %d MobCount %d", ch->GetMapIndex(), f.nCount);
}

ACMD(do_clear_land)
{
	const building::CLand* pLand = building::CManager::instance().FindLand(ch->GetMapIndex(), ch->GetX(), ch->GetY());

	if ( NULL == pLand )
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "Guild Land(%d) Cleared", pLand->GetID());

	building::CManager::instance().ClearLand(pLand->GetID());
}

ACMD(do_special_item)
{
	ITEM_MANAGER::instance().ConvSpecialDropItemFile();
}

ACMD(do_set_stat)
{
	char szName [256];
	char szChangeAmount[256];

	two_arguments (argument, szName, sizeof (szName), szChangeAmount, sizeof(szChangeAmount));

	if (*szName == '\0' || *szChangeAmount == '\0')
	{
		ch->ChatPacket (CHAT_TYPE_INFO, "Invalid argument.");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(szName);

	if (!tch)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(szName);

		if (pkCCI)
		{
			ch->ChatPacket (CHAT_TYPE_INFO, "Cannot find player(%s). %s is not in your game server.", szName, szName);
			return;
		}
		else
		{
			ch->ChatPacket (CHAT_TYPE_INFO, "Cannot find player(%s). Perhaps %s doesn't login or exist.", szName, szName);
			return;
		}
	}
	else
	{
		if (tch->IsPolymorphed())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your state as long as you are transformed."));
			return;
		}

		if (subcmd != POINT_HT && subcmd != POINT_IQ && subcmd != POINT_ST && subcmd != POINT_DX)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Suborder or the Order is incorrect."));
			return;
		}

#ifdef __ADMIN_MANAGER__
		if (tch != ch && !GM::check_allow(ch->GetGMLevel(), GM_ALLOW_MODIFY_OTHERS))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have no rights to modify other players.");
			return;
		}
#endif

		int nRemainPoint = tch->GetPoint(POINT_STAT);
		int nCurPoint = tch->GetRealPoint(subcmd);
		int nChangeAmount = 0;
		str_to_number(nChangeAmount, szChangeAmount);
		int nPoint = nCurPoint + nChangeAmount;

		int n = -1;
		switch (subcmd)
		{
		case POINT_HT:
			if (nPoint < JobInitialPoints[tch->GetJob()].ht)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot set stat under initial stat."));
				return;
			}
			n = 0;
			break;
		case POINT_IQ:
			if (nPoint < JobInitialPoints[tch->GetJob()].iq)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot set stat under initial stat."));
				return;
			}
			n = 1;
			break;
		case POINT_ST:
			if (nPoint < JobInitialPoints[tch->GetJob()].st)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot set stat under initial stat."));
				return;
			}
			n = 2;
			break;
		case POINT_DX:
			if (nPoint < JobInitialPoints[tch->GetJob()].dx)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot set stat under initial stat."));
				return;
			}
			n = 3;
			break;
		}

		if (nPoint > 90)
		{
			nChangeAmount -= nPoint - 90;
			nPoint = 90;
		}

		if (nRemainPoint < nChangeAmount)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Left State Points are too low."));
			return;
		}

		tch->SetRealPoint(subcmd, nPoint);
		tch->SetPoint(subcmd, tch->GetPoint(subcmd) + nChangeAmount);
		tch->ComputePoints();
		tch->PointChange(subcmd, 0);

		tch->PointChange(POINT_STAT, -nChangeAmount);
		tch->ComputePoints();

		const char* stat_name[4] = {"con", "int", "str", "dex"};
		if (-1 == n)
		{
			return;
		}
		ch->ChatPacket(CHAT_TYPE_INFO, "%s's %s change %d to %d", szName, stat_name[n], nCurPoint, nPoint);
	}
}

ACMD(do_get_item_id_list)
{
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; i++)
	{
		LPITEM item = ch->GetInventoryItem(i);
		if (item != NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cell : %d, name : %s, id : %d", item->GetCell(), item->GetName(), item->GetID());
		}
	}
}

ACMD(do_set_socket)
{
	char arg1 [256];
	char arg2 [256];
	char arg3 [256];

	one_argument (two_arguments (argument, arg1, sizeof (arg1), arg2, sizeof(arg2)), arg3, sizeof (arg3));

	int item_id, socket_num, value;
	if (!str_to_number (item_id, arg1) || !str_to_number (socket_num, arg2) || !str_to_number (value, arg3))
	{
		return;
	}

	LPITEM item = ITEM_MANAGER::instance().Find (item_id);
	if (item)
	{
		item->SetSocket (socket_num, value);
	}
}

ACMD (do_can_dead)
{
	if (subcmd)
	{
		ch->SetArmada();
	}
	else
	{
		ch->ResetArmada();
	}
}

ACMD (do_all_skill_master)
{
	ch->SetHorseLevel(SKILL_MAX_LEVEL);
	for (int i = 0; i < SKILL_MAX_NUM; i++)
	{
		if (true == ch->CanUseSkill(i))
		{
			switch (i)
			{
			// @fixme154 BEGIN
			// taking out the it->second->bMaxLevel from map_pkSkillProto (&& 1==40|SKILL_MAX_LEVEL) will be very resource-wasting, so we go full ugly so far
			case SKILL_COMBO:
				ch->SetSkillLevel(i, 2);
				break;
			case SKILL_LANGUAGE1:
			case SKILL_LANGUAGE2:
			case SKILL_LANGUAGE3:
				ch->SetSkillLevel(i, 20);
				break;
			case SKILL_HORSE_SUMMON:
				ch->SetSkillLevel(i, 10);
				break;
			case SKILL_HORSE:
				ch->SetSkillLevel(i, HORSE_MAX_LEVEL);
				break;
			// CanUseSkill will be true for skill_horse_skills if riding
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:
				ch->SetSkillLevel(i, 20);
				break;
			// @fixme154 END
			default:
				ch->SetSkillLevel(i, SKILL_MAX_LEVEL);
				break;
			}
		}
		else
		{
			switch (i)
			{
			case SKILL_HORSE_WILDATTACK:
			case SKILL_HORSE_CHARGE:
			case SKILL_HORSE_ESCAPE:
			case SKILL_HORSE_WILDATTACK_RANGE:
				ch->SetSkillLevel(i, 20); // @fixme154 40 -> 20
				break;
			}
		}
	}
	ch->SetHorseLevel(HORSE_MAX_LEVEL);
	ch->ComputePoints();
	ch->SkillLevelPacket();
}

ACMD (do_item_full_set)
{
	BYTE job = ch->GetJob();
	LPITEM item;
	for (int i = 0; i < 6; i++)
	{
		item = ch->GetWear(i);
		if (item != NULL)
		{
			ch->UnequipItem(item);
		}
	}
	item = ch->GetWear(WEAR_SHIELD);
	if (item != NULL)
	{
		ch->UnequipItem(item);
	}

	switch (job)
	{
	case JOB_SURA:
	{

		item = ITEM_MANAGER::instance().CreateItem(11699);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(13049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(15189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(12529 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(14109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(17209 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(16209 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
	}
	break;
	case JOB_WARRIOR:
	{

		item = ITEM_MANAGER::instance().CreateItem(11299);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(13049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(15189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(3159 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(12249 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(14109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(17109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(16109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
	}
	break;
	case JOB_SHAMAN:
	{

		item = ITEM_MANAGER::instance().CreateItem(11899);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(13049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(15189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(7159 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(12669 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(14109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(17209 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(16209 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
	}
	break;
	case JOB_ASSASSIN:
	{

		item = ITEM_MANAGER::instance().CreateItem(11499);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(13049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(15189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(1139 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(12389 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(14109 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(17189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(16189 );
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
	}
	break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case JOB_WOLFMAN:
	{

		item = ITEM_MANAGER::instance().CreateItem(21049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(13049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(15189);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(6049);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(21559);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(14109);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(17209);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
		item = ITEM_MANAGER::instance().CreateItem(16209);
		if (!item || !item->EquipTo(ch, item->FindEquipCell(ch)))
		{
			M2_DESTROY_ITEM(item);
		}
	}
	break;
#endif
	}
}

ACMD (do_attr_full_set)
{
	BYTE job = ch->GetJob();
	LPITEM item;

	switch (job)
	{
	case JOB_WARRIOR:
	case JOB_ASSASSIN:
	case JOB_SURA:
	case JOB_SHAMAN:
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case JOB_WOLFMAN:
#endif
	{


		item = ch->GetWear(WEAR_HEAD);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_ATT_SPEED, 8);
			item->SetForceAttribute( 1, APPLY_HP_REGEN, 30);
			item->SetForceAttribute( 2, APPLY_SP_REGEN, 30);
			item->SetForceAttribute( 3, APPLY_DODGE, 15);
			item->SetForceAttribute( 4, APPLY_STEAL_SP, 10);
		}

		item = ch->GetWear(WEAR_WEAPON);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_CAST_SPEED, 20);
			item->SetForceAttribute( 1, APPLY_CRITICAL_PCT, 10);
			item->SetForceAttribute( 2, APPLY_PENETRATE_PCT, 10);
			item->SetForceAttribute( 3, APPLY_ATTBONUS_DEVIL, 20);
			item->SetForceAttribute( 4, APPLY_STR, 12);
		}

		item = ch->GetWear(WEAR_SHIELD);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_CON, 12);
			item->SetForceAttribute( 1, APPLY_BLOCK, 15);
			item->SetForceAttribute( 2, APPLY_REFLECT_MELEE, 10);
			item->SetForceAttribute( 3, APPLY_IMMUNE_STUN, 1);
			item->SetForceAttribute( 4, APPLY_IMMUNE_SLOW, 1);
		}

		item = ch->GetWear(WEAR_BODY);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
			item->SetForceAttribute( 1, APPLY_CAST_SPEED, 20);
			item->SetForceAttribute( 2, APPLY_STEAL_HP, 10);
			item->SetForceAttribute( 3, APPLY_REFLECT_MELEE, 10);
			item->SetForceAttribute( 4, APPLY_ATT_GRADE_BONUS, 50);
		}

		item = ch->GetWear(WEAR_FOOTS);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
			item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
			item->SetForceAttribute( 2, APPLY_MOV_SPEED, 8);
			item->SetForceAttribute( 3, APPLY_ATT_SPEED, 8);
			item->SetForceAttribute( 4, APPLY_CRITICAL_PCT, 10);
		}

		item = ch->GetWear(WEAR_WRIST);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
			item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
			item->SetForceAttribute( 2, APPLY_PENETRATE_PCT, 10);
			item->SetForceAttribute( 3, APPLY_STEAL_HP, 10);
			item->SetForceAttribute( 4, APPLY_MANA_BURN_PCT, 10);
		}
		item = ch->GetWear(WEAR_NECK);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_MAX_HP, 2000);
			item->SetForceAttribute( 1, APPLY_MAX_SP, 80);
			item->SetForceAttribute( 2, APPLY_CRITICAL_PCT, 10);
			item->SetForceAttribute( 3, APPLY_PENETRATE_PCT, 10);
			item->SetForceAttribute( 4, APPLY_STEAL_SP, 10);
		}
		item = ch->GetWear(WEAR_EAR);
		if (item != NULL)
		{
			item->ClearAttribute();
			item->SetForceAttribute( 0, APPLY_MOV_SPEED, 20);
			item->SetForceAttribute( 1, APPLY_MANA_BURN_PCT, 10);
			item->SetForceAttribute( 2, APPLY_POISON_REDUCE, 5);
			item->SetForceAttribute( 3, APPLY_ATTBONUS_DEVIL, 20);
			item->SetForceAttribute( 4, APPLY_ATTBONUS_UNDEAD, 20);
		}
	}
	break;
	}
}

ACMD (do_full_set)
{
	do_all_skill_master(ch, NULL, 0, 0);
	do_item_full_set(ch, NULL, 0, 0);
	do_attr_full_set(ch, NULL, 0, 0);
}

ACMD (do_use_item)
{
	char arg1 [256];

	one_argument (argument, arg1, sizeof (arg1));

	int cell = 0;
	str_to_number(cell, arg1);

	LPITEM item = ch->GetInventoryItem(cell);
	if (item)
	{
		ch->UseItem(TItemPos (INVENTORY, cell));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "ľĆŔĚĹŰŔĚ ľřľîĽ­ ÂřżëÇŇ Ľö ľřľî.");
	}
}

ACMD (do_clear_affect)
{
	ch->ClearAffect(true);
}

ACMD (do_dragon_soul)
{
	char arg1[512];
	const char* rest = one_argument (argument, arg1, sizeof(arg1));
	switch (arg1[0])
	{
	case 'a':
	{
		one_argument (rest, arg1, sizeof(arg1));
		int deck_idx;
		if (str_to_number(deck_idx, arg1) == false)
		{
			return;
		}
		ch->DragonSoul_ActivateDeck(deck_idx);
	}
	break;
	case 'd':
	{
		ch->DragonSoul_DeactivateAll();
	}
	break;
	}
}

ACMD (do_ds_list)
{
	for (int i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; i++)
	{
		TItemPos cell(DRAGON_SOUL_INVENTORY, i);

		LPITEM item = ch->GetItem(cell);
		if (item != NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "cell : %d, name : %s, id : %d", item->GetCell(), item->GetName(), item->GetID());
		}
	}
}

#ifdef __EVENT_MANAGER_ENABLE__
ACMD(do_event_manager_open)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "EventManagerOpenPanel");
}

ACMD(do_event_manager_update)
{
	char arg1[256], arg2[256];
	const char * line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	std::string e_key = arg1;
	bool bActionType = static_cast<bool>(atoi(arg2));

	if (bActionType)
	{
		char arg3[256];
		one_argument(line, arg3, sizeof(arg3));
		if (!*arg3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You have not provided event time.");
			return;
		}

		if (CEventManager::instance().RegisterEvent(std::move(e_key), (atoi(arg3) + get_global_time())))
		{
			CEventManager::instance().RefreshImplementorPanel(ch);
		}
	}
	else
	{
		if (CEventManager::instance().UnregisterEvent(std::move(e_key)))
		{
			CEventManager::instance().RefreshImplementorPanel(ch);
		}
	}
}
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
ACMD(do_technical_maintenance_open_panel)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "TechnicalMaintenanceOpenPanel");
}

ACMD(do_technical_maintenance_add)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
	{
		return;
	}

	std::string sMaintenanceName = arg1;
	time_t ttMaintenanceTime = atoi(arg2);

	// Replacing underscores with whitespaces
	// boost::replace_all(sMaintenanceName, "_", " ");

	if (CTechnicalMaintenance::instance().FindMaintenance(sMaintenanceName))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Maintenance is already scheduled. You better try delaying/canceling it.");
	}
	else
	{
		CTechnicalMaintenance::instance().RegisterMaintenance(sMaintenanceName, ttMaintenanceTime, true);
	}
}

ACMD(do_technical_maintenance_delay)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
	{
		return;
	}

	std::string sMaintenanceName = arg1;
	time_t ttMaintenanceNewTime = atoi(arg2);

	// Replacing underscores with whitespaces
	// boost::replace_all(sMaintenanceName, "_", " ");

	if (!CTechnicalMaintenance::instance().FindMaintenance(sMaintenanceName))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no maintenance scheduled. You better try append one.");
	}
	else
	{
		CTechnicalMaintenance::instance().DelayMaintenance(sMaintenanceName, ttMaintenanceNewTime, true);
	}
}

ACMD(do_technical_maintenance_cancel)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	std::string sMaintenanceName = arg1;

	// Replacing underscores with whitespaces
	// boost::replace_all(sMaintenanceName, "_", " ");

	if (!CTechnicalMaintenance::instance().FindMaintenance(sMaintenanceName))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "There is no maintenance scheduled. You better try append one.");
	}
	else
	{
		CTechnicalMaintenance::instance().CancelMaintenance(sMaintenanceName, true);
	}
}
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
ACMD(do_notification_open_interface)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleOpenInterface");
}

ACMD(do_notification_sender_request_list)
{
	NotificatonSender::SendPlayerList(ch);
}

ACMD(do_notification_sender_send)
{
	switch (NotificatonSender::SendNotifications(ch))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NOTIFICATION_SENDER_CANNOT_SEND_SYSTEM_ERROR"));
		break;
	case 0:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NOTIFICATION_SENDER_NO_LIST_LOADED"));
		break;
	case 1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NOTIFICATION_SENDER_NOTE_HAS_BEEN_SENT"));
		break;
	}
}
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
ACMD(do_respawn_monster)
{
	char arg1[256], arg2[256];
	const char * line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Unknown command. Should be: /respawn_monster MapIndex(0 in case of all maps) Broadcast(0 in case of disable) vnum_list...");
		return;
	}

	long lMapIndex = atol(arg1);
	bool bBroadcast = static_cast<bool>(atoi(arg2));

	std::set<DWORD> s_monster;
	char arg_extra[256] = {0};

	do
	{
		line = one_argument(line, arg_extra, sizeof(arg_extra));
		if (!*arg_extra)
		{
			break;
		}
		else
		{
			s_monster.insert(static_cast<DWORD>(atoi(arg_extra)));
		}
	} while (*arg_extra);

	MonsterRespawner::RespawnSingleMonster(std::move(s_monster), lMapIndex, bBroadcast);
}
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
ACMD(do_zuo_event_open_panel)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ZuoMenuDialog");
}

ACMD(do_zuo_event_manage_status)
{
	if (g_bChannel != 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ZUO_EVENT_CHANNEL_RESTRICTION"));
		return;
	}

	if (ch->GetMapIndex() != CZuoEventManager::GetEventMapIndex())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ZUO_EVENT_MAP_RESTRICTION"));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	CZuoEventManager::instance().ChangeEventStatus(!*arg1 ? -1 : atoi(arg1));
}

ACMD(do_zuo_event_spawn_monster)
{
	if (g_bChannel != 1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ZUO_EVENT_CHANNEL_RESTRICTION"));
		return;
	}

	if (ch->GetMapIndex() != CZuoEventManager::GetEventMapIndex())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ZUO_EVENT_MAP_RESTRICTION"));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	if (!*arg1 || !*arg2)
	{
		return;
	}

	DWORD dwVnum = atoi(arg1);
	WORD wCount = atoi(arg2);

	CZuoEventManager::instance().SpawnMonster(dwVnum, wCount);
}
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
ACMD(do_rights_controller_give)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "usage: give_rights <right name[\"LOW_WIZARD\",\"WIZARD\",\"HIGH_WIZARD\",\"GOD\",\"IMPLEMENTOR\"]/id[1-5]) <player name>");
		}
		return;
	}

	BYTE bAuthority;
	if (str_is_number(arg1))
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Please write the right name in words");
		}
		return;
		/*		str_to_number(bAuthority, arg1);
		if (bAuthority <= GM_PLAYER || bAuthority > GM_IMPLEMENTOR)
		{
		ch->ChatPacket(CHAT_TYPE_INFO, "Unkown right-ID %u [expected number between 1 and 5]", bAuthority);
		return;
		}*/
	}
	else
	{
		if (!strcasecmp(arg1, "LOW_WIZARD"))
		{
			bAuthority = GM_LOW_WIZARD;
		}
		else if (!strcasecmp(arg1, "WIZARD"))
		{
			bAuthority = GM_WIZARD;
		}
		else if (!strcasecmp(arg1, "HIGH_WIZARD"))
		{
			bAuthority = GM_HIGH_WIZARD;
		}
		else if (!strcasecmp(arg1, "GOD"))
		{
			bAuthority = GM_GOD;
		}
		else if (!strcasecmp(arg1, "IMPLEMENTOR"))
		{
			bAuthority = GM_IMPLEMENTOR;
		}
		else
		{
			if (ch)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Unkown right-name %s [expected \"LOW_WIZARD\", \"WIZARD\", \"HIGH_WIZARD\", \"GOD\" or \"IMPLEMENTOR\"]", arg1);
			}
			return;
		}
	}

	if (ch && !strcasecmp(ch->GetName(), arg2))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You cannot give rights to yourself.");
		return;
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);
	if (!tch)
	{
		CCI* p2pCCI = P2P_MANAGER::instance().Find(arg2);
		if (!p2pCCI)
		{
			char szPlayerName[CHARACTER_NAME_MAX_LEN * 2 + 1];
			DBManager::instance().EscapeString(szPlayerName, sizeof(szPlayerName), arg2, strlen(arg2));
			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT name FROM player WHERE name LIKE '%s'", szPlayerName));
			if (pMsg->Get()->uiNumRows == 0)
			{
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "The player %s does not exist.", arg2);
				}
				return;
			}

			strlcpy(arg2, *mysql_fetch_row(pMsg->Get()->pSQLResult), sizeof(arg2));
		}
		else
		{
			strlcpy(arg2, p2pCCI->szName, sizeof(arg2));
		}
	}
	else
	{
		strlcpy(arg2, tch->GetName(), sizeof(arg2));
	}

	tAdminInfo info;
	memset(&info, 0, sizeof(info));
	info.m_Authority = bAuthority;
	strlcpy(info.m_szName, arg2, sizeof(info.m_szName));
	strlcpy(info.m_szAccount, "[ALL]", sizeof(info.m_szAccount));
	GM::insert(info);

	if (tch)
	{
		tch->SetGMLevel();
	}

	TPacketGGUpdateRights packet;
	packet.header = HEADER_GG_UPDATE_RIGHTS;
	strlcpy(packet.name, arg2, sizeof(packet.name));
	packet.gm_level = bAuthority;
	P2P_MANAGER::instance().Send(&packet, sizeof(packet));

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT login FROM player INNER join account.account ON account.id = player.account_id WHERE name = '%s'", info.m_szName));
	if (pMsg->Get()->uiNumRows == 0)
	{
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);

	DBManager::instance().DirectQuery("REPLACE INTO common.`gmlist` (`mAccount`, `mName`, `mAuthority`) VALUES ('%s', '%s', '%s')", row[0], info.m_szName, arg1);

	if (ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The rights of %s has been changed to %s.", arg2, arg1);
	}
}

ACMD(do_rights_controller_remove)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "usage: remove_rights <player name>");
		}
		return;
	}

	if (!strcasecmp(ch->GetName(), arg1))
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You cannot remove your own rights.");
		}
		return;
	}

	if (GM::get_level(arg1) == GM_PLAYER)
	{
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "No player %s with GM-Rights has been found.", arg1);
		}
		return;
	}

	GM::remove(arg1);

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg1);
	if (tch)
	{
		tch->SetGMLevel();
	}

	TPacketGGUpdateRights packet;
	packet.header = HEADER_GG_UPDATE_RIGHTS;
	strlcpy(packet.name, arg1, sizeof(packet.name));
	packet.gm_level = GM_PLAYER;
	P2P_MANAGER::instance().Send(&packet, sizeof(packet));

	char szPlayerName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	DBManager::instance().EscapeString(szPlayerName, sizeof(szPlayerName), arg1, strlen(arg1));
	DBManager::instance().DirectQuery("DELETE FROM common.`gmlist` WHERE (`mName`='%s')", szPlayerName);

	if (ch)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "The rights of %s has been removed (new status: GM_PLAYER).", arg1);
	}
}

#endif

#ifdef __BATTLE_PASS_ENABLE__
ACMD(do_battle_pass_test)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	CBattlePassManager::instance().RegisterBattlePass(ch, static_cast<BattlePassNS::EDiffLevel>(atoi(arg1)));
}
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
#include "find_letters_event.h"
ACMD(do_find_letters_request)
{
	ch->FindLettersEventSendInfo();
}

ACMD(do_find_letters_add)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	int iLetterIndex = -1;
	int iInventoryIndex = -1;

	if (!*arg1 && !*arg2)
	{
		return;
	}

	if (isnhdigit(*arg1) && isnhdigit(*arg2))
	{
		str_to_number(iLetterIndex, arg1);
		str_to_number(iInventoryIndex, arg2);

		ch->FindLettersAddLetter(iLetterIndex, iInventoryIndex);
	}
}

ACMD(do_find_letters)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: find_letters <command> <time>");
		ch->ChatPacket(CHAT_TYPE_INFO, "	0 = Disable.");
		ch->ChatPacket(CHAT_TYPE_INFO, "	1 = Enable.");
		ch->ChatPacket(CHAT_TYPE_INFO, "<time> Time that event is online.");
		return;
	}

	if (isnhdigit(*arg1))
	{
		int iCommand = 0;
		str_to_number(iCommand, arg1);

		if (iCommand == 0)
		{
			quest::CQuestManager::instance().RequestSetEventFlag("enable_find_letters", 0);
			quest::CQuestManager::instance().RequestSetEventFlag("find_letters_drop", 0);
			ch->ChatPacket(CHAT_TYPE_INFO, "You deactivated find letters event.");
		}
		else
		{
			if (quest::CQuestManager::instance().GetEventFlag("enable_find_letters") == 0)
			{
				if (!*arg2)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You need to enter time.");
					return;
				}

				if (CFindLetters::instance().GetWord() == "")
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You haven't set a word yet.");
					return;
				}

				std::string stArg = argument;
				std::string realTime = stArg.substr(3, stArg.length());

				long lEventDuration = parse_time_str(realTime.c_str());
				int iEndTime = time(0) + lEventDuration;

				quest::CQuestManager::instance().RequestSetEventFlag("enable_find_letters", 1);
				quest::CQuestManager::instance().RequestSetEventFlag("find_letters_drop", 1);
				quest::CQuestManager::instance().RequestSetEventFlag("find_letters_event_end_time", iEndTime);

				SendNotice("Find letters event is now active.");
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "This event is already opened.");
			}
		}
	}
}
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
ACMD(do_set_is_show_teamler)
{
	if (!ch)
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (*arg1 == '0')
	{
		ch->SetIsShowTeamler(false);
	}
	else
	{
		ch->SetIsShowTeamler(true);
	}
}
#endif

#ifdef __OX_EVENT_SYSTEM_ENABLE__
static bool IsAllowedToOX(LPCHARACTER ch)
{
	if (ch->GetGMLevel() != GM_IMPLEMENTOR)
	{
		return false;
	}

	//if (g_bChannel != 1)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_CHANNEL_RESTRICTION"));
	//	return false;
	//}

	if (ch->GetMapIndex() != OXEVENT_MAP_INDEX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_MAP_RESTRICTION"));
		return false;
	}

	return true;
}

ACMD(do_ox_menu)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "OxMenuDialog");
}

ACMD(do_ox_start_event)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	BYTE status = COXEventManager::instance().GetStatus();
	if (status == OXEVENT_FINISH)
	{
		COXEventManager::instance().ClearQuiz();

		char script[256];
		snprintf(script, sizeof(script), "%s/oxquiz.lua", GetBasePath().c_str());
		int result = lua_dofile(quest::CQuestManager::instance().GetLuaState(), script);
		if (result != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_QUESTION_LOAD_ERROR"));
			return;
		}

		COXEventManager::instance().SetStatus(OXEVENT_OPEN);
		BroadcastNotice(LC_TEXT("OX_EVENT_START_MESSAGE_1"));
		BroadcastNotice(LC_TEXT("OX_EVENT_START_MESSAGE_2"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_ON_GOING"));
	}
}

ACMD(do_ox_close_event)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	BYTE status = COXEventManager::instance().GetStatus();
	if (status == OXEVENT_OPEN)
	{
		COXEventManager::instance().SetStatus(OXEVENT_CLOSE);
		BroadcastNotice(LC_TEXT("OX_EVENT_REGISTRATION_CLOSE"));
		BroadcastNotice(LC_TEXT("OX_EVENT_HAS_BEEN_STARTED"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_ON_GOING"));
	}
}

EVENTINFO(end_oxevent_info_n)
{
	int empty;

	end_oxevent_info_n()
		: empty(0)
	{
	}
};

EVENTFUNC(end_oxevent_n)
{
	COXEventManager::instance().CloseEvent();
	return 0;
}

ACMD(do_ox_cancel_event)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	BYTE status = COXEventManager::instance().GetStatus();
	if (status != OXEVENT_FINISH)
	{
		COXEventManager::instance().SetStatus(OXEVENT_FINISH);
		BroadcastNotice(LC_TEXT("OX_EVENT_HAS_BEEN_ENDED"));

		end_oxevent_info_n* info = AllocEventInfo<end_oxevent_info_n>();
		event_create(end_oxevent_n, info, PASSES_PER_SEC(5));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_IS_NOT_ONGOING"));
	}
}

enum OxCommandType
{
	TYPE_NORMAL,
	TYPE_TRAP,
};

ACMD(do_ox_question)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	BYTE status = COXEventManager::instance().GetStatus();
	if (status != OXEVENT_QUIZ && status != OXEVENT_CLOSE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_NOT_VALID_STATE_FOR_QUESTION"));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE type = atoi(arg1);
	switch (type)
	{
	case TYPE_NORMAL:
		COXEventManager::instance().Quiz(1, 30);
		break;
	case TYPE_TRAP:
		COXEventManager::instance().Quiz("OX_EVENT_TRAP_QUESTION", 30);
		break;
	}
}

ACMD(do_ox_give_reward)
{
	if (!IsAllowedToOX(ch))
	{
		return;
	}

	BYTE status = COXEventManager::instance().GetStatus();
	if (status != OXEVENT_QUIZ && status != OXEVENT_CLOSE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_NOT_VALID_STATE_FOR_PRIZE"));
		return;
	}

	char arg1[256], arg2[256], arg3[256];
	const char* line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));
	one_argument(line, arg3, sizeof(arg3));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	DWORD vnum = std::strtoul(arg1, NULL, 10);
	WORD count = std::strtoul(arg2, NULL, 10);
	DWORD item_time = 0;

	const TItemTable* item_proto = ITEM_MANAGER::instance().GetTable_NEW(vnum);
	if (!item_proto)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("OX_EVENT_ITEM_NOT_EXIST"));
		return;
	}
	else
	{
		if (*arg3)
		{
			for (const auto& limit : item_proto->aLimits)
			{
				if (limit.bType == LIMIT_TIMER_BASED_ON_WEAR || limit.bType == LIMIT_REAL_TIME || limit.bType == LIMIT_REAL_TIME_START_FIRST_USE)
				{
					item_time = std::strtoul(arg3, NULL, 10);
					break;
				}
			}
		}
	}

	if (count >= ITEM_MAX_COUNT)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Nie za duzo kurwa?"); // Translate
		return;
	}

	COXEventManager::instance().GiveItemToAttender(vnum, count, item_time);
#ifndef __MULTI_LANGUAGE_SYSTEM__
	std::string notice = "[EVENT OX] Administrator " + std::string(ch->GetName()) + " rozdal uczetnikom nagrode w postaci " + std::string(item_proto->szLocaleName) + " w ilosci " + std::to_string(count) + "!";
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNoticeMap(LC_TEXT("OX_EVENT_PRIZE_TEXT %s %s %d", ch->GetName(), item_proto->szLocaleName[MINMAX(0, ch->GetLanguage(), (MAX_LANGUAGE_COUNT - 1))], count), ch->GetMapIndex(), true);
#else
	SendNoticeMap(notice.c_str(), ch->GetMapIndex(), true);
#endif
}

// Pool
ACMD(do_ox_print_pools)
{
	COXEventManager::instance().PrintCurrentPools(ch);
}

ACMD(do_ox_add_question_to_pool)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	BYTE bType = atoi(arg1);
	WORD wCount = atoi(arg2);

	if (!COXEventManager::instance().AddQuestionToPool(bType, wCount))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Unknown pool name or selected pool is empty.");
		return;
	}

	if (!wCount)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "All questions have been added!");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "%d questions have not been added! (Pool got empty))", wCount);
	}
}

ACMD(do_ox_reset_pool)
{
	COXEventManager::instance().FlushPool();
}
#endif

#ifdef __ENABLE_FIND_LOCATION__
ACMD(do_get_player_location)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1)
	{
		return;
	}

	std::string p_name = arg1;
	auto p_info = std::move(CHARACTER_MANAGER::instance().GetPlayerLocation(p_name));

	if (!p_info.dwMapIndex)
	{
		p_info.bHeader = HEADER_GG_PLAYER_LOCATION;
		p_info.searchPID = ch->GetPlayerID();
		strlcpy(p_info.sPlayerName, p_name.c_str(), sizeof(p_info.sPlayerName));
		P2P_MANAGER::instance().Send(&p_info, sizeof(p_info));

		ch->ChatPacket(CHAT_TYPE_INFO, "There is no %s on this channel.", p_name.c_str());
		ch->ChatPacket(CHAT_TYPE_INFO, "If you don't receive further response that means provided player is currently offline.");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Player %s has been found on %d channel.", p_name.c_str(), g_bChannel);
		ch->ChatPacket(CHAT_TYPE_INFO, "Details: MapIndex: %u, X coords: %ld, Y coords: %ld", p_info.dwMapIndex, p_info.lx, p_info.ly);
	}
}

ACMD(do_find_monster)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Syntax: find_monster <vnum>");
		return;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(ch->GetMapIndex());
	if (!pMap)
	{
		return;
	}

	DWORD dwVnum = 0;
	str_to_number(dwVnum, arg1);

	auto lbPrintMonsters = [&ch, &dwVnum](LPENTITY ent)
	{
		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER pChar = static_cast<LPCHARACTER>(ent);
			if (pChar->IsMonster() && pChar->GetRaceNum() == dwVnum)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "%s in location %dx%d", pChar->GetName(), pChar->GetX(), pChar->GetY());
			}
		}
	};

	pMap->for_each(lbPrintMonsters);
}

#endif

#ifdef __ENABLE_OFFLINE_SHOP__
ACMD(do_close_offline_shop_force)
{
	char arg1[16];
	one_argument(argument, arg1, sizeof(arg1));

	uint32_t playerId = static_cast<uint32_t>(atoi(arg1));

	auto shops = COfflineShop::GetPlayerShops(playerId);
	for (auto shopId : shops)
	{
		auto shopPtr = COfflineShop::Get(shopId);
		if (!shopPtr)
		{
			continue;
		}

		auto shop = shopPtr->get();
		shop->Close();

		ch->ChatPacket(CHAT_TYPE_INFO, "Closed shop \"%s\".", shop->GetName().c_str());
	}
}
#endif

#ifdef __ITEM_SHOP_ENABLE__
ACMD(do_itemshop_editor_mode)
{
	static const std::string sEditorPassword("HAJSKURWY");
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	one_argument(argument, arg1, sizeof(arg1));
	if (!*arg1 || !*arg2)
	{
		return;
	}

	if (arg1 != sEditorPassword)
	{
		return;
	}

	ch->SetItemShopEditor(static_cast<bool>(atoi(arg2)));
	ch->ChatPacket(CHAT_TYPE_INFO, "ItemShopEditorMode %d", ch->IsItemShopEditor());
}

ACMD(do_itemshop_amendment)
{
}
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
#include <boost/lexical_cast.hpp>

#ifdef __HARDWARE_BAN__
	#include "Hardware.h"
#endif

static bool ChopBanTime(std::string str)
{
	if (!str.size())
	{
		return false;
	}

	if (str.find(":") == std::string::npos)
	{
		return false;
	}

	BYTE col_count = 0;

	while (str.find_last_of(":") != std::string::npos)
	{
		if (str.substr(str.find_last_of(":")) != "")
		{
			try
			{
				boost::lexical_cast<WORD>(str.substr(str.find_last_of(":") + 1));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				return false;
			}
		}

		str.erase(str.begin() + str.find_last_of(":"), str.end());
		col_count++;
	}

	if (col_count != 2)
	{
		return false;
	}

	if (str != "")
	{
		try
		{
			boost::lexical_cast<unsigned short>(str);
		}
		catch (const boost::bad_lexical_cast& e)
		{
			return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

static std::string GetDateFromTime(std::string str)
{
	static const std::string sDelimiter = ":";
	int iSubstractor = 60 * 60;
	time_t tFutureTime = get_global_time();

	while (str.find_first_of(sDelimiter, 0) != std::string::npos && iSubstractor > 0)
	{
		time_t ttDelPosition = str.find_first_of(sDelimiter, 0);
		tFutureTime += std::stoi(str.substr(0, ttDelPosition)) * iSubstractor;
		iSubstractor /= 60;
		str.erase(str.begin(), str.begin() + ttDelPosition + sDelimiter.size());
	}

	char buf[80];
	struct tm* timeinfo = localtime(&tFutureTime);
	strftime(buf, sizeof(buf), "%F %T", timeinfo);

	return std::string(buf);
}

ACMD(do_ban)
{
	char arg1[256], arg2[256], arg3[256];
	const char* line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	ch->ChatPacket(CHAT_TYPE_COMMAND, "OpenBanPanel");

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: <nickname> <type> <reason>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 1 - type normal");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 2 - type ip");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 3 - type time");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 4 - type unban");
		#ifdef __HARDWARE_BAN__
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 5 - type hardware");
		#endif
		return;
	}

	BYTE type = atoi(arg2);

	if (type >= BAN_MAX_NUM || type == BAN_NONE)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: <nickname> <type> <reason>");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 1 - type normal");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 2 - type ip");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 3 - type time");
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 4 - type unban");
		#ifdef __HARDWARE_BAN__
		ch->ChatPacket(CHAT_TYPE_INFO, "Ban Config: 5 - type hardware");
		#endif
		return;
	}

	std::string sReason, sReasonPrint;
	if (type != BAN_UNBAN)
	{
		line = one_argument(line, arg3, sizeof(arg3));
		if (!*arg3)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Not reason has been provided.");
			return;
		}

		sReason = arg3;
		while (sReason.find("|") != std::string::npos)
		{
			sReason.replace(sReason.find("|"), std::string("|").size(), " ");
		}

		sReasonPrint = sReason;
	}

	DWORD aid = 0;
	DWORD pid = 0;
	std::string ip = "";
	std::string hardware = "";
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT account_id, id, ip FROM player.player WHERE name = '%s' LIMIT 1", arg1));
	MYSQL_ROW row;

	if (msg->Get()->uiNumRows == 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Player %s does not exists.", arg1);
		return;
	}
	else
	{
		row = mysql_fetch_row(msg->Get()->pSQLResult);
		aid = atoi(row[0]);
		pid = atoi(row[1]);
		ip = row[2];
	}

	if (!aid || !pid)
	{
		return;
	}

	switch (type)
	{
	case BAN_NORMAL:
		sReason += "|NEVER";
		delete DBManager::instance().DirectQuery("UPDATE account.account SET status = 'BLOCK', ban_by = %d, ban_date = NOW(), ban_reason = '%s' WHERE id = %d", ch->GetPlayerID(), sReason.c_str(), aid);
		break;
	case BAN_IP:
	{
		sReason += "|NEVER";

		msg.reset(DBManager::instance().DirectQuery("SELECT DISTINCT account_id FROM player.player WHERE ip = '%s'", ip.c_str()));
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
		{
			delete DBManager::instance().DirectQuery("UPDATE account.account SET status = 'BLOCK', ban_by = %d, ban_date = NOW(), ban_reason = '%s' WHERE id = %d", ch->GetPlayerID(), sReason.c_str(), atoi(row[0]));
		}
		ch->ChatPacket(CHAT_TYPE_INFO, "All accounts of player named %s have been permanently banned.", arg1);
		break;
	}
	case BAN_TIME:
	{
		char arg4[256];
		one_argument(line, arg4, sizeof(arg4));
		std::string time = arg4;
		if (!ChopBanTime(time))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Invalid Data Format. Should be: HH:MM:SS");
			return;
		}

		sReason += "|" + std::string(GetDateFromTime(time));

		delete DBManager::instance().DirectQuery("UPDATE account.account SET availDt = NOW() + INTERVAL '%s' DAY_SECOND, ban_by = %d, ban_date = NOW(), ban_reason = '%s' WHERE id = %d", time.c_str(), ch->GetPlayerID(), sReason.c_str(), aid);
		ch->ChatPacket(CHAT_TYPE_INFO, "Player %s has been banned for: %s", arg1, time.c_str());
		break;
	}
	case BAN_UNBAN:
	{
		#ifdef __HARDWARE_BAN__
			msg.reset(DBManager::instance().DirectQuery("SELECT hardware_id FROM account.account WHERE id = %u LIMIT 1", aid));
			row = mysql_fetch_row(msg->Get()->pSQLResult);
			if (row[0])
			{
				TPacketGDDisconnectPlayer p;
				memset(&p, 0, sizeof(p));
				strlcpy(p.ban_hardware, row[0], sizeof(p.ban_hardware));
				p.type = false; // delete
				db_clientdesc->DBPacket(HEADER_GD_UPDATE_BAN, 0, &p, sizeof(p));
			}
		#endif

		msg.reset(DBManager::instance().DirectQuery("SELECT DISTINCT account_id FROM player.player WHERE ip = '%s'", ip.c_str()));
		while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
		{
			delete DBManager::instance().DirectQuery("UPDATE account.account SET status = 'OK', availDt = NOW(), ban_by = 0, ban_reason = '' WHERE id = %d", atoi(row[0]));
		}
		ch->ChatPacket(CHAT_TYPE_INFO, "Player %s has been unbanned.", arg1);
		break;
	}
#ifdef __HARDWARE_BAN__
	case BAN_HARDWARE:
	{
		sReason += "|NEVER";

		msg.reset(DBManager::instance().DirectQuery("SELECT hardware_id FROM account.account WHERE id = %u LIMIT 1", aid));
		row = mysql_fetch_row(msg->Get()->pSQLResult);
		if (row[0] && *row[0])
		{
			hardware = row[0];
			if (!hardware.size() || hardware[0] == '0')
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "Player %s cannot be banned on Hardware.", arg1);
				return;
			}

			TPacketGDDisconnectPlayer p;
			memset(&p, 0, sizeof(p));
			strlcpy(p.ban_hardware, row[0], sizeof(p.ban_hardware));
			p.type = true; // add
			db_clientdesc->DBPacket(HEADER_GD_UPDATE_BAN, 0, &p, sizeof(p));

			if (hardware.size())
				delete DBManager::instance().DirectQuery("UPDATE account.account SET ban_reason = '%s', status = 'BLOCK' WHERE hardware_id = '%s'", sReason.c_str(), row[0]);
		}

		if (hardware.size())
			ch->ChatPacket(CHAT_TYPE_INFO, "Player %s has been banned on his Hardware.", arg1);
		else
			ch->ChatPacket(CHAT_TYPE_INFO, "Player %s cannot be banned on Hardware.", arg1);
	}
	break;
#endif
	}

	if (type != BAN_UNBAN)
	{
		LPDESC d = DESC_MANAGER::instance().FindByCharacterName(arg1);
		LPCHARACTER	tch = d ? d->GetCharacter() : NULL;

		// Clearing billing
		SendBillingExpire(arg1, BILLING_DAY, 0, NULL);

		if (tch != ch)
		{
			if (tch)
			{
				DESC_MANAGER::instance().DestroyDesc(d);
			}
			else
			{
				// Sending P2P Disconnect Packet
				TPacketGGDisconnectPlayer p2;
				memset(&p2, 0, sizeof(p2));
				p2.bHeader = HEADER_GG_DISCONNECT_PLAYER;
				p2.pid = pid;
				P2P_MANAGER::instance().Send(&p2, sizeof(TPacketGGDisconnectPlayer));
			}
		}

		std::ostringstream os;
		os << "Gracz " << arg1 << " zablokowany";
		BroadcastNotice(os.str().c_str());

		os.str("");
		os.clear();
		os << "Typ banu: " << ((type == BAN_TIME) ? "czasowy" : "permamentny") << ". Opis: " << sReasonPrint.c_str() << ".";
		BroadcastNotice(os.str().c_str());
	}
}
#endif

#ifdef __TEAM_DAMAGE_FLAG_CHECK__
ACMD(do_set_show_damage_flag)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || !str_is_int(arg1))
	{
		return;
	}

	bool flag = false;
	str_to_number(flag, arg1);

	if (flag == ch->IsTeamSendDamageFlag())
	{
		if (flag)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "The damage flag is already set.");
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "The damage flag is already unset.");
		}

		return;
	}

	ch->SetTeamSendDamageFlag(flag);

	if (flag)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You can now see the damage of other players around.");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You can not see the damage of other players around anymore.");
	}
}
#endif

#ifdef __ENABLE_MISSION_MANAGER__
ACMD(do_mission_set)
{
	char arg1[16];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	CMissionManager::instance().RegisterMission(ch, static_cast<DWORD>(atoi(arg1)));
}
#endif
