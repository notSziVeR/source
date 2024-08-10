#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shopEx.h"
#include "group_text_parse_tree.h"

#ifdef __ENABLE_NEW_LOGS_CHAT__
	#include "ChatLogsHelper.hpp"
#endif

#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#include "PageGrid.hpp"


bool CShopEx::Create(DWORD dwVnum, DWORD dwNPCVnum)
{
	m_dwVnum = dwVnum;
	m_dwNPCVnum = dwNPCVnum;
	return true;
}

bool CShopEx::AddShopTable(TShopTableEx& shopTable)
{
	for (auto it = m_vec_shopTabs.begin(); it != m_vec_shopTabs.end(); it++)
	{
		const TShopTableEx& _shopTable = *it;
		if (0 != _shopTable.dwVnum && _shopTable.dwVnum == shopTable.dwVnum)
		{
			return false;
		}
		if (0 != _shopTable.dwNPCVnum && _shopTable.dwNPCVnum == shopTable.dwVnum)
		{
			return false;
		}
	}
	m_vec_shopTabs.push_back(shopTable);
	return true;
}

bool CShopEx::AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire)
{
	if (!ch)
	{
		return false;
	}

	if (ch->GetExchange())
	{
		return false;
	}

	if (ch->GetShop())
	{
		return false;
	}

	ch->SetShop(this);

	m_map_guest.insert(GuestMapType::value_type(ch, bOtherEmpire));

	TPacketGCShop pack;

	pack.header		= HEADER_GC_SHOP;
	pack.subheader	= SHOP_SUBHEADER_GC_START_EX;

	TPacketGCShopStartEx pack2;

	memset(&pack2, 0, sizeof(pack2));

	pack2.owner_vid = owner_vid;
	pack2.shop_tab_count = m_vec_shopTabs.size();
	TEMP_BUFFER tempBuffer;

	for (const TShopTableEx& shop_tab : m_vec_shopTabs)
	{
		TPacketGCShopStartEx::TSubPacketShopTab pack_tab;
		pack_tab.coin_type = shop_tab.coinType;
		memcpy(pack_tab.name, shop_tab.name.c_str(), SHOP_TAB_NAME_MAX);

		for (BYTE i = 0; i < SHOP_HOST_ITEM_MAX_NUM; i++)
		{
			pack_tab.items[i].vnum = shop_tab.items[i].vnum;
			pack_tab.items[i].count = shop_tab.items[i].count;
			pack_tab.items[i].coin_vnum = shop_tab.itemsEx[i].coin_vnum;
			switch (shop_tab.coinType)
			{
			case SHOP_COIN_TYPE_GOLD:
			case SHOP_COIN_TYPE_SPECIFIC_ITEM:
				if (shop_tab.items[i].price > 0)
				{
					pack_tab.items[i].price = shop_tab.items[i].price;
				}
				else
				{
					const TItemTable* proto = ITEM_MANAGER::instance().GetTable(shop_tab.items[i].vnum);
					if (!proto)
					{
						pack_tab.items[i].price = 0;
					}
					else
					{
						pack_tab.items[i].price = proto->dwGold * shop_tab.items[i].count;
					}
				}
#ifdef __ENABLE_NEWSTUFF__
				if (bOtherEmpire && !g_bEmpireShopPriceTripleDisable) // no empire price penalty for pc shop
#else
				if (bOtherEmpire) // no empire price penalty for pc shop
#endif
					pack_tab.items[i].price = pack_tab.items[i].price * 3;
				break;
			case SHOP_COIN_TYPE_SECONDARY_COIN:
				pack_tab.items[i].price = shop_tab.items[i].price;
				break;
			}
			memset(pack_tab.items[i].aAttr, 0, sizeof(pack_tab.items[i].aAttr));
			memset(pack_tab.items[i].alSockets, 0, sizeof(pack_tab.items[i].alSockets));
#ifdef __TRANSMUTATION_SYSTEM__
			pack_tab.items[i].transmutate_id = 0;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
			pack_tab.items[i].dwRefineElement = 0;
#endif
		}

		tempBuffer.write(&pack_tab, sizeof(pack_tab));
	}

	pack.size = sizeof(pack) + sizeof(pack2) + tempBuffer.size();

	ch->GetDesc()->BufferedPacket(&pack, sizeof(TPacketGCShop));
	ch->GetDesc()->BufferedPacket(&pack2, sizeof(TPacketGCShopStartEx));
	ch->GetDesc()->Packet(tempBuffer.read_peek(), tempBuffer.size());

	return true;
}

