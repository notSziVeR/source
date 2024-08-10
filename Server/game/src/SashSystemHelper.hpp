#pragma once
#ifdef __SASH_ABSORPTION_ENABLE__
#include <unordered_map>

namespace SashSystemHelper
{
	/// Local Structs ///
	struct TSashItemRecord
	{
		DWORD id;
		WORD cell;

		TSashItemRecord(const DWORD & _id, const WORD & _cell) : id(_id), cell(_cell)
		{}

		TSashItemRecord(DWORD && _id, WORD && _cell) : id(std::move(_id)), cell(std::move(_cell))
		{}

		TSashItemRecord() : id(0), cell(0)
		{}
	};

	struct TSashRecord
	{
		using TSashArray = std::array<TSashItemRecord, 2>;

		TSashRecord()
		{}

		TSashRecord(const TSashItemRecord & item1, const TSashItemRecord & item2)
		{
			a_records[0] = item1;
			a_records[1] = item2;
		}

		TSashRecord(TSashItemRecord && item1, TSashItemRecord && item2)
		{
			a_records[0] = std::move(item1);
			a_records[1] = std::move(item2);
		}

		const TSashArray & GetItemList()
		{
			return a_records;
		}

		const TSashItemRecord & GetItem(const BYTE & bSlotNum)
		{
			return a_records[bSlotNum];
		}

		int GetFreeSlot()
		{
			int i = 0;
			for (const auto & rItem : GetItemList())
			{
				if (!rItem.id)
					return i;
				else
					++i;
			}

			return -1;
		}

		void UpdateItem(LPCHARACTER ch, const BYTE & iSlotNum, const DWORD & dwID, const WORD & wCell);
		void ReleaseItems(LPCHARACTER ch);

		TSashArray a_records;
	};
	/// END ///

	using TSashMapType = std::unordered_map<DWORD, TSashRecord>;

	/// Enums ///
	enum ESashSystem
	{
		SASH_TYPE_SOCKET = 7,
		SASH_ABSORPTION_SOCKET = 8,

		// Refine Types
		SASH_REFINE_ABSORB_ITEM_NPC = 0,
		SASH_REFINE_ABSORB_ITEM_ITEM = 1,
		SASH_REFINE_ABSORB_ITEM_MAX_NUM = SASH_REFINE_ABSORB_ITEM_ITEM,

		// Types
		SASH_TYPE_NORMAL = 0,
		SASH_TYPE_UNIQUE = 1,
		SASH_TYPE_RARE = 2,
		SASH_TYPE_LEGENDARY = 3,

		// Chances
		SASH_REFINE_CHANCE_ITEM_VALUE = 1,
		SASH_ABSORPTION_CHANCE = 100,
		SASH_UPGRADE_MAX_VALUE = 3,

		// Other
		SASH_ABSORPTION_BASE_VALUE = 5,
		SASH_ABSORPTION_LEGENDARY_MIN = 11,
		SASH_ABSORPTION_LEGENDARY_MAX = 19,
		SASH_ABSORPTION_VALUE_MAX = 25,

		SASH_FEE_BASE_VALUE = 5000000LL,
		SASH_FEE_LEGENDARY_VALUE = 50000000LL,

		// Cases
		SASH_CASE_TYPE_SASH = 0,
		SASH_CASE_TYPE_ITEM = 1,
		SASH_CASE_RESULT_ITEM = 2,

		// Interface
		SASH_INTERFACE_TYPE_COMBINATION = 0,
		SASH_INTERFACE_TYPE_ABSORPTION = 1,

		SASH_ABSORPTION_RELEASE_ITEM = 90000,
	};

	enum class ESashBroadcast : int
	{
		TYPE_COMBINATION_OPEN,
		TYPE_COMBINATION_ITEM_UPDATE,
		TYPE_COMBINATION_COST_UPDATE,
		TYPE_COMBINATION_CLEAR,
		TYPE_COMBINATION_CLOSE,
		TYPE_ABSORPTION_OPEN,
		TYPE_ABSORPTION_ITEM_UPDATE,
		TYPE_ABSORPTION_CLEAR,
		TYPE_ABSORPTION_CLOSE,
		TYPE_INVENTORY_REFRESH,
	};

	enum class ESashChatInfo : int
	{
		INFO_COMBINATION_ITEM_ADDED,
		INFO_COMBINATION_ITEM_REMOVED,
		INFO_COMBINATION_ITEM_NOT_MATCH,
		INFO_COMBINATION_NOT_ENOUGH_MONEY,
		INFO_COMBINATION_PROCESS_SUCCESS,
		INFO_COMBINATION_PROCESS_FAIL,
		INFO_ABSORPTION_ITEM_ADDED,
		INFO_ABSORPTION_ITEM_REMOVED,
		INFO_ABSORPTION_ITEM_NOT_MATCH,
		INFO_ABSORPTION_PROCESS_SUCCESS,
		INFO_ABSORPTION_PROCESS_FAIL,
		INFO_ABSORPTION_REMOVE_SUCCESS,
		INFO_COMBINATION_ITEM_CANNOT_UPGRADE,
	};
	/// END ///

	/// Global Functions ///
	int GetSashFreeSlot(LPCHARACTER ch);
	bool HasAbsorption(LPITEM sash);
	DWORD GetAbsorptionBase(LPITEM sash);
	float GetAbsorptionValue(LPITEM sash);
	int GetSashType(LPITEM sash);
	LPITEM CreateSash(const DWORD & sash_vnum);
	void CreateSash(LPITEM nItem);
	long long ComputeRefineFee(LPITEM sash);
	void ReleasePlayer(const DWORD & pid);
	void ClearUp();
	void OpenSashInterface(LPCHARACTER ch, const BYTE & bIfType);
	void CloseSashInterface(LPCHARACTER ch);
	/// END ///

	/// Combination Functions ///
	void UnregisterSashRefine(LPCHARACTER ch);
	bool RegisterSashRefine(LPCHARACTER ch);
	bool RegisterSashToRefine(LPCHARACTER ch, LPITEM sash, const BYTE & iSlotNum);
	bool EraseSashToRefine(LPCHARACTER ch, const BYTE & iSlotNum);
	bool RefineSash(LPCHARACTER ch);
	/// END ///

	/// Absorption Functions ///
	void UnregisterSashAbsorption(LPCHARACTER ch);
	bool RegisterSashAbsorption(LPCHARACTER ch);
	bool RegisterSashToAbsorption(LPCHARACTER ch, LPITEM item, const BYTE & iSlotNum);
	bool EraseSashToAbsorption(LPCHARACTER ch, const BYTE & iSlotNum);
	bool AbsorbForSash(LPCHARACTER ch);
	void ReleaseSashAbsorption(LPCHARACTER ch, LPITEM sash, LPITEM r_item);
	bool RefineSashAbsorbed(LPCHARACTER ch, LPITEM sash, int iRefineType = -1, bool bReopen = false, LPITEM uItem = nullptr);
	/// END ///

	/// Cointainers ///
	extern TSashMapType m_sash_refine_map;
	extern TSashMapType m_sash_absorption_map;
	/// END ///

	/// Broadcaster ///
	void Sash_Broadcast(LPCHARACTER ch, const ESashBroadcast & broadType, LPITEM item = nullptr, WORD slot = 0, int cost = 0);
	void Sash_ChatInfo(LPCHARACTER ch, const ESashChatInfo & chatType);
	/// END ///
}
#endif

