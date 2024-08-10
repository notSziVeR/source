#include "stdafx.h"
#ifdef __LEGENDARY_STONES_ENABLE__
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "utils.h"
#include "item.h"
#include "item_manager.h"
#include "skill.h"
#include "LegendaryStonesHandler.hpp"

#include <algorithm>

namespace LegendaryStonesHandler
{
	namespace
	{
		// Passive config
		constexpr size_t MAX_PASSIVE_PERCENT = 35;
		constexpr BYTE PASSIVE_PERCENT_ADDER = 2;

		std::unordered_map<DWORD, BYTE> v_PassiveConfiguration = {};

		const std::string sFlagBase("LEGENDARY_STONES.PASSIVE_GIVED_{}");

		const std::string sBroadcastClearInterface("LEGENDARY_STONES_CLEAR {}");
		const std::string sBroadcastBroadcastInformation("LEGENDARY_STONES_SET_ITEM {} ");

		// Crafting config
		constexpr int iEmptySlotValue = -1;
		constexpr size_t MAX_CRAFTING_COUNT = 12;

		std::unordered_map<ECraftTypes, std::pair<DWORD, std::vector<SCrafting>>> m_CraftingConfiguration =
		{
			{
				ECraftTypes::CRAFT_TYPE_MINERALS,
				{
					0,

					{
					},
				},
			},

			{
				ECraftTypes::CRAFT_TYPE_SHARDS,
				{
					0,

					{
					},
				},
			}
		};

		// Exchange config
		constexpr size_t MAX_STONE_COUNT = 10;
		const DWORD STONE_EXCHANGE_ADDER = 100;
		std::map<DWORD, std::array<int, MAX_STONE_COUNT>> m_exchange_stones;
		std::unordered_set<DWORD> us_exchange_stones = {
			28530, 28531, 28532, 28533, 28534,
			28535, 28536, 28537, 28538, 28539,
			28540, 28541, 28542, 28543
		};

		// Additional required items
		std::vector<std::pair<DWORD, CountType>> v_exchange_additional = {};

		// Refine config
		constexpr size_t MAX_REFINE_STONE_COUNT = 2;

		std::map<DWORD, std::array<int, MAX_REFINE_STONE_COUNT>> m_refine_stone;
		DWORD REFINE_STONE_ADDITIONAL = 25041;
		CountType REFINE_STONE_ADDITIONAL_COUNT = 1;

		uint64_t REFINE_STONE_COST = 5000000;

		std::unordered_set<DWORD> us_refine_stones = {
			28630, 28631, 28632, 28633, 28634,
			28635, 28636, 28637, 28638, 28639,
			28640, 28641, 28642, 28643,
		};
	}

