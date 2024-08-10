#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "event.h"
#include "minilzo.h"
#include "packet.h"
#include "desc_manager.h"
#include "item_manager.h"
#include "char.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "motion.h"
#include "sectree_manager.h"
#include "shop_manager.h"
#include "regen.h"
#include "text_file_loader.h"
#include "skill.h"
#include "pvp.h"
#include "party.h"
#include "questmanager.h"
#include "profiler.h"
#include "lzo_manager.h"
#include "messenger_manager.h"
#include "db.h"
#include "log.h"
#include "p2p.h"
#include "guild_manager.h"
#include "dungeon.h"
#include "cmd.h"
#include "refine.h"
#include "banword.h"
#include "priv_manager.h"
#include "war_map.h"
#include "building.h"
#include "login_sim.h"
#include "target.h"
#include "marriage.h"
#include "wedding.h"
#include "fishing.h"
#include "item_addon.h"
#include "TrafficProfiler.h"
#include "locale_service.h"
#include "arena.h"
#include "OXEvent.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "passpod.h"
#include "ani.h"
#include "BattleArena.h"
#include "over9refine.h"
#include "horsename_manager.h"
#include "pcbang.h"
#include "MarkManager.h"
#include "spam.h"
#include "panama.h"
#include "threeway_war.h"
#include "auth_brazil.h"
#include "DragonLair.h"
#include "HackShield.h"
#include "skill_power.h"
#include "SpeedServer.h"
#include "XTrapManager.h"
#include "DragonSoul.h"
#include <boost/bind.hpp>

#ifndef __WIN32__
	#include "limit_time.h"
#endif

#ifdef __ENABLE_SWITCHBOT__
	#include "switchbot.h"
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	#include "MonsterRespawner.hpp"
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	#include "NotificatonSender.hpp"
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	#include "message_offline.h"
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	#include "GayaSystemManager.hpp"
#endif

#ifdef __NEW_EVENT_HANDLER__
	#include "EventFunctionHandler.hpp"
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	#include "EventManager.hpp"
#endif

//#define __FILEMONITOR__
#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	#include "TechnicalMaintenance.hpp"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
#endif

#ifdef __LEGENDARY_MONSTER_HELPER_ENABLE__
	#include "MonsterSpecialHelper.hpp"
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	#include "ShipDefendDungeonManager.hpp"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	#include "find_letters_event.h"
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	#include "TeleportManagerSystem.hpp"
#endif

#ifdef __ENABLE_MOB_TRAKCER__
#include "MobTrackerManager.hpp"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __ITEM_SHOP_ENABLE__
	#include "ItemShopManager.hpp"
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	#include "TombolaManager.hpp"
#endif

#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	#include "FileMonitor_FreeBSD.h"
#endif

#ifdef ENABLE_GOOGLE_TEST
	#ifndef __WIN32__
		#include <gtest/gtest.h>
	#endif
#endif

#ifdef USE_STACKTRACE
	#include <execinfo.h>
#endif

#ifdef __ENABLE_AUTO_NOTICE__
	#include "CAutoNotice.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	#include "SashSystemHelper.hpp"
#endif

#ifdef __AUTH_ULTIMATE_QUEUEING__
	#include "AuthQueueUltimate.h"
#endif

#ifdef __ENABLE_MISSION_MANAGER__
	#include "MissionManager.hpp"
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

#include "EquipmentSet.hpp"

extern void WriteVersion();
//extern const char * _malloc_options;
#if defined(__FreeBSD__) && defined(DEBUG_ALLOC)
extern void (*_malloc_message)(const char* p1, const char* p2, const char* p3, const char* p4);
// FreeBSD _malloc_message replacement
void WriteMallocMessage(const char* p1, const char* p2, const char* p3, const char* p4)
{
	FILE* fp = ::fopen(DBGALLOC_LOG_FILENAME, "a");
	if (fp == NULL)
	{
		return;
	}
	::fprintf(fp, "%s %s %s %s\n", p1, p2, p3, p4);
	::fclose(fp);
}
#endif

// TRAFFIC_PROFILER
static const DWORD	TRAFFIC_PROFILE_FLUSH_CYCLE = 3600;
// END_OF_TRAFFIC_PROFILER


