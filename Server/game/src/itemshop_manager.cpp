#include "stdafx.h"
#include "desc_client.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "item.h"
#include "log.h"
#include "buffer_manager.h"
#include "constants.h"
#ifdef __ITEM_SHOP__
#include "itemshop_manager.h"
CItemShopManager::CItemShopManager()
{
}

CItemShopManager::~CItemShopManager()
{
}

void CItemShopManager::Initialize()
{
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP_ITEMS_REQUEST, 0, 0);
	db_clientdesc->DBPacketHeader(HEADER_GD_ITEMSHOP_EDITORS_REQUEST, 0, 0);
}

void CItemShopManager::LoadItems(DWORD dwCount, TItemShopItem * pItems)
{
	for (DWORD i = 0; i < dwCount; ++i)
	{
		m_vec_itemShopItems.push_back(pItems[i]);
		sys_log(0, "Loaded data: vnum [%d], count [%d], price [%d], category [%d], id [%d] ", pItems[i].vnum, pItems[i].count, pItems[i].price, pItems[i].category, pItems[i].id);
	}
}

void CItemShopManager::LoadEditors(DWORD dwCount, TItemShopEditor * pEditors)
{
	for (DWORD i = 0; i < dwCount; ++i)
	{
		char * szName = &pEditors[i].name[0];
		AddEditor(szName);
		sys_log(0, "Loaded editor name[%s] ", szName);
	}
}

TItemShopItem CItemShopManager::FindItemByID(DWORD id, DWORD category)
{
	itertype(m_vec_itemShopItems) it = m_vec_itemShopItems.begin();
	while (it != m_vec_itemShopItems.end())
	{
		TItemShopItem item = *(it++);

		if (item.id == id && item.category == category)
		{
			return item;
		}
	}
	TItemShopItem item;
	return item;
}

void CItemShopManager::OpenItemShop(LPCHARACTER ch)
{

	BYTE isEditor = IsEditor(ch) ? 1 : 0;
	ClientPacket(ITEMSHOP_SUBHEADER_GC_EDITOR, &isEditor, sizeof(BYTE), ch);

	if (!IsViewer(ch))
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_CLEAR, NULL, 0, ch);

		itertype(m_vec_itemShopItems) it = m_vec_itemShopItems.begin();
		while (it != m_vec_itemShopItems.end())
		{
			TItemShopItem item = *(it++);

			SendItem(item, ITEMSHOP_SUBHEADER_GC_ITEM, ch);
		}
	}

	SendCoins(ch);

	ClientPacket(ITEMSHOP_SUBHEADER_GC_OPEN, NULL, 0, ch);

	AddViewer(ch);
}

void CItemShopManager::SendItem(TItemShopItem item, BYTE header, LPCHARACTER ch)
{
	TItemShopItem pack;
	pack.vnum = item.vnum;
	pack.count = item.count;
	pack.price = item.price;
	pack.category = item.category;
	pack.fixed_count = item.fixed_count;
	pack.id = item.id;

	std::memcpy(pack.alSockets, item.alSockets, sizeof(pack.alSockets));
	std::memcpy(pack.aAttr, item.aAttr, sizeof(pack.aAttr));

	ClientPacket(header, &pack, sizeof(TItemShopItem), ch);
}

void CItemShopManager::SendCoins(LPCHARACTER ch)
{
	TPacketGCItemShopCoins pack;
	pack.coins = ch->GetCash();

	ClientPacket(ITEMSHOP_SUBHEADER_GC_COINS, &pack, sizeof(TPacketGCItemShopCoins), ch);
}

void CItemShopManager::BuyItem(LPCHARACTER ch, DWORD id, DWORD category, BYTE count)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) tried to buy item id[%d] category[%d]",
				ch->GetPlayerID(), ch->GetName(), id, category);
		return;
	}

	if (category < 0 || category > 12)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
		return;
	}
	if (ch->GetCash() <= 0 )
	{
		return ;
	}

	TItemShopItem item = FindItemByID(id, category);
	if (item.price < 0)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
		return;
	}

	if (count < 0 || count > 200)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_COUNT, NULL, 0, ch);
		return;
	}

	TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item.vnum);

	if (!pTable)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
		return;
	}

	DWORD itemCount = (item.fixed_count == 1) ? item.count : count;
	DWORD price = (item.fixed_count == 1) ? item.price : itemCount * item.price;

	sys_err("!item.fixed_count: %d, count: %d, pTable->bType == ITEM_COSTUME: %d", !item.fixed_count, count, pTable->bType == ITEM_COSTUME);
	if (!item.fixed_count && count > 0 && count < ITEMSHOP_TIME_MAX_NUM + 1 && pTable->bType == ITEM_COSTUME)
	{
		itemCount = item.count;
		price = item.price + aItemShopTime[count].dwPrice;
	}

	if (ch->GetCash() < price)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_NOT_ENOUGH_COINS, NULL, 0, ch);
		return;
	}

	LPITEM pkItem = ITEM_MANAGER::instance().CreateItem(item.vnum, itemCount, 0, false);
	//LPITEM pkItem = ch->AutoGiveItem(item.vnum, item.count);
	int iEmptyPos;
	if (pkItem->IsDragonSoul())
	{
		iEmptyPos = ch->GetEmptyDragonSoulInventory(pkItem);
	}
	else
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		iEmptyPos = ch->GetEmptyInventory(pkItem->GetSize());
#else
		iEmptyPos = ch->GetEmptyInventory(pkItem->GetSize(), pkItem->GetVirtualWindow());