	// Sending Configuration
	void BroadcastInterface(LPCHARACTER& ch, const LegendaryStonesHandler::EWindowType& eType)
	{
		if (!ch)
			return;

		switch (eType)
		{
			case LegendaryStonesHandler::EWindowType::WINDOW_TYPE_CRAFT_MINERALS:
			{
				auto iFind = m_CraftingConfiguration.find(LegendaryStonesHandler::ECraftTypes::CRAFT_TYPE_MINERALS);
				if (iFind == m_CraftingConfiguration.end())
					return;

				auto iReward = iFind->second.first;

				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_OPEN {} {}", "MINERALS", iReward));
			}
			break;

			case LegendaryStonesHandler::EWindowType::WINDOW_TYPE_CRAFT_SHARDS:
			{
				auto iFind = m_CraftingConfiguration.find(LegendaryStonesHandler::ECraftTypes::CRAFT_TYPE_SHARDS);
				if (iFind == m_CraftingConfiguration.end())
					return;

				auto iReward = iFind->second.first;

				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_OPEN {} {}", "SHARDS", iReward));
			}
			break;

			case LegendaryStonesHandler::EWindowType::WINDOW_TYPE_CRAFT_STONES:
			{
				for (const auto& rItem : v_exchange_additional)
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_CONFIGURATION {} {} {}", "STONES", rItem.first, rItem.second));
				}

				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_OPEN {} {}", "STONES", GetPassivePercent(ch)));
			}
			break;

			case LegendaryStonesHandler::EWindowType::WINDOW_TYPE_REFINE_STONE:
			{
				LegendaryStonesHandler::RegisterRefine(ch);
				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_CONFIGURATION {} {} {}", "REFINE", REFINE_STONE_ADDITIONAL, REFINE_STONE_ADDITIONAL_COUNT));
				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("LEGENDARY_STONES_OPEN {} {}", "REFINE", REFINE_STONE_COST));
			}
			break;
		}
	}

	// Passive
	void RegisterPassiveRecord(const DWORD& dwVnum, const BYTE& bCount)
	{
		auto iFind = v_PassiveConfiguration.find(dwVnum);
		if (iFind != v_PassiveConfiguration.end())
		{
			sys_log(0, fmt::format("This stone already exist {}!", dwVnum).c_str());
			return;
		}

		v_PassiveConfiguration.emplace(dwVnum, bCount);

		sys_log(0, fmt::format("Record registered sucessfully! ItemVnum: {}, RequiredCount: {}", dwVnum, bCount).c_str());
	}

	void EmitPassiveRequires(LPCHARACTER ch, bool bClear, const int iNum)
	{
		if (!ch)
			return;

		if (bClear)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastClearInterface, "PASSIVE"));
		}

		if (iNum > -1)
		{
			auto fIt = v_PassiveConfiguration.find(iNum);
			if (fIt == v_PassiveConfiguration.end())
				return;

			auto iT = std::distance(v_PassiveConfiguration.begin(), fIt);
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {} {} {}", "PASSIVE", iT, fIt->first, GetPassiveGivedCount(ch, fIt->first), fIt->second));
			return;
		}

		auto iT = 0;
		for (auto& it : v_PassiveConfiguration)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {} {} {}", "PASSIVE", iT, it.first, GetPassiveGivedCount(ch, it.first), it.second));
			iT++;
		}
	}

	BYTE GetPassiveGivedCount(LPCHARACTER ch, DWORD dwVnum)
	{
		return ch->GetQuestFlag(fmt::format(sFlagBase, dwVnum));
	}

	int GivePassiveRequiredItem(LPCHARACTER ch, DWORD dwVnum)
	{
		if (!ch)
			return -1;
		
		auto fIt = v_PassiveConfiguration.find(dwVnum);
		if (fIt != v_PassiveConfiguration.end())
		{
			auto v_gived = GetPassiveGivedCount(ch, dwVnum);

			//We have to check if you already gived all items.
			if (v_gived == fIt->second)
				return -1;

			//Check if you have enough items
			if (ch->CountSpecifyItem(dwVnum) < 1)
				return -2;

			//Remove this item
			ch->RemoveSpecifyItem(dwVnum, 1);

			// Update flag
			ch->SetQuestFlag(fmt::format(sFlagBase, dwVnum), v_gived + 1);

			//Check if we have to up ur level of skill
			auto v_after = GetPassiveGivedCount(ch, dwVnum);
			if (v_after == fIt->second)
			{
				EmitPassiveRequires(ch, false, dwVnum);
				PassiveLevelUp(ch);
				return 0;
			}

			EmitPassiveRequires(ch, false, dwVnum);
		}

		return 0;
	}

	void PassiveLevelUp(LPCHARACTER ch)
	{
		auto v_skillID = SKILL_PASSIVE_CRAFT_STONES;
		auto v_curLevel = ch->GetSkillLevel(v_skillID);

		ch->SetSkillLevel(v_skillID, v_curLevel + 1);
		ch->Save();

		ch->ComputePoints();
		ch->SkillLevelPacket();
	}

	BYTE GetPassivePercent(LPCHARACTER ch)
	{
		DWORD v_skillID = SKILL_PASSIVE_CRAFT_STONES;
		auto v_skillLevel = ch->GetSkillLevel(v_skillID);

		return v_skillLevel == v_PassiveConfiguration.size() ? MAX_PASSIVE_PERCENT : v_skillLevel * PASSIVE_PERCENT_ADDER;
	}

	//Crafting
	void RegisterCraftingRecord(const LegendaryStonesHandler::ECraftTypes& eCraftType, const DWORD& dwVnum, const CountType& ctItem, const uint64_t& iPrice)
	{
		auto iFind = m_CraftingConfiguration.find(eCraftType);
		if (iFind == m_CraftingConfiguration.end())
		{
			sys_log(0, fmt::format("The category does not exist {}!", eCraftType).c_str());
			return;
		}

		SCrafting cData = {};
		cData.dwVnum = dwVnum;
		cData.dwRewardCount = ctItem;
		cData.dwPricePer = iPrice;

		iFind->second.second.push_back(cData);

		sys_log(0, fmt::format("Record registered sucessfully! ItemVnum: {}, RequiredCount: {}, PricePer: {}", dwVnum, ctItem, iPrice).c_str());
	}

	void RegisterCraftingReward(const LegendaryStonesHandler::ECraftTypes& eCraftType, const DWORD& dwVnum)
	{
		auto iFind = m_CraftingConfiguration.find(eCraftType);
		if (iFind == m_CraftingConfiguration.end())
		{
			sys_log(0, fmt::format("The category does not exist {}!", eCraftType).c_str());
			return;
		}

		iFind->second.first = dwVnum;
		sys_log(0, fmt::format("Record registered sucessfully! CraftingCategory: {}, RewardVnum: {}", eCraftType, dwVnum).c_str());
	}

	SCrafting* GetCraftingConfiguration(const LegendaryStonesHandler::ECraftTypes& eCraftType, const uint32_t& dwVnum)
	{
		const auto it = m_CraftingConfiguration.find(eCraftType);
		if (it != m_CraftingConfiguration.end())
		{
			auto fIt = std::find_if(it->second.second.begin(), it->second.second.end(), [&dwVnum](const SCrafting& rItem) {return rItem.dwVnum == dwVnum; });
			if (fIt != it->second.second.end())
			{
				return &(*fIt);
			}
		}

		return nullptr;
	}

	int SetItemCraft(LPCHARACTER ch, const LegendaryStonesHandler::ECraftTypes& eCraftType, const BYTE& bPos, const int& iSlot)
	{
		LPITEM item = (iSlot > iEmptySlotValue) ? ch->GetInventoryItem(iSlot) : nullptr;
		if (bPos > MAX_CRAFTING_COUNT || iSlot < iEmptySlotValue || !ch || !ch->GetDesc())
			return -1;

		if (iSlot > iEmptySlotValue && !item)
			return -1;

		if (iSlot > iEmptySlotValue)
		{
			// Check if pointed item belongs to whitelist
			auto pConfiguration = GetCraftingConfiguration(eCraftType, item->GetVnum());
			if (!pConfiguration)
				return -2;

			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {} {} {}", eCraftType == LegendaryStonesHandler::ECraftTypes::CRAFT_TYPE_MINERALS ? "MINERALS" : "SHARDS", bPos, iSlot, pConfiguration->dwRewardCount, pConfiguration->dwPricePer));
			return 0;
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {} {} {}", eCraftType == LegendaryStonesHandler::ECraftTypes::CRAFT_TYPE_MINERALS ? "MINERALS" : "SHARDS", bPos, iSlot, 0, 0));
		return 0;
	}

	int CraftItem(LPCHARACTER ch, const LegendaryStonesHandler::ECraftTypes& eCraftType, const WORD& wSlot)
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		if (wSlot >= INVENTORY_MAX_NUM)
		{
			return -1;
		}
#else
		if ((wSlot >= INVENTORY_MAX_NUM && wSlot < SPECIAL_STORAGE_START_CELL) || wSlot >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			return -1;
		}
#endif

		LPITEM item = ch->GetInventoryItem(wSlot);
		if (!item)
		{
			return -1;
		}

		if (item->GetType() != ITEM_METIN)
		{
			return -1;
		}
		
		auto pConfiguration = GetCraftingConfiguration(eCraftType, item->GetVnum());
		if (!pConfiguration)
			return -3;

		const auto craftCount = pConfiguration->dwRewardCount;
		const auto craftPrice = pConfiguration->dwPricePer;

		if (craftCount <= 0)
		{
			return -1;
		}

		auto totalCount = craftCount * item->GetCount();
		auto totalPrice = craftPrice * item->GetCount();

		if (ch->GetGold() < static_cast<int64_t>(totalPrice))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Not enough money");
			return -2;
		}

		ITEM_MANAGER::instance().RemoveItem(item, "CRAFT_STONES");
		ch->PointChange(POINT_GOLD, -static_cast<int64_t>(totalPrice));

		auto iFind = m_CraftingConfiguration.find(eCraftType);
		if (iFind == m_CraftingConfiguration.end())
		{
			sys_log(0, fmt::format("The category does not exist! {}", eCraftType).c_str());
			return -1;
		}

		DWORD iRewardVnum = iFind->second.first;
		if (iRewardVnum == 0)
		{
			sys_log(0, fmt::format("The reward of category {}, setted to {}", eCraftType, iRewardVnum).c_str());
			return -1;
		}

		ch->AutoGiveItem(iRewardVnum, totalCount);

		return 0;
	}
	
	// Exchange Private
	void RegisterExchangeAdditional(const DWORD& dwVnum, const CountType& iCount)
	{
		auto iFind = std::find_if(v_exchange_additional.begin(), v_exchange_additional.end(), [&dwVnum](const std::pair<DWORD, CountType>& rElement) { return (rElement.first == dwVnum); });
		if (iFind != v_exchange_additional.end())
		{
			sys_log(0, fmt::format("This additional item already exist {}!", dwVnum).c_str());
			return;
		}

		v_exchange_additional.push_back(std::make_pair(dwVnum, iCount));
	}

	void ExchangeClearUp(LPCHARACTER ch)
	{
		auto fIt = m_exchange_stones.find(ch->GetPlayerID());
		// Remove materials
		for (auto & iSlot : fIt->second)
		{
			if (LPITEM item = ch->GetInventoryItem(iSlot))
				item->SetCount(item->GetCount()-1);

			iSlot = iEmptySlotValue;
		}

		// Remove additional items
		for (auto& rItem : v_exchange_additional)
		{
			ch->RemoveSpecifyItem(rItem.first, rItem.second);
		}

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastClearInterface, "STONES"));
	}

	bool IsExchangeRegistered(LPCHARACTER ch)
	{
		return (m_exchange_stones.find(ch->GetPlayerID()) != m_exchange_stones.end());
	}

	DWORD GetExchangeReward(LPCHARACTER ch)
	{
		auto fIt = m_exchange_stones.find(ch->GetPlayerID());
		if (fIt == m_exchange_stones.end())
			return 0;

		auto & rSlotsArray = fIt->second;
		std::array<DWORD, MAX_STONE_COUNT> a_stones;

		for (unsigned int i = 0; i < rSlotsArray.size(); ++i)
			a_stones[i] = ch->GetInventoryItem(rSlotsArray[i]) ? ch->GetInventoryItem(rSlotsArray[i])->GetVnum() : 0;

		// If items do not match array list or are not within a range, return an error
		if (std::count_if(a_stones.begin(), a_stones.end(), [](const DWORD dwVnum) { return (dwVnum && us_exchange_stones.find(dwVnum) != us_exchange_stones.end()); }) != MAX_STONE_COUNT)
			return 0;

		// Establish chance by stone types
		std::vector<std::pair<DWORD, BYTE>> v_chances;
		std::for_each(a_stones.begin(), a_stones.end(), [&v_chances, &a_stones](const DWORD dwVnum) { v_chances.emplace_back(dwVnum, std::count(a_stones.begin(), a_stones.end(), dwVnum)*10); } );
		std::unique(v_chances.begin(), v_chances.end());
		std::sort(v_chances.begin(), v_chances.end(), [](const std::pair<DWORD, BYTE> & l, const std::pair<DWORD, BYTE> & r) { return l.second > r.second; });

		// Iterate through vector
		for (const auto & rPair : v_chances)
		{
			if (number(1, 100) >= rPair.second)
				return rPair.first + STONE_EXCHANGE_ADDER;
		}

		// If no item matched dice, get some random stuff
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(v_chances.begin(), v_chances.end(), g);

		return v_chances[0].first + STONE_EXCHANGE_ADDER;
	}

	// Exchange Public
	void RegisterExchange(LPCHARACTER ch)
	{
		decltype(m_exchange_stones)::mapped_type val;
		val.fill(iEmptySlotValue);
		m_exchange_stones.emplace(ch->GetPlayerID(), std::move(val));
	}

	void UnregisterExchange(LPCHARACTER ch)
	{
		m_exchange_stones.erase(ch->GetPlayerID());
	}

	bool CanExchange(LPCHARACTER ch)
	{
		if (!ch || !ch->GetDesc() || !IsExchangeRegistered(ch))
			return false;

		auto fIt = m_exchange_stones.find(ch->GetPlayerID());
		return std::count_if(fIt->second.begin(), fIt->second.end(), [] (const int iSlot) { return iSlot > iEmptySlotValue; });
	}

	int SetExchangeItem(LPCHARACTER ch, BYTE bPos, int iSlot)
	{
		LPITEM item = (iSlot > iEmptySlotValue) ? ch->GetInventoryItem(iSlot) : nullptr;
		if (bPos >= MAX_STONE_COUNT || iSlot < iEmptySlotValue || !ch || !ch->GetDesc() || !IsExchangeRegistered(ch))
			return -1;

		if (iSlot > iEmptySlotValue && !item)
			return -1;

		auto fIt = m_exchange_stones.find(ch->GetPlayerID());
		if (iSlot > iEmptySlotValue)
		{
			// Check if pointed item belongs to whitelist
			if (us_exchange_stones.find(item->GetVnum()) == us_exchange_stones.end())
				return -2;

			// Check for duplication records
			if (std::find(fIt->second.begin(), fIt->second.end(), iSlot) != fIt->second.end())
				return -3;
		}

		(fIt->second)[bPos] = iSlot;
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {}", "STONES", bPos, iSlot));

		return 0;
	}

	int StartExchange(LPCHARACTER ch)
	{
		if (!ch || !ch->GetDesc() || !IsExchangeRegistered(ch) || !CanExchange(ch))
			return -1;

		// Checking if we have additional items
		for (const auto& rItem : v_exchange_additional)
		{
			if (ch->CountSpecifyItem(rItem.first) < rItem.second)
			{
				return -2;
				break;
			}
		}

		DWORD dwResult = GetExchangeReward(ch);
		if (!dwResult)
			return -3;

		// Delete materials and clear an array
		ExchangeClearUp(ch);
		
		// Calculate success
		const BYTE iPercent = GetPassivePercent(ch);
		if (iPercent < Random::get(0, 100))
			return -4;

		LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(dwResult, 1);

		if (rewardItem)
		{
			// Setting the strength of legendary stone
			rewardItem->SetSocket(0, number(rewardItem->GetValue(3), rewardItem->GetValue(4)));
			if (!ch->AutoGiveItem(rewardItem, true, false))
				M2_DESTROY_ITEM(rewardItem);
		}

		return 0;
	}

	// Refine Private
	void RefineClearUp(LPCHARACTER ch)
	{
		auto fIt = m_refine_stone.find(ch->GetPlayerID());
		// Remove materials
		for (auto& iSlot : fIt->second)
		{
			if (LPITEM item = ch->GetInventoryItem(iSlot))
				item->SetCount(item->GetCount() - 1);

			iSlot = iEmptySlotValue;
		}

		ch->RemoveSpecifyItem(REFINE_STONE_ADDITIONAL, REFINE_STONE_ADDITIONAL_COUNT);
		
		// Remove gold
		ch->PointChange(POINT_GOLD, -REFINE_STONE_COST);

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastClearInterface, "REFINE"));
	}

	bool IsRefineRegistred(LPCHARACTER ch)
	{
		return (m_refine_stone.find(ch->GetPlayerID()) != m_refine_stone.end());
	}

	// Refine Public
	void RegisterRefineRequires(const DWORD& dwVnum, const CountType& iCount, const uint64_t& iPrice)
	{
		REFINE_STONE_ADDITIONAL = dwVnum;
		REFINE_STONE_ADDITIONAL_COUNT = iCount;
		REFINE_STONE_COST = iPrice;
	}

	void RegisterRefine(LPCHARACTER ch)
	{
		decltype(m_refine_stone)::mapped_type val;
		val.fill(iEmptySlotValue);
		m_refine_stone.emplace(ch->GetPlayerID(), std::move(val));
	}

	void UnregisterRefine(LPCHARACTER ch)
	{
		m_refine_stone.erase(ch->GetPlayerID());
	}

	bool CanRefine(LPCHARACTER ch)
	{
		if (!ch || !ch->GetDesc() || !IsRefineRegistred(ch))
			return false;

		auto fIt = m_refine_stone.find(ch->GetPlayerID());
		return std::count_if(fIt->second.begin(), fIt->second.end(), [](const int iSlot) { return iSlot > iEmptySlotValue; });
	}

	int SetRefineItem(LPCHARACTER ch, BYTE bPos, int iSlot)
	{
		LPITEM item = (iSlot > iEmptySlotValue) ? ch->GetInventoryItem(iSlot) : nullptr;
		if (bPos >= MAX_REFINE_STONE_COUNT || iSlot < iEmptySlotValue || !ch || !ch->GetDesc() || !IsRefineRegistred(ch))
			return -1;

		if (iSlot > iEmptySlotValue && !item)
			return -1;

		auto fIt = m_refine_stone.find(ch->GetPlayerID());
		if (iSlot > iEmptySlotValue)
		{
			// Check if pointed item belongs to whitelist
			if (us_refine_stones.find(item->GetVnum()) == us_refine_stones.end())
				return -2;

			// Check for duplication records
			if (std::find(fIt->second.begin(), fIt->second.end(), iSlot) != fIt->second.end())
				return -3;

			// Check if item have max value of bonus
			if (item->GetSocket(0) == item->GetValue(4))
				return -4;

			// We gonna do simple check just in case if we have already any item in records
			for (auto& rItem : fIt->second)
			{
				if (rItem && ch->GetInventoryItem(rItem))
				{
					if (ch->GetInventoryItem(rItem)->GetVnum() != item->GetVnum())
						return -5;
				}
			}
		}

		(fIt->second)[bPos] = iSlot;
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastBroadcastInformation + "{} {}", "REFINE", bPos, iSlot));

		return 0;
	}

	int StartRefine(LPCHARACTER ch)
	{
		if (!ch || !ch->GetDesc() || !IsRefineRegistred(ch) || !CanRefine(ch))
			return -1;

		// Getting current strength and vnum of reward
		auto fIt = m_refine_stone.find(ch->GetPlayerID());
		long iCurrentStrength = ch->GetInventoryItem((fIt->second).at(0)) ? ch->GetInventoryItem((fIt->second).at(0))->GetSocket(0) : 0;
		DWORD dwResult = ch->GetInventoryItem((fIt->second).at(0)) ? ch->GetInventoryItem((fIt->second).at(0))->GetVnum() : 0;
		if (!dwResult)
			return -1;

		// Checking if we have additional item
		if (ch->CountSpecifyItem(REFINE_STONE_ADDITIONAL) < REFINE_STONE_ADDITIONAL_COUNT)
			return -2;

		// Checking if we have enough gold
		if (ch->GetGold() < REFINE_STONE_COST)
			return -3;

		// Delete materials and clear an array
		RefineClearUp(ch);

		LPITEM rewardItem = ITEM_MANAGER::instance().CreateItem(dwResult, 1);

		if (rewardItem)
		{
			// Setting the strength of legendary stone
			rewardItem->SetSocket(0, std::min(Random::get(iCurrentStrength + 1, rewardItem->GetValue(4)), rewardItem->GetValue(4)));
			if (!ch->AutoGiveItem(rewardItem, true, false))
				M2_DESTROY_ITEM(rewardItem);
		}

		return 0;
	}

	// Getters
	bool IsLegendaryStone(const DWORD& dwVnum)
	{
		auto fIt = std::find_if(us_refine_stones.begin(), us_refine_stones.end(), [&dwVnum](const DWORD& iElementVnum) { return (iElementVnum == dwVnum); });
		return (fIt != us_refine_stones.end());
	}
}
#endif
