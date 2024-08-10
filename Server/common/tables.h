#ifndef __INC_TABLES_H__
#define __INC_TABLES_H__

#include "length.h"
#include "item_length.h"
#include "CommonDefines.h"

#include <string>
#include <vector>

#include <array>
#include <cstdint>
#include <utility>
#include <functional>
#include <cstring>

typedef	DWORD IDENT;


enum
{
	HEADER_GD_LOGIN								= 1,
	HEADER_GD_LOGOUT							= 2,

	HEADER_GD_PLAYER_LOAD						= 3,
	HEADER_GD_PLAYER_SAVE						= 4,
	HEADER_GD_PLAYER_CREATE						= 5,
	HEADER_GD_PLAYER_DELETE						= 6,

	HEADER_GD_LOGIN_KEY							= 7,

	HEADER_GD_BOOT								= 8,
	HEADER_GD_PLAYER_COUNT						= 9,
	HEADER_GD_QUEST_SAVE						= 10,
	HEADER_GD_SAFEBOX_LOAD						= 11,
	HEADER_GD_SAFEBOX_SAVE						= 12,
	HEADER_GD_SAFEBOX_CHANGE_SIZE				= 13,
	HEADER_GD_EMPIRE_SELECT						= 14,

	HEADER_GD_SAFEBOX_CHANGE_PASSWORD			= 15,
	HEADER_GD_SAFEBOX_CHANGE_PASSWORD_SECOND	= 16, // Not really a packet, used internal
	HEADER_GD_DIRECT_ENTER						= 17,

	HEADER_GD_GUILD_SKILL_UPDATE				= 18,
	HEADER_GD_GUILD_EXP_UPDATE					= 19,
	HEADER_GD_GUILD_ADD_MEMBER					= 20,
	HEADER_GD_GUILD_REMOVE_MEMBER				= 21,
	HEADER_GD_GUILD_CHANGE_GRADE				= 22,
	HEADER_GD_GUILD_CHANGE_MEMBER_DATA			= 23,
	HEADER_GD_GUILD_DISBAND						= 24,
	HEADER_GD_GUILD_WAR							= 25,
	HEADER_GD_GUILD_WAR_SCORE					= 26,
	HEADER_GD_GUILD_CREATE						= 27,

	HEADER_GD_ITEM_SAVE							= 28,
	HEADER_GD_ITEM_DESTROY						= 29,

	HEADER_GD_ADD_AFFECT						= 30,
	HEADER_GD_REMOVE_AFFECT						= 31,

	HEADER_GD_HIGHSCORE_REGISTER				= 32,
	HEADER_GD_ITEM_FLUSH						= 33,

	HEADER_GD_PARTY_CREATE						= 34,
	HEADER_GD_PARTY_DELETE						= 35,
	HEADER_GD_PARTY_ADD							= 36,
	HEADER_GD_PARTY_REMOVE						= 37,
	HEADER_GD_PARTY_STATE_CHANGE				= 38,
	HEADER_GD_PARTY_HEAL_USE					= 39,

	HEADER_GD_FLUSH_CACHE						= 40,
	HEADER_GD_RELOAD_PROTO						= 41,

	HEADER_GD_CHANGE_NAME						= 42,
	HEADER_GD_SMS								= 43,

	HEADER_GD_GUILD_CHANGE_LADDER_POINT			= 44,
	HEADER_GD_GUILD_USE_SKILL					= 45,

	HEADER_GD_REQUEST_EMPIRE_PRIV				= 46,
	HEADER_GD_REQUEST_GUILD_PRIV				= 47,

	HEADER_GD_MONEY_LOG							= 48,

	HEADER_GD_GUILD_DEPOSIT_MONEY				= 49,
	HEADER_GD_GUILD_WITHDRAW_MONEY				= 50,
	HEADER_GD_GUILD_WITHDRAW_MONEY_GIVE_REPLY	= 51,

	HEADER_GD_REQUEST_CHARACTER_PRIV			= 52,

	HEADER_GD_SET_EVENT_FLAG					= 53,

	HEADER_GD_PARTY_SET_MEMBER_LEVEL			= 54,

	HEADER_GD_GUILD_WAR_BET						= 55,

	HEADER_GD_CREATE_OBJECT						= 56,
	HEADER_GD_DELETE_OBJECT						= 57,
	HEADER_GD_UPDATE_LAND						= 58,

	HEADER_GD_MARRIAGE_ADD						= 59,
	HEADER_GD_MARRIAGE_UPDATE					= 60,
	HEADER_GD_MARRIAGE_REMOVE					= 61,

	HEADER_GD_WEDDING_REQUEST					= 62,
	HEADER_GD_WEDDING_READY						= 63,
	HEADER_GD_WEDDING_END						= 64,

	HEADER_GD_AUTH_LOGIN						= 65,
	HEADER_GD_LOGIN_BY_KEY						= 66,
	HEADER_GD_BILLING_EXPIRE					= 67,
	HEADER_GD_VCARD								= 68,
	HEADER_GD_BILLING_CHECK						= 69,
	HEADER_GD_MALL_LOAD							= 70,

	HEADER_GD_MYSHOP_PRICELIST_UPDATE			= 71,
	HEADER_GD_MYSHOP_PRICELIST_REQ				= 72,

	HEADER_GD_BLOCK_CHAT						= 73,

	// PCBANG_IP_LIST_BY_AUTH
	HEADER_GD_PCBANG_REQUEST_IP_LIST			= 74,
	HEADER_GD_PCBANG_CLEAR_IP_LIST				= 75,
	HEADER_GD_PCBANG_INSERT_IP					= 76,
	// END_OF_PCBANG_IP_LIST_BY_AUTH

	HEADER_GD_HAMMER_OF_TOR						= 77,
	HEADER_GD_RELOAD_ADMIN						= 78,
	HEADER_GD_BREAK_MARRIAGE					= 79,

	HEADER_GD_BLOCK_COUNTRY_IP					= 90,
	HEADER_GD_BLOCK_EXCEPTION					= 91,

	HEADER_GD_REQ_CHANGE_GUILD_MASTER			= 92,

	HEADER_GD_REQ_SPARE_ITEM_ID_RANGE			= 93,

	HEADER_GD_UPDATE_HORSE_NAME					= 94,
	HEADER_GD_REQ_HORSE_NAME					= 95,

	HEADER_GD_DC								= 96,

	HEADER_GD_VALID_LOGOUT						= 97,

	HEADER_GD_REQUEST_CHARGE_CASH				= 100,

	HEADER_GD_DELETE_AWARDID					= 101,	// delete gift notify icon

	HEADER_GD_UPDATE_CHANNELSTATUS				= 102,
	HEADER_GD_REQUEST_CHANNELSTATUS				= 103,

#ifdef __ENABLE_CHANGE_CHANNEL__
	HEADER_GD_CHANNEL_SWITCH					= 105,
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	HEADER_GD_OFFLINE_MESSAGE					= 106,
#endif

#ifdef __ADMIN_MANAGER__
	HEADER_GD_ADMIN_MANAGER_CHAT_SEARCH_PLAYER	= 107,
#endif

#ifdef __BATTLE_PASS_ENABLE__
	HEADER_GD_BATTLEPASS_RANKING				= 108,
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	HEADER_GD_REQUEST_OFFLINE_SHOP_ID,
	HEADER_GD_OFFLINE_SHOP_SAVE,
	HEADER_GD_OFFLINE_SHOP_SAVE_ITEM,
	HEADER_GD_OFFLINE_SHOP_DESTROY,
#endif

#ifdef __SHOP_SEARCH__
	HEADER_GD_SHOP_SEARCH_REGISTER_ITEM,
	HEADER_GD_SHOP_SEARCH_UNREGISTER_ITEM,
	HEADER_GD_SHOP_SEARCH_SOLD_ITEM,
	HEADER_GD_SHOP_SEARCH_BY_NAME,
	HEADER_GD_SHOP_SEARCH_BY_OPTIONS,
	HEADER_GD_SHOP_SEARCH_REQUEST_BUY,
	HEADER_GD_SHOP_SEARCH_BUY_FROM_SHOP_ERROR,
	HEADER_GD_SHOP_SEARCH_BOUGHT_FROM_SHOP,
	HEADER_GD_SHOP_SEARCH_REQUEST_SOLD_INFO,
	HEADER_GD_SHOP_SEARCH_RELOAD_AVERAGE,
#endif

#ifdef __ITEM_SHOP_ENABLE__
	HEADER_GD_ITEMSHOP,
#endif

