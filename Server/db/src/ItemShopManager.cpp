#include "stdafx.h"
#ifdef __ITEM_SHOP_ENABLE__
#include "Main.h"
#include "DBManager.h"
#include "Peer.h"
#include "ClientManager.h"
#include "ItemShopManager.hpp"

#include <boost/algorithm/string.hpp>

namespace
{
time_t CACHE_UPDATE = 60 * 10;
};

CItemShopManager::CItemShopManager() : ttCacheNextUpdate{time(0) + CACHE_UPDATE}
{
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

void CItemShopManager::Process(bool bSkipCache)
{
	if (!bSkipCache)
	{
		if (time(0) < ttCacheNextUpdate)
		{
			return;
		}
	}

	// Process special offers
	std::unordered_set<TItemShopItem*> us_collector;
	for (auto & rOffer : v_special_offers)
	{
		if (rOffer.tSpecialOffer.eType == EItemShopSpecialOfferType::OFFER_LIMITED_TIME &&
				rOffer.tSpecialOffer.stValue < time(0))
		{
			// Query
			char sBuff[1024];
			snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_special_offers%s WHERE %s", GetTablePostfix(), rOffer.GetSelectQuery().c_str());
			std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

			us_collector.insert(&rOffer);
		}
	}

	v_special_offers.erase(std::remove_if(v_special_offers.begin(), v_special_offers.end(), [&us_collector] (TItemShopItem & rItem) { return us_collector.find(&rItem) != us_collector.end(); }), v_special_offers.end());

	// Process discounts
	for (auto & rPair : m_itemshop_items)
	{
		// Query
		char sBuff[1024];
		snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_discounts%s WHERE %s", GetTablePostfix(), rPair.first.GetSelectQuery().c_str());
		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

		for (auto & rItem : rPair.second)
		{
			if (rItem.tDiscount.ttDuration && rItem.tDiscount.ttDuration < time(0))
			{
				// Query
				char sBuff[1024];
				snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_discounts%s WHERE %s", GetTablePostfix(), rItem.tDiscount.GetSelectQuery().c_str());
				std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
			}
		}
	}

	ttCacheNextUpdate = time(0) + CACHE_UPDATE;
}

void CItemShopManager::LoadCategories()
{
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "SELECT categoryID, categoryName FROM itemshop_categories%s ORDER BY categoryID ASC", GetTablePostfix());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

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
	snprintf(sBuff, sizeof(sBuff), "SELECT categoryID, itemVnum, itemCount, itemPrice, socket0, socket1, socket2, socket3, socket4, socket5, socket6, socket7, socket8, socket9 FROM itemshop_items%s ORDER BY categoryID ASC, itemVnum ASC", GetTablePostfix());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

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
	snprintf(sBuff, sizeof(sBuff), "SELECT discountType, additionalArg, discountValue, discountDuration FROM itemshop_discounts%s", GetTablePostfix());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

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

			if (tDiscount.ttDuration < time(0))
			{
				sys_log(0, "Discount is already outdated! Discount type: %d, Discount value: %u", tDiscount.eType, tDiscount.bValue);
				sys_log(0, "Doing cleanup work..");

				// Doing clenup work
				char sBuff[1024];
				snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_discounts%s WHERE %s", GetTablePostfix(), tDiscount.GetSelectQuery().c_str());
				std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

				continue;
			}

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
	snprintf(sBuff, sizeof(sBuff), "SELECT eType, stValue, itemVnum, itemCount, itemPrice, socket0, socket1, socket2, socket3, socket4, socket5, socket6, socket7, socket8, socket9 FROM itemshop_special_offers%s", GetTablePostfix());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

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
					sys_log(0, "Doing cleanup work..");

					// Doing clenup work
					char sBuff[1024];
					snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_special_offers%s WHERE %s", GetTablePostfix(), pItem.GetSelectQuery().c_str());
					std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

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

void CItemShopManager::RecvItemShopPacket(CPeer * pPeer, const TItemShopUniversalPacket * pPacket)
{
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
	case EItemShopPacketType::GD_REQUEST_LIMITED_ITEM:
		CheckSpecialOfferAvailability(pPeer, &pPacket->tItem);
		break;
	case EItemShopPacketType::GD_CONFIRM_LIMITED_ITEM_PURCHASE:
		UpdateSpecialOfferAvailability(&pPacket->tItem);
		break;
	default:
		sys_err("Unsupported packet type: %d", static_cast<int>(pPacket->ePacketType));
	}
}

