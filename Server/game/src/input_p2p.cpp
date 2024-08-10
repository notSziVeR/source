#include "stdafx.h"
#include "../../common/billing.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "p2p.h"
#include "guild.h"
#include "guild_manager.h"
#include "party.h"
#include "messenger_manager.h"
#include "empire_text_convert.h"
#include "unique_item.h"
#include "xmas_event.h"
#include "affect.h"
#include "castle.h"
#include "dev_log.h"
#include "locale_service.h"
#include "questmanager.h"
#include "pcbang.h"
#include "skill.h"
#include "threeway_war.h"

#include "map_location.h"
#include "dungeon.h"

#ifdef __GAYA_SHOP_SYSTEM__
	#include "GayaSystemManager.hpp"
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	#include "EventManager.hpp"
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	#include "TechnicalMaintenance.hpp"
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	#include "MonsterRespawner.hpp"
#endif

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
	#include "log.h"
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	#include "gm.h"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	#include "dungeon.h"
	#include "sectree_manager.h"
#endif

#ifdef __DUNGEON_RETURN_ENABLE__
	#include "questmanager.h"
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	#include "TombolaManager.hpp"
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

////////////////////////////////////////////////////////////////////////////////
// Input Processor
CInputP2P::CInputP2P()
{
	BindPacketInfo(&m_packetInfoGG);
}

void CInputP2P::Login(LPDESC d, const char * c_pData)
{
	P2P_MANAGER::instance().Login(d, (TPacketGGLogin *) c_pData);
}

void CInputP2P::Logout(LPDESC d, const char * c_pData)
{
	TPacketGGLogout * p = (TPacketGGLogout *) c_pData;
	P2P_MANAGER::instance().Logout(p->szName);
}

int CInputP2P::Relay(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGRelay * p = (TPacketGGRelay *) c_pData;

	if (uiBytes < sizeof(TPacketGGRelay) + p->lSize)
	{
		return -1;
	}

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	sys_log(0, "InputP2P::Relay : %s size %d", p->szName, p->lSize);

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(p->szName);

	const BYTE* c_pbData = (const BYTE *) (c_pData + sizeof(TPacketGGRelay));

	if (!pkChr)
	{
		return p->lSize;
	}

	if (*c_pbData == HEADER_GC_WHISPER)
	{
		if (pkChr->IsBlockMode(BLOCK_WHISPER))
		{

			return p->lSize;
		}

		char buf[1024];
		memcpy(buf, c_pbData, MIN(p->lSize, sizeof(buf)));

		TPacketGCWhisper* p2 = (TPacketGCWhisper*) buf;

		BYTE bToEmpire = (p2->bType >> 4);
		p2->bType = p2->bType & 0x0F;
		if (p2->bType == 0x0F)
		{

			p2->bType = WHISPER_TYPE_SYSTEM;
		}
		else
		{
			if (!pkChr->IsEquipUniqueGroup(UNIQUE_GROUP_RING_OF_LANGUAGE))
				if (bToEmpire >= 1 && bToEmpire <= 3 && pkChr->GetEmpire() != bToEmpire)
				{
					ConvertEmpireText(bToEmpire,
									  buf + sizeof(TPacketGCWhisper),
									  p2->wSize - sizeof(TPacketGCWhisper),
									  10 + 2 * pkChr->GetSkillPower(SKILL_LANGUAGE1 + bToEmpire - 1));
				}
		}

		pkChr->GetDesc()->Packet(buf, p->lSize);
	}
	else
	{
		pkChr->GetDesc()->Packet(c_pbData, p->lSize);
	}

	return (p->lSize);
}

int CInputP2P::Notice(LPDESC d, const char * c_pData, size_t uiBytes)
{
	TPacketGGNotice * p = (TPacketGGNotice *) c_pData;

	if (uiBytes < sizeof(TPacketGGNotice) + p->lSize)
	{
		return -1;
	}

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256 + 1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendNotice(szBuf);
	return (p->lSize);
}

