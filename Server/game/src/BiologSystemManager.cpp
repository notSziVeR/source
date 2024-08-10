#include "stdafx.h"
#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "BiologSystemManager.hpp"
#include "questmanager.h"
#include "char.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "char_manager.h"
#include "p2p.h"
#include "item_manager.h"
#include "item.h"
#include "utils.h"

#include "mob_manager.h"

#include "EventFunctionHandler.hpp"
#include <cstdarg>

#ifdef __POPUP_SYSTEM_ENABLE__
	#include "PopupManager.hpp"
#endif

namespace
{
	const std::string sBiologState("biolog_manager.active_state");
	const std::string sBiologMissionOverlay("biolog_manager.mission_overlay");
	const std::string sBiologMissionAffectGranted("biolog_manager.aff_granted_{}");

	const std::string sBroadcastMissionBasic("BiologManager_RegisterProgress {} {} {} {}");
	const std::string sBroadcastMissionRegisterMission("BiologManager_RegisterMission {} {} {} {}");
	const std::string sBroadcastMissionRegisterRewardBasic("BiologManager_RegisterRewardBasic {} {} {}");
	const std::string sBroadcastMissionRegisterRewardAffect("BiologManager_RegisterRewardAffect {} {} {} {}");

	const std::string sBroadcastMissionRequestOpen("BiologManager_RequestOpen {} {}");
	const std::string sBroadcastMissionRequestClose("BiologManager_RequestClose {}");
	const std::string sBroadcastMissionRequestClear("BiologManager_RequestClear {}");

	const std::string sBroadcastSetInfo("BiologManager_RegisterSet {} {} {} {} {}");
	const std::string sBroadcastSetAffect("BiologManager_RegisterSetAffect {} {} {} {} {}");
	const std::string sBroadcastSetClear("BiologManager_ClearSet");
	const std::string sBroadcastSetOpen("BiologManager_OpenSet");

	std::map<CBiologSystem::EStates, std::string> m_requester = {
		{CBiologSystem::EStates::BIOLOG_STATE_MISSION, "MISSION"},
		{CBiologSystem::EStates::BIOLOG_STATE_SELECTOR, "SELECTOR"},
	};
};

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystem - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

CBiologSystem::CBiologSystem(LPCHARACTER m_ch)
	: m_pkChar(m_ch)
{
	s_current_biolog_reminder = "BIOLOG_MANAGER_ALERT";

	if (m_pkChar->GetBiologCooldownReminder())
	{
		this->ActiveAlert(true);
	}
}

void CBiologSystem::OnDestroy()
{
	CEventFunctionHandler::Instance().RemoveEvent(s_current_biolog_reminder);
	m_pkChar = nullptr;
}

/*******************************************************************\
| [PUBLIC] Update Functions
\*******************************************************************/

void CBiologSystem::FinishMission()
{
	m_pkChar->SetBiologCollectedItems(0);
	m_pkChar->SetBiologCooldown(0);
	ActiveAlert(false);

	// Set next mission
	m_pkChar->SetBiologMissions(m_pkChar->GetBiologMissions() + 1);
	m_pkChar->SetQuestFlag(sBiologState, EStates::BIOLOG_STATE_MISSION);
}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

void CBiologSystem::Broadcast()
{
	if (!m_pkChar) return;

	auto curMission = m_pkChar->GetQuestFlag(sBiologMissionOverlay) ? m_pkChar->GetQuestFlag(sBiologMissionOverlay) : m_pkChar->GetBiologMissions();
	auto iCount = m_pkChar->GetBiologCollectedItems();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto tReminder = m_pkChar->GetBiologCooldownReminder();

	auto mission_set = CBiologSystemManager::instance().FindBiologSet(curMission);
	if (!mission_set)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, "You have completed all missions!");
		return;
	}

	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionBasic, mission_set->GetId(), iCount, static_cast<time_t>((tWait - get_global_time())), tReminder));
	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRegisterMission, mission_set->GetLevel(), mission_set->GetRequiredItem(), mission_set->GetRequiredCount(), mission_set->GetChance()));
	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRegisterRewardBasic, mission_set->GetReward().GetRewardItem().first, mission_set->GetReward().GetRewardItem().second, static_cast<int>(mission_set->GetReward().GetRewardSelectable())));

	BroadcastAffects(mission_set->GetReward());

	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRequestOpen, m_requester[static_cast<EStates>(m_pkChar->GetQuestFlag(sBiologState))], mission_set->GetId()));
}

