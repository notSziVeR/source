#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <queue>
#include <vector>

#include "../../common/tables.h"
#include "../../libgame/include/grid.h"
#include "item.h"

// Networking packets
namespace net_offline_shop
{
enum EGCHeader
{
	// General
	HEADER_GC_SPAWN,
	HEADER_GC_OPEN_SHOP,
	HEADER_GC_CLOSE_SHOP,

	// Edit shop
	HEADER_GC_NAME,
	HEADER_GC_ADD_ITEM,
	HEADER_GC_MOVE_ITEM,
	HEADER_GC_REMOVE_ITEM,

	// My shop
	HEADER_GC_OPENING_TIME,
	HEADER_GC_GOLD,
	HEADER_GC_POSITION,
};

enum ECGHeader
{
	// Manage my shop
	HEADER_CG_CREATE,
	HEADER_CG_REOPEN,
	HEADER_CG_CLOSE,

	// All shops
	HEADER_CG_ADD_VIEWER,
	HEADER_CG_REMOVE_VIEWER,
	HEADER_CG_BUY_ITEM,

	// Edit shop
	HEADER_CG_CHANGE_NAME,
	HEADER_CG_ADD_ITEM,
	HEADER_CG_MOVE_ITEM,
	HEADER_CG_REMOVE_ITEM,
	HEADER_CG_WITHDRAW,
};

enum EGGHeader
{
	HEADER_GG_SHOP,
	HEADER_GG_CLOSE,
	HEADER_GG_DESTROY,
	HEADER_GG_REOPEN,

	HEADER_GG_NAME,
	HEADER_GG_ADD_ITEM,
	HEADER_GG_MOVE_ITEM,
	HEADER_GG_REMOVE_ITEM,
	HEADER_GG_GOLD,

	HEADER_GG_GIVE_ITEM,
	HEADER_GG_GIVE_GOLD,
	HEADER_GG_REFRESH_GOLD,
};

#pragma pack(1)
struct GC_packet
{
	uint8_t header;
	uint16_t size;

	uint8_t subHeader;

	uint32_t id;
};

struct GC_spawn_packet
{
	uint32_t vid;

	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;

	uint8_t decoBoard;
};

struct GC_open_shop_packet
{
	bool isMyShop;

	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;
	bool isClosed;

	uint32_t itemCount;

	// std::vector<GC_add_item_packet> items;
};

struct GC_name_packet
{
	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;
};

struct GC_add_item_packet
{
	uint32_t vnum;
	uint32_t count;

#ifdef __TRANSMUTATION_SYSTEM__
	uint32_t transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	uint32_t refineElement;
#endif

	std::array<int32_t, ITEM_SOCKET_MAX_NUM> sockets;
	std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM> attributes;

	uint8_t shopPosition;
	int64_t price;
};

struct GC_move_item_packet
{
	uint8_t oldShopPosition;
	uint8_t newShopPosition;
};

struct GC_remove_item_packet
{
	uint8_t shopPosition;
};

struct GC_opening_time_packet
{
	uint32_t openingTime;
};

struct GC_offlineshop_position
{
	uint8_t channel;
	uint32_t local_x;
	uint32_t local_y;
	uint32_t global_x;
	uint32_t global_y;
};

struct GC_gold_packet
{
	int64_t gold;
};

struct CG_item_info
{
	uint8_t shopPosition;
	TItemPos inventoryPosition;
	int64_t price;
};

struct CG_packet
{
	uint8_t header;
	uint8_t subHeader;

	uint32_t id;
};

struct CG_create_packet
{
	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;

	uint32_t x, y;
	uint32_t decoRace;
	uint8_t decoBoard;

	uint32_t itemCount;
	// std::vector<CG_item_info> items;
};

struct CG_buy_item_packet
{
	uint8_t shopPosition;
};

struct CG_change_name_packet
{
	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;
};

struct CG_add_item_packet
{
	CG_item_info itemInfo;
};

struct CG_move_item_packet
{
	uint8_t oldShopPosition;
	uint8_t newShopPosition;
};

struct CG_remove_item_packet
{
	uint8_t shopPosition;
	TItemPos inventoryPosition;
};

struct CG_withdraw_packet
{
	int64_t gold;
};

struct GG_packet
{
	uint8_t header;
	uint8_t subHeader;
	uint32_t extraSize;