#endif
	}

	if (iEmptyPos < 0)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_INVENTORY_FULL, NULL, 0, ch);
		M2_DESTROY_ITEM(pkItem);
		return;

	}
	if (pkItem)
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long lSocket = item.alSockets[i];
			if (lSocket)
			{
				pkItem->SetSocket(i, lSocket);
			}
		}

		if (!item.fixed_count && count > 0 && count < ITEMSHOP_TIME_MAX_NUM + 1 && pTable->bType == ITEM_COSTUME)
		{
			sys_err("Bought costume with time %d", time(0) + aItemShopTime[count].dwTime);
			if (pkItem->IsRealTimeItem())
			{
				pkItem->SetSocket(0, time(0) + aItemShopTime[count].dwTime);
			}
		}

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			TPlayerItemAttribute aAttr = item.aAttr[i];
			if (aAttr.bType && aAttr.sValue)
			{
				pkItem->SetAttribute(i, aAttr.bType, aAttr.sValue);
			}
		}

		//LogManager::instance().ItemShopBuyLog(item.vnum, itemCount, id, category, ch->GetDesc()->GetAccountTable().id, price);
		//ch->UpdateCoins(price);
		//SendCoins(ch);
		//ch->AutoGiveItem(pkItem, true);
		LogManager::instance().ItemShopBuyLog(item.vnum, itemCount, id, category, ch->GetDesc()->GetAccountTable().id, price);
		ch->SetCash(ch->GetCash() - price);
		SendCoins(ch);
		ch->AutoGiveItem(pkItem, true);
	}
}

void CItemShopManager::ManageItem(LPCHARACTER ch, BYTE flag, TItemShopItem item)
{
	if (!IsViewer(ch))
	{
		sys_err("no viewer character (%d %s) tried to add item vnum[%d] count[%d] price[%d] category[%d]",
				ch->GetPlayerID(), ch->GetName(), item.vnum, item.count, item.price, item.category);
		ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
		return;
	}

	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) tried to add item vnum[%d] count[%d] price[%d] category[%d]",
				ch->GetPlayerID(), ch->GetName(), item.vnum, item.count, item.price, item.category);
		ClientPacket(ITEMSHOP_SUBHEADER_GC_NON_EDITOR, NULL, 0, ch);
		return;
	}

	if (item.category < 0 || item.category > 12)
	{
		ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
		return;
	}

	memset(&m_pkItemAdd, 0, sizeof(m_pkItemAdd));
	memset(&m_pkItemEdit, 0, sizeof(m_pkItemEdit));

	switch (flag)
	{
	case EDITOR_FLAG_ADD:
	case EDITOR_FLAG_EDIT:
	{
		if (item.vnum == 0 || item.count == 0 || item.price == 0)
		{
			ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
			return;
		}

		TItemTable * pTable = ITEM_MANAGER::instance().GetTable(item.vnum);

		if (!pTable)
		{
			ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
			return;
		}

		TItemShopEdit p;
		p.item = item;
		if (flag == EDITOR_FLAG_ADD)
		{
			p.flag = ITEMSHOP_ITEM_FLAG_ADD;
		}
		else
		{
			p.flag = ITEMSHOP_ITEM_FLAG_EDIT;
		}

		db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_ITEM_ADD, 0, &p, sizeof(TItemShopEdit));

		if (flag == EDITOR_FLAG_ADD)
		{
			std::memcpy(&m_pkItemAdd, &item, sizeof(TItemShopItem));
		}
		else
		{
			std::memcpy(&m_pkItemEdit, &item, sizeof(TItemShopItem));
		}

		break;
	}

	case EDITOR_FLAG_DELETE:
	{
		if (item.id == 0)
		{
			ClientPacket(ITEMSHOP_SUBHEADER_GC_UNKNOWN_ERROR, NULL, 0, ch);
			return;
		}

		for (DWORD i = 0; i < m_vec_itemShopItems.size(); ++i)
			if (m_vec_itemShopItems[i].id == item.id && m_vec_itemShopItems[i].category == item.category)
			{
				m_vec_itemShopItems.erase(m_vec_itemShopItems.begin() + i);
			}

		TItemShopDeleteItem p;
		p.id = item.id;
		p.category = item.category;
		db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP_ITEM_DELETE, 0, &p, sizeof(TItemShopDeleteItem));


		TPacketGCItemShopDeleteItem pack;
		pack.id = item.id;
		pack.category = item.category;

		ClientPacket(ITEMSHOP_SUBHEADER_GC_DELETE, &pack, sizeof(TPacketGCItemShopDeleteItem));
		ClientPacket(ITEMSHOP_SUBHEADER_GC_REFRESH, NULL, 0);
		break;
	}

	default:
		break;

	}
}