void CBiologSystem::BroadcastAffects(BiologCollection::BiologSet::biolog_reward_collection_t reward) {
	for (const auto& [iKey, rElements] : reward.GetRewardAffects())
	{
		m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRegisterRewardAffect, m_requester[static_cast<EStates>(m_pkChar->GetQuestFlag(sBiologState))], iKey, rElements.type, rElements.value));
	}
}

void CBiologSystem::CollectItem(bool bAdditionalChance, bool bAdditionalTime)
{
	if (!m_pkChar) return;

	// In case if we have SELECTOR state we gonna return!
	if (m_pkChar->GetQuestFlag(sBiologState) == EStates::BIOLOG_STATE_SELECTOR) return;

	auto bMission = m_pkChar->GetBiologMissions();
	auto tWait = m_pkChar->GetBiologCooldown();
	auto iCount = m_pkChar->GetBiologCollectedItems();

	// Getting actual biolog mission
	auto pMission = CBiologSystemManager::instance().FindBiologSet(bMission);
	if (!pMission || CBiologSystemManager::instance().LastMission(bMission))
	{
		//We have to inform players they've done every mission!
		return;
	}

	// Checking required level
	if (m_pkChar->GetLevel() < pMission->GetLevel())
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have a sufficient level: %d!"), pMission->GetLevel());
		return;
	}

	// Counting required item
	if (m_pkChar->CountSpecifyItem(pMission->GetRequiredItem()) < 1)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enogh items."));
		return;
	}

	// Checking time
	if (get_global_time() < tWait && bAdditionalTime)
	{
		if (m_pkChar->CountSpecifyItem(BIOLOG_TIME_ITEM) < 1)
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have required item, disable time skip."));
			return;
		}
		else
		{
			m_pkChar->RemoveSpecifyItem(BIOLOG_TIME_ITEM, 1);
		}
	}

	if (get_global_time() < tWait && !bAdditionalTime)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Try again in: %s!"), GetFullDateFromTime(tWait).c_str());
		return;
	}

	// Define variables
	BYTE chance = pMission->GetChance();

	if (bAdditionalChance)
	{
		LPITEM biologChanceItem = m_pkChar->FindSpecifyItem(BIOLOG_CHANCE_ITEM);
		if (biologChanceItem)
		{
			const int BIOLOG_CHANCE = biologChanceItem->GetValue(0);

			if (BIOLOG_CHANCE > 0)
			{
				if (biologChanceItem->GetCount() > 0)
				{
					biologChanceItem->SetCount(biologChanceItem->GetCount() - 1);
					chance += BIOLOG_CHANCE;
				}
				else
				{
					m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have the required chance boost item."));
					return;
				}
			}
			else
			{
				return;
			}
		}
		else
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have the required chance boost item."));
		}
	}

	// Checking count
	if (iCount < pMission->GetRequiredCount()) // it means you haven't give all items
	{
		m_pkChar->RemoveSpecifyItem(pMission->GetRequiredItem(), 1);

		if (chance >= number(1, 100))
		{
			// Update count
			m_pkChar->SetBiologCollectedItems(iCount + 1);

			// Update cooldown
			m_pkChar->SetBiologCooldown(get_global_time() + pMission->GetCooldown());

			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item successfully returned!"));
		}
		else
		{
			m_pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item was too damaged, please try again!"));
		}

		// Active Alert
		if (m_pkChar->GetBiologCooldownReminder())
		{
			ActiveAlert(true);
		}

		// Compare current count with required count
		if (m_pkChar->GetBiologCollectedItems() == pMission->GetRequiredCount())
		{
			// We have to try give reward, atleast items if not then selecter!
			if (!RequestReward(bMission))
			{
				m_pkChar->SetQuestFlag(sBiologState, EStates::BIOLOG_STATE_SELECTOR);
				m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRequestClose, m_requester[EStates::BIOLOG_STATE_MISSION]));
			}
		}

		// Update client!
		Broadcast();
	}
}