volatile int	num_events_called = 0;
int             max_bytes_written = 0;
int             current_bytes_written = 0;
int             total_bytes_written = 0;
BYTE		g_bLogLevel = 0;

socket_t	tcp_socket = 0;
socket_t	udp_socket = 0;
socket_t	p2p_socket = 0;

LPFDWATCH	main_fdw = NULL;

int		io_loop(LPFDWATCH fdw);

int		start(int argc, char **argv);
int		idle();
void	destroy();

void 	test();
#ifdef __SOCKET_CHECK_ENABLE__
	DWORD	dw_check_socket_time = 0;
#endif

enum EProfile
{
	PROF_EVENT,
	PROF_CHR_UPDATE,
	PROF_IO,
	PROF_HEARTBEAT,
	PROF_MAX_NUM
};

static DWORD s_dwProfiler[PROF_MAX_NUM];

int g_shutdown_disconnect_pulse;
int g_shutdown_disconnect_force_pulse;
int g_shutdown_core_pulse;
bool g_bShutdown = false;

extern void CancelReloadSpamEvent();

void ContinueOnFatalError()
{
#ifdef USE_STACKTRACE
	void* array[200];
	std::size_t size;
	char** symbols;

	size = backtrace(array, 200);
	symbols = backtrace_symbols(array, size);

	std::ostringstream oss;
	oss << std::endl;
	for (std::size_t i = 0; i < size; ++i)
	{
		oss << "  Stack> " << symbols[i] << std::endl;
	}

	free(symbols);

	sys_err("FatalError on %s", oss.str().c_str());
#else
	sys_err("FatalError");
#endif
}

void ShutdownOnFatalError()
{
	if (!g_bShutdown)
	{
		sys_err("ShutdownOnFatalError!!!!!!!!!!");
		{
			char buf[256];

			strlcpy(buf, LC_TEXT("A critical server error has occurred. The server will restart automatically."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("You will be disconnected automatically in 10 seconds."), sizeof(buf));
			SendNotice(buf);
			strlcpy(buf, LC_TEXT("You can connect again after 5 minutes."), sizeof(buf));
			SendNotice(buf);
		}

		g_bShutdown = true;
		g_bNoMoreClient = true;

		g_shutdown_disconnect_pulse = thecore_pulse() + PASSES_PER_SEC(10);
		g_shutdown_disconnect_force_pulse = thecore_pulse() + PASSES_PER_SEC(20);
		g_shutdown_core_pulse = thecore_pulse() + PASSES_PER_SEC(30);
	}
}

namespace
{
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

		d->SetPhase(PHASE_CLOSE);
	}
};
}

extern std::map<DWORD, CLoginSim *> g_sim; // first: AID
extern std::map<DWORD, CLoginSim *> g_simByPID;
extern std::vector<TPlayerTable> g_vec_save;
unsigned int save_idx = 0;

