#include "StdAfx.h"

#ifdef ENABLE_SHOP_SEARCH
#include "PythonShopSearch.h"

CPythonShopSearch::CPythonShopSearch()
{
	m_resultMaxPageNum = 0;
}

CPythonShopSearch::~CPythonShopSearch()
{

}

BYTE CPythonShopSearch::GetResultPageMaxNum() const
{
	return m_resultMaxPageNum;
}

void CPythonShopSearch::SetResultMaxPage(BYTE maxPage)
{
	m_resultMaxPageNum = maxPage;
}

void CPythonShopSearch::ClearResultItems()
{
	m_resultItems.clear();
}

void CPythonShopSearch::AppendResultItem(const TShopSearchClientItem& itemData)
{
	m_resultItems.push_back(itemData);
}

void CPythonShopSearch::RemoveResultItem(const TShopSearchItemID& itemID)
{
	for (auto it = m_resultItems.begin(); it != m_resultItems.end(); ++it)
	{
		if (it->offlineID == itemID)
		{
			m_resultItems.erase(it);
			break;
		}
	}
}

DWORD CPythonShopSearch::GetResultItemMaxNum() const
{
	return m_resultItems.size();
}

const TShopSearchClientItem* CPythonShopSearch::GetResultItem(DWORD index) const
{
	if (index >= GetResultItemMaxNum())
	{
		return NULL;
	}

	return &m_resultItems[index];
}

void CPythonShopSearch::ClearSoldItemInfo()
{
	ZeroMemory(&m_soldItemInfo, sizeof(m_soldItemInfo));
}

void CPythonShopSearch::SetSoldItemInfo(BYTE day, const TShopSearchSoldItemInfo& data)
{
	if (day >= SHOPSEARCH_SOLD_ITEM_INFO_COUNT)
	{
		return;
	}

	memcpy(&m_soldItemInfo[day], &data, sizeof(TShopSearchSoldItemInfo));
}

const TShopSearchSoldItemInfo* CPythonShopSearch::GetSoldItemInfo(BYTE day) const
{
	if (day >= SHOPSEARCH_SOLD_ITEM_INFO_COUNT)
	{
		return NULL;
	}

	return &m_soldItemInfo[day];
}
#endif
