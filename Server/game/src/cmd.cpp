#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "locale_service.h"
#include "log.h"
#include "desc.h"

ACMD(do_user_horse_ride);
ACMD(do_user_horse_back);
ACMD(do_user_horse_feed);

ACMD(do_pcbang_update);
ACMD(do_pcbang_check);

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	ACMD(do_zuo_event_open_panel);
	ACMD(do_zuo_event_manage_status);
	ACMD(do_zuo_event_spawn_monster);
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	ACMD(do_respawn_monster);
#endif

#ifdef __HIT_TRACKER_ENABLE__
	ACMD(do_hit_record_test);
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	ACMD(do_notification_open_interface);
	ACMD(do_notification_sender_request_list);
	ACMD(do_notification_sender_send);
	ACMD(do_notification_answer);
#endif

// ADD_COMMAND_SLOW_STUN
ACMD(do_slow);
ACMD(do_stun);
// END_OF_ADD_COMMAND_SLOW_STUN

ACMD(do_warp);
ACMD(do_goto);
ACMD(do_item);
ACMD(do_mob);
ACMD(do_mob_ld);
ACMD(do_mob_aggresive);
ACMD(do_mob_coward);
ACMD(do_mob_map);
ACMD(do_purge);
ACMD(do_weaken);
ACMD(do_item_purge);
ACMD(do_state);
ACMD(do_notice);
ACMD(do_map_notice);
ACMD(do_big_notice);

#ifdef __ENABLE_FULL_NOTICE__
	ACMD(do_notice_test);
	ACMD(do_big_notice_test);
	ACMD(do_map_big_notice);
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	ACMD(do_event_manager_open);
	ACMD(do_event_manager_update);
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	ACMD(do_technical_maintenance_open_panel);
	ACMD(do_technical_maintenance_add);
	ACMD(do_technical_maintenance_delay);
	ACMD(do_technical_maintenance_cancel);
#endif

ACMD(do_who);
ACMD(do_user);
ACMD(do_disconnect);
ACMD(do_kill);
ACMD(do_emotion_allow);
ACMD(do_emotion);
ACMD(do_transfer);
ACMD(do_set);
ACMD(do_cmd);
ACMD(do_reset);
ACMD(do_greset);
ACMD(do_mount);
ACMD(do_fishing);
ACMD(do_refine_rod);

// REFINE_PICK
ACMD(do_max_pick);
ACMD(do_refine_pick);
// END_OF_REFINE_PICK

ACMD(do_fishing_simul);
ACMD(do_console);
ACMD(do_restart);
ACMD(do_advance);
ACMD(do_stat);
ACMD(do_respawn);
ACMD(do_skillup);
ACMD(do_guildskillup);
ACMD(do_pvp);
ACMD(do_point_reset);
ACMD(do_safebox_size);
ACMD(do_safebox_close);
ACMD(do_safebox_password);
ACMD(do_safebox_change_password);
ACMD(do_mall_password);
ACMD(do_mall_close);
ACMD(do_ungroup);
ACMD(do_makeguild);
ACMD(do_deleteguild);
ACMD(do_shutdown);
ACMD(do_group);
ACMD(do_group_random);
ACMD(do_invisibility);
ACMD(do_event_flag);
ACMD(do_get_event_flag);
ACMD(do_private);
ACMD(do_qf);
ACMD(do_clear_quest);
ACMD(do_book);
ACMD(do_reload);
ACMD(do_war);
ACMD(do_nowar);
ACMD(do_setskill);
ACMD(do_setskillother);
ACMD(do_level);
ACMD(do_polymorph);
ACMD(do_polymorph_item);
/*
   ACMD(do_b1);
   ACMD(do_b2);
   ACMD(do_b3);
   ACMD(do_b4);
   ACMD(do_b5);
   ACMD(do_b6);
   ACMD(do_b7);
 */
ACMD(do_close_shop);
ACMD(do_set_walk_mode);
ACMD(do_set_run_mode);
ACMD(do_set_skill_group);
ACMD(do_set_skill_point);
ACMD(do_cooltime);
ACMD(do_detaillog);
ACMD(do_monsterlog);

ACMD(do_gwlist);
ACMD(do_stop_guild_war);
ACMD(do_cancel_guild_war);
ACMD(do_guild_state);

ACMD(do_pkmode);
ACMD(do_mobile);
ACMD(do_mobile_auth);
ACMD(do_messenger_auth);

ACMD(do_getqf);
ACMD(do_setqf);
ACMD(do_delqf);
ACMD(do_set_state);

ACMD(do_forgetme);
ACMD(do_aggregate);
ACMD(do_attract_ranger);
ACMD(do_pull_monster);
ACMD(do_setblockmode);
ACMD(do_priv_empire);
ACMD(do_priv_guild);
ACMD(do_mount_test);
ACMD(do_unmount);
ACMD(do_observer);
ACMD(do_observer_exit);
ACMD(do_socket_item);
ACMD(do_xmas);
ACMD(do_stat_minus);
ACMD(do_stat_reset);
ACMD(do_view_equip);
ACMD(do_block_chat);
ACMD(do_vote_block_chat);

// BLOCK_CHAT
ACMD(do_block_chat_list);
// END_OF_BLOCK_CHAT

ACMD(do_party_request);
ACMD(do_party_request_deny);
ACMD(do_party_request_accept);
ACMD(do_build);
ACMD(do_clear_land);

ACMD(do_horse_state);
ACMD(do_horse_level);
ACMD(do_horse_ride);
ACMD(do_horse_summon);
ACMD(do_horse_unsummon);
ACMD(do_horse_set_stat);

