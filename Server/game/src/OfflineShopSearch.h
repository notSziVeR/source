#pragma once

#include "OfflineShop.h"

#undef min
#undef max

class COfflineShopSearchSession
{
public:
	static const uint8_t ITEMS_PER_PAGE = 10;
	static const uint32_t ITEM_CAN_BUY_IDX = 0;

	using ShopWeakPtr = COfflineShop::WeakPtr;
	using ItemWeakPtr = COfflineShop::ItemWeakPtr;

	using ShopItemPair = std::pair<ShopWeakPtr, ItemWeakPtr>;
	using ItemVector = std::vector<ShopItemPair>;

private:
	template <typename T>
	struct range
	{
		T min, max;

		bool in_range(T val) const
		{
			return val >= min && val <= max;
		}
	};

public:
	typedef struct filter
	{
		uint8_t job;

		uint8_t itemType;
		uint8_t itemSubType;

		range<uint8_t> levelRange;
		range<uint8_t> itemLevelRange;

		range<int64_t> priceRange;

		std::string name;
	} Filter;

private:
	CHARACTER* player_;
	Filter filter_;
	bool canBuy_;
	uint32_t page_;

	ItemVector items_;

public:
	COfflineShopSearchSession(CHARACTER* player, bool canBuy)
		: player_(player), canBuy_(canBuy), page_(1)
	{}

	COfflineShopSearchSession(const COfflineShopSearchSession&) = delete;

	~COfflineShopSearchSession() = default;

	void RequestPage(uint32_t page);
	uint32_t GetPageOffset(uint32_t page) const;
	uint32_t GetPageCount() const;
	uint32_t GetPage() const;

	void Buy(CHARACTER* player, uint32_t shopId, uint8_t itemPosition);

	void SetFilter(const Filter& filter);

	void CollectItems();
	void EraseExpiredItems();

	bool IsValidItem(const std::shared_ptr<TOfflineShopItem>& itemPtr) const;

	bool CheckJob(const TItemTable& table, const uint8_t& job) const;
	bool CheckItemType(const TItemTable& table, const uint8_t& itemType) const;
	bool CheckItemSubType(const TItemTable& table, const uint8_t& itemSubType) const;
	bool CheckLevelRange(const TItemTable& table, const range<uint8_t>& levelRange) const;
	bool CheckItemLevelRange(const TItemTable& table, const range<uint8_t>& itemLevelRange) const;
	bool CheckPriceRange(const int64_t& price, const range<int64_t>& priceRange) const;
	bool CheckName(const TItemTable& table, const std::string& namePattern) const;

	CHARACTER* GetPlayer() const;
	const Filter& GetFilter() const;
	bool CanBuy() const;
	const ItemVector& GetItems() const;
};
