#include "stdafx.h"
#include "constants.h"
#include "../../common/teen_packet.h"
#include "config.h"
#include "utils.h"
#include "input.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "cmd.h"
#include "buffer_manager.h"
#include "protocol.h"
#include "pvp.h"
#include "start_position.h"
#include "messenger_manager.h"
#include "guild_manager.h"
#include "party.h"
#include "dungeon.h"
#include "war_map.h"
#include "questmanager.h"
#include "building.h"
#include "wedding.h"
#include "affect.h"
#include "arena.h"
#include "OXEvent.h"
#include "priv_manager.h"
#include "block_country.h"
#include "dev_log.h"
#include "log.h"
#include "horsename_manager.h"
#include "MarkManager.h"
#include "HackShield.h"
#include "XTrapManager.h"
#include "../../common/CommonDefines.h"

#ifdef __ENABLE_SWITCHBOT__
	#include "switchbot.h"
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	#include "EventManager.hpp"
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	#include "TechnicalMaintenance.hpp"
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	#include "ZuoEvent.hpp"
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	#include "message_offline.h"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
	#include "gm.h"
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	#include "ShipDefendDungeonManager.hpp"
	#include "ShipDefendDungeon.hpp"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	#include "find_letters_event.h"
#endif

#ifdef __ENABLE_SAVE_POSITION__
	#include "PositionsManagerHelper.hpp"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __DUNGEON_RETURN_ENABLE__
	#include "p2p.h"
#endif

#ifdef __ITEM_SHOP_ENABLE__
	#include "ItemShopManager.hpp"
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	#include "TombolaManager.hpp"
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __DUNGEON_INFO_ENABLE__
	#include "DungeonInfoManager.hpp"
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	#include "TeleportManagerSystem.hpp"
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	#include "AmuletSystemHelper.hpp"
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

#ifdef __ENABLE_WOLFMAN_CHARACTER__
// #define USE_LYCAN_CREATE_POSITION
#ifdef USE_LYCAN_CREATE_POSITION
DWORD g_lycan_create_position[4][2] =
{
	{		0,		0 },
	{ 768000 + 38300, 896000 + 35500 },
	{ 819200 + 38300, 896000 + 35500 },
	{ 870400 + 38300, 896000 + 35500 },
};

inline DWORD LYCAN_CREATE_START_X(BYTE e, BYTE job)
{
	if (1 <= e && e <= 3)
	{
		return (job == JOB_WOLFMAN) ? g_lycan_create_position[e][0] : g_create_position[e][0];
	}
	return 0;
}

inline DWORD LYCAN_CREATE_START_Y(BYTE e, BYTE job)
{
	if (1 <= e && e <= 3)
	{
		return (job == JOB_WOLFMAN) ? g_lycan_create_position[e][1] : g_create_position[e][1];
	}
	return 0;
}

#endif


#endif

static void _send_bonus_info(LPCHARACTER ch)
{
	int	item_drop_bonus = 0;
	int gold_drop_bonus = 0;
	int gold10_drop_bonus	= 0;
	int exp_bonus		= 0;

	item_drop_bonus		= CPrivManager::instance().GetPriv(ch, PRIV_ITEM_DROP);
	gold_drop_bonus		= CPrivManager::instance().GetPriv(ch, PRIV_GOLD_DROP);
	gold10_drop_bonus	= CPrivManager::instance().GetPriv(ch, PRIV_GOLD10_DROP);
	exp_bonus			= CPrivManager::instance().GetPriv(ch, PRIV_EXP_PCT);

	if (item_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE,
					   LC_TEXT("The quota of the Item Drop is at the moment plus %d%%"), item_drop_bonus);
	}
	if (gold_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE,
					   LC_TEXT("The quota of the Yang Drop is at the moment plus %d%%"), gold_drop_bonus);
	}
	if (gold10_drop_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE,
					   LC_TEXT("The drop rate for Yang rain is currently %d%% higher."), gold10_drop_bonus);
	}
	if (exp_bonus)
	{
		ch->ChatPacket(CHAT_TYPE_NOTICE,
					   LC_TEXT("The experience bonus is currently an additional %d%%."), exp_bonus);
	}
}

static bool FN_is_battle_zone(LPCHARACTER ch)
{
	switch (ch->GetMapIndex())
	{
	case 1:
	case 2:
	case 21:
	case 23:
	case 41:
	case 43:
	case 113:
		return false;
	}

	return true;
}

void CInputLogin::Login(LPDESC d, const char * data)
{
	TPacketCGLogin * pinfo = (TPacketCGLogin *) data;

	char login[LOGIN_MAX_LEN + 1];
	trim_and_lower(pinfo->login, login, sizeof(login));

	sys_log(0, "InputLogin::Login : %s", login);

	TPacketGCLoginFailure failurePacket;

	if (!test_server)
	{
		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "VERSION", sizeof(failurePacket.szStatus));
		d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
		return;
	}

	if (g_bNoMoreClient)
	{
		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "SHUTDOWN", sizeof(failurePacket.szStatus));
		d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
		return;
	}

	if (g_iUserLimit > 0)
	{
		int iTotal;
		int * paiEmpireUserCount;
		int iLocal;

		DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

		if (g_iUserLimit <= iTotal)
		{
			failurePacket.header = HEADER_GC_LOGIN_FAILURE;
			strlcpy(failurePacket.szStatus, "FULL", sizeof(failurePacket.szStatus));
			d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
			return;
		}
	}

	TLoginPacket login_packet;

	strlcpy(login_packet.login, login, sizeof(login_packet.login));
	strlcpy(login_packet.passwd, pinfo->passwd, sizeof(login_packet.passwd));

	db_clientdesc->DBPacket(HEADER_GD_LOGIN, d->GetHandle(), &login_packet, sizeof(TLoginPacket));
}

