#pragma once
#ifdef __LEGENDARY_STONES_ENABLE__
#include <vector>
#include <unordered_map>

namespace LegendaryStonesHandler
{
	constexpr int START_SOCKET_VALUES = 3;

	enum class EWindowType : int
	{
		WINDOW_TYPE_CRAFT_MINERALS,
		WINDOW_TYPE_CRAFT_SHARDS,
		WINDOW_TYPE_CRAFT_STONES,
		WINDOW_TYPE_REFINE_STONE
	};

	enum class ECraftTypes : int
	{
		CRAFT_TYPE_MINERALS,
		CRAFT_TYPE_SHARDS,
	};

	struct SCrafting
	{
		DWORD dwVnum;
		CountType dwRewardCount;
		uint64_t dwPricePer;
	};

	// Sending Configuration function
	void BroadcastInterface(LPCHARACTER& ch, const LegendaryStonesHandler::EWindowType& eType);

	// Passive skill
	void RegisterPassiveRecord(const DWORD& dwVnum, const BYTE& bCount);

	void EmitPassiveRequires(LPCHARACTER ch, bool bClear = false, const int iNum = -1);
	BYTE GetPassiveGivedCount(LPCHARACTER ch, DWORD dwVnum);
	int GivePassiveRequiredItem(LPCHARACTER ch, DWORD dwVnum);
	void PassiveLevelUp(LPCHARACTER ch);

	BYTE GetPassivePercent(LPCHARACTER ch);

	//Crafting
	void RegisterCraftingRecord(const LegendaryStonesHandler::ECraftTypes& eCraftType, const DWORD& dwVnum, const CountType& ctItem, const uint64_t& iPrice);
	void RegisterCraftingReward(const LegendaryStonesHandler::ECraftTypes& eCraftType, const DWORD& dwVnum);

	SCrafting* GetCraftingConfiguration(const LegendaryStonesHandler::ECraftTypes& eCraftType, const uint32_t& dwVnum);

	int SetItemCraft(LPCHARACTER ch, const LegendaryStonesHandler::ECraftTypes& eCraftType, const BYTE& bPos, const int& iSlot);
	int CraftItem(LPCHARACTER ch, const LegendaryStonesHandler::ECraftTypes& eCraftType, const WORD& wSlot);

	// Exchange
	void RegisterExchangeAdditional(const DWORD& dwVnum, const CountType& iCount);

	void RegisterExchange(LPCHARACTER ch);
	void UnregisterExchange(LPCHARACTER ch);
	bool CanExchange(LPCHARACTER ch);
	int SetExchangeItem(LPCHARACTER ch, BYTE bPos, int wSlot);
	int StartExchange(LPCHARACTER ch);

	// Refine

	void RegisterRefineRequires(const DWORD& dwVnum, const CountType& iCount, const uint64_t& iPrice);

	void RegisterRefine(LPCHARACTER ch);
	void UnregisterRefine(LPCHARACTER ch);

	bool CanRefine(LPCHARACTER ch);
	int  SetRefineItem(LPCHARACTER ch, BYTE bPos, int iSlot);
	int StartRefine(LPCHARACTER ch);

	// Getter
	bool IsLegendaryStone(const DWORD& dwVnum);
}
#endif