ACMD(do_save_attribute_to_image);
ACMD(do_affect_remove);

ACMD(do_change_attr);
ACMD(do_add_attr);
ACMD(do_add_socket);

ACMD(do_inputall)
{
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please enter the Order in full length."));
}

ACMD(do_show_arena_list);
ACMD(do_end_all_duel);
ACMD(do_end_duel);
ACMD(do_duel);

ACMD(do_stat_plus_amount);

ACMD(do_break_marriage);

ACMD(do_oxevent_show_quiz);
ACMD(do_oxevent_log);
ACMD(do_oxevent_get_attender);

ACMD(do_effect);
ACMD(do_threeway_war_info );
ACMD(do_threeway_war_myinfo );


ACMD(do_hair);
//gift notify quest command
ACMD(do_gift);

ACMD(do_inventory);

ACMD(do_siege);
ACMD(do_frog);

ACMD(do_reset_subskill );
ACMD(do_flush);

ACMD(do_eclipse);
ACMD(do_weeklyevent);

ACMD(do_event_helper);

ACMD(do_in_game_mall);

ACMD(do_get_mob_count);

ACMD(do_dice);
ACMD(do_special_item);

ACMD(do_click_mall);

ACMD(do_ride);
ACMD(do_get_item_id_list);
ACMD(do_set_socket);

ACMD(do_costume);
ACMD(do_set_stat);


ACMD (do_can_dead);

ACMD (do_full_set);

ACMD (do_item_full_set);

ACMD (do_attr_full_set);

ACMD (do_all_skill_master);

ACMD (do_use_item);
ACMD (do_dragon_soul);
ACMD (do_ds_list);
ACMD (do_clear_affect);

#ifdef __ENABLE_NEWSTUFF__
	ACMD(do_change_rare_attr);
	ACMD(do_add_rare_attr);

	ACMD(do_click_safebox);
	ACMD(do_force_logout);

	ACMD(do_poison);
	ACMD(do_rewarp);
#endif
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	ACMD(do_bleeding);
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
	ACMD(do_transfer_to_special_storage);
	ACMD(do_sort_inventory);
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	ACMD(do_open_gaya_shop);
	ACMD(do_craft_gaya_item);
	ACMD(do_purchase_gaya_item);
	ACMD(do_unlock_gaya_slot);
	ACMD(do_request_gaya_rotation);
#endif

#ifdef __ENABLE_ANTY_EXP__
	ACMD(do_block_exp);
#endif

#ifdef __ENABLE_REMOVE_SKILLS_AFFECT__
	ACMD(do_remove_affect_player);
#endif

#ifdef __ENABLE_REMOVE_POLYMORPH__
	ACMD(do_remove_polymorph);
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
	ACMD(do_change_channel);
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
	ACMD(do_user_costume_option);
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	ACMD(do_rights_controller_give);
	ACMD(do_rights_controller_remove);
#endif

#ifdef __BATTLE_PASS_ENABLE__
	ACMD(do_battle_pass_test);
	ACMD(do_battle_pass_collect_reward);
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	ACMD(do_transmutate_item);
	ACMD(do_transmutation_add);
	ACMD(do_transmutation_delete);
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	ACMD(do_find_letters);
	ACMD(do_find_letters_add);
	ACMD(do_find_letters_request);
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	ACMD(do_set_is_show_teamler);
#endif

#ifdef __OX_EVENT_SYSTEM_ENABLE__
	ACMD(do_ox_menu);
	ACMD(do_ox_start_event);
	ACMD(do_ox_close_event);
	ACMD(do_ox_cancel_event);
	ACMD(do_ox_question);
	ACMD(do_ox_give_reward);
	ACMD(do_ox_print_pools);
	ACMD(do_ox_add_question_to_pool);
	ACMD(do_ox_reset_pool);
#endif

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
	ACMD(do_passive_system_req_data);
#endif

#ifdef __ENABLE_SAVE_POSITION__
	ACMD(do_positions_action);
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	ACMD(do_teleport_open);
	ACMD(do_teleport_action);
#endif

#ifdef __INVENTORY_BUFFERING__
	ACMD(do_quick_open);
#endif

ACMD(do_items_around);

#ifdef __ENABLE_FIND_LOCATION__
	ACMD(do_get_player_location);
	ACMD(do_find_monster);
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	ACMD(do_cancel_opening_offline_shop);
	ACMD(do_open_offline_shop);
	ACMD(do_close_offline_shop);
	ACMD(do_close_offline_shop_force);
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	ACMD(do_skill_select);
#endif

#ifdef __ITEM_SHOP_ENABLE__
	ACMD(do_request_itemshop);
	ACMD(do_itemshop_purchase);
	ACMD(do_itemshop_editor_mode);
	ACMD(do_itemshop_amendment);
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	ACMD(do_tombola_reload);
	ACMD(do_tombola_status);
	ACMD(do_tombola_spin);
	ACMD(do_tombola_reward);
	ACMD(do_tombola_open_dialog);
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	ACMD(do_ban);
#endif

#ifdef __TEAM_DAMAGE_FLAG_CHECK__
	ACMD(do_set_show_damage_flag);
#endif

