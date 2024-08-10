#ifndef __INC_METIN_II_CHAR_H__
#define __INC_METIN_II_CHAR_H__

#include <boost/unordered_map.hpp>
#include "../../common/stl.h"
#include "entity.h"
#include "FSM.h"
#include "horse_rider.h"
#include "vid.h"
#include "constants.h"
#include "affect.h"
#include "affect_flag.h"
#ifdef __ENABLE_CUBE_RENEWAL__
	#include "cube_renewal.h"
#endif
#include "mining.h"
#include "../../common/CommonDefines.h"

#ifdef __ABUSE_CONTROLLER_ENABLE__
	#include "AbuseController.h"
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	#include "BiologSystemManager.hpp"
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	#include "Transmutation_extra.h"
#endif

#ifdef __PET_SYSTEM__
	#include "PetSystem.h"
#endif

#ifdef __ACTION_RESTRICTIONS__
	#include "ActionRestricts.h"
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	#include "TombolaManager.hpp"
#endif

#define ENABLE_ANTI_CMD_FLOOD
#define ENABLE_OPEN_SHOP_WITH_ARMOR
enum eMountType {MOUNT_TYPE_NONE = 0, MOUNT_TYPE_NORMAL = 1, MOUNT_TYPE_COMBAT = 2, MOUNT_TYPE_MILITARY = 3};
eMountType GetMountLevelByVnum(DWORD dwMountVnum, bool IsNew);
const DWORD GetRandomSkillVnum(BYTE bJob = JOB_MAX_NUM);


class CBuffOnAttributes;
class CPetSystem;
class CPetActor;

#ifdef __ENABLE_OFFLINE_SHOP__
	class COfflineShop;
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	class CShamanSystem;
	class CShamanActor;
#endif

#define INSTANT_FLAG_DEATH_PENALTY		(1 << 0)
#define INSTANT_FLAG_SHOP			(1 << 1)
#define INSTANT_FLAG_EXCHANGE			(1 << 2)
#define INSTANT_FLAG_STUN			(1 << 3)
#define INSTANT_FLAG_NO_REWARD			(1 << 4)

#define AI_FLAG_NPC				(1 << 0)
#define AI_FLAG_AGGRESSIVE			(1 << 1)
#define AI_FLAG_HELPER				(1 << 2)
#define AI_FLAG_STAYZONE			(1 << 3)


#define SET_OVER_TIME(ch, time)	(ch)->SetOverTime(time)

extern int g_nPortalLimitTime;

enum
{
	MAIN_RACE_WARRIOR_M,
	MAIN_RACE_ASSASSIN_W,
	MAIN_RACE_SURA_M,
	MAIN_RACE_SHAMAN_W,
	MAIN_RACE_WARRIOR_W,
	MAIN_RACE_ASSASSIN_M,
	MAIN_RACE_SURA_W,
	MAIN_RACE_SHAMAN_M,
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	MAIN_RACE_WOLFMAN_M,
#endif
	MAIN_RACE_MAX_NUM,
};

enum
{
	POISON_LENGTH = 30,
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	BLEEDING_LENGTH = 30,
#endif
	STAMINA_PER_STEP = 1,
	SAFEBOX_PAGE_SIZE = 9,
	AI_CHANGE_ATTACK_POISITION_TIME_NEAR = 10000,
	AI_CHANGE_ATTACK_POISITION_TIME_FAR = 1000,
	AI_CHANGE_ATTACK_POISITION_DISTANCE = 100,
	SUMMON_MONSTER_COUNT = 3,
};

enum
{
	FLY_NONE,
	FLY_EXP,
	FLY_HP_MEDIUM,
	FLY_HP_BIG,
	FLY_SP_SMALL,
	FLY_SP_MEDIUM,
	FLY_SP_BIG,
	FLY_FIREWORK1,
	FLY_FIREWORK2,
	FLY_FIREWORK3,
	FLY_FIREWORK4,
	FLY_FIREWORK5,
	FLY_FIREWORK6,
	FLY_FIREWORK_CHRISTMAS,
	FLY_CHAIN_LIGHTNING,
	FLY_HP_SMALL,
	FLY_SKILL_MUYEONG,
#ifdef __ENABLE_QUIVER_SYSTEM__
	FLY_QUIVER_ATTACK_NORMAL,
#endif
};

enum EDamageType
{
	DAMAGE_TYPE_NONE,
	DAMAGE_TYPE_NORMAL,
	DAMAGE_TYPE_NORMAL_RANGE,
	DAMAGE_TYPE_MELEE,
	DAMAGE_TYPE_RANGE,
	DAMAGE_TYPE_FIRE,
	DAMAGE_TYPE_ICE,
	DAMAGE_TYPE_ELEC,
	DAMAGE_TYPE_MAGIC,
	DAMAGE_TYPE_POISON,
	DAMAGE_TYPE_SPECIAL,
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	DAMAGE_TYPE_BLEEDING,
#endif
};

enum DamageFlag
{
	DAMAGE_NORMAL	= (1 << 0),
	DAMAGE_POISON	= (1 << 1),
	DAMAGE_DODGE	= (1 << 2),
	DAMAGE_BLOCK	= (1 << 3),
	DAMAGE_PENETRATE = (1 << 4),
	DAMAGE_CRITICAL = (1 << 5),
	DAMAGE_FIRE = (1 << 6),
#if defined(__ENABLE_WOLFMAN_CHARACTER__) && !defined(__USE_MOB_BLEEDING_AS_POISON__)
	DAMAGE_BLEEDING	= (1 << 6),
#endif
};

enum EPointTypes
{
	POINT_NONE,						// 0
	POINT_LEVEL,					// 1
	POINT_VOICE,					// 2
	POINT_EXP,						// 3
	POINT_NEXT_EXP,					// 4
	POINT_HP,						// 5
	POINT_MAX_HP,					// 6
	POINT_SP,						// 7
	POINT_MAX_SP,					// 8
	POINT_STAMINA,					// 9
	POINT_MAX_STAMINA,				// 10

	POINT_GOLD,						// 11
	POINT_ST,						// 12
	POINT_HT,						// 13
	POINT_DX,						// 14
	POINT_IQ,						// 15
	POINT_DEF_GRADE,				// 16 ...
	POINT_ATT_SPEED,				// 17
	POINT_ATT_GRADE,				// 18
	POINT_MOV_SPEED,				// 19
	POINT_CLIENT_DEF_GRADE,			// 20
	POINT_CASTING_SPEED,			// 21
	POINT_MAGIC_ATT_GRADE,			// 22
	POINT_MAGIC_DEF_GRADE,			// 23
	POINT_EMPIRE_POINT,				// 24
	POINT_LEVEL_STEP,				// 25
	POINT_STAT,						// 26
	POINT_SUB_SKILL,				// 27
	POINT_SKILL,					// 28
	POINT_WEAPON_MIN,				// 29
	POINT_WEAPON_MAX,				// 30
	POINT_PLAYTIME,					// 31
	POINT_HP_REGEN,					// 32
	POINT_SP_REGEN,					// 33

	POINT_BOW_DISTANCE,				// 34

	POINT_HP_RECOVERY,				// 35
	POINT_SP_RECOVERY,				// 36

	POINT_POISON_PCT,				// 37
	POINT_STUN_PCT,					// 38
	POINT_SLOW_PCT,					// 39
	POINT_CRITICAL_PCT,				// 40
	POINT_PENETRATE_PCT,			// 41
	POINT_CURSE_PCT,				// 42

	POINT_ATTBONUS_HUMAN,			// 43
	POINT_ATTBONUS_ANIMAL,			// 44
	POINT_ATTBONUS_ORC,				// 45
	POINT_ATTBONUS_MILGYO,			// 46
	POINT_ATTBONUS_UNDEAD,			// 47
	POINT_ATTBONUS_DEVIL,			// 48
	POINT_ATTBONUS_INSECT,			// 49
	POINT_ATTBONUS_FIRE,			// 50
	POINT_ATTBONUS_ICE,				// 51
	POINT_ATTBONUS_DESERT,			// 52
	POINT_ATTBONUS_MONSTER,			// 53
	POINT_ATTBONUS_WARRIOR,			// 54
	POINT_ATTBONUS_ASSASSIN,		// 55
	POINT_ATTBONUS_SURA,			// 56
	POINT_ATTBONUS_SHAMAN,			// 57
	POINT_ATTBONUS_TREE,     		// 58

	POINT_RESIST_WARRIOR,			// 59
	POINT_RESIST_ASSASSIN,			// 60
	POINT_RESIST_SURA,				// 61
	POINT_RESIST_SHAMAN,			// 62

	POINT_STEAL_HP,					// 63
	POINT_STEAL_SP,					// 64

	POINT_MANA_BURN_PCT,			// 65
	POINT_DAMAGE_SP_RECOVER,		// 66

	POINT_BLOCK,					// 67
	POINT_DODGE,					// 68

	POINT_RESIST_SWORD,				// 69
	POINT_RESIST_TWOHAND,			// 70
	POINT_RESIST_DAGGER,			// 71
	POINT_RESIST_BELL,				// 72
	POINT_RESIST_FAN,				// 73
	POINT_RESIST_BOW,				// 74
	POINT_RESIST_FIRE,				// 75
	POINT_RESIST_ELEC,				// 76
	POINT_RESIST_MAGIC,				// 77
	POINT_RESIST_WIND,				// 78

	POINT_REFLECT_MELEE,			// 79

	POINT_REFLECT_CURSE,			// 80
	POINT_POISON_REDUCE,			// 81

	POINT_KILL_SP_RECOVER,			// 82
	POINT_EXP_DOUBLE_BONUS,			// 83
	POINT_GOLD_DOUBLE_BONUS,		// 84
	POINT_ITEM_DROP_BONUS,			// 85

	POINT_POTION_BONUS,				// 86
	POINT_KILL_HP_RECOVERY,			// 87

	POINT_IMMUNE_STUN,				// 88
	POINT_IMMUNE_SLOW,				// 89
	POINT_IMMUNE_FALL,				// 90
	//////////////////

	POINT_PARTY_ATTACKER_BONUS,		// 91
	POINT_PARTY_TANKER_BONUS,		// 92

	POINT_ATT_BONUS,				// 93
	POINT_DEF_BONUS,				// 94

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

#ifdef __ENABLE_WOLFMAN_CHARACTER__
	POINT_BLEEDING_REDUCE = 138,
	POINT_BLEEDING_PCT = 139,

	POINT_ATTBONUS_WOLFMAN = 140,
	POINT_RESIST_WOLFMAN = 141,
	POINT_RESIST_CLAW = 142,
#endif

#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	POINT_RESIST_MAGIC_REDUCTION = 144,
#endif

#ifdef __ENABLE_SASH_SYSTEM__
	POINT_ACCEDRAIN_RATE,
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	POINT_GAYA,
#endif

#ifdef __ENABLE_12ZI_ELEMENT_ADD__
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

	//POINT_MAX_NUM = 129	common/length.h
};

enum EPKModes
{
	PK_MODE_PEACE,
	PK_MODE_REVENGE,
	PK_MODE_FREE,
	PK_MODE_PROTECT,
	PK_MODE_GUILD,
	PK_MODE_MAX_NUM
};

enum EPositions
{
	POS_DEAD,
	POS_SLEEPING,
	POS_RESTING,
	POS_SITTING,
	POS_FISHING,
	POS_FIGHTING,
	POS_MOUNTING,
	POS_STANDING
};

enum EBlockAction
{
	BLOCK_EXCHANGE		= (1 << 0),
	BLOCK_PARTY_INVITE		= (1 << 1),
	BLOCK_GUILD_INVITE		= (1 << 2),
	BLOCK_WHISPER		= (1 << 3),
	BLOCK_MESSENGER_INVITE	= (1 << 4),
	BLOCK_PARTY_REQUEST		= (1 << 5),
};

