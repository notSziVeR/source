#include "stdafx.h"
#ifdef __FreeBSD__
	#include <md5.h>
#else
	#include "../../libthecore/include/xmd5.h"
#endif
#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	#include "ZuoEvent.hpp"
#endif

#include "utils.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "motion.h"
#include "packet.h"
#include "affect.h"
#include "pvp.h"
#include "start_position.h"
#include "party.h"
#include "guild_manager.h"
#include "p2p.h"
#include "dungeon.h"
#include "messenger_manager.h"
#include "war_map.h"
#include "questmanager.h"
#include "item_manager.h"
#include "mob_manager.h"
#include "dev_log.h"
#include "item.h"
#include "arena.h"
#include "buffer_manager.h"
#include "unique_item.h"
#include "threeway_war.h"
#include "log.h"
#include "../../common/VnumHelper.h"
#ifdef __GAYA_SHOP_SYSTEM__
	#include "GayaSystemManager.hpp"
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	#include "NotificatonSender.hpp"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	#include "Transmutation.h"
#endif

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
	#include "PSkillsManager.hpp"
#endif

#ifdef __ENABLE_SAVE_POSITION__
	#include "PositionsManagerHelper.hpp"
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	#include "TeleportManagerSystem.hpp"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	#include "CharUtil.hpp"
#endif
#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __ITEM_SHOP_ENABLE__
	#include "ItemShopManager.hpp"
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __DUNGEON_INFO_ENABLE__
	#include "DungeonInfoManager.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	#include "SashSystemHelper.hpp"
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	#include "AmuletSystemHelper.hpp"
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	#include "AttendanceRewardManager.hpp"
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	#include "BeginnerHelper.hpp"
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	#include "MobTrackerManager.hpp"
#endif

// Workaround for string to number functions like
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

ACMD(do_user_horse_ride)
{
	if (ch->IsObserverMode())
	{
		return;
	}

	if (ch->IsDead() || ch->IsStun())
	{
		return;
	}

	if (ch->IsHorseRiding() == false)
	{

		if (ch->GetMountVnum())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You're already riding. Get off first."));
			return;
		}

		if (ch->GetHorse() == NULL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
			return;
		}

		ch->StartRiding();
	}
	else
	{
		ch->StopRiding();
	}
}

ACMD(do_user_horse_back)
{
	if (ch->GetHorse() != NULL)
	{
		ch->HorseSummon(false);
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have sent your horse away."));
	}
	else if (ch->IsHorseRiding() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to get off your Horse."));
	}
	else
	{
#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
		// search for TOGGLE_MOUNT
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			if (LPITEM item = ch->GetInventoryItem(i))
			{
				if (item->GetType() != ITEM_TOGGLE || item->GetSubType() != TOGGLE_MOUNT)
				{
					continue;
				}

				if (item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE) != true)
				{
					continue;
				}

				DeactivateToggleItem(ch, item);
				return;
			}
		}
#endif
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
	}
}

ACMD(do_user_horse_feed)
{

	if (ch->GetMyShop())
	{
		return;
	}

	if (ch->GetHorse() == NULL)
	{
		if (ch->IsHorseRiding() == false)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed your Horse whilst sitting on it."));
		}
		return;
	}

	DWORD dwFood = ch->GetHorseGrade() + 50054 - 1;

	if (ch->CountSpecifyItem(dwFood) > 0)
	{
		ch->RemoveSpecifyItem(dwFood, 1);
		ch->FeedHorse();
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have fed the Horse with %s%s."),
					   ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName,
					   "");
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %s."), ITEM_MANAGER::instance().GetTable(dwFood)->szLocaleName);
	}
}

#define MAX_REASON_LEN		128

EVENTINFO(TimedEventInfo)
{
	DynamicCharacterPtr ch;
	int		subcmd;
	int         	left_second;
	char		szReason[MAX_REASON_LEN];

	TimedEventInfo()
		: ch()
		, subcmd( 0 )
		, left_second( 0 )
	{
		::memset( szReason, 0, MAX_REASON_LEN );
	}
};

struct SendDisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetCharacter())
		{
			if (d->GetCharacter()->GetGMLevel() == GM_PLAYER)
			{
				d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "quit Shutdown(SendDisconnectFunc)");
			}
		}
	}
};

struct DisconnectFunc
{
	void operator () (LPDESC d)
	{
		if (d->GetType() == DESC_TYPE_CONNECTOR)
		{
			return;
		}

		if (d->IsPhase(PHASE_P2P))
		{
			return;
		}

		if (d->GetCharacter())
		{
			d->GetCharacter()->Disconnect("Shutdown(DisconnectFunc)");
		}

		d->SetPhase(PHASE_CLOSE);
	}
};

EVENTINFO(shutdown_event_data)
{
	int seconds;

	shutdown_event_data()
		: seconds( 0 )
	{
	}
};

EVENTFUNC(shutdown_event)
{
	shutdown_event_data* info = dynamic_cast<shutdown_event_data*>( event->info );

	if ( info == NULL )
	{
		sys_err( "shutdown_event> <Factor> Null pointer" );
		return 0;
	}

	int * pSec = & (info->seconds);

	if (*pSec < 0)
	{
		sys_log(0, "shutdown_event sec %d", *pSec);

		if (--*pSec == -10)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), DisconnectFunc());
			return passes_per_sec;
		}
		else if (*pSec < -10)
		{
			return 0;
		}

		return passes_per_sec;
	}
	else if (*pSec == 0)
	{
		const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_set_desc.begin(), c_set_desc.end(), SendDisconnectFunc());
		g_bNoMoreClient = true;
		--*pSec;
		return passes_per_sec;
	}
	else
	{
		char buf[64];
		snprintf(buf, sizeof(buf), LC_TEXT("%d seconds until Exit."), *pSec);
		SendNotice(buf);

		--*pSec;
		return passes_per_sec;
	}
}

void Shutdown(int iSec)
{
	if (g_bNoMoreClient)
	{
		thecore_shutdown();
		return;
	}

	CWarMapManager::instance().OnShutdown();

	char buf[64];
	snprintf(buf, sizeof(buf), LC_TEXT("The game will be closed in %d seconds."), iSec);

	SendNotice(buf);

	shutdown_event_data* info = AllocEventInfo<shutdown_event_data>();
	info->seconds = iSec;

	event_create(shutdown_event, info, 1);
}

ACMD(do_shutdown)
{
	if (NULL == ch)
	{
		sys_err("Accept shutdown command from %s.", ch->GetName());
	}

	TPacketGGShutdown p;
	p.bHeader = HEADER_GG_SHUTDOWN;
	P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGShutdown));

	Shutdown(10);
}

EVENTFUNC(timed_event)
{
	TimedEventInfo * info = dynamic_cast<TimedEventInfo *>( event->info );

	if ( info == NULL )
	{
		sys_err( "timed_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;
	if (ch == NULL)   // <Factor>
	{
		return 0;
	}
	LPDESC d = ch->GetDesc();

	if (info->left_second <= 0)
	{
		ch->m_pkTimedEvent = NULL;

		switch (info->subcmd)
		{
		case SCMD_LOGOUT:
		case SCMD_QUIT:
		case SCMD_PHASE_SELECT:
		{
			TPacketNeedLoginLogInfo acc_info;
			acc_info.dwPlayerID = ch->GetDesc()->GetAccountTable().id;
			db_clientdesc->DBPacket( HEADER_GD_VALID_LOGOUT, 0, &acc_info, sizeof(acc_info) );

			LogManager::instance().DetailLoginLog( false, ch );
		}
		break;
		}

		switch (info->subcmd)
		{
		case SCMD_LOGOUT:
			if (d)
			{
				d->SetPhase(PHASE_CLOSE);
			}
			break;

		case SCMD_QUIT:
			ch->ChatPacket(CHAT_TYPE_COMMAND, "quit");
			if (d)
			{
				d->DelayedDisconnect(1);
			}
			break;

		case SCMD_PHASE_SELECT:
		{
			ch->Disconnect("timed_event - SCMD_PHASE_SELECT");

			if (d)
			{
				d->SetPhase(PHASE_SELECT);
			}
		}
		break;
		}

		return 0;
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d seconds until Exit."), info->left_second);
		--info->left_second;
	}

	return PASSES_PER_SEC(1);
}

ACMD(do_cmd)
{


	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your logout has been cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

#ifdef __ENABLE_CHANGE_CHANNEL__
	if (ch->QuitSwitchChannelEvent())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The channel switching were cancelled."));
		return;
	}
#endif

	switch (subcmd)
	{
	case SCMD_LOGOUT:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Back to login window. Please wait."));
		break;

	case SCMD_QUIT:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have been disconnected from the server. Please wait."));
		break;

	case SCMD_PHASE_SELECT:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are changing character. Please wait."));
		break;
	}

	int nExitLimitTime = 10;

	if (ch->IsHack(false, true, nExitLimitTime) &&
			false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()) &&
			(!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		return;
	}

	switch (subcmd)
	{
	case SCMD_LOGOUT:
	case SCMD_QUIT:
	case SCMD_PHASE_SELECT:
	{
		TimedEventInfo* info = AllocEventInfo<TimedEventInfo>();

		{
			if (ch->IsPosition(POS_FIGHTING))
			{
				info->left_second = 10;
			}
			else
			{
				info->left_second = 3;
			}
		}

		info->ch		= ch;
		info->subcmd		= subcmd;
		strlcpy(info->szReason, argument, sizeof(info->szReason));

		ch->m_pkTimedEvent	= event_create(timed_event, info, 1);
	}
	break;
	}
}

ACMD(do_mount)
{

}

ACMD(do_fishing)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	ch->SetRotation(atof(arg1));
	ch->fishing();
}

