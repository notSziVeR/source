#include "stdafx.h"
#include "OfflineShopSearch.h"
#include "buffer_manager.h"
#include "char.h"
#include "desc.h"
#include "item_manager.h"
#include "packet.h"
#include "utils.h"

#include <cctype>

void COfflineShopSearchSession::RequestPage(uint32_t page)
{
	const auto playerDesc = GetPlayer()->GetDesc();
	if (!playerDesc)
	{
		return;
	}

	EraseExpiredItems();

	const auto pageCount = GetPageCount();
	if (page > pageCount)
	{
		page = pageCount;
	}

	page_ = page;

	TPacketGCOfflineShopSearchItems packet;
	packet.header = HEADER_GC_OFFLINE_SHOP_SEARCH_ITEMS;
	packet.page = page;
	packet.pageCount = pageCount;
	packet.itemCount = 0;

	const auto startOffset = GetPageOffset(page);
	const auto endOffset = std::min(GetItems().size(), GetPageOffset(page + 1));
	const auto start = GetItems().begin() + startOffset;
	const auto end = GetItems().begin() + endOffset;

	if (start == end && end == GetItems().end())
	{
		playerDesc->Packet(&packet, sizeof(packet));
		return;
	}

	TEMP_BUFFER buffer(sizeof(TPacketGCOfflineShopSearchItems::item) * (endOffset - startOffset));

	for (auto it = start; it != end; ++it)
	{
		const auto& shopPtr = it->first;
		const auto& itemPtr = it->second;
		if (shopPtr.expired() || itemPtr.expired())
		{
			continue;
		}

		auto offlineShop = shopPtr.lock();
		auto offlineShopItem = itemPtr.lock();

		TPacketGCOfflineShopSearchItems::item item;
		item.id = offlineShopItem->id;
		item.vnum = offlineShopItem->vnum;
		item.count = offlineShopItem->count;
		item.sockets = offlineShopItem->sockets;
		item.attributes = offlineShopItem->attributes;
		item.price = offlineShopItem->price;

		copy_stoca(offlineShop->GetOwnerName(), item.ownerName);

		buffer.write(&item, sizeof(item));

		packet.itemCount++;
	}

	playerDesc->BufferedPacket(&packet, sizeof(packet));
	playerDesc->Packet(buffer.read_peek(), buffer.size());
}

uint32_t COfflineShopSearchSession::GetPageOffset(uint32_t page) const
{
	const auto index = page > 0 ? page - 1 : page;
	return index * ITEMS_PER_PAGE;
}