// <Factor> Dynamically evaluated CHARACTER* equivalent.
// Referring to SCharDeadEventInfo.
struct DynamicCharacterPtr
{
	DynamicCharacterPtr() : is_pc(false), id(0) {}
	DynamicCharacterPtr(const DynamicCharacterPtr& o)
		: is_pc(o.is_pc), id(o.id) {}

	// Returns the LPCHARACTER found in CHARACTER_MANAGER.
	LPCHARACTER Get() const;
	// Clears the current settings.
	void Reset()
	{
		is_pc = false;
		id = 0;
	}

	// Basic assignment operator.
	DynamicCharacterPtr& operator=(const DynamicCharacterPtr& rhs)
	{
		is_pc = rhs.is_pc;
		id = rhs.id;
		return *this;
	}
	// Supports assignment with LPCHARACTER type.
	DynamicCharacterPtr& operator=(LPCHARACTER character);
	// Supports type casting to LPCHARACTER.
	operator LPCHARACTER() const
	{
		return Get();
	}

	bool is_pc;
	uint32_t id;
};


typedef struct character_point
{
	long			points[POINT_MAX_NUM];

	BYTE			job;
	BYTE			voice;

	BYTE			level;
	DWORD			exp;

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t			gold;
#else
	long			gold;
#endif

	int				hp;
	int				sp;

	int				iRandomHP;
	int				iRandomSP;

	int				stamina;

	BYTE			skill_group;
#ifdef __GAYA_SHOP_SYSTEM__
	DWORD			gaya_coins;
#endif
} CHARACTER_POINT;


typedef struct character_point_instant
{
	long			points[POINT_MAX_NUM];

	float			fRot;

	int				iMaxHP;
	int				iMaxSP;

	long			position;

	long			instant_flag;
	DWORD			dwAIFlag;
	DWORD			dwImmuneFlag;
	DWORD			dwLastShoutPulse;

#ifdef __ENABLE_HIDE_COSTUMES__
	DWORD			dwCostumeFlag;
#endif

	DWORD			parts[PART_MAX_NUM];

	LPITEM			pItems[INVENTORY_AND_EQUIP_SLOT_MAX];
#ifdef __SPECIAL_STORAGE_ENABLE__
	WORD			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#else
	DWORD			bItemGrid[INVENTORY_AND_EQUIP_SLOT_MAX];
#endif

	LPITEM			pDSItems[DRAGON_SOUL_INVENTORY_MAX_NUM];
	WORD			wDSItemGrid[DRAGON_SOUL_INVENTORY_MAX_NUM];
#ifdef __ENABLE_SWITCHBOT__
	LPITEM			pSwitchbotItems[SWITCHBOT_SLOT_COUNT];
#endif

	// by mhh
	// LPITEM			pCubeItems[CUBE_MAX_NUM];
	// LPCHARACTER		pCubeNpc;
	LPCHARACTER		battle_victim;

	BYTE			gm_level;

	BYTE			bBasePart;

	int				iMaxStamina;

	BYTE			bBlockMode;

	int				iDragonSoulActiveDeck;
	LPENTITY		m_pDragonSoulRefineWindowOpener;
} CHARACTER_POINT_INSTANT;

#define TRIGGERPARAM		LPCHARACTER ch, LPCHARACTER causer

typedef struct trigger
{
	BYTE	type;
	int		(*func) (TRIGGERPARAM);
	long	value;
} TRIGGER;

class CTrigger
{
public:
	CTrigger() : bType(0), pFunc(NULL)
	{
	}

	BYTE	bType;
	int	(*pFunc) (TRIGGERPARAM);
};

EVENTINFO(char_event_info)
{
	DynamicCharacterPtr ch;
};

typedef std::map<VID, size_t> target_map;
struct TSkillUseInfo
{
	int	    iHitCount;
	int	    iMaxHitCount;
	int	    iSplashCount;
	DWORD   dwNextSkillUsableTime;
	int	    iRange;
	bool    bUsed;
	DWORD   dwVID;
	bool    isGrandMaster;

	target_map TargetVIDMap;

	TSkillUseInfo()
		: iHitCount(0), iMaxHitCount(0), iSplashCount(0), dwNextSkillUsableTime(0), iRange(0), bUsed(false),
		  dwVID(0), isGrandMaster(false)
	{}

	bool    HitOnce(DWORD dwVnum = 0);

	bool    UseSkill(bool isGrandMaster, DWORD vid, DWORD dwCooltime, int splashcount = 1, int hitcount = -1, int range = -1);
	DWORD   GetMainTargetVID() const	{ return dwVID; }
	void    SetMainTargetVID(DWORD vid) { dwVID = vid; }
	void    ResetHitCount() { if (iSplashCount) { iHitCount = iMaxHitCount; iSplashCount--; } }
};

typedef struct packet_party_update TPacketGCPartyUpdate;
class CExchange;
class CSkillProto;
class CParty;
class CDungeon;
class CWarMap;
class CAffect;
class CGuild;
class CSafebox;
class CArena;

class CShop;
typedef class CShop * LPSHOP;

class CMob;
class CMobInstance;
typedef struct SMobSkillInfo TMobSkillInfo;

//SKILL_POWER_BY_LEVEL
extern int GetSkillPowerByLevelFromType(int job, int skillgroup, int skilllevel);
//END_SKILL_POWER_BY_LEVEL

namespace marriage
{
class WeddingMap;
}
enum e_overtime
{
	OT_NONE,
	OT_3HOUR,
	OT_5HOUR,
};

#ifdef __ENABLE_DELETE_SINGLE_STONE__
using TDeleteSocket = struct SDeleteSocket
{
	bool open{ false };
	int cooltime{ 0 };
	TItemPos itemPos{ INVENTORY, WORD_MAX };
};
#endif

#define NEW_ICEDAMAGE_SYSTEM
class CHARACTER : public CEntity, public CFSM, public CHorseRider
{
protected:
	//////////////////////////////////////////////////////////////////////////////////
	virtual void	EncodeInsertPacket(LPENTITY entity);
	virtual void	EncodeRemovePacket(LPENTITY entity);
	//////////////////////////////////////////////////////////////////////////////////

public:
	LPCHARACTER			FindCharacterInView(const char * name, bool bFindPCOnly);
	void				UpdatePacket();

	//////////////////////////////////////////////////////////////////////////////////
protected:
	CStateTemplate<CHARACTER>	m_stateMove;
	CStateTemplate<CHARACTER>	m_stateBattle;
	CStateTemplate<CHARACTER>	m_stateIdle;

public:
	virtual void		StateMove();
	virtual void		StateBattle();
	virtual void		StateIdle();
	virtual void		StateFlag();
	virtual void		StateFlagBase();
	void				StateHorse();

protected:
	// STATE_IDLE_REFACTORING
	void				__StateIdle_Monster();
	void				__StateIdle_Stone();
	void				__StateIdle_NPC();
	// END_OF_STATE_IDLE_REFACTORING

public:
	DWORD GetAIFlag() const	{ return m_pointsInstant.dwAIFlag; }

	void				SetAggressive();
	bool				IsAggressive() const;

	void				SetCoward();
	bool				IsCoward() const;
	void				CowardEscape();

	void				SetNoAttackShinsu();
	bool				IsNoAttackShinsu() const;

	void				SetNoAttackChunjo();
	bool				IsNoAttackChunjo() const;

	void				SetNoAttackJinno();
	bool				IsNoAttackJinno() const;

	void				SetAttackMob();
	bool				IsAttackMob() const;

	virtual void			BeginStateEmpty();
	virtual void			EndStateEmpty() {}

	void				RestartAtSamePos();

protected:
	DWORD				m_dwStateDuration;
	//////////////////////////////////////////////////////////////////////////////////

public:
	CHARACTER();
	virtual ~CHARACTER();

	void			Create(const char * c_pszName, DWORD vid, bool isPC);
	void			Destroy();

	void			Disconnect(const char * c_pszReason);

protected:
	void			Initialize();

	//////////////////////////////////////////////////////////////////////////////////
	// Basic Points
public:
	DWORD			GetPlayerID() const	{ return m_dwPlayerID; }

	void			SetPlayerProto(const TPlayerTable * table);
	void			CreatePlayerProto(TPlayerTable & tab);

	void			SetProto(const CMob * c_pkMob);
	WORD			GetRaceNum() const;
#ifdef __ADMIN_MANAGER__
	uint16_t		GetRealRaceNum() const;
#endif

	void			Save();		// DelayedSave
	void			SaveReal();
	void			FlushDelayedSaveItem();

	const char *	GetName() const;
	const VID &		GetVID() const		{ return m_vid;		}

	void			SetName(const std::string& name) { m_stName = name; }

	void			SetRace(BYTE race);
	bool			ChangeSex();

	DWORD			GetAID() const;
	int				GetChangeEmpireCount() const;
	void			SetChangeEmpireCount();
	int				ChangeEmpire(BYTE empire);

	BYTE			GetJob() const;
	BYTE			GetCharType() const;

	bool			IsPC() const		{ return GetDesc() ? true : false; }
	bool			IsNPC()	const		{ return m_bCharType != CHAR_TYPE_PC; }
	bool			IsMonster()	const	{ return m_bCharType == CHAR_TYPE_MONSTER; }
	bool			IsStone() const		{ return m_bCharType == CHAR_TYPE_STONE; }
	bool			IsDoor() const		{ return m_bCharType == CHAR_TYPE_DOOR; }
	bool			IsBuilding() const	{ return m_bCharType == CHAR_TYPE_BUILDING;  }
	bool			IsWarp() const		{ return m_bCharType == CHAR_TYPE_WARP; }
	bool			IsGoto() const		{ return m_bCharType == CHAR_TYPE_GOTO; }
//		bool			IsPet() const		{ return m_bCharType == CHAR_TYPE_PET; }
#ifdef __CYCLIC_MONSTER_RESPAWNER__
	bool			IsBoss() const { return m_bCharType == CHAR_TYPE_MONSTER && (GetMobRank() >= MOB_RANK_BOSS || GetMobRank() == MOB_RANK_KING); }
#endif
	DWORD			GetLastShoutPulse() const	{ return m_pointsInstant.dwLastShoutPulse; }
	void			SetLastShoutPulse(DWORD pulse) { m_pointsInstant.dwLastShoutPulse = pulse; }
	int				GetLevel() const		{ return m_points.level;	}
	void			SetLevel(BYTE level);

#ifdef __ADMIN_MANAGER__
	BYTE			GetGMLevel(bool bIgnoreTestServer = false) const;
#else
	BYTE			GetGMLevel() const;
#endif
	BOOL 			IsGM() const;
	void			SetGMLevel();

	DWORD			GetExp() const		{ return m_points.exp;	}
	void			SetExp(DWORD exp)	{ m_points.exp = exp;	}
	DWORD			GetNextExp() const;
	LPCHARACTER		DistributeExp();
	void			DistributeHP(LPCHARACTER pkKiller);
	void			DistributeSP(LPCHARACTER pkKiller, int iMethod = 0);

	void			SetPosition(int pos);
	bool			IsPosition(int pos) const	{ return m_pointsInstant.position == pos ? true : false; }
	int				GetPosition() const		{ return m_pointsInstant.position; }

	void			SetPart(BYTE bPartPos, DWORD wVal);
	DWORD			GetPart(BYTE bPartPos) const;
	DWORD			GetOriginalPart(BYTE bPartPos) const;

	void			SetHP(int hp)		{ m_points.hp = hp; }
	int				GetHP() const		{ return m_points.hp; }