void heartbeat(LPHEART ht, int pulse)
{
	DWORD t;

	t = get_dword_time();
	num_events_called += event_process(pulse);
	s_dwProfiler[PROF_EVENT] += (get_dword_time() - t);

	t = get_dword_time();


	if (!(pulse % ht->passes_per_sec))
	{
#ifdef __ENABLE_FIND_LETTERS_EVENT__
		CFindLetters::instance().CheckEventEnd();
#endif

#ifdef ENABLE_LIMIT_TIME
		if ((unsigned)get_global_time() >= GLOBAL_LIMIT_TIME)
		{
			sys_err("Server life time expired.");
			g_bShutdown = true;
		}
#endif

		if (!g_bAuthServer)
		{
			TPlayerCountPacket pack;
			pack.dwCount = DESC_MANAGER::instance().GetLocalUserCount();
			db_clientdesc->DBPacket(HEADER_GD_PLAYER_COUNT, 0, &pack, sizeof(TPlayerCountPacket));
		}
		else
		{
			DESC_MANAGER::instance().ProcessExpiredLoginKey();
			DBManager::instance().FlushBilling();
			/*
			   if (!(pulse % (ht->passes_per_sec * 600)))
			   DBManager::instance().CheckBilling();
			 */
		}

		{
			int count = 0;
#ifndef __AUTH_ULTIMATE_QUEUEING__
			itertype(g_sim) it = g_sim.begin();

			while (it != g_sim.end())
			{
				if (!it->second->IsCheck())
				{
					it->second->SendLogin();

					if (++count > 50)
					{
						sys_log(0, "FLUSH_SENT");
						break;
					}
				}

				it++;
			}
#else
			count = CLoginQueue::instance().PassQueue();
			CLoginQueue::instance().CleanUp();
#endif

			if (save_idx < g_vec_save.size())
			{
				count = MIN(100, g_vec_save.size() - save_idx);

				for (int i = 0; i < count; ++i, ++save_idx)
				{
					db_clientdesc->DBPacket(HEADER_GD_PLAYER_SAVE, 0, &g_vec_save[save_idx], sizeof(TPlayerTable));
				}

				sys_log(0, "SAVE_FLUSH %d", count);
			}
		}
	}

	//
	//


	if (!(pulse % (passes_per_sec + 4)))
	{
		CHARACTER_MANAGER::instance().ProcessDelayedSave();
	}


#if defined (__FreeBSD__) && defined(__FILEMONITOR__)
	if (!(pulse % (passes_per_sec * 5)))
	{
		FileMonitorFreeBSD::Instance().Update(pulse);
	}
#endif


	if (!(pulse % (passes_per_sec * 5 + 2)))
	{
		ITEM_MANAGER::instance().Update();
		DESC_MANAGER::instance().UpdateLocalUserCount();
	}

	s_dwProfiler[PROF_HEARTBEAT] += (get_dword_time() - t);

	DBManager::instance().Process();
	AccountDB::instance().Process();
	CPVPManager::instance().Process();

#ifdef __ENABLE_OFFLINE_SHOP__
	if (!g_bAuthServer)
	{
		COfflineShop::Process();
	}
#endif

	if (g_bShutdown)
	{
		if (thecore_pulse() > g_shutdown_disconnect_pulse)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), ::SendDisconnectFunc());
			g_shutdown_disconnect_pulse = INT_MAX;
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse)
		{
			const DESC_MANAGER::DESC_SET & c_set_desc = DESC_MANAGER::instance().GetClientSet();
			std::for_each(c_set_desc.begin(), c_set_desc.end(), ::DisconnectFunc());
		}
		else if (thecore_pulse() > g_shutdown_disconnect_force_pulse + PASSES_PER_SEC(5))
		{
			thecore_shutdown();
		}
	}
}

static void CleanUpForEarlyExit()
{
	CancelReloadSpamEvent();
}

