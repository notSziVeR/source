#pragma once
#include <unordered_map>

#ifdef __GAYA_SHOP_SYSTEM__

class CHARACTER;

static constexpr int GAYA_SLOT_SIZE = 9;
static constexpr int NEXT_ROTATION_TIME = 60 * 60 * 3;

using tGayaItem = std::tuple<BYTE, DWORD, WORD, long long>;
using aGayaItems = std::array<tGayaItem, GAYA_SLOT_SIZE>;
using GayaGuest = std::tuple<std::vector<tGayaItem>, size_t>;
using tGayaGuestIterator = std::unordered_map<DWORD, GayaGuest>::iterator;

class CGayaSystemManager : public singleton<CGayaSystemManager>
{
	const long long GAYA_CRAFTING_COST = 5000LL;
	const int GAYA_CRAFTING_CHANCE = 100;
	const int GAYA_DEFAULT_UNLOCKED_COUNT = 3;
	const DWORD GAYA_UNLOCK_ITEM = 39064;
	const DWORD GAYA_ROTATION_ITEM = 39063;
	const DWORD GAYA_CRAFTING_ITEM[2] = { 50926, 10 };
	const std::vector<DWORD> GAYA_STONES_LIST = { 50927 };

public:
	CGayaSystemManager();
	~CGayaSystemManager();
	void Destroy();

public:
	void AddGayaGuest(LPCHARACTER ch);
	void RemoveGayaGuest(LPCHARACTER ch);
	int PurchaseItem(LPCHARACTER ch, const BYTE& iSlotID);
	void UpdateGayaSlotStatus(LPCHARACTER ch, const BYTE& iSlotID, const bool& bStatus);
	int CraftItem(LPCHARACTER ch, const WORD& iSlotID);
	int UnlockGayaSlot(LPCHARACTER ch, const WORD& iSlotID, const WORD& iUnlockItemID);
	int RequestGayaRotation(LPCHARACTER ch);
	void CycleRotation();

	// P2P
public:
	void BroadcastGayaRecord(tGayaGuestIterator it, LPDESC inpt_d = nullptr);
	void EraseGayaRecord(const DWORD& dwPID);
	void RequestGayaRecords();
	void BroadcastAllGayaRecords(LPDESC inpt_d);
	void RegisterGayaRecord(LPDESC inpt_d, TPacketGGGayaInfo* rec);

private:
	void LoadGayaItems();
	tGayaGuestIterator FindGayaGuest(LPCHARACTER ch);
	tGayaGuestIterator FindGayaGuest(const DWORD& dwPID);
	tGayaGuestIterator RegisterGayaAttender(LPCHARACTER ch);
	void AddGayaItem(LPCHARACTER ch, tGayaItem&& gItem);
	void SendGayaItemList(LPCHARACTER ch);
	void RollGayaItems(LPCHARACTER ch);
	void EraseGayaItems(LPCHARACTER ch);
	void EraseGayaItems(const DWORD& dwPID);
	void OpenGayaInterface(LPCHARACTER ch);
	void PullGayaRotation(LPCHARACTER ch);
	std::array<tGayaItem, GAYA_SLOT_SIZE> GetRandomItems(LPCHARACTER ch);
	bool GetGayaSlotStatus(LPCHARACTER ch, const BYTE& iSlotID);

private:
	std::unordered_map<DWORD, GayaGuest> m_gaya_guests;
	std::unordered_map<BYTE, std::vector<tGayaItem>> m_gaya_items;

	LPEVENT	m_pkGayaRotationEvent;
};
#endif
