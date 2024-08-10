#pragma once

//////////////////////////////////////////////////////////////////////////
// ### General Features ###
#define __AUTH_ULTIMATE_QUEUEING__


//#define ENABLE_QUEST_CATEGORY
#define __ENABLE_D_NJGUILD__
#define __ENABLE_FULL_NOTICE__
#define __ENABLE_NEWSTUFF__
#define __ENABLE_PORT_SECURITY__
#define __ENABLE_BELT_INVENTORY_EX__
#define __DISABLE_STOP_RIDING_WHEN_DIE__

#define __EXTENDED_ITEM_COUNT__

#define __ENABLE_EQUIP_ITEMS_WHILE_ATTACKING__

//#define __ENABLE_FULL_HP_AFTER_SPAWN__

#define __DISABLE_DROP_POTIONS_WHEN_LV_UP__

#define __ENABLE_YANG_BOMB_AUTO_LOOT__

#define __NEW_AGGREGATE_MONSTERS__

#define __ENABLE_TICKET_COINS__

#define __ENABLE_RIGHTS_CONTROLLER__

#define __ENABLE_BEGINNER_MANAGER__

#define __ENABLE_LEVEL_INFORMATION__

#define __ENABLE_BOSS_NOTIFICATION__

#define __ENABLE_STOP_CONSUMING_STAMINA__

#define __ENABLE_MULTI_LANGUAGE_SYSTEM__
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
#define __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
#define __ENABLE_MULTI_LANGUAGE_DB_LOAD_ITEM__
#endif

enum eCommonDefines
{
	MAP_ALLOW_LIMIT = 32, // 32 default

	EXPERIENCE_RING_30 = 170701,
	EXPERIENCE_RING_30_PERCENT = 25,

	EXPERIENCE_RING_60 = 170702,
	EXPERIENCE_RING_60_PERCENT = 50,

	THIEF_GLOVES_30 = 170703,
	THIEF_GLOVES_60 = 170704,
};
// ### General Features ###
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// ### CommonDefines Systems ###
//#define __ENABLE_WOLFMAN_CHARACTER__
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	#define __USE_MOB_BLEEDING_AS_POISON__
	#define __USE_MOB_CLAW_AS_DAGGER__
	// #define __USE_ITEM_BLEEDING_AS_POISON__
	// #define __USE_ITEM_CLAW_AS_DAGGER__
	#define __USE_WOLFMAN_STONES__
	#define __USE_WOLFMAN_BOOKS__
#endif

//#define __ENABLE_PLAYER_PER_ACCOUNT5__
#define __ENABLE_DICE_SYSTEM__

// #define __ENABLE_MAGIC_REDUCTION_SYSTEM__
#ifdef __ENABLE_MAGIC_REDUCTION_SYSTEM__
	// #define __USE_MAGIC_REDUCTION_STONES__
#endif

//////////////////////////////////////////////////////////////////////////
// ### Currency Relateds ###
#define __EXTANDED_GOLD_AMOUNT__
#define __GAYA_SHOP_SYSTEM__

//////////////////////////////////////////////////////////////////////////
// ### Dragon Soul Relateds ###
#define __ENABLE_DS_REFINE_ALL__
#define __ENABLE_DS_SET__

//////////////////////////////////////////////////////////////////////////
// ### Skills Relateds ###
#define __ENABLE_PASSIVE_SKILLS_HELPER__
#define __SKILL_COOLTIME_UPDATE__

#define __ENABLE_REMOVE_SKILLS_AFFECT__
#define __ENABLE_SKILLS_INFORMATION__

//////////////////////////////////////////////////////////////////////////
// ### Party Relateds ###
#define __ENABLE_IGNORE_LOWER_BUFFS__
#define __ENABLE_PARTY_BUFFS__
#define __ENABLE_PARTY_ATLAS__
#define __ENABLE_PARTY_UPDATE__