void CInputLogin::LoginByKey(LPDESC d, const char * data)
{
	TPacketCGLogin2 * pinfo = (TPacketCGLogin2 *) data;

	char login[LOGIN_MAX_LEN + 1];
	trim_and_lower(pinfo->login, login, sizeof(login));

	// is blocked ip?
	{
		dev_log(LOG_DEB0, "check_blocked_country_start");

		if (!is_block_exception(login) && is_blocked_country_ip(d->GetHostName()))
		{
			sys_log(0, "BLOCK_COUNTRY_IP (%s)", d->GetHostName());
			d->SetPhase(PHASE_CLOSE);
			return;
		}

		dev_log(LOG_DEB0, "check_blocked_country_end");
	}

	if (g_bNoMoreClient)
	{
		TPacketGCLoginFailure failurePacket;

		failurePacket.header = HEADER_GC_LOGIN_FAILURE;
		strlcpy(failurePacket.szStatus, "SHUTDOWN", sizeof(failurePacket.szStatus));
		d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
		return;
	}

	if (g_iUserLimit > 0)
	{
		int iTotal;
		int * paiEmpireUserCount;
		int iLocal;

		DESC_MANAGER::instance().GetUserCount(iTotal, &paiEmpireUserCount, iLocal);

		if (g_iUserLimit <= iTotal)
		{
			TPacketGCLoginFailure failurePacket;

			failurePacket.header = HEADER_GC_LOGIN_FAILURE;
			strlcpy(failurePacket.szStatus, "FULL", sizeof(failurePacket.szStatus));

			d->Packet(&failurePacket, sizeof(TPacketGCLoginFailure));
			return;
		}
	}

	sys_log(0, "LOGIN_BY_KEY: %s key %u", login, pinfo->dwLoginKey);

	d->SetLoginKey(pinfo->dwLoginKey);
#ifndef _IMPROVED_PACKET_ENCRYPTION_
	d->SetSecurityKey(pinfo->adwClientKey);
#endif
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	d->SetLocale(pinfo->locale);
#endif
	TPacketGDLoginByKey ptod;

	strlcpy(ptod.szLogin, login, sizeof(ptod.szLogin));
	ptod.dwLoginKey = pinfo->dwLoginKey;
	thecore_memcpy(ptod.adwClientKey, pinfo->adwClientKey, sizeof(DWORD) * 4);
	strlcpy(ptod.szIP, d->GetHostName(), sizeof(ptod.szIP));

	db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, d->GetHandle(), &ptod, sizeof(TPacketGDLoginByKey));
}

void CInputLogin::ChangeName(LPDESC d, const char * data)
{
	TPacketCGChangeName * p = (TPacketCGChangeName *) data;
	const TAccountTable & c_r = d->GetAccountTable();

	if (!c_r.id)
	{
		sys_err("no account table");
		return;
	}

	if (!c_r.players[p->index].bChangeName)
	{
		return;
	}

	if (!check_name(p->name))
	{
		TPacketGCCreateFailure pack;
		pack.header = HEADER_GC_CHARACTER_CREATE_FAILURE;
		pack.bType = 0;
		d->Packet(&pack, sizeof(pack));
		return;
	}

	TPacketGDChangeName pdb;

	pdb.pid = c_r.players[p->index].dwID;
	strlcpy(pdb.name, p->name, sizeof(pdb.name));
	db_clientdesc->DBPacket(HEADER_GD_CHANGE_NAME, d->GetHandle(), &pdb, sizeof(TPacketGDChangeName));
}

void CInputLogin::CharacterSelect(LPDESC d, const char * data)
{
	struct command_player_select * pinfo = (struct command_player_select *) data;
	const TAccountTable & c_r = d->GetAccountTable();

	sys_log(0, "player_select: login: %s index: %d", c_r.login, pinfo->index);

	if (!c_r.id)
	{
		sys_err("no account table");
		return;
	}

	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("index overflow %d, login: %s", pinfo->index, c_r.login);
		return;
	}

	if (c_r.players[pinfo->index].bChangeName)
	{
		sys_err("name must be changed idx %d, login %s, name %s",
				pinfo->index, c_r.login, c_r.players[pinfo->index].szName);
		return;
	}

	TPlayerLoadPacket player_load_packet;

	player_load_packet.account_id	= c_r.id;
	player_load_packet.player_id	= c_r.players[pinfo->index].dwID;
	player_load_packet.account_index	= pinfo->index;

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, d->GetHandle(), &player_load_packet, sizeof(TPlayerLoadPacket));
}

bool NewPlayerTable(TPlayerTable * table,
					const char * name,
					BYTE job,
					BYTE shape,
					BYTE bEmpire,
					BYTE bCon,
					BYTE bInt,
					BYTE bStr,
					BYTE bDex)
{
	if (job >= JOB_MAX_NUM)
	{
		return false;
	}

	memset(table, 0, sizeof(TPlayerTable));

	strlcpy(table->name, name, sizeof(table->name));

	table->level = 1;
	table->job = job;
	table->voice = 0;
	table->part_base = shape;

	table->st = JobInitialPoints[job].st;
	table->dx = JobInitialPoints[job].dx;
	table->ht = JobInitialPoints[job].ht;
	table->iq = JobInitialPoints[job].iq;

	table->hp = JobInitialPoints[job].max_hp + table->ht * JobInitialPoints[job].hp_per_ht;
	table->sp = JobInitialPoints[job].max_sp + table->iq * JobInitialPoints[job].sp_per_iq;
	table->stamina = JobInitialPoints[job].max_stamina;

#if defined(__ENABLE_WOLFMAN_CHARACTER__) && defined(USE_LYCAN_CREATE_POSITION)
	table->x 	= LYCAN_CREATE_START_X(bEmpire, job) + number(-300, 300);
	table->y 	= LYCAN_CREATE_START_Y(bEmpire, job) + number(-300, 300);
#else
	table->x 	= CREATE_START_X(bEmpire) + number(-300, 300);
	table->y 	= CREATE_START_Y(bEmpire) + number(-300, 300);
#endif
	table->z	= 0;
	table->dir	= 0;
	table->playtime = 0;
	table->gold 	= 0;

	table->skill_group = 0;

	if (china_event_server)
	{
		table->level = 35;

		for (int i = 1; i < 35; ++i)
		{
			int iHP = number(JobInitialPoints[job].hp_per_lv_begin, JobInitialPoints[job].hp_per_lv_end);
			int iSP = number(JobInitialPoints[job].sp_per_lv_begin, JobInitialPoints[job].sp_per_lv_end);
			table->sRandomHP += iHP;
			table->sRandomSP += iSP;
			table->stat_point += 3;
		}

		table->hp += table->sRandomHP;
		table->sp += table->sRandomSP;

		table->gold = 1000000;
	}

	return true;
}

