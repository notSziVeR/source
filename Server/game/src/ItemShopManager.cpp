#include "stdafx.h"
#ifdef __ITEM_SHOP_ENABLE__
#include "char.h"
#include "char_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "config.h"
#include "p2p.h"
#include "sectree.h"
#include "item.h"
#include "item_manager.h"
#include "utils.h"
#include "db.h"
#include "ItemShopManager.hpp"
#include "log.h"

#include <boost/algorithm/string.hpp>

CItemShopManager::CItemShopManager()
{
	// Loading services..
	LoadCategories();
	LoadItems();
	LoadPromotions();
	LoadSpecialOffers();
}

void CItemShopManager::ReloadService()
{
	// Clearing old values
	m_itemshop_items.clear();
	v_special_offers.clear();

	// Loading services..
	LoadCategories();
	LoadItems();
	LoadPromotions();
	LoadSpecialOffers();
}

static inline bool CanParseRecord(MYSQL_ROW & row, int iRowNum)
{
	return (*row && row[iRowNum]);
}

void CItemShopManager::LoadCategories()
{
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "SELECT categoryID, categoryName FROM itemshop_categories%s ORDER BY categoryID ASC", get_table_postfix());
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(sBuff));

	if (!msg->Get() || !msg->Get()->uiNumRows)
	{
		sys_err("No categories found!");
		return;
	}

	MYSQL_ROW row = nullptr;
	while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		if (CanParseRecord(row, 0) && CanParseRecord(row, 1))
		{
			std::string sName(row[1]);
			boost::replace_all(sName, " ", "_");

			TItemShopCategory pCategory{static_cast<BYTE>(strtoul(row[0], NULL, 10)), sName};
			m_itemshop_items[pCategory] = std::vector<TItemShopItem> {};

			sys_log(0, "Category of id: %d, name: %s was succefully added!", pCategory.bID, pCategory.sName);
		}
	}

	sys_log(0, "%u categories were added!", m_itemshop_items.size());
}

void CItemShopManager::LoadItems()
{
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "SELECT categoryID, itemVnum, itemCount, itemPrice, socket0, socket1, socket2, socket3, socket4, socket5, socket6, socket7, socket8, socket9 FROM itemshop_items%s ORDER BY categoryID ASC, itemVnum ASC", get_table_postfix());
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(sBuff));

	if (!msg->Get() || !msg->Get()->uiNumRows)
	{
		return;
	}

	MYSQL_ROW row = nullptr;
	int iParsedCount = 0;

	while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		int i = 0;
		TItemShopItem pItem{};

		// Category ID
		BYTE bCategoryID = 0;
		if (CanParseRecord(row, i))
		{
			bCategoryID = strtoul(row[i++], NULL, 10);
		}
		else
		{
			sys_err("Cannot parse itemshop record: bCategoryID!");
			continue;
		}

		// Vnum
		if (CanParseRecord(row, i))
		{
			pItem.dwVnum = strtoul(row[i++], NULL, 10);
		}
		else
		{
			sys_err("Cannot parse itemshop record: dwVnum!");
			continue;
		}

		// Count
		if (CanParseRecord(row, i))
		{
			pItem.wCount = strtoul(row[i++], NULL, 10);
		}
		else
		{
			sys_err("Cannot parse itemshop record: wCount!");
			continue;
		}

		// Price
		if (CanParseRecord(row, i))
		{
			pItem.iPrice = strtoul(row[i++], NULL, 10);
		}
		else
		{
			sys_err("Cannot parse itemshop record: iPrice!");
			continue;
		}

		// Sockets
		for (int a = 0; a < ITEM_SOCKET_MAX_NUM; ++a)
		{
			if (CanParseRecord(row, i))
			{
				pItem.alSockets[a] = strtoul(row[i++], NULL, 10);
			}
			else
			{
				sys_err("Cannot parse itemshop record: Socket %d!", a);
				continue;
			}
		}

		// Rehash item
		pItem.Rehash();
		auto fIt = GetCategoryByID(bCategoryID);
		fIt->second.push_back(pItem);

		iParsedCount++;
	}

	sys_log(0, "Records %d of %d were parsed!", iParsedCount, msg->Get()->uiNumRows);
}

