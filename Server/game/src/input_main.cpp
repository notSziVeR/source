#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "utils.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "protocol.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "cmd.h"
#include "shop.h"
#include "shop_manager.h"
#include "safebox.h"
#include "regen.h"
#include "battle.h"
#include "exchange.h"
#include "questmanager.h"
#include "profiler.h"
#include "messenger_manager.h"
#include "party.h"
#include "p2p.h"
#include "affect.h"
#include "guild.h"
#include "guild_manager.h"
#include "log.h"
#include "banword.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "building.h"
#include "locale_service.h"
#include "gm.h"
#include "spam.h"
#include "ani.h"
#include "motion.h"
#include "OXEvent.h"
#include "locale_service.h"
#include "HackShield.h"
#include "XTrapManager.h"
#include "DragonSoul.h"
#include "belt_inventory_helper.h" // @fixme119
#include "../../common/CommonDefines.h"

#include "input.h"
#include "refine.h"

#ifdef __ENABLE_SWITCHBOT__
	#include "switchbot.h"
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	#include "message_offline.h"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	#include "MarbleCreatorSystem.hpp"
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	#include "cube_renewal.h"
#endif

#ifdef __INGAME_WIKI__
	#include "mob_manager.h"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __SHOP_SEARCH__
	#include "ShopSearchManager.h"
#endif

#ifdef __ACTION_RESTRICTIONS__
	#include "ActionRestricts.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#define ENABLE_CHAT_LOGGING
#define ENABLE_CHAT_SPAMLIMIT
#define ENABLE_WHISPER_CHAT_SPAMLIMIT
#define ENABLE_CHECK_GHOSTMODE

#ifdef ENABLE_CHAT_LOGGING
	static char	__escape_string[1024];
	static char	__escape_string2[1024];
#endif

static int __deposit_limit()
{
	return (1000 * 10000);
}

#ifdef __CHEAT_ENGINE_FIX_ENABLE__
	static const char* CHEATER_TEXT = "Error#";
	static const int HYPERLINK_LIMIT = 3;
#endif

void SendBlockChatInfo(LPCHARACTER ch, int sec)
{
	if (sec <= 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your chat is blocked."));
		return;
	}

	long hour = sec / 3600;
	sec -= hour * 3600;

	long min = (sec / 60);
	sec -= min * 60;

	char buf[128 + 1];

	if (hour > 0 && min > 0)
	{
		snprintf(buf, sizeof(buf), LC_TEXT("%ld hours %ld minutes %d seconds left on your chat block"), hour, min, sec);
	}
	else if (hour > 0 && min == 0)
	{
		snprintf(buf, sizeof(buf), LC_TEXT("%ld hours %d seconds left on your chat block"), hour, sec);
	}
	else if (hour == 0 && min > 0)
	{
		snprintf(buf, sizeof(buf), LC_TEXT("%ld minutes %d seconds left on your chat block"), min, sec);
	}
	else
	{
		snprintf(buf, sizeof(buf), LC_TEXT("%d seconds left on your chat block"), sec);
	}

	ch->ChatPacket(CHAT_TYPE_INFO, buf);
}

EVENTINFO(spam_event_info)
{
	char host[MAX_HOST_LENGTH + 1];

	spam_event_info()
	{
		::memset( host, 0, MAX_HOST_LENGTH + 1 );
	}
};

typedef boost::unordered_map<std::string, std::pair<unsigned int, LPEVENT> > spam_score_of_ip_t;
spam_score_of_ip_t spam_score_of_ip;

EVENTFUNC(block_chat_by_ip_event)
{
	spam_event_info* info = dynamic_cast<spam_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "block_chat_by_ip_event> <Factor> Null pointer" );
		return 0;
	}

	const char * host = info->host;

	spam_score_of_ip_t::iterator it = spam_score_of_ip.find(host);

	if (it != spam_score_of_ip.end())
	{
		it->second.first = 0;
		it->second.second = NULL;
	}

	return 0;
}

bool SpamBlockCheck(LPCHARACTER ch, const char* const buf, const size_t buflen)
{
	if (ch->GetLevel() < g_iSpamBlockMaxLevel)
	{
		spam_score_of_ip_t::iterator it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());

		if (it == spam_score_of_ip.end())
		{
			spam_score_of_ip.insert(std::make_pair(ch->GetDesc()->GetHostName(), std::make_pair(0, (LPEVENT) NULL)));
			it = spam_score_of_ip.find(ch->GetDesc()->GetHostName());
		}

		if (it->second.second)
		{
			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);
			return true;
		}

		unsigned int score;
		const char * word = SpamManager::instance().GetSpamScore(buf, buflen, score);

		it->second.first += score;

		if (word)
		{
			sys_log(0, "SPAM_SCORE: %s text: %s score: %u total: %u word: %s", ch->GetName(), buf, score, it->second.first, word);
		}

		if (it->second.first >= g_uiSpamBlockScore)
		{
			spam_event_info* info = AllocEventInfo<spam_event_info>();
			strlcpy(info->host, ch->GetDesc()->GetHostName(), sizeof(info->host));

			it->second.second = event_create(block_chat_by_ip_event, info, PASSES_PER_SEC(g_uiSpamBlockDuration));
			sys_log(0, "SPAM_IP: %s for %u seconds", info->host, g_uiSpamBlockDuration);

			LogManager::instance().CharLog(ch, 0, "SPAM", word);

			SendBlockChatInfo(ch, event_time(it->second.second) / passes_per_sec);

			return true;
		}
	}

	return false;
}

enum
{
	TEXT_TAG_PLAIN,
	TEXT_TAG_TAG, // ||
	TEXT_TAG_COLOR, // |cffffffff
	TEXT_TAG_HYPERLINK_START, // |H
	TEXT_TAG_HYPERLINK_END, // |h ex) |Hitem:1234:1:1:1|h
	TEXT_TAG_RESTORE_COLOR,
};

int GetTextTag(const char * src, int maxLen, int & tagLen, std::string & extraInfo)
{
	tagLen = 1;

	if (maxLen < 2 || *src != '|')
	{
		return TEXT_TAG_PLAIN;
	}

	const char * cur = ++src;

	if (*cur == '|')
	{
		tagLen = 2;
		return TEXT_TAG_TAG;
	}
	else if (*cur == 'c') // color |cffffffffblahblah|r
	{
		tagLen = 2;
		return TEXT_TAG_COLOR;
	}
	else if (*cur == 'H')
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_START;
	}
	else if (*cur == 'h') // end of hyperlink
	{
		tagLen = 2;
		return TEXT_TAG_HYPERLINK_END;
	}

	return TEXT_TAG_PLAIN;
}

void GetTextTagInfo(const char * src, int src_len, int & hyperlinks, bool & colored)
{
	colored = false;
	hyperlinks = 0;

	int len;
	std::string extraInfo;

	for (int i = 0; i < src_len;)
	{
		int tag = GetTextTag(&src[i], src_len - i, len, extraInfo);

		if (tag == TEXT_TAG_HYPERLINK_START)
		{
			++hyperlinks;
		}

		if (tag == TEXT_TAG_COLOR)
		{
			colored = true;
		}

		i += len;
	}
}

#ifdef __CHEAT_ENGINE_FIX_ENABLE__
static bool StrExtractAndMove(std::string& str, const std::string& mark)
{
	size_t findPos = str.find(mark);
	if (findPos == std::string::npos)
	{
		return false;
	}

	str = str.substr(findPos + mark.size());
	return true;
}

bool GetValidHyperLink(std::string HyperLink, int& window_type, int& slot_number)
{
	std::string SlotN;

	if (!StrExtractAndMove(HyperLink, "||"))
	{
		return false;
	}

	size_t findPos = HyperLink.find("|cf");
	if (findPos == std::string::npos)
	{
		return false;
	}

	SlotN = HyperLink.substr(0, findPos);

	try
	{
		slot_number = stoi(SlotN, NULL, 10);
	}
	catch (const std::exception& e)
	{
		return false;
	}

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (slot_number >= INVENTORY_MAX_NUM)
	{
		window_type = EQUIPMENT;
	}
	else
	{
		window_type = INVENTORY;
	}
#else
	if (slot_number >= INVENTORY_MAX_NUM && slot_number < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	{
		window_type = EQUIPMENT;
	}
	else
	{
		window_type = INVENTORY;
	}
#endif

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (slot_number >= DRAGON_SOUL_EQUIP_SLOT_START)
	{
		return false;
	}
#else
	if (slot_number >= INVENTORY_MAX_NUM && slot_number < SPECIAL_STORAGE_START_CELL)
	{
		return false;
	}
#endif

	if ((window_type != INVENTORY && window_type != EQUIPMENT) || slot_number < 0)
	{
		return false;
	}

	return true;
}

int GetValidHyperLink(std::string HyperLink, int& window_type, int& slot_number, size_t& pos_beg, size_t& s_size)
{
	std::transform(HyperLink.begin(), HyperLink.end(), HyperLink.begin(), ::tolower);

	// Fail values
	window_type = INVENTORY + 1;

#ifndef __SPECIAL_STORAGE_ENABLE__
	slot_number = DRAGON_SOUL_EQUIP_SLOT_START;
#else
	slot_number = SPECIAL_STORAGE_END_CELL;
#endif

	std::string SlotN;

	// Looking for beggining of hyperlink
	size_t findPos = HyperLink.find("|cf");
	if (findPos == std::string::npos)
	{
		return 0;
	}
	else
	{
		pos_beg += findPos;
	}

	// Looking for end of hyperlink
	findPos = HyperLink.find("|r");
	if (findPos == std::string::npos)
	{
		return 0;
	}
	else
		// Computing size of string
	{
		s_size = findPos - HyperLink.find("|cf") + 2;
	}

	if (!StrExtractAndMove(HyperLink, "||"))
	{
		return 1;
	}

	findPos = HyperLink.find("|");
	if (findPos == std::string::npos)
	{
		return 1;
	}

	SlotN = HyperLink.substr(0, findPos);

	try
	{
		slot_number = stoi(SlotN, NULL, 10);
	}
	catch (const std::exception& e)
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		slot_number = DRAGON_SOUL_EQUIP_SLOT_START;
#else
		slot_number = SPECIAL_STORAGE_END_CELL;
#endif
		return 1;
	}

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (slot_number >= INVENTORY_MAX_NUM)
	{
		window_type = EQUIPMENT;
	}
	else
	{
		window_type = INVENTORY;
	}
#else
	if (slot_number >= INVENTORY_MAX_NUM && slot_number < INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	{
		window_type = EQUIPMENT;
	}
	else
	{
		window_type = INVENTORY;
	}
#endif

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (slot_number >= DRAGON_SOUL_EQUIP_SLOT_START)
	{
		return 1;
	}
#else
	if (slot_number >= INVENTORY_MAX_NUM && slot_number < SPECIAL_STORAGE_START_CELL)
	{
		return 1;
	}
#endif

	if ((window_type != INVENTORY && window_type != EQUIPMENT) || slot_number < 0)
	{
		return 1;
	}

	return 2;
}

std::string GenerateHyperLink(LPCHARACTER ch, const int& slot_index, const int& slot_window)
{
	// Clearing buf
	char buf[CHAT_MAX_LEN];
	memset(buf, 0, sizeof(buf));

	// Generating HyperLink
	LPITEM item;
	char ItemBuf[CHAT_MAX_LEN / 2];
	int len;
	bool bAttr = false;

	if (!(item = ch->GetItem(TItemPos(slot_window, slot_index))))
	{
		strlcpy(buf, CHEATER_TEXT, sizeof(buf));
	}
	else
	{
		len = snprintf(ItemBuf, sizeof(ItemBuf), "|Hitem:%x", item->GetVnum());

#ifdef __TRANSMUTATION_SYSTEM__
		len += snprintf(ItemBuf + len, sizeof(ItemBuf) - len, "|%x", item->GetTransmutate());
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		len += snprintf(ItemBuf + len, sizeof(ItemBuf) - len, ":%d", item->GetRefineElement());
#endif

		len += snprintf(ItemBuf + len, sizeof(ItemBuf) - len, ":%d", static_cast<int>(item->GetFlag()));

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			len += snprintf(ItemBuf + len, sizeof(ItemBuf) - len, ":%x", static_cast<unsigned int>(item->GetSocket(i)));
		}

		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (item->GetAttributeType(i) > 0 && !bAttr)
			{
				bAttr = true;
			}

			len += snprintf(ItemBuf + len, sizeof(ItemBuf) - len, ":%x:%d", item->GetAttributeType(i), item->GetAttributeValue(i));
		}

		if (bAttr)
		{
			snprintf(buf, sizeof(buf), "|cffffc700%s|h[%s]|h|r", ItemBuf, item->GetName());
		}
		else
		{
			snprintf(buf, sizeof(buf), "|cfff1e6c0%s|h[%s]|h|r", ItemBuf, item->GetName());
		}
	}

	return buf;
}

/*
static bool IsHyperLinkItem(const std::string& HyperLink)
{
	return (HyperLink.find("|Hitem") != std::string::npos);
}
*/

int RewriteHyperLink(LPCHARACTER ch, char* buf, const size_t buf_len)
{
	std::string tmp_buf = buf;

	int window_type, slot_type;
	size_t pos_beg = 0;
	size_t s_size = tmp_buf.size() - 1;
	int state = -1;
	int hyperlink_count = HYPERLINK_LIMIT; // limit

	while ((state = GetValidHyperLink(tmp_buf.substr(pos_beg), window_type, slot_type, pos_beg, s_size)))
	{
		if (state == 1)
		{
			break;
		}

		tmp_buf.erase(pos_beg, s_size);

		if ((hyperlink_count--) > 0)
		{
			std::string NewHyperLink = (state == 2) ? GenerateHyperLink(ch, slot_type, window_type) : CHEATER_TEXT;
			tmp_buf.insert(pos_beg, NewHyperLink);
			pos_beg += NewHyperLink.size();
		}

		if (pos_beg >= tmp_buf.size())
		{
			break;
		}
	}

	strlcpy(buf, tmp_buf.c_str(), buf_len);
	return strlen(buf);
}
#endif

int ProcessTextTag(LPCHARACTER ch, const char * c_pszText, size_t len)
{
	int hyperlinks;
	bool colored;

	GetTextTagInfo(c_pszText, len, hyperlinks, colored);

#ifdef __CHEAT_ENGINE_FIX_ENABLE__
	if (hyperlinks)
	{
		return 1;
	}

	return 0;
#else
	if (colored == true && hyperlinks == 0)
	{
		return 4;
	}

#ifdef __ENABLE_NEWSTUFF__
	if (g_bDisablePrismNeed)
	{
		return 0;
	}
#endif
	int nPrismCount = ch->CountSpecifyItem(ITEM_PRISM);

	if (nPrismCount < hyperlinks)
	{
		return 1;
	}


	if (!ch->GetMyShop())
	{
		ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
		return 0;
	}
	else
	{
		int sellingNumber = ch->GetMyShop()->GetNumberByVnum(ITEM_PRISM);
		if (nPrismCount - sellingNumber < hyperlinks)
		{
			return 2;
		}
		else
		{
			ch->RemoveSpecifyItem(ITEM_PRISM, hyperlinks);
			return 0;
		}
	}

	return 4;
#endif
}

int CInputMain::Whisper(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGWhisper* pinfo = reinterpret_cast<const TPacketCGWhisper*>(data);

	if (uiBytes < pinfo->wSize)
	{
		return -1;
	}

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGWhisper);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (!ch->GetDesc()) return iExtraLen;

#ifdef __ACTION_RESTRICTIONS__
	if (ch->GetActionRestrictions())
	{
		if (ch->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_WHISPER) == false) { return - 1; }
	}
#endif

#ifdef ENABLE_WHISPER_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 10)
	{
		ch->GetDesc()->DelayedDisconnect(0);
		return (iExtraLen);
	}
#endif

	if (ch->FindAffect(AFFECT_BLOCK_CHAT))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your chat is blocked."));
		return (iExtraLen);
	}

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(pinfo->szNameTo);

	if (pkChr == ch)
	{
		return (iExtraLen);
	}

	LPDESC pkDesc = NULL;

#ifdef __ADMIN_MANAGER__
	unsigned char bOpponentEmpire = 0;
	DWORD dwOpponentPID = 0;
	const char* szOpponentName = "";

#else
	BYTE bOpponentEmpire = 0;
#endif

	if (test_server)
	{
		if (!pkChr)
		{
			sys_log(0, "Whisper to %s(%s) from %s", "Null", pinfo->szNameTo, ch->GetName());
		}
		else
		{
			sys_log(0, "Whisper to %s(%s) from %s", pkChr->GetName(), pinfo->szNameTo, ch->GetName());
		}
	}

	if (ch->IsBlockMode(BLOCK_WHISPER))
	{
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;
			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
			pack.iLocale = -1;
#endif
			pack.wSize = sizeof(TPacketGCWhisper);
			pack.bLevel = 0;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));
			ch->GetDesc()->Packet(&pack, sizeof(pack));
		}
		return iExtraLen;
	}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	auto iTargetLocale = -1;