	void			SetSP(int sp)		{ m_points.sp = sp; }
	int				GetSP() const		{ return m_points.sp; }

	void			SetStamina(int stamina)	{ m_points.stamina = stamina; }
	int				GetStamina() const		{ return m_points.stamina; }

	void			SetMaxHP(int iVal)	{ m_pointsInstant.iMaxHP = iVal; }
	int				GetMaxHP() const	{ return std::max((int) 1, m_pointsInstant.iMaxHP); }

	void			SetMaxSP(int iVal)	{ m_pointsInstant.iMaxSP = iVal; }
	int				GetMaxSP() const	{ return m_pointsInstant.iMaxSP; }

	void			SetMaxStamina(int iVal)	{ m_pointsInstant.iMaxStamina = iVal; }
	int				GetMaxStamina() const	{ return m_pointsInstant.iMaxStamina; }

	void			SetRandomHP(int v)	{ m_points.iRandomHP = v; }
	void			SetRandomSP(int v)	{ m_points.iRandomSP = v; }

	int				GetRandomHP() const	{ return m_points.iRandomHP; }
	int				GetRandomSP() const	{ return m_points.iRandomSP; }

	int				GetHPPct() const;

	void			SetRealPoint(BYTE idx, int val);
	int				GetRealPoint(BYTE idx) const;

	void			SetPoint(BYTE idx, int val);
	int				GetPoint(BYTE idx) const;
#ifdef __ADMIN_MANAGER__
	int64_t			GetConvPoint(BYTE idx) const;
#endif
	int				GetLimitPoint(BYTE idx) const;
	int				GetPolymorphPoint(BYTE idx) const;

	const TMobTable &	GetMobTable() const;
	BYTE				GetMobRank() const;
	BYTE				GetMobBattleType() const;
	BYTE				GetMobSize() const;
	DWORD				GetMobDamageMin() const;
	DWORD				GetMobDamageMax() const;
	WORD				GetMobAttackRange() const;
	DWORD				GetMobDropItemVnum() const;
	float				GetMobDamageMultiply() const;

	// NEWAI
	bool			IsBerserker() const;
	bool			IsBerserk() const;
	void			SetBerserk(bool mode);

	bool			IsStoneSkinner() const;

	bool			IsGodSpeeder() const;
	bool			IsGodSpeed() const;
	void			SetGodSpeed(bool mode);

	bool			IsDeathBlower() const;
	bool			IsDeathBlow() const;

	bool			IsReviver() const;
	bool			HasReviverInParty() const;
	bool			IsRevive() const;
	void			SetRevive(bool mode);
	// NEWAI END

	bool			IsRaceFlag(DWORD dwBit) const;
	bool			IsSummonMonster() const;
	DWORD			GetSummonVnum() const;

	DWORD			GetPolymorphItemVnum() const;
	DWORD			GetMonsterDrainSPPoint() const;

	void			MainCharacterPacket();

	void			ComputePoints();
	void			ComputeBattlePoints();

#ifdef __EXTANDED_GOLD_AMOUNT__
	void			PointChange(BYTE type, int64_t amount, bool bAmount = false, bool bBroadcast = false);
#else
	void			PointChange(BYTE type, int amount, bool bAmount = false, bool bBroadcast = false);
#endif
	void			PointsPacket();
	void			ApplyPoint(BYTE bApplyType, int iVal);
	void			CheckMaximumPoints();

	bool			Show(long lMapIndex, long x, long y, long z = LONG_MAX, bool bShowSpawnMotion = false);

	void			Sitdown(int is_ground);
	void			Standup();

	void			SetRotation(float fRot);
	void			SetRotationToXY(long x, long y);
	float			GetRotation() const	{ return m_pointsInstant.fRot; }

	void			MotionPacketEncode(BYTE motion, LPCHARACTER victim, struct packet_motion * packet);
	void			Motion(BYTE motion, LPCHARACTER victim = NULL);

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	void			ChatPacket(BYTE type, const std::string text, ...);
	void			ChatPacket(BYTE type, BYTE locale, const std::string text, ...);
	void			RawChatPacket(BYTE type, BYTE locale, const char* text, ...);
	void			RawChatPacket(BYTE type, const char* text, ...);
	void			SendChatPacket(BYTE type, int locale, int chatLength, const char* chat);
#else
	void			ChatPacket(BYTE type, const char *format, ...);
#endif
	void			MonsterChat(BYTE bMonsterChatType);
	void			SendGreetMessage();

	void			ResetPoint(int iLv);

	void			SetBlockMode(BYTE bFlag);
	void			SetBlockModeForce(BYTE bFlag);
	bool			IsBlockMode(BYTE bFlag) const	{ return (m_pointsInstant.bBlockMode & bFlag) ? true : false; }

	bool			IsPolymorphed() const		{ return m_dwPolymorphRace > 0; }
	bool			IsPolyMaintainStat() const	{ return m_bPolyMaintainStat; }
	void			SetPolymorph(DWORD dwRaceNum, bool bMaintainStat = false);
	DWORD			GetPolymorphVnum() const	{ return m_dwPolymorphRace; }
	int				GetPolymorphPower() const;

	// FISING
#ifdef __ABUSE_CONTROLLER_ENABLE__
	bool			IS_VALID_FISHING_POSITION(int* returnPosx, int* returnPosy);
#endif

	// FISING
	void			fishing();
	void			fishing_take();
	// END_OF_FISHING

	// MINING
	void			mining(LPCHARACTER chLoad);
	void			mining_cancel();
	void			mining_take();
	// END_OF_MINING

	void			ResetPlayTime(DWORD dwTimeRemain = 0);

	void			CreateFly(BYTE bType, LPCHARACTER pkVictim);

	void			ResetChatCounter();
	BYTE			IncreaseChatCounter();
	BYTE			GetChatCounter() const;

protected:
	DWORD			m_dwPolymorphRace;
	bool			m_bPolyMaintainStat;
	DWORD			m_dwLoginPlayTime;
	DWORD			m_dwPlayerID;
	VID				m_vid;
	std::string		m_stName;
	BYTE			m_bCharType;

	CHARACTER_POINT		m_points = {};
	CHARACTER_POINT_INSTANT	m_pointsInstant = {};

	int				m_iMoveCount;
	DWORD			m_dwPlayStartTime;
	BYTE			m_bAddChrState;
	bool			m_bSkipSave;
	std::string		m_stMobile;
	char			m_szMobileAuth[5];
	BYTE			m_bChatCounter;

	// End of Basic Points

	//////////////////////////////////////////////////////////////////////////////////
	// Move & Synchronize Positions
	//////////////////////////////////////////////////////////////////////////////////
public:
	bool			IsStateMove() const			{ return IsState((CState&)m_stateMove); }
	bool			IsStateIdle() const			{ return IsState((CState&)m_stateIdle); }
	bool			IsWalking() const			{ return m_bNowWalking || GetStamina() <= 0; }
	void			SetWalking(bool bWalkFlag)	{ m_bWalking = bWalkFlag; }
	void			SetNowWalking(bool bWalkFlag);
	void			ResetWalking()			{ SetNowWalking(m_bWalking); }

	bool			Goto(long x, long y);
	void			Stop();

	bool			CanMove() const;

	void			SyncPacket();
	bool			Sync(long x, long y);
	bool			Move(long x, long y);
	void			OnMove(bool bIsAttack = false);
	DWORD			GetMotionMode() const;
	float			GetMoveMotionSpeed() const;
	float			GetMoveSpeed() const;
	void			CalculateMoveDuration();
	void			SendMovePacket(BYTE bFunc, BYTE bArg, DWORD x, DWORD y, DWORD dwDuration, DWORD dwTime = 0, int iRot = -1);
	DWORD			GetCurrentMoveDuration() const	{ return m_dwMoveDuration; }
	DWORD			GetWalkStartTime() const	{ return m_dwWalkStartTime; }
	DWORD			GetLastMoveTime() const		{ return m_dwLastMoveTime; }
	DWORD			GetLastAttackTime() const	{ return m_dwLastAttackTime; }

	void			SetLastAttacked(DWORD time);

	bool			SetSyncOwner(LPCHARACTER ch, bool bRemoveFromList = true);
	bool			IsSyncOwner(LPCHARACTER ch) const;

	bool			WarpSet(long x, long y, long lRealMapIndex = 0);
	bool			WarpSet(long x, long y, long lPrivateMapIndex, long lMapIndex, long lAddr, WORD wPort);
	void			WarpSet(long x, long y, long lPrivateMapIndex, long lAddr, WORD wPort);

	void			SetWarpLocation(long lMapIndex, long x, long y);
	void			WarpEnd();
	const PIXEL_POSITION & GetWarpPosition() const { return m_posWarp; }
	bool			WarpToPID(DWORD dwPID);

	void			SaveExitLocation();
	void			ExitToSavedLocation();

	void			StartStaminaConsume();
	void			StopStaminaConsume();
	bool			IsStaminaConsume() const;
	bool			IsStaminaHalfConsume() const;

	void			ResetStopTime();
	DWORD			GetStopTime() const;

protected:
	void			ClearSync();

	float			m_fSyncTime;
	LPCHARACTER		m_pkChrSyncOwner;
	CHARACTER_LIST	m_kLst_pkChrSyncOwned;

	PIXEL_POSITION	m_posDest;
	PIXEL_POSITION	m_posStart;
	PIXEL_POSITION	m_posWarp;
	long			m_lWarpMapIndex;

	PIXEL_POSITION	m_posExit;
	long			m_lExitMapIndex;

	DWORD			m_dwMoveStartTime;
	DWORD			m_dwMoveDuration;

	DWORD			m_dwLastMoveTime;
	DWORD			m_dwLastAttackTime;
	DWORD			m_dwWalkStartTime;
	DWORD			m_dwStopTime;

	bool			m_bWalking;
	bool			m_bNowWalking;
	bool			m_bStaminaConsume;
	// End

public:
	void			SyncQuickslot(BYTE bType, BYTE bOldPos, BYTE bNewPos);
	bool			GetQuickslot(BYTE pos, TQuickslot ** ppSlot);
	bool			SetQuickslot(BYTE pos, TQuickslot & rSlot);
	bool			DelQuickslot(BYTE pos);
	bool			SwapQuickslot(BYTE a, BYTE b);
	void			ChainQuickslotItem(LPITEM pItem, BYTE bType, BYTE bOldPos);

protected:
	TQuickslot		m_quickslot[QUICKSLOT_MAX_NUM];

	////////////////////////////////////////////////////////////////////////////////////////
	// Affect
public:
	void			StartAffectEvent();
	void			ClearAffect(bool bSave = false);
	void			ComputeAffect(CAffect * pkAff, bool bAdd);
	bool			AddAffect(DWORD dwType, BYTE bApplyOn, long lApplyValue, DWORD dwFlag, long lDuration, long lSPCost, bool bOverride, bool IsCube = false);
	void			RefreshAffect();
	bool			RemoveAffect(DWORD dwType);
	bool			IsAffectFlag(DWORD dwAff) const;

	bool			UpdateAffect();	// called from EVENT
	int				ProcessAffect();

	void			LoadAffect(DWORD dwCount, TPacketAffectElement * pElements);
	void			SaveAffect();


	bool			IsLoadedAffect() const	{ return m_bIsLoadedAffect; }

	bool			IsGoodAffect(BYTE bAffectType) const;

	void			RemoveGoodAffect();
	void			RemoveBadAffect();