void CItemShopManager::LoadPromotions()
{
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "SELECT discountType, additionalArg, discountValue, discountDuration FROM itemshop_discounts%s", get_table_postfix());
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(sBuff));

	if (!msg->Get() || !msg->Get()->uiNumRows)
	{
		sys_err("No promotions found!");
		return;
	}

	MYSQL_ROW row = nullptr;
	while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		if (CanParseRecord(row, 0) && CanParseRecord(row, 1) && CanParseRecord(row, 2) && CanParseRecord(row, 3))
		{
			TItemShopDiscount tDiscount{};
			tDiscount.eType = static_cast<EItemShopDiscountType>(std::strtoul(row[0], NULL, 10));
			tDiscount.bValue = std::strtoul(row[2], NULL, 10);
			tDiscount.ttDuration = std::strtoul(row[3], NULL, 10);

			switch (tDiscount.eType)
			{
			case EItemShopDiscountType::ITEM_DISCOUNT:
			{
				auto pItem = GetItemByHash(row[1]);
				if (!pItem)
				{
					sys_err("Cannot find item by hash: %s", row[1]);
					continue;
				}

				std::memcpy(&pItem->tDiscount, &tDiscount, sizeof(tDiscount));
				pItem->Rehash();
				sys_log(0, "Discount for %d%% was added to item %s", tDiscount.bValue, row[1]);
			}
			break;
			case EItemShopDiscountType::CATEGORY_DISCOUNT:
			{
				auto pCategory = GetCategoryByID(std::strtoul(row[1], NULL, 10));
				if (pCategory == m_itemshop_items.end())
				{
					sys_err("Cannot category: %s", row[1]);
					continue;
				}

				TItemShopCategory cpy_first = pCategory->first;
				std::vector<TItemShopItem> cpy_second(std::move(pCategory->second));
				std::memcpy(&(cpy_first.tDiscount), &tDiscount, sizeof(TItemShopDiscount));

				m_itemshop_items.erase(pCategory);
				m_itemshop_items[cpy_first] = cpy_second;
				sys_log(0, "Discount for %d%% was added to category %s", tDiscount.bValue, row[1]);
			}
			break;
			}
		}
	}

	sys_log(0, "%u promotions were added!", msg->Get()->uiNumRows);
}

void CItemShopManager::LoadSpecialOffers()
{
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "SELECT eType, stValue, itemVnum, itemCount, itemPrice, socket0, socket1, socket2, socket3, socket4, socket5, socket6, socket7, socket8, socket9 FROM itemshop_special_offers%s", get_table_postfix());
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery(sBuff));

	if (!msg->Get() || !msg->Get()->uiNumRows)
	{
		sys_err("No special offers found!");
		return;
	}

	MYSQL_ROW row = nullptr;
	while ((row = mysql_fetch_row(msg->Get()->pSQLResult)))
	{
		if (CanParseRecord(row, 0) && CanParseRecord(row, 1))
		{
			int i = 2;
			TItemShopSpecialOffer tSpecialOffer{};
			tSpecialOffer.eType = static_cast<EItemShopSpecialOfferType>(std::strtoul(row[0], NULL, 10));
			tSpecialOffer.stValue = std::strtoul(row[1], NULL, 10);

			if (tSpecialOffer.eType == EItemShopSpecialOfferType::OFFER_LIMITED_TIME && tSpecialOffer.stValue < time(0))
			{
				sys_log(0, "Discount is already outdated! Discount type: %d, Discount value: %u", tSpecialOffer.eType, tSpecialOffer.stValue);
				continue;
			}

			TItemShopItem pItem{};

			// Vnum
			if (CanParseRecord(row, i))
			{
				pItem.dwVnum = strtoul(row[i++], NULL, 10);
			}
			else
			{
				sys_err("Cannot parse itemshop record: dwVnum!");
				continue;
			}

			// Count
			if (CanParseRecord(row, i))
			{
				pItem.wCount = strtoul(row[i++], NULL, 10);
			}
			else
			{
				sys_err("Cannot parse itemshop record: wCount!");
				continue;
			}

			// Price
			if (CanParseRecord(row, i))
			{
				pItem.iPrice = strtoul(row[i++], NULL, 10);
			}
			else
			{
				sys_err("Cannot parse itemshop record: iPrice!");
				continue;
			}

			// Sockets
			for (int a = 0; a < ITEM_SOCKET_MAX_NUM; ++a)
			{
				if (CanParseRecord(row, i))
				{
					pItem.alSockets[a] = strtoul(row[i++], NULL, 10);
				}
				else
				{
					sys_err("Cannot parse itemshop record: Socket %d!", a);
					continue;
				}
			}

			// Rehash item
			pItem.Rehash();

			switch (tSpecialOffer.eType)
			{
			case EItemShopSpecialOfferType::OFFER_LIMITED_TIME:
			{
				auto pOldItem = GetSpecialOffer(pItem.GetString());
				if (pOldItem)
				{
					sys_err("Special offer already exists: %s", pItem.GetString().c_str());
					continue;
				}

				if (tSpecialOffer.stValue < time(0))
				{
					sys_log(0, "Offer is already outdated! Item has: %s", pItem.GetString().c_str());
					continue;
				}

				std::memcpy(&pItem.tSpecialOffer, &tSpecialOffer, sizeof(tSpecialOffer));
				pItem.Rehash();
				v_special_offers.push_back(pItem);

				sys_log(0, "Special of type %d and value %u was added to item %s", static_cast<int>(tSpecialOffer.eType), tSpecialOffer.stValue, pItem.GetString().c_str());
			}
			break;
			case EItemShopSpecialOfferType::OFFER_LIMITED_QUANTITY:
			{
				auto pOldItem = GetSpecialOffer(pItem.GetString());
				if (pOldItem)
				{
					sys_err("Special offer already exists: %s", pItem.GetString().c_str());
					continue;
				}

				if (!tSpecialOffer.stValue)
				{
					sys_log(0, "Offer ran out of items! Item has: %s", pItem.GetString().c_str());
					continue;
				}

				std::memcpy(&pItem.tSpecialOffer, &tSpecialOffer, sizeof(tSpecialOffer));
				pItem.Rehash();
				v_special_offers.push_back(pItem);

				sys_log(0, "Special of type %d and value %u was added to item %s", static_cast<int>(tSpecialOffer.eType), tSpecialOffer.stValue, pItem.GetString().c_str());
			}
			break;
			}
		}
	}

	sys_log(0, "%u special offers were added!", msg->Get()->uiNumRows);
}

