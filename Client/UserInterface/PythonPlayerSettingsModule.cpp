/***********************************************************
* title_name		: Player Settings Module
* date_created		: 2020.01.19
* filename			: PythonPlayerSettingsModule.cpp
* author			: VegaS
* version_actual	: Version 2.0
*/

#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "PythonPlayer.h"
#include "PythonPlayerSettingsModule.h"

#include "../EterLib/ResourceManager.h"
#include "../EterPack/EterPackManager.h"
#include "../GameLib/RaceManager.h"
#include "PythonSystem.h"
#include "PythonNonPlayer.h"

#include "PythonConfig.h"

extern const DWORD c_iSkillIndex_Language1;
extern const DWORD c_iSkillIndex_Language2;
extern const DWORD c_iSkillIndex_Language3;

CPythonPlayerSettingsModule::CPythonPlayerSettingsModule()
	= default;

CPythonPlayerSettingsModule::~CPythonPlayerSettingsModule()
	= default;

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__SetIntroMotions(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "intro/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_GENERAL);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_SELECTED, "selected.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_INTRO_NOT_SELECTED, "not_selected.msa");
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__SetGeneralMotions(CRaceData * pRaceData, const uint16_t wMotionMode, const std::string & c_rstrFolderName) -> void
{
	CRaceManager::Instance().SetPathName(c_rstrFolderName.c_str());
	pRaceData->RegisterMotionMode(wMotionMode);

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_FLYING, "damage_flying.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_STAND_UP, "falling_stand.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_STAND_UP_BACK, "back_falling_stand.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DIG, "dig.msa");
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadFishing(CRaceData * pRaceData, const std::string & c_rstrFolderName) -> void
{
	CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "fishing/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_FISHING);

	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_WAIT, "wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_THROW, "throw.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_WAIT, "fishing_wait.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_STOP, "fishing_cancel.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_REACT, "fishing_react.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_CATCH, "fishing_catch.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_FISHING, CRaceMotionData::NAME_FISHING_FAIL, "fishing_fail.msa");
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
*/
auto CPythonPlayerSettingsModule::__LoadCombo(CRaceData * pRaceData, const uint16_t wMotionMode) -> void
{
	if (wMotionMode == CRaceMotionData::MODE_BOW)
	{
		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
		return;
	}

	// Combo Type 1
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_1, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_4);
	// Combo Type 2
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_2, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_7);
	// Combo Type 3
	pRaceData->ReserveComboAttack(wMotionMode, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_4, CRaceMotionData::NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_5, CRaceMotionData::NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(wMotionMode, COMBO_TYPE_3, COMBO_INDEX_6, wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD ? CRaceMotionData::NAME_COMBO_ATTACK_8 : CRaceMotionData::NAME_COMBO_ATTACK_4);
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadWeaponMotion(CRaceData * pRaceData, const uint16_t wMotionMode, const std::string & c_rstrFolderName) const -> void
{
	const BYTE bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	// Set path name
	CRaceManager::Instance().SetPathName(c_rstrFolderName.c_str());

	// Register motion mode
	pRaceData->RegisterMotionMode(wMotionMode);

	if (bJobIndex == NRaceData::JOB_WARRIOR)
	{
		const bool isOneHand = wMotionMode == CRaceMotionData::MODE_ONEHAND_SWORD;
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", isOneHand ? 50 : 70);
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", isOneHand ? 50 : 30);
	}
	else if (bJobIndex == NRaceData::JOB_ASSASSIN)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa", 70);
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait_1.msa", 30);
	}
	else if (bJobIndex == NRaceData::JOB_SURA)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	}
	else if (bJobIndex == NRaceData::JOB_SHAMAN)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WAIT, "wait.msa");
	}

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_RUN, "run.msa");

	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE, "damage_1.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_2.msa", 50);
	__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_DAMAGE_BACK, "damage_3.msa", 50);

	if (wMotionMode == CRaceMotionData::MODE_BOW)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
	}

	if (wMotionMode != CRaceMotionData::MODE_BOW)
	{
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_4, "combo_04.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_5, "combo_05.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_6, "combo_06.msa");
		__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_7, "combo_07.msa");

		if (bJobIndex == NRaceData::JOB_ASSASSIN && wMotionMode == CRaceMotionData::MODE_DUALHAND_SWORD)
		{
			__RegisterCacheMotionData(pRaceData, wMotionMode, CRaceMotionData::NAME_COMBO_ATTACK_8, "combo_08.msa");
		}
	}

	// Load combo
	__LoadCombo(pRaceData, wMotionMode);
}

