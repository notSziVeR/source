#include "stdafx.h"
#include "../../common/stl.h"
#include "constants.h"
#include "packet_info.h"
#include "HackShield_Impl.h"
#include "XTrapManager.h"

#ifdef __INGAME_WIKI__
	#include "../../common/in_game_wiki.h"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

CPacketInfo::CPacketInfo()
	: m_pCurrentPacket(NULL), m_dwStartTime(0)
{
}

CPacketInfo::~CPacketInfo()
{
	itertype(m_pPacketMap) it = m_pPacketMap.begin();
	for ( ; it != m_pPacketMap.end(); ++it)
	{
		M2_DELETE(it->second);
	}
}

void CPacketInfo::Set(int header, int iSize, const char * c_pszName, bool bSeq)
{
	if (m_pPacketMap.find(header) != m_pPacketMap.end())
	{
		return;
	}

	TPacketElement * element = M2_NEW TPacketElement;

	element->iSize = iSize;
	element->stName.assign(c_pszName);
	element->iCalled = 0;
	element->dwLoad = 0;

#ifdef ENABLE_SEQUENCE_SYSTEM
	element->bSequencePacket = bSeq;

	if (element->bSequencePacket)
	{
		element->iSize += sizeof(BYTE);
	}
#endif

	m_pPacketMap.insert(std::map<int, TPacketElement *>::value_type(header, element));
}

bool CPacketInfo::Get(int header, int * size, const char ** c_ppszName)
{
	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.find(header);

	if (it == m_pPacketMap.end())
	{
		return false;
	}

	*size = it->second->iSize;
	*c_ppszName = it->second->stName.c_str();

	m_pCurrentPacket = it->second;
	return true;
}

#ifdef ENABLE_SEQUENCE_SYSTEM
bool CPacketInfo::IsSequence(int header)
{
	TPacketElement * pkElement = GetElement(header);
	return pkElement ? pkElement->bSequencePacket : false;
}

void CPacketInfo::SetSequence(int header, bool bSeq)
{
	TPacketElement * pkElem = GetElement(header);

	if (pkElem)
	{
		if (bSeq)
		{
			if (!pkElem->bSequencePacket)
			{
				pkElem->iSize++;
			}
		}
		else
		{
			if (pkElem->bSequencePacket)
			{
				pkElem->iSize--;
			}
		}

		pkElem->bSequencePacket = bSeq;
	}
}
#endif

TPacketElement * CPacketInfo::GetElement(int header)
{
	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.find(header);

	if (it == m_pPacketMap.end())
	{
		return NULL;
	}

	return it->second;
}

void CPacketInfo::Start()
{
	assert(m_pCurrentPacket != NULL);
	m_dwStartTime = get_dword_time();
}

void CPacketInfo::End()
{
	++m_pCurrentPacket->iCalled;
	m_pCurrentPacket->dwLoad += get_dword_time() - m_dwStartTime;
}

void CPacketInfo::Log(const char * c_pszFileName)
{
	FILE * fp;

	fp = fopen(c_pszFileName, "w");

	if (!fp)
	{
		return;
	}

	std::map<int, TPacketElement *>::iterator it = m_pPacketMap.begin();

	fprintf(fp, "Name             Called     Load       Ratio\n");

	while (it != m_pPacketMap.end())
	{
		TPacketElement * p = it->second;
		++it;

		fprintf(fp, "%-16s %-10d %-10u %.2f\n",
				p->stName.c_str(),
				p->iCalled,
				p->dwLoad,
				p->iCalled != 0 ? (float) p->dwLoad / p->iCalled : 0.0f);
	}

	fclose(fp);
}
///---------------------------------------------------------