int CItemShopManager::GetPlayerCash(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
	{
		return 0;
	}

	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT cash FROM account.account WHERE id = %d", ch->GetDesc()->GetAccountTable().id));
	if (!msg->Get())
	{
		return 0;
	}

	MYSQL_ROW row = mysql_fetch_row(msg->Get()->pSQLResult);
	if (!row)
	{
		return 0;
	}

	return std::strtoul(row[0], NULL, 10);
}

void CItemShopManager::BuyItem(LPCHARACTER ch, const std::string & sItemHash)
{
	if (!IsViewer(ch))
	{
		return;
	}

	if (!CanBuyItem(ch))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_SHOP_CANNOT_BUY_RIGHT_NOW"));
		return;
	}

	auto pItem = GetItemByHash(sItemHash);
	if (!pItem)
	{
		// Checking if we deal with special offer
		pItem = GetSpecialOffer(sItemHash);
		if (!pItem)
		{
			return;
		}
	}

	// Checking if player has enough inventory space
	if (ch->GetEmptyInventory(3) == -1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_SHOP_NOT_ENOUGH_INVENTORY_SPACE"));
		return;
	}

	// Checking if player has enough cash
	int iPrice = pItem->iPrice;
	if (GetPlayerCash(ch) < iPrice)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_SHOP_NOT_ENOUGH_MONEY"));
		return;
	}

	// Checking special offers invariants
	if (pItem->tSpecialOffer.stValue)
	{
		switch (pItem->tSpecialOffer.eType)
		{
		case EItemShopSpecialOfferType::OFFER_LIMITED_TIME:
		{
			if (get_global_time() > pItem->tSpecialOffer.stValue)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEMSHOP_SPECIAL_OFFER_EXPIRED"));
				return;
			}
		}
		break;
		case EItemShopSpecialOfferType::OFFER_LIMITED_QUANTITY:
		{
			TItemShopItem tmpItem{};
			std::memcpy(&tmpItem, pItem, sizeof(tmpItem));
			tmpItem.dwBuyerID = ch->GetPlayerID();

			SendItemShopPacket(EItemShopPacketType::GD_REQUEST_LIMITED_ITEM, (const void *) &tmpItem);
			return;
		}
		break;
		}
	}

	GiveItem(ch, pItem);
}

void CItemShopManager::BroadcastItemShop(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	if (!IsViewer(ch))
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopClear");
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopUpdateCash %d", GetPlayerCash(ch));

	// Categories&Items
	for (const auto & rItem : m_itemshop_items)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopCategory %s 0", rItem.first.GetString().c_str());
		for (const auto & rISItem : rItem.second)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopItem %u %s 0", rItem.first.bID, rISItem.GetString().c_str());
		}
	}

	// Special Offers
	for (const auto & rPromo : v_special_offers)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopPromos %s 0", rPromo.GetString().c_str());
	}

	// Show
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopShow");
}

void CItemShopManager::AddViewer(LPCHARACTER ch)
{
	us_viewers.insert(ch);
}

void CItemShopManager::RemoveViewer(LPCHARACTER ch)
{
	us_viewers.erase(ch);
}

bool CItemShopManager::IsViewer(LPCHARACTER ch)
{
	return (us_viewers.find(ch) != us_viewers.end());
}

void CItemShopManager::RecvInputItemShopPacket(LPCHARACTER ch, const TItemShopUniversalPacket * pPacket)
{
	if (!ch || !ch->IsItemShopEditor())
	{
		sys_err("%s is not a shop editor!", (ch) ? ch->GetName() : "Nullptr");
		return;
	}

	switch (pPacket->ePacketType)
	{
	case EItemShopPacketType::GD_ADD_ITEM:
		AddNewItem(pPacket);
		break;
	case EItemShopPacketType::GD_REMOVE_ITEM:
		DeleteItem(pPacket);
		break;
	case EItemShopPacketType::GD_ADD_CATEGORY:
		AddCategory(&pPacket->tCategory);
		break;
	case EItemShopPacketType::GD_REMOVE_CATEGORY:
		DeleteCategory(&pPacket->tCategory);
		break;
	case EItemShopPacketType::GD_EDIT_ITEM:
		EditItem(&pPacket->tItem);
		break;
	case EItemShopPacketType::GD_ADD_DISCOUNT:
		AddDiscount(&pPacket->tDiscount);
		break;
	case EItemShopPacketType::GD_REMOVE_DISCOUNT:
		DeleteDiscount(&pPacket->tDiscount);
		break;
	case EItemShopPacketType::GD_ADD_SPECIAL_OFFER:
		AddSpecialOffer(&pPacket->tItem);
		break;
	case EItemShopPacketType::GD_REMOVE_SPECIAL_OFFER:
		DeleteSpecialOffer(&pPacket->tItem);
		break;
	default:
		sys_err("Unsupported packet type: %d", static_cast<int>(pPacket->ePacketType));
	}
}