bool RaceToJob(unsigned race, unsigned* ret_job)
{
	*ret_job = 0;

	if (race >= MAIN_RACE_MAX_NUM)
	{
		return false;
	}

	switch (race)
	{
	case MAIN_RACE_WARRIOR_M:
		*ret_job = JOB_WARRIOR;
		break;

	case MAIN_RACE_WARRIOR_W:
		*ret_job = JOB_WARRIOR;
		break;

	case MAIN_RACE_ASSASSIN_M:
		*ret_job = JOB_ASSASSIN;
		break;

	case MAIN_RACE_ASSASSIN_W:
		*ret_job = JOB_ASSASSIN;
		break;

	case MAIN_RACE_SURA_M:
		*ret_job = JOB_SURA;
		break;

	case MAIN_RACE_SURA_W:
		*ret_job = JOB_SURA;
		break;

	case MAIN_RACE_SHAMAN_M:
		*ret_job = JOB_SHAMAN;
		break;

	case MAIN_RACE_SHAMAN_W:
		*ret_job = JOB_SHAMAN;
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case MAIN_RACE_WOLFMAN_M:
		*ret_job = JOB_WOLFMAN;
		break;
#endif
	default:
		return false;
		break;
	}
	return true;
}


bool NewPlayerTable2(TPlayerTable * table, const char * name, BYTE race, BYTE shape, BYTE bEmpire)
{
	if (race >= MAIN_RACE_MAX_NUM)
	{
		sys_err("NewPlayerTable2.OUT_OF_RACE_RANGE(%d >= max(%d))\n", race, MAIN_RACE_MAX_NUM);
		return false;
	}

	unsigned job;

	if (!RaceToJob(race, &job))
	{
		sys_err("NewPlayerTable2.RACE_TO_JOB_ERROR(%d)\n", race);
		return false;
	}

	sys_log(0, "NewPlayerTable2(name=%s, race=%d, job=%d)", name, race, job);

	memset(table, 0, sizeof(TPlayerTable));

	strlcpy(table->name, name, sizeof(table->name));

	table->level		= 1;
	table->job			= race;
	table->voice		= 0;
	table->part_base	= shape;

	table->st		= JobInitialPoints[job].st;
	table->dx		= JobInitialPoints[job].dx;
	table->ht		= JobInitialPoints[job].ht;
	table->iq		= JobInitialPoints[job].iq;

	table->hp		= JobInitialPoints[job].max_hp + table->ht * JobInitialPoints[job].hp_per_ht;
	table->sp		= JobInitialPoints[job].max_sp + table->iq * JobInitialPoints[job].sp_per_iq;
	table->stamina	= JobInitialPoints[job].max_stamina;

#if defined(__ENABLE_WOLFMAN_CHARACTER__) && defined(USE_LYCAN_CREATE_POSITION)
	table->x 		= LYCAN_CREATE_START_X(bEmpire, job) + number(-300, 300);
	table->y 		= LYCAN_CREATE_START_Y(bEmpire, job) + number(-300, 300);
#else
	table->x		= CREATE_START_X(bEmpire) + number(-300, 300);
	table->y		= CREATE_START_Y(bEmpire) + number(-300, 300);
#endif
	table->z		= 0;
	table->dir		= 0;
	table->playtime = 0;
	table->gold 	= 0;

	table->skill_group = 0;

	return true;
}

void CInputLogin::CharacterCreate(LPDESC d, const char * data)
{
	struct command_player_create * pinfo = (struct command_player_create *) data;
	TPlayerCreatePacket player_create_packet;

	sys_log(0, "PlayerCreate: name %s pos %d job %d shape %d",
			pinfo->name,
			pinfo->index,
			pinfo->job,
			pinfo->shape);

	TPacketGCLoginFailure packFailure;
	memset(&packFailure, 0, sizeof(packFailure));
	packFailure.header = HEADER_GC_CHARACTER_CREATE_FAILURE;

	if (true == g_BlockCharCreation)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

#ifdef __ENABLE_BLOCK_MORE_THAN_12_CHARACTERS__
	if (strlen(pinfo->name) > 12)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}
#endif