	CAffect *		FindAffect(DWORD dwType, BYTE bApply = APPLY_NONE) const;
	const std::list<CAffect *> & GetAffectContainer() const	{ return m_list_pkAffect; }
	bool			RemoveAffect(CAffect * pkAff);

protected:
	bool			m_bIsLoadedAffect;
	TAffectFlag		m_afAffectFlag;
	std::list<CAffect *>	m_list_pkAffect;

public:
	// PARTY_JOIN_BUG_FIX
	void			SetParty(LPPARTY pkParty);
	LPPARTY			GetParty() const	{ return m_pkParty; }

	bool			RequestToParty(LPCHARACTER leader);
	void			DenyToParty(LPCHARACTER member);
	void			AcceptToParty(LPCHARACTER member);



	void			PartyInvite(LPCHARACTER pchInvitee);



	void			PartyInviteAccept(LPCHARACTER pchInvitee);



	void			PartyInviteDeny(DWORD dwPID);

	bool			BuildUpdatePartyPacket(TPacketGCPartyUpdate & out);
	int				GetLeadershipSkillLevel() const;

	bool			CanSummon(int iLeaderShip);

	void			SetPartyRequestEvent(LPEVENT pkEvent) { m_pkPartyRequestEvent = pkEvent; }

protected:



	void			PartyJoin(LPCHARACTER pkLeader);


	enum PartyJoinErrCode
	{
		PERR_NONE		= 0,
		PERR_SERVER,
		PERR_DUNGEON,
		PERR_OBSERVER,
		PERR_LVBOUNDARY,
		PERR_LOWLEVEL,
		PERR_HILEVEL,
		PERR_ALREADYJOIN,
		PERR_PARTYISFULL,
		PERR_SEPARATOR,			///< Error type separator.
		PERR_DIFFEMPIRE,
		PERR_MAX
	};



	static PartyJoinErrCode	IsPartyJoinableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);



	static PartyJoinErrCode	IsPartyJoinableMutableCondition(const LPCHARACTER pchLeader, const LPCHARACTER pchGuest);

	LPPARTY			m_pkParty;
	DWORD			m_dwLastDeadTime;
	LPEVENT			m_pkPartyRequestEvent;


	typedef std::map< DWORD, LPEVENT >	EventMap;
	EventMap		m_PartyInviteEventMap;

	// END_OF_PARTY_JOIN_BUG_FIX

	////////////////////////////////////////////////////////////////////////////////////////
	// Dungeon
public:
	void			SetDungeon(LPDUNGEON pkDungeon);
	LPDUNGEON		GetDungeon() const	{ return m_pkDungeon; }
	LPDUNGEON		GetDungeonForce() const;
protected:
	LPDUNGEON	m_pkDungeon;
	int			m_iEventAttr;

	////////////////////////////////////////////////////////////////////////////////////////
	// Guild
public:
	void			SetGuild(CGuild * pGuild);
	CGuild*			GetGuild() const	{ return m_pGuild; }

	void			SetWarMap(CWarMap* pWarMap);
	CWarMap*		GetWarMap() const	{ return m_pWarMap; }

protected:
	CGuild *		m_pGuild;
	DWORD			m_dwUnderGuildWarInfoMessageTime;
	CWarMap *		m_pWarMap;

	////////////////////////////////////////////////////////////////////////////////////////
	// Item related
public:
	bool			CanHandleItem(bool bSkipRefineCheck = false, bool bSkipObserver = false);

	bool			IsItemLoaded() const	{ return m_bItemLoaded; }
	void			SetItemLoaded()	{ m_bItemLoaded = true; }

	void			ClearItem();
#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
	void			SetItem(TItemPos Cell, LPITEM item, bool bWereMine = false);
#else
	void			SetItem(TItemPos Cell, LPITEM item);
#endif
	LPITEM			GetItem(TItemPos Cell) const;
	LPITEM			GetInventoryItem(WORD wCell) const;
	bool			IsEmptyItemGrid(TItemPos Cell, BYTE size, int iExceptionCell = -1) const;

	void			SetWear(BYTE bCell, LPITEM item);
	LPITEM			GetWear(BYTE bCell) const;

	// MYSHOP_PRICE_LIST
	void			UseSilkBotary(void);



	void			UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p);
	// END_OF_MYSHOP_PRICE_LIST

	bool			UseItemEx(LPITEM item, TItemPos DestCell);
	bool			UseItem(TItemPos Cell, TItemPos DestCell = NPOS);

	// ADD_REFINE_BUILDING
	bool			IsRefineThroughGuild() const;
	CGuild *		GetRefineGuild() const;
	int				ComputeRefineFee(int iCost, int iMultiply = 5) const;
	void			PayRefineFee(int iTotalMoney);
	void			SetRefineNPC(LPCHARACTER ch);
#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
	//Add_logical_getter_that_ymir_doesnt_use_because_its_ymir
	LPCHARACTER		GetRefineNPC() const;
	//End_of_logic
#endif
	// END_OF_ADD_REFINE_BUILDING

	bool			RefineItem(LPITEM pkItem, LPITEM pkTarget);
	bool			DropItem(TItemPos Cell, CountType bCount = 0);

	bool			GiveRecallItem(LPITEM item);
	void			ProcessRecallItem(LPITEM item);

	//	void			PotionPacket(int iPotionType);
	void			EffectPacket(int enumEffectType);
	void			SpecificEffectPacket(const char filename[128]);

	// ADD_MONSTER_REFINE
	// END_OF_ADD_MONSTER_REFINE
#ifdef __ENABLE_FAST_REFINE_OPTION__
	bool			DoRefine(LPITEM item, bool bMoneyOnly = false, bool autoReopen = false);
	bool			DoRefineWithScroll(LPITEM item, bool autoReopen = false);
	bool			RefineInformation(CItem* item, uint8_t bType, CItem* additionalItem = nullptr, bool fast_refine = true, int addedPercent = 0);
#else
	bool			DoRefine(LPITEM item, bool bMoneyOnly = false);
	bool			DoRefineWithScroll(LPITEM item);
	bool			RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell = -1);
#endif

	void			SetRefineMode(CItem* additionalItem);
	void			ClearRefineMode();

	bool			GiveItem(LPCHARACTER victim, TItemPos Cell);
	bool			CanReceiveItem(LPCHARACTER from, LPITEM item, bool ignoreDist = false) const;
	void			ReceiveItem(LPCHARACTER from, LPITEM item);
	bool			GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector <DWORD> &dwItemVnums,
			std::vector <DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count);

	bool			MoveItem(TItemPos pos, TItemPos change_pos, CountType num);

	bool			PickupItem(DWORD vid);
#ifdef __ENABLE_DESTROY_ITEM_PACKET__
	bool			DestroyItem(TItemPos Cell);
#endif
	bool			EquipItem(LPITEM item, int iCandidateCell = -1);
	bool			UnequipItem(LPITEM item);


	bool			CanEquipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);


	bool			CanUnequipNow(const LPITEM item, const TItemPos& srcCell = NPOS, const TItemPos& destCell = NPOS);

	bool			SwapItem(BYTE bCell, BYTE bDestCell);
	bool CanTakeInventoryItem(LPITEM item, TItemPos* cell);

	LPITEM			AutoGiveItem(DWORD dwItemVnum, CountType bCount = 1, int iRarePct = -1, bool bMsg = true, bool bCheckDSGrid = false, bool bCommand = false);

	bool			AutoGiveItem(LPITEM item, bool longOwnerShip = false, bool autoStack = false);

#ifndef __SPECIAL_STORAGE_ENABLE__
	int				GetEmptyInventory(BYTE size) const;
#else
	int				GetEmptyInventory(BYTE size, DWORD wWindowType = INVENTORY, bool bSkipStandard = false) const;
#endif

	int				GetEmptyDragonSoulInventory(const TItemTable* pItemTable) const;
	int				GetEmptyDragonSoulInventory(LPITEM pItem) const;
	void			CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const;

	int				CountEmptyInventory() const;

#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
	int				CountSpecifyItem(DWORD vnum, int iExceptionCell = -1) const;
	void			RemoveSpecifyItem(DWORD vnum, CountType count = 1, int iExceptionCell = -1);
#else
	int				CountSpecifyItem(DWORD vnum) const;
	void			RemoveSpecifyItem(DWORD vnum, CountType count = 1);
#endif

	LPITEM			FindSpecifyItem(DWORD vnum) const;
	LPITEM			FindItemByID(DWORD id) const;

	CountType		CountSpecifyTypeItem(BYTE type) const;
	void			RemoveSpecifyTypeItem(BYTE type, CountType count = 1);

	bool			IsEquipUniqueItem(DWORD dwItemVnum) const;

	// CHECK_UNIQUE_GROUP
	bool			IsEquipUniqueGroup(DWORD dwGroupVnum) const;
	// END_OF_CHECK_UNIQUE_GROUP

	void			SendEquipment(LPCHARACTER ch);
	// End of Item

protected:

#ifdef __EXTANDED_GOLD_AMOUNT__
	void			SendMyShopPriceListCmd(DWORD dwItemVnum, int64_t dwItemPrice);
#else
	void			SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice);
#endif

	bool			m_bNoOpenedShop;

	bool			m_bItemLoaded;
	int				m_iRefineAdditionalCell;
	bool			m_bUnderRefine;
	DWORD			m_dwRefineNPCVID;

public:
	////////////////////////////////////////////////////////////////////////////////////////
	// Money related
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t			GetGold() const { return m_points.gold; }
	void			SetGold(int64_t gold) { m_points.gold = gold; }
	bool			DropGold(int64_t gold);
	int64_t			GetAllowedGold() const;
	void			GiveGold(int64_t iAmount);
#else
	INT				GetGold() const		{ return m_points.gold;	}
	void			SetGold(INT gold)	{ m_points.gold = gold;	}
	INT				GetAllowedGold() const;
	bool			DropGold(INT gold);
	void			GiveGold(INT iAmount);
#endif
	// End of Money

	////////////////////////////////////////////////////////////////////////////////////////
	// Shop related
public:
	void			SetShop(LPSHOP pkShop);
	LPSHOP			GetShop() const { return m_pkShop; }
	void			ShopPacket(BYTE bSubHeader);

	void			SetShopOwner(LPCHARACTER ch) { m_pkChrShopOwner = ch; }
	LPCHARACTER		GetShopOwner() const { return m_pkChrShopOwner;}

	void			OpenMyShop(const char * c_pszSign, TShopItemTable * pTable, BYTE bItemCount);

	LPSHOP			GetMyShop() const { return m_pkMyShop; }
	void			CloseMyShop();

protected:

	LPSHOP			m_pkShop;
	LPSHOP			m_pkMyShop;
	std::string		m_stShopSign;
	LPCHARACTER		m_pkChrShopOwner;
	// End of shop

	////////////////////////////////////////////////////////////////////////////////////////
	// Exchange related
public:
	bool			ExchangeStart(LPCHARACTER victim);
	void			SetExchange(CExchange * pkExchange);
	CExchange *		GetExchange() const	{ return m_pkExchange;	}

protected:
	CExchange *		m_pkExchange;
	// End of Exchange

	////////////////////////////////////////////////////////////////////////////////////////
	// Battle
