#ifndef __INC_PACKET_H__
#define __INC_PACKET_H__
#include "stdafx.h"

enum PacketHCG
{
	HEADER_CG_HANDSHAKE				= 0xff,
	HEADER_CG_PONG					= 0xfe,
	HEADER_CG_TIME_SYNC				= 0xfc,
	HEADER_CG_KEY_AGREEMENT			= 0xfb, // _IMPROVED_PACKET_ENCRYPTION_

	HEADER_CG_LOGIN					= 1,
	HEADER_CG_ATTACK,
	HEADER_CG_CHAT,
	HEADER_CG_CHARACTER_CREATE,
	HEADER_CG_CHARACTER_DELETE,
	HEADER_CG_CHARACTER_SELECT,
	HEADER_CG_MOVE,
	HEADER_CG_SYNC_POSITION,
	HEADER_CG_ENTERGAME,

	HEADER_CG_ITEM_USE,
	HEADER_CG_ITEM_MULTI_USE,
	HEADER_CG_ITEM_DROP,
	HEADER_CG_ITEM_MOVE,
	HEADER_CG_ITEM_PICKUP,

	HEADER_CG_QUICKSLOT_ADD,
	HEADER_CG_QUICKSLOT_DEL,
	HEADER_CG_QUICKSLOT_SWAP,
	HEADER_CG_WHISPER,
	HEADER_CG_ITEM_DROP2,

	HEADER_CG_ON_CLICK,
	HEADER_CG_EXCHANGE,
	HEADER_CG_CHARACTER_POSITION,
	HEADER_CG_SCRIPT_ANSWER,
	HEADER_CG_QUEST_INPUT_STRING,
	HEADER_CG_QUEST_CONFIRM,

	HEADER_CG_SHOP,
	HEADER_CG_FLY_TARGETING,
	HEADER_CG_USE_SKILL,
	HEADER_CG_ADD_FLY_TARGETING,
	HEADER_CG_SHOOT,
	HEADER_CG_MYSHOP,

	HEADER_CG_ITEM_USE_TO_ITEM,
	HEADER_CG_TARGET,

	HEADER_CG_WARP,
	HEADER_CG_SCRIPT_BUTTON,
	HEADER_CG_MESSENGER,

	HEADER_CG_MALL_CHECKOUT,
	HEADER_CG_SAFEBOX_CHECKIN,
	HEADER_CG_SAFEBOX_CHECKOUT,

	HEADER_CG_PARTY_INVITE,
	HEADER_CG_PARTY_INVITE_ANSWER,
	HEADER_CG_PARTY_REMOVE,
	HEADER_CG_PARTY_SET_STATE,
	HEADER_CG_PARTY_USE_SKILL,
	HEADER_CG_SAFEBOX_ITEM_MOVE,
	HEADER_CG_PARTY_PARAMETER,

	HEADER_CG_GUILD,
	HEADER_CG_ANSWER_MAKE_GUILD,

	HEADER_CG_FISHING,

	HEADER_CG_ITEM_GIVE,

	HEADER_CG_EMPIRE,

	HEADER_CG_REFINE,

	HEADER_CG_MARK_LOGIN,
	HEADER_CG_MARK_CRCLIST,
	HEADER_CG_MARK_UPLOAD,
	HEADER_CG_MARK_IDXLIST,

	HEADER_CG_HACK,
	HEADER_CG_CHANGE_NAME,
	HEADER_CG_LOGIN2,
	HEADER_CG_DUNGEON,
	HEADER_CG_LOGIN3,

	HEADER_CG_GUILD_SYMBOL_UPLOAD,
	HEADER_CG_SYMBOL_CRC,

	HEADER_CG_SCRIPT_SELECT_ITEM,

	HEADER_CG_LOGIN5_OPENID,

	HEADER_CG_PASSPOD_ANSWER,

	HEADER_CG_HS_ACK,
	HEADER_CG_XTRAP_ACK,

	HEADER_CG_DRAGON_SOUL_REFINE,
	HEADER_CG_STATE_CHECKER,

	HEADER_CG_TEXT,

#ifdef __ENABLE_DESTROY_ITEM_PACKET__
	HEADER_CG_ITEM_DESTROY,
#endif

#ifdef __ENABLE_TREASURE_BOX_LOOT__
	HEADER_CG_REQUEST_TREASURE_BOX_LOOT,
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
	HEADER_CG_TARGET_LOAD,
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	HEADER_CG_REQUEST_DELETE_SOCKET,
#endif

#ifdef __ENABLE_SWITCHBOT__
	HEADER_CG_SWITCHBOT,
#endif

#ifdef __VOICE_CHAT_ENABLE__
	HEADER_CG_VOICE_CHAT,
#endif

#ifdef __ENABLE_NEW_FILTERS__
	HEADER_CG_ITEMS_PICKUP,
#endif

#ifdef __ADMIN_MANAGER__
	HEADER_CG_ADMIN_MANAGER,
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	HEADER_CG_MARBLE_MANAGER,
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	HEADER_CG_CUBE_CLOSE,
	HEADER_CG_CUBE_MAKE,
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	HEADER_CG_ADD_LETTER,
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	HEADER_CG_REFINE_ELEMENT,
#endif

#ifdef	__ENABLE_LUCKY_BOX__
	HEADER_CG_LUCKY_BOX,
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	HEADER_CG_OFFLINE_SHOP,
#endif

#ifdef __SHOP_SEARCH__
	HEADER_CG_SHOP_SEARCH_BY_NAME,
	HEADER_CG_SHOP_SEARCH_BY_OPTION,
	HEADER_CG_SHOP_SEARCH_BUY,
	HEADER_CG_SHOP_SEARCH_OWNER_MESSAGE,
	HEADER_CG_SHOP_SEARCH_REQUEST_SOLD_INFO,
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	HEADER_CG_WHISPER_REQUEST_LANGUAGE,
#endif

#ifdef __ENABLE_PING_TIME__
	HEADER_CG_PING_TIMER			= 0xfa,
#endif
	HEADER_CG_CLIENT_VERSION		= 0xfd,
	HEADER_CG_CLIENT_VERSION2		= 0xf1,
};

enum PacketHGC
{
#ifdef __ENABLE_PING_TIME__
	HEADER_GC_PING_TIMER = 0xf9,
#endif
	HEADER_GC_KEY_AGREEMENT_COMPLETED = 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT = 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_TIME_SYNC = 0xfc,
	HEADER_GC_PHASE = 0xfd,
	HEADER_GC_BINDUDP = 0xfe,
	HEADER_GC_HANDSHAKE = 0xff,

	HEADER_GC_CHARACTER_ADD = 1,
	HEADER_GC_CHARACTER_DEL,
	HEADER_GC_MOVE,
	HEADER_GC_CHAT,
	HEADER_GC_SYNC_POSITION,

	HEADER_GC_LOGIN_SUCCESS,
	HEADER_GC_LOGIN_FAILURE,

	HEADER_GC_CHARACTER_CREATE_SUCCESS,
	HEADER_GC_CHARACTER_CREATE_FAILURE,
	HEADER_GC_CHARACTER_DELETE_SUCCESS,
	HEADER_GC_CHARACTER_DELETE_WRONG_SOCIAL_ID,

	HEADER_GC_STUN,
	HEADER_GC_DEAD,

	HEADER_GC_MAIN_CHARACTER,
	HEADER_GC_CHARACTER_POINTS,
	HEADER_GC_CHARACTER_POINT_CHANGE,
	HEADER_GC_CHANGE_SPEED,
	HEADER_GC_CHARACTER_UPDATE,

	HEADER_GC_ITEM_DEL,
	HEADER_GC_ITEM_SET,
	HEADER_GC_ITEM_USE,
	HEADER_GC_ITEM_DROP,
	HEADER_GC_ITEM_UPDATE,

	HEADER_GC_ITEM_GROUND_ADD,
	HEADER_GC_ITEM_GROUND_DEL,

	HEADER_GC_QUICKSLOT_ADD,
	HEADER_GC_QUICKSLOT_DEL,
	HEADER_GC_QUICKSLOT_SWAP,

	HEADER_GC_ITEM_OWNERSHIP,

	HEADER_GC_WHISPER,

	HEADER_GC_MOTION,

	HEADER_GC_SHOP,
	HEADER_GC_SHOP_SIGN,

	HEADER_GC_DUEL_START,
	HEADER_GC_PVP,
	HEADER_GC_EXCHANGE,
	HEADER_GC_CHARACTER_POSITION,

	HEADER_GC_PING,
	HEADER_GC_SCRIPT,
	HEADER_GC_QUEST_CONFIRM,