	#ifdef __DUNGEON_INFO_ENABLE__
		HEADER_GD_DUNGEONINFO_RANKING,
		HEADER_GD_DUNGEONINFO_RANKING_BOOT,
	#endif

	#ifdef __HARDWARE_BAN__
		HEADER_GD_UPDATE_BAN,
	#endif

	HEADER_GD_SETUP								= 0xff,

	///////////////////////////////////////////////
	HEADER_DG_NOTICE							= 1,

	HEADER_DG_LOGIN_SUCCESS						= 2,
	HEADER_DG_LOGIN_NOT_EXIST					= 3,
	HEADER_DG_LOGIN_WRONG_PASSWD				= 4,
	HEADER_DG_LOGIN_ALREADY						= 5,

	HEADER_DG_PLAYER_LOAD_SUCCESS				= 6,
	HEADER_DG_PLAYER_LOAD_FAILED				= 7,
	HEADER_DG_PLAYER_CREATE_SUCCESS				= 8,
	HEADER_DG_PLAYER_CREATE_ALREADY				= 9,
	HEADER_DG_PLAYER_CREATE_FAILED				= 10,
	HEADER_DG_PLAYER_DELETE_SUCCESS				= 11,
	HEADER_DG_PLAYER_DELETE_FAILED				= 12,

	HEADER_DG_ITEM_LOAD							= 13,

	HEADER_DG_BOOT								= 14,
	HEADER_DG_QUEST_LOAD						= 15,

	HEADER_DG_SAFEBOX_LOAD						= 16,
	HEADER_DG_SAFEBOX_CHANGE_SIZE				= 17,
	HEADER_DG_SAFEBOX_WRONG_PASSWORD			= 18,
	HEADER_DG_SAFEBOX_CHANGE_PASSWORD_ANSWER	= 19,

	HEADER_DG_EMPIRE_SELECT						= 20,

	HEADER_DG_AFFECT_LOAD						= 21,
	HEADER_DG_MALL_LOAD							= 22,

	HEADER_DG_DIRECT_ENTER						= 23,

	HEADER_DG_GUILD_SKILL_UPDATE				= 24,
	HEADER_DG_GUILD_SKILL_RECHARGE				= 25,
	HEADER_DG_GUILD_EXP_UPDATE					= 26,

	HEADER_DG_PARTY_CREATE						= 27,
	HEADER_DG_PARTY_DELETE						= 28,
	HEADER_DG_PARTY_ADD							= 29,
	HEADER_DG_PARTY_REMOVE						= 30,
	HEADER_DG_PARTY_STATE_CHANGE				= 31,
	HEADER_DG_PARTY_HEAL_USE					= 32,
	HEADER_DG_PARTY_SET_MEMBER_LEVEL			= 33,

	HEADER_DG_TIME								= 34,
	HEADER_DG_ITEM_ID_RANGE						= 35,

	HEADER_DG_GUILD_ADD_MEMBER					= 36,
	HEADER_DG_GUILD_REMOVE_MEMBER				= 37,
	HEADER_DG_GUILD_CHANGE_GRADE				= 38,
	HEADER_DG_GUILD_CHANGE_MEMBER_DATA			= 39,
	HEADER_DG_GUILD_DISBAND						= 40,
	HEADER_DG_GUILD_WAR							= 41,
	HEADER_DG_GUILD_WAR_SCORE					= 42,
	HEADER_DG_GUILD_TIME_UPDATE					= 43,
	HEADER_DG_GUILD_LOAD						= 44,
	HEADER_DG_GUILD_LADDER						= 45,
	HEADER_DG_GUILD_SKILL_USABLE_CHANGE			= 46,
	HEADER_DG_GUILD_MONEY_CHANGE				= 47,
	HEADER_DG_GUILD_WITHDRAW_MONEY_GIVE			= 48,

	HEADER_DG_SET_EVENT_FLAG					= 49,

	HEADER_DG_GUILD_WAR_RESERVE_ADD				= 50,
	HEADER_DG_GUILD_WAR_RESERVE_DEL				= 51,
	HEADER_DG_GUILD_WAR_BET						= 52,

	HEADER_DG_RELOAD_PROTO						= 53,
	HEADER_DG_CHANGE_NAME						= 54,

	HEADER_DG_AUTH_LOGIN						= 55,

	HEADER_DG_CHANGE_EMPIRE_PRIV				= 56,
	HEADER_DG_CHANGE_GUILD_PRIV					= 57,

	HEADER_DG_MONEY_LOG							= 58,

	HEADER_DG_CHANGE_CHARACTER_PRIV				= 59,

	HEADER_DG_BILLING_REPAIR					= 60,
	HEADER_DG_BILLING_EXPIRE					= 61,
	HEADER_DG_BILLING_LOGIN						= 62,
	HEADER_DG_VCARD								= 63,
	HEADER_DG_BILLING_CHECK						= 64,

	HEADER_DG_CREATE_OBJECT						= 65,
	HEADER_DG_DELETE_OBJECT						= 66,
	HEADER_DG_UPDATE_LAND						= 67,

	HEADER_DG_MARRIAGE_ADD						= 68,
	HEADER_DG_MARRIAGE_UPDATE					= 69,
	HEADER_DG_MARRIAGE_REMOVE					= 70,

	HEADER_DG_WEDDING_REQUEST					= 71,
	HEADER_DG_WEDDING_READY						= 72,
	HEADER_DG_WEDDING_START						= 73,
	HEADER_DG_WEDDING_END						= 74,

	HEADER_DG_MYSHOP_PRICELIST_RES				= 75,
	HEADER_DG_RELOAD_ADMIN						= 76,
	HEADER_DG_BREAK_MARRIAGE					= 77,

	HEADER_DG_BLOCK_COUNTRY_IP					= 89,
	HEADER_DG_BLOCK_EXCEPTION					= 90,

	HEADER_DG_ACK_CHANGE_GUILD_MASTER			= 91,

	HEADER_DG_ACK_SPARE_ITEM_ID_RANGE			= 92,

	HEADER_DG_UPDATE_HORSE_NAME 				= 93,
	HEADER_DG_ACK_HORSE_NAME					= 94,

	HEADER_DG_NEED_LOGIN_LOG					= 95,

	HEADER_DG_RESULT_CHARGE_CASH				= 97,
	HEADER_DG_ITEMAWARD_INFORMER				= 98,	//gift notify
	HEADER_DG_RESPOND_CHANNELSTATUS				= 99,

#ifdef __ENABLE_CHANGE_CHANNEL__
	HEADER_DG_CHANNEL_SWITCH					= 100,
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
	HEADER_DG_OFFLINE_MESSAGE					= 101,
#endif

#ifdef __ADMIN_MANAGER__
	HEADER_DG_ADMIN_MANAGER_CHAT_SEARCH_PLAYER	= 102,
#endif

#ifdef __BATTLE_PASS_ENABLE__
	HEADER_DG_BATTLEPASS_RANKING				= 103,
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	HEADER_DG_RESPOND_OFFLINE_SHOP_ID,
#endif

#ifdef __SHOP_SEARCH__
	HEADER_DG_SHOP_SEARCH_RESULT,
	HEADER_DG_SHOP_SEARCH_BUY_RESULT,
	HEADER_DG_SHOP_SEARCH_BUY_FROM_SHOP,
	HEADER_DG_SHOP_SEARCH_SOLD_INFO,
#endif

#ifdef __ITEM_SHOP_ENABLE__
	HEADER_DG_ITEMSHOP,
#endif

	#ifdef __DUNGEON_INFO_ENABLE__
		HEADER_DG_DUNGEONINFO_RANKING,
	#endif

	#ifdef __HARDWARE_BAN__
		HEADER_DG_UPDATE_BAN,
	#endif

	HEADER_DG_MAP_LOCATIONS						= 0xfe,
	HEADER_DG_P2P								= 0xff,

	HEADER_GP_CONFIRM_PASSPOD = 1,
	HEADER_PG_CONFIRM_PASSPOD = 2,

};

enum E_PASSPOD
{
	E_PASSPOD_SUCCESS = 0,
	E_PASSPOD_FAILED_PASSPOD_ERROR,
	E_PASSPOD_FAILED_USER_NOT_FOUND,
	E_PASSPOD_FAILED_SYSTEM_NOT_FOUND,
	E_PASSPOD_FAILED_TOKEN_DISABLED,
	E_PASSPOD_FAILED_EMPTY,
};