public:
	struct TBattleInfo
	{
		int iTotalDamage;
		int iAggro;

		TBattleInfo(int iTot, int iAggr)
			: iTotalDamage(iTot), iAggro(iAggr)
		{}
	};
	typedef std::map<VID, TBattleInfo>	TDamageMap;

	typedef struct SAttackLog
	{
		DWORD	dwVID;
		DWORD	dwTime;
	} AttackLog;

	bool				Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL, bool bSkill = false);
	void				OnDamage(LPCHARACTER pAttacker, EDamageType eDamageType, int iRealDamage);

	bool				__Profile__Damage(LPCHARACTER pAttacker, int dam, EDamageType type = DAMAGE_TYPE_NORMAL);
	void				DeathPenalty(BYTE bExpLossPercent);
	void				ReviveInvisible(int iDur);

	bool				Attack(LPCHARACTER pkVictim, BYTE bType = 0);
	bool				IsAlive() const		{ return m_pointsInstant.position == POS_DEAD ? false : true; }
	bool				CanFight() const;

	bool				CanBeginFight() const;
	void				BeginFight(LPCHARACTER pkVictim);

	bool				CounterAttack(LPCHARACTER pkChr);

	bool				IsStun() const;
	void				Stun();
	bool				IsDead() const;
	void				Dead(LPCHARACTER pkKiller = NULL, bool bImmediateDead = false);

	void				Reward(bool bItemDrop);
	void				RewardGold(LPCHARACTER pkAttacker);

	bool				Shoot(BYTE bType);
	void				FlyTarget(DWORD dwTargetVID, long x, long y, BYTE bHeader);

	void				ForgetMyAttacker();
	void				AggregateMonster();

	void				AttractRanger();
	void				PullMonster();

	int					GetArrowAndBow(LPITEM * ppkBow, LPITEM * ppkArrow, CountType iArrowCount = 1);
	void				UseArrow(LPITEM pkArrow, CountType dwArrowCount);

	void				AttackedByPoison(LPCHARACTER pkAttacker);

	void				RemovePoison();
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	void				AttackedByBleeding(LPCHARACTER pkAttacker);
	void				RemoveBleeding();
#endif
	void				AttackedByFire(LPCHARACTER pkAttacker, int amount, int count);
	void				RemoveFire();

	void				UpdateAlignment(int iAmount);
	int					GetAlignment() const;


	int					GetRealAlignment() const;
	void				ShowAlignment(bool bShow);

	void				SetKillerMode(bool bOn);
	bool				IsKillerMode() const;
	void				UpdateKillerMode();

	BYTE				GetPKMode() const;
	void				SetPKMode(BYTE bPKMode);

	void				ItemDropPenalty(LPCHARACTER pkKiller);

	void				UpdateAggrPoint(LPCHARACTER ch, EDamageType type, int dam);

	//
	// HACK
	//
public:
	void SetComboSequence(BYTE seq);
	BYTE GetComboSequence() const;

	void SetLastComboTime(DWORD time);
	DWORD GetLastComboTime() const;

	int GetValidComboInterval() const;
	void SetValidComboInterval(int interval);

	BYTE GetComboIndex() const;

	void IncreaseComboHackCount(int k = 1);
	void ResetComboHackCount();
	void SkipComboAttackByTime(int interval);
	DWORD GetSkipComboAttackByTime() const;

protected:
	BYTE m_bComboSequence;
	DWORD m_dwLastComboTime;
	int m_iValidComboInterval;
	BYTE m_bComboIndex;
	int m_iComboHackCount;
	DWORD m_dwSkipComboAttackByTime;

protected:
	void				UpdateAggrPointEx(LPCHARACTER ch, EDamageType type, int dam, TBattleInfo & info);
	void				ChangeVictimByAggro(int iNewAggro, LPCHARACTER pNewVictim);

	DWORD				m_dwFlyTargetID;
	std::vector<DWORD>	m_vec_dwFlyTargets;
#ifdef __ANTY_SKILL_FLYING_TARGET__
	typedef std::vector<VID>	t_vec_Targets;
	std::map<BYTE, t_vec_Targets*> m_SkillMap;
#endif
	TDamageMap			m_map_kDamage;
//		AttackLog			m_kAttackLog;
	DWORD				m_dwKillerPID;

	int					m_iAlignment;		// Lawful/Chaotic value -200000 ~ 200000
	int					m_iRealAlignment;
	int					m_iKillerModePulse;
	BYTE				m_bPKMode;

	// Aggro
	DWORD				m_dwLastVictimSetTime;
	int					m_iMaxAggro;
	// End of Battle

	// Stone
public:
	void				SetStone(LPCHARACTER pkChrStone);
	void				ClearStone();
	void				DetermineDropMetinStone();
	DWORD				GetDropMetinStoneVnum() const { return m_dwDropMetinStone; }
	BYTE				GetDropMetinStonePct() const { return m_bDropMetinStonePct; }

protected:
	LPCHARACTER			m_pkChrStone;
	CHARACTER_SET		m_set_pkChrSpawnedBy;
	DWORD				m_dwDropMetinStone;
	BYTE				m_bDropMetinStonePct;
	// End of Stone

public:
	enum
	{
		SKILL_UP_BY_POINT,
		SKILL_UP_BY_BOOK,
		SKILL_UP_BY_TRAIN,

		// ADD_GRANDMASTER_SKILL
		SKILL_UP_BY_QUEST,
		// END_OF_ADD_GRANDMASTER_SKILL
	};

	void				SkillLevelPacket();
	void				SkillLevelUp(DWORD dwVnum, BYTE bMethod = SKILL_UP_BY_POINT);
	bool				SkillLevelDown(DWORD dwVnum);
	// ADD_GRANDMASTER_SKILL
	bool				UseSkill(DWORD dwVnum, LPCHARACTER pkVictim, bool bUseGrandMaster = true);
	void				ResetSkill();
#if defined(__SKILL_COOLTIME_UPDATE__)
	void				ResetSkillCoolTimes();
#endif
	void				SetSkillLevel(DWORD dwVnum, BYTE bLev);
	int					GetUsedSkillMasterType(DWORD dwVnum);

	bool				IsLearnableSkill(DWORD dwSkillVnum) const;
	// END_OF_ADD_GRANDMASTER_SKILL

	bool				CheckSkillHitCount(const BYTE SkillID, const VID dwTargetVID);
	bool				CanUseSkill(DWORD dwSkillVnum) const;
	bool				IsUsableSkillMotion(DWORD dwMotionIndex) const;
	int					GetSkillLevel(DWORD dwVnum) const;
	int					GetSkillMasterType(DWORD dwVnum) const;
	int					GetSkillPower(DWORD dwVnum, BYTE bLevel = 0) const;

	time_t				GetSkillNextReadTime(DWORD dwVnum) const;
	void				SetSkillNextReadTime(DWORD dwVnum, time_t time);
	void				SkillLearnWaitMoreTimeMessage(DWORD dwVnum);

	void				ComputePassiveSkill(DWORD dwVnum);
	int					ComputeSkill(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	int					ComputeSkillParty(DWORD dwVnum, LPCHARACTER pkVictim, BYTE bSkillLevel = 0);
#endif
	int					ComputeSkillAtPosition(DWORD dwVnum, const PIXEL_POSITION& posTarget, BYTE bSkillLevel = 0);
	void				ComputeSkillPoints();

	void				SetSkillGroup(BYTE bSkillGroup);
	BYTE				GetSkillGroup() const		{ return m_points.skill_group; }

	int					ComputeCooltime(int time);

	void				GiveRandomSkillBook();

	void				DisableCooltime();
	bool				LearnSkillByBook(DWORD dwSkillVnum, BYTE bProb = 0);
#ifdef __ENABLE_SKILLS_INFORMATION__
	void				BroadcastSkillInformation();
#endif
	bool				LearnGrandMasterSkill(DWORD dwSkillVnum, bool bGreater);

private:
	bool				m_bDisableCooltime;
	DWORD				m_dwLastSkillTime;
	// End of Skill

	// MOB_SKILL
public:
	bool				HasMobSkill() const;
	size_t				CountMobSkill() const;
	const TMobSkillInfo * GetMobSkill(unsigned int idx) const;
	bool				CanUseMobSkill(unsigned int idx) const;
	bool				UseMobSkill(unsigned int idx);
	void				ResetMobSkillCooltime();
protected:
	DWORD				m_adwMobSkillCooltime[MOB_SKILL_MAX_NUM];
	// END_OF_MOB_SKILL

	// for SKILL_MUYEONG
public:
	void				StartMuyeongEvent();
	void				StopMuyeongEvent();

private:
	LPEVENT				m_pkMuyeongEvent;

	// for SKILL_CHAIN lighting
public:
	int					GetChainLightningIndex() const { return m_iChainLightingIndex; }
	void				IncChainLightningIndex() { ++m_iChainLightingIndex; }
	void				AddChainLightningExcept(LPCHARACTER ch) { m_setExceptChainLighting.insert(ch); }
	void				ResetChainLightningIndex() { m_iChainLightingIndex = 0; m_setExceptChainLighting.clear(); }
	int					GetChainLightningMaxCount() const;
	const CHARACTER_SET& GetChainLightingExcept() const { return m_setExceptChainLighting; }

private:
	int					m_iChainLightingIndex;
	CHARACTER_SET m_setExceptChainLighting;

	// for SKILL_EUNHYUNG
public:
	void				SetAffectedEunhyung();
	void				ClearAffectedEunhyung() { m_dwAffectedEunhyungLevel = 0; }
	bool				GetAffectedEunhyung() const { return m_dwAffectedEunhyungLevel; }

private:
	DWORD				m_dwAffectedEunhyungLevel;

	//
	// Skill levels
	//


#ifdef __ADMIN_MANAGER__
public:
	const TPlayerSkill* GetPlayerSkills() const { return m_pSkillLevels; }
#endif

protected:
	TPlayerSkill*					m_pSkillLevels;
	boost::unordered_map<BYTE, int>		m_SkillDamageBonus;
	std::map<int, TSkillUseInfo>	m_SkillUseInfo;

	////////////////////////////////////////////////////////////////////////////////////////
	// AI related
public:
	void			AssignTriggers(const TMobTable * table);
	LPCHARACTER		GetVictim() const;
	void			SetVictim(LPCHARACTER pkVictim);
	LPCHARACTER		GetNearestVictim(LPCHARACTER pkChr);
	LPCHARACTER		GetProtege() const;

	bool			Follow(LPCHARACTER pkChr, float fMinimumDistance = 150.0f);
	bool			Return();
	bool			IsGuardNPC() const;
	bool			IsChangeAttackPosition(LPCHARACTER target) const;
	void			ResetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time() - AI_CHANGE_ATTACK_POISITION_TIME_NEAR;}
	void			SetChangeAttackPositionTime() { m_dwLastChangeAttackPositionTime = get_dword_time();}

	bool			OnIdle();

	void			OnAttack(LPCHARACTER pkChrAttacker);
	void			OnClick(LPCHARACTER pkChrCauser);

	VID				m_kVIDVictim;

protected:
	DWORD			m_dwLastChangeAttackPositionTime;
	CTrigger		m_triggerOnClick;
	// End of AI

	////////////////////////////////////////////////////////////////////////////////////////
	// Target
protected:
	LPCHARACTER				m_pkChrTarget;
	CHARACTER_SET	m_set_pkChrTargetedBy;

public:
	void				SetTarget(LPCHARACTER pkChrTarget);
	void				BroadcastTargetPacket();
	void				ClearTarget();
	void				CheckTarget();
	LPCHARACTER			GetTarget() const { return m_pkChrTarget; }

	////////////////////////////////////////////////////////////////////////////////////////
	// Safebox
public:
	int					GetSafeboxSize() const;
	void				QuerySafeboxSize();
	void				SetSafeboxSize(int size);

	CSafebox *			GetSafebox() const;
	void				LoadSafebox(int iSize, DWORD dwGold, int iItemCount, TPlayerItem * pItems);
	void				ChangeSafeboxSize(BYTE bSize);
	void				CloseSafebox();



	void				ReqSafeboxLoad(const char* pszPassword);



	void				CancelSafeboxLoad( void ) { m_bOpeningSafebox = false; }

	void				SetMallLoadTime(int t) { m_iMallLoadTime = t; }
	int					GetMallLoadTime() const { return m_iMallLoadTime; }

	CSafebox *			GetMall() const;
	void				LoadMall(int iItemCount, TPlayerItem * pItems);
	void				CloseMall();

	void				SetSafeboxOpenPosition();
	float				GetDistanceFromSafeboxOpen() const;