//////////////////////////////////////////////////////////////////////////
// ### Costume Relateds ###
#define __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
#define __ENABLE_WEAPON_COSTUME_SYSTEM__
#define __ENABLE_SASH_SYSTEM__
#define __SASH_ABSORPTION_ENABLE__

#define __PET_SYSTEM__

#define __ENABLE_NEW_COSTUME_BONUS__
// #define ENABLE_ITEM_ATTR_COSTUME
#define __ENABLE_HIDE_COSTUMES__

//////////////////////////////////////////////////////////////////////////
// ### Inventory & Equipment Related ###
#define __ENABLE_EXTEND_INVEN_SYSTEM__
#define __ENABLE_HIGHLIGHT_NEW_ITEM__

#define __ITEM_HIGHTLIGHT__
#define __ITEM_TOGGLE_SYSTEM__
#define __INVENTORY_BUFFERING__

#define __ENABLE_AMULET_SYSTEM__
#define __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
#define __ENABLE_SHAMAN_SYSTEM__

//////////////////////////////////////////////////////////////////////////
// ### Application management ###
#define __ADMIN_MANAGER__
#define __ENABLE_TEAMLER_STATUS__
#define __ENABLE_FIND_LOCATION__
#define __ENABLE_ADMIN_BAN_PANEL__
#define __TEAM_DAMAGE_FLAG_CHECK__

//////////////////////////////////////////////////////////////////////////
// ### Game Related ###
#define __ENABLE_OFFLINE_SHOP__
#define __SHOP_SEARCH__
#define __ENABLE_REFINE_MSG_ADD__
#define __ENABLE_INFINITE_HORSE_HEALTH_STAMINA__
#define __ENABLE_DROP_ITEM_COUNT__

#define __ENABLE_FAST_INTERACTION__
#ifdef __ENABLE_FAST_INTERACTION__
	#define __ENABLE_MULTI_USE_PACKET__
#endif

#define __ENABLE_DESTROY_ITEM_PACKET__

#define __ENABLE_TREASURE_BOX_LOOT__
#define __ENABLE_TARGET_MONSTER_LOOT__
#define __ENABLE_DELETE_SINGLE_STONE__

#define __SPECIAL_STORAGE_ENABLE__
#define __ENABLE_SWITCHBOT__

#define __ENABLE_RENEWAL_EXCHANGE__

#define __RENEWAL_DEAD_PACKET__

#define __QUEST_RENEWAL__
#ifdef __QUEST_RENEWAL__
	#define _QR_MS_
#endif

//#define __VOICE_CHAT_ENABLE__
#define __ENABLE_ANTY_EXP__

#define __ENABLE_REMOVE_POLYMORPH__

#define __ENABLE_INSTANT_EQUIP__

//#define __MULTI_LANGUAGE_SYSTEM__

#define __ENABLE_FAST_REFINE_OPTION__
#define __TECHNICAL_MAINTENANCE_ENABLE__
#define __MM_COMMAND_FEATURE__

#define __NEW_EVENT_HANDLER__
#define __EVENT_MANAGER_ENABLE__

#define __ENABLE_CHANGE_CHANNEL__
#define __ENABLE_NEW_FILTERS__

#define __OFFLINE_MESSAGE_SYSTEM__
#define __NOTIFICATON_SENDER_ENABLE__

#define __ENABLE_BIOLOG_SYSTEM__

#define __ENABLE_MARBLE_CREATOR_SYSTEM__

#define __TRANSMUTATION_SYSTEM__

#define __ENABLE_CUBE_RENEWAL__
#define __ENABLE_CUBE_RENEWAL_COPY_BONUS__

#define __ENABLE_SOUL_SYSTEM__
#define __ENABLE_LUCKY_BOX__

#define __ENABLE_SAVE_POSITION__
#define __ENABLE_TELEPORT_SYSTEM__

#define ENABLE_OFFICIAL_SELECT_GUI

#define __INGAME_WIKI__
#define __ENABLE_NEW_LOGS_CHAT__
#define __ENABLE_MULTIPLE_BUY_SYSTEM__

