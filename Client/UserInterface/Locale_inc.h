#pragma once

#include <string>
#include <cstdint>
using Gold = int64_t;
using PointValue = int32_t;
using GuildAuthority = int64_t;
using Exp = uint32_t;
using ExchangeArg1 = int64_t;
using SocketValue = int64_t;
using CountType = uint32_t;

#define ENABLE_MULTI_LANGUAGE_SYSTEM
#define __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__

#define ENABLE_RENDER_TARGET_EFFECT

//////////////////////////////////////////////////////////////////////////
// ### Default Ymir Macros ###
#define LOCALE_SERVICE_EUROPE
#define ENABLE_COSTUME_SYSTEM
#define ENABLE_ENERGY_SYSTEM
#define ENABLE_DRAGON_SOUL_SYSTEM
// ### Default Ymir Macros ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### New From LocaleInc ###
#define ENABLE_PACK_GET_CHECK
//#define ENABLE_CANSEEHIDDENTHING_FOR_GM
#define ENABLE_PROTOSTRUCT_AUTODETECT

//#define ENABLE_PLAYER_PER_ACCOUNT5
#define ENABLE_DICE_SYSTEM
#define ENABLE_LVL115_ARMOR_EFFECT
#define ENABLE_SLOT_WINDOW_EX
// ### New From LocaleInc ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### From GameLib ###
//#define ENABLE_WOLFMAN_CHARACTER

// #define ENABLE_MAGIC_REDUCTION_SYSTEM

//////////////////////////////////////////////////////////////////////////
// ### Currency Relateds ###
#define EXTANDED_GOLD_AMOUNT

//////////////////////////////////////////////////////////////////////////
// ### Dragon Soul Relateds ###
#define ENABLE_DS_REFINE_ALL
#define ENABLE_DS_SET

//////////////////////////////////////////////////////////////////////////
// ### Party Relateds ###
#define ENABLE_PARTY_ATLAS
#define ENABLE_PARTY_UPDATE

//////////////////////////////////////////////////////////////////////////
// ### Costume Relateds ###
#define ENABLE_ALTERNATIVE_MOUNT_SYSTEM
#define ENABLE_WEAPON_COSTUME_SYSTEM
#define ENABLE_SASH_COSTUME_SYSTEM
#define SASH_ABSORPTION_ENABLE

#define ENABLE_USE_COSTUME_ATTR

#define ENABLE_NEW_COSTUME_BONUS

#define ENABLE_PET_SYSTEM
#define ENABLE_HIDE_COSTUMES

//////////////////////////////////////////////////////////////////////////
// ### Inventory & Equipment Related ###
#define ENABLE_EXTEND_INVEN_SYSTEM
#define ENABLE_HIGHLIGHT_NEW_ITEM

#define ENABLE_TOGGLE_SYSTEM
#define __INVENTORY_BUFFERING__
#define __ITEM_PICKUP_HIGHTLIGHT__

#define ENABLE_AMULET_SYSTEM
#define ENABLE_SHAMAN_SYSTEM
#ifdef ENABLE_SHAMAN_SYSTEM
	#define SHAMAN_MOB_VNUM 34463
#endif

//////////////////////////////////////////////////////////////////////////
// ### Application Related ###
//#define ENABLE_MOUSEWHEEL_EVENT
#define ENABLE_MOUSE_WHEEL_EVENT

#define ENABLE_EMOJI_SYSTEM

#define WJ_SHOW_MOB_INFO
#ifdef WJ_SHOW_MOB_INFO
	#define ENABLE_SHOW_MOBAIFLAG
	#define ENABLE_SHOW_MOBLEVEL
#endif

#define ENABLE_RT_EXTENSION

#define ENABLE_TEXT_LEVEL_REFRESH

#define ENABLE_CPP_PSM

#define ENABLE_RENEWAL_SHOP_SELLING

#define ENABLE_RENEWAL_WHISPER

#define ENABLE_LOGIN_COUNT_DOWN_UI_MODIFY
#define ENABLE_SERVER_SELECT_RENEWAL
#define ENABLE_CHANNEL_LIST

#define ENABLE_DETAILS_UI
#define ENABLE_STONES_MINIMAP

#define ENABLE_OFFICIAL_SELECT_GUI

#define ENABLE_NEW_LOGS_CHAT

#define ENABLE_ORE_REFACTOR