protected:
	CSafebox *			m_pkSafebox;
	int					m_iSafeboxSize;
	int					m_iSafeboxLoadTime;
	bool				m_bOpeningSafebox;

	CSafebox *			m_pkMall;
	int					m_iMallLoadTime;

	PIXEL_POSITION		m_posSafeboxOpen;

	////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////////////////////////////////////
	// Mounting
public:
	void				MountVnum(DWORD vnum);
	DWORD				GetMountVnum() const { return m_dwMountVnum; }
	DWORD				GetLastMountTime() const { return m_dwMountTime; }

	bool				CanUseHorseSkill();

	// Horse
	virtual	void		SetHorseLevel(int iLevel);

	virtual	bool		StartRiding();
	virtual	bool		StopRiding();

	virtual	DWORD		GetMyHorseVnum() const;

	virtual	void		HorseDie();
	virtual bool		ReviveHorse();

	virtual void		SendHorseInfo();
	virtual	void		ClearHorseInfo();

	void				HorseSummon(bool bSummon, bool bFromFar = false, DWORD dwVnum = 0, const char* name = 0);

	LPCHARACTER			GetHorse() const			{ return m_chHorse; }
	LPCHARACTER			GetRider() const; // rider on horse
	void				SetRider(LPCHARACTER ch);

	bool				IsRiding() const;

#ifdef __PET_SYSTEM__
public:
	CPetSystem*			GetPetSystem()				{ return m_petSystem; }

protected:
	CPetSystem*			m_petSystem;

public:
#endif

protected:
	LPCHARACTER			m_chHorse;
	LPCHARACTER			m_chRider;

	DWORD				m_dwMountVnum;
	DWORD				m_dwMountTime;

	BYTE				m_bSendHorseLevel;
	BYTE				m_bSendHorseHealthGrade;
	BYTE				m_bSendHorseStaminaGrade;

	////////////////////////////////////////////////////////////////////////////////////////
	// Detailed Log
public:
	void				DetailLog() { m_bDetailLog = !m_bDetailLog; }
	void				ToggleMonsterLog();
	void				MonsterLog(const char* format, ...);
private:
	bool				m_bDetailLog;
	bool				m_bMonsterLog;

	////////////////////////////////////////////////////////////////////////////////////////
	// Empire

public:
	void 				SetEmpire(BYTE bEmpire);
	BYTE				GetEmpire() const { return m_bEmpire; }

protected:
	BYTE				m_bEmpire;

	////////////////////////////////////////////////////////////////////////////////////////
	// Regen
public:
	void				SetRegen(LPREGEN pkRegen);
#ifdef __CYCLIC_MONSTER_RESPAWNER__
	LPREGEN				GetRegen()
	{
		return m_pkRegen;
	};
#endif

protected:
	PIXEL_POSITION			m_posRegen;
	float				m_fRegenAngle;
	LPREGEN				m_pkRegen;
	size_t				regen_id_; // to help dungeon regen identification
	// End of Regen

	////////////////////////////////////////////////////////////////////////////////////////
	// Resists & Proofs
public:
	bool				CannotMoveByAffect() const;
	bool				IsImmune(DWORD dwImmuneFlag);
	void				SetImmuneFlag(DWORD dw) { m_pointsInstant.dwImmuneFlag = dw; }
	void				UpdateImmuneFlags();

protected:
	void				ApplyMobAttribute(const TMobTable* table);
	// End of Resists & Proofs

	////////////////////////////////////////////////////////////////////////////////////////
	// QUEST
	//
public:
	void				SetQuestNPCID(DWORD vid);
	DWORD				GetQuestNPCID() const { return m_dwQuestNPCVID; }
	LPCHARACTER			GetQuestNPC() const;

	void				SetQuestItemPtr(LPITEM item);
	void				ClearQuestItemPtr();
	LPITEM				GetQuestItemPtr() const;

	void				SetQuestBy(DWORD dwQuestVnum)	{ m_dwQuestByVnum = dwQuestVnum; }
	DWORD				GetQuestBy() const			{ return m_dwQuestByVnum; }

	long				GetQuestFlag(const std::string& flag) const;
	void				SetQuestFlag(const std::string& flag, long value);

	void				ConfirmWithMsg(const char* szMsg, int iTimeout, DWORD dwRequestPID);

private:
	DWORD				m_dwQuestNPCVID;
	DWORD				m_dwQuestByVnum;
	LPITEM				m_pQuestItem;

	// Events
public:
	bool				StartStateMachine(int iPulse = 1);
	void				StopStateMachine();
	void				UpdateStateMachine(DWORD dwPulse);
	void				SetNextStatePulse(int iPulseNext);


	void				UpdateCharacter(DWORD dwPulse);

protected:
	DWORD				m_dwNextStatePulse;

	// Marriage
public:
	LPCHARACTER			GetMarryPartner() const;
	void				SetMarryPartner(LPCHARACTER ch);
	int					GetMarriageBonus(DWORD dwItemVnum, bool bSum = true);

	void				SetWeddingMap(marriage::WeddingMap* pMap);
	marriage::WeddingMap* GetWeddingMap() const { return m_pWeddingMap; }

private:
	marriage::WeddingMap* m_pWeddingMap;
	LPCHARACTER			m_pkChrMarried;

	// Warp Character
public:
	void				StartWarpNPCEvent();

public:
	void				StartSaveEvent();
	void				StartRecoveryEvent();
	void				StartCheckSpeedHackEvent();
	void				StartDestroyWhenIdleEvent();

	LPEVENT				m_pkDeadEvent;
	LPEVENT				m_pkStunEvent;
	LPEVENT				m_pkSaveEvent;
	LPEVENT				m_pkRecoveryEvent;
	LPEVENT				m_pkTimedEvent;
	LPEVENT				m_pkFishingEvent;
	LPEVENT				m_pkAffectEvent;
	LPEVENT				m_pkPoisonEvent;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	LPEVENT				m_pkBleedingEvent;
#endif
	LPEVENT				m_pkFireEvent;
	LPEVENT				m_pkWarpNPCEvent;
	//DELAYED_WARP
	//END_DELAYED_WARP

	// MINING
	LPEVENT				m_pkMiningEvent;
	// END_OF_MINING
	LPEVENT				m_pkWarpEvent;
	LPEVENT				m_pkCheckSpeedHackEvent;
	LPEVENT				m_pkDestroyWhenIdleEvent;
	LPEVENT				m_pkPetSystemUpdateEvent;

	bool IsWarping() const { return m_pkWarpEvent ? true : false; }

	bool				m_bHasPoisoned;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	bool				m_bHasBled;
#endif

	const CMob *		m_pkMobData;
	CMobInstance *		m_pkMobInst;

	std::map<int, LPEVENT> m_mapMobSkillEvent;

	friend struct FuncSplashDamage;
	friend struct FuncSplashAffect;
	friend class CFuncShoot;

public:
	int				GetPremiumRemainSeconds(BYTE bType) const;

private:
	int				m_aiPremiumTimes[PREMIUM_MAX_NUM];

	// CHANGE_ITEM_ATTRIBUTES
	static const char		msc_szLastChangeItemAttrFlag[];
	// END_OF_CHANGE_ITEM_ATTRIBUTES

	// PC_BANG_ITEM_ADD
private :
	bool m_isinPCBang;

public :
	bool SetPCBang(bool flag) { m_isinPCBang = flag; return m_isinPCBang; }
	bool IsPCBang() const { return m_isinPCBang; }
	// END_PC_BANG_ITEM_ADD

	// NEW_HAIR_STYLE_ADD
public :
	bool ItemProcess_Hair(LPITEM item, int iDestCell);
	// END_NEW_HAIR_STYLE_ADD

public :
	void ClearSkill();
	void ClearSubSkill();

	// RESET_ONE_SKILL
	bool ResetOneSkill(DWORD dwVnum);
	// END_RESET_ONE_SKILL

private :
	void SendDamagePacket(LPCHARACTER pAttacker, int Damage, BYTE DamageFlag);

	// ARENA
private :
	CArena *m_pArena;
	bool m_ArenaObserver;
	int m_nPotionLimit;

public :
	void 	SetArena(CArena* pArena) { m_pArena = pArena; }
	void	SetArenaObserverMode(bool flag) { m_ArenaObserver = flag; }

	CArena* GetArena() const { return m_pArena; }
	bool	GetArenaObserverMode() const { return m_ArenaObserver; }

	void	SetPotionLimit(int count) { m_nPotionLimit = count; }
	int		GetPotionLimit() const { return m_nPotionLimit; }
	// END_ARENA

	//PREVENT_TRADE_WINDOW
public:
	bool	IsOpenSafebox() const { return m_isOpenSafebox ? true : false; }
	void 	SetOpenSafebox(bool b) { m_isOpenSafebox = b; }

	int		GetSafeboxLoadTime() const { return m_iSafeboxLoadTime; }
	void	SetSafeboxLoadTime() { m_iSafeboxLoadTime = thecore_pulse(); }
	//END_PREVENT_TRADE_WINDOW
private:
	bool	m_isOpenSafebox;

public:
	int		GetSkillPowerByLevel(int level, bool bMob = false) const;

	//PREVENT_REFINE_HACK
	int		GetRefineTime() const { return m_iRefineTime; }
	void	SetRefineTime() { m_iRefineTime = thecore_pulse(); }
	int		m_iRefineTime;
	//END_PREVENT_REFINE_HACK

	//RESTRICT_USE_SEED_OR_MOONBOTTLE
	int 	GetUseSeedOrMoonBottleTime() const { return m_iSeedTime; }
	void  	SetUseSeedOrMoonBottleTime() { m_iSeedTime = thecore_pulse(); }
	int 	m_iSeedTime;
	//END_RESTRICT_USE_SEED_OR_MOONBOTTLE

	//PREVENT_PORTAL_AFTER_EXCHANGE
	int		GetExchangeTime() const { return m_iExchangeTime; }
	void	SetExchangeTime() { m_iExchangeTime = thecore_pulse(); }
	int		m_iExchangeTime;
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	int 	m_iMyShopTime;
	int		GetMyShopTime() const	{ return m_iMyShopTime; }
	void	SetMyShopTime() { m_iMyShopTime = thecore_pulse(); }


	bool	IsHack(bool bSendMsg = true, bool bCheckShopOwner = true, int limittime = g_nPortalLimitTime);

	void Say(const std::string & s);

public:
	bool ItemProcess_Polymorph(LPITEM item);

public:
	bool IsSiegeNPC() const;

private:

	e_overtime m_eOverTime;

public:
	bool IsOverTime(e_overtime e) const { return (e == m_eOverTime); }
	void SetOverTime(e_overtime e) { m_eOverTime = e; }

private:
	int		m_deposit_pulse;

public:
	void	UpdateDepositPulse();
	bool	CanDeposit() const;

private:
	void	__OpenPrivateShop();

public:
	struct AttackedLog
	{
		DWORD 	dwPID;
		DWORD	dwAttackedTime;

		AttackedLog() : dwPID(0), dwAttackedTime(0)
		{
		}
	};

	AttackLog	m_kAttackLog;
	AttackedLog m_AttackedLog;
	int			m_speed_hack_count;

private :
	std::string m_strNewName;

public :
	const std::string GetNewName() const { return this->m_strNewName; }
	void SetNewName(const std::string name) { this->m_strNewName = name; }

public :
	void GoHome();

private :
	std::set<DWORD>	m_known_guild;