#endif

	if (!pkChr)
	{
		CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

		if (pkCCI)
		{
			pkDesc = pkCCI->pkDesc;
			pkDesc->SetRelay(pinfo->szNameTo);
			bOpponentEmpire = pkCCI->bEmpire;

#ifdef __ADMIN_MANAGER__
			szOpponentName = pkCCI->szName;
			dwOpponentPID = pkCCI->dwPID;
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
			iTargetLocale = pkCCI->iLocale;
#endif

			if (test_server)
			{
				sys_log(0, "Whisper to %s from %s (Channel %d Mapindex %d)", "Null", ch->GetName(), pkCCI->bChannel, pkCCI->lMapIndex);
			}
		}
	}
	else
	{
		pkDesc = pkChr->GetDesc();
		bOpponentEmpire = pkChr->GetEmpire();
#ifdef __ADMIN_MANAGER__
		szOpponentName = pkChr->GetName();
		dwOpponentPID = pkChr->GetPlayerID();
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		iTargetLocale = pkChr->GetLocale();
#endif
	}

	if (!pkDesc)
	{
#ifdef __OFFLINE_MESSAGE_SYSTEM__
		char buf[CHAT_MAX_LEN + 1];
		strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
		const size_t buflen = strlen(buf);

		if (true == SpamBlockCheck(ch, buf, buflen))
		{
			return iExtraLen;
		}

		int iRes = COfflineMessage::instance().AddNewMessage(ch, pinfo, buf, buflen);
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;

			pack.bHeader = HEADER_GC_WHISPER;

			if (iRes == -2)
			{
				pack.bType = WHISPER_TYPE_MESSAGE_PLAYER_NOT_EXIST;
			}
			else
			{
				pack.bType = (iRes == -1) ? WHISPER_TYPE_MESSAGE_LIMIT_REACHED : WHISPER_TYPE_MESSAGE_GONE;
			}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
			pack.iLocale = -1;
#endif

			pack.wSize = sizeof(TPacketGCWhisper);
			pack.bLevel = 0;
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
		}
#else
		if (ch->GetDesc())
		{
			TPacketGCWhisper pack;

			pack.bHeader = HEADER_GC_WHISPER;
			pack.bType = WHISPER_TYPE_NOT_EXIST;
			pack.wSize = sizeof(TPacketGCWhisper);
			pack.bLevel = ch->GetLevel();
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
			iTargetLocale = -1;
#endif
			strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
			strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));
			ch->GetDesc()->Packet(&pack, sizeof(TPacketGCWhisper));
			sys_log(0, "WHISPER: no player");
		}
#endif
	}
	else
	{
		if (ch->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_SENDER_BLOCKED;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
				pack.iLocale = iTargetLocale;;
#endif
				pack.wSize = sizeof(TPacketGCWhisper);
				pack.bLevel = 0;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else if (pkChr && pkChr->IsBlockMode(BLOCK_WHISPER))
		{
			if (ch->GetDesc())
			{
				TPacketGCWhisper pack;
				pack.bHeader = HEADER_GC_WHISPER;
				pack.bType = WHISPER_TYPE_TARGET_BLOCKED;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
				pack.iLocale = iTargetLocale;;
#endif
				pack.wSize = sizeof(TPacketGCWhisper);
				pack.bLevel = 0;
				strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));
				strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));
				ch->GetDesc()->Packet(&pack, sizeof(pack));
			}
		}
		else
		{
			BYTE bType = WHISPER_TYPE_NORMAL;

			char buf[CHAT_MAX_LEN + 1];
			strlcpy(buf, data + sizeof(TPacketCGWhisper), MIN(iExtraLen + 1, sizeof(buf)));
#ifdef __CHEAT_ENGINE_FIX_ENABLE__
			size_t buflen = strlen(buf);
#else
			const size_t buflen = strlen(buf);
#endif

			if (true == SpamBlockCheck(ch, buf, buflen))
			{
				if (!pkChr)
				{
					CCI * pkCCI = P2P_MANAGER::instance().Find(pinfo->szNameTo);

					if (pkCCI)
					{
						pkDesc->SetRelay("");
					}
				}
				return iExtraLen;
			}

			//CBanwordManager::instance().ConvertString(buf, buflen);

			if (g_bEmpireWhisper)
				if (!ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
					if (!(pkChr && pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)))
						if (bOpponentEmpire != ch->GetEmpire() && ch->GetEmpire() && bOpponentEmpire
#ifdef __ADMIN_MANAGER__
								&& ch->GetGMLevel() == GM_PLAYER && GM::get_level(pinfo->szNameTo) == GM_PLAYER)
#else
								&& ch->GetGMLevel() == GM_PLAYER && gm_get_level(pinfo->szNameTo) == GM_PLAYER)
#endif

						{
							if (!pkChr)
							{

								bType = ch->GetEmpire() << 4;
							}
							else
							{
								ConvertEmpireText(ch->GetEmpire(), buf, buflen, 10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + ch->GetEmpire() - 1));
							}
						}

#ifdef __CHEAT_ENGINE_FIX_ENABLE__
			if (ProcessTextTag(ch, buf, buflen) == 1)
			{
				buflen = RewriteHyperLink(ch, buf, sizeof(buf));
			}
#else
			int processReturn = ProcessTextTag(ch, buf, buflen);
			if (0 != processReturn)
			{
				if (ch->GetDesc())
				{
					TItemTable * pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

					if (pTable)
					{
						char buf[128];
						int len;
						if (3 == processReturn)
						{
							len = snprintf(buf, sizeof(buf), LC_TEXT("This function is not available right now."), pTable->szLocaleName);
						}
						else
						{
							len = snprintf(buf, sizeof(buf), LC_TEXT("You need %s."), pTable->szLocaleName);
						}

						if (len < 0 || len >= (int) sizeof(buf))
						{
							len = sizeof(buf) - 1;
						}

						++len;

						TPacketGCWhisper pack;

						pack.bHeader = HEADER_GC_WHISPER;
						pack.bType = WHISPER_TYPE_ERROR;
						pack.wSize = sizeof(TPacketGCWhisper) + len;
						strlcpy(pack.szNameFrom, pinfo->szNameTo, sizeof(pack.szNameFrom));

						ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
						ch->GetDesc()->Packet(buf, len);

						sys_log(0, "WHISPER: not enough %s: char: %s", pTable->szLocaleName, ch->GetName());
					}
				}


				pkDesc->SetRelay("");
				return (iExtraLen);
			}
#endif

			if (ch->IsGM())
			{
				bType = (bType & 0xF0) | WHISPER_TYPE_GM;
			}

			if (buflen > 0)
			{
				TPacketGCWhisper pack;

				pack.bHeader = HEADER_GC_WHISPER;
				pack.wSize = sizeof(TPacketGCWhisper) + buflen;
				pack.bType = bType;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
				pack.iLocale = ch->GetLocale();;
#endif
				pack.bLevel = ch->GetLevel();
				strlcpy(pack.szNameFrom, ch->GetName(), sizeof(pack.szNameFrom));
				strlcpy(pack.szSentDate, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.szSentDate));

				TEMP_BUFFER tmpbuf;

				tmpbuf.write(&pack, sizeof(pack));
				tmpbuf.write(buf, buflen);

				pkDesc->Packet(tmpbuf.read_peek(), tmpbuf.size());

#ifdef __ADMIN_MANAGER__
				LogManager::Instance().WhisperLog(ch->GetPlayerID(), ch->GetName(), dwOpponentPID, szOpponentName, buf, false);
#endif

				// @warme006
				// sys_log(0, "WHISPER: %s -> %s : %s", ch->GetName(), pinfo->szNameTo, buf);
#ifdef ENABLE_CHAT_LOGGING
				if (ch->IsGM())
				{
					LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), buf, buflen);
					LogManager::instance().EscapeString(__escape_string2, sizeof(__escape_string2), pinfo->szNameTo, sizeof(pack.szNameFrom));
					LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, __escape_string2, "WHISPER", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
				}
#endif
			}
		}
	}
	if (pkDesc)
	{
		pkDesc->SetRelay("");
	}

	return (iExtraLen);
}

struct RawPacketToCharacterFunc
{
	const void * m_buf;
	int	m_buf_len;

	RawPacketToCharacterFunc(const void * buf, int buf_len) : m_buf(buf), m_buf_len(buf_len)
	{
	}

	void operator () (LPCHARACTER c)
	{
		if (!c->GetDesc())
		{
			return;
		}

		c->GetDesc()->Packet(m_buf, m_buf_len);
	}
};

struct FEmpireChatPacket
{
	packet_chat& p;
	const char* orig_msg;
	int orig_len;
	char converted_msg[CHAT_MAX_LEN + 1];

	BYTE bEmpire;
	int iMapIndex;
	int namelen;

	FEmpireChatPacket(packet_chat& p, const char* chat_msg, int len, BYTE bEmpire, int iMapIndex, int iNameLen)
		: p(p), orig_msg(chat_msg), orig_len(len), bEmpire(bEmpire), iMapIndex(iMapIndex), namelen(iNameLen)
	{
		memset( converted_msg, 0, sizeof(converted_msg) );
	}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		if (d->GetCharacter()->GetMapIndex() != iMapIndex)
		{
			return;
		}

		d->BufferedPacket(&p, sizeof(packet_chat));

		if (d->GetEmpire() == bEmpire ||
				bEmpire == 0 ||
				d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
				d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			d->Packet(orig_msg, orig_len);
		}
		else
		{

			size_t len = strlcpy(converted_msg, orig_msg, sizeof(converted_msg));

			if (len >= sizeof(converted_msg))
			{
				len = sizeof(converted_msg) - 1;
			}

			ConvertEmpireText(bEmpire, converted_msg + namelen, len - namelen, 10 + 2 * d->GetCharacter()->GetSkillPower(SKILL_LANGUAGE1 + bEmpire - 1));
			d->Packet(converted_msg, orig_len);
		}
	}
};

struct FYmirChatPacket
{
	packet_chat& packet;
	const char* m_szChat;
	size_t m_lenChat;
	const char* m_szName;

	int m_iMapIndex;
	BYTE m_bEmpire;
	bool m_ring;

	char m_orig_msg[CHAT_MAX_LEN + 1];
	int m_len_orig_msg;
	char m_conv_msg[CHAT_MAX_LEN + 1];
	int m_len_conv_msg;

	FYmirChatPacket(packet_chat& p, const char* chat, size_t len_chat, const char* name, size_t len_name, int iMapIndex, BYTE empire, bool ring)
		: packet(p),
		  m_szChat(chat), m_lenChat(len_chat),
		  m_szName(name),
		  m_iMapIndex(iMapIndex), m_bEmpire(empire),
		  m_ring(ring)
	{
		m_len_orig_msg = snprintf(m_orig_msg, sizeof(m_orig_msg), "%s : %s", m_szName, m_szChat) + 1;

		if (m_len_orig_msg < 0 || m_len_orig_msg >= (int) sizeof(m_orig_msg))
		{
			m_len_orig_msg = sizeof(m_orig_msg) - 1;
		}

		m_len_conv_msg = snprintf(m_conv_msg, sizeof(m_conv_msg), "??? : %s", m_szChat) + 1;

		if (m_len_conv_msg < 0 || m_len_conv_msg >= (int) sizeof(m_conv_msg))
		{
			m_len_conv_msg = sizeof(m_conv_msg) - 1;
		}

		ConvertEmpireText(m_bEmpire, m_conv_msg + 6, m_len_conv_msg - 6, 10);
	}

	void operator() (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		if (d->GetCharacter()->GetMapIndex() != m_iMapIndex)
		{
			return;
		}

		if (m_ring ||
				d->GetEmpire() == m_bEmpire ||
				d->GetCharacter()->GetGMLevel() > GM_PLAYER ||
				d->GetCharacter()->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
		{
			packet.size = m_len_orig_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_orig_msg, m_len_orig_msg);
		}
		else
		{
			packet.size = m_len_conv_msg + sizeof(TPacketGCChat);

			d->BufferedPacket(&packet, sizeof(packet_chat));
			d->Packet(m_conv_msg, m_len_conv_msg);
		}
	}
};

int CInputMain::Chat(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	const TPacketCGChat* pinfo = reinterpret_cast<const TPacketCGChat*>(data);

	if (uiBytes < pinfo->size)
	{
		return -1;
	}

	const int iExtraLen = pinfo->size - sizeof(TPacketCGChat);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->size, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char buf[CHAT_MAX_LEN - (CHARACTER_NAME_MAX_LEN + 3) + 1];
	strlcpy(buf, data + sizeof(TPacketCGChat), MIN(iExtraLen + 1, sizeof(buf)));
	const size_t buflen = strlen(buf);

	if (buflen > 1 && *buf == '/')
	{
		interpret_command(ch, buf + 1, buflen - 1);
		return iExtraLen;
	}

#ifdef ENABLE_CHAT_SPAMLIMIT
	if (ch->IncreaseChatCounter() >= 10 && !ch->IsGM())
	{
		if (ch->GetChatCounter() == 10)
		{
			ch->GetDesc()->DelayedDisconnect(0);
		}
		return iExtraLen;
	}
#else
	if (ch->IncreaseChatCounter() >= 10)
	{
		if (ch->GetChatCounter() == 10)
		{
			sys_log(0, "CHAT_HACK: %s", ch->GetName());
			ch->GetDesc()->DelayedDisconnect(5);
		}

		return iExtraLen;
	}
#endif

	const CAffect* pAffect = ch->FindAffect(AFFECT_BLOCK_CHAT);

	if (pAffect != NULL)
	{
		SendBlockChatInfo(ch, pAffect->lDuration);
		return iExtraLen;
	}

	if (true == SpamBlockCheck(ch, buf, buflen))
	{
		return iExtraLen;
	}

	int len;
	// @fixme133 begin
	CBanwordManager::instance().ConvertString(buf, buflen);

#ifdef __CHEAT_ENGINE_FIX_ENABLE__
	if (ProcessTextTag(ch, buf, buflen) == 1)
	{
		len = RewriteHyperLink(ch, buf, sizeof(buf));
	}
#else
	int processReturn = ProcessTextTag(ch, buf, buflen);
	if (0 != processReturn)
	{
		const TItemTable* pTable = ITEM_MANAGER::instance().GetTable(ITEM_PRISM);

		if (NULL != pTable)
		{
			if (3 == processReturn)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This function is not available right now."), pTable->szLocaleName);
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %s."), pTable->szLocaleName);
			}

		}

		return iExtraLen;
	}
	// @fixme133 end
#endif

	char chatbuf[CHAT_MAX_LEN + 1];
	len = snprintf(chatbuf, sizeof(chatbuf), "%s: %s", ch->GetName(), buf);

	if (len < 0 || len >= (int)sizeof(chatbuf))
	{
		len = sizeof(chatbuf) - 1;
	}

	if (pinfo->type == CHAT_TYPE_SHOUT)
	{
		// Check player level
		const int SHOUT_LEVEL_LIMIT = 15;
		if (ch->GetLevel() < SHOUT_LEVEL_LIMIT)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a minimum levelof %d to be able to call."), SHOUT_LEVEL_LIMIT);
			return iExtraLen;
		}

		// Check wait time between shouts
		const int SHOUT_WAIT_TIME = 15;
		if (thecore_heart->pulse - (int)ch->GetLastShoutPulse() < passes_per_sec * SHOUT_WAIT_TIME)
		{
			return iExtraLen;
		}

		ch->SetLastShoutPulse(thecore_heart->pulse);

		len = sizeof(chatbuf) - 1;		// Build shout string
		const std::string START_COLOR = "|cFF";
		const std::string START_HYPERLINK = "|H";
		const std::string END_HYPERLINK = "|h";
		const std::string RESET_LINK_AND_COVER = "|h|r";

		const std::string EMPIRE_COLORS[3] = { "FF0000", "FFFF00", "00C0FC" };
		const std::string GM_COLOR = "47DA00";
		const std::string TEXT_COLOR = "A7FFD4";

		const std::string START_LEVEL = "[Lv. ";
		const std::string END_LEVEL = "] ";

		std::string shoutText = "";

		//Append Player Level
		shoutText += START_LEVEL;
		shoutText += std::to_string(ch->GetLevel());
		shoutText += END_LEVEL;

		// Colorize shout
		shoutText += START_COLOR;
		if (ch->IsGM())
		{
			shoutText += GM_COLOR;
		}
		else
		{
			shoutText += EMPIRE_COLORS[ch->GetEmpire() - 1];
		}

		// Append player link
		shoutText += START_HYPERLINK;
		shoutText += ch->GetPlayerLink();
		shoutText += END_HYPERLINK;

		// Append player name
		shoutText += ch->GetName();
		shoutText += RESET_LINK_AND_COVER;
		shoutText += ": ";

		// Append shout text
		shoutText += buf;

		// Send P2P shout packet
		TPacketGGShout packet;
		packet.bHeader = HEADER_GG_SHOUT;
		packet.bEmpire = ch->GetEmpire();
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		packet.bLocale = ch->GetLocale();
#endif
		strncpy(packet.szText, shoutText.c_str(), sizeof(packet.szText));
		packet.szText[sizeof(packet.szText) - 1] = 0; // Null terminate char array

		P2P_MANAGER::instance().Send(&packet, sizeof(packet));

		// Send shout on this core
		SendShout(shoutText.c_str(), ch->GetEmpire()
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		, ch->GetLocale()
#endif
		);

		return iExtraLen;
	}

	TPacketGCChat pack_chat;

	pack_chat.header = HEADER_GC_CHAT;
	pack_chat.size = sizeof(TPacketGCChat) + len;
	pack_chat.type = pinfo->type;
	pack_chat.id = ch->GetVID();
	pack_chat.bEmpire = ch->GetEmpire();
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	pack_chat.locale = ch->GetLocale();
#endif

	switch (pinfo->type)
	{
	case CHAT_TYPE_TALKING:
	{
		const DESC_MANAGER::DESC_SET& c_ref_set = DESC_MANAGER::instance().GetClientSet();

		if (false)
		{
			std::for_each(c_ref_set.begin(), c_ref_set.end(),
						  FYmirChatPacket(pack_chat,
										  buf,
										  strlen(buf),
										  ch->GetName(),
										  strlen(ch->GetName()),
										  ch->GetMapIndex(),
										  ch->GetEmpire(),
										  ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)));
		}
		else
		{
			std::for_each(c_ref_set.begin(), c_ref_set.end(),
						  FEmpireChatPacket(pack_chat,
											chatbuf,
											len,
											(ch->GetGMLevel() > GM_PLAYER ||
											 ch->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE)) ? 0 : ch->GetEmpire(),
											ch->GetMapIndex(), strlen(ch->GetName())));
#ifdef ENABLE_CHAT_LOGGING
			if (ch->IsGM())
			{
				LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
				LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), 0, "", "NORMAL", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
			}