ACMD(do_console)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
}

ACMD(do_restart)
{
	if (false == ch->IsDead())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");
		ch->StartRecoveryEvent();
		return;
	}

	if (NULL == ch->m_pkDeadEvent)
	{
		return;
	}

	int iTimeToDead = (event_time(ch->m_pkDeadEvent) / passes_per_sec);

	if (subcmd != SCMD_RESTART_TOWN && (!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG))
	{
		if (!test_server)
		{
			if (ch->IsHack())
			{

				if (false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A new start is not possible at the moment. Please wait %d seconds."), iTimeToDead - (180 - g_nPortalLimitTime));
					return;
				}
			}
#define eFRS_HERESEC	590
			if (iTimeToDead > eFRS_HERESEC)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A new start is not possible at the moment. Please wait %d seconds."), iTimeToDead - eFRS_HERESEC);
				return;
			}
		}
	}

	//PREVENT_HACK


	if (subcmd == SCMD_RESTART_TOWN)
	{
		if (ch->GetDungeon())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this in dungeon."));
			return;
		}

		if (ch->IsHack())
		{

			if ((!ch->GetWarMap() || ch->GetWarMap()->GetType() == GUILD_WAR_TYPE_FLAG) ||
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A new start is not possible at the moment. Please wait %d seconds."), iTimeToDead - (600 - g_nPortalLimitTime));
				return;
			}
		}

#define eFRS_TOWNSEC	593
		if (iTimeToDead > eFRS_TOWNSEC)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot restart in the city yet. Wait another %d seconds."), iTimeToDead - eFRS_TOWNSEC);
			return;
		}
	}
	//END_PREVENT_HACK

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CloseRestartWindow");

	ch->GetDesc()->SetPhase(PHASE_GAME);
	ch->SetPosition(POS_STANDING);

	ch->StartRecoveryEvent();
#ifdef __ADMIN_MANAGER__
	CAdminManager::instance().OnPlayerStateChange(ch);
#endif

	//FORKED_LOAD

	if (1 == quest::CQuestManager::instance().GetEventFlag("threeway_war"))
	{
		if (subcmd == SCMD_RESTART_TOWN || subcmd == SCMD_RESTART_HERE)
		{
			if (true == CThreeWayWar::instance().IsThreeWayWarMapIndex(ch->GetMapIndex()) &&
					false == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));

				ch->ReviveInvisible(5);
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());

				return;
			}


			if (true == CThreeWayWar::instance().IsSungZiMapIndex(ch->GetMapIndex()))
			{
				if (CThreeWayWar::instance().GetReviveTokenForPlayer(ch->GetPlayerID()) <= 0)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The waiting time has expired. You will be revived in the city."));
					ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
				}
				else
				{
					ch->Show(ch->GetMapIndex(), GetSungziStartX(ch->GetEmpire()), GetSungziStartY(ch->GetEmpire()));
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);

				return;
			}
		}
	}
	//END_FORKED_LOAD

	if (ch->GetDungeon())
	{
		ch->GetDungeon()->UseRevive(ch);
	}

	if (ch->GetWarMap() && !ch->IsObserverMode())
	{
		CWarMap * pMap = ch->GetWarMap();
		DWORD dwGuildOpponent = pMap ? pMap->GetGuildOpponent(ch) : 0;

		if (dwGuildOpponent)
		{
			switch (subcmd)
			{
			case SCMD_RESTART_TOWN:
			{
				sys_log(0, "do_restart: restart town");
				PIXEL_POSITION pos;

				if (CWarMapManager::instance().GetStartPosition(ch->GetMapIndex(), ch->GetGuild()->GetID() < dwGuildOpponent ? 0 : 1, pos))
				{
					ch->Show(ch->GetMapIndex(), pos.x, pos.y);
				}
				else
				{
					ch->ExitToSavedLocation();
				}

				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
			}
			break;

			case SCMD_RESTART_HERE:
			{
				sys_log(0, "do_restart: restart here");
				ch->RestartAtSamePos();
				//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
				ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
				ch->PointChange(POINT_SP, ch->GetMaxSP() - ch->GetSP());
				ch->ReviveInvisible(5);
			}
			break;
			}

			return;
		}
	}
	switch (subcmd)
	{
	case SCMD_RESTART_TOWN:
	{
		sys_log(0, "do_restart: restart town");
		PIXEL_POSITION pos;

		if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos))
		{
			ch->WarpSet(pos.x, pos.y);
		}
		else
		{
			ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
		}
#ifdef __ENABLE_FULL_HP_AFTER_SPAWN__
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
#else
		ch->PointChange(POINT_HP, 50 - ch->GetHP());
#endif
		ch->DeathPenalty(1);
	}
	break;

	case SCMD_RESTART_HERE:
	{
		sys_log(0, "do_restart: restart here");
		ch->RestartAtSamePos();
		//ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY());
#ifdef __ENABLE_FULL_HP_AFTER_SPAWN__
		ch->PointChange(POINT_HP, ch->GetMaxHP() - ch->GetHP());
#else
		ch->PointChange(POINT_HP, 50 - ch->GetHP());
#endif
		ch->DeathPenalty(0);
		ch->ReviveInvisible(5);
	}
	break;
	}
}

#define MAX_STAT g_iStatusPointSetMaxValue

ACMD(do_stat_reset)
{
	ch->PointChange(POINT_STAT_RESET_COUNT, 12 - ch->GetPoint(POINT_STAT_RESET_COUNT));
}

ACMD(do_stat_minus)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT_RESET_COUNT) <= 0)
	{
		return;
	}

	if (!strcmp(arg1, "st"))
	{
		if (ch->GetRealPoint(POINT_ST) <= JobInitialPoints[ch->GetJob()].st)
		{
			return;
		}

		ch->SetRealPoint(POINT_ST, ch->GetRealPoint(POINT_ST) - 1);
		ch->SetPoint(POINT_ST, ch->GetPoint(POINT_ST) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_ST, 0);
	}
	else if (!strcmp(arg1, "dx"))
	{
		if (ch->GetRealPoint(POINT_DX) <= JobInitialPoints[ch->GetJob()].dx)
		{
			return;
		}

		ch->SetRealPoint(POINT_DX, ch->GetRealPoint(POINT_DX) - 1);
		ch->SetPoint(POINT_DX, ch->GetPoint(POINT_DX) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_DX, 0);
	}
	else if (!strcmp(arg1, "ht"))
	{
		if (ch->GetRealPoint(POINT_HT) <= JobInitialPoints[ch->GetJob()].ht)
		{
			return;
		}

		ch->SetRealPoint(POINT_HT, ch->GetRealPoint(POINT_HT) - 1);
		ch->SetPoint(POINT_HT, ch->GetPoint(POINT_HT) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_HT, 0);
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (!strcmp(arg1, "iq"))
	{
		if (ch->GetRealPoint(POINT_IQ) <= JobInitialPoints[ch->GetJob()].iq)
		{
			return;
		}

		ch->SetRealPoint(POINT_IQ, ch->GetRealPoint(POINT_IQ) - 1);
		ch->SetPoint(POINT_IQ, ch->GetPoint(POINT_IQ) - 1);
		ch->ComputePoints();
		ch->PointChange(POINT_IQ, 0);
		ch->PointChange(POINT_MAX_SP, 0);
	}
	else
	{
		return;
	}

	ch->PointChange(POINT_STAT, +1);
	ch->PointChange(POINT_STAT_RESET_COUNT, -1);
	ch->ComputePoints();
}

ACMD(do_stat)
{
	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		return;
	}

	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
		return;
	}

	if (ch->GetPoint(POINT_STAT) <= 0)
	{
		return;
	}

	BYTE idx = 0;

	if (!strcmp(arg1, "st"))
	{
		idx = POINT_ST;
	}
	else if (!strcmp(arg1, "dx"))
	{
		idx = POINT_DX;
	}
	else if (!strcmp(arg1, "ht"))
	{
		idx = POINT_HT;
	}
	else if (!strcmp(arg1, "iq"))
	{
		idx = POINT_IQ;
	}
	else
	{
		return;
	}

	int count;
	if (!str_to_number(count, arg2) || count < 1)
	{
		count = 1;
	}

	int maxCount = MIN(MAX_STAT - ch->GetRealPoint(idx), ch->GetPoint(POINT_STAT));
	if (maxCount <= 0)
	{
		return;
	}

	if (count > maxCount)
	{
		count = maxCount;
	}

	ch->SetRealPoint(idx, ch->GetRealPoint(idx) + count);
	ch->SetPoint(idx, ch->GetPoint(idx) + count);
	ch->ComputePoints();
	ch->PointChange(idx, 0);

	if (idx == POINT_IQ)
	{
		ch->PointChange(POINT_MAX_HP, 0);
	}
	else if (idx == POINT_HT)
	{
		ch->PointChange(POINT_MAX_SP, 0);
	}

	ch->PointChange(POINT_STAT, -count);
	ch->ComputePoints();
}

ACMD(do_pvp)
{
	if (ch->GetArena() != NULL || CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER pkVictim = CHARACTER_MANAGER::instance().Find(vid);

	if (!pkVictim)
	{
		return;
	}

	if (pkVictim->IsNPC())
	{
		return;
	}

	if (pkVictim->GetArena() != NULL)
	{
		pkVictim->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This player is currently fighting."));
		return;
	}

	CPVPManager::instance().Insert(ch, pkVictim);
}

ACMD(do_guildskillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (!ch->GetGuild())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] It does not belong to the guild."));
		return;
	}

	CGuild* g = ch->GetGuild();
	TGuildMember* gm = g->GetMember(ch->GetPlayerID());
	if (gm->grade == GUILD_LEADER_GRADE)
	{
		DWORD vnum = 0;
		str_to_number(vnum, arg1);
		g->SkillLevelUp(vnum);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to change the level of the guild skills."));
	}
}