int CInputP2P::BigNotice(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGBigNotice* p = (TPacketGGBigNotice*)c_pData;

	if (uiBytes < sizeof(TPacketGGBigNotice) + p->lSize)
	{
		return -1;
	}

	if (p->lSize < 0)
	{
		sys_err("invalid packet length %d", p->lSize);
		d->SetPhase(PHASE_CLOSE);
		return -1;
	}

	char szBuf[256 + 1];
	strlcpy(szBuf, c_pData + sizeof(TPacketGGBigNotice), MIN(p->lSize + 1, sizeof(szBuf)));
	SendBigNotice(szBuf);
	return (p->lSize);
}

int CInputP2P::Guild(LPDESC d, const char* c_pData, size_t uiBytes)
{
	TPacketGGGuild * p = (TPacketGGGuild *) c_pData;
	uiBytes -= sizeof(TPacketGGGuild);
	c_pData += sizeof(TPacketGGGuild);

	CGuild * g = CGuildManager::instance().FindGuild(p->dwGuild);

	switch (p->bSubHeader)
	{
	case GUILD_SUBHEADER_GG_CHAT:
	{
		if (uiBytes < sizeof(TPacketGGGuildChat))
		{
			return -1;
		}

		TPacketGGGuildChat * p = (TPacketGGGuildChat *) c_pData;

		if (g)
		{
			g->P2PChat(p->szText);
		}

		return sizeof(TPacketGGGuildChat);
	}

	case GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS:
	{
		if (uiBytes < sizeof(int))
		{
			return -1;
		}

		int iBonus = *((int *) c_pData);
		CGuild* pGuild = CGuildManager::instance().FindGuild(p->dwGuild);
		if (pGuild)
		{
			pGuild->SetMemberCountBonus(iBonus);
		}
		return sizeof(int);
	}
	default:
		sys_err ("UNKNOWN GUILD SUB PACKET");
		break;
	}
	return 0;
}


struct FuncShout
{
	const char * m_str;
	BYTE m_bEmpire;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	BYTE m_bLocale;
#endif

	FuncShout(const char * str, BYTE bEmpire
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, BYTE bLocale
#endif
	) : m_str(str), m_bEmpire(bEmpire)
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, m_bLocale(bLocale)
#endif
	{
	}

	void operator () (LPDESC d)
	{
#ifdef __ENABLE_NEWSTUFF__
		if (!d->GetCharacter() || (!g_bGlobalShoutEnable && d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
		{
			return;
		}
#else
		if (!d->GetCharacter() || (d->GetCharacter()->GetGMLevel() == GM_PLAYER && d->GetEmpire() != m_bEmpire))
		{
			return;
		}
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
		d->GetCharacter()->RawChatPacket(CHAT_TYPE_SHOUT, m_bLocale, "%s", m_str);
#else
		d->GetCharacter()->ChatPacket(CHAT_TYPE_SHOUT, "%s", m_str);
#endif
			
	}
};

void SendShout(const char * szText, BYTE bEmpire
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, BYTE bLocale
#endif
)
{
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), FuncShout(szText, bEmpire
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, bLocale
#endif
	));
}

void CInputP2P::Shout(const char * c_pData)
{
	TPacketGGShout * p = (TPacketGGShout *) c_pData;
	SendShout(p->szText, p->bEmpire
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, p->bLocale
#endif
	);
}

void CInputP2P::Disconnect(const char * c_pData)
{
	TPacketGGDisconnect * p = (TPacketGGDisconnect *) c_pData;

	LPDESC d = DESC_MANAGER::instance().FindByLoginName(p->szLogin);

	if (!d)
	{
		return;
	}

	if (!d->GetCharacter())
	{
		d->SetPhase(PHASE_CLOSE);
	}
	else
	{
		d->DisconnectOfSameLogin();
	}
}

void CInputP2P::Setup(LPDESC d, const char * c_pData)
{
	TPacketGGSetup * p = (TPacketGGSetup *) c_pData;
	sys_log(0, "P2P: Setup %s:%d", d->GetHostName(), p->wPort);
	d->SetP2P(d->GetHostName(), p->wPort, p->bChannel);
#ifdef __ADMIN_MANAGER__
	d->SetListenPort(p->wListenPort);
#endif
}

void CInputP2P::MessengerAdd(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Add %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__AddToList(p->szAccount, p->szCompanion);
}