bool CBiologSystem::RequestReward(BYTE bMission)
{
	if (!m_pkChar) return false;

	auto pMission = CBiologSystemManager::instance().FindBiologSet(bMission);
	if (!pMission) return false;

	auto tReward = pMission->GetReward();
	auto rewardItem = tReward.GetRewardItem();
	if (rewardItem.first != 0 && rewardItem.second != 0)
	{
		m_pkChar->AutoGiveItem(rewardItem.first, rewardItem.second);
	}

	// Lets check if we have any bonus to select!
	if (tReward.GetRewardSelectable())
	{
		// Lets display an window in game and return in this case!
		return false;
	}

	for (const auto& affect : tReward.GetRewardAffects())
	{
		m_pkChar->AddAffect(AFFECT_BIOLOG, aApplyInfo[affect.second.type].bPointType, affect.second.value, 0, BIOLOG_AFF_TIME, 0, false);
	}

	// Lets reset current progress
	FinishMission();

	return true;
}

void CBiologSystem::RequestAffect(uint8_t key)
{
	if (!m_pkChar) return;

	auto currMission = m_pkChar->GetQuestFlag(sBiologMissionOverlay) ? m_pkChar->GetQuestFlag(sBiologMissionOverlay) : m_pkChar->GetBiologMissions();
	auto set = CBiologSystemManager::instance().FindBiologSet(currMission);
	if (!set) return;

	BiologCollection::BiologSet::biolog_reward_collection_t reward = set->GetReward();

	if (!reward.GetRewardSelectable()) return;

	auto affects = reward.GetRewardAffects();
	auto fIt = std::find_if(affects.begin(), affects.end(), [&key](const std::pair<uint8_t, BiologCollection::BiologSet::Affect> rElement) {
			return (rElement.first == key);
		});

	if (fIt == affects.end()) return;

	m_pkChar->AddAffect(AFFECT_BIOLOG, aApplyInfo[fIt->second.type].bPointType, fIt->second.value, 0, BIOLOG_AFF_TIME, 0, false);
	
	// Lets save which bonus we selected!
	m_pkChar->SetQuestFlag(fmt::format(sBiologMissionAffectGranted, currMission), key);

	// Lets clear your window!
	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRequestClear, m_requester[static_cast<EStates>(m_pkChar->GetQuestFlag(sBiologState))]));

	// Lets reset current progress
	if (m_pkChar->GetQuestFlag(sBiologMissionOverlay))
	{
		m_pkChar->SetQuestFlag(sBiologMissionOverlay, 0);
		m_pkChar->SetQuestFlag(sBiologState, EStates::BIOLOG_STATE_MISSION);
	}
	else
		FinishMission();
};

void CBiologSystem::ResetAffect(uint8_t key)
{
	if (!m_pkChar) return;
	
	// In case if we have SELECTOR state we gonna return!
	if (m_pkChar->GetQuestFlag(sBiologState) == EStates::BIOLOG_STATE_SELECTOR) return;

	// Checking if we can reset it!
	if (key > m_pkChar->GetBiologMissions()) return;

	auto set = CBiologSystemManager::instance().FindBiologSet(key);
	if (!set) return;

	BiologCollection::BiologSet::biolog_reward_collection_t reward = set->GetReward();

	// At beggin let's check if this bonus is selectable!
	if (!reward.GetRewardSelectable()) return;

	// We have to check if we have enough item!
	if (m_pkChar->CountSpecifyItem(BIOLOG_RESET_ITEM) < 1) return;

	// Lets remove affect, if we couldn't find it its will simply return!
	for (const auto& [iKey, element] : reward.GetRewardAffects())
	{
		auto pAffect = m_pkChar->FindAffect(AFFECT_BIOLOG, aApplyInfo[element.type].bPointType);
		if (!pAffect)
			continue;

		if (!m_pkChar->RemoveAffect(pAffect))
			return;
	}

	m_pkChar->RemoveSpecifyItem(BIOLOG_RESET_ITEM, 1);

	// Right now we have to close all windows, and set SELECTOR state!
	for (const auto& [eType, sKey] : m_requester)
	{
		m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRequestClose, sKey));
	}

	m_pkChar->SetQuestFlag(sBiologState, EStates::BIOLOG_STATE_SELECTOR);

	// We gonna set overlay due to broadcasting if player's disconnect or smth!
	m_pkChar->SetQuestFlag(sBiologMissionOverlay, key);

	// We have to broadcast data to selector!
	BroadcastAffects(reward);
	m_pkChar->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastMissionRequestOpen, m_requester[EStates::BIOLOG_STATE_SELECTOR], key));
}