//////////////////////////////////////////////////////////////////////////
// ### Application Config Related ###
#define ENABLE_CONFIG_MODULE

#define ENABLE_FOG_FIX
#define ENABLE_DOG_MODE
#define ENABLE_HIDE_EFFECTS
#define ENABLE_HIDE_OBJECTS
#define ENABLE_NEW_FILTERS
#define ENABLE_SCALING_INSTANCES

//////////////////////////////////////////////////////////////////////////
// ### Application management ###
#define ENABLE_ADMIN_MANAGER
#define ENABLE_TEAMLER_STATUS

//////////////////////////////////////////////////////////////////////////
// ### Game Related ###


#define ENABLE_OFFLINE_SHOP
#define ENABLE_SHOP_SEARCH

#define ENABLE_REFINE_MSG_ADD
#define ENABLE_NEW_EQUIPMENT_SYSTEM
#define ENABLE_DROP_ITEM_COUNT
#define ENABLE_MULTI_USE_PACKET
#define ENABLE_DESTROY_ITEM_PACKET

#define ENABLE_TREASURE_BOX_LOOT
#define ENABLE_TARGET_MONSTER_LOOT
#define ENABLE_DELETE_SINGLE_STONE

#define ENABLE_SPECIAL_STORAGE
#define ENABLE_SWITCHBOT
#define ENABLE_RENEWAL_EXCHANGE
#define ENABLE_GAYA_SYSTEM

#define ENABLE_RENEWAL_DEAD_PACKET

#define ENABLE_QUEST_RENEWAL
#define ENABLE_RENEWAL_CHARACTER_WINDOW

#define ENABLE_ANTY_EXP

#define SKILL_COOLTIME_UPDATE

//#define ENABLE_VOICE_CHAT

#define ENABLE_FAST_REFINE_OPTION

#define ENABLE_CHANGE_CHANNEL

#define OFFLINE_MESSAGE_SYSTEM

#define ENABLE_BIOLOG_SYSTEM

#define ENABLE_MARBLE_CREATOR_SYSTEM

#define TRANSMUTATION_SYSTEM

#define ENABLE_CUBE_RENEWAL
#define ENABLE_CUBE_RENEWAL_COPY_BONUS

#define ENABLE_SOUL_SYSTEM
#define ENABLE_LUCKY_BOX

#define ENABLE_PASSIVE_SKILLS_HELPER
#define ENABLE_SAVE_POSITIONS_SYSTEM

#define ENABLE_TELEPORT_SYSTEM

#define INGAME_WIKI
#ifdef INGAME_WIKI
	/*
	Only define this if you have wolfman in you server, for more
	informations reed wikipedia system guide!
	*/
	// #define INGAME_WIKI_WOLFMAN
#endif

#define ENABLE_MULTIPLE_BUY_SYSTEM

#define ENABLE_BELT_ATTR
#define ENABLE_QUIVER_SYSTEM

//#define ENABLE_PING_TIME

#define __ENABLE_BOSS_ON_ATLAS__

#define __TOMBOLA_EVENT_ENABLE__

#define __INGAME_ITEMSHOP_ENABLE__
#define __POPUP_SYSTEM_ENABLE__
//#define __HARDWARE_BAN__

//////////////////////////////////////////////////////////////////////////
// ### Elemental System ###
#define ENABLE_REFINE_ELEMENT
#define ENABLE_12ZI_ELEMENT_ADD
#define ENABLE_12ZI_ELEMENT_SLOT

//////////////////////////////////////////////////////////////////////////
// ### Events ###
#define ZUO_PANEL_ENABLE
#define BATTLE_PASS_ENABLE
#define ENABLE_FIND_LETTERS_EVENT
#define OX_EVENT_SYSTEM_ENABLE

//////////////////////////////////////////////////////////////////////////
// ### Dungeons ###
#define SHIP_DUNGEON_ENABLE
#define SHIP_DEFEND_DUNGEON_ALLIE 20434
#define ENABLE_DUNGEON_MISSION_INFORMATION
#define __DUNGEON_INFO_ENABLE__


// -- ** FIXES ** --
//#define ENABLE_NO_RECV_GAME_LIMIT
//#define ENABLE_RETRY_CONNECT
#define ENABLE_LOD_ERROR_FIX
#define ENABLE_FIX_MOBS_LAG


/*
	###		New Debugging Defines
*/
// #define ENABLE_PRINT_RECV_PACKET_DEBUG