CPacketInfoCG::CPacketInfoCG()
{
	Set(HEADER_CG_TEXT, sizeof(TPacketCGText), "Text", false);
	Set(HEADER_CG_HANDSHAKE, sizeof(TPacketCGHandshake), "Handshake", false);
	Set(HEADER_CG_TIME_SYNC, sizeof(TPacketCGHandshake), "TimeSync", true);
	Set(HEADER_CG_MARK_LOGIN, sizeof(TPacketCGMarkLogin), "MarkLogin", false);
	Set(HEADER_CG_MARK_IDXLIST, sizeof(TPacketCGMarkIDXList), "MarkIdxList", false);
	Set(HEADER_CG_MARK_CRCLIST, sizeof(TPacketCGMarkCRCList), "MarkCrcList", false);
	Set(HEADER_CG_MARK_UPLOAD, sizeof(TPacketCGMarkUpload), "MarkUpload", false);
#ifdef _IMPROVED_PACKET_ENCRYPTION_
	Set(HEADER_CG_KEY_AGREEMENT, sizeof(TPacketKeyAgreement), "KeyAgreement", false);
#endif

	Set(HEADER_CG_GUILD_SYMBOL_UPLOAD, sizeof(TPacketCGGuildSymbolUpload), "SymbolUpload", false);
	Set(HEADER_CG_SYMBOL_CRC, sizeof(TPacketCGSymbolCRC), "SymbolCRC", false);
	Set(HEADER_CG_LOGIN, sizeof(TPacketCGLogin), "Login", true);
	Set(HEADER_CG_LOGIN2, sizeof(TPacketCGLogin2), "Login2", true);
	Set(HEADER_CG_LOGIN3, sizeof(TPacketCGLogin3), "Login3", true);
	Set(HEADER_CG_LOGIN5_OPENID, sizeof(TPacketCGLogin5), "Login5", true);	//OpenID
	Set(HEADER_CG_ATTACK, sizeof(TPacketCGAttack), "Attack", true);
	Set(HEADER_CG_CHAT, sizeof(TPacketCGChat), "Chat", true);
	Set(HEADER_CG_WHISPER, sizeof(TPacketCGWhisper), "Whisper", true);

	Set(HEADER_CG_CHARACTER_SELECT, sizeof(TPacketCGPlayerSelect), "Select", true);
	Set(HEADER_CG_CHARACTER_CREATE, sizeof(TPacketCGPlayerCreate), "Create", true);
	Set(HEADER_CG_CHARACTER_DELETE, sizeof(TPacketCGPlayerDelete), "Delete", true);
	Set(HEADER_CG_ENTERGAME, sizeof(TPacketCGEnterGame), "EnterGame", true);

	Set(HEADER_CG_ITEM_USE, sizeof(TPacketCGItemUse), "ItemUse", true);
#ifdef __ENABLE_MULTI_USE_PACKET__
	Set(HEADER_CG_ITEM_MULTI_USE, sizeof(TPacketCGItemMultiUse), "ItemMultiUse", true);
#endif
	Set(HEADER_CG_ITEM_DROP, sizeof(TPacketCGItemDrop), "ItemDrop", true);
	Set(HEADER_CG_ITEM_DROP2, sizeof(TPacketCGItemDrop2), "ItemDrop2", true);
	Set(HEADER_CG_ITEM_MOVE, sizeof(TPacketCGItemMove), "ItemMove", true);
	Set(HEADER_CG_ITEM_PICKUP, sizeof(TPacketCGItemPickup), "ItemPickup", true);
#ifdef __ENABLE_DESTROY_ITEM_PACKET__
	Set(HEADER_CG_ITEM_DESTROY, sizeof(TPacketCGItemDestroyPacket), "ItemDestroy", true);
#endif

	Set(HEADER_CG_QUICKSLOT_ADD, sizeof(TPacketCGQuickslotAdd), "QuickslotAdd", true);
	Set(HEADER_CG_QUICKSLOT_DEL, sizeof(TPacketCGQuickslotDel), "QuickslotDel", true);
	Set(HEADER_CG_QUICKSLOT_SWAP, sizeof(TPacketCGQuickslotSwap), "QuickslotSwap", true);

	Set(HEADER_CG_SHOP, sizeof(TPacketCGShop), "Shop", true);

	Set(HEADER_CG_ON_CLICK, sizeof(TPacketCGOnClick), "OnClick", true);
	Set(HEADER_CG_EXCHANGE, sizeof(TPacketCGExchange), "Exchange", true);
	Set(HEADER_CG_CHARACTER_POSITION, sizeof(TPacketCGPosition), "Position", true);
	Set(HEADER_CG_SCRIPT_ANSWER, sizeof(TPacketCGScriptAnswer), "ScriptAnswer", true);
	Set(HEADER_CG_SCRIPT_BUTTON, sizeof(TPacketCGScriptButton), "ScriptButton", true);
	Set(HEADER_CG_QUEST_INPUT_STRING, sizeof(TPacketCGQuestInputString), "QuestInputString", true);
	Set(HEADER_CG_QUEST_CONFIRM, sizeof(TPacketCGQuestConfirm), "QuestConfirm", true);

	Set(HEADER_CG_MOVE, sizeof(TPacketCGMove), "Move", true);
	Set(HEADER_CG_SYNC_POSITION, sizeof(TPacketCGSyncPosition), "SyncPosition", true);

	Set(HEADER_CG_FLY_TARGETING, sizeof(TPacketCGFlyTargeting), "FlyTarget", true);
	Set(HEADER_CG_ADD_FLY_TARGETING, sizeof(TPacketCGFlyTargeting), "AddFlyTarget", true);
	Set(HEADER_CG_SHOOT, sizeof(TPacketCGShoot), "Shoot", true);

	Set(HEADER_CG_USE_SKILL, sizeof(TPacketCGUseSkill), "UseSkill", true);

	Set(HEADER_CG_ITEM_USE_TO_ITEM, sizeof(TPacketCGItemUseToItem), "UseItemToItem", true);
	Set(HEADER_CG_TARGET, sizeof(TPacketCGTarget), "Target", true);
	Set(HEADER_CG_WARP, sizeof(TPacketCGWarp), "Warp", true);
	Set(HEADER_CG_MESSENGER, sizeof(TPacketCGMessenger), "Messenger", true);

	Set(HEADER_CG_PARTY_REMOVE, sizeof(TPacketCGPartyRemove), "PartyRemove", true);
	Set(HEADER_CG_PARTY_INVITE, sizeof(TPacketCGPartyInvite), "PartyInvite", true);
	Set(HEADER_CG_PARTY_INVITE_ANSWER, sizeof(TPacketCGPartyInviteAnswer), "PartyInviteAnswer", true);
	Set(HEADER_CG_PARTY_SET_STATE, sizeof(TPacketCGPartySetState), "PartySetState", true);
	Set(HEADER_CG_PARTY_USE_SKILL, sizeof(TPacketCGPartyUseSkill), "PartyUseSkill", true);
	Set(HEADER_CG_PARTY_PARAMETER, sizeof(TPacketCGPartyParameter), "PartyParam", true);

	Set(HEADER_CG_EMPIRE, sizeof(TPacketCGEmpire), "Empire", true);
	Set(HEADER_CG_SAFEBOX_CHECKOUT, sizeof(TPacketCGSafeboxCheckout), "SafeboxCheckout", true);
	Set(HEADER_CG_SAFEBOX_CHECKIN, sizeof(TPacketCGSafeboxCheckin), "SafeboxCheckin", true);

	Set(HEADER_CG_SAFEBOX_ITEM_MOVE, sizeof(TPacketCGItemMove), "SafeboxItemMove", true);

	Set(HEADER_CG_GUILD, sizeof(TPacketCGGuild), "Guild", true);
	Set(HEADER_CG_ANSWER_MAKE_GUILD, sizeof(TPacketCGAnswerMakeGuild), "AnswerMakeGuild", true);

	Set(HEADER_CG_FISHING, sizeof(TPacketCGFishing), "Fishing", true);
	Set(HEADER_CG_ITEM_GIVE, sizeof(TPacketCGGiveItem), "ItemGive", true);
	Set(HEADER_CG_HACK, sizeof(TPacketCGHack), "Hack", true);
	Set(HEADER_CG_MYSHOP, sizeof(TPacketCGMyShop), "MyShop", true);

	Set(HEADER_CG_REFINE, sizeof(TPacketCGRefine), "Refine", true);
	Set(HEADER_CG_CHANGE_NAME, sizeof(TPacketCGChangeName), "ChangeName", true);

	Set(HEADER_CG_CLIENT_VERSION, sizeof(TPacketCGClientVersion), "Version", true);
	Set(HEADER_CG_CLIENT_VERSION2, sizeof(TPacketCGClientVersion2), "Version", true);
	Set(HEADER_CG_PONG, sizeof(BYTE), "Pong", true);
	Set(HEADER_CG_MALL_CHECKOUT, sizeof(TPacketCGSafeboxCheckout), "MallCheckout", true);

	Set(HEADER_CG_SCRIPT_SELECT_ITEM, sizeof(TPacketCGScriptSelectItem), "ScriptSelectItem", true);
	Set(HEADER_CG_PASSPOD_ANSWER, sizeof(TPacketCGPasspod), "PasspodAnswer", true);

	Set(HEADER_CG_HS_ACK, sizeof(TPacketGCHSCheck), "HackShieldResponse", false);
	Set(HEADER_CG_XTRAP_ACK, sizeof(TPacketXTrapCSVerify), "XTrapResponse", false);
	Set(HEADER_CG_DRAGON_SOUL_REFINE, sizeof(TPacketCGDragonSoulRefine), "DragonSoulRefine", false);
	Set(HEADER_CG_STATE_CHECKER, sizeof(BYTE), "ServerStateCheck", false);

#ifdef __ENABLE_TREASURE_BOX_LOOT__
	Set(HEADER_CG_REQUEST_TREASURE_BOX_LOOT, sizeof(TPacketCGDefault), "RequestTreasureBoxLoot", false);
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
	Set(HEADER_CG_TARGET_LOAD, sizeof(TPacketCGTargetLoad), "TargetLoad", true);
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	Set(HEADER_CG_REQUEST_DELETE_SOCKET, sizeof(TPacketCGDefault), "RequestDeleteSocket", true);
#endif

#ifdef __ENABLE_SWITCHBOT__
	Set(HEADER_CG_SWITCHBOT, sizeof(TPacketGCSwitchbot), "Switchbot", true);
#endif

#ifdef __VOICE_CHAT_ENABLE__
	Set(HEADER_CG_VOICE_CHAT, sizeof(TPacketCGVoiceChat), "VoiceChat", true);
#endif

#ifdef __ENABLE_NEW_FILTERS__
	Set(HEADER_CG_ITEMS_PICKUP, sizeof(TPacketCGItemsPickup), "ItemsPickup", true);
#endif

#ifdef __ADMIN_MANAGER__
	Set(HEADER_CG_ADMIN_MANAGER, sizeof(TPacketCGAdminManager), "AdminManager", true);
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	Set(HEADER_CG_MARBLE_MANAGER, sizeof(TPacketCGMarbleManagerAction), "MarbleManager");
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	Set(HEADER_CG_CUBE_CLOSE, sizeof(BYTE), "CubeClose", false);
	Set(HEADER_CG_CUBE_MAKE, sizeof(TPacketCGCubeMake), "CubeMake", true);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	Set(HEADER_CG_REFINE_ELEMENT, sizeof(TPacketCGRefineElement), "ElementalRefine");
#endif

#ifdef __ENABLE_LUCKY_BOX__
	Set(HEADER_CG_LUCKY_BOX, sizeof(TPacketCGLuckyBox), "LuckyBox", false);
#endif

#ifdef __INGAME_WIKI__
	Set(InGameWiki::HEADER_CG_WIKI, sizeof(InGameWiki::TCGWikiPacket), "RecvWikiPacket", true);
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	Set(HEADER_CG_OFFLINE_SHOP, sizeof(net_offline_shop::CG_packet), "OfflineShop", true);
#endif

#ifdef __SHOP_SEARCH__
	Set(HEADER_CG_SHOP_SEARCH_BY_NAME, sizeof(TPacketCGShopSearchByName), "ShopSearchByName", true);
	Set(HEADER_CG_SHOP_SEARCH_BY_OPTION, sizeof(TPacketCGShopSearchByOptions), "ShopSearchByOption", true);
	Set(HEADER_CG_SHOP_SEARCH_BUY, sizeof(TPacketCGShopSearchBuy), "ShopSearchBuy", true);
	Set(HEADER_CG_SHOP_SEARCH_OWNER_MESSAGE, sizeof(TPacketCGShopSearchOwnerMessage), "ShopSearchOwnerMessage", true);
	Set(HEADER_CG_SHOP_SEARCH_REQUEST_SOLD_INFO, sizeof(TPacketCGShopSearchRequestSoldInfo), "ShopSearchRequestSoldInfo", true);
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	Set(HEADER_CG_WHISPER_REQUEST_LANGUAGE, sizeof(TPacketCGWhisperRequestLanguage), "WhisperRequestLanguage", true);
#endif

#ifdef __ENABLE_PING_TIME__
	Set(HEADER_CG_PING_TIMER, sizeof(BYTE), "PingTimer", false);
#endif
}

