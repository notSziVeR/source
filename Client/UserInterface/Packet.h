#pragma once

#ifdef USE_AHNLAB_HACKSHIELD
#include "Hackshield.h"
#include METIN2HS_INCLUDE_ANTICPXSVR
#else
#pragma pack(push)
#pragma pack(8)

#define ANTICPX_TRANS_BUFFER_MAX			400

typedef struct _AHNHS_TRANS_BUFFER
{
	unsigned char byBuffer[ANTICPX_TRANS_BUFFER_MAX];
	unsigned short nLength;
} AHNHS_TRANS_BUFFER, *PAHNHS_TRANS_BUFFER;

#pragma pack(pop)
#endif /* !USE_AHNLAB_HACKSHIELD */

#include "Locale.h"
#include "../gamelib/RaceData.h"
#include "../gamelib/ItemData.h"

#include <array>

typedef BYTE TPacketHeader;

enum PacketHCG
{
	HEADER_CG_LOGIN = 1,
	HEADER_CG_ATTACK,
	HEADER_CG_CHAT,
	HEADER_CG_PLAYER_CREATE,
	HEADER_CG_PLAYER_DESTROY,
	HEADER_CG_PLAYER_SELECT,
	HEADER_CG_CHARACTER_MOVE,
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

	HEADER_CG_GIVE_ITEM,

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
	HEADER_CG_GUILD_SYMBOL_CRC,

	HEADER_CG_SCRIPT_SELECT_ITEM,

	HEADER_CG_LOGIN5_OPENID,

	HEADER_CG_NEWCIBN_PASSPOD_ANSWER,

	HEADER_CG_HS_ACK,
	HEADER_CG_XTRAP_ACK,

	HEADER_CG_DRAGON_SOUL_REFINE,
	HEADER_CG_STATE_CHECKER,

	HEADER_CG_TEXT,

#ifdef ENABLE_DESTROY_ITEM_PACKET
	HEADER_CG_ITEM_DESTROY,
#endif

#ifdef ENABLE_TREASURE_BOX_LOOT
	HEADER_CG_REQUEST_TREASURE_BOX_LOOT,
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
	HEADER_CG_TARGET_LOAD,
#endif

#ifdef ENABLE_DELETE_SINGLE_STONE
	HEADER_CG_REQUEST_DELETE_SOCKET,
#endif

#ifdef ENABLE_SWITCHBOT
	HEADER_CG_SWITCHBOT,
#endif

#ifdef ENABLE_VOICE_CHAT
	HEADER_CG_VOICE_CHAT,
#endif

#ifdef ENABLE_NEW_FILTERS
	HEADER_CG_ITEMS_PICKUP,
#endif
#ifdef ENABLE_ADMIN_MANAGER
	HEADER_CG_ADMIN_MANAGER,
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_CG_BIOLOG_MANAGER,
#endif

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
	HEADER_CG_MARBLE_MANAGER,
#endif

#ifdef ENABLE_CUBE_RENEWAL
	HEADER_CG_CUBE_CLOSE,
	HEADER_CG_CUBE_MAKE,
#endif

#ifdef ENABLE_FIND_LETTERS_EVENT
	HEADER_CG_ADD_LETTER,
#endif

#ifdef ENABLE_REFINE_ELEMENT
	HEADER_CG_REFINE_ELEMENT,
#endif

#ifdef ENABLE_LUCKY_BOX
	HEADER_CG_LUCKY_BOX,
#endif

#ifdef ENABLE_OFFLINE_SHOP
	HEADER_CG_OFFLINE_SHOP,
#endif

#ifdef ENABLE_SHOP_SEARCH
	HEADER_CG_SHOP_SEARCH_BY_NAME,
	HEADER_CG_SHOP_SEARCH_BY_OPTION,
	HEADER_CG_SHOP_SEARCH_BUY,
	HEADER_CG_SHOP_SEARCH_OWNER_MESSAGE,
	HEADER_CG_SHOP_SEARCH_REQUEST_SOLD_INFO,
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	HEADER_CG_WHISPER_REQUEST_LANGUAGE,
#endif

	//HEADER_CG_RUNUP_MATRIX_ANSWER = 201,
	//HEADER_CG_CRC_REPORT = 103,
	//HEADER_CG_SMS = 59,
	//HEADER_CG_CHINA_MATRIX_CARD = 60,

#ifdef ENABLE_PING_TIME
	HEADER_CG_PING_TIMER = 0xfa,
#endif

	HEADER_CG_KEY_AGREEMENT = 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_CG_TIME_SYNC = 0xfc,
	HEADER_CG_CLIENT_VERSION = 0xfd,
	HEADER_CG_CLIENT_VERSION2 = 0xf1,
	HEADER_CG_PONG = 0xfe,
	HEADER_CG_HANDSHAKE = 0xff,
};

/////////////////////////////////////////////////
// From Server
enum PacketHGC
{
	HEADER_GC_CHARACTER_ADD = 1,
	HEADER_GC_CHARACTER_DEL,
	HEADER_GC_CHARACTER_MOVE,
	HEADER_GC_CHAT,
	HEADER_GC_SYNC_POSITION,

	HEADER_GC_LOGIN_SUCCESS,
	HEADER_GC_LOGIN_FAILURE,

	HEADER_GC_PLAYER_CREATE_SUCCESS,
	HEADER_GC_PLAYER_CREATE_FAILURE,
	HEADER_GC_PLAYER_DELETE_SUCCESS,
	HEADER_GC_PLAYER_DELETE_WRONG_SOCIAL_ID,

	HEADER_GC_STUN,
	HEADER_GC_DEAD,

	HEADER_GC_MAIN_CHARACTER,
	HEADER_GC_PLAYER_POINTS,
	HEADER_GC_PLAYER_POINT_CHANGE,
	HEADER_GC_CHANGE_SPEED,
	HEADER_GC_CHARACTER_UPDATE,

#if defined(GAIDEN)
	HEADER_GC_ITEM_DEL,
	HEADER_GC_ITEM_SET,
#else
	HEADER_GC_ITEM_SET,
	HEADER_GC_ITEM_SET2,
#endif
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
	HEADER_GC_CHANGE_SKILL_GROUP,

	HEADER_GC_SEPCIAL_EFFECT,

	HEADER_GC_NPC_POSITION,

	HEADER_GC_CHINA_MATRIX_CARD,
	HEADER_GC_LOGIN_KEY,
	HEADER_GC_CHANNEL,

	HEADER_GC_TARGET_UPDATE,
	HEADER_GC_TARGET_DELETE,
	HEADER_GC_TARGET_CREATE_NEW,

	HEADER_GC_AFFECT_ADD,
	HEADER_GC_AFFECT_REMOVE,

	HEADER_GC_MALL_OPEN,
	HEADER_GC_MALL_SET,
	HEADER_GC_MALL_DEL,

	HEADER_GC_LAND_LIST,
	HEADER_GC_LOVER_INFO,
	HEADER_GC_LOVE_POINT_UPDATE,

	HEADER_GC_GUILD_SYMBOL_DATA,

	// MINING
	HEADER_GC_DIG_MOTION,
	// END_OF_MINING

	HEADER_GC_DAMAGE_INFO,
	HEADER_GC_CHAR_ADDITIONAL_INFO,

	HEADER_GC_AUTH_SUCCESS,
	HEADER_GC_PANAMA_PACK,

	//HYBRID CRYPT
	HEADER_GC_HYBRIDCRYPT_KEYS,
	HEADER_GC_HYBRIDCRYPT_SDB,
	//HYBRID CRYPT

	HEADER_GC_AUTH_SUCCESS_OPENID,

	HEADER_GC_NEWCIBN_PASSPOD_REQUEST,
	HEADER_GC_NEWCIBN_PASSPOD_FAILURE,

	HEADER_GC_HS_REQUEST,
	HEADER_GC_XTRAP_CS1_REQUEST,

	HEADER_GC_SPECIFIC_EFFECT,

	HEADER_GC_DRAGON_SOUL_REFINE,
	HEADER_GC_RESPOND_CHANNELSTATUS,

#ifdef ENABLE_TREASURE_BOX_LOOT
	HEADER_GC_REQUEST_TREASURE_BOX_LOOT,
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
	HEADER_GC_TARGET_INFO,
#endif

#ifdef ENABLE_DELETE_SINGLE_STONE
	HEADER_GC_REQUEST_DELETE_SOCKET,
#endif

#ifdef ENABLE_SWITCHBOT
	HEADER_GC_SWITCHBOT,
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
	HEADER_GC_EXCHANGE_INFO,
#endif

#ifdef ENABLE_VOICE_CHAT
	HEADER_GC_VOICE_CHAT,
#endif

#ifdef ENABLE_ADMIN_MANAGER
	HEADER_GC_ADMIN_MANAGER_LOAD,
	HEADER_GC_ADMIN_MANAGER,
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	HEADER_GC_BIOLOG_MANAGER,
#endif

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
	HEADER_GC_MARBLE_MANAGER,
#endif

#ifdef ENABLE_PARTY_ATLAS
	HEADER_GC_PARTY_POSITION,
#endif

#ifdef ENABLE_CUBE_RENEWAL
	HEADER_GC_CUBE_ITEM,
	HEADER_GC_CUBE_CRAFT,
#endif

#ifdef ENABLE_FIND_LETTERS_EVENT
	HEADER_GC_FIND_LETTERS_INFO,
#endif

#ifdef ENABLE_REFINE_ELEMENT
	HEADER_GC_REFINE_ELEMENT,
#endif

#ifdef ENABLE_LUCKY_BOX
	HEADER_GC_LUCKY_BOX,
#endif

#ifdef ENABLE_TEAMLER_STATUS
	HEADER_GC_SHOW_TEAMLER,
	HEADER_GC_TEAMLER_STATUS,
#endif

#ifdef __INVENTORY_BUFFERING__
	HEADER_GC_ITEM_BUFFERED,
#endif

#ifdef ENABLE_OFFLINE_SHOP
	HEADER_GC_OFFLINE_SHOP,
#endif

#ifdef ENABLE_SHOP_SEARCH
	HEADER_GC_SHOP_SEARCH_RESULT,
	HEADER_GC_SHOP_SEARCH_BUY_RESULT,
	HEADER_GC_SHOP_SEARCH_OWNER_MESSAGE,
	HEADER_GC_SHOP_SEARCH_SOLD_INFO,
#endif

#ifdef ENABLE_NEW_LOGS_CHAT
	HEADER_GC_CHAT_LOGS,
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	HEADER_GC_WHISPER_LANGUAGE_INFO,
#endif

#ifdef ENABLE_SHAMAN_SYSTEM
	HEADER_GC_SHAMAN_SKILL,
#endif