ACMD(do_skillup)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD vnum = 0;
	str_to_number(vnum, arg1);

	if (true == ch->CanUseSkill(vnum))
	{
		ch->SkillLevelUp(vnum);
	}
	else
	{
		switch (vnum)
		{
		case SKILL_HORSE_WILDATTACK:
		case SKILL_HORSE_CHARGE:
		case SKILL_HORSE_ESCAPE:
		case SKILL_HORSE_WILDATTACK_RANGE:

		case SKILL_7_A_ANTI_TANHWAN:
		case SKILL_7_B_ANTI_AMSEOP:
		case SKILL_7_C_ANTI_SWAERYUNG:
		case SKILL_7_D_ANTI_YONGBI:

		case SKILL_8_A_ANTI_GIGONGCHAM:
		case SKILL_8_B_ANTI_YEONSA:
		case SKILL_8_C_ANTI_MAHWAN:
		case SKILL_8_D_ANTI_BYEURAK:

		case SKILL_ADD_HP:
		case SKILL_RESIST_PENETRATE:
			ch->SkillLevelUp(vnum);
			break;
		}
	}
}

//
//
ACMD(do_safebox_close)
{
	ch->CloseSafebox();
}

//
//
ACMD(do_safebox_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));
	ch->ReqSafeboxLoad(arg1);
}

ACMD(do_safebox_change_password)
{
	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	if (!*arg2 || strlen(arg2) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	TSafeboxChangePasswordPacket p;

	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szOldPassword, arg1, sizeof(p.szOldPassword));
	strlcpy(p.szNewPassword, arg2, sizeof(p.szNewPassword));

	db_clientdesc->DBPacket(HEADER_GD_SAFEBOX_CHANGE_PASSWORD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_password)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || strlen(arg1) > 6)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have entered an incorrect password."));
		return;
	}

	int iPulse = thecore_pulse();

	if (ch->GetMall())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The Storeroom is already open."));
		return;
	}

	if (iPulse - ch->GetMallLoadTime() < passes_per_sec * 10)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] You have to wait 10 seconds before you can open the Storeroom again."));
		return;
	}

	ch->SetMallLoadTime(iPulse);

	TSafeboxLoadPacket p;
	p.dwID = ch->GetDesc()->GetAccountTable().id;
	strlcpy(p.szLogin, ch->GetDesc()->GetAccountTable().login, sizeof(p.szLogin));
	strlcpy(p.szPassword, arg1, sizeof(p.szPassword));

	db_clientdesc->DBPacket(HEADER_GD_MALL_LOAD, ch->GetDesc()->GetHandle(), &p, sizeof(p));
}

ACMD(do_mall_close)
{
	if (ch->GetMall())
	{
		ch->SetMallLoadTime(thecore_pulse());
		ch->CloseMall();
		ch->Save();
	}
}

ACMD(do_ungroup)
{
	if (!ch->GetParty())
	{
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot leave a group while you are in a dungeon."));
		return;
	}

	LPPARTY pParty = ch->GetParty();

	if (pParty->GetMemberCount() == 2)
	{
		// party disband
		CPartyManager::instance().DeleteParty(pParty);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You have left the group."));
		//pParty->SendPartyRemoveOneToAll(ch);
		pParty->Quit(ch->GetPlayerID());
		//pParty->SendPartyRemoveAllToOne(ch);
	}
}

ACMD(do_close_shop)
{
	if (ch->GetMyShop())
	{
		ch->CloseMyShop();
		return;
	}
}

ACMD(do_set_walk_mode)
{
	ch->SetNowWalking(true);
	ch->SetWalking(true);
}

ACMD(do_set_run_mode)
{
	ch->SetNowWalking(false);
	ch->SetWalking(false);
}

ACMD(do_war)
{

	CGuild * g = ch->GetGuild();

	if (!g)
	{
		return;
	}


	if (g->UnderAnyWar())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Your guild is already participating in another war."));
		return;
	}


	char arg1[256], arg2[256];
	DWORD type = GUILD_WAR_TYPE_FIELD; //fixme102 base int modded uint
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		return;
	}

	if (*arg2)
	{
		str_to_number(type, arg2);

		if (type >= GUILD_WAR_TYPE_MAX_NUM)
		{
			type = GUILD_WAR_TYPE_FIELD;
		}

		//We not gonna let to make a crash
		if (type < 0)
		{
			return;
		}
	}


	DWORD gm_pid = g->GetMasterPID();


	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No one is entitled to a guild war."));
		return;
	}


	CGuild * opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No guild with this name exists."));
		return;
	}


	switch (g->GetGuildWarState(opp_g->GetID()))
	{
	case GUILD_WAR_NONE:
	{
		if (opp_g->UnderAnyWar())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in another war."));
			return;
		}

		int iWarPrice = KOR_aGuildWarInfo[type].iWarPrice;

		if (g->GetGuildMoney() < iWarPrice)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Not enough Yang to participate in a guild war."));
			return;
		}

		if (opp_g->GetGuildMoney() < iWarPrice)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough Yang to participate in a guild war."));
			return;
		}
	}
	break;

	case GUILD_WAR_SEND_DECLARE:
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in a war."));
		return;
	}
	break;

	case GUILD_WAR_RECV_DECLARE:
	{
		if (opp_g->UnderAnyWar())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is already participating in another war."));
			g->RequestRefuseWar(opp_g->GetID());
			return;
		}
	}
	break;

	case GUILD_WAR_RESERVE:
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This Guild is already scheduled for another war."));
		return;
	}
	break;

	case GUILD_WAR_END:
		return;

	default:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This guild is taking part in a battle at the moment."));
		g->RequestRefuseWar(opp_g->GetID());
		return;
	}

	if (!g->CanStartWar(type))
	{

		if (g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild level is too low."));
			sys_log(0, "GuildWar.StartError.NEED_LADDER_POINT");
		}
		else if (g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] A minimum of %d players are needed to participate in a guild war."), GUILD_WAR_MIN_MEMBER_COUNT);
			sys_log(0, "GuildWar.StartError.NEED_MINIMUM_MEMBER[%d]", GUILD_WAR_MIN_MEMBER_COUNT);
		}
		else
		{
			sys_log(0, "GuildWar.StartError.UNKNOWN_ERROR");
		}
		return;
	}


	if (!opp_g->CanStartWar(GUILD_WAR_TYPE_FIELD))
	{
		if (opp_g->GetLadderPoint() == 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough points to participate in a guild war."));
		}
		else if (opp_g->GetMemberCount() < GUILD_WAR_MIN_MEMBER_COUNT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild does not have enough members to participate in a guild war."));
		}
		return;
	}

	do
	{
		if (g->GetMasterCharacter() != NULL)
		{
			break;
		}

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(g->GetMasterPID());

		if (pCCI != NULL)
		{
			break;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The enemy's guild leader is offline."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	do
	{
		if (opp_g->GetMasterCharacter() != NULL)
		{
			break;
		}

		CCI *pCCI = P2P_MANAGER::instance().FindByPID(opp_g->GetMasterPID());

		if (pCCI != NULL)
		{
			break;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The enemy's guild leader is offline."));
		g->RequestRefuseWar(opp_g->GetID());
		return;

	} while (false);

	g->RequestDeclareWar(opp_g->GetID(), type);
}

ACMD(do_nowar)
{
	CGuild* g = ch->GetGuild();
	if (!g)
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD gm_pid = g->GetMasterPID();

	if (gm_pid != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No one is entitled to a guild war."));
		return;
	}

	CGuild* opp_g = CGuildManager::instance().FindGuildByName(arg1);

	if (!opp_g)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] No guild with this name exists."));
		return;
	}

	g->RequestRefuseWar(opp_g->GetID());
}

ACMD(do_detaillog)
{
	ch->DetailLog();
}

ACMD(do_monsterlog)
{
	ch->ToggleMonsterLog();
}

ACMD(do_pkmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE mode = 0;
	str_to_number(mode, arg1);

	if (mode == PK_MODE_PROTECT)
	{
		return;
	}

	if (ch->GetLevel() < PK_PROTECT_LEVEL && mode != 0)
	{
		return;
	}

	ch->SetPKMode(mode);
}

ACMD(do_messenger_auth)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	char answer = LOWER(*arg1);
	// @fixme130 AuthToAdd void -> bool
	bool bIsDenied = answer != 'y';
	bool bIsAdded = MessengerManager::instance().AuthToAdd(ch->GetName(), arg2, bIsDenied); // DENY
	if (bIsAdded && bIsDenied)
	{
		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(arg2);

		if (tch)
		{
			tch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s declined the invitation."), ch->GetName());
		}
	}

}

ACMD(do_setblockmode)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		BYTE flag = 0;
		str_to_number(flag, arg1);
		ch->SetBlockMode(flag);
	}
}

ACMD(do_unmount)
{
	if (true == ch->UnEquipSpecialRideUniqueItem())
	{
		ch->RemoveAffect(AFFECT_MOUNT);
		ch->RemoveAffect(AFFECT_MOUNT_BONUS);

		if (ch->IsHorseRiding())
		{
			ch->StopRiding();
		}
	}
	else
	{
		ch->ChatPacket( CHAT_TYPE_INFO, LC_TEXT("Your inventory is full."));
	}

}

