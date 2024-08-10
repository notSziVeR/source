#pragma once

#include "ItemData.h"

class CItemManager : public CSingleton<CItemManager>
{
public:
	enum EItemDescCol
	{
		ITEMDESC_COL_VNUM,
		ITEMDESC_COL_NAME,
		ITEMDESC_COL_DESC,
		ITEMDESC_COL_SUMM,
		ITEMDESC_COL_NUM,
	};

	enum EItemScaleCol
	{
		ITEMSCALE_COL_VNUM,
		ITEMSCALE_COL_JOB,
		ITEMSCALE_COL_SEX,
		ITEMSCALE_COL_SCALE_X,
		ITEMSCALE_COL_SCALE_Y,
		ITEMSCALE_COL_SCALE_Z,
		ITEMSCALE_COL_PARTICLE_SCALE,
	};

public:
	typedef std::map<DWORD, CItemData*> TItemMap;
	typedef std::map<std::string, CItemData*> TItemNameMap;
	typedef std::vector<CItemData*> TItemVector;
	typedef std::pair<CItemManager::TItemVector::const_iterator, CItemManager::TItemVector::const_iterator> TItemRange;

#ifdef INGAME_WIKI
public:
	typedef std::vector<CItemData*> TItemVec;
	typedef std::vector<DWORD> TItemNumVec;

public:
	void WikiAddVnumToBlacklist(DWORD vnum)
	{
		auto it = m_ItemMap.find(vnum);
		if (it != m_ItemMap.end())
		{
			it->second->SetBlacklisted(true);
		}
	};

	TItemNumVec* WikiGetLastItems()
	{
		return &m_tempItemVec;
	}
	BOOL CanIncrSelectedItemRefineLevel();
	BOOL CanIncrItemRefineLevel(DWORD itemVnum);
	bool								CanLoadWikiItem(DWORD dwVnum);
	DWORD							GetWikiItemStartRefineVnum(DWORD dwVnum);
	std::string							GetWikiItemBaseRefineName(DWORD dwVnum);
	size_t								WikiLoadClassItems(BYTE classType, DWORD raceFilter);
	std::tuple<const char*, int>	SelectByNamePart(const char * namePart);

protected:
	TItemNumVec m_tempItemVec;

private:
	bool IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter);
#endif

public:
	CItemManager();
	virtual ~CItemManager();

	void			Destroy();

	BOOL			SelectItemData(DWORD dwIndex);
	CItemData *		GetSelectedItemDataPointer();

	BOOL			GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData);
	CItemData* GetProto(DWORD dwItemID);

	/////
	bool			LoadItemDesc(const char* c_szFileName);
	bool			LoadItemList(const char* c_szFileName);
	bool			LoadItemTable(const char* c_szFileName);
	CItemData *		MakeItemData(DWORD dwIndex);

	bool			LoadItemScale(const char* szItemScale);

#ifdef OX_EVENT_SYSTEM_ENABLE
	std::map<DWORD, std::string> GetItemByName(std::string ItemName);
#endif

	TItemRange		GetProtosByName(const char* name) const;

	BYTE			GetApplyTypeByName(const std::string& c_rstApplyName);
	BYTE			GetApplyTypeByNameEquipment(const std::string& c_rstApplyName);
	TItemMap		GetItems() const { return m_ItemMap; }

private:
	void			CreateSortedItemData();

protected:
	TItemMap m_ItemMap;
	//TItemVector  m_vec_ItemRange;

	std::vector<CItemData*>  m_vec_ItemRange;
	CItemData * m_pSelectedItemData;

	TItemVector	m_vecItemDataSorted;

	std::map<std::string, BYTE>	m_applyNameMap;
};
