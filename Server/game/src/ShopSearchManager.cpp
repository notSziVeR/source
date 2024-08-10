#include "stdafx.h"

#ifdef __SHOP_SEARCH__
#include "ShopSearchManager.h"
#include "desc_client.h"
#include "char.h"
#include "desc.h"
#include "OfflineShop.h"
#include "item_manager.h"

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

//////////////////////////////////////////////////////
// OUTGOING DB PACKETS
//////////////////////////////////////////////////////

void CShopSearchManager::RegisterShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem, DWORD endTime)
{
	auto& item = ConvertOfflineShopItem(ownerID, offlineItem, endTime);
	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_REGISTER_ITEM, 0, &item, sizeof(TShopSearchItem));
}

void CShopSearchManager::UnregisterShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem)
{
	auto& item = ConvertOfflineShopItem(ownerID, offlineItem, 0);
	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_UNREGISTER_ITEM, 0, &item, sizeof(TShopSearchItem));
}

void CShopSearchManager::OnBuyShopItem(DWORD itemVnum, DWORD itemCount, uint64_t price)
{
	TPacketGDShopSearchSoldItem pack;
	pack.itemVnum = itemVnum;
	pack.itemCount = itemCount;
	pack.price = price;

	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_SOLD_ITEM, 0, &pack, sizeof(pack));
}

void CShopSearchManager::ReloadAverageData()
{
	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_RELOAD_AVERAGE, 0, NULL, 0);
}

void CShopSearchManager::PC_RequestSearch(LPCHARACTER ch, const std::string& itemName, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	TPacketGDShopSearchByName pack;
	pack.langID = ch->GetLocale();
	strlcpy(pack.itemName, itemName.c_str(), sizeof(pack.itemName));
	pack.page = page;
	pack.entryCountIdx = entryCountIdx;
	pack.sortType = sortType;

	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BY_NAME, ch->GetDesc()->GetHandle(), &pack, sizeof(pack));
}

void CShopSearchManager::PC_RequestSearch(LPCHARACTER ch, const TShopSearchOptions* options, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	TPacketGDShopSearchByOptions pack;
	pack.options = *options;
	pack.page = page;
	pack.entryCountIdx = entryCountIdx;
	pack.sortType = sortType;

	db_clientdesc->DBPacketHeader(HEADER_GD_SHOP_SEARCH_BY_OPTIONS, ch->GetDesc()->GetHandle(),
								  sizeof(pack) +
								  sizeof(TShopSearchItemType) * pack.options.typeFlagCount +
								  sizeof(DWORD) * pack.options.specificVnumCount);

	db_clientdesc->Packet(&pack, sizeof(pack));
	db_clientdesc->Packet(itemTypeFlags, sizeof(TShopSearchItemType) * pack.options.typeFlagCount);
	db_clientdesc->Packet(vnumList, sizeof(DWORD) * pack.options.specificVnumCount);
}

void CShopSearchManager::PC_RequestBuy(LPCHARACTER ch, TOfflineItemID itemID, DWORD itemVnum, int64_t itemPrice)
{
	if (ch->GetGold() < itemPrice)
	{
		return;
	}

	ch->PointChange(POINT_GOLD, -itemPrice);

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(itemPrice) });
#endif

	TPacketGDShopSearchRequestBuy pack;
	pack.ownerID = ch->GetPlayerID();
	pack.offlineID = itemID;
	pack.itemVnum = itemVnum;
	pack.itemPrice = itemPrice;

	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_REQUEST_BUY, ch->GetDesc()->GetHandle(), &pack, sizeof(pack));
}

void CShopSearchManager::PC_RequestSoldInfo(LPCHARACTER ch, DWORD itemVnum)
{
	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_REQUEST_SOLD_INFO, ch->GetDesc()->GetHandle(), &itemVnum, sizeof(DWORD));
}

//////////////////////////////////////////////////////
// INCOMING DB PACKETS
//////////////////////////////////////////////////////

void CShopSearchManager::DB_ResultSearch(LPCHARACTER ch, WORD maxPageNum, const TShopSearchClientItem* items, WORD itemCount)
{
	TPacketGCShopSearchResult pack;
	pack.header = HEADER_GC_SHOP_SEARCH_RESULT;
	pack.size = sizeof(pack) + sizeof(TShopSearchClientItem) * itemCount;
	pack.itemCount = itemCount;
	pack.maxPageNum = maxPageNum;

	ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
	ch->GetDesc()->Packet(items, sizeof(TShopSearchClientItem) * itemCount);
}