void CItemShopManager::RecvItemShopPacket(const TItemShopUniversalPacket * pPacket)
{
	switch (pPacket->ePacketType)
	{
	case EItemShopPacketType::DG_ADD_ITEM:
	{
		if (!CategoryExists(pPacket->tCategory.bID))
		{
			sys_err("There is no category %d!", pPacket->tCategory.bID);
			sys_err("Item hash: %s", pPacket->tItem.sHash);
			return;
		}

		if (GetItemByHash(pPacket->tItem.sHash))
		{
			sys_err("Item %s already exists!", pPacket->tItem.sHash);
			return;
		}

		m_itemshop_items[pPacket->tCategory].push_back(pPacket->tItem);
		BroadcastViewerPacket(pPacket->ePacketType, pPacket);

		sys_log(0, "Item %s was added to category %d", pPacket->tItem.sHash, pPacket->tCategory.bID);
	}
	break;
	case EItemShopPacketType::DG_REMOVE_ITEM:
	{
		if (!CategoryExists(pPacket->tCategory.bID))
		{
			sys_err("There is no category %d!", pPacket->tCategory.bID);
			sys_err("Item hash: %s", pPacket->tItem.sHash);
			return;
		}

		if (!GetItemByHash(pPacket->tItem.sHash))
		{
			sys_err("Item %s doesn't exist!", pPacket->tItem.sHash);
			return;
		}

		auto fItem = std::find_if(m_itemshop_items[pPacket->tCategory].begin(), m_itemshop_items[pPacket->tCategory].end(), [&pPacket] (const TItemShopItem & rItem)
		{
			return (!strcmp(pPacket->tItem.sHash, rItem.sHash));
		});

		m_itemshop_items[pPacket->tCategory].erase(fItem);
		BroadcastViewerPacket(pPacket->ePacketType, pPacket);

		sys_log(0, "Item %s was added to category %d", pPacket->tItem.sHash, pPacket->tCategory.bID);
	}
	break;
	case EItemShopPacketType::DG_ADD_CATEGORY:
	{
		if (CategoryExists(pPacket->tCategory.bID))
		{
			sys_err("Category %s already exists!", pPacket->tCategory.sName);
			return;
		}

		m_itemshop_items[pPacket->tCategory] = std::vector<TItemShopItem> {};
		BroadcastViewerPacket(pPacket->ePacketType, pPacket);

		sys_log(0, "Category %s was added", pPacket->tCategory.sName);
	}
	break;
	case EItemShopPacketType::DG_REMOVE_CATEGORY:
	{
		if (!CategoryExists(pPacket->tCategory.bID))
		{
			sys_err("Category %s doesn't exist!", pPacket->tCategory.sName);
			return;
		}

		m_itemshop_items.erase(pPacket->tCategory);
		BroadcastViewerPacket(pPacket->ePacketType, pPacket);

		sys_log(0, "Category %s was removed", pPacket->tCategory.sName);
	}
	break;
	case EItemShopPacketType::DG_EDIT_ITEM:
	{
		if (!CategoryExists(pPacket->tCategory.bID))
		{
			sys_err("There is no category %d!", pPacket->tCategory.bID);
			sys_err("Item hash: %s", pPacket->tItem.sHash);
			return;
		}

		auto pItem = GetItemByHash(pPacket->tItem.sHash);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->tItem.sHash);
			return;
		}

		// Broadcast it first
		BroadcastViewerPacket(pPacket->ePacketType, pPacket, (const void *) pItem);

		std::memcpy(pItem, &pPacket->tItem, sizeof(TItemShopItem));
		sys_log(0, "Item %s was edited", pPacket->tItem.sHash);
	}
	break;
	case EItemShopPacketType::DG_ADD_DISCOUNT:
	{
		if (pPacket->tDiscount.eType == EItemShopDiscountType::CATEGORY_DISCOUNT)
		{
			int iCategoryID = std::stoi(pPacket->tDiscount.szAdditionalArg);
			if (!CategoryExists(iCategoryID))
			{
				sys_err("There is no category %d!", iCategoryID);
				return;
			}

			auto fIt = GetCategoryByID(iCategoryID);
			TItemShopCategory cpy_first = fIt->first;
			std::vector<TItemShopItem> cpy_second(std::move(fIt->second));
			std::memcpy(&(cpy_first.tDiscount), &pPacket->tDiscount, sizeof(pPacket->tDiscount));

			// Broadcast first
			BroadcastViewerPacket(pPacket->ePacketType, pPacket, &fIt->first);

			m_itemshop_items.erase(fIt);
			m_itemshop_items[cpy_first] = cpy_second;

			sys_log(0, "Discount of value %s was applied for %u seconds on category %d", pPacket->tDiscount.bValue, pPacket->tDiscount.ttDuration, iCategoryID);
		}
		else if (pPacket->tDiscount.eType == EItemShopDiscountType::ITEM_DISCOUNT)
		{
			auto pItem = GetItemByHash(pPacket->tDiscount.szAdditionalArg);
			if (!pItem)
			{
				sys_err("Item %s doesn't exist!", pPacket->tDiscount.szAdditionalArg);
				return;
			}

			// Broadcast first
			BroadcastViewerPacket(pPacket->ePacketType, pPacket, pItem);

			std::memcpy(&pItem->tDiscount, &pPacket->tDiscount, sizeof(pItem->tDiscount));
			sys_log(0, "Discount of value %s was applied for %u seconds on item %s", pPacket->tDiscount.bValue, pPacket->tDiscount.ttDuration, pPacket->tDiscount.szAdditionalArg);
		}
		else
		{
			sys_err("Unsupported packet type: %d", static_cast<int>(pPacket->tDiscount.eType));
		}
		return;
	}
	break;
	case EItemShopPacketType::GD_REMOVE_DISCOUNT:
	{
		if (pPacket->tDiscount.eType == EItemShopDiscountType::CATEGORY_DISCOUNT)
		{
			int iCategoryID = std::stoi(pPacket->tDiscount.szAdditionalArg);
			if (!CategoryExists(iCategoryID))
			{
				sys_err("There is no category %d!", iCategoryID);
				return;
			}

			auto fIt = GetCategoryByID(iCategoryID);
			TItemShopCategory cpy_first = fIt->first;
			std::vector<TItemShopItem> cpy_second(std::move(fIt->second));
			cpy_first.tDiscount = TItemShopDiscount{};

			// Broadcast first
			BroadcastViewerPacket(pPacket->ePacketType, pPacket, &fIt->first);

			m_itemshop_items.erase(fIt);
			m_itemshop_items[cpy_first] = cpy_second;

			sys_log(0, "Discount of value %s was removed from category %d", iCategoryID);
		}
		else if (pPacket->tDiscount.eType == EItemShopDiscountType::ITEM_DISCOUNT)
		{
			auto pItem = GetItemByHash(pPacket->tDiscount.szAdditionalArg);
			if (!pItem)
			{
				sys_err("Item %s doesn't exist!", pPacket->tDiscount.szAdditionalArg);
				return;
			}

			// Broadcast first
			BroadcastViewerPacket(pPacket->ePacketType, pPacket, pItem);

			pItem->tDiscount = TItemShopDiscount{};
			sys_log(0, "Discount of value %s was removed from item %s", pPacket->tDiscount.szAdditionalArg);
		}
		else
		{
			sys_err("Unsupported packet type: %d", static_cast<int>(pPacket->tDiscount.eType));
		}
		return;
	}
	break;
	case EItemShopPacketType::DG_ADD_SPECIAL_OFFER:
	{
		auto pItem = GetSpecialOffer(pPacket->tItem.sHash);
		if (pItem)
		{
			sys_err("Item %s already exists!", pPacket->tItem.sHash);
			return;
		}

		v_special_offers.push_back(pPacket->tItem);
		NoticeSpecialOffer(pPacket);

		BroadcastViewerPacket(pPacket->ePacketType, pPacket);
		sys_log(0, "Special offer of item has %s was added!", pPacket->tItem.sHash);
	}
	break;
	case EItemShopPacketType::DG_REMOVE_SPECIAL_OFFER:
	{
		auto pItem = GetSpecialOffer(pPacket->tItem.sHash);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->tItem.sHash);
			return;
		}

		v_special_offers.erase(std::find(v_special_offers.begin(), v_special_offers.end(), pPacket->tItem));
		BroadcastViewerPacket(pPacket->ePacketType, pPacket);

		sys_log(0, "Special offer of item has %s was removed!", pPacket->tItem.sHash);
	}
	break;
	case EItemShopPacketType::DG_REQUEST_LIMITED_ITEM:
	{
		auto pItem = GetSpecialOffer(pPacket->tItem.sHash);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->tItem.sHash);
			return;
		}

		auto pChar = CHARACTER_MANAGER::instance().FindByPID(pPacket->tItem.dwBuyerID);
		if (!pChar)
		{
			sys_err("No buyer id: %u", pPacket->tItem.dwBuyerID);
			return;
		}

		if (!IsViewer(pChar))
		{
			return;
		}

		if (GiveItem(pChar, pItem))
		{
			SendItemShopPacket(EItemShopPacketType::GD_CONFIRM_LIMITED_ITEM_PURCHASE, (const void *) pItem);
			sys_log(0, "Special offer of hash: %s was bought by %u!", pPacket->tItem.sHash, pPacket->tItem.dwBuyerID);
		}
	}
	break;
	case EItemShopPacketType::DG_CONFIRM_LIMITED_ITEM_PURCHASE:
	{
		TItemShopItem tItemBef{};
		std::memcpy(&tItemBef, &pPacket->tItem, sizeof(tItemBef));
		tItemBef.tSpecialOffer.stValue++;
		tItemBef.Rehash();

		auto pItem = GetSpecialOffer(tItemBef.sHash);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->tItem.sHash);
			return;
		}

		std::memcpy(pItem, &tItemBef, sizeof(pPacket->tItem));
		BroadcastViewerPacket(pPacket->ePacketType, pPacket, &tItemBef);

		sys_log(0, "Special offer of hash: %s was updated!", pPacket->tItem.sHash);
	}
	break;
	default:
		sys_err("Unsupported packet type: %d", static_cast<int>(pPacket->ePacketType));
	}
}