	HEADER_GC_MOUNT,
	HEADER_GC_OWNERSHIP,
	HEADER_GC_TARGET,

	HEADER_GC_WARP,

	HEADER_GC_ADD_FLY_TARGETING,
	HEADER_GC_CREATE_FLY,
	HEADER_GC_FLY_TARGETING,
	HEADER_GC_SKILL_LEVEL,

	HEADER_GC_MESSENGER,
	HEADER_GC_GUILD,

	HEADER_GC_PARTY_INVITE,
	HEADER_GC_PARTY_ADD,
	HEADER_GC_PARTY_UPDATE,
	HEADER_GC_PARTY_REMOVE,

	HEADER_GC_QUEST_INFO,
	HEADER_GC_REQUEST_MAKE_GUILD,
	HEADER_GC_PARTY_PARAMETER,

	HEADER_GC_SAFEBOX_SET,
	HEADER_GC_SAFEBOX_DEL,
	HEADER_GC_SAFEBOX_WRONG_PASSWORD,
	HEADER_GC_SAFEBOX_SIZE,

	HEADER_GC_FISHING,

	HEADER_GC_EMPIRE,

	HEADER_GC_PARTY_LINK,
	HEADER_GC_PARTY_UNLINK,

	HEADER_GC_REFINE_INFORMATION,

	HEADER_GC_VIEW_EQUIP,

	HEADER_GC_MARK_BLOCK,
	HEADER_GC_MARK_IDXLIST,

	HEADER_GC_TIME,
	HEADER_GC_CHANGE_NAME,

	HEADER_GC_DUNGEON,

	HEADER_GC_WALK_MODE,
	HEADER_GC_SKILL_GROUP,

	HEADER_GC_SEPCIAL_EFFECT,

	HEADER_GC_NPC_POSITION,

	HEADER_GC_MATRIX_CARD,
	HEADER_GC_LOGIN_KEY,
	HEADER_GC_CHANNEL,

	HEADER_GC_TARGET_UPDATE,
	HEADER_GC_TARGET_DELETE,
	HEADER_GC_TARGET_CREATE,

	HEADER_GC_AFFECT_ADD,
	HEADER_GC_AFFECT_REMOVE,

	HEADER_GC_MALL_OPEN,
	HEADER_GC_MALL_SET,
	HEADER_GC_MALL_DEL,

	HEADER_GC_LAND_LIST,
	HEADER_GC_LOVER_INFO,
	HEADER_GC_LOVE_POINT_UPDATE,

	HEADER_GC_SYMBOL_DATA,

	// MINING
	HEADER_GC_DIG_MOTION,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO,
	HEADER_GC_CHAR_ADDITIONAL_INFO,

	HEADER_GC_AUTH_SUCCESS,

	HEADER_GC_PANAMA_PACK,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS,
	HEADER_GC_HYBRIDCRYPT_SDB, // SDB means Supplmentary Data Blocks
	//HYBRID CRYPT

	HEADER_GC_AUTH_SUCCESS_OPENID,

	HEADER_GC_REQUEST_PASSPOD,
	HEADER_GC_REQUEST_PASSPOD_FAILED,

	HEADER_GC_HS_REQUEST,
	HEADER_GC_XTRAP_CS1_REQUEST,

	HEADER_GC_SPECIFIC_EFFECT,

	HEADER_GC_DRAGON_SOUL_REFINE,
	HEADER_GC_RESPOND_CHANNELSTATUS,

#ifdef __ENABLE_TREASURE_BOX_LOOT__
	HEADER_GC_REQUEST_TREASURE_BOX_LOOT,
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
	HEADER_GC_TARGET_INFO,
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	HEADER_GC_REQUEST_DELETE_SOCKET,
#endif

#ifdef __ENABLE_SWITCHBOT__
	HEADER_GC_SWITCHBOT,
#endif

#ifdef __ENABLE_RENEWAL_EXCHANGE__
	HEADER_GC_EXCHANGE_INFO,
#endif

#ifdef __VOICE_CHAT_ENABLE__
	HEADER_GC_VOICE_CHAT,
#endif

#ifdef __ADMIN_MANAGER__
	HEADER_GC_ADMIN_MANAGER_LOAD,
	HEADER_GC_ADMIN_MANAGER,
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	HEADER_GC_MARBLE_MANAGER,
#endif

#ifdef __ENABLE_PARTY_ATLAS__
	HEADER_GC_PARTY_POSITION,
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
	HEADER_GC_CUBE_ITEM,
	HEADER_GC_CUBE_CRAFT,
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	HEADER_GC_FIND_LETTERS_INFO,
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	HEADER_GC_REFINE_ELEMENT,
#endif

#ifdef __ENABLE_LUCKY_BOX__
	HEADER_GC_LUCKY_BOX,
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	HEADER_GC_SHOW_TEAMLER,
	HEADER_GC_TEAMLER_STATUS,
#endif

#ifdef __INVENTORY_BUFFERING__
	HEADER_GC_ITEM_BUFFERED,
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	HEADER_GC_OFFLINE_SHOP,
#endif

#ifdef __SHOP_SEARCH__
	HEADER_GC_SHOP_SEARCH_RESULT,
	HEADER_GC_SHOP_SEARCH_BUY_RESULT,
	HEADER_GC_SHOP_SEARCH_OWNER_MESSAGE,
	HEADER_GC_SHOP_SEARCH_SOLD_INFO,
#endif

#ifdef __ENABLE_NEW_LOGS_CHAT__
	HEADER_GC_LOGS_CHAT,
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	HEADER_GC_WHISPER_LANGUAGE_INFO,
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	HEADER_GC_AUTO_SHAMAN_SKILL,
#endif
};

enum P2P_Headers
{
	HEADER_GG_LOGIN = 1,
	HEADER_GG_LOGOUT,
	HEADER_GG_RELAY,
	HEADER_GG_NOTICE,
	HEADER_GG_SHUTDOWN,
	HEADER_GG_GUILD,
	HEADER_GG_DISCONNECT,
	HEADER_GG_SHOUT,
	HEADER_GG_SETUP,
	HEADER_GG_MESSENGER_ADD,
	HEADER_GG_MESSENGER_REMOVE,
	HEADER_GG_FIND_POSITION,
	HEADER_GG_WARP_CHARACTER,
	HEADER_GG_MESSENGER_MOBILE,
	HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX,
	HEADER_GG_TRANSFER,
	HEADER_GG_XMAS_WARP_SANTA,
	HEADER_GG_XMAS_WARP_SANTA_REPLY,
	HEADER_GG_RELOAD_CRC_LIST,
	HEADER_GG_LOGIN_PING,
	HEADER_GG_CHECK_CLIENT_VERSION,
	HEADER_GG_BLOCK_CHAT,

	HEADER_GG_BLOCK_EXCEPTION,
	HEADER_GG_SIEGE,
	HEADER_GG_PCBANG_UPDATE,

	HEADER_GG_CHECK_AWAKENESS,

#ifdef __ENABLE_FULL_NOTICE__
	HEADER_GG_BIG_NOTICE,
#endif

#ifdef __ENABLE_SWITCHBOT__
	HEADER_GG_SWITCHBOT,
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	HEADER_GG_GAYA_BROADCAST,
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	HEADER_GG_EVENT_BROADCAST,
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	HEADER_GG_MAINTENANCE,
#endif

#ifdef __ADMIN_MANAGER__
	HEADER_GG_ADMIN_MANAGER,
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	HEADER_GG_RESET_SPOTS,
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	HEADER_GG_UPDATE_RIGHTS,
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
	HEADER_GG_GUILD_PLAYER_NAME,
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	HEADER_GG_PLAYER_PACKET,
	HEADER_GG_TEAMLER_STATUS,
#endif

#ifdef __ENABLE_FIND_LOCATION__
	HEADER_GG_PLAYER_LOCATION,
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	HEADER_GG_OFFLINE_SHOP,
#endif

	HEADER_GG_EXEC_RELOAD_COMMAND,

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	HEADER_GG_CREATE_DUNGEON_INSTANCE,
#ifdef __DUNGEON_RETURN_ENABLE__
	HEADER_GG_REJOIN_DUNGEON,
	HEADER_GG_CHECK_REJOIN_DUNGEON,
#endif
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	HEADER_GG_TOMBOLA_RELOAD,
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	HEADER_GG_DISCONNECT_PLAYER,
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	HEADER_GG_TELEPORT_RELOAD,
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	HEADER_GG_ATTENDANCE_RELOAD,
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	HEADER_GG_BEGINNER_RELOAD,
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	HEADER_GG_BIOLOG_RELOAD,
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	HEADER_GG_MARBLE_RELOAD,
#endif
};