public :
	void SendGuildName(CGuild* pGuild);
	void SendGuildName(DWORD dwGuildID);

private :
	DWORD m_dwLogOffInterval;

public :
	DWORD GetLogOffInterval() const { return m_dwLogOffInterval; }

public:
	bool UnEquipSpecialRideUniqueItem ();

	bool CanWarp () const;

private:
	DWORD m_dwLastGoldDropTime;
#ifdef __ENABLE_NEWSTUFF__
	DWORD m_dwLastItemDropTime;
	DWORD m_dwLastBoxUseTime;
	DWORD m_dwLastBuySellTime;
public:
	DWORD GetLastBuySellTime() const { return m_dwLastBuySellTime; }
	void SetLastBuySellTime(DWORD dwLastBuySellTime) { m_dwLastBuySellTime = dwLastBuySellTime; }
#endif
public:
	void StartHackShieldCheckCycle(int seconds);
	void StopHackShieldCheckCycle();

	bool GetHackShieldCheckMode() const { return m_HackShieldCheckMode; }
	void SetHackShieldCheckMode(bool m) { m_HackShieldCheckMode = m; }

	LPEVENT m_HackShieldCheckEvent;

private:
	bool	m_HackShieldCheckMode;

public:
	void AutoRecoveryItemProcess (const EAffectTypes);

public:
	void BuffOnAttr_AddBuffsFromItem(LPITEM pItem);
	void BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem);

private:
	void BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue);
	void BuffOnAttr_ClearAll();

	typedef std::map <BYTE, CBuffOnAttributes*> TMapBuffOnAttrs;
	TMapBuffOnAttrs m_map_buff_on_attrs;

public:
	void SetArmada() { cannot_dead = true; }
	void ResetArmada() { cannot_dead = false; }
private:
	bool cannot_dead;

#ifdef NEW_ICEDAMAGE_SYSTEM
private:
	DWORD m_dwNDRFlag;
	std::set<DWORD> m_setNDAFlag;
public:
	const DWORD GetNoDamageRaceFlag();
	void SetNoDamageRaceFlag(DWORD dwRaceFlag);
	void UnsetNoDamageRaceFlag(DWORD dwRaceFlag);
	void ResetNoDamageRaceFlag();
	const std::set<DWORD> & GetNoDamageAffectFlag();
	void SetNoDamageAffectFlag(DWORD dwAffectFlag);
	void UnsetNoDamageAffectFlag(DWORD dwAffectFlag);
	void ResetNoDamageAffectFlag();
#endif


private:
	float m_fAttMul;
	float m_fDamMul;
public:
	float GetAttMul() { return this->m_fAttMul; }
	void SetAttMul(float newAttMul) {this->m_fAttMul = newAttMul; }
	float GetDamMul() { return this->m_fDamMul; }
	void SetDamMul(float newDamMul) {this->m_fDamMul = newDamMul; }

private:
	bool IsValidItemPosition(TItemPos Pos) const;

public:
	void	DragonSoul_Initialize();

	bool	DragonSoul_IsQualified() const;
	void	DragonSoul_GiveQualification();

	int		DragonSoul_GetActiveDeck() const;
	bool	DragonSoul_IsDeckActivated() const;
	bool	DragonSoul_ActivateDeck(int deck_idx);

	void	DragonSoul_DeactivateAll();

	void	DragonSoul_CleanUp();

public:
	bool		DragonSoul_RefineWindow_Open(LPENTITY pEntity);
	bool		DragonSoul_RefineWindow_Close();
	LPENTITY	DragonSoul_RefineWindow_GetOpener() { return  m_pointsInstant.m_pDragonSoulRefineWindowOpener; }
	bool		DragonSoul_RefineWindow_CanRefine();
	int			DragonSoul_GetEmptyInventoryTypeCount(BYTE bSize = 1, BYTE bGrade = 0);


private:
	unsigned int itemAward_vnum;
	char		 itemAward_cmd[20];
	//bool		 itemAward_flag;
public:
	unsigned int GetItemAward_vnum() { return itemAward_vnum; }
	char*		 GetItemAward_cmd() { return itemAward_cmd;	  }
	//bool		 GetItemAward_flag() { return itemAward_flag; }
	void		 SetItemAward_vnum(unsigned int vnum) { itemAward_vnum = vnum; }
	void		 SetItemAward_cmd(char* cmd) { strcpy(itemAward_cmd, cmd); }
	//void		 SetItemAward_flag(bool flag) { itemAward_flag = flag; }
#ifdef ENABLE_ANTI_CMD_FLOOD
private:
	int m_dwCmdAntiFloodPulse;
	DWORD m_dwCmdAntiFloodCount;
public:
	int GetCmdAntiFloodPulse() {return m_dwCmdAntiFloodPulse;}
	DWORD GetCmdAntiFloodCount() {return m_dwCmdAntiFloodCount;}
	DWORD IncreaseCmdAntiFloodCount() {return ++m_dwCmdAntiFloodCount;}
	void SetCmdAntiFloodPulse(int dwPulse) {m_dwCmdAntiFloodPulse = dwPulse;}
	void SetCmdAntiFloodCount(DWORD dwCount) {m_dwCmdAntiFloodCount = dwCount;}
#endif
private:


	timeval		m_tvLastSyncTime;
	int			m_iSyncHackCount;
public:
	void			SetLastSyncTime(const timeval &tv) { memcpy(&m_tvLastSyncTime, &tv, sizeof(timeval)); }
	const timeval&	GetLastSyncTime() { return m_tvLastSyncTime; }
	void			SetSyncHackCount(int iCount) { m_iSyncHackCount = iCount;}
	int				GetSyncHackCount() { return m_iSyncHackCount; }


#ifdef __ENABLE_TREASURE_BOX_LOOT__
public:
	void	SendTreasureBoxLoot(DWORD dwItemVnum);
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
public:
	void		OpenDestroyItemSocket(WORD wIndex);
	void		DestroyItemSocket(WORD wIndex);
	void		CloseDestroyItemSocket();

private:
	TDeleteSocket	m_deleteSocket;

#endif

