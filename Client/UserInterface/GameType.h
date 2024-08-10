#pragma once
#include "../GameLib/ItemData.h"

struct SAffects
{
	enum
	{
		AFFECT_MAX_NUM = 32,
	};

	SAffects() : dwAffects(0) {}
	SAffects(const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}
	int operator = (const DWORD & c_rAffects)
	{
		__SetAffects(c_rAffects);
	}

	BOOL IsAffect(BYTE byIndex)
	{
		return dwAffects & (1 << byIndex);
	}

	void __SetAffects(const DWORD & c_rAffects)
	{
		dwAffects = c_rAffects;
	}

	DWORD dwAffects;
};

extern std::string g_strGuildSymbolPathName;

const DWORD c_Name_Max_Length = 64;
const DWORD c_FileName_Max_Length = 128;
const DWORD c_Short_Name_Max_Length = 32;

const DWORD c_Inventory_Page_Column = 5;
const DWORD c_Inventory_Page_Row = 9;
const DWORD c_Inventory_Page_Size = c_Inventory_Page_Column * c_Inventory_Page_Row; // x*y
#ifdef ENABLE_EXTEND_INVEN_SYSTEM
	const DWORD c_Inventory_Page_Count = 4;
#else
	const DWORD c_Inventory_Page_Count = 2;
#endif
const DWORD c_ItemSlot_Count = c_Inventory_Page_Size * c_Inventory_Page_Count;
const DWORD c_Equipment_Count = 12;

const DWORD c_Equipment_Start = c_ItemSlot_Count;

const DWORD c_Equipment_Body	= c_Equipment_Start + 0;
const DWORD c_Equipment_Head	= c_Equipment_Start + 1;
const DWORD c_Equipment_Shoes	= c_Equipment_Start + 2;
const DWORD c_Equipment_Wrist	= c_Equipment_Start + 3;
const DWORD c_Equipment_Weapon	= c_Equipment_Start + 4;
const DWORD c_Equipment_Neck	= c_Equipment_Start + 5;
const DWORD c_Equipment_Ear		= c_Equipment_Start + 6;
const DWORD c_Equipment_Unique1	= c_Equipment_Start + 7;
const DWORD c_Equipment_Unique2	= c_Equipment_Start + 8;
const DWORD c_Equipment_Arrow	= c_Equipment_Start + 9;
const DWORD c_Equipment_Shield	= c_Equipment_Start + 10;

#ifdef ENABLE_COSTUME_SYSTEM
	const DWORD c_Costume_Slot_Start = c_Equipment_Start + 19;
	const DWORD	c_Costume_Slot_Body = c_Costume_Slot_Start + CItemData::COSTUME_BODY;
	const DWORD	c_Costume_Slot_Hair = c_Costume_Slot_Start + CItemData::COSTUME_HAIR;

#ifdef ENABLE_WEAPON_COSTUME_SYSTEM
	const DWORD	c_Costume_Slot_Weapon = c_Costume_Slot_Start + CItemData::COSTUME_WEAPON;
#endif

#ifdef ENABLE_SASH_COSTUME_SYSTEM
const DWORD	c_Costume_Slot_Sash = c_Costume_Slot_Start + CItemData::COSTUME_SASH;
	#endif

#ifdef ENABLE_ALTERNATIVE_MOUNT_SYSTEM
	const DWORD	c_Costume_Slot_Mount = c_Costume_Slot_Start + CItemData::COSTUME_MOUNT;
#endif

#ifdef ENABLE_PET_SYSTEM
	const DWORD c_Costume_Slot_Pet = c_Costume_Slot_Start + CItemData::COSTUME_PET;
#endif

	const DWORD c_Costume_Slot_Count = CItemData::COSTUME_NUM_TYPES;

	const DWORD c_Costume_Slot_End = c_Costume_Slot_Start + c_Costume_Slot_Count;
#endif

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	const DWORD c_New_Equipment_Start = c_Costume_Slot_End;
	const DWORD c_Equipment_Unique3 = c_New_Equipment_Start + 0;
	const DWORD c_Equipment_Unique4 = c_New_Equipment_Start + 1;

	const DWORD c_Equipment_Belt = c_New_Equipment_Start + 2;

	#ifdef ENABLE_12ZI_ELEMENT_SLOT
		const DWORD c_Equipment_Pendant = c_New_Equipment_Start + 3;
	#endif

	#ifdef ENABLE_AMULET_SYSTEM
		const DWORD c_Equipment_Amulet = c_New_Equipment_Start + 4;
	#endif

	const DWORD c_New_Equipment_Count = 5;