int main(int argc, char **argv)
{
#ifdef DEBUG_ALLOC
	DebugAllocator::StaticSetUp();
#endif

#ifdef ENABLE_GOOGLE_TEST
#ifndef __WIN32__
	// <Factor> start unit tests if option is set
	if ( argc > 1 )
	{
		if ( strcmp( argv[1], "unittest" ) == 0 )
		{
			::testing::InitGoogleTest(&argc, argv);
			return RUN_ALL_TESTS();
		}
	}
#endif
#endif

	ilInit(); // DevIL Initialize

	WriteVersion();

	SECTREE_MANAGER::get_instance();
	CHARACTER_MANAGER::get_instance();
	ITEM_MANAGER::get_instance();
	CShopManager::get_instance();
	CMobManager::get_instance();
	CMotionManager::get_instance();
	CPartyManager::get_instance();
	CSkillManager::get_instance();
	CPVPManager::get_instance();
	LZOManager::get_instance();
	DBManager::get_instance();
	AccountDB::get_instance();

	LogManager::get_instance();
	MessengerManager::get_instance();
	P2P_MANAGER::get_instance();
	CGuildManager::get_instance();
	CFishing::get_instance();
	CGuildMarkManager::get_instance();
	CDungeonManager::get_instance();
	CRefineManager::get_instance();
	CBanwordManager::get_instance();
	CPrivManager::get_instance();
	CWarMapManager::get_instance();
	building::CManager::get_instance();
	CTargetManager::get_instance();
	marriage::CManager::get_instance();
	marriage::WeddingManager::get_instance();
	CItemAddonManager::get_instance();
	CArenaManager::get_instance();
	COXEventManager::get_instance();
	CHorseNameManager::get_instance();
	CPCBangManager::get_instance();

	DESC_MANAGER::get_instance();

	TrafficProfiler::get_instance();
	CTableBySkill::get_instance();
	CPolymorphUtils::get_instance();
	CProfiler::get_instance();
	CPasspod::get_instance();
	CBattleArena::get_instance();
	COver9RefineManager::get_instance();
	SpamManager::get_instance();
	CThreeWayWar::get_instance();
	CDragonLairManager::get_instance();

	CHackShieldManager::get_instance();
	CXTrapManager::get_instance();

	CSpeedServerManager::get_instance();
	DSManager::get_instance();

#ifdef __ENABLE_SWITCHBOT__
	CSwitchbotManager::get_instance();
#endif

#ifdef __ADMIN_MANAGER__
	CAdminManager::get_instance();
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	CCubeRenewal::get_instance();
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	CFindLetters::get_instance();
#endif

#ifdef __ENABLE_MISSION_MANAGER__
	CMissionManager::get_instance();
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	CTeleportManager::get_instance();
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	CAttendanceRewardManager::get_instance();
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	BeginnerHelper::get_instance();
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	CBiologSystemManager::get_instance();
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	CMarbleSystemManager::get_instance();
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	MobTrackerManager::get_instance();
#endif

	EquipmentSetSettings::get_instance();

	if (!start(argc, argv))
	{
		CleanUpForEarlyExit();
		return 0;
	}

	quest::CQuestManager::get_instance();

	if (!quest::CQuestManager::instance().Initialize())
	{
		CleanUpForEarlyExit();
		return 0;
	}

	MessengerManager::instance().Initialize();
	CGuildManager::instance().Initialize();
	CFishing::instance().Initialize();
	COXEventManager::instance().Initialize();
	if (speed_server)
	{
		CSpeedServerManager::instance().Initialize();
	}

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	CFindLetters::instance().Initialize();
#endif

	Blend_Item_init();
	ani_init();
	PanamaLoad();

#ifdef __ITEM_SHOP_ENABLE__
	CItemShopManager::get_instance();
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	TombolaEvent::LoadConfig();
#endif

#ifdef __ENABLE_MISSION_MANAGER__
	CMissionManager::instance().Create();
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	CTeleportManager::instance().Create();
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	MobTrackerManager::instance().Create();
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	CAttendanceRewardManager::instance().Create();
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	BeginnerHelper::instance().Create();
#endif

#ifdef	__ENABLE_BIOLOG_SYSTEM__
	CBiologSystemManager::instance().Create();
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	CMarbleSystemManager::instance().Create();
#endif

	EquipmentSetSettings::instance().Create();
	
	if ( g_bTrafficProfileOn )
	{
		TrafficProfiler::instance().Initialize( TRAFFIC_PROFILE_FLUSH_CYCLE, "ProfileLog" );
	}

#ifdef __NEW_EVENT_HANDLER__
	CEventFunctionHandler::get_instance();
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	COfflineMessage::get_instance();
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	CGayaSystemManager::get_instance();
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	// CShipDefendDungeonManager ShipDefendDungeonManager;
	CShipDefendDungeonManager::get_instance();
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	MonsterRespawner::Initialize();
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	CTechnicalMaintenance::get_instance();
#endif

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::get_instance();
#endif


#ifdef __EVENT_MANAGER_ENABLE__
	CEventManager::get_instance();
#endif

#ifdef __ENABLE_AUTO_NOTICE__
	CAutoNoticeManager::Initialize();
#endif

	while (idle());

	sys_log(0, "<shutdown> Starting...");
	g_bShutdown = true;
	g_bNoMoreClient = true;

	if (g_bAuthServer)
	{
		DBManager::instance().FlushBilling(true);

		int iLimit = DBManager::instance().CountQuery() / 50;
		int i = 0;

		do
		{
			DWORD dwCount = DBManager::instance().CountQuery();
			sys_log(0, "Queries %u", dwCount);

			if (dwCount == 0)
			{
				break;
			}

			usleep(500000);

			if (++i >= iLimit)
				if (dwCount == DBManager::instance().CountQuery())
				{
					break;
				}
		} while (1);
	}

#ifdef __ENABLE_OFFLINE_SHOP__
	sys_log(0, "<shutdown> Destroying COfflineShop...");
	COfflineShop::DestroyAll();
#endif

#ifdef __AUTH_ULTIMATE_QUEUEING__
	sys_log(0, "<shutdown> Destroying CLoginQueue...");
	CLoginQueue::instance().CleanUp();
#endif

	sys_log(0, "<shutdown> Destroying CArenaManager...");
	CArenaManager::instance().Destroy();
	sys_log(0, "<shutdown> Destroying COXEventManager...");
	COXEventManager::instance().Destroy();

	sys_log(0, "<shutdown> Disabling signal timer...");
	signal_timer_disable();

	sys_log(0, "<shutdown> Shutting down CHARACTER_MANAGER...");
	CHARACTER_MANAGER::instance().GracefulShutdown();
	sys_log(0, "<shutdown> Shutting down ITEM_MANAGER...");
	ITEM_MANAGER::instance().GracefulShutdown();

	sys_log(0, "<shutdown> Flushing db_clientdesc...");
	db_clientdesc->FlushOutput();
	sys_log(0, "<shutdown> Flushing p2p_manager...");
	P2P_MANAGER::instance().FlushOutput();

	sys_log(0, "<shutdown> Destroying CShopManager...");
	CShopManager::instance().Destroy();
	sys_log(0, "<shutdown> Destroying CHARACTER_MANAGER...");
	CHARACTER_MANAGER::instance().Destroy();
	sys_log(0, "<shutdown> Destroying ITEM_MANAGER...");
	ITEM_MANAGER::instance().Destroy();
	sys_log(0, "<shutdown> Destroying DESC_MANAGER...");
	DESC_MANAGER::instance().Destroy();
	sys_log(0, "<shutdown> Destroying quest::CQuestManager...");
	quest::CQuestManager::instance().Destroy();
	sys_log(0, "<shutdown> Destroying building::CManager...");
	building::CManager::instance().Destroy();

#ifdef __NEW_EVENT_HANDLER__
	sys_log(0, "<shutdown> Destroying CEventFunctionHandler...");
	CEventFunctionHandler::instance().Destroy();
#endif
#ifdef __OFFLINE_MESSAGE_SYSTEM__
	sys_log(0, "<shutdown> Destroying COfflineMessage...");
	COfflineMessage::instance().Destroy();
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	sys_log(0, "<shutdown> Destroying CGayaSystemManager...");
	CGayaSystemManager::instance().Destroy();
#endif
#ifdef __EVENT_MANAGER_ENABLE__
	sys_log(0, "<shutdown> Destroying CEventManager");
	CEventManager::instance().Destroy();
#endif
#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	sys_log(0, "<shutdown> Clearing CTechnicalMaintenance...");
	CTechnicalMaintenance::instance().Destroy();
#endif

	sys_log(0, "<shutdown> Flushing TrafficProfiler...");
	TrafficProfiler::instance().Flush();
#ifdef __NOTIFICATON_SENDER_ENABLE__
	sys_log(0, "<shutdown> Clearing NotificatonSender...");
	NotificatonSender::CleanUp();
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	sys_log(0, "<shutdown> Destroying CTeleportManager...");
	CTeleportManager::instance().Destroy();
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	sys_log(0, "<shutdown> Destroying MobTrackerManager...");
	MobTrackerManager::instance().Destroy();
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	sys_log(0, "<shutdown> Clearing SashSystemHelper...");
	SashSystemHelper::ClearUp();
#endif

	destroy();

#ifdef DEBUG_ALLOC
	DebugAllocator::StaticTearDown();
#endif

	return 1;
}

