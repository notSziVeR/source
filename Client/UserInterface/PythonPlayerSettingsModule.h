/***********************************************************
* title_name		: Player Settings Module
* date_created		: 2020.01.19
* filename			: PythonPlayerSettingsModule.h
* author			: VegaS
* version_actual	: Version 2.0
*/
#pragma once
#include "StdAfx.h"
#include "InstanceBase.h"
#include "PythonItem.h"
#include <PythonPlayerSettingsModuleLib.h>

#ifdef ENABLE_OPTIMIZATION_MODE
enum EOptimizationModeClient
{
	OPTIMIZATION_MODE_DISABLED,
	OPTIMIZATION_MODE_PVP_MEDIUM,
	OPTIMIZATION_MODE_PVP_HIGH,
	OPTIMIZATION_MODE_PVM,
};
#endif

static const auto EMOTICON_PATH = "d:/ymir work/effect/etc/emoticon/";
static const auto BUFF_PATH = "d:/ymir work/effect/etc/buff/";
static const auto NPC_LIST_FILE_NAME = "npclist.txt";

enum
{
	HORSE_SKILL_WILDATTACK = CRaceMotionData::NAME_SKILL + 121,
	HORSE_SKILL_CHARGE = CRaceMotionData::NAME_SKILL + 122,
	HORSE_SKILL_SPLASH = CRaceMotionData::NAME_SKILL + 123,

	GUILD_SKILL_DRAGONBLOOD = CRaceMotionData::NAME_SKILL + 101,
	GUILD_SKILL_DRAGONBLESS = CRaceMotionData::NAME_SKILL + 102,
	GUILD_SKILL_BLESSARMOR = CRaceMotionData::NAME_SKILL + 103,
	GUILD_SKILL_SPPEDUP = CRaceMotionData::NAME_SKILL + 104,
	GUILD_SKILL_DRAGONWRATH = CRaceMotionData::NAME_SKILL + 105,
	GUILD_SKILL_MAGICUP = CRaceMotionData::NAME_SKILL + 106,

	COMBO_TYPE_1 = 0,
	COMBO_TYPE_2 = 1,
	COMBO_TYPE_3 = 2,

	COMBO_INDEX_1 = 0,
	COMBO_INDEX_2 = 1,
	COMBO_INDEX_3 = 2,
	COMBO_INDEX_4 = 3,
	COMBO_INDEX_5 = 4,
	COMBO_INDEX_6 = 5,
};

//! @brief skill index of passive guild skill
static const std::vector<uint8_t> m_vecPassiveGuildSkillIndex = { 151, };
//! @brief skill index of active guild skill
static const std::vector<uint8_t> m_vecActiveGuildSkillIndex = { 152, 153, 154, 155, 156, 157, };