	// @fixme007
	HEADER_GC_UNK_213 = 213,

#ifdef ENABLE_PING_TIME
	HEADER_GC_PING_TIMER = 0xf9,
#endif
	HEADER_GC_KEY_AGREEMENT_COMPLETED = 0xfa, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_KEY_AGREEMENT = 0xfb, // _IMPROVED_PACKET_ENCRYPTION_
	HEADER_GC_HANDSHAKE_OK = 0xfc, // 252
	HEADER_GC_PHASE = 0xfd,	// 253
	HEADER_GC_BINDUDP = 0xfe, // 254
	HEADER_GC_HANDSHAKE = 0xff, // 255
};

#ifdef ENABLE_DELETE_SINGLE_STONE
enum ESubHeader : uint8_t
{
	SUBHEADER_REQUEST_DELETE_SOCKET_OPEN = 0,
	SUBHEADER_REQUEST_DELETE_SOCKET_DELETE,
	SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE,
};
#endif

enum
{
	ID_MAX_NUM = 30,
	PASS_MAX_NUM = 16,
	CHAT_MAX_NUM = 128,
	PATH_NODE_MAX_NUM = 64,
	SHOP_SIGN_MAX_LEN = 32,

	PLAYER_PER_ACCOUNT3 = 3,
#ifndef ENABLE_PLAYER_PER_ACCOUNT5
	PLAYER_PER_ACCOUNT4 = 4,
	PLAYER_PER_ACCOUNT5 = 4,
#else
	PLAYER_PER_ACCOUNT4 = 5,
	PLAYER_PER_ACCOUNT5 = 5,
#endif

	PLAYER_ITEM_SLOT_MAX_NUM = 20,

	QUICKSLOT_MAX_LINE = 4,
	QUICKSLOT_MAX_COUNT_PER_LINE = 8,
	QUICKSLOT_MAX_COUNT = QUICKSLOT_MAX_LINE * QUICKSLOT_MAX_COUNT_PER_LINE,

	QUICKSLOT_MAX_NUM = 36,

	SHOP_HOST_ITEM_MAX_NUM = 40,

	METIN_SOCKET_COUNT = 6,

	PARTY_AFFECT_SLOT_MAX_NUM = 7,

	GUILD_GRADE_NAME_MAX_LEN = 8,
	GUILD_NAME_MAX_LEN = 12,
	GUILD_GRADE_COUNT = 15,
	GULID_COMMENT_MAX_LEN = 50,

	MARK_CRC_NUM = 8 * 8,
	MARK_DATA_SIZE = 16 * 12,
	SYMBOL_DATA_SIZE = 128 * 256,
	QUEST_INPUT_STRING_MAX_NUM = 64,

	PRIVATE_CODE_LENGTH = 8,

	REFINE_MATERIAL_MAX_NUM = 5,

	CHINA_MATRIX_ANSWER_MAX_LEN	= 8,

	RUNUP_MATRIX_QUIZ_MAX_LEN	= 8,
	RUNUP_MATRIX_ANSWER_MAX_LEN = 4,
	NEWCIBN_PASSPOD_ANSWER_MAX_LEN = 8,
	NEWCIBN_PASSPOD_FAILURE_MAX_LEN = 128,

	WEAR_MAX_NUM = CItemData::WEAR_MAX_NUM,

	OPENID_AUTHKEY_LEN = 32,

	SHOP_TAB_NAME_MAX = 32,
	SHOP_TAB_COUNT_MAX = 3,

#ifdef __HARDWARE_BAN__
	HDID_MAX_LEN = 255,
#endif
};

#pragma pack(push)
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mark
typedef struct command_mark_login
{
	BYTE    header;
	DWORD   handle;
	DWORD   random_key;
} TPacketCGMarkLogin;

typedef struct command_mark_upload
{
	BYTE    header;
	DWORD   gid;
	BYTE    image[16 * 12 * 4];
} TPacketCGMarkUpload;

typedef struct command_mark_idxlist
{
	BYTE    header;
} TPacketCGMarkIDXList;

typedef struct command_mark_crclist
{
	BYTE    header;
	BYTE    imgIdx;
	DWORD   crclist[80];
} TPacketCGMarkCRCList;

typedef struct packet_mark_idxlist
{
	BYTE    header;
	DWORD	bufSize;
	WORD    count;
} TPacketGCMarkIDXList;

typedef struct packet_mark_block
{
	BYTE    header;
	DWORD   bufSize;
	BYTE	imgIdx;
	DWORD   count;
} TPacketGCMarkBlock;

typedef struct command_symbol_upload
{
	BYTE	header;
	WORD	size;
	DWORD	handle;
} TPacketCGSymbolUpload;

typedef struct command_symbol_crc
{
	BYTE	header;
	DWORD	dwGuildID;
	DWORD	dwCRC;
	DWORD	dwSize;
} TPacketCGSymbolCRC;

typedef struct packet_symbol_data
{
	BYTE header;
	WORD size;
	DWORD guild_id;
} TPacketGCGuildSymbolData;

//
//
//
typedef struct packet_observer_add
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverAdd;

typedef struct packet_observer_move
{
	BYTE	header;
	DWORD	vid;
	WORD	x;
	WORD	y;
} TPacketGCObserverMove;


typedef struct packet_observer_remove
{
	BYTE	header;
	DWORD	vid;
} TPacketGCObserverRemove;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// To Server

typedef struct command_checkin
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
} TPacketCGCheckin;

typedef struct command_login
{
	BYTE header;
	char name[ID_MAX_NUM + 1];
	char pwd[PASS_MAX_NUM + 1];
#ifdef __HARDWARE_BAN__
	BYTE	HDId[HDID_MAX_LEN + 1];
#endif
} TPacketCGLogin;

typedef struct command_login2
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	DWORD	login_key;
	DWORD	adwClientKey[4];
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	uint8_t	locale;
#endif
} TPacketCGLogin2;