#endif

enum EDragonSoulDeckType
{
	DS_DECK_1,
	DS_DECK_2,
	DS_DECK_MAX_NUM = 2,
};

enum EDragonSoulGradeTypes
{
	DRAGON_SOUL_GRADE_NORMAL,
	DRAGON_SOUL_GRADE_BRILLIANT,
	DRAGON_SOUL_GRADE_RARE,
	DRAGON_SOUL_GRADE_ANCIENT,
	DRAGON_SOUL_GRADE_LEGENDARY,
	DRAGON_SOUL_GRADE_MAX,

};

enum EDragonSoulStepTypes
{
	DRAGON_SOUL_STEP_LOWEST,
	DRAGON_SOUL_STEP_LOW,
	DRAGON_SOUL_STEP_MID,
	DRAGON_SOUL_STEP_HIGH,
	DRAGON_SOUL_STEP_HIGHEST,
	DRAGON_SOUL_STEP_MAX,
};

const DWORD c_Wear_Max = 32;
const DWORD c_DragonSoul_Equip_Start = c_ItemSlot_Count + c_Wear_Max;
const DWORD c_DragonSoul_Equip_Slot_Max = 6;
const DWORD c_DragonSoul_Equip_End = c_DragonSoul_Equip_Start + c_DragonSoul_Equip_Slot_Max * DS_DECK_MAX_NUM;

const DWORD c_DragonSoul_Equip_Reserved_Count = c_DragonSoul_Equip_Slot_Max * 3;

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	const DWORD c_Belt_Inventory_Slot_Start = c_DragonSoul_Equip_End + c_DragonSoul_Equip_Reserved_Count;
	const DWORD c_Belt_Inventory_Width = 4;
	const DWORD c_Belt_Inventory_Height = 4;
	const DWORD c_Belt_Inventory_Slot_Count = c_Belt_Inventory_Width * c_Belt_Inventory_Height;
	const DWORD c_Belt_Inventory_Slot_End = c_Belt_Inventory_Slot_Start + c_Belt_Inventory_Slot_Count;

	#ifdef ENABLE_SPECIAL_STORAGE
		const DWORD c_Special_Storage_Count = 4;
		const DWORD c_Special_Storage_Page_Length = 5;
		const DWORD c_Special_Storage_Page_Breadth = 9;
		const DWORD c_Special_Storage_Page_Count = 4;

		const DWORD c_Special_Storage_Slot_Start = c_Belt_Inventory_Slot_End;
		const DWORD c_Special_Storage_Slot_End = c_Belt_Inventory_Slot_End + c_Special_Storage_Count * c_Special_Storage_Page_Length * c_Special_Storage_Page_Breadth * c_Special_Storage_Page_Count;
		const DWORD c_Inventory_Count = c_Special_Storage_Slot_End;
	#else
		const DWORD c_Inventory_Count	= c_Belt_Inventory_Slot_End;
	#endif
#else
	const DWORD c_Inventory_Count	= c_DragonSoul_Equip_End;
#endif

const DWORD c_DragonSoul_Inventory_Start = 0;
const DWORD c_DragonSoul_Inventory_Box_Size = 32;
const DWORD c_DragonSoul_Inventory_Count = CItemData::DS_SLOT_NUM_TYPES * DRAGON_SOUL_GRADE_MAX * c_DragonSoul_Inventory_Box_Size;
const DWORD c_DragonSoul_Inventory_End = c_DragonSoul_Inventory_Start + c_DragonSoul_Inventory_Count;

enum ESlotType
{
	SLOT_TYPE_NONE,
	SLOT_TYPE_INVENTORY,
	SLOT_TYPE_SKILL,
	SLOT_TYPE_EMOTION,
	SLOT_TYPE_SHOP,
	SLOT_TYPE_EXCHANGE_OWNER,
	SLOT_TYPE_EXCHANGE_TARGET,
	SLOT_TYPE_QUICK_SLOT,
	SLOT_TYPE_SAFEBOX,
	SLOT_TYPE_PRIVATE_SHOP,
	SLOT_TYPE_MALL,
	SLOT_TYPE_DRAGON_SOUL_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SLOT_TYPE_SWITCHBOT,
#endif
#ifdef ENABLE_OFFLINE_SHOP
	SLOT_TYPE_OFFLINE_SHOP,
#endif
	SLOT_TYPE_MAX,
};

