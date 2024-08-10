#include "stdafx.h"

#ifdef __SHOP_SEARCH__
#include "ShopSearchManager.h"
#include "ClientManager.h"
#include <boost/algorithm/string.hpp>

#define IS_SET(flag, bit)		((flag) & (bit))
#define SET_BIT(var, bit)		((var) |= (bit))
#define REMOVE_BIT(var, bit)	((var) &= ~(bit))
#define TOGGLE_BIT(var, bit)	((var) = (var) ^ (bit))

typedef const boost::iterator_range<std::string::const_iterator> StringRange;

CShopSearchManager::CShopSearchManager()
{
}

CShopSearchManager::~CShopSearchManager()
{
}

void CShopSearchManager::Initialize()
{
	m_map_SoldItemInfo.clear();

	char szQuery[QUERY_MAX_LEN];
	snprintf(szQuery, sizeof(szQuery), "DELETE FROM shopsearch_sold_items WHERE DATEDIFF(NOW(), date) >= %d", SHOPSEARCH_SOLD_ITEM_INFO_COUNT);
	std::unique_ptr<SQLMsg> pMsgDelete(CDBManager::Instance().DirectQuery(szQuery));

	snprintf(szQuery, sizeof(szQuery), "SELECT vnum, count, price, DATEDIFF(NOW(), date) FROM shopsearch_sold_items WHERE DATEDIFF(NOW(), date) < %d", SHOPSEARCH_SOLD_ITEM_INFO_COUNT);
	std::unique_ptr<SQLMsg> pMsg(CDBManager::Instance().DirectQuery(szQuery));

	m_currentMonthDay = GetCurrentMonthDay();
	while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
	{
		DWORD vnum;
		str_to_number(vnum, row[0]);
		DWORD count;
		str_to_number(count, row[1]);
		uint64_t price;
		str_to_number(price, row[2]);
		int dateDiff;
		str_to_number(dateDiff, row[3]);

		if (dateDiff < 0)
		{
			sys_err("Initializing CShopSearchManager sold_items: INVALID DATE [vnum %u count %u price %llu dateDiff %d]", vnum, count, price, dateDiff);
			continue;
		}

		AppendSoldItemInfo(vnum, count, price, dateDiff, false);
	}
}

void CShopSearchManager::RecvRegisterItem(CPeer* peer, const TShopSearchItem& data)
{
	RegisterItem(peer, &data, GetTypePair(data.vnum));
}

void CShopSearchManager::RecvUnregisterItem(const TShopSearchItem& data)
{
	UnregisterItem(data.offlineID, GetTypePair(data.vnum));
}

void CShopSearchManager::RecvSoldItem(const TPacketGDShopSearchSoldItem& data)
{
	AppendSoldItemInfo(data.itemVnum, data.itemCount, data.price);
}

void CShopSearchManager::RecvSearchByName(CPeer* peer, DWORD handle, const TPacketGDShopSearchByName& data)
{
	if (data.sortType >= SHOPSEARCH_SORT_MAX_NUM)
	{
		return;
	}

	TSearchResult ret = DoSearch(data.langID, data.itemName, data.page, data.entryCountIdx, data.sortType);
	SendSearchResult(peer, handle, ret);
}

void CShopSearchManager::RecvSearchByOptions(CPeer* peer, DWORD handle, const TPacketGDShopSearchByOptions& data, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList)
{
	if (data.sortType >= SHOPSEARCH_SORT_MAX_NUM)
	{
		return;
	}

	TSearchResult ret = DoSearch(data.options, itemTypeFlags, vnumList, data.page, data.entryCountIdx, data.sortType);
	SendSearchResult(peer, handle, ret);
}

