#pragma once
#ifdef __ENABLE_AMULET_SYSTEM__
#include <unordered_map>

namespace AmuletSystemHelper
{
	// Local structs
	struct sUpgradeInfo
	{
		uint32_t dwVnum;
		CountType count;
	};

	/// Local Structs ///
	struct TAmuletItemRecord
	{
		DWORD id;
		WORD cell;

		TAmuletItemRecord(const DWORD& _id, const WORD& _cell) : id(_id), cell(_cell)
		{}

		TAmuletItemRecord(DWORD&& _id, WORD&& _cell) : id(std::move(_id)), cell(std::move(_cell))
		{}

		TAmuletItemRecord() : id(0), cell(0)
		{}
	};

	struct TAmuletRecord
	{
		using TAmuletArray = std::array<TAmuletItemRecord, 2>;

		TAmuletRecord()
		{}

		TAmuletRecord(const TAmuletItemRecord& item1, const TAmuletItemRecord& item2)
		{
			a_records[0] = item1;
			a_records[1] = item2;
		}

		TAmuletRecord(TAmuletItemRecord&& item1, TAmuletItemRecord&& item2)
		{
			a_records[0] = std::move(item1);
			a_records[1] = std::move(item2);
		}

		const TAmuletArray& GetItemList()
		{
			return a_records;
		}

		const TAmuletItemRecord& GetItem(const BYTE& bSlotNum)
		{
			return a_records[bSlotNum];
		}

		int GetFreeSlot()
		{
			int i = 0;
			for (const auto& rItem : GetItemList())
			{
				if (!rItem.id)
					return i;
				else
					++i;
			}

			return -1;
		}

		void UpdateItem(LPCHARACTER ch, const BYTE& iSlotNum, const DWORD& dwID, const WORD& wCell);
		void ReleaseItems(LPCHARACTER ch);

		TAmuletArray a_records;
	};
	/// END ///

	using TAmuletMapType = std::unordered_map<DWORD, TAmuletRecord>;

	// Enums
	enum EAmuletConfiguration
	{
		AMULET_UPGRADE_SOCKET = 7,

		// Configuration
		AMULET_BASE_BONUS_COUNT = 2,
		AMULET_ADDITIONAL_BONUS_COUNT = 4,
		AMULET_TOTAL_BONUS_COUNT = AMULET_BASE_BONUS_COUNT + AMULET_ADDITIONAL_BONUS_COUNT,

		AMULET_TYPE_VALUE = 0,
		AMULET_MAX_GRADE_VALUE = 1,
		AMULET_GOLD_VALUE = 2,
		AMULET_GOLD_CRAFT_VALUE = 3,
		AMULET_CRAFT_REWARD_COUNT = 4,

		AMULET_ROLL_VNUM = 178101,
		AMULET_LOCK_VNUM = 178102,

		AMULET_DUST_ITEM = 178103,

		// Combination
		AMULET_MAX_POSSIBLE_REFINE_VALUE = 4,

		COMBINE_TYPE_AMULET = 0,
		COMBINE_TYPE_ITEM = 1,
		COMBINE_TYPE_RESULT = 2,
		COMBINE_TYPE_ADDITIONAL = 3,
	};

	/// Global Functions ///
	int GetAmuletFreeSlot(LPCHARACTER ch);
	int GetAmuletType(LPITEM item);
	/// End ///

	// Public functions
	void OnCreate(CItem* item);

	void AmuletUpgradeInfo(LPCHARACTER ch, CItem* item);
	void AmuletUpgrade(LPCHARACTER ch, CItem* item, int iApplyIndex);
	void AmuletRoll(LPCHARACTER ch, CItem* item, BYTE iBit); 

	// Crafting
	int AmuletCraftProcess(LPCHARACTER ch, CItem* item);

	// Combination / Passive
	void BroadcastPassiveInformation(LPCHARACTER ch);

	// Global Functions
	void ReleasePlayer(const DWORD& pid);
	void ClearUp();

	// Opening
	void AmuletCombineOpen(LPCHARACTER ch);
	void AmuletCombineClose(LPCHARACTER ch);
	
	// Register window
	bool RegisterCombine(LPCHARACTER ch);
	void UnregisterCombine(LPCHARACTER ch);

	// Register Combine Item
	bool AmuletCombineRegisterItem(LPCHARACTER ch, LPITEM item, const BYTE& iSlotNum);
	bool AmuletCombineEraseItem(LPCHARACTER ch, const BYTE& iSlotNum);
	
	bool AmuletCombineProcess(LPCHARACTER ch);

	/// Cointainer ///
	extern TAmuletMapType m_amulet_combine_map;
	/// Container ///
}
#endif