#ifdef ENABLE_BIOLOG_SYSTEM
enum EBiologBonuses
{
	MAX_BONUSES_LENGTH = 3,
};
#endif

#ifdef ENABLE_FIND_LETTERS_EVENT
enum EFindLettersInfo
{
	FIND_LETTERS_SLOTS_NUM = 20,
	FIND_LETTERS_MAX_SIZE = 20,
	FIND_LETTERS_MAX_WORD_SIZE = 10,
};
#endif

enum EWindows
{
	RESERVED_WINDOW,
	INVENTORY,
	EQUIPMENT,
	SAFEBOX,
	MALL,
	DRAGON_SOUL_INVENTORY,
	BELT_INVENTORY,
#ifdef ENABLE_SWITCHBOT
	SWITCHBOT,
#endif
	GROUND,
	DUMMY_EXCHANGE,
	WINDOW_TYPE_MAX,
};

enum EDSInventoryMaxNum
{
	DS_INVENTORY_MAX_NUM = c_DragonSoul_Inventory_Count,
	DS_REFINE_WINDOW_MAX_NUM = 15,
};

#ifdef ENABLE_REFINE_ELEMENT
enum ERefineElement
{
	REFINE_ELEMENT_MAX = 3,
	ELEMENT_MIN_REFINE_LEVEL = 7,
	REFINE_ELEMENT_UPGRADE_YANG = 25000000,
	REFINE_ELEMENT_DOWNGRADE_YANG = 10000000,
	REFINE_ELEMENT_CHANGE_YANG = 50000000,

	REFINE_ELEMENT_TYPE_UPGRADE = 0,
	REFINE_ELEMENT_TYPE_DOWNGRADE = 1,
	REFINE_ELEMENT_TYPE_CHANGE = 2,
	REFINE_ELEMENT_TYPE_UPGRADE_SUCCES = 10,
	REFINE_ELEMENT_TYPE_UPGRADE_FAIL = 11,
	REFINE_ELEMENT_TYPE_DOWNGRADE_SUCCES = 12,
	REFINE_ELEMENT_TYPE_CHANGE_SUCCES = 13,

	REFINE_ELEMENT_RANDOM_VALUE_MIN = 1,
	REFINE_ELEMENT_RANDOM_VALUE_MAX = 8,
	REFINE_ELEMENT_RANDOM_BONUS_VALUE_MIN = 2,
	REFINE_ELEMENT_RANDOM_BONUS_VALUE_MAX = 12,
};
#endif

#pragma pack (push, 1)
#define WORD_MAX 0xffff

#ifdef ENABLE_SWITCHBOT
enum ESwitchbotValues
{
	SWITCHBOT_SLOT_COUNT = 5,
	SWITCHBOT_ALTERNATIVE_COUNT = 2,
	MAX_NORM_ATTR_NUM = 5,
};

enum EAttributeSet
{
	ATTRIBUTE_SET_WEAPON,
	ATTRIBUTE_SET_BODY,
	ATTRIBUTE_SET_WRIST,
	ATTRIBUTE_SET_FOOTS,
	ATTRIBUTE_SET_NECK,
	ATTRIBUTE_SET_HEAD,
	ATTRIBUTE_SET_SHIELD,
	ATTRIBUTE_SET_EAR,
	ATTRIBUTE_SET_MAX_NUM,
};

#endif

typedef struct SItemPos
{
	BYTE window_type;
	WORD cell;
	SItemPos ()
	{
		window_type =     INVENTORY;
		cell = WORD_MAX;
	}
	SItemPos (BYTE _window_type, WORD _cell)
	{
		window_type = _window_type;
		cell = _cell;
	}

	//  int operator=(const int _cell)
	//  {
	//window_type = INVENTORY;
	//      cell = _cell;
	//      return cell;
	//  }
	bool IsValidCell()
	{
		switch (window_type)
		{
		case INVENTORY:
			return cell < c_Inventory_Count;
			break;
		case EQUIPMENT:
			return cell < c_DragonSoul_Equip_End;
			break;
		case DRAGON_SOUL_INVENTORY:
			return cell < (DS_INVENTORY_MAX_NUM);
			break;
#ifdef ENABLE_SWITCHBOT
		case SWITCHBOT:
			return cell < SWITCHBOT_SLOT_COUNT;
			break;
#endif
		default:
			return false;
		}
	}
	bool IsEquipCell()
	{
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
			return (c_Equipment_Start + c_Wear_Max > cell) && (c_Equipment_Start <= cell);
			break;

		case BELT_INVENTORY:
		case DRAGON_SOUL_INVENTORY:
			return false;
			break;

		default:
			return false;
		}
	}