#ifdef __ADMIN_MANAGER__
	if (!GM::check_account_allow(d->GetAccountTable().login, GM_ALLOW_CREATE_PLAYER))
	{
		sys_err("gm may not create a character");
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}
#endif

	if (!check_name(pinfo->name) || pinfo->shape > 1)
	{
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	const TAccountTable & c_rAccountTable = d->GetAccountTable();

	if (0 == strcmp(c_rAccountTable.login, pinfo->name))
	{
		TPacketGCCreateFailure pack;
		pack.header = HEADER_GC_CHARACTER_CREATE_FAILURE;
		pack.bType = 1;

		d->Packet(&pack, sizeof(pack));
		return;
	}

	memset(&player_create_packet, 0, sizeof(TPlayerCreatePacket));

	if (!NewPlayerTable2(&player_create_packet.player_table, pinfo->name, pinfo->job, pinfo->shape, d->GetEmpire()))
	{
		sys_err("player_prototype error: job %d face %d ", pinfo->job);
		d->Packet(&packFailure, sizeof(packFailure));
		return;
	}

	trim_and_lower(c_rAccountTable.login, player_create_packet.login, sizeof(player_create_packet.login));
	strlcpy(player_create_packet.passwd, c_rAccountTable.passwd, sizeof(player_create_packet.passwd));

	player_create_packet.account_id	= c_rAccountTable.id;
	player_create_packet.account_index	= pinfo->index;

#ifdef __EXTANDED_GOLD_AMOUNT__
	sys_log(0, "PlayerCreate: name %s account_id %d, slot %d, TPlayerCreatePacketSize(%d), Packet->Gold %lld",
#else
	sys_log(0, "PlayerCreate: name %s account_id %d, TPlayerCreatePacketSize(%d), Packet->Gold %d",
#endif
			pinfo->name,
			pinfo->index,
			sizeof(TPlayerCreatePacket),
			player_create_packet.player_table.gold);

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_CREATE, d->GetHandle(), &player_create_packet, sizeof(TPlayerCreatePacket));
}

void CInputLogin::CharacterDelete(LPDESC d, const char * data)
{
	struct command_player_delete * pinfo = (struct command_player_delete *) data;
	const TAccountTable & c_rAccountTable = d->GetAccountTable();

	if (!c_rAccountTable.id)
	{
		sys_err("PlayerDelete: no login data");
		return;
	}

	sys_log(0, "PlayerDelete: login: %s index: %d, social_id %s", c_rAccountTable.login, pinfo->index, pinfo->private_code);

	if (pinfo->index >= PLAYER_PER_ACCOUNT)
	{
		sys_err("PlayerDelete: index overflow %d, login: %s", pinfo->index, c_rAccountTable.login);
		return;
	}

	if (!c_rAccountTable.players[pinfo->index].dwID)
	{
		sys_err("PlayerDelete: Wrong Social ID index %d, login: %s", pinfo->index, c_rAccountTable.login);
		d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
		return;
	}

#ifdef __ADMIN_MANAGER__
	if (!GM::check_allow(GM::get_level(c_rAccountTable.players[pinfo->index].szName, c_rAccountTable.login), GM_ALLOW_DELETE_PLAYER))
	{
		sys_err("PlayerDelete: cannot delete gm character [%s]", c_rAccountTable.players[pinfo->index].szName);
		d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
		return;
	}
#endif

	// Support for marriage and guild leader
	{
		MYSQL_ROW row = nullptr;
		MYSQL_RES* pRes = nullptr;
		std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM player%s.marriage WHERE pid1 = %u or pid2 = %u", get_table_postfix(), c_rAccountTable.players[pinfo->index].dwID));

		pRes = msg->Get()->pSQLResult;
		if (msg->Get()->uiNumRows)
		{
			row = mysql_fetch_row(pRes);
			if (atoi(row[0]))
			{
				sys_err("PlayerDelete: Is married index %d, login: %s", pinfo->index, c_rAccountTable.login);
				d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
				return;
			}
		}

		msg.reset(DBManager::instance().DirectQuery("SELECT COUNT(*) FROM player%s.guild WHERE master = %u", get_table_postfix(), c_rAccountTable.players[pinfo->index].dwID));

		pRes = msg->Get()->pSQLResult;
		if (msg->Get()->uiNumRows)
		{
			row = mysql_fetch_row(pRes);
			if (atoi(row[0]))
			{
				sys_err("PlayerDelete: Is guild leader index %d, login: %s", pinfo->index, c_rAccountTable.login);
				d->Packet(encode_byte(HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID), 1);
				return;
			}
		}
	}

	TPlayerDeletePacket	player_delete_packet;

	trim_and_lower(c_rAccountTable.login, player_delete_packet.login, sizeof(player_delete_packet.login));
	player_delete_packet.player_id	= c_rAccountTable.players[pinfo->index].dwID;
	player_delete_packet.account_index	= pinfo->index;
	strlcpy(player_delete_packet.private_code, pinfo->private_code, sizeof(player_delete_packet.private_code));

	db_clientdesc->DBPacket(HEADER_GD_PLAYER_DELETE, d->GetHandle(), &player_delete_packet, sizeof(TPlayerDeletePacket));
}

#pragma pack(1)
typedef struct SPacketGTLogin
{
	BYTE header;
	WORD empty;
	DWORD id;
} TPacketGTLogin;
#pragma pack()

void CInputLogin::Entergame(LPDESC d, const char * data)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	if (d && strcmp(d->GetAccountTable().status, "OK") != 0)
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	PIXEL_POSITION pos = ch->GetXYZ();

	if (!SECTREE_MANAGER::instance().GetMovablePosition(ch->GetMapIndex(), pos.x, pos.y, pos))
	{
		PIXEL_POSITION pos2;
		SECTREE_MANAGER::instance().GetRecallPositionByEmpire(ch->GetMapIndex(), ch->GetEmpire(), pos2);

		sys_err("!GetMovablePosition (name %s %dx%d map %d changed to %dx%d)",
				ch->GetName(),
				pos.x, pos.y,
				ch->GetMapIndex(),
				pos2.x, pos2.y);
		pos = pos2;
	}

	CGuildManager::instance().LoginMember(ch);

	ch->Show(ch->GetMapIndex(), pos.x, pos.y, pos.z);

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	ch->CheckMountStartToggleItem();
#endif

#if !defined(__BINARY_ATLAS_MARK_INFO__) && !defined(__MULTI_LANGUAGE_SYSTEM__)
	SECTREE_MANAGER::instance().SendNPCPosition(ch);