#endif
		}
	}
	break;

	case CHAT_TYPE_PARTY:
	{
		if (!ch->GetParty())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not in this Group."));
		}
		else
		{
			TEMP_BUFFER tbuf;

			tbuf.write(&pack_chat, sizeof(pack_chat));
			tbuf.write(chatbuf, len);

			RawPacketToCharacterFunc f(tbuf.read_peek(), tbuf.size());
			ch->GetParty()->ForEachOnlineMember(f);
#ifdef ENABLE_CHAT_LOGGING
			if (ch->IsGM())
			{
				LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
				LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetParty()->GetLeaderPID(), "", "PARTY", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
			}
#endif
		}
	}
	break;

	case CHAT_TYPE_GUILD:
	{
		if (!ch->GetGuild())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not join this Guild."));
		}
		else
		{
			ch->GetGuild()->Chat(chatbuf);
#ifdef ENABLE_CHAT_LOGGING
			if (ch->IsGM())
			{
				LogManager::instance().EscapeString(__escape_string, sizeof(__escape_string), chatbuf, len);
				LogManager::instance().ChatLog(ch->GetMapIndex(), ch->GetPlayerID(), ch->GetName(), ch->GetGuild()->GetID(), ch->GetGuild()->GetName(), "GUILD", __escape_string, ch->GetDesc() ? ch->GetDesc()->GetHostName() : "");
			}
#endif
		}
	}
	break;

	default:
		sys_err("Unknown chat type %d", pinfo->type);
		break;
	}

	return (iExtraLen);
}

void CInputMain::ItemUse(LPCHARACTER ch, const char * data)
{
	ch->UseItem(((struct command_item_use *) data)->Cell);
}

#ifdef __ENABLE_MULTI_USE_PACKET__
void CInputMain::ItemMultiUse(LPCHARACTER ch, const char* data)
{
	TPacketCGItemMultiUse* p = (TPacketCGItemMultiUse*)data;
	for (int i = 0; i < p->Count; ++i)
	{
		LPITEM pkItem = ch->GetItem(p->Cell);
		if (!pkItem)
		{
			break;
		}

		ch->UseItem(p->Cell);
	}
}
#endif

void CInputMain::ItemToItem(LPCHARACTER ch, const char * pcData)
{
	TPacketCGItemUseToItem * p = (TPacketCGItemUseToItem *) pcData;
	if (ch)
	{
		ch->UseItem(p->Cell, p->TargetCell);
	}
}

void CInputMain::ItemDrop(LPCHARACTER ch, const char * data)
{
	struct command_item_drop * pinfo = (struct command_item_drop *) data;

	if (!ch)
	{
		return;
	}


	if (pinfo->gold > 0)
	{
		ch->DropGold(pinfo->gold);
	}
	else
	{
		ch->DropItem(pinfo->Cell);
	}
}

void CInputMain::ItemDrop2(LPCHARACTER ch, const char * data)
{
	TPacketCGItemDrop2 * pinfo = (TPacketCGItemDrop2 *) data;

	if (!ch)
	{
		return;
	}
	if (pinfo->gold > 0)
	{
		ch->DropGold(pinfo->gold);
	}
	else
	{
		ch->DropItem(pinfo->Cell, pinfo->count);
	}
}

void CInputMain::ItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (ch)
	{
		ch->MoveItem(pinfo->Cell, pinfo->CellTo, pinfo->count);
	}
}

void CInputMain::ItemPickup(LPCHARACTER ch, const char * data)
{
	struct command_item_pickup * pinfo = (struct command_item_pickup*) data;
	if (ch)
	{
		ch->PickupItem(pinfo->vid);
	}
}

#ifdef __ENABLE_DESTROY_ITEM_PACKET__
void CInputMain::ItemDestroy(LPCHARACTER ch, const char* data)
{
	struct command_item_destroy_packet* pinfo = (struct command_item_destroy_packet*)data;
	if (ch)
	{
		ch->DestroyItem(pinfo->Cell);
	}
}
#endif

#ifdef __ENABLE_NEW_FILTERS__
void CInputMain::ItemsPickup(LPCHARACTER ch, const char* data)
{
	struct command_items_pickup* pinfo = (struct command_items_pickup*)data;
	if (ch)
	{
		for (BYTE i = 0; i < 20; i++)
		{
			if (pinfo->items[i] != 0)
			{
				ch->PickupItem(pinfo->items[i]);
			}
		}
	}
}
#endif

void CInputMain::QuickslotAdd(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_add * pinfo = (struct command_quickslot_add *) data;
	ch->SetQuickslot(pinfo->pos, pinfo->slot);
}

void CInputMain::QuickslotDelete(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_del * pinfo = (struct command_quickslot_del *) data;
	ch->DelQuickslot(pinfo->pos);
}

void CInputMain::QuickslotSwap(LPCHARACTER ch, const char * data)
{
	struct command_quickslot_swap * pinfo = (struct command_quickslot_swap *) data;
	ch->SwapQuickslot(pinfo->pos, pinfo->change_pos);
}

int CInputMain::Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGMessenger* p = (TPacketCGMessenger*) c_pData;

	if (uiBytes < sizeof(TPacketCGMessenger))
	{
		return -1;
	}

	c_pData += sizeof(TPacketCGMessenger);
	uiBytes -= sizeof(TPacketCGMessenger);

	switch (p->subheader)
	{
	case MESSENGER_SUBHEADER_CG_ADD_BY_VID:
	{
		if (uiBytes < sizeof(TPacketCGMessengerAddByVID))
		{
			return -1;
		}

		TPacketCGMessengerAddByVID * p2 = (TPacketCGMessengerAddByVID *) c_pData;
		LPCHARACTER ch_companion = CHARACTER_MANAGER::instance().Find(p2->vid);

		if (!ch_companion)
		{
			return sizeof(TPacketCGMessengerAddByVID);
		}

		if (ch->IsObserverMode())
		{
			return sizeof(TPacketCGMessengerAddByVID);
		}

		if (ch_companion->IsBlockMode(BLOCK_MESSENGER_INVITE))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player has rejected your request to add him to your friend list."));
			return sizeof(TPacketCGMessengerAddByVID);
		}

		LPDESC d = ch_companion->GetDesc();

		if (!d)
		{
			return sizeof(TPacketCGMessengerAddByVID);
		}

		if (ch->GetGMLevel() == GM_PLAYER && ch_companion->GetGMLevel() != GM_PLAYER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Friends] You cannot add a GM to your list."));
			return sizeof(TPacketCGMessengerAddByVID);
		}

		if (ch->GetDesc() == d)
		{
			return sizeof(TPacketCGMessengerAddByVID);
		}

		MessengerManager::instance().RequestToAdd(ch, ch_companion);
		//MessengerManager::instance().AddToList(ch->GetName(), ch_companion->GetName());
	}
	return sizeof(TPacketCGMessengerAddByVID);

	case MESSENGER_SUBHEADER_CG_ADD_BY_NAME:
	{
		if (uiBytes < CHARACTER_NAME_MAX_LEN)
		{
			return -1;
		}

		char name[CHARACTER_NAME_MAX_LEN + 1];
		strlcpy(name, c_pData, sizeof(name));

#ifdef __ADMIN_MANAGER__
		if (ch->GetGMLevel() == GM_PLAYER && GM::get_level(name) != GM_PLAYER)
#else
		if (ch->GetGMLevel() == GM_PLAYER && gm_get_level(name) != GM_PLAYER)
#endif
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Friends] You cannot add a GM to your list."));
			return CHARACTER_NAME_MAX_LEN;
		}

		LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(name);

		if (!tch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s is not online."), name);
		}
		else
		{
			if (tch == ch)
			{
				return CHARACTER_NAME_MAX_LEN;
			}

			if (tch->IsBlockMode(BLOCK_MESSENGER_INVITE) == true)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player has rejected your request to add him to your friend list."));
			}
			else
			{

				MessengerManager::instance().RequestToAdd(ch, tch);
				//MessengerManager::instance().AddToList(ch->GetName(), tch->GetName());
			}
		}
	}
	return CHARACTER_NAME_MAX_LEN;

	case MESSENGER_SUBHEADER_CG_REMOVE:
	{
		if (uiBytes < CHARACTER_NAME_MAX_LEN)
		{
			return -1;
		}

		char char_name[CHARACTER_NAME_MAX_LEN + 1];
		strlcpy(char_name, c_pData, sizeof(char_name));
		MessengerManager::instance().RemoveFromList(ch->GetName(), char_name);
		MessengerManager::instance().RemoveFromList(char_name, ch->GetName());
	}
	return CHARACTER_NAME_MAX_LEN;

	default:
		sys_err("CInputMain::Messenger : Unknown subheader %d : %s", p->subheader, ch->GetName());
		break;
	}

	return 0;
}

typedef struct CSellPacket
{
	WORD		bySlot;
	CountType	byCount;
	BYTE		byType;
} TSellPacket;

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
typedef struct CMultipleBuy
{
	BYTE		bySlot;
	CountType	byCount;
} TMultipleBuy;
#endif

int CInputMain::Shop(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	TPacketCGShop * p = (TPacketCGShop *) data;

	if (uiBytes < sizeof(TPacketCGShop))
	{
		return -1;
	}

	if (test_server)
	{
		sys_log(0, "CInputMain::Shop() ==> SubHeader %d", p->subheader);
	}

	const char * c_pData = data + sizeof(TPacketCGShop);
	uiBytes -= sizeof(TPacketCGShop);

	switch (p->subheader)
	{
	case SHOP_SUBHEADER_CG_END:
		sys_log(1, "INPUT: %s SHOP: END", ch->GetName());
		CShopManager::instance().StopShopping(ch);
		return 0;

	case SHOP_SUBHEADER_CG_BUY:
	{
		if (uiBytes < sizeof(BYTE) + sizeof(BYTE))
		{
			return -1;
		}

		BYTE bPos = *(c_pData + 1);
		sys_log(1, "INPUT: %s SHOP: BUY %d", ch->GetName(), bPos);
		CShopManager::instance().Buy(ch, bPos);
		return (sizeof(BYTE) + sizeof(BYTE));
	}

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	case SHOP_SUBHEADER_GC_BUY_MULTIPLE:
	{
		if (uiBytes < sizeof(TMultipleBuy))
		{
			return -1;
		}

		TMultipleBuy* multiple = (TMultipleBuy*)c_pData;

		sys_log(1, "INPUT: %s SHOP: BUY MULTIPLE %d", ch->GetName(), multiple->bySlot);

		CShopManager::instance().Buy(ch, multiple->bySlot, multiple->byCount);
		return sizeof(TMultipleBuy);
	}
#endif

	case SHOP_SUBHEADER_CG_SELL:
	{
		if (uiBytes < sizeof(BYTE))
		{
			return -1;
		}

		BYTE pos = *c_pData;

		sys_log(0, "INPUT: %s SHOP: SELL", ch->GetName());
		CShopManager::instance().Sell(ch, pos);
		return sizeof(BYTE);
	}

	case SHOP_SUBHEADER_CG_SELL2:
	{
		if (uiBytes < sizeof(TSellPacket))
		{
			return -1;
		}

		TSellPacket* p2 = (TSellPacket*)c_pData;

		sys_log(0, "INPUT: %s SHOP: SELL2 | wPos: %d bCount: %d bType: %d", ch->GetName(), p2->bySlot, p2->byCount, p2->byType);
		CShopManager::instance().Sell(ch, p2->bySlot, p2->byCount, p2->byType);
		return sizeof(TSellPacket);
	}

	default:
		sys_err("CInputMain::Shop : Unknown subheader %d : %s", p->subheader, ch->GetName());
		break;
	}

	return 0;
}

void CInputMain::OnClick(LPCHARACTER ch, const char * data)
{
	struct command_on_click *	pinfo = (struct command_on_click *) data;
	LPCHARACTER			victim;

	if ((victim = CHARACTER_MANAGER::instance().Find(pinfo->vid)))
	{
		victim->OnClick(ch);
	}
	else if (test_server)
	{
		sys_err("CInputMain::OnClick %s.Click.NOT_EXIST_VID[%d]", ch->GetName(), pinfo->vid);
	}
}

void CInputMain::Exchange(LPCHARACTER ch, const char * data)
{
	struct command_exchange * pinfo = (struct command_exchange *) data;
	LPCHARACTER	to_ch = NULL;

	if (!ch->CanHandleItem())
	{
		return;
	}

	int iPulse = thecore_pulse();

	if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
	{
		if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you have to wait %d seconds before you can open a shop."), g_nPortalLimitTime);
			return;
		}

		if ( true == to_ch->IsDead() )
		{
			return;
		}
	}

	sys_log(0, "CInputMain()::Exchange()  SubHeader %d ", pinfo->sub_header);

	if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you have to wait %d seconds before you can open a shop."), g_nPortalLimitTime);
		return;
	}

#ifdef __ACTION_RESTRICTIONS__
	if (ch->GetActionRestrictions())
	{
		if (ch->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_EXCHANGE) == false) { return; }
	}