typedef struct SRequestConfirmPasspod
{
	int pid;
	char passpod[MAX_PASSPOD + 1];
	char login[LOGIN_MAX_LEN + 1];

} RequestConfirmPasspod;

typedef struct SResultConfirmPasspod
{
	int pid;
	int ret_code;
	char login[LOGIN_MAX_LEN + 1];
} ResultConfirmPasspod;
/* ----------------------------------------------
 * table
 * ----------------------------------------------
 */

/* game Server -> DB Server */
#pragma pack(1)
enum ERequestChargeType
{
	ERequestCharge_Cash = 0,
	ERequestCharge_Mileage,
};

typedef struct SRequestChargeCash
{
	DWORD		dwAID;		// id(primary key) - Account Table
	DWORD		dwAmount;
	ERequestChargeType	eChargeType;

} TRequestChargeCash;

typedef struct SSimplePlayer
{
	DWORD		dwID;
	char		szName[CHARACTER_NAME_MAX_LEN + 1];
	BYTE		byJob;
	BYTE		byLevel;
	DWORD		dwPlayMinutes;
	BYTE		byST, byHT, byDX, byIQ;
	WORD		wMainPart;
	BYTE		bChangeName;
	WORD		wHairPart;
#ifdef __ENABLE_SASH_SYSTEM__
	DWORD		dwSashPart;
#endif
	BYTE		bDummy[4];
	long		x, y;
	long		lAddr;
	WORD		wPort;
	BYTE		skill_group;
#ifdef ENABLE_OFFICIAL_SELECT_GUI
	DWORD		last_play;
#endif
} TSimplePlayer;

typedef struct SAccountTable
{
	DWORD		id;
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	char		social_id[SOCIAL_ID_MAX_LEN + 1];
	char		status[ACCOUNT_STATUS_MAX_LEN + 1];
	BYTE		bEmpire;
#ifdef __ADMIN_MANAGER__
	DWORD		accban_count;
#endif
	TSimplePlayer	players[PLAYER_PER_ACCOUNT];
} TAccountTable;

typedef struct SPacketDGCreateSuccess
{
	BYTE		bAccountCharacterIndex;
	TSimplePlayer	player;
} TPacketDGCreateSuccess;

typedef struct TPlayerItemAttribute
{
	BYTE	bType;
	short	sValue;
} TPlayerItemAttribute;

typedef struct SPlayerItem
{
	DWORD	id;
	BYTE	window;
	WORD	pos;
	CountType	count;

	DWORD	vnum;
#ifdef __ADMIN_MANAGER__
	bool	is_gm_owner;
#endif
	long	alSockets[ITEM_SOCKET_MAX_NUM];

	TPlayerItemAttribute    aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	DWORD	owner;
#ifdef __TRANSMUTATION_SYSTEM__
	DWORD	transmutate_id;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD	dwRefineElement;
#endif
} TPlayerItem;

typedef struct SQuickslot
{
	BYTE	type;
	BYTE	pos;
} TQuickslot;

typedef struct SPlayerSkill
{
	BYTE	bMasterType;
	BYTE	bLevel;
	time_t	tNextRead;
} TPlayerSkill;

struct	THorseInfo
{
	BYTE	bLevel;
	BYTE	bRiding;
	short	sStamina;
	short	sHealth;
	DWORD	dwHorseHealthDropTime;
};

#ifdef __ENABLE_FIND_LETTERS_EVENT__
typedef struct SPlayerLetterSlot
{
	BYTE	bAsciiChar;
	BYTE	bIsFilled;
} TPlayerLetterSlot;

typedef struct SPlayerLetterReward
{
	DWORD	dwVnum;
	CountType	bCount;
} TPlayerLetterReward;
#endif

typedef struct SPlayerTable
{
	DWORD	id;

	char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	ip[IP_ADDRESS_LENGTH + 1];

	WORD	job;
	BYTE	voice;

	BYTE	level;
	BYTE	level_step;
	short	st, ht, dx, iq;

	DWORD	exp;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	gold;
#else
	INT		gold;
#endif

	BYTE	dir;
	INT		x, y, z;
	INT		lMapIndex;

	long	lExitX, lExitY;
	long	lExitMapIndex;

	// @fixme301
	int		hp;
	int		sp;

	short	sRandomHP;
	short	sRandomSP;

	int         playtime;

	short	stat_point;
	short	skill_point;
	short	sub_skill_point;
	short	horse_skill_point;

	TPlayerSkill skills[SKILL_MAX_NUM];

	TQuickslot  quickslot[QUICKSLOT_MAX_NUM];

	BYTE	part_base;
	DWORD	parts[PART_MAX_NUM];

	short	stamina;

	BYTE	skill_group;
	long	lAlignment;
	char	szMobile[MOBILE_MAX_LEN + 1];

	short	stat_reset_count;

	THorseInfo	horse;

	DWORD	logoff_interval;

#ifdef ENABLE_OFFICIAL_SELECT_GUI
	DWORD	last_play;
#endif

	int		aiPremiumTimes[PREMIUM_MAX_NUM];
#ifdef __GAYA_SHOP_SYSTEM__
	DWORD	gaya_coins;
#endif
#ifdef __ENABLE_HIDE_COSTUMES__
	DWORD	dwCostumeFlag;
#endif
#ifdef __ADMIN_MANAGER__
	DWORD	chat_ban_count;
#endif
#ifdef __ENABLE_BIOLOG_SYSTEM__
	BYTE	m_BiologActualMission;
	WORD	m_BiologCollectedItems;
	BYTE	m_BiologCooldownReminder;
	long	m_BiologCooldown;
#endif
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	DWORD	m_MarbleActiveMission;
	BYTE	m_MarbleActiveExtandedCount;
	WORD	m_MarbleKilledMonsters;
#endif
#ifdef __ENABLE_FIND_LETTERS_EVENT__
	TPlayerLetterSlot letterSlots[FIND_LETTERS_SLOTS_NUM];
#endif
} TPlayerTable;

typedef struct SMobSkillLevel
{
	DWORD	dwVnum;
	BYTE	bLevel;
} TMobSkillLevel;

typedef struct SEntityTable
{
	DWORD dwVnum;
} TEntityTable;

typedef struct SMobTable : public SEntityTable
{
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
	char	szLocaleName[CHARACTER_NAME_MAX_LEN + 1];

	BYTE	bType;			// Monster, NPC
	BYTE	bRank;			// PAWN, KNIGHT, KING
	BYTE	bBattleType;		// MELEE, etc..
	BYTE	bLevel;			// Level
	BYTE	bSize;

	DWORD	dwGoldMin;
	DWORD	dwGoldMax;
	DWORD	dwExp;
	DWORD	dwMaxHP;
	BYTE	bRegenCycle;
	BYTE	bRegenPercent;
	WORD	wDef;

	DWORD	dwAIFlag;
	DWORD	dwRaceFlag;
	DWORD	dwImmuneFlag;

	BYTE	bStr, bDex, bCon, bInt;
	DWORD	dwDamageRange[2];

	short	sAttackSpeed;
	short	sMovingSpeed;
	BYTE	bAggresiveHPPct;
	WORD	wAggressiveSight;
	WORD	wAttackRange;

	char	cEnchants[MOB_ENCHANTS_MAX_NUM];
	char	cResists[MOB_RESISTS_MAX_NUM];

	DWORD	dwResurrectionVnum;
	DWORD	dwDropItemVnum;

	BYTE	bMountCapacity;
	BYTE	bOnClickType;

	BYTE	bEmpire;
	char	szFolder[64 + 1];

	float	fDamMultiply;

	DWORD	dwSummonVnum;
	DWORD	dwDrainSP;
	DWORD	dwMobColor;
	DWORD	dwPolymorphItemVnum;

	TMobSkillLevel Skills[MOB_SKILL_MAX_NUM];

	BYTE	bBerserkPoint;
	BYTE	bStoneSkinPoint;
	BYTE	bGodSpeedPoint;
	BYTE	bDeathBlowPoint;
	BYTE	bRevivePoint;
} TMobTable;