/**
* \brief
* \param pRaceData
* \param wMotionMode
* \param wMotionIndex
* \param c_szFileName
* \param byPercentage
*/
auto CPythonPlayerSettingsModule::__RegisterCacheMotionData(CRaceData * pRaceData, const uint16_t wMotionMode, const uint16_t wMotionIndex, const char * c_szFileName, const uint8_t byPercentage) -> void
{
	const char * c_szFullFileName = CRaceManager::Instance().GetFullPathFileName(c_szFileName);
	const CGraphicThing * pkMotionThing = pRaceData->RegisterMotionData(wMotionMode, wMotionIndex, c_szFullFileName, byPercentage);
	if (pkMotionThing)
	{
		CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
	}

}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGuildSkill(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "skill/" }).c_str());
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, GUILD_SKILL_MAGICUP, "guild_jumunsul.msa");
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadHorse(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	const BYTE bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "horse/" }).c_str());
	pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WALK, "walk.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_RUN, "run.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DEAD, "dead.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, HORSE_SKILL_CHARGE, "skill_charge.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE, "damage.msa");
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, CRaceMotionData::NAME_DAMAGE_BACK, "damage.msa");

	if (bJobIndex == NRaceData::JOB_SHAMAN || bJobIndex == NRaceData::JOB_SURA)
	{
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE, HORSE_SKILL_SPLASH, "skill_splash.msa");
	}

	for (const auto & info : m_vecHorse)
	{
		if (info.bJobIndex == bJobIndex)
		{
			for (const auto & motion : info.vecMotions)
			{
				CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, GetHorsePathNameByMode(motion) }).c_str());
				pRaceData->RegisterMotionMode(motion);

				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_1, "combo_01.msa");
				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_2, "combo_02.msa");
				__RegisterCacheMotionData(pRaceData, motion, CRaceMotionData::NAME_COMBO_ATTACK_3, "combo_03.msa");

				pRaceData->ReserveComboAttack(motion, COMBO_TYPE_1, 3);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_2, CRaceMotionData::NAME_COMBO_ATTACK_2);
				pRaceData->RegisterComboAttack(motion, COMBO_TYPE_1, COMBO_INDEX_3, CRaceMotionData::NAME_COMBO_ATTACK_3);

				__RegisterCacheMotionData(pRaceData, motion, HORSE_SKILL_WILDATTACK, "skill_wildattack.msa");
				if (bJobIndex == NRaceData::JOB_WARRIOR || bJobIndex == NRaceData::JOB_ASSASSIN)
				{
					__RegisterCacheMotionData(pRaceData, motion, HORSE_SKILL_SPLASH, "skill_splash.msa");
				}
			}
			break;
		}
	}

	if (bJobIndex == NRaceData::JOB_ASSASSIN)
	{
		CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "horse_bow/" }).c_str());
		pRaceData->RegisterMotionMode(CRaceMotionData::MODE_HORSE_BOW);

		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait.msa", 90);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_1.msa", 9);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_WAIT, "wait_2.msa", 1);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_RUN, "run.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DAMAGE, "damage.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_DEAD, "dead.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, HORSE_SKILL_WILDATTACK, "skill_wildattack.msa");
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_HORSE_BOW, HORSE_SKILL_SPLASH, "skill_splash.msa");

		pRaceData->ReserveComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, 1);
		pRaceData->RegisterComboAttack(CRaceMotionData::MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);
	}
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameWarriorEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// TWOHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_TWOHAND_SWORD, string_path({ c_rstrFolderName, "twohand_sword/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right_hand");
#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_SASH, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameAssassinEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) const  -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// DUALHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_DUALHAND_SWORD, string_path({ c_rstrFolderName, "dualhand_sword/" }));
	// BOW BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_BOW, string_path({ c_rstrFolderName, "bow/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_SASH, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameSuraEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	// ONEHAND_SWORD BATTLE
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_ONEHAND_SWORD, string_path({ c_rstrFolderName, "onehand_sword/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_SASH, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameShamanEx(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	// Fan
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_FAN, string_path({ c_rstrFolderName, "fan/" }));
	// Bell
	__LoadWeaponMotion(pRaceData, CRaceMotionData::MODE_BELL, string_path({ c_rstrFolderName, "Bell/" }));
	// Bone
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_WEAPON_LEFT, "equip_left");
#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::EParts::PART_SASH, "Bip01 Spine2");
#endif
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadGameRace(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	const BYTE bJobIndex = RaceToJob(pRaceData->GetRaceIndex());

	__SetGeneralMotions(pRaceData, CRaceMotionData::MODE_GENERAL, string_path({ c_rstrFolderName, "general/" }));

	if (bJobIndex == NRaceData::JOB_WARRIOR || bJobIndex == NRaceData::JOB_ASSASSIN)
	{
		pRaceData->SetMotionRandomWeight(CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, 0, 70);
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_WAIT, "wait_1.msa", 30);
	}

	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack.msa", 50);
	__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_COMBO_ATTACK_1, "attack_1.msa", 50);

	pRaceData->ReserveComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(CRaceMotionData::MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, CRaceMotionData::NAME_COMBO_ATTACK_1);

	switch (pRaceData->GetRaceIndex())
	{
	case MAIN_RACE_WARRIOR_M:
	case MAIN_RACE_WARRIOR_W:
		__LoadGameWarriorEx(pRaceData, c_rstrFolderName);
		break;
	case MAIN_RACE_ASSASSIN_M:
	case MAIN_RACE_ASSASSIN_W:
		__LoadGameAssassinEx(pRaceData, c_rstrFolderName);
		break;
	case MAIN_RACE_SURA_M:
	case MAIN_RACE_SURA_W:
		__LoadGameSuraEx(pRaceData, c_rstrFolderName);
		break;
	case MAIN_RACE_SHAMAN_M:
	case MAIN_RACE_SHAMAN_W:
		__LoadGameShamanEx(pRaceData, c_rstrFolderName);
		break;
	default:
		break;
	}
}

