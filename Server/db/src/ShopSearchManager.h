#pragma once

#include "stdafx.h"

#ifdef __SHOP_SEARCH__
#include <unordered_set>
#include <unordered_map>

class CPeer;

class CShopSearchManager : public singleton<CShopSearchManager>
{
public:
	enum EGeneral
	{
		ITEM_SUBTYPE_MAX_VALUE = 10, // maximum expected possible subtypes; all subtypes after this value will be reduced to this value for searching performance reasons, adjust clientside as well
		ITEMS_PER_PAGE1 = 50, // maximum items displayed per page by option1
		ITEMS_PER_PAGE2 = 100, // maximum items displayed per page by option2
		ITEMS_PER_PAGE3 = 150, // maximum items displayed per page by option3
	};

	typedef std::tuple<BYTE, BYTE> TItemTypePair; // pair of itemType and itemSubType
	typedef struct SSearchResult
	{
		SSearchResult(const std::vector<TShopSearchItem*>* resItems, BYTE resStartIndex, WORD resMaxPageNum, BYTE resEntryCountIdx, BYTE resSortType) :
			items(resItems), startIndex(resStartIndex), maxPageNum(resMaxPageNum), entryCountIdx(resEntryCountIdx), sortType(resSortType) {}

		const std::vector<TShopSearchItem*>* items;
		BYTE startIndex;
		WORD maxPageNum;
		BYTE entryCountIdx;
		BYTE sortType;
	} TSearchResult;
	typedef std::unordered_map<TOfflineItemID, TShopSearchItem> TSellingItemMap; // map of selling items with key=itemID

public:
	CShopSearchManager();
	~CShopSearchManager();

public:
	void	Initialize();

	void	RecvRegisterItem(CPeer* peer, const TShopSearchItem& data);
	void	RecvUnregisterItem(const TShopSearchItem& data);
	void	RecvSoldItem(const TPacketGDShopSearchSoldItem& data);

	void	RecvSearchByName(CPeer* peer, DWORD handle, const TPacketGDShopSearchByName& data);
	void	RecvSearchByOptions(CPeer* peer, DWORD handle, const TPacketGDShopSearchByOptions& data, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList);
	void	RecvBuyItem(CPeer* peer, DWORD handle, const TPacketGDShopSearchRequestBuy& data);
	void	RecvBuyFromShopError(const TPacketDGShopSearchBuyFromShop& data);
	void	RecvBoughtFromShop(DWORD handle, const TPacketGDShopSearchBoughtFromShop& data);

	void	RecvRequestSoldInfo(CPeer* peer, DWORD handle, DWORD itemVnum);

	void	CleanupPeer(CPeer* peer);

	void	Update();

private:
	TItemTypePair	GetTypePair(DWORD itemVnum);

	void	RegisterItem(CPeer* peer, const TShopSearchItem* itemData, const TItemTypePair& type);
	void	UnregisterItem(const TOfflineItemID& offlineID, const TItemTypePair& type);

	TSellingItemMap&	GetSellingItemMap(const TItemTypePair& type) { return m_SellingItems[std::get<0>(type)][std::get<1>(type) < ITEM_SUBTYPE_MAX_VALUE ? std::get<1>(type) : ITEM_SUBTYPE_MAX_VALUE - 1]; }

	template <class _SearchFunc> inline
	void			__DoSearch(std::vector<TShopSearchItem*>& result, BYTE& startIdx, BYTE& insertIdx, WORD& curPage, WORD& curItemCount, WORD page, BYTE entryCountIdx, BYTE sortType, TSellingItemMap& searchItems, _SearchFunc func);
	void			__DoSort(std::vector<TShopSearchItem*>& result, BYTE sortType);

	TSearchResult	DoSearch(BYTE langID, const std::string& itemName, WORD page, BYTE entryCountIdx, BYTE sortType);
	TSearchResult	DoSearch(const TShopSearchOptions& searchOptions, const TShopSearchItemType* itemTypeFlags, const DWORD* vnumList, WORD page, BYTE entryCountIdx, BYTE sortType);

	BYTE			GetAvgPriceLevel(DWORD vnum, int64_t price);

	void			SendSearchResult(CPeer* peer, DWORD handle, const TSearchResult& result);
	void			SendBuyResult(CPeer* peer, DWORD handle, DWORD ownerPID, BYTE resultType, int64_t price, WORD extraSize = 0);

	BYTE			GetMaxEntryCount(BYTE index);

	void			AppendSoldItemInfo(DWORD vnum, DWORD soldCount, uint64_t price, BYTE daysAgo = 0, bool addSQL = true);
	int				GetCurrentMonthDay();

private:
	TSellingItemMap	m_SellingItems[ITEM_TYPE_MAX_NUM][ITEM_SUBTYPE_MAX_VALUE];
	std::map<TOfflineItemID, CPeer*> m_ItemToPeer;

	int m_currentMonthDay;
	std::map<DWORD, std::array<TShopSearchSoldItemInfo, SHOPSEARCH_SOLD_ITEM_INFO_COUNT> > m_map_SoldItemInfo;
};
#endif