typedef struct SSkillTable
{
	DWORD	dwVnum;
	char	szName[32 + 1];
	BYTE	bType;
	BYTE	bMaxLevel;
	DWORD	dwSplashRange;

	char	szPointOn[64];
	char	szPointPoly[100 + 1];
	char	szSPCostPoly[100 + 1];
	char	szDurationPoly[100 + 1];
	char	szDurationSPCostPoly[100 + 1];
	char	szCooldownPoly[100 + 1];
	char	szMasterBonusPoly[100 + 1];
	//char	szAttackGradePoly[100 + 1];
	char	szGrandMasterAddSPCostPoly[100 + 1];
	DWORD	dwFlag;
	DWORD	dwAffectFlag;

	// Data for secondary skill
	char 	szPointOn2[64];
	char 	szPointPoly2[100 + 1];
	char 	szDurationPoly2[100 + 1];
	DWORD 	dwAffectFlag2;

	// Data for grand master point
	char 	szPointOn3[64];
	char 	szPointPoly3[100 + 1];
	char 	szDurationPoly3[100 + 1];

	BYTE	bLevelStep;
	BYTE	bLevelLimit;
	DWORD	preSkillVnum;
	BYTE	preSkillLevel;

	long	lMaxHit;
	char	szSplashAroundDamageAdjustPoly[100 + 1];

	BYTE	bSkillAttrType;

	DWORD	dwTargetRange;
} TSkillTable;

typedef struct SShopItemTable
{
	DWORD		vnum;
	CountType	count;

	TItemPos	pos;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	price;
#else
	DWORD		price;
#endif
	BYTE		display_pos;
} TShopItemTable;

typedef struct SShopItemExTable : SShopItemTable
{
	DWORD	coin_vnum;
} TShopItemExTable;

typedef struct SShopTable
{
	DWORD		dwVnum;
	DWORD		dwNPCVnum;

	CountType	byItemCount;
	TShopItemTable	items[SHOP_HOST_ITEM_MAX_NUM];
} TShopTable;

#define QUEST_NAME_MAX_LEN	32
#define QUEST_STATE_MAX_LEN	64

typedef struct SQuestTable
{
	DWORD		dwPID;
	char		szName[QUEST_NAME_MAX_LEN + 1];
	char		szState[QUEST_STATE_MAX_LEN + 1];
	long		lValue;
} TQuestTable;

typedef struct SItemLimit
{
	BYTE	bType;
	long	lValue;
} TItemLimit;

typedef struct SItemApply
{
	BYTE	bType;
	long	lValue;
} TItemApply;

typedef struct SItemTable : public SEntityTable
{
	DWORD		dwVnumRange;
	char        szName[ITEM_NAME_MAX_LEN + 1];
	char		szLocaleName[ITEM_NAME_MAX_LEN + 1];
	BYTE		bType;
	BYTE		bSubType;

	BYTE        bWeight;
	BYTE		bSize;

	DWORD		dwAntiFlags;
	DWORD		dwFlags;
	DWORD		dwWearFlags;
	DWORD		dwImmuneFlag;

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t       dwGold;
	int64_t       dwShopBuyPrice;
#else
	DWORD		dwGold;
	DWORD		dwShopBuyPrice;
#endif

	TItemLimit	aLimits[ITEM_LIMIT_MAX_NUM];
	TItemApply	aApplies[ITEM_APPLY_MAX_NUM];
	long        alValues[ITEM_VALUES_MAX_NUM];
	long		alSockets[ITEM_SOCKET_MAX_NUM];
	DWORD		dwRefinedVnum;
	WORD		wRefineSet;
#ifdef __SPECIAL_STORAGE_ENABLE__
	DWORD		dwVirtualWindow;
#endif
	BYTE		bAlterToMagicItemPct;
	BYTE		bSpecular;
	BYTE		bGainSocketPct;

	short int	sAddonType;



	char		cLimitRealTimeFirstUseIndex;
	char		cLimitTimerBasedOnWearIndex;

	uint32_t	equipmentSetId;

} TItemTable;

struct TItemAttrTable
{
	TItemAttrTable() :
		dwApplyIndex(0),
		dwProb(0)
	{
		szApply[0] = 0;
		memset(&lValues, 0, sizeof(lValues));
		memset(&bMaxLevelBySet, 0, sizeof(bMaxLevelBySet));
	}

	char    szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD   dwApplyIndex;
	DWORD   dwProb;
	long    lValues[ITEM_ATTRIBUTE_MAX_LEVEL];
	BYTE    bMaxLevelBySet[ATTRIBUTE_SET_MAX_NUM];
};

#ifdef __BELT_ATTR__
struct TItemBeltAttrTable
{
	TItemBeltAttrTable() :
		dwApplyIndex(0),
		dwProb(0)
	{
		szApply[0] = 0;
		memset(&lValues, 0, sizeof(lValues));
		memset(&bMaxLevelBySet, 0, sizeof(bMaxLevelBySet));
	}

	char    szApply[APPLY_NAME_MAX_LEN + 1];
	DWORD   dwApplyIndex;
	DWORD   dwProb;
	long    lValues[3];
	BYTE    bMaxLevelBySet[ATTRIBUTE_SET_BELT_MAX_NUM];
};
#endif

typedef struct SConnectTable
{
	char	login[LOGIN_MAX_LEN + 1];
	IDENT	ident;
} TConnectTable;

typedef struct SLoginPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLoginPacket;

typedef struct SPlayerLoadPacket
{
	DWORD	account_id;
	DWORD	player_id;
	BYTE	account_index;
} TPlayerLoadPacket;

typedef struct SPlayerCreatePacket
{
	char		login[LOGIN_MAX_LEN + 1];
	char		passwd[PASSWD_MAX_LEN + 1];
	DWORD		account_id;
	BYTE		account_index;
	TPlayerTable	player_table;
} TPlayerCreatePacket;

typedef struct SPlayerDeletePacket
{
	char	login[LOGIN_MAX_LEN + 1];
	DWORD	player_id;
	BYTE	account_index;
	//char	name[CHARACTER_NAME_MAX_LEN + 1];
	char	private_code[8];
} TPlayerDeletePacket;

typedef struct SLogoutPacket
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
} TLogoutPacket;

typedef struct SPlayerCountPacket
{
	DWORD	dwCount;
} TPlayerCountPacket;

#define SAFEBOX_MAX_NUM			135
#define SAFEBOX_PASSWORD_MAX_LEN	6

typedef struct SSafeboxTable
{
	DWORD	dwID;
	BYTE	bSize;
	DWORD	dwGold;
	WORD	wItemCount;
} TSafeboxTable;

typedef struct SSafeboxChangeSizePacket
{
	DWORD	dwID;
	BYTE	bSize;
} TSafeboxChangeSizePacket;

typedef struct SSafeboxLoadPacket
{
	DWORD	dwID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxLoadPacket;

typedef struct SSafeboxChangePasswordPacket
{
	DWORD	dwID;
	char	szOldPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
	char	szNewPassword[SAFEBOX_PASSWORD_MAX_LEN + 1];
} TSafeboxChangePasswordPacket;

typedef struct SSafeboxChangePasswordPacketAnswer
{
	BYTE	flag;
} TSafeboxChangePasswordPacketAnswer;

typedef struct SEmpireSelectPacket
{
	DWORD	dwAccountID;
	BYTE	bEmpire;
} TEmpireSelectPacket;

typedef struct SPacketGDSetup
{
	char	szPublicIP[16];	// Public IP which listen to users
	BYTE	bChannel;
	WORD	wListenPort;
	WORD	wP2PPort;
	long	alMaps[MAP_ALLOW_LIMIT];
	DWORD	dwLoginCount;
	BYTE	bAuthServer;
} TPacketGDSetup;

typedef struct SPacketDGMapLocations
{
	BYTE	bCount;
} TPacketDGMapLocations;

typedef struct SMapLocation
{
	long	alMaps[MAP_ALLOW_LIMIT];
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
} TMapLocation;

typedef struct SPacketDGP2P
{
	char	szHost[MAX_HOST_LENGTH + 1];
	WORD	wPort;
#ifdef __ADMIN_MANAGER__
	WORD	wListenPort;
#endif
	BYTE	bChannel;
} TPacketDGP2P;

typedef struct SPacketGDDirectEnter
{
	char	login[LOGIN_MAX_LEN + 1];
	char	passwd[PASSWD_MAX_LEN + 1];
	BYTE	index;
} TPacketGDDirectEnter;

typedef struct SPacketDGDirectEnter
{
	TAccountTable accountTable;
	TPlayerTable playerTable;
} TPacketDGDirectEnter;

typedef struct SPacketGuildSkillUpdate
{
	DWORD guild_id;
	int amount;
	BYTE skill_levels[12];
	BYTE skill_point;
	BYTE save;
} TPacketGuildSkillUpdate;

typedef struct SPacketGuildExpUpdate
{
	DWORD guild_id;
	int amount;
} TPacketGuildExpUpdate;

typedef struct SPacketGuildChangeMemberData
{
	DWORD guild_id;
	DWORD pid;
	DWORD offer;
	BYTE level;
	BYTE grade;
} TPacketGuildChangeMemberData;


typedef struct SPacketDGLoginAlready
{
	char	szLogin[LOGIN_MAX_LEN + 1];
} TPacketDGLoginAlready;

typedef struct TPacketAffectElement
{
	DWORD	dwType;
	BYTE	bApplyOn;
	long	lApplyValue;
	DWORD	dwFlag;
	long	lDuration;
	long	lSPCost;
} TPacketAffectElement;

typedef struct SPacketGDAddAffect
{
	DWORD			dwPID;
	TPacketAffectElement	elem;
} TPacketGDAddAffect;

typedef struct SPacketGDRemoveAffect
{
	DWORD	dwPID;
	DWORD	dwType;
	BYTE	bApplyOn;
} TPacketGDRemoveAffect;

typedef struct SPacketGDHighscore
{
	DWORD	dwPID;
	long	lValue;
	char	cDir;
	char	szBoard[21];
} TPacketGDHighscore;

typedef struct SPacketPartyCreate
{
	DWORD	dwLeaderPID;
} TPacketPartyCreate;

typedef struct SPacketPartyDelete
{
	DWORD	dwLeaderPID;
} TPacketPartyDelete;

typedef struct SPacketPartyAdd
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bState;
} TPacketPartyAdd;