void CInputP2P::MessengerRemove(const char * c_pData)
{
	TPacketGGMessenger * p = (TPacketGGMessenger *) c_pData;
	sys_log(0, "P2P: Messenger Remove %s %s", p->szAccount, p->szCompanion);
	MessengerManager::instance().__RemoveFromList(p->szAccount, p->szCompanion);
}

void CInputP2P::FindPosition(LPDESC d, const char* c_pData)
{
	TPacketGGFindPosition* p = (TPacketGGFindPosition*)c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->dwTargetPID);
	if (ch && (ch->GetMapIndex() < 10000 || p->bAllowDungeon))
	{
		TPacketGGWarpCharacter pw;
		pw.header = HEADER_GG_WARP_CHARACTER;
		pw.pid = p->dwFromPID;
		pw.x = ch->GetX();
		pw.y = ch->GetY();
		pw.real_map_index = ch->GetMapIndex();

		if (!CMapLocation::instance().Get(pw.x, pw.y, pw.map_index, pw.addr, pw.port))
		{
			sys_err("cannot find map location for FindPosition index %d x %d y %d name %s", pw.map_index, pw.x, pw.y, ch->GetName());
			return;
		}

		d->Packet(&pw, sizeof(pw));
	}
}

void CInputP2P::WarpCharacter(const char* c_pData)
{
	TPacketGGWarpCharacter* p = (TPacketGGWarpCharacter*)c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(p->pid);
	if (ch)
	{
		ch->WarpSet(p->x, p->y, p->real_map_index, p->map_index, p->addr, p->port);
	}
}

void CInputP2P::GuildWarZoneMapIndex(const char* c_pData)
{
	TPacketGGGuildWarMapIndex * p = (TPacketGGGuildWarMapIndex*) c_pData;
	CGuildManager & gm = CGuildManager::instance();

	sys_log(0, "P2P: GuildWarZoneMapIndex g1(%u) vs g2(%u), mapIndex(%d)", p->dwGuildID1, p->dwGuildID2, p->lMapIndex);

	CGuild * g1 = gm.FindGuild(p->dwGuildID1);
	CGuild * g2 = gm.FindGuild(p->dwGuildID2);

	if (g1 && g2)
	{
		g1->SetGuildWarMapIndex(p->dwGuildID2, p->lMapIndex);
		g2->SetGuildWarMapIndex(p->dwGuildID1, p->lMapIndex);
	}
}

void CInputP2P::Transfer(const char * c_pData)
{
	TPacketGGTransfer * p = (TPacketGGTransfer *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		ch->WarpSet(p->lX, p->lY);
	}
}

void CInputP2P::XmasWarpSanta(const char * c_pData)
{
	TPacketGGXmasWarpSanta * p = (TPacketGGXmasWarpSanta *) c_pData;

	if (p->bChannel == g_bChannel && map_allow_find(p->lMapIndex))
	{
		int	iNextSpawnDelay = 50 * 60;

		xmas::SpawnSanta(p->lMapIndex, iNextSpawnDelay);

		TPacketGGXmasWarpSantaReply pack_reply;
		pack_reply.bHeader = HEADER_GG_XMAS_WARP_SANTA_REPLY;
		pack_reply.bChannel = g_bChannel;
		P2P_MANAGER::instance().Send(&pack_reply, sizeof(pack_reply));
	}
}

void CInputP2P::XmasWarpSantaReply(const char* c_pData)
{
	TPacketGGXmasWarpSantaReply* p = (TPacketGGXmasWarpSantaReply*) c_pData;

	if (p->bChannel == g_bChannel)
	{
		CharacterVectorInteractor i;

		if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_SANTA_VNUM, i))
		{
			CharacterVectorInteractor::iterator it = i.begin();

			while (it != i.end())
			{
				M2_DESTROY_CHARACTER(*it++);
			}
		}
	}
}

void CInputP2P::LoginPing(LPDESC d, const char * c_pData)
{
	TPacketGGLoginPing * p = (TPacketGGLoginPing *) c_pData;

	SendBillingExpire(p->szLogin, BILLING_DAY, 0, NULL);

	if (!g_pkAuthMasterDesc) // If I am master, I have to broadcast
	{
		P2P_MANAGER::instance().Send(p, sizeof(TPacketGGLoginPing), d);
	}
}