void CShopSearchManager::RecvBuyItem(CPeer* peer, DWORD handle, const TPacketGDShopSearchRequestBuy& data)
{
	TSellingItemMap& itemMap = GetSellingItemMap(GetTypePair(data.itemVnum));
	auto it = itemMap.find(data.offlineID);
	if (it == itemMap.end())
	{
		SendBuyResult(peer, handle, data.ownerID, SHOPSEARCH_BUY_NOT_EXIST, data.itemPrice);
		return;
	}

	const TShopSearchItem& buyItem = it->second;
	if (buyItem.price != data.itemPrice)
	{
		SendBuyResult(peer, handle, data.ownerID, SHOPSEARCH_BUY_PRICE_CHANGE, data.itemPrice);
		return;
	}

	if (buyItem.endTime != 0 && time(0) >= buyItem.endTime)
	{
		SendBuyResult(peer, handle, data.ownerID, SHOPSEARCH_BUY_TIMEOUT, data.itemPrice);
		return;
	}

	auto itPeer = m_ItemToPeer.find(data.offlineID);
	if (itPeer == m_ItemToPeer.end())
	{
		SendBuyResult(peer, handle, data.ownerID, SHOPSEARCH_BUY_NO_PEER, data.itemPrice);
		return;
	}

	CPeer* itemPeer = itPeer->second;

	TPacketDGShopSearchBuyFromShop pack;
	pack.buyerDBHandle = peer->GetHandle();
	pack.buyerPCHandle = handle;
	pack.buyerPID = data.ownerID;
	pack.ownerPID = buyItem.owner;
	pack.offlineID = data.offlineID;
	pack.itemPrice = data.itemPrice;

	itemPeer->EncodeHeader(HEADER_DG_SHOP_SEARCH_BUY_FROM_SHOP, 0, sizeof(TPacketDGShopSearchBuyFromShop));
	itemPeer->Encode(&pack, sizeof(pack));
}

void CShopSearchManager::RecvBuyFromShopError(const TPacketDGShopSearchBuyFromShop& data)
{
	CPeer* peer = CClientManager::Instance().GetPeer(data.buyerDBHandle);
	if (!peer)
	{
		return;
	}

	SendBuyResult(peer, data.buyerPCHandle, data.buyerPID, SHOPSEARCH_BUY_UNKNOWN_ERROR, data.itemPrice);
}

void CShopSearchManager::RecvBoughtFromShop(DWORD handle, const TPacketGDShopSearchBoughtFromShop& data)
{
	CPeer* peer = CClientManager::Instance().GetPeer(data.buyerDBHandle);
	if (!peer)
	{
		return;
	}

	SendBuyResult(peer, handle, data.item.owner, SHOPSEARCH_BUY_SUCCESS, 0, sizeof(TPlayerItem));
	peer->Encode(&data.item, sizeof(TPlayerItem));
}

void CShopSearchManager::RecvRequestSoldInfo(CPeer* peer, DWORD handle, DWORD itemVnum)
{
	auto it = m_map_SoldItemInfo.find(itemVnum);
	if (it == m_map_SoldItemInfo.end())
	{
		peer->EncodeHeader(HEADER_DG_SHOP_SEARCH_SOLD_INFO, handle, sizeof(BYTE));
		peer->EncodeBYTE(false);
		return;
	}

	peer->EncodeHeader(HEADER_DG_SHOP_SEARCH_SOLD_INFO, handle, sizeof(BYTE) + sizeof(it->second));
	peer->EncodeBYTE(true);
	peer->Encode(&it->second, sizeof(it->second));
}

void CShopSearchManager::CleanupPeer(CPeer* peer)
{
	auto it = m_ItemToPeer.begin();
	while (it != m_ItemToPeer.end())
	{
		auto itCur = it++;

		if (itCur->second == peer)
		{
			m_ItemToPeer.erase(itCur);
		}
	}
}

void CShopSearchManager::Update()
{
	if (m_currentMonthDay != GetCurrentMonthDay())
	{
		m_currentMonthDay = GetCurrentMonthDay();

		for (auto it = m_map_SoldItemInfo.begin(); it != m_map_SoldItemInfo.end(); ++it)
		{
			// move all elements one index back
			auto& arr = it->second;
			for (int i = SHOPSEARCH_SOLD_ITEM_INFO_COUNT - 1; i > 0; --i)
			{
				arr[i] = arr[i - 1];
			}

			// cleanup first element for the new current day
			memset(&arr[0], 0, sizeof(TShopSearchSoldItemInfo));
		}
	}
}

CShopSearchManager::TItemTypePair CShopSearchManager::GetTypePair(DWORD itemVnum)
{
	const TItemTable* itemProto = CClientManager::Instance().GetItemTable(itemVnum);
	if (!itemProto)
	{
		return TItemTypePair(ITEM_NONE, 0);
	}

	return TItemTypePair(itemProto->bType, MIN(itemProto->bSubType, ITEM_SUBTYPE_MAX_VALUE));
}