	uint32_t id;
};

struct GG_state_packet
{
	bool opened;
	bool destroy;
};

struct GG_name_packet
{
	std::array < char, SHOP_TAB_NAME_MAX + 1 > shopName;
	int32_t name_change_time;
};

struct GG_add_item_packet
{
	uint32_t vnum;
	uint32_t count;

#ifdef __TRANSMUTATION_SYSTEM__
	uint32_t transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	uint32_t refineElement;
#endif

	std::array<int32_t, ITEM_SOCKET_MAX_NUM> sockets;
	std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM> attributes;

	uint8_t shopPosition;
	int64_t price;
};

struct GG_move_item_packet
{
	uint8_t oldShopPosition;
	uint8_t newShopPosition;
};

struct GG_remove_item_packet
{
	uint8_t shopPosition;
};

struct GG_opening_time_packet
{
	uint32_t openingTime;
};

struct GG_gold_packet
{
	int64_t gold;
};

struct GG_give_item_packet
{
	uint32_t pid;

	uint32_t vnum;
	uint32_t count;

#ifdef __TRANSMUTATION_SYSTEM__
	uint32_t transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	uint32_t refineElement;
#endif

	std::array<int32_t, ITEM_SOCKET_MAX_NUM> sockets;
	std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM> attributes;
};

struct GG_give_gold_packet
{
	uint32_t pid;
	int64_t gold;
	uint32_t sid;
};
struct GG_refresh_gold_packet
{
	int64_t gold;
	uint32_t sid;
};
#pragma pack()
};

class COfflineShop
{
public:
	static const uint32_t ITEM_TIME_IDX = 0;
	static const uint32_t ITEM_GOLD_IDX = 1;
	static const uint32_t PROCESS_SHOP_COUNT = 10;
	static const uint32_t CLOSED_RACE = 31100;

	typedef std::shared_ptr<COfflineShop> SharedPtr;
	typedef std::weak_ptr<COfflineShop> WeakPtr;

	typedef std::shared_ptr<TOfflineShopItem> ItemSharedPtr;
	typedef std::weak_ptr<TOfflineShopItem> ItemWeakPtr;

	EVENTINFO(ptr_event_info)
	{
		COfflineShop* shop;
	};

////////////////////////////////////////
// Create shop queue handling waiting
// shop create requests which need new
// ids distributed by the database cache
private:
	struct create_queue_item_info
	{
		uint8_t shopPosition;
		int64_t price;
		CItem* item;
	};

	struct create_queue_info
	{
		uint32_t ownerPid;
		std::string ownerName;
		std::string shopName;
		uint8_t channel;
		uint32_t mapIndex, x, y;
		uint32_t decoRace;
		uint8_t decoBoard;
		uint32_t openingTime;
		std::vector<create_queue_item_info> items;
	};

private:
	static std::map<uint32_t, create_queue_info> createQueue_;

private:
	static void EnqueueCreate(uint32_t ownerPID, const std::string& ownerName,
							  const std::string& shopName, uint8_t channel, uint32_t mapIndex, uint32_t x, uint32_t y,
							  uint32_t decoRace, uint8_t decoBoard, uint32_t openingTime,
							  const std::vector<create_queue_item_info>& itemInfos);

public:
	static void DequeueCreate(uint32_t queueId, uint32_t shopId);
////////////////////////////////////////

////////////////////////////////////////
// Shop manager
private:
	static std::map<uint32_t, SharedPtr> shops_;
	static std::map<uint32_t, std::set<uint32_t> > playerToShops_;

	static std::queue<WeakPtr> openingTimeChecks_;

public:
	static const SharedPtr* Get(uint32_t id);

	static void LoadShop(const TOfflineShop& data, DESC* realListener = nullptr);
	static void DestroyShop(uint32_t id, bool isRealDestroy);
	static uint32_t GetShopCount();

	static std::set<uint32_t>& GetPlayerShops(uint32_t playerId);

	static void Process();

	static void OpenShops();
	static void StartOpeningTimeCheck(const SharedPtr& shop);

	static const std::map<uint32_t, SharedPtr>& GetShops();

	static void Login(uint32_t pid, DESC* desc);
	static void Login(CHARACTER* player);
	static void Logout(uint32_t pid);

	static void DestroyAll();

	// Networking
	static int32_t ReceivePacket(CHARACTER* player, const char* data, uint32_t leftSize);
	static int32_t ReceiveInterprocessPacket(DESC* desc, const char* data);

	static void PeerDisconnect(DESC* peer);
////////////////////////////////////////

private:
	uint32_t id_;

	uint32_t ownerPid_;
	std::string ownerName_;