#ifdef __ENABLE_SKILLS_INFORMATION__
	ACMD(do_get_skill_information);
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	ACMD(do_legendary_stones_open_type);

	ACMD(do_legendary_stones_passive_up);

	ACMD(do_legendary_stones_craft_set);
	ACMD(do_legendary_stones_craft_run);

	ACMD(do_legendary_stones_exchange_start);
	ACMD(do_legendary_stones_exchange_cancel);
	ACMD(do_legendary_stones_exchange_set_item);
	ACMD(do_legendary_stones_exchange_run);

	ACMD(do_legendary_stones_refine_cancel);
	ACMD(do_legendary_stones_refine_set_item);
	ACMD(do_legendary_stones_refine_run);
#endif

#ifdef __DUNGEON_INFO_ENABLE__
	ACMD(do_dungeon_info_open_panel);
	ACMD(do_dungeon_info_join_dungeon);
	#ifdef __DUNGEON_RETURN_ENABLE__
		ACMD(do_dungeon_info_rejoin_dungeon);
	#endif
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	ACMD(do_open_sash_combination);
	ACMD(do_register_sash_system_combination);
	ACMD(do_process_sash_system_combination);
	ACMD(do_open_sash_absorption);
	ACMD(do_register_sash_system_absorption);
	ACMD(do_process_sash_system_absorption);
	ACMD(do_sash_system_cancel);
	ACMD(do_release_sash_absorption);
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	ACMD(do_amulet_upgrade_info);
	ACMD(do_amulet_upgrade_apply);
	ACMD(do_amulet_roll);

	// Crafting
	ACMD(do_amulet_crafting_open);
	ACMD(do_amulet_crafting_process);

	// Refine
	ACMD(do_amulet_combine_open);
	ACMD(do_amulet_combine_close);
	ACMD(do_amulet_combine_register);
	ACMD(do_amulet_combine_process);
#endif

ACMD(do_register_mark);

#ifdef __ENABLE_MISSION_MANAGER__
	ACMD(do_mission_set);
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	ACMD(do_attendance_collect);
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	ACMD(do_request_biolog);
	ACMD(do_request_biolog_sets);
	ACMD(do_request_biolog_timer);
	ACMD(do_biolog_collect);
	ACMD(do_biolog_collect_affect);
	ACMD(do_biolog_reset);
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	ACMD(do_request_tracker);
	ACMD(do_request_tracker_teleport);
#endif