typedef struct SPacketPartyRemove
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
} TPacketPartyRemove;

typedef struct SPacketPartyStateChange
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bRole;
	BYTE	bFlag;
} TPacketPartyStateChange;

typedef struct SPacketPartySetMemberLevel
{
	DWORD	dwLeaderPID;
	DWORD	dwPID;
	BYTE	bLevel;
} TPacketPartySetMemberLevel;

typedef struct SPacketGDBoot
{
	DWORD	dwItemIDRange[2];
	char	szIP[16];
} TPacketGDBoot;

typedef struct SPacketGuild
{
	DWORD	dwGuild;
	DWORD	dwInfo;
} TPacketGuild;

typedef struct SPacketGDGuildAddMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
} TPacketGDGuildAddMember;

typedef struct SPacketDGGuildMember
{
	DWORD	dwPID;
	DWORD	dwGuild;
	BYTE	bGrade;
	BYTE	isGeneral;
	BYTE	bJob;
	BYTE	bLevel;
	DWORD	dwOffer;
	char	szName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGGuildMember;

typedef struct SPacketGuildWar
{
	BYTE	bType;
	BYTE	bWar;
	DWORD	dwGuildFrom;
	DWORD	dwGuildTo;
	long	lWarPrice;
	long	lInitialScore;
} TPacketGuildWar;



typedef struct SPacketGuildWarScore
{
	DWORD dwGuildGainPoint;
	DWORD dwGuildOpponent;
	long lScore;
	long lBetScore;
} TPacketGuildWarScore;

typedef struct SRefineMaterial
{
	DWORD vnum;
	int count;
} TRefineMaterial;

typedef struct SRefineTable
{
	DWORD id;
	BYTE material_count;
	int cost;
	int prob;
	TRefineMaterial materials[REFINE_MATERIAL_MAX_NUM];
} TRefineTable;

typedef struct SBanwordTable
{
	char szWord[BANWORD_MAX_LEN + 1];

	bool operator == (const char* word) const
	{
		return !strcmp(szWord, word);
	}
} TBanwordTable;

typedef struct SPacketGDChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketGDChangeName;

typedef struct SPacketDGChangeName
{
	DWORD pid;
	char name[CHARACTER_NAME_MAX_LEN + 1];
} TPacketDGChangeName;

typedef struct SPacketGuildLadder
{
	DWORD dwGuild;
	long lLadderPoint;
	long lWin;
	long lDraw;
	long lLoss;
} TPacketGuildLadder;

typedef struct SPacketGuildLadderPoint
{
	DWORD dwGuild;
	long lChange;
} TPacketGuildLadderPoint;

typedef struct SPacketGDSMS
{
	char szFrom[CHARACTER_NAME_MAX_LEN + 1];
	char szTo[CHARACTER_NAME_MAX_LEN + 1];
	char szMobile[MOBILE_MAX_LEN + 1];
	char szMsg[SMS_MAX_LEN + 1];
} TPacketGDSMS;

typedef struct SPacketGuildUseSkill
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	DWORD dwCooltime;
} TPacketGuildUseSkill;

typedef struct SPacketGuildSkillUsableChange
{
	DWORD dwGuild;
	DWORD dwSkillVnum;
	BYTE bUsable;
} TPacketGuildSkillUsableChange;

typedef struct SPacketGDLoginKey
{
	DWORD dwAccountID;
	DWORD dwLoginKey;
} TPacketGDLoginKey;

typedef struct SPacketGDAuthLogin
{
	DWORD	dwID;
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szSocialID[SOCIAL_ID_MAX_LEN + 1];
	DWORD	adwClientKey[4];
	BYTE	bBillType;
	DWORD	dwBillID;
	int		iPremiumTimes[PREMIUM_MAX_NUM];
#ifdef __ADMIN_MANAGER__
	DWORD	accban_count;
#endif
} TPacketGDAuthLogin;

typedef struct SPacketGDLoginByKey
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwLoginKey;
	DWORD	adwClientKey[4];
	char	szIP[MAX_HOST_LENGTH + 1];
} TPacketGDLoginByKey;


typedef struct SPacketGiveGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	time_t duration_sec;
} TPacketGiveGuildPriv;
typedef struct SPacketGiveEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	time_t duration_sec;
} TPacketGiveEmpirePriv;
typedef struct SPacketGiveCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
} TPacketGiveCharacterPriv;
typedef struct SPacketRemoveGuildPriv
{
	BYTE type;
	DWORD guild_id;
} TPacketRemoveGuildPriv;
typedef struct SPacketRemoveEmpirePriv
{
	BYTE type;
	BYTE empire;
} TPacketRemoveEmpirePriv;

typedef struct SPacketDGChangeCharacterPriv
{
	BYTE type;
	int value;
	DWORD pid;
	BYTE bLog;
} TPacketDGChangeCharacterPriv;