void CItemShopManager::AddItem(BYTE success, BYTE flag, DWORD id, DWORD category)
{
	TItemShopItem item = (flag == ITEMSHOP_ITEM_FLAG_ADD) ? m_pkItemAdd : m_pkItemEdit;
	if (success)
	{
		item.id = id;
		item.category = category;

		switch (flag)
		{
		case ITEMSHOP_ITEM_FLAG_ADD:
		{
			SendItem(item, ITEMSHOP_SUBHEADER_GC_ITEM);
			ClientPacket(ITEMSHOP_SUBHEADER_GC_REFRESH, NULL, 0);

			break;
		}

		case ITEMSHOP_ITEM_FLAG_EDIT:
		{
			SendItem(item, ITEMSHOP_SUBHEADER_GC_UPDATE);
			ClientPacket(ITEMSHOP_SUBHEADER_GC_REFRESH, NULL, 0);

			for (DWORD i = 0; i < m_vec_itemShopItems.size(); ++i)
				if (m_vec_itemShopItems[i].id == id && m_vec_itemShopItems[i].category == category)
				{
					m_vec_itemShopItems.erase(m_vec_itemShopItems.begin() + i);
				}

			break;
		}

		default:
			break;
		}
		m_vec_itemShopItems.push_back(item);
	}
}

void CItemShopManager::AddEditor(char* szName)
{
	m_vec_pkEditors.push_back(szName);
}

bool CItemShopManager::IsEditor(LPCHARACTER ch)
{
	for (auto it = m_vec_pkEditors.begin(); it != m_vec_pkEditors.end(); ++it )
	{
		char* szEditorName = *(it);

		if (strcmp(ch->GetName(), szEditorName) == 0)
		{
			return true;
		}

	}

	return false;
}

void CItemShopManager::ReloadItems(LPCHARACTER ch)
{
	if (!IsEditor(ch))
	{
		sys_err("non editor character (%d %s) tried to reload items",
				ch->GetPlayerID(), ch->GetName());
		return;
	}

	m_vec_itemShopItems.clear();
	Initialize();
}

bool CItemShopManager::IsViewer(LPCHARACTER ch)
{
	return m_set_pkCurrentViewer.find(ch) != m_set_pkCurrentViewer.end();
}

void CItemShopManager::AddViewer(LPCHARACTER ch)
{
	if (!IsViewer(ch))
	{
		m_set_pkCurrentViewer.insert(ch);
	}
}

void CItemShopManager::RemoveViewer(LPCHARACTER ch)
{
	if (IsViewer(ch))
	{
		m_set_pkCurrentViewer.erase(ch);
	}
}

void CItemShopManager::ClientPacket(BYTE subheader, const void* c_pData, size_t size, LPCHARACTER ch)
{
	TPacketGCItemShop packet;
	packet.header = HEADER_GC_ITEMSHOP;
	packet.size = sizeof(TPacketGCItemShop) + size;
	packet.subheader = subheader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(TPacketGCItemShop));
	if (size)
	{
		buf.write(c_pData, size);
	}

	if (ch)
	{
		ch->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		ViewerPacket(buf.read_peek(), buf.size());
	}
}

void CItemShopManager::ViewerPacket(const void* c_pData, size_t size)
{
	for (std::set<LPCHARACTER>::iterator it = m_set_pkCurrentViewer.begin(); it != m_set_pkCurrentViewer.end(); ++it)
	{
		LPCHARACTER viewer = (*it);
		if (viewer && viewer->GetDesc())
		{
			viewer->GetDesc()->Packet(c_pData, size);
		}
		else
		{
			RemoveViewer(viewer);
		}
	}
}
#endif