#ifdef __ENABLE_DELETE_SINGLE_STONE__
enum ESubHeader : uint8_t
{
	SUBHEADER_REQUEST_DELETE_SOCKET_OPEN = 0,
	SUBHEADER_REQUEST_DELETE_SOCKET_DELETE,
	SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE,
};
#endif

#pragma pack(1)
using TPacketCGDefault = struct SPacketCGDefault
{
	BYTE header;
	WORD size;

	SPacketCGDefault(BYTE _header) :
		header(_header)
	{
		size = sizeof(SPacketCGDefault);
	}
};

typedef struct SPacketGGSetup
{
	BYTE	bHeader;
	WORD	wPort;
#ifdef __ADMIN_MANAGER__
	WORD	wListenPort;
#endif
	BYTE	bChannel;
} TPacketGGSetup;

typedef struct SPacketGGLogin
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD	dwPID;
	BYTE	bEmpire;
	long	lMapIndex;
	BYTE	bChannel;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	int		iLocale;
#endif
} TPacketGGLogin;

typedef struct SPacketGGLogout
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGLogout;

typedef struct SPacketGGRelay
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lSize;
} TPacketGGRelay;

typedef struct SPacketGGNotice
{
	BYTE	bHeader;
	long	lSize;
} TPacketGGNotice;

typedef struct SPacketGGBigNotice
{
	BYTE bHeader;
	long lSize;
} TPacketGGBigNotice;

//FORKED_ROAD
typedef struct SPacketGGForkedMapInfo
{
	BYTE	bHeader;
	BYTE	bPass;
	BYTE	bSungzi;
} TPacketGGForkedMapInfo;
//END_FORKED_ROAD
typedef struct SPacketGGShutdown
{
	BYTE	bHeader;
} TPacketGGShutdown;

typedef struct SPacketGGGuild
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
} TPacketGGGuild;

enum
{
	GUILD_SUBHEADER_GG_CHAT,
	GUILD_SUBHEADER_GG_SET_MEMBER_COUNT_BONUS,
};

typedef struct SPacketGGGuildChat
{
	BYTE	bHeader;
	BYTE	bSubHeader;
	DWORD	dwGuild;
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGGuildChat;

typedef struct SPacketGGParty
{
	BYTE	header;
	BYTE	subheader;
	DWORD	pid;
	DWORD	leaderpid;
} TPacketGGParty;

enum
{
	PARTY_SUBHEADER_GG_CREATE,
	PARTY_SUBHEADER_GG_DESTROY,
	PARTY_SUBHEADER_GG_JOIN,
	PARTY_SUBHEADER_GG_QUIT,
};

typedef struct SPacketGGDisconnect
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGDisconnect;

typedef struct SPacketGGShout
{
	BYTE	bHeader;
	BYTE	bEmpire;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	BYTE	bLocale;
#endif
	char	szText[CHAT_MAX_LEN + 1];
} TPacketGGShout;

typedef struct SPacketGGXmasWarpSanta
{
	BYTE	bHeader;
	BYTE	bChannel;
	long	lMapIndex;
} TPacketGGXmasWarpSanta;

typedef struct SPacketGGXmasWarpSantaReply
{
	BYTE	bHeader;
	BYTE	bChannel;
} TPacketGGXmasWarpSantaReply;

typedef struct SMessengerData
{
	char        szMobile[MOBILE_MAX_LEN + 1];
} TMessengerData;

typedef struct SPacketGGMessenger
{
	BYTE        bHeader;
	char        szAccount[CHARACTER_NAME_MAX_LEN + 1];
	char        szCompanion[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGMessenger;

typedef struct SPacketGGMessengerMobile
{
	BYTE        bHeader;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char        szMobile[MOBILE_MAX_LEN + 1];
} TPacketGGMessengerMobile;

typedef struct SPacketGGFindPosition
{
	BYTE header;
	DWORD dwFromPID;
	DWORD dwTargetPID;
	bool bAllowDungeon;
} TPacketGGFindPosition;

typedef struct SPacketGGWarpCharacter
{
	BYTE header;
	DWORD pid;
	long x;
	long y;
	long real_map_index;
	long map_index;
	long addr;
	WORD port;
} TPacketGGWarpCharacter;

//  HEADER_GG_GUILD_WAR_ZONE_MAP_INDEX	    = 15,

typedef struct SPacketGGGuildWarMapIndex
{
	BYTE bHeader;
	DWORD dwGuildID1;
	DWORD dwGuildID2;
	long lMapIndex;
} TPacketGGGuildWarMapIndex;

typedef struct SPacketGGTransfer
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lX, lY;
} TPacketGGTransfer;

typedef struct SPacketGGLoginPing
{
	BYTE	bHeader;
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketGGLoginPing;

#ifdef __ADMIN_MANAGER__
typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwGMPid;
	char szGMName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
	char szDesc[BLOCK_DESC_MAX_LEN + 1];
	char szProof[BLOCK_PROOF_MAX_LEN + 1];
	bool bIncreaseBanCounter;
} TPacketGGBlockChat;
#else
typedef struct SPacketGGBlockChat
{
	BYTE	bHeader;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	long	lBlockDuration;
} TPacketGGBlockChat;
#endif



typedef struct command_text
{
	BYTE	bHeader;
} TPacketCGText;


typedef struct command_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketCGHandshake;

typedef struct command_login
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	#ifdef __HARDWARE_BAN__
	BYTE	HDId[HDID_MAX_LEN + 1];
	#endif
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	uint8_t	locale;
#endif
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	DWORD	adwClientKey[4];
	uint32_t version;
	#ifdef __HARDWARE_BAN__
	BYTE	HDId[HDID_MAX_LEN + 1];
	#endif
} TPacketCGLogin3;

typedef struct command_login5
{
	BYTE	header;
	char	authKey[OPENID_AUTHKEY_LEN + 1];
	DWORD	adwClientKey[4];
} TPacketCGLogin5;

typedef struct command_matrix_card
{
	BYTE	bHeader;
	char	szAnswer[MATRIX_ANSWER_MAX_LEN + 1];
} TPacketCGMatrixCard;

typedef struct packet_matrix_card
{
	BYTE	bHeader;
	DWORD	dwRows;
	DWORD	dwCols;
} TPacketGCMatrixCard;

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	index;
} TPacketCGPlayerSelect;

typedef struct command_player_delete
{
	BYTE	header;
	BYTE	index;
	char	private_code[8];
} TPacketCGPlayerDelete;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE	shape;
	BYTE	Con;
	BYTE	Int;
	BYTE	Str;
	BYTE	Dex;
} TPacketCGPlayerCreate;

typedef struct command_player_create_success
{
	BYTE		header;
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketGCPlayerCreateSuccess;


typedef struct command_attack
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

enum EMoveFuncType
{
	FUNC_WAIT,
	FUNC_MOVE,
	FUNC_ATTACK,
	FUNC_COMBO,
	FUNC_MOB_SKILL,
	_FUNC_SKILL,
	FUNC_MAX_NUM,
	FUNC_SKILL = 0x80,
};


typedef struct command_move
{
	BYTE	bHeader;
	BYTE	bFunc;
	BYTE	bArg;
	BYTE	bRot;
	long	lX;
	long	lY;
	DWORD	dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketCGSyncPositionElement;


typedef struct command_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketCGSyncPosition;


typedef struct command_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
} TPacketCGChat;


typedef struct command_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	char 	szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_entergame
{
	BYTE	header;
} TPacketCGEnterGame;

typedef struct command_item_use
{
	BYTE 	header;
	TItemPos 	Cell;
} TPacketCGItemUse;

#ifdef __ENABLE_MULTI_USE_PACKET__
typedef struct command_item_multi_use
{
	BYTE 	header;
	TItemPos 	Cell;
	BYTE		Count;
} TPacketCGItemMultiUse;
#endif

typedef struct command_item_use_to_item
{
	BYTE	header;
	TItemPos	Cell;
	TItemPos	TargetCell;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE 	header;
	TItemPos 	Cell;
	DWORD	gold;
	CountType count;
} TPacketCGItemDrop2;

typedef struct command_item_move
{
	BYTE 	header;
	TItemPos	Cell;
	TItemPos	CellTo;
	CountType count;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE 	header;
	DWORD	vid;
} TPacketCGItemPickup;

#ifdef __ENABLE_DESTROY_ITEM_PACKET__
typedef struct command_item_destroy_packet
{
	BYTE		header;
	TItemPos	Cell;
} TPacketCGItemDestroyPacket;
#endif

typedef struct command_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
} TPacketCGQuickslotAdd;

typedef struct command_quickslot_del
{
	BYTE	header;
	BYTE	pos;
} TPacketCGQuickslotDel;