void CItemShopManager::AddNewItem(const TItemShopUniversalPacket * pPacket)
{
	std::string sItemHash(pPacket->tItem.sHash);

	auto pItem = GetItemByHash(pPacket->tItem.sHash);
	if (pItem)
		// Item already exists
	{
		return;
	}

	SendItemShopPacket(EItemShopPacketType::GD_ADD_ITEM, (const void *) pPacket);
}

void CItemShopManager::DeleteItem(const TItemShopUniversalPacket * pPacket)
{
	std::string sItemHash(pPacket->tItem.sHash);

	auto pItem = GetItemByHash(pPacket->tItem.sHash);
	if (!pItem)
		// Item doesn't exist
	{
		return;
	}

	SendItemShopPacket(EItemShopPacketType::GD_REMOVE_ITEM, (const void *) pPacket);
}

void CItemShopManager::AddCategory(const TItemShopCategory * pPacket)
{
	if (m_itemshop_items.find(*pPacket) != m_itemshop_items.end())
		// Category exists
	{
		return;
	}

	SendItemShopPacket(EItemShopPacketType::GD_ADD_CATEGORY, (const void *) pPacket);
}

void CItemShopManager::DeleteCategory(const TItemShopCategory * pPacket)
{
	if (m_itemshop_items.find(*pPacket) == m_itemshop_items.end())
		// Category doesn't exists
	{
		return;
	}

	SendItemShopPacket(EItemShopPacketType::GD_REMOVE_CATEGORY, (const void *) pPacket);
}

