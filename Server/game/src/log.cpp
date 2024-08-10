#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "log.h"

#include "char.h"
#include "desc.h"
#include "item.h"
#include "sstream"
#include "db.h"

static char	__escape_hint[1024];

LogManager::LogManager() : m_bIsConnect(false)
{
}

LogManager::~LogManager()
{
}

bool LogManager::Connect(const char * host, const int port, const char * user, const char * pwd, const char * db)
{
	if (m_sql.Setup(host, user, pwd, db, MYSQL_CHARSET, false, port))
	{
		m_bIsConnect = true;
	}

	return m_bIsConnect;
}

void LogManager::Query(const char * c_pszFormat, ...)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);

	char szQuery[4096];
	va_list args;

	va_start(args, c_pszFormat);
	vsnprintf(szQuery, sizeof(szQuery), c_pszFormat, args);
	va_end(args);

	if (test_server)
	{
		sys_log(0, "LOG: %s", szQuery);
	}

	m_sql.AsyncQuery(szQuery);
}

bool LogManager::IsConnected()
{
	return m_bIsConnect;
}

size_t LogManager::EscapeString(char* dst, size_t dstSize, const char *src, size_t srcSize)
{
	return m_sql.EscapeString(dst, dstSize, src, srcSize);
}

void LogManager::ItemLog(DWORD dwPID, DWORD x, DWORD y, DWORD dwItemID, const char * c_pszText, const char * c_pszHint, const char * c_pszIP, DWORD dwVnum)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), c_pszHint, strlen(c_pszHint));

	Query("INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint, ip, vnum) VALUES('ITEM', NOW(), %u, %u, %u, %u, '%s', '%s', '%s', %u)",
		  get_table_postfix(), dwPID, x, y, dwItemID, c_pszText, __escape_hint, c_pszIP, dwVnum);
}

void LogManager::ItemLog(LPCHARACTER ch, LPITEM item, const char * c_pszText, const char * c_pszHint)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	if (NULL == ch || NULL == item)
	{
		sys_err("character or item nil (ch %p item %p text %s)", get_pointer(ch), get_pointer(item), c_pszText);
		return;
	}

	ItemLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), item->GetID(),
			NULL == c_pszText ? "" : c_pszText,
			c_pszHint, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "",
			item->GetOriginalVnum());
}

void LogManager::ItemLog(LPCHARACTER ch, int itemID, int itemVnum, const char * c_pszText, const char * c_pszHint)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	ItemLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), itemID, c_pszText, c_pszHint, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "", itemVnum);
}

void LogManager::CharLog(DWORD dwPID, DWORD x, DWORD y, DWORD dwValue, const char * c_pszText, const char * c_pszHint, const char * c_pszIP)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), c_pszHint, strlen(c_pszHint));

	Query("INSERT DELAYED INTO log%s (type, time, who, x, y, what, how, hint, ip) VALUES('CHARACTER', NOW(), %u, %u, %u, %u, '%s', '%s', '%s')",
		  get_table_postfix(), dwPID, x, y, dwValue, c_pszText, __escape_hint, c_pszIP);
}

void LogManager::CharLog(LPCHARACTER ch, DWORD dw, const char * c_pszText, const char * c_pszHint)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	if (ch)
	{
		CharLog(ch->GetPlayerID(), ch->GetX(), ch->GetY(), dw, c_pszText, c_pszHint, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
	}
	else
	{
		CharLog(0, 0, 0, dw, c_pszText, c_pszHint, "");
	}
}

void LogManager::LoginLog(bool isLogin, DWORD dwAccountID, DWORD dwPID, BYTE bLevel, BYTE bJob, DWORD dwPlayTime)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	Query("INSERT DELAYED INTO loginlog%s (type, time, channel, account_id, pid, level, job, playtime) VALUES (%s, NOW(), %d, %u, %u, %d, %d, %u)",
		  get_table_postfix(), isLogin ? "'LOGIN'" : "'LOGOUT'", g_bChannel, dwAccountID, dwPID, bLevel, bJob, dwPlayTime);
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	void LogManager::MoneyLog(BYTE type, DWORD vnum, int64_t gold)
#else
	void LogManager::MoneyLog(BYTE type, DWORD vnum, int gold)
#endif
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	if (type == MONEY_LOG_RESERVED || type >= MONEY_LOG_TYPE_MAX_NUM)
	{
#ifdef __EXTANDED_GOLD_AMOUNT__
		sys_err("TYPE ERROR: type %d vnum %u gold %lld", type, vnum, gold);
#else
		sys_err("TYPE ERROR: type %d vnum %u gold %d", type, vnum, gold);
#endif
		return;
	}