#endif

	switch (pinfo->sub_header)
	{
	case EXCHANGE_SUBHEADER_CG_START:	// arg1 == vid of target character
		if (!ch->GetExchange())
		{
			if ((to_ch = CHARACTER_MANAGER::instance().Find(pinfo->arg1)))
			{
				if (iPulse - ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can trade again in %d seconds."), g_nPortalLimitTime);

					if (test_server)
					{
						ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
					}
					return;
				}

				if (iPulse - to_ch->GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
				{
					to_ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can trade again in %d seconds."), g_nPortalLimitTime);


					if (test_server)
					{
						to_ch->ChatPacket(CHAT_TYPE_INFO, "[TestOnly][Safebox]Pulse %d LoadTime %d PASS %d", iPulse, to_ch->GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
					}
					return;
				}

				if (ch->GetGold() >= GOLD_MAX)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have more than 2 Billion Yang. You cannot trade."));

#ifdef __EXTANDED_GOLD_AMOUNT__
					sys_err("[OVERFLOG_GOLD] START (%lld) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#else
					sys_err("[OVERFLOG_GOLD] START (%u) id %u name %s ", ch->GetGold(), ch->GetPlayerID(), ch->GetName());
#endif
					return;
				}

				if (to_ch->IsPC())
				{
					if (quest::CQuestManager::instance().GiveItemToPC(ch->GetPlayerID(), to_ch))
					{
						sys_log(0, "Exchange canceled by quest %s %s", ch->GetName(), to_ch->GetName());
						return;
					}
				}

				if (ch->GetMyShop() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a private shop while another window is open."));
					return;
				}

				ch->ExchangeStart(to_ch);
			}
		}
		break;

	case EXCHANGE_SUBHEADER_CG_ITEM_ADD:	// arg1 == position of item, arg2 == position in exchange window
		if (ch->GetExchange())
		{
			if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
			{
				ch->GetExchange()->AddItem(pinfo->Pos, pinfo->arg2);
			}
		}
		break;

	case EXCHANGE_SUBHEADER_CG_ITEM_DEL:	// arg1 == position of item
		if (ch->GetExchange())
		{
			if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
			{
				ch->GetExchange()->RemoveItem(pinfo->arg1);
			}
		}
		break;

	case EXCHANGE_SUBHEADER_CG_ELK_ADD:	// arg1 == amount of gold
#ifdef __EXTANDED_GOLD_AMOUNT__
		if (CExchange* exchange = ch->GetExchange())
		{
			LPCHARACTER owner = exchange->GetCompany()->GetOwner();
			const int64_t maxGoldAdd = GOLD_MAX - (owner->GetGold());

			if (pinfo->arg1 > maxGoldAdd)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player has more than 2 Billion Yang. You cannot trade with him."));

				sys_err("[OVERFLOW_GOLD] ELK_ADD (%lld) id %u name %s ",
						owner->GetGold(),
						owner->GetPlayerID(),
						owner->GetName());

				return;
			}

			if (exchange->GetCompany()->GetAcceptStatus() != true)
			{
				exchange->AddGold(pinfo->arg1);
			}
#else

		if (ch->GetExchange())
		{
			const int64_t nTotalGold = static_cast<int64_t>(ch->GetExchange()->GetCompany()->GetOwner()->GetGold()) + static_cast<int64_t>(pinfo->arg1);

			if (GOLD_MAX <= nTotalGold)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The player has more than 2 Billion Yang. You cannot trade with him."));

				sys_err("[OVERFLOW_GOLD] ELK_ADD (%u) id %u name %s ",
						ch->GetExchange()->GetCompany()->GetOwner()->GetGold(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetPlayerID(),
						ch->GetExchange()->GetCompany()->GetOwner()->GetName());

				return;
			}

			if (ch->GetExchange()->GetCompany()->GetAcceptStatus() != true)
			{
				ch->GetExchange()->AddGold(pinfo->arg1);
			}
#endif
		}
		break;

	case EXCHANGE_SUBHEADER_CG_ACCEPT:	// arg1 == not used
		if (ch->GetExchange())
		{
			sys_log(0, "CInputMain()::Exchange() ==> ACCEPT ");
			ch->GetExchange()->Accept(true);
		}

		break;

	case EXCHANGE_SUBHEADER_CG_CANCEL:	// arg1 == not used
		if (ch->GetExchange())
		{
			ch->GetExchange()->Cancel();
		}
		break;
	}
}

void CInputMain::Position(LPCHARACTER ch, const char * data)
{
	struct command_position * pinfo = (struct command_position *) data;

	switch (pinfo->position)
	{
	case POSITION_GENERAL:
		ch->Standup();
		break;

	case POSITION_SITTING_CHAIR:
		ch->Sitdown(0);
		break;

	case POSITION_SITTING_GROUND:
		ch->Sitdown(1);
		break;
	}
}

static const int ComboSequenceBySkillLevel[3][8] =
{
	// 0   1   2   3   4   5   6   7
	{ 14, 15, 16, 17,  0,  0,  0,  0 },
	{ 14, 15, 16, 18, 20,  0,  0,  0 },
	{ 14, 15, 16, 18, 19, 17,  0,  0 },
};

#define COMBO_HACK_ALLOWABLE_MS	100

bool CheckComboHack(LPCHARACTER ch, BYTE bArg, DWORD dwTime, bool CheckSpeedHack)
{
	if (!gHackCheckEnable) { return false; }



	//	if (IsStun() || IsDead()) return false;




	if (ch->IsStun() || ch->IsDead())
	{
		return false;
	}
	int ComboInterval = dwTime - ch->GetLastComboTime();
	int HackScalar = 0;
#if 0
	sys_log(0, "COMBO: %s arg:%u seq:%u delta:%d checkspeedhack:%d",
			ch->GetName(), bArg, ch->GetComboSequence(), ComboInterval - ch->GetValidComboInterval(), CheckSpeedHack);
#endif




	if (bArg == 14)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{





			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 300;

			//sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED),
			//	    ch->IsRiding() ? "yes" : "no");
		}

		ch->SetComboSequence(1);
		ch->SetValidComboInterval((int) (ani_combo_speed(ch, 1) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
		ch->SetLastComboTime(dwTime);
	}
	else if (bArg > 14 && bArg < 22)
	{
		int idx = MIN(2, ch->GetComboIndex());

		if (ch->GetComboSequence() > 5)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);
			sys_log(0, "COMBO_HACK: 5 %s combo_seq:%d", ch->GetName(), ch->GetComboSequence());
		}

		else if (bArg == 21 &&
				 idx == 2 &&
				 ch->GetComboSequence() == 5 &&
				 ch->GetJob() == JOB_ASSASSIN &&
				 ch->GetWear(WEAR_WEAPON) &&
				 ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_DAGGER)
		{
			ch->SetValidComboInterval(300);
		}
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		else if (bArg == 21 && idx == 2 && ch->GetComboSequence() == 5 && ch->GetJob() == JOB_WOLFMAN && ch->GetWear(WEAR_WEAPON) && ch->GetWear(WEAR_WEAPON)->GetSubType() == WEAPON_CLAW)
		{
			ch->SetValidComboInterval(300);
		}
#endif
		else if (ComboSequenceBySkillLevel[idx][ch->GetComboSequence()] != bArg)
		{
			HackScalar = 1;
			ch->SetValidComboInterval(300);

			sys_log(0, "COMBO_HACK: 3 %s arg:%u valid:%u combo_idx:%d combo_seq:%d",
					ch->GetName(),
					bArg,
					ComboSequenceBySkillLevel[idx][ch->GetComboSequence()],
					idx,
					ch->GetComboSequence());
		}
		else
		{
			if (CheckSpeedHack && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
			{
				HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

				sys_log(0, "COMBO_HACK: 2 %s arg:%u interval:%d valid:%u atkspd:%u riding:%s",
						ch->GetName(),
						bArg,
						ComboInterval,
						ch->GetValidComboInterval(),
						ch->GetPoint(POINT_ATT_SPEED),
						ch->IsRiding() ? "yes" : "no");
			}


			//if (ch->IsHorseRiding())
			if (ch->IsRiding())
			{
				ch->SetComboSequence(ch->GetComboSequence() == 1 ? 2 : 1);
			}
			else
			{
				ch->SetComboSequence(ch->GetComboSequence() + 1);
			}

			ch->SetValidComboInterval((int) (ani_combo_speed(ch, bArg - 13) / (ch->GetPoint(POINT_ATT_SPEED) / 100.f)));
			ch->SetLastComboTime(dwTime);
		}
	}
	else if (bArg == 13)
	{
		if (CheckSpeedHack && ComboInterval > 0 && ComboInterval < ch->GetValidComboInterval() - COMBO_HACK_ALLOWABLE_MS)
		{



			//HackScalar = 1 + (ch->GetValidComboInterval() - ComboInterval) / 100;

			//sys_log(0, "COMBO_HACK: 6 %s arg:%u interval:%d valid:%u atkspd:%u",
			//		ch->GetName(),
			//		bArg,
			//		ComboInterval,
			//		ch->GetValidComboInterval(),
			//		ch->GetPoint(POINT_ATT_SPEED));
		}

		if (ch->GetRaceNum() >= MAIN_RACE_MAX_NUM)
		{
			// POLYMORPH_BUG_FIX

			// DELETEME
			/*
			const CMotion * pkMotion = CMotionManager::instance().GetMotion(ch->GetRaceNum(), MAKE_MOTION_KEY(MOTION_MODE_GENERAL, MOTION_NORMAL_ATTACK));

			if (!pkMotion)
				sys_err("cannot find motion by race %u", ch->GetRaceNum());
			else
			{


				int k = (int) (pkMotion->GetDuration() / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
				ch->SetValidComboInterval(k);
				ch->SetLastComboTime(dwTime);
			}
			*/
			float normalAttackDuration = CMotionManager::instance().GetNormalAttackDuration(ch->GetRaceNum());
			int k = (int) (normalAttackDuration / ((float) ch->GetPoint(POINT_ATT_SPEED) / 100.f) * 900.f);
			ch->SetValidComboInterval(k);
			ch->SetLastComboTime(dwTime);
			// END_OF_POLYMORPH_BUG_FIX
		}
		else
		{

			//if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
			//{
			//	LogManager::instance().HackLog("Hacker", ch);
			//	sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
			//}







			//

			//




			// by rtsummit
		}
	}
	else
	{

		if (ch->GetDesc()->DelayedDisconnect(number(2, 9)))
		{
			LogManager::instance().HackLog("Hacker", ch);
			sys_log(0, "HACKER: %s arg %u", ch->GetName(), bArg);
		}

		HackScalar = 10;
		ch->SetValidComboInterval(300);
	}

	if (HackScalar)
	{

		if (get_dword_time() - ch->GetLastMountTime() > 1500)
		{
			ch->IncreaseComboHackCount(1 + HackScalar);
		}

		ch->SkipComboAttackByTime(ch->GetValidComboInterval());
	}

	return HackScalar;


}

void CInputMain::Move(LPCHARACTER ch, const char * data)
{
	if (!ch->CanMove())
	{
		return;
	}

	struct command_move * pinfo = (struct command_move *) data;

	if (pinfo->bFunc >= FUNC_MAX_NUM && !(pinfo->bFunc & 0x80))
	{
		sys_err("invalid move type: %s", ch->GetName());
		return;
	}

	//enum EMoveFuncType
	//{
	//	FUNC_WAIT,
	//	FUNC_MOVE,
	//	FUNC_ATTACK,
	//	FUNC_COMBO,
	//	FUNC_MOB_SKILL,
	//	_FUNC_SKILL,
	//	FUNC_MAX_NUM,
	//	FUNC_SKILL = 0x80,
	//};



//	if (!test_server)
	{
		const float fDist = DISTANCE_SQRT((ch->GetX() - pinfo->lX) / 100, (ch->GetY() - pinfo->lY) / 100);
		// @fixme106 (changed 40 to 60)
		if (((false == ch->IsRiding() && fDist > 32) || fDist > 75) && OXEVENT_MAP_INDEX != ch->GetMapIndex())
		{
#ifdef ENABLE_HACK_TELEPORT_LOG // @warme006
			{
				const PIXEL_POSITION & warpPos = ch->GetWarpPosition();

				if (warpPos.x == 0 && warpPos.y == 0)
				{
					LogManager::instance().HackLog("Teleport", ch);
				}
			}
#endif
			sys_log(0, "MOVE: %s trying to move too far (dist: %.1fm) Riding(%d)", ch->GetName(), fDist, ch->IsRiding());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#ifdef ENABLE_CHECK_GHOSTMODE
		if (ch->IsPC() && ch->IsDead())
		{
			sys_log(0, "MOVE: %s trying to move as dead", ch->GetName());

			ch->Show(ch->GetMapIndex(), ch->GetX(), ch->GetY(), ch->GetZ());
			ch->Stop();
			return;
		}
#endif

		DWORD dwCurTime = get_dword_time();

		bool CheckSpeedHack = (false == ch->GetDesc()->IsHandshaking() && dwCurTime - ch->GetDesc()->GetClientTime() > 7000);

		if (CheckSpeedHack)
		{
			int iDelta = (int) (pinfo->dwTime - ch->GetDesc()->GetClientTime());
			int iServerDelta = (int) (dwCurTime - ch->GetDesc()->GetClientTime());

			iDelta = (int) (dwCurTime - pinfo->dwTime);


			if (iDelta >= 30000)
			{
				sys_log(0, "SPEEDHACK: slow timer name %s delta %d", ch->GetName(), iDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}

			else if (iDelta < -(iServerDelta / 50))
			{
				sys_log(0, "SPEEDHACK: DETECTED! %s (delta %d %d)", ch->GetName(), iDelta, iServerDelta);
				ch->GetDesc()->DelayedDisconnect(3);
			}
		}

		if (pinfo->bFunc == FUNC_COMBO && g_bCheckMultiHack)
		{
			CheckComboHack(ch, pinfo->bArg, pinfo->dwTime, CheckSpeedHack);
		}
	}

	if (pinfo->bFunc == FUNC_MOVE)
	{
		if (ch->GetLimitPoint(POINT_MOV_SPEED) == 0)
		{
			return;
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Goto(pinfo->lX, pinfo->lY);
	}
	else
	{
		if (pinfo->bFunc == FUNC_ATTACK || pinfo->bFunc == FUNC_COMBO)
		{
			ch->OnMove(true);
#ifdef __ABUSE_CONTROLLER_ENABLE__
			ch->GetAbuseController()->ReceiveMoveAttackPacket();
#endif
		}
		else if (pinfo->bFunc & FUNC_SKILL)
		{
			const int MASK_SKILL_MOTION = 0x7F;
			unsigned int motion = pinfo->bFunc & MASK_SKILL_MOTION;

			if (!ch->IsUsableSkillMotion(motion))
			{
				const char* name = ch->GetName();
				unsigned int job = ch->GetJob();
				unsigned int group = ch->GetSkillGroup();

				char szBuf[256];
				snprintf(szBuf, sizeof(szBuf), "SKILL_HACK: name=%s, job=%d, group=%d, motion=%d", name, job, group, motion);
				LogManager::instance().HackLog(szBuf, ch->GetDesc()->GetAccountTable().login, ch->GetName(), ch->GetDesc()->GetHostName());
				sys_log(0, "%s", szBuf);

				if (test_server)
				{
					ch->GetDesc()->DelayedDisconnect(number(2, 8));
					ch->ChatPacket(CHAT_TYPE_INFO, szBuf);
				}
				else
				{
					ch->GetDesc()->DelayedDisconnect(number(150, 500));
				}
			}
#ifdef __ABUSE_CONTROLLER_ENABLE__
			ch->GetAbuseController()->ReceiveMoveAttackPacket();
#endif
			ch->OnMove();
		}

		ch->SetRotation(pinfo->bRot * 5);
		ch->ResetStopTime();				// ""

		ch->Move(pinfo->lX, pinfo->lY);
		ch->Stop();
		ch->StopStaminaConsume();
	}

	TPacketGCMove pack;

	pack.bHeader      = HEADER_GC_MOVE;
	pack.bFunc        = pinfo->bFunc;
	pack.bArg         = pinfo->bArg;
	pack.bRot         = pinfo->bRot;
	pack.dwVID        = ch->GetVID();
	pack.lX           = pinfo->lX;
	pack.lY           = pinfo->lY;
	pack.dwTime       = pinfo->dwTime;
	pack.dwDuration   = (pinfo->bFunc == FUNC_MOVE) ? ch->GetCurrentMoveDuration() : 0;

	ch->PacketAround(&pack, sizeof(TPacketGCMove), ch);

	/*
	sys_log(0,
			"MOVE: %s Func:%u Arg:%u Pos:%dx%d Time:%u Dist:%.1f",
			ch->GetName(),
			pinfo->bFunc,
			pinfo->bArg,
			pinfo->lX / 100,
			pinfo->lY / 100,
			pinfo->dwTime,
			fDist);
	*/
}

void CInputMain::Attack(LPCHARACTER ch, const BYTE header, const char* data)
{
	if (NULL == ch)
	{
		return;
	}

	struct type_identifier
	{
		BYTE header;
		BYTE type;
	};

	const struct type_identifier* const type = reinterpret_cast<const struct type_identifier*>(data);

	if (type->type > 0)
	{
		if (false == ch->CanUseSkill(type->type))
		{
			return;
		}

		switch (type->type)
		{
		case SKILL_GEOMPUNG:
		case SKILL_SANGONG:
		case SKILL_YEONSA:
		case SKILL_KWANKYEOK:
		case SKILL_HWAJO:
		case SKILL_GIGUNG:
		case SKILL_PABEOB:
		case SKILL_MARYUNG:
		case SKILL_TUSOK:
		case SKILL_MAHWAN:
		case SKILL_BIPABU:
		case SKILL_NOEJEON:
		case SKILL_CHAIN:
		case SKILL_HORSE_WILDATTACK_RANGE:
			if (HEADER_CG_SHOOT != type->header)
			{
				if (test_server)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Attack :name[%s] Vnum[%d] can't use skill by attack(warning)"), type->type);
				}
				return;
			}
			break;
		}
	}

	switch (header)
	{
	case HEADER_CG_ATTACK:
	{
		if (NULL == ch->GetDesc())
		{
			return;
		}

		const TPacketCGAttack* const packMelee = reinterpret_cast<const TPacketCGAttack*>(data);

		ch->GetDesc()->AssembleCRCMagicCube(packMelee->bCRCMagicCubeProcPiece, packMelee->bCRCMagicCubeFilePiece);

		LPCHARACTER	victim = CHARACTER_MANAGER::instance().Find(packMelee->dwVID);

		if (NULL == victim || ch == victim)
		{
			return;
		}

		switch (victim->GetCharType())
		{
		case CHAR_TYPE_NPC:
		case CHAR_TYPE_WARP:
		case CHAR_TYPE_GOTO:
			return;
		}

		if (packMelee->bType > 0)
		{
			if (false == ch->CheckSkillHitCount(packMelee->bType, victim->GetVID()))
			{
				return;
			}
		}

		ch->Attack(victim, packMelee->bType);
	}
	break;

	case HEADER_CG_SHOOT:
	{
		const TPacketCGShoot* const packShoot = reinterpret_cast<const TPacketCGShoot*>(data);

		ch->Shoot(packShoot->bType);
	}
	break;
	}
}

int CInputMain::SyncPosition(LPCHARACTER ch, const char * c_pcData, size_t uiBytes)
{
	const TPacketCGSyncPosition* pinfo = reinterpret_cast<const TPacketCGSyncPosition*>( c_pcData );

	if (uiBytes < pinfo->wSize)
	{
		return -1;
	}

	int iExtraLen = pinfo->wSize - sizeof(TPacketCGSyncPosition);

	if (iExtraLen < 0)
	{
		sys_err("invalid packet length (len %d size %u buffer %u)", iExtraLen, pinfo->wSize, uiBytes);
		ch->GetDesc()->SetPhase(PHASE_CLOSE);
		return -1;
	}

	if (0 != (iExtraLen % sizeof(TPacketCGSyncPositionElement)))
	{
		sys_err("invalid packet length %d (name: %s)", pinfo->wSize, ch->GetName());
		return iExtraLen;
	}

	int iCount = iExtraLen / sizeof(TPacketCGSyncPositionElement);

	if (iCount <= 0)
	{
		return iExtraLen;
	}

	static const int nCountLimit = 16;

	if ( iCount > nCountLimit )
	{
		//LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );
		sys_err( "Too many SyncPosition Count(%d) from Name(%s)", iCount, ch->GetName() );
		//ch->GetDesc()->SetPhase(PHASE_CLOSE);
		//return -1;
		iCount = nCountLimit;
	}

	TEMP_BUFFER tbuf;
	LPBUFFER lpBuf = tbuf.getptr();

	TPacketGCSyncPosition * pHeader = (TPacketGCSyncPosition *) buffer_write_peek(lpBuf);
	buffer_write_proceed(lpBuf, sizeof(TPacketGCSyncPosition));

	const TPacketCGSyncPositionElement* e =
		reinterpret_cast<const TPacketCGSyncPositionElement*>(c_pcData + sizeof(TPacketCGSyncPosition));

	timeval tvCurTime;
	gettimeofday(&tvCurTime, NULL);

	for (int i = 0; i < iCount; ++i, ++e)
	{
		LPCHARACTER victim = CHARACTER_MANAGER::instance().Find(e->dwVID);

		if (!victim)
		{
			continue;
		}

		switch (victim->GetCharType())
		{
		case CHAR_TYPE_NPC:
		case CHAR_TYPE_WARP:
		case CHAR_TYPE_GOTO:
			continue;
		}


		if (!victim->SetSyncOwner(ch))
		{
			continue;
		}

		const float fDistWithSyncOwner = DISTANCE_SQRT( (victim->GetX() - ch->GetX()) / 100, (victim->GetY() - ch->GetY()) / 100 );
		static const float fLimitDistWithSyncOwner = 2500.f + 1000.f;





		if (fDistWithSyncOwner > fLimitDistWithSyncOwner)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too far SyncPosition DistanceWithSyncOwner(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
						 fDistWithSyncOwner, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
						 e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}

		const float fDist = DISTANCE_SQRT( (victim->GetX() - e->lX) / 100, (victim->GetY() - e->lY) / 100 );
		static const long g_lValidSyncInterval = 100 * 1000; // 100ms
		const timeval &tvLastSyncTime = victim->GetLastSyncTime();
		timeval *tvDiff = timediff(&tvCurTime, &tvLastSyncTime);



		if (tvDiff->tv_sec == 0 && tvDiff->tv_usec < g_lValidSyncInterval)
		{
			if (ch->GetSyncHackCount() < g_iSyncHackLimitCount)
			{
				ch->SetSyncHackCount(ch->GetSyncHackCount() + 1);
				continue;
			}
			else
			{
				LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

				sys_err( "Too often SyncPosition Interval(%ldms)(%s) from Name(%s) VICTIM(%d,%d) SYNC(%d,%d)",
						 tvDiff->tv_sec * 1000 + tvDiff->tv_usec / 1000, victim->GetName(), ch->GetName(), victim->GetX(), victim->GetY(),
						 e->lX, e->lY );

				ch->GetDesc()->SetPhase(PHASE_CLOSE);

				return -1;
			}
		}
		else if ( fDist > 25.0f )
		{
			LogManager::instance().HackLog( "SYNC_POSITION_HACK", ch );

			sys_err( "Too far SyncPosition Distance(%f)(%s) from Name(%s) CH(%d,%d) VICTIM(%d,%d) SYNC(%d,%d)",
					 fDist, victim->GetName(), ch->GetName(), ch->GetX(), ch->GetY(), victim->GetX(), victim->GetY(),
					 e->lX, e->lY );

			ch->GetDesc()->SetPhase(PHASE_CLOSE);

			return -1;
		}
		else
		{
			victim->SetLastSyncTime(tvCurTime);
			victim->Sync(e->lX, e->lY);
			buffer_write(lpBuf, e, sizeof(TPacketCGSyncPositionElement));
		}
	}

	if (buffer_size(lpBuf) != sizeof(TPacketGCSyncPosition))
	{
		pHeader->bHeader = HEADER_GC_SYNC_POSITION;
		pHeader->wSize = buffer_size(lpBuf);

		ch->PacketAround(buffer_read_peek(lpBuf), buffer_size(lpBuf), ch);
	}

	return iExtraLen;
}

void CInputMain::FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader)
{
	TPacketCGFlyTargeting * p = (TPacketCGFlyTargeting *) pcData;
	ch->FlyTarget(p->dwTargetVID, p->x, p->y, bHeader);
}

void CInputMain::UseSkill(LPCHARACTER ch, const char * pcData)
{
	TPacketCGUseSkill * p = (TPacketCGUseSkill *) pcData;
	ch->UseSkill(p->dwVnum, CHARACTER_MANAGER::instance().Find(p->dwVID));
}

void CInputMain::ScriptButton(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptButton * p = (TPacketCGScriptButton *) c_pData;
	sys_log(0, "QUEST ScriptButton pid %d idx %u", ch->GetPlayerID(), p->idx);

	quest::PC* pc = quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID());
	if (pc && pc->IsConfirmWait())
	{
		quest::CQuestManager::instance().Confirm(ch->GetPlayerID(), quest::CONFIRM_TIMEOUT);
	}
	else if (p->idx & 0x80000000)
	{

		quest::CQuestManager::Instance().QuestInfo(ch->GetPlayerID(), p->idx & 0x7fffffff);
	}
	else
	{
		quest::CQuestManager::Instance().QuestButton(ch->GetPlayerID(), p->idx);
	}
}

void CInputMain::ScriptAnswer(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptAnswer * p = (TPacketCGScriptAnswer *) c_pData;
	sys_log(0, "QUEST ScriptAnswer pid %d answer %d", ch->GetPlayerID(), p->answer);

	if (p->answer > 250)
	{
		quest::CQuestManager::Instance().Resume(ch->GetPlayerID());
	}
	else
	{
		quest::CQuestManager::Instance().Select(ch->GetPlayerID(),  p->answer);
	}
}


// SCRIPT_SELECT_ITEM
void CInputMain::ScriptSelectItem(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGScriptSelectItem* p = (TPacketCGScriptSelectItem*) c_pData;
	sys_log(0, "QUEST ScriptSelectItem pid %d answer %d", ch->GetPlayerID(), p->selection);
	quest::CQuestManager::Instance().SelectItem(ch->GetPlayerID(), p->selection);
}
// END_OF_SCRIPT_SELECT_ITEM

void CInputMain::QuestInputString(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestInputString * p = (TPacketCGQuestInputString*) c_pData;

	char msg[65];
	strlcpy(msg, p->msg, sizeof(msg));
	sys_log(0, "QUEST InputString pid %u msg %s", ch->GetPlayerID(), msg);

	quest::CQuestManager::Instance().Input(ch->GetPlayerID(), msg);
}

void CInputMain::QuestConfirm(LPCHARACTER ch, const void* c_pData)
{
	TPacketCGQuestConfirm* p = (TPacketCGQuestConfirm*) c_pData;
	LPCHARACTER ch_wait = CHARACTER_MANAGER::instance().FindByPID(p->requestPID);
	if (p->answer)
	{
		p->answer = quest::CONFIRM_YES;
	}
	sys_log(0, "QuestConfirm from %s pid %u name %s answer %d", ch->GetName(), p->requestPID, (ch_wait) ? ch_wait->GetName() : "", p->answer);
	if (ch_wait)
	{
		quest::CQuestManager::Instance().Confirm(ch_wait->GetPlayerID(), (quest::EQuestConfirmType) p->answer, ch->GetPlayerID());
	}
}

void CInputMain::Target(LPCHARACTER ch, const char * pcData)
{
	TPacketCGTarget * p = (TPacketCGTarget *) pcData;

	building::LPOBJECT pkObj = building::CManager::instance().FindObjectByVID(p->dwVID);

	if (pkObj)
	{
		TPacketGCTarget pckTarget;
		pckTarget.header = HEADER_GC_TARGET;
		pckTarget.dwVID = p->dwVID;
		ch->GetDesc()->Packet(&pckTarget, sizeof(TPacketGCTarget));
	}
	else
	{
		ch->SetTarget(CHARACTER_MANAGER::instance().Find(p->dwVID));
	}
}

void CInputMain::Warp(LPCHARACTER ch, const char * pcData)
{
	ch->WarpEnd();
}

void CInputMain::SafeboxCheckin(LPCHARACTER ch, const char * c_pData)
{
	if (quest::CQuestManager::instance().GetPCForce(ch->GetPlayerID())->IsRunning() == true)
	{
		return;
	}

	TPacketCGSafeboxCheckin * p = (TPacketCGSafeboxCheckin *) c_pData;

	if (!ch->CanHandleItem())
	{
		return;
	}

	// Blocking items beyond inventory
#ifndef __SPECIAL_STORAGE_ENABLE__
	if (p->ItemPos.cell < 0 || p->ItemPos.cell >= INVENTORY_MAX_NUM)
#else
	if (p->ItemPos.cell < 0 || (p->ItemPos.cell >= INVENTORY_MAX_NUM && p->ItemPos.cell < SPECIAL_STORAGE_START_CELL) || p->ItemPos.cell >= INVENTORY_AND_EQUIP_SLOT_MAX)
#endif
		return;

	CSafebox * pkSafebox = ch->GetSafebox();
	LPITEM pkItem = ch->GetItem(p->ItemPos);

	if (!pkSafebox || !pkItem)
	{
		return;
	}

	// Random item relocation
	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		int iNewPos = pkSafebox->GetEmptyPos(pkItem->GetSize());
		if (iNewPos == -1)
		{
			return;
		}

		p->bSafePos = iNewPos;
	}

	if (pkItem->GetType() == ITEM_BELT && pkItem->IsEquipped())
	{
		return;
	}

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (pkItem->GetCell() >= INVENTORY_MAX_NUM && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
#else
	if ((pkItem->GetCell() >= INVENTORY_MAX_NUM && pkItem->GetCell() < SPECIAL_STORAGE_START_CELL) && IS_SET(pkItem->GetFlag(), ITEM_FLAG_IRREMOVABLE))
#endif
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<창고> 창고로 옮길 수 없는 아이템 입니다."));
		return;
	}

	if (!pkSafebox->IsEmpty(p->bSafePos, pkItem->GetSize()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] No movement possible."));
		return;
	}

	if (pkItem->GetVnum() == UNIQUE_ITEM_SAFEBOX_EXPAND)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The item cannot be stored."));
		return;
	}

	if ( IS_SET(pkItem->GetAntiFlag(), ITEM_ANTIFLAG_SAFEBOX) )
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The item cannot be stored."));
		return;
	}

	if (true == pkItem->isLocked())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] The item cannot be stored."));
		return;
	}

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	if (pkItem->IsEquipped())
	{
		int iWearCell = pkItem->FindEquipCell(ch);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = ch->GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !ch->UnequipItem(costumeWeapon))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return;
			}
		}
	}