// BLOCK_CHAT
#ifdef __ADMIN_MANAGER__
void CInputP2P::BlockChat(const char* c_pData)
{
	TPacketGGBlockChat* p = (TPacketGGBlockChat*)c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		sys_log(0, "BLOCK CHAT apply name %s dur %d", p->szName, p->lBlockDuration);

		int iOldDuration = 0;
		if (CAffect* pAff = ch->FindAffect(AFFECT_BLOCK_CHAT))
		{
			iOldDuration = pAff->lDuration;
		}
		LogManager::instance().ChatBanLog(ch, p->dwGMPid, p->szGMName, p->lBlockDuration - iOldDuration, p->lBlockDuration, p->szDesc, p->szProof,
										  p->bIncreaseBanCounter);

		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, p->lBlockDuration, 0, true);
		if (p->bIncreaseBanCounter)
		{
			ch->SetChatBanCount(ch->GetChatBanCount() + 1);
		}
	}
	else
	{
		sys_log(0, "BLOCK CHAT fail name %s dur %d", p->szName, p->lBlockDuration);
	}
}
#else
void CInputP2P::BlockChat(const char * c_pData)
{
	TPacketGGBlockChat * p = (TPacketGGBlockChat *) c_pData;

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindPC(p->szName);

	if (ch)
	{
		sys_log(0, "BLOCK CHAT apply name %s dur %d", p->szName, p->lBlockDuration);
		ch->AddAffect(AFFECT_BLOCK_CHAT, POINT_NONE, 0, AFF_NONE, p->lBlockDuration, 0, true);
	}
	else
	{
		sys_log(0, "BLOCK CHAT fail name %s dur %d", p->szName, p->lBlockDuration);
	}
}
#endif
// END_OF_BLOCK_CHAT
//

void CInputP2P::PCBangUpdate(const char* c_pData)
{
	TPacketPCBangUpdate* p = (TPacketPCBangUpdate*)c_pData;

	CPCBangManager::instance().RequestUpdateIPList(p->ulPCBangID);
}

void CInputP2P::IamAwake(LPDESC d, const char * c_pData)
{
	std::string hostNames;
	P2P_MANAGER::instance().GetP2PHostNames(hostNames);
	sys_log(0, "P2P Awakeness check from %s. My P2P connection number is %d. and details...\n%s", d->GetHostName(), P2P_MANAGER::instance().GetDescCount(), hostNames.c_str());
}

#ifdef __ADMIN_MANAGER__
int CInputP2P::AdminManager(const char* c_pData)
{
	TPacketGGAdminManager* p = (TPacketGGAdminManager*)c_pData;
	c_pData += sizeof(TPacketGGAdminManager);
	return CAdminManager::instance().RecvP2P(p->sub_header, c_pData);
}
#endif