#ifdef __EXTANDED_GOLD_AMOUNT__
	Query("INSERT DELAYED INTO money_log%s VALUES (NOW(), %d, %d, %lld)", get_table_postfix(), type, vnum, gold);
#else
	Query("INSERT DELAYED INTO money_log%s VALUES (NOW(), %d, %d, %d)", get_table_postfix(), type, vnum, gold);
#endif
}

void LogManager::HackLog(const char * c_pszHackName, const char * c_pszLogin, const char * c_pszName, const char * c_pszIP)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), c_pszHackName, strlen(c_pszHackName));

	Query("INSERT INTO hack_log (time, login, name, ip, server, why) VALUES(NOW(), '%s', '%s', '%s', '%s', '%s')", c_pszLogin, c_pszName, c_pszIP, g_stHostname.c_str(), __escape_hint);
}

void LogManager::HackLog(const char * c_pszHackName, LPCHARACTER ch)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	if (ch->GetDesc())
	{
		HackLog(c_pszHackName,
				ch->GetDesc()->GetAccountTable().login,
				ch->GetName(),
				ch->GetDesc()->GetHostName());
	}
}

void LogManager::HackCRCLog(const char * c_pszHackName, const char * c_pszLogin, const char * c_pszName, const char * c_pszIP, DWORD dwCRC)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	Query("INSERT INTO hack_crc_log (time, login, name, ip, server, why, crc) VALUES(NOW(), '%s', '%s', '%s', '%s', '%s', %u)", c_pszLogin, c_pszName, c_pszIP, g_stHostname.c_str(), c_pszHackName, dwCRC);
}

void LogManager::PCBangLoginLog(DWORD dwPCBangID, const char* c_szPCBangIP, DWORD dwPlayerID, DWORD dwPlayTime)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	Query("INSERT INTO pcbang_loginlog (time, pcbang_id, ip, pid, play_time) VALUES (NOW(), %u, '%s', %u, %u)",
		  dwPCBangID, c_szPCBangIP, dwPlayerID, dwPlayTime);
}

void LogManager::GoldBarLog(DWORD dwPID, DWORD dwItemID, GOLDBAR_HOW eHow, const char* c_pszHint)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	char szHow[32 + 1];

	switch (eHow)
	{
	case PERSONAL_SHOP_BUY:
		snprintf(szHow, sizeof(szHow), "'BUY'");
		break;

	case PERSONAL_SHOP_SELL:
		snprintf(szHow, sizeof(szHow), "'SELL'");
		break;

	case SHOP_BUY:
		snprintf(szHow, sizeof(szHow), "'SHOP_BUY'");
		break;

	case SHOP_SELL:
		snprintf(szHow, sizeof(szHow), "'SHOP_SELL'");
		break;

	case EXCHANGE_TAKE:
		snprintf(szHow, sizeof(szHow), "'EXCHANGE_TAKE'");
		break;

	case EXCHANGE_GIVE:
		snprintf(szHow, sizeof(szHow), "'EXCHANGE_GIVE'");
		break;

	case QUEST:
		snprintf(szHow, sizeof(szHow), "'QUEST'");
		break;

	default:
		snprintf(szHow, sizeof(szHow), "''");
		break;
	}

	Query("INSERT DELAYED INTO goldlog%s (date, time, pid, what, how, hint) VALUES(CURDATE(), CURTIME(), %u, %u, %s, '%s')",
		  get_table_postfix(), dwPID, dwItemID, szHow, c_pszHint);
}

void LogManager::CubeLog(DWORD dwPID, DWORD x, DWORD y, DWORD item_vnum, DWORD item_uid, int item_count, bool success)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	Query("INSERT DELAYED INTO cube%s (pid, time, x, y, item_vnum, item_uid, item_count, success) "
		  "VALUES(%u, NOW(), %u, %u, %u, %u, %d, %d)",
		  get_table_postfix(), dwPID, x, y, item_vnum, item_uid, item_count, success ? 1 : 0);
}

void LogManager::SpeedHackLog(DWORD pid, DWORD x, DWORD y, int hack_count)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	Query("INSERT INTO speed_hack%s (pid, time, x, y, hack_count) "
		  "VALUES(%u, NOW(), %u, %u, %d)",
		  get_table_postfix(), pid, x, y, hack_count);
}