#define __BELT_ATTR__
#define __ENABLE_QUIVER_SYSTEM__

#define __ITEM_SHOP_ENABLE__
#define __TOMBOLA_EVENT_ENABLE__
#define __ENABLE_AUTO_NOTICE__

#define __ENABLE_ORE_REFACTOR__

#define __LEGENDARY_STONES_ENABLE__
//#define __ENABLE_PING_TIME__

#define __POPUP_SYSTEM_ENABLE__
//#define __HARDWARE_BAN__

#define __ENABLE_MISSION_MANAGER__
#define __ENABLE_MOB_TRAKCER__

//////////////////////////////////////////////////////////////////////////
// ### Elemental System ###
#define __ENABLE_REFINE_ELEMENT__
#define __ENABLE_12ZI_ELEMENT_ADD__
#define __ENABLE_12ZI_ELEMENT_SLOT__
//#define __ENABLE_12ZI_ELEMENT_SLOT_SEND_TARGET__

//////////////////////////////////////////////////////////////////////////
// ### Monster Helpers ###
#define __MONSTER_HELPER__
#define __CYCLIC_MONSTER_RESPAWNER__
#define __LEGENDARY_MONSTER_HELPER_ENABLE__
#define __REGEN_RESPAWN_ON_BLOCKED_AREA__

//////////////////////////////////////////////////////////////////////////
// ### Events ###
#define __ZUO_EVENT_SYSTEM_ENABLE__
#define __BATTLE_PASS_ENABLE__
#define __ENABLE_FIND_LETTERS_EVENT__
#define __OX_EVENT_SYSTEM_ENABLE__
#define __ENABLE_ATTENDANCE_EVENT__

//////////////////////////////////////////////////////////////////////////
// ### Dungeons ###
#define __DUNGEON_RETURN_ENABLE__
#define __ENABLE_DUNGEON_MISSION_INFORMATION__
#define __SHIP_DUNGEON_ENABLE__
#define __DUNGEON_INFO_ENABLE__

#define __CROSS_CHANNEL_DUNGEON_WARP__

// -- ** FIXES ** --
#define __ENABLE_ANTI_POLY_ON_MOUNT__
#define __ENABLE_ANTY_AUTO_LOGIN__
#define __ENABLE_BLOCK_MORE_THAN_12_CHARACTERS__
#define __ENABLE_FIX_STACK_ITEMS__
#define __ENABLE_FIX_QUEST_FLAG__
#define __ENABLE_FIX_DUNGEON_PARTY_CRASH__

#define __ENABLE_GET_ONE_ITEM_FROM_STACK__
#define __ENABLE_FIX_USING_STACK_ITEMS__

#define __ENABLE_CLEAR_OLD_GUILD_LANDS_BY_INACTIVITY__
#define __ENABLE_FIX_USING_SKILLS_WITHOUT_WEAPON__

#define __ENABLE_FIX_GIFTBOX_GROUP__
#define __ENABLE_FIX_DOUBLE_EXP_LEADER_EXPLOIT__

#define __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__

#define __ENABLE_FIX_FIND_DESC__

#define __SOCKET_CHECK_ENABLE__

#define __ENABLE_FIX_CHANGE_NAME__

//////////////////////////////////////////////////////////////////////////
// ### Anty Cheat ###
#define __ABUSE_CONTROLLER_ENABLE__
#define __HIT_TRACKER_ENABLE__
#define __HIT_LIMITER_ENABLE__
#define __CHEAT_ENGINE_FIX_ENABLE__
#define __ANTY_SKILL_FLYING_TARGET__

//////////////////////////////////////////////////////////////////////////
// ### Blocks ###
#define __ACTION_RESTRICTIONS__

/*
	***  Defines To DEBUG  ***
*/
//#define __ENABLE_SYSLOG_PACKET_SENT__

// ### CommonDefines Systems ###
//////////////////////////////////////////////////////////////////////////

#define ENABLE_RENEWAL_ARENA_PVP