bool CBiologSystem::GetBiologItemByMobVnum(LPCHARACTER pkKiller, WORD MonsterVnum, DWORD& ItemVnum, BYTE& bChance)
{
	if (!pkKiller || !MonsterVnum) return false;

	const CMob* pkMob = CMobManager::instance().Get(MonsterVnum);
	if (!pkMob) return false;

	auto currMission = pkKiller->GetBiologMissions();
	auto setMission = CBiologSystemManager::instance().FindBiologSet(currMission);
	if (!setMission) return false;

	if (pkKiller->GetLevel() < setMission->GetLevel()) return false;

	auto monsters = setMission->GetMonsters();
	if (!monsters.size()) return false;

	auto monster = std::find_if(monsters.begin(), monsters.end(), [&MonsterVnum](const BiologCollection::BiologSet::monster_t& m)
	{
		return m.vnum == MonsterVnum;
	});
	if (monster == monsters.end()) return false;

	ItemVnum = setMission->GetRequiredItem();
	bChance = monster->chance;

	return true;
}

/*******************************************************************\
| [PUBLIC] Alert Functions
\*******************************************************************/

void CBiologSystem::ActiveAlert(bool bReminder)
{
	if (!m_pkChar) return;

	if (bReminder)
	{
		m_pkChar->SetBiologCooldownReminder(1);

		if (m_pkChar->GetBiologCooldown() < get_global_time()) return;

		if (!CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
		{
			CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl*)
			{
				this->BroadcastAlert();
			},
			(s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());
		}
		else
			CEventFunctionHandler::instance().DelayEvent((s_current_biolog_reminder), m_pkChar->GetBiologCooldown() - get_global_time());


		return;
	}

	m_pkChar->SetBiologCooldownReminder(0);
	if (CEventFunctionHandler::Instance().FindEvent(s_current_biolog_reminder))
		CEventFunctionHandler::Instance().RemoveEvent((s_current_biolog_reminder));
}

void CBiologSystem::BroadcastAlert()
{
	if (m_pkChar == nullptr) return;

	if (m_pkChar && m_pkChar->GetDesc() && m_pkChar->GetDesc()->IsPhase(PHASE_GAME))
	{
#ifdef __POPUP_SYSTEM_ENABLE__
		auto pMission = CBiologSystemManager::instance().FindBiologSet(m_pkChar->GetBiologMissions());
		if (!pMission) return;

		CPopupManager::instance().ProcessPopup(m_pkChar, NSPopupMessage::ECPopupTypes::BIOLOGIST_COLLECTION, static_cast<int>(pMission->GetRequiredItem()));
#else
		SendClientPacket(m_pkChar->GetDesc(), GC_BIOLOG_ALERT, NULL, 0);
#endif
	}
}

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CBiologSystemManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

BiologCollection::BiologSet::BiologSet(uint32_t id, uint8_t level, uint32_t item, uint32_t count, time_t cooldown, uint8_t chance, biolog_reward_collection_t reward, biolog_monster_collection_t monsters)
	: id_(std::move(id))
	, level_(std::move(level))
	, item_(std::move(item))
	, count_(std::move(count))
	, cooldown_(std::move(cooldown))
	, chance_(std::move(chance))
	, reward_(std::move(reward))
	, monsters_(std::move(monsters))
{}

const BiologCollection::BiologSet* CBiologSystemManager::FindBiologSet(const uint8_t id) const
{
	auto it = sets_.find(id);
	if (it == sets_.end())
		return nullptr;

	return &it->second;
}

void CBiologSystemManager::Create()
{
	Load("locale/germany/biolog-manager.xml");
}

void CBiologSystemManager::Clear()
{
	sets_.clear();
}

bool CBiologSystemManager::Load(const std::string& filename)
{
	BiologCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	auto sets = std::move(parser.BiologSets());
	for (auto& set : sets)
		sets_.emplace(set.GetId(), std::move(set));

	return true;
}

void CBiologSystemManager::BroadcastSets(LPCHARACTER ch)
{
	if (!ch || !ch->IsPC()) return;
	
	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastSetClear);

	for (const auto& [key, set] : sets_)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastSetInfo, key, static_cast<int>(ch->GetBiologMissions() > key), static_cast<int>(set.GetReward().GetRewardSelectable()), set.GetRequiredItem(), set.GetLevel()));
		for (const auto& [applyKey, apply] : set.GetReward().GetRewardAffects())
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastSetAffect, key, applyKey, static_cast<int>(set.GetReward().GetRewardSelectable() != true ? true : ch->GetQuestFlag(fmt::format(sBiologMissionAffectGranted, key)) == applyKey), apply.type, apply.value));
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastSetOpen);
}
#endif