void LogManager::ChangeNameLog(DWORD pid, const char *old_name, const char *new_name, const char *ip)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	Query("INSERT DELAYED INTO change_name%s (pid, old_name, new_name, time, ip) "
		  "VALUES(%u, '%s', '%s', NOW(), '%s') ",
		  get_table_postfix(), pid, old_name, new_name, ip);
}

void LogManager::GMCommandLog(DWORD dwPID, const char* szName, const char* szIP, BYTE byChannel, const char* szCommand)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), szCommand, strlen(szCommand));

	Query("INSERT DELAYED INTO command_log%s (userid, server, ip, port, username, command, date ) "
		  "VALUES(%u, 999, '%s', %u, '%s', '%s', NOW()) ",
		  get_table_postfix(), dwPID, szIP, byChannel, szName, __escape_hint);
}

void LogManager::RefineLog(DWORD pid, const char* item_name, DWORD item_id, int item_refine_level, int is_success, const char* how)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), item_name, strlen(item_name));

	Query("INSERT INTO refinelog%s (pid, item_name, item_id, step, time, is_success, setType) VALUES(%u, '%s', %u, %d, NOW(), %d, '%s')",
		  get_table_postfix(), pid, __escape_hint, item_id, item_refine_level, is_success, how);
}


void LogManager::ShoutLog(BYTE bChannel, BYTE bEmpire, const char * pszText)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	m_sql.EscapeString(__escape_hint, sizeof(__escape_hint), pszText, strlen(pszText));

	Query("INSERT INTO shout_log%s VALUES(NOW(), %d, %d,'%s')", get_table_postfix(), bChannel, bEmpire, __escape_hint);
}

void LogManager::LevelLog(LPCHARACTER pChar, unsigned int level, unsigned int playhour)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	DWORD aid = 0;

	if (NULL != pChar->GetDesc())
	{
		aid = pChar->GetDesc()->GetAccountTable().id;
	}

	Query("REPLACE INTO levellog%s (name, level, time, account_id, pid, playtime) VALUES('%s', %u, NOW(), %u, %u, %d)",
		  get_table_postfix(), pChar->GetName(), level, aid, pChar->GetPlayerID(), playhour);
}

void LogManager::BootLog(const char * c_pszHostName, BYTE bChannel)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MIN);
	Query("INSERT INTO bootlog (time, hostname, channel) VALUES(NOW(), '%s', %d)",
		  c_pszHostName, bChannel);
}

void LogManager::VCardLog(DWORD vcard_id, DWORD x, DWORD y, const char * hostname, const char * giver_name, const char * giver_ip, const char * taker_name, const char * taker_ip)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	Query("INSERT DELAYED INTO vcard_log (vcard_id, x, y, hostname, giver_name, giver_ip, taker_name, taker_ip) VALUES(%u, %u, %u, '%s', '%s', '%s', '%s', '%s')",
		  vcard_id, x, y, hostname, giver_name, giver_ip, taker_name, taker_ip);
}

void LogManager::FishLog(DWORD dwPID, int prob_idx, int fish_id, int fish_level, DWORD dwMiliseconds, DWORD dwVnum, DWORD dwValue)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	Query("INSERT INTO fish_log%s VALUES(NOW(), %u, %d, %u, %d, %u, %u, %u)",
		  get_table_postfix(),
		  dwPID,
		  prob_idx,
		  fish_id,
		  fish_level,
		  dwMiliseconds,
		  dwVnum,
		  dwValue);
}

void LogManager::QuestRewardLog(const char * c_pszQuestName, DWORD dwPID, DWORD dwLevel, int iValue1, int iValue2)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	Query("INSERT INTO quest_reward_log%s VALUES('%s',%u,%u,2,%u,%u,NOW())",
		  get_table_postfix(),
		  c_pszQuestName,
		  dwPID,
		  dwLevel,
		  iValue1,
		  iValue2);
}

void LogManager::DetailLoginLog(bool isLogin, LPCHARACTER ch)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MID);
	if (NULL == ch->GetDesc())
	{
		return;
	}

	if (true == isLogin)
	{
		Query("INSERT INTO loginlog2(type, is_gm, login_time, channel, account_id, pid, ip, client_version) "
			  "VALUES('INVALID', %s, NOW(), %d, %u, %u, inet_aton('%s'), '%s')",
			  ch->IsGM() == true ? "'Y'" : "'N'",
			  g_bChannel,
			  ch->GetDesc()->GetAccountTable().id,
			  ch->GetPlayerID(),
			  ch->GetDesc()->GetHostName(),
			  ch->GetDesc()->GetClientVersion());
	}
	else
	{
		Query("SET @i = (SELECT MAX(id) FROM loginlog2 WHERE account_id=%u AND pid=%u)",
			  ch->GetDesc()->GetAccountTable().id,
			  ch->GetPlayerID());

		Query("UPDATE loginlog2 SET type='VALID', logout_time=NOW(), playtime=TIMEDIFF(logout_time,login_time) WHERE id=@i");
	}
}