void CShopSearchManager::DB_BuyFromShop(const TPacketDGShopSearchBuyFromShop* data)
{
	auto pShopPtr = COfflineShop::Get(data->offlineID.first);
	if (!pShopPtr)
	{
		db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BUY_FROM_SHOP_ERROR, 0, data, sizeof(TPacketDGShopSearchBuyFromShop));
		return;
	}

	auto& shopPtr = *pShopPtr;
	if (!shopPtr->HasItem(data->offlineID.second))
	{
		sys_err("no item itemsPos[%d]", data->offlineID.second);
		db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BUY_FROM_SHOP_ERROR, 0, data, sizeof(TPacketDGShopSearchBuyFromShop));
		return;
	}

	auto& offlineItem = shopPtr->GetItem(data->offlineID.second);
	if (offlineItem->price != data->itemPrice)
	{
		sys_err("invalid price (price %lld recvPrice %lld) itemsPos[%d]", offlineItem->price, data->itemPrice, data->offlineID.second);
		db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BUY_FROM_SHOP_ERROR, 0, data, sizeof(TPacketDGShopSearchBuyFromShop));
		return;
	}

	LPITEM item = shopPtr->BuyItem(data->buyerPID, data->offlineID.second);
	if (!item)
	{
		sys_err("cannot buy item");
		db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BUY_FROM_SHOP_ERROR, 0, data, sizeof(TPacketDGShopSearchBuyFromShop));
		return;
	}

	TPacketGDShopSearchBoughtFromShop pack;
	pack.buyerDBHandle = data->buyerDBHandle;

	ITEM_MANAGER::Instance().GetPlayerItem(item, &pack.item);
	pack.item.owner = data->buyerPID;

	db_clientdesc->DBPacket(HEADER_GD_SHOP_SEARCH_BOUGHT_FROM_SHOP, data->buyerPCHandle, &pack, sizeof(pack));
}

void CShopSearchManager::DB_BuyResult(LPDESC desc, const char* data)
{
	LPCHARACTER ch = desc ? desc->GetCharacter() : NULL;

	DWORD ownerID = *(DWORD*) data;
	data += sizeof(DWORD);
	BYTE resultType = *(BYTE*) data;
	data += sizeof(BYTE);
	int64_t itemPrice = *(int64_t*) data;
	data += sizeof(int64_t);

	if (ch && ch->GetPlayerID() != ownerID)
	{
		ch = NULL;
	}

	if (resultType == SHOPSEARCH_BUY_SUCCESS)
	{
		TPlayerItem* itemData = (TPlayerItem*) data;

		LPITEM item = ITEM_MANAGER::instance().CreateItem(itemData->vnum, itemData->count);
		item->SetAttributes(itemData->aAttr);
		item->SetSockets(itemData->alSockets);
#ifdef __TRANSMUTATION_SYSTEM__
		item->SetTransmutate(itemData->transmutate_id);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		item->SetRefineElement(itemData->dwRefineElement);
#endif

		if (!ch)
		{
			// save item for player so it won't disappear
			item->SetWindow(INVENTORY);
			item->SetCell(NULL, 0);
			ITEM_MANAGER::instance().SaveSingleItem(item);

			return;
		}

		ch->ChatPacket(CHAT_TYPE_INFO, "%s received", item->GetName());
		ch->AutoGiveItem(item);
	}
	else
	{
		if (!ch)
		{
			sys_err("DB_BuyResult: failed: cannot restore %lld gold to player %u [failed: %d]", itemPrice, ownerID, resultType);
			return;
		}

		ch->PointChange(POINT_GOLD, itemPrice);
	}

	if (ch)
	{
		TPacketGCShopSearchBuyResult pack;
		pack.header = HEADER_GC_SHOP_SEARCH_BUY_RESULT;
		pack.result = resultType;

		desc->Packet(&pack, sizeof(pack));
	}
}

void CShopSearchManager::DB_SoldInfo(LPCHARACTER ch, bool hasResults, const TShopSearchSoldItemInfo* results)
{
	TPacketGCShopSearchSoldInfo pack;
	pack.header = HEADER_GC_SHOP_SEARCH_SOLD_INFO;
	pack.size = sizeof(pack);
	pack.results = hasResults;

	if (hasResults)
	{
		pack.size += sizeof(TShopSearchSoldItemInfo) * SHOPSEARCH_SOLD_ITEM_INFO_COUNT;
		ch->GetDesc()->BufferedPacket(&pack, sizeof(pack));
		ch->GetDesc()->Packet(results, sizeof(TShopSearchSoldItemInfo) * SHOPSEARCH_SOLD_ITEM_INFO_COUNT);
	}
	else
	{
		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

//////////////////////////////////////////////////////
// HELPING
//////////////////////////////////////////////////////

const TShopSearchItem& CShopSearchManager::ConvertOfflineShopItem(DWORD ownerID, const TOfflineShopItem& offlineItem, DWORD endTime)
{
	static TShopSearchItem item;
	item.id = 0;
	item.window = RESERVED_WINDOW;
	item.owner = ownerID;
	item.pos = offlineItem.id.second;
	item.vnum = offlineItem.vnum;
	item.count = offlineItem.count;
#ifdef __TRANSMUTATION_SYSTEM__
	item.transmutate_id = offlineItem.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	item.dwRefineElement = offlineItem.refineElement;
#endif

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		item.alSockets[i] = offlineItem.sockets[i];
	}
	thecore_memcpy(item.aAttr, &offlineItem.attributes[0], sizeof(item.aAttr));
	item.offlineID = offlineItem.id;
	item.price = (int64_t)offlineItem.price;
	item.endTime = endTime;
	return item;
}
#endif