#endif

	// @fixme140 BEGIN
	if (ITEM_BELT == pkItem->GetType() && CBeltInventoryHelper::IsExistItemInBeltInventory(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only discard the belt when there are no longer any items in its inventory."));
		return;
	}
	// @fixme140 END

	pkItem->RemoveFromCharacter();
	if (!pkItem->IsDragonSoul())
	{
		ch->SyncQuickslot(QUICKSLOT_TYPE_ITEM, p->ItemPos.cell, 255);
	}
	pkSafebox->Add(p->bSafePos, pkItem);

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX PUT", szHint);
}

void CInputMain::SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall)
{
	TPacketCGSafeboxCheckout * p = (TPacketCGSafeboxCheckout *) c_pData;

	if (!ch->CanHandleItem())
	{
		return;
	}

	CSafebox * pkSafebox;

	if (bMall)
	{
		pkSafebox = ch->GetMall();
	}
	else
	{
		pkSafebox = ch->GetSafebox();
	}

	if (!pkSafebox)
	{
		return;
	}

	LPITEM pkItem = pkSafebox->Get(p->bSafePos);

	if (!pkItem)
	{
		return;
	}

#ifdef __ENABLE_FAST_INTERACTION_SAFEBOX___
#ifndef __SPECIAL_STORAGE_ENABLE__
	if (p->ItemPos.cell >= INVENTORY_MAX_NUM)
#else
	if ((p->ItemPos.cell >= INVENTORY_MAX_NUM && (p->ItemPos.cell < SPECIAL_STORAGE_START_CELL)) || p->ItemPos.cell >= INVENTORY_AND_EQUIP_SLOT_MAX)
#endif
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		p->ItemPos.cell = ch->GetEmptyInventory(pkItem->GetSize());
#else
		p->ItemPos.cell = ch->GetEmptyInventory(pkItem->GetSize(), pkItem->GetVirtualWindow());
#endif

#ifndef __SPECIAL_STORAGE_ENABLE__
		if (p->ItemPos.cell < 0 || p->ItemPos.cell >= INVENTORY_MAX_NUM)
#else
		if (p->ItemPos.cell < 0 || (p->ItemPos.cell >= INVENTORY_MAX_NUM && p->ItemPos.cell < SPECIAL_STORAGE_START_CELL) || p->ItemPos.cell >= INVENTORY_AND_EQUIP_SLOT_MAX)
#endif
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
			return;
		}
	}

	if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
	{
		return;
	}
#endif

	if (!ch->IsEmptyItemGrid(p->ItemPos, pkItem->GetSize()))
	{
		return;
	}

	if (pkItem->IsDragonSoul())
	{
		if (bMall)
		{
			DSManager::instance().DragonSoulItemInitialize(pkItem);
		}

		if (DRAGON_SOUL_INVENTORY != p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] No movement possible."));
			return;
		}

		TItemPos DestPos = p->ItemPos;
		if (!DSManager::instance().IsValidCellForThisItem(pkItem, DestPos))
		{
			int iCell = ch->GetEmptyDragonSoulInventory(pkItem);
			if (iCell < 0)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] No movement possible."));
				return ;
			}
			DestPos = TItemPos (DRAGON_SOUL_INVENTORY, iCell);
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, DestPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}
	else
	{
		if (DRAGON_SOUL_INVENTORY == p->ItemPos.window_type)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Storeroom] No movement possible."));
			return;
		}
		// @fixme119
		if (p->ItemPos.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(pkItem))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this item in your belt inventory."));
			return;
		}

		pkSafebox->Remove(p->bSafePos);
		pkItem->AddToCharacter(ch, p->ItemPos);
		ITEM_MANAGER::instance().FlushDelayedSave(pkItem);
	}

	DWORD dwID = pkItem->GetID();
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEM_FLUSH, 0, sizeof(DWORD));
	db_clientdesc->Packet(&dwID, sizeof(DWORD));

	char szHint[128];
	snprintf(szHint, sizeof(szHint), "%s %u", pkItem->GetName(), pkItem->GetCount());
	if (bMall)
	{
		LogManager::instance().ItemLog(ch, pkItem, "MALL GET", szHint);
	}
	else
	{
		LogManager::instance().ItemLog(ch, pkItem, "SAFEBOX GET", szHint);
	}
}

void CInputMain::SafeboxItemMove(LPCHARACTER ch, const char * data)
{
	struct command_item_move * pinfo = (struct command_item_move *) data;

	if (!ch->CanHandleItem())
	{
		return;
	}

	if (!ch->GetSafebox())
	{
		return;
	}

	ch->GetSafebox()->MoveItem(pinfo->Cell.cell, pinfo->CellTo.cell, pinfo->count);
}

// PARTY_JOIN_BUG_FIX
void CInputMain::PartyInvite(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

#ifdef __ACTION_RESTRICTIONS__
	if (ch->GetActionRestrictions())
	{
		if (ch->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_PARTY_ADD) == false) { return; }
	}
#endif

	TPacketCGPartyInvite * p = (TPacketCGPartyInvite*) c_pData;

	LPCHARACTER pInvitee = CHARACTER_MANAGER::instance().Find(p->vid);

	if (!pInvitee || !ch->GetDesc() || !pInvitee->GetDesc())
	{
		sys_err("PARTY Cannot find invited character");
		return;
	}

	ch->PartyInvite(pInvitee);
}