void LogManager::DragonSlayLog(DWORD dwGuildID, DWORD dwDragonVnum, DWORD dwStartTime, DWORD dwEndTime)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	Query( "INSERT INTO dragon_slay_log%s VALUES( %d, %d, FROM_UNIXTIME(%d), FROM_UNIXTIME(%d) )",
		   get_table_postfix(),
		   dwGuildID, dwDragonVnum, dwStartTime, dwEndTime);
}

void LogManager::HackShieldLog(unsigned long ErrorCode, LPCHARACTER ch)
{
	LOG_LEVEL_CHECK_N_RET(LOG_LEVEL_MAX);
	struct in_addr st_addr;

#ifndef __WIN32__
	if (0 == inet_aton(ch->GetDesc()->GetHostName(), &st_addr))
#else
	unsigned long in_address;
	in_address = inet_addr(ch->GetDesc()->GetHostName());
	st_addr.s_addr = in_address;
	if (INADDR_NONE == in_address)
#endif
	{
		Query( "INSERT INTO hackshield_log(time, account_id, login, pid, name, reason, ip) "
			   "VALUES(NOW(), %u, '%s', %u, '%s', %u, 0)",
			   ch->GetDesc()->GetAccountTable().id, ch->GetDesc()->GetAccountTable().login,
			   ch->GetPlayerID(), ch->GetName(),
			   ErrorCode);
	}
	else
	{
		Query( "INSERT INTO hackshield_log(time, account_id, login, pid, name, reason, ip) "
			   "VALUES(NOW(), %u, '%s', %u, '%s', %u, inet_aton('%s'))",
			   ch->GetDesc()->GetAccountTable().id, ch->GetDesc()->GetAccountTable().login,
			   ch->GetPlayerID(), ch->GetName(),
			   ErrorCode,
			   ch->GetDesc()->GetHostName());
	}
}

void LogManager::ChatLog(DWORD where, DWORD who_id, const char* who_name, DWORD whom_id, const char* whom_name, const char* type, const char* msg, const char* ip)
{
	Query("INSERT DELAYED INTO `chat_log%s` (`where`, `who_id`, `who_name`, `whom_id`, `whom_name`, `type`, `msg`, `when`, `ip`) "
		  "VALUES (%u, %u, '%s', %u, '%s', '%s', '%s', NOW(), '%s');",
		  get_table_postfix(),
		  where, who_id, who_name, whom_id, whom_name, type, msg, ip);
}

#ifdef __ENABLE_RENEWAL_EXCHANGE__
DWORD LogManager::ExchangeLog(int type, DWORD dwPID1, DWORD dwPID2, long x, long y, int goldA /*=0*/, int goldB /*=0*/)
{
	char szQuery[512];
	snprintf(szQuery, sizeof(szQuery), "INSERT INTO `log`.exchanges (type, playerA, playerB, goldA, goldB, x, y, date) VALUES (%d, %d, %d, %d, %d, %ld, %ld, NOW())", type, dwPID1, dwPID2, goldA, goldB, x, y);
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(szQuery));

	if (!msg || msg->Get()->uiAffectedRows == 0 || msg->Get()->uiAffectedRows == (uint32_t) -1)
	{
		sys_err("Issue logging trade. Query: %s", szQuery);
		return 0;
	}

	return (DWORD)msg->Get()->uiInsertID;
}