void CItemShopManager::AddNewItem(const TItemShopUniversalPacket * pPacket)
{
	if (!CategoryExists(pPacket->tCategory.bID))
	{
		sys_err("Category %u doesn't exist!", pPacket->tCategory.bID);
		return;
	}

	auto pItem = GetItemByHash(pPacket->tItem.sHash);
	if (pItem)
		// Item already exists
	{
		return;
	}

	auto fIt = GetCategoryByID(pPacket->tCategory.bID);
	fIt->second.push_back(pPacket->tItem);
	SendItemShopPacket(EItemShopPacketType::DG_ADD_ITEM, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_items%s (...) VALUES (%s)", GetTablePostfix(), pPacket->tItem.GetInsertQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::DeleteItem(const TItemShopUniversalPacket * pPacket)
{
	auto pItem = GetItemByHash(pPacket->tItem.sHash);
	if (!pItem)
		// Item doesn't exist
	{
		return;
	}

	auto fIt = GetCategoryByID(pPacket->tCategory.bID);
	fIt->second.erase(std::find(fIt->second.begin(), fIt->second.end(), pPacket->tItem.sHash));
	SendItemShopPacket(EItemShopPacketType::DG_REMOVE_ITEM, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_items%s WHERE %s", GetTablePostfix(), pPacket->tItem.GetSelectQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::AddCategory(const TItemShopCategory * pPacket)
{
	if (m_itemshop_items.find(*pPacket) != m_itemshop_items.end())
		// Category exists
	{
		return;
	}

	m_itemshop_items[*pPacket] = std::vector<TItemShopItem> {};
	SendItemShopPacket(EItemShopPacketType::DG_ADD_CATEGORY, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_categories%s (...) VALUES (%s)", GetTablePostfix(), pPacket->GetInsertQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::DeleteCategory(const TItemShopCategory * pPacket)
{
	if (m_itemshop_items.find(*pPacket) == m_itemshop_items.end())
		// Category doesn't exists
	{
		return;
	}

	m_itemshop_items.erase(*pPacket);
	SendItemShopPacket(EItemShopPacketType::DG_REMOVE_CATEGORY, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_categories%s WHERE %s", GetTablePostfix(), pPacket->GetSelectQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::EditItem(const TItemShopItem * pPacket)
{
	auto pItem = GetItemByHash(pPacket->sHash);
	if (!pItem)
		// Item doesn't exist
	{
		return;
	}

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "UPDATE itemshop_items%s SET %s WHERE %s", GetTablePostfix(), pPacket->GetReplaceQuery().c_str(), pPacket->GetSelectQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

	std::memcpy(pItem, pPacket, sizeof(TItemShopItem));
	SendItemShopPacket(EItemShopPacketType::DG_EDIT_ITEM, (const void *) pPacket);
}

void CItemShopManager::AddDiscount(const TItemShopDiscount * pPacket)
{
	switch (pPacket->eType)
	{
	case EItemShopDiscountType::CATEGORY_DISCOUNT:
	{
		if (!CategoryExists(static_cast<BYTE>(std::strtoul(pPacket->szAdditionalArg, NULL, 10))))
		{
			return;
		}

		auto fIt = GetCategoryByID(std::strtoul(pPacket->szAdditionalArg, NULL, 10));
		TItemShopCategory cpy_first = fIt->first;
		std::vector<TItemShopItem> cpy_second(std::move(fIt->second));
		std::memcpy(&(cpy_first.tDiscount), pPacket, sizeof(TItemShopDiscount));

		m_itemshop_items.erase(fIt);
		m_itemshop_items[cpy_first] = cpy_second;

		// Query
		char sBuff[1024];
		snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_discounts%s (...) VALUES (%s)", GetTablePostfix(), pPacket->GetInsertQuery().c_str());
		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
	}
	break;
	case EItemShopDiscountType::ITEM_DISCOUNT:
	{
		if (!GetItemByHash(pPacket->szAdditionalArg))
		{
			return;
		}

		auto pItem = GetItemByHash(pPacket->szAdditionalArg);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->szAdditionalArg);
			return;
		}

		std::memcpy(&pItem->tDiscount, pPacket, sizeof(pItem->tDiscount));

		// Query
		char sBuff[1024];
		snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_discounts%s (...) VALUES (%s)", GetTablePostfix(), pPacket->GetInsertQuery().c_str());
		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
	}
	break;
	}

	SendItemShopPacket(EItemShopPacketType::DG_ADD_DISCOUNT, (const void *) pPacket);
}

void CItemShopManager::DeleteDiscount(const TItemShopDiscount * pPacket)
{
	switch (pPacket->eType)
	{
	case EItemShopDiscountType::CATEGORY_DISCOUNT:
	{
		if (!CategoryExists(static_cast<BYTE>(std::strtoul(pPacket->szAdditionalArg, NULL, 10))))
		{
			return;
		}

		auto fIt = GetCategoryByID(std::strtoul(pPacket->szAdditionalArg, NULL, 10));
		TItemShopCategory cpy_first = fIt->first;
		std::vector<TItemShopItem> cpy_second(std::move(fIt->second));
		cpy_first.tDiscount = TItemShopDiscount{};

		m_itemshop_items.erase(fIt);
		m_itemshop_items[cpy_first] = cpy_second;

		// Query
		char sBuff[1024];
		snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_discounts%s WHERE %s", GetTablePostfix(), pPacket->GetSelectQuery().c_str());
		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
	}
	break;
	case EItemShopDiscountType::ITEM_DISCOUNT:
	{
		if (!GetItemByHash(pPacket->szAdditionalArg))
		{
			return;
		}

		auto pItem = GetItemByHash(pPacket->szAdditionalArg);
		if (!pItem)
		{
			sys_err("Item %s doesn't exist!", pPacket->szAdditionalArg);
			return;
		}

		pItem->tDiscount = TItemShopDiscount{};

		// Query
		char sBuff[1024];
		snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_discounts%s (...) VALUES (%s)", GetTablePostfix(), pPacket->GetSelectQuery().c_str());
		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
	}
	break;
	}

	SendItemShopPacket(EItemShopPacketType::DG_REMOVE_DISCOUNT, pPacket);
}

void CItemShopManager::AddSpecialOffer(const TItemShopItem * pPacket)
{
	auto pItem = GetSpecialOffer(pPacket->sHash);
	if (pItem)
	{
		sys_err("Item %s already exists!", pPacket->sHash);
		return;
	}

	v_special_offers.push_back(*pPacket);
	SendItemShopPacket(EItemShopPacketType::DG_ADD_SPECIAL_OFFER, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "INSERT INTO itemshop_special_offers%s (...) VALUES (%s)", GetTablePostfix(), pPacket->GetInsertSpecialQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::DeleteSpecialOffer(const TItemShopItem * pPacket)
{
	auto pItem = GetSpecialOffer(pPacket->sHash);
	if (!pItem)
	{
		sys_err("Item %s doesn't exist!", pPacket->sHash);
		return;
	}

	v_special_offers.erase(std::find(v_special_offers.begin(), v_special_offers.end(), *pPacket));
	SendItemShopPacket(EItemShopPacketType::DG_REMOVE_SPECIAL_OFFER, (const void *) pPacket);

	// Query
	char sBuff[1024];
	snprintf(sBuff, sizeof(sBuff), "DELETE FROM itemshop_special_offers%s WHERE %s", GetTablePostfix(), pPacket->GetSelectSpecialQuery().c_str());
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));
}

void CItemShopManager::CheckSpecialOfferAvailability(CPeer * pPeer, const TItemShopItem * pPacket)
{
	auto pItem = GetSpecialOffer(pPacket->sHash);
	if (!pItem)
	{
		sys_err("Item %s doesn't exist!", pPacket->sHash);
		return;
	}

	switch (pItem->tSpecialOffer.eType)
	{
	case EItemShopSpecialOfferType::OFFER_LIMITED_TIME:
	{
		if (pItem->tSpecialOffer.stValue >= time(0))
		{
			SendItemShopPacket(EItemShopPacketType::DG_REQUEST_LIMITED_ITEM, (const void *) pPacket, pPeer);
		}
	}
	break;
	case EItemShopSpecialOfferType::OFFER_LIMITED_QUANTITY:
	{
		if (pItem->tSpecialOffer.stValue)
		{
			SendItemShopPacket(EItemShopPacketType::DG_REQUEST_LIMITED_ITEM, (const void *) pPacket, pPeer);
		}
	}
	break;
	}
}

void CItemShopManager::UpdateSpecialOfferAvailability(const TItemShopItem * pPacket)
{
	auto pItem = GetSpecialOffer(pPacket->sHash);
	if (!pItem)
	{
		sys_err("Item %s doesn't exist!", pPacket->sHash);
		return;
	}

	switch (pItem->tSpecialOffer.eType)
	{
	case EItemShopSpecialOfferType::OFFER_LIMITED_TIME:
		break;
	case EItemShopSpecialOfferType::OFFER_LIMITED_QUANTITY:
	{
		if (pItem->tSpecialOffer.stValue > 0)
		{
			// Query
			char sBuff[1024];
			snprintf(sBuff, sizeof(sBuff), "UPDATE itemshop_special_offers%s SET stValue = %d WHERE %s", GetTablePostfix(), (--pItem->tSpecialOffer.stValue), pPacket->GetInsertQuery().c_str());
			std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sBuff));

			SendItemShopPacket(EItemShopPacketType::DG_CONFIRM_LIMITED_ITEM_PURCHASE, (const void *) pPacket);
		}
	}
	break;
	}
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

void CItemShopManager::SendItemShopPacket(const EItemShopPacketType & eType, const void * pPacket, CPeer * pPeer)
{
	TItemShopUniversalPacket tDBPacket{};

	switch (eType)
	{
	case EItemShopPacketType::DG_ADD_ITEM:
	case EItemShopPacketType::DG_REMOVE_ITEM:
		std::memcpy(&tDBPacket, pPacket, sizeof(tDBPacket));
		break;
	case EItemShopPacketType::DG_EDIT_ITEM:
	case EItemShopPacketType::DG_ADD_SPECIAL_OFFER:
	case EItemShopPacketType::DG_REMOVE_SPECIAL_OFFER:
	case EItemShopPacketType::DG_REQUEST_LIMITED_ITEM:
	case EItemShopPacketType::DG_CONFIRM_LIMITED_ITEM_PURCHASE:
		std::memcpy(&tDBPacket.tItem, pPacket, sizeof(tDBPacket.tItem));
		break;
	case EItemShopPacketType::DG_ADD_CATEGORY:
	case EItemShopPacketType::DG_REMOVE_CATEGORY:
		std::memcpy(&tDBPacket.tCategory, pPacket, sizeof(tDBPacket.tCategory));
		break;
	case EItemShopPacketType::DG_ADD_DISCOUNT:
	case EItemShopPacketType::DG_REMOVE_DISCOUNT:
		std::memcpy(&tDBPacket.tDiscount, pPacket, sizeof(tDBPacket.tDiscount));
		break;
	default:
		sys_err("Unsupported type: %d", static_cast<int>(eType));
		return;
	}

	tDBPacket.bHeader = HEADER_DG_ITEMSHOP;
	tDBPacket.ePacketType = eType;

	if (pPeer)
	{
		pPeer->EncodeHeader(tDBPacket.bHeader, 0, sizeof(TItemShopUniversalPacket));
		pPeer->Encode(&tDBPacket, sizeof(TItemShopUniversalPacket));
	}
	else
	{
		CClientManager::instance().ForwardPacket(tDBPacket.bHeader, &tDBPacket, sizeof(tDBPacket));
	}
}
#endif