typedef struct command_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	change_pos;
} TPacketCGQuickslotSwap;

enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	SHOP_SUBHEADER_GC_BUY_MULTIPLE,
#endif
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2
};

typedef struct command_shop
{
	BYTE	header;
	BYTE	subheader;
} TPacketCGShop;

typedef struct command_on_click
{
	BYTE	header;
	DWORD	vid;
} TPacketCGOnClick;

enum
{
	EXCHANGE_SUBHEADER_CG_START,	/* arg1 == vid of target character */
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,	/* arg1 == position of item */
	EXCHANGE_SUBHEADER_CG_ELK_ADD,	/* arg1 == amount of gold */
	EXCHANGE_SUBHEADER_CG_ACCEPT,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_CG_CANCEL,	/* arg1 == not used */
};

typedef struct command_exchange
{
	BYTE	header;
	BYTE	sub_header;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	arg1;
#else
	DWORD	arg1;
#endif
	BYTE	arg2;
	TItemPos	Pos;
} TPacketCGExchange;

typedef struct command_position
{
	BYTE	header;
	BYTE	position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE	header;
	BYTE	answer;
	//char	file[32 + 1];
	//BYTE	answer[16 + 1];
} TPacketCGScriptAnswer;


typedef struct command_script_button
{
	BYTE        header;
	unsigned int	idx;
} TPacketCGScriptButton;

typedef struct command_quest_input_string
{
	BYTE header;
	char msg[64 + 1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;


typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64 + 1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_handshake
{
	BYTE	bHeader;
	DWORD	dwHandshake;
	DWORD	dwTime;
	long	lDelta;
} TPacketGCHandshake;

enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_CLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
	PHASE_TEEN,
	PHASE_PASSPOD,
};

typedef struct packet_phase
{
	BYTE	header;
	BYTE	phase;
} TPacketGCPhase;

typedef struct packet_bindudp
{
	BYTE	header;
	DWORD	addr;
	WORD	port;
} TPacketGCBindUDP;

enum
{
	LOGIN_FAILURE_ALREADY	= 1,
	LOGIN_FAILURE_ID_NOT_EXIST	= 2,
	LOGIN_FAILURE_WRONG_PASS	= 3,
	LOGIN_FAILURE_FALSE		= 4,
	LOGIN_FAILURE_NOT_TESTOR	= 5,
	LOGIN_FAILURE_NOT_TEST_TIME	= 6,
	LOGIN_FAILURE_FULL		= 7
};

typedef struct packet_login_success
{
	unsigned char		bHeader;
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
	DWORD		guild_id[PLAYER_PER_ACCOUNT];
	char		guild_name[PLAYER_PER_ACCOUNT][GUILD_NAME_MAX_LEN + 1];

	DWORD		handle;
	DWORD		random_key;
	bool		hwid_check;
} TPacketGCLoginSuccess;

typedef struct packet_auth_success
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
} TPacketGCAuthSuccess;

typedef struct packet_auth_success_openid
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
	BYTE	bResult;
	char	login[LOGIN_MAX_LEN + 1];
} TPacketGCAuthSuccessOpenID;

typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
	char	szReason[BAN_REASON_MAX_LEN];
	int		iValue;
} TPacketGCLoginFailure;

typedef struct packet_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

enum
{
	ADD_CHARACTER_STATE_DEAD		= (1 << 0),
	ADD_CHARACTER_STATE_SPAWN		= (1 << 1),
	ADD_CHARACTER_STATE_GUNGON		= (1 << 2),
	ADD_CHARACTER_STATE_KILLER		= (1 << 3),
	ADD_CHARACTER_STATE_PARTY		= (1 << 4),
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,
#ifdef __ENABLE_SASH_SYSTEM__
	CHR_EQUIPPART_SASH,
#endif
	CHR_EQUIPPART_NUM,
};

typedef struct packet_add_char
{
	BYTE	header;
	DWORD	dwVID;

	float	angle;
	long	x;
	long	y;
	long	z;

	BYTE	bType;
	WORD	wRaceNum;
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;
	BYTE	dwLevel;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD    awPart[CHR_EQUIPPART_NUM];
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
	short	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
#ifdef __ENABLE_REFINE_ELEMENT__
	BYTE	bRefineElementType;
#endif
#ifdef __ENABLE_QUIVER_SYSTEM__
	DWORD	dwArrow;
#endif
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	BYTE	locale;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_update_char
{
	BYTE	header;
	DWORD	dwVID;

	DWORD        awPart[CHR_EQUIPPART_NUM];
	BYTE	bMovingSpeed;
	BYTE	bAttackSpeed;

	BYTE	bStateFlag;
	DWORD	dwAffectFlag[2];

	DWORD	dwGuildID;
	short	sAlignment;
	BYTE	bPKMode;
	DWORD	dwMountVnum;
	//WORD	wRaceNum;
#ifdef __ENABLE_REFINE_ELEMENT__
	BYTE	bRefineElementType;
#endif
#ifdef __ENABLE_QUIVER_SYSTEM__
	DWORD	dwArrow;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	id;
} TPacketGCCharacterDelete;

typedef struct packet_chat
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	id;
	BYTE	bEmpire;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	int		locale;
#endif
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bType;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	int		iLocale;
#endif
	char	szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#ifdef __OFFLINE_MESSAGE_SYSTEM__
	BYTE	bLevel;
	char	szSentDate[64];
#endif
} TPacketGCWhisper;