void CItemShopManager::EditItem(const TItemShopItem * pPacket)
{
	std::string sItemHash(pPacket->sHash);

	auto pItem = GetItemByHash(pPacket->sHash);
	if (!pItem)
		// Item doesn't exist
	{
		return;
	}

	SendItemShopPacket(EItemShopPacketType::GD_EDIT_ITEM, (const void *) pPacket);
}

void CItemShopManager::AddDiscount(const TItemShopDiscount * pPacket)
{
	switch (pPacket->eType)
	{
	case EItemShopDiscountType::CATEGORY_DISCOUNT:
	{
		if (!CategoryExists(static_cast<BYTE>(std::stoi(pPacket->szAdditionalArg))))
		{
			return;
		}
	}
	break;
	case EItemShopDiscountType::ITEM_DISCOUNT:
	{
		if (!GetItemByHash(pPacket->szAdditionalArg))
		{
			return;
		}
	}
	break;
	}

	SendItemShopPacket(EItemShopPacketType::GD_ADD_DISCOUNT, (const void *) pPacket);
}

void CItemShopManager::DeleteDiscount(const TItemShopDiscount * pPacket)
{
	switch (pPacket->eType)
	{
	case EItemShopDiscountType::CATEGORY_DISCOUNT:
	{
		if (!CategoryExists(static_cast<BYTE>(std::stoi(pPacket->szAdditionalArg))))
		{
			return;
		}
	}
	break;
	case EItemShopDiscountType::ITEM_DISCOUNT:
	{
		if (!GetItemByHash(pPacket->szAdditionalArg))
		{
			return;
		}
	}
	break;
	}

	SendItemShopPacket(EItemShopPacketType::GD_REMOVE_DISCOUNT, (const void *) pPacket);
}

void CItemShopManager::AddSpecialOffer(const TItemShopItem * pPacket)
{
	if (std::find_if(v_special_offers.begin(), v_special_offers.end(), [&pPacket] (const TItemShopItem & rItem) { return (!strcmp(pPacket->sHash, rItem.sHash)); }) != v_special_offers.end())
	return;

	SendItemShopPacket(EItemShopPacketType::GD_ADD_SPECIAL_OFFER, (const void *) pPacket);
}

void CItemShopManager::DeleteSpecialOffer(const TItemShopItem * pPacket)
{
	if (std::find_if(v_special_offers.begin(), v_special_offers.end(), [&pPacket] (const TItemShopItem & rItem) { return (!strcmp(pPacket->sHash, rItem.sHash)); }) == v_special_offers.end())
	return;

	SendItemShopPacket(EItemShopPacketType::GD_REMOVE_SPECIAL_OFFER, (const void *) pPacket);
}

TItemShopItem * CItemShopManager::GetItemByHash(const std::string & sHash)
{
	for (auto & rPair : m_itemshop_items)
	{
		auto fItem = std::find_if(rPair.second.begin(), rPair.second.end(), [&sHash] (const TItemShopItem & rItem)
		{
			return (!strcmp(sHash.c_str(), rItem.sHash));
		});

		if (fItem != rPair.second.end())
		{
			return &(*fItem);
		}
	}

	return nullptr;
}