ACMD(do_observer_exit)
{
	if (ch->IsObserverMode())
	{
		if (ch->GetWarMap())
		{
			ch->SetWarMap(NULL);
		}

		if (ch->GetArena() != NULL || ch->GetArenaObserverMode() == true)
		{
			ch->SetArenaObserverMode(false);

			if (ch->GetArena() != NULL)
			{
				ch->GetArena()->RemoveObserver(ch->GetPlayerID());
			}

			ch->SetArena(NULL);
			ch->WarpSet(ARENA_RETURN_POINT_X(ch->GetEmpire()), ARENA_RETURN_POINT_Y(ch->GetEmpire()));
		}
		else
		{
			ch->ExitToSavedLocation();
		}
		ch->SetObserverMode(false);
	}
}

ACMD(do_view_equip)
{
	if (ch->GetGMLevel() <= GM_PLAYER)
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (*arg1)
	{
		DWORD vid = 0;
		str_to_number(vid, arg1);
		LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

		if (!tch)
		{
			return;
		}

		if (!tch->IsPC())
		{
			return;
		}

		tch->SendEquipment(ch);
	}
}

ACMD(do_party_request)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	if (ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot accept the invitation because you are already in the group."));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
		if (!ch->RequestToParty(tch))
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "PartyRequestDenied");
		}
}

ACMD(do_party_request_accept)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
	{
		ch->AcceptToParty(tch);
	}
}

ACMD(do_party_request_deny)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD vid = 0;
	str_to_number(vid, arg1);
	LPCHARACTER tch = CHARACTER_MANAGER::instance().Find(vid);

	if (tch)
	{
		ch->DenyToParty(tch);
	}
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

static const char* FN_point_string(int apply_number)
{
	switch (apply_number)
	{
	case POINT_MAX_HP:
		return LC_TEXT("Hit Points +%d");
	case POINT_MAX_SP:
		return LC_TEXT("Spell Points +%d");
	case POINT_HT:
		return LC_TEXT("Endurance +%d");
	case POINT_IQ:
		return LC_TEXT("Intelligence +%d");
	case POINT_ST:
		return LC_TEXT("Strength +%d");
	case POINT_DX:
		return LC_TEXT("Dexterity +%d");
	case POINT_ATT_SPEED:
		return LC_TEXT("Attack Speed +%d");
	case POINT_MOV_SPEED:
		return LC_TEXT("Movement Speed %d");
	case POINT_CASTING_SPEED:
		return LC_TEXT("Cooldown Time -%d");
	case POINT_HP_REGEN:
		return LC_TEXT("Energy Recovery +%d");
	case POINT_SP_REGEN:
		return LC_TEXT("Spell Point Recovery +%d");
	case POINT_POISON_PCT:
		return LC_TEXT("Poison Attack %d");
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_BLEEDING_PCT:
		return LC_TEXT("Poison Attack %d");
#endif
	case POINT_STUN_PCT:
		return LC_TEXT("Star +%d");
	case POINT_SLOW_PCT:
		return LC_TEXT("Speed Reduction +%d");
	case POINT_CRITICAL_PCT:
		return LC_TEXT("Critical Attack with a chance of %d%%");
	case POINT_RESIST_CRITICAL:
		return LC_TEXT("상대의 치명타 확률 %d%% 감소");
	case POINT_PENETRATE_PCT:
		return LC_TEXT("Chance of a Speared Attack of %d%%");
	case POINT_RESIST_PENETRATE:
		return LC_TEXT("상대의 관통 공격 확률 %d%% 감소");
	case POINT_ATTBONUS_HUMAN:
		return LC_TEXT("Player's Attack Power against Monsters +%d%%");
	case POINT_ATTBONUS_ANIMAL:
		return LC_TEXT("Horse's Attack Power against Monsters +%d%%");
	case POINT_ATTBONUS_ORC:
		return LC_TEXT("Attack Boost against Wonggui + %d%%");
	case POINT_ATTBONUS_MILGYO:
		return LC_TEXT("Attack Boost against Milgyo + %d%%");
	case POINT_ATTBONUS_UNDEAD:
		return LC_TEXT("Attack boost against zombies + %d%%");
	case POINT_ATTBONUS_DEVIL:
		return LC_TEXT("Attack boost against devils + %d%%");
	case POINT_STEAL_HP:
		return LC_TEXT("Absorbing of Energy %d%% while attacking.");
	case POINT_STEAL_SP:
		return LC_TEXT("Absorption of Spell Points (SP) %d%% while attacking.");
	case POINT_MANA_BURN_PCT:
		return LC_TEXT("With a chance of %d%% Spell Points (SP) will be taken from the enemy.");
	case POINT_DAMAGE_SP_RECOVER:
		return LC_TEXT("Absorbing of Spell Points (SP) with a chance of %d%%");
	case POINT_BLOCK:
		return LC_TEXT("%d%% Chance of blocking a close-combat attack");
	case POINT_DODGE:
		return LC_TEXT("%d%% Chance of blocking a long range attack");
	case POINT_RESIST_SWORD:
		return LC_TEXT("One-Handed Sword defence %d%%");
	case POINT_RESIST_TWOHAND:
		return LC_TEXT("Two-Handed Sword Defence %d%%");
	case POINT_RESIST_DAGGER:
		return LC_TEXT("Two-Handed Sword Defence %d%%");
	case POINT_RESIST_BELL:
		return LC_TEXT("Bell Defence %d%%");
	case POINT_RESIST_FAN:
		return LC_TEXT("Fan Defence %d%%");
	case POINT_RESIST_BOW:
		return LC_TEXT("Distant Attack Resistance %d%%");
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_RESIST_CLAW:
		return LC_TEXT("Two-Handed Sword Defence %d%%");
#endif
	case POINT_RESIST_FIRE:
		return LC_TEXT("Fire Resistance %d%%");
	case POINT_RESIST_ELEC:
		return LC_TEXT("Lightning Resistance %d%%");
	case POINT_RESIST_MAGIC:
		return LC_TEXT("Magic Resistance %d%%");
#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	case POINT_RESIST_MAGIC_REDUCTION:
		return LC_TEXT("Magic Resistance %d%%");
#endif
	case POINT_RESIST_WIND:
		return LC_TEXT("Wind Resistance %d%%");
	case POINT_RESIST_ICE:
		return LC_TEXT("냉기 저항 %d%%");
	case POINT_RESIST_EARTH:
		return LC_TEXT("대지 저항 %d%%");
	case POINT_RESIST_DARK:
		return LC_TEXT("어둠 저항 %d%%");
	case POINT_REFLECT_MELEE:
		return LC_TEXT("Reflect Direct Hit: %d%%");
	case POINT_REFLECT_CURSE:
		return LC_TEXT("Reflect Curse: %d%%");
	case POINT_POISON_REDUCE:
		return LC_TEXT("Poison Resistance %d%%");
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_BLEEDING_REDUCE:
		return LC_TEXT("Poison Resistance %d%%");
#endif
	case POINT_KILL_SP_RECOVER:
		return LC_TEXT("Spell Points (SP) will be increased by %d%% if you win.");
	case POINT_EXP_DOUBLE_BONUS:
		return LC_TEXT("Experience increases by %d%% if you win against an opponent.");
	case POINT_GOLD_DOUBLE_BONUS:
		return LC_TEXT("Increase of Yang up to %d%% if you win.");
	case POINT_ITEM_DROP_BONUS:
		return LC_TEXT("Increase of captured Items up to %d%% if you win.");
	case POINT_POTION_BONUS:
		return LC_TEXT("Power increase of up to %d%% after taking the potion.");
	case POINT_KILL_HP_RECOVERY:
		return LC_TEXT("%d%% Chance of filling up Hit Points after a victory.");
//		case POINT_SKILL:	return LC_TEXT("");
//		case POINT_BOW_DISTANCE:	return LC_TEXT("");
	case POINT_ATT_GRADE_BONUS:
		return LC_TEXT("Attack Power + %d");
	case POINT_DEF_GRADE_BONUS:
		return LC_TEXT("Armour + %d");
	case POINT_MAGIC_ATT_GRADE:
		return LC_TEXT("Magical Attack + %d");
	case POINT_MAGIC_DEF_GRADE:
		return LC_TEXT("Magical Defence + %d");
//		case POINT_CURSE_PCT:	return LC_TEXT("");
	case POINT_MAX_STAMINA:
		return LC_TEXT("Maximum Endurance + %d");
	case POINT_ATTBONUS_WARRIOR:
		return LC_TEXT("Strong against Warriors + %d%%");
	case POINT_ATTBONUS_ASSASSIN:
		return LC_TEXT("Strong against Ninjas + %d%%");
	case POINT_ATTBONUS_SURA:
		return LC_TEXT("Strong against Sura + %d%%");
	case POINT_ATTBONUS_SHAMAN:
		return LC_TEXT("Strong against Shamans + %d%%");
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_ATTBONUS_WOLFMAN:
		return LC_TEXT("Strong against Shamans + %d%%");
#endif
	case POINT_ATTBONUS_MONSTER:
		return LC_TEXT("Strength against monsters + %d%%");
	case POINT_MALL_ATTBONUS:
		return LC_TEXT("Attack + %d%%");
	case POINT_MALL_DEFBONUS:
		return LC_TEXT("Defence + %d%%");
	case POINT_MALL_EXPBONUS:
		return LC_TEXT("Experience %d%%");
	case POINT_MALL_ITEMBONUS:
		return LC_TEXT("Chance to find an Item %. 1f");
	case POINT_MALL_GOLDBONUS:
		return LC_TEXT("Chance to find Yang %. 1f");
	case POINT_MAX_HP_PCT:
		return LC_TEXT("Maximum Energy +%d%%");
	case POINT_MAX_SP_PCT:
		return LC_TEXT("Maximum Energy +%d%%");
	case POINT_SKILL_DAMAGE_BONUS:
		return LC_TEXT("Skill Damage %d%%");
	case POINT_NORMAL_HIT_DAMAGE_BONUS:
		return LC_TEXT("Hit Damage %d%%");
	case POINT_SKILL_DEFEND_BONUS:
		return LC_TEXT("Resistance against Skill Damage %d%%");
	case POINT_NORMAL_HIT_DEFEND_BONUS:
		return LC_TEXT("Resistance against Hits %d%%");
//		case POINT_PC_BANG_EXP_BONUS:	return LC_TEXT("");
//		case POINT_PC_BANG_DROP_BONUS:	return LC_TEXT("");
//		case POINT_EXTRACT_HP_PCT:	return LC_TEXT("");
	case POINT_RESIST_WARRIOR:
		return LC_TEXT("%d%% Resistance against Warrior Attacks");
	case POINT_RESIST_ASSASSIN:
		return LC_TEXT("%d%% Resistance against Ninja Attacks");
	case POINT_RESIST_SURA:
		return LC_TEXT("%d%% Resistance against Sura Attacks");
	case POINT_RESIST_SHAMAN:
		return LC_TEXT("%d%% Resistance against Shaman Attacks");
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case POINT_RESIST_WOLFMAN:
		return LC_TEXT("%d%% Resistance against Shaman Attacks");
#endif
#ifdef __ENABLE_12ZI_ELEMENT_ADD__
	case POINT_ATTBONUS_ELEC:
		return LC_TEXT("Poder del relampago %d%%");
	case POINT_ATTBONUS_FIRE:
		return LC_TEXT("Poder del fuego %d%%");
	case POINT_ATTBONUS_ICE:
		return LC_TEXT("Poder del hielo %d%%");
	case POINT_ATTBONUS_WIND:
		return LC_TEXT("Poder del viento %d%%");
	case POINT_ATTBONUS_EARTH:
		return LC_TEXT("Poder de la tierra %d%%");
	case POINT_ATTBONUS_DARK:
		return LC_TEXT("Poder de oscuridad %d%%");
	case POINT_ATTBONUS_INSECT:
		return LC_TEXT("Fuerza contra insectos %d%%");
	case POINT_ATTBONUS_DESERT:
		return LC_TEXT("Fuerza contra monstruos del desierto %d%%");
	case POINT_ATTBONUS_CZ:
		return LC_TEXT("Fuerza contra monstruos del zodiaco %d%%");
#endif
	default:
		return NULL;
	}
}