typedef struct packet_main_character
{
	unsigned char		header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[24 + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	unsigned char	empire;
	unsigned char	skill_group;
} TPacketGCMainCharacter;

// SUPPORT_BGM
typedef struct packet_main_character3_bgm
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter3_BGM;

typedef struct packet_main_character4_bgm_vol
{
	enum
	{
		MUSIC_NAME_LEN = 24,
	};

	BYTE    header;
	DWORD	dwVID;
	WORD	wRaceNum;
	char	szChrName[CHARACTER_NAME_MAX_LEN + 1];
	char	szBGMName[MUSIC_NAME_LEN + 1];
	float	fBGMVol;
	long	lx, ly, lz;
	BYTE	empire;
	BYTE	skill_group;
} TPacketGCMainCharacter4_BGM_VOL;
// END_OF_SUPPORT_BGM

typedef struct packet_points
{
	BYTE	header;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	points[POINT_MAX_NUM];
#else
	INT		points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;


typedef struct packet_skill_level
{
	BYTE		bHeader;
	TPlayerSkill	skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

typedef struct packet_point_change
{
	int		header;
	DWORD	dwVID;
	BYTE	type;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	amount;
	int64_t	value;
#else
	long	amount;
	long	value;
#endif
} TPacketGCPointChange;

typedef struct packet_stun
{
	BYTE	header;
	DWORD	vid;
} TPacketGCStun;

#ifdef __RENEWAL_DEAD_PACKET__
enum EReviveTypes
{
	REVIVE_TYPE_HERE,
	REVIVE_TYPE_TOWN,
	REVIVE_TYPE_AUTO_TOWN,
	REVIVE_TYPE_MAX
};
#endif
typedef struct packet_dead
{
#ifdef __RENEWAL_DEAD_PACKET__
	packet_dead() { memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE	header;
	DWORD	vid;
#ifdef __RENEWAL_DEAD_PACKET__
	DWORD	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

struct TPacketGCItemDelDeprecated
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	CountType count;

	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD	transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD	dwRefineElement;
#endif
};

typedef struct packet_item_set
{
	BYTE	header;
	TItemPos Cell;
	DWORD	vnum;
	CountType count;
	DWORD	flags;
	DWORD	anti_flags;
	bool	highlight;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD	transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD	dwRefineElement;
#endif
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE	header;
	BYTE	pos;
} TPacketGCItemDel;

struct packet_item_use
{
	BYTE	header;
	TItemPos Cell;
	DWORD	ch_vid;
	DWORD	victim_vid;
	DWORD	vnum;
};

struct packet_item_move
{
	BYTE	header;
	TItemPos Cell;
	TItemPos CellTo;
};

typedef struct packet_item_update
{
	BYTE	header;
	TItemPos Cell;
	CountType count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD	transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD	dwRefineElement;
#endif
} TPacketGCItemUpdate;

typedef struct packet_item_ground_add
{
	BYTE	bHeader;
	long 	x, y, z;
	DWORD	dwVID;
	DWORD	dwVnum;
#ifdef __ENABLE_DROP_ITEM_COUNT__
	CountType	count;
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_item_ownership
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_item_ground_del
{
	BYTE	bHeader;
	DWORD	dwVID;
} TPacketGCItemGroundDel;

struct packet_quickslot_add
{
	BYTE	header;
	BYTE	pos;
	TQuickslot	slot;
};

struct packet_quickslot_del
{
	BYTE	header;
	BYTE	pos;
};

struct packet_quickslot_swap
{
	BYTE	header;
	BYTE	pos;
	BYTE	pos_to;
};

struct packet_motion
{
	BYTE	header;
	DWORD	vid;
	DWORD	victim_vid;
	WORD	motion;
};

enum EPacketShopSubHeaders
{
	SHOP_SUBHEADER_GC_START,
	SHOP_SUBHEADER_GC_END,
	SHOP_SUBHEADER_GC_UPDATE_ITEM,
	SHOP_SUBHEADER_GC_UPDATE_PRICE,
	SHOP_SUBHEADER_GC_OK,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY,
	SHOP_SUBHEADER_GC_SOLDOUT,
	SHOP_SUBHEADER_GC_INVENTORY_FULL,
	SHOP_SUBHEADER_GC_INVALID_POS,
	SHOP_SUBHEADER_GC_SOLD_OUT,
	SHOP_SUBHEADER_GC_START_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX,
	SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM_EX,
};

struct packet_shop_item
{
	DWORD       vnum;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t		price;
#else
	DWORD       price;
#endif
	CountType	count;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD		transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD		dwRefineElement;
#endif
};

struct packet_shop_item_ex : packet_shop_item
{
	DWORD		coin_vnum;
};

typedef struct packet_shop_start
{
	DWORD   owner_vid;
	struct packet_shop_item	items[SHOP_HOST_ITEM_MAX_NUM];
} TPacketGCShopStart;

typedef struct packet_shop_start_ex
{
	typedef struct sub_packet_shop_tab
	{
		char name[SHOP_TAB_NAME_MAX];
		BYTE coin_type;
		packet_shop_item_ex items[SHOP_HOST_ITEM_MAX_NUM];
	} TSubPacketShopTab;
	DWORD owner_vid;
	BYTE shop_tab_count;
} TPacketGCShopStartEx;

typedef struct packet_shop_update_item
{
	BYTE			pos;
	struct packet_shop_item	item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t			iPrice;
#else
	int				iPrice;
#endif
} TPacketGCShopUpdatePrice;

typedef struct packet_shop
{
	BYTE        header;
	WORD	size;
	BYTE        subheader;
} TPacketGCShop;

struct packet_exchange
{
	BYTE	header;
	BYTE	sub_header;
	BYTE	is_me;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	arg1;
#else
	DWORD	arg1;	// vnum
#endif
	TItemPos	arg2;	// cell
	CountType	arg3;	// count
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD	transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD	dwRefineElement;
#endif
};

enum EPacketTradeSubHeaders
{
	EXCHANGE_SUBHEADER_GC_START,	/* arg1 == vid */
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,	/* arg1 == vnum  arg2 == pos  arg3 == count */
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,
	EXCHANGE_SUBHEADER_GC_GOLD_ADD,	/* arg1 == gold */
	EXCHANGE_SUBHEADER_GC_ACCEPT,	/* arg1 == accept */
	EXCHANGE_SUBHEADER_GC_END,		/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_ALREADY,	/* arg1 == not used */
	EXCHANGE_SUBHEADER_GC_LESS_GOLD,	/* arg1 == not used */
};

struct packet_position
{
	BYTE	header;
	DWORD	vid;
	BYTE	position;
};

typedef struct packet_ping
{
	BYTE	header;
} TPacketGCPing;

struct packet_script
{
	BYTE	header;
	WORD	size;
	BYTE	skin;
	WORD	src_size;
#ifdef ENABLE_QUEST_CATEGORY
	BYTE	quest_flag;
#endif
};

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

struct packet_mount
{
	BYTE	header;
	DWORD	vid;
	DWORD	mount_vid;
	BYTE	pos;
	DWORD	x, y;
};

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	long		lX;
	long		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;


typedef struct packet_ownership
{
	BYTE		bHeader;
	DWORD		dwOwnerVID;
	DWORD		dwVictimVID;
} TPacketGCOwnership;


typedef struct packet_sync_position_element
{
	DWORD	dwVID;
	long	lX;
	long	lY;
} TPacketGCSyncPositionElement;


typedef struct packet_sync_position
{
	BYTE	bHeader;
	WORD	wSize;
} TPacketGCSyncPosition;

typedef struct packet_fly
{
	BYTE	bHeader;
	BYTE	bType;
	DWORD	dwStartVID;
	DWORD	dwEndVID;
} TPacketGCCreateFly;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		x, y;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct packet_duel_start
{
	BYTE	header;
	WORD	wSize;
} TPacketGCDuelStart;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE
};

typedef struct packet_pvp
{
	BYTE        bHeader;
	DWORD       dwVIDSrc;
	DWORD       dwVIDDst;
	BYTE        bMode;
} TPacketGCPVP;

typedef struct command_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVID;
} TPacketCGUseSkill;

typedef struct command_target
{
	BYTE	header;
	DWORD	dwVID;
} TPacketCGTarget;

typedef struct packet_target
{
	BYTE	header;
	DWORD	dwVID;
	long	lHP, lMaxHP;
	bool	isPoisoned;
} TPacketGCTarget;

typedef struct packet_warp
{
	BYTE	bHeader;
	long	lX;
	long	lY;
	long	lAddr;
	WORD	wPort;
} TPacketGCWarp;

typedef struct command_warp
{
	BYTE	bHeader;
} TPacketCGWarp;

struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef __QUEST_RENEWAL__
	WORD c_index;
#endif
	BYTE flag;
};

enum
{
	MESSENGER_SUBHEADER_GC_LIST,
	MESSENGER_SUBHEADER_GC_LOGIN,
	MESSENGER_SUBHEADER_GC_LOGOUT,
	MESSENGER_SUBHEADER_GC_INVITE,
	MESSENGER_SUBHEADER_GC_MOBILE,
	MESSENGER_SUBHEADER_GC_REMOVE_FRIEND,
};

typedef struct packet_messenger
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCMessenger;

typedef struct packet_messenger_guild_list
{
	BYTE connected;
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildList;

typedef struct packet_messenger_guild_login
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogin;

typedef struct packet_messenger_guild_logout
{
	BYTE length;

	//char login[LOGIN_MAX_LEN+1];
} TPacketGCMessengerGuildLogout;

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

typedef struct packet_messenger_list_online
{
	BYTE connected; // always 1
	BYTE length;
} TPacketGCMessengerListOnline;

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
	MESSENGER_SUBHEADER_CG_INVITE_ANSWER,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_add_by_vid
{
	DWORD vid;
} TPacketCGMessengerAddByVID;

typedef struct command_messenger_add_by_name
{
	BYTE length;
	//char login[LOGIN_MAX_LEN+1];
} TPacketCGMessengerAddByName;

typedef struct command_messenger_remove
{
	char login[LOGIN_MAX_LEN + 1];
	//DWORD account;
} TPacketCGMessengerRemove;

typedef struct command_safebox_checkout
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE	bHeader;
	BYTE	bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_party_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketCGPartyParameter;

typedef struct paryt_parameter
{
	BYTE	bHeader;
	BYTE	bDistributeMode;
} TPacketGCPartyParameter;

typedef struct packet_party_add
{
	BYTE	header;
	DWORD	pid;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCPartyAdd;

typedef struct command_party_invite
{
	BYTE	header;
	DWORD	vid;
} TPacketCGPartyInvite;

typedef struct packet_party_invite
{
	BYTE	header;
	DWORD	leader_vid;
} TPacketGCPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE	header;
	DWORD	leader_vid;
	BYTE	accept;
} TPacketCGPartyInviteAnswer;

typedef struct packet_party_update
{
	BYTE	header;
	DWORD	pid;
	BYTE	role;
#ifdef __ENABLE_PARTY_UPDATE__
	bool	bLeader;
#endif
	BYTE	percent_hp;
	short	affects[7];
} TPacketGCPartyUpdate;

#ifdef __ENABLE_PARTY_ATLAS__
typedef struct SPacketGCPartyPosition
{
	BYTE	bHeader;
	DWORD	dwPID;
	DWORD	dwX;
	DWORD	dwY;
} TPacketGCPartyPosition;
#endif

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

typedef struct packet_party_link
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyLink;

typedef struct packet_party_unlink
{
	BYTE header;
	DWORD pid;
	DWORD vid;
} TPacketGCPartyUnlink;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE header;
	DWORD pid;
	BYTE byRole;
	BYTE flag;
} TPacketCGPartySetState;

enum
{
	PARTY_SKILL_HEAL = 1,
	PARTY_SKILL_WARP = 2
};

typedef struct command_party_use_skill
{
	BYTE header;
	BYTE bySkillIndex;
	DWORD vid;
} TPacketCGPartyUseSkill;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketCGSafeboxSize;

typedef struct packet_safebox_wrong_password
{
	BYTE	bHeader;
} TPacketCGSafeboxWrongPassword;

typedef struct command_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE	bHeader;
	BYTE	bEmpire;
} TPacketGCEmpire;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	long	lMoney;
} TPacketCGSafeboxMoney;