void CShopSearchManager::RegisterItem(CPeer* peer, const TShopSearchItem* itemData, const TItemTypePair& type)
{
	auto& itemMap = GetSellingItemMap(type);
	TShopSearchItem& registerItem = itemMap[itemData->offlineID];

	thecore_memcpy(&registerItem, itemData, sizeof(TShopSearchItem));
	m_ItemToPeer[itemData->offlineID] = peer;
}

void CShopSearchManager::UnregisterItem(const TOfflineItemID& offlineID, const TItemTypePair& type)
{
	auto& itemMap = GetSellingItemMap(type);
	itemMap.erase(offlineID);

	m_ItemToPeer.erase(offlineID);
}

struct search_items_all : std::unary_function<TShopSearchItem, bool>
{
	bool operator()(const CShopSearchManager::TSellingItemMap::value_type& o)
	{
		return true;
	}
};

struct search_items_by_vnum : std::unary_function<TShopSearchItem, bool>
{
	const std::vector<std::pair<DWORD, DWORD> >& m_vnumList;
	search_items_by_vnum(const std::vector<std::pair<DWORD, DWORD> >& vnumList) : m_vnumList(vnumList) {}
	bool operator()(const CShopSearchManager::TSellingItemMap::value_type& o)
	{
		for (const std::pair<DWORD, DWORD>& vnumRange : m_vnumList)
		{
			if (o.second.vnum >= vnumRange.first && o.second.vnum <= vnumRange.second)
			{
				return true;
			}
		}

		return false;
	}
};

template <class _SearchFunc>
void CShopSearchManager::__DoSearch(std::vector<TShopSearchItem*>& result, BYTE& startIdx, BYTE& insertIdx, WORD& curPage, WORD& curItemCount, WORD page, BYTE entryCountIdx, BYTE sortType, TSellingItemMap& searchItems, _SearchFunc func)
{
	static TSellingItemMap::iterator it;
	it = searchItems.begin();

	BYTE maxEntryCount = GetMaxEntryCount(entryCountIdx);

	do
	{
		it = std::find_if(it, searchItems.end(), func);
		if (it != searchItems.end())
		{
			if (sortType == SHOPSEARCH_SORT_RANDOM)
			{
				if (curPage <= page)
				{
					// reset start idx on new page if there is at least one item on that page
					if (curItemCount == 0)
					{
						startIdx = insertIdx;
					}

					// add item into vector
					result[insertIdx++] = &it->second;
					if (insertIdx >= maxEntryCount)
					{
						insertIdx = 0;
					}
					// increase start index if already max items are in the vector
					if (curPage > 0)
					{
						if (++startIdx >= maxEntryCount)
						{
							startIdx = 0;
						}
					}
				}

				if (++curItemCount >= maxEntryCount)
				{
					curItemCount = 0;
					++curPage;
				}
			}
			else
			{
				result.push_back(&it->second);
			}

			// increase iterator to start next search after this element
			it++;
		}
	} while (it != searchItems.end());
}

int ShopSearch_SortItemsAscending(const void* first, const void* second)
{
	const TShopSearchItem* a = (const TShopSearchItem*) * ((const void**) first);
	const TShopSearchItem* b = (const TShopSearchItem*) * ((const void**) second);

	if (a->count < b->count)
	{
		return -1;
	}
	else if (a->count > b->count)
	{
		return 1;
	}

	return 0;
}
int ShopSearch_SortItemsDescending(const void* first, const void* second)
{
	return -ShopSearch_SortItemsAscending(first, second);
}

void CShopSearchManager::__DoSort(std::vector<TShopSearchItem*>& result, BYTE sortType)
{
	switch (sortType)
	{
	case SHOPSEARCH_SORT_ASC:
		std::qsort(&result[0], result.size(), sizeof(TShopSearchItem*), ShopSearch_SortItemsAscending);
		break;

	case SHOPSEARCH_SORT_DESC:
		std::qsort(&result[0], result.size(), sizeof(TShopSearchItem*), ShopSearch_SortItemsDescending);
		break;
	}
}