bool CShopEx::CheckFreeInventory(CHARACTER* ch, uint16_t pos, CountType amount,
	const TItemTable* item)
{
	PagedGrid<bool> inventoryGrid(5, 9, INVENTORY_PAGE_COUNT);
	for (uint32_t i = 0; i < inventoryGrid.GetSize(); ++i) {
		auto* inventoryItem = ch->GetInventoryItem(i);
		if (!inventoryItem)
			continue;

		inventoryGrid.PutPos(true, i, inventoryItem->GetSize());
	}

	for (int i = 0; i < amount; ++i) {
		std::optional<uint32_t> newPos = std::nullopt;

		newPos = inventoryGrid.FindBlankBetweenPages(
			item->bSize, 0,
			4);

		if (!newPos.has_value()) {
			return false;
		}

		inventoryGrid.PutPos(true, newPos.value(), item->bSize);
	}
	return true;
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t CShopEx::Buy(LPCHARACTER ch, BYTE pos, CountType wAmount)
#else
	int CShopEx::Buy(LPCHARACTER ch, BYTE pos)
#endif
{
	BYTE tabIdx = pos / SHOP_HOST_ITEM_MAX_NUM;
	BYTE slotPos = pos % SHOP_HOST_ITEM_MAX_NUM;
	if (tabIdx >= GetTabCount())
	{
		sys_log(0, "ShopEx::Buy : invalid position %d : %s", pos, ch->GetName());
		return SHOP_SUBHEADER_GC_INVALID_POS;
	}

	sys_log(0, "ShopEx::Buy : name %s pos %d", ch->GetName(), pos);

	auto it = m_map_guest.find(ch);
	if (it == m_map_guest.end())
		return SHOP_SUBHEADER_CG_END;

	TShopTableEx& shopTab = m_vec_shopTabs[tabIdx];
	TShopItemExTable& r_item = shopTab.itemsEx[slotPos];

	EShopCoinType coinType = SHOP_COIN_TYPE_GOLD;
	if (r_item.coin_vnum)
	{
		coinType = SHOP_COIN_TYPE_SPECIFIC_ITEM;
	}

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	if (wAmount <= 0)
	{
		sys_log(0, "ShopEx::BuyMultiple multiple == 0");
		return SHOP_SUBHEADER_GC_END;
	}
#endif

	auto price = r_item.price;
	if (price < 0)
	{
		const TItemTable* pProto = ITEM_MANAGER::instance().GetTable(r_item.vnum);
		if (!pProto)
		{
			return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
		}

		price = pProto->dwGold * r_item.count;
	}

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	if (wAmount)
	{
		price *= wAmount;
	}
#endif

	if (price < 0)
	{
		LogManager::instance().HackLog("SHOP_BUY_GOLD_OVERFLOW", ch);
		return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
	}

	switch (coinType)
	{
		case SHOP_COIN_TYPE_GOLD:
			if (it->second && !g_bEmpireShopPriceTripleDisable)	// if other empire, price is triple
			{
				price *= 3;
			}

			if (ch->GetGold() < price)
			{
				sys_log(1, "ShopEx::Buy : Not enough money : %s has %d, price %lld", ch->GetName(), ch->GetGold(), price);
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY;
			}
			break;

		case SHOP_COIN_TYPE_SPECIFIC_ITEM:
			if (it->second && !g_bEmpireShopPriceTripleDisable)	// if other empire, price is triple
			{
				price *= 3;
			}

			if (ch->CountSpecifyItem(r_item.coin_vnum) < price)
			{
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_ITEM_EX;
			}
			break;

		case SHOP_COIN_TYPE_SECONDARY_COIN:
		{
			CountType count = ch->CountSpecifyTypeItem(ITEM_SECONDARY_COIN);
			if (count < price)
			{
				sys_log(1, "ShopEx::Buy : Not enough myeongdojun : %s has %d, price %lld", ch->GetName(), count, price);
				return SHOP_SUBHEADER_GC_NOT_ENOUGH_MONEY_EX;
			}
		}
		break;
	}

	const auto* item = ITEM_MANAGER::instance().GetTable(r_item.vnum);
	if (!item)
		return SHOP_SUBHEADER_GC_SOLD_OUT;

	if (!CheckFreeInventory(ch, pos, wAmount, item))
	{
		return SHOP_SUBHEADER_GC_INVENTORY_FULL;
	}

	switch (coinType)
	{
		case SHOP_COIN_TYPE_GOLD:
		{
			ch->PointChange(POINT_GOLD, -price, false);
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_NPC_SPEND_MONEY, 0, static_cast<uint64_t>(price) });
			CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(price) });
#endif
		}
		break;

		case SHOP_COIN_TYPE_SPECIFIC_ITEM:
		{
			ch->RemoveSpecifyItem(r_item.coin_vnum, price);
		}
		break;
		case SHOP_COIN_TYPE_SECONDARY_COIN:
		{
			ch->RemoveSpecifyTypeItem(ITEM_SECONDARY_COIN, price);
		}
		break;
	}

	for (int i = 0; i < wAmount; ++i)
	{
		auto* newItem = ch->AutoGiveItem(r_item.vnum, r_item.count);
		ITEM_MANAGER::instance().FlushDelayedSave(newItem);
	}

	ch->Save();
	return (SHOP_SUBHEADER_GC_OK);
}