#ifdef ENABLE_NEW_EQUIPMENT_SYSTEM
	bool IsBeltInventoryCell()
	{
		bool bResult = c_Belt_Inventory_Slot_Start <= cell && c_Belt_Inventory_Slot_End > cell;
		return bResult;
	}
#endif

#ifdef ENABLE_SPECIAL_STORAGE
	bool IsSpecialStorageCell()
	{
		bool bResult = c_Special_Storage_Slot_Start <= cell && c_Special_Storage_Slot_End > cell;
		return bResult;
	}
#endif

	bool operator==(const struct SItemPos& rhs) const
	{
		return (window_type == rhs.window_type) && (cell == rhs.cell);
	}

	bool operator<(const struct SItemPos& rhs) const
	{
		return (window_type < rhs.window_type) || ((window_type == rhs.window_type) && (cell < rhs.cell));
	}
} TItemPos;
#pragma pack(pop)

const DWORD c_QuickBar_Line_Count = 3;
const DWORD c_QuickBar_Slot_Count = 12;

const float c_Idle_WaitTime = 5.0f;

const int c_Monster_Race_Start_Number = 6;
const int c_Monster_Model_Start_Number = 20001;

const float c_fAttack_Delay_Time = 0.2f;
const float c_fHit_Delay_Time = 0.1f;
const float c_fCrash_Wave_Time = 0.2f;
const float c_fCrash_Wave_Distance = 3.0f;

const float c_fHeight_Step_Distance = 50.0f;

enum
{
	DISTANCE_TYPE_FOUR_WAY,
	DISTANCE_TYPE_EIGHT_WAY,
	DISTANCE_TYPE_ONE_WAY,
	DISTANCE_TYPE_MAX_NUM,
};

const float c_fMagic_Script_Version = 1.0f;
const float c_fSkill_Script_Version = 1.0f;
const float c_fMagicSoundInformation_Version = 1.0f;
const float c_fBattleCommand_Script_Version = 1.0f;
const float c_fEmotionCommand_Script_Version = 1.0f;
const float c_fActive_Script_Version = 1.0f;
const float c_fPassive_Script_Version = 1.0f;

// Used by PushMove
const float c_fWalkDistance = 175.0f;
const float c_fRunDistance = 310.0f;

#define FILE_MAX_LEN 128

enum
{
	ITEM_SOCKET_SLOT_MAX_NUM = 10,
	// refactored attribute slot begin
	ITEM_ATTRIBUTE_SLOT_NORM_NUM	= 5,
	ITEM_ATTRIBUTE_SLOT_RARE_NUM	= 2,

	ITEM_ATTRIBUTE_SLOT_NORM_START	= 0,
	ITEM_ATTRIBUTE_SLOT_NORM_END	= ITEM_ATTRIBUTE_SLOT_NORM_START + ITEM_ATTRIBUTE_SLOT_NORM_NUM,

	ITEM_ATTRIBUTE_SLOT_RARE_START	= ITEM_ATTRIBUTE_SLOT_NORM_END,
	ITEM_ATTRIBUTE_SLOT_RARE_END	= ITEM_ATTRIBUTE_SLOT_RARE_START + ITEM_ATTRIBUTE_SLOT_RARE_NUM,

	ITEM_ATTRIBUTE_SLOT_MAX_NUM		= ITEM_ATTRIBUTE_SLOT_RARE_END, // 7
	// refactored attribute slot end
};

#pragma pack(push)
#pragma pack(1)

typedef struct SQuickSlot
{
	BYTE Type;
	BYTE Position;
} TQuickSlot;

typedef struct TPlayerItemAttribute
{
	BYTE        bType;
	short       sValue;
} TPlayerItemAttribute;

typedef struct packet_item
{
	DWORD       vnum;
	CountType	count;
	DWORD		flags;
	DWORD		anti_flags;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif
} TItemData;

typedef struct packet_shop_item
{
	DWORD       vnum;
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t		price;
#else
	DWORD       price;
#endif
	CountType	count;
	BYTE		display_pos;
	long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
	TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
#ifdef TRANSMUTATION_SYSTEM
	DWORD		transmutation_id;
#endif
#ifdef ENABLE_REFINE_ELEMENT
	DWORD		dwRefineElement;
#endif
} TShopItemData;

