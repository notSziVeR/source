#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "GameType.h"
#include "Packet.h"

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
	//uint32_t old_item_id;

#ifdef TRANSMUTATION_SYSTEM
	uint32_t transmutation;
#endif

#ifdef ENABLE_REFINE_ELEMENT
	uint32_t refineElement;
#endif

	std::array<int32_t, ITEM_SOCKET_SLOT_MAX_NUM> sockets;
	std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_SLOT_MAX_NUM> attributes;

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
#pragma pack()
};

class CPythonOfflineShop
{
	friend std::unique_ptr<CPythonOfflineShop>::deleter_type;

////////////////////////////////////////
// MANAGEMENT
private:
	static std::map<uint32_t, std::unique_ptr<CPythonOfflineShop>> offlineShops_;
	static PyObject* managerHandler_;

public:
	static CPythonOfflineShop* New(uint32_t id, bool isMyShop, bool isClosed);
	static void Destroy(CPythonOfflineShop* shop);

	static CPythonOfflineShop* Get(uint32_t id);

	static bool ReceivePacket();
	static bool SendPacket(uint8_t header, uint32_t id, const void* data = nullptr, uint32_t size = 0);

	static void SetManagerHandler(PyObject* handler);
	static PyObject* GetManagerHandler();

	static void SendCreatePacket(std::string shopName,
								 uint32_t x, uint32_t y, uint32_t decoRace, uint8_t decoBoard,
								 const std::vector<net_offline_shop::CG_item_info>& items);

	static void SendAddViewerPacket(uint32_t id);

////////////////////////////////////////
// SHOP INSTANCE
private:
	uint32_t id_;
	bool isMyShop_;
	bool isClosed_;

	PyObject* shopHandler_;

private:
	CPythonOfflineShop(uint32_t id, bool isMyShop, bool isClosed);
	~CPythonOfflineShop();

	uint32_t GetId() const;
	bool IsMyShop() const;
	bool IsClosed() const;
	PyObject* GetHandler() const;

	void SendPacket(uint8_t header, const void* data = nullptr, uint32_t size = 0) const;

	// Game to client
	void SetName(const std::string& shopName);

	void AddItem(uint8_t shopPosition, int64_t price, uint32_t vnum, uint32_t count,
#ifdef TRANSMUTATION_SYSTEM
				uint32_t transmutation,
#endif

#ifdef ENABLE_REFINE_ELEMENT
				uint32_t refineElement,
#endif
				 const std::array<int32_t, ITEM_SOCKET_SLOT_MAX_NUM>& sockets,
				 const std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_SLOT_MAX_NUM>& attributes);
	void MoveItem(uint8_t oldShopPosition, uint8_t newShopPosition);
	void RemoveItem(uint8_t shopPosition);

	void SetOpeningTime(uint32_t openingTime);
	void SetPosition(uint8_t channel, uint32_t local_x, uint32_t local_y, uint32_t global_x, uint32_t global_y);
	void SetGold(int64_t gold);

	void Open() const;
	void Handler(std::string functionName, PyObject* args);

public:
	void RegisterHandler(PyObject* handler);

	// Client to game
	void SendReopenPacket() const;
	void SendClosePacket() const;
	void SendRemoveViewerPacket() const;
	void SendBuyItemPacket(uint8_t shopPosition) const;
	void SendChangeNamePacket(const std::string& shopName) const;
	void SendAddItemPacket(uint8_t shopPosition, const TItemPos& inventoryPosition, int64_t price) const;
	void SendMoveItemPacket(uint8_t oldShopPosition, uint8_t newShopPosition) const;
	void SendRemoveItemPacket(uint8_t shopPosition, const TItemPos& inventoryPosition) const;
	void SendWithdrawPacket(int64_t gold) const;
};