/**
* \brief
* \param pRaceData
* \param c_rstrFolderName
*/
auto CPythonPlayerSettingsModule::__LoadSkill(CRaceData * pRaceData, const std::string & c_rstrFolderName) const -> void
{
	const BYTE bJobIndex = static_cast<uint8_t>(RaceToJob(pRaceData->GetRaceIndex()));
	CRaceManager::Instance().SetPathName(string_path({ c_rstrFolderName, "skill/" }).c_str());

	for (const auto & skillData : m_vecSkillFileName)
	{
		if (skillData.bJobIndex == bJobIndex)
		{
			uint8_t iRow = 1;
			for (const auto & skillName : skillData.vecExtraFileName)
			{
				if (iRow == 7)
				{
					iRow += 9;
				}

				__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + iRow, string_path({ skillName, ".msa" }).c_str());
				++iRow;
			}

			static const std::vector<uint8_t> vecGeneralSkills = { 0, 1, 2, 3 };
			static const std::vector<uint8_t> vecSkillShaman = { 1, 2, 3 };

			for (const auto & iSkillEffectCount : bJobIndex == NRaceData::JOB_SHAMAN ? vecSkillShaman : vecGeneralSkills)
			{
				char szSuffixSkillName[8] = {};
				if (iSkillEffectCount != 0)
				{
					snprintf(szSuffixSkillName, sizeof(szSuffixSkillName), "_%d", iSkillEffectCount + 1);
				}

				uint8_t iSecondRow = 1;
				for (const auto & skillName : skillData.vecFileName)
				{
					if (iSecondRow == 7)
					{
						iSecondRow += 9;
					}

					__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + (iSkillEffectCount * CPythonSkill::SKILL_GRADEGAP) + iSecondRow, string_path({ skillName, szSuffixSkillName, ".msa" }).c_str());
					++iSecondRow;
				}
			}

			break;
		}
	}

