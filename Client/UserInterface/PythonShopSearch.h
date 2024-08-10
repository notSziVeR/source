#pragma once

#include "StdAfx.h"

#ifdef ENABLE_SHOP_SEARCH
#include "Packet.h"

class CPythonShopSearch : public singleton<CPythonShopSearch>
{
public:
	CPythonShopSearch();
	~CPythonShopSearch();

	BYTE	GetResultPageMaxNum() const;
	void	SetResultMaxPage(BYTE maxPage);
	void	ClearResultItems();
	void	AppendResultItem(const TShopSearchClientItem& itemData);
	void	RemoveResultItem(const TShopSearchItemID& itemID);

	DWORD	GetResultItemMaxNum() const;
	const TShopSearchClientItem*	GetResultItem(DWORD index) const;

	void	ClearSoldItemInfo();
	void	SetSoldItemInfo(BYTE day, const TShopSearchSoldItemInfo& data);
	const TShopSearchSoldItemInfo*	GetSoldItemInfo(BYTE day) const;

private:
	BYTE	m_resultMaxPageNum;
	std::vector<TShopSearchClientItem> m_resultItems;

	std::array<TShopSearchSoldItemInfo, SHOPSEARCH_SOLD_ITEM_INFO_COUNT>	m_soldItemInfo;
};
#endif