void LogManager::ExchangeItemLog(DWORD tradeID, LPITEM item, const char* player)
{
	/*
	 *"exchange_items"
	 Structure: trade_id, toPlayer, item_id, vnum, count, socket0..5, attrtype0...6, attrvalue0...6*/

	if (!tradeID)
	{
		sys_err("Lost trade due to mysql error (tradeID = 0)");
		return;
	}

	Query("INSERT INTO `log`.exchange_items" \
		  "(trade_id, `toPlayer`, `item_id`, `vnum`, count, socket0, socket1, socket2, socket3, socket4, socket5," \
		  " attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3," \
		  "attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6,"
		  " date)" \
		  "VALUES ("\
		  "%lu,'%s',%ld,%ld,%ld,%ld,%ld,%ld,"\
		  "%ld,%ld,%ld,"\
		  "%d,%d,%d,%d,"\
		  "%d,%d,%d,%d,%d,"\
		  "%d,%d,%d,%d,%d,"\
		  "NOW())"
		  , tradeID, player, item->GetID(), item->GetVnum(), item->GetCount()
		  , item->GetSocket(0), item->GetSocket(1), item->GetSocket(2), item->GetSocket(3), item->GetSocket(4), item->GetSocket(5)
		  , item->GetAttributeType(0), item->GetAttributeValue(0), item->GetAttributeType(1), item->GetAttributeValue(1)
		  , item->GetAttributeType(2), item->GetAttributeValue(2), item->GetAttributeType(3), item->GetAttributeValue(3)
		  , item->GetAttributeType(4), item->GetAttributeValue(4), item->GetAttributeType(5), item->GetAttributeValue(5)
		  , item->GetAttributeType(6), item->GetAttributeValue(6)
		 );
}
#endif