CPacketInfoCG::~CPacketInfoCG()
{
	Log("packet_info.txt");
}

////////////////////////////////////////////////////////////////////////////////
CPacketInfoGG::CPacketInfoGG()
{
	Set(HEADER_GG_SETUP,		sizeof(TPacketGGSetup),		"Setup", false);
	Set(HEADER_GG_LOGIN,		sizeof(TPacketGGLogin),		"Login", false);
	Set(HEADER_GG_LOGOUT,		sizeof(TPacketGGLogout),	"Logout", false);
	Set(HEADER_GG_RELAY,		sizeof(TPacketGGRelay),		"Relay", false);
	Set(HEADER_GG_NOTICE,		sizeof(TPacketGGNotice),	"Notice", false);
#ifdef __ENABLE_FULL_NOTICE__
	Set(HEADER_GG_BIG_NOTICE,	sizeof(TPacketGGBigNotice),	"BigNotice", false);
#endif
	Set(HEADER_GG_SHUTDOWN,		sizeof(TPacketGGShutdown),	"Shutdown", false);
	Set(HEADER_GG_GUILD,		sizeof(TPacketGGGuild),		"Guild", false);
	Set(HEADER_GG_SHOUT,		sizeof(TPacketGGShout),		"Shout", false);
	Set(HEADER_GG_DISCONNECT,	    	sizeof(TPacketGGDisconnect),	"Disconnect", false);
	Set(HEADER_GG_MESSENGER_ADD,	sizeof(TPacketGGMessenger),	"MessengerAdd", false);
	Set(HEADER_GG_MESSENGER_REMOVE,	sizeof(TPacketGGMessenger),	"MessengerRemove", false);
	Set(HEADER_GG_FIND_POSITION,	sizeof(TPacketGGFindPosition),	"FindPosition", false);
	Set(HEADER_GG_WARP_CHARACTER,	sizeof(TPacketGGWarpCharacter),	"WarpCharacter", false);
	Set(HEADER_GG_MESSENGER_MOBILE,	sizeof(TPacketGGMessengerMobile), "MessengerMobile", false);
	Set(HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX, sizeof(TPacketGGGuildWarMapIndex), "GuildWarMapIndex", false);
	Set(HEADER_GG_TRANSFER,		sizeof(TPacketGGTransfer),	"Transfer", false);
	Set(HEADER_GG_XMAS_WARP_SANTA,	sizeof(TPacketGGXmasWarpSanta),	"XmasWarpSanta", false);
	Set(HEADER_GG_XMAS_WARP_SANTA_REPLY, sizeof(TPacketGGXmasWarpSantaReply), "XmasWarpSantaReply", false);
	Set(HEADER_GG_RELOAD_CRC_LIST,	sizeof(BYTE),			"ReloadCRCList", false);
	Set(HEADER_GG_CHECK_CLIENT_VERSION, sizeof(BYTE),			"CheckClientVersion", false);
	Set(HEADER_GG_LOGIN_PING,		sizeof(TPacketGGLoginPing),	"LoginPing", false);

	// BLOCK_CHAT
	Set(HEADER_GG_BLOCK_CHAT,		sizeof(TPacketGGBlockChat),	"BlockChat", false);
	// END_OF_BLOCK_CHAT
	Set(HEADER_GG_SIEGE,	sizeof(TPacketGGSiege),	"Siege", false);

	Set(HEADER_GG_PCBANG_UPDATE,		sizeof(TPacketPCBangUpdate),		"PCBangUpdate",		false);
	Set(HEADER_GG_CHECK_AWAKENESS,		sizeof(TPacketGGCheckAwakeness),	"CheckAwakeness",		false);

#ifdef __ENABLE_SWITCHBOT__
	Set(HEADER_GG_SWITCHBOT, sizeof(TPacketGGSwitchbot), "Switchbot", false);
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	Set(HEADER_GG_GAYA_BROADCAST, sizeof(TPacketGGGayaInfo), "GayaSystem", false);
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	Set(HEADER_GG_EVENT_BROADCAST,		sizeof(TPacketGGEventInfo),	"EventManager",		false);
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	Set(HEADER_GG_MAINTENANCE,		sizeof(TPacketGGMaintenance),	"TechnicalMaintenance",		false);
#endif

#ifdef __ADMIN_MANAGER__
	Set(HEADER_GG_ADMIN_MANAGER, sizeof(TPacketGGAdminManager), "AdminManager", false);
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	Set(HEADER_GG_RESET_SPOTS,		sizeof(TPacketGGClearSpots),	"ResetSpots",		false);
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	Set(HEADER_GG_UPDATE_RIGHTS, sizeof(TPacketGGUpdateRights), "UpdateRights", false);
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
	Set(HEADER_GG_GUILD_PLAYER_NAME, sizeof(TPacketGGGuildNameUpdate), "UpdateGuildPlayerName", false);
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	Set(HEADER_GG_PLAYER_PACKET, sizeof(TPacketGGPlayerPacket), "PlayerPacket", false);
	Set(HEADER_GG_TEAMLER_STATUS, sizeof(TPacketGGTeamlerStatus), "TeamlerStatus", false);
#endif

#ifdef __ENABLE_FIND_LOCATION__
	Set(HEADER_GG_PLAYER_LOCATION, sizeof(TPacketGGPlayerLocation), "PlayerLocationSearch", false);
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	Set(HEADER_GG_OFFLINE_SHOP, sizeof(net_offline_shop::GG_packet), "OfflineShop", false);
#endif

	Set(HEADER_GG_EXEC_RELOAD_COMMAND, sizeof(TPacketGGExecReloadCommand), "ExecReloadCommand", false);

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	Set(HEADER_GG_CREATE_DUNGEON_INSTANCE, sizeof(TPacketGGCreateDungeonInstance), "CreateDungeonInstance", false);
#ifdef __DUNGEON_RETURN_ENABLE__
	Set(HEADER_GG_REJOIN_DUNGEON,		sizeof(TPacketGGCreateDungeonInstance),	"ReturnDungeon",		false);
	Set(HEADER_GG_CHECK_REJOIN_DUNGEON,		sizeof(TPacketGGCreateDungeonInstance),	"CanReturnToDungeon",		false);
#endif
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	Set(HEADER_GG_TOMBOLA_RELOAD,	sizeof(BYTE),			"ReloadTombola", false);
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	Set(HEADER_GG_DISCONNECT_PLAYER, sizeof(TPacketGGDisconnectPlayer), "DisconnectPlayer", false);
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	Set(HEADER_GG_TELEPORT_RELOAD, sizeof(BYTE), "ReloadTeleport", false);
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	Set(HEADER_GG_ATTENDANCE_RELOAD, sizeof(BYTE), "ReloadAttendanceManager", false);
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	Set(HEADER_GG_BEGINNER_RELOAD, sizeof(BYTE), "ReloadBeginnerManager", false);
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	Set(HEADER_GG_BIOLOG_RELOAD, sizeof(BYTE), "ReloadBiologManager", false);
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	Set(HEADER_GG_MARBLE_RELOAD, sizeof(BYTE), "ReloadMarbleManager", false);
#endif
}

CPacketInfoGG::~CPacketInfoGG()
{
	Log("p2p_packet_info.txt");
}