int CInputP2P::Analyze(LPDESC d, BYTE bHeader, const char * c_pData)
{
	if (test_server)
	{
		sys_log(0, "CInputP2P::Anlayze[Header %d]", bHeader);
	}

	int iExtraLen = 0;

	switch (bHeader)
	{
	case HEADER_GG_SETUP:
		Setup(d, c_pData);
		break;

	case HEADER_GG_LOGIN:
		Login(d, c_pData);
		break;

	case HEADER_GG_LOGOUT:
		Logout(d, c_pData);
		break;

	case HEADER_GG_RELAY:
		if ((iExtraLen = Relay(d, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#ifdef __ENABLE_FULL_NOTICE__
		if ((iExtraLen = BigNotice(d, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;
#endif
	case HEADER_GG_NOTICE:
		if ((iExtraLen = Notice(d, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_GG_SHUTDOWN:
		sys_err("Accept shutdown p2p command from %s.", d->GetHostName());
		Shutdown(10);
		break;

	case HEADER_GG_GUILD:
		if ((iExtraLen = Guild(d, c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_GG_SHOUT:
		Shout(c_pData);
		break;

	case HEADER_GG_DISCONNECT:
		Disconnect(c_pData);
		break;

	case HEADER_GG_MESSENGER_ADD:
		MessengerAdd(c_pData);
		break;

	case HEADER_GG_MESSENGER_REMOVE:
		MessengerRemove(c_pData);
		break;

	case HEADER_GG_FIND_POSITION:
		FindPosition(d, c_pData);
		break;

	case HEADER_GG_WARP_CHARACTER:
		WarpCharacter(c_pData);
		break;

	case HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX:
		GuildWarZoneMapIndex(c_pData);
		break;

	case HEADER_GG_TRANSFER:
		Transfer(c_pData);
		break;

	case HEADER_GG_XMAS_WARP_SANTA:
		XmasWarpSanta(c_pData);
		break;

	case HEADER_GG_XMAS_WARP_SANTA_REPLY:
		XmasWarpSantaReply(c_pData);
		break;

	case HEADER_GG_RELOAD_CRC_LIST:
		LoadValidCRCList();
		break;

	case HEADER_GG_CHECK_CLIENT_VERSION:
		CheckClientVersion();
		break;

	case HEADER_GG_LOGIN_PING:
		LoginPing(d, c_pData);
		break;

	case HEADER_GG_BLOCK_CHAT:
		BlockChat(c_pData);
		break;

	case HEADER_GG_SIEGE:
	{
		TPacketGGSiege* pSiege = (TPacketGGSiege*)c_pData;
		castle_siege(pSiege->bEmpire, pSiege->bTowerCount);
	}
	break;

	case HEADER_GG_PCBANG_UPDATE :
		PCBangUpdate(c_pData);
		break;

	case HEADER_GG_CHECK_AWAKENESS:
		IamAwake(d, c_pData);
		break;

#ifdef __ENABLE_SWITCHBOT__
	case HEADER_GG_SWITCHBOT:
		Switchbot(d, c_pData);
		break;
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	case HEADER_GG_GAYA_BROADCAST:
		GayaInputPacket(d, c_pData);
		break;
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	case HEADER_GG_EVENT_BROADCAST:
		EventManagerPacket(c_pData);
		break;
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	case HEADER_GG_MAINTENANCE:
		TechnicalMaintenancePacket(d, c_pData);
		break;
#endif

#ifdef __ADMIN_MANAGER__
	case HEADER_GG_ADMIN_MANAGER:
		if ((iExtraLen = AdminManager(c_pData)) < 0)
		{
			return -1;
		}
		break;
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	case HEADER_GG_RESET_SPOTS:
		ResetSpots(c_pData);
		break;
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	case HEADER_GG_UPDATE_RIGHTS:
		UpdateRights(c_pData);
		break;
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
	case HEADER_GG_GUILD_PLAYER_NAME:
		UpdateGuildPlayerName(c_pData);
		break;
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	case HEADER_GG_PLAYER_PACKET:
		if ((iExtraLen = PlayerPacket(c_pData, m_iBufferLeft)) < 0)
		{
			return -1;
		}
		break;

	case HEADER_GG_TEAMLER_STATUS:
		TeamlerStatus(c_pData);
		break;
#endif

#ifdef __ENABLE_FIND_LOCATION__
	case HEADER_GG_PLAYER_LOCATION:
		GetPlayerLocation(c_pData);
		break;
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	case HEADER_GG_OFFLINE_SHOP:
		if ((iExtraLen = COfflineShop::ReceiveInterprocessPacket(d, c_pData)) < 0)
		{
			return -1;
		}
		break;
#endif

	case HEADER_GG_EXEC_RELOAD_COMMAND:
		ExecReloadCommand(c_pData);
		break;

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	case HEADER_GG_CREATE_DUNGEON_INSTANCE:
		CreateDungeonInstance(d, c_pData);
		break;
#ifdef __DUNGEON_RETURN_ENABLE__
	case HEADER_GG_REJOIN_DUNGEON:
		RejoinDungeon(d, c_pData);
		break;

	case HEADER_GG_CHECK_REJOIN_DUNGEON:
		CanRejoinDungeon(d, c_pData);
		break;
#endif
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	case HEADER_GG_TOMBOLA_RELOAD:
		TombolaEvent::ReloadConfig();
		break;
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	case HEADER_GG_DISCONNECT_PLAYER:
		DisconnectPlayer(c_pData);
		break;
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	case HEADER_GG_TELEPORT_RELOAD:
		CTeleportManager::instance().Reload();
		break;
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	case HEADER_GG_ATTENDANCE_RELOAD:
		CAttendanceRewardManager::instance().Create();
		break;
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	case HEADER_GG_BEGINNER_RELOAD:
		BeginnerHelper::instance().Create();
		break;
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	case HEADER_GG_BIOLOG_RELOAD:
		CBiologSystemManager::instance().Create();
		break;
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	case HEADER_GG_MARBLE_RELOAD:
		CMarbleSystemManager::instance().Create();
		break;
#endif
	}

	return (iExtraLen);
}

#ifdef __ENABLE_SWITCHBOT__
#include "switchbot.h"
void CInputP2P::Switchbot(LPDESC d, const char* c_pData)
{
	const TPacketGGSwitchbot* p = reinterpret_cast<const TPacketGGSwitchbot*>(c_pData);
	if (p->wPort != mother_port)
	{
		return;
	}

	CSwitchbotManager::Instance().P2PReceiveSwitchbot(p->table);
}
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
void CInputP2P::ResetSpots(const char * c_pData)
{
	const TPacketGGClearSpots * p = (const TPacketGGClearSpots *) c_pData;
	CHARACTER_MANAGER::instance().EraseMonsterByValue(p->dwMonster, p->lMapIndex);
}
#endif

#ifdef __GAYA_SHOP_SYSTEM__
void CInputP2P::GayaInputPacket(LPDESC inpt_d, const char* c_pData)
{
	TPacketGGGayaInfo* data = (TPacketGGGayaInfo*)c_pData;
	switch (data->bSubHeader)
	{
	case SUBHEADER_GG_GAYA_ADD_ITEM:
	case SUBHEADER_GG_GAYA_REMOVE_GUEST:
		CGayaSystemManager::instance().RegisterGayaRecord(inpt_d, data);
		break;
	case SUBHEADER_GG_GAYA_REQUEST_RECORDS:
		CGayaSystemManager::instance().BroadcastAllGayaRecords(inpt_d);
		break;
	}
}
#endif

#ifdef __EVENT_MANAGER_ENABLE__
void CInputP2P::EventManagerPacket(const char * c_pData)
{
	CEventManager::instance().RecvEventPacket((const TPacketGGEventInfo *) c_pData);
}
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
void CInputP2P::TechnicalMaintenancePacket(LPDESC d, const char * c_pData)
{
	const TPacketGGMaintenance * pack = (const TPacketGGMaintenance *) c_pData;
	switch (pack->subHeader)
	{
	case SUBHEADER_MAINTENANCE_APPEND:
		CTechnicalMaintenance::instance().RegisterMaintenance(pack->sMaintenanceName, pack->ttMaintenanceTime);
		break;
	case SUBHEADER_MAINTENANCE_DELAY:
		CTechnicalMaintenance::instance().DelayMaintenance(pack->sMaintenanceName, pack->ttMaintenanceTime);
		break;
	case SUBHEADER_MAINTENANCE_CANCEL:
		CTechnicalMaintenance::instance().CancelMaintenance(pack->sMaintenanceName);
		break;
	case SUBHEADER_MAINTENANCE_REQUEST:
		CTechnicalMaintenance::instance().SendMaintenanceInfo(d);
		break;
	}
}
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
void CInputP2P::UpdateRights(const char* c_pData)
{
	TPacketGGUpdateRights* p = (TPacketGGUpdateRights*)c_pData;

	if (p->gm_level > GM_PLAYER)
	{
		tAdminInfo info;
		memset(&info, 0, sizeof(info));
		info.m_Authority = p->gm_level;
		strlcpy(info.m_szName, p->name, sizeof(info.m_szName));
		strlcpy(info.m_szAccount, "[ALL]", sizeof(info.m_szAccount));
		GM::insert(info);
	}
	else
	{
		GM::remove(p->name);
	}

	LPCHARACTER tch = CHARACTER_MANAGER::instance().FindPC(p->name);
	if (tch)
	{
		tch->SetGMLevel();
	}

	sys_log(0, "P2P::UpdateRights: update rights to %u of %s", p->gm_level, p->name);
}
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
void CInputP2P::UpdateGuildPlayerName(const char* c_pData)
{
	TPacketGGGuildNameUpdate* data = (TPacketGGGuildNameUpdate*)c_pData;
	CGuild* guild = CGuildManager::instance().FindGuild(data->dwGuild);
	if (!guild)
	{
		return;
	}

	guild->ChangeMemberName(data->pid, data->szName, nullptr);
}
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
int CInputP2P::PlayerPacket(const char* c_pData, size_t uiBytes)
{
	TPacketGGPlayerPacket* p = (TPacketGGPlayerPacket*)c_pData;

	if (uiBytes < sizeof(TPacketGGPlayerPacket) + p->size)
	{
		return -1;
	}

	c_pData += sizeof(TPacketGGPlayerPacket);

	if (p->size <= 0)
	{
		return -1;
	}

	const CHARACTER_MANAGER::NAME_MAP& rkPCMap = CHARACTER_MANAGER::Instance().GetPCMap();
	for (auto it = rkPCMap.begin(); it != rkPCMap.end(); ++it)
	{
		if (it->second->GetDesc())
		{
			if (it->second->GetDesc()->IsPhase(PHASE_GAME) || it->second->GetDesc()->IsPhase(PHASE_DEAD))
			{
				if ((p->language == -1))
				{
					it->second->GetDesc()->Packet(c_pData, p->size);
				}
			}
		}
	}

	return p->size;
}

void CInputP2P::TeamlerStatus(const char* c_pData)
{
	TPacketGGTeamlerStatus* p = (TPacketGGTeamlerStatus*)c_pData;
	if (p->is_online)
	{
		CHARACTER_MANAGER::instance().AddOnlineTeamler(p->szName);
	}
	else
	{
		CHARACTER_MANAGER::instance().RemoveOnlineTeamler(p->szName);
	}
}
#endif

#ifdef __ENABLE_FIND_LOCATION__
void CInputP2P::GetPlayerLocation(const char* c_pData)
{
	const TPacketGGPlayerLocation* pack = (const TPacketGGPlayerLocation*)c_pData;

	// Return packet
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->searchPID);
	if (ch)
	{
		if (pack->dwMapIndex)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Player %s has been found on %d channel.", pack->sPlayerName, pack->bChannel);
			ch->ChatPacket(CHAT_TYPE_INFO, "Details: MapIndex: %u, X coords: %ld, Y coords: %ld", pack->dwMapIndex, pack->lx, pack->ly);
		}

		return;
	}

	// Skipping junk packet
	if (pack->dwMapIndex)
	{
		return;
	}

	auto p_info = std::move(CHARACTER_MANAGER::instance().GetPlayerLocation(pack->sPlayerName));
	if (p_info.dwMapIndex)
	{
		TPacketGGPlayerLocation ret_pack;
		memcpy(&ret_pack, pack, sizeof(ret_pack));

		ret_pack.bChannel = g_bChannel;
		ret_pack.dwMapIndex = p_info.dwMapIndex;
		ret_pack.lx = p_info.lx;
		ret_pack.ly = p_info.lx;

		P2P_MANAGER::instance().Send(&ret_pack, sizeof(ret_pack));
	}
}
#endif

ACMD(do_reload);
void CInputP2P::ExecReloadCommand(const char* c_pData)
{
	TPacketGGExecReloadCommand* p = (TPacketGGExecReloadCommand*)c_pData;
	do_reload(NULL, p->arg1, 0, 1);
}

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
void CInputP2P::CreateDungeonInstance(LPDESC d, const char * c_pData)
{
	TPacketGGCreateDungeonInstance * pack = (TPacketGGCreateDungeonInstance *) c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel (99 is exempted)
		if (map_allow_find(pack->lMapIndex) && (g_bChannel == pack->bChannel || g_bChannel == 99))
		{
			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;
			pack->lAddr = inet_addr(g_szPublicIP);
			pack->wPort = mother_port;

			// Create dungeon instance
			LPDUNGEON pDungeon = CDungeonManager::instance().Create(pack->lMapIndex);
			if (!pDungeon)
			{
				// Oops, something failed!
				sys_err("Could not construct map for index %d", pack->lMapIndex);
				return;
			}

			// Add pids to attender list
			for (const auto & dwPID : pack->aPids)
			{
				pDungeon->AddAttenderByPID(dwPID);
			}

			LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(pDungeon->GetMapIndex());
			pack->lX = pkSectreeMap->m_setting.posSpawn.x;
			pack->lY = pkSectreeMap->m_setting.posSpawn.y;

			// Replace origin map index
			pack->lMapIndex = pDungeon->GetMapIndex();

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
		{
			return;
		}

		sys_log(0, "Receive for PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);

		// Otherwise warp everyone
		for (unsigned int i = 0; i < pack->bCount; ++i)
		{
			auto pChar = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[i]);
			if (pChar)
			{
				// Save exit location
				pChar->SaveExitLocation();

				// Warp
				pChar->WarpSet(pack->lX, pack->lY, pack->lMapIndex, pack->lAddr, pack->wPort);
			}
		}
	}
}

#ifdef __DUNGEON_RETURN_ENABLE__
void CInputP2P::RejoinDungeon(LPDESC d, const char * c_pData)
{
	TPacketGGCreateDungeonInstance * pack = (TPacketGGCreateDungeonInstance *) c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel (99 is exempted)
		if (map_allow_find(pack->lMapIndex))
		{
			auto pDungeon = CDungeonManager::instance().FindDungeonByPID(pack->aPids[0], pack->lMapIndex);
			if (!pDungeon)
			{
				return;
			}

			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;
			pack->lAddr = inet_addr(g_szPublicIP);
			pack->wPort = mother_port;

			pack->lX = pDungeon->GetPartyCount() ? pDungeon->GetPartyX() : pack->lX;
			pack->lY = pDungeon->GetPartyCount() ? pDungeon->GetPartyY() : pack->lY;

			// Replace origin map index
			pack->lMapIndex = pDungeon->GetMapIndex();

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
		{
			return;
		}

		sys_log(0, "Receive for PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);

		// Save exit location
		ch->SaveExitLocation();

		// Clear flag
		ch->setDungeonRejoinWaiting(false);

		// Warp
		ch->WarpSet(pack->lX, pack->lY, pack->lMapIndex, pack->lAddr, pack->wPort);
	}
}

void CInputP2P::CanRejoinDungeon(LPDESC d, const char * c_pData)
{
	TPacketGGCreateDungeonInstance * pack = (TPacketGGCreateDungeonInstance *) c_pData;
	// Let's check whether it's input or output packet
	if (pack->bRequest)
	{
		// All right, it's input packet
		// Run over map pools and check if there is a requested index
		// Be sure we lookup on same channel (99 is exempted)
		if (map_allow_find(pack->lMapIndex))
		{
			auto pDungeon = CDungeonManager::instance().FindDungeonByPID(pack->aPids[0], pack->lMapIndex);
			if (!pDungeon)
			{
				return;
			}

			// Rang a bell
			// Send the output packet back
			pack->bRequest = false;

			// Send it back to source
			d->Packet(pack, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "Request from PID %d was accepted. The private map instance is %d.", pack->aPids[0], pack->lMapIndex);
		}
	}
	else
	{
		// We got an output packet then
		// Let's see if requested PID is still online
		LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pack->aPids[0]);
		// If not, give up
		if (!ch)
		{
			return;
		}

		ch->SetDungeonReturn(true);
		quest::CQuestManager::instance().Letter(ch->GetPlayerID(), quest::CQuestManager::instance().GetQuestIndexByName("dungeon_teleport"), 0);
	}
}
#endif
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
void CInputP2P::DisconnectPlayer(const char* c_pData)
{
	TPacketGGDisconnectPlayer* data = (TPacketGGDisconnectPlayer*)c_pData;
	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(data->pid);
	if (!ch)
	{
		return;
	}

	if (ch->GetDesc())
	{
		DESC_MANAGER::instance().DestroyDesc(ch->GetDesc());
	}
}
#endif