#ifdef __ADMIN_MANAGER__
void LogManager::ChatBanLog(LPCHARACTER pkChr, LPCHARACTER pkChrGM, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	ChatBanLog(pkChr->GetPlayerID(), pkChr->GetAID(), pkChr->GetName(), pkChrGM ? pkChrGM->GetPlayerID() : 0, pkChrGM ? pkChrGM->GetName() : "noname", iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::ChatBanLog(LPCHARACTER pkChr, DWORD dwGMPid, const char* c_pszGMName, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	ChatBanLog(pkChr->GetPlayerID(), pkChr->GetAID(), pkChr->GetName(), dwGMPid, c_pszGMName, iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::ChatBanLog(DWORD dwPID, DWORD dwAID, const char* c_pszName, LPCHARACTER pkChrGM, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	ChatBanLog(dwPID, dwAID, c_pszName, pkChrGM ? pkChrGM->GetPlayerID() : 0, pkChrGM ? pkChrGM->GetName() : "noname", iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::ChatBanLog(DWORD dwPID, DWORD dwAID, const char* c_pszName, DWORD dwGMPid, const char* c_pszGMName, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	BanLog(dwPID, dwAID, c_pszName, dwGMPid, c_pszGMName, BAN_LOG_TYPE_CHAT, iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::AccountBanLog(LPCHARACTER pkChr, LPCHARACTER pkChrGM, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	AccountBanLog(pkChr->GetPlayerID(), pkChr->GetAID(), pkChr->GetName(), pkChrGM->GetPlayerID(), pkChrGM->GetName(), iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::AccountBanLog(LPCHARACTER pkChr, DWORD dwGMPid, const char* c_pszGMName, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	AccountBanLog(pkChr->GetPlayerID(), pkChr->GetAID(), pkChr->GetName(), dwGMPid, c_pszGMName, iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::AccountBanLog(DWORD dwPID, DWORD dwAID, const char* c_pszName, LPCHARACTER pkChrGM, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	AccountBanLog(dwPID, dwAID, c_pszName, pkChrGM->GetPlayerID(), pkChrGM->GetName(), iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::AccountBanLog(DWORD dwPID, DWORD dwAID, const char* c_pszName, DWORD dwGMPid, const char* c_pszGMName, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	BanLog(dwPID, dwAID, c_pszName, dwGMPid, c_pszGMName, BAN_LOG_TYPE_ACCOUNT, iDuration, iNewDuration, c_pszReason, c_pszProof, bIncreasedCounter);
}

void LogManager::BanLog(DWORD dwPID, DWORD dwAID, const char* c_pszName, DWORD dwGMPid, const char* c_pszGMName, BYTE bType, int iDuration, int iNewDuration, const char* c_pszReason, const char* c_pszProof, bool bIncreasedCounter)
{
	char szName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szName, sizeof(szName), c_pszName, strlen(c_pszName));

	char szGMName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szGMName, sizeof(szGMName), c_pszGMName, strlen(c_pszGMName));

	char szReason[BLOCK_DESC_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szReason, sizeof(szReason), c_pszReason, strlen(c_pszReason));

	char szProof[BLOCK_PROOF_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szProof, sizeof(szProof), c_pszProof, strlen(c_pszProof));

	Query("INSERT INTO ban_log (pid, aid, name, gm_pid, gm_name, `type`, duration, new_duration, reason, proof, increased_counter, date) VALUES "
		  "(%u, %u, '%s', %u, '%s', %u, %d, %d, '%s', '%s', %u, NOW())", dwPID, dwAID, szName, dwGMPid, szGMName, bType, iDuration, iNewDuration, szReason, szProof, (BYTE)bIncreasedCounter + 1);
}

void LogManager::WhisperLog(DWORD dwSenderPID, const char* c_pszSenderName, DWORD dwReceiverPID, const char* c_pszReceiverName, const char* c_pszText, bool bIsOfflineMessage)
{
	char szSenderName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szSenderName, sizeof(szSenderName), c_pszSenderName, strlen(c_pszSenderName));

	char szReceiverName[CHARACTER_NAME_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szReceiverName, sizeof(szReceiverName), c_pszReceiverName, strlen(c_pszReceiverName));

	char szText[CHAT_MAX_LEN * 2 + 1];
	m_sql.EscapeString(szText, sizeof(szText), c_pszText, strlen(c_pszText));

	Query("INSERT INTO whisper_log (sender, sender_name, receiver, receiver_name, `text`, is_offline) VALUES (%u, '%s', %u, '%s', '%s', %u)",
		  dwSenderPID, szSenderName, dwReceiverPID, szReceiverName, szText, bIsOfflineMessage);
}
#endif

#ifdef __ABUSE_CONTROLLER_ENABLE__
void LogManager::HackLog(const std::string& description, LPCHARACTER ch)
{
	if (ch->GetDesc())
	{
		HackLog(description.c_str(),
				ch->GetDesc()->GetAccountTable().login,
				ch->GetName(),
				ch->GetDesc()->GetHostName());
	}
}
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
void LogManager::OfflineShopLog(uint32_t sellerPid, uint32_t sellerOfflineShopId, const std::string& type, uint32_t buyerPid, uint32_t itemId, uint32_t vnum, uint32_t count, int64_t gold)
{
	Query("INSERT INTO offline_shop%s (owner_pid, shop_id, type, buyer_pid, item_id, vnum, count, gold, time) VALUES(%u, %u, '%s', %u, %u, %u, %u, %lld, NOW())",
		  get_table_postfix(), sellerPid, sellerOfflineShopId, type.c_str(), buyerPid, itemId, vnum, count, gold);
}
#endif

#ifdef __ACTION_RESTRICTIONS__
void LogManager::RestrictionsLog(DWORD dwPID, BYTE restriction, BYTE counter)
{
	Query("INSERT INTO restriction_log%s VALUES(%u,%d,%u,NOW())", get_table_postfix(), dwPID, restriction, counter);
}
#endif

#ifdef __ITEM_SHOP_ENABLE__
void LogManager::ItemShopLog(DWORD dwPlayer, const std::string & sItemHash, int iPrice, int iBalanceBefore, int iBalanceAfter)
{
	Query("INSERT INTO itemshop_logs%s(dwPID, dtTime, sItemHash, iPrice, iBalanceBefore, iBalanceAfter) VALUES(%u, NOW(), '%s', %d, %d, %d)",
		  get_table_postfix(), dwPlayer, sItemHash.c_str(), iPrice, iBalanceBefore, iBalanceAfter);
}
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
void LogManager::TombolaLog(DWORD dwPlayer, const std::string & sAction, DWORD dwVnum, WORD wCount, int iCashBefore, int iCashAfter)
{
	Query("INSERT INTO tombola_logs%s(dwPID, dtTime, sAction, dwVnum, wCount, iCashBefore, iCashAfter) VALUES(%u, NOW(), '%s', %u, %u, %d, %d)",
		  get_table_postfix(), dwPlayer, sAction.c_str(), dwVnum, wCount, iCashBefore, iCashAfter);
}
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
void LogManager::TranslationErrorLog(BYTE bType, BYTE bLocale, const char* c_pszLangKey, const char* c_pszError)
{
	char szLanguage_key[255 * 2 + 1];
	m_sql.EscapeString(szLanguage_key, sizeof(szLanguage_key), c_pszLangKey, strlen(c_pszLangKey));

	char szError[255 * 2 + 1];
	m_sql.EscapeString(szError, sizeof(szError), c_pszError, strlen(c_pszError));

	Query("INSERT IGNORE INTO translation_error (`type`, `lang_type`, lang_key, error) VALUES (%d, %d, '%s', '%s')",
		bType, bLocale + 1, szLanguage_key, szError);
}
#endif