static bool FN_hair_affect_string(LPCHARACTER ch, char *buf, size_t bufsiz)
{
	if (NULL == ch || NULL == buf)
	{
		return false;
	}

	CAffect* aff = NULL;
	time_t expire = 0;
	struct tm ltm;
	int	year, mon, day;
	int	offset = 0;

	aff = ch->FindAffect(AFFECT_HAIR);

	if (NULL == aff)
	{
		return false;
	}

	expire = ch->GetQuestFlag("hair.limit_time");

	if (expire < get_global_time())
	{
		return false;
	}

	// set apply string
	offset = snprintf(buf, bufsiz, FN_point_string(aff->bApplyOn), aff->lApplyValue);

	if (offset < 0 || offset >= (int) bufsiz)
	{
		offset = bufsiz - 1;
	}

	localtime_r(&expire, &ltm);

	year	= ltm.tm_year + 1900;
	mon		= ltm.tm_mon + 1;
	day		= ltm.tm_mday;

	snprintf(buf + offset, bufsiz - offset, LC_TEXT("(Procedure: %d y- %d m - %d d)"), year, mon, day);

	return true;
}

ACMD(do_costume)
{
	char buf[768];

	const size_t bufferSize = sizeof(buf);

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	CItem* pBody = ch->GetWear(WEAR_COSTUME_BODY);
	CItem* pHair = ch->GetWear(WEAR_COSTUME_HAIR);
#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	CItem* pWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
#endif

	ch->ChatPacket(CHAT_TYPE_INFO, "COSTUME status:");

	if (pHair)
	{
		const char* itemName = pHair->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  HAIR : %s", itemName);

		for (int i = 0; i < pHair->GetAttributeCount(); ++i)
		{
			const TPlayerItemAttribute& attr = pHair->GetAttribute(i);
			if (0 < attr.bType)
			{
				snprintf(buf, bufferSize, FN_point_string(attr.bType), attr.sValue);
				ch->ChatPacket(CHAT_TYPE_INFO, "     %s", buf);
			}
		}

		if (pHair->IsEquipped() && arg1[0] == 'h')
		{
			ch->UnequipItem(pHair);
		}
	}

	if (pBody)
	{
		const char* itemName = pBody->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  BODY : %s", itemName);

		if (pBody->IsEquipped() && arg1[0] == 'b')
		{
			ch->UnequipItem(pBody);
		}
	}

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	if (pWeapon)
	{
		const char* itemName = pWeapon->GetName();
		ch->ChatPacket(CHAT_TYPE_INFO, "  WEAPON : %s", itemName);

		if (pWeapon->IsEquipped() && arg1[0] == 'w')
		{
			ch->UnequipItem(pWeapon);
		}
	}
#endif
}

ACMD(do_hair)
{
	char buf[256];

	if (false == FN_hair_affect_string(ch, buf, sizeof(buf)))
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

ACMD(do_inventory)
{
	int	index = 0;
	int	count		= 1;

	char arg1[256];
	char arg2[256];

	LPITEM	item;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Usage: inventory <start_index> <count>");
		return;
	}

	if (!*arg2)
	{
		index = 0;
		str_to_number(count, arg1);
	}
	else
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		str_to_number(index, arg1);
		index = MIN(index, INVENTORY_MAX_NUM);
#else
		str_to_number(index, arg1);
		index = MIN(index, INVENTORY_AND_EQUIP_SLOT_MAX);
#endif
		str_to_number(count, arg2);
		count = MIN(count, g_bItemCountLimit);
	}

	for (int i = 0; i < count; ++i)
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		if (index >= INVENTORY_MAX_NUM)
		{
			break;
		}
#else
		if (index >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			break;
		}

		if (index >= INVENTORY_MAX_NUM && index < SPECIAL_STORAGE_START_CELL)
		{
			continue;
		}
#endif

		item = ch->GetInventoryItem(index);

		ch->ChatPacket(CHAT_TYPE_INFO, "inventory [%d] = %s",
					   index, item ? item->GetName() : "<NONE>");
		++index;
	}
}

//gift notify quest command
ACMD(do_gift)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "gift");
}

ACMD(do_in_game_mall)
{
	char buf[512 + 1];
	char sas[33];
	MD5_CTX ctx;
	const char sas_key[] = "GF9001";

	char language[3];
	strcpy(language, "pl");//If you have multilanguage, update this

	snprintf(buf, sizeof(buf), "%u%u%s", ch->GetPlayerID(), ch->GetAID(), sas_key);

	MD5Init(&ctx);
	MD5Update(&ctx, (const unsigned char*)buf, strlen(buf));
#ifdef __FreeBSD__
	MD5End(&ctx, sas);
#else
	static const char hex[] = "0123456789abcdef";
	unsigned char digest[16];
	MD5Final(digest, &ctx);
	int i;
	for (i = 0; i < 16; ++i)
	{
		sas[i + i] = hex[digest[i] >> 4];
		sas[i + i + 1] = hex[digest[i] & 0x0f];
	}
	sas[i + i] = '\0';
#endif

	snprintf(buf, sizeof(buf), "mall https://%s/shop?pid=%u&lang=%s&sid=%d&sas=%s",
			 g_strWebMallURL.c_str(), ch->GetPlayerID(), language, g_server_id, sas);

	ch->ChatPacket(CHAT_TYPE_COMMAND, buf);
}


ACMD(do_dice)
{
	char arg1[256], arg2[256];
	int start = 1, end = 100;

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (*arg1 && *arg2)
	{
		start = atoi(arg1);
		end = atoi(arg2);
	}
	else if (*arg1 && !*arg2)
	{
		start = 1;
		end = atoi(arg1);
	}

	end = MAX(start, end);
	start = MIN(start, end);

	int n = number(start, end);

#ifdef __ENABLE_DICE_SYSTEM__
	if (ch->GetParty())
	{
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_DICE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_DICE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
	}
#else
	if (ch->GetParty())
	{
		ch->GetParty()->ChatPacketToAllMember(CHAT_TYPE_INFO, LC_TEXT("%s님이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), ch->GetName(), n, start, end);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("당신이 주사위를 굴려 %d가 나왔습니다. (%d-%d)"), n, start, end);
	}
#endif
}

#ifdef __ENABLE_NEWSTUFF__
ACMD(do_click_safebox)
{
	if ((ch->GetGMLevel() <= GM_PLAYER) && (ch->GetDungeon() || ch->GetWarMap()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the safebox in dungeon or at war."));
		return;
	}

	ch->SetSafeboxOpenPosition();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeSafeboxPassword");
}
ACMD(do_force_logout)
{
	LPDESC pDesc = DESC_MANAGER::instance().FindByCharacterName(ch->GetName());
	if (!pDesc)
	{
		return;
	}
	pDesc->DelayedDisconnect(0);
}
#endif

ACMD(do_click_mall)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ShowMeMallPassword");
}

ACMD(do_ride)
{
	if (ch->IsDead() || ch->IsStun())
	{
		return;
	}
	
	if (ch->IsRiding())
	{
		Unmount(ch);
		return;
	}
	
	if (ch->IsPolymorphed())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot ride your mount while being polymorphed."));
		return;
	}
	
	if (!Mount(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please call your Horse first."));
	}
}

#ifdef __SPECIAL_STORAGE_ENABLE__
ACMD(do_transfer_to_special_storage)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	WORD wSlot = atoi(arg1);
	if (wSlot >= INVENTORY_MAX_NUM)
	{
		return;
	}

	LPITEM item = ch->GetInventoryItem(wSlot);
	if (!item || item->GetVirtualWindow() == INVENTORY)
	{
		return;
	}

	int iEmptyCell = ch->GetEmptyInventory(item->GetSize(), item->GetVirtualWindow(), true);
	if (iEmptyCell > -1)
	{
		ch->MoveItem(TItemPos(INVENTORY, item->GetCell()), TItemPos(INVENTORY, iEmptyCell), item->GetCount());
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, ("SPECIAL_STORAGE_ITEM_DOES_NOT_BELONG"));
	}
}
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
ACMD(do_sort_inventory)
{
	if (!ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, ("SORT_INVENTORY_CANNOT_DO_ACTION"));
		return;
	}

	ch->SortInventory();
}
#endif