#endif

	ch->ReviveInvisible(5);

	d->SetPhase(PHASE_GAME);

	if (ch->GetItemAward_cmd())
	{
		quest::CQuestManager::instance().ItemInformer(ch->GetPlayerID(), ch->GetItemAward_vnum());
	}

	sys_log(0, "ENTERGAME: %s %dx%dx%d %s map_index %d",
			ch->GetName(), ch->GetX(), ch->GetY(), ch->GetZ(), d->GetHostName(), ch->GetMapIndex());

	if (ch->GetHorseLevel() > 0)
	{
		ch->EnterHorse();
	}


#ifdef __ENABLE_TEAMLER_STATUS__
	if (ch->IsGM())
	{
		if (ch->GetGMLevel() <= GM_GOD)
		{
			ch->SetIsShowTeamler(true);
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are shown in the teamler list."));
		}
	}

	if (ch->IsPC())
	{
		CHARACTER_MANAGER::instance().SendOnlineTeamlerList(d);
	}
#endif

	ch->ResetPlayTime();


	ch->StartSaveEvent();
	ch->StartRecoveryEvent();
	ch->StartCheckSpeedHackEvent();

#ifdef __ABUSE_CONTROLLER_ENABLE__
	ch->SetLastMoveAblePosition(ch->GetXYZ());
	ch->SetLastMoveableMapIndex();

	ch->StartCheckWallhack();
#endif

	CPVPManager::instance().Connect(ch);
	CPVPManager::instance().SendList(d);

	MessengerManager::instance().Login(ch->GetName());

	CPartyManager::instance().SetParty(ch);
	CGuildManager::instance().SendGuildWar(ch);

	building::CManager::instance().SendLandList(d, ch->GetMapIndex());

	marriage::CManager::instance().Login(ch);

#ifdef __ADMIN_MANAGER__
	CAdminManager::instance().OnLoginPlayer(ch);
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	CTechnicalMaintenance::instance().BroadcastMaintenanceInfo(ch);
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	CZuoEventManager::instance().RegisterNewPlayer(ch);
	if (CZuoEventManager::GetEventMapIndex() == ch->GetMapIndex())
	{
		CZuoEventManager::instance().BroadcastEventInfo((ch->GetGMLevel() == GM_IMPLEMENTOR) ? EBroadcastType::BROADCAST_ALL : EBroadcastType::BROADCAST_TIME);
	}
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	CEventManager::instance().SendEventList(ch);
	if (ch->GetGMLevel() == GM_IMPLEMENTOR)
	{
		CEventManager::instance().RefreshImplementorPanel(ch);
	}
#endif

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().BroadcastBattlePass(ch);
#endif

#ifdef __DUNGEON_INFO_ENABLE__
	CDungeonInfoManager::instance().BroadcastConfig(ch);
#endif

#ifdef __ENABLE_MISSION_MANAGER__
	CMissionManager::instance().BroadcastMission(ch);
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	COfflineShop::Login(ch);
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	TombolaEvent::BroadcastConfig(ch);
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	LegendaryStonesHandler::EmitPassiveRequires(ch);
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	CAttendanceRewardManager::instance().Broadcast(ch);
#endif

	TPacketGCTime p;
	p.bHeader = HEADER_GC_TIME;
	p.time = get_global_time();
	d->Packet(&p, sizeof(p));

	TPacketGCChannel p2;
	p2.header = HEADER_GC_CHANNEL;
	p2.channel = g_bChannel;
	d->Packet(&p2, sizeof(p2));

	ch->SendGreetMessage();

#ifdef __ENABLE_TELEPORT_SYSTEM__
	auto bValidate = CTeleportManager::instance().ValidateMap(ch, ch->GetMapIndex());
	if (!bValidate)
	{
		ch->GoHome();
	}
#endif

#ifdef __ITEM_SHOP_ENABLE__
	ch->SetVolatileCoinValue(CItemShopManager::instance().GetPlayerCash(ch));
#endif

	_send_bonus_info(ch);

	for (int i = 0; i <= PREMIUM_MAX_NUM; ++i)
	{
		int remain = ch->GetPremiumRemainSeconds(i);

		if (remain <= 0)
		{
			continue;
		}

		ch->AddAffect(AFFECT_PREMIUM_START + i, POINT_NONE, 0, 0, remain, 0, true);
		sys_log(0, "PREMIUM: %s type %d %dmin", ch->GetName(), i, remain);
	}

	if (g_bCheckClientVersion)
	{
		sys_log(0, "VERSION CHECK %s %s", g_stClientVersion.c_str(), d->GetClientVersion());

		if (!d->GetClientVersion())
		{
			d->DelayedDisconnect(10);
		}
		else
		{
			if (0 != g_stClientVersion.compare(d->GetClientVersion())) // @fixme103 (version > date)
			{
				ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("You do not have the correct client version. Please install the normal patch."));
				d->DelayedDisconnect(0); // @fixme103 (10);
				LogManager::instance().HackLog("VERSION_CONFLICT", ch);

				sys_log(0, "VERSION : WRONG VERSION USER : account:%s name:%s hostName:%s server_version:%s client_version:%s",
						d->GetAccountTable().login,
						ch->GetName(),
						d->GetHostName(),
						g_stClientVersion.c_str(),
						d->GetClientVersion());
			}
		}
	}
	else
	{
		sys_log(0, "VERSION : NO CHECK");
	}

	if (ch->IsGM() == true)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ConsoleEnable");
	}

	if (ch->GetMapIndex() >= 10000)
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()))
		{
			ch->SetWarMap(CWarMapManager::instance().Find(ch->GetMapIndex()));
		}
		else if (marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
		{
			ch->SetWeddingMap(marriage::WeddingManager::instance().Find(ch->GetMapIndex()));
		}
		else
		{
			ch->SetDungeon(CDungeonManager::instance().FindByMapIndex(ch->GetMapIndex()));
#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
			if (ch->GetDungeon() && ch->GetParty() && ch->GetParty()->GetLeaderPID() == ch->GetPlayerID())
			{
				ch->GetDungeon()->RegisterPartyForDungeon(ch->GetParty());
			}
#endif
		}
	}
	else if (CArenaManager::instance().IsArenaMap(ch->GetMapIndex()) == true)
	{
		int memberFlag = CArenaManager::instance().IsMember(ch->GetMapIndex(), ch->GetPlayerID());
		if (memberFlag == MEMBER_OBSERVER)
		{
			ch->SetObserverMode(true);
			ch->SetArenaObserverMode(true);
			if (CArenaManager::instance().RegisterObserverPtr(ch, ch->GetMapIndex(), ch->GetX() / 100, ch->GetY() / 100))
			{
				sys_log(0, "ARENA : Observer add failed");
			}

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}
		}
		else if (memberFlag == MEMBER_DUELIST)
		{
			TPacketGCDuelStart duelStart;
			duelStart.header = HEADER_GC_DUEL_START;
			duelStart.wSize = sizeof(TPacketGCDuelStart);

			ch->GetDesc()->Packet(&duelStart, sizeof(TPacketGCDuelStart));

			if (ch->IsHorseRiding() == true)
			{
				ch->StopRiding();
				ch->HorseSummon(false);
			}

			LPPARTY pParty = ch->GetParty();
			if (pParty != NULL)
			{
				if (pParty->GetMemberCount() == 2)
				{
					CPartyManager::instance().DeleteParty(pParty);
				}
				else
				{
					pParty->Quit(ch->GetPlayerID());
				}
			}
		}
		else if (memberFlag == MEMBER_NO)
		{
			if (ch->GetGMLevel() == GM_PLAYER)
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
			}
		}
		else
		{
			// wtf
		}
	}
	else if (ch->GetMapIndex() == OXEVENT_MAP_INDEX)
	{

		if (COXEventManager::instance().Enter(ch) == false)
		{

			if (ch->GetGMLevel() == GM_PLAYER)
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
			}
		}