struct command_info cmd_info[] =
{
	{ "!RESERVED!",	NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "who",		do_who,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "war",		do_war,			0,			POS_DEAD,	GM_PLAYER	},
	{ "warp",		do_warp,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "user",		do_user,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "notice",		do_notice,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "notice_map",	do_map_notice,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "big_notice",	do_big_notice,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
#ifdef __ENABLE_FULL_NOTICE__
	{ "big_notice_map",	do_map_big_notice,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "notice_test",	do_notice_test,		0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "big_notice_test", do_big_notice_test,	0,	POS_DEAD,	GM_HIGH_WIZARD	},
#endif
	{ "nowar",		do_nowar,		0,			POS_DEAD,	GM_PLAYER	},
	{ "purge",		do_purge,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "weaken",		do_weaken,		0,			POS_DEAD,	GM_GOD		},
	{ "dc",		do_disconnect,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "transfer",	do_transfer,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "goto",		do_goto,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "level",		do_level,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "eventflag",	do_event_flag,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "geteventflag",	do_get_event_flag,	0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "item",		do_item,		0,			POS_DEAD,	GM_GOD		},

	{ "mob",		do_mob,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mob_ld",		do_mob_ld,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "ma",		do_mob_aggresive,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mc",		do_mob_coward,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "mm",		do_mob_map,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "kill",		do_kill,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "ipurge",		do_item_purge,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "group",		do_group,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "grrandom",	do_group_random,	0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "set",		do_set,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reset",		do_reset,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "greset",		do_greset,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "advance",	do_advance,		0,			POS_DEAD,	GM_GOD		},
	{ "book",		do_book,		0,			POS_DEAD,	GM_IMPLEMENTOR  },

	{ "console",	do_console,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "shutdow",	do_inputall,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "shutdown",	do_shutdown,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "stat",		do_stat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat-",		do_stat_minus,		0,			POS_DEAD,	GM_PLAYER	},
	{ "stat_reset",	do_stat_reset,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "state",		do_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	// ADD_COMMAND_SLOW_STUN
	{ "stun",		do_stun,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "slow",		do_slow,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	// END_OF_ADD_COMMAND_SLOW_STUN

	{ "respawn",	do_respawn,		0,			POS_DEAD,	GM_WIZARD	},

	{ "makeguild",	do_makeguild,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "deleteguild",	do_deleteguild,		0,			POS_DEAD,	GM_HIGH_WIZARD	},

	{ "mount",		do_mount,		0,			POS_MOUNTING,	GM_PLAYER	},
	{ "restart_here",	do_restart,		SCMD_RESTART_HERE,	POS_DEAD,	GM_PLAYER	},
	{ "restart_town",	do_restart,		SCMD_RESTART_TOWN,	POS_DEAD,	GM_PLAYER	},
	{ "phase_selec",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "phase_select",	do_cmd,			SCMD_PHASE_SELECT,	POS_DEAD,	GM_PLAYER	},
	{ "qui",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "quit",		do_cmd,			SCMD_QUIT,		POS_DEAD,	GM_PLAYER	},
	{ "logou",		do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "logout",		do_cmd,			SCMD_LOGOUT,		POS_DEAD,	GM_PLAYER	},
	{ "skillup",	do_skillup,		0,			POS_DEAD,	GM_PLAYER	},
	{ "gskillup",	do_guildskillup,	0,			POS_DEAD,	GM_PLAYER	},
	{ "pvp",		do_pvp,			0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox",	do_safebox_size,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "safebox_close",	do_safebox_close,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_passwor", do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_password", do_safebox_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_passwor", do_inputall,	0,			POS_DEAD,	GM_PLAYER	},
	{ "safebox_change_password", do_safebox_change_password,	0,	POS_DEAD,	GM_PLAYER	},
	{ "mall_passwor",	do_inputall,		0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_password",	do_mall_password,	0,			POS_DEAD,	GM_PLAYER	},
	{ "mall_close",	do_mall_close,		0,			POS_DEAD,	GM_PLAYER	},

	// Group Command
	{ "ungroup",	do_ungroup,		0,			POS_DEAD,	GM_PLAYER	},

	// REFINE_ROD_HACK_BUG_FIX
	{ "refine_rod",	do_refine_rod,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_ROD_HACK_BUG_FIX

	// REFINE_PICK
	{ "refine_pick",	do_refine_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "max_pick",	do_max_pick,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	// END_OF_REFINE_PICK

	{ "fish_simul",	do_fishing_simul,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "invisible",	do_invisibility,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "qf",		do_qf,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "clear_quest",	do_clear_quest,		0,			POS_DEAD,	GM_HIGH_WIZARD	},

	{ "close_shop",	do_close_shop,		0,			POS_DEAD,	GM_PLAYER	},

	{ "set_walk_mode",	do_set_walk_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "set_run_mode",	do_set_run_mode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "setjob", do_set_skill_group,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "setskill",	do_setskill,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "setskillother",	do_setskillother,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "setskillpoint",  do_set_skill_point,	0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "reload",		do_reload,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "cooltime",	do_cooltime,		0,			POS_DEAD,	GM_HIGH_WIZARD	},

	{ "gwlist",		do_gwlist,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gwstop",		do_stop_guild_war,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gwcancel",	do_cancel_guild_war, 0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "gstate",		do_guild_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "pkmode",		do_pkmode,		0,			POS_DEAD,	GM_PLAYER	},
	{ "messenger_auth",	do_messenger_auth,	0,			POS_DEAD,	GM_PLAYER	},

	{ "getqf",		do_getqf,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "setqf",		do_setqf,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "delqf",		do_delqf,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "set_state",	do_set_state,		0,			POS_DEAD,	GM_LOW_WIZARD	},


	{ "detaillog",	do_detaillog,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "monsterlog",	do_monsterlog,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "forgetme",	do_forgetme,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "aggregate",	do_aggregate,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "attract_ranger",	do_attract_ranger,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "pull_monster",	do_pull_monster,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "setblockmode",	do_setblockmode,	0,			POS_DEAD,	GM_PLAYER	},
	{ "polymorph",	do_polymorph,		0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "polyitem",	do_polymorph_item,	0,			POS_DEAD,	GM_HIGH_WIZARD },
	{ "priv_empire",	do_priv_empire,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "priv_guild",	do_priv_guild,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "mount_test",	do_mount_test,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "unmount",	do_unmount,		0,			POS_DEAD,	GM_PLAYER	},
	{ "private",	do_private,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "party_request",	do_party_request,	0,			POS_DEAD,	GM_PLAYER	},
	{ "party_request_accept", do_party_request_accept, 0,		POS_DEAD,	GM_PLAYER	},
	{ "party_request_deny", do_party_request_deny, 0,			POS_DEAD,	GM_PLAYER	},
	{ "observer",	do_observer,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "observer_exit",	do_observer_exit,	0,			POS_DEAD,	GM_PLAYER	},
	{ "socketitem",	do_socket_item,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "saveati",	do_save_attribute_to_image, 0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "xmas_boom",	do_xmas,		SCMD_XMAS_BOOM,		POS_DEAD,	GM_HIGH_WIZARD	},
	{ "xmas_snow",	do_xmas,		SCMD_XMAS_SNOW,		POS_DEAD,	GM_HIGH_WIZARD	},
	{ "xmas_santa",	do_xmas,		SCMD_XMAS_SANTA,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "view_equip",	do_view_equip,		0,			POS_DEAD,	GM_PLAYER   	},
	{ "jy",				do_block_chat,		0,			POS_DEAD,	GM_HIGH_WIZARD	},

	// BLOCK_CHAT
	{ "vote_block_chat", do_vote_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat",		do_block_chat,		0,			POS_DEAD,	GM_PLAYER	},
	{ "block_chat_list", do_block_chat_list,	0,			POS_DEAD,	GM_PLAYER	},
	// END_OF_BLOCK_CHAT

	{ "build",		do_build,		0,		POS_DEAD,	GM_PLAYER	},
	{ "clear_land", do_clear_land,	0,		POS_DEAD,	GM_HIGH_WIZARD	},

	{ "affect_remove",	do_affect_remove,	0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "horse_state",	do_horse_state,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "horse_level",	do_horse_level,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "horse_ride",	do_horse_ride,		0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "horse_summon",	do_horse_summon,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "horse_unsummon",	do_horse_unsummon,	0,			POS_DEAD,	GM_HIGH_WIZARD	},
	{ "horse_set_stat", do_horse_set_stat,	0,			POS_DEAD,	GM_HIGH_WIZARD	},

	{ "pcbang_update", 	do_pcbang_update,	0,			POS_DEAD,	GM_LOW_WIZARD	},
	{ "pcbang_check", 	do_pcbang_check,	0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "emotion_allow",	do_emotion_allow,	0,			POS_FIGHTING,	GM_PLAYER	},
	{ "kiss",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "slap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "french_kiss",	do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "clap",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "cheer2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},

	// DANCE
	{ "dance1",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance2",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance3",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance4",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance5",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	{ "dance6",		do_emotion,		0,			POS_FIGHTING,	GM_PLAYER	},
	// END_OF_DANCE

	{ "congratulation",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "forgive",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "angry",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "attractive",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "sad",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "shy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "cheerup",	do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "banter",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},
	{ "joy",		do_emotion,	0,	POS_FIGHTING,	GM_PLAYER	},


	{ "change_attr",	do_change_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_attr",	do_add_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "add_socket",	do_add_socket,		0,			POS_DEAD,	GM_IMPLEMENTOR	},

	{ "user_horse_ride",	do_user_horse_ride,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_back",	do_user_horse_back,		0,		POS_FISHING,	GM_PLAYER	},
	{ "user_horse_feed",	do_user_horse_feed,		0,		POS_FISHING,	GM_PLAYER	},

	{ "show_arena_list",	do_show_arena_list,		0,		POS_DEAD,	GM_LOW_WIZARD	},
	{ "end_all_duel",		do_end_all_duel,		0,		POS_DEAD,	GM_LOW_WIZARD	},
	{ "end_duel",			do_end_duel,			0,		POS_DEAD,	GM_LOW_WIZARD	},
	{ "duel",				do_duel,				0,		POS_DEAD,	GM_LOW_WIZARD	},

	{ "con+",			do_stat_plus_amount,	POINT_HT,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "int+",			do_stat_plus_amount,	POINT_IQ,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "str+",			do_stat_plus_amount,	POINT_ST,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "dex+",			do_stat_plus_amount,	POINT_DX,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "break_marriage",	do_break_marriage,		0,			POS_DEAD,	GM_LOW_WIZARD	},

	{ "show_quiz",			do_oxevent_show_quiz,	0,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "log_oxevent",		do_oxevent_log,			0,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "get_oxevent_att",	do_oxevent_get_attender, 0,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "effect",				do_effect,				0,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "threeway_info",		do_threeway_war_info,	0,	POS_DEAD,	GM_LOW_WIZARD},
	{ "threeway_myinfo",	do_threeway_war_myinfo, 0,	POS_DEAD,	GM_LOW_WIZARD},

	{ "hair",				do_hair,				0,	POS_DEAD,	GM_PLAYER	},
	{ "inventory",			do_inventory,			0,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "siege",				do_siege,				0,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "frog",				do_frog,				0,	POS_DEAD,	GM_HIGH_WIZARD	},
	{ "reset_subskill",		do_reset_subskill,		0,	POS_DEAD,	GM_HIGH_WIZARD },
	{ "flush",				do_flush,				0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "gift",				do_gift,				0,  POS_DEAD,   GM_PLAYER	},	//gift

	{ "eclipse",			do_eclipse,				0,	POS_DEAD,	GM_HIGH_WIZARD	},

	{ "weeklyevent",		do_weeklyevent,			0,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "eventhelper",		do_event_helper,		0,	POS_DEAD,	GM_HIGH_WIZARD	},

	{ "in_game_mall",		do_in_game_mall,		0,	POS_DEAD,	GM_PLAYER	},

	{ "get_mob_count",		do_get_mob_count,		0,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "dice",				do_dice,				0,	POS_DEAD,	GM_PLAYER		},
	{ "special_item",			do_special_item,	0,	POS_DEAD,	GM_IMPLEMENTOR		},

	{ "click_mall",			do_click_mall,			0,	POS_DEAD,	GM_PLAYER		},

	{ "ride",				do_ride,				0,	POS_DEAD,	GM_PLAYER	},

	{ "item_id_list",	do_get_item_id_list,	0,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "set_socket",		do_set_socket,			0,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "costume",			do_costume, 			0,	POS_DEAD,	GM_PLAYER	},

	{ "tcon",			do_set_stat,	POINT_HT,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "tint",			do_set_stat,	POINT_IQ,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "tstr",			do_set_stat,	POINT_ST,	POS_DEAD,	GM_LOW_WIZARD	},
	{ "tdex",			do_set_stat,	POINT_DX,	POS_DEAD,	GM_LOW_WIZARD	},

	{ "cannot_dead",			do_can_dead,	1,	POS_DEAD,		GM_LOW_WIZARD},
	{ "can_dead",				do_can_dead,	0,	POS_DEAD,		GM_LOW_WIZARD},

	{ "full_set",	do_full_set, 0, POS_DEAD,		GM_LOW_WIZARD},
	{ "item_full_set",	do_item_full_set, 0, POS_DEAD,		GM_LOW_WIZARD},
	{ "attr_full_set",	do_attr_full_set, 0, POS_DEAD,		GM_LOW_WIZARD},
	{ "all_skill_master",	do_all_skill_master,	0,	POS_DEAD,	GM_LOW_WIZARD},
	{ "use_item",		do_use_item,	0, POS_DEAD,		GM_LOW_WIZARD},

	{ "dragon_soul",				do_dragon_soul,				0,	POS_DEAD,	GM_PLAYER	},
	{ "ds_list",				do_ds_list,				0,	POS_DEAD,	GM_PLAYER	},
	{ "do_clear_affect", do_clear_affect, 	0, POS_DEAD,		GM_LOW_WIZARD},

#ifdef __ENABLE_NEWSTUFF__
	//item
	{ "add_rare_attr",		do_add_rare_attr,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "change_rare_attr",	do_change_rare_attr,		0,			POS_DEAD,	GM_IMPLEMENTOR	},
	//player
	{ "click_safebox",		do_click_safebox,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "force_logout",		do_force_logout,			0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "poison",				do_poison,					0,			POS_DEAD,	GM_IMPLEMENTOR	},
	{ "rewarp",				do_rewarp,					0,			POS_DEAD,	GM_LOW_WIZARD	},
#endif

#ifdef __ENABLE_WOLFMAN_CHARACTER__
	{ "bleeding",			do_bleeding,				0,			POS_DEAD,	GM_IMPLEMENTOR	},
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
	{ "transfer_to_special_storage",	do_transfer_to_special_storage, 0, POS_DEAD, GM_PLAYER },
	{ "sort_inventory",					do_sort_inventory,				0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	{ "open_gaya_shop",					do_open_gaya_shop,				0, POS_DEAD, GM_PLAYER },
	{ "craft_gaya_item",				do_craft_gaya_item,				0, POS_DEAD, GM_PLAYER },
	{ "purchase_gaya_item",				do_purchase_gaya_item,			0, POS_DEAD, GM_PLAYER },
	{ "unlock_gaya_slot",				do_unlock_gaya_slot,			0, POS_DEAD, GM_PLAYER },
	{ "request_gaya_rotation",			do_request_gaya_rotation,		0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_ANTY_EXP__
	{ "_block_exp",						do_block_exp,					0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_REMOVE_SKILLS_AFFECT__
	{ "remove_affect",					do_remove_affect_player,		0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_REMOVE_POLYMORPH__
	{ "remove_polymorph",				do_remove_polymorph,			0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	{ "event_manager_open",				do_event_manager_open,			0, POS_DEAD, GM_IMPLEMENTOR },
	{ "event_manager_update",			do_event_manager_update,		0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	{ "tech_maint_open_panel",			do_technical_maintenance_open_panel,	0, POS_DEAD, GM_IMPLEMENTOR },
	{ "tech_maint_add",					do_technical_maintenance_add,			0, POS_DEAD, GM_IMPLEMENTOR },
	{ "tech_maint_delay",				do_technical_maintenance_delay,			0, POS_DEAD, GM_IMPLEMENTOR },
	{ "tech_maint_cancel",				do_technical_maintenance_cancel,		0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_CHANGE_CHANNEL__
	{ "channel",						do_change_channel,						0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __HIT_TRACKER_ENABLE__
	{ "hit_record_test",				do_hit_record_test,						0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
	{ "user_costume_option",			do_user_costume_option,					0,	POS_DEAD, GM_PLAYER, },
#endif

#ifdef __NOTIFICATON_SENDER_ENABLE__
	{ "notification_open_interface", do_notification_open_interface, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "notification_sender_request_list", do_notification_sender_request_list, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "notification_sender_send", do_notification_sender_send, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "notification_answer", do_notification_answer, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	{ "respawn_monster", do_respawn_monster, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
	{ "zuo_event_open_panel", do_zuo_event_open_panel, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "zuo_event_manage_status", do_zuo_event_manage_status, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "zuo_event_spawn_monster", do_zuo_event_spawn_monster, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	{ "give_rights",				do_rights_controller_give,				0,					POS_DEAD,		GM_IMPLEMENTOR },
	{ "remove_rights",				do_rights_controller_remove,			0,					POS_DEAD,		GM_IMPLEMENTOR },
#endif

#ifdef __BATTLE_PASS_ENABLE__
	{ "battle_pass_test", do_battle_pass_test, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "battle_pass_collect_reward", do_battle_pass_collect_reward, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __TRANSMUTATION_SYSTEM__
	{ "transmutate_item", do_transmutate_item, 	0, POS_DEAD,		GM_PLAYER },
	{ "transmutation_add", do_transmutation_add, 	0, POS_DEAD,		GM_PLAYER },
	{ "transmutation_delete", do_transmutation_delete, 	0, POS_DEAD,		GM_PLAYER },
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	{ "find_letters",		do_find_letters,	0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "find_letters_add",		do_find_letters_add,	0,	POS_DEAD,	GM_PLAYER },
	{ "find_letters_request",		do_find_letters_request,	0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	{ "set_is_show_teamler",		do_set_is_show_teamler,		0,			POS_DEAD,	GM_LOW_WIZARD },
#endif

#ifdef __OX_EVENT_SYSTEM_ENABLE__
	{ "ox_menu", do_ox_menu, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_start_event", do_ox_start_event, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_close_event", do_ox_close_event, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_cancel_event", do_ox_cancel_event, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_question", do_ox_question, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_give_reward", do_ox_give_reward, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_print_pools", do_ox_print_pools, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_add_question_to_pool", do_ox_add_question_to_pool, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "ox_reset_pool", do_ox_reset_pool, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_PASSIVE_SKILLS_HELPER__
	{ "passive_req_data",	do_passive_system_req_data,	0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __ENABLE_SAVE_POSITION__
	{ "positions_action",	do_positions_action,	0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __ENABLE_TELEPORT_SYSTEM__
	{ "teleport_open",		do_teleport_open,	0,	POS_DEAD,	GM_PLAYER },
	{ "teleport_action",	do_teleport_action,	0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __INVENTORY_BUFFERING__
	{ "quick_open", do_quick_open, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "items_around",	do_items_around,	0,		POS_DEAD,	GM_PLAYER },

#ifdef __ENABLE_FIND_LOCATION__
	{ "gpl", do_get_player_location, 0, POS_DEAD, GM_IMPLEMENTOR },
	{ "find_monster", do_find_monster, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	{ "cancel_opening_offline_shop", do_cancel_opening_offline_shop, 0, POS_DEAD, GM_PLAYER },
	{ "open_offline_shop", do_open_offline_shop, 0, POS_DEAD, GM_PLAYER },
	{ "close_offline_shop", do_close_offline_shop, 0, POS_DEAD, GM_PLAYER },
	{ "close_offline_shop_force", do_close_offline_shop_force, 0, POS_DEAD, GM_IMPLEMENTOR },

#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
	{ "select_skills", do_skill_select, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ITEM_SHOP_ENABLE__
	{ "request_itemshop",		do_request_itemshop,	0,	POS_DEAD,	GM_PLAYER },
	{ "itemshop_purchase",		do_itemshop_purchase,	0,	POS_DEAD,	GM_PLAYER },
	{ "itemshop_editor_mode",		do_itemshop_editor_mode,	0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "itemshop_amendment",		do_itemshop_amendment,	0,	POS_DEAD,	GM_IMPLEMENTOR },
#endif

#ifdef __TOMBOLA_EVENT_ENABLE__
	{ "tombola_reload",		do_tombola_reload,	0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "tombola_status",		do_tombola_status,	0,	POS_DEAD,	GM_IMPLEMENTOR },
	{ "tombola_spin",		do_tombola_spin,	0,	POS_DEAD,	GM_PLAYER },
	{ "tombola_reward",		do_tombola_reward,	0,	POS_DEAD,	GM_PLAYER },
	{ "tombola_open_dialog",		do_tombola_open_dialog,	0,	POS_DEAD,	GM_PLAYER },
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	{ "gm_ban", do_ban, 	0, POS_DEAD,		GM_IMPLEMENTOR },
#endif

#ifdef __TEAM_DAMAGE_FLAG_CHECK__
	{ "set_show_damage_flag", do_set_show_damage_flag, 0, POS_DEAD, GM_WIZARD },
#endif

#ifdef __ENABLE_SKILLS_INFORMATION__
	{ "skill_information_get", do_get_skill_information, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	{ "legendary_stones_open_window", do_legendary_stones_open_type, 0, POS_DEAD, GM_PLAYER },

	{ "legendary_stones_passive_up", do_legendary_stones_passive_up, 0, POS_DEAD, GM_PLAYER },

	{ "legendary_stones_craft_set", do_legendary_stones_craft_set, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_craft_run", do_legendary_stones_craft_run, 0, POS_DEAD, GM_PLAYER },

	{ "legendary_stones_exchange_start", do_legendary_stones_exchange_start, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_exchange_cancel", do_legendary_stones_exchange_cancel, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_exchange_set", do_legendary_stones_exchange_set_item, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_exchange_run", do_legendary_stones_exchange_run, 0, POS_DEAD, GM_PLAYER },

	{ "legendary_stones_refine_cancel", do_legendary_stones_refine_cancel, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_refine_set", do_legendary_stones_refine_set_item, 0, POS_DEAD, GM_PLAYER },
	{ "legendary_stones_refine_run", do_legendary_stones_refine_run, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __DUNGEON_INFO_ENABLE__
	{ "dungeon_info_open_panel",	do_dungeon_info_open_panel,	0,	POS_DEAD,	GM_PLAYER},
	{ "dungeon_info_join_dungeon",	do_dungeon_info_join_dungeon,	0,	POS_DEAD,	GM_PLAYER},
#ifdef __DUNGEON_RETURN_ENABLE__
	{ "dungeon_info_rejoin_dungeon",	do_dungeon_info_rejoin_dungeon,	0,	POS_DEAD,	GM_PLAYER },
#endif
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	{ "open_sash_combination", do_open_sash_combination, 0, POS_DEAD, GM_PLAYER },
	{ "sash_register_system_combination", do_register_sash_system_combination, 0, POS_DEAD, GM_PLAYER },
	{ "process_sash_system_combination", do_process_sash_system_combination, 0, POS_DEAD, GM_PLAYER },
	{ "open_sash_absorption", do_open_sash_absorption, 0, POS_DEAD, GM_PLAYER },
	{ "sash_register_system_absorption", do_register_sash_system_absorption, 0, POS_DEAD, GM_PLAYER },
	{ "process_sash_system_absorption", do_process_sash_system_absorption, 0, POS_DEAD, GM_PLAYER },
	{ "sash_system_cancel", do_sash_system_cancel, 0, POS_DEAD, GM_PLAYER },
	{ "release_sash_absorption", do_release_sash_absorption, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	{ "amulet_upgrade_info",	do_amulet_upgrade_info, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_upgrade_apply", do_amulet_upgrade_apply, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_roll", do_amulet_roll, 0, POS_DEAD, GM_PLAYER },

	// Crafting
	{ "amulet_crafting_open", do_amulet_crafting_open, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_crafting_process", do_amulet_crafting_process, 0, POS_DEAD, GM_PLAYER },

	// Combine
	{ "amulet_combine_open", do_amulet_combine_open, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_combine_close", do_amulet_combine_close, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_combine_register", do_amulet_combine_register, 0, POS_DEAD, GM_PLAYER },
	{ "amulet_combine_process", do_amulet_combine_process, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "register_mark", do_register_mark, 0, POS_DEAD, GM_PLAYER },

#ifdef __ENABLE_MISSION_MANAGER__
	{ "mission_set", do_mission_set, 0, POS_DEAD, GM_IMPLEMENTOR },
#endif

#ifdef __ENABLE_ATTENDANCE_EVENT__
	{ "attendance_collect", do_attendance_collect, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
	{ "request_biolog", do_request_biolog, 0, POS_DEAD, GM_PLAYER },
	{ "request_biolog_sets", do_request_biolog_sets, 0, POS_DEAD, GM_PLAYER },
	{ "request_biolog_timer", do_request_biolog_timer, 0, POS_DEAD, GM_PLAYER },
	{ "request_biolog_collect", do_biolog_collect, 0, POS_DEAD, GM_PLAYER },
	{ "request_biolog_collect_affect", do_biolog_collect_affect, 0, POS_DEAD, GM_PLAYER },
	{ "request_biolog_reset", do_biolog_reset, 0, POS_DEAD, GM_PLAYER },
#endif

#ifdef __ENABLE_MOB_TRAKCER__
	{ "request_tracker", do_request_tracker, 0, POS_DEAD, GM_PLAYER },
	{ "request_tracker_teleport", do_request_tracker_teleport, 0, POS_DEAD, GM_PLAYER },
#endif

	{ "\n",		NULL,			0,			POS_DEAD,	GM_IMPLEMENTOR	}
};

void interpreter_set_privilege(const char *cmd, int lvl)
{
	int i;

	for (i = 0; *cmd_info[i].command != '\n'; ++i)
	{
		if (!str_cmp(cmd, cmd_info[i].command))
		{
			cmd_info[i].gm_level = lvl;
			sys_log(0, "Setting command privilege: %s -> %d", cmd, lvl);
			break;
		}
	}
}

void double_dollar(const char *src, size_t src_len, char *dest, size_t dest_len)
{
	const char * tmp = src;
	size_t cur_len = 0;


	dest_len -= 1;

	while (src_len-- && *tmp)
	{
		if (*tmp == '$')
		{
			if (cur_len + 1 >= dest_len)
			{
				break;
			}

			*(dest++) = '$';
			*(dest++) = *(tmp++);
			cur_len += 2;
		}
		else
		{
			if (cur_len >= dest_len)
			{
				break;
			}

			*(dest++) = *(tmp++);
			cur_len += 1;
		}
	}

	*dest = '\0';
}
// #define ENABLE_BLOCK_CMD_SHORTCUT
void interpret_command(LPCHARACTER ch, const char * argument, size_t len)
{
#ifdef ENABLE_ANTI_CMD_FLOOD
	if (ch && !ch->IsGM())
	{
		if (thecore_pulse() > ch->GetCmdAntiFloodPulse() + PASSES_PER_SEC(1))
		{
			ch->SetCmdAntiFloodCount(0);
			ch->SetCmdAntiFloodPulse(thecore_pulse());
		}
		if (ch->IncreaseCmdAntiFloodCount() >= 10)
		{
			ch->GetDesc()->DelayedDisconnect(0);
			return;
		}
	}
#endif
	if (NULL == ch)
	{
		sys_err ("NULL CHRACTER");
		return ;
	}

	char cmd[128 + 1];
	char new_line[256 + 1];
	const char * line;
	int icmd;

	if (len == 0 || !*argument)
	{
		return;
	}

	double_dollar(argument, len, new_line, sizeof(new_line));

	size_t cmdlen;
	line = first_cmd(new_line, cmd, sizeof(cmd), &cmdlen);

	for (icmd = 1; *cmd_info[icmd].command != '\n'; ++icmd)
	{
		if (cmd_info[icmd].command_pointer == do_cmd)
		{
			if (!strcmp(cmd_info[icmd].command, cmd))
			{
				break;
			}
		}
#ifdef ENABLE_BLOCK_CMD_SHORTCUT
		else if (!strcmp(cmd_info[icmd].command, cmd))
#else
		else if (!strncmp(cmd_info[icmd].command, cmd, cmdlen))
#endif
			break;
	}

	if (ch->GetPosition() < cmd_info[icmd].minimum_position)
	{
		switch (ch->GetPosition())
		{
		case POS_MOUNTING:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do this whilst sitting on a Horse."));
			break;

		case POS_DEAD:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot do that while you are lying on the ground."));
			break;

		case POS_SLEEPING:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("In my Dreams? What?"));
			break;

		case POS_RESTING:
		case POS_SITTING:
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Get up first."));
			break;

		default:
			sys_err("unknown position %d", ch->GetPosition());
			break;
		}

		return;
	}

	if (*cmd_info[icmd].command == '\n')
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}

#ifdef __ADMIN_MANAGER__
	if (cmd_info[icmd].gm_level && (cmd_info[icmd].gm_level > ch->GetGMLevel()))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}
#else
	if (cmd_info[icmd].gm_level && (cmd_info[icmd].gm_level > ch->GetGMLevel() || cmd_info[icmd].gm_level == GM_DISABLE))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This command does not exist."));
		return;
	}
#endif

	if (strncmp("phase", cmd_info[icmd].command, 5) != 0)
	{
		sys_log(0, "COMMAND: %s: %s", ch->GetName(), cmd_info[icmd].command);
	}

	((*cmd_info[icmd].command_pointer) (ch, line, icmd, cmd_info[icmd].subcmd));

	if (ch->GetGMLevel() >= GM_LOW_WIZARD)
	{
		if (cmd_info[icmd].gm_level >= GM_LOW_WIZARD)
		{
			char buf[1024];
			snprintf( buf, sizeof(buf), "%s", argument );

			LogManager::instance().GMCommandLog(ch->GetPlayerID(), ch->GetName(), ch->GetDesc()->GetHostName(), g_bChannel, buf);
		}
	}
}