typedef struct SPacketDGChangeGuildPriv
{
	BYTE type;
	int value;
	DWORD guild_id;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeGuildPriv;

typedef struct SPacketDGChangeEmpirePriv
{
	BYTE type;
	int value;
	BYTE empire;
	BYTE bLog;
	time_t end_time_sec;
} TPacketDGChangeEmpirePriv;

typedef struct SPacketMoneyLog
{
	BYTE type;
	DWORD vnum;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t gold;
#else
	INT gold;
#endif
} TPacketMoneyLog;

typedef struct SPacketGDGuildMoney
{
	DWORD dwGuild;
	INT iGold;
} TPacketGDGuildMoney;

typedef struct SPacketDGGuildMoneyChange
{
	DWORD dwGuild;
	INT iTotalGold;
} TPacketDGGuildMoneyChange;

typedef struct SPacketDGGuildMoneyWithdraw
{
	DWORD dwGuild;
	INT iChangeGold;
} TPacketDGGuildMoneyWithdraw;

typedef struct SPacketGDGuildMoneyWithdrawGiveReply
{
	DWORD dwGuild;
	INT iChangeGold;
	BYTE bGiveSuccess;
} TPacketGDGuildMoneyWithdrawGiveReply;

typedef struct SPacketSetEventFlag
{
	char	szFlagName[EVENT_FLAG_NAME_MAX_LEN + 1];
	long	lValue;
} TPacketSetEventFlag;

typedef struct SPacketBillingLogin
{
	DWORD	dwLoginKey;
	BYTE	bLogin;
} TPacketBillingLogin;

typedef struct SPacketBillingRepair
{
	DWORD	dwLoginKey;
	char	szLogin[LOGIN_MAX_LEN + 1];
	char	szHost[MAX_HOST_LENGTH + 1];
} TPacketBillingRepair;

typedef struct SPacketBillingExpire
{
	char	szLogin[LOGIN_MAX_LEN + 1];
	BYTE	bBillType;
	DWORD	dwRemainSeconds;
} TPacketBillingExpire;

typedef struct SPacketLoginOnSetup
{
	DWORD   dwID;
	char    szLogin[LOGIN_MAX_LEN + 1];
	char    szSocialID[SOCIAL_ID_MAX_LEN + 1];
	char    szHost[MAX_HOST_LENGTH + 1];
	DWORD   dwLoginKey;
	DWORD   adwClientKey[4];
} TPacketLoginOnSetup;

typedef struct SPacketGDCreateObject
{
	DWORD	dwVnum;
	DWORD	dwLandID;
	INT		lMapIndex;
	INT	 	x, y;
	float	xRot;
	float	yRot;
	float	zRot;
} TPacketGDCreateObject;

typedef struct SPacketGDHammerOfTor
{
	DWORD 	key;
	DWORD	delay;
} TPacketGDHammerOfTor;

typedef struct SPacketGDVCard
{
	DWORD	dwID;
	char	szSellCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szSellAccount[LOGIN_MAX_LEN + 1];
	char	szBuyCharacter[CHARACTER_NAME_MAX_LEN + 1];
	char	szBuyAccount[LOGIN_MAX_LEN + 1];
} TPacketGDVCard;

typedef struct SGuildReserve
{
	DWORD       dwID;
	DWORD       dwGuildFrom;
	DWORD       dwGuildTo;
	DWORD       dwTime;
	BYTE        bType;
	long        lWarPrice;
	long        lInitialScore;
	bool        bStarted;
	DWORD		dwBetFrom;
	DWORD		dwBetTo;
	long		lPowerFrom;
	long		lPowerTo;
	long		lHandicap;
} TGuildWarReserve;

typedef struct
{
	DWORD	dwWarID;
	char	szLogin[LOGIN_MAX_LEN + 1];
	DWORD	dwGold;
	DWORD	dwGuild;
} TPacketGDGuildWarBet;

// Marriage

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	time_t tMarryTime;
	char szName1[CHARACTER_NAME_MAX_LEN + 1];
	char szName2[CHARACTER_NAME_MAX_LEN + 1];
} TPacketMarriageAdd;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	INT  iLovePoint;
	BYTE  byMarried;
} TPacketMarriageUpdate;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketMarriageRemove;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingRequest;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
	DWORD dwMapIndex;
} TPacketWeddingReady;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingStart;

typedef struct
{
	DWORD dwPID1;
	DWORD dwPID2;
} TPacketWeddingEnd;

typedef struct SPacketMyshopPricelistHeader
{
	DWORD	dwOwnerID;
	BYTE	byCount;
} TPacketMyshopPricelistHeader;


typedef struct SItemPriceInfo
{
	DWORD	dwVnum;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	dwPrice;
#else
	DWORD	dwPrice;
#endif
} TItemPriceInfo;


typedef struct SItemPriceListTable
{
	DWORD	dwOwnerID;
	BYTE	byCount;

	TItemPriceInfo	aPriceInfo[SHOP_PRICELIST_MAX_NUM];
} TItemPriceListTable;


#ifdef __ADMIN_MANAGER__
typedef struct SPacketBlockChat
{
	DWORD dwPID;
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lDuration;
	bool bIncreaseBanCounter;
} TPacketBlockChat;

typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	int m_Authority;
} tAdminInfo;

typedef struct SPacketGGUpdateRights
{
	unsigned char header;
	char name[CHARACTER_NAME_MAX_LEN + 1];
	unsigned char gm_level;
} TPacketGGUpdateRights;
#else
typedef struct SPacketBlockChat
{
	char szName[CHARACTER_NAME_MAX_LEN + 1];
	long lDuration;
} TPacketBlockChat;

//ADMIN_MANAGER
typedef struct TAdminInfo
{
	int m_ID;
	char m_szAccount[32];
	char m_szName[32];
	char m_szContactIP[16];
	char m_szServerIP[16];
	int m_Authority;
} tAdminInfo;
//END_ADMIN_MANAGER
#endif

// PCBANG_IP_LIST
typedef struct SPacketPCBangIP
{
	DWORD id;
	DWORD ip;
} TPacketPCBangIP;
// END_OF_PCBANG_IP_LIST

//BOOT_LOCALIZATION
struct tLocale
{
	char szValue[32];
	char szKey[32];
};
//BOOT_LOCALIZATION

#ifndef __ADMIN_MANAGER__
typedef struct SPacketReloadAdmin
{
	char szIP[16];
} TPacketReloadAdmin;
#endif

// Block Country Ip
typedef struct tBlockCountryIp
{
	DWORD	ip_from;
	DWORD	ip_to;
} TPacketBlockCountryIp;

enum EBlockExceptionCommand
{
	BLOCK_EXCEPTION_CMD_ADD = 1,
	BLOCK_EXCEPTION_CMD_DEL = 2,
};

// Block Exception Account
typedef struct tBlockException
{
	BYTE	cmd;	// 1 == add, 2 == delete
	char	login[LOGIN_MAX_LEN + 1];
} TPacketBlockException;

typedef struct tChangeGuildMaster
{
	DWORD dwGuildID;
	DWORD idFrom;
	DWORD idTo;
} TPacketChangeGuildMaster;

typedef struct tItemIDRange
{
	DWORD dwMin;
	DWORD dwMax;
	DWORD dwUsableItemIDMin;
} TItemIDRangeTable;

typedef struct tUpdateHorseName
{
	DWORD dwPlayerID;
	char szHorseName[CHARACTER_NAME_MAX_LEN + 1];
} TPacketUpdateHorseName;

typedef struct tDC
{
	char	login[LOGIN_MAX_LEN + 1];
} TPacketDC;

typedef struct tNeedLoginLogInfo
{
	DWORD dwPlayerID;
} TPacketNeedLoginLogInfo;


typedef struct tItemAwardInformer
{
	char	login[LOGIN_MAX_LEN + 1];
	char	command[20];
	unsigned int vnum;
} TPacketItemAwardInfromer;

typedef struct tDeleteAwardID
{
	DWORD dwID;
} TPacketDeleteAwardID;

typedef struct SChannelStatus
{
	short nPort;
	BYTE bStatus;
} TChannelStatus;

#ifdef __ENABLE_SWITCHBOT__
struct TSwitchbotAttributeAlternativeTable
{
	TPlayerItemAttribute attributes[MAX_NORM_ATTR_NUM];

	bool IsConfigured() const
	{
#if __cplusplus < 199711L
		for (int i = 0; i < MAX_NORM_ATTR_NUM; ++i)
		{
			if (attributes[i].bType && attributes[i].sValue)
			{
				return true;
			}
		}
#else
		for (const auto& it : attributes)
		{
			if (it.bType && it.sValue)
			{
				return true;
			}
		}
#endif

		return false;
	}
};

struct TSwitchbotTable
{
	DWORD player_id;
	bool active[SWITCHBOT_SLOT_COUNT];
	bool finished[SWITCHBOT_SLOT_COUNT];
	DWORD items[SWITCHBOT_SLOT_COUNT];
	TSwitchbotAttributeAlternativeTable alternatives[SWITCHBOT_SLOT_COUNT][SWITCHBOT_ALTERNATIVE_COUNT];

	TSwitchbotTable() : player_id(0)
	{
		memset(&items, 0, sizeof(items));
		memset(&alternatives, 0, sizeof(alternatives));
		memset(&active, false, sizeof(active));
		memset(&finished, false, sizeof(finished));
	}
};

struct TSwitchbottAttributeTable
{
	BYTE attribute_set;
	int apply_num;
	long max_value;
};
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
typedef struct SChannelSwitch
{
	BYTE	byChannel;
	long	lMapIndex;
} TChannelSwitch;

typedef struct SChannelSwitchInfo
{
	char	szIP[16];
	WORD	wPort;
} TChannelSwitchInfo;
#endif

#ifdef __OFFLINE_MESSAGE_SYSTEM__
enum EOfflineMessage
{
	SUBHEADER_OFFLINE_MESSAGE_BOOT,
	SUBHEADER_OFFLINE_MESSAGE_DELETE_NAME,
	SUBHEADER_OFFLINE_MESSAGE_CHANGE_NAME,
	SUBHEADER_OFFLINE_MESSAGE_ADD_RECORD,
	SUBHEADER_OFFLINE_MESSAGE_REMOVE_RECORD,

	// CACHE
	OFFLINE_MESSAGE_COLLECTOR = 60,
	OFFLINE_MESSAGE_EXPIRATION_TIME = 60
};