#ifdef ENABLE_CUBE_RENEWAL
enum ECubeCategoryEnum
{
	CUBE_WEAPONPVP,
	CUBE_ARMORPVM,
	CUBE_ARMORPVP,

	CUBE_ACCESSORYPVM,
	CUBE_ACCESSORYPVP,

	CUBE_SHIELDSPVM,
	CUBE_SHIELDSPVP,

	CUBE_HELMETPVM,
	CUBE_HELMETPVP,

	CUBE_BRACELETPVM,
	CUBE_BRACELETPVP,

	CUBE_NECKLACEPVM,
	CUBE_NECKLACEPVP,

	CUBE_EARRINGSPVM,
	CUBE_EARRINGSPVP,

	CUBE_SHOESPVM,
	CUBE_SHOESPVP,

	CUBE_BELTPVM,
	CUBE_BELTPVP,

	CUBE_EVENT,
	CUBE_ETC,

	CUBE_PET,
	CUBE_MOUNT,
	CUBE_SOULS,
	CUBE_SOULS_SCROLLS,
	CUBE_REFINE_SCROLLS,

	CUBE_CATEGORY_MAX,
};

enum ECubeInfoEnum
{
	CUBE_MATERIAL_MAX_NUM = 5,
};

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
#ifdef ENABLE_CUBE_RENEWAL_COPY_BONUS
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
#ifdef ENABLE_CUBE_RENEWAL_COPY_BONUS
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

#ifdef ENABLE_FIND_LETTERS_EVENT
typedef struct SPlayerLetterSlot
{
	BYTE	bAsciiChar;
	BYTE	bIsFilled;
} TPlayerLetterSlot;

typedef struct SPlayerLetterReward
{
	DWORD	dwVnum;
	BYTE	bCount;
} TPlayerLetterReward;
#endif

#ifdef ENABLE_LUCKY_BOX
enum ELuckyBox
{
	LUCKY_BOX_MAX_NUM = 16,
};

typedef struct SLuckyBoxItem
{
	DWORD	dwVnum;
	BYTE	bCount;
	bool	bIsReward;
} TLuckyBoxItem;
#endif

typedef struct packet_shop_item_ex : packet_shop_item
{
	DWORD		coin_vnum;
} TShopItemExData;

#pragma pack(pop)

inline float GetSqrtDistance(int ix1, int iy1, int ix2, int iy2) // By sqrt
{
	float dx, dy;

	dx = float(ix1 - ix2);
	dy = float(iy1 - iy2);

	return sqrtf(dx * dx + dy * dy);
}

// DEFAULT_FONT
void DefaultFont_Startup();
void DefaultFont_Cleanup();
void DefaultFont_SetName(const char * c_szFontName);
CResource* DefaultFont_GetResource();
CResource* DefaultItalicFont_GetResource();
// END_OF_DEFAULT_FONT

void SetGuildSymbolPath(const char * c_szPathName);
const char * GetGuildSymbolFileName(DWORD dwGuildID);
BYTE SlotTypeToInvenType(BYTE bSlotType);
BYTE ApplyTypeToPointType(BYTE bApplyType);

#ifdef SASH_ABSORPTION_ENABLE
enum ESashSystem
{
	SASH_TYPE_SOCKET = 7,
	SASH_ABSORPTION_SOCKET = 8,

	// Types
	SASH_TYPE_NORMAL = 0,
	SASH_TYPE_UNIQUE = 1,
	SASH_TYPE_RARE = 2,
	SASH_TYPE_LEGENDARY = 3,

	// Chances
	SASH_REFINE_CHANCE_ITEM_VALUE = 1,
	SASH_ABSORPTION_CHANCE = 100,
	SASH_UPGRADE_MAX_VALUE = 3,

	// Other
	SASH_ABSORPTION_BASE_VALUE = 5,
	SASH_ABSORPTION_LEGENDARY_MIN = 11,
	SASH_ABSORPTION_LEGENDARY_MAX = 19,
	SASH_ABSORPTION_VALUE_MAX = 25,

	SASH_FEE_BASE_VALUE = 100000LL,
	SASH_FEE_LEGENDARY_VALUE = 1000000LL,

	SASH_ABSORPTION_RELEASE_ITEM = 90000,
};
#endif