void usage()
{
	printf("Option list\n"
		   "-p <port>    : bind port number (port must be over 1024)\n"
		   "-l <level>   : sets log level\n"
		   "-n <locale>  : sets locale name\n"
#ifdef __ENABLE_NEWSTUFF__
		   "-C <on-off>  : checkpointing check on/off\n"
#endif
		   "-v           : log to stdout\n"
		   "-r           : do not load regen tables\n"
		   "-t           : traffic profile on\n");
}

int start(int argc, char **argv)
{
	std::string st_localeServiceName;

	bool bVerbose = false;
	char ch;

	//_malloc_options = "A";
#if defined(__FreeBSD__) && defined(DEBUG_ALLOC)
	_malloc_message = WriteMallocMessage;
#endif
#ifdef ENABLE_LIMIT_TIME
	if ((unsigned)get_global_time() >= GLOBAL_LIMIT_TIME)
	{
		sys_err("Server life time expired.");
		return 0;
	}
#endif

#ifdef __ENABLE_NEWSTUFF__
	while ((ch = getopt(argc, argv, "npverltIC")) != -1)
#else
	while ((ch = getopt(argc, argv, "npverltI")) != -1)
#endif
	{
		char* ep = NULL;

		switch (ch)
		{
		case 'I': // IP
			strlcpy(g_szPublicIP, argv[optind], sizeof(g_szPublicIP));

			printf("IP %s\n", g_szPublicIP);

			optind++;
			optreset = 1;
			break;

		case 'p': // port
			mother_port = strtol(argv[optind], &ep, 10);

			if (mother_port <= 1024)
			{
				usage();
				return 0;
			}

			printf("port %d\n", mother_port);

			optind++;
			optreset = 1;
			break;

		case 'l':
		{
			long l = strtol(argv[optind], &ep, 10);

			log_set_level(l);

			optind++;
			optreset = 1;
		}
		break;

		// LOCALE_SERVICE
		case 'n':
		{
			if (optind < argc)
			{
				st_localeServiceName = argv[optind++];
				optreset = 1;
			}
		}
		break;
			// END_OF_LOCALE_SERVICE

#ifdef __ENABLE_NEWSTUFF__
		case 'C': // checkpoint check
			bCheckpointCheck = strtol(argv[optind], &ep, 10);;
			printf("CHECKPOINT_CHECK %d\n", bCheckpointCheck);

			optind++;
			optreset = 1;
			break;
#endif

		case 'v': // verbose
			bVerbose = true;
			break;

		case 'r':
			g_bNoRegen = true;
			break;

		// TRAFFIC_PROFILER
		case 't':
			g_bTrafficProfileOn = true;
			break;
			// END_OF_TRAFFIC_PROFILER
		}
	}

	// LOCALE_SERVICE
	config_init(st_localeServiceName);
	// END_OF_LOCALE_SERVICE

#ifdef __WIN32__
	// In Windows dev mode, "verbose" option is [on] by default.
	bVerbose = true;
#endif
	if (!bVerbose)
	{
		freopen("stdout", "a", stdout);
	}

	bool is_thecore_initialized = thecore_init(25, heartbeat);

	if (!is_thecore_initialized)
	{
		fprintf(stderr, "Could not initialize thecore, check owner of pid, syslog\n");
		exit(0);
	}

	if (false == CThreeWayWar::instance().LoadSetting("forkedmapindex.txt"))
	{
		if (false == g_bAuthServer)
		{
			fprintf(stderr, "Could not Load ThreeWayWar Setting file");
			exit(0);
		}
	}

	signal_timer_disable();

	main_fdw = fdwatch_new(4096);

	if ((tcp_socket = socket_tcp_bind(g_szPublicIP, mother_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: tcp_socket");
		return 0;
	}


#ifndef __UDP_BLOCK__
	if ((udp_socket = socket_udp_bind(g_szPublicIP, mother_port)) == INVALID_SOCKET)
	{
		perror("socket_udp_bind: udp_socket");
		return 0;
	}
#endif

	// if internal ip exists, p2p socket uses internal ip, if not use public ip
	//if ((p2p_socket = socket_tcp_bind(*g_szInternalIP ? g_szInternalIP : g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	if ((p2p_socket = socket_tcp_bind(g_szPublicIP, p2p_port)) == INVALID_SOCKET)
	{
		perror("socket_tcp_bind: p2p_socket");
		return 0;
	}

	fdwatch_add_fd(main_fdw, tcp_socket, NULL, FDW_READ, false);
#ifndef __UDP_BLOCK__
	fdwatch_add_fd(main_fdw, udp_socket, NULL, FDW_READ, false);
#endif
	fdwatch_add_fd(main_fdw, p2p_socket, NULL, FDW_READ, false);

	db_clientdesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, db_addr, db_port, PHASE_DBCLIENT, true);
	if (!g_bAuthServer)
	{
		db_clientdesc->UpdateChannelStatus(0, true);
	}

	if (g_bAuthServer)
	{
		if (g_stAuthMasterIP.length() != 0)
		{
			fprintf(stderr, "SlaveAuth");
			g_pkAuthMasterDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, g_stAuthMasterIP.c_str(), g_wAuthMasterPort, PHASE_P2P, true);
			P2P_MANAGER::instance().RegisterConnector(g_pkAuthMasterDesc);
			g_pkAuthMasterDesc->SetP2P(g_stAuthMasterIP.c_str(), g_wAuthMasterPort, g_bChannel);

		}
		else
		{
			fprintf(stderr, "MasterAuth\n");
		}
	}
	/* game server to teen server */
	else
	{
		if (teen_addr[0] && teen_port)
		{
			g_TeenDesc = DESC_MANAGER::instance().CreateConnectionDesc(main_fdw, teen_addr, teen_port, PHASE_TEEN, true);
		}

		sys_log(0, "SPAM_CONFIG: duration %u score %u reload cycle %u\n",
				g_uiSpamBlockDuration, g_uiSpamBlockScore, g_uiSpamReloadCycle);

		extern void LoadSpamDB();
		LoadSpamDB();
	}

	signal_timer_enable(30);
	return 1;
}