#ifdef __GAYA_SHOP_SYSTEM__
ACMD(do_open_gaya_shop)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_GAYA_SHOP"));
		return;
	}

	CGayaSystemManager::instance().AddGayaGuest(ch);
}

ACMD(do_craft_gaya_item)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_CRAFT_GAYA"));
		return;
	}

	switch (CGayaSystemManager::instance().CraftItem(ch, atoi(arg1)))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_INTERNAL_ERROR"));
		break;
	case -2:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_NOT_ENOUGH_MONEY"));
		break;
	case -3:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_NOT_VALID_ITEM"));
		break;
	case -4:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_NOT_VALID_ITEM_2"));
		break;
	case -5:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_FAIL"));
		break;
	default:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_CRAFTING_DEFAULT"));
		break;
	}
}

ACMD(do_purchase_gaya_item)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_PURCHASE_GAYA"));
		return;
	}

	switch (CGayaSystemManager::instance().PurchaseItem(ch, atoi(arg1)))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_PURCHASE_INTERNAL_ERROR"));
		break;
	case -2:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_PURCHASE_INTERNAL_ERROR"));
		break;
	case -3:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_PURCHASE_NOT_ENOUGH_MONEY"));
		break;
	default:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_PURCHASE_DEFAULT"));
		break;
	}
}

ACMD(do_unlock_gaya_slot)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_UNLOCK_GAYA"));
		return;
	}

	switch (CGayaSystemManager::instance().UnlockGayaSlot(ch, atoi(arg1), atoi(arg2)))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_UNLOCK_INTERNAL_ERROR"));
		break;
	case -2:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_UNLOCK_ALREADY_DONE"));
		break;
	case -3:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_UNLOCK_MISSING_ITEM"));
		break;
	default:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_UNLOCK_DEFAULT"));
		break;
	}
}

ACMD(do_request_gaya_rotation)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_REQUEST_GAYA_ROTATION"));
		return;
	}

	switch (CGayaSystemManager::instance().RequestGayaRotation(ch))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_ROTATION_INTERNAL_ERROR"));
		break;
	case -2:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_ROTATION_MISSING_ITEM"));
		break;
	default:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("GAYA_ROTATION_DONE"));
		break;
	}
}
#endif

#ifdef __ENABLE_ANTY_EXP__
ACMD(do_block_exp)
{
	if (!ch)
	{
		return;
	}

	if (ch->GetLevel() < 30)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot block exp."));
		return;
	}

	if (ch->FindAffect(AFFECT_EXP_CURSE))
	{
		ch->RemoveAffect(AFFECT_EXP_CURSE);
	}
	else
	{
		ch->AddAffect(AFFECT_EXP_CURSE, POINT_NONE, 0, AFF_NONE, INFINITE_AFFECT_DURATION, 0, true);
	}
}
#endif


#ifdef __ENABLE_REMOVE_SKILLS_AFFECT__
ACMD(do_remove_affect_player)
{
	if (!ch) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	uint32_t affectType = 0;
	str_to_number(affectType, arg1);

	if (affectType == AFFECT_DS_SET) return;

	if (!ch->IsGoodAffect(affectType)) return;

	ch->RemoveAffect(affectType);
}
#endif

#ifdef __ENABLE_REMOVE_POLYMORPH__
ACMD(do_remove_polymorph)
{
	if (!ch)
	{
		return;
	}

	if (!ch->IsPolymorphed())
	{
		return;
	}

	ch->SetPolymorph(0);
	ch->RemoveAffect(AFFECT_POLYMORPH);
}
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
ACMD(do_change_channel)
{
	//int iLeftSeconds = 0;
	//if ((iLeftSeconds = ch->GetLeftTimeUntilNextChannelSwitch()) > 0)
	//{
	//	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can switch the channel in %d seconds."), iLeftSeconds);
	//	return;
	//}

	if (ch->m_pkTimedEvent)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your Logout was cancelled."));
		event_cancel(&ch->m_pkTimedEvent);
		return;
	}

	if (ch->QuitSwitchChannelEvent())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The channel switching were cancelled."));
		return;
	}

	if (ch->GetDungeon() || g_bChannel == 99)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can not change the channel since you are in a dungeon."));
		return;
	}

	if (!ch->IsPC())
	{
		return;
	}

	if (!ch->CanWarp())
	{
		return;
	}

	if (!ch->GetDesc())
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	TChannelSwitch packet;
	packet.byChannel = ((BYTE)atoi(arg1));
	packet.lMapIndex = ch->GetMapIndex();

	if (packet.byChannel == g_bChannel)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already on channel %d."), g_bChannel);
		return;
	}

	db_clientdesc->DBPacketHeader(HEADER_GD_CHANNEL_SWITCH, ch->GetDesc()->GetHandle(), sizeof(TChannelSwitch));
	db_clientdesc->Packet(&packet, sizeof(TChannelSwitch));
}
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
ACMD(do_user_costume_option)
{
	if (!ch || !ch->IsPC()) return;

	if (!ch->CanDoAction()) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	std::string sType = arg1;

	auto fIt = m_HideCostume.find(sType);
	if (fIt == m_HideCostume.end()) return;

	DWORD dCurrFlag = ch->GetCostumeFlag();

	if (ch->HasCostumeFlag(fIt->second))
		ch->SetCostumeFlag(dCurrFlag - fIt->second);
	else
		ch->SetCostumeFlag(dCurrFlag + fIt->second);

	ch->UpdatePacket();
	ch->BroadcastHCostume();
	return;
}
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
ACMD(do_notification_answer)
{
	NotificatonSender::GetNotificationAnswer(ch);
}
#endif

#ifdef __BATTLE_PASS_ENABLE__
ACMD(do_battle_pass_collect_reward)
{
	if (!ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("BATTLE_PASS_CANNOT_DO_ACTION"));
		return;
	}

	CBattlePassManager::instance().CollectReward(ch);
}
#endif

#ifdef __TRANSMUTATION_SYSTEM__
ACMD(do_transmutate_item)
{
	if (!ch->CanDoAction())
	{
		return;
	}

	if (!ch->GetTransmutationItems())
	{
		return;
	}

	// Item that we are gonna change
	TransMutation_Struct* trans_item = ch->GetTransmutationItem(0);
	// Item that we are gonna get rid of
	TransMutation_Struct* base_item = ch->GetTransmutationItem(1);

	if (!trans_item || !base_item)
	{
		return;
	}

	if (!Transmutation::CheckPair(trans_item, base_item))
	{
		return;
	}

	if (Transmutation::TRANSMITATION_COST > ch->GetGold())
	{
		return;
	}

	Transmutation::DoTransmutation(trans_item, base_item);
	ch->ChatPacket(CHAT_TYPE_COMMAND, "SetNewTransMutationItem %d", trans_item->pos);
	ch->ClearTransmutation();
	ch->PointChange(POINT_GOLD, -Transmutation::TRANSMITATION_COST);

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, Transmutation::TRANSMITATION_COST });
#endif
}

ACMD(do_transmutation_add)
{
	if (!ch->CanDoAction())
	{
		return;
	}

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
	{
		return;
	}

	BYTE gSlot = static_cast<BYTE>(std::strtoul(arg1, NULL, 10));

	if (gSlot > 1)
	{
		return;
	}

	WORD iSlot = static_cast<WORD>(std::strtoul(arg2, NULL, 10));

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (iSlot >= INVENTORY_MAX_NUM)
	{
		return;
	}
#else
	if ((iSlot >= INVENTORY_MAX_NUM && iSlot < SPECIAL_STORAGE_START_CELL) || iSlot >= INVENTORY_AND_EQUIP_SLOT_MAX)
	{
		return;
	}
#endif

	LPITEM item = ch->GetInventoryItem(iSlot);

	// Checking if item exists
	if (!item)
	{
		return;
	}

	// Checking if this item is being transmutated
	if (ch->FindTransMutationItem(item->GetID()))
	{
		return;
	}

	// Checking if item is fit for being transmutated
	if (!Transmutation::CheckItem(item, gSlot))
	{
		return;
	}
	else
	{
		item->Lock(true);
		ch->AddTransmutationItem(gSlot, item->GetID(), iSlot);
	}
}

ACMD(do_transmutation_delete)
{
	if (!ch->CanDoAction())
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	BYTE gSlot = static_cast<BYTE>(std::strtoul(arg1, NULL, 10));

	if (gSlot > 1)
	{
		return;
	}

	TransMutation_Struct* item_point = ch->GetTransmutationItem(gSlot);

	if (!item_point)
	{
		return;
	}
	else
	{
		// Checking if item exists
		LPITEM item = ITEM_MANAGER::instance().Find(item_point->item_id);
		if (!item)
		{
			return;
		}

		// Checking if this item is being transmutated
		if (!ch->FindTransMutationItem(item->GetID()))
		{
			return;
		}

		item->Lock(false);
		ch->RemoveTransmutationItem(gSlot);
	}
}
#endif

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
ACMD(do_passive_system_req_data)
{
	if (ch)
	{
		PSkillsManager::BroadcastData(ch);
	}
}
#endif