public:
	inline bool	CanDoAction()
	{
		if (!IsPC())
		{
			return false;
		}

		if (GetShopOwner() || GetShop() || !CanHandleItem() || !CanWarp() || GetExchange())
		{
			return false;
		}

		return true;
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
public:
	void SortInventory();
#endif

#ifdef __GAYA_SHOP_SYSTEM__
public:
	DWORD	GetGayaPoints() const { return m_points.gaya_coins; }
	void	ChargeGayaPoints(const int& iPoints) { PointChange(POINT_GAYA, -iPoints); m_points.gaya_coins -= iPoints; }
	void	GiveGayaPoints(const int& iPoints) { PointChange(POINT_GAYA, iPoints); m_points.gaya_coins += iPoints; }
	void	InsertGayaPoints(LPITEM item);
#endif

#ifdef __RENEWAL_DEAD_PACKET__
public:
	DWORD			CalculateDeadTime(BYTE type);
#endif

#ifdef __ENABLE_TICKET_COINS__
public:
	void			InsertCoins(LPITEM item);
#endif

#ifdef __ENABLE_FAST_REFINE_OPTION__
public:
	void			SetFastRefineVnum(DWORD dwVnum);

	void			DoFastRefine(LPITEM item);

	void			CloseRefine();

protected:
	DWORD			m_dwRefineScrollVnum;
#endif

#ifdef __NEW_AGGREGATE_MONSTERS__
public:
	void			SetCapeEffectPulse(int i) { m_iCapeEffectPulse = i; }
	int				GetCapeEffectPulse() { return m_iCapeEffectPulse; }

protected:
	int				m_iCapeEffectPulse;
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
private:
	LPEVENT	m_pkSwitchChannelEvent;
	int		iNextChannelSwitchTime;

public:
	void	SwitchChannel(long lAddr, WORD wPort);
	void	StartSwitchChannelEvent(long lAddr, WORD wPort);
	bool	QuitSwitchChannelEvent();
	void	SetSwitchChannelEvent(LPEVENT pkEvent);
	int		GetLeftTimeUntilNextChannelSwitch();
#endif

#ifdef __HIT_TRACKER_ENABLE__
public:
	void RegisterHitTrackingInfo(const std::string & sEventName, const std::string & sTestName, const time_t & ttEventDelay);
	void StartHitTracking();
	void RegisterPCDamage();
	void StopHitTracking();

private:
	std::string sTrackingEventName;
	std::string sTrackingTestName;
	time_t ttTrackingEventDelay;

	int iHitCount;
	bool bTrackingTrigger;
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
public:
	bool				HasCostumeFlag(DWORD dwCostumeFlag);
	void				SetCostumeFlag(DWORD dw) { m_pointsInstant.dwCostumeFlag = dw; }
	DWORD				GetCostumeFlag() { return m_pointsInstant.dwCostumeFlag; }
	void				BroadcastHCostume();
#endif

#ifdef __ADMIN_MANAGER__
public:
	void SetChatBanCount(DWORD dwChatBanCount) { m_dwChatBanCount = dwChatBanCount; Save(); }
	DWORD GetChatBanCount() const { return m_dwChatBanCount; }
	DWORD GetAccountBanCount() const;

private:
	DWORD m_dwChatBanCount;
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
public:
	void RegisterNotificationCache(const DWORD & pid, const time_t & ttSentTime) { dwNotificationCollector = pid; ttNotificationSentTime = ttSentTime; }
	const DWORD & GetNotificationCache() { return dwNotificationCollector; }
	const time_t & GetNotificationSentTime() { return ttNotificationSentTime; }

private:
	DWORD dwNotificationCollector;
	time_t	ttNotificationSentTime;
#endif

#ifdef __HIT_LIMITER_ENABLE__
public:
	const DWORD& GetHitCount() { return dw_hit_count; }
	const DWORD& GetHitNextUpdate() { return dw_hit_next_update; }
	void SetHitCount(const DWORD& dwHitCount) { dw_hit_count = dwHitCount; }
	void SetHitNextUpdate(const DWORD& dwHitNextUpdate) { dw_hit_next_update = dwHitNextUpdate; }

	bool FindEnemyVID(const DWORD& dwVID) { return (um_enemy_vids.find(dwVID) != um_enemy_vids.end()); }
	void AppendEnemyVID(const DWORD& dwVID) { um_enemy_vids.insert(std::make_pair(dwVID, 0)); }
	void UpdateEnemyHitCount(const DWORD& dwVID, const WORD& wCount) { um_enemy_vids[dwVID] = wCount; }
	const WORD& GetEnemyVIDHitCount(const DWORD& dwVID) { return um_enemy_vids[dwVID]; }
	const WORD GetEnemyVIDCount() { return um_enemy_vids.size(); }
	void ClearEnemyVids() { return um_enemy_vids.clear(); }

private:
	DWORD dw_hit_count;
	DWORD dw_hit_next_update;
	std::unordered_map<DWORD, WORD> um_enemy_vids;
#endif

#ifdef __ABUSE_CONTROLLER_ENABLE__
	//Abuse
protected:
	bool			m_bIsSuspectWaitDmg;

private:
	spAbuseController	m_abuse;
	PIXEL_POSITION m_lastMoveAblePos;
	int32_t m_lastMoveableMapIndex;

public:
	spAbuseController	GetAbuseController() const
	{
		return m_abuse;
	}

	const PIXEL_POSITION& GetLastMoveAblePosition(int32_t index = 0)
	{
		return m_lastMoveAblePos;
	}

	void SetLastMoveAblePosition(const PIXEL_POSITION& lastPos)
	{
		m_lastMoveAblePos = lastPos;
	}

	bool IsSuspectWaitDmg() const
	{
		return m_bIsSuspectWaitDmg;
	}

	void SetSuspectWaitDmg()
	{
		m_bIsSuspectWaitDmg = true;
	}

	void SetLastMoveableMapIndex()
	{
		m_lastMoveableMapIndex = GetMapIndex();
	}

	int32_t GetLastMoveableMapIndex()
	{
		return m_lastMoveableMapIndex;
	}

public:
	LPEVENT m_pkCheckWallHackEvent;
	void StartCheckWallhack();
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
protected:
	BYTE			m_BiologActualMission;
	WORD			m_BiologCollectedItems;
	BYTE			m_BiologCooldownReminder;
	long			m_BiologCooldown;

public:
	CBiologSystem* GetBiologManager() const { return m_pkBiologManager; }

	BYTE			GetBiologMissions() const { return m_BiologActualMission; }
	WORD			GetBiologCollectedItems() const { return m_BiologCollectedItems; }
	BYTE			GetBiologCooldownReminder() const { return m_BiologCooldownReminder; }
	long			GetBiologCooldown() const { return m_BiologCooldown; }

	void			SetBiologMissions(BYTE value) { m_BiologActualMission = value; }
	void			SetBiologCollectedItems(WORD value) { m_BiologCollectedItems = value; }
	void			SetBiologCooldownReminder(BYTE value) { m_BiologCooldownReminder = value; }
	void			SetBiologCooldown(long value) { m_BiologCooldown = value; }

private:
	CBiologSystem* m_pkBiologManager;
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
protected:
	DWORD			m_MarbleActiveMission;
	BYTE			m_MarbleActiveExtandedCount;
	WORD			m_MarbleKilledMonsters;

public:
	DWORD			GetMarbleActiveMission() const { return m_MarbleActiveMission; }
	BYTE			GetMarbleActiveExtandedCount() const { return m_MarbleActiveExtandedCount; }
	WORD			GetMarbleKilledMonsters() const { return m_MarbleKilledMonsters; }

	void			SetMarbleActiveMission(DWORD value) { m_MarbleActiveMission = value; }
	void			SetMarbleActiveExtandedCount(BYTE value) { m_MarbleActiveExtandedCount = value; }
	void			SetMarbleKilledMonsters(WORD value) { m_MarbleKilledMonsters = value; }
#endif

#ifdef __TRANSMUTATION_SYSTEM__
public:
	bool			GetTransmutationItems() const;
	TransMutation_Struct* GetTransmutationItem(BYTE nr) const;
	void			AddTransmutationItem(BYTE nr, DWORD item_id, WORD pos);
	void			RemoveTransmutationItem(BYTE nr);
	bool			FindTransMutationItem(DWORD item_id);
	void			ClearTransmutation();

private:
	TransMutation_Struct* transmutation_items;
#endif

#ifdef __ENABLE_CUBE_RENEWAL__
public:
	bool			IsCubeOpen() const { return m_bIsCubeOpen; }
	void			SetCubeOpen(bool bFlag) { m_bIsCubeOpen = bFlag; }
protected:
	bool			m_bIsCubeOpen;
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
public:
	void 			FindLettersEventSendInfo();
	void 			FindLettersAddLetter(int iLetterIndex, int iInventoryIndex);
protected:
	TPlayerLetterSlot* m_letterSlots;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
public:
	BYTE			GetRefineElementType();

	void 			ClearRefineElement();
	bool 			DoRefineElement(BYTE bArg);
	void 			SendRefineElementPacket(WORD wSrcCell, WORD wDstCell, BYTE bType);
	bool 			RefineElementInformation(WORD wSrcCell, WORD wDstCell, BYTE bType);
private:
	short			m_sRefineElementSrcCell;
	short			m_sRefineElementDstCell;
	char 			m_cRefineElementType;
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
public:
	bool 			DoRefineItemSoul(LPITEM item);
	int 			GetSoulItemDamage(LPCHARACTER pkVictim, int iDamage, BYTE bSoulType);
#endif

#ifdef __ENABLE_LUCKY_BOX__
public:
	void 			LuckyBox(BYTE bAction);

protected:
	DWORD			dwLuckyBoxVnum;
	BYTE			bLuckyBoxRewardIndex;
	BYTE			bLuckyBoxOpenCount;
#ifdef __ENABLE_LUCKY_BOX__
	int64_t			dwLuckyBoxPrice;
#else
	DWORD			dwLuckyBoxPrice;
#endif
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
public:
	void		SetIsShowTeamler(bool bIsShowTeamler);
	bool		IsShowTeamler() const { return m_bIsShowTeamler; }
	void		ShowTeamlerPacket();
	void		GlobalShowTeamlerPacket(bool bIsOnline);

private:
	bool		m_bIsShowTeamler;
#endif

#ifdef __INVENTORY_BUFFERING__
public:
	void SetInventoryBuffer(bool bMode) { bInvBuff = bMode; }
	bool IsInvBuffOn() { return bInvBuff; }
	void AddItemToInvBuff(LPITEM item) { us_buffered_items.insert(item); }
	void RemoveItemFromInvBuff(LPITEM item) { us_buffered_items.erase(item); }
	void SendBufferedInventoryPacket();
	void QuickOpenStack(LPITEM item, CountType iCount);
	void AddInventoryBufferLog(const std::string & sName, DWORD dwVnum, CountType count);

private:
	std::unordered_set<LPITEM> us_buffered_items;
	std::map<DWORD, std::pair<std::string, int>> us_buffered_items_logs;
	bool bInvBuff;
#endif

#ifdef __DUNGEON_RETURN_ENABLE__
public:
	bool isDungeonRejoinWaiting() { return bWaitingForDungeonReponse; }
	void setDungeonRejoinWaiting(bool bResponse) { bWaitingForDungeonReponse = bResponse; }
	void SetDungeonReturn(bool bCanReturn) { bCanReturnToDungeon = bCanReturn; }
	bool CanReturnToDungeon() { return bCanReturnToDungeon; }

private:
	bool bWaitingForDungeonReponse;
	bool bCanReturnToDungeon;
#endif

public:
	time_t	GetNextPickupTime() { return tt_next_pickup_time; }
	void	PickupClosesItems();
	void	UpdateNextPickupTime();

private:
	time_t tt_next_pickup_time;

#ifdef __ENABLE_PARTY_ATLAS__
public:
	void	SendPartyPositionInfo(LPCHARACTER pkTarget = NULL, bool bClear = false);
	void	ClearPartyPositionInfo();

private:
	DWORD	m_dwLastPartyPositionSent;
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
private:
	uint32_t keepingOfflineShop_;

	std::set<uint32_t> viewingOfflineShops_;

	bool isOpeningOfflineShop_;
	CItem* offlineShopOpeningItem_;

public:
	void SetKeepingOfflineShop(uint32_t keepingOfflineShop);
	uint32_t GetKeepingOfflineShop() const;

	void AddViewingOfflineShop(uint32_t id);
	void RemoveViewingOfflineShop(uint32_t id);
	void RemoveFromViewingOfflineShops();

	bool UseItemOpenOfflineShop(CItem* item);
	void SetOpeningOfflineShopState(bool isOpeningOfflineShop);
	bool IsOpeningOfflineShop() const;
	void SetOfflineShopOpeningItem(CItem* item);
	CItem* GetOfflineShopOpeningItem() const;
#endif

#ifdef __PET_SYSTEM__
private:
	CPetActor* m_petActor;
public:
	void SetPet(CPetActor* pet) { m_petActor = pet; }
	CPetActor* GetPet() const { return m_petActor; }
	bool IsPetStandard() const {
		return m_petActor != nullptr;
	}

	bool IsPet() { return m_petActor; }
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
public:
	void			SetStateHorse();

public:
	void			SetMountStartToggleItem(LPITEM item);
	void			CheckMountStartToggleItem();

	// Overlays
	void			GetSpecialMount(DWORD& dwVnum, bool bTakeOut = false);
	void			RefreshMount(bool bTakeOut = false);

	void			ComputeMountPoints();

	void			MarkAsMount() { m_bCharType = CHAR_TYPE_MOUNT; };

private:
	DWORD	m_dwMountToggleItemStartID;

public:
	template<typename T>
	using deleted_unique_ptr = std::unique_ptr<T, void(*)(T*)>;
private:
	deleted_unique_ptr < CHARACTER > m_toggleMount;

public:
	CHARACTER* CreateToggleMount(CItem* item);
	void DestroyToggleMount()
	{
		m_toggleMount.reset();
	}
#endif

private:
	bool	m_bIsDisconnecting;

public:
	void SetSkipUpdatePacket(bool skip);
	bool IsSkipUpdatePacket();

protected:
	bool m_skipUpdatePacket;

#ifdef __ACTION_RESTRICTIONS__
public:
	CActionRestricts* GetActionRestrictions() const { return m_pkRestrictions; }
protected:
	CActionRestricts* m_pkRestrictions;
#endif

public:
	std::string GetPlayerLink();

#ifdef __ITEM_SHOP_ENABLE__
public:
	int		GetItemShopTime() const { return m_iItemShopBuyTime; }
	void	SetItemShopTime() { m_iItemShopBuyTime = thecore_pulse(); }
	int		GetVolatileCoinValue() { return i_VolatileCoinValue; }
	void	SetVolatileCoinValue(int i_VolatileCoinValue_) { i_VolatileCoinValue = i_VolatileCoinValue_; }
	void	SetItemShopEditor(bool bSel) { bItemShopEditor = bSel; }
	bool	IsItemShopEditor() { return (GetGMLevel() == GM_IMPLEMENTOR && bItemShopEditor); }

private:
	int	m_iItemShopBuyTime;
	int i_VolatileCoinValue;
	bool bItemShopEditor;
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
public:
	void SetTombolaItem(TombolaEvent::STombolaItem * pTombolaItem);
	const TombolaEvent::STombolaItem * GetTombolaItem();
	void FlushTombolaItem();

private:
	TombolaEvent::STombolaItem * pTombolaReward;
#endif

#ifdef __BATTLE_PASS_ENABLE__
	public:
		void BattlePassActivation(LPITEM item);
#endif

#ifdef __TEAM_DAMAGE_FLAG_CHECK__
public:
	void	SetTeamSendDamageFlag(bool flag) { m_bTeamSendDamageFlag = flag; }
	bool	IsTeamSendDamageFlag() const { return m_bTeamSendDamageFlag; }

private:
	bool	m_bTeamSendDamageFlag;
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
protected:
	int	m_locale;

public:
	void	SetLocale(int locale);
	int		GetLocale();
#endif

#ifdef __ENABLE_DS_SET__
public:
	void DragonSoul_HandleSetBonus();
#endif

public:
	void MarkHitReceived(uint32_t time) { m_dwLastHitReceivedTime = time; }

	// Both PC/NPC run this, executed when damaged.
	uint32_t GetLastHitReceivedTime() const { return m_dwLastHitReceivedTime; }
protected:
	uint32_t m_dwLastHitReceivedTime = 0;


public:
	void RefreshEquipmentSet();

#ifdef __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
	public:
		int	UnlockEquipmentSlot(DWORD dwVnum);
		bool EquipmentSlotStatus(BYTE bSlot);
		void BroadcastEquipmentLockedSlot();
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	public:
		CShamanSystem* GetShamanSystem() { return m_shamanSystem; }

		void SetAutoShaman(CShamanActor* pShaman) { m_shamanActor = pShaman; }
		bool IsAutoShaman() { return m_shamanActor; }

		void SendAutoShamanSkill(DWORD dwSkillVnum, BYTE byLevel);
		void SendAutoShamanInformations();

	protected:
		CShamanSystem* m_shamanSystem;
		CShamanActor* m_shamanActor;
#endif
};

ESex GET_SEX(LPCHARACTER ch);

#endif