BYTE CShopSearchManager::GetMaxEntryCount(BYTE index)
{
	static const BYTE ITEMS_PER_PAGE[] = { ITEMS_PER_PAGE1, ITEMS_PER_PAGE2, ITEMS_PER_PAGE3 };
	static const BYTE maxIdx = sizeof(ITEMS_PER_PAGE) / sizeof(ITEMS_PER_PAGE[0]);

	if (index >= maxIdx)
	{
		index = 0;
	}

	return ITEMS_PER_PAGE[index];
}

CShopSearchManager::TSearchResult CShopSearchManager::DoSearch(BYTE langID, const std::string& itemName, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	// collect vnum list by itemName
	std::vector<std::pair<DWORD, DWORD> > itemVnumList;
	if (!itemName.empty())
	{
		StringRange itemNameRange(itemName.begin(), itemName.end());
		auto nameMap = CClientManager::Instance().GetItemNames(langID);
		for (auto it = nameMap->begin(); it != nameMap->end(); it++)
		{
			if (boost::ifind_first(StringRange(it->second.begin(), it->second.end()), itemNameRange))
			{
				itemVnumList.push_back(std::make_pair(it->first, it->first));
			}
		}
	}

	// collect items
	static std::vector<TShopSearchItem*> s_resItems;
	if (sortType == SHOPSEARCH_SORT_RANDOM)
	{
		s_resItems.resize(GetMaxEntryCount(entryCountIdx));
	}
	else
	{
		s_resItems.clear();
	}

	static BYTE s_startIndex, s_insertIndex;

	s_startIndex = 0;
	s_insertIndex = 0;
	WORD curPage = 0;
	WORD curItemCount = 0;

	search_items_by_vnum searchFunc(itemVnumList);
	for (int itemType = 0; itemType < ITEM_TYPE_MAX_NUM; ++itemType)
	{
		for (int itemSubType = 0; itemSubType < ITEM_SUBTYPE_MAX_VALUE; ++itemSubType)
		{
			TSellingItemMap& items = m_SellingItems[itemType][itemSubType];
			__DoSearch(s_resItems, s_startIndex, s_insertIndex, curPage, curItemCount, page, entryCountIdx, sortType, items, searchFunc);
		}
	}

	BYTE maxPage;
	if (sortType == SHOPSEARCH_SORT_RANDOM)
	{
		if (curPage <= page)
		{
			if (curItemCount < GetMaxEntryCount(entryCountIdx))
			{
				s_resItems[s_insertIndex] = NULL;
			}
		}

		maxPage = MAX(1, curItemCount > 0 ? curPage + 1 : curPage);
	}
	else
	{
		maxPage = (MAX(1, s_resItems.size()) - 1) / GetMaxEntryCount(entryCountIdx) + 1;

		if (page >= maxPage)
		{
			page = maxPage - 1;
		}

		__DoSort(s_resItems, sortType);
		s_startIndex = page * GetMaxEntryCount(entryCountIdx);

		// add NULL so the search result will detect the end
		if (s_resItems.size() < s_startIndex + GetMaxEntryCount(entryCountIdx))
		{
			s_resItems.push_back(NULL);
		}
	}

	return TSearchResult(&s_resItems, s_startIndex, maxPage, entryCountIdx, sortType);
}