#ifdef __OX_EVENT_SYSTEM_ENABLE__
		if (ch->GetGMLevel() > GM_PLAYER)
		{
			COXEventManager::instance().AddSuperVisior(ch->GetPlayerID());
			COXEventManager::instance().BroadcastPoolInfo();
		}
#endif
	}
	else
	{
		if (CWarMapManager::instance().IsWarMap(ch->GetMapIndex()) ||
				marriage::WeddingManager::instance().IsWeddingMap(ch->GetMapIndex()))
		{
			if (!test_server)
			{
				ch->WarpSet(EMPIRE_START_X(ch->GetEmpire()), EMPIRE_START_Y(ch->GetEmpire()));
			}
		}
	}


	if (g_TeenDesc)
	{
		TPacketGTLogin p;

		p.header = HEADER_GT_LOGIN;
		p.empty = 0;
		p.id = d->GetAccountTable().id;

		g_TeenDesc->Packet(&p, sizeof(p));
		sys_log(0, "TEEN_SEND: (%u, %s)", d->GetAccountTable().id, ch->GetName());
	}

	if (ch->GetHorseLevel() > 0)
	{
		DWORD pid = ch->GetPlayerID();

		if (pid != 0 && CHorseNameManager::instance().GetHorseName(pid) == NULL)
		{
			db_clientdesc->DBPacket(HEADER_GD_REQ_HORSE_NAME, 0, &pid, sizeof(DWORD));
		}

		// @fixme182 BEGIN
		ch->SetHorseLevel(ch->GetHorseLevel());
		ch->SkillLevelPacket();
		// @fixme182 END
	}


	if (g_noticeBattleZone)
	{
		if (FN_is_battle_zone(ch))
		{
			ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("이 맵에선 강제적인 대전이 있을수 도 있습니다."));
			ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("이 조항에 동의하지 않을시"));
			ch->ChatPacket(CHAT_TYPE_NOTICE, LC_TEXT("본인의 주성 및 부성으로 돌아가시기 바랍니다."));
		}
	}

#ifdef __ENABLE_SWITCHBOT__
	CSwitchbotManager::Instance().EnterGame(ch);
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
	ch->ChatPacket(CHAT_TYPE_COMMAND, "UpdateChannelInfo %d", g_bChannel);
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	COfflineMessage::instance().SendMessage(ch);
#endif

#ifdef __SHIP_DUNGEON_ENABLE__
	if (CShipDefendDungeonManager::instance().FindDungeonByPID(ch) && CShipDefendDungeonManager::instance().FindDungeonByPID(ch)->GetDungeon()->GetMapIndex() == ch->GetMapIndex())
	{
		CShipDefendDungeonManager::instance().FindDungeonByPID(ch)->BroadcastInitStatus(ch);
	}
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	// Because is hide by default, so why to send when the event is not active
	if (quest::CQuestManager::instance().GetEventFlag("enable_find_letters") != 0)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "FindLettersIcon 1");
	}
#endif

#ifdef __ENABLE_SAVE_POSITION__
	PositionsManagerHelper::instance().EmitPositions(ch);
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	BeginnerHelper::instance().Broadcast(ch);
	BeginnerHelper::instance().Collect(ch);
#endif

#ifdef __ENABLE_SKILLS_INFORMATION__
	ch->BroadcastSkillInformation();
#endif

#ifdef __DUNGEON_RETURN_ENABLE__
	if (ch->GetQuestFlag("dungeon_return.index"))
	{
		if (!CDungeonManager::instance().FindDungeonByPID(ch->GetPlayerID()))
#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
		{
			TPacketGGCreateDungeonInstance p{};
			p.bHeader = HEADER_GG_CHECK_REJOIN_DUNGEON;
			p.bRequest = true;
			p.lMapIndex = static_cast<long>(ch->GetQuestFlag("dungeon_return.index")) / 10000;
			p.aPids[0] = ch->GetPlayerID();

			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGCreateDungeonInstance));
		}
#else
			;
#endif
		else
		{
			ch->SetDungeonReturn(true);
			quest::CQuestManager::instance().Letter(ch->GetPlayerID(), quest::CQuestManager::instance().GetQuestIndexByName("dungeon_teleport"), 0);
		}
	}
