#include "stdafx.h"
#include "PSkillsManager.hpp"
#include "char.h"
#include "skill.h"

namespace PSkillsManager
{
	namespace nConfiguration
	{
		std::unordered_map<uint32_t, std::vector<uint32_t>> m_Configuration {
			{ SKILL_PASSIVE_BOSSESS, {
				10,
				25,
				50,
				100,
				150,
				200,
				250,
				300,
				400,
				500,
			}},

			{ SKILL_PASSIVE_STONES, {
				100,
				250,
				500,
				1000,
				1500,
				2000,
				2500,
				3000,
				4000,
				5000,
			}},

			{ SKILL_PASSIVE_MONSTERS, {
				1000,
				5000,
				10000,
				20000,
				50000,
				100000,
				200000,
				250000,
				350000,
				500000,
			}},
		};

		const std::string sFlag("passive_manager.skill_");

		const std::string sBroadcastClearInterface("SkillsManager_ClearInterface");
		const std::string sBrodadcastInformation("SkillsManager_RegisterInformation {} {} {} {}");
	}

	// Sending the data
	void BroadcastData(LPCHARACTER& ch, bool bClear, int iNum)
	{
		if (!ch) return;

		if (bClear)
			ch->ChatPacket(CHAT_TYPE_COMMAND, nConfiguration::sBroadcastClearInterface.c_str());

		// Just in case if we wanna update isngle record
		if (iNum > -1)
		{
			auto mData = nConfiguration::m_Configuration.find(iNum);
			if (mData == nConfiguration::m_Configuration.end()) return;

			auto fIte = std::find_if(mData->second.begin(), mData->second.end(), [&](const uint32_t& rItem) { return rItem > GetPersonalHighScore(ch, mData->first); });
			if (fIte == mData->second.end()) return;

			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(nConfiguration::sBrodadcastInformation, mData->first, mData->second.size(), GetPersonalHighScore(ch, mData->first), (*fIte)).c_str());
			return;
		}

		for (const auto & rElement : nConfiguration::m_Configuration)
		{
			auto fIte = std::find_if(rElement.second.begin(), rElement.second.end(), [&](const uint32_t& rItem) { return rItem > GetPersonalHighScore(ch, rElement.first); });
			if (fIte != rElement.second.end())
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(nConfiguration::sBrodadcastInformation, rElement.first, rElement.second.size(), GetPersonalHighScore(ch, rElement.first), (*fIte)).c_str());
			}
		}
	}
	
	// Character actions
	void RegisterHighscore(LPCHARACTER& ch, const uint8_t iType)
	{
		if (!ch) return;

		// Validate the skill type
		CSkillProto* pkSk = CSkillManager::instance().Get(iType);
		if (!pkSk) return;

		// Lets check if we have this skill in the map
		const auto vType = nConfiguration::m_Configuration.find(iType);
		if (vType == nConfiguration::m_Configuration.end()) return;

		// Lets check if we still can intercase the points
		auto fIte = std::find_if(vType->second.begin(), vType->second.end(), [&](const uint32_t & rItem) { return rItem > GetPersonalHighScore(ch, vType->first); });
		if (fIte == vType->second.end()) return;

		uint32_t iCurPoints = GetPersonalHighScore(ch, iType);
		if (iCurPoints + 1 >= static_cast<uint32_t>(*fIte))
		{
			// Setting lv up!
			ch->SetSkillLevel(iType, ch->GetSkillLevel(iType) + 1);
			ch->Save();

			ch->ComputePoints();
			ch->SkillLevelPacket();
		}

		// Intercase points
		SetPersonalHighScore(ch, iType);

		// Broadcast update data!
		BroadcastData(ch, false, iType);
	}

	void ComputePoints(LPCHARACTER ch)
	{
		if (!ch) return;

		for (const auto& rElements : nConfiguration::m_Configuration)
		{
			CSkillProto* pkSk = CSkillManager::Instance().Get(rElements.first);
			if (!pkSk) continue;

			if (ch->GetSkillLevel(rElements.first) == 0) continue;

			pkSk->SetPointVar("k", ch->GetSkillLevel(rElements.first));
			int iStrength = (int)pkSk->kPointPoly.Eval();

			ch->PointChange(pkSk->bPointOn, iStrength);
		}
	}

	// Personal Data
	void SetPersonalHighScore(LPCHARACTER& ch, const uint8_t iType)
	{
		auto iCur = GetPersonalHighScore(ch, iType);
		ch->SetQuestFlag(nConfiguration::sFlag + std::to_string(iType), iCur + 1);
	}

	int GetPersonalHighScore(LPCHARACTER& ch, const uint8_t iType)
	{
		return ch->GetQuestFlag(nConfiguration::sFlag + std::to_string(iType));
	}
}