void CInputMain::PartyInviteAnswer(LPCHARACTER ch, const char * c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	TPacketCGPartyInviteAnswer * p = (TPacketCGPartyInviteAnswer*) c_pData;

	LPCHARACTER pInviter = CHARACTER_MANAGER::instance().Find(p->leader_vid);



	if (!pInviter)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The player who invited you is not online."));
	}
	else if (!p->accept)
	{
		pInviter->PartyInviteDeny(ch->GetPlayerID());
	}
	else
	{
		pInviter->PartyInviteAccept(ch);
	}
}
// END_OF_PARTY_JOIN_BUG_FIX

void CInputMain::PartySetState(LPCHARACTER ch, const char* c_pData)
{
	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;
	}

	TPacketCGPartySetState* p = (TPacketCGPartySetState*) c_pData;

	if (!ch->GetParty())
	{
		return;
	}

	if (ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only the group leader can change this."));
		return;
	}

	if (!ch->GetParty()->IsMember(p->pid))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The target is not a member of your group."));
		return;
	}

	DWORD pid = p->pid;
	sys_log(0, "PARTY SetRole pid %d to role %d state %s", pid, p->byRole, p->flag ? "on" : "off");

	switch (p->byRole)
	{
	case PARTY_ROLE_NORMAL:
		break;

	case PARTY_ROLE_ATTACKER:
	case PARTY_ROLE_TANKER:
	case PARTY_ROLE_BUFFER:
	case PARTY_ROLE_SKILL_MASTER:
	case PARTY_ROLE_HASTE:
	case PARTY_ROLE_DEFENDER:
		if (ch->GetParty()->SetRole(pid, p->byRole, p->flag))
		{
			TPacketPartyStateChange pack;
			pack.dwLeaderPID = ch->GetPlayerID();
			pack.dwPID = p->pid;
			pack.bRole = p->byRole;
			pack.bFlag = p->flag;
			db_clientdesc->DBPacket(HEADER_GD_PARTY_STATE_CHANGE, 0, &pack, sizeof(pack));
		}

		break;

	default:
		sys_err("wrong byRole in PartySetState Packet name %s state %d", ch->GetName(), p->byRole);
		break;
	}
}

void CInputMain::PartyRemove(LPCHARACTER ch, const char* c_pData)
{
	if (ch->GetArena())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
		return;
	}

	if (!CPartyManager::instance().IsEnablePCParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The server cannot execute this group request."));
		return;
	}

	if (ch->GetDungeon())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot kick out a player while you are in a dungeon."));
		return;
	}

	TPacketCGPartyRemove* p = (TPacketCGPartyRemove*) c_pData;

	if (!ch->GetParty())
	{
		return;
	}

	LPPARTY pParty = ch->GetParty();
	if (pParty->GetLeaderPID() == ch->GetPlayerID())
	{
		if (ch->GetDungeon())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot kick out a player while you are in a dungeon."));
		}
		else
		{

			if (pParty->IsPartyInDungeon(351))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<파티>던전 안에 파티원이 있어 파티를 해산 할 수 없습니다."));
				return;
			}

			// leader can remove any member
			if (p->pid == ch->GetPlayerID() || pParty->GetMemberCount() == 2)
			{
				// party disband
				CPartyManager::instance().DeleteParty(pParty);
			}
			else
			{
				LPCHARACTER B = CHARACTER_MANAGER::instance().FindByPID(p->pid);
				if (B)
				{
					//pParty->SendPartyRemoveOneToAll(B);
					B->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You have been out kicked of the group."));
					//pParty->Unlink(B);
					//CPartyManager::instance().SetPartyMember(B->GetPlayerID(), NULL);
				}
				pParty->Quit(p->pid);
			}
		}
	}
	else
	{
		// otherwise, only remove itself
		if (p->pid == ch->GetPlayerID())
		{
			if (ch->GetDungeon())
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot leave a group while you are in a dungeon."));
			}
			else
			{
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
					//CPartyManager::instance().SetPartyMember(ch->GetPlayerID(), NULL);
				}
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] You cannot kick out group members."));
		}
	}
}

void CInputMain::AnswerMakeGuild(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGAnswerMakeGuild* p = (TPacketCGAnswerMakeGuild*) c_pData;

	if (ch->GetGold() < 200000)
	{
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_disband_time") <
			CGuildManager::instance().GetDisbandDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] After disbanding a guild, you cannot create a new one for %d days."),
					   quest::CQuestManager::instance().GetEventFlag("guild_disband_delay"));
		return;
	}

	if (get_global_time() - ch->GetQuestFlag("guild_manage.new_withdraw_time") <
			CGuildManager::instance().GetWithdrawDelay())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] After leaving a guild, you cannot create a new one for %d days."),
					   quest::CQuestManager::instance().GetEventFlag("guild_withdraw_delay"));
		return;
	}

	if (ch->GetGuild())
	{
		return;
	}

	CGuildManager& gm = CGuildManager::instance();

	TGuildCreateParameter cp;
	memset(&cp, 0, sizeof(cp));

	cp.master = ch;
	strlcpy(cp.name, p->guild_name, sizeof(cp.name));

	if (cp.name[0] == 0 || !check_name(cp.name))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This guild name is invalid."));
		return;
	}

	DWORD dwGuildID = gm.CreateGuild(cp);

	if (dwGuildID)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] [%s] guild has been created."), cp.name);

		int GuildCreateFee = 200000;

		ch->PointChange(POINT_GOLD, -GuildCreateFee);
		DBManager::instance().SendMoneyLog(MONEY_LOG_GUILD, ch->GetPlayerID(), -GuildCreateFee);

		char Log[128];
		snprintf(Log, sizeof(Log), "GUILD_NAME %s MASTER %s", cp.name, ch->GetName());
		LogManager::instance().CharLog(ch, 0, "MAKE_GUILD", Log);

		ch->RemoveSpecifyItem(GUILD_CREATE_ITEM_VNUM, 1);
		//ch->SendGuildName(dwGuildID);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Creation of the guild has failed."));
	}
}

void CInputMain::PartyUseSkill(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGPartyUseSkill* p = (TPacketCGPartyUseSkill*) c_pData;
	if (!ch->GetParty())
	{
		return;
	}

	if (ch->GetPlayerID() != ch->GetParty()->GetLeaderPID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] Only the group leader can use group skills."));
		return;
	}

	switch (p->bySkillIndex)
	{
	case PARTY_SKILL_HEAL:
		ch->GetParty()->HealParty();
		break;
	case PARTY_SKILL_WARP:
	{
		LPCHARACTER pch = CHARACTER_MANAGER::instance().Find(p->vid);
		if (pch)
		{
			ch->GetParty()->SummonToLeader(pch->GetPlayerID());
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Group] The target has not been found."));
		}
	}
	break;
	}
}

void CInputMain::PartyParameter(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGPartyParameter * p = (TPacketCGPartyParameter *) c_pData;

	if (ch->GetParty())
	{
		ch->GetParty()->SetParameter(p->bDistributeMode);
	}
}

#ifdef __INGAME_WIKI__
void CInputMain::RecvWikiPacket(LPCHARACTER ch, const char* c_pData)
{
	if (!ch || (ch && !ch->GetDesc()))
	{
		return;
	}

	if (!c_pData)
	{
		return;
	}

	InGameWiki::TCGWikiPacket* p = nullptr;
	if (!(p = (InGameWiki::TCGWikiPacket*)c_pData))
	{
		return;
	}

	InGameWiki::TGCWikiPacket pack;
	pack.set_data_type(!p->is_mob ? InGameWiki::LOAD_WIKI_ITEM : InGameWiki::LOAD_WIKI_MOB);
	pack.increment_data_size(WORD(sizeof(InGameWiki::TGCWikiPacket)));

	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const std::vector<CommonWikiData::TWikiItemOriginInfo>& originVec = ITEM_MANAGER::Instance().GetItemOrigin(p->vnum);
		const std::vector<CSpecialItemGroup::CSpecialItemInfo> _gV = ITEM_MANAGER::instance().GetWikiChestInfo(p->vnum);
		const std::vector<CommonWikiData::TWikiRefineInfo> _rV = ITEM_MANAGER::instance().GetWikiRefineInfo(p->vnum);
		const CommonWikiData::TWikiInfoTable* _wif = ITEM_MANAGER::instance().GetItemWikiInfo(p->vnum);

		if (!_wif)
		{
			return;
		}

		const size_t origin_size = originVec.size();
		const size_t chest_info_count = _wif->chest_info_count;
		const size_t refine_infos_count = _wif->refine_infos_count;
		const size_t buf_data_dize = sizeof(InGameWiki::TGCItemWikiPacket) +
									 (origin_size * sizeof(CommonWikiData::TWikiItemOriginInfo)) +
									 (chest_info_count * sizeof(CommonWikiData::TWikiChestInfo)) +
									 (refine_infos_count * sizeof(CommonWikiData::TWikiRefineInfo));

		if (chest_info_count != _gV.size())
		{
			sys_err("Item Vnum : %d || ERROR TYPE -> 1", p->vnum);
			return;
		}

		if (refine_infos_count != _rV.size())
		{
			sys_err("Item Vnum : %d || ERROR TYPE -> 2", p->vnum);
			return;
		}

		pack.increment_data_size(WORD(buf_data_dize));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));

		InGameWiki::TGCItemWikiPacket data_packet;
		data_packet.mutable_wiki_info(*_wif);
		data_packet.set_origin_infos_count(origin_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(data_packet));

		{
			if (origin_size)
				for (int idx = 0; idx < (int)origin_size; ++idx)
				{
					buf.write(&(originVec[idx]), sizeof(CommonWikiData::TWikiItemOriginInfo));
				}

			if (chest_info_count > 0)
			{
				for (int idx = 0; idx < (int)chest_info_count; ++idx)
				{
					CommonWikiData::TWikiChestInfo write_struct(_gV[idx].vnum, _gV[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiChestInfo));
				}
			}

			if (refine_infos_count > 0)
				for (int idx = 0; idx < (int)refine_infos_count; ++idx)
				{
					buf.write(&(_rV[idx]), sizeof(CommonWikiData::TWikiRefineInfo));
				}
		}

		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		CMobManager::TMobWikiInfoVector& mobVec = CMobManager::instance().GetMobWikiInfo(p->vnum);
		const size_t _mobVec_size = mobVec.size();

		if (!_mobVec_size)
		{
			if (test_server)
			{
				sys_log(0, "Mob Vnum: %d : || LOG TYPE -> 1", p->vnum);
			}
			return;
		}

		const size_t buf_data_dize = (sizeof(InGameWiki::TGCMobWikiPacket) + (_mobVec_size * sizeof(CommonWikiData::TWikiMobDropInfo)));
		pack.increment_data_size(WORD(buf_data_dize));

		TEMP_BUFFER buf;
		buf.write(&pack, sizeof(InGameWiki::TGCWikiPacket));

		InGameWiki::TGCMobWikiPacket data_packet;
		data_packet.set_drop_info_count(_mobVec_size);
		data_packet.set_vnum(p->vnum);
		data_packet.set_ret_id(p->ret_id);
		buf.write(&data_packet, sizeof(InGameWiki::TGCMobWikiPacket));

		{
			if (_mobVec_size)
			{
				for (int idx = 0; idx < (int)_mobVec_size; ++idx)
				{
					CommonWikiData::TWikiMobDropInfo write_struct(mobVec[idx].vnum, mobVec[idx].count);
					buf.write(&write_struct, sizeof(CommonWikiData::TWikiMobDropInfo));
				}
			}
		}

		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
}
#endif

size_t GetSubPacketSize(const GUILD_SUBHEADER_CG& header)
{
	switch (header)
	{
	case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
		return sizeof(int);
	case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
		return sizeof(int);
	case GUILD_SUBHEADER_CG_ADD_MEMBER:
		return sizeof(DWORD);
	case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
		return sizeof(DWORD);
	case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
		return 10;
	case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
		return sizeof(BYTE) + sizeof(BYTE);
	case GUILD_SUBHEADER_CG_OFFER:
		return sizeof(DWORD);
	case GUILD_SUBHEADER_CG_CHARGE_GSP:
		return sizeof(int);
	case GUILD_SUBHEADER_CG_POST_COMMENT:
		return 1;
	case GUILD_SUBHEADER_CG_DELETE_COMMENT:
		return sizeof(DWORD);
	case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
		return 0;
	case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
		return sizeof(DWORD) + sizeof(BYTE);
	case GUILD_SUBHEADER_CG_USE_SKILL:
		return sizeof(TPacketCGGuildUseSkill);
	case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
		return sizeof(DWORD) + sizeof(BYTE);
	case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
		return sizeof(DWORD) + sizeof(BYTE);
	}

	return 0;
}

int CInputMain::Guild(LPCHARACTER ch, const char * data, size_t uiBytes)
{
	if (uiBytes < sizeof(TPacketCGGuild))
	{
		return -1;
	}

	const TPacketCGGuild* p = reinterpret_cast<const TPacketCGGuild*>(data);
	const char* c_pData = data + sizeof(TPacketCGGuild);

	uiBytes -= sizeof(TPacketCGGuild);

	const GUILD_SUBHEADER_CG SubHeader = static_cast<GUILD_SUBHEADER_CG>(p->subheader);
	const size_t SubPacketLen = GetSubPacketSize(SubHeader);

	if (uiBytes < SubPacketLen)
	{
		return -1;
	}

	CGuild* pGuild = ch->GetGuild();

	if (NULL == pGuild)
	{
		if (SubHeader != GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] It does not belong to the guild."));
			return SubPacketLen;
		}
	}

	switch (SubHeader)
	{
	case GUILD_SUBHEADER_CG_DEPOSIT_MONEY:
	{
		return SubPacketLen;

		const int gold = MIN(*reinterpret_cast<const int*>(c_pData), __deposit_limit());

		if (gold < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] That is not the correct amount of Yang."));
			return SubPacketLen;
		}

		if (ch->GetGold() < gold)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have enough Yang."));
			return SubPacketLen;
		}

		pGuild->RequestDepositMoney(ch, gold);
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_WITHDRAW_MONEY:
	{

		return SubPacketLen;

		const int gold = MIN(*reinterpret_cast<const int*>(c_pData), 500000);

		if (gold < 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] That is not the correct amount of Yang."));
			return SubPacketLen;
		}

		pGuild->RequestWithdrawMoney(ch, gold);
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_ADD_MEMBER:
	{
#ifdef __ACTION_RESTRICTIONS__
		if (ch->GetActionRestrictions())
		{
			if (ch->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_GUILD_ADD) == false) { return SubPacketLen; }
		}
