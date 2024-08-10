#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

#include "PythonGuild.h"
#include "PythonCharacterManager.h"
#include "PythonPlayer.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonTextTail.h"
#include "PythonItem.h"
#include "PythonChat.h"
#include "PythonShop.h"
#include "PythonExchange.h"
#include "PythonQuest.h"
#include "PythonEventManager.h"
#include "PythonMessenger.h"
#include "PythonApplication.h"

#include "../EterPack/EterPackManager.h"
#include "../gamelib/ItemManager.h"

#include "AbstractApplication.h"
#include "AbstractCharacterManager.h"
#include "InstanceBase.h"

#include "ProcessCRC.h"

#ifdef INGAME_WIKI
	#include "../GameLib/in_game_wiki.h"
#endif

#ifdef ENABLE_OFFLINE_SHOP
	#include "PythonOfflineShop.h"
#endif

#ifdef ENABLE_NEW_LOGS_CHAT
	#include "PythonLogsChatModule.h"
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	#include "PythonConfig.h"
#endif

#include <thread>
#include <chrono>

BOOL gs_bEmpireLanuageEnable = TRUE;

void CPythonNetworkStream::__RefreshAlignmentWindow()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshAlignment", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshTargetBoardByVID(DWORD dwVID)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByVID", Py_BuildValue("(i)", dwVID));
}

void CPythonNetworkStream::__RefreshTargetBoardByName(const char * c_szName)
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoardByName", Py_BuildValue("(s)", c_szName));
}

void CPythonNetworkStream::__RefreshTargetBoard()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshTargetBoard", Py_BuildValue("()"));
}