#endif

#ifdef __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
	ch->BroadcastEquipmentLockedSlot();
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
	ch->BroadcastHCostume();
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	AmuletSystemHelper::BroadcastPassiveInformation(ch);
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	ch->SendAutoShamanInformations();
#endif
}

void CInputLogin::Empire(LPDESC d, const char * c_pData)
{
	const TPacketCGEmpire* p = reinterpret_cast<const TPacketCGEmpire*>(c_pData);

	if (EMPIRE_MAX_NUM <= p->bEmpire)
	{
		d->SetPhase(PHASE_CLOSE);
		return;
	}

	const TAccountTable& r = d->GetAccountTable();

	if (r.bEmpire != 0)
	{
		for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		{
			if (0 != r.players[i].dwID)
			{
				sys_err("EmpireSelectFailed %d", r.players[i].dwID);
				return;
			}
		}
	}

	TEmpireSelectPacket pd;

	pd.dwAccountID = r.id;
	pd.bEmpire = p->bEmpire;

	db_clientdesc->DBPacket(HEADER_GD_EMPIRE_SELECT, d->GetHandle(), &pd, sizeof(pd));
}

int CInputLogin::GuildSymbolUpload(LPDESC d, const char* c_pData, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuildSymbolUpload))
	{
		return -1;
	}

	sys_log(0, "GuildSymbolUpload uiBytes %u", uiBytes);

	TPacketCGGuildSymbolUpload* p = (TPacketCGGuildSymbolUpload*) c_pData;

	if (uiBytes < p->size)
	{
		return -1;
	}

	int iSymbolSize = p->size - sizeof(TPacketCGGuildSymbolUpload);

	if (iSymbolSize <= 0 || iSymbolSize > 64 * 1024)
	{


		d->SetPhase(PHASE_CLOSE);
		return 0;
	}


	if (!test_server)
		if (!building::CManager::instance().FindLandByGuild(p->guild_id))
		{
			d->SetPhase(PHASE_CLOSE);
			return 0;
		}

	sys_log(0, "GuildSymbolUpload Do Upload %02X%02X%02X%02X %d", c_pData[7], c_pData[8], c_pData[9], c_pData[10], sizeof(*p));

	CGuildMarkManager::instance().UploadSymbol(p->guild_id, iSymbolSize, (const BYTE*)(c_pData + sizeof(*p)));
	CGuildMarkManager::instance().SaveSymbol(GUILD_SYMBOL_FILENAME);
	return iSymbolSize;
}

void CInputLogin::GuildSymbolCRC(LPDESC d, const char* c_pData)
{
	const TPacketCGSymbolCRC & CGPacket = *((TPacketCGSymbolCRC *) c_pData);

	sys_log(0, "GuildSymbolCRC %u %u %u", CGPacket.guild_id, CGPacket.crc, CGPacket.size);

	const CGuildMarkManager::TGuildSymbol * pkGS = CGuildMarkManager::instance().GetGuildSymbol(CGPacket.guild_id);

	if (!pkGS)
	{
		return;
	}

	sys_log(0, "  Server %u %u", pkGS->crc, pkGS->raw.size());

	if (pkGS->raw.size() != CGPacket.size || pkGS->crc != CGPacket.crc)
	{
		TPacketGCGuildSymbolData GCPacket;

		GCPacket.header = HEADER_GC_SYMBOL_DATA;
		GCPacket.size = sizeof(GCPacket) + pkGS->raw.size();
		GCPacket.guild_id = CGPacket.guild_id;

		d->BufferedPacket(&GCPacket, sizeof(GCPacket));
		d->Packet(&pkGS->raw[0], pkGS->raw.size());

		sys_log(0, "SendGuildSymbolHead %02X%02X%02X%02X Size %d",
				pkGS->raw[0], pkGS->raw[1], pkGS->raw[2], pkGS->raw[3], pkGS->raw.size());
	}
}

void CInputLogin::GuildMarkUpload(LPDESC d, const char* c_pData)
{
	TPacketCGMarkUpload * p = (TPacketCGMarkUpload *) c_pData;
	CGuildManager& rkGuildMgr = CGuildManager::instance();
	CGuild * pkGuild;

	if (!(pkGuild = rkGuildMgr.FindGuild(p->gid)))
	{
		sys_err("MARK_SERVER: GuildMarkUpload: no guild. gid %u", p->gid);
		return;
	}

	if (pkGuild->GetMasterCharacter())
	{
		auto iCurrentFlag = quest::CQuestManager::Instance().GetEventFlag(fmt::format("register_mark_{}", pkGuild->GetMasterCharacter()->GetGuild()->GetID()));
		if (iCurrentFlag == 0)
		{
			return;
		}

		quest::CQuestManager::Instance().RequestSetEventFlag(fmt::format("register_mark_{}", pkGuild->GetMasterCharacter()->GetGuild()->GetID()), 0);
		quest::CQuestManager::Instance().SetEventFlag(fmt::format("register_mark_{}", pkGuild->GetMasterCharacter()->GetGuild()->GetID()), 0);
	}

	if (pkGuild->GetLevel() < guild_mark_min_level)
	{
		sys_log(0, "MARK_SERVER: GuildMarkUpload: level < %u (%u)", guild_mark_min_level, pkGuild->GetLevel());
		return;
	}

	CGuildMarkManager & rkMarkMgr = CGuildMarkManager::instance();

	sys_log(0, "MARK_SERVER: GuildMarkUpload: gid %u", p->gid);

	bool isEmpty = true;

	for (DWORD iPixel = 0; iPixel < SGuildMark::SIZE; ++iPixel)
		if (*((DWORD *) p->image + iPixel) != 0x00000000)
		{
			isEmpty = false;
		}

	if (isEmpty)
	{
		rkMarkMgr.DeleteMark(p->gid);
	}
	else
	{
		rkMarkMgr.SaveMark(p->gid, p->image);
	}
}