CShopSearchManager::TSearchResult CShopSearchManager::DoSearch(const TShopSearchOptions& searchOptions, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList, WORD page, BYTE entryCountIdx, BYTE sortType)
{
	// order basic item types
	std::unordered_map<DWORD, DWORD> mapBasicTypes;
	for (const TShopSearchItemType* itemTypeFlag = itemTypeFlags; itemTypeFlag < itemTypeFlags + searchOptions.typeFlagCount; ++itemTypeFlag)
	{
		mapBasicTypes[itemTypeFlag->first] = itemTypeFlag->second;
	}

	// collect extra types + subtypes by the specific vnumList
	std::vector<std::pair<DWORD, DWORD> > vecVnumRangeList;
	std::unordered_map<DWORD, std::unordered_set<DWORD> > mapExtraTypes;
	for (const DWORD* pVnum = vnumList; pVnum < vnumList + searchOptions.specificVnumCount; ++pVnum)
	{
		DWORD curVnumStart = *pVnum;
		DWORD curVnumEnd = curVnumStart;

		DWORD rangeFlagBit = 1 << (sizeof(DWORD) * 8 - 1);
		if (IS_SET(curVnumStart, rangeFlagBit))
		{
			if (pVnum + 1 >= vnumList + searchOptions.specificVnumCount)
			{
				sys_err("invalid vnum range input from client");
				continue;
			}

			curVnumEnd = *(++pVnum);
			REMOVE_BIT(curVnumStart, rangeFlagBit);
			REMOVE_BIT(curVnumEnd, rangeFlagBit);

			if (curVnumEnd <= curVnumStart)
			{
				sys_err("invalid vnum range input from client (curVnumStart %u curVnumEnd %u)", curVnumStart, curVnumEnd);
				continue;
			}
		}

		for (DWORD curVnum = curVnumStart; curVnum <= curVnumEnd; ++curVnum)
		{
			const TItemTable* pProto = CClientManager::Instance().GetItemTable(curVnum);
			if (!pProto)
			{
				continue;
			}

			mapExtraTypes[pProto->bType].insert(MIN(pProto->bSubType, ITEM_SUBTYPE_MAX_VALUE));
		}

		vecVnumRangeList.push_back(std::make_pair(curVnumStart, curVnumEnd));
	}

	// collect items
	static std::vector<TShopSearchItem*> s_resItems;
	if (sortType == SHOPSEARCH_SORT_RANDOM)
	{
		s_resItems.resize(GetMaxEntryCount(entryCountIdx));
	}
	else
	{
		s_resItems.clear();
	}
	static BYTE s_startIndex, s_insertIndex;

	s_startIndex = 0;
	s_insertIndex = 0;
	WORD curPage = 0;
	WORD curItemCount = 0;

	search_items_by_vnum searchVnumFunc(vecVnumRangeList);
	search_items_all searchAllFunc;

	for (int itemType = 0; itemType < ITEM_TYPE_MAX_NUM; ++itemType)
	{
		// check if the itemType is in the basicTypes or extraTypes
		auto basicTypeIt = mapBasicTypes.find(itemType);
		if (basicTypeIt == mapBasicTypes.end() &&
				mapExtraTypes.find(itemType) == mapExtraTypes.end())
		{
			continue;
		}

		for (int itemSubType = 0; itemSubType < ITEM_SUBTYPE_MAX_VALUE; ++itemSubType)
		{
			TSellingItemMap& items = m_SellingItems[itemType][itemSubType];

			// not in basic type -> find in specific vnum list
			if (basicTypeIt == mapBasicTypes.end() || !IS_SET(basicTypeIt->second, 1 << itemSubType))
			{
				auto it = mapExtraTypes.find(itemType);
				if (it == mapExtraTypes.end() || it->second.find(itemSubType) == it->second.end())
				{
					continue;
				}

				__DoSearch(s_resItems, s_startIndex, s_insertIndex, curPage, curItemCount, page, entryCountIdx, sortType, items, searchVnumFunc);
			}
			// is basic type
			else
			{
				__DoSearch(s_resItems, s_startIndex, s_insertIndex, curPage, curItemCount, page, entryCountIdx, sortType, items, searchAllFunc);
			}
		}
	}

	BYTE maxPage;
	if (sortType == SHOPSEARCH_SORT_RANDOM)
	{
		if (curPage <= page)
		{
			if (curItemCount < GetMaxEntryCount(entryCountIdx))
			{
				s_resItems[s_insertIndex] = NULL;
			}
		}

		maxPage = MAX(1, curItemCount > 0 ? curPage + 1 : curPage);
	}
	else
	{
		maxPage = (MAX(1, s_resItems.size()) - 1) / GetMaxEntryCount(entryCountIdx) + 1;

		if (page >= maxPage)
		{
			page = maxPage - 1;
		}

		__DoSort(s_resItems, sortType);
		s_startIndex = page * GetMaxEntryCount(entryCountIdx);

		// add NULL so the search result will detect the end
		if (s_resItems.size() < s_startIndex + GetMaxEntryCount(entryCountIdx))
		{
			s_resItems.push_back(NULL);
		}
	}

	return TSearchResult(&s_resItems, s_startIndex, curItemCount > 0 ? curPage + 1 : curPage, entryCountIdx, sortType);
}