#endif

		const DWORD vid = *reinterpret_cast<const DWORD*>(c_pData);
		LPCHARACTER newmember = CHARACTER_MANAGER::instance().Find(vid);

		if (!newmember)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The person you were searching for cannot be found."));
			return SubPacketLen;
		}

		// @fixme145 BEGIN (+newmember ispc check)
		if (!ch->IsPC() || !newmember->IsPC())
		{
			return SubPacketLen;
		}
		// @fixme145 END

		pGuild->Invite(ch, newmember);
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_REMOVE_MEMBER:
	{
		if (pGuild->UnderAnyWar() != 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드전 중에는 길드원을 탈퇴시킬 수 없습니다."));
			return SubPacketLen;
		}

		const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
		const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		LPCHARACTER member = CHARACTER_MANAGER::instance().FindByPID(pid);

		if (member)
		{
			if (member->GetGuild() != pGuild)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This person is not in the same guild."));
				return SubPacketLen;
			}

			if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to kick out guild members."));
				return SubPacketLen;
			}

			member->SetQuestFlag("guild_manage.new_withdraw_time", get_global_time());
			pGuild->RequestRemoveMember(member->GetPlayerID());

			if (g_bGuildInviteLimit)
			{
				DBManager::instance().Query("REPLACE INTO guild_invite_limit VALUES(%d, %d)", pGuild->GetID(), get_global_time());
			}
		}
		else
		{
			if (!pGuild->HasGradeAuth(m->grade, GUILD_AUTH_REMOVE_MEMBER))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to kick out guild members."));
				return SubPacketLen;
			}

			if (pGuild->RequestRemoveMember(pid))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You have kicked a guild member out."));
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The person you were searching for cannot be found."));
			}
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME:
	{
		char gradename[GUILD_GRADE_NAME_MAX_LEN + 1];
		strlcpy(gradename, c_pData + 1, sizeof(gradename));

		const TGuildMember * m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		if (m->grade != GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to change your rank name."));
		}
		else if (*c_pData == GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild leader's rights cannot be changed."));
		}
		else if (!check_name(gradename))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] This rank name is invalid."));
		}
		else
		{
			pGuild->ChangeGradeName(*c_pData, gradename);
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY:
	{
		const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		if (m->grade != GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to change your position."));
		}
		else if (*c_pData == GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The rights of the guild leader cannot be changed."));
		}
		else
		{
			pGuild->ChangeGradeAuth(*c_pData, *(c_pData + 1));
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_OFFER:
	{
		DWORD offer = *reinterpret_cast<const DWORD*>(c_pData);

		if (pGuild->GetLevel() >= GUILD_MAX_LEVEL)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("<길드> 길드가 이미 최고 레벨입니다."));
		}
		else
		{
			offer /= 100;
			offer *= 100;

			if (pGuild->OfferExp(ch, offer))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] %u experience points used."), offer);
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Experience usage has failed."));
			}
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_CHARGE_GSP:
	{
		const int offer = *reinterpret_cast<const int*>(c_pData);
		const int gold = offer * 100;

		if (offer < 0 || gold < offer || gold < 0 || ch->GetGold() < gold)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Insufficient Yang in the guild treasury."));
			return SubPacketLen;
		}

		if (!pGuild->ChargeSP(ch, offer))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] Dragon ghost was not restored."));
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_POST_COMMENT:
	{
		const size_t length = *c_pData;

		if (length > GUILD_COMMENT_MAX_LEN)
		{

			sys_err("POST_COMMENT: %s comment too long (length: %u)", ch->GetName(), length);
			ch->GetDesc()->SetPhase(PHASE_CLOSE);
			return -1;
		}

		if (uiBytes < 1 + length)
		{
			return -1;
		}

		const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		if (length && !pGuild->HasGradeAuth(m->grade, GUILD_AUTH_NOTICE) && *(c_pData + 1) == '!')
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to make an announcement."));
		}
		else
		{
			std::string str(c_pData + 1, length);
			pGuild->AddComment(ch, str);
		}

		return (1 + length);
	}

	case GUILD_SUBHEADER_CG_DELETE_COMMENT:
	{
		const DWORD comment_id = *reinterpret_cast<const DWORD*>(c_pData);

		pGuild->DeleteComment(ch, comment_id);
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_REFRESH_COMMENT:
		pGuild->RefreshComment(ch);
		return SubPacketLen;

	case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE:
	{
		const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
		const BYTE grade = *(c_pData + sizeof(DWORD));
		const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		if (m->grade != GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to change the position."));
		}
		else if (ch->GetPlayerID() == pid)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] The guild leader's position cannot be changed."));
		}
		else if (grade == 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You cannot make yourself guild leader."));
		}
		else
		{
			pGuild->ChangeMemberGrade(pid, grade);
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_USE_SKILL:
	{
		const TPacketCGGuildUseSkill* p = reinterpret_cast<const TPacketCGGuildUseSkill*>(c_pData);

		pGuild->UseSkill(p->dwVnum, ch, p->dwPID);
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL:
	{
		const DWORD pid = *reinterpret_cast<const DWORD*>(c_pData);
		const BYTE is_general = *(c_pData + sizeof(DWORD));
		const TGuildMember* m = pGuild->GetMember(ch->GetPlayerID());

		if (NULL == m)
		{
			return -1;
		}

		if (m->grade != GUILD_LEADER_GRADE)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You do not have the authority to choose the guild leader."));
		}
		else
		{
			if (!pGuild->ChangeMemberGeneral(pid, is_general))
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Guild] You cannot choose any more guild leaders."));
			}
		}
	}
	return SubPacketLen;

	case GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER:
	{
		const DWORD guild_id = *reinterpret_cast<const DWORD*>(c_pData);
		const BYTE accept = *(c_pData + sizeof(DWORD));

		CGuild * g = CGuildManager::instance().FindGuild(guild_id);

		if (g)
		{
			if (accept)
			{
				g->InviteAccept(ch);
			}
			else
			{
				g->InviteDeny(ch->GetPlayerID());
			}
		}
	}
	return SubPacketLen;

	}

	return 0;
}

void CInputMain::Fishing(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGFishing* p = (TPacketCGFishing*)c_pData;
	ch->SetRotation(p->dir * 5);
	ch->fishing();
	return;
}

void CInputMain::ItemGive(LPCHARACTER ch, const char* c_pData)
{
	TPacketCGGiveItem* p = (TPacketCGGiveItem*) c_pData;
	LPCHARACTER to_ch = CHARACTER_MANAGER::instance().Find(p->dwTargetVID);

	if (to_ch)
	{
		ch->GiveItem(to_ch, p->ItemPos);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade this item."));
	}
}

void CInputMain::Hack(LPCHARACTER ch, const char * c_pData)
{
	TPacketCGHack * p = (TPacketCGHack *) c_pData;

	char buf[sizeof(p->szBuf)];
	strlcpy(buf, p->szBuf, sizeof(buf));

	sys_err("HACK_DETECT: %s %s", ch->GetName(), buf);


	ch->GetDesc()->SetPhase(PHASE_CLOSE);
}

int CInputMain::MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes)
{
	TPacketCGMyShop * p = (TPacketCGMyShop *) c_pData;
	int iExtraLen = p->bCount * sizeof(TShopItemTable);

	if (uiBytes < sizeof(TPacketCGMyShop) + iExtraLen)
	{
		return -1;
	}

	if (ch->GetGold() >= GOLD_MAX)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have more than 2 Billion Yang with you. You cannot trade."));
		sys_log(0, "MyShop ==> OverFlow Gold id %u name %s ", ch->GetPlayerID(), ch->GetName());
		return (iExtraLen);
	}

	if (ch->IsStun() || ch->IsDead())
	{
		return (iExtraLen);
	}

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a private shop while another window is open."));
		return (iExtraLen);
	}

	sys_log(0, "MyShop count %d", p->bCount);
	ch->OpenMyShop(p->szSign, (TShopItemTable *) (c_pData + sizeof(TPacketCGMyShop)), p->bCount);
	return (iExtraLen);
}

void CInputMain::Refine(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefine* p = reinterpret_cast<const TPacketCGRefine*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->GetShopOwner() || ch->GetMyShop() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO,  LC_TEXT("You cannot upgrade anything while another window is open."));
		ch->ClearRefineMode();
		return;
	}

	if (p->type == 255)
	{
		// DoRefine Cancel
		ch->ClearRefineMode();
		return;
	}

	if (p->pos >= INVENTORY_MAX_NUM)
	{
#ifdef __SPECIAL_STORAGE_ENABLE__
		if ((p->pos >= INVENTORY_MAX_NUM && p->pos < SPECIAL_STORAGE_START_CELL) || p->pos >= INVENTORY_AND_EQUIP_SLOT_MAX)
#endif
		{
			ch->ClearRefineMode();
			return;
		}
	}

	LPITEM item = ch->GetInventoryItem(p->pos);

	if (!item)
	{
		ch->ClearRefineMode();
		return;
	}

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());
#ifdef __ENABLE_SOUL_SYSTEM__
	if (!prt && p->type != REFINE_TYPE_SOUL)
#else
	if (!prt)
#endif
	{
		ch->ClearRefineMode();
		return;
	}

	ch->SetRefineTime();

	bool canFastRefine = p->fast_refine && item->GetRefinedVnum();
	if (p->type == REFINE_TYPE_NORMAL)
	{
#ifdef __ENABLE_FAST_REFINE_OPTION__
		ch->DoRefine(item, false, canFastRefine);
#else
		ch->DoRefine(item);
#endif
	}
	else if (p->type == REFINE_TYPE_SCROLL || p->type == REFINE_TYPE_HYUNIRON || p->type == REFINE_TYPE_MUSIN || p->type == REFINE_TYPE_BDRAGON)
	{
#ifdef __ENABLE_FAST_REFINE_OPTION__
		ch->DoRefineWithScroll(item, canFastRefine);
#else
		ch->DoRefineWithScroll(item);
#endif
	}
#ifdef __ENABLE_SOUL_SYSTEM__
	else if (p->type == REFINE_TYPE_SOUL)
	{
		ch->DoRefineItemSoul(item);
	}
#endif
	else if (p->type == REFINE_TYPE_MONEY_ONLY)
	{
		const LPITEM item = ch->GetInventoryItem(p->pos);

		if (NULL != item)
		{
			if (3000 <= item->GetRefineSet())
			{
				LogManager::instance().HackLog("DEVIL_TOWER_REFINE_HACK", ch);
			}
			else
			{
#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
				LPCHARACTER refNPC = ch->GetRefineNPC();
				if (!refNPC)
				{
					LogManager::instance().HackLog("DT_DIRECT_REFINE_HACK", ch);
				}
				else if (!refNPC->CanReceiveItem(ch, item, true))
				{
					LogManager::instance().HackLog("DT_TYPE_REFINE_HACK", ch);
					return;
				}
#endif
				if (ch->GetQuestFlag("demon_tower.can_refine") > 0) // @fixme158 (allow multiple refine attempts)
				{
					if (ch->DoRefine(item, true, canFastRefine))
					{
						ch->SetQuestFlag("demon_tower.can_refine", ch->GetQuestFlag("demon_tower.can_refine") - 1);
					}
				}
				else
				{
					ch->ChatPacket(CHAT_TYPE_INFO, "You can only be rewarded once for the Demon Tower Quest.");
				}
			}
		}
	}

#ifdef __ENABLE_FAST_REFINE_OPTION__
	if (!(p->fast_refine && item->GetRefinedVnum()))
	{
		ch->ClearRefineMode();
	}
#else
	ch->ClearRefineMode();
#endif
}

#ifdef __ADMIN_MANAGER__
int CInputMain::AdminManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	TPacketCGAdminManager* p = (TPacketCGAdminManager*)c_pData;
	c_pData += sizeof(TPacketCGAdminManager);

	sys_err("Call AdminManagerClientPacket %u", p->sub_header);
	return CAdminManager::instance().RecvClientPacket(ch, p->sub_header, c_pData, uiBytes - sizeof(TPacketCGAdminManager));
}
#endif