typedef struct SPacketGDOfflineMessage
{
	BYTE header;
	BYTE subheader;
	char sender[CHARACTER_NAME_MAX_LEN + 1];
	char target[CHARACTER_NAME_MAX_LEN + 1];
	char text[CHAT_MAX_LEN - 1];
	BYTE sender_level;
	char sent_date[64];
	time_t expiration_time;
} TPacketGDOfflineMessage;
#endif

#ifdef __ADMIN_MANAGER__
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

typedef struct SAdminManagerBanClientAccountInfo
{
	DWORD dwAID;
	char szLoginName[LOGIN_MAX_LEN + 1];
	DWORD dwPID[PLAYER_PER_ACCOUNT];
	char szName[PLAYER_PER_ACCOUNT][CHARACTER_NAME_MAX_LEN + 1];
	BYTE bRace[PLAYER_PER_ACCOUNT];
	BYTE bLevel[PLAYER_PER_ACCOUNT];
	WORD wChatbanCount[PLAYER_PER_ACCOUNT];
	WORD wAccountbanCount;
	DWORD dwDuration;
	bool bIsOnline[PLAYER_PER_ACCOUNT];
} TAdminManagerBanClientAccountInfo;

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
#endif

#ifdef __BATTLE_PASS_ENABLE__
enum EBattlePass
{
	SUBHEADER_BATTLEPASS_GD_ADD_ITEM = 0,
	SUBHEADER_BATTLEPASS_GD_REQUEST_ITEMS = 1,

	SUBHEADER_BATTLEPASS_DG_ADD_ELEMENT = 2,
	SUBHEADER_BATTLEPASS_DG_WIPE_RANKING = 3,
};

typedef struct SBattlePassHighScore
{
	BYTE bSubHeader;
	size_t stPos;
	int iDiffLevel;
	int iMonth;
	char sName[CHARACTER_NAME_MAX_LEN + 1];
	time_t ttTime;
} TBattlePassHighScore;
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
typedef struct SCubeItem
{
	DWORD dwVnum;
	int iCount;

	inline SCubeItem& operator = (const SCubeItem& rhs)
	{
		dwVnum = rhs.dwVnum;
		iCount = rhs.iCount;
		return *this;
	}
} TCubeItem;

typedef struct SCubeDataBase
{
	int iIndex;
	TCubeItem tReward;
	int iPercent;
	unsigned long long dwGold;
	BYTE bCategory;
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
	bool bCopyBonus;
#endif

	TCubeItem tIncItem;
	int iIncPercent;

	SCubeDataBase()
	{
		this->iIndex = 0;
		this->iPercent = 0;
		this->dwGold = 0;
		this->bCategory = 0;
		this->tReward.dwVnum = 0;
		this->tReward.iCount = 0;

		this->tIncItem.dwVnum = 0;
		this->tIncItem.iCount = 0;
		this->iIncPercent = 0;
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
		this->bCopyBonus = false;
#endif
	}
} TCubeDataBase;

typedef struct SCubeData : public SCubeDataBase
{
	std::vector<TCubeItem> materialV;
} TCubeData;

typedef struct SCubeDataP : public SCubeDataBase
{
	TCubeItem materialV[CUBE_MATERIAL_MAX_NUM];
} TCubeDataP;
#endif

#ifdef __ENABLE_LUCKY_BOX__
typedef struct SLuckyBoxItem
{
	DWORD	dwVnum;
	CountType	bCount;
	bool	bIsReward;
} TLuckyBoxItem;
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
typedef struct SOfflineShop
{
	uint32_t id;

	uint32_t ownerPid;
	std::array < char, CHARACTER_NAME_MAX_LEN + 1 > ownerName;

	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;
	int32_t shopNameChangeTime;
	uint8_t channel;
	uint32_t mapIndex, x, y;
	uint32_t decoRace;
	uint8_t decoBoard;

	uint32_t openingTime;
	int64_t gold;
} TOfflineShop;

// The id is composed of the shop id and item position
typedef std::pair<uint32_t, uint8_t> TOfflineItemID;
namespace std
{
template <> struct hash <TOfflineItemID>
{
	inline size_t operator()(const TOfflineItemID& v) const
	{
		std::hash<int> int_hasher;
		return int_hasher(v.first) ^ int_hasher(v.second);
	}
};
}

typedef struct SOfflineShopItem
{
	TOfflineItemID id;

	uint32_t vnum;
	uint32_t count;
#ifdef __TRANSMUTATION_SYSTEM__
	uint32_t transmutation;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	uint32_t refineElement;
#endif
	std::array<int32_t, ITEM_SOCKET_MAX_NUM> sockets;
	std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM> attributes;
	int64_t price;
} TOfflineShopItem;
#endif

#ifdef __SHOP_SEARCH__
typedef std::pair<DWORD, DWORD> TShopSearchItemType;

enum EShopSearchData
{
	SHOPSEARCH_SOLD_ITEM_INFO_COUNT = 30, // keep sold item information for 30 days (update clientside as well if changed!)
};

enum EShopSearchBuyResult
{
	SHOPSEARCH_BUY_SUCCESS,
	SHOPSEARCH_BUY_NOT_EXIST,
	SHOPSEARCH_BUY_PRICE_CHANGE,
	SHOPSEARCH_BUY_TIMEOUT,
	SHOPSEARCH_BUY_NO_PEER,
	SHOPSEARCH_BUY_UNKNOWN_ERROR,
};

enum EShopSearchSortTypes
{
	SHOPSEARCH_SORT_RANDOM,
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

typedef struct SShopSearchItem : TPlayerItem
{
	TOfflineItemID offlineID;

	int64_t price;
	DWORD endTime;
} TShopSearchItem;

typedef struct SShopSearchClientItem : SShopSearchItem
{
	BYTE	avgPriceLevel;
} TShopSearchClientItem;

typedef struct SShopSearchOptions
{
	BYTE		typeFlagCount;
	BYTE		specificVnumCount;
} TShopSearchOptions;

typedef struct SPacketGDShopSearchByName
{
	BYTE	langID;
	char	itemName[ITEM_NAME_MAX_LEN + 1];
	WORD	page;
	BYTE	entryCountIdx;
	BYTE	sortType;
} TPacketGDShopSearchByName;

typedef struct SPacketGDShopSearchByOptions
{
	TShopSearchOptions	options;
	WORD				page;
	BYTE				entryCountIdx;
	BYTE				sortType;
} TPacketGDShopSearchByOptions;

typedef struct SPacketGDShopSearchRequestBuy
{
	DWORD	ownerID;
	TOfflineItemID	offlineID;
	DWORD	itemVnum;
	int64_t	itemPrice;
} TPacketGDShopSearchRequestBuy;

typedef struct SPacketDGShopSearchBuyFromShop
{
	DWORD	buyerDBHandle;
	DWORD	buyerPCHandle;
	DWORD	buyerPID;
	DWORD	ownerPID;
	TOfflineItemID	offlineID;
	int64_t	itemPrice;
} TPacketDGShopSearchBuyFromShop;

typedef struct SPacketGDShopSearchBoughtFromShop
{
	DWORD	buyerDBHandle;
	TPlayerItem	item;
} TPacketGDShopSearchBoughtFromShop;

typedef struct SPacketGDShopSearchSoldItem
{
	DWORD	itemVnum;
	DWORD	itemCount;
	uint64_t	price;
} TPacketGDShopSearchSoldItem;

typedef struct SShopSearchSoldItemInfo
{
	SShopSearchSoldItemInfo() : count(0), averagePrice(0) { }

	uint64_t	count;
	double		averagePrice;
} TShopSearchSoldItemInfo;
#endif

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
typedef struct SPacketGGCreateDungeonInstance
{
	BYTE bHeader;
	long lX;
	long lY;
	long lMapIndex;
	BYTE bChannel;
	long lAddr;
	WORD wPort;
	bool bRequest;
	BYTE bCount;
	DWORD aPids[32]; // Better move PARTY_MAX_NUM to tables.h
} TPacketGGCreateDungeonInstance;
#endif

#ifdef __ITEM_SHOP_ENABLE__
enum class EItemShopDiscountType : int
{
	CATEGORY_DISCOUNT,
	ITEM_DISCOUNT,
};

enum class EItemShopSpecialOfferType : int
{
	OFFER_LIMITED_TIME,
	OFFER_LIMITED_QUANTITY,
};

enum class EItemShopPacketType : int
{
	GD_ADD_ITEM,
	GD_REMOVE_ITEM,
	GD_ADD_CATEGORY,
	GD_REMOVE_CATEGORY,
	GD_EDIT_ITEM,
	GD_ADD_DISCOUNT,
	GD_REMOVE_DISCOUNT,
	GD_ADD_SPECIAL_OFFER,
	GD_REMOVE_SPECIAL_OFFER,
	GD_REQUEST_LIMITED_ITEM,
	GD_CONFIRM_LIMITED_ITEM_PURCHASE,