uint32_t COfflineShopSearchSession::GetPageCount() const
{
	return (GetItems().size() + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE;
}

uint32_t COfflineShopSearchSession::GetPage() const
{
	return page_;
}

void COfflineShopSearchSession::Buy(CHARACTER* player, uint32_t shopId, uint8_t itemPosition)
{
	if (!CanBuy() || !player)
	{
		return;
	}

	auto shop = COfflineShop::Get(shopId);
	if (!shop || !shop->get()->HasItem(itemPosition))
	{
		player->ChatPacket(CHAT_TYPE_INFO, "The shop is closed or the item has been sold already.");

		RequestPage(GetPage());
		return;
	}

	shop->get()->BuyItem(player, itemPosition);
	RequestPage(GetPage());
}

void COfflineShopSearchSession::SetFilter(const Filter& filter)
{
	filter_ = filter;

	CollectItems();
}

void COfflineShopSearchSession::CollectItems()
{
	items_.clear();

	for (const auto& shopEntry : COfflineShop::GetShops())
	{
		if (shopEntry.second->IsClosed())
		{
			continue;
		}

		for (const auto& itemEntry : shopEntry.second->GetItems())
		{
			if (!IsValidItem(itemEntry.second))
			{
				continue;
			}

			items_.push_back({ ShopWeakPtr(shopEntry.second), ItemWeakPtr(itemEntry.second) });
		}
	}

	std::sort(items_.begin(), items_.end(), [](const ItemVector::value_type & a, const ItemVector::value_type & b)
	{
		return a.second.lock()->price < b.second.lock()->price;
	});
}

void COfflineShopSearchSession::EraseExpiredItems()
{
	items_.erase(std::remove_if(items_.begin(), items_.end(), [](const ItemVector::value_type & elem)
	{
		return elem.first.expired() || elem.second.expired();
	}), items_.end());
}

bool COfflineShopSearchSession::IsValidItem(const std::shared_ptr<TOfflineShopItem>& itemPtr) const
{
	const auto& item = *itemPtr.get();

	const auto itemTablePtr = ITEM_MANAGER::Instance().GetTable(item.vnum);
	if (!itemTablePtr)
	{
		return false;
	}

	const auto& itemTable = *itemTablePtr;

	return CheckJob(itemTable, filter_.job) &&
		   CheckItemType(itemTable, filter_.itemType) &&
		   CheckItemSubType(itemTable, filter_.itemSubType) &&
		   CheckLevelRange(itemTable, filter_.levelRange) &&
		   CheckItemLevelRange(itemTable, filter_.itemLevelRange) &&
		   CheckPriceRange(item.price, filter_.priceRange) &&
		   CheckName(itemTable, filter_.name);
}

bool COfflineShopSearchSession::CheckJob(const TItemTable& table, const uint8_t& job) const
{
	uint32_t antiFlag = 0;

	switch (job)
	{
	case JOB_WARRIOR:
		antiFlag = ITEM_ANTIFLAG_WARRIOR;
		break;
	case JOB_ASSASSIN:
		antiFlag = ITEM_ANTIFLAG_ASSASSIN;
		break;
	case JOB_SURA:
		antiFlag = ITEM_ANTIFLAG_SURA;
		break;
	case JOB_SHAMAN:
		antiFlag = ITEM_ANTIFLAG_SHAMAN;
		break;
		// case JOB_WOLFMAN: antiFlag = ITEM_ANTIFLAG_WOLFMAN; break;
	}

	return (table.dwAntiFlags & antiFlag) == 0;
}

bool COfflineShopSearchSession::CheckItemType(const TItemTable& table, const uint8_t& itemType) const
{
	return table.bType == itemType;
}

bool COfflineShopSearchSession::CheckItemSubType(const TItemTable& table, const uint8_t& itemSubType) const
{
	return table.bSubType == itemSubType;
}

bool COfflineShopSearchSession::CheckLevelRange(const TItemTable& table, const range<uint8_t>& levelRange) const
{
	for (auto it = std::begin(table.aLimits); it != std::end(table.aLimits); ++it)
	{
		if (it->bType != LIMIT_LEVEL)
		{
			continue;
		}

		return it->lValue == 0 || levelRange.in_range(it->lValue);
	}

	return true;
}

bool COfflineShopSearchSession::CheckItemLevelRange(const TItemTable& table, const range<uint8_t>& itemLevelRange) const
{
	if (table.bType != ITEM_WEAPON && table.bType != ITEM_ARMOR)
	{
		return true;
	}

	return itemLevelRange.in_range(table.dwVnum % 10);
}

bool COfflineShopSearchSession::CheckPriceRange(const int64_t& price, const range<int64_t>& priceRange) const
{
	return priceRange.in_range(price);
}

bool COfflineShopSearchSession::CheckName(const TItemTable& table, const std::string& namePattern) const
{
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	std::string name(LC::TranslateItemName(table.dwVnum));
#else
	LPITEM item = ITEM_MANAGER::instance().CreateItem(table.dwVnum, 1);

	std::string name(item->GetName());
#endif

	auto it = std::search(name.begin(), name.end(),
						  namePattern.begin(), namePattern.end(),
	[](char a, char b) { return std::tolower(a) == std::tolower(b); });

#ifndef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	M2_DESTROY_ITEM(item);
#endif

	return it != name.end();
}

CHARACTER* COfflineShopSearchSession::GetPlayer() const
{
	return player_;
}

const COfflineShopSearchSession::Filter& COfflineShopSearchSession::GetFilter() const
{
	return filter_;
}

bool COfflineShopSearchSession::CanBuy() const
{
	return canBuy_;
}

const COfflineShopSearchSession::ItemVector& COfflineShopSearchSession::GetItems() const
{
	return items_;
}