typedef struct packet_safebox_money_change
{
	BYTE	bHeader;
	long	lMoney;
} TPacketGCSafeboxMoneyChange;

// Guild

enum
{
	GUILD_SUBHEADER_GC_LOGIN,
	GUILD_SUBHEADER_GC_LOGOUT,
	GUILD_SUBHEADER_GC_LIST,
	GUILD_SUBHEADER_GC_GRADE,
	GUILD_SUBHEADER_GC_ADD,
	GUILD_SUBHEADER_GC_REMOVE,
	GUILD_SUBHEADER_GC_GRADE_NAME,
	GUILD_SUBHEADER_GC_GRADE_AUTH,
	GUILD_SUBHEADER_GC_INFO,
	GUILD_SUBHEADER_GC_COMMENTS,
	GUILD_SUBHEADER_GC_CHANGE_EXP,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_GC_SKILL_INFO,
	GUILD_SUBHEADER_GC_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_GC_GUILD_INVITE,
	GUILD_SUBHEADER_GC_WAR,
	GUILD_SUBHEADER_GC_GUILD_NAME,
	GUILD_SUBHEADER_GC_GUILD_WAR_LIST,
	GUILD_SUBHEADER_GC_GUILD_WAR_END_LIST,
	GUILD_SUBHEADER_GC_WAR_SCORE,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

enum GUILD_SUBHEADER_CG
{
	GUILD_SUBHEADER_CG_ADD_MEMBER,
	GUILD_SUBHEADER_CG_REMOVE_MEMBER,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_NAME,
	GUILD_SUBHEADER_CG_CHANGE_GRADE_AUTHORITY,
	GUILD_SUBHEADER_CG_OFFER,
	GUILD_SUBHEADER_CG_POST_COMMENT,
	GUILD_SUBHEADER_CG_DELETE_COMMENT,
	GUILD_SUBHEADER_CG_REFRESH_COMMENT,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GRADE,
	GUILD_SUBHEADER_CG_USE_SKILL,
	GUILD_SUBHEADER_CG_CHANGE_MEMBER_GENERAL,
	GUILD_SUBHEADER_CG_GUILD_INVITE_ANSWER,
	GUILD_SUBHEADER_CG_CHARGE_GSP,
	GUILD_SUBHEADER_CG_DEPOSIT_MONEY,
	GUILD_SUBHEADER_CG_WITHDRAW_MONEY,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

typedef struct packet_guild_name_t
{
	BYTE header;
	WORD size;
	BYTE subheader;
	DWORD	guildID;
	char	guildName[GUILD_NAME_MAX_LEN];
} TPacketGCGuildName;

typedef struct packet_guild_war
{
	DWORD	dwGuildSelf;
	DWORD	dwGuildOpp;
	BYTE	bType;
	BYTE 	bWarState;
} TPacketGCGuildWar;

typedef struct command_guild
{
	BYTE header;
	BYTE subheader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN + 1];
} TPacketCGAnswerMakeGuild;

typedef struct command_guild_use_skill
{
	DWORD	dwVnum;
	DWORD	dwPID;
} TPacketCGGuildUseSkill;

// Guild Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE	header;
	DWORD	gid;
	BYTE	image[16 * 12 * 4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE	header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE	header;
	BYTE	imgIdx;
	DWORD	crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD	count;

} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE	header;
	DWORD	bufSize;
	BYTE	imgIdx;
	DWORD	count;

} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	guild_id;
} TPacketCGGuildSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE header;
	DWORD guild_id;
	DWORD crc;
	DWORD size;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

// Fishing

typedef struct command_fishing
{
	BYTE header;
	BYTE dir;
} TPacketCGFishing;

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
	DWORD length;
} TPacketGCFishing;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	CountType byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE	bHeader;
	char	szBuf[255 + 1];
} TPacketCGHack;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE bHeader;
	WORD size;
	BYTE subheader;
} TPacketGCDungeon;

typedef struct packet_dungeon_dest_position
{
	long x;
	long y;
} TPacketGCDungeonDestPosition;