	DG_ADD_ITEM,
	DG_REMOVE_ITEM,
	DG_ADD_CATEGORY,
	DG_REMOVE_CATEGORY,
	DG_EDIT_ITEM,
	DG_ADD_DISCOUNT,
	DG_REMOVE_DISCOUNT,
	DG_ADD_SPECIAL_OFFER,
	DG_REMOVE_SPECIAL_OFFER,
	DG_REQUEST_LIMITED_ITEM,
	DG_CONFIRM_LIMITED_ITEM_PURCHASE,
};

typedef struct SItemShopDiscount
{
	EItemShopDiscountType eType;
	char szAdditionalArg[64];
	BYTE bValue;
	time_t ttDuration;

	std::string GetSelectQuery() const
	{
		return (std::string("discountType = ") + std::to_string(static_cast<int>(eType)) + ", additionalArg = " + szAdditionalArg + ", discountValue = " + std::to_string(bValue) + ", discountDuration = " + std::to_string(ttDuration));
	}

	std::string GetInsertQuery() const
	{
		return (std::to_string(static_cast<int>(eType)) + ", '" + szAdditionalArg + "', " + std::to_string(bValue) + ", " + std::to_string(ttDuration));
	}
} TItemShopDiscount;

typedef struct SItemShopSpecialOffer
{
	EItemShopSpecialOfferType eType;
	size_t stValue;
} TItemShopSpecialOffer;

typedef struct SItemShopItem
{
	char sHash[128];
	DWORD dwVnum;
	WORD wCount;
	int iPrice;
	DWORD dwBuyerID;

	long alSockets[ITEM_SOCKET_MAX_NUM];
	TItemShopDiscount tDiscount;
	TItemShopSpecialOffer tSpecialOffer;

	SItemShopItem() = default;
	SItemShopItem(DWORD dwVnum_, WORD wCount_, int iPrice_, std::array<long, ITEM_SOCKET_MAX_NUM> & aSockets) : dwVnum{dwVnum_}, wCount{wCount_}, iPrice{iPrice_}, dwBuyerID{0}
	{
		std::memcpy(&alSockets, aSockets.data(), sizeof(alSockets));
		Rehash();
	}

	void Rehash()
	{
		std::string sReturn(std::to_string(dwVnum) + std::string("|") + std::to_string(wCount) + std::string("|") + std::to_string(iPrice) + std::string("|"));
		for (const auto & rSocket : alSockets)
		{
			sReturn += std::to_string(rSocket) + std::string("/");
		}

		sReturn.pop_back();
		sReturn += "|" + std::to_string(tDiscount.bValue) + std::string("/") + std::to_string(tDiscount.ttDuration) + std::string("|") + std::to_string(static_cast<int>(tSpecialOffer.eType)) + std::string("/") + std::to_string(tSpecialOffer.stValue);
		strlcpy(sHash, sReturn.c_str(), sizeof(sHash));
	}

	std::string GetString() const
	{
		return sHash;
	}

	bool operator==(const SItemShopItem & r)
	{
		return GetString() == r.GetString();
	}

	bool operator==(const std::string & r)
	{
		return GetString() == r;
	}

	std::string GetSelectQuery() const
	{
		std::string sQuery = std::string("dwVnum = ") + std::to_string(dwVnum) + std::string(" AND wCount = ") + std::to_string(wCount) + std::string(" AND iPrice = ") + std::to_string(iPrice);
		for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			sQuery += " AND socket" + std::to_string(i) + " = " + std::to_string(alSockets[i]);
		}

		return sQuery;
	}

	std::string GetReplaceQuery() const
	{
		std::string sQuery = std::string("dwVnum = ") + std::to_string(dwVnum) + std::string(", wCount = ") + std::to_string(wCount) + std::string(", iPrice = ") + std::to_string(iPrice);
		for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			sQuery += std::string(", socket") + std::to_string(i) + " = " + std::to_string(alSockets[i]);
		}

		return sQuery;
	}

	std::string GetInsertQuery() const
	{
		std::string sQuery = std::to_string(dwVnum) + std::string(", ") + std::to_string(wCount) + std::string(", ") + std::to_string(iPrice);
		for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			sQuery += std::string(", ") + std::to_string(alSockets[i]);
		}

		return sQuery;
	}

	std::string GetInsertSpecialQuery() const
	{
		std::string sQuery = std::to_string(static_cast<int>(tSpecialOffer.eType)) + ", " + std::to_string(tSpecialOffer.stValue) + ", " + std::to_string(dwVnum) + std::string(", ") + std::to_string(wCount) + std::string(", ") + std::to_string(iPrice);
		for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			sQuery += std::string(", ") + std::to_string(alSockets[i]);
		}

		return sQuery;
	}

	std::string GetSelectSpecialQuery() const
	{
		std::string sQuery = std::string("eType = ") + std::to_string(static_cast<int>(tSpecialOffer.eType)) + ", stValue = " + std::to_string(tSpecialOffer.stValue) + std::string(", dwVnum = ") + std::to_string(dwVnum) + std::string(" AND wCount = ") + std::to_string(wCount) + std::string(" AND iPrice = ") + std::to_string(iPrice);
		for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			sQuery += " AND socket" + std::to_string(i) + " = " + std::to_string(alSockets[i]);
		}

		return sQuery;
	}
} TItemShopItem;

typedef struct SItemShopCategory
{
	BYTE bID;
	char sName[128];
	TItemShopDiscount tDiscount;

	SItemShopCategory() = default;

	SItemShopCategory(BYTE bID_, const std::string & sName_) : bID{bID_}
	{
		strlcpy(sName, sName_.c_str(), sizeof(sName));
	}

	struct Hasher
	{
		size_t operator()(const SItemShopCategory & rCategory) const
		{
			return rCategory.bID;
		}

		bool operator()(const SItemShopCategory & l, const SItemShopCategory & r) const
		{
			return l.bID < r.bID;
		}
	};

	std::string GetString() const
	{
		return (std::to_string(bID) + std::string("|") + sName + std::string("|") + std::to_string(tDiscount.bValue) + std::string("/") + std::to_string(tDiscount.ttDuration));
	}

	bool operator==(const SItemShopCategory & r)
	{
		return GetString() == r.GetString();
	}

	bool operator==(const BYTE & bID_) const
	{
		return bID == bID_;
	}

	std::string GetSelectQuery() const
	{
		return std::string("categoryID = ") + std::to_string(bID) + std::string(" AND categoryName = '") + sName + std::string("'");
	}

	std::string GetInsertQuery() const
	{
		return std::to_string(bID) + std::string(", '") + sName + std::string("'");
	}
} TItemShopCategory;

typedef struct SItemShopUniversalPacket
{
	BYTE bHeader;
	EItemShopPacketType ePacketType;

	TItemShopItem tItem;
	TItemShopCategory tCategory;
	TItemShopDiscount tDiscount;
} TItemShopUniversalPacket;
#endif

#ifdef __DUNGEON_INFO_ENABLE__
enum class EDIHighScoreTypes : int
{
	RUN_COUNT,
	QUICKER_RUN,
	GREATEST_DAMAGE,
	MAX_NUM,
};

enum class EDIEnums : int
{
	DUNGEON_INFO_RANKING_MAX = 100,
	DUNGEON_INFO_RANKIGN_CACHE = 60*30,
};

typedef struct SPacketGDDIRanking
{
	WORD wPos;
	int iType;
	char sKey[CHARACTER_NAME_MAX_LEN+1];
	char sName[CHARACTER_NAME_MAX_LEN+1];
	WORD wLevel;
	long long lValue;
} TPacketGDDIRanking;

typedef struct SPacketGDDIRankingBoot
{
	char sKey[CHARACTER_NAME_MAX_LEN+1];
} TPacketGDDIRankingBoot;
#endif

#ifdef __HARDWARE_BAN__
typedef struct SPacketGDDisconnectPlayer
{
	char	ban_hardware[40 + 1];
	bool	type;
} TPacketGDDisconnectPlayer;
#endif

#pragma pack()
#endif