TItemShopItem * CItemShopManager::GetSpecialOffer(const std::string & sHash)
{
	auto fItem = std::find_if(v_special_offers.begin(), v_special_offers.end(), [&sHash] (const TItemShopItem & rItem)
	{
		return (!strcmp(sHash.c_str(), rItem.sHash));
	});

	return (fItem != v_special_offers.end()) ? &(*fItem) : nullptr;
}

std::map<TItemShopCategory, std::vector<TItemShopItem>, TItemShopCategory::Hasher>::iterator CItemShopManager::GetCategoryByID(BYTE bID)
{
	return std::find_if(m_itemshop_items.begin(), m_itemshop_items.end(), [&bID] (const std::pair<TItemShopCategory, std::vector<TItemShopItem>> & rPair) { return rPair.first == bID; });
}

const TItemShopCategory * CItemShopManager::GetCategoryByItemHash(const std::string & sHash)
{
	for (const auto & rPair : m_itemshop_items)
	{
		auto fItem = std::find_if(rPair.second.begin(), rPair.second.end(), [&sHash] (const TItemShopItem & rItem)
		{
			return (!strcmp(sHash.c_str(), rItem.sHash));
		});

		if (fItem != rPair.second.end())
		{
			return &(rPair.first);
		}
	}

	return nullptr;
}

bool CItemShopManager::CategoryExists(BYTE bID)
{
	return (GetCategoryByID(bID) != m_itemshop_items.end());
}

void CItemShopManager::SendItemShopPacket(const EItemShopPacketType & eType, const void * pPacket)
{
	TItemShopUniversalPacket tDBPacket{};
	switch (eType)
	{
	case EItemShopPacketType::GD_ADD_ITEM:
	case EItemShopPacketType::GD_REMOVE_ITEM:
		std::memcpy(&tDBPacket, pPacket, sizeof(tDBPacket));
		break;
	case EItemShopPacketType::GD_EDIT_ITEM:
	case EItemShopPacketType::GD_ADD_SPECIAL_OFFER:
	case EItemShopPacketType::GD_REMOVE_SPECIAL_OFFER:
	case EItemShopPacketType::GD_REQUEST_LIMITED_ITEM:
	case EItemShopPacketType::GD_CONFIRM_LIMITED_ITEM_PURCHASE:
		std::memcpy(&tDBPacket.tItem, pPacket, sizeof(tDBPacket.tItem));
		break;
	case EItemShopPacketType::GD_ADD_CATEGORY:
	case EItemShopPacketType::GD_REMOVE_CATEGORY:
		std::memcpy(&tDBPacket.tCategory, pPacket, sizeof(tDBPacket.tCategory));
		break;
	case EItemShopPacketType::GD_ADD_DISCOUNT:
	case EItemShopPacketType::GD_REMOVE_DISCOUNT:
		std::memcpy(&tDBPacket.tDiscount, pPacket, sizeof(tDBPacket.tDiscount));
		break;
	default:
		sys_err("Unsupported type: %d", static_cast<int>(eType));
		return;
	}

	tDBPacket.bHeader = HEADER_GD_ITEMSHOP;
	tDBPacket.ePacketType = eType;

	db_clientdesc->DBPacket(HEADER_GD_ITEMSHOP, 0, &tDBPacket, sizeof(tDBPacket));
}

bool CItemShopManager::CanBuyItem(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
	{
		return false;
	}

	if (!ch->CanDoAction())
	{
		return false;
	}

	return true;
}

void CItemShopManager::ChargePlayer(LPCHARACTER ch, int iPrice)
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("UPDATE account.account SET cash = cash-%d WHERE id = %d", iPrice, ch->GetDesc()->GetAccountTable().id));
}

void CItemShopManager::NoticeSpecialOffer(const TItemShopUniversalPacket * pPacket)
{
	switch (pPacket->tItem.tSpecialOffer.eType)
	{
	case EItemShopSpecialOfferType::OFFER_LIMITED_TIME:
	{
		auto pItem = ITEM_MANAGER::instance().GetTable(pPacket->tItem.dwVnum);
		if (!pItem)
		{
			return;
		}

		//BroadcastNotice(LC_TEXT("ITEMSHOP_SPECIAL_OFFER_LIMITED_TIME %s %s", pItem->szLocaleName, GetFullDateFromTime(pPacket->tItem.tSpecialOffer.stValue).c_str()));
	}
	break;
	case EItemShopSpecialOfferType::OFFER_LIMITED_QUANTITY:
	{
		auto pItem = ITEM_MANAGER::instance().GetTable(pPacket->tItem.dwVnum);
		if (!pItem)
		{
			return;
		}

		//BroadcastNotice(LC_TEXT("ITEMSHOP_SPECIAL_OFFER_LIMITED_QUANTITY %s %u", pItem->szLocaleName, pPacket->tItem.tSpecialOffer.stValue));
	}
	break;
	default:
		sys_err("Unsupported type: %d", static_cast<int>(pPacket->tItem.tSpecialOffer.eType));
	}
}