int CInputMain::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		d->SetPhase(PHASE_CLOSE);
		return (0);
	}

	int iExtraLen = 0;

	if (test_server && bHeader != HEADER_CG_MOVE)
	{
		sys_log(0, "CInputMain::Analyze() ==> Header [%d] ", bHeader);
	}

	switch (bHeader)
	{
	case HEADER_CG_PONG:
		Pong(d);
		break;

	case HEADER_CG_TIME_SYNC:
		Handshake(d, c_pData);
		break;

	case HEADER_CG_CHAT:
		if (test_server)
		{
			char* pBuf = (char*)c_pData;
			sys_log(0, "%s", pBuf + sizeof(TPacketCGChat));
		}

		if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_WHISPER:
		if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_MOVE:
		Move(ch, c_pData);
		// @fixme103 (removed CheckClientVersion since useless in here)
		break;

	case HEADER_CG_CHARACTER_POSITION:
		Position(ch, c_pData);
		break;

	case HEADER_CG_ITEM_USE:
		if (!ch->IsObserverMode())
		{
			ItemUse(ch, c_pData);
		}
		break;

#ifdef __ENABLE_MULTI_USE_PACKET__
	case HEADER_CG_ITEM_MULTI_USE:
		if (!ch->IsObserverMode())
		{
			ItemMultiUse(ch, c_pData);
		}
		break;
#endif

	case HEADER_CG_ITEM_DROP:
		if (!ch->IsObserverMode())
		{
			ItemDrop(ch, c_pData);
		}
		break;

	case HEADER_CG_ITEM_DROP2:
		if (!ch->IsObserverMode())
		{
			ItemDrop2(ch, c_pData);
		}
		break;

	case HEADER_CG_ITEM_MOVE:
		if (!ch->IsObserverMode())
		{
			ItemMove(ch, c_pData);
		}
		break;

	case HEADER_CG_ITEM_PICKUP:
		if (!ch->IsObserverMode())
		{
			ItemPickup(ch, c_pData);
		}
		break;

#ifdef __ENABLE_DESTROY_ITEM_PACKET__
	case HEADER_CG_ITEM_DESTROY:
		if (!ch->IsObserverMode())
		{
			ItemDestroy(ch, c_pData);
		}
		break;
#endif


#ifdef __ENABLE_NEW_FILTERS__
	case HEADER_CG_ITEMS_PICKUP:
		if (!ch->IsObserverMode())
		{
			ItemsPickup(ch, c_pData);
		}
		break;
#endif

	case HEADER_CG_ITEM_USE_TO_ITEM:
		if (!ch->IsObserverMode())
		{
			ItemToItem(ch, c_pData);
		}
		break;

	case HEADER_CG_ITEM_GIVE:
		if (!ch->IsObserverMode())
		{
			ItemGive(ch, c_pData);
		}
		break;

	case HEADER_CG_EXCHANGE:
		if (!ch->IsObserverMode())
		{
			Exchange(ch, c_pData);
		}
		break;

	case HEADER_CG_ATTACK:
	case HEADER_CG_SHOOT:
		if (!ch->IsObserverMode())
		{
			Attack(ch, bHeader, c_pData);
		}
		break;

	case HEADER_CG_USE_SKILL:
		if (!ch->IsObserverMode())
		{
			UseSkill(ch, c_pData);
		}
		break;

	case HEADER_CG_QUICKSLOT_ADD:
		QuickslotAdd(ch, c_pData);
		break;

	case HEADER_CG_QUICKSLOT_DEL:
		QuickslotDelete(ch, c_pData);
		break;

	case HEADER_CG_QUICKSLOT_SWAP:
		QuickslotSwap(ch, c_pData);
		break;

	case HEADER_CG_SHOP:
		if ((iExtraLen = Shop(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_MESSENGER:
		if ((iExtraLen = Messenger(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_ON_CLICK:
		OnClick(ch, c_pData);
		break;

	case HEADER_CG_SYNC_POSITION:
		if ((iExtraLen = SyncPosition(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_ADD_FLY_TARGETING:
	case HEADER_CG_FLY_TARGETING:
		FlyTarget(ch, c_pData, bHeader);
		break;

	case HEADER_CG_SCRIPT_BUTTON:
		ScriptButton(ch, c_pData);
		break;

	// SCRIPT_SELECT_ITEM
	case HEADER_CG_SCRIPT_SELECT_ITEM:
		ScriptSelectItem(ch, c_pData);
		break;
	// END_OF_SCRIPT_SELECT_ITEM

	case HEADER_CG_SCRIPT_ANSWER:
		ScriptAnswer(ch, c_pData);
		break;

	case HEADER_CG_QUEST_INPUT_STRING:
		QuestInputString(ch, c_pData);
		break;

	case HEADER_CG_QUEST_CONFIRM:
		QuestConfirm(ch, c_pData);
		break;

	case HEADER_CG_TARGET:
		Target(ch, c_pData);
		break;

	case HEADER_CG_WARP:
		Warp(ch, c_pData);
		break;

	case HEADER_CG_SAFEBOX_CHECKIN:
		SafeboxCheckin(ch, c_pData);
		break;

	case HEADER_CG_SAFEBOX_CHECKOUT:
		SafeboxCheckout(ch, c_pData, false);
		break;

	case HEADER_CG_SAFEBOX_ITEM_MOVE:
		SafeboxItemMove(ch, c_pData);
		break;

	case HEADER_CG_MALL_CHECKOUT:
		SafeboxCheckout(ch, c_pData, true);
		break;

	case HEADER_CG_PARTY_INVITE:
		PartyInvite(ch, c_pData);
		break;

	case HEADER_CG_PARTY_REMOVE:
		PartyRemove(ch, c_pData);
		break;

	case HEADER_CG_PARTY_INVITE_ANSWER:
		PartyInviteAnswer(ch, c_pData);
		break;

	case HEADER_CG_PARTY_SET_STATE:
		PartySetState(ch, c_pData);
		break;

	case HEADER_CG_PARTY_USE_SKILL:
		PartyUseSkill(ch, c_pData);
		break;

	case HEADER_CG_PARTY_PARAMETER:
		PartyParameter(ch, c_pData);
		break;

	case HEADER_CG_ANSWER_MAKE_GUILD:
#ifdef ENABLE_NEWGUILDMAKE
		ch->ChatPacket(CHAT_TYPE_INFO, "<%s> AnswerMakeGuild disabled", __FUNCTION__);
#else
		AnswerMakeGuild(ch, c_pData);
#endif
		break;

	case HEADER_CG_GUILD:
		if ((iExtraLen = Guild(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_FISHING:
		Fishing(ch, c_pData);
		break;

	case HEADER_CG_HACK:
		Hack(ch, c_pData);
		break;

	case HEADER_CG_MYSHOP:
		if ((iExtraLen = MyShop(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_REFINE:
		Refine(ch, c_pData);
		break;

	case HEADER_CG_CLIENT_VERSION:
		Version(ch, c_pData);
		break;

	case HEADER_CG_HS_ACK:
		if (isHackShieldEnable)
		{
			CHackShieldManager::instance().VerifyAck(d->GetCharacter(), c_pData);
		}
		break;

	case HEADER_CG_XTRAP_ACK:
	{
		TPacketXTrapCSVerify* p = reinterpret_cast<TPacketXTrapCSVerify*>((void*)c_pData);
		CXTrapManager::instance().Verify_CSStep3(d->GetCharacter(), p->bPacketData);
	}
	break;
	case HEADER_CG_DRAGON_SOUL_REFINE:
	{
		TPacketCGDragonSoulRefine* p = reinterpret_cast <TPacketCGDragonSoulRefine*>((void*)c_pData);
		switch (p->bSubType)
		{
		case DS_SUB_HEADER_CLOSE:
			ch->DragonSoul_RefineWindow_Close();
			break;
		case DS_SUB_HEADER_DO_REFINE_GRADE:
		{
			DSManager::instance().DoRefineGrade(ch, p->ItemGrid);
		}
		break;
#ifdef __ENABLE_DS_REFINE_ALL__
		case DS_SUB_HEADER_DO_REFINE_GRADE_ALL:
		{
			DSManager::instance().DoAllRefineGrade(ch, p->ItemGrid[0].cell, p->ItemGrid[1].cell);
		}
		break;
#endif
		case DS_SUB_HEADER_DO_REFINE_STEP:
		{
			DSManager::instance().DoRefineStep(ch, p->ItemGrid);
		}
		break;
		case DS_SUB_HEADER_DO_REFINE_STRENGTH:
		{
			DSManager::instance().DoRefineStrength(ch, p->ItemGrid);
		}
		break;
		}
	}
	break;

#ifdef __ENABLE_TREASURE_BOX_LOOT__
	case HEADER_CG_REQUEST_TREASURE_BOX_LOOT:
	{
		if ((iExtraLen = RequestTreasureBoxLoot(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
	}
	break;
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
	case HEADER_CG_TARGET_LOAD:
		LoadTargetInfo(ch, (TPacketCGTargetLoad*)c_pData);
		break;
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	case HEADER_CG_REQUEST_DELETE_SOCKET:
	{
		if ((iExtraLen = RequestDeleteSocket(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
	}
	break;
#endif

#ifdef __ENABLE_SWITCHBOT__
	case HEADER_CG_SWITCHBOT:
		if ((iExtraLen = Switchbot(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __VOICE_CHAT_ENABLE__
	case HEADER_CG_VOICE_CHAT:
		if ((iExtraLen = ProcessVoicePacket(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __ADMIN_MANAGER__
	case HEADER_CG_ADMIN_MANAGER:
		if ((iExtraLen = AdminManager(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	case HEADER_CG_MARBLE_MANAGER:
		if ((iExtraLen = MarbleManager(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	case HEADER_CG_CUBE_CLOSE:
		CCubeRenewal::instance().CubeClose(ch);
		break;

	case HEADER_CG_CUBE_MAKE:
		if (!ch->IsObserverMode())
		{
			CCubeRenewal::instance().CubeMake(ch, c_pData);
		}
		break;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	case HEADER_CG_REFINE_ELEMENT:
		RefineElement(ch, c_pData);
		break;
#endif

#ifdef __ENABLE_LUCKY_BOX__
	case HEADER_CG_LUCKY_BOX:
		if ((iExtraLen = LuckyBox(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __INGAME_WIKI__
	case InGameWiki::HEADER_CG_WIKI:
		RecvWikiPacket(ch, c_pData);
		break;
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	case HEADER_CG_OFFLINE_SHOP:
	{
		if ((iExtraLen = COfflineShop::ReceivePacket(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}

		break;
	}
#endif

#ifdef __SHOP_SEARCH__
	case HEADER_CG_SHOP_SEARCH_BY_NAME:
		ShopSearchByName(ch, c_pData);
		break;

	case HEADER_CG_SHOP_SEARCH_BY_OPTION:
		if ((iExtraLen = ShopSearchByOptions(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_CG_SHOP_SEARCH_BUY:
		ShopSearchBuy(ch, c_pData);
		break;

	case HEADER_CG_SHOP_SEARCH_OWNER_MESSAGE:
		ShopSearchOwnerMessage(ch, c_pData);
		break;

	case HEADER_CG_SHOP_SEARCH_REQUEST_SOLD_INFO:
		ShopSearchRequestSoldInfo(ch, c_pData);
		break;
#endif

#ifdef __ENABLE_PING_TIME__
	case HEADER_CG_PING_TIMER:
	{
		BYTE packet = HEADER_GC_PING_TIMER;
		ch->GetDesc()->Packet(&packet, sizeof(packet));
	}
	break;
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	case HEADER_CG_WHISPER_REQUEST_LANGUAGE:
		WhisperRequestLanguage(ch, c_pData);
		break;
#endif

	}
	return (iExtraLen);
}

#ifdef __VOICE_CHAT_ENABLE__
int CInputMain::ProcessVoicePacket(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	sys_err("ProcessVoicePacket#1!");
	TPacketCGVoiceChat* pPack = (TPacketCGVoiceChat*)c_pData;
	c_pData += sizeof(TPacketCGVoiceChat);

	unsigned int extraLen = pPack->stSize;
	if (uiBytes < extraLen)
	{
		sys_err("ProcessVoicePacketError");
		return -1;
	}

	TPacketGCVoiceChat retPack{};
	retPack.bHeader = HEADER_GC_VOICE_CHAT;
	strlcpy(retPack.szName, ch->GetName(), sizeof(retPack.szName));
	retPack.stSize = pPack->stSize + sizeof(TPacketGCVoiceChat);
	retPack.stSizeUncp = pPack->stSizeUncp;

	TEMP_BUFFER tmpbuf(sizeof(TPacketGCVoiceChat) + extraLen);
	tmpbuf.write(&retPack, sizeof(TPacketGCVoiceChat));
	tmpbuf.write((BYTE*)c_pData, extraLen);

	// Packet around
	ch->PacketAround(tmpbuf.read_peek(), tmpbuf.size());

	sys_err("ProcessVoicePacket#2!");
	return extraLen;
}
#endif

#ifdef __ENABLE_TREASURE_BOX_LOOT__
int CInputMain::RequestTreasureBoxLoot(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	//const TPacketCGDefault* p = reinterpret_cast<const TPacketCGDefault*>(c_pData);
	if (uiBytes < sizeof(TPacketCGDefault))
	{
		return -1;
	}

	const char* pData = c_pData;

	pData += sizeof(TPacketCGDefault);
	uiBytes -= sizeof(TPacketCGDefault);

	if (uiBytes < sizeof(DWORD))
	{
		return -1;
	}

	const DWORD dwItemVnum = *reinterpret_cast<const DWORD*>(pData);
	ch->SendTreasureBoxLoot(dwItemVnum);

	return sizeof(DWORD);
}
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
void CInputMain::LoadTargetInfo(LPCHARACTER pkCharacter, TPacketCGTargetLoad* pkPacket)
{
	if (!pkCharacter || !pkCharacter->GetDesc())
	{
		return;
	}

	LPCHARACTER pkMonster = CHARACTER_MANAGER::instance().Find(pkPacket->dwVID);
	if (!pkMonster)
	{
		return;
	}

	TPacketGCTargetInfo kPacket;
	kPacket.byHeader = HEADER_GC_TARGET_INFO;
	kPacket.wSize = sizeof(kPacket);
	kPacket.dwVNum = pkMonster->GetRaceNum();
	kPacket.uCount = 0;

	auto pvecDropInfo = ITEM_MANAGER::instance().FindDropInfo(pkMonster->GetRaceNum());
	if (pvecDropInfo)
	{
		kPacket.uCount = pvecDropInfo->size();
	}

	if (kPacket.uCount > 0)
	{
		kPacket.wSize += sizeof(ITEM_MANAGER::TDropInfo) * kPacket.uCount;

		pkCharacter->GetDesc()->BufferedPacket(&kPacket, sizeof(kPacket));
		pkCharacter->GetDesc()->Packet(&(*pvecDropInfo)[0], sizeof(ITEM_MANAGER::TDropInfo) * kPacket.uCount);
	}
	else
	{
		pkCharacter->GetDesc()->Packet(&kPacket, sizeof(kPacket));
	}
}
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
int CInputMain::RequestDeleteSocket(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	//const TPacketCGDefault* p = reinterpret_cast<const TPacketCGDefault*>(c_pData);
	if (uiBytes < sizeof(TPacketCGDefault))
	{
		return -1;
	}

	const char* pData = c_pData;

	pData += sizeof(TPacketCGDefault);
	uiBytes -= sizeof(TPacketCGDefault);

	if (uiBytes < sizeof(BYTE))
	{
		return -1;
	}

	BYTE subheader = *(BYTE*)(pData++);
	uiBytes -= sizeof(BYTE);

	switch (subheader)
	{
	case SUBHEADER_REQUEST_DELETE_SOCKET_OPEN:
	{
		if (uiBytes < sizeof(WORD))
		{
			return -1;
		}

		WORD index = *(WORD*)(pData);
		ch->OpenDestroyItemSocket(index);
	}
	return sizeof(BYTE) + sizeof(WORD);
	case SUBHEADER_REQUEST_DELETE_SOCKET_DELETE:
	{
		if (uiBytes < sizeof(WORD))
		{
			return -1;
		}

		WORD index = *(WORD*)(pData);
		ch->DestroyItemSocket(index);
	}
	return sizeof(BYTE) + sizeof(WORD);
	case SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE:
	{
		ch->CloseDestroyItemSocket();
	}
	return sizeof(BYTE);
	default:
		sys_err("CInputMain::RequestDeleteSocketnger : Unknown subheader %d : %s", subheader, ch->GetName());
		break;
	}

	return 0;
}
#endif

int CInputDead::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	LPCHARACTER ch;

	if (!(ch = d->GetCharacter()))
	{
		sys_err("no character on desc");
		return 0;
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
	case HEADER_CG_PONG:
		Pong(d);
		break;

	case HEADER_CG_TIME_SYNC:
		Handshake(d, c_pData);
		break;

	case HEADER_CG_CHAT:
		if ((iExtraLen = Chat(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}

		break;

	case HEADER_CG_WHISPER:
		if ((iExtraLen = Whisper(ch, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}

		break;

	case HEADER_CG_HACK:
		Hack(ch, c_pData);
		break;

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	case HEADER_CG_WHISPER_REQUEST_LANGUAGE:
		WhisperRequestLanguage(ch, c_pData);
		break;
#endif

	default:
		return (0);
	}

	return (iExtraLen);
}

#ifdef __ENABLE_SWITCHBOT__
int CInputMain::Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	const TPacketCGSwitchbot* p = reinterpret_cast<const TPacketCGSwitchbot*>(data);

	if (uiBytes < sizeof(TPacketCGSwitchbot))
	{
		return -1;
	}

	const char* c_pData = data + sizeof(TPacketCGSwitchbot);
	uiBytes -= sizeof(TPacketCGSwitchbot);

	switch (p->subheader)
	{
	case SUBHEADER_CG_SWITCHBOT_START:
	{
		size_t extraLen = sizeof(TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
		if (uiBytes < extraLen)
		{
			return -1;
		}

		std::vector<TSwitchbotAttributeAlternativeTable> vec_alternatives;

		for (BYTE alternative = 0; alternative < SWITCHBOT_ALTERNATIVE_COUNT; ++alternative)
		{
			const TSwitchbotAttributeAlternativeTable* pAttr = reinterpret_cast<const TSwitchbotAttributeAlternativeTable*>(c_pData);
			c_pData += sizeof(TSwitchbotAttributeAlternativeTable);

#if __cplusplus < 199711L
			vec_alternatives.push_back(*pAttr);
#else
			vec_alternatives.emplace_back(*pAttr);
#endif
		}

		CSwitchbotManager::Instance().Start(ch->GetPlayerID(), p->slot, vec_alternatives);
		return extraLen;
	}

	case SUBHEADER_CG_SWITCHBOT_STOP:
	{
		CSwitchbotManager::Instance().Stop(ch->GetPlayerID(), p->slot);
		return 0;
	}
	}

	return 0;
}
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
int CInputMain::MarbleManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	if (!ch)
	{
		return -1;
	}

	TPacketCGMarbleManagerAction* p = (TPacketCGMarbleManagerAction*)c_pData;
	c_pData += sizeof(TPacketCGMarbleManagerAction);

	return CMarbleSystemManager::Instance().RecvClientPacket(p->bSubHeader, ch, c_pData, uiBytes);
}
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
void CInputMain::RefineElement(LPCHARACTER ch, const char* c_pData)
{
	const TPacketCGRefineElement* p = reinterpret_cast<const TPacketCGRefineElement*>(c_pData);

	if (ch->GetExchange() || ch->IsOpenSafebox() || ch->IsCubeOpen())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Can't do this right now"));
		ch->ClearRefineElement();
		return;
	}

	// Window close
	if (p->bArg == 255)
	{
		ch->ClearRefineElement();
		return;
	}

	ch->DoRefineElement(p->bArg);
}
#endif

#ifdef __ENABLE_LUCKY_BOX__
int CInputMain::LuckyBox(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGLuckyBox* p = (TPacketCGLuckyBox*)data;

	if (uiBytes < sizeof(TPacketCGLuckyBox))
	{
		return -1;
	}

	//const char* c_pData = data + sizeof(TPacketCGLuckyBox);
	uiBytes -= sizeof(TPacketCGLuckyBox);

	ch->LuckyBox(p->bAction);

	return 0;
}
#endif

#ifdef __SHOP_SEARCH__
void CInputMain::ShopSearchByName(LPCHARACTER ch, const char* data)
{
	TPacketCGShopSearchByName* p = (TPacketCGShopSearchByName*)data;
	CShopSearchManager::PC_RequestSearch(ch, p->itemName, p->page, p->entryCountIdx, p->sortType);
}

int CInputMain::ShopSearchByOptions(LPCHARACTER ch, const char* data, size_t uiBytes)
{
	TPacketCGShopSearchByOptions* p = (TPacketCGShopSearchByOptions*)data;
	data += sizeof(TPacketCGShopSearchByOptions);
	uiBytes -= sizeof(TPacketCGShopSearchByOptions);

	if (uiBytes < sizeof(TShopSearchItemType) * p->options.typeFlagCount)
	{
		return -1;
	}

	TShopSearchItemType* itemTypeFlags = (TShopSearchItemType*)data;
	data += sizeof(TShopSearchItemType) * p->options.typeFlagCount;
	uiBytes -= sizeof(TShopSearchItemType) * p->options.typeFlagCount;

	if (uiBytes < sizeof(DWORD) * p->options.specificVnumCount)
	{
		return -1;
	}

	CShopSearchManager::PC_RequestSearch(ch, &p->options, itemTypeFlags, (const DWORD*)data, p->page, p->entryCountIdx, p->sortType);

	return sizeof(TShopSearchItemType) * p->options.typeFlagCount + sizeof(DWORD) * p->options.specificVnumCount;
}

void CInputMain::ShopSearchBuy(LPCHARACTER ch, const char* data)
{
	TPacketCGShopSearchBuy* p = (TPacketCGShopSearchBuy*)data;

	CShopSearchManager::PC_RequestBuy(ch, p->itemID, p->itemVnum, p->itemPrice);
}

void CInputMain::ShopSearchOwnerMessage(LPCHARACTER ch, const char* data)
{
	TPacketCGShopSearchOwnerMessage* p = (TPacketCGShopSearchOwnerMessage*)data;

	TPacketGCShopSearchOwnerMessage pack;
	pack.header = HEADER_GC_SHOP_SEARCH_OWNER_MESSAGE;
	memset(pack.ownerName, 0, sizeof(pack.ownerName));

	if (LPCHARACTER tch = CHARACTER_MANAGER::instance().FindByPID(p->ownerID))
	{
		strlcpy(pack.ownerName, tch->GetName(), sizeof(pack.ownerName));
	}
	else if (CCI* pkCCI = P2P_MANAGER::instance().FindByPID(p->ownerID))
	{
		strlcpy(pack.ownerName, pkCCI->szName, sizeof(pack.ownerName));
	}
	else
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT name FROM player WHERE id = %u", p->ownerID));
		if (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			strlcpy(pack.ownerName, row[0], sizeof(pack.ownerName));
		}
	}

	ch->GetDesc()->Packet(&pack, sizeof(pack));
}

void CInputMain::ShopSearchRequestSoldInfo(LPCHARACTER ch, const char* data)
{
	TPacketCGShopSearchRequestSoldInfo* p = (TPacketCGShopSearchRequestSoldInfo*)data;

	CShopSearchManager::PC_RequestSoldInfo(ch, p->itemVnum);
}
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
void CInputMain::WhisperRequestLanguage(LPCHARACTER ch, const char* data)
{
	TPacketCGWhisperRequestLanguage* p = (TPacketCGWhisperRequestLanguage*)data;
	p->name[CHARACTER_NAME_MAX_LEN] = '\0';

	int iLang = -1;
	if (LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->name))
	{
		iLang = ch->GetLocale();
	}
	else if (CCI* pkCCI = P2P_MANAGER::instance().Find(p->name))
	{
		iLang = pkCCI->iLocale;
	}

	if (iLang >= 0)
	{
		TPacketGCWhisperLanguageInfo pack;
		pack.header = HEADER_GC_WHISPER_LANGUAGE_INFO;
		strcpy(pack.name, p->name);
		pack.language = iLang;

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}
#endif