void destroy()
{
	sys_log(0, "<shutdown> Canceling ReloadSpamEvent...");
	CancelReloadSpamEvent();

	sys_log(0, "<shutdown> regen_free()...");
	regen_free();

	sys_log(0, "<shutdown> Closing sockets...");
	socket_close(tcp_socket);
#ifndef __UDP_BLOCK__
	socket_close(udp_socket);
#endif
	socket_close(p2p_socket);

	sys_log(0, "<shutdown> fdwatch_delete()...");
	fdwatch_delete(main_fdw);

	sys_log(0, "<shutdown> event_destroy()...");
	event_destroy();

	sys_log(0, "<shutdown> CTextFileLoader::DestroySystem()...");
	CTextFileLoader::DestroySystem();

	sys_log(0, "<shutdown> thecore_destroy()...");
	thecore_destroy();
}

int idle()
{
	static struct timeval	pta = { 0, 0 };
	static int			process_time_count = 0;
	struct timeval		now;

	if (pta.tv_sec == 0)
	{
		gettimeofday(&pta, (struct timezone *) 0);
	}

	int passed_pulses;

	if (!(passed_pulses = thecore_idle()))
	{
		return 0;
	}

	assert(passed_pulses > 0);

	DWORD t;

	while (passed_pulses--)
	{
		heartbeat(thecore_heart, ++thecore_heart->pulse);

		// To reduce the possibility of abort() in checkpointing
		thecore_tick();
	}

	t = get_dword_time();
	CHARACTER_MANAGER::instance().Update(thecore_heart->pulse);
	db_clientdesc->Update(t);
	s_dwProfiler[PROF_CHR_UPDATE] += (get_dword_time() - t);

	t = get_dword_time();
	if (!io_loop(main_fdw)) { return 0; }
	s_dwProfiler[PROF_IO] += (get_dword_time() - t);

	log_rotate();

	gettimeofday(&now, (struct timezone *) 0);
	++process_time_count;

	if (now.tv_sec - pta.tv_sec > 0)
	{
		pt_log("[%3d] event %5d/%-5d idle %-4ld event %-4ld heartbeat %-4ld I/O %-4ld chrUpate %-4ld | WRITE: %-7d | PULSE: %d",
			   process_time_count,
			   num_events_called,
			   event_count(),
			   thecore_profiler[PF_IDLE],
			   s_dwProfiler[PROF_EVENT],
			   s_dwProfiler[PROF_HEARTBEAT],
			   s_dwProfiler[PROF_IO],
			   s_dwProfiler[PROF_CHR_UPDATE],
			   current_bytes_written,
			   thecore_pulse());

		num_events_called = 0;
		current_bytes_written = 0;

		process_time_count = 0;
		gettimeofday(&pta, (struct timezone *) 0);

		memset(&thecore_profiler[0], 0, sizeof(thecore_profiler));
		memset(&s_dwProfiler[0], 0, sizeof(s_dwProfiler));
	}

#ifdef __WIN32__
	if (_kbhit())
	{
		int c = _getch();
		switch (c)
		{
		case 0x1b: // Esc
			return 0; // shutdown
			break;
		default:
			break;
		}
	}
#endif

#ifdef __NEW_EVENT_HANDLER__
	CEventFunctionHandler::instance().Process();
#endif
#ifdef __CYCLIC_MONSTER_RESPAWNER__
	MonsterRespawner::ProcessRespawn();
#endif
#ifdef __LEGENDARY_MONSTER_HELPER_ENABLE__
	MonsterSpecialHelper::LegendaryBossSpawn();
#endif

#ifdef __ENABLE_AUTO_NOTICE__
	CAutoNoticeManager::Process();
#endif

#ifdef __SOCKET_CHECK_ENABLE__
	if (dw_check_socket_time < static_cast<DWORD>(get_global_time()))
	{
		if (socket_check_status(db_clientdesc->GetSocket()) == 0) // Error Code
		{
			sys_err("Server down due to the DB issue.");
			return 0;
		}
		else
		{
			dw_check_socket_time = get_global_time() + 60;    // 60 Sec
		}
	}
#endif

	return 1;
}