BYTE CShopSearchManager::GetAvgPriceLevel(DWORD vnum, int64_t price)
{
	auto avgIt = m_map_SoldItemInfo.find(vnum);
	if (avgIt != m_map_SoldItemInfo.end())
	{
		double avgPrice = 0.0;
		int avgPriceCnt = 0;

		for (int i = 0; i < SHOPSEARCH_SOLD_ITEM_INFO_COUNT; ++i)
		{
			double curAvgPrice = avgIt->second[i].averagePrice;

			if (curAvgPrice > 0.0 && avgIt->second[i].count > 0)
			{
				avgPriceCnt += 1;
				avgPrice += (curAvgPrice - avgPrice) / (double)avgPriceCnt;
			}
		}

		if (avgPriceCnt > 0)
		{
			double priceDifference = price / avgPrice * 100.0;

			// price is lower (or equal) than 90% of the average price
			if (priceDifference <= 90.0)
			{
				return SHOPSEARCH_AVG_PRICE_GOOD;
			}
			// price is higher (or equal) than 120% of the average price
			else if (priceDifference >= 120.0)
			{
				return SHOPSEARCH_AVG_PRICE_WORST;
			}
			// price is higher (or equal) than 110% and lower than 115% of the average price
			else if (priceDifference >= 110.0)
			{
				return SHOPSEARCH_AVG_PRICE_BAD;
			}
		}
	}

	return SHOPSEARCH_AVG_PRICE_NORMAL;
}

void CShopSearchManager::SendSearchResult(CPeer* peer, DWORD handle, const TSearchResult& result)
{
	BYTE maxEntryCount = GetMaxEntryCount(result.entryCountIdx);

	std::vector<TShopSearchClientItem> sendItems;
	sendItems.reserve(maxEntryCount);

	for (int i = 0; i < maxEntryCount; ++i)
	{
		int index = result.startIndex + i;
		if (index >= maxEntryCount && result.sortType == SHOPSEARCH_SORT_RANDOM) { index -= maxEntryCount; }

		const TShopSearchItem* curItem = (*result.items)[index];
		if (!curItem)
		{
			break;
		}

		TShopSearchClientItem addingItem;
		thecore_memcpy(&addingItem, curItem, sizeof(TShopSearchItem));
		addingItem.avgPriceLevel = GetAvgPriceLevel(curItem->vnum, curItem->price);
		sendItems.push_back(addingItem);
	}

	peer->EncodeHeader(HEADER_DG_SHOP_SEARCH_RESULT, handle, sizeof(WORD) + sizeof(WORD) + sizeof(TShopSearchClientItem) * sendItems.size());
	peer->EncodeWORD(result.maxPageNum);
	peer->EncodeWORD(sendItems.size());
	peer->Encode(&sendItems[0], sizeof(TShopSearchClientItem) * sendItems.size());
}

void CShopSearchManager::SendBuyResult(CPeer* peer, DWORD handle, DWORD ownerID, BYTE resultType, int64_t itemPrice, WORD extraSize)
{
	peer->EncodeHeader(HEADER_DG_SHOP_SEARCH_BUY_RESULT, handle, sizeof(DWORD) + sizeof(BYTE) + sizeof(int64_t) + extraSize);
	peer->EncodeDWORD(ownerID);
	peer->EncodeBYTE(resultType);
	peer->Encode(&itemPrice, sizeof(int64_t));
}

void CShopSearchManager::AppendSoldItemInfo(DWORD vnum, DWORD soldCount, uint64_t price, BYTE daysAgo, bool addSQL)
{
	if (soldCount == 0)
	{
		return;
	}

	TShopSearchSoldItemInfo& info = m_map_SoldItemInfo[vnum][daysAgo];
	if (info.count == 0)
	{
		info.count = soldCount;
		info.averagePrice = (double)price / (double)soldCount;
	}
	else
	{
		info.count += soldCount;
		info.averagePrice += ((double)price - (double)info.averagePrice) / (double)info.count;
	}

	if (addSQL)
	{
		char szQuery[1024];
		snprintf(szQuery, sizeof(szQuery), "INSERT INTO shopsearch_sold_items (vnum, count, price, date) VALUES (%u, %u, %llu, NOW())", vnum, soldCount, price);
		CDBManager::Instance().AsyncQuery(szQuery);
	}
}

int CShopSearchManager::GetCurrentMonthDay()
{
	time_t now = time(NULL);
	return localtime(&now)->tm_mday;
}

#endif