//! @brief Skill index vector with for all characters
static const std::vector<TSkillIndex> m_vecSkillIndex =
{
	{
		NRaceData::JOB_WARRIOR,
		{
			{ 1, { 1, 2, 3, 4, 5, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
			{ 2, { 16, 17, 18, 19, 20, 0, 0, 0, 137, 0, 138, 0, 139, 0, } },
		},
		{ 122, 130, 131, 121, 129, 123, 124, 143, 144, 145, 146, 147 }
	},
	{
		NRaceData::JOB_ASSASSIN,
		{
			{ 1, { 31, 32, 33, 34, 35, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
			{ 2, { 46, 47, 48, 49, 50, 0, 0, 0, 137, 0, 138, 0, 139, 0, 140 } },
		},
		{ 122, 130, 131, 121, 129, 123, 124, 143, 144, 145, 146, 147 }
	},
	{
		NRaceData::JOB_SURA,
		{
			{ 1, { 61, 62, 63, 64, 65, 66, 0, 0, 137, 0, 138, 0, 139, 0 } },
			{ 2, { 76, 77, 78, 79, 80, 81, 0, 0, 137, 0, 138, 0, 139, 0 } },
		},
		{ 122, 130, 131, 121, 129, 123, 124, 143, 144, 145, 146, 147 }
	},
	{
		NRaceData::JOB_SHAMAN,
		{
			{ 1, { 91, 92, 93, 94, 95, 96, 0, 0, 137, 0, 138, 0, 139, 0 } },
			{ 2, { 106, 107, 108, 109, 110, 111, 0, 0, 137, 0, 138, 0, 139, 0 } },
		},
		{ 122, 130, 131, 121, 129, 123, 124, 143, 144, 145, 146, 147 }
	},
};

/**
* \brief Concatenate unlimited string arguments into one string.
*/
static auto string_path(const std::initializer_list<std::string> & args) -> std::string
{
	std::string stPathName;
	for (const auto & it : args)
	{
		stPathName.append(it);
	}
	return stPathName;
}

/**
* \brief
*/
static const std::vector<THorse> m_vecHorse =
{
	{ NRaceData::JOB_WARRIOR, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::MODE_HORSE_TWOHAND_SWORD } },
	{ NRaceData::JOB_ASSASSIN, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD, CRaceMotionData::MODE_HORSE_DUALHAND_SWORD } },
	{ NRaceData::JOB_SURA, { CRaceMotionData::MODE_HORSE_ONEHAND_SWORD } },
	{ NRaceData::JOB_SHAMAN, { CRaceMotionData::MODE_HORSE_FAN, CRaceMotionData::MODE_HORSE_BELL } },
};

/**
* \brief
*/
static const std::vector<TSkillData> m_vecSkillFileName =
{
	{ NRaceData::JOB_WARRIOR, { "samyeon", "palbang", "jeongwi", "geomgyeong", "tanhwan", "gihyeol", "gigongcham", "gyeoksan", "daejin", "cheongeun", "geompung", "noegeom" }, },
	{ NRaceData::JOB_ASSASSIN, { "amseup", "gungsin", "charyun", "eunhyeong", "sangong", "seomjeon", "yeonsa", "gwangyeok", "hwajo", "gyeonggong", "dokgigung", "seomgwang" }, },
	{
		NRaceData::JOB_SHAMAN,
		{ "bipabu", "yongpa", "paeryong", "hosin", "boho", "gicheon", "noejeon", "byeorak", "pokroe", "jeongeop", "kwaesok", "jeungryeok" },
		{ "bipabu", "yongpa", "paeryong", "hosin_target", "boho_target", "gicheon_target", "noejeon", "byeorak", "pokroe", "jeongeop_target", "kwaesok_target", "jeungryeok_target" }
	},
	{ NRaceData::JOB_SURA, { "swaeryeong", "yonggwon", "gwigeom", "gongpo", "jumagap", "pabeop", "maryeong", "hwayeom", "muyeong", "heuksin", "tusok", "mahwan" }, },
};

/**
* \brief Cached effects
*/
static const std::vector<TCacheEffect> m_vecEffects =
{
	{ CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", true },
	{ CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", true },
	{ CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", true },
	{ CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", true },
	{ CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", true },
	{ CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", true },
	{ CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", true },
	{ CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", true },
	{ CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", true },
	{ CInstanceBase::EFFECT_PENETRATE, "Bip01", "d:/ymir work/effect/hit/gwantong.mse", true },
	{ CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", true },
	{ CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", true },
	{ CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", true },
	{ CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", true },
	{ CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", true },
	{ CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_CRITICAL, "", "d:/ymir work/effect/affect/damagevalue/critical.mse", true },
	{ CInstanceBase::EFFECT_DAMAGE_FIRE, "", "d:/ymir work/effect/affect/damagevalue/fire.mse", true },

	//{ CInstanceBase::EFFECT_SUCCESS, "", "d:/ymir work/effect/success.mse", true },
	//{ CInstanceBase::EFFECT_FAIL, "", "d:/ymir work/effect/fail.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", true },
	{ CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", true },

	{ CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item1.mse", true },
	{ CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", true },
	{ CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", true },
	{ CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", true },

#ifdef WJ_COMBAT_ZONE
	{ CInstanceBase::EFFECT_COMBAT_ZONE_POTION, "", "d:/ymir work/effect/etc/buff/buff_item12.mse", true },
#endif
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// No cache
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	{ CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", false },
	{ CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", false },
	{ CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", false },
	{ CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", false },
	{ CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", false },
	{ CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", false },
	{ CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", false },
	//{ CInstanceBase::EFFECT_BOSS, "", "d:/ymir work/effect/boss.mse", false },

	{ CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", false },
	{ CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", false },

	{ CInstanceBase::EFFECT_WEAPON + 1, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", false },
	{ CInstanceBase::EFFECT_WEAPON + 2, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", false },

	{ CInstanceBase::EFFECT_AFFECT + 0, "Bip01", string_path({ "locale/common", "/effect/gm.mse" }), false },
	{ CInstanceBase::EFFECT_AFFECT + 3, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 4, "", "d:/ymir work/effect/affect/slow.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 5, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 6, "", "d:/ymir work/effect/etc/ready/ready.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 16, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 17, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 19, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 20, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 21, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 22, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 23, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 24, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 25, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 26, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 28, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 29, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 30, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 32, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 33, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
	//## 34 Polymoph
	{ CInstanceBase::EFFECT_AFFECT + 35, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 36, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + 37, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", false },

	{ CInstanceBase::EFFECT_REFINED + 1, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 2, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 3, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 4, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 5, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 6, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_b.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 7, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 8, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 9, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_f.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 10, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 11, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 12, "PART_WEAPON", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 13, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 14, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 15, "PART_WEAPON_LEFT", "D:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 16, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_7.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 17, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_8.mse", false },
	{ CInstanceBase::EFFECT_REFINED + 18, "Bip01", "D:/ymir work/pc/common/effect/armor/armor_9.mse", false },
	{ CInstanceBase::EFFECT_BOSS, "", "d:/ymir work/effect/etc/boss_effect/boss.mse", false },

#ifdef ENABLE_SOUL_SYSTEM
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_RED, "", "d:/ymir work/effect/etc/soul/soul_red_001.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_BLUE, "", "d:/ymir work/effect/etc/soul/soul_blue_001.mse", false },
	{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SOUL_MIX, "", "d:/ymir work/effect/etc/soul/soul_mix_001.mse", false },
#endif
};

/**
* \brief Emoticons vector.
*/
static const std::vector<TEmoticon> m_vecEmoticons =
{
	{ CInstanceBase::EFFECT_EMOTICON + 0, "", string_path({ EMOTICON_PATH, "sweat.mse" }), "(sweat)" },
	{ CInstanceBase::EFFECT_EMOTICON + 1, "", string_path({ EMOTICON_PATH, "money.mse" }), "(money)" },
	{ CInstanceBase::EFFECT_EMOTICON + 2, "", string_path({ EMOTICON_PATH, "happy.mse" }), "(happy)" },
	{ CInstanceBase::EFFECT_EMOTICON + 3, "", string_path({ EMOTICON_PATH, "love_s.mse" }), "(love_s)" },
	{ CInstanceBase::EFFECT_EMOTICON + 4, "", string_path({ EMOTICON_PATH, "love_l.mse" }), "(love_l)" },
	{ CInstanceBase::EFFECT_EMOTICON + 5, "", string_path({ EMOTICON_PATH, "angry.mse" }), "(angry)" },
	{ CInstanceBase::EFFECT_EMOTICON + 6, "", string_path({ EMOTICON_PATH, "aha.mse" }), "(aha)" },
	{ CInstanceBase::EFFECT_EMOTICON + 7, "", string_path({ EMOTICON_PATH, "gloom.mse" }), "(gloom)" },
	{ CInstanceBase::EFFECT_EMOTICON + 8, "", string_path({ EMOTICON_PATH, "sorry.mse" }), "(sorry)" },
	{ CInstanceBase::EFFECT_EMOTICON + 9, "", string_path({ EMOTICON_PATH, "!_mix_back.mse" }), "(!)" },
	{ CInstanceBase::EFFECT_EMOTICON + 10, "", string_path({ EMOTICON_PATH, "question.mse" }), "(question)" },
	{ CInstanceBase::EFFECT_EMOTICON + 11, "", string_path({ EMOTICON_PATH, "fish.mse" }), "(fish)" },
};

/**
* \brief Vector of race
*/
static const std::vector<TRaceInfo> m_vecRaceData =
{
	{ "warrior_m.msm", "d:/ymir work/pc/warrior/" },
	{ "assassin_w.msm", "d:/ymir work/pc/assassin/" },
	{ "sura_m.msm", "d:/ymir work/pc/sura/" },
	{ "shaman_w.msm", "d:/ymir work/pc/shaman/" },

	{ "warrior_w.msm", "d:/ymir work/pc2/warrior/" },
	{ "assassin_m.msm", "d:/ymir work/pc2/assassin/" },
	{ "sura_w.msm", "d:/ymir work/pc2/sura/" },
	{ "shaman_m.msm", "d:/ymir work/pc2/shaman/" },
};

/**
* \brief
*/
static const std::vector<TSoundFileName> m_vecUseSoundFileName =
{
	{ CPythonItem::USESOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::USESOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::USESOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::USESOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::USESOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
	{ CPythonItem::USESOUND_POTION, "sound/ui/eat_potion.wav" },
	{ CPythonItem::USESOUND_PORTAL, "sound/ui/potal_scroll.wav" },
};

/**
* \brief
*/
static const std::vector<TSoundFileName> m_vecDropSoundFileName =
{
	{ CPythonItem::DROPSOUND_DEFAULT, "sound/ui/drop.wav" },
	{ CPythonItem::DROPSOUND_ACCESSORY, "sound/ui/equip_ring_amulet.wav" },
	{ CPythonItem::DROPSOUND_ARMOR, "sound/ui/equip_metal_armor.wav" },
	{ CPythonItem::DROPSOUND_BOW, "sound/ui/equip_bow.wav" },
	{ CPythonItem::DROPSOUND_WEAPON, "sound/ui/equip_metal_weapon.wav" },
};

/**
* \brief
*/
static const std::vector<TFlyEffects> m_vecFlyEffects =
{
	{ CFlyingManager::FLY_EXP, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_yellow_small2.msf" },
	{ CFlyingManager::FLY_HP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_small.msf" },
	{ CFlyingManager::FLY_HP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_big.msf" },
	{ CFlyingManager::FLY_SP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_warrior_small.msf" },
	{ CFlyingManager::FLY_SP_MEDIUM, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_small.msf" },
	{ CFlyingManager::FLY_SP_BIG, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_blue_big.msf" },
	{ CFlyingManager::FLY_FIREWORK1, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_1.msf" },
	{ CFlyingManager::FLY_FIREWORK2, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_2.msf" },
	{ CFlyingManager::FLY_FIREWORK3, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_3.msf" },
	{ CFlyingManager::FLY_FIREWORK4, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_4.msf" },
	{ CFlyingManager::FLY_FIREWORK5, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_5.msf" },
	{ CFlyingManager::FLY_FIREWORK6, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_6.msf" },
	{ CFlyingManager::FLY_FIREWORK_XMAS, CFlyingManager::INDEX_FLY_TYPE_FIRE_CRACKER, "d:/ymir work/effect/etc/firecracker/firecracker_xmas.msf" },
	{ CFlyingManager::FLY_CHAIN_LIGHTNING, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/shaman/effect/pokroe.msf" },
	{ CFlyingManager::FLY_HP_SMALL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/effect/etc/gathering/ga_piece_red_smallest.msf" },
	{ CFlyingManager::FLY_SKILL_MUYEONG, CFlyingManager::INDEX_FLY_TYPE_AUTO_FIRE, "d:/ymir work/pc/sura/effect/muyeong_fly.msf" },
	{ CFlyingManager::FLY_QUIVER_ATTACK_NORMAL, CFlyingManager::INDEX_FLY_TYPE_NORMAL, "d:/ymir work/pc/assassin/effect/arrow_02.msf" },
};

/**
 * \brief
 */
class CPythonPlayerSettingsModule : public CSingleton<CPythonPlayerSettingsModule>
{
public:
	CPythonPlayerSettingsModule();
	virtual ~CPythonPlayerSettingsModule();

	auto Load() const -> void;
	static auto __LoadGameNPC() -> void;

protected:
	static auto __RegisterCacheMotionData(CRaceData* pRaceData, const uint16_t wMotionMode, const uint16_t wMotionIndex, const char* c_szFileName, const uint8_t byPercentage = 100) -> void;
	static auto __SetIntroMotions(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __SetGeneralMotions(CRaceData* pRaceData, const uint16_t wMotionMode, const std::string& c_rstrFolderName) -> void;

	static auto __LoadFishing(CRaceData* pRaceData, const std::string& c_rstrFolderName) -> void;
	static auto __LoadCombo(CRaceData* pRaceData, const uint16_t wMotionMode) -> void;
	auto __LoadWeaponMotion(CRaceData* pRaceData, WORD wMotionMode, const std::string & c_rstrFolderName) const -> void;
	auto __LoadHorse(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void;
	auto __LoadSkill(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGuildSkill(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGameWarriorEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGameAssassinEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGameSuraEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGameShamanEx(CRaceData* pRaceData, const std::string& c_rstrFolderName) const -> void;
	auto __LoadGameRace(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void;

};
