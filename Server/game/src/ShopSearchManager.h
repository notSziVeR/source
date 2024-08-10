#pragma once

#include "stdafx.h"

#ifdef __SHOP_SEARCH__
#include "../../common/tables.h"

class CShopSearchManager
{
public:
	// outgoing db
	static void	RegisterShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem, DWORD endTime);
	static void	UnregisterShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem);
	static void OnBuyShopItem(DWORD itemVnum, DWORD itemCount, uint64_t price);
	static void ReloadAverageData();

	static void PC_RequestSearch(LPCHARACTER ch, const std::string& itemName, WORD page, BYTE entryCountIdx, BYTE sortType);
	static void PC_RequestSearch(LPCHARACTER ch, const TShopSearchOptions* options, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList, WORD page, BYTE entryCountIdx, BYTE sortType);
	static void PC_RequestBuy(LPCHARACTER ch, TOfflineItemID itemID, DWORD itemVnum, int64_t itemPrice);
	static void PC_RequestSoldInfo(LPCHARACTER ch, DWORD itemVnum);

	// incoming db
	static void DB_ResultSearch(LPCHARACTER ch, WORD maxPageNum, const TShopSearchClientItem* items, WORD itemCount);
	static void DB_BuyFromShop(const TPacketDGShopSearchBuyFromShop* data);
	static void DB_BuyResult(LPDESC desc, const char* data);
	static void DB_SoldInfo(LPCHARACTER ch, bool hasResults, const TShopSearchSoldItemInfo* results);

private:
	// helping
	static const TShopSearchItem& ConvertOfflineShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem, DWORD endTime);
};
#endif