#ifdef __ENABLE_SAVE_POSITION__
ACMD(do_positions_action)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1)
	{
		return;
	}

	if (!*arg2 || !isnhdigit(*arg2))
	{
		return;
	}

	BYTE bPos = 0;
	str_to_number(bPos, arg2);

	if (bPos >= PositionsManagerHelper::EPositionsManagerHelper::MAX_SAVE_POSITIONS)
	{
		return;
	}

	if (!ch->CanDoAction() || !ch->GetDesc())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_POSITIONS_ACTION"));
		return;
	}

	if (!strcmp(arg1, "save"))
	{
		PositionsManagerHelper::Instance().SavePosition(ch, bPos);
	}

	if (!strcmp(arg1, "delete"))
	{
		PositionsManagerHelper::Instance().ClearPosition(ch, bPos);
	}

	if (!strcmp(arg1, "teleport"))
	{
		PositionsManagerHelper::Instance().TeleportPosition(ch, bPos);
	}

	return;
}
#endif

#ifdef __INVENTORY_BUFFERING__
ACMD(do_quick_open)
{
	if (!ch || !ch->CanWarp())
	{
		return;
	}

	char arg1[256];
	char arg2[256];

	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !isnhdigit(*arg1))
	{
		return;
	}

	if (!*arg2 || !isnhdigit(*arg2))
	{
		return;
	}

	LPITEM item = ch->GetInventoryItem(atoi(arg1));
	if (!item)
	{
		return;
	}

	CountType iCount = atoi(arg2);

	ch->QuickOpenStack(item, iCount);
}
#endif

ACMD(do_items_around)
{
	if (!ch->CanDoAction() || ch->IsDead())
	{
		return;
	}

	if (ch->GetNextPickupTime() > get_global_time())
	{
		return;
	}

	ch->PickupClosesItems();
	ch->UpdateNextPickupTime();
}

#ifdef __ENABLE_OFFLINE_SHOP__
ACMD(do_cancel_opening_offline_shop)
{
	if (!ch->IsOpeningOfflineShop())
	{
		return;
	}

	ch->SetOpeningOfflineShopState(false);
	if (ch->GetOfflineShopOpeningItem())
	{
		ch->GetOfflineShopOpeningItem()->Lock(false);
		ch->SetOfflineShopOpeningItem(nullptr);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "CancelOpeningOfflineShop");
}

ACMD(do_open_offline_shop)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	uint32_t id = std::atoi(arg1);

	auto shopPtr = COfflineShop::Get(id);
	if (shopPtr)
	{
		shopPtr->get()->AddViewer(ch);
	}
}

ACMD(do_close_offline_shop)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	uint32_t id = std::atoi(arg1);

	auto shopPtr = COfflineShop::Get(id);
	if (shopPtr)
	{
		auto shop = shopPtr->get();

		if (shop->GetOwnerPid() != ch->GetPlayerID())
		{
			return;
		}

		shop->RequestClose();
	}
}
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
ACMD(do_skill_select)
{
	if (!ch)
	{
		return;
	}

	if (!ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SKILL_SELECT_CANNOT_ACTUALY_DO_IT"));
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1 || !isnhdigit(*arg1))
	{
		return;
	}

	BeginnerHelper::instance().RecvChoice(ch, atoi(arg1));
}
#endif

#ifdef __ITEM_SHOP_ENABLE__
ACMD(do_request_itemshop)
{
	if (!ch->CanWarp())
	{
		return;
	}

	if (!CItemShopManager::instance().IsViewer(ch))
	{
		CItemShopManager::instance().AddViewer(ch);
		CItemShopManager::instance().BroadcastItemShop(ch);
	}
	else
	{
		CItemShopManager::instance().RemoveViewer(ch);
	}
}

ACMD(do_itemshop_purchase)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	CItemShopManager::instance().BuyItem(ch, arg1);
}
#endif

#ifdef __ENABLE_SKILLS_INFORMATION__
ACMD(do_get_skill_information)
{
	if (ch)
	{
		ch->BroadcastSkillInformation();
	}
}
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
// Passive
ACMD(do_legendary_stones_open_type)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	auto lType = static_cast<LegendaryStonesHandler::EWindowType>(atoi(arg1));

	LegendaryStonesHandler::BroadcastInterface(ch, lType);
}

ACMD(do_legendary_stones_passive_up)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_PASSIVE_CANNOT"));
		return;
	}

	switch (LegendaryStonesHandler::GivePassiveRequiredItem(ch, atoi(arg1)))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_PASSIVE_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_PASSIVE_NOT_ENOUGH_ITEMS"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_PASSIVE_SUCCESS"));
			break;
	}
}

// Craftings
ACMD(do_legendary_stones_craft_set)
{
	auto args = split_string(argument);

	if (args.size() < 3)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_CANNOT"));
		return;
	}

	auto eCat = ston<int32_t>(args[0]);
	auto iPos = ston<int32_t>(args[1]);
	auto wSlot = ston<int32_t>(args[2]);

	auto eCategory = static_cast<LegendaryStonesHandler::ECraftTypes>(eCat);

	switch (LegendaryStonesHandler::SetItemCraft(ch, eCategory, iPos, wSlot))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_SET_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_SET_VALID_ITEM"));
			break;

		default:
			break;
	}
}

ACMD(do_legendary_stones_craft_run)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_CANNOT"));
		return;
	}

	auto eCategory = static_cast<LegendaryStonesHandler::ECraftTypes>(atoi(arg1));

	switch (LegendaryStonesHandler::CraftItem(ch, eCategory, atoi(arg2)))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_NOT_ENOUGH_MONEY"));
			break;

		case -3:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_VALID_ITEM"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_SUCCESS"));
			break;
	}
}

// Exchange
ACMD(do_legendary_stones_exchange_start)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_CANNOT"));
		return;
	}

	LegendaryStonesHandler::RegisterExchange(ch);
}

ACMD(do_legendary_stones_exchange_cancel)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_CANNOT"));
		return;
	}

	LegendaryStonesHandler::UnregisterExchange(ch);
}

ACMD(do_legendary_stones_exchange_set_item)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_CANNOT"));
		return;
	}

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	switch (LegendaryStonesHandler::SetExchangeItem(ch, static_cast<BYTE>(atoi(arg1)), atoi(arg2)))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_ADD_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_ITEM_NOT_WHITE_LIST"));
			break;

		case -3:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_ADD_DUPLICATION"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_ADD_SUCCESS"));
			break;
	}
}

ACMD(do_legendary_stones_exchange_run)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_CANNOT"));
		return;
	}

	switch (LegendaryStonesHandler::StartExchange(ch))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_RUN_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_RUN_NOT_MATCH_ADDITIONAL"));
			break;

		case -3:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_RUN_NOT_MATCH"));
			break;

		case -4:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_RUN_FAILURE"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_EXCHANGE_RUN_SUCCESS"));
			break;
	}
}

// Refine
ACMD(do_legendary_stones_refine_cancel)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_CANNOT"));
		return;
	}

	LegendaryStonesHandler::UnregisterRefine(ch);
}

ACMD(do_legendary_stones_refine_set_item)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_CANNOT"));
		return;
	}

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	switch (LegendaryStonesHandler::SetRefineItem(ch, static_cast<BYTE>(atoi(arg1)), atoi(arg2)))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ADD_INTERNAL_ERROR"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ITEM_NOT_WHITE_LIST"));
			break;

		case -3:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ADD_DUPLICATION"));
			break;

		case -4:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ITEM_HAS_MAX_VALUE"));
			break;

		case -5:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ITEM_NOT_MATCH"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_ADD_SUCCESS"));
			break;
	}
}

ACMD(do_legendary_stones_refine_run)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_CANNOT"));
		return;
	}

	switch (LegendaryStonesHandler::StartRefine(ch))
	{
	case -1:
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_RUN_INTERNAL_ERROR"));
		break;													
																
	case -2:													
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_RUN_NOT_MATCH_ADDITIONAL"));
		break;													
																
	case -3:													
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_RUN_NOT_ENOUGH_GOLD"));
		break;													
																											
																
	default:													
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_REFINE_RUN_SUCCESS"));
		break;
	}
}
#endif

#ifdef __DUNGEON_INFO_ENABLE__
ACMD(do_dungeon_info_open_panel)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "DungeonInfo_OpenPanel");
}

ACMD(do_dungeon_info_join_dungeon)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	CDungeonInfoManager::instance().JoinDungeon(ch, arg1);
}

#ifdef __DUNGEON_RETURN_ENABLE__
ACMD(do_dungeon_info_rejoin_dungeon)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	CDungeonInfoManager::instance().ReJoinDungeon(ch, arg1);
}
#endif
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
ACMD(do_open_sash_combination)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_SASH_COMBINATION"));
		return;
	}

	SashSystemHelper::OpenSashInterface(ch, SashSystemHelper::SASH_INTERFACE_TYPE_COMBINATION);
}

ACMD(do_register_sash_system_combination)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_SASH_COMBINATION"));
		return;
	}

	BYTE bSlot = static_cast<BYTE>(std::strtoul(arg1, NULL, 10));
	int iSlot = static_cast<int>(std::strtoul(arg2, NULL, 10));

	if (bSlot == UCHAR_MAX)
		bSlot = static_cast<BYTE>(SashSystemHelper::GetSashFreeSlot(ch));

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (iSlot >= INVENTORY_MAX_NUM)
		return;