#ifdef ENABLE_BACKSTEP_SKILL
	if (bJobIndex != NRaceData::JOB_WOLFMAN)
	{
		const BYTE bSexIndex = RaceToSex(pRaceData->GetRaceIndex());

		static const std::vector<std::string> vecJobNames = { "war", "ninja", "sura", "shamy" };
		static const std::vector<std::string> vecSexNames = { "f", "m" };

		char szBackstepSkillFileName[64];
		snprintf(szBackstepSkillFileName, sizeof(szBackstepSkillFileName), "phill_%s_%s_backstep.msa", vecJobNames[bJobIndex].c_str(), vecSexNames[bSexIndex].c_str());
		__RegisterCacheMotionData(pRaceData, CRaceMotionData::MODE_GENERAL, CRaceMotionData::NAME_SKILL + CPythonSkill::MOTION_BACKSTEP, szBackstepSkillFileName);
	}
#endif
}

/**
* \brief Load npclist file.
*/
auto CPythonPlayerSettingsModule::__LoadGameNPC() -> void
{
	CMappedFile file;
	const VOID * pvData;
	if (!CEterPackManager::Instance().Get(file, NPC_LIST_FILE_NAME, &pvData))
	{
		TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=%s) - Load Error", NPC_LIST_FILE_NAME);
		return;
	}

	CMemoryTextFileLoader fileLoader;
	fileLoader.Bind(file.Size(), pvData);

	CTokenVector kTokenVector;
	for (uint32_t i = 0; i < fileLoader.GetLineCount(); ++i)
	{
		if (!fileLoader.SplitLineByTab(i, &kTokenVector))
		{
			continue;
		}

		for (auto & token : kTokenVector)
		{
			token.erase(std::remove_if(token.begin(), token.end(), isspace), token.end());
		}

		while (kTokenVector.size() < 4)
		{
			kTokenVector.push_back("");
		}

		const uint32_t dwMobVnum = atoi(kTokenVector.at(0).c_str());
		const std::string & c_rstName = kTokenVector.at(1);
		const std::string & c_rstSrcName = kTokenVector.at(2);

		/* CHECK IT! */
		if (dwMobVnum)
			if (CPythonNonPlayer::Instance().GetMobRank(dwMobVnum) < 4 && CPythonConfig::Instance().GetString(CPythonConfig::CLASS_OPTION, "MODEL_MODE", "0") == "1")
			{
				bool passed = true;
				const auto m_dwList = {
					636, 2002, 502,
					2591
				};

				for (const auto& it : m_dwList) if (it == dwMobVnum) passed = false;
				if (passed)
					CRaceManager::Instance().RegisterRaceName(dwMobVnum, "stray_dog");
				else
					CRaceManager::Instance().RegisterRaceName(dwMobVnum, c_rstName.c_str());
			}
			else
			{
				CRaceManager::Instance().RegisterRaceName(dwMobVnum, c_rstName.c_str());
			}
		else if (!c_rstSrcName.empty())
		{
			CRaceManager::Instance().RegisterRaceSrcName(c_rstName.c_str(), c_rstSrcName.c_str());
		}
		else
		{
			TraceError("CPythonPlayerSettingsModule::LoadGameNPC(fileName=%s) - %d, line #%d", NPC_LIST_FILE_NAME, dwMobVnum, i);
		}
	}
}

/**
 * \brief Load all of the functions.
 */
auto CPythonPlayerSettingsModule::Load() const -> void
{
	__LoadGameNPC();

	uint32_t dwRaceIndex = 0;
	for (const auto& it : m_vecRaceData)
	{
		CRaceData * pRaceData = nullptr;
		CRaceManager::Instance().CreateRace(dwRaceIndex);
		if (CRaceManager::Instance().GetRaceDataPointer(dwRaceIndex, &pRaceData))
		{
			pRaceData->LoadRaceData(it.stFileName.c_str());

			__SetIntroMotions(pRaceData, it.stFolderName);
			__LoadGameRace(pRaceData, it.stFolderName);
			__LoadHorse(pRaceData, it.stFolderName);
			__LoadFishing(pRaceData, it.stFolderName);
			__LoadGuildSkill(pRaceData, it.stFolderName);
			__LoadSkill(pRaceData, it.stFolderName);
			++dwRaceIndex;
		}
	}
}

///////////////////////////////////////////////////
/// initPlayerSettingsModule
///////////////////////////////////////////////////