void CInputLogin::GuildMarkIDXList(LPDESC d, const char* c_pData)
{
	CGuildMarkManager & rkMarkMgr = CGuildMarkManager::instance();

	DWORD bufSize = sizeof(WORD) * 2 * rkMarkMgr.GetMarkCount();
	char * buf = NULL;

	if (bufSize > 0)
	{
		buf = (char *) malloc(bufSize);
		rkMarkMgr.CopyMarkIdx(buf);
	}

	TPacketGCMarkIDXList p;
	p.header = HEADER_GC_MARK_IDXLIST;
	p.bufSize = sizeof(p) + bufSize;
	p.count = rkMarkMgr.GetMarkCount();

	if (buf)
	{
		d->BufferedPacket(&p, sizeof(p));
		d->LargePacket(buf, bufSize);
		free(buf);
	}
	else
	{
		d->Packet(&p, sizeof(p));
	}

	sys_log(0, "MARK_SERVER: GuildMarkIDXList %d bytes sent.", p.bufSize);
}

void CInputLogin::GuildMarkCRCList(LPDESC d, const char* c_pData)
{
	TPacketCGMarkCRCList * pCG = (TPacketCGMarkCRCList *) c_pData;

	std::map<BYTE, const SGuildMarkBlock *> mapDiffBlocks;
	CGuildMarkManager::instance().GetDiffBlocks(pCG->imgIdx, pCG->crclist, mapDiffBlocks);

	DWORD blockCount = 0;
	TEMP_BUFFER buf(1024 * 1024);

	for (itertype(mapDiffBlocks) it = mapDiffBlocks.begin(); it != mapDiffBlocks.end(); ++it)
	{
		BYTE posBlock = it->first;
		const SGuildMarkBlock & rkBlock = *it->second;

		buf.write(&posBlock, sizeof(BYTE));
		buf.write(&rkBlock.m_sizeCompBuf, sizeof(DWORD));
		buf.write(rkBlock.m_abCompBuf, rkBlock.m_sizeCompBuf);

		++blockCount;
	}

	TPacketGCMarkBlock pGC;

	pGC.header = HEADER_GC_MARK_BLOCK;
	pGC.imgIdx = pCG->imgIdx;
	pGC.bufSize = buf.size() + sizeof(TPacketGCMarkBlock);
	pGC.count = blockCount;

	sys_log(0, "MARK_SERVER: Sending blocks. (imgIdx %u diff %u size %u)", pCG->imgIdx, mapDiffBlocks.size(), pGC.bufSize);

	if (buf.size() > 0)
	{
		d->BufferedPacket(&pGC, sizeof(TPacketGCMarkBlock));
		d->LargePacket(buf.read_peek(), buf.size());
	}
	else
	{
		d->Packet(&pGC, sizeof(TPacketGCMarkBlock));
	}
}

int CInputLogin::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	int iExtraLen = 0;

	switch (bHeader)
	{
	case HEADER_CG_PONG:
		Pong(d);
		break;

	case HEADER_CG_TIME_SYNC:
		Handshake(d, c_pData);
		break;

	case HEADER_CG_LOGIN:
		Login(d, c_pData);
		break;

	case HEADER_CG_LOGIN2:
		LoginByKey(d, c_pData);
		break;

	case HEADER_CG_CHARACTER_SELECT:
		CharacterSelect(d, c_pData);
		break;

	case HEADER_CG_CHARACTER_CREATE:
		CharacterCreate(d, c_pData);
		break;

	case HEADER_CG_CHARACTER_DELETE:
		CharacterDelete(d, c_pData);
		break;

	case HEADER_CG_ENTERGAME:
		Entergame(d, c_pData);
		break;

	case HEADER_CG_EMPIRE:
		Empire(d, c_pData);
		break;

	case HEADER_CG_MOVE:
		break;

	///////////////////////////////////////
	// Guild Mark
	/////////////////////////////////////
	case HEADER_CG_MARK_CRCLIST:
		GuildMarkCRCList(d, c_pData);
		break;

	case HEADER_CG_MARK_IDXLIST:
		GuildMarkIDXList(d, c_pData);
		break;

	case HEADER_CG_MARK_UPLOAD:
		GuildMarkUpload(d, c_pData);
		break;

	//////////////////////////////////////
	// Guild Symbol
	/////////////////////////////////////
	case HEADER_CG_GUILD_SYMBOL_UPLOAD:
		if ((iExtraLen = GuildSymbolUpload(d, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_SYMBOL_CRC:
		GuildSymbolCRC(d, c_pData);
		break;

	case HEADER_CG_MARK_LOGIN:
		break;
	/////////////////////////////////////

	case HEADER_CG_HACK:
		break;

	case HEADER_CG_CHANGE_NAME:
		ChangeName(d, c_pData);
		break;

	case HEADER_CG_CLIENT_VERSION:
		Version(d->GetCharacter(), c_pData);
		break;

	case HEADER_CG_CLIENT_VERSION2:
		Version(d->GetCharacter(), c_pData);
		break;

	case HEADER_CG_HS_ACK:
		if (isHackShieldEnable)
		{
			CHackShieldManager::instance().VerifyAck(d->GetCharacter(), c_pData);
		}
		break;

	// @fixme120
	case HEADER_CG_ITEM_USE:
	case HEADER_CG_TARGET:
		break;

	case HEADER_CG_XTRAP_ACK:
	{
		TPacketXTrapCSVerify* p = reinterpret_cast<TPacketXTrapCSVerify*>((void*)c_pData);
		CXTrapManager::instance().Verify_CSStep3(d->GetCharacter(), p->bPacketData);
	}
	break;

	default:
		sys_err("login phase does not handle this packet! header %d", bHeader);
		//d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	return (iExtraLen);
}