void CPythonNetworkStream::__RefreshGuildWindowGradePage()
{
	m_isRefreshGuildWndGradePage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowSkillPage()
{
	m_isRefreshGuildWndSkillPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPageGradeComboBox()
{
	m_isRefreshGuildWndMemberPageGradeComboBox = true;
}

void CPythonNetworkStream::__RefreshGuildWindowMemberPage()
{
	m_isRefreshGuildWndMemberPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowBoardPage()
{
	m_isRefreshGuildWndBoardPage = true;
}

void CPythonNetworkStream::__RefreshGuildWindowInfoPage()
{
	m_isRefreshGuildWndInfoPage = true;
}

void CPythonNetworkStream::__RefreshMessengerWindow()
{
	m_isRefreshMessengerWnd = true;
}

void CPythonNetworkStream::__RefreshSafeboxWindow()
{
	m_isRefreshSafeboxWnd = true;
}

void CPythonNetworkStream::__RefreshMallWindow()
{
	m_isRefreshMallWnd = true;
}

void CPythonNetworkStream::__RefreshSkillWindow()
{
	m_isRefreshSkillWnd = true;
}

void CPythonNetworkStream::__RefreshExchangeWindow()
{
	m_isRefreshExchangeWnd = true;
}

void CPythonNetworkStream::__RefreshStatus()
{
	m_isRefreshStatus = true;
}

void CPythonNetworkStream::__RefreshCharacterWindow()
{
	m_isRefreshCharacterWnd = true;
}

void CPythonNetworkStream::__RefreshInventoryWindow()
{
	m_isRefreshInventoryWnd = true;
}

void CPythonNetworkStream::__RefreshEquipmentWindow()
{
	m_isRefreshEquipmentWnd = true;
}

void CPythonNetworkStream::__SetGuildID(DWORD id)
{
	if (m_dwGuildID != id)
	{
		m_dwGuildID = id;
		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();

		for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
			if (!strncmp(m_akSimplePlayerInfo[i].szName, rkPlayer.GetName(), CHARACTER_NAME_MAX_LEN))
			{
				m_adwGuildID[i] = id;

				std::string  guildName;
				if (CPythonGuild::Instance().GetGuildName(id, &guildName))
				{
					m_astrGuildName[i] = guildName;
				}
				else
				{
					m_astrGuildName[i] = "";
				}
			}
	}
}

struct PERF_PacketInfo
{
	DWORD dwCount;
	DWORD dwTime;

	PERF_PacketInfo()
	{
		dwCount = 0;
		dwTime = 0;
	}
};


#ifdef __PERFORMANCE_CHECK__

class PERF_PacketTimeAnalyzer
{
public:
	~PERF_PacketTimeAnalyzer()
	{
		FILE* fp = fopen("perf_dispatch_packet_result.txt", "w");

		for (std::map<DWORD, PERF_PacketInfo>::iterator i = m_kMap_kPacketInfo.begin(); i != m_kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime > 0)
			{
				fprintf(fp, "header %d: count %d, time %d, tpc %d\n", i->first, i->second.dwCount, i->second.dwTime, i->second.dwTime / i->second.dwCount);
			}
		}
		fclose(fp);
	}

public:
	std::map<DWORD, PERF_PacketInfo> m_kMap_kPacketInfo;
};

PERF_PacketTimeAnalyzer gs_kPacketTimeAnalyzer;

#endif

// Game Phase ---------------------------------------------------------------------------
void CPythonNetworkStream::GamePhase()
{
	if (!m_kQue_stHack.empty())
	{
		__SendHack(m_kQue_stHack.front().c_str());
		m_kQue_stHack.pop_front();
	}

#ifdef ENABLE_PING_TIME
	if (m_strPhase == "Game")
	{
		auto current_time = ELTimer_GetMSec();
		if ((!m_waitForPingTimer && current_time - m_lastPingTimerSent >= 1000) || (m_waitForPingTimer && current_time - m_lastPingTimerSent >= 5000))
		{
			m_lastPingTimerSent = current_time;
			m_waitForPingTimer = true;

			BYTE packet = HEADER_CG_PING_TIMER;
			Send(sizeof(packet), &packet);
		}
	}
#endif

	TPacketHeader header = 0;
	bool ret = true;


#ifdef __PERFORMANCE_CHECK__
	DWORD timeBeginDispatch = timeGetTime();

	static std::map<DWORD, PERF_PacketInfo> kMap_kPacketInfo;
	kMap_kPacketInfo.clear();
#endif

#ifndef ENABLE_NO_RECV_GAME_LIMIT
	const uint32_t MAX_RECV_COUNT = 64;
	const uint32_t SAFE_RECV_BUFSIZE = 8192;
	uint32_t dwRecvCount = 0;
#endif

	while (ret)
	{
#ifndef ENABLE_NO_RECV_GAME_LIMIT
		if (dwRecvCount++ >= MAX_RECV_COUNT - 1 && GetRecvBufferSize() < SAFE_RECV_BUFSIZE
				&& m_strPhase == "Game")
		{
			break;
		}
#endif

		if (!CheckPacket(&header))
		{
			break;
		}

#ifdef __PERFORMANCE_CHECK__
		DWORD timeBeginPacket = timeGetTime();
#endif

#if defined(_DEBUG) && defined(ENABLE_PRINT_RECV_PACKET_DEBUG)
		Tracenf("RECV HEADER : %u , phase %s ", header, m_strPhase.c_str());
#endif

		switch (header)
		{
		case HEADER_GC_WARP:
			ret = RecvWarpPacket();
			break;

		case HEADER_GC_PHASE:
			ret = RecvPhasePacket();
			return;
			break;

		case HEADER_GC_PVP:
			ret = RecvPVPPacket();
			break;

		case HEADER_GC_DUEL_START:
			ret = RecvDuelStartPacket();
			break;

		case HEADER_GC_CHARACTER_ADD:
			ret = RecvCharacterAppendPacket();
			break;

		case HEADER_GC_CHAR_ADDITIONAL_INFO:
			ret = RecvCharacterAdditionalInfo();
			break;

		case HEADER_GC_CHARACTER_UPDATE:
			ret = RecvCharacterUpdatePacket();
			break;

		case HEADER_GC_CHARACTER_DEL:
			ret = RecvCharacterDeletePacket();
			break;

		case HEADER_GC_CHAT:
			ret = RecvChatPacket();
			break;

		case HEADER_GC_SYNC_POSITION:
			ret = RecvSyncPositionPacket();
			break;

		case HEADER_GC_OWNERSHIP:
			ret = RecvOwnerShipPacket();
			break;

		case HEADER_GC_WHISPER:
			ret = RecvWhisperPacket();
			break;

		case HEADER_GC_CHARACTER_MOVE:
			ret = RecvCharacterMovePacket();
			break;

		// Position
		case HEADER_GC_CHARACTER_POSITION:
			ret = RecvCharacterPositionPacket();
			break;

		// Battle Packet
		case HEADER_GC_STUN:
			ret = RecvStunPacket();
			break;

		case HEADER_GC_DEAD:
			ret = RecvDeadPacket();
			break;

		case HEADER_GC_PLAYER_POINT_CHANGE:
			ret = RecvPointChange();
			break;

		// item packet.
		case HEADER_GC_ITEM_SET:
			ret = RecvItemSetPacket();
			break;

		case HEADER_GC_ITEM_SET2:
			ret = RecvItemSetPacket2();
			break;

		case HEADER_GC_ITEM_USE:
			ret = RecvItemUsePacket();
			break;

		case HEADER_GC_ITEM_UPDATE:
			ret = RecvItemUpdatePacket();
			break;

		case HEADER_GC_ITEM_GROUND_ADD:
			ret = RecvItemGroundAddPacket();
			break;

		case HEADER_GC_ITEM_GROUND_DEL:
			ret = RecvItemGroundDelPacket();
			break;

		case HEADER_GC_ITEM_OWNERSHIP:
			ret = RecvItemOwnership();
			break;

		case HEADER_GC_QUICKSLOT_ADD:
			ret = RecvQuickSlotAddPacket();
			break;

		case HEADER_GC_QUICKSLOT_DEL:
			ret = RecvQuickSlotDelPacket();
			break;

		case HEADER_GC_QUICKSLOT_SWAP:
			ret = RecvQuickSlotMovePacket();
			break;

		case HEADER_GC_MOTION:
			ret = RecvMotionPacket();
			break;

		case HEADER_GC_SHOP:
			ret = RecvShopPacket();
			break;

		case HEADER_GC_SHOP_SIGN:
			ret = RecvShopSignPacket();
			break;

		case HEADER_GC_EXCHANGE:
			ret = RecvExchangePacket();
			break;

		case HEADER_GC_QUEST_INFO:
			ret = RecvQuestInfoPacket();
			break;

		case HEADER_GC_REQUEST_MAKE_GUILD:
			ret = RecvRequestMakeGuild();
			break;

		case HEADER_GC_PING:
			ret = RecvPingPacket();
			break;

		case HEADER_GC_SCRIPT:
			ret = RecvScriptPacket();
			break;

		case HEADER_GC_QUEST_CONFIRM:
			ret = RecvQuestConfirmPacket();
			break;

		case HEADER_GC_TARGET:
			ret = RecvTargetPacket();
			break;

		case HEADER_GC_DAMAGE_INFO:
			ret = RecvDamageInfoPacket();
			break;

		case HEADER_GC_MOUNT:
			ret = RecvMountPacket();
			break;

		case HEADER_GC_CHANGE_SPEED:
			ret = RecvChangeSpeedPacket();
			break;

		case HEADER_GC_PLAYER_POINTS:
			ret = __RecvPlayerPoints();
			break;

		case HEADER_GC_CREATE_FLY:
			ret = RecvCreateFlyPacket();
			break;

		case HEADER_GC_FLY_TARGETING:
			ret = RecvFlyTargetingPacket();
			break;

		case HEADER_GC_ADD_FLY_TARGETING:
			ret = RecvAddFlyTargetingPacket();
			break;

		case HEADER_GC_SKILL_LEVEL:
			ret = RecvSkillLevel();
			break;

		case HEADER_GC_MESSENGER:
			ret = RecvMessenger();
			break;

		case HEADER_GC_GUILD:
			ret = RecvGuild();
			break;

		case HEADER_GC_PARTY_INVITE:
			ret = RecvPartyInvite();
			break;

		case HEADER_GC_PARTY_ADD:
			ret = RecvPartyAdd();
			break;

		case HEADER_GC_PARTY_UPDATE:
			ret = RecvPartyUpdate();
			break;

#ifdef ENABLE_PARTY_ATLAS
		case HEADER_GC_PARTY_POSITION:
			ret = RecvPartyPosition();
			break;
#endif

		case HEADER_GC_PARTY_REMOVE:
			ret = RecvPartyRemove();
			break;

		case HEADER_GC_PARTY_LINK:
			ret = RecvPartyLink();
			break;

		case HEADER_GC_PARTY_UNLINK:
			ret = RecvPartyUnlink();
			break;

		case HEADER_GC_PARTY_PARAMETER:
			ret = RecvPartyParameter();
			break;

		case HEADER_GC_SAFEBOX_SET:
			ret = RecvSafeBoxSetPacket();
			break;

		case HEADER_GC_SAFEBOX_DEL:
			ret = RecvSafeBoxDelPacket();
			break;

		case HEADER_GC_SAFEBOX_WRONG_PASSWORD:
			ret = RecvSafeBoxWrongPasswordPacket();
			break;

		case HEADER_GC_SAFEBOX_SIZE:
			ret = RecvSafeBoxSizePacket();
			break;

		case HEADER_GC_FISHING:
			ret = RecvFishing();
			break;

		case HEADER_GC_DUNGEON:
			ret = RecvDungeon();
			break;

		case HEADER_GC_TIME:
			ret = RecvTimePacket();
			break;

		case HEADER_GC_WALK_MODE:
			ret = RecvWalkModePacket();
			break;

		case HEADER_GC_CHANGE_SKILL_GROUP:
			ret = RecvChangeSkillGroupPacket();
			break;

		case HEADER_GC_REFINE_INFORMATION:
			ret = RecvRefineInformationPacket();
			break;

		case HEADER_GC_SEPCIAL_EFFECT:
			ret = RecvSpecialEffect();
			break;

		case HEADER_GC_NPC_POSITION:
			ret = RecvNPCList();
			break;

		case HEADER_GC_CHANNEL:
			ret = RecvChannelPacket();
			break;

		case HEADER_GC_VIEW_EQUIP:
			ret = RecvViewEquipPacket();
			break;

		case HEADER_GC_LAND_LIST:
			ret = RecvLandPacket();
			break;

		//case HEADER_GC_TARGET_CREATE:
		//	ret = RecvTargetCreatePacket();
		//	break;

		case HEADER_GC_TARGET_CREATE_NEW:
			ret = RecvTargetCreatePacketNew();
			break;

		case HEADER_GC_TARGET_UPDATE:
			ret = RecvTargetUpdatePacket();
			break;

		case HEADER_GC_TARGET_DELETE:
			ret = RecvTargetDeletePacket();
			break;

		case HEADER_GC_AFFECT_ADD:
			ret = RecvAffectAddPacket();
			break;

		case HEADER_GC_AFFECT_REMOVE:
			ret = RecvAffectRemovePacket();
			break;

		case HEADER_GC_MALL_OPEN:
			ret = RecvMallOpenPacket();
			break;

		case HEADER_GC_MALL_SET:
			ret = RecvMallItemSetPacket();
			break;

		case HEADER_GC_MALL_DEL:
			ret = RecvMallItemDelPacket();
			break;

		case HEADER_GC_LOVER_INFO:
			ret = RecvLoverInfoPacket();
			break;

		case HEADER_GC_LOVE_POINT_UPDATE:
			ret = RecvLovePointUpdatePacket();
			break;

		case HEADER_GC_DIG_MOTION:
			ret = RecvDigMotionPacket();
			break;

		case HEADER_GC_HANDSHAKE:
			RecvHandshakePacket();
			return;
			break;

		case HEADER_GC_HANDSHAKE_OK:
			RecvHandshakeOKPacket();
			return;
			break;

		case HEADER_GC_HYBRIDCRYPT_KEYS:
			RecvHybridCryptKeyPacket();
			return;
			break;

		case HEADER_GC_HYBRIDCRYPT_SDB:
			RecvHybridCryptSDBPacket();
			return;
			break;


#ifdef _IMPROVED_PACKET_ENCRYPTION_
		case HEADER_GC_KEY_AGREEMENT:
			RecvKeyAgreementPacket();
			return;
			break;

		case HEADER_GC_KEY_AGREEMENT_COMPLETED:
			RecvKeyAgreementCompletedPacket();
			return;
			break;
#endif

		case HEADER_GC_HS_REQUEST:
			ret = RecvHSCheckRequest();
			break;

		case HEADER_GC_XTRAP_CS1_REQUEST:
			ret = RecvXTrapVerifyRequest();
			break;

		case HEADER_GC_SPECIFIC_EFFECT:
			ret = RecvSpecificEffect();
			break;

		case HEADER_GC_DRAGON_SOUL_REFINE:
			ret = RecvDragonSoulRefine();
			break;

		case HEADER_GC_UNK_213: // @fixme007
			ret = RecvUnk213();
			break;

#ifdef ENABLE_TREASURE_BOX_LOOT
		case HEADER_GC_REQUEST_TREASURE_BOX_LOOT:
			ret = RecvRequestTreasureBoxLoot();
			break;
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
		case HEADER_GC_TARGET_INFO:
			ret = RecvTargetInfoPacket();
			break;
#endif

#ifdef ENABLE_DELETE_SINGLE_STONE
		case HEADER_GC_REQUEST_DELETE_SOCKET:
			ret = RecvRequestDeleteSocket();
			break;
#endif

#ifdef ENABLE_SWITCHBOT
		case HEADER_GC_SWITCHBOT:
			ret = RecvSwitchbotPacket();
			break;
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
		case HEADER_GC_EXCHANGE_INFO:
			ret = RecvExchangeInfoPacket();
			break;
#endif

#ifdef ENABLE_VOICE_CHAT
		case HEADER_GC_VOICE_CHAT:
			ret = RecvDataVoiceChat();
			break;
#endif

#ifdef ENABLE_ADMIN_MANAGER
		case HEADER_GC_ADMIN_MANAGER_LOAD:
			ret = RecvAdminManagerLoad();
			break;

		case HEADER_GC_ADMIN_MANAGER:
			ret = RecvAdminManager();
			break;
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
		case HEADER_GC_BIOLOG_MANAGER:
			ret = RecvBiologManager();
			break;
#endif

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
		case HEADER_GC_MARBLE_MANAGER:
			ret = RecvMarbleManager();
			break;
#endif

#ifdef ENABLE_CUBE_RENEWAL
		case HEADER_GC_CUBE_ITEM:
			ret = RecvCubeItemPacket();
			break;

		case HEADER_GC_CUBE_CRAFT:
			ret = RecvCubePacket();
			break;
#endif

#ifdef ENABLE_FIND_LETTERS_EVENT
		case HEADER_GC_FIND_LETTERS_INFO:
			ret = RecvFindLettersInfo();
			break;
#endif

#ifdef ENABLE_REFINE_ELEMENT
		case HEADER_GC_REFINE_ELEMENT:
			ret = RecvRefineElementPacket();
			break;
#endif

#ifdef ENABLE_LUCKY_BOX
		case HEADER_GC_LUCKY_BOX:
			ret = RecvLuckyBox();
			break;
#endif

#ifdef ENABLE_TEAMLER_STATUS
		case HEADER_GC_SHOW_TEAMLER:
			ret = RecvShowTeamler();
			break;

		case HEADER_GC_TEAMLER_STATUS:
			ret = RecvTeamlerStatus();
			break;
#endif

#ifdef __INVENTORY_BUFFERING__
		case HEADER_GC_ITEM_BUFFERED:
			ret = RecvBufferedInventoryPacket();
			break;
#endif

#ifdef INGAME_WIKI
		case InGameWiki::HEADER_GC_WIKI:
			ret = RecvWikiPacket();
			break;
#endif

#ifdef ENABLE_OFFLINE_SHOP
		case HEADER_GC_OFFLINE_SHOP:
			ret = CPythonOfflineShop::ReceivePacket();
			break;
#endif

#ifdef ENABLE_SHOP_SEARCH
		case HEADER_GC_SHOP_SEARCH_RESULT:
			ret = RecvShopSearchResult();
			break;

		case HEADER_GC_SHOP_SEARCH_BUY_RESULT:
			ret = RecvShopSearchBuyResult();
			break;

		case HEADER_GC_SHOP_SEARCH_OWNER_MESSAGE:
			ret = RecvShopSearchOwnerMessage();
			break;

		case HEADER_GC_SHOP_SEARCH_SOLD_INFO:
			ret = RecvShopSearchSoldInfo();
			break;
#endif

#ifdef ENABLE_PING_TIME
		case HEADER_GC_PING_TIMER:
			ret = RecvPingTimer();
			break;
#endif

#ifdef ENABLE_NEW_LOGS_CHAT
		case HEADER_GC_CHAT_LOGS:
			ret = CPythonLogsChatModule::instance().ReceivePacket();
			break;
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		case HEADER_GC_WHISPER_LANGUAGE_INFO:
			ret = RecvWhisperLanguageInfo();
			break;
#endif

#ifdef ENABLE_SHAMAN_SYSTEM
		case HEADER_GC_SHAMAN_SKILL:
			ret = RecvShamanUseSkill();
			break;
#endif

		default:
			ret = RecvDefaultPacket(header);
			break;

		}
#ifdef __PERFORMANCE_CHECK__
		DWORD timeEndPacket = timeGetTime();

		{
			PERF_PacketInfo& rkPacketInfo = kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime += timeEndPacket - timeBeginPacket;
		}

		{
			PERF_PacketInfo& rkPacketInfo = gs_kPacketTimeAnalyzer.m_kMap_kPacketInfo[header];
			rkPacketInfo.dwCount++;
			rkPacketInfo.dwTime += timeEndPacket - timeBeginPacket;
		}
#endif
	}

#ifdef __PERFORMANCE_CHECK__
	DWORD timeEndDispatch = timeGetTime();

	if (timeEndDispatch - timeBeginDispatch > 2)
	{
		static FILE* fp = fopen("perf_dispatch_packet.txt", "w");

		fprintf(fp, "delay %d\n", timeEndDispatch - timeBeginDispatch);
		for (std::map<DWORD, PERF_PacketInfo>::iterator i = kMap_kPacketInfo.begin(); i != kMap_kPacketInfo.end(); ++i)
		{
			if (i->second.dwTime > 0)
			{
				fprintf(fp, "header %d: count %d, time %d\n", i->first, i->second.dwCount, i->second.dwTime);
			}
		}
		fputs("=====================================================\n", fp);
		fflush(fp);
	}
#endif

	if (!ret)
	{
		RecvErrorPacket(header);
	}

	static DWORD s_nextRefreshTime = ELTimer_GetMSec();

	DWORD curTime = ELTimer_GetMSec();
	if (s_nextRefreshTime > curTime)
	{
		return;
	}



	if (m_isRefreshCharacterWnd)
	{
		m_isRefreshCharacterWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshEquipmentWnd)
	{
		m_isRefreshEquipmentWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshEquipment", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshInventoryWnd)
	{
		m_isRefreshInventoryWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshInventory", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshExchangeWnd)
	{
		m_isRefreshExchangeWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshExchange", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSkillWnd)
	{
		m_isRefreshSkillWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSkill", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshSafeboxWnd)
	{
		m_isRefreshSafeboxWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSafebox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMallWnd)
	{
		m_isRefreshMallWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMall", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshStatus)
	{
		m_isRefreshStatus = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshStatus", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshMessengerWnd)
	{
		m_isRefreshMessengerWnd = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshMessenger", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndInfoPage)
	{
		m_isRefreshGuildWndInfoPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildInfoPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndBoardPage)
	{
		m_isRefreshGuildWndBoardPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildBoardPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPage)
	{
		m_isRefreshGuildWndMemberPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndMemberPageGradeComboBox)
	{
		m_isRefreshGuildWndMemberPageGradeComboBox = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildMemberPageGradeComboBox", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndSkillPage)
	{
		m_isRefreshGuildWndSkillPage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildSkillPage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}

	if (m_isRefreshGuildWndGradePage)
	{
		m_isRefreshGuildWndGradePage = false;
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGradePage", Py_BuildValue("()"));
		s_nextRefreshTime = curTime + 300;
	}
}

void CPythonNetworkStream::__InitializeGamePhase()
{
	__ServerTimeSync_Initialize();

	m_isRefreshStatus = false;
	m_isRefreshCharacterWnd = false;
	m_isRefreshEquipmentWnd = false;
	m_isRefreshInventoryWnd = false;
	m_isRefreshExchangeWnd = false;
	m_isRefreshSkillWnd = false;
	m_isRefreshSafeboxWnd = false;
	m_isRefreshMallWnd = false;
	m_isRefreshMessengerWnd = false;
	m_isRefreshGuildWndInfoPage = false;
	m_isRefreshGuildWndBoardPage = false;
	m_isRefreshGuildWndMemberPage = false;
	m_isRefreshGuildWndMemberPageGradeComboBox = false;
	m_isRefreshGuildWndSkillPage = false;
	m_isRefreshGuildWndGradePage = false;
	m_EmoticonStringVector.clear();

	m_pInstTarget = NULL;
}

void CPythonNetworkStream::Warp(LONG lGlobalX, LONG lGlobalY)
{
	CPythonBackground& rkBgMgr = CPythonBackground::Instance();
	rkBgMgr.Destroy();
	rkBgMgr.Create();
	rkBgMgr.Warp(lGlobalX, lGlobalY);
	//rkBgMgr.SetShadowLevel(CPythonBackground::SHADOW_ALL);
	rkBgMgr.RefreshShadowLevel();

	LONG lLocalX = lGlobalX;
	LONG lLocalY = lGlobalY;
	__GlobalPositionToLocalPosition(lLocalX, lLocalY);
	float fHeight = CPythonBackground::Instance().GetHeight(float(lLocalX), float(lLocalY));

	IAbstractApplication& rkApp = IAbstractApplication::GetSingleton();
	rkApp.SetCenterPosition(float(lLocalX), float(lLocalY), fHeight);

	__ShowMapName(lLocalX, lLocalY);
}

void CPythonNetworkStream::__ShowMapName(LONG lLocalX, LONG lLocalY)
{
	const std::string & c_rstrMapFileName = CPythonBackground::Instance().GetWarpMapName();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ShowMapName", Py_BuildValue("(sii)", c_rstrMapFileName.c_str(), lLocalX, lLocalY));
}

void CPythonNetworkStream::__LeaveGamePhase()
{
	CInstanceBase::ClearPVPKeySystem();

	__ClearNetworkActorManager();

	m_bComboSkillFlag = FALSE;

	IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
	rkChrMgr.Destroy();

	CPythonItem& rkItemMgr = CPythonItem::Instance();
	rkItemMgr.Destroy();

	CPythonPlayer::Instance().Clear();

#ifdef ENABLE_TRANSMUTE
	CPythonTransmute::Instance().Clear();
#endif
}

void CPythonNetworkStream::SetGamePhase()
{
	if ("Game" != m_strPhase)
	{
		m_phaseLeaveFunc.Run();
	}

	Tracen("");
	Tracen("## Network - Game Phase ##");
	Tracen("");

	m_strPhase = "Game";

	m_dwChangingPhaseTime = ELTimer_GetMSec();
	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::GamePhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveGamePhase);


	IAbstractPlayer & rkPlayer = IAbstractPlayer::GetSingleton();
	rkPlayer.SetMainCharacterIndex(GetMainActorVID());

	__RefreshStatus();
}

bool CPythonNetworkStream::RecvUnk213() // @fixme007
{
	TPacketGCUnk213 kUnk213Packet;
	if (!Recv(sizeof(TPacketGCUnk213)), &kUnk213Packet)
	{
		return false;
	}
	return true;
}

bool CPythonNetworkStream::RecvWarpPacket()
{
	TPacketGCWarp kWarpPacket;

	if (!Recv(sizeof(kWarpPacket), &kWarpPacket))
	{
		return false;
	}

	bDisableChatPacket = true;
	__DirectEnterMode_Set(m_dwSelectedCharacterIndex);

	CNetworkStream::Connect((DWORD)kWarpPacket.lAddr, kWarpPacket.wPort);

	return true;
}

bool CPythonNetworkStream::RecvDuelStartPacket()
{
	TPacketGCDuelStart kDuelStartPacket;
	if (!Recv(sizeof(kDuelStartPacket), &kDuelStartPacket))
	{
		return false;
	}

	DWORD count = (kDuelStartPacket.wSize - sizeof(kDuelStartPacket)) / sizeof(DWORD);

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		TraceError("CPythonNetworkStream::RecvDuelStartPacket - MainCharacter is NULL");
		return false;
	}
	DWORD dwVIDSrc = pkInstMain->GetVirtualID();
	DWORD dwVIDDest;

	for ( DWORD i = 0; i < count; i++)
	{
		Recv(sizeof(dwVIDDest), &dwVIDDest);
		CInstanceBase::InsertDUELKey(dwVIDSrc, dwVIDDest);
	}

	if (count == 0)
	{
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_CANNOTATTACK);
	}
	else
	{
		pkInstMain->SetDuelMode(CInstanceBase::DUEL_START);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));

	rkChrMgr.RefreshAllPCTextTail();

	return true;
}

bool CPythonNetworkStream::RecvPVPPacket()
{
	TPacketGCPVP kPVPPacket;
	if (!Recv(sizeof(kPVPPacket), &kPVPPacket))
	{
		return false;
	}

	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();
	CPythonPlayer & rkPlayer = CPythonPlayer::Instance();

	switch (kPVPPacket.bMode)
	{
	case PVP_MODE_AGREE:
		rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

		if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDDst))
		{
			rkPlayer.RememberChallengeInstance(kPVPPacket.dwVIDSrc);
		}

		if (rkPlayer.IsMainCharacterIndex(kPVPPacket.dwVIDSrc))
		{
			rkPlayer.RememberCantFightInstance(kPVPPacket.dwVIDDst);
		}
		break;
	case PVP_MODE_REVENGE:
	{
		rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);

		DWORD dwKiller = kPVPPacket.dwVIDSrc;
		DWORD dwVictim = kPVPPacket.dwVIDDst;

		if (rkPlayer.IsMainCharacterIndex(dwVictim))
		{
			rkPlayer.RememberRevengeInstance(dwKiller);
		}

		if (rkPlayer.IsMainCharacterIndex(dwKiller))
		{
			rkPlayer.RememberCantFightInstance(dwVictim);
		}
		break;
	}

	case PVP_MODE_FIGHT:
		rkChrMgr.InsertPVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
		rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
		rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
		break;
	case PVP_MODE_NONE:
		rkChrMgr.RemovePVPKey(kPVPPacket.dwVIDSrc, kPVPPacket.dwVIDDst);
		rkPlayer.ForgetInstance(kPVPPacket.dwVIDSrc);
		rkPlayer.ForgetInstance(kPVPPacket.dwVIDDst);
		break;
	}

	__RefreshTargetBoardByVID(kPVPPacket.dwVIDSrc);
	__RefreshTargetBoardByVID(kPVPPacket.dwVIDDst);

	return true;
}

// DELETEME
/*
void CPythonNetworkStream::__SendWarpPacket()
{
	TPacketCGWarp kWarpPacket;
	kWarpPacket.bHeader=HEADER_GC_WARP;
	if (!Send(sizeof(kWarpPacket), &kWarpPacket))
	{
		return;
	}
}
*/
void CPythonNetworkStream::NotifyHack(const char* c_szMsg)
{
	if (!m_kQue_stHack.empty())
		if (c_szMsg == m_kQue_stHack.back())
		{
			return;
		}

	m_kQue_stHack.push_back(c_szMsg);
}

bool CPythonNetworkStream::__SendHack(const char* c_szMsg)
{
	Tracen(c_szMsg);

	TPacketCGHack kPacketHack;
	kPacketHack.bHeader = HEADER_CG_HACK;
	strncpy(kPacketHack.szBuf, c_szMsg, sizeof(kPacketHack.szBuf) - 1);

	if (!Send(sizeof(kPacketHack), &kPacketHack))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByVIDPacket(DWORD vid)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_VID;
	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}
	if (!Send(sizeof(vid), &vid))
	{
		return false;
	}
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerAddByNamePacket(const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_ADD_BY_NAME;
	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}
	char szName[CHARACTER_NAME_MAX_LEN];
	strncpy(szName, c_szName, CHARACTER_NAME_MAX_LEN - 1);
	szName[CHARACTER_NAME_MAX_LEN - 1] = '\0';

	if (!Send(sizeof(szName), &szName))
	{
		return false;
	}
	Tracef(" SendMessengerAddByNamePacket : %s\n", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendMessengerRemovePacket(const char * c_szKey, const char * c_szName)
{
	TPacketCGMessenger packet;
	packet.header = HEADER_CG_MESSENGER;
	packet.subheader = MESSENGER_SUBHEADER_CG_REMOVE;
	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}
	char szKey[CHARACTER_NAME_MAX_LEN];
	strncpy(szKey, c_szKey, CHARACTER_NAME_MAX_LEN - 1);
	if (!Send(sizeof(szKey), &szKey))
	{
		return false;
	}
	__RefreshTargetBoardByName(c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendCharacterStatePacket(const TPixelPosition& c_rkPPosDst, float fDstRot, UINT eFunc, UINT uArg)
{
	NANOBEGIN
	if (!__CanActMainInstance())
	{
		return true;
	}

	if (fDstRot < 0.0f)
	{
		fDstRot = 360 + fDstRot;
	}
	else if (fDstRot > 360.0f)
	{
		fDstRot = fmodf(fDstRot, 360.0f);
	}

	TPacketCGMove kStatePacket;
	kStatePacket.bHeader = HEADER_CG_CHARACTER_MOVE;
	kStatePacket.bFunc = eFunc;
	kStatePacket.bArg = uArg;
	kStatePacket.bRot = fDstRot / 5.0f;
	kStatePacket.lX = long(c_rkPPosDst.x);
	kStatePacket.lY = long(c_rkPPosDst.y);
	kStatePacket.dwTime = ELTimer_GetServerMSec();

	assert(kStatePacket.lX >= 0 && kStatePacket.lX < 204800);

	__LocalPositionToGlobalPosition(kStatePacket.lX, kStatePacket.lY);

	if (!Send(sizeof(kStatePacket), &kStatePacket))
	{
		Tracenf("CPythonNetworkStream::SendCharacterStatePacket(dwCmdTime=%u, fDstPos=(%f, %f), fDstRot=%f, eFunc=%d uArg=%d) - PACKET SEND ERROR",
				kStatePacket.dwTime,
				float(kStatePacket.lX),
				float(kStatePacket.lY),
				fDstRot,
				kStatePacket.bFunc,
				kStatePacket.bArg);
		return false;
	}
	NANOEND
	return SendSequence();
}

bool CPythonNetworkStream::SendUseSkillPacket(DWORD dwSkillIndex, DWORD dwTargetVID)
{
	TPacketCGUseSkill UseSkillPacket;
	UseSkillPacket.bHeader = HEADER_CG_USE_SKILL;
	UseSkillPacket.dwVnum = dwSkillIndex;
	UseSkillPacket.dwTargetVID = dwTargetVID;
	if (!Send(sizeof(TPacketCGUseSkill), &UseSkillPacket))
	{
		Tracen("CPythonNetworkStream::SendUseSkillPacket - SEND PACKET ERROR");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendChatPacket(const char * c_szChat, BYTE byType)
{
	if (bDisableChatPacket)
		return true;

	if (strlen(c_szChat) == 0)
	{
		return true;
	}

	if (strlen(c_szChat) >= 512)
	{
		return true;
	}

	if (c_szChat[0] == '/')
	{
		if (1 == strlen(c_szChat))
		{
			if (!m_strLastCommand.empty())
			{
				c_szChat = m_strLastCommand.c_str();
			}
		}
		else
		{
			m_strLastCommand = c_szChat;
		}
	}

	if (ClientCommand(c_szChat))
	{
		return true;
	}

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGChat ChatPacket;
	ChatPacket.header = HEADER_CG_CHAT;
	ChatPacket.length = sizeof(ChatPacket) + iTextLen;
	ChatPacket.type = byType;

	if (!Send(sizeof(ChatPacket), &ChatPacket))
	{
		return false;
	}

	if (!Send(iTextLen, c_szChat))
	{
		return false;
	}

	return SendSequence();
}

//////////////////////////////////////////////////////////////////////////
// Emoticon
void CPythonNetworkStream::RegisterEmoticonString(const char * pcEmoticonString)
{
	if (m_EmoticonStringVector.size() >= CInstanceBase::EMOTICON_NUM)
	{
		TraceError("Can't register emoticon string... vector is full (size:%d)", m_EmoticonStringVector.size() );
		return;
	}
	m_EmoticonStringVector.push_back(pcEmoticonString);
}

bool CPythonNetworkStream::ParseEmoticon(const char * pChatMsg, DWORD * pdwEmoticon)
{
	for (DWORD dwEmoticonIndex = 0; dwEmoticonIndex < m_EmoticonStringVector.size() ; ++dwEmoticonIndex)
	{
		if (strlen(pChatMsg) > m_EmoticonStringVector[dwEmoticonIndex].size())
		{
			continue;
		}

		const char * pcFind = strstr(pChatMsg, m_EmoticonStringVector[dwEmoticonIndex].c_str());

		if (pcFind != pChatMsg)
		{
			continue;
		}

		*pdwEmoticon = dwEmoticonIndex;

		return true;
	}

	return false;
}
// Emoticon
//////////////////////////////////////////////////////////////////////////

void CPythonNetworkStream::__ConvertEmpireText(DWORD dwEmpireID, char* szText)
{
	if (dwEmpireID < 1 || dwEmpireID > 3)
	{
		return;
	}

	UINT uHanPos;

	STextConvertTable& rkTextConvTable = m_aTextConvTable[dwEmpireID - 1];

	BYTE* pbText = (BYTE*)szText;
	while (*pbText)
	{
		if (*pbText & 0x80)
		{
			if (pbText[0] >= 0xb0 && pbText[0] <= 0xc8 && pbText[1] >= 0xa1 && pbText[1] <= 0xfe)
			{
				uHanPos = (pbText[0] - 0xb0) * (0xfe - 0xa1 + 1) + (pbText[1] - 0xa1);
				pbText[0] = rkTextConvTable.aacHan[uHanPos][0];
				pbText[1] = rkTextConvTable.aacHan[uHanPos][1];
			}
			pbText += 2;
		}
		else
		{
			if (*pbText >= 'a' && *pbText <= 'z')
			{
				*pbText = rkTextConvTable.acLower[*pbText - 'a'];
			}
			else if (*pbText >= 'A' && *pbText <= 'Z')
			{
				*pbText = rkTextConvTable.acUpper[*pbText - 'A'];
			}
			pbText++;
		}
	}
}

bool CPythonNetworkStream::RecvChatPacket()
{
	TPacketGCChat kChat;
	char buf[1024 + 1];
	char line[1024 + 1];

	if (!Recv(sizeof(kChat), &kChat))
	{
		return false;
	}

	UINT uChatSize = kChat.size - sizeof(kChat);

	if (!Recv(uChatSize, buf))
	{
		return false;
	}

	buf[uChatSize] = '\0';

	if (kChat.type >= CHAT_TYPE_MAX_NUM)
	{
		return true;
	}

	if (CHAT_TYPE_COMMAND == kChat.type)
	{
		ServerCommand(buf);
		return true;
	}

	if (kChat.dwVID != 0)
	{
		CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
		CInstanceBase * pkInstChatter = rkChrMgr.GetInstancePtr(kChat.dwVID);
		if (NULL == pkInstChatter)
		{
			return true;
		}

		switch (kChat.type)
		{
		case CHAT_TYPE_TALKING:
		case CHAT_TYPE_PARTY:
		case CHAT_TYPE_GUILD:
		case CHAT_TYPE_SHOUT:
		case CHAT_TYPE_WHISPER:
		{
			char * p = strchr(buf, ':');

			if (p)
			{
				p += 2;
			}
			else
			{
				p = buf;
			}

			DWORD dwEmoticon;

			if (ParseEmoticon(p, &dwEmoticon))
			{
				pkInstChatter->SetEmoticon(dwEmoticon);
				return true;
			}
			else
			{
				if (gs_bEmpireLanuageEnable)
				{
					CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
					if (pkInstMain)
						if (!pkInstMain->IsSameEmpire(*pkInstChatter))
						{
							__ConvertEmpireText(pkInstChatter->GetEmpireID(), p);
						}
				}

				if (m_isEnableChatInsultFilter)
				{
					if (false == pkInstChatter->IsNPC() && false == pkInstChatter->IsEnemy())
					{
						__FilterInsult(p, strlen(p));
					}
				}

				_snprintf(line, sizeof(line), "%s", p);
			}
		}
		break;
		case CHAT_TYPE_COMMAND:
		case CHAT_TYPE_INFO:
		case CHAT_TYPE_NOTICE:
		case CHAT_TYPE_BIG_NOTICE:
			// case CHAT_TYPE_UNK_10:
#ifdef ENABLE_DICE_SYSTEM
		case CHAT_TYPE_DICE_INFO:
#endif
		case CHAT_TYPE_MAX_NUM:
		default:
			_snprintf(line, sizeof(line), "%s", buf);
			break;
		}

		if (CHAT_TYPE_SHOUT != kChat.type)
		{
			CPythonTextTail::Instance().RegisterChatTail(kChat.dwVID, line);
		}

		if (pkInstChatter->IsPC())
		{
			CPythonChat::Instance().AppendChat(kChat.type, buf);
		}
	}
	else
	{
		if (CHAT_TYPE_NOTICE == kChat.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetTipMessage", Py_BuildValue("(s)", buf));
		}
		else if (CHAT_TYPE_BIG_NOTICE == kChat.type)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_SetBigMessage", Py_BuildValue("(s)", buf));
		}
		else if (CHAT_TYPE_SHOUT == kChat.type)
		{
			char * p = strchr(buf, ':');

			if (p)
			{
				if (m_isEnableChatInsultFilter)
				{
					__FilterInsult(p, strlen(p));
				}
			}
		}

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		auto bLocaleName = CLocaleManager::instance().GetLanguageNameByID(kChat.locale);
		auto m_Enabled = CPythonConfig::instance().GetBool(CPythonConfig::CLASS_GENERAL, "language_chat_" + std::string(bLocaleName), 1);
		if (!m_Enabled)
			return true;

		CPythonChat::Instance().AppendChat(kChat.type, kChat.locale, buf);
#else
		CPythonChat::Instance().AppendChat(kChat.type, buf);
#endif
	}
	return true;
}

bool CPythonNetworkStream::RecvWhisperPacket()
{
	TPacketGCWhisper whisperPacket;
	char buf[512 + 1];

	if (!Recv(sizeof(whisperPacket), &whisperPacket))
	{
		return false;
	}

	assert(whisperPacket.wSize - sizeof(whisperPacket) < 512);

	if (!Recv(whisperPacket.wSize - sizeof(whisperPacket), &buf))
	{
		return false;
	}

	buf[whisperPacket.wSize - sizeof(whisperPacket)] = '\0';

	static char line[256];
	if (CPythonChat::WHISPER_TYPE_CHAT == whisperPacket.bType || CPythonChat::WHISPER_TYPE_GM == whisperPacket.bType)
	{
		_snprintf(line, sizeof(line), "[%s] (Lv. %d) %s : %s", whisperPacket.szSentDate, whisperPacket.bLevel, whisperPacket.szNameFrom, buf);
#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(isis)", (int)whisperPacket.bType, whisperPacket.szNameFrom, whisperPacket.iLocale, line));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, line));
#endif
	}
	else if (CPythonChat::WHISPER_TYPE_SYSTEM == whisperPacket.bType || CPythonChat::WHISPER_TYPE_ERROR == whisperPacket.bType)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperSystemMessage", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}
#ifdef OFFLINE_MESSAGE_SYSTEM
	else if (CPythonChat::WHISPER_TYPE_MESSAGE_SENT == whisperPacket.bType)
	{
		_snprintf(line, sizeof(line), "[%s] (Lv. %d) %s : %s", whisperPacket.szSentDate, whisperPacket.bLevel, whisperPacket.szNameFrom, buf);

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(isisi)", (int)whisperPacket.bType, whisperPacket.szNameFrom, whisperPacket.iLocale, line, 1));
#else
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisper", Py_BuildValue("(issi)", (int)whisperPacket.bType, whisperPacket.szNameFrom, line, 1));
#endif
	}
#endif
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvWhisperError", Py_BuildValue("(iss)", (int) whisperPacket.bType, whisperPacket.szNameFrom, buf));
	}

	return true;
}

bool CPythonNetworkStream::SendWhisperPacket(const char * name, const char * c_szChat)
{
	if (strlen(c_szChat) >= 255)
	{
		return true;
	}

	int iTextLen = strlen(c_szChat) + 1;
	TPacketCGWhisper WhisperPacket;
	WhisperPacket.bHeader = HEADER_CG_WHISPER;
	WhisperPacket.wSize = sizeof(WhisperPacket) + iTextLen;

	strncpy(WhisperPacket.szNameTo, name, sizeof(WhisperPacket.szNameTo) - 1);

	if (!Send(sizeof(WhisperPacket), &WhisperPacket))
	{
		return false;
	}

	if (!Send(iTextLen, c_szChat))
	{
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::RecvPointChange()
{
	TPacketGCPointChange PointChange;

	if (!Recv(sizeof(TPacketGCPointChange), &PointChange))
	{
		Tracen("Recv Point Change Packet Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	rkChrMgr.ShowPointEffect(PointChange.Type, PointChange.dwVID);

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pInstance && PointChange.dwVID == pInstance->GetVirtualID())
	{
		CPythonPlayer& rkPlayer = CPythonPlayer::Instance();
		//if (PointChange.Type == POINT_GOLD) {
		//	auto DoChanges = [=](const int64_t val, bool IsGrowing) {
		//		if (abs(PointChange.value - val) < 10) { //0-9
		//			//CPythonPlayer::Instance().SetStatus(PointChange.Type, PointChange.value);
		//		}
		//		else {
		//			int64_t lg = ceil(log10(abs(PointChange.value - val)) / 3.0f);
		//			int64_t diff = (PointChange.value - val) / pow(10, min(lg, 3));
		//			int64_t sleepPow = max(0, min(2, 3 - lg));
		//
		//			for (int64_t i = 0; i < pow(10, 3 - sleepPow); i++) {
		//				CPythonPlayer::Instance().SetStatus(PointChange.Type, val + (diff * i));
		//				std::this_thread::sleep_for(std::chrono::microseconds((int64_t)pow(10, sleepPow)));
		//				__RefreshStatus();
		//			}
		//		}
		//
		//		CPythonPlayer::Instance().SetStatus(PointChange.Type, PointChange.value);
		//	};
		//
		//	std::thread mythread(DoChanges, static_cast<int64_t>(CPythonPlayer::Instance().GetStatus(PointChange.Type)), CPythonPlayer::Instance().GetStatus(PointChange.Type) < PointChange.value);
		//	if (mythread.joinable())
		//		mythread.detach(); // do not use join()
		//	else
		//	{
		//		rkPlayer.SetStatus(PointChange.Type, PointChange.value);
		//	}
		//}
		//else
		rkPlayer.SetStatus(PointChange.Type, PointChange.value);

		switch (PointChange.Type)
		{
		case POINT_STAT_RESET_COUNT:
			__RefreshStatus();
			break;
		case POINT_LEVEL:
		case POINT_ST:
		case POINT_DX:
		case POINT_HT:
		case POINT_IQ:
			__RefreshStatus();
			__RefreshSkillWindow();
			break;
		case POINT_SKILL:
		case POINT_SUB_SKILL:
		case POINT_HORSE_SKILL:
			__RefreshSkillWindow();
			break;
		case POINT_ENERGY:
			if (PointChange.value == 0)
			{
				rkPlayer.SetStatus(POINT_ENERGY_END_TIME, 0);
			}
			__RefreshStatus();
			break;
		default:
			__RefreshStatus();
			break;
		}

		if (POINT_GOLD == PointChange.Type)
		{
			if (PointChange.amount > 0)
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnPickMoney", Py_BuildValue("(i)", PointChange.amount));
			}
		}
	}
#ifdef ENABLE_TEXT_LEVEL_REFRESH
	else
	{
		// the /advance command will provide no global refresh! it sends the pointchange only to the specific player and not all
		pInstance = CPythonCharacterManager::Instance().GetInstancePtr(PointChange.dwVID);
		if (pInstance && PointChange.Type == POINT_LEVEL)
		{
			pInstance->SetLevel(PointChange.value);
			pInstance->UpdateTextTailLevel(PointChange.value);
		}
	}
#endif

	return true;
}

bool CPythonNetworkStream::RecvStunPacket()
{
	TPacketGCStun StunPacket;

	if (!Recv(sizeof(StunPacket), &StunPacket))
	{
		Tracen("CPythonNetworkStream::RecvStunPacket Error");
		return false;
	}

	//Tracef("RecvStunPacket %d\n", StunPacket.vid);

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase * pkInstSel = rkChrMgr.GetInstancePtr(StunPacket.vid);

	if (pkInstSel)
	{
		if (CPythonCharacterManager::Instance().GetMainInstancePtr() == pkInstSel)
		{
			pkInstSel->Die();
		}
		else
		{
			pkInstSel->Stun();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvDeadPacket()
{
	TPacketGCDead DeadPacket;
	if (!Recv(sizeof(DeadPacket), &DeadPacket))
	{
		Tracen("CPythonNetworkStream::RecvDeadPacket Error");
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase * pkChrInstSel = rkChrMgr.GetInstancePtr(DeadPacket.vid);
	if (pkChrInstSel)
	{
		CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
		if (pkInstMain == pkChrInstSel)
		{
			Tracenf("On MainActor");
			if (false == pkInstMain->GetDuelMode())
			{
#ifndef ENABLE_RENEWAL_DEAD_PACKET
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("()"));
#else
				PyObject* times = PyTuple_New(REVIVE_TYPE_MAX);
				for (int i = REVIVE_TYPE_HERE; i < REVIVE_TYPE_MAX; i++)
				{
					PyTuple_SetItem(times, i, PyInt_FromLong(DeadPacket.t_d[i]));
				}
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnGameOver", Py_BuildValue("(O)", times));
#endif
			}
			CPythonPlayer::Instance().NotifyDeadMainCharacter();
#if defined(SKILL_COOLTIME_UPDATE)
			CPythonPlayer::Instance().ResetSkillCoolTimes();
#endif
		}

		pkChrInstSel->Die();
	}

	return true;
}

bool CPythonNetworkStream::SendCharacterPositionPacket(BYTE iPosition)
{
	TPacketCGPosition PositionPacket;

	PositionPacket.header = HEADER_CG_CHARACTER_POSITION;
	PositionPacket.position = iPosition;

	if (!Send(sizeof(TPacketCGPosition), &PositionPacket))
	{
		Tracen("Send Character Position Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendOnClickPacket(DWORD vid)
{
	TPacketCGOnClick OnClickPacket;
	OnClickPacket.header	= HEADER_CG_ON_CLICK;
	OnClickPacket.vid		= vid;

	if (!Send(sizeof(OnClickPacket), &OnClickPacket))
	{
		Tracen("Send On_Click Packet Error");
		return false;
	}

	Tracef("SendOnClickPacket\n");
	return SendSequence();
}

bool CPythonNetworkStream::RecvCharacterPositionPacket()
{
	TPacketGCPosition PositionPacket;

	if (!Recv(sizeof(TPacketGCPosition), &PositionPacket))
	{
		return false;
	}

	CInstanceBase * pChrInstance = CPythonCharacterManager::Instance().GetInstancePtr(PositionPacket.vid);

	if (!pChrInstance)
	{
		return true;
	}

	//pChrInstance->UpdatePosition(PositionPacket.position);

	return true;
}

bool CPythonNetworkStream::RecvMotionPacket()
{
	TPacketGCMotion MotionPacket;

	if (!Recv(sizeof(TPacketGCMotion), &MotionPacket))
	{
		return false;
	}

	CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.vid);
	CInstanceBase * pVictimInstance = NULL;

	if (0 != MotionPacket.victim_vid)
	{
		pVictimInstance = CPythonCharacterManager::Instance().GetInstancePtr(MotionPacket.victim_vid);
	}

	if (!pMainInstance)
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvShopPacket()
{
	std::vector<char> vecBuffer;
	vecBuffer.clear();

	TPacketGCShop  packet_shop;
	if (!Recv(sizeof(packet_shop), &packet_shop))
	{
		return false;
	}

	int iSize = packet_shop.size - sizeof(packet_shop);
	if (iSize > 0)
	{
		vecBuffer.resize(iSize);
		if (!Recv(iSize, &vecBuffer[0]))
		{
			return false;
		}
	}

	switch (packet_shop.subheader)
	{
	case SHOP_SUBHEADER_GC_START:
	{
		CPythonShop::Instance().Clear();

		DWORD dwVID = *(DWORD *)&vecBuffer[0];

		TPacketGCShopStart * pShopStartPacket = (TPacketGCShopStart *)&vecBuffer[4];
		for (BYTE iItemIndex = 0; iItemIndex < SHOP_HOST_ITEM_MAX_NUM; ++iItemIndex)
		{
			CPythonShop::Instance().SetItemData(iItemIndex, pShopStartPacket->items[iItemIndex]);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
	}
	break;

	case SHOP_SUBHEADER_GC_START_EX:
	{
		CPythonShop::Instance().Clear();

		TPacketGCShopStartEx * pShopStartPacket = (TPacketGCShopStartEx *)&vecBuffer[0];
		size_t read_point = sizeof(TPacketGCShopStartEx);

		DWORD dwVID = pShopStartPacket->owner_vid;
		BYTE shop_tab_count = pShopStartPacket->shop_tab_count;

		CPythonShop::instance().SetTabCount(shop_tab_count);

		for (size_t i = 0; i < shop_tab_count; i++)
		{
			TPacketGCShopStartEx::TSubPacketShopTab* pPackTab = (TPacketGCShopStartEx::TSubPacketShopTab*)&vecBuffer[read_point];
			read_point += sizeof(TPacketGCShopStartEx::TSubPacketShopTab);

			CPythonShop::instance().SetTabCoinType(i, pPackTab->coin_type);
			CPythonShop::instance().SetTabName(i, pPackTab->name);

			struct packet_shop_item_ex* item = &pPackTab->items[0];

			for (BYTE j = 0; j < SHOP_HOST_ITEM_MAX_NUM; j++)
			{
				TShopItemExData* itemData = (item + j);
				CPythonShop::Instance().SetItemData(i, j, *itemData);
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartShop", Py_BuildValue("(i)", dwVID));
	}
	break;


	case SHOP_SUBHEADER_GC_END:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndShop", Py_BuildValue("()"));
		break;

	case SHOP_SUBHEADER_GC_UPDATE_ITEM:
	{
		TPacketGCShopUpdateItem * pShopUpdateItemPacket = (TPacketGCShopUpdateItem *)&vecBuffer[0];
		CPythonShop::Instance().SetItemData(pShopUpdateItemPacket->pos, pShopUpdateItemPacket->item);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshShop", Py_BuildValue("()"));
	}
	break;

	case SHOP_SUBHEADER_GC_UPDATE_PRICE:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetShopSellingPrice", Py_BuildValue("(L)", *(int64_t*)&vecBuffer[0]));
		break;

	case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY"));
		break;

	case SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_MONEY_EX"));
		break;

	case SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM_EX:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "NOT_ENOUGH_ITEM_EX"));
		break;

	case SHOP_SUBHEADER_GC_SOLDOUT:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "SOLDOUT"));
		break;

	case SHOP_SUBHEADER_GC_INVENTORY_FULL:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVENTORY_FULL"));
		break;

	case SHOP_SUBHEADER_GC_INVALID_POS:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnShopError", Py_BuildValue("(s)", "INVALID_POS"));
		break;

	default:
		TraceError("CPythonNetworkStream::RecvShopPacket: Unknown subheader\n");
		break;
	}

	return true;
}

bool CPythonNetworkStream::RecvExchangePacket()
{
	TPacketGCExchange exchange_packet;

	if (!Recv(sizeof(exchange_packet), &exchange_packet))
	{
		return false;
	}

	switch (exchange_packet.subheader)
	{
	case EXCHANGE_SUBHEADER_GC_START:
		CPythonExchange::Instance().Clear();
		CPythonExchange::Instance().Start();
		CPythonExchange::Instance().SetSelfName(CPythonPlayer::Instance().GetName());
#ifdef ENABLE_RENEWAL_EXCHANGE
		CPythonExchange::Instance().SetSelfRace(CPythonPlayer::Instance().GetRace());
		CPythonExchange::Instance().SetSelfLevel(CPythonPlayer::Instance().GetStatus(POINT_LEVEL));
#endif


		{
			CInstanceBase * pCharacterInstance = CPythonCharacterManager::Instance().GetInstancePtr(exchange_packet.arg1);

			if (pCharacterInstance)
			{
				CPythonExchange::Instance().SetTargetName(pCharacterInstance->GetNameString());
#ifdef ENABLE_RENEWAL_EXCHANGE
				CPythonExchange::Instance().SetTargetRace(pCharacterInstance->GetRace());
				CPythonExchange::Instance().SetTargetLevel(pCharacterInstance->GetLevel());
				CPythonExchange::Instance().SetTargetVID(pCharacterInstance->GetVirtualID());
#endif
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartExchange", Py_BuildValue("()"));
		break;

	case EXCHANGE_SUBHEADER_GC_ITEM_ADD:
		if (exchange_packet.is_me)
		{
			int iSlotIndex = exchange_packet.arg2.cell;
			CPythonExchange::Instance().SetItemToSelf(iSlotIndex, exchange_packet.arg1, exchange_packet.arg3);
			for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
			{
				CPythonExchange::Instance().SetItemMetinSocketToSelf(iSlotIndex, i, exchange_packet.alValues[i]);
			}
			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
			{
				CPythonExchange::Instance().SetItemAttributeToSelf(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);
			}

#ifdef TRANSMUTATION_SYSTEM
			CPythonExchange::Instance().SetItemTransmutateToSelf(iSlotIndex, exchange_packet.transmutation_id);
#endif

#ifdef ENABLE_REFINE_ELEMENT
			CPythonExchange::Instance().SetItemRefineElementToSelf(iSlotIndex, exchange_packet.dwRefineElement);
#endif
		}
		else
		{
			int iSlotIndex = exchange_packet.arg2.cell;
			CPythonExchange::Instance().SetItemToTarget(iSlotIndex, exchange_packet.arg1, exchange_packet.arg3);
			for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
			{
				CPythonExchange::Instance().SetItemMetinSocketToTarget(iSlotIndex, i, exchange_packet.alValues[i]);
			}
			for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
			{
				CPythonExchange::Instance().SetItemAttributeToTarget(iSlotIndex, j, exchange_packet.aAttr[j].bType, exchange_packet.aAttr[j].sValue);
			}

#ifdef TRANSMUTATION_SYSTEM
			CPythonExchange::Instance().SetItemTransmutateToTarget(iSlotIndex, exchange_packet.transmutation_id);
#endif

#ifdef ENABLE_REFINE_ELEMENT
			CPythonExchange::Instance().SetItemRefineElementToTarget(iSlotIndex, exchange_packet.dwRefineElement);
#endif
		}

		__RefreshExchangeWindow();
		__RefreshInventoryWindow();
		break;

	case EXCHANGE_SUBHEADER_GC_ITEM_DEL:
		if (exchange_packet.is_me)
		{
			CPythonExchange::Instance().DelItemOfSelf((BYTE)exchange_packet.arg1);
		}
		else
		{
			CPythonExchange::Instance().DelItemOfTarget((BYTE)exchange_packet.arg1);
		}
		__RefreshExchangeWindow();
		__RefreshInventoryWindow();
		break;

	case EXCHANGE_SUBHEADER_GC_ELK_ADD:
		if (exchange_packet.is_me)
		{
			CPythonExchange::Instance().SetElkToSelf(exchange_packet.arg1);
		}
		else
		{
			CPythonExchange::Instance().SetElkToTarget(exchange_packet.arg1);
		}

		__RefreshExchangeWindow();
		break;

	case EXCHANGE_SUBHEADER_GC_ACCEPT:
		if (exchange_packet.is_me)
		{
			CPythonExchange::Instance().SetAcceptToSelf((BYTE) exchange_packet.arg1);
		}
		else
		{
			CPythonExchange::Instance().SetAcceptToTarget((BYTE) exchange_packet.arg1);
		}
		__RefreshExchangeWindow();
		break;

	case EXCHANGE_SUBHEADER_GC_END:
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "EndExchange", Py_BuildValue("()"));
		__RefreshInventoryWindow();
		CPythonExchange::Instance().End();
		break;

	case EXCHANGE_SUBHEADER_GC_ALREADY:
		Tracef("trade_already");
		break;

	case EXCHANGE_SUBHEADER_GC_LESS_ELK:
		Tracef("trade_less_elk");
		break;
	};

	return true;
}

bool CPythonNetworkStream::RecvQuestInfoPacket()
{
	TPacketGCQuestInfo QuestInfo;

	if (!Peek(sizeof(TPacketGCQuestInfo), &QuestInfo))
	{
		Tracen("Recv Quest Info Packet Error #1");
		return false;
	}

	if (!Peek(QuestInfo.size))
	{
		Tracen("Recv Quest Info Packet Error #2");
		return false;
	}

	Recv(sizeof(TPacketGCQuestInfo));

	const BYTE & c_rFlag = QuestInfo.flag;

	enum
	{
		QUEST_PACKET_TYPE_NONE,
		QUEST_PACKET_TYPE_BEGIN,
		QUEST_PACKET_TYPE_UPDATE,
		QUEST_PACKET_TYPE_END,
	};

	BYTE byQuestPacketType = QUEST_PACKET_TYPE_NONE;

	if (0 != (c_rFlag & QUEST_SEND_IS_BEGIN))
	{
		BYTE isBegin;
		if (!Recv(sizeof(isBegin), &isBegin))
		{
			return false;
		}

		if (isBegin)
		{
			byQuestPacketType = QUEST_PACKET_TYPE_BEGIN;
		}
		else
		{
			byQuestPacketType = QUEST_PACKET_TYPE_END;
		}
	}
	else
	{
		byQuestPacketType = QUEST_PACKET_TYPE_UPDATE;
	}

	// Recv Data Start
	char szTitle[30 + 1] = "";
	char szClockName[16 + 1] = "";
	int iClockValue = 0;
	char szCounterName[16 + 1] = "";
	int iCounterValue = 0;
	char szIconFileName[24 + 1] = "";

	if (0 != (c_rFlag & QUEST_SEND_TITLE))
	{
		if (!Recv(sizeof(szTitle), &szTitle))
		{
			return false;
		}

		szTitle[30] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_NAME))
	{
		if (!Recv(sizeof(szClockName), &szClockName))
		{
			return false;
		}

		szClockName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_CLOCK_VALUE))
	{
		if (!Recv(sizeof(iClockValue), &iClockValue))
		{
			return false;
		}
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_NAME))
	{
		if (!Recv(sizeof(szCounterName), &szCounterName))
		{
			return false;
		}

		szCounterName[16] = '\0';
	}
	if (0 != (c_rFlag & QUEST_SEND_COUNTER_VALUE))
	{
		if (!Recv(sizeof(iCounterValue), &iCounterValue))
		{
			return false;
		}
	}
	if (0 != (c_rFlag & QUEST_SEND_ICON_FILE))
	{
		if (!Recv(sizeof(szIconFileName), &szIconFileName))
		{
			return false;
		}

		szIconFileName[24] = '\0';
	}
	// Recv Data End

	CPythonQuest& rkQuest = CPythonQuest::Instance();

	// Process Start
	if (QUEST_PACKET_TYPE_END == byQuestPacketType)
	{
		rkQuest.DeleteQuestInstance(QuestInfo.index);
	}
	else if (QUEST_PACKET_TYPE_UPDATE == byQuestPacketType)
	{
		if (!rkQuest.IsQuest(QuestInfo.index))
		{
#ifdef ENABLE_QUEST_RENEWAL
			rkQuest.MakeQuest(QuestInfo.index, QuestInfo.c_index);
#else
			rkQuest.MakeQuest(QuestInfo.index);
#endif
		}

		if (strlen(szTitle) > 0)
		{
			rkQuest.SetQuestTitle(QuestInfo.index, szTitle);
		}
		if (strlen(szClockName) > 0)
		{
			rkQuest.SetQuestClockName(QuestInfo.index, szClockName);
		}
		if (strlen(szCounterName) > 0)
		{
			rkQuest.SetQuestCounterName(QuestInfo.index, szCounterName);
		}
		if (strlen(szIconFileName) > 0)
		{
			rkQuest.SetQuestIconFileName(QuestInfo.index, szIconFileName);
		}

		if (c_rFlag & QUEST_SEND_CLOCK_VALUE)
		{
			rkQuest.SetQuestClockValue(QuestInfo.index, iClockValue);
		}
		if (c_rFlag & QUEST_SEND_COUNTER_VALUE)
		{
			rkQuest.SetQuestCounterValue(QuestInfo.index, iCounterValue);
		}
	}
	else if (QUEST_PACKET_TYPE_BEGIN == byQuestPacketType)
	{
		CPythonQuest::SQuestInstance QuestInstance;
		QuestInstance.dwIndex = QuestInfo.index;
		QuestInstance.strTitle = szTitle;
		QuestInstance.strClockName = szClockName;
		QuestInstance.iClockValue = iClockValue;
		QuestInstance.strCounterName = szCounterName;
		QuestInstance.iCounterValue = iCounterValue;
		QuestInstance.strIconFileName = szIconFileName;
		CPythonQuest::Instance().RegisterQuestInstance(QuestInstance);
	}
	// Process Start End

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshQuest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvQuestConfirmPacket()
{
	TPacketGCQuestConfirm kQuestConfirmPacket;
	if (!Recv(sizeof(kQuestConfirmPacket), &kQuestConfirmPacket))
	{
		Tracen("RecvQuestConfirmPacket Error");
		return false;
	}

	PyObject * poArg = Py_BuildValue("(sii)", kQuestConfirmPacket.msg, kQuestConfirmPacket.timeout, kQuestConfirmPacket.requestPID);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OnQuestConfirm", poArg);
	return true;
}

bool CPythonNetworkStream::RecvRequestMakeGuild()
{
	TPacketGCBlank blank;
	if (!Recv(sizeof(blank), &blank))
	{
		Tracen("RecvRequestMakeGuild Packet Error");
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AskGuildName", Py_BuildValue("()"));

	return true;
}

void CPythonNetworkStream::ToggleGameDebugInfo()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ToggleDebugInfo", Py_BuildValue("()"));
}

bool CPythonNetworkStream::SendExchangeStartPacket(DWORD vid)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_START;
	packet.arg1			= vid;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_start_packet Error\n");
		return false;
	}

	Tracef("send_trade_start_packet   vid %d \n", vid);
	return SendSequence();
}

#ifdef EXTANDED_GOLD_AMOUNT
	bool CPythonNetworkStream::SendExchangeElkAddPacket(int64_t elk)
#else
	bool CPythonNetworkStream::SendExchangeElkAddPacket(DWORD elk)
#endif
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ELK_ADD;
	packet.arg1			= elk;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_elk_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemAddPacket(TItemPos ItemPos, BYTE byDisplayPos)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_ADD;
	packet.Pos			= ItemPos;
	packet.arg2			= byDisplayPos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_add_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeItemDelPacket(BYTE pos)
{
	assert(!"Can't be called function - CPythonNetworkStream::SendExchangeItemDelPacket");
	return true;

	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ITEM_DEL;
	packet.arg1			= pos;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_item_del_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeAcceptPacket()
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_ACCEPT;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_accept_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendExchangeExitPacket()
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGExchange	packet;

	packet.header		= HEADER_CG_EXCHANGE;
	packet.subheader	= EXCHANGE_SUBHEADER_CG_CANCEL;

	if (!Send(sizeof(packet), &packet))
	{
		Tracef("send_trade_exit_packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendPointResetPacket()
{
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "StartPointReset", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__IsPlayerAttacking()
{
	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		return false;
	}

	if (!pkInstMain->IsAttacking())
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvScriptPacket()
{
	TPacketGCScript ScriptPacket;

	if (!Recv(sizeof(TPacketGCScript), &ScriptPacket))
	{
		TraceError("RecvScriptPacket_RecvError");
		return false;
	}

	if (ScriptPacket.size < sizeof(TPacketGCScript))
	{
		TraceError("RecvScriptPacket_SizeError");
		return false;
	}

	ScriptPacket.size -= sizeof(TPacketGCScript);

	static std::string str;
	str = "";
	str.resize(ScriptPacket.size + 1);

	if (!Recv(ScriptPacket.size, &str[0]))
	{
		return false;
	}

	str[str.size() - 1] = '\0';

	int iIndex = CPythonEventManager::Instance().RegisterEventSetFromString(str);

	if (-1 != iIndex)
	{
		CPythonEventManager::Instance().SetVisibleLineCount(iIndex, 30);
		CPythonNetworkStream::Instance().OnScriptEventStart(ScriptPacket.skin, iIndex);
	}

	return true;
}

bool CPythonNetworkStream::SendScriptAnswerPacket(int iAnswer)
{
	TPacketCGScriptAnswer ScriptAnswer;

	ScriptAnswer.header = HEADER_CG_SCRIPT_ANSWER;
	ScriptAnswer.answer = (BYTE) iAnswer;
	if (!Send(sizeof(TPacketCGScriptAnswer), &ScriptAnswer))
	{
		Tracen("Send Script Answer Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendScriptButtonPacket(unsigned int iIndex)
{
	TPacketCGScriptButton ScriptButton;

	ScriptButton.header = HEADER_CG_SCRIPT_BUTTON;
	ScriptButton.idx = iIndex;
	if (!Send(sizeof(TPacketCGScriptButton), &ScriptButton))
	{
		Tracen("Send Script Button Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAnswerMakeGuildPacket(const char * c_szName)
{
	TPacketCGAnswerMakeGuild Packet;

	Packet.header = HEADER_CG_ANSWER_MAKE_GUILD;
	strncpy(Packet.guild_name, c_szName, GUILD_NAME_MAX_LEN);
	Packet.guild_name[GUILD_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendAnswerMakeGuild Packet Error");
		return false;
	}

// 	Tracef(" SendAnswerMakeGuildPacket : %s", c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendQuestInputStringPacket(const char * c_szString)
{
	TPacketCGQuestInputString Packet;
	Packet.bHeader = HEADER_CG_QUEST_INPUT_STRING;
	strncpy(Packet.szString, c_szString, QUEST_INPUT_STRING_MAX_NUM);

	if (!Send(sizeof(Packet), &Packet))
	{
		Tracen("SendQuestInputStringPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendQuestConfirmPacket(BYTE byAnswer, DWORD dwPID)
{
	TPacketCGQuestConfirm kPacket;
	kPacket.header = HEADER_CG_QUEST_CONFIRM;
	kPacket.answer = byAnswer;
	kPacket.requestPID = dwPID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		Tracen("SendQuestConfirmPacket Error");
		return false;
	}

	Tracenf(" SendQuestConfirmPacket : %d, %d", byAnswer, dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::RecvSkillLevel()
{
	TPacketGCSkillLevel packet;

	if (!Recv(sizeof(TPacketGCSkillLevel), &packet))
	{
		Tracen("CPythonNetworkStream::RecvSkillLevelNew - RecvError");
		return false;
	}

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

	rkPlayer.SetSkill(7, 0);
	rkPlayer.SetSkill(8, 0);

	for (int i = 0; i < SKILL_MAX_NUM; ++i)
	{
		TPlayerSkill& rPlayerSkill = packet.skills[i];

		if (i >= 112 && i <= 115 && rPlayerSkill.bLevel)
		{
			rkPlayer.SetSkill(7, i);
		}

		if (i >= 116 && i <= 119 && rPlayerSkill.bLevel)
		{
			rkPlayer.SetSkill(8, i);
		}

		rkPlayer.SetSkillLevel_(i, rPlayerSkill.bMasterType, rPlayerSkill.bLevel);
	}

	__RefreshSkillWindow();
	__RefreshStatus();
	//Tracef(" >> RecvSkillLevelNew\n");
	return true;
}

bool CPythonNetworkStream::RecvDamageInfoPacket()
{
	TPacketGCDamageInfo DamageInfoPacket;

	if (!Recv(sizeof(TPacketGCDamageInfo), &DamageInfoPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(DamageInfoPacket.dwVID);
	bool bSelf = (pInstTarget == CPythonCharacterManager::Instance().GetMainInstancePtr());
	bool bTarget = (pInstTarget == m_pInstTarget);
	if (pInstTarget)
	{
		if (DamageInfoPacket.damage >= 0)
		{
			pInstTarget->AddDamageEffect(DamageInfoPacket.damage, DamageInfoPacket.flag, bSelf, bTarget);
		}
		else
		{
			TraceError("Damage is equal or below 0.");
		}
	}

	return true;
}
bool CPythonNetworkStream::RecvTargetPacket()
{
	TPacketGCTarget TargetPacket;

	if (!Recv(sizeof(TPacketGCTarget), &TargetPacket))
	{
		Tracen("Recv Target Packet Error");
		return false;
	}

	CInstanceBase * pInstPlayer = CPythonCharacterManager::Instance().GetMainInstancePtr();
	CInstanceBase * pInstTarget = CPythonCharacterManager::Instance().GetInstancePtr(TargetPacket.dwVID);
	if (pInstPlayer && pInstTarget)
	{
#ifdef ENABLE_OFFLINE_SHOP
		bool isOfflineShop = false;
		switch (pInstTarget->GetRace())
		{
		case 30000:
		case 30002:
		case 30003:
		case 30004:
		case 30005:
		case 30006:
		case 30007:
		case 30008:
			isOfflineShop = true;
			break;
		}
#endif
		if (!pInstTarget->IsDead())
		{
			if (pInstTarget->IsPC() || pInstTarget->IsBuilding())
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoardIfDifferent", Py_BuildValue("(i)", TargetPacket.dwVID));
			}
			else if (pInstPlayer->CanViewTargetHP(*pInstTarget))
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetHPTargetBoard", Py_BuildValue("(iiib)", TargetPacket.dwVID, TargetPacket.lHP, TargetPacket.lMaxHP, TargetPacket.isPoisoned));
			}
			else
			{
				PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
			}

			m_pInstTarget = pInstTarget;
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "CloseTargetBoard", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::RecvMountPacket()
{
	TPacketGCMount MountPacket;

	if (!Recv(sizeof(TPacketGCMount), &MountPacket))
	{
		Tracen("Recv Mount Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(MountPacket.vid);

	if (pInstance)
	{
		// Mount
		if (0 != MountPacket.mount_vid)
		{
//			pInstance->Ride(MountPacket.pos, MountPacket.mount_vid);
		}
		// Unmount
		else
		{
//			pInstance->Unride(MountPacket.pos, MountPacket.x, MountPacket.y);
		}
	}

	if (CPythonPlayer::Instance().IsMainCharacterIndex(MountPacket.vid))
	{
//		CPythonPlayer::Instance().SetRidingVehicleIndex(MountPacket.mount_vid);
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSpeedPacket()
{
	TPacketGCChangeSpeed SpeedPacket;

	if (!Recv(sizeof(TPacketGCChangeSpeed), &SpeedPacket))
	{
		Tracen("Recv Speed Packet Error");
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(SpeedPacket.vid);

	if (!pInstance)
	{
		return true;
	}

//	pInstance->SetWalkSpeed(SpeedPacket.walking_speed);
//	pInstance->SetRunSpeed(SpeedPacket.running_speed);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Recv

bool CPythonNetworkStream::SendAttackPacket(UINT uMotAttack, DWORD dwVIDVictim)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

#ifdef ATTACK_TIME_LOG
	static DWORD prevTime = timeGetTime();
	DWORD curTime = timeGetTime();
	TraceError("TIME: %.4f(%.4f) ATTACK_PACKET: %d TARGET: %d", curTime / 1000.0f, (curTime - prevTime) / 1000.0f, uMotAttack, dwVIDVictim);
	prevTime = curTime;
#endif

	TPacketCGAttack kPacketAtk;

	kPacketAtk.header = HEADER_CG_ATTACK;
	kPacketAtk.bType = uMotAttack;
	kPacketAtk.dwVictimVID = dwVIDVictim;

	if (!SendSpecial(sizeof(kPacketAtk), &kPacketAtk))
	{
		Tracen("Send Battle Attack Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSpecial(int nLen, void * pvBuf)
{
	BYTE bHeader = *(BYTE *) pvBuf;

	switch (bHeader)
	{
	case HEADER_CG_ATTACK:
	{
		TPacketCGAttack * pkPacketAtk = (TPacketCGAttack *) pvBuf;
		pkPacketAtk->bCRCMagicCubeProcPiece = GetProcessCRCMagicCubePiece();
		pkPacketAtk->bCRCMagicCubeFilePiece = GetProcessCRCMagicCubePiece();
		return Send(nLen, pvBuf);
	}
	break;
	}

	return Send(nLen, pvBuf);
}

bool CPythonNetworkStream::RecvAddFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
	{
		return false;
	}

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	Tracef("VID [%d] Added to target settings\n", kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifndef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->AddFlyTarget(D3DXVECTOR3(kPacket.lX, kPacket.lY, h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::RecvFlyTargetingPacket()
{
	TPacketGCFlyTargeting kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
	{
		return false;
	}

	__GlobalPositionToLocalPosition(kPacket.lX, kPacket.lY);

	//Tracef("CPythonNetworkStream::RecvFlyTargetingPacket - VID [%d]\n",kPacket.dwShooterVID);

	CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	CInstanceBase * pShooter = rpcm.GetInstancePtr(kPacket.dwShooterVID);

	if (!pShooter)
	{
#ifdef _DEBUG
		TraceError("CPythonNetworkStream::RecvFlyTargetingPacket() - dwShooterVID[%d] NOT EXIST", kPacket.dwShooterVID);
#endif
		return true;
	}

	CInstanceBase * pTarget = rpcm.GetInstancePtr(kPacket.dwTargetVID);

	if (kPacket.dwTargetVID && pTarget)
	{
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(pTarget->GetGraphicThingInstancePtr());
	}
	else
	{
		float h = CPythonBackground::Instance().GetHeight(kPacket.lX, kPacket.lY) + 60.0f; // TEMPORARY HEIGHT
		pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(D3DXVECTOR3(kPacket.lX, kPacket.lY, h));
		//pShooter->GetGraphicThingInstancePtr()->SetFlyTarget(kPacket.kPPosTarget.x,kPacket.kPPosTarget.y,);
	}

	return true;
}

bool CPythonNetworkStream::SendShootPacket(UINT uSkill)
{
	TPacketCGShoot kPacketShoot;
	kPacketShoot.bHeader = HEADER_CG_SHOOT;
	kPacketShoot.bType = uSkill;

	if (!Send(sizeof(kPacketShoot), &kPacketShoot))
	{
		Tracen("SendShootPacket Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendAddFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_ADD_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}


bool CPythonNetworkStream::SendFlyTargetingPacket(DWORD dwTargetVID, const TPixelPosition & kPPosTarget)
{
	TPacketCGFlyTargeting packet;

	//CPythonCharacterManager & rpcm = CPythonCharacterManager::Instance();

	packet.bHeader	= HEADER_CG_FLY_TARGETING;
	packet.dwTargetVID = dwTargetVID;
	packet.lX = kPPosTarget.x;
	packet.lY = kPPosTarget.y;

	__LocalPositionToGlobalPosition(packet.lX, packet.lY);

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send FlyTargeting Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvCreateFlyPacket()
{
	TPacketGCCreateFly kPacket;
	if (!Recv(sizeof(TPacketGCCreateFly), &kPacket))
	{
		return false;
	}

	CFlyingManager& rkFlyMgr = CFlyingManager::Instance();
	CPythonCharacterManager & rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase * pkStartInst = rkChrMgr.GetInstancePtr(kPacket.dwStartVID);
	CInstanceBase * pkEndInst = rkChrMgr.GetInstancePtr(kPacket.dwEndVID);
	if (!pkStartInst || !pkEndInst)
	{
		return true;
	}

	rkFlyMgr.CreateIndexedFly(kPacket.bType, pkStartInst->GetGraphicThingInstancePtr(), pkEndInst->GetGraphicThingInstancePtr());

	return true;
}

bool CPythonNetworkStream::SendTargetPacket(DWORD dwVID)
{
	TPacketCGTarget packet;
	packet.header = HEADER_CG_TARGET;
	packet.dwVID = dwVID;

	if (!Send(sizeof(packet), &packet))
	{
		Tracen("Send Target Packet Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSyncPositionElementPacket(DWORD dwVictimVID, DWORD dwVictimX, DWORD dwVictimY)
{
	TPacketCGSyncPositionElement kSyncPos;
	kSyncPos.dwVID = dwVictimVID;
	kSyncPos.lX = dwVictimX;
	kSyncPos.lY = dwVictimY;

	__LocalPositionToGlobalPosition(kSyncPos.lX, kSyncPos.lY);

	if (!Send(sizeof(kSyncPos), &kSyncPos))
	{
		Tracen("CPythonNetworkStream::SendSyncPositionElementPacket - ERROR");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvMessenger()
{
	TPacketGCMessenger p;
	if (!Recv(sizeof(p), &p))
	{
		return false;
	}

	int iSize = p.size - sizeof(p);
	char char_name[24 + 1];

	switch (p.subheader)
	{
	case MESSENGER_SUBHEADER_GC_LIST:
	{
		TPacketGCMessengerListOnline on;
		while (iSize)
		{
			if (!Recv(sizeof(TPacketGCMessengerListOffline), &on))
			{
				return false;
			}

			if (!Recv(on.length, char_name))
			{
				return false;
			}

			char_name[on.length] = 0;

			if (on.connected & MESSENGER_CONNECTED_STATE_ONLINE)
			{
				CPythonMessenger::Instance().OnFriendLogin(char_name);
			}
			else
			{
				CPythonMessenger::Instance().OnFriendLogout(char_name);
			}

			if (on.connected & MESSENGER_CONNECTED_STATE_MOBILE)
			{
				CPythonMessenger::Instance().SetMobile(char_name, TRUE);
			}

			iSize -= sizeof(TPacketGCMessengerListOffline);
			iSize -= on.length;
		}
		break;
	}

	case MESSENGER_SUBHEADER_GC_LOGIN:
	{
		TPacketGCMessengerLogin p;
		if (!Recv(sizeof(p), &p))
		{
			return false;
		}
		if (!Recv(p.length, char_name))
		{
			return false;
		}
		char_name[p.length] = 0;
		CPythonMessenger::Instance().OnFriendLogin(char_name);
		__RefreshTargetBoardByName(char_name);
		break;
	}

	case MESSENGER_SUBHEADER_GC_LOGOUT:
	{
		TPacketGCMessengerLogout logout;
		if (!Recv(sizeof(logout), &logout))
		{
			return false;
		}
		if (!Recv(logout.length, char_name))
		{
			return false;
		}
		char_name[logout.length] = 0;
		CPythonMessenger::Instance().OnFriendLogout(char_name);
		break;
	}

	case MESSENGER_SUBHEADER_GC_MOBILE:
	{
		BYTE byState;
		BYTE byLength;
		if (!Recv(sizeof(byState), &byState))
		{
			return false;
		}
		if (!Recv(sizeof(byLength), &byLength))
		{
			return false;
		}
		if (!Recv(byLength, char_name))
		{
			return false;
		}
		char_name[byLength] = 0;
		CPythonMessenger::Instance().SetMobile(char_name, byState);
		break;
	}
	case MESSENGER_SUBHEADER_GC_REMOVE_FRIEND:
	{
		BYTE bLength;
		if (!Recv(sizeof(bLength), &bLength))
		{
			return false;
		}

		if (!Recv(bLength, char_name))
		{
			return false;
		}

		char_name[bLength] = 0;
		CPythonMessenger::Instance().RemoveFriend(char_name);
		break;
	}
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Party

bool CPythonNetworkStream::SendPartyInvitePacket(DWORD dwVID)
{
	TPacketCGPartyInvite kPartyInvitePacket;
	kPartyInvitePacket.header = HEADER_CG_PARTY_INVITE;
	kPartyInvitePacket.vid = dwVID;

	if (!Send(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInvitePacket [%ud] - PACKET SEND ERROR", dwVID);
		return false;
	}

	Tracef(" << SendPartyInvitePacket : %d\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyInviteAnswerPacket(DWORD dwLeaderVID, BYTE byAnswer)
{
	TPacketCGPartyInviteAnswer kPartyInviteAnswerPacket;
	kPartyInviteAnswerPacket.header = HEADER_CG_PARTY_INVITE_ANSWER;
	kPartyInviteAnswerPacket.leader_pid = dwLeaderVID;
	kPartyInviteAnswerPacket.accept = byAnswer;

	if (!Send(sizeof(kPartyInviteAnswerPacket), &kPartyInviteAnswerPacket))
	{
		Tracenf("CPythonNetworkStream::SendPartyInviteAnswerPacket [%ud %ud] - PACKET SEND ERROR", dwLeaderVID, byAnswer);
		return false;
	}

	Tracef(" << SendPartyInviteAnswerPacket : %d, %d\n", dwLeaderVID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyRemovePacket(DWORD dwPID)
{
	TPacketCGPartyRemove kPartyInviteRemove;
	kPartyInviteRemove.header = HEADER_CG_PARTY_REMOVE;
	kPartyInviteRemove.pid = dwPID;

	if (!Send(sizeof(kPartyInviteRemove), &kPartyInviteRemove))
	{
		Tracenf("CPythonNetworkStream::SendPartyRemovePacket [%ud] - PACKET SEND ERROR", dwPID);
		return false;
	}

	Tracef(" << SendPartyRemovePacket : %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartySetStatePacket(DWORD dwVID, BYTE byState, BYTE byFlag)
{
	TPacketCGPartySetState kPartySetState;
	kPartySetState.byHeader = HEADER_CG_PARTY_SET_STATE;
	kPartySetState.dwVID = dwVID;
	kPartySetState.byState = byState;
	kPartySetState.byFlag = byFlag;

	if (!Send(sizeof(kPartySetState), &kPartySetState))
	{
		Tracenf("CPythonNetworkStream::SendPartySetStatePacket(%ud, %ud) - PACKET SEND ERROR", dwVID, byState);
		return false;
	}

	Tracef(" << SendPartySetStatePacket : %d, %d, %d\n", dwVID, byState, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyUseSkillPacket(BYTE bySkillIndex, DWORD dwVID)
{
	TPacketCGPartyUseSkill kPartyUseSkill;
	kPartyUseSkill.byHeader = HEADER_CG_PARTY_USE_SKILL;
	kPartyUseSkill.bySkillIndex = bySkillIndex;
	kPartyUseSkill.dwTargetVID = dwVID;

	if (!Send(sizeof(kPartyUseSkill), &kPartyUseSkill))
	{
		Tracenf("CPythonNetworkStream::SendPartyUseSkillPacket(%ud, %ud) - PACKET SEND ERROR", bySkillIndex, dwVID);
		return false;
	}

	Tracef(" << SendPartyUseSkillPacket : %d, %d\n", bySkillIndex, dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendPartyParameterPacket(BYTE byDistributeMode)
{
	TPacketCGPartyParameter kPartyParameter;
	kPartyParameter.bHeader = HEADER_CG_PARTY_PARAMETER;
	kPartyParameter.bDistributeMode = byDistributeMode;

	if (!Send(sizeof(kPartyParameter), &kPartyParameter))
	{
		Tracenf("CPythonNetworkStream::SendPartyParameterPacket(%d) - PACKET SEND ERROR", byDistributeMode);
		return false;
	}

	Tracef(" << SendPartyParameterPacket : %d\n", byDistributeMode);
	return SendSequence();
}

bool CPythonNetworkStream::RecvPartyInvite()
{
	TPacketGCPartyInvite kPartyInvitePacket;
	if (!Recv(sizeof(kPartyInvitePacket), &kPartyInvitePacket))
	{
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(kPartyInvitePacket.leader_pid);
	if (!pInstance)
	{
		TraceError(" CPythonNetworkStream::RecvPartyInvite - Failed to find leader instance [%d]\n", kPartyInvitePacket.leader_pid);
		return true;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvPartyInviteQuestion", Py_BuildValue("(is)", kPartyInvitePacket.leader_pid, pInstance->GetNameString()));
	Tracef(" >> RecvPartyInvite : %d, %s\n", kPartyInvitePacket.leader_pid, pInstance->GetNameString());

	return true;
}

bool CPythonNetworkStream::RecvPartyAdd()
{
	TPacketGCPartyAdd kPartyAddPacket;
	if (!Recv(sizeof(kPartyAddPacket), &kPartyAddPacket))
	{
		return false;
	}

	CPythonPlayer::Instance().AppendPartyMember(kPartyAddPacket.pid, kPartyAddPacket.name);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AddPartyMember", Py_BuildValue("(is)", kPartyAddPacket.pid, kPartyAddPacket.name));
	Tracef(" >> RecvPartyAdd : %d, %s\n", kPartyAddPacket.pid, kPartyAddPacket.name);

	return true;
}

bool CPythonNetworkStream::RecvPartyUpdate()
{
	TPacketGCPartyUpdate kPartyUpdatePacket;
	if (!Recv(sizeof(kPartyUpdatePacket), &kPartyUpdatePacket))
	{
		return false;
	}

	CPythonPlayer::TPartyMemberInfo * pPartyMemberInfo;
	if (!CPythonPlayer::Instance().GetPartyMemberPtr(kPartyUpdatePacket.pid, &pPartyMemberInfo))
	{
		return true;
	}

	BYTE byOldState = pPartyMemberInfo->byState;

	CPythonPlayer::Instance().UpdatePartyMemberInfo(kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp
#ifdef ENABLE_PARTY_UPDATE
	, kPartyUpdatePacket.bLeader
#endif
	);
	for (int i = 0; i < PARTY_AFFECT_SLOT_MAX_NUM; ++i)
	{
		CPythonPlayer::Instance().UpdatePartyMemberAffect(kPartyUpdatePacket.pid, i, kPartyUpdatePacket.affects[i]);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UpdatePartyMemberInfo", Py_BuildValue("(i)", kPartyUpdatePacket.pid));

	DWORD dwVID;
	if (CPythonPlayer::Instance().PartyMemberPIDToVID(kPartyUpdatePacket.pid, &dwVID))
		if (byOldState != kPartyUpdatePacket.state)
		{
			__RefreshTargetBoardByVID(dwVID);
		}

// 	Tracef(" >> RecvPartyUpdate : %d, %d, %d\n", kPartyUpdatePacket.pid, kPartyUpdatePacket.state, kPartyUpdatePacket.percent_hp);

	return true;
}

#ifdef ENABLE_PARTY_ATLAS
bool CPythonNetworkStream::RecvPartyPosition()
{
	TPacketGCPartyPosition packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	if (packet.dwX != 0 || packet.dwY != 0)
	{
		CPythonMiniMap::Instance().UpdatePartyMember(packet.dwPID, packet.dwX, packet.dwY);
	}
	else
	{
		CPythonMiniMap::Instance().RemovePartyMember(packet.dwPID);
	}

	return true;
}

#endif

bool CPythonNetworkStream::RecvPartyRemove()
{
	TPacketGCPartyRemove kPartyRemovePacket;
	if (!Recv(sizeof(kPartyRemovePacket), &kPartyRemovePacket))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RemovePartyMember", Py_BuildValue("(i)", kPartyRemovePacket.pid));
	Tracef(" >> RecvPartyRemove : %d\n", kPartyRemovePacket.pid);

	return true;
}

bool CPythonNetworkStream::RecvPartyLink()
{
	TPacketGCPartyLink kPartyLinkPacket;
	if (!Recv(sizeof(kPartyLinkPacket), &kPartyLinkPacket))
	{
		return false;
	}

	CPythonPlayer::Instance().LinkPartyMember(kPartyLinkPacket.pid, kPartyLinkPacket.vid);
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "LinkPartyMember", Py_BuildValue("(ii)", kPartyLinkPacket.pid, kPartyLinkPacket.vid));
	Tracef(" >> RecvPartyLink : %d, %d\n", kPartyLinkPacket.pid, kPartyLinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyUnlink()
{
	TPacketGCPartyUnlink kPartyUnlinkPacket;
	if (!Recv(sizeof(kPartyUnlinkPacket), &kPartyUnlinkPacket))
	{
		return false;
	}

	CPythonPlayer::Instance().UnlinkPartyMember(kPartyUnlinkPacket.pid);

	if (CPythonPlayer::Instance().IsMainCharacterIndex(kPartyUnlinkPacket.vid))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkAllPartyMember", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "UnlinkPartyMember", Py_BuildValue("(i)", kPartyUnlinkPacket.pid));
	}

	Tracef(" >> RecvPartyUnlink : %d, %d\n", kPartyUnlinkPacket.pid, kPartyUnlinkPacket.vid);

	return true;
}

bool CPythonNetworkStream::RecvPartyParameter()
{
	TPacketGCPartyParameter kPartyParameterPacket;
	if (!Recv(sizeof(kPartyParameterPacket), &kPartyParameterPacket))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "ChangePartyParameter", Py_BuildValue("(i)", kPartyParameterPacket.bDistributeMode));
	Tracef(" >> RecvPartyParameter : %d\n", kPartyParameterPacket.bDistributeMode);

	return true;
}

// Party
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// Guild

bool CPythonNetworkStream::SendGuildAddMemberPacket(DWORD dwVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_ADD_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(dwVID), &dwVID))
	{
		return false;
	}

	Tracef(" SendGuildAddMemberPacket\n", dwVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRemoveMemberPacket(DWORD dwPID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REMOVE_MEMBER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(dwPID), &dwPID))
	{
		return false;
	}

	Tracef(" SendGuildRemoveMemberPacket %d\n", dwPID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeNamePacket(BYTE byGradeNumber, const char * c_szName)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
	{
		return false;
	}

	char szName[GUILD_GRADE_NAME_MAX_LEN + 1];
	strncpy(szName, c_szName, GUILD_GRADE_NAME_MAX_LEN);
	szName[GUILD_GRADE_NAME_MAX_LEN] = '\0';

	if (!Send(sizeof(szName), &szName))
	{
		return false;
	}

	Tracef(" SendGuildChangeGradeNamePacket %d, %s\n", byGradeNumber, c_szName);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeGradeAuthorityPacket(BYTE byGradeNumber, BYTE byAuthority)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(byGradeNumber), &byGradeNumber))
	{
		return false;
	}
	if (!Send(sizeof(byAuthority), &byAuthority))
	{
		return false;
	}

	Tracef(" SendGuildChangeGradeAuthorityPacket %d, %d\n", byGradeNumber, byAuthority);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildOfferPacket(DWORD dwExperience)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_OFFER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(dwExperience), &dwExperience))
	{
		return false;
	}

	Tracef(" SendGuildOfferPacket %d\n", dwExperience);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildPostCommentPacket(const char * c_szMessage)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_POST_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	BYTE bySize = BYTE(strlen(c_szMessage)) + 1;
	if (!Send(sizeof(bySize), &bySize))
	{
		return false;
	}
	if (!Send(bySize, c_szMessage))
	{
		return false;
	}

	Tracef(" SendGuildPostCommentPacket %d, %s\n", bySize, c_szMessage);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDeleteCommentPacket(DWORD dwIndex)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DELETE_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwIndex), &dwIndex))
	{
		return false;
	}

	Tracef(" SendGuildDeleteCommentPacket %d\n", dwIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildRefreshCommentsPacket(DWORD dwHighestIndex)
{
	static DWORD s_LastTime = timeGetTime() - 1001;

	if (timeGetTime() - s_LastTime < 1000)
	{
		return true;
	}
	s_LastTime = timeGetTime();

	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_REFRESH_COMMENT;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	Tracef(" SendGuildRefreshCommentPacket %d\n", dwHighestIndex);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGradePacket(DWORD dwPID, BYTE byGrade)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwPID), &dwPID))
	{
		return false;
	}
	if (!Send(sizeof(byGrade), &byGrade))
	{
		return false;
	}

	Tracef(" SendGuildChangeMemberGradePacket %d, %d\n", dwPID, byGrade);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildUseSkillPacket(DWORD dwSkillID, DWORD dwTargetVID)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_USE_SKILL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwSkillID), &dwSkillID))
	{
		return false;
	}
	if (!Send(sizeof(dwTargetVID), &dwTargetVID))
	{
		return false;
	}

	Tracef(" SendGuildUseSkillPacket %d, %d\n", dwSkillID, dwTargetVID);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChangeMemberGeneralPacket(DWORD dwPID, BYTE byFlag)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwPID), &dwPID))
	{
		return false;
	}
	if (!Send(sizeof(byFlag), &byFlag))
	{
		return false;
	}

	Tracef(" SendGuildChangeMemberGeneralFlagPacket %d, %d\n", dwPID, byFlag);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildInviteAnswerPacket(DWORD dwGuildID, BYTE byAnswer)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwGuildID), &dwGuildID))
	{
		return false;
	}
	if (!Send(sizeof(byAnswer), &byAnswer))
	{
		return false;
	}

	Tracef(" SendGuildInviteAnswerPacket %d, %d\n", dwGuildID, byAnswer);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildChargeGSPPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_CHARGE_GSP;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	if (!Send(sizeof(dwMoney), &dwMoney))
	{
		return false;
	}

	Tracef(" SendGuildChargeGSPPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildDepositMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_DEPOSIT_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(dwMoney), &dwMoney))
	{
		return false;
	}

	Tracef(" SendGuildDepositMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::SendGuildWithdrawMoneyPacket(DWORD dwMoney)
{
	TPacketCGGuild GuildPacket;
	GuildPacket.byHeader = HEADER_CG_GUILD;
	GuildPacket.bySubHeader = GUILD_SUBHEADER_CG_WITHDRAW_MONEY;
	if (!Send(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}
	if (!Send(sizeof(dwMoney), &dwMoney))
	{
		return false;
	}

	Tracef(" SendGuildWithdrawMoneyPacket %d\n", dwMoney);
	return SendSequence();
}

bool CPythonNetworkStream::RecvGuild()
{
	TPacketGCGuild GuildPacket;
	if (!Recv(sizeof(GuildPacket), &GuildPacket))
	{
		return false;
	}

	switch (GuildPacket.subheader)
	{
	case GUILD_SUBHEADER_GC_LOGIN:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		// Messenger
		CPythonGuild::TGuildMemberData * pGuildMemberData;
		if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
			if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
			{
				CPythonMessenger::Instance().LoginGuildMember(pGuildMemberData->strName.c_str());
			}

		//Tracef(" <Login> %d\n", dwPID);
		break;
	}
	case GUILD_SUBHEADER_GC_LOGOUT:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		// Messenger
		CPythonGuild::TGuildMemberData * pGuildMemberData;
		if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pGuildMemberData))
			if (0 != pGuildMemberData->strName.compare(CPythonPlayer::Instance().GetName()))
			{
				CPythonMessenger::Instance().LogoutGuildMember(pGuildMemberData->strName.c_str());
			}

		//Tracef(" <Logout> %d\n", dwPID);
		break;
	}
	case GUILD_SUBHEADER_GC_REMOVE:
	{
		DWORD dwPID;
		if (!Recv(sizeof(dwPID), &dwPID))
		{
			return false;
		}

		if (CPythonGuild::Instance().IsMainPlayer(dwPID))
		{
			CPythonGuild::Instance().Destroy();
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "DeleteGuild", Py_BuildValue("()"));
			CPythonMessenger::Instance().RemoveAllGuildMember();
			__SetGuildID(0);
			__RefreshMessengerWindow();
			__RefreshTargetBoard();
			__RefreshCharacterWindow();
		}
		else
		{
			// Get Member Name
			std::string strMemberName = "";
			CPythonGuild::TGuildMemberData * pData;
			if (CPythonGuild::Instance().GetMemberDataPtrByPID(dwPID, &pData))
			{
				strMemberName = pData->strName;
				CPythonMessenger::Instance().RemoveGuildMember(pData->strName.c_str());
			}

			CPythonGuild::Instance().RemoveMember(dwPID);

			// Refresh
			__RefreshTargetBoardByName(strMemberName.c_str());
			__RefreshGuildWindowMemberPage();
		}

		Tracef(" <Remove> %d\n", dwPID);
		break;
	}
	case GUILD_SUBHEADER_GC_LIST:
	{
		int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

		for (; iPacketSize > 0;)
		{
			TPacketGCGuildSubMember memberPacket;
			if (!Recv(sizeof(memberPacket), &memberPacket))
			{
				return false;
			}

			char szName[CHARACTER_NAME_MAX_LEN + 1] = "";
			if (memberPacket.byNameFlag)
			{
				if (!Recv(sizeof(szName), &szName))
				{
					return false;
				}

				iPacketSize -= CHARACTER_NAME_MAX_LEN + 1;
			}
			else
			{
				CPythonGuild::TGuildMemberData * pMemberData;
				if (CPythonGuild::Instance().GetMemberDataPtrByPID(memberPacket.pid, &pMemberData))
				{
					strncpy(szName, pMemberData->strName.c_str(), CHARACTER_NAME_MAX_LEN);
				}
			}

			//Tracef(" <List> %d : %s, %d (%d, %d, %d)\n", memberPacket.pid, szName, memberPacket.byGrade, memberPacket.byJob, memberPacket.byLevel, memberPacket.dwOffer);

			CPythonGuild::SGuildMemberData GuildMemberData;
			GuildMemberData.dwPID = memberPacket.pid;
			GuildMemberData.byGrade = memberPacket.byGrade;
			GuildMemberData.strName = szName;
			GuildMemberData.byJob = memberPacket.byJob;
			GuildMemberData.byLevel = memberPacket.byLevel;
			GuildMemberData.dwOffer = memberPacket.dwOffer;
			GuildMemberData.byGeneralFlag = memberPacket.byIsGeneral;
			CPythonGuild::Instance().RegisterMember(GuildMemberData);

			// Messenger
			if (strcmp(szName, CPythonPlayer::Instance().GetName()))
			{
				CPythonMessenger::Instance().AppendGuildMember(szName);
			}

			__RefreshTargetBoardByName(szName);

			iPacketSize -= sizeof(memberPacket);
		}

		__RefreshGuildWindowInfoPage();
		__RefreshGuildWindowMemberPage();
		__RefreshMessengerWindow();
		__RefreshCharacterWindow();
		break;
	}
	case GUILD_SUBHEADER_GC_GRADE:
	{
		BYTE byCount;
		if (!Recv(sizeof(byCount), &byCount))
		{
			return false;
		}

		for (BYTE i = 0; i < byCount; ++ i)
		{
			BYTE byIndex;
			if (!Recv(sizeof(byCount), &byIndex))
			{
				return false;
			}
			TPacketGCGuildSubGrade GradePacket;
			if (!Recv(sizeof(GradePacket), &GradePacket))
			{
				return false;
			}

			CPythonGuild::Instance().SetGradeData(byIndex, CPythonGuild::SGuildGradeData(GradePacket.auth_flag, GradePacket.grade_name));
			//Tracef(" <Grade> [%d/%d] : %s, %d\n", byIndex, byCount, GradePacket.grade_name, GradePacket.auth_flag);
		}
		__RefreshGuildWindowGradePage();
		__RefreshGuildWindowMemberPageGradeComboBox();
		break;
	}
	case GUILD_SUBHEADER_GC_GRADE_NAME:
	{
		BYTE byGradeNumber;
		if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
		{
			return false;
		}

		char szGradeName[GUILD_GRADE_NAME_MAX_LEN + 1] = "";
		if (!Recv(sizeof(szGradeName), &szGradeName))
		{
			return false;
		}

		CPythonGuild::Instance().SetGradeName(byGradeNumber, szGradeName);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

		Tracef(" <Change Grade Name> %d, %s\n", byGradeNumber, szGradeName);
		__RefreshGuildWindowGradePage();
		__RefreshGuildWindowMemberPageGradeComboBox();
		break;
	}
	case GUILD_SUBHEADER_GC_GRADE_AUTH:
	{
		BYTE byGradeNumber;
		if (!Recv(sizeof(byGradeNumber), &byGradeNumber))
		{
			return false;
		}
		BYTE byAuthorityFlag;
		if (!Recv(sizeof(byAuthorityFlag), &byAuthorityFlag))
		{
			return false;
		}

		CPythonGuild::Instance().SetGradeAuthority(byGradeNumber, byAuthorityFlag);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshGuildGrade", Py_BuildValue("()"));

		Tracef(" <Change Grade Authority> %d, %d\n", byGradeNumber, byAuthorityFlag);
		__RefreshGuildWindowGradePage();
		break;
	}
	case GUILD_SUBHEADER_GC_INFO:
	{
		TPacketGCGuildInfo GuildInfo;
		if (!Recv(sizeof(GuildInfo), &GuildInfo))
		{
			return false;
		}

		CPythonGuild::Instance().EnableGuild();
		CPythonGuild::TGuildInfo & rGuildInfo = CPythonGuild::Instance().GetGuildInfoRef();
		strncpy(rGuildInfo.szGuildName, GuildInfo.name, GUILD_NAME_MAX_LEN);
		rGuildInfo.szGuildName[GUILD_NAME_MAX_LEN] = '\0';

		rGuildInfo.dwGuildID = GuildInfo.guild_id;
		rGuildInfo.dwMasterPID = GuildInfo.master_pid;
		rGuildInfo.dwGuildLevel = GuildInfo.level;
		rGuildInfo.dwCurrentExperience = GuildInfo.exp;
		rGuildInfo.dwCurrentMemberCount = GuildInfo.member_count;
		rGuildInfo.dwMaxMemberCount = GuildInfo.max_member_count;
		rGuildInfo.dwGuildMoney = GuildInfo.gold;
		rGuildInfo.bHasLand = GuildInfo.hasLand;

		//Tracef(" <Info> %s, %d, %d : %d\n", GuildInfo.name, GuildInfo.master_pid, GuildInfo.level, rGuildInfo.bHasLand);
		__RefreshGuildWindowInfoPage();
		break;
	}
	case GUILD_SUBHEADER_GC_COMMENTS:
	{
		BYTE byCount;
		if (!Recv(sizeof(byCount), &byCount))
		{
			return false;
		}

		CPythonGuild::Instance().ClearComment();
		//Tracef(" >>> Comments Count : %d\n", byCount);

		for (BYTE i = 0; i < byCount; ++i)
		{
			DWORD dwCommentID;
			if (!Recv(sizeof(dwCommentID), &dwCommentID))
			{
				return false;
			}

			char szName[CHARACTER_NAME_MAX_LEN + 1] = "";
			if (!Recv(sizeof(szName), &szName))
			{
				return false;
			}

			char szComment[GULID_COMMENT_MAX_LEN + 1] = "";
			if (!Recv(sizeof(szComment), &szComment))
			{
				return false;
			}

			//Tracef(" [Comment-%d] : %s, %s\n", dwCommentID, szName, szComment);
			CPythonGuild::Instance().RegisterComment(dwCommentID, szName, szComment);
		}

		__RefreshGuildWindowBoardPage();
		break;
	}
	case GUILD_SUBHEADER_GC_CHANGE_EXP:
	{
		BYTE byLevel;
		if (!Recv(sizeof(byLevel), &byLevel))
		{
			return false;
		}
		DWORD dwEXP;
		if (!Recv(sizeof(dwEXP), &dwEXP))
		{
			return false;
		}
		CPythonGuild::Instance().SetGuildEXP(byLevel, dwEXP);
		Tracef(" <ChangeEXP> %d, %d\n", byLevel, dwEXP);
		__RefreshGuildWindowInfoPage();
		break;
	}
	case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE:
	{
		DWORD dwPID;
		if (!Recv(sizeof(dwPID), &dwPID))
		{
			return false;
		}
		BYTE byGrade;
		if (!Recv(sizeof(byGrade), &byGrade))
		{
			return false;
		}
		CPythonGuild::Instance().ChangeGuildMemberGrade(dwPID, byGrade);
		Tracef(" <ChangeMemberGrade> %d, %d\n", dwPID, byGrade);
		__RefreshGuildWindowMemberPage();
		break;
	}
	case GUILD_SUBHEADER_GC_SKILL_INFO:
	{
		CPythonGuild::TGuildSkillData & rSkillData = CPythonGuild::Instance().GetGuildSkillDataRef();
		if (!Recv(sizeof(rSkillData.bySkillPoint), &rSkillData.bySkillPoint))
		{
			return false;
		}
		if (!Recv(sizeof(rSkillData.bySkillLevel), rSkillData.bySkillLevel))
		{
			return false;
		}
		if (!Recv(sizeof(rSkillData.wGuildPoint), &rSkillData.wGuildPoint))
		{
			return false;
		}
		if (!Recv(sizeof(rSkillData.wMaxGuildPoint), &rSkillData.wMaxGuildPoint))
		{
			return false;
		}

		Tracef(" <SkillInfo> %d / %d, %d\n", rSkillData.bySkillPoint, rSkillData.wGuildPoint, rSkillData.wMaxGuildPoint);
		__RefreshGuildWindowSkillPage();
		break;
	}
	case GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL:
	{
		DWORD dwPID;
		if (!Recv(sizeof(dwPID), &dwPID))
		{
			return false;
		}
		BYTE byFlag;
		if (!Recv(sizeof(byFlag), &byFlag))
		{
			return false;
		}

		CPythonGuild::Instance().ChangeGuildMemberGeneralFlag(dwPID, byFlag);
		Tracef(" <ChangeMemberGeneralFlag> %d, %d\n", dwPID, byFlag);
		__RefreshGuildWindowMemberPage();
		break;
	}
	case GUILD_SUBHEADER_GC_GUILD_INVITE:
	{
		DWORD dwGuildID;
		if (!Recv(sizeof(dwGuildID), &dwGuildID))
		{
			return false;
		}
		char szGuildName[GUILD_NAME_MAX_LEN + 1];
		if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
		{
			return false;
		}

		szGuildName[GUILD_NAME_MAX_LEN] = 0;

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RecvGuildInviteQuestion", Py_BuildValue("(is)", dwGuildID, szGuildName));
		Tracef(" <Guild Invite> %d, %s\n", dwGuildID, szGuildName);
		break;
	}
	case GUILD_SUBHEADER_GC_WAR:
	{
		TPacketGCGuildWar kGuildWar;
		if (!Recv(sizeof(kGuildWar), &kGuildWar))
		{
			return false;
		}

		switch (kGuildWar.bWarState)
		{
		case GUILD_WAR_SEND_DECLARE:
			Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_SEND_DECLARE\n");
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
								  "BINARY_GuildWar_OnSendDeclare",
								  Py_BuildValue("(i)", kGuildWar.dwGuildOpp)
								 );
			break;
		case GUILD_WAR_RECV_DECLARE:
			Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_RECV_DECLARE\n");
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
								  "BINARY_GuildWar_OnRecvDeclare",
								  Py_BuildValue("(ii)", kGuildWar.dwGuildOpp, kGuildWar.bType)
								 );
			break;
		case GUILD_WAR_ON_WAR:
			Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_ON_WAR : %d, %d\n", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
								  "BINARY_GuildWar_OnStart",
								  Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
								 );
			CPythonGuild::Instance().StartGuildWar(kGuildWar.dwGuildOpp);
			break;
		case GUILD_WAR_END:
			Tracef(" >> GUILD_SUBHEADER_GC_WAR : GUILD_WAR_END\n");
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
								  "BINARY_GuildWar_OnEnd",
								  Py_BuildValue("(ii)", kGuildWar.dwGuildSelf, kGuildWar.dwGuildOpp)
								 );
			CPythonGuild::Instance().EndGuildWar(kGuildWar.dwGuildOpp);
			break;
		}
		break;
	}
	case GUILD_SUBHEADER_GC_GUILD_NAME:
	{
		DWORD dwID;
		char szGuildName[GUILD_NAME_MAX_LEN + 1];

		int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);

		int nItemSize = sizeof(dwID) + GUILD_NAME_MAX_LEN;

		assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_NAME");

		for (; iPacketSize > 0;)
		{
			if (!Recv(sizeof(dwID), &dwID))
			{
				return false;
			}

			if (!Recv(GUILD_NAME_MAX_LEN, &szGuildName))
			{
				return false;
			}

			szGuildName[GUILD_NAME_MAX_LEN] = 0;

			//Tracef(" >> GulidName [%d : %s]\n", dwID, szGuildName);
			CPythonGuild::Instance().RegisterGuildName(dwID, szGuildName);
			iPacketSize -= nItemSize;
		}
		break;
	}
	case GUILD_SUBHEADER_GC_GUILD_WAR_LIST:
	{
		DWORD dwSrcGuildID;
		DWORD dwDstGuildID;

		int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
		int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

		assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_WAR_LIST");

		for (; iPacketSize > 0;)
		{
			if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
			{
				return false;
			}

			if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
			{
				return false;
			}

			Tracef(" >> GulidWarList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
			CInstanceBase::InsertGVGKey(dwSrcGuildID, dwDstGuildID);
			CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
			iPacketSize -= nItemSize;
		}
		break;
	}
	case GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST:
	{
		DWORD dwSrcGuildID;
		DWORD dwDstGuildID;

		int iPacketSize = int(GuildPacket.size) - sizeof(GuildPacket);
		int nItemSize = sizeof(dwSrcGuildID) + sizeof(dwDstGuildID);

		assert(iPacketSize % nItemSize == 0 && "GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST");

		for (; iPacketSize > 0;)
		{

			if (!Recv(sizeof(dwSrcGuildID), &dwSrcGuildID))
			{
				return false;
			}

			if (!Recv(sizeof(dwDstGuildID), &dwDstGuildID))
			{
				return false;
			}

			Tracef(" >> GulidWarEndList [%d vs %d]\n", dwSrcGuildID, dwDstGuildID);
			CInstanceBase::RemoveGVGKey(dwSrcGuildID, dwDstGuildID);
			CPythonCharacterManager::Instance().ChangeGVG(dwSrcGuildID, dwDstGuildID);
			iPacketSize -= nItemSize;
		}
		break;
	}
	case GUILD_SUBHEADER_GC_WAR_POINT:
	{
		TPacketGuildWarPoint GuildWarPoint;
		if (!Recv(sizeof(GuildWarPoint), &GuildWarPoint))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
							  "BINARY_GuildWar_OnRecvPoint",
							  Py_BuildValue("(iii)", GuildWarPoint.dwGainGuildID, GuildWarPoint.dwOpponentGuildID, GuildWarPoint.lPoint)
							 );
		break;
	}
	case GUILD_SUBHEADER_GC_MONEY_CHANGE:
	{
		DWORD dwMoney;
		if (!Recv(sizeof(dwMoney), &dwMoney))
		{
			return false;
		}

		CPythonGuild::Instance().SetGuildMoney(dwMoney);

		__RefreshGuildWindowInfoPage();
		Tracef(" >> Guild Money Change : %d\n", dwMoney);
		break;
	}
	}

	return true;
}

// Guild
//////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////
// Fishing

bool CPythonNetworkStream::SendFishingPacket(int iRotation)
{
	BYTE byHeader = HEADER_CG_FISHING;
	if (!Send(sizeof(byHeader), &byHeader))
	{
		return false;
	}
	BYTE byPacketRotation = iRotation / 5;
	if (!Send(sizeof(BYTE), &byPacketRotation))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendGiveItemPacket(DWORD dwTargetVID, TItemPos ItemPos, int iItemCount)
{
	TPacketCGGiveItem GiveItemPacket;
	GiveItemPacket.byHeader = HEADER_CG_GIVE_ITEM;
	GiveItemPacket.dwTargetVID = dwTargetVID;
	GiveItemPacket.ItemPos = ItemPos;
	GiveItemPacket.byItemCount = iItemCount;

	if (!Send(sizeof(GiveItemPacket), &GiveItemPacket))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvFishing()
{
	TPacketGCFishing FishingPacket;
	if (!Recv(sizeof(FishingPacket), &FishingPacket))
	{
		return false;
	}

	CInstanceBase * pFishingInstance = NULL;
	if (FISHING_SUBHEADER_GC_FISH != FishingPacket.subheader)
	{
		pFishingInstance = CPythonCharacterManager::Instance().GetInstancePtr(FishingPacket.info);
		if (!pFishingInstance)
		{
			return true;
		}
	}

	switch (FishingPacket.subheader)
	{
	case FISHING_SUBHEADER_GC_START:
		pFishingInstance->StartFishing(float(FishingPacket.dir) * 5.0f);
		break;
	case FISHING_SUBHEADER_GC_STOP:
		if (pFishingInstance->IsFishing())
		{
			pFishingInstance->StopFishing();
		}
		break;
	case FISHING_SUBHEADER_GC_REACT:
		if (pFishingInstance->IsFishing())
		{
			pFishingInstance->SetFishEmoticon(); // Fish Emoticon
			pFishingInstance->ReactFishing();
		}
		break;
	case FISHING_SUBHEADER_GC_SUCCESS:
		pFishingInstance->CatchSuccess();
		break;
	case FISHING_SUBHEADER_GC_FAIL:
		pFishingInstance->CatchFail();
		if (pFishingInstance == CPythonCharacterManager::Instance().GetMainInstancePtr())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingFailure", Py_BuildValue("()"));
		}
		break;
	case FISHING_SUBHEADER_GC_FISH:
	{
		DWORD dwFishID = FishingPacket.info;

		if (0 == FishingPacket.info)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotifyUnknown", Py_BuildValue("()"));
			return true;
		}

		CItemData * pItemData;
		if (!CItemManager::Instance().GetItemDataPointer(dwFishID, &pItemData))
		{
			return true;
		}

		CInstanceBase * pMainInstance = CPythonCharacterManager::Instance().GetMainInstancePtr();
		if (!pMainInstance)
		{
			return true;
		}

		if (pMainInstance->IsFishing())
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingNotify", Py_BuildValue("(is)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName()));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnFishingSuccess", Py_BuildValue("(isi)", CItemData::ITEM_TYPE_FISH == pItemData->GetType(), pItemData->GetName(), FishingPacket.length));
		}
		break;
	}
	}

	return true;
}
// Fishing
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// Dungeon
bool CPythonNetworkStream::RecvDungeon()
{
	TPacketGCDungeon DungeonPacket;
	if (!Recv(sizeof(DungeonPacket), &DungeonPacket))
	{
		return false;
	}

	switch (DungeonPacket.subheader)
	{
	case DUNGEON_SUBHEADER_GC_TIME_ATTACK_START:
	{
		break;
	}
	case DUNGEON_SUBHEADER_GC_DESTINATION_POSITION:
	{
		unsigned long ulx, uly;
		if (!Recv(sizeof(ulx), &ulx))
		{
			return false;
		}
		if (!Recv(sizeof(uly), &uly))
		{
			return false;
		}

		CPythonPlayer::Instance().SetDungeonDestinationPosition(ulx, uly);
		break;
	}
	}

	return true;
}
// Dungeon
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
// MyShop
bool CPythonNetworkStream::SendBuildPrivateShopPacket(const char * c_szName, const std::vector<TShopItemTable> & c_rSellingItemStock)
{
	TPacketCGMyShop packet;
	packet.bHeader = HEADER_CG_MYSHOP;
	strncpy(packet.szSign, c_szName, SHOP_SIGN_MAX_LEN);
	packet.bCount = c_rSellingItemStock.size();
	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	for (std::vector<TShopItemTable>::const_iterator itor = c_rSellingItemStock.begin(); itor < c_rSellingItemStock.end(); ++itor)
	{
		const TShopItemTable & c_rItem = *itor;
		if (!Send(sizeof(c_rItem), &c_rItem))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvShopSignPacket()
{
	TPacketGCShopSign p;
	if (!Recv(sizeof(TPacketGCShopSign), &p))
	{
		return false;
	}

	CPythonPlayer& rkPlayer = CPythonPlayer::Instance();

	if (0 == strlen(p.szSign))
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
							  "BINARY_PrivateShop_Disappear",
							  Py_BuildValue("(i)", p.dwVID)
							 );

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
		{
			rkPlayer.ClosePrivateShop();
		}
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
							  "BINARY_PrivateShop_Appear",
							  Py_BuildValue("(is)", p.dwVID, p.szSign)
							 );

		if (rkPlayer.IsMainCharacterIndex(p.dwVID))
		{
			rkPlayer.OpenPrivateShop();
		}
	}

	return true;
}
/////////////////////////////////////////////////////////////////////////

bool CPythonNetworkStream::RecvTimePacket()
{
	TPacketGCTime TimePacket;
	if (!Recv(sizeof(TimePacket), &TimePacket))
	{
		return false;
	}

	IAbstractApplication& rkApp = IAbstractApplication::GetSingleton();
	rkApp.SetServerTime(TimePacket.time);

	return true;
}

bool CPythonNetworkStream::RecvWalkModePacket()
{
	TPacketGCWalkMode WalkModePacket;
	if (!Recv(sizeof(WalkModePacket), &WalkModePacket))
	{
		return false;
	}

	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(WalkModePacket.vid);
	if (pInstance)
	{
		if (WALKMODE_RUN == WalkModePacket.mode)
		{
			pInstance->SetRunMode();
		}
		else
		{
			pInstance->SetWalkMode();
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvChangeSkillGroupPacket()
{
	TPacketGCChangeSkillGroup ChangeSkillGroup;
	if (!Recv(sizeof(ChangeSkillGroup), &ChangeSkillGroup))
	{
		return false;
	}

	m_dwMainActorSkillGroup = ChangeSkillGroup.skill_group;

	CPythonPlayer::Instance().NEW_ClearSkillData();
	__RefreshCharacterWindow();
	return true;
}

void CPythonNetworkStream::__TEST_SetSkillGroupFake(int iIndex)
{
	m_dwMainActorSkillGroup = DWORD(iIndex);

	CPythonPlayer::Instance().NEW_ClearSkillData();
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshCharacter", Py_BuildValue("()"));
}

#ifdef ENABLE_FAST_REFINE_OPTION
	bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType, bool fast_refine)
#else
	bool CPythonNetworkStream::SendRefinePacket(BYTE byPos, BYTE byType)
#endif
{
	TPacketCGRefine kRefinePacket;
	kRefinePacket.header = HEADER_CG_REFINE;
	kRefinePacket.pos = byPos;
	kRefinePacket.type = byType;
#ifdef ENABLE_FAST_REFINE_OPTION
	kRefinePacket.fast_refine = fast_refine;
#endif

	if (!Send(sizeof(kRefinePacket), &kRefinePacket))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSelectItemPacket(DWORD dwItemPos)
{
	TPacketCGScriptSelectItem kScriptSelectItem;
	kScriptSelectItem.header = HEADER_CG_SCRIPT_SELECT_ITEM;
	kScriptSelectItem.selection = dwItemPos;

	if (!Send(sizeof(kScriptSelectItem), &kScriptSelectItem))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvRefineInformationPacket()
{
	TPacketGCRefineInformation kRefineInfoPacket;
	if (!Recv(sizeof(kRefineInfoPacket), &kRefineInfoPacket))
	{
		return false;
	}

	TRefineTable & rkRefineTable = kRefineInfoPacket.refine_table;
#ifdef ENABLE_FAST_REFINE_OPTION
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						  "OpenRefineDialog",
						  Py_BuildValue("(iiiiiiii)",
										kRefineInfoPacket.pos,
										kRefineInfoPacket.refine_table.result_vnum,
										rkRefineTable.cost,
										rkRefineTable.prob,
										kRefineInfoPacket.type,
										kRefineInfoPacket.fast_refine,
										kRefineInfoPacket.addedProb,
										kRefineInfoPacket.iSashRefine
							  ));
#else
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						  "OpenRefineDialog",
						  Py_BuildValue("(iiiii)",
										kRefineInfoPacket.pos,
										kRefineInfoPacket.refine_table.result_vnum,
										rkRefineTable.cost,
										rkRefineTable.prob,
										kRefineInfoPacket.type)
						 );
#endif

	for (int i = 0; i < rkRefineTable.material_count; ++i)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "AppendMaterialToRefineDialog", Py_BuildValue("(ii)", rkRefineTable.materials[i].vnum, rkRefineTable.materials[i].count));
	}

#ifdef _DEBUG
	Tracef(" >> RecvRefineInformationPacket(pos=%d, result_vnum=%d, cost=%d, prob=%d, type=%d)\n",
		   kRefineInfoPacket.pos,
		   kRefineInfoPacket.refine_table.result_vnum,
		   rkRefineTable.cost,
		   rkRefineTable.prob,
		   kRefineInfoPacket.type);
#endif

	return true;
}

bool CPythonNetworkStream::RecvNPCList()
{
	TPacketGCNPCPosition kNPCPosition;
	if (!Recv(sizeof(kNPCPosition), &kNPCPosition))
	{
		return false;
	}

	assert(int(kNPCPosition.size) - sizeof(kNPCPosition) == kNPCPosition.count * sizeof(TNPCPosition) && "HEADER_GC_NPC_POSITION");

	CPythonMiniMap::Instance().ClearAtlasMarkInfo();

	for (int i = 0; i < kNPCPosition.count; ++i)
	{
		TNPCPosition NPCPosition;
		if (!Recv(sizeof(TNPCPosition), &NPCPosition))
		{
			return false;
		}

		CPythonMiniMap::Instance().RegisterAtlasMark(NPCPosition.bType, NPCPosition.name, NPCPosition.x, NPCPosition.y);
	}

	return true;
}

bool CPythonNetworkStream::__SendCRCReportPacket()
{
	/*
	DWORD dwProcessCRC = 0;
	DWORD dwFileCRC = 0;
	CFilename exeFileName;
	//LPCVOID c_pvBaseAddress = NULL;

	GetExeCRC(dwProcessCRC, dwFileCRC);

	CFilename strRootPackFileName = CEterPackManager::Instance().GetRootPacketFileName();
	strRootPackFileName.ChangeDosPath();

	TPacketCGCRCReport kReportPacket;

	kReportPacket.header = HEADER_CG_CRC_REPORT;
	kReportPacket.byPackMode = CEterPackManager::Instance().GetSearchMode();
	kReportPacket.dwBinaryCRC32 = dwFileCRC;
	kReportPacket.dwProcessCRC32 = dwProcessCRC;
	kReportPacket.dwRootPackCRC32 = GetFileCRC32(strRootPackFileName.c_str());

	if (!Send(sizeof(kReportPacket), &kReportPacket))
		Tracef("SendClientReportPacket Error");

	return SendSequence();
	*/
	return true;
}

bool CPythonNetworkStream::SendClientVersionPacket()
{
	std::string filename;

	GetExcutedFileName(filename);

	filename = CFileNameHelper::NoPath(filename);
	CFileNameHelper::ChangeDosPath(filename);

	TPacketCGClientVersion2 kVersionPacket;
	kVersionPacket.header = HEADER_CG_CLIENT_VERSION2;
	strncpy(kVersionPacket.filename, filename.c_str(), sizeof(kVersionPacket.filename) - 1);
	strncpy(kVersionPacket.timestamp, "1215955205", sizeof(kVersionPacket.timestamp) - 1); // # python time.time ¾ÕÀÚ¸®
	//strncpy(kVersionPacket.timestamp, __TIMESTAMP__, sizeof(kVersionPacket.timestamp)-1); // old_string_ver
	//strncpy(kVersionPacket.timestamp, "1218055205", sizeof(kVersionPacket.timestamp)-1); // new_future
	//strncpy(kVersionPacket.timestamp, "1214055205", sizeof(kVersionPacket.timestamp)-1); // old_past

	if (!Send(sizeof(kVersionPacket), &kVersionPacket))
	{
		Tracef("SendClientReportPacket Error");
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvAffectAddPacket()
{
	TPacketGCAffectAdd kAffectAdd;
	if (!Recv(sizeof(kAffectAdd), &kAffectAdd))
	{
		return false;
	}

	TPacketAffectElement & rkElement = kAffectAdd.elem;
	if (rkElement.bPointIdxApplyOn == POINT_ENERGY)
	{
		CPythonPlayer::instance().SetStatus (POINT_ENERGY_END_TIME, CPythonApplication::Instance().GetServerTimeStamp() + rkElement.lDuration);
		__RefreshStatus();
	}
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_AddAffect", Py_BuildValue("(iiii)", rkElement.dwType, rkElement.bPointIdxApplyOn, rkElement.lApplyValue, rkElement.lDuration));

	return true;
}

bool CPythonNetworkStream::RecvAffectRemovePacket()
{
	TPacketGCAffectRemove kAffectRemove;
	if (!Recv(sizeof(kAffectRemove), &kAffectRemove))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_NEW_RemoveAffect", Py_BuildValue("(ii)", kAffectRemove.dwType, kAffectRemove.bApplyOn));

	return true;
}

bool CPythonNetworkStream::RecvChannelPacket()
{
	TPacketGCChannel kChannelPacket;
	if (!Recv(sizeof(kChannelPacket), &kChannelPacket))
	{
		return false;
	}

	bDisableChatPacket = false;
	//Tracef(" >> CPythonNetworkStream::RecvChannelPacket(channel=%d)\n", kChannelPacket.channel);

	return true;
}

bool CPythonNetworkStream::RecvViewEquipPacket()
{
	TPacketGCViewEquip kViewEquipPacket;
	if (!Recv(sizeof(kViewEquipPacket), &kViewEquipPacket))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OpenEquipmentDialog", Py_BuildValue("(i)", kViewEquipPacket.dwVID));

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		TEquipmentItemSet & rItemSet = kViewEquipPacket.equips[i];
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogItem", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, rItemSet.vnum, rItemSet.count));

		for (int j = 0; j < ITEM_SOCKET_SLOT_MAX_NUM; ++j)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogSocket", Py_BuildValue("(iiii)", kViewEquipPacket.dwVID, i, j, rItemSet.alSockets[j]));
		}

		for (int k = 0; k < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++k)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "SetEquipmentDialogAttr", Py_BuildValue("(iiiii)", kViewEquipPacket.dwVID, i, k, rItemSet.aAttr[k].bType, rItemSet.aAttr[k].sValue));
		}
	}

	return true;
}

bool CPythonNetworkStream::RecvLandPacket()
{
	TPacketGCLandList kLandList;
	if (!Recv(sizeof(kLandList), &kLandList))
	{
		return false;
	}

	std::vector<DWORD> kVec_dwGuildID;

	CPythonMiniMap & rkMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkBG = CPythonBackground::Instance();
	CInstanceBase * pMainInstance = CPythonPlayer::Instance().NEW_GetMainActorPtr();

	rkMiniMap.ClearGuildArea();
	rkBG.ClearGuildArea();

	int iPacketSize = (kLandList.size - sizeof(TPacketGCLandList));
	for (; iPacketSize > 0; iPacketSize -= sizeof(TLandPacketElement))
	{
		TLandPacketElement kElement;
		if (!Recv(sizeof(TLandPacketElement), &kElement))
		{
			return false;
		}

		rkMiniMap.RegisterGuildArea(kElement.dwID,
									kElement.dwGuildID,
									kElement.x,
									kElement.y,
									kElement.width,
									kElement.height);

		if (pMainInstance)
			if (kElement.dwGuildID == pMainInstance->GetGuildID())
			{
				rkBG.RegisterGuildArea(kElement.x,
									   kElement.y,
									   kElement.x + kElement.width,
									   kElement.y + kElement.height);
			}

		if (0 != kElement.dwGuildID)
		{
			kVec_dwGuildID.push_back(kElement.dwGuildID);
		}
	}
	// @fixme006
	if (kVec_dwGuildID.size() > 0)
	{
		__DownloadSymbol(kVec_dwGuildID);
	}

	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacket()
{
	TPacketGCTargetCreate kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
	{
		return false;
	}

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacket %d : %s\n", kTargetCreate.lID, kTargetCreate.szTargetName);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetCreatePacketNew()
{
	TPacketGCTargetCreateNew kTargetCreate;
	if (!Recv(sizeof(kTargetCreate), &kTargetCreate))
	{
		return false;
	}

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	if (CREATE_TARGET_TYPE_LOCATION == kTargetCreate.byType)
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName);
	}
	else
	{
		rkpyMiniMap.CreateTarget(kTargetCreate.lID, kTargetCreate.szTargetName, kTargetCreate.dwVID);
		rkpyBG.CreateTargetEffect(kTargetCreate.lID, kTargetCreate.dwVID);
	}

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetCreatePacketNew %d : %d/%d\n", kTargetCreate.lID, kTargetCreate.byType, kTargetCreate.dwVID);
//#endif

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenAtlasWindow", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::RecvTargetUpdatePacket()
{
	TPacketGCTargetUpdate kTargetUpdate;
	if (!Recv(sizeof(kTargetUpdate), &kTargetUpdate))
	{
		return false;
	}

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.UpdateTarget(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.CreateTargetEffect(kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);

//#ifdef _DEBUG
//	char szBuf[256+1];
//	CPythonChat::Instance().AppendChat(CHAT_TYPE_NOTICE, szBuf);
//	Tracef(" >> RecvTargetUpdatePacket %d : %d, %d\n", kTargetUpdate.lID, kTargetUpdate.lX, kTargetUpdate.lY);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvTargetDeletePacket()
{
	TPacketGCTargetDelete kTargetDelete;
	if (!Recv(sizeof(kTargetDelete), &kTargetDelete))
	{
		return false;
	}

	CPythonMiniMap & rkpyMiniMap = CPythonMiniMap::Instance();
	rkpyMiniMap.DeleteTarget(kTargetDelete.lID);

	CPythonBackground & rkpyBG = CPythonBackground::Instance();
	rkpyBG.DeleteTargetEffect(kTargetDelete.lID);

//#ifdef _DEBUG
//	Tracef(" >> RecvTargetDeletePacket %d\n", kTargetDelete.lID);
//#endif

	return true;
}

bool CPythonNetworkStream::RecvLoverInfoPacket()
{
	TPacketGCLoverInfo kLoverInfo;
	if (!Recv(sizeof(kLoverInfo), &kLoverInfo))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_LoverInfo", Py_BuildValue("(si)", kLoverInfo.szName, kLoverInfo.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVER INFO : %s, %d\n", kLoverInfo.szName, kLoverInfo.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvLovePointUpdatePacket()
{
	TPacketGCLovePointUpdate kLovePointUpdate;
	if (!Recv(sizeof(kLovePointUpdate), &kLovePointUpdate))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_UpdateLovePoint", Py_BuildValue("(i)", kLovePointUpdate.byLovePoint));
#ifdef _DEBUG
	Tracef("RECV LOVE POINT UPDATE : %d\n", kLovePointUpdate.byLovePoint);
#endif
	return true;
}

bool CPythonNetworkStream::RecvDigMotionPacket()
{
	TPacketGCDigMotion kDigMotion;
	if (!Recv(sizeof(kDigMotion), &kDigMotion))
	{
		return false;
	}

#ifdef _DEBUG
	Tracef(" Dig Motion [%d/%d]\n", kDigMotion.vid, kDigMotion.count);
#endif

	IAbstractCharacterManager& rkChrMgr = IAbstractCharacterManager::GetSingleton();
	CInstanceBase * pkInstMain = rkChrMgr.GetInstancePtr(kDigMotion.vid);
	CInstanceBase * pkInstTarget = rkChrMgr.GetInstancePtr(kDigMotion.target_vid);
	if (NULL == pkInstMain)
	{
		return true;
	}

	if (pkInstTarget)
	{
		pkInstMain->NEW_LookAtDestInstance(*pkInstTarget);
	}

	for (int i = 0; i < kDigMotion.count; ++i)
	{
		pkInstMain->PushOnceMotion(CRaceMotionData::NAME_DIG);
	}

	return true;
}


bool CPythonNetworkStream::SendDragonSoulRefinePacket(BYTE bRefineType, TItemPos* pos)
{
	TPacketCGDragonSoulRefine pk;
	pk.header = HEADER_CG_DRAGON_SOUL_REFINE;
	pk.bSubType = bRefineType;
	memcpy (pk.ItemGrid, pos, sizeof (TItemPos) * DS_REFINE_WINDOW_MAX_NUM);
	if (!Send(sizeof (pk), &pk))
	{
		return false;
	}
	return true;
}

#ifdef ENABLE_TREASURE_BOX_LOOT
bool CPythonNetworkStream::SendRequestTreasureBoxLoot(DWORD dwItemVnum)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGDefault packet(HEADER_CG_REQUEST_TREASURE_BOX_LOOT);
	packet.size += sizeof(dwItemVnum);

	if (!(Send(sizeof(TPacketCGDefault), &packet)))
	{
		return false;
	}

	if (!(Send(sizeof(DWORD), &dwItemVnum)))
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvRequestTreasureBoxLoot()
{
	TPacketCGDefault p(HEADER_GC_REQUEST_TREASURE_BOX_LOOT);
	if (!(Recv(sizeof(TPacketCGDefault), &p)))
	{
		return false;
	}

	size_t size = p.size - sizeof(TPacketCGDefault);

	struct CSpecialItemInfo
	{
		DWORD vnum;
		int count;
		int rare;
	};

	if (size != sizeof(DWORD))
	{
		std::vector<CSpecialItemInfo> items;
		while (size != sizeof(DWORD))
		{
			CSpecialItemInfo item;

			if (!(Recv(sizeof(CSpecialItemInfo), &item)))
			{
				return false;
			}

			items.push_back(item);
			size -= sizeof(CSpecialItemInfo);
		}

		DWORD dwItemVnum;
		if (!(Recv(sizeof(DWORD), &dwItemVnum)))
		{
			return false;
		}

		std::sort(items.begin(), items.end(), [](const CSpecialItemInfo & lhs, const CSpecialItemInfo & rhs)
		{
			return lhs.vnum < rhs.vnum;
		});

		auto poArgs = PyTuple_New(2);
		PyTuple_SetItem(poArgs, 0, PyInt_FromLong(dwItemVnum));

		auto list = PyList_New(items.size());

		auto j = 0;
		for (const auto i : items)
		{
			auto tuple = PyTuple_New(2);
			PyTuple_SetItem(tuple, 0, PyInt_FromLong(i.vnum));
			PyTuple_SetItem(tuple, 1, PyInt_FromLong(i.count));

			PyList_SET_ITEM(list, j++, tuple);
		}

		PyTuple_SetItem(poArgs, 1, list);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendTreasureBoxLoot", poArgs);
	}
	else
	{
		DWORD dwItemVnum;
		if (!(Recv(sizeof(DWORD), &dwItemVnum)))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AppendTreasureBoxLoot", Py_BuildValue("(i)", dwItemVnum));
	}
	return true;
}
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
bool targetDropInfoCompare(const CPythonNonPlayer::TDropInfo& a, const CPythonNonPlayer::TDropInfo& b)
{
	if (a.dwVNum != b.dwVNum)
	{
		return a.dwVNum < b.dwVNum;
	}

	return a.bCount < b.bCount;
}

bool CPythonNetworkStream::SendLoadTargetInfo(DWORD dwVID)
{
	TPacketCGTargetLoad kPacket;
	kPacket.byHeader = HEADER_CG_TARGET_LOAD;
	kPacket.dwVID = dwVID;

	if (!Send(sizeof(kPacket), &kPacket))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvTargetInfoPacket()
{
	TPacketGCTargetInfo kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
	{
		Tracen("Recv TargetInfo Packet Error");
		return false;
	}

	if (kPacket.uCount > 0)
	{
		CPythonNonPlayer::TVecDropInfo vecDropInfo;
		vecDropInfo.resize(kPacket.uCount);

		if (!Recv(sizeof(CPythonNonPlayer::TDropInfo) * kPacket.uCount, &vecDropInfo[0]))
		{
			Tracen("Recv TargetInfo Invalid DropInfo Count");
			return false;
		}

		std::sort(vecDropInfo.begin(), vecDropInfo.end(), targetDropInfoCompare);
		for (const auto& c_rDropInfo : vecDropInfo)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddDropInfo", Py_BuildValue("(iiiiii)", kPacket.dwVNum, c_rDropInfo.dwVNum, c_rDropInfo.bCount, c_rDropInfo.iMinLevel, c_rDropInfo.iMaxLevel, c_rDropInfo.iRarity));
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshDropInfo", Py_BuildValue("()"));
	}

	return true;
}
#endif

#ifdef ENABLE_DELETE_SINGLE_STONE
bool CPythonNetworkStream::SendRequestDeleteSocket(ESubHeader subHeader, WORD index)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGDefault packet(HEADER_CG_REQUEST_DELETE_SOCKET);
	if (!(Send(sizeof(TPacketCGDefault), &packet)))
	{
		return false;
	}

	if (!(Send(sizeof(BYTE), &subHeader)))
	{
		return false;
	}

	if (subHeader != SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE)
	{
		if (!(Send(sizeof(WORD), &index)))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvRequestDeleteSocket()
{
	TPacketCGDefault p(HEADER_GC_REQUEST_DELETE_SOCKET);
	if (!(Recv(sizeof(TPacketCGDefault), &p)))
	{
		return false;
	}

	size_t size = p.size -= sizeof(TPacketCGDefault);

	uint8_t subheader;
	if (!(Recv(sizeof(uint8_t), &subheader)))
	{
		return false;
	}

	switch (subheader)
	{
	case SUBHEADER_REQUEST_DELETE_SOCKET_OPEN:
	{
		uint16_t cell;
		if (!(Recv(sizeof(uint16_t), &cell)))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RemoveStoneSetItem", Py_BuildValue("(i)", cell));
	}
	break;
	case SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_CloseRemoveStoneWindow", Py_BuildValue("()"));
	}
	break;
	}
	return true;
}
#endif

#ifdef ENABLE_SWITCHBOT
bool CPythonNetworkStream::RecvSwitchbotPacket()
{
	TPacketGCSwitchbot pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	size_t packet_size = int(pack.size) - sizeof(TPacketGCSwitchbot);
	if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE)
	{
		if (packet_size != sizeof(CPythonSwitchbot::TSwitchbotTable))
		{
			return false;
		}

		CPythonSwitchbot::TSwitchbotTable table;
		if (!Recv(sizeof(table), &table))
		{
			return false;
		}

		CPythonSwitchbot::Instance().Update(table);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotWindow", Py_BuildValue("()"));
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM)
	{
		if (packet_size != sizeof(TSwitchbotUpdateItem))
		{
			return false;
		}

		TSwitchbotUpdateItem update;
		if (!Recv(sizeof(update), &update))
		{
			return false;
		}

		TItemPos pos(SWITCHBOT, update.slot);

		IAbstractPlayer& rkPlayer = IAbstractPlayer::GetSingleton();
		rkPlayer.SetItemCount(pos, update.count);

		for (int i = 0; i < ITEM_SOCKET_SLOT_MAX_NUM; ++i)
		{
			rkPlayer.SetItemMetinSocket(pos, i, update.alSockets[i]);

		}

		for (int j = 0; j < ITEM_ATTRIBUTE_SLOT_MAX_NUM; ++j)
		{
			rkPlayer.SetItemAttribute(pos, j, update.aAttr[j].bType, update.aAttr[j].sValue);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "RefreshSwitchbotItem", Py_BuildValue("(i)", update.slot));
		return true;
	}
	else if (pack.subheader == SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION)
	{
		CPythonSwitchbot::Instance().ClearAttributeMap();

		size_t table_size = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);
		while (packet_size >= table_size)
		{
			const int test = sizeof(CPythonSwitchbot::TSwitchbottAttributeTable);

			CPythonSwitchbot::TSwitchbottAttributeTable table;
			if (!Recv(table_size, &table))
			{
				return false;
			}

			CPythonSwitchbot::Instance().AddAttributeToMap(table);
			packet_size -= table_size;
		}
	}

	return true;
}
bool CPythonNetworkStream::SendSwitchbotStartPacket(BYTE slot, std::vector<CPythonSwitchbot::TSwitchbotAttributeAlternativeTable> alternatives)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_START;
	pack.size = sizeof(TPacketCGSwitchbot) + sizeof(CPythonSwitchbot::TSwitchbotAttributeAlternativeTable) * SWITCHBOT_ALTERNATIVE_COUNT;
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	for (const auto& it : alternatives)
	{
		if (!Send(sizeof(it), &it))
		{
			return false;
		}
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendSwitchbotStopPacket(BYTE slot)
{
	TPacketCGSwitchbot pack;
	pack.header = HEADER_CG_SWITCHBOT;
	pack.subheader = SUBHEADER_CG_SWITCHBOT_STOP;
	pack.size = sizeof(TPacketCGSwitchbot);
	pack.slot = slot;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
bool CPythonNetworkStream::RecvExchangeInfoPacket()
{
	TPacketGCExchageInfo infoPacket;
	char buf[512 + 1];

	if (!Recv(sizeof(infoPacket), &infoPacket))
	{
		return false;
	}

	assert(infoPacket.wSize - sizeof(infoPacket) < 512);

	if (!Recv(infoPacket.wSize - sizeof(infoPacket), &buf))
	{
		return false;
	}

	buf[infoPacket.wSize - sizeof(infoPacket)] = '\0';

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvExchangeInfo", Py_BuildValue("(ibs)", infoPacket.iUnixTime, infoPacket.bError, buf));
	return true;
}
#endif

#ifdef ENABLE_VOICE_CHAT
bool CPythonNetworkStream::SendVoiceStart(short* bData)
{
	TPacketCGVoice p;
	p.header = HEADER_CG_VOICE_CHAT;

	for (int i = 0; i < 1024; i++)
	{
		p.data[i] = bData[i];
	}

	if (!Send(sizeof(p), &p))
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvDataVoiceChat()
{
	TPacketGCVoiceChat infoPacket;

	if (!Recv(sizeof(infoPacket), &infoPacket))
	{
		return false;
	}

	PyObject* list = PyTuple_New(sizeof(infoPacket.data) / sizeof(short));
	PyObject* item;
	for (int i = 0; i < PyTuple_GET_SIZE(list); i++)
	{
		item = PyInt_FromLong(infoPacket.data[i]);
		PyTuple_SET_ITEM(list, i, item);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "OnRecvVoice", Py_BuildValue("(sfO)", infoPacket.name, infoPacket.distance, list));
	return true;
}
#endif

#ifdef ENABLE_ADMIN_MANAGER
bool CPythonNetworkStream::RecvAdminManagerLoad()
{
	TPacketGCAdminManagerLoad packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	// initialize
	CPythonAdmin::instance().Initialize();
	CPythonAdmin::Instance().SetGameWindow(m_apoPhaseWnd[PHASE_WINDOW_GAME]);
	CPythonAdmin::Instance().SetAllowFlag(packet.dwAllowFlag);

	for (DWORD i = 0; i < packet.dwPlayerCount; ++i)
	{
		TAdminManagerPlayerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonAdmin::instance().AddOnlinePlayer(&kInfo);
	}

	CPythonAdmin::Instance().SetGMItemTradeBlock(packet.bIsGMItemTradeBlock);

	for (int i = 0; i < packet.dwBanChatCount; ++i)
	{
		TAdminManagerBanClientPlayerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonAdmin::instance().Ban_UpdateChatInfo(&kInfo);
	}

	for (int i = 0; i < packet.dwBanAccountCount; ++i)
	{
		TAdminManagerBanClientAccountInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonAdmin::Instance().Ban_UpdateAccountInfo(&kInfo);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminInit", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvAdminManager()
{
	TPacketGCAdminManager packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	CPythonAdmin& rkAdmin = CPythonAdmin::instance();

	switch (packet.sub_header)
	{
	case ADMIN_MANAGER_GC_SUBHEADER_PLAYER_ONLINE:
	{
		TAdminManagerPlayerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		if (!rkAdmin.AddOnlinePlayer(&kInfo))
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminPlayerOnline", Py_BuildValue("(i)", kInfo.dwPID));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_PLAYER_OFFLINE:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		if (rkAdmin.RemoveOnlinePlayer(dwPID))
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminPlayerOffline", Py_BuildValue("(i)", dwPID));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_GM_ITEM_TRADE_BLOCK:
	{
		bool bIsActive;
		if (!Recv(sizeof(bool), &bIsActive))
		{
			return false;
		}

		rkAdmin.SetGMItemTradeBlock(bIsActive);
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminPlayerRefreshGMItemTradeOption", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_LOAD:
	{
		rkAdmin.MapViewer_Clear();

		DWORD dwBaseX, dwBaseY;
		if (!Recv(sizeof(DWORD), &dwBaseX) || !Recv(sizeof(DWORD), &dwBaseY))
		{
			return false;
		}
		rkAdmin.MapViewer_LoadMapInfo(dwBaseX, dwBaseY);

		// if (CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_MAPVIEWER_PLAYER))
		{
			DWORD dwPlayerCount;
			if (!Recv(sizeof(DWORD), &dwPlayerCount))
			{
				return false;
			}

			for (DWORD i = 0; i < dwPlayerCount; ++i)
			{
				TAdminManagerMapViewerPlayerInfo kInfo;
				if (!Recv(sizeof(kInfo), &kInfo))
				{
					return false;
				}
				rkAdmin.MapViewer_AddPlayer(kInfo);
			}
		}

		if (CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_MAPVIEWER_MONSTER | CPythonAdmin::ALLOW_VIEW_MAPVIEWER_NPC |
											  CPythonAdmin::ALLOW_VIEW_MAPVIEWER_STONE))
		{
			DWORD dwMobCount;
			if (!Recv(sizeof(DWORD), &dwMobCount))
			{
				return false;
			}

			for (DWORD i = 0; i < dwMobCount; ++i)
			{
				TAdminManagerMapViewerMobInfo kInfo;
				if (!Recv(sizeof(kInfo), &kInfo))
				{
					return false;
				}
				rkAdmin.MapViewer_AddMonster(kInfo);
			}
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminMapViewerStart", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_MOVE:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		TAdminManagerMapViewerPlayerInfo* pPlayer;
		if (!(pPlayer = rkAdmin.MapViewer_GetPlayerByPID(dwPID)))
		{
			TraceError("cannot get player by PID %u", dwPID);
			return false;
		}

		DWORD x, y;
		if (!Recv(sizeof(DWORD), &x) || !Recv(sizeof(DWORD), &y))
		{
			return false;
		}

		pPlayer->x = x;
		pPlayer->y = y;
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_STATE:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		TAdminManagerMapViewerPlayerInfo* pPlayer;
		if (!(pPlayer = rkAdmin.MapViewer_GetPlayerByPID(dwPID)))
		{
			TraceError("cannot get player by PID %u", dwPID);
			return false;
		}

		bool is_alive;
		if (!Recv(sizeof(bool), &is_alive))
		{
			return false;
		}

		pPlayer->is_alive = is_alive;
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_APPEND:
	{
		TAdminManagerMapViewerPlayerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		rkAdmin.MapViewer_AddPlayer(kInfo);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_DESTROY:
	{
		DWORD dwPID;
		if (!Recv(sizeof(DWORD), &dwPID))
		{
			return false;
		}

		rkAdmin.MapViewer_ErasePlayer(dwPID);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_MOVE:
	{
		DWORD dwVID;
		if (!Recv(sizeof(DWORD), &dwVID))
		{
			return false;
		}

		TAdminManagerMapViewerMobInfo* pMonster;
		if (!(pMonster = rkAdmin.MapViewer_GetMonsterByVID(dwVID)))
		{
			TraceError("cannot get monster by VID %u", dwVID);
			return false;
		}

		DWORD x, y;
		if (!Recv(sizeof(DWORD), &x) || !Recv(sizeof(DWORD), &y))
		{
			return false;
		}

		pMonster->x = x;
		pMonster->y = y;
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_STATE:
	{
		DWORD dwVID;
		if (!Recv(sizeof(DWORD), &dwVID))
		{
			return false;
		}

		TAdminManagerMapViewerMobInfo* pMonster;
		if (!(pMonster = rkAdmin.MapViewer_GetMonsterByVID(dwVID)))
		{
			TraceError("cannot get monster by VID %u", dwVID);
			return false;
		}

		bool is_alive;
		if (!Recv(sizeof(bool), &is_alive))
		{
			return false;
		}

		pMonster->is_alive = is_alive;
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_APPEND:
	{
		TAdminManagerMapViewerMobInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		rkAdmin.MapViewer_AddMonster(kInfo);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_DESTROY:
	{
		DWORD dwVID;
		if (!Recv(sizeof(DWORD), &dwVID))
		{
			return false;
		}

		rkAdmin.MapViewer_EraseMonster(dwVID);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_LOAD:
	{
		DWORD dwPID;
		DWORD dwAID;
		BYTE bRaceNum;
		if (!Recv(sizeof(dwPID), &dwPID) || !Recv(sizeof(DWORD), &dwAID) || !Recv(sizeof(BYTE), &bRaceNum))
		{
			return false;
		}
		bool bClearMessages = !rkAdmin.Observer_IsStoppedByForce() || dwPID != rkAdmin.Observer_GetPID();
		rkAdmin.Observer_Clear(bClearMessages);
		rkAdmin.Observer_SetPID(dwPID);
		rkAdmin.Observer_SetAID(dwAID);
		rkAdmin.Observer_SetRaceNum(bRaceNum);

		int iLoginNameLen;
		if (!Recv(sizeof(int), &iLoginNameLen))
		{
			return false;
		}
		char* szLoginName = new char[iLoginNameLen + 1];
		if (!Recv(iLoginNameLen, szLoginName))
		{
			delete[] szLoginName;
			return false;
		}
		szLoginName[iLoginNameLen] = '\0';
		rkAdmin.Observer_SetLoginName(szLoginName);
		delete[] szLoginName;

		DWORD dwX, dwY;
		if (!Recv(sizeof(dwX), &dwX) || !Recv(sizeof(dwY), &dwY))
		{
			return false;
		}
		rkAdmin.Observer_LoadMapName(dwX, dwY);

		if (CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_OBSERVER_SKILL))
		{
			BYTE bSkillGroup;
			if (!Recv(sizeof(bSkillGroup), &bSkillGroup))
			{
				return false;
			}
			rkAdmin.Observer_SetSkillGroup(bSkillGroup);
			TPlayerSkill akSkills[SKILL_MAX_NUM];
			if (!Recv(sizeof(akSkills), &akSkills[0]))
			{
				return false;
			}
			for (int i = 0; i < SKILL_MAX_NUM; ++i)
			{
				rkAdmin.Observer_SetSkill(i, &akSkills[i]);
			}
		}

		DWORD dwPointsCount;
		if (!Recv(sizeof(dwPointsCount), &dwPointsCount))
		{
			return false;
		}
		for (DWORD i = 0; i < dwPointsCount; ++i)
		{
			BYTE bType;
			int64_t iValue;
			if (!Recv(sizeof(bType), &bType) || !Recv(sizeof(iValue), &iValue))
			{
				return false;
			}
			rkAdmin.Observer_SetPoint(bType, iValue);
		}

		if (CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_OBSERVER_INVENTORY | CPythonAdmin::ALLOW_VIEW_OBSERVER_EQUIPMENT))
		{
			DWORD dwItemCount;
			if (!Recv(sizeof(dwItemCount), &dwItemCount))
			{
				return false;
			}
			for (DWORD i = 0; i < dwItemCount; ++i)
			{
				TAdminManagerObserverItemInfo kItemInfo;
				if (!Recv(sizeof(kItemInfo), &kItemInfo))
				{
					return false;
				}
				rkAdmin.Observer_SetInventoryItem(&kItemInfo);
			}
		}

		DWORD dwChatBanTimeLeft;
		if (!Recv(sizeof(dwChatBanTimeLeft), &dwChatBanTimeLeft))
		{
			return false;
		}
		rkAdmin.Observer_SetChatBanTimeLeft(dwChatBanTimeLeft);

		if (bClearMessages)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverStart", Py_BuildValue("()"));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefresh", Py_BuildValue("()"));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILLGROUP:
	{
		BYTE bSkillGroup;
		if (!Recv(sizeof(bSkillGroup), &bSkillGroup))
		{
			return false;
		}

		rkAdmin.Observer_SetSkillGroup(bSkillGroup);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshSkill", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_UPDATE:
	{
		DWORD dwSkillVnum;
		if (!Recv(sizeof(dwSkillVnum), &dwSkillVnum))
		{
			return false;
		}
		TPlayerSkill kSkill;
		if (!Recv(sizeof(kSkill), &kSkill))
		{
			return false;
		}

		rkAdmin.Observer_SetSkill(dwSkillVnum, &kSkill);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshSkill", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_UPDATE_ALL:
	{
		TPlayerSkill akSkills[SKILL_MAX_NUM];
		if (!Recv(sizeof(akSkills), &akSkills))
		{
			return false;
		}

		for (int i = 0; i < SKILL_MAX_NUM; ++i)
		{
			rkAdmin.Observer_SetSkill(i, &akSkills[i]);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshSkill", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_COOLDOWN:
	{
		DWORD dwSkillVnum;
		DWORD dwCoolTime;
		if (!Recv(sizeof(dwSkillVnum), &dwSkillVnum) || !Recv(sizeof(dwCoolTime), &dwCoolTime))
		{
			return false;
		}

		rkAdmin.Observer_SetSkillCoolTime(dwSkillVnum, dwCoolTime);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshSkill", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_POINT_UPDATE:
	{
		BYTE bType;
		int64_t llValue;
		if (!Recv(sizeof(bType), &bType) || !Recv(sizeof(llValue), &llValue))
		{
			return false;
		}

		rkAdmin.Observer_SetPoint(bType, llValue);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverPointChange", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ITEM_SET:
	{
		TAdminManagerObserverItemInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		rkAdmin.Observer_SetInventoryItem(&kInfo);

		if (kInfo.cell < c_ItemSlot_Count)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshInventory", Py_BuildValue("(i)", kInfo.cell / c_Inventory_Page_Size));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshEquipment", Py_BuildValue("()"));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ITEM_DEL:
	{
		WORD wCell;
		if (!Recv(sizeof(wCell), &wCell))
		{
			return false;
		}

		rkAdmin.Observer_DelInventoryItem(wCell);

		if (wCell < c_ItemSlot_Count)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshInventory", Py_BuildValue("(i)", wCell / c_Inventory_Page_Size));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverRefreshEquipment", Py_BuildValue("()"));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_CHATBAN_STATE:
	{
		DWORD dwTimeLeft;
		if (!Recv(sizeof(dwTimeLeft), &dwTimeLeft))
		{
			return false;
		}

		rkAdmin.Observer_SetChatBanTimeLeft(dwTimeLeft);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ACCOUNTBAN_STATE:
	{
		DWORD dwTimeLeft;
		if (!Recv(sizeof(dwTimeLeft), &dwTimeLeft))
		{
			return false;
		}

		rkAdmin.Observer_SetAccountBanTimeLeft(dwTimeLeft);
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_STOP_FORCED:
	{
		rkAdmin.Observer_StopRunning();

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminObserverStop", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_BAN_CHAT_STATE:
	{
		TAdminManagerBanClientPlayerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		rkAdmin.Ban_UpdateChatInfo(&kInfo);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanChatUpdate", Py_BuildValue("(i)", kInfo.dwPID));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_BAN_CHAT_SEARCH_PLAYER:
	{
		bool bSuccess;
		if (!Recv(sizeof(bool), &bSuccess))
		{
			return false;
		}

		if (bSuccess)
		{
			TAdminManagerBanClientPlayerInfo kPlayerInfo;
			if (!Recv(sizeof(kPlayerInfo), &kPlayerInfo))
			{
				return false;
			}

			CPythonAdmin::Instance().Ban_SetChatSearchResult(&kPlayerInfo);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanChatSearchResult", Py_BuildValue("(b)", true));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanChatSearchResult", Py_BuildValue("(b)", false));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_BAN_PLAYER_LOG:
	{
		DWORD dwPlayerPID;
		if (!Recv(sizeof(DWORD), &dwPlayerPID))
		{
			return false;
		}

		DWORD dwLogEntries;
		if (!Recv(sizeof(DWORD), &dwLogEntries))
		{
			return false;
		}

		CPythonAdmin::Instance().Ban_ClearLogInfo();
		for (int i = 0; i < dwLogEntries; ++i)
		{
			TAdminManagerBanClientLogInfo kInfo;
			if (!Recv(sizeof(kInfo), &kInfo))
			{
				return false;
			}

			char* pszReason = new char[kInfo.iReasonStrLen + 1];
			if (!Recv(kInfo.iReasonStrLen, pszReason))
			{
				return false;
			}
			pszReason[kInfo.iReasonStrLen] = '\0';

			char* pszProof = new char[kInfo.iProofStrLen + 1];
			if (!Recv(kInfo.iProofStrLen, pszProof))
			{
				return false;
			}
			pszProof[kInfo.iProofStrLen] = '\0';

			CPythonAdmin::Instance().Ban_AddLogInfo(&kInfo, pszReason, pszProof);
			delete[] pszReason;
			delete[] pszProof;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanLoadLog", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_BAN_ACCOUNT_STATE:
	{
		TAdminManagerBanClientAccountInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		rkAdmin.Ban_UpdateAccountInfo(&kInfo);

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanAccountUpdate", Py_BuildValue("(i)", kInfo.dwAID));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_BAN_ACCOUNT_SEARCH:
	{
		bool bSuccess;
		if (!Recv(sizeof(bool), &bSuccess))
		{
			return false;
		}

		if (bSuccess)
		{
			TAdminManagerBanClientAccountInfo kAccountInfo;
			if (!Recv(sizeof(kAccountInfo), &kAccountInfo))
			{
				return false;
			}

			CPythonAdmin::Instance().Ban_SetAccountSearchResult(&kAccountInfo);
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanAccountSearchResult", Py_BuildValue("(b)", true));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminBanAccountSearchResult", Py_BuildValue("(b)", false));
		}
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_ITEM_SEARCH:
	{
		DWORD dwItemCount;
		if (!Recv(sizeof(DWORD), &dwItemCount))
		{
			return false;
		}

		CPythonAdmin::Instance().Item_ClearInfo();
		for (int i = 0; i < dwItemCount; ++i)
		{
			TAdminManagerItemInfo kItemInfo;
			if (!Recv(sizeof(kItemInfo), &kItemInfo))
			{
				return false;
			}

			char* szOwnerName = new char[kItemInfo.bOwnerNameLen + 1];
			if (kItemInfo.bOwnerNameLen)
			{
				if (!Recv(kItemInfo.bOwnerNameLen, szOwnerName))
				{
					return false;
				}
			}
			szOwnerName[kItemInfo.bOwnerNameLen] = '\0';

			CPythonAdmin::Instance().Item_AddInfo(&kItemInfo, szOwnerName);
			delete[] szOwnerName;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminItemLoadResult", Py_BuildValue("()"));
	}
	break;

	case ADMIN_MANAGER_GC_SUBHEADER_RECV_LOGS:
	{
		DWORD dwItemCount;
		if (!Recv(sizeof(DWORD), &dwItemCount))
		{
			return false;
		}

		CPythonAdmin::Instance().Hack_ClearInfo();
		for (int i = 0; i < dwItemCount; ++i)
		{
			TAdminManagerHackLog kHackInfo;
			if (!Recv(sizeof(kHackInfo), &kHackInfo))
			{
				return false;
			}

			char* pszHackerName = new char[kHackInfo.dwHackerNameLen + 1];
			if (!Recv(kHackInfo.dwHackerNameLen, pszHackerName))
			{
				return false;
			}

			pszHackerName[kHackInfo.dwHackerNameLen] = '\0';

			char* pszReason = new char[kHackInfo.dwHackerReasonLen + 1];
			if (!Recv(kHackInfo.dwHackerReasonLen, pszReason))
			{
				return false;
			}

			pszReason[kHackInfo.dwHackerReasonLen] = '\0';

			char* pszTime = new char[kHackInfo.dwHackTimeLen + 1];
			if (!Recv(kHackInfo.dwHackTimeLen, pszTime))
			{
				return false;
			}

			pszTime[kHackInfo.dwHackTimeLen] = '\0';

			CPythonAdmin::Instance().Hack_AddInfo(&kHackInfo, i, pszHackerName, pszReason, pszTime);
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AdminHackLogsLoadResult", Py_BuildValue("()"));
	}
	break;
	}

	return true;
}

bool CPythonNetworkStream::SendAdminManagerHead(BYTE bSubHeader)
{
	TPacketCGAdminManager packet;
	packet.header = HEADER_CG_ADMIN_MANAGER;
	packet.sub_header = bSubHeader;
	if (!Send(sizeof(packet), &packet))
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::SendAdminManager(BYTE bSubHeader, const void* c_pvData, int iSize)
{
	if (!SendAdminManagerHead(bSubHeader))
	{
		return false;
	}

	if (iSize && !Send(iSize, c_pvData))
	{
		return false;
	}

	return true;
}
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
bool CPythonNetworkStream::SendBiologManagerAction(BYTE bSubHeader)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGBiologManagerAction packet;
	packet.bHeader = HEADER_CG_BIOLOG_MANAGER;
	packet.bSubHeader = bSubHeader;

	if (!Send(sizeof(TPacketCGBiologManagerAction), &packet))
	{
		Tracef("SendBiologManagerAction Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvBiologManager()
{
	TPacketGCBiologManager packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("RecvBiologManager Error");
		return false;
	}

	switch (packet.bSubHeader)
	{
	case GC_BIOLOG_MANAGER_OPEN:
	{
		TPacketGCBiologManagerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonBiologManager::Instance()._LoadBiologInformation(&kInfo);
		if (kInfo.bUpdate)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerUpdate", Py_BuildValue("()"));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerOpen", Py_BuildValue("()"));
		}
	}
	break;
	case GC_BIOLOG_MANAGER_ALERT:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerAlert", Py_BuildValue("()"));
	}
	break;
	case GC_BIOLOG_MANAGER_CLOSE:
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_BiologManagerClose", Py_BuildValue("()"));
	}
	break;
	}
	return true;
}
#endif

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
bool CPythonNetworkStream::SendMarbleManagerAction(BYTE bSubHeader)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGMarbleManagerAction packet;
	packet.bHeader = HEADER_CG_MARBLE_MANAGER;
	packet.bSubHeader = bSubHeader;

	if (!Send(sizeof(TPacketCGMarbleManagerAction), &packet))
	{
		Tracef("SendMarbleManagerAction Send Packet Error\n");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::RecvMarbleManager()
{
	TPacketGCMarbleManager packet;
	if (!Recv(sizeof(packet), &packet))
	{
		TraceError("TPacketGCMarbleManager Error");
		return false;
	}

	switch (packet.bSubHeader)
	{
	case GC_MARBLE_MANAGER_DATA:
	{
		TPacketGCMarbleManagerInfo kInfo;
		if (!Recv(sizeof(kInfo), &kInfo))
		{
			return false;
		}

		CPythonMarbleManager::Instance().RegisterMarbleInfo(kInfo.bID, kInfo.dwMarbleVnum, kInfo.wRequiredKillCount, kInfo.bActiveMission, kInfo.bActiveExtandedCount, kInfo.wKilledMonsters, kInfo.tCooldownTime);
	}
	break;

	case GC_MARBLE_MANAGER_REFRESH:
	{
		CPythonMarbleManager::instance().ClearMarbleData();
	}
	break;

	case GC_MARBLE_MANAGER_OPEN:
	{
		BOOL bUpdate;
		if (!Recv(sizeof(BOOL), &bUpdate))
		{
			return false;
		}

		if (bUpdate)
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_MarbleManagerUpdate", Py_BuildValue("()"));
		}
		else
		{
			PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_MarbleManagerOpen", Py_BuildValue("()"));
		}
	}
	break;
	}
	return true;
}
#endif

#ifdef ENABLE_CUBE_RENEWAL
bool CPythonNetworkStream::RecvCubeItemPacket()
{
	TPacketGCCubeItem packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	packet.wSize -= sizeof(packet);

	if (packet.wSize == 0)
	{
		return true;
	}

	int i = 0;

	while (packet.wSize > 0)
	{
		TCubeDataP cubeItem;
		if (!Recv(sizeof(cubeItem), &cubeItem))
		{
			return false;
		}

		TCubeData itemCube;
		itemCube.iIndex = cubeItem.iIndex;
		itemCube.tReward = cubeItem.tReward;
		itemCube.tIncItem = cubeItem.tIncItem;
		itemCube.iIncPercent = cubeItem.iIncPercent;
		itemCube.iPercent = cubeItem.iPercent;
		itemCube.dwGold = cubeItem.dwGold;

		itemCube.bCategory = cubeItem.bCategory;
#ifdef ENABLE_CUBE_RENEWAL_COPY_BONUS
		itemCube.bCopyBonus = cubeItem.bCopyBonus;
#endif
		for (i = 0; i < CUBE_MATERIAL_MAX_NUM; ++i)
		{
			if (!cubeItem.materialV[i].dwVnum)
			{
				continue;
			}

			itemCube.materialV.push_back( cubeItem.materialV[i] );
		}

		CPythonPlayer::Instance().SetCubeItem(itemCube, packet.dwNpcVnum);

		packet.wSize -= sizeof(cubeItem);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Open", Py_BuildValue("(i)", packet.dwNpcVnum));

	return true;
}

bool CPythonNetworkStream::RecvCubePacket()
{
	TPacketGCCubeCraft packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_Cube_Make", Py_BuildValue("(ii)", packet.bErrorType, packet.dwErrorArg));
	return true;
}

bool CPythonNetworkStream::SendCubeClose()
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	BYTE bHeader = HEADER_CG_CUBE_CLOSE;
	if (!Send(sizeof(BYTE), &bHeader))
	{
		Tracen("SendCubeClose Send Packet Error\n");
		return false;
	}

	return true;
}

bool CPythonNetworkStream::SendCubeMake(DWORD dwNpcVnum, int iIndex, BYTE bMakeCount, BYTE bIsIncrease)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGCubeMake packet;
	packet.bHeader = HEADER_CG_CUBE_MAKE;
	packet.dwNpcVnum = dwNpcVnum;
	packet.iIndex = iIndex;
	packet.bMakeCount = bMakeCount;
	packet.bIsIncrease = bIsIncrease;

	if (!Send(sizeof(TPacketCGCubeMake), &packet))
	{
		Tracef("SendCubeMake Send Packet Error\n");
		return false;
	}

	return SendSequence();
}
#endif

#ifdef ENABLE_FIND_LETTERS_EVENT
bool CPythonNetworkStream::RecvFindLettersInfo()
{
	TPacketGCFindLettersInfo packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	packet.wSize -= sizeof(packet);

	int iPos = 0;
	while (packet.wSize > 0)
	{
		TPlayerLetterReward pReward;
		if (!Recv(sizeof(pReward), &pReward))
		{
			return false;
		}

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddFindLettersReward", Py_BuildValue("(iii)",
							  iPos, pReward.dwVnum, pReward.bCount));

		iPos++;
		packet.wSize -= sizeof(pReward);
	}

	for (int i = 0; i < FIND_LETTERS_SLOTS_NUM; i++)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_AddFindLetters", Py_BuildValue("(iii)",
							  i, packet.letterSlots[i].bAsciiChar, packet.letterSlots[i].bIsFilled));
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenFindLetters", Py_BuildValue("()"));
	return true;
}
#endif

#ifdef ENABLE_REFINE_ELEMENT
bool CPythonNetworkStream::RecvRefineElementPacket()
{
	TPacketGCRefineElement packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME],
						  "BINARY_RefineElementProcess",
						  Py_BuildValue("(iii)",
										packet.bType, packet.wSrcCell, packet.wDstCell));

	return true;
}

bool CPythonNetworkStream::SendRefineElementPacket(BYTE bArg)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGRefineElement packet;
	packet.bHeader = HEADER_CG_REFINE_ELEMENT;
	packet.bArg = bArg;

	if (!Send(sizeof(TPacketCGRefineElement), &packet))
	{
		TraceError("SendRefineElementPacket Error - Arg %d", bArg);
		return false;
	}

	return true;
}
#endif

#ifdef ENABLE_LUCKY_BOX
bool CPythonNetworkStream::RecvLuckyBox()
{
	TPacketGCLuckyBox kPacket;
	if (!Recv(sizeof(kPacket), &kPacket))
	{
		return false;
	}

	CPythonPlayer::Instance().SetLuckyBoxVnum(kPacket.dwBoxVnum);
	CPythonPlayer::Instance().SetLuckyBoxPrice(kPacket.dwPrice);

	CPythonPlayer::Instance().ClearLuckyBoxVector();

	for (int i = 0; i < LUCKY_BOX_MAX_NUM; ++i)
	{
		TLuckyBoxItem luckyItem;
		luckyItem.dwVnum = kPacket.luckyItems[i].dwVnum;
		luckyItem.bCount = kPacket.luckyItems[i].bCount;
		luckyItem.bIsReward = kPacket.luckyItems[i].bIsReward;

		CPythonPlayer::Instance().SetLuckyBoxItem(luckyItem);
	}

	if (kPacket.bIsOpen)
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_OpenLuckyBox", Py_BuildValue("()"));
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_RefreshLuckyBox", Py_BuildValue("()"));
	}

	return true;
}

bool CPythonNetworkStream::SendLuckyBoxAction(BYTE bAction)
{
	if (!__CanActMainInstance())
	{
		return true;
	}

	TPacketCGLuckyBox packet;
	packet.bHeader = HEADER_CG_LUCKY_BOX;
	packet.bAction = bAction;

	if (!Send(sizeof(TPacketCGLuckyBox), &packet))
	{
		Tracef("SendLuckyBoxAction Error\n");
		return false;
	}

	return true;
}
#endif

#ifdef ENABLE_TEAMLER_STATUS
bool CPythonNetworkStream::RecvShowTeamler()
{
	TPacketGCShowTeamler packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	CPythonPlayer::Instance().SetIsShowTeamler(packet.is_show);
	CPythonMessenger::Instance().RefreshTeamlerState();

	return true;
}

bool CPythonNetworkStream::RecvTeamlerStatus()
{
	TPacketGCTeamlerStatus packet;
	if (!Recv(sizeof(packet), &packet))
	{
		return false;
	}

	if (packet.is_online)
	{
		CPythonMessenger::Instance().OnTeamLogin(packet.szName);
	}
	else
	{
		CPythonMessenger::Instance().OnTeamLogout(packet.szName);
	}

	return true;
}
#endif

#ifdef INGAME_WIKI
extern PyObject* wikiModule;

bool CPythonNetworkStream::SendWikiRequestInfo(unsigned long long retID, DWORD vnum, bool isMob)
{
	InGameWiki::TCGWikiPacket pack;
	pack.vnum = vnum;
	pack.is_mob = isMob;
	pack.ret_id = retID;

	if (!Send(sizeof(InGameWiki::TCGWikiPacket), &pack))
	{
		return false;
	}

	return true;
}

bool CPythonNetworkStream::RecvWikiPacket()
{
	InGameWiki::TGCWikiPacket pack;
	if (!Recv(sizeof(InGameWiki::TGCWikiPacket), &pack))
	{
		return false;
	}

	WORD iPacketSize = pack.size - sizeof(InGameWiki::TGCWikiPacket);
	if (iPacketSize <= 0)
	{
		return false;
	}

	unsigned long long ret_id = 0;
	DWORD data_vnum = 0;

	if (pack.is_data_type(InGameWiki::LOAD_WIKI_ITEM))
	{
		const size_t recv_size = sizeof(InGameWiki::TGCItemWikiPacket);
		iPacketSize -= WORD(recv_size);

		InGameWiki::TGCItemWikiPacket item_data;
		if (!Recv(recv_size, &item_data))
		{
			return false;
		}

		ret_id = item_data.ret_id;
		data_vnum = item_data.vnum;

		CItemData* pData = nullptr;
		if (!CItemManager::instance().GetItemDataPointer(item_data.vnum, &pData))
		{
			TraceError("Cant get pointer from item -> %d", item_data.vnum);
			return false;
		}

		auto& recv_wiki = item_data.wiki_info;
		auto wikiInfo = pData->GetWikiTable();

		const int origin_size = item_data.origin_infos_count;
		const int chest_info_count = recv_wiki.chest_info_count;
		const int refine_infos_count = recv_wiki.refine_infos_count;

		wikiInfo->isSet = true;
		wikiInfo->hasData = true;
		wikiInfo->bIsCommon = recv_wiki.is_common;
		wikiInfo->dwOrigin = recv_wiki.origin_vnum;
		wikiInfo->maxRefineLevel = refine_infos_count;

		{
			wikiInfo->pOriginInfo.clear();
			const size_t origin_info_recv_base_size = sizeof(CommonWikiData::TWikiItemOriginInfo);

			for (int idx = 0; idx < origin_size; ++idx)
			{
				CommonWikiData::TWikiItemOriginInfo origin_data;
				if (!Recv(origin_info_recv_base_size, &origin_data))
				{
					return false;
				}

				wikiInfo->pOriginInfo.emplace_back(origin_data);
				iPacketSize -= WORD(origin_info_recv_base_size);
			}


			wikiInfo->pChestInfo.clear();
			const size_t chest_info_recv_base_size = sizeof(CommonWikiData::TWikiChestInfo);

			for (int idx = 0; idx < chest_info_count; ++idx)
			{
				CommonWikiData::TWikiChestInfo chest_data;
				if (!Recv(chest_info_recv_base_size, &chest_data))
				{
					return false;
				}

				wikiInfo->pChestInfo.emplace_back(chest_data);
				iPacketSize -= WORD(chest_info_recv_base_size);
			}

			wikiInfo->pRefineData.clear();
			const size_t refine_info_recv_base_size = sizeof(CommonWikiData::TWikiRefineInfo);

			for (int idx = 0; idx < refine_infos_count; ++idx)
			{
				CommonWikiData::TWikiRefineInfo refine_info_data;
				if (!Recv(refine_info_recv_base_size, &refine_info_data))
				{
					return false;
				}

				wikiInfo->pRefineData.emplace_back(refine_info_data);
				iPacketSize -= WORD(refine_info_recv_base_size);
			}
		}

		if (iPacketSize != 0)
		{
			return false;
		}
	}
	else
	{
		const size_t recv_size = sizeof(InGameWiki::TGCMobWikiPacket);
		iPacketSize -= WORD(recv_size);

		InGameWiki::TGCMobWikiPacket mob_data;
		if (!Recv(recv_size, &mob_data))
		{
			return false;
		}

		ret_id = mob_data.ret_id;
		data_vnum = mob_data.vnum;
		const int drop_info_count = mob_data.drop_info_count;

		CPythonNonPlayer::TWikiInfoTable* mobData = nullptr;
		if (!(mobData = CPythonNonPlayer::instance().GetWikiTable(mob_data.vnum)))
		{
			TraceError("Cant get mob data from monster -> %d", mob_data.vnum);
			return false;
		}

		mobData->isSet = (drop_info_count > 0);

		{
			mobData->dropList.clear();
			const size_t mob_drop_info_recv_base_size = sizeof(CommonWikiData::TWikiMobDropInfo);

			for (int idx = 0; idx < drop_info_count; ++idx)
			{
				CommonWikiData::TWikiMobDropInfo drop_data;
				if (!Recv(mob_drop_info_recv_base_size, &drop_data))
				{
					return false;
				}

				mobData->dropList.push_back(drop_data);
				iPacketSize -= WORD(mob_drop_info_recv_base_size);
			}
		}

		if (iPacketSize != 0)
		{
			return false;
		}
	}

	if (wikiModule)
	{
		PyCallClassMemberFunc(wikiModule, "BINARY_LoadInfo", Py_BuildValue("(Li)", (long long)ret_id, data_vnum));
	}

	return true;
}
#endif

#ifdef ENABLE_SHOP_SEARCH
bool CPythonNetworkStream::RecvShopSearchResult()
{
	TPacketGCShopSearchResult pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	CPythonShopSearch::Instance().SetResultMaxPage(pack.maxPageNum);

	CPythonShopSearch::Instance().ClearResultItems();
	for (int i = 0; i < pack.itemCount; ++i)
	{
		TShopSearchClientItem itemData;
		if (!Recv(sizeof(itemData), &itemData))
		{
			TraceError("Bad size of packet");
			return false;
		}

		CPythonShopSearch::Instance().AppendResultItem(itemData);
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ShopSearch_RecvResult", Py_BuildValue("()"));

	return true;
}

bool CPythonNetworkStream::RecvShopSearchBuyResult()
{
	TPacketGCShopSearchBuyResult pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ShopSearch_RecvBuyDone", Py_BuildValue("(i)", pack.result));

	return true;
}

bool CPythonNetworkStream::RecvShopSearchOwnerMessage()
{
	TPacketGCShopSearchOwnerMessage pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ShopSearch_RecvOwnerMessage", Py_BuildValue("(s)", pack.ownerName));

	return true;
}

bool CPythonNetworkStream::RecvShopSearchSoldInfo()
{
	TPacketGCShopSearchSoldInfo pack;
	if (!Recv(sizeof(pack), &pack))
	{
		return false;
	}

	CPythonShopSearch::Instance().ClearSoldItemInfo();

	if (pack.results)
	{
		for (int i = 0; i < SHOPSEARCH_SOLD_ITEM_INFO_COUNT; ++i)
		{
			TShopSearchSoldItemInfo info;
			if (!Recv(sizeof(info), &info))
			{
				return false;
			}

			CPythonShopSearch::Instance().SetSoldItemInfo(i, info);
		}
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_ShopSearch_RecvSoldInfo", Py_BuildValue("(b)", pack.results));

	return true;
}

bool CPythonNetworkStream::SendShopSearchByName(const std::string& itemName, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	TPacketCGShopSearchByName pack;
	pack.header = HEADER_CG_SHOP_SEARCH_BY_NAME;
	strncpy(pack.itemName, itemName.c_str(), sizeof(pack.itemName));
	pack.itemName[CItemData::ITEM_NAME_MAX_LEN] = '\0'; // strncpy is not automatically 0-terminated
	pack.page = page;
	pack.entryCountIdx = entryCountIdx;
	pack.sortType = sortType;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSearchByOptions(const TShopSearchOptions& options, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	TPacketCGShopSearchByOptions pack;
	pack.header = HEADER_CG_SHOP_SEARCH_BY_OPTION;
	pack.options = options;
	pack.page = page;
	pack.entryCountIdx = entryCountIdx;
	pack.sortType = sortType;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	if (options.typeFlagCount && !Send(options.typeFlagCount * sizeof(TShopSearchItemType), itemTypeFlags))
	{
		return false;
	}

	if (options.specificVnumCount && !Send(options.specificVnumCount * sizeof(DWORD), vnumList))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSearchBuy(TShopSearchItemID itemID, DWORD itemVnum, int64_t itemPrice)
{
	TPacketCGShopSearchBuy pack;
	pack.header = HEADER_CG_SHOP_SEARCH_BUY;
	pack.itemID = itemID;
	pack.itemVnum = itemVnum;
	pack.itemPrice = itemPrice;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSearchOwnerMessage(DWORD ownerID)
{
	TPacketCGShopSearchOwnerMessage pack;
	pack.header = HEADER_CG_SHOP_SEARCH_OWNER_MESSAGE;
	pack.ownerID = ownerID;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendShopSearchRequestSoldInfo(DWORD itemVnum)
{
	TPacketCGShopSearchRequestSoldInfo pack;
	pack.header = HEADER_CG_SHOP_SEARCH_REQUEST_SOLD_INFO;
	pack.itemVnum = itemVnum;

	if (!Send(sizeof(pack), &pack))
	{
		return false;
	}

	return SendSequence();
}
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
bool CPythonNetworkStream::RecvWhisperLanguageInfo()
{
	TPacketGCWhisperLanguageInfo pack;
	if (!Recv(sizeof(pack), &pack))
		return false;

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_GAME], "BINARY_WhisperLanguageInfo", Py_BuildValue("(si)", pack.name, pack.language));

	return true;
}

bool CPythonNetworkStream::SendWhisperRequestLanguage(const std::string& targetName)
{
	TPacketCGWhisperRequestLanguage pack;
	pack.header = HEADER_CG_WHISPER_REQUEST_LANGUAGE;
	strcpy_s(pack.name, targetName.c_str());

	if (!Send(sizeof(pack), &pack))
		return false;

	return SendSequence();
}
#endif

#ifdef ENABLE_PING_TIME
bool CPythonNetworkStream::RecvPingTimer()
{
	Recv(sizeof(BYTE));

	if (!m_waitForPingTimer)
	{
		return true;
	}

	m_waitForPingTimer = false;
	m_lastPingTime = ELTimer_GetMSec() - m_lastPingTimerSent;

	return true;
}
#endif