PyObject * appRegisterSkill(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bRaceIndex = 0, bSkillGroupIndex = 0, bEmpireIndex = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bRaceIndex) || !PyTuple_GetByte(poArgs, 1, &bSkillGroupIndex) || !PyTuple_GetByte(poArgs, 2, &bEmpireIndex))
	{
		return Py_BuildException();
	}

	RegisterSkill(bRaceIndex, bSkillGroupIndex, bEmpireIndex);
	return Py_BuildNone();
}

PyObject * appGetEmotionPath(PyObject* poSelf, PyObject* poArgs)
{
	const auto & poList = PyList_New(0);
	for (const auto & it : m_vecRaceData)
	{
		PyList_Append(poList, PyString_FromString(it.stFolderName.c_str()));
	}

	return poList;
}

PyObject* appInit(PyObject* poSelf, PyObject* poArgs)
{
	std::string buf;
	//GetExcutedFileName(buf);
	//buf = CFileNameHelper::NoPath(buf);
	//CFileNameHelper::ChangeDosPath(buf);
	buf = "Rodnia.exe";

	LoadPythonPlayerSettingsModuleLibrary(buf);
	CPythonPlayerSettingsModule::instance().Load();

	PyRun_SimpleString("import introLoading\nintroLoading.__main__()\n");
	return Py_BuildNone();
}

PyObject* appLoadingProgress(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bIndex = 0;
	if (!PyTuple_GetByte(poArgs, 0, &bIndex))
	{
		return Py_BuildException();
	}

	LoadingProgress(bIndex);
	return Py_BuildNone();
}

PyObject* appReloadNPC(PyObject* poSelf, PyObject* poArgs)
{
	CPythonPlayerSettingsModule::Instance().__LoadGameNPC();
	return Py_BuildNone();
}

void TraceErrorHandler(const char* c_szFormat)
{
	TraceError(c_szFormat);
}

void initPlayerSettingsModule()
{
	auto pkPlayer = &CPythonPlayer::Instance();
	auto pkItem = &CPythonItem::Instance();
	auto pkFlying = &CFlyingManager::Instance();
	auto rkNetStream = &CPythonNetworkStream::Instance();
	auto pkResource = &CResourceManager::Instance();
	auto rkSystem = &CPythonSystem::Instance();

	PythonPlayerSettingsModuleLibRegisterFunctions([pkItem](const DWORD arg1, const std::string & arg2) { pkItem->SetUseSoundFileName(arg1, arg2); }, [pkItem](const DWORD arg1, const std::string & arg2) { pkItem->SetDropSoundFileName(arg1, arg2); }, [pkFlying](const DWORD arg1, const BYTE arg2, const char* arg3) { pkFlying->RegisterIndexedFlyData(arg1, arg2, arg3); }, [rkNetStream](const char* arg1) { rkNetStream->RegisterEmoticonString(arg1); }, [pkPlayer](const DWORD arg1, const DWORD arg2) { pkPlayer->SetSkill(arg1, arg2); }, [pkPlayer](const DWORD arg1, const char* arg2, bool arg3) { pkPlayer->RegisterEffect(arg1, arg2, arg3); }, [pkResource](const std::string & arg1) { pkResource->LoadMotionFile(arg1); }, CInstanceBase::SetDustGap, CInstanceBase::SetHorseDustGap, CInstanceBase::RegisterEffect, TraceErrorHandler, static_cast<BYTE>(0));
	PythonPlayerSettingsModuleLibRegisterVariables(m_vecPassiveGuildSkillIndex, m_vecActiveGuildSkillIndex, m_vecSkillIndex, m_vecHorse, m_vecSkillFileName, m_vecEffects, m_vecEmoticons, m_vecRaceData, m_vecUseSoundFileName, m_vecDropSoundFileName, m_vecFlyEffects, 0);

	static struct PyMethodDef s_methods[] =
	{
		{ "RegisterSkill", appRegisterSkill, METH_VARARGS },
		{ "GetEmotionsPath", appGetEmotionPath, METH_VARARGS },
		{ "Init", appInit, METH_VARARGS },
		{ "LoadingProgress", appLoadingProgress, METH_VARARGS },
		{ "ReloadNPC", appReloadNPC, METH_VARARGS },
	};

	Py_InitModule("playerLoad", s_methods);
}