#else
	if ((iSlot >= INVENTORY_MAX_NUM && iSlot < SPECIAL_STORAGE_START_CELL) || iSlot >= INVENTORY_AND_EQUIP_SLOT_MAX)
		return;
#endif

	if (iSlot < 0)
	{
		SashSystemHelper::EraseSashToRefine(ch, bSlot);
		return;
	}

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item)
		return;

	if (item->IsEquipped() || item->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SASH_ITEM_CANNNOT_BE_USED"));
		return;
	}

	SashSystemHelper::RegisterSashToRefine(ch, item, bSlot);
}

ACMD(do_process_sash_system_combination)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_DO_SASH_COMBINATION"));
		return;
	}

	if (!SashSystemHelper::RefineSash(ch))
	{
		// Reloading
		SashSystemHelper::UnregisterSashRefine(ch);
		SashSystemHelper::RegisterSashRefine(ch);
	}
}

ACMD(do_open_sash_absorption)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_SASH_ABSORPTION"));
		return;
	}

	SashSystemHelper::OpenSashInterface(ch, SashSystemHelper::SASH_INTERFACE_TYPE_ABSORPTION);
}

ACMD(do_register_sash_system_absorption)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_SASH_ABSORPTION"));
		return;
	}

	BYTE bSlot = static_cast<BYTE>(std::strtoul(arg1, NULL, 10));
	int iSlot = static_cast<int>(std::strtoul(arg2, NULL, 10));

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (iSlot >= INVENTORY_MAX_NUM)
		return;
#else
	if ((iSlot >= INVENTORY_MAX_NUM && iSlot < SPECIAL_STORAGE_START_CELL) || iSlot >= INVENTORY_AND_EQUIP_SLOT_MAX)
		return;
#endif

	if (iSlot < 0)
	{
		SashSystemHelper::EraseSashToAbsorption(ch, bSlot);
		return;
	}

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item)
		return;

	if (item->IsEquipped() || item->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SASH_ITEM_CANNNOT_BE_USED"));
		return;
	}

	SashSystemHelper::RegisterSashToAbsorption(ch, item, bSlot);
}

ACMD(do_process_sash_system_absorption)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_DO_SASH_ABSORPTION"));
		return;
	}

	if (!SashSystemHelper::AbsorbForSash(ch))
	{
		// Reloading
		SashSystemHelper::UnregisterSashAbsorption(ch);
		SashSystemHelper::RegisterSashAbsorption(ch);
	}
}

ACMD(do_release_sash_absorption)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_SASH_ABSORPTION"));
		return;
	}

	WORD sSlot = static_cast<WORD>(std::strtoul(arg1, NULL, 10));
	WORD iSlot = static_cast<WORD>(std::strtoul(arg2, NULL, 10));

	LPITEM items[2] = { ch->GetInventoryItem(sSlot), ch->GetInventoryItem(iSlot) };
	if (!items[0] || !items[1])
		return;

	for (auto const& item : items)
	{
		if (item->IsEquipped() || item->isLocked())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SASH_ITEM_CANNNOT_BE_USED"));
			return;
		}
	}
	
	SashSystemHelper::ReleaseSashAbsorption(ch, items[0], items[1]);
}

ACMD(do_sash_system_cancel)
{
	SashSystemHelper::CloseSashInterface(ch);
}
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
ACMD(do_amulet_upgrade_info)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	WORD iSlot = static_cast<WORD>(std::strtoul(arg1, NULL, 10));
	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item) return;

	AmuletSystemHelper::AmuletUpgradeInfo(ch, item);
}

ACMD(do_amulet_upgrade_apply)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	WORD iSlot = static_cast<WORD>(std::strtoul(arg1, NULL, 10));
	int iApplyIndex = static_cast<int>(std::strtoul(arg2, NULL, 10));

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item) return;

	AmuletSystemHelper::AmuletUpgrade(ch, item, std::min<int>(iApplyIndex, ITEM_ATTRIBUTE_MAX_NUM));
}

ACMD(do_amulet_roll)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	WORD iSlot = static_cast<WORD>(std::strtoul(arg1, NULL, 10));
	BYTE iBit = static_cast<BYTE>(std::strtoul(arg2, NULL, 10));

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item) return;

	AmuletSystemHelper::AmuletRoll(ch, item, iBit);
}

// Crafting
ACMD(do_amulet_crafting_open)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_AMULET_CRAFTING"));
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCrafting_Open {}", AmuletSystemHelper::AMULET_DUST_ITEM));
}

ACMD(do_amulet_crafting_process)
{
	if (!ch->CanDoAction())
		return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
		return;

	WORD iSlot = static_cast<WORD>(std::strtoul(arg1, NULL, 10));

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item) return;

	switch (AmuletSystemHelper::AmuletCraftProcess(ch, item))
	{
		case -1:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("AMULET_CRAFT_VALID_ITEM"));
			break;

		case -2:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("AMULET_CRAFT_NOT_ENOUGH_MONEY"));
			break;

		default:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("LEGENDARY_STONE_CRAFT_SUCCESS"));
			break;
	}
}

// Combine
ACMD(do_amulet_combine_open)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_AMULET_COMBINATION"));
		return;
	}

	AmuletSystemHelper::AmuletCombineOpen(ch);
}

ACMD(do_amulet_combine_close)
{
	AmuletSystemHelper::AmuletCombineClose(ch);
}

ACMD(do_amulet_combine_register)
{
	char arg1[256];
	char arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2)
		return;

	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_OPEN_AMULET_COMBINATION"));
		return;
	}

	BYTE bSlot = static_cast<BYTE>(std::strtoul(arg1, NULL, 10));
	int iSlot = static_cast<int>(std::strtoul(arg2, NULL, 10));

	if (bSlot == UCHAR_MAX)
		bSlot = static_cast<BYTE>(AmuletSystemHelper::GetAmuletFreeSlot(ch));

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (iSlot >= INVENTORY_MAX_NUM)
		return;
#else
	if ((iSlot >= INVENTORY_MAX_NUM && iSlot < SPECIAL_STORAGE_START_CELL) || iSlot >= INVENTORY_AND_EQUIP_SLOT_MAX)
		return;
#endif

	if (iSlot < 0)
	{
		AmuletSystemHelper::AmuletCombineEraseItem(ch, bSlot);
		return;
	}

	LPITEM item = ch->GetInventoryItem(iSlot);
	if (!item)
		return;

	if (item->IsEquipped() || item->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("AMULET_ITEM_CANNNOT_BE_USED"));
		return;
	}

	AmuletSystemHelper::AmuletCombineRegisterItem(ch, item, bSlot);
}

ACMD(do_amulet_combine_process)
{
	if (!ch->CanDoAction() || !ch->GetDesc() || ch->GetDungeon() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CANNOT_DO_AMULET_COMBINATION"));
		return;
	}

	if (!AmuletSystemHelper::AmuletCombineProcess(ch))
	{
		// Reloading
		AmuletSystemHelper::UnregisterCombine(ch);
		AmuletSystemHelper::RegisterCombine(ch);
	}
}
#endif

ACMD(do_register_mark)
{
	if (!ch->CanDoAction())
		return;

	if (!ch->GetGuild() || ch->GetPlayerID() != ch->GetGuild()->GetMasterPID())
		return;

	quest::CQuestManager::Instance().RequestSetEventFlag(fmt::format("register_mark_{}", ch->GetGuild()->GetID()), 1);
	quest::CQuestManager::Instance().SetEventFlag(fmt::format("register_mark_{}", ch->GetGuild()->GetID()), 1);
}

#ifdef __ENABLE_ATTENDANCE_EVENT__
ACMD(do_attendance_collect)
{
	if (!ch || !ch->CanDoAction())
		return;

	CAttendanceRewardManager::instance().CollectReward(ch);
}
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
ACMD(do_request_biolog)
{
	if (!ch->CanDoAction()) return;

	if (!ch->GetBiologManager()) return;

	ch->GetBiologManager()->Broadcast();
}

ACMD(do_request_biolog_sets)
{
	if (!ch->CanDoAction()) return;

	CBiologSystemManager::instance().BroadcastSets(ch);
}

ACMD(do_request_biolog_timer)
{
	if (!ch->CanDoAction()) return;

	if (!ch->GetBiologManager()) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	ch->GetBiologManager()->ActiveAlert(static_cast<bool>(atoi(arg1)));
}

ACMD(do_biolog_collect)
{
	if (!ch->CanDoAction()) return;

	if (!ch->GetBiologManager()) return;

	char arg1[256], arg2[256];
	two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	if (!*arg1 || !*arg2) return;

	bool bAdditionalChance = static_cast<bool>(atoi(arg1));
	bool bAdditionalTime = static_cast<bool>(atoi(arg2));

	ch->GetBiologManager()->CollectItem(bAdditionalChance, bAdditionalTime);
}

ACMD(do_biolog_collect_affect)
{
	if (!ch->CanDoAction()) return;

	if (!ch->GetBiologManager()) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	ch->GetBiologManager()->RequestAffect(static_cast<uint8_t>(atoi(arg1)));
}

ACMD(do_biolog_reset)
{
	if (!ch->CanDoAction()) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;
	ch->GetBiologManager()->ResetAffect(static_cast<uint8_t>(atoi(arg1)));
}
#endif

#ifdef __ENABLE_MOB_TRAKCER__
ACMD(do_request_tracker)
{
	MobTrackerManager::instance().Broadcast(ch);
}

ACMD(do_request_tracker_teleport)
{
	if (!ch->CanDoAction()) return;

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1) return;

	MobTrackerManager::instance().RequestTeleport(ch, static_cast<uint8_t>(atoi(arg1)));
}
#endif