typedef struct SPacketGCShopSign
{
	BYTE	bHeader;
	DWORD	dwVID;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketCGMyShop
{
	BYTE	bHeader;
	char	szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE	bCount;
} TPacketCGMyShop;

typedef struct SPacketGCTime
{
	BYTE	bHeader;
	time_t	time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE	header;
	DWORD	vid;
	BYTE	mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

typedef struct SPacketCGRefine
{
	BYTE	header;
	BYTE	pos;
	BYTE	type;
#ifdef __ENABLE_FAST_REFINE_OPTION__
	bool	fast_refine;
#endif
} TPacketCGRefine;

typedef struct SPacketCGRequestRefineInfo
{
	BYTE	header;
	BYTE	pos;
} TPacketCGRequestRefineInfo;

typedef struct SPacketGCRefineInformaion
{
	BYTE	header;
	BYTE	type;
	BYTE	pos;
#ifdef __ENABLE_FAST_REFINE_OPTION__
	bool	fast_refine;
#endif
#ifdef __SASH_ABSORPTION_ENABLE__
	int		iSashRefine;
#endif
	int		addedProb;
	DWORD	src_vnum;
	DWORD	result_vnum;
	CountType material_count;
	int		cost;
	int		prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TPacketGCRefineInformation;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;

	// array of TNPCPosition
} TPacketGCNPCPosition;

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGChangeName;

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCChangeName;


typedef struct command_client_version
{
	BYTE header;
	char filename[32 + 1];
	char timestamp[32 + 1];
} TPacketCGClientVersion;

typedef struct command_client_version2
{
	BYTE header;
	char filename[32 + 1];
	char timestamp[32 + 1];
} TPacketCGClientVersion2;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	BYTE    count;
	long    alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE  header;
	DWORD vid;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketViewEquip;

typedef struct
{
	DWORD	dwID;
	long	x, y;
	long	width, height;
	DWORD	dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE	header;
	WORD	size;
} TPacketGCLandList;

typedef struct
{
	BYTE	bHeader;
	long	lID;
	char	szName[32 + 1];
	DWORD	dwVID;
	BYTE	bType;
} TPacketGCTargetCreate;

typedef struct
{
	BYTE	bHeader;
	long	lID;
	long	lX, lY;
} TPacketGCTargetUpdate;

typedef struct
{
	BYTE	bHeader;
	long	lID;
} TPacketGCTargetDelete;

typedef struct
{
	BYTE		bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
	BYTE	bHeader;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_lover_info
{
	BYTE header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE love_point;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE header;
	BYTE love_point;
} TPacketGCLovePointUpdate;

// MINING
typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;
// END_OF_MINING

// SCRIPT_SELECT_ITEM
typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;
// END_OF_SCRIPT_SELECT_ITEM

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int damage;
} TPacketGCDamageInfo;

enum
{
	RUNUP_MATRIX_ANSWER_MAX_LEN = 4,
	RUNUP_MATRIX_QUIZ_MAX_LEN = 8,

};

typedef struct packet_runup_matrix_quiz
{
	BYTE    bHeader;
	char    szQuiz[RUNUP_MATRIX_QUIZ_MAX_LEN + 1];
} TPacketGCRunupMatrixQuiz;

typedef struct command_runup_matrix_answer
{
	BYTE    bHeader;
	char    szAnswer[RUNUP_MATRIX_ANSWER_MAX_LEN + 1];
} TPacketCGRunupMatrixAnswer;

typedef struct packet_passpod
{
	BYTE    bHeader;
	char    szAnswer[8 + 1];
} TPacketCGPasspod;

typedef struct packet_passpod_failed
{
	BYTE    bHeader;
	char    szMessage[128];
} TPacketCGPasspodFailed;

typedef struct tag_GGSiege
{
	BYTE	bHeader;
	BYTE	bEmpire;
	BYTE	bTowerCount;
} TPacketGGSiege;

typedef struct SPacketGGPCBangUpdate
{
	BYTE bHeader;
	unsigned long ulPCBangID;
} TPacketPCBangUpdate;

typedef struct SPacketGGCheckAwakeness
{
	BYTE bHeader;
} TPacketGGCheckAwakeness;

typedef struct SPacketGCPanamaPack
{
	BYTE	bHeader;
	char	szPackName[256];
	BYTE	abIV[32];
} TPacketGCPanamaPack;


typedef struct SPacketGCHybridCryptKeys
{
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}
	~SPacketGCHybridCryptKeys()
	{

		if ( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + KeyStreamLen;
	}

	BYTE* GetStreamData()
	{
		if ( m_pStream )
		{
			delete[] m_pStream;
		}

		uDynamicPacketSize = (WORD)GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream + 1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream + 3, &KeyStreamLen, 4 );

		if ( KeyStreamLen > 0 )
		{
			memcpy( m_pStream + 7, pDataKeyStream, KeyStreamLen );
		}

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		KeyStreamLen;
	BYTE*   pDataKeyStream;

private:
	BYTE* m_pStream;


} TPacketGCHybridCryptKeys;


typedef struct SPacketGCPackageSDB
{
	SPacketGCPackageSDB() : m_pDataSDBStream(NULL), m_pStream(NULL) {}
	~SPacketGCPackageSDB()
	{
		if ( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}

	DWORD GetStreamSize()
	{
		return sizeof(bHeader) + sizeof(WORD) + sizeof(int) + iStreamLen;
	}

	BYTE* GetStreamData()
	{
		if ( m_pStream )
		{
			delete[] m_pStream;
		}

		uDynamicPacketSize =  GetStreamSize();

		m_pStream = new BYTE[ uDynamicPacketSize ];

		memcpy( m_pStream, &bHeader, 1 );
		memcpy( m_pStream + 1, &uDynamicPacketSize, 2 );
		memcpy( m_pStream + 3, &iStreamLen, 4 );

		if ( iStreamLen > 0 )
		{
			memcpy( m_pStream + 7, m_pDataSDBStream, iStreamLen );
		}

		return m_pStream;
	}

	BYTE	bHeader;
	WORD    uDynamicPacketSize;
	int		iStreamLen;
	BYTE*   m_pDataSDBStream;

private:
	BYTE* m_pStream;


} TPacketGCPackageSDB;

#ifdef _IMPROVED_PACKET_ENCRYPTION_
struct TPacketKeyAgreement
{
	static const int MAX_DATA_LEN = 256;
	BYTE bHeader;
	WORD wAgreedLength;
	WORD wDataLength;
	BYTE data[MAX_DATA_LEN];
};

struct TPacketKeyAgreementCompleted
{
	BYTE bHeader;
	BYTE data[3]; // dummy (not used)
};

#endif // _IMPROVED_PACKET_ENCRYPTION_

#define MAX_EFFECT_FILE_NAME 128
typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[MAX_EFFECT_FILE_NAME];
} TPacketGCSpecificEffect;


enum EDragonSoulRefineWindowRefineType
{
	DragonSoulRefineWindow_UPGRADE,
	DragonSoulRefineWindow_IMPROVEMENT,
	DragonSoulRefineWindow_REFINE,
};

enum EPacketCGDragonSoulSubHeaderType
{
	DS_SUB_HEADER_OPEN,
	DS_SUB_HEADER_CLOSE,
	DS_SUB_HEADER_DO_REFINE_GRADE,
#ifdef __ENABLE_DS_REFINE_ALL__
	DS_SUB_HEADER_DO_REFINE_GRADE_ALL,
#endif
	DS_SUB_HEADER_DO_REFINE_STEP,
	DS_SUB_HEADER_DO_REFINE_STRENGTH,
	DS_SUB_HEADER_REFINE_FAIL,
	DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE,
	DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY,
	DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL,
	DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL,
	DS_SUB_HEADER_REFINE_SUCCEED,
};
typedef struct SPacketCGDragonSoulRefine
{
	SPacketCGDragonSoulRefine() : header (HEADER_CG_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos ItemGrid[DRAGON_SOUL_REFINE_GRID_SIZE];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SPacketCGStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
} TPacketCGStateCheck;

typedef struct SPacketGCStateCheck
{
	BYTE header;
	unsigned long key;
	unsigned long index;
	unsigned char state;
} TPacketGCStateCheck;

#ifdef __ENABLE_SWITCHBOT__
struct TPacketGGSwitchbot
{
	BYTE bHeader;
	WORD wPort;
	TSwitchbotTable table;

	TPacketGGSwitchbot() : bHeader(HEADER_GG_SWITCHBOT), wPort(0)
	{
#if __cplusplus < 199711L
		memset(&table, 0, sizeof(table));
#else
		table = {};
#endif
	}
};

enum ECGSwitchbotSubheader
{
	SUBHEADER_CG_SWITCHBOT_START,
	SUBHEADER_CG_SWITCHBOT_STOP,
};

struct TPacketCGSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

enum EGCSwitchbotSubheader
{
	SUBHEADER_GC_SWITCHBOT_UPDATE,
	SUBHEADER_GC_SWITCHBOT_UPDATE_ITEM,
	SUBHEADER_GC_SWITCHBOT_SEND_ATTRIBUTE_INFORMATION,
};

struct TPacketGCSwitchbot
{
	BYTE header;
	int size;
	BYTE subheader;
	BYTE slot;
};

struct TSwitchbotUpdateItem
{
	BYTE	slot;
	BYTE	vnum;
	BYTE	count;
	long	alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
};
#endif

#ifdef __ENABLE_RENEWAL_EXCHANGE__
typedef struct packet_exchange_info
{
	BYTE        bHeader;
	WORD        wSize;
	bool        bError;
	int			iUnixTime;
} TPacketGCExchageInfo;
#endif

#ifdef __GAYA_SHOP_SYSTEM__
enum GayaSubheader
{
	SUBHEADER_GG_GAYA_ADD_ITEM,
	SUBHEADER_GG_GAYA_REMOVE_GUEST,
	SUBHEADER_GG_GAYA_REQUEST_RECORDS,
};

typedef struct SPacketGGGayaInfo
{
	BYTE bHeader;
	BYTE bSubHeader;
	DWORD dwPID;
	size_t uiRotationTime;
	BYTE sNum;
	DWORD dwVnum;
	WORD wCount;
	long long llPrice;
} TPacketGGGayaInfo;
#endif
#ifdef __EVENT_MANAGER_ENABLE__
typedef struct SPacketGGEventInfo
{
	BYTE	bHeader;
	time_t	ttEndTime;
	char	sEventName[256];
	char	sEventTime[256];
} TPacketGGEventInfo;
#endif
#ifdef __CYCLIC_MONSTER_RESPAWNER__
typedef struct SPacketGGClearSpots
{
	BYTE header;
	long lMapIndex;
	DWORD dwMonster;
} TPacketGGClearSpots;
#endif
#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
enum EMaintenanceSubheaders
{
	SUBHEADER_MAINTENANCE_APPEND,
	SUBHEADER_MAINTENANCE_DELAY,
	SUBHEADER_MAINTENANCE_CANCEL,
	SUBHEADER_MAINTENANCE_REQUEST,
};

typedef struct SPacketGGMaintenance
{
	BYTE bHeader;
	BYTE subHeader;
	time_t ttMaintenanceTime;
	char sMaintenanceName[256];
} TPacketGGMaintenance;
#endif

#ifdef __VOICE_CHAT_ENABLE__
typedef struct SPacketCGVoiceChat
{
	BYTE byHeader;
	WORD stSize;
	WORD stSizeUncp;
} TPacketCGVoiceChat;

typedef struct SPacketGCVoiceChat
{
	BYTE bHeader;
	WORD stSize;
	WORD stSizeUncp;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCVoiceChat;
#endif

#ifdef __ENABLE_NEW_FILTERS__
typedef struct command_items_pickup
{
	BYTE header;
	DWORD items[20];
} TPacketCGItemsPickup;
#endif

#ifdef __ADMIN_MANAGER__
typedef struct SPacketGGAdminManager
{
	unsigned char header;
	unsigned char sub_header;
} TPacketGGAdminManager;

typedef struct SAdminManagerPlayerInfo
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lMapIndex;
	unsigned char bChannel;
	unsigned char bEmpire;
} TAdminManagerPlayerInfo;

typedef struct SAdminManagerMapViewerPlayerInfo
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	unsigned char bEmpire;
	bool is_alive;
	DWORD x;
	DWORD y;
} TAdminManagerMapViewerPlayerInfo;

typedef struct SAdminManagerMapViewerMobInfo
{
	DWORD dwVID;
	DWORD dwRaceNum;
	bool is_alive;
	DWORD x;
	DWORD y;
	DWORD stone_drop_vnum;
} TAdminManagerMapViewerMobInfo;

typedef struct SAdminManagerObserverItemInfo
{
	DWORD id;
	DWORD vnum;
	CountType count;
	WORD cell;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	bool is_gm_item;
} TAdminManagerObserverItemInfo;

typedef struct SPacketGCAdminManagerLoad
{
	unsigned char header;
	WORD size;
	DWORD dwAllowFlag;
	DWORD dwPlayerCount;
	bool bIsGMItemTradeBlock;
	DWORD dwBanChatCount;
	DWORD dwBanAccountCount;
} TPacketGCAdminManagerLoad;

typedef struct SPacketGCAdminManager
{
	unsigned char header;
	WORD size;
	unsigned char sub_header;
} TPacketGCAdminManager;

typedef struct SPacketCGAdminManager
{
	unsigned char header;
	unsigned char sub_header;
} TPacketCGAdminManager;

typedef struct SAdminManagerItemInfo
{
	BYTE bOwnerType;
	DWORD dwOwnerID;
	BYTE bOwnerNameLen;
	DWORD dwItemID;
	TItemPos kCell;
	DWORD dwItemVnum;
	BYTE bCount;
	long alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
	bool bIsGMItem;
} TAdminManagerItemInfo;

typedef struct SAdminManagerHackLog
{
	BYTE dwHackerNameLen;
	BYTE dwHackerReasonLen;
	int dwHackCount;
	BYTE dwHackTimeLen;
} TAdminManagerHackLog;
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
enum ECGMarbleSubHeaders
{
	CG_MARBLE_MANAGER_OPEN,
	CG_MARBLE_MANAGER_ACTIVE,
	CG_MARBLE_MANAGER_DEACTIVE,
};

enum EGCMarbleSubHeaders
{
	GC_MARBLE_MANAGER_DATA,
	GC_MARBLE_MANAGER_REFRESH,
	GC_MARBLE_MANAGER_OPEN,
};

typedef struct SPacketCGMarbleManagerAction
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGMarbleManagerAction;

typedef struct SPacketGCMarbleManager
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCMarbleManager;

typedef struct SPacketGCMarbleManagerInfo
{
	BYTE	bID;
	DWORD	dwMarbleVnum;
	WORD	wRequiredKillCount;
	DWORD	bActiveMission;
	BYTE	bActiveExtandedCount;
	WORD	wKilledMonsters;
	time_t	tCooldownTime;
} TPacketGCMarbleManagerInfo;
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
typedef struct SPacketGGGuildNameUpdate
{
	BYTE	bHeader;
	DWORD	dwGuild;
	DWORD	pid;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGGGuildNameUpdate;
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
enum EPacketCraftFail
{
	CUBE_CRAFT_END,
	CUBE_CRAFT_NOT_ENOUGH_MONEY,
	CUBE_CRAFT_NOT_ENOUGH_OBJECT,
	CUBE_CRAFT_CREATE_ITEM,
	CUBE_CRAFT_INVENTORY_FULL,
	CUBE_CRAFT_SUCCES,
};

typedef struct SPacketGCCubeItem
{
	BYTE	bHeader;
	WORD	wSize;
	DWORD	dwNpcVnum;
} TPacketGCCubeItem;

typedef struct SPacketGCCubeCraft
{
	BYTE	bHeader;
	BYTE	bErrorType;
	DWORD	dwErrorArg;
} TPacketGCCubeCraft;

typedef struct SPacketCGCubeMake
{
	BYTE	bHeader;
	DWORD	dwNpcVnum;
	int 	iIndex;
	BYTE	bMakeCount;
	BYTE	bIsIncrease;
} TPacketCGCubeMake;
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
typedef struct SPacketGCFindLettersInfo
{
	BYTE	bHeader;
	WORD	wSize;
	TPlayerLetterSlot letterSlots[FIND_LETTERS_SLOTS_NUM];
} TPacketGCFindLettersInfo;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
typedef struct SPacketCGRefineElement
{
	BYTE	bHeader;

	// 1. bArg is used for close window to clear the info
	// 2. bArg is used for change element type and it represent the new type
	BYTE	bArg;
} TPacketCGRefineElement;

typedef struct SPacketGCRefineElement
{
	BYTE	bHeader;
	WORD	wSrcCell;
	WORD	wDstCell;
	BYTE	bType;
} TPacketGCRefineElement;
#endif

#ifdef __ENABLE_LUCKY_BOX__
typedef struct SPacketGCLuckyBox
{
	BYTE	bHeader;
	DWORD	dwBoxVnum;
	DWORD	dwPrice;
	bool	bIsOpen;
	TLuckyBoxItem luckyItems[LUCKY_BOX_MAX_NUM];
} TPacketGCLuckyBox;

typedef struct SPacketCGLuckyBox
{
	BYTE	bHeader;
	BYTE	bAction;
} TPacketCGLuckyBox;
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
typedef struct SPacketGGPlayerPacket
{
	BYTE	header;
	int		size;
	int		language;
} TPacketGGPlayerPacket;

typedef struct SPacketGGTeamlerStatus
{
	BYTE	header;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	bool	is_online;
} TPacketGGTeamlerStatus;

typedef struct SPacketGCShowTeamler
{
	BYTE	header;
	bool	is_show;
} TPacketGCShowTeamler;

typedef struct SPacketGCTeamlerStatus
{
	BYTE	header;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	bool	is_online;
} TPacketGCTeamlerStatus;
#endif

#ifdef __INVENTORY_BUFFERING__
typedef struct SPacketGCInventoryHeader
{
	BYTE bHeader;
	WORD wSize;
} TPacketGCInventoryHeader;
#endif

#ifdef __ENABLE_FIND_LOCATION__
typedef struct SPacketGGPlayerLocation
{
	BYTE bHeader;
	DWORD searchPID;
	char sPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bChannel;
	DWORD dwMapIndex;
	long lx;
	long ly;
} TPacketGGPlayerLocation;
#endif

#ifdef __SHOP_SEARCH__
typedef struct SPacketCGShopSearchByName
{
	BYTE	header;
	char	itemName[ITEM_NAME_MAX_LEN + 1];
	WORD	page;
	BYTE	entryCountIdx;
	BYTE	sortType;
} TPacketCGShopSearchByName;

typedef struct SPacketCGShopSearchByOptions
{
	BYTE	header;
	TShopSearchOptions	options;
	WORD	page;
	BYTE	entryCountIdx;
	BYTE	sortType;
} TPacketCGShopSearchByOptions;

typedef struct SPacketGCShopSearchResult
{
	BYTE	header;
	WORD	size;
	WORD	itemCount;
	WORD	maxPageNum;
} TPacketGCShopSearchResult;
typedef struct SPacketCGShopSearchBuy
{
	BYTE	header;
	TOfflineItemID	itemID;
	DWORD	itemVnum;
	int64_t	itemPrice;

} TPacketCGShopSearchBuy;

typedef struct SPacketGCShopSearchBuyResult
{
	BYTE	header;
	BYTE	result;
} TPacketGCShopSearchBuyResult;

typedef struct SPacketCGShopSearchOwnerMessage
{
	BYTE	header;
	DWORD	ownerID;
} TPacketCGShopSearchOwnerMessage;

typedef struct SPacketGCShopSearchOwnerMessage
{
	BYTE	header;
	char	ownerName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCShopSearchOwnerMessage;

typedef struct SPacketCGShopSearchRequestSoldInfo
{
	BYTE	header;
	DWORD	itemVnum;
} TPacketCGShopSearchRequestSoldInfo;

typedef struct SPacketGCShopSearchSoldInfo
{
	BYTE	header;
	WORD	size;
	bool	results;
} TPacketGCShopSearchSoldInfo;
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
typedef struct SPacketCGTargetLoad
{
	BYTE byHeader;
	DWORD dwVID;
} TPacketCGTargetLoad;

typedef struct SPacketGCTargetInfo
{
	BYTE byHeader;
	WORD wSize;
	DWORD dwVNum;
	size_t uCount;
} TPacketGCTargetInfo;
#endif

typedef struct SPacketGGExecReloadCommand
{
	BYTE	header;
	char	arg1[100];
} TPacketGGExecReloadCommand;

#ifdef __ENABLE_ADMIN_BAN_PANEL__
typedef struct SPacketGGDisconnectPlayer
{
	BYTE	bHeader;
	DWORD	pid;
} TPacketGGDisconnectPlayer;
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
typedef struct SPacketCGWhisperRequestLanguage
{
	BYTE	header;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperRequestLanguage;

typedef struct SPacketGCWhisperLanguageInfo
{
	BYTE	header;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	language;
} TPacketGCWhisperLanguageInfo;
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
typedef struct shaman_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVid;
	BYTE	dwLevel;
}TPacketGCShamanUseSkill;
#endif

#pragma pack()
#endif