	std::string shopName_;
	int32_t shopNameChangeTime_;
	uint8_t channel_;
	uint32_t mapIndex_, x_, y_;
	uint32_t decoRace_;
	uint8_t decoBoard_;

	std::map<uint8_t, ItemSharedPtr> items_;
	CGrid itemGrid_;

	uint32_t openingTime_;
	bool openingTimeRunning_;
	int64_t gold_;

	std::set<CHARACTER*> viewers_;

	CHARACTER* keeper_;

	bool skipSave_;

	bool isReal_;
	DESC* realListener_;
	DESC* fakeListener_;

	LPEVENT clearTimedItemEvent_;

public:
	COfflineShop(const TOfflineShop& data, DESC* realListener);
	COfflineShop(uint32_t id, const create_queue_info& createInfo);
	~COfflineShop();

	// TimedItems
	void StartTimedItemEvent();
	bool OnTimedItemEvent();
	void CancelTimedItemEvent();
	void SetFakeGold(int64_t gold) { gold_ = gold; }
	// Items
	bool AddItem(const TOfflineShopItem& item);
	bool AddItem(uint8_t shopPosition, CItem* item, int64_t price);
	void MoveItem(uint8_t oldShopPosition, uint8_t newShopPosition);
	bool RemoveItem(uint8_t shopPosition, bool fromFake = false);
	void BuyItem(CHARACTER* buyer, uint8_t shopPosition);
	LPITEM BuyItem(DWORD buyerPID, uint8_t shopPosition);

	const std::map<uint8_t, ItemSharedPtr>& GetItems() const;
	const ItemSharedPtr& GetItem(uint8_t shopPosition) const; // Always check for item existence using HasItem
	bool HasItem(uint8_t shopPosition) const;
	uint32_t GetItemCount() const;

	// Viewers
	void AddViewer(CHARACTER* player);
	void RemoveViewer(CHARACTER* player);
	void RemoveAllViewers(bool exceptOwner = false);
	bool IsViewer(CHARACTER* player) const;

	// Keeper
	CHARACTER* GetKeeper() const;
	uint32_t GetVID() const;

	void SpawnKeeper();
	void DespawnKeeper();
	bool IsNear(CHARACTER* player) const;

	void Reopen(DWORD itemVnum);
	void RequestClose();
	void Close();
	bool IsClosed() const;

	// Location
	uint8_t GetChannel() const;
	uint32_t GetMapIndex() const;
	uint32_t GetX() const;
	uint32_t GetY() const;

	// Gold
	void SetGold(int64_t gold);
	int64_t GetGold() const;

	// General
	uint32_t GetId() const;
	uint32_t GetOwnerPid() const;
	const std::string& GetOwnerName() const;
	uint32_t GetDecoRace() const;
	uint8_t GetDecoBoard() const;

	void SetOpeningTime(uint32_t openingTime);
	uint32_t GetOpeningTime() const;
#ifdef __SHOP_SEARCH__
	uint32_t GetShopSearchTime() const;
#endif

	void StopOpeningTime();
	void ContinueOpeningTime();

	void SetName(const std::string& shopName, int32_t nameChangeTime);
	const std::string& GetName() const;
	int32_t GetNameChangeTime() const;

	void SendSpawnPacket(CHARACTER* player);

	// Saving
	void SetSkipSave(bool skipSave);
	bool IsSkipSave() const;

	// Fake shop (duplicate on other core for management)
	bool IsReal() const;
	bool HasFakeListener() const;

	void SetFakeListener(DESC* listener);

	void FakeRequestAddItem(uint8_t shopPosition, CItem* item, int64_t price);
	void FakeRequestMoveItem(uint8_t oldShopPosition, uint8_t newShopPosition);
	void FakeRequestRemoveItem(uint8_t shopPosition);

	void FakeRequestClose();
	void FakeRequestWithdrawGold(int64_t gold);
	void FakeRequestChangeName(const std::string& name, int32_t nameChangeTime);

private:
	// Networking
	void SendViewerPacket(uint8_t header, const void* data, uint32_t size, CHARACTER* specificViewer = nullptr, DESC* desc = nullptr) const;
	void SendRealListenerPacket(uint8_t header, const void* data = nullptr, uint32_t size = 0) const;
	void SendFakeListenerPacket(uint8_t header, const void* data = nullptr, uint32_t size = 0) const;

	// Keeper
	void SetKeeper(CHARACTER* keeper);

	// Saving
	void Save() const;
	void SaveItem(uint8_t shopPosition) const;
};
