#pragma once

#include "stdafx.h"

#ifdef __ITEM_SHOP__

enum EEditorFlag
{
	EDITOR_FLAG_ADD,
	EDITOR_FLAG_EDIT,
	EDITOR_FLAG_DELETE
};

class CItemShopManager : public singleton<CItemShopManager>
{
public:
	CItemShopManager();
	~CItemShopManager();

	void						Initialize();
	void						LoadItems(DWORD dwCount, TItemShopItem * pItems);
	void						LoadEditors(DWORD dwCount, TItemShopEditor* pEditors);
	TItemShopItem				FindItemByID(DWORD id, DWORD category);
	void						OpenItemShop(LPCHARACTER ch);
	void						SendItem(TItemShopItem item, BYTE header, LPCHARACTER ch = NULL);
	void						SendCoins(LPCHARACTER ch);
	void						BuyItem(LPCHARACTER ch, DWORD id, DWORD category, BYTE count);
	void						ManageItem(LPCHARACTER ch, BYTE flag, TItemShopItem item);
	void						AddItem(BYTE success, BYTE flag, DWORD id, DWORD category);
	void						ReloadItems(LPCHARACTER ch);
	bool						IsViewer(LPCHARACTER ch);
	void						RemoveViewer(LPCHARACTER ch);
	void						ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch = NULL);

private:
	std::vector<TItemShopItem>	m_vec_itemShopItems;
	std::set<LPCHARACTER>		m_set_pkCurrentViewer;
	std::vector<char*>			m_vec_pkEditors;
	TItemShopItem				m_pkItemAdd;
	TItemShopItem				m_pkItemEdit;
	void						AddEditor(char* szName);
	bool						IsEditor(LPCHARACTER ch);
	void						AddViewer(LPCHARACTER ch);
	void						ViewerPacket(const void* c_pData, size_t size);
};
#endif