int io_loop(LPFDWATCH fdw)
{
	LPDESC	d;
	int		num_events, event_idx;

	DESC_MANAGER::instance().DestroyClosed();
	DESC_MANAGER::instance().TryConnect();

	if ((num_events = fdwatch(fdw, 0)) < 0)
	{
		return 0;
	}

	for (event_idx = 0; event_idx < num_events; ++event_idx)
	{
		d = (LPDESC) fdwatch_get_client_data(fdw, event_idx);

		if (!d)
		{
			if (FDW_READ == fdwatch_check_event(fdw, tcp_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptDesc(fdw, tcp_socket);
				fdwatch_clear_event(fdw, tcp_socket, event_idx);
			}
			else if (FDW_READ == fdwatch_check_event(fdw, p2p_socket, event_idx))
			{
				DESC_MANAGER::instance().AcceptP2PDesc(fdw, p2p_socket);
				fdwatch_clear_event(fdw, p2p_socket, event_idx);
			}
			/*
			else if (FDW_READ == fdwatch_check_event(fdw, udp_socket, event_idx))
			{
				char			buf[256];
				struct sockaddr_in	cliaddr;
				socklen_t		socklen = sizeof(cliaddr);

				int iBytesRead;

				if ((iBytesRead = socket_udp_read(udp_socket, buf, 256, (struct sockaddr *) &cliaddr, &socklen)) > 0)
				{
					static CInputUDP s_inputUDP;

					s_inputUDP.SetSockAddr(cliaddr);

					int iBytesProceed;
					s_inputUDP.Process(NULL, buf, iBytesRead, iBytesProceed);
				}

				fdwatch_clear_event(fdw, udp_socket, event_idx);
			}
			*/
			continue;
		}

		int iRet = fdwatch_check_event(fdw, d->GetSocket(), event_idx);

		switch (iRet)
		{
		case FDW_READ:
			if (db_clientdesc == d)
			{
				int size = d->ProcessInput();

				if (size)
				{
					sys_log(1, "DB_BYTES_READ: %d", size);
				}

				if (size < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}
			}
			else if (d->ProcessInput() < 0)
			{
				d->SetPhase(PHASE_CLOSE);
			}
			break;

		case FDW_WRITE:
			if (db_clientdesc == d)
			{
				int buf_size = buffer_size(d->GetOutputBuffer());
				int sock_buf_size = fdwatch_get_buffer_size(fdw, d->GetSocket());

				int ret = d->ProcessOutput();

				if (ret < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}

				if (buf_size)
				{
					sys_log(1, "DB_BYTES_WRITE: size %d sock_buf %d ret %d", buf_size, sock_buf_size, ret);
				}
			}
			else if (d->ProcessOutput() < 0)
			{
				d->SetPhase(PHASE_CLOSE);
			}
			else if (g_TeenDesc == d)
			{
				int buf_size = buffer_size(d->GetOutputBuffer());
				int sock_buf_size = fdwatch_get_buffer_size(fdw, d->GetSocket());

				int ret = d->ProcessOutput();

				if (ret < 0)
				{
					d->SetPhase(PHASE_CLOSE);
				}

				if (buf_size)
				{
					sys_log(0, "TEEN::Send(size %d sock_buf %d ret %d)", buf_size, sock_buf_size, ret);
				}
			}
			break;

		case FDW_EOF:
		{
			d->SetPhase(PHASE_CLOSE);
		}
		break;

		default:
			sys_err("fdwatch_check_event returned unknown %d", iRet);
			d->SetPhase(PHASE_CLOSE);
			break;
		}
	}

	return 1;
}