bool CItemShopManager::GiveItem(LPCHARACTER ch, TItemShopItem * pItem)
{
	// Applying discount
	int iPrice = pItem->iPrice;
	auto pCategory = GetCategoryByItemHash(pItem->GetString());

	if (pCategory && pCategory->tDiscount.bValue && pCategory->tDiscount.ttDuration >= get_global_time())
	{
		iPrice *= (100 - std::min<int>(100, pCategory->tDiscount.bValue)) / 100;
	}
	else
	{
		if (pItem->tDiscount.bValue && pItem->tDiscount.ttDuration >= get_global_time())
		{
			iPrice *= (100 - std::min<int>(100, pItem->tDiscount.bValue)) / 100;
		}
	}

	// Checking if player has enough cash
	int iPlayerCash = GetPlayerCash(ch);
	if (iPlayerCash < iPrice)
	{
		return false;
	}

	// Creating item
	auto pNewItem = ITEM_MANAGER::instance().CreateItem(pItem->dwVnum, pItem->wCount);
	if (!pNewItem)
	{
		return false;
	}

	// Charge player
	ChargePlayer(ch, iPrice);

	// Apply item sockets
	pNewItem->SetSockets(pItem->alSockets);

	// Support for real time items
	if (pNewItem->GetSocket(0) > 0)
	{
		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
		{
			if (LIMIT_REAL_TIME == pNewItem->GetLimitType(i))
			{
				pNewItem->SetSocket(0, time(0) + pNewItem->GetSocket(0));
			}
		}
	}

	// Support for unique items
	if (pNewItem->GetType() == ITEM_UNIQUE)
	{
		if (pNewItem->GetValue(2) == 0)
		{
			pNewItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pNewItem->GetValue(0));
		}
		else
		{
			pNewItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, get_global_time() + pNewItem->GetValue(0));
		}
	}

	// Give item to player
	ch->AutoGiveItem(pNewItem);
	pNewItem->Save();
	ITEM_MANAGER::instance().FlushDelayedSave(pNewItem);

	iPlayerCash -= iPrice;
	ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopUpdateCash %d", iPlayerCash);
	ch->SetItemShopTime();
	ch->SetVolatileCoinValue(iPlayerCash);

#ifdef __ITEM_SHOP_ENABLE__
	LogManager::instance().ItemShopLog(ch->GetPlayerID(), pItem->sHash, iPrice, (iPlayerCash + iPrice), iPlayerCash);
#endif

	return true;
}

void CItemShopManager::BroadcastViewerPacket(const EItemShopPacketType & eType, const TItemShopUniversalPacket * pPacket, const void * oldPack)
{
	for (const auto & ch : us_viewers)
	{
		switch (eType)
		{
		case EItemShopPacketType::DG_ADD_ITEM:
		case EItemShopPacketType::DG_REMOVE_ITEM:
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopItem %d %s 0", pPacket->tCategory.bID, pPacket->tItem.GetString().c_str());
		}
		break;
		case EItemShopPacketType::DG_EDIT_ITEM:
		{
			const TItemShopItem * oldItem = (const TItemShopItem *) oldPack;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopItem %d %s %s", pPacket->tCategory.bID, oldItem->GetString().c_str(), pPacket->tItem.GetString().c_str());
		}
		break;
		case EItemShopPacketType::DG_ADD_SPECIAL_OFFER:
		case EItemShopPacketType::DG_REMOVE_SPECIAL_OFFER:
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopSpecialOffer %s 0", pPacket->tItem.GetString().c_str());
		}
		break;
		case EItemShopPacketType::DG_CONFIRM_LIMITED_ITEM_PURCHASE:
		{
			const TItemShopItem * oldItem = (const TItemShopItem *) oldPack;
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopSpecialOffer %s %s", oldItem->GetString().c_str(), pPacket->tItem.GetString().c_str());
		}
		break;
		case EItemShopPacketType::DG_ADD_CATEGORY:
		case EItemShopPacketType::DG_REMOVE_CATEGORY:
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopCategory %s 0", pPacket->tCategory.GetString().c_str());
		}
		break;
		case EItemShopPacketType::DG_ADD_DISCOUNT:
		case EItemShopPacketType::DG_REMOVE_DISCOUNT:
		{
			if (pPacket->tDiscount.eType == EItemShopDiscountType::CATEGORY_DISCOUNT)
			{
				const TItemShopCategory * oldCategory = (const TItemShopCategory *) oldPack;
				ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopCategory %s %s", oldCategory->GetString().c_str(), pPacket->tCategory.GetString().c_str());
			}
			else if (pPacket->tDiscount.eType == EItemShopDiscountType::ITEM_DISCOUNT)
			{
				const TItemShopItem * oldItem = (const TItemShopItem *) oldPack;
				ch->ChatPacket(CHAT_TYPE_COMMAND, "ItemShopItem %d %s %s", pPacket->tCategory.bID, oldItem->GetString().c_str(), pPacket->tCategory.GetString().c_str());
			}
		}
		break;
		default:
			sys_err("Unsupported type: %d", static_cast<int>(eType));
			return;
		}
	}
}
#endif