typedef struct command_login3
{
	BYTE	header;
	char	name[ID_MAX_NUM + 1];
	char	pwd[PASS_MAX_NUM + 1];
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

typedef struct command_direct_enter
{
	BYTE        bHeader;
	char        login[ID_MAX_NUM + 1];
	char        passwd[PASS_MAX_NUM + 1];
	BYTE        index;
} TPacketCGDirectEnter;

typedef struct command_player_select
{
	BYTE	header;
	BYTE	player_index;
} TPacketCGSelectCharacter;

typedef struct command_attack
{
	BYTE	header;
	BYTE	bType;
	DWORD	dwVictimVID;
	BYTE	bCRCMagicCubeProcPiece;
	BYTE	bCRCMagicCubeFilePiece;
} TPacketCGAttack;

typedef struct command_chat
{
	BYTE	header;
	WORD	length;
	BYTE	type;
} TPacketCGChat;

typedef struct command_whisper
{
	BYTE        bHeader;
	WORD        wSize;
	char        szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisper;

typedef struct command_sms
{
	BYTE        bHeader;
	WORD        wSize;
	char        szNameTo[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGSMS;

enum EBattleMode
{
	BATTLEMODE_ATTACK = 0,
	BATTLEMODE_DEFENSE = 1,
};

typedef struct command_EnterFrontGame
{
	BYTE header;
} TPacketCGEnterFrontGame;

typedef struct command_item_use
{
	BYTE header;
	TItemPos pos;
} TPacketCGItemUse;

#ifdef ENABLE_MULTI_USE_PACKET
typedef struct command_item_multi_use
{
	BYTE header;
	TItemPos pos;
	BYTE count;
} TPacketCGItemMultiUse;
#endif

typedef struct command_item_use_to_item
{
	BYTE header;
	TItemPos source_pos;
	TItemPos target_pos;
} TPacketCGItemUseToItem;

typedef struct command_item_drop
{
	BYTE  header;
	TItemPos pos;
	DWORD elk;
} TPacketCGItemDrop;

typedef struct command_item_drop2
{
	BYTE        header;
	TItemPos pos;
	DWORD       gold;
	CountType	count;
} TPacketCGItemDrop2;

typedef struct command_item_move
{
	BYTE header;
	TItemPos pos;
	TItemPos change_pos;
	CountType		num;
} TPacketCGItemMove;

typedef struct command_item_pickup
{
	BYTE header;
	DWORD vid;
} TPacketCGItemPickUp;

#ifdef ENABLE_DESTROY_ITEM_PACKET
typedef struct command_item_destroy_packet
{
	BYTE		header;
	TItemPos	pos;
} TPacketCGItemDestroyPacket;
#endif

typedef struct command_quickslot_add
{
	BYTE        header;
	BYTE        pos;
	TQuickSlot	slot;
} TPacketCGQuickSlotAdd;

typedef struct command_quickslot_del
{
	BYTE        header;
	BYTE        pos;
} TPacketCGQuickSlotDel;

typedef struct command_quickslot_swap
{
	BYTE        header;
	BYTE        pos;
	BYTE        change_pos;
} TPacketCGQuickSlotSwap;

typedef struct command_on_click
{
	BYTE		header;
	DWORD		vid;
} TPacketCGOnClick;


enum
{
	SHOP_SUBHEADER_CG_END,
	SHOP_SUBHEADER_CG_BUY,
#ifdef ENABLE_MULTIPLE_BUY_SYSTEM
	SHOP_SUBHEADER_CG_BUY_MULTIPLE,
#endif
	SHOP_SUBHEADER_CG_SELL,
	SHOP_SUBHEADER_CG_SELL2,
};

typedef struct command_shop
{
	BYTE        header;
	BYTE		subheader;
} TPacketCGShop;

enum
{
	EXCHANGE_SUBHEADER_CG_START,			// arg1 == vid of target character
	EXCHANGE_SUBHEADER_CG_ITEM_ADD,		// arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ITEM_DEL,		// arg1 == position of item
	EXCHANGE_SUBHEADER_CG_ELK_ADD,			// arg1 == amount of elk
	EXCHANGE_SUBHEADER_CG_ACCEPT,			// arg1 == not used
	EXCHANGE_SUBHEADER_CG_CANCEL,			// arg1 == not used
};

typedef struct command_exchange
{
	BYTE		header;
	BYTE		subheader;
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t		arg1;
#else
	DWORD		arg1;
#endif
	BYTE		arg2;
	TItemPos	Pos;
} TPacketCGExchange;

typedef struct command_position
{
	BYTE        header;
	BYTE        position;
} TPacketCGPosition;

typedef struct command_script_answer
{
	BYTE        header;
	BYTE		answer;
} TPacketCGScriptAnswer;

typedef struct command_script_button
{
	BYTE        header;
	unsigned int			idx;
} TPacketCGScriptButton;

typedef struct command_target
{
	BYTE        header;
	DWORD       dwVID;
} TPacketCGTarget;

typedef struct command_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;
} TPacketCGMove;

typedef struct command_sync_position_element
{
	DWORD       dwVID;
	long        lX;
	long        lY;
} TPacketCGSyncPositionElement;

typedef struct command_sync_position
{
	BYTE        bHeader;
	WORD		wSize;
} TPacketCGSyncPosition;

typedef struct command_fly_targeting
{
	BYTE		bHeader;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketCGFlyTargeting;

typedef struct packet_fly_targeting
{
	BYTE        bHeader;
	DWORD		dwShooterVID;
	DWORD		dwTargetVID;
	long		lX;
	long		lY;
} TPacketGCFlyTargeting;

typedef struct packet_shoot
{
	BYTE		bHeader;
	BYTE		bType;
} TPacketCGShoot;

typedef struct command_warp
{
	BYTE			bHeader;
} TPacketCGWarp;

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

typedef struct packet_messenger_list_offline
{
	BYTE connected; // always 0
	BYTE length;
} TPacketGCMessengerListOffline;

enum
{
	MESSENGER_CONNECTED_STATE_OFFLINE,
	MESSENGER_CONNECTED_STATE_ONLINE,
	MESSENGER_CONNECTED_STATE_MOBILE,
};

typedef struct packet_messenger_list_online
{
	BYTE connected;
	BYTE length;
	//BYTE length_char_name;
} TPacketGCMessengerListOnline;

typedef struct packet_messenger_login
{
	//BYTE length_login;
	//BYTE length_char_name;
	BYTE length;
} TPacketGCMessengerLogin;

typedef struct packet_messenger_logout
{
	BYTE length;
} TPacketGCMessengerLogout;

enum
{
	MESSENGER_SUBHEADER_CG_ADD_BY_VID,
	MESSENGER_SUBHEADER_CG_ADD_BY_NAME,
	MESSENGER_SUBHEADER_CG_REMOVE,
};

typedef struct command_messenger
{
	BYTE header;
	BYTE subheader;
} TPacketCGMessenger;

typedef struct command_messenger_remove
{
	BYTE length;
} TPacketCGMessengerRemove;

enum
{
	SAFEBOX_MONEY_STATE_SAVE,
	SAFEBOX_MONEY_STATE_WITHDRAW,
};

typedef struct command_safebox_money
{
	BYTE        bHeader;
	BYTE        bState;
	DWORD       dwMoney;
} TPacketCGSafeboxMoney;

typedef struct command_safebox_checkout
{
	BYTE        bHeader;
	BYTE        bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckout;

typedef struct command_safebox_checkin
{
	BYTE        bHeader;
	BYTE        bSafePos;
	TItemPos	ItemPos;
} TPacketCGSafeboxCheckin;

typedef struct command_mall_checkout
{
	BYTE        bHeader;
	BYTE        bMallPos;
	TItemPos	ItemPos;
} TPacketCGMallCheckout;

///////////////////////////////////////////////////////////////////////////////////
// Party

typedef struct command_use_skill
{
	BYTE                bHeader;
	DWORD               dwVnum;
	DWORD				dwTargetVID;
} TPacketCGUseSkill;

typedef struct command_party_invite
{
	BYTE header;
	DWORD vid;
} TPacketCGPartyInvite;

typedef struct command_party_invite_answer
{
	BYTE header;
	DWORD leader_pid;
	BYTE accept;
} TPacketCGPartyInviteAnswer;

typedef struct command_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketCGPartyRemove;

typedef struct command_party_set_state
{
	BYTE byHeader;
	DWORD dwVID;
	BYTE byState;
	BYTE byFlag;
} TPacketCGPartySetState;

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

typedef struct command_party_use_skill
{
	BYTE byHeader;
	BYTE bySkillIndex;
	DWORD dwTargetVID;
} TPacketCGPartyUseSkill;

enum
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

typedef struct command_guild
{
	BYTE byHeader;
	BYTE bySubHeader;
} TPacketCGGuild;

typedef struct command_guild_answer_make_guild
{
	BYTE header;
	char guild_name[GUILD_NAME_MAX_LEN + 1];
} TPacketCGAnswerMakeGuild;

typedef struct command_give_item
{
	BYTE byHeader;
	DWORD dwTargetVID;
	TItemPos ItemPos;
	CountType byItemCount;
} TPacketCGGiveItem;

typedef struct SPacketCGHack
{
	BYTE        bHeader;
	char        szBuf[255 + 1];
} TPacketCGHack;

typedef struct command_dungeon
{
	BYTE		bHeader;
	WORD		size;
} TPacketCGDungeon;

// Private Shop
typedef struct SShopItemTable
{
	DWORD		vnum;
	CountType count;

	TItemPos	pos;
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t		price;
#else
	DWORD		price;
#endif
	BYTE		display_pos;
} TShopItemTable;

typedef struct SPacketCGMyShop
{
	BYTE        bHeader;
	char        szSign[SHOP_SIGN_MAX_LEN + 1];
	BYTE		bCount;
} TPacketCGMyShop;

typedef struct SPacketCGRefine
{
	BYTE		header;
	BYTE		pos;
	BYTE		type;
#ifdef ENABLE_FAST_REFINE_OPTION
	bool		fast_refine;
#endif
} TPacketCGRefine;

typedef struct SPacketCGChangeName
{
	BYTE header;
	BYTE index;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGChangeName;

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

typedef struct command_crc_report
{
	BYTE header;
	BYTE byPackMode;
	DWORD dwBinaryCRC32;
	DWORD dwProcessCRC32;
	DWORD dwRootPackCRC32;
} TPacketCGCRCReport;

typedef struct command_china_matrix_card
{
	BYTE	bHeader;
	char	szAnswer[CHINA_MATRIX_ANSWER_MAX_LEN + 1];
} TPacketCGChinaMatrixCard;

typedef struct command_runup_matrix_answer
{
	BYTE	bHeader;
	char	szAnswer[RUNUP_MATRIX_ANSWER_MAX_LEN + 1];
} TPacketCGRunupMatrixAnswer;

typedef struct command_newcibn_passpod_answer
{
	BYTE	bHeader;
	char	szAnswer[NEWCIBN_PASSPOD_ANSWER_MAX_LEN + 1];
} TPacketCGNEWCIBNPasspodAnswer;

enum EPartyExpDistributionType
{
	PARTY_EXP_DISTRIBUTION_NON_PARITY,
	PARTY_EXP_DISTRIBUTION_PARITY,
};

typedef struct command_party_parameter
{
	BYTE        bHeader;
	BYTE        bDistributeMode;
} TPacketCGPartyParameter;

typedef struct command_quest_input_string
{
	BYTE        bHeader;
	char		szString[QUEST_INPUT_STRING_MAX_NUM + 1];
} TPacketCGQuestInputString;

typedef struct command_quest_confirm
{
	BYTE header;
	BYTE answer;
	DWORD requestPID;
} TPacketCGQuestConfirm;

typedef struct command_script_select_item
{
	BYTE header;
	DWORD selection;
} TPacketCGScriptSelectItem;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// From Server
enum EPhase
{
	PHASE_CLOSE,
	PHASE_HANDSHAKE,
	PHASE_LOGIN,
	PHASE_SELECT,
	PHASE_LOADING,
	PHASE_GAME,
	PHASE_DEAD,

	PHASE_DBCLIENT_CONNECTING,
	PHASE_DBCLIENT,
	PHASE_P2P,
	PHASE_AUTH,
};

typedef struct packet_phase
{
	BYTE        header;
	BYTE        phase;
} TPacketGCPhase;

typedef struct packet_blank
{
	BYTE		header;
} TPacketGCBlank;

typedef struct packet_blank_dynamic
{
	BYTE		header;
	WORD		size;
} TPacketGCBlankDynamic;

typedef struct packet_header_handshake
{
	BYTE		header;
	DWORD		dwHandshake;
	DWORD		dwTime;
	LONG		lDelta;
} TPacketGCHandshake;

typedef struct packet_header_bindudp
{
	BYTE		header;
	DWORD		addr;
	WORD		port;
} TPacketGCBindUDP;

typedef struct packet_header_dynamic_size
{
	BYTE		header;
	WORD		size;
} TDynamicSizePacketHeader;

typedef struct SSimplePlayerInformation
{
	DWORD               dwID;
	char                szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE                byJob;
	BYTE                byLevel;
	DWORD               dwPlayMinutes;
	BYTE                byST, byHT, byDX, byIQ;
//	WORD				wParts[CRaceData::PART_MAX_NUM];
	WORD                wMainPart;
	BYTE                bChangeName;
	WORD				wHairPart;
#ifdef ENABLE_SASH_COSTUME_SYSTEM
	DWORD				wSashPart;
#endif
	BYTE                bDummy[4];
	long				x, y;
	LONG				lAddr;
	WORD				wPort;
	BYTE				bySkillGroup;
#ifdef ENABLE_OFFICIAL_SELECT_GUI
	DWORD				dwLastPlay;
#endif
} TSimplePlayerInformation;

typedef struct packet_login_success
{
	unsigned char						header;
	TSimplePlayerInformation	akSimplePlayerInformation[PLAYER_PER_ACCOUNT4];
	DWORD						guild_id[PLAYER_PER_ACCOUNT4];
	char						guild_name[PLAYER_PER_ACCOUNT4][GUILD_NAME_MAX_LEN + 1];
	DWORD handle;
	DWORD random_key;
	bool  hwid_check;
} TPacketGCLoginSuccess;

enum { LOGIN_STATUS_MAX_LEN = 8, BAN_REASON_MAX_LEN = 61 };
typedef struct packet_login_failure
{
	BYTE	header;
	char	szStatus[LOGIN_STATUS_MAX_LEN + 1];
	char	szReason[BAN_REASON_MAX_LEN];
	int		iValue;
} TPacketGCLoginFailure;

typedef struct command_player_create
{
	BYTE        header;
	BYTE        index;
	char        name[CHARACTER_NAME_MAX_LEN + 1];
	WORD        job;
	BYTE		shape;
	BYTE		CON;
	BYTE		INT;
	BYTE		STR;
	BYTE		DEX;
} TPacketCGCreateCharacter;

typedef struct command_player_create_success
{
	BYTE						header;
	BYTE						bAccountCharacterSlot;
	TSimplePlayerInformation	kSimplePlayerInfomation;
} TPacketGCPlayerCreateSuccess;

typedef struct command_create_failure
{
	BYTE	header;
	BYTE	bType;
} TPacketGCCreateFailure;

typedef struct command_player_delete
{
	BYTE        header;
	BYTE        index;
	char		szPrivateCode[PRIVATE_CODE_LENGTH];
} TPacketCGDestroyCharacter;

typedef struct packet_player_delete_success
{
	BYTE        header;
	BYTE        account_index;
} TPacketGCDestroyCharacterSuccess;

enum
{
	ADD_CHARACTER_STATE_DEAD   = (1 << 0),
	ADD_CHARACTER_STATE_SPAWN  = (1 << 1),
	ADD_CHARACTER_STATE_GUNGON = (1 << 2),
	ADD_CHARACTER_STATE_KILLER = (1 << 3),
	ADD_CHARACTER_STATE_PARTY  = (1 << 4),
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM,
};

enum ECharacterEquipmentPart
{
	CHR_EQUIPPART_ARMOR,
	CHR_EQUIPPART_WEAPON,
	CHR_EQUIPPART_HEAD,
	CHR_EQUIPPART_HAIR,

#ifdef ENABLE_SASH_COSTUME_SYSTEM
	CHR_EQUIPPART_SASH,
#endif

	CHR_EQUIPPART_NUM,
};

typedef struct packet_char_additional_info
{
	BYTE    header;
	DWORD   dwVID;
	char    name[CHARACTER_NAME_MAX_LEN + 1];
	DWORD    awPart[CHR_EQUIPPART_NUM];
	BYTE	bEmpire;
	DWORD   dwGuildID;
	DWORD   dwLevel;
	short   sAlignment;
	BYTE    bPKMode;
	DWORD   dwMountVnum;
#ifdef ENABLE_REFINE_ELEMENT
	BYTE	bRefineElementType;
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD	dwArrow;
#endif
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	BYTE	locale;
#endif
} TPacketGCCharacterAdditionalInfo;

typedef struct packet_add_char
{
	BYTE        header;

	DWORD       dwVID;

	float       angle;
	long        x;
	long        y;
	long        z;

	BYTE		bType;
	WORD        wRaceNum;
	BYTE        bMovingSpeed;
	BYTE        bAttackSpeed;
	BYTE		dwLevel;

	BYTE        bStateFlag;
	DWORD       dwAffectFlag[2];
} TPacketGCCharacterAdd;

typedef struct packet_update_char
{
	BYTE        header;
	DWORD       dwVID;

	DWORD        awPart[CHR_EQUIPPART_NUM];
	BYTE        bMovingSpeed;
	BYTE		bAttackSpeed;

	BYTE        bStateFlag;
	DWORD       dwAffectFlag[2];

	DWORD		dwGuildID;
	short       sAlignment;
	BYTE		bPKMode;
	DWORD		dwMountVnum;
#ifdef ENABLE_REFINE_ELEMENT
	BYTE		bRefineElementType;
#endif
#ifdef ENABLE_QUIVER_SYSTEM
	DWORD		dwArrow;
#endif
} TPacketGCCharacterUpdate;

typedef struct packet_del_char
{
	BYTE	header;
	DWORD	dwVID;
} TPacketGCCharacterDelete;

typedef struct packet_GlobalTime
{
	BYTE	header;
	float	GlobalTime;
} TPacketGCGlobalTime;

enum EChatType
{
	CHAT_TYPE_TALKING,
	CHAT_TYPE_INFO,
	CHAT_TYPE_NOTICE,
	CHAT_TYPE_PARTY,
	CHAT_TYPE_GUILD,
	CHAT_TYPE_COMMAND,
	CHAT_TYPE_SHOUT,
	CHAT_TYPE_WHISPER,
	CHAT_TYPE_BIG_NOTICE,
#ifdef ENABLE_DICE_SYSTEM
	CHAT_TYPE_DICE_INFO, //11
#endif
	CHAT_TYPE_MAX_NUM,
};

typedef struct packet_chatting
{
	BYTE	header;
	WORD	size;
	BYTE	type;
	DWORD	dwVID;
	BYTE	bEmpire;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	int		locale;
#endif
} TPacketGCChat;

typedef struct packet_whisper
{
	BYTE        bHeader;
	WORD        wSize;
	BYTE        bType;
#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	int			iLocale;
#endif
	char        szNameFrom[CHARACTER_NAME_MAX_LEN + 1];
#ifdef OFFLINE_MESSAGE_SYSTEM
	BYTE		bLevel;
	char		szSentDate[64];
#endif
} TPacketGCWhisper;

typedef struct packet_stun
{
	BYTE		header;
	DWORD		vid;
} TPacketGCStun;

#ifdef ENABLE_RENEWAL_DEAD_PACKET
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
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	packet_dead() { memset(&t_d, 0, sizeof(t_d)); }
#endif
	BYTE	header;
	DWORD	vid;
#ifdef ENABLE_RENEWAL_DEAD_PACKET
	DWORD	t_d[REVIVE_TYPE_MAX];
#endif
} TPacketGCDead;

typedef struct packet_main_character
{
	unsigned char        header;
	DWORD       dwVID;
	WORD		wRaceNum;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
	char		szBGMName[24 + 1];
	float		fBGMVol;
	LONG        lX, lY, lZ;
	unsigned char		byEmpire;
	unsigned char		bySkillGroup;
} TPacketGCMainCharacter;

enum EPointTypes
{
	POINT_NONE,                 // 0
	POINT_LEVEL,                // 1
	POINT_VOICE,                // 2
	POINT_EXP,                  // 3
	POINT_NEXT_EXP,             // 4
	POINT_HP,                   // 5
	POINT_MAX_HP,               // 6
	POINT_SP,                   // 7
	POINT_MAX_SP,               // 8
	POINT_STAMINA,              // 9
	POINT_MAX_STAMINA,          // 10

	POINT_GOLD,                 // 11
	POINT_ST,                   // 12
	POINT_HT,                   // 13
	POINT_DX,                   // 14
	POINT_IQ,                   // 15
	POINT_ATT_POWER,            // 16
	POINT_ATT_SPEED,            // 17
	POINT_EVADE_RATE,           // 18
	POINT_MOV_SPEED,            // 19
	POINT_DEF_GRADE,            // 20
	POINT_CASTING_SPEED,
	POINT_MAGIC_ATT_GRADE,      // 22
	POINT_MAGIC_DEF_GRADE,      // 23
	POINT_EMPIRE_POINT,         // 24
	POINT_LEVEL_STEP,           // 25
	POINT_STAT,                 // 26
	POINT_SUB_SKILL,            // 27
	POINT_SKILL,                // 28
	POINT_MIN_ATK,				// 29
	POINT_MAX_ATK,				// 30
	POINT_PLAYTIME,             // 31
	POINT_HP_REGEN,             // 32
	POINT_SP_REGEN,             // 33

	POINT_BOW_DISTANCE,         // 34

	POINT_HP_RECOVERY,          // 35
	POINT_SP_RECOVERY,          // 36

	POINT_POISON_PCT,           // 37
	POINT_STUN_PCT,             // 38
	POINT_SLOW_PCT,             // 39
	POINT_CRITICAL_PCT,         // 40
	POINT_PENETRATE_PCT,        // 41
	POINT_CURSE_PCT,            // 42

	POINT_ATTBONUS_HUMAN,       // 43
	POINT_ATTBONUS_ANIMAL,      // 44
	POINT_ATTBONUS_ORC,         // 45
	POINT_ATTBONUS_MILGYO,      // 46
	POINT_ATTBONUS_UNDEAD,      // 47
	POINT_ATTBONUS_DEVIL,       // 48
	POINT_ATTBONUS_INSECT,      // 49
	POINT_ATTBONUS_FIRE,        // 50
	POINT_ATTBONUS_ICE,         // 51
	POINT_ATTBONUS_DESERT,      // 52
	POINT_ATTBONUS_MONSTER,     // 53
	POINT_ATTBONUS_WARRIOR,     // 54
	POINT_ATTBONUS_ASSASSIN,	// 55
	POINT_ATTBONUS_SURA,		// 56
	POINT_ATTBONUS_SHAMAN,		// 57
	POINT_ATTBONUS_TREE,     	// 58
	POINT_RESIST_WARRIOR,		// 59
	POINT_RESIST_ASSASSIN,		// 60
	POINT_RESIST_SURA,			// 61
	POINT_RESIST_SHAMAN,		// 62

	POINT_STEAL_HP,             // 63
	POINT_STEAL_SP,             // 64

	POINT_MANA_BURN_PCT,        // 65

	POINT_DAMAGE_SP_RECOVER,    // 66

	POINT_BLOCK,                // 67
	POINT_DODGE,                // 68

	POINT_RESIST_SWORD,         // 69
	POINT_RESIST_TWOHAND,       // 70
	POINT_RESIST_DAGGER,        // 71
	POINT_RESIST_BELL,          // 72
	POINT_RESIST_FAN,           // 73
	POINT_RESIST_BOW,           // 74
	POINT_RESIST_FIRE,          // 75
	POINT_RESIST_ELEC,          // 76
	POINT_RESIST_MAGIC,         // 77
	POINT_RESIST_WIND,          // 78

	POINT_REFLECT_MELEE,        // 79

	POINT_REFLECT_CURSE,        // 80
	POINT_POISON_REDUCE,        // 81

	POINT_KILL_SP_RECOVER,      // 82
	POINT_EXP_DOUBLE_BONUS,     // 83
	POINT_GOLD_DOUBLE_BONUS,    // 84
	POINT_ITEM_DROP_BONUS,      // 85

	POINT_POTION_BONUS,         // 86
	POINT_KILL_HP_RECOVER,      // 87

	POINT_IMMUNE_STUN,          // 88
	POINT_IMMUNE_SLOW,          // 89
	POINT_IMMUNE_FALL,          // 90

	POINT_PARTY_ATT_GRADE,      // 91
	POINT_PARTY_DEF_GRADE,      // 92

	POINT_ATT_BONUS,            // 93
	POINT_DEF_BONUS,            // 94

	POINT_ATT_GRADE_BONUS,			// 95
	POINT_DEF_GRADE_BONUS,			// 96
	POINT_MAGIC_ATT_GRADE_BONUS,	// 97
	POINT_MAGIC_DEF_GRADE_BONUS,	// 98

	POINT_RESIST_NORMAL_DAMAGE,		// 99

	POINT_HIT_HP_RECOVERY,			// 100
	POINT_HIT_SP_RECOVERY, 			// 101
	POINT_MANASHIELD,				// 102

	POINT_PARTY_BUFFER_BONUS,		// 103
	POINT_PARTY_SKILL_MASTER_BONUS,	// 104

	POINT_HP_RECOVER_CONTINUE,		// 105
	POINT_SP_RECOVER_CONTINUE,		// 106

	POINT_STEAL_GOLD,				// 107
	POINT_POLYMORPH,				// 108
	POINT_MOUNT,					// 109

	POINT_PARTY_HASTE_BONUS,		// 110
	POINT_PARTY_DEFENDER_BONUS,		// 111
	POINT_STAT_RESET_COUNT,			// 112

	POINT_HORSE_SKILL,				// 113

	POINT_MALL_ATTBONUS,			// 114
	POINT_MALL_DEFBONUS,			// 115
	POINT_MALL_EXPBONUS,			// 116
	POINT_MALL_ITEMBONUS,			// 117
	POINT_MALL_GOLDBONUS,			// 118

	POINT_MAX_HP_PCT,				// 119
	POINT_MAX_SP_PCT,				// 120

	POINT_SKILL_DAMAGE_BONUS,		// 121
	POINT_NORMAL_HIT_DAMAGE_BONUS,	// 122

	// DEFEND_BONUS_ATTRIBUTES
	POINT_SKILL_DEFEND_BONUS,		// 123
	POINT_NORMAL_HIT_DEFEND_BONUS,	// 124
	// END_OF_DEFEND_BONUS_ATTRIBUTES

	// PC_BANG_ITEM_ADD
	POINT_PC_BANG_EXP_BONUS,		// 125
	POINT_PC_BANG_DROP_BONUS,		// 126
	// END_PC_BANG_ITEM_ADD
	POINT_RAMADAN_CANDY_BONUS_EXP,

	POINT_ENERGY = 128,				// 128

	POINT_ENERGY_END_TIME = 129,	// 129

	POINT_COSTUME_ATTR_BONUS = 130,
	POINT_MAGIC_ATT_BONUS_PER = 131,
	POINT_MELEE_MAGIC_ATT_BONUS_PER = 132,


	POINT_RESIST_ICE = 133,
	POINT_RESIST_EARTH = 134,
	POINT_RESIST_DARK = 135,

	POINT_RESIST_CRITICAL = 136,
	POINT_RESIST_PENETRATE = 137,

#ifdef ENABLE_SASH_COSTUME_SYSTEM
	POINT_ACCEDRAIN_RATE = 138,
#endif

#ifdef ENABLE_WOLFMAN_CHARACTER
	POINT_BLEEDING_REDUCE = 138,
	POINT_BLEEDING_PCT = 139,

	POINT_ATTBONUS_WOLFMAN = 140,
	POINT_RESIST_WOLFMAN = 141,
	POINT_RESIST_CLAW = 142,
#endif

#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	POINT_RESIST_MAGIC_REDUCTION = 144,
#endif

#ifdef ENABLE_GAYA_SYSTEM
	POINT_GAYA,
#endif

#ifdef ENABLE_12ZI_ELEMENT_ADD
	POINT_ATTBONUS_ELEC,
	POINT_ATTBONUS_WIND,
	POINT_ATTBONUS_EARTH,
	POINT_ATTBONUS_DARK,
	POINT_ATTBONUS_CZ,
#endif

	POINT_RESIST_ALL,
	POINT_ATTBONUS_METIN,
	POINT_ATTBONUS_BOSS,
	POINT_RESIST_MONSTER,
	POINT_RESIST_BOSS,

	POINT_PRECISION,

	POINT_DUNGEON_DAMAGE_BONUS,
	POINT_DUNGEON_RECV_DAMAGE_BONUS,
	POINT_AGGRO_MONSTER_BONUS,
	POINT_DOUBLE_ITEM_DROP_BONUS,

	POINT_MIN_WEP = 200,
	POINT_MAX_WEP,
	POINT_MIN_MAGIC_WEP,
	POINT_MAX_MAGIC_WEP,
	POINT_HIT_RATE,


};

typedef struct packet_points
{
	BYTE        header;

#ifdef EXTANDED_GOLD_AMOUNT
	int64_t	points[POINT_MAX_NUM];
#else
	long        points[POINT_MAX_NUM];
#endif
} TPacketGCPoints;

typedef struct packet_point_change
{
	int         header;

	DWORD		dwVID;
	BYTE		Type;

#ifdef EXTANDED_GOLD_AMOUNT
	int64_t	amount;
	int64_t	value;
#else
	long        amount;
	long        value;
#endif
} TPacketGCPointChange;

typedef struct packet_motion
{
	BYTE		header;
	DWORD		vid;
	DWORD		victim_vid;
	WORD		motion;
} TPacketGCMotion;

#if defined(GAIDEN)
struct TPacketGCItemDelDeprecate
{
	TPacketGCItemDelDeprecate() :
		header(HEADER_GC_ITEM_DEL),
		pos(0),
		vnum(0),
		count(0)
	{
		memset(&alSockets, 0, sizeof(alSockets));
		memset(&aAttr, 0, sizeof(aAttr));
	}

	BYTE    header;
	BYTE    pos;
	DWORD   vnum;
	BYTE    count;
	long    alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
};

typedef struct packet_set_item
{
	BYTE		header;
	BYTE		pos;
	DWORD		vnum;
	BYTE		count;
	DWORD		flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
} TPacketGCItemSet;

typedef struct packet_item_del
{
	BYTE        header;
	BYTE        pos;
} TPacketGCItemDel;
#else
typedef struct packet_set_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
	CountType	count;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif

} TPacketGCItemSet;

typedef struct packet_set_item2
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		vnum;
	CountType	count;
	DWORD		flags;
	DWORD		anti_flags;
	bool		highlight;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif
} TPacketGCItemSet2;
#endif

typedef struct packet_item_del
{
	BYTE        header;
	BYTE        pos;
} TPacketGCItemDel;

typedef struct packet_use_item
{
	BYTE		header;
	TItemPos	Cell;
	DWORD		ch_vid;
	DWORD		victim_vid;

	DWORD		vnum;
} TPacketGCItemUse;

typedef struct packet_update_item
{
	BYTE		header;
	TItemPos	Cell;
	CountType	count;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif
} TPacketGCItemUpdate;

typedef struct packet_ground_add_item
{
	BYTE        bHeader;
	long        lX;
	long		lY;
	long		lZ;

	DWORD       dwVID;
	DWORD       dwVnum;
#ifdef ENABLE_DROP_ITEM_COUNT
	CountType	count;
#endif
} TPacketGCItemGroundAdd;

typedef struct packet_ground_del_item
{
	BYTE		header;
	DWORD		vid;
} TPacketGCItemGroundDel;

typedef struct packet_item_ownership
{
	BYTE        bHeader;
	DWORD       dwVID;
	char        szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCItemOwnership;

typedef struct packet_quickslot_add
{
	BYTE        header;
	BYTE        pos;
	TQuickSlot	slot;
} TPacketGCQuickSlotAdd;

typedef struct packet_quickslot_del
{
	BYTE        header;
	BYTE        pos;
} TPacketGCQuickSlotDel;

typedef struct packet_quickslot_swap
{
	BYTE        header;
	BYTE        pos;
	BYTE        change_pos;
} TPacketGCQuickSlotSwap;

typedef struct packet_shop_start
{
	struct packet_shop_item		items[SHOP_HOST_ITEM_MAX_NUM];
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
	BYTE						pos;
	struct packet_shop_item		item;
} TPacketGCShopUpdateItem;

typedef struct packet_shop_update_price
{
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t	iElkAmount;
#else
	int iElkAmount;
#endif
} TPacketGCShopUpdatePrice;

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

typedef struct packet_shop
{
	BYTE        header;
	WORD		size;
	BYTE        subheader;
} TPacketGCShop;

typedef struct packet_exchange
{
	BYTE        header;
	BYTE        subheader;
	BYTE        is_me;
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t		arg1;
#else
	DWORD       arg1;
#endif
	TItemPos	arg2;
	CountType   arg3;
	long		alValues[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif
} TPacketGCExchange;

enum
{
	EXCHANGE_SUBHEADER_GC_START,			// arg1 == vid
	EXCHANGE_SUBHEADER_GC_ITEM_ADD,		// arg1 == vnum  arg2 == pos  arg3 == count
	EXCHANGE_SUBHEADER_GC_ITEM_DEL,		// arg1 == pos
	EXCHANGE_SUBHEADER_GC_ELK_ADD,			// arg1 == elk
	EXCHANGE_SUBHEADER_GC_ACCEPT,			// arg1 == accept
	EXCHANGE_SUBHEADER_GC_END,				// arg1 == not used
	EXCHANGE_SUBHEADER_GC_ALREADY,			// arg1 == not used
	EXCHANGE_SUBHEADER_GC_LESS_ELK,		// arg1 == not used
};

typedef struct packet_position
{
	BYTE        header;
	DWORD		vid;
	BYTE        position;
} TPacketGCPosition;

typedef struct packet_ping
{
	BYTE		header;
} TPacketGCPing;

typedef struct packet_pong
{
	BYTE		bHeader;
} TPacketCGPong;

typedef struct packet_script
{
	BYTE		header;
	WORD        size;
	BYTE		skin;
	WORD        src_size;
} TPacketGCScript;

typedef struct packet_target
{
	BYTE        header;
	DWORD       dwVID;
	long		lHP, lMaxHP;
	bool		isPoisoned;
} TPacketGCTarget;

typedef struct packet_damage_info
{
	BYTE header;
	DWORD dwVID;
	BYTE flag;
	int  damage;
} TPacketGCDamageInfo;

typedef struct packet_mount
{
	BYTE        header;
	DWORD       vid;
	DWORD       mount_vid;
	BYTE        pos;
	DWORD		_x, _y;
} TPacketGCMount;

typedef struct packet_change_speed
{
	BYTE		header;
	DWORD		vid;
	WORD		moving_speed;
} TPacketGCChangeSpeed;

typedef struct packet_move
{
	BYTE		bHeader;
	BYTE		bFunc;
	BYTE		bArg;
	BYTE		bRot;
	DWORD		dwVID;
	LONG		lX;
	LONG		lY;
	DWORD		dwTime;
	DWORD		dwDuration;
} TPacketGCMove;

enum
{
	QUEST_SEND_IS_BEGIN         = 1 << 0,
	QUEST_SEND_TITLE            = 1 << 1,
	QUEST_SEND_CLOCK_NAME       = 1 << 2,
	QUEST_SEND_CLOCK_VALUE      = 1 << 3,
	QUEST_SEND_COUNTER_NAME     = 1 << 4,
	QUEST_SEND_COUNTER_VALUE    = 1 << 5,
	QUEST_SEND_ICON_FILE		= 1 << 6,
};

typedef struct packet_quest_info
{
	BYTE header;
	WORD size;
	WORD index;
#ifdef ENABLE_QUEST_RENEWAL
	WORD c_index;
#endif
	BYTE flag;
} TPacketGCQuestInfo;

typedef struct packet_quest_confirm
{
	BYTE header;
	char msg[64 + 1];
	long timeout;
	DWORD requestPID;
} TPacketGCQuestConfirm;

typedef struct packet_attack
{
	BYTE        header;
	DWORD       dwVID;
	DWORD       dwVictimVID;
	BYTE        bType;
} TPacketGCAttack;

typedef struct packet_c2c
{
	BYTE		header;
	WORD		wSize;
} TPacketGCC2C;

typedef struct packetd_sync_position_element
{
	DWORD       dwVID;
	long        lX;
	long        lY;
} TPacketGCSyncPositionElement;

typedef struct packetd_sync_position
{
	BYTE        bHeader;
	WORD		wSize;
} TPacketGCSyncPosition;

typedef struct packet_ownership
{
	BYTE                bHeader;
	DWORD               dwOwnerVID;
	DWORD               dwVictimVID;
} TPacketGCOwnership;

#define	SKILL_MAX_NUM 255

typedef struct SPlayerSkill
{
	BYTE bMasterType;
	BYTE bLevel;
	time_t tNextRead;
} TPlayerSkill;

typedef struct packet_skill_level
{
	BYTE bHeader;
	TPlayerSkill skills[SKILL_MAX_NUM];
} TPacketGCSkillLevel;

// fly
typedef struct packet_fly
{
	BYTE        bHeader;
	BYTE        bType;
	DWORD       dwStartVID;
	DWORD       dwEndVID;
} TPacketGCCreateFly;

enum EPVPModes
{
	PVP_MODE_NONE,
	PVP_MODE_AGREE,
	PVP_MODE_FIGHT,
	PVP_MODE_REVENGE,
};

typedef struct packet_duel_start
{
	BYTE	header ;
	WORD	wSize ;
} TPacketGCDuelStart ;

typedef struct packet_pvp
{
	BYTE		header;
	DWORD		dwVIDSrc;
	DWORD		dwVIDDst;
	BYTE		bMode;
} TPacketGCPVP;

typedef struct packet_warp
{
	BYTE			bHeader;
	LONG			lX;
	LONG			lY;
	LONG			lAddr;
	WORD			wPort;
} TPacketGCWarp;

typedef struct packet_party_invite
{
	BYTE header;
	DWORD leader_pid;
} TPacketGCPartyInvite;

typedef struct packet_party_add
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCPartyAdd;

typedef struct packet_party_update
{
	BYTE header;
	DWORD pid;
	BYTE state;
#ifdef ENABLE_PARTY_UPDATE
	bool bLeader;
#endif
	BYTE percent_hp;
	short affects[PARTY_AFFECT_SLOT_MAX_NUM];
} TPacketGCPartyUpdate;

typedef struct packet_party_remove
{
	BYTE header;
	DWORD pid;
} TPacketGCPartyRemove;

#ifdef ENABLE_PARTY_ATLAS
typedef struct SPacketGCPartyPosition
{
	BYTE	bHeader;
	DWORD	dwPID;
	DWORD	dwX;
	DWORD	dwY;
} TPacketGCPartyPosition;
#endif

typedef TPacketCGSafeboxCheckout TPacketGCSafeboxCheckout;
typedef TPacketCGSafeboxCheckin TPacketGCSafeboxCheckin;

typedef struct packet_safebox_wrong_password
{
	BYTE        bHeader;
} TPacketGCSafeboxWrongPassword;

typedef struct packet_safebox_size
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCSafeboxSize;

typedef struct command_empire
{
	BYTE        bHeader;
	BYTE        bEmpire;
} TPacketCGEmpire;

typedef struct packet_empire
{
	BYTE        bHeader;
	BYTE        bEmpire;
} TPacketGCEmpire;

enum
{
	FISHING_SUBHEADER_GC_START,
	FISHING_SUBHEADER_GC_STOP,
	FISHING_SUBHEADER_GC_REACT,
	FISHING_SUBHEADER_GC_SUCCESS,
	FISHING_SUBHEADER_GC_FAIL,
	FISHING_SUBHEADER_GC_FISH,
};

typedef struct packet_fishing
{
	BYTE header;
	BYTE subheader;
	DWORD info;
	BYTE dir;
	DWORD length;
} TPacketGCFishing;

typedef struct paryt_parameter
{
	BYTE        bHeader;
	BYTE        bDistributeMode;
} TPacketGCPartyParameter;

//////////////////////////////////////////////////////////////////////////
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
	GUILD_SUBHEADER_GC_WAR_POINT,
	GUILD_SUBHEADER_GC_MONEY_CHANGE,
};

typedef struct packet_guild
{
	BYTE header;
	WORD size;
	BYTE subheader;
} TPacketGCGuild;

// SubHeader - Grade
enum
{
	GUILD_AUTH_ADD_MEMBER       = (1 << 0),
	GUILD_AUTH_REMOVE_MEMBER    = (1 << 1),
	GUILD_AUTH_NOTICE           = (1 << 2),
	GUILD_AUTH_SKILL            = (1 << 3),
};

typedef struct packet_guild_sub_grade
{
	char grade_name[GUILD_GRADE_NAME_MAX_LEN + 1];
	BYTE auth_flag;
} TPacketGCGuildSubGrade;

typedef struct packet_guild_sub_member
{
	DWORD pid;
	BYTE byGrade;
	BYTE byIsGeneral;
	BYTE byJob;
	BYTE byLevel;
	DWORD dwOffer;
	BYTE byNameFlag;
// if NameFlag is TRUE, name is sent from server.
//	char szName[CHARACTER_ME_MAX_LEN+1];
} TPacketGCGuildSubMember;

typedef struct packet_guild_sub_info
{
	WORD member_count;
	WORD max_member_count;
	DWORD guild_id;
	DWORD master_pid;
	DWORD exp;
	BYTE level;
	char name[GUILD_NAME_MAX_LEN + 1];
	DWORD gold;
	BYTE hasLand;
} TPacketGCGuildInfo;

enum EGuildWarState
{
	GUILD_WAR_NONE,
	GUILD_WAR_SEND_DECLARE,
	GUILD_WAR_REFUSE,
	GUILD_WAR_RECV_DECLARE,
	GUILD_WAR_WAIT_START,
	GUILD_WAR_CANCEL,
	GUILD_WAR_ON_WAR,
	GUILD_WAR_END,

	GUILD_WAR_DURATION = 2 * 60 * 60,
};

typedef struct packet_guild_war
{
	DWORD       dwGuildSelf;
	DWORD       dwGuildOpp;
	BYTE        bType;
	BYTE        bWarState;
} TPacketGCGuildWar;

typedef struct SPacketGuildWarPoint
{
	DWORD dwGainGuildID;
	DWORD dwOpponentGuildID;
	long lPoint;
} TPacketGuildWarPoint;

// SubHeader - Dungeon
enum
{
	DUNGEON_SUBHEADER_GC_TIME_ATTACK_START = 0,
	DUNGEON_SUBHEADER_GC_DESTINATION_POSITION = 1,
};

typedef struct packet_dungeon
{
	BYTE		bHeader;
	WORD		size;
	BYTE		subheader;
} TPacketGCDungeon;

// Private Shop
typedef struct SPacketGCShopSign
{
	BYTE        bHeader;
	DWORD       dwVID;
	char        szSign[SHOP_SIGN_MAX_LEN + 1];
} TPacketGCShopSign;

typedef struct SPacketGCTime
{
	BYTE        bHeader;
	time_t      time;
} TPacketGCTime;

enum
{
	WALKMODE_RUN,
	WALKMODE_WALK,
};

typedef struct SPacketGCWalkMode
{
	BYTE        header;
	DWORD       vid;
	BYTE        mode;
} TPacketGCWalkMode;

typedef struct SPacketGCChangeSkillGroup
{
	BYTE        header;
	BYTE        skill_group;
} TPacketGCChangeSkillGroup;

struct TMaterial
{
	DWORD vnum;
	DWORD count;
};

typedef struct SRefineTable
{
	DWORD src_vnum;
	DWORD result_vnum;
	CountType material_count;

	int cost;
	int prob;
	TMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SPacketGCRefineInformation
{
	BYTE			header;
	BYTE			type;
	BYTE			pos;
#ifdef ENABLE_FAST_REFINE_OPTION
	bool			fast_refine;
#endif
#ifdef SASH_ABSORPTION_ENABLE
	int				iSashRefine;
#endif
	int				addedProb;
	TRefineTable	refine_table;
} TPacketGCRefineInformation;

enum SPECIAL_EFFECT
{
	SE_NONE,
	SE_HPUP_RED,
	SE_SPUP_BLUE,
	SE_SPEEDUP_GREEN,
	SE_DXUP_PURPLE,
	SE_CRITICAL,
	SE_PENETRATE,
	SE_BLOCK,
	SE_DODGE,
	SE_CHINA_FIREWORK,
	SE_SPIN_TOP,
	SE_SUCCESS,
	SE_FAIL,
	SE_FR_SUCCESS,
	SE_LEVELUP_ON_14_FOR_GERMANY,
	SE_LEVELUP_UNDER_15_FOR_GERMANY,
	SE_PERCENT_DAMAGE1,
	SE_PERCENT_DAMAGE2,
	SE_PERCENT_DAMAGE3,
	SE_AUTO_HPUP,
	SE_AUTO_SPUP,
	SE_EQUIP_RAMADAN_RING,
	SE_EQUIP_HALLOWEEN_CANDY,
	SE_EQUIP_HAPPINESS_RING,
	SE_EQUIP_LOVE_PENDANT,
};

typedef struct SPacketGCSpecialEffect
{
	BYTE header;
	BYTE type;
	DWORD vid;
} TPacketGCSpecialEffect;

typedef struct SPacketGCNPCPosition
{
	BYTE header;
	WORD size;
	WORD count;
} TPacketGCNPCPosition;

struct TNPCPosition
{
	BYTE bType;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	long x;
	long y;
};

typedef struct SPacketGCChangeName
{
	BYTE header;
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGCChangeName;

enum EBlockAction
{
	BLOCK_EXCHANGE              = (1 << 0),
	BLOCK_PARTY_INVITE          = (1 << 1),
	BLOCK_GUILD_INVITE          = (1 << 2),
	BLOCK_WHISPER               = (1 << 3),
	BLOCK_MESSENGER_INVITE      = (1 << 4),
	BLOCK_PARTY_REQUEST         = (1 << 5),
};

typedef struct packet_china_matrixd_card
{
	BYTE	bHeader;
	DWORD	dwRows;
	DWORD	dwCols;
} TPacketGCChinaMatrixCard;

typedef struct packet_newcibn_passpod_request
{
	BYTE	bHeader;
} TPacketGCNEWCIBNPasspodRequest;

typedef struct packet_newcibn_passpod_failure
{
	BYTE	bHeader;
	char	szMessage[NEWCIBN_PASSPOD_FAILURE_MAX_LEN + 1];
} TPacketGCNEWCIBNPasspodFailure;

typedef struct packet_login_key
{
	BYTE	bHeader;
	DWORD	dwLoginKey;
} TPacketGCLoginKey;

typedef struct packet_auth_success
{
	BYTE        bHeader;
	DWORD       dwLoginKey;
	BYTE        bResult;
} TPacketGCAuthSuccess;

typedef struct packet_auth_success_openid
{
	BYTE        bHeader;
	DWORD       dwLoginKey;
	BYTE        bResult;
	char		login[ID_MAX_NUM + 1];
} TPacketGCAuthSuccessOpenID;

typedef struct packet_channel
{
	BYTE header;
	BYTE channel;
} TPacketGCChannel;

typedef struct SEquipmentItemSet
{
	DWORD   vnum;
	CountType	count;
	long    alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD	transmutation_id;
#endif
} TEquipmentItemSet;

typedef struct pakcet_view_equip
{
	BYTE header;
	DWORD dwVID;
	TEquipmentItemSet equips[WEAR_MAX_NUM];
} TPacketGCViewEquip;

typedef struct
{
	DWORD       dwID;
	long        x, y;
	long        width, height;
	DWORD       dwGuildID;
} TLandPacketElement;

typedef struct packet_land_list
{
	BYTE        header;
	WORD        size;
} TPacketGCLandList;

typedef struct
{
	BYTE        bHeader;
	long        lID;
	char        szTargetName[32 + 1];
} TPacketGCTargetCreate;

enum
{
	CREATE_TARGET_TYPE_NONE,
	CREATE_TARGET_TYPE_LOCATION,
	CREATE_TARGET_TYPE_CHARACTER,
};

typedef struct
{
	BYTE		bHeader;
	long		lID;
	char		szTargetName[32 + 1];
	DWORD		dwVID;
	BYTE		byType;
} TPacketGCTargetCreateNew;

typedef struct
{
	BYTE        bHeader;
	long        lID;
	long        lX, lY;
} TPacketGCTargetUpdate;

typedef struct
{
	BYTE        bHeader;
	long        lID;
} TPacketGCTargetDelete;

typedef struct
{
	DWORD       dwType;
	BYTE        bPointIdxApplyOn;
	long        lApplyValue;
	DWORD       dwFlag;
	long        lDuration;
	long        lSPCost;
} TPacketAffectElement;

typedef struct
{
	BYTE bHeader;
	TPacketAffectElement elem;
} TPacketGCAffectAdd;

typedef struct
{
	BYTE bHeader;
	DWORD dwType;
	BYTE bApplyOn;
} TPacketGCAffectRemove;

typedef struct packet_mall_open
{
	BYTE bHeader;
	BYTE bSize;
} TPacketGCMallOpen;

typedef struct packet_lover_info
{
	BYTE bHeader;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE byLovePoint;
} TPacketGCLoverInfo;

typedef struct packet_love_point_update
{
	BYTE bHeader;
	BYTE byLovePoint;
} TPacketGCLovePointUpdate;

typedef struct packet_dig_motion
{
	BYTE header;
	DWORD vid;
	DWORD target_vid;
	BYTE count;
} TPacketGCDigMotion;

typedef struct SPacketGCOnTime
{
	BYTE header;
	int ontime;     // sec
} TPacketGCOnTime;

typedef struct SPacketGCResetOnTime
{
	BYTE header;
} TPacketGCResetOnTime;

typedef struct SPacketGCPanamaPack
{
	BYTE    bHeader;
	char    szPackName[256];
	BYTE    abIV[32];
} TPacketGCPanamaPack;

typedef struct SPacketGCHybridCryptKeys
{
private:
	SPacketGCHybridCryptKeys() : m_pStream(NULL) {}

public:
	SPacketGCHybridCryptKeys(int iStreamSize) : iKeyStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridCryptKeys()
	{
		if ( m_pStream )
		{
			delete[] m_pStream;
			m_pStream = NULL;
		}
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE) + sizeof(WORD) + sizeof(int);
	}

	BYTE	bHeader;
	WORD    wDynamicPacketSize;
	int		iKeyStreamLen;
	BYTE*	m_pStream;

} TPacketGCHybridCryptKeys;


typedef struct SPacketGCHybridSDB
{
private:
	SPacketGCHybridSDB() : m_pStream(NULL) {}

public:
	SPacketGCHybridSDB(int iStreamSize) : iSDBStreamLen(iStreamSize)
	{
		m_pStream = new BYTE[iStreamSize];
	}
	~SPacketGCHybridSDB()
	{
		delete[] m_pStream;
		m_pStream = NULL;
	}
	static int GetFixedHeaderSize()
	{
		return sizeof(BYTE) + sizeof(WORD) + sizeof(int);
	}

	BYTE	bHeader;
	WORD    wDynamicPacketSize;
	int		iSDBStreamLen;
	BYTE*	m_pStream;

} TPacketGCHybridSDB;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Client To Client

typedef struct packet_state
{
	BYTE			bHeader;
	BYTE			bFunc;
	BYTE			bArg;
	BYTE			bRot;
	DWORD			dwVID;
	DWORD			dwTime;
	TPixelPosition	kPPos;
} TPacketCCState;

typedef struct packet_hs_check_req
{
	BYTE	bHeader;
	AHNHS_TRANS_BUFFER	Req;
} TPacketHSCheck;

typedef struct packet_xtrap_verify
{
	BYTE	bHeader;
	BYTE	bPacketData[128];

} TPacketXTrapCSVerify;
// AUTOBAN
typedef struct packet_autoban_quiz
{
	BYTE bHeader;
	BYTE bDuration;
	BYTE bCaptcha[64 * 32];
	char szQuiz[256];
} TPacketGCAutoBanQuiz;
// END_OF_AUTOBAN

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

typedef struct SPacketGCSpecificEffect
{
	BYTE header;
	DWORD vid;
	char effect_file[128];
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
	DS_SUB_HEADER_DO_UPGRADE,
#ifdef ENABLE_DS_REFINE_ALL
	DS_SUB_HEADER_DO_UPGRADE_ALL,
#endif
	DS_SUB_HEADER_DO_IMPROVEMENT,
	DS_SUB_HEADER_DO_REFINE,
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
	TItemPos ItemGrid[DS_REFINE_WINDOW_MAX_NUM];
} TPacketCGDragonSoulRefine;

typedef struct SPacketGCDragonSoulRefine
{
	SPacketGCDragonSoulRefine() : header(HEADER_GC_DRAGON_SOUL_REFINE)
	{}
	BYTE header;
	BYTE bSubType;
	TItemPos Pos;
} TPacketGCDragonSoulRefine;

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;

// @fixme007 length 2
typedef struct packet_unk_213
{
	BYTE bHeader;
	BYTE bUnk2;
} TPacketGCUnk213;

#ifdef ENABLE_SWITCHBOT
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
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
};
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
typedef struct packet_exchange_info
{
	BYTE        bHeader;
	WORD        wSize;
	bool        bError;
	int			iUnixTime;
} TPacketGCExchageInfo;
#endif

#ifdef ENABLE_VOICE_CHAT
typedef struct SPacketCGVoice
{
	BYTE	header;
	short	data[1024];
} TPacketCGVoice;

typedef struct SPacketGCVoiceChat
{
	BYTE	header;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
	short	data[1024];
	float	distance;
} TPacketGCVoiceChat;
#endif

#ifdef ENABLE_NEW_FILTERS
typedef struct command_items_pickup
{
	BYTE header;
	DWORD items[20];
} TPacketCGItemsPickUp;
#endif

#ifdef ENABLE_ADMIN_MANAGER
enum EAdminManagerCGSubHeaders
{
	ADMIN_MANAGER_CG_SUBHEADER_MAPVIEWER_START,
	ADMIN_MANAGER_CG_SUBHEADER_MAPVIEWER_STOP,
	ADMIN_MANAGER_CG_SUBHEADER_OBSERVER_START,
	ADMIN_MANAGER_CG_SUBHEADER_OBSERVER_STOP,
	ADMIN_MANAGER_CG_SUBHEADER_BAN_CHAT_SEARCH,
	ADMIN_MANAGER_CG_SUBHEADER_BAN_REQUEST_LOG,
	ADMIN_MANAGER_CG_SUBHEADER_BAN_ACCOUNT_SEARCH,
	ADMIN_MANAGER_CG_SUBHEADER_BAN_ACCOUNT,
	ADMIN_MANAGER_CG_SUBHEADER_ITEM_SEARCH,

	ADMIN_MANAGER_CG_SUBHEADER_GET_LOGS,
};

enum EAdminManagerGCSubHeaders
{
	ADMIN_MANAGER_GC_SUBHEADER_NONE,
	ADMIN_MANAGER_GC_SUBHEADER_PLAYER_ONLINE,
	ADMIN_MANAGER_GC_SUBHEADER_PLAYER_OFFLINE,
	ADMIN_MANAGER_GC_SUBHEADER_GM_ITEM_TRADE_BLOCK,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_LOAD,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_MOVE,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_APPEND,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_PLAYER_DESTROY,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_MOVE,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_APPEND,
	ADMIN_MANAGER_GC_SUBHEADER_MAPVIEWER_MONSTER_DESTROY,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_LOAD,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILLGROUP,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_UPDATE,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_UPDATE_ALL,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_SKILL_COOLDOWN,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_POINT_UPDATE,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ITEM_SET,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ITEM_DEL,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_CHATBAN_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_ACCOUNTBAN_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_OBSERVER_STOP_FORCED,
	ADMIN_MANAGER_GC_SUBHEADER_BAN_CHAT_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_BAN_CHAT_SEARCH_PLAYER,
	ADMIN_MANAGER_GC_SUBHEADER_BAN_PLAYER_LOG,
	ADMIN_MANAGER_GC_SUBHEADER_BAN_ACCOUNT_STATE,
	ADMIN_MANAGER_GC_SUBHEADER_BAN_ACCOUNT_SEARCH,
	ADMIN_MANAGER_GC_SUBHEADER_ITEM_SEARCH,

	ADMIN_MANAGER_GC_SUBHEADER_RECV_LOGS,
};

typedef struct SAdminManagerPlayerInfo
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	LONG lMapIndex;
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
	LONG alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
	bool is_gm_item;
} TAdminManagerObserverItemInfo;

typedef struct SAdminManagerBanClientPlayerInfo
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bRace;
	BYTE bLevel;
	WORD wChatbanCount;
	WORD wAccountbanCount;
	DWORD dwDuration;
	bool bIsOnline;
} TAdminManagerBanClientPlayerInfo;

typedef struct SAdminManagerBanClientLogInfo
{
	DWORD dwPlayerID;
	char szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwGMPlayerID;
	char szGMName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bType;
	int iNewDuration;
	int iReasonStrLen;
	int iProofStrLen;
	char szDate[20 + 1];
} TAdminManagerBanClientLogInfo;

typedef struct SAdminManagerBanClientAccountInfo
{
	DWORD dwAID;
	char szLoginName[ID_MAX_NUM + 1];
	DWORD dwPID[PLAYER_PER_ACCOUNT5];
	char szName[PLAYER_PER_ACCOUNT5][CHARACTER_NAME_MAX_LEN + 1];
	BYTE bRace[PLAYER_PER_ACCOUNT5];
	BYTE bLevel[PLAYER_PER_ACCOUNT5];
	WORD wChatbanCount[PLAYER_PER_ACCOUNT5];
	WORD wAccountbanCount;
	DWORD dwDuration;
	bool bIsOnline[PLAYER_PER_ACCOUNT5];
} TAdminManagerBanClientAccountInfo;

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

typedef struct SAdminManagerBanPlayerInfo
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bRace;
	BYTE bLevel;
	WORD wChatbanCount;
	WORD wAccbanCount;
	DWORD dwTimeout;
	bool bIsOnline;
} TAdminManagerBanPlayerInfo;

typedef struct SAdminManagerBanLogInfo
{
	DWORD dwPlayerID;
	char szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
	DWORD dwGMPlayerID;
	char szGMName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE bType;
	int iNewDuration;
	char szReason[128 + 1];
	char szProof[256 + 1];
	char szDate[20 + 1];
} TAdminManagerBanLogInfo;

typedef struct SAdminManagerItemInfo
{
	BYTE bOwnerType;
	DWORD dwOwnerID;
	BYTE bOwnerNameLen;
	DWORD dwItemID;
	TItemPos kCell;
	DWORD dwItemVnum;
	BYTE bCount;
	LONG alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
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

#ifdef ENABLE_BIOLOG_SYSTEM
enum ECGBiologSubHeaders
{
	CG_BIOLOG_MANAGER_OPEN,
	CG_BIOLOG_MANAGER_SEND,
	CG_BIOLOG_MANAGER_TIMER,
};

enum EGCBiologSubHeaders
{
	GC_BIOLOG_MANAGER_OPEN,
	GC_BIOLOG_MANAGER_ALERT,
	GC_BIOLOG_MANAGER_CLOSE,
};

typedef struct SPacketCGBiologManagerAction
{
	BYTE bHeader;
	BYTE bSubHeader;
} TPacketCGBiologManagerAction;

typedef struct SPacketGCBiologManager
{
	BYTE	bHeader;
	WORD	wSize;
	BYTE	bSubHeader;
} TPacketGCBiologManager;

typedef struct SPacketGCBiologManagerInfo
{
	bool	bUpdate;
	BYTE	bRequiredLevel;
	DWORD	iRequiredItem;
	WORD	wGivenItems;
	WORD	wRequiredItemCount;
	time_t	iGlobalCooldown;
	time_t	iCooldown;
	bool	iCooldownReminder;
	BYTE	bChance;
	DWORD	bApplyType[MAX_BONUSES_LENGTH];
	long	lApplyValue[MAX_BONUSES_LENGTH];
	DWORD	dRewardItem;
	WORD	wRewardItemCount;
} TPacketGCBiologManagerInfo;
#endif

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
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

#ifdef ENABLE_CUBE_RENEWAL
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

#ifdef ENABLE_FIND_LETTERS_EVENT
typedef struct SPacketGCFindLettersInfo
{
	BYTE	bHeader;
	WORD	wSize;
	TPlayerLetterSlot letterSlots[FIND_LETTERS_SLOTS_NUM];
} TPacketGCFindLettersInfo;
#endif

#ifdef ENABLE_REFINE_ELEMENT
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

#ifdef ENABLE_LUCKY_BOX
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

#ifdef ENABLE_TEAMLER_STATUS
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

#ifdef ENABLE_SHOP_SEARCH
typedef std::pair<uint32_t, uint8_t> TShopSearchItemID;

enum EShopSearchData
{
	SHOPSEARCH_ITEM_SUBTYPE_MAX_VALUE = 10,
	SHOPSEARCH_SOLD_ITEM_INFO_COUNT = 30,

	SHOPSEARCH_BUY_SUCCESS = 0,
	SHOPSEARCH_BUY_NOT_EXIST,
	SHOPSEARCH_BUY_PRICE_CHANGE,
	SHOPSEARCH_BUY_TIMEOUT,
	SHOPSEARCH_BUY_NO_PEER,
	SHOPSEARCH_BUY_UNKNOWN_ERROR,

	SHOPSEARCH_SORT_RANDOM = 0,
	SHOPSEARCH_SORT_ASC,
	SHOPSEARCH_SORT_DESC,
	SHOPSEARCH_SORT_MAX_NUM,
};

enum EShopSearchAveragePriceLevels
{
	SHOPSEARCH_AVG_PRICE_GOOD,
	SHOPSEARCH_AVG_PRICE_NORMAL,
	SHOPSEARCH_AVG_PRICE_BAD,
	SHOPSEARCH_AVG_PRICE_WORST,
};

typedef struct SShopSearchClientItem
{
	DWORD	id;
	BYTE	window;
	WORD	pos;
	DWORD	count;

	DWORD	vnum;
	bool	is_gm_owner;
	long	alSockets[ITEM_SOCKET_SLOT_MAX_NUM];

	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];

	DWORD	owner;

	DWORD	transmutate_id;
	DWORD	dwRefineElement;

	TShopSearchItemID offlineID;
	int64_t price;
	DWORD endTime;

	BYTE	avgPriceLevel;
} TShopSearchClientItem;

typedef struct SShopSearchOptions
{
	BYTE		typeFlagCount;
	BYTE		specificVnumCount;
} TShopSearchOptions;

typedef std::pair<DWORD, DWORD> TShopSearchItemType;

typedef struct SPacketGCShopSearchResult
{
	BYTE	header;
	WORD	size;
	WORD	itemCount;
	WORD	maxPageNum;
} TPacketGCShopSearchResult;

typedef struct SPacketCGShopSearchByName
{
	BYTE	header;
	char	itemName[CItemData::ITEM_NAME_MAX_LEN + 1];
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

typedef struct SPacketCGShopSearchBuy
{
	BYTE	header;
	TShopSearchItemID	itemID;
	DWORD	itemVnum;
	int64_t	itemPrice;//try this.yea, try t his for now but wait
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

typedef struct SShopSearchSoldItemInfo
{
	SShopSearchSoldItemInfo() : count(0), averagePrice(0) { }

	uint64_t	count;
	double		averagePrice;
} TShopSearchSoldItemInfo;

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

#ifdef ENABLE_TARGET_MONSTER_LOOT
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

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
typedef struct SPacketCGWhisperRequestLanguage {
	BYTE	header;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketCGWhisperRequestLanguage;

typedef struct SPacketGCWhisperLanguageInfo {
	BYTE	header;
	char	name[CHARACTER_NAME_MAX_LEN + 1];
	BYTE	language;
} TPacketGCWhisperLanguageInfo;
#endif

#ifdef ENABLE_SHAMAN_SYSTEM
typedef struct shaman_use_skill
{
	BYTE	bHeader;
	DWORD	dwVnum;
	DWORD	dwVid;
	BYTE	dwLevel;
}TPacketGCShamanUseSkill;
#endif

#pragma pack(pop)
