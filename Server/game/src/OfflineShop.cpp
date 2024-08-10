#include "stdafx.h"
#include "OfflineShop.h"
#include "DynamicPacket.h"
#include "buffer_manager.h"
#include "char.h"
#include "char_manager.h"
#include "config.h"
#include "desc_client.h"
#include "item_manager.h"
#include "p2p.h"
#include "questmanager.h"
#include "sectree_manager.h"
#include "utils.h"
#include "log.h"
#ifdef __SHOP_SEARCH__
	#include "ShopSearchManager.h"
#endif

#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#include <sstream>
#include <algorithm>

#undef min

std::vector<uint32_t> g_offlineShopAllowedMaps = { 1, 41 };
uint32_t g_offlineShopMaxPerPlayer = 1;
uint32_t g_offlineShopMaxDistance = 8000;

#define GET_ITEM_TABLE(vnum) ITEM_MANAGER::Instance().GetTable(vnum)

EVENTFUNC(offline_shop_timed_item_event)
{
	COfflineShop::ptr_event_info* info = dynamic_cast<COfflineShop::ptr_event_info*>(event->info);
	if (info == NULL)
	{
		sys_err("offline_shop_timed_item_event <Factor> Null pointer");
		return 0;
	}

	COfflineShop* shop = info->shop;
	if (shop->OnTimedItemEvent())
	{
		return 0;
	}

	return PASSES_PER_SEC(60);
}

bool ValidateName(const std::string& shopName)
{
	if (shopName.size() < 3 || shopName.size() > SHOP_TAB_NAME_MAX)
	{
		return false;
	}

	return true;
}

bool CanAddItem(CItem* item)
{
	if (!item)
	{
		return false;
	}

	static const auto ANTI_FLAGS = ITEM_ANTIFLAG_MYSHOP |
								   ITEM_ANTIFLAG_GIVE;

	return (!IS_SET(item->GetAntiFlag(), ANTI_FLAGS) && CanModifyItem(item));
}

bool IsAllowedMap(uint32_t index)
{
	auto it = std::find(g_offlineShopAllowedMaps.begin(), g_offlineShopAllowedMaps.end(), index);
	return it != g_offlineShopAllowedMaps.end();
}

bool HasReachedLimit(CHARACTER* player)
{
	if (!player)
	{
		return true;
	}

	return COfflineShop::GetPlayerShops(player->GetPlayerID()).size() >= g_offlineShopMaxPerPlayer;
}

std::map<uint32_t, COfflineShop::create_queue_info> COfflineShop::createQueue_;
std::map<uint32_t, COfflineShop::SharedPtr> COfflineShop::shops_;
std::map<uint32_t, std::set<uint32_t> > COfflineShop::playerToShops_;
std::queue<COfflineShop::WeakPtr> COfflineShop::openingTimeChecks_;

void COfflineShop::EnqueueCreate(uint32_t ownerPid, const std::string& ownerName,
								 const std::string& shopName, uint8_t channel, uint32_t mapIndex, uint32_t x, uint32_t y,
								 uint32_t decoRace, uint8_t decoBoard, uint32_t openingTime,
								 const std::vector<create_queue_item_info>& itemInfos)
{
	static uint32_t nextId = 0;

	for (const auto& itemInfo : itemInfos)
	{
		itemInfo.item->SetSkipSave(true);
		itemInfo.item->RemoveFromCharacter();
	}

	createQueue_.insert({nextId, {ownerPid, ownerName,
								  shopName, channel,
								  mapIndex, x, y,
								  decoRace, decoBoard,
								  openingTime,
								  itemInfos
								 }});

	db_clientdesc->DBPacket(HEADER_GD_REQUEST_OFFLINE_SHOP_ID, 0, &nextId, sizeof(uint32_t));

	nextId++;
}

void COfflineShop::DequeueCreate(uint32_t queueId, uint32_t shopId)
{
	auto createInfo = createQueue_.at(queueId);

	auto shop = std::make_shared<COfflineShop>(shopId, createInfo);
	shops_.insert({ shop->GetId(), shop });

	auto& playerShops = GetPlayerShops(shop->GetOwnerPid());
	playerShops.insert(shop->GetId());

	auto player = CHARACTER_MANAGER::Instance().FindByPID(shop->GetOwnerPid());
	if (player)
	{
		player->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShop %u \"%s\"", shop->GetId(), shop->GetName().c_str());
	}

	shop->SpawnKeeper();
	StartOpeningTimeCheck(shop);

	createQueue_.erase(queueId);
}

const COfflineShop::SharedPtr* COfflineShop::Get(uint32_t id)
{
	auto it = shops_.find(id);
	if (it == shops_.end())
	{
		return nullptr;
	}

	return &it->second;
}

void COfflineShop::LoadShop(const TOfflineShop& data, DESC* realListener)
{
	sys_log(0, "OfflineShop :: LoadShop(%u, %s)", data.id, realListener ? "fake" : "real");

	auto shop = std::make_shared<COfflineShop>(data, realListener);

	auto& playerShops = GetPlayerShops(shop->GetOwnerPid());

	shops_.insert({ shop->GetId(), shop });
	playerShops.insert(shop->GetId());
}

void COfflineShop::DestroyShop(uint32_t id, bool isRealDestroy)
{
	const auto it = shops_.find(id);
	if (it == shops_.end())
	{
		return;
	}

	sys_log(0, "OfflineShop :: DestroyShop(%u, %s)", id, isRealDestroy ? "true" : "false");

	if (isRealDestroy)
	{
		db_clientdesc->DBPacket(HEADER_GD_OFFLINE_SHOP_DESTROY, 0, &id, sizeof(id));
	}

	const auto& shop = it->second;
	const auto playerId = shop->GetOwnerPid();
	auto& playerShops = GetPlayerShops(playerId);

	shop->CancelTimedItemEvent();
	shops_.erase(id);
	playerShops.erase(id);
}

uint32_t COfflineShop::GetShopCount()
{
	return shops_.size();
}

std::set<uint32_t>& COfflineShop::GetPlayerShops(uint32_t playerId)
{
	auto it = playerToShops_.find(playerId);
	if (it == playerToShops_.end())
	{
		auto ret = playerToShops_.emplace(playerId, std::set<uint32_t> {});

		return ret.first->second;
	}

	return it->second;
}

int32_t COfflineShop::ReceivePacket(CHARACTER* player, const char* data, uint32_t leftSize)
{
	if (!player || !data)
	{
		return -1;
	}

	CDynamicPacket dynamicPacket(data, leftSize);

	auto headerPacket = dynamicPacket.Get<net_offline_shop::CG_packet>(false);
	if (!headerPacket)
	{
		return -1;
	}

	if (headerPacket->subHeader == net_offline_shop::HEADER_CG_CREATE)
	{
		auto packet = dynamicPacket.Get<net_offline_shop::CG_create_packet>();
		if (!packet)
		{
			return -1;
		}

		// Receive items
		std::vector<net_offline_shop::CG_item_info> itemInfos(packet->itemCount);
		for (auto i = 0; i < packet->itemCount; ++i)
		{
			auto itemInfo = dynamicPacket.Get<net_offline_shop::CG_item_info>();
			if (!itemInfo)
			{
				return -1;
			}

			itemInfos[i] = *itemInfo;
		}

		if (HasReachedLimit(player))
		{
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already reached the limit of owned offline shops."));
			return dynamicPacket.GetExtraSize();
		}

		switch (packet->decoRace)
		{
		case 30000:
		case 30002:
		case 30003:
		case 30004:
		case 30005:
		case 30006:
		case 30007:
		case 30008:
			break;

		default:
			sys_err("POTENTIAL HACKER: %s - Tried to exploit offline shop decoration npc!", player->GetName());
			return dynamicPacket.GetExtraSize();
		}

		{
			PIXEL_POSITION mapBasePosition;
			if (!SECTREE_MANAGER::Instance().GetMapBasePositionByMapIndex(player->GetMapIndex(), mapBasePosition))
			{
				return dynamicPacket.GetExtraSize();
			}

			if (!IsAllowedMap(player->GetMapIndex()) ||
					!SECTREE_MANAGER::Instance().IsMovablePosition(player->GetMapIndex(), packet->x + mapBasePosition.x, packet->y + mapBasePosition.y))
			{
				player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot place your shop there."));
				return dynamicPacket.GetExtraSize();
			}
		}

		auto openingItem = player->GetOfflineShopOpeningItem();
		if (!openingItem)
		{
			return dynamicPacket.GetExtraSize();
		}

		std::vector<create_queue_item_info> items;
		int64_t availableGold = GOLD_MAX;
		for (const auto& itemInfo : itemInfos)
		{
			auto item = player->GetItem(itemInfo.inventoryPosition);
			if (!item || item == openingItem || !CanAddItem(item))
			{
				continue;
			}

			if (itemInfo.price <= 0)
			{
				break;
			}

			availableGold -= itemInfo.price;
			if (availableGold < 0)
			{
				break;
			}

			// Ignore equipped items
			if (item->IsEquipped())
			{
				continue;
			}

			items.push_back({ itemInfo.shopPosition, itemInfo.price, item });
		}

		if (availableGold < 0)
		{
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The sum of prices in your shop is too high."));
			return dynamicPacket.GetExtraSize();
		}

		std::string shopName(packet->shopName.data());
		if (!ValidateName(shopName))
		{
			return dynamicPacket.GetExtraSize();
		}

		if (player->GetGold() < openingItem->GetValue(COfflineShop::ITEM_GOLD_IDX))
		{
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough gold."));
			return dynamicPacket.GetExtraSize();
		}

		if (items.size() < 1)
		{
			return dynamicPacket.GetExtraSize();
		}

		std::string ownerName(player->GetName());
		COfflineShop::EnqueueCreate(player->GetPlayerID(), ownerName, shopName, g_bChannel, player->GetMapIndex(), packet->x, packet->y, packet->decoRace, packet->decoBoard, openingItem->GetValue(COfflineShop::ITEM_TIME_IDX), items);

		openingItem->SetCount(openingItem->GetCount() - 1);

		player->SetOfflineShopOpeningItem(nullptr);
		player->SetOpeningOfflineShopState(false);

		return dynamicPacket.GetExtraSize();
	}

	auto shopPtr = Get(headerPacket->id);
	if (!shopPtr)
	{
		return dynamicPacket.GetExtraSize();
	}

	auto shop = shopPtr->get();
	if (!shop->IsViewer(player))
	{
		return -1;
	}

	switch (headerPacket->subHeader)
	{
	case net_offline_shop::HEADER_CG_REOPEN:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		LPITEM item = NULL;
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		{
			if (LPITEM curItem = player->GetInventoryItem(i))
			{
				if (curItem->GetType() == ITEM_USE && curItem->GetSubType() == USE_OPEN_OFFLINE_SHOP)
				{
					item = curItem;
					break;
				}
			}
		}

		if (!item)
		{
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't own a shop open item."));
			return dynamicPacket.GetExtraSize();
		}

		if (!shop->IsClosed())
		{
			return dynamicPacket.GetExtraSize();
		}

		DWORD itemVnum = item->GetVnum();
		item->SetCount(item->GetCount() - 1);

		if (shop->IsReal())
		{
			shop->Reopen(itemVnum);
		}
		else
		{
			shop->SendRealListenerPacket(net_offline_shop::HEADER_GG_REOPEN, &itemVnum, sizeof(DWORD));
		}

		break;
	}

	case net_offline_shop::HEADER_CG_CLOSE:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		if (shop->IsReal())
		{
			if (!shop->IsClosed())
			{
				shop->Close();
			}
		}
		else
		{
			shop->FakeRequestClose();
		}
		break;
	}
	case net_offline_shop::HEADER_CG_ADD_VIEWER:
	{
		shop->AddViewer(player);
		break;
	}
	case net_offline_shop::HEADER_CG_REMOVE_VIEWER:
	{
		shop->RemoveViewer(player);
		break;
	}
	case net_offline_shop::HEADER_CG_BUY_ITEM:
	{
		auto packet = dynamicPacket.Get<net_offline_shop::CG_buy_item_packet>();
		if (!packet)
		{
			return -1;
		}

		shop->BuyItem(player, packet->shopPosition);
		break;
	}
	case net_offline_shop::HEADER_CG_CHANGE_NAME:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		auto packet = dynamicPacket.Get<net_offline_shop::CG_change_name_packet>();
		if (!packet)
		{
			return -1;
		}

		int remaining_time = shop->GetNameChangeTime() - get_global_time();
		if (remaining_time > 0)
		{
			int mins = remaining_time / 60;
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can change the name of the shop in %d minutes again."), mins);
			return dynamicPacket.GetExtraSize();
		}

		std::string shopName(packet->shopName.data());
		if (!ValidateName(shopName))
		{
			return dynamicPacket.GetExtraSize();
		}

		if (shop->IsClosed())
		{
			return dynamicPacket.GetExtraSize();
		}

		if (shop->IsReal())
		{
			shop->SetName(shopName, get_global_time() + OFFLINE_SHOP_NAME_CHANGE_TIMELIMIT);
		}
		else
		{
			shop->FakeRequestChangeName(shopName, get_global_time() + OFFLINE_SHOP_NAME_CHANGE_TIMELIMIT);
		}
		break;
	}
	case net_offline_shop::HEADER_CG_ADD_ITEM:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		auto packet = dynamicPacket.Get<net_offline_shop::CG_add_item_packet>();
		if (!packet)
		{
			return -1;
		}

		auto& itemInfo = packet->itemInfo;
		auto item = player->GetItem(itemInfo.inventoryPosition);
		if (!item)
		{
			return dynamicPacket.GetExtraSize();
		}

		if (itemInfo.price <= 0)
		{
			return dynamicPacket.GetExtraSize();
		}

		if (shop->IsReal())
		{
			shop->AddItem(itemInfo.shopPosition, item, itemInfo.price);
			sys_err("Trying to add item");
		}
		else
		{
			shop->FakeRequestAddItem(itemInfo.shopPosition, item, itemInfo.price);
			sys_err("Fake add item");
		}
		break;
	}
	case net_offline_shop::HEADER_CG_MOVE_ITEM:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		auto packet = dynamicPacket.Get<net_offline_shop::CG_move_item_packet>();
		if (!packet)
		{
			return -1;
		}

		if (shop->IsReal())
		{
			shop->MoveItem(packet->oldShopPosition, packet->newShopPosition);
		}
		else
		{
			shop->FakeRequestMoveItem(packet->oldShopPosition, packet->newShopPosition);
		}
		break;
	}
	case net_offline_shop::HEADER_CG_REMOVE_ITEM:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		auto packet = dynamicPacket.Get<net_offline_shop::CG_remove_item_packet>();
		if (!packet)
		{
			return -1;
		}

		if (!shop->HasItem(packet->shopPosition))
		{
			return dynamicPacket.GetExtraSize();
		}

		if (shop->IsReal())
		{
			const auto itemData = shop->GetItem(packet->shopPosition);

			auto item = ITEM_MANAGER::Instance().CreateItem(itemData->vnum, itemData->count);
			if (!item)
			{
				return dynamicPacket.GetExtraSize();
			}

			int cell = -1;
			if (packet->inventoryPosition == NPOS ||
					!player->IsEmptyItemGrid(packet->inventoryPosition, item->GetSize()))
			{
				cell = item->IsDragonSoul()
					   ? player->GetEmptyDragonSoulInventory(item)
					   : player->GetEmptyInventory(item->GetSize());

				if (cell == -1)
				{
					player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough free inventory space."));
					return dynamicPacket.GetExtraSize();
				}
			}

			auto ownerPid = shop->GetOwnerPid();
			auto shopId = shop->GetId();
			if (shop->RemoveItem(packet->shopPosition))
			{
				LogManager::Instance().OfflineShopLog(ownerPid, shopId, "REMOVE_ITEM", 0, item->GetID(), itemData->vnum, itemData->count, 0LL);

				item->SetSockets(itemData->sockets);
				item->SetAttributes(itemData->attributes.data());

#ifdef __TRANSMUTATION_SYSTEM__
				item->SetTransmutate(itemData->transmutation);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
				item->SetRefineElement(itemData->refineElement);
#endif

				if (packet->inventoryPosition != NPOS &&
						player->IsEmptyItemGrid(packet->inventoryPosition, item->GetSize()))
				{
					item->AddToCharacter(player, packet->inventoryPosition);
				}
				else
				{
					item->AddToCharacter(player, TItemPos(item->IsDragonSoul() ? DRAGON_SOUL_INVENTORY : INVENTORY, cell));
					LogManager::Instance().ItemLog(player, item, "SYSTEM", item->GetName());
				}
			}
		}
		else
		{
			shop->FakeRequestRemoveItem(packet->shopPosition);
		}

		break;
	}
	case net_offline_shop::HEADER_CG_WITHDRAW:
	{
		if (shop->GetOwnerPid() != player->GetPlayerID())
		{
			return dynamicPacket.GetExtraSize();
		}

		auto packet = dynamicPacket.Get<net_offline_shop::CG_withdraw_packet>();
		if (!packet)
		{
			return -1;
		}

		auto gold = packet->gold;
		if (gold < 0 || gold > shop->GetGold())
		{
			gold = shop->GetGold();
		}

		if (player->GetGold() + gold > GOLD_MAX)
		{
			player->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too much gold on you."));
			break;
		}

		if (shop->IsReal())
		{
			LogManager::Instance().OfflineShopLog(shop->GetOwnerPid(), shop->GetId(), "WITHDRAW_GOLD_NORM", 0, 0, 0, 0, gold);

			shop->SetGold(shop->GetGold() - gold);

			player->PointChange(POINT_GOLD, gold);
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(player, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, static_cast<uint64_t>(gold) });
#endif

		}
		else
		{
			LogManager::Instance().OfflineShopLog(shop->GetOwnerPid(), shop->GetId(), "WITHDRAW_GOLD_BEFORE_P2P", 0, 0, 0, 0, gold);

			shop->FakeRequestWithdrawGold(gold);
		}
		break;
	}
	}

	return dynamicPacket.GetExtraSize();
}

void COfflineShop::StartOpeningTimeCheck(const SharedPtr& shop)
{
	openingTimeChecks_.push(WeakPtr(shop));
}

void COfflineShop::Process()
{
	auto i = std::min<int32_t>(10, openingTimeChecks_.size());

	auto time = static_cast<uint32_t>(get_global_time());
	while (openingTimeChecks_.size() > 0 && i > 0)
	{
		auto ptr = openingTimeChecks_.front();
		openingTimeChecks_.pop();

		auto shop = ptr.lock();
		if (!shop)
		{
			// Shop is not existent anymore
			continue;
		}
		else if (shop->IsClosed())
		{
			// Shop closed earlier
			continue;
		}

		if (shop->GetOpeningTime() < time)
		{
			shop->Close();
		}
		else
		{
			openingTimeChecks_.push(ptr);
		}

		--i;
	}
}

void COfflineShop::OpenShops()
{
	std::vector<uint32_t> destroy;

	for (const auto& entry : shops_)
	{
		auto shop = entry.second.get();

		if (shop->GetItemCount() == 0 && shop->GetGold() == 0)
		{
			destroy.push_back(shop->GetId());
			continue;
		}

		shop->SpawnKeeper();

		if (!shop->IsClosed())
		{
			StartOpeningTimeCheck(entry.second);
		}
	}

	for (const auto& id : destroy)
	{
		COfflineShop::DestroyShop(id, true);
	}
}

const std::map<uint32_t, COfflineShop::SharedPtr>& COfflineShop::GetShops()
{
	return shops_;
}

void COfflineShop::Login(uint32_t pid, DESC* desc)
{
	sys_log(0, "OfflineShop :: Login(%u) -> P2P", pid);

	if (!desc)
	{
		return;
	}

	const auto& playerShops = GetPlayerShops(pid);
	for (const auto& id : playerShops)
	{
		auto shopPtr = Get(id);
		if (!shopPtr)
		{
			continue;
		}

		auto shop = shopPtr->get();
		shop->StopOpeningTime();

		if (!shop->IsReal())
		{
			continue;
		}

		shop->SetFakeListener(desc);
	}
}

void COfflineShop::Login(CHARACTER* player)
{
	sys_log(0, "OfflineShop :: Login(%u)", player->GetPlayerID());

	if (!player)
	{
		return;
	}

	const auto& playerShops = GetPlayerShops(player->GetPlayerID());
	for (const auto& id : playerShops)
	{
		auto shopPtr = Get(id);
		if (!shopPtr)
		{
			continue;
		}

		auto shop = shopPtr->get();
		shop->StopOpeningTime();
		player->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShop %u \"%s\"", shop->GetId(), shop->GetName().c_str());
	}
}

void COfflineShop::Logout(uint32_t pid)
{
	sys_log(0, "OfflineShop :: Logout(%u)", pid);

	const auto playerShops = GetPlayerShops(pid);
	for (const auto& id : playerShops)
	{
		auto shopPtr = Get(id);
		if (!shopPtr)
		{
			continue;
		}

		auto shop = shopPtr->get();
		shop->ContinueOpeningTime();

		sys_log(0, "    -> %s", shop->IsReal() ? "REAL" : "FAKE");

		if (!shop->IsReal())
		{
			DestroyShop(shop->GetId(), false);
			continue;
		}

		shop->SetFakeListener(nullptr);
	}
}

void COfflineShop::DestroyAll()
{
	while (openingTimeChecks_.size())
	{
		openingTimeChecks_.pop();
	}

	shops_.clear();
	playerToShops_.clear();
}

int32_t COfflineShop::ReceiveInterprocessPacket(DESC* desc, const char* data)
{
	if (!data)
	{
		return 0;
	}

	CDynamicPacket dynamicPacket(data);

	auto headerPacket = *dynamicPacket.Get<net_offline_shop::GG_packet>(false);

	switch (headerPacket.subHeader)
	{
	case net_offline_shop::HEADER_GG_SHOP:
	{
		const auto& shopData = *dynamicPacket.Get<TOfflineShop>();

		COfflineShop::LoadShop(shopData, desc);
		auto& shop = *COfflineShop::Get(shopData.id);

		if (shop)
		{
			auto itemCount = *dynamicPacket.Get<uint32_t>();
			while (itemCount > 0)
			{
				shop->AddItem(*dynamicPacket.Get<TOfflineShopItem>());

				--itemCount;
			}

			auto player = CHARACTER_MANAGER::Instance().FindByPID(shop->GetOwnerPid());
			if (player)
			{
				player->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShop %u \"%s\"", shop->GetId(), shop->GetName().c_str());
			}

			if (dynamicPacket.GetExtraSize() != headerPacket.extraSize)
			{
				sys_err("invalid offline shop packet size [header %d expected %d recv %d]", headerPacket.subHeader, headerPacket.extraSize, dynamicPacket.GetExtraSize());
			}
		}

		return dynamicPacket.GetExtraSize();
	}
	case net_offline_shop::HEADER_GG_GIVE_ITEM:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_give_item_packet>();

		auto player = CHARACTER_MANAGER::Instance().FindByPID(packet.pid);
		if (!player)
		{
			sys_err("Player is not online anymore!");
			return dynamicPacket.GetExtraSize();
		}

		auto item = ITEM_MANAGER::Instance().CreateItem(packet.vnum, packet.count);
		if (!item)
		{
			return dynamicPacket.GetExtraSize();
		}

		item->SetSockets(packet.sockets);
		item->SetAttributes(packet.attributes.data());
#ifdef __TRANSMUTATION_SYSTEM__
		item->SetTransmutate(packet.transmutation);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		item->SetRefineElement(packet.refineElement);
#endif

		player->AutoGiveItem(item);

		if (dynamicPacket.GetExtraSize() != headerPacket.extraSize)
		{
			sys_err("invalid offline shop packet size [header %d expected %d recv %d]", headerPacket.subHeader, headerPacket.extraSize, dynamicPacket.GetExtraSize());
		}
		return dynamicPacket.GetExtraSize();
	}
	case net_offline_shop::HEADER_GG_GIVE_GOLD:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_give_gold_packet>();

		auto player = CHARACTER_MANAGER::Instance().FindByPID(packet.pid);
		if (!player)
		{
			sys_err("Player is not online anymore!");
			return dynamicPacket.GetExtraSize();
		}

		auto shopPtr = COfflineShop::Get(packet.sid);

		auto shop = shopPtr ? shopPtr->get() : NULL;

		if (shop)
		{
			player->PointChange(POINT_GOLD, packet.gold, true);

			LogManager::Instance().OfflineShopLog(shop->GetOwnerPid(), shop->GetId(), "WITHDRAW_GOLD_AFTER", 0, 0, 0, 0, packet.gold);
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(player, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, static_cast<uint64_t>(packet.gold) });
#endif
			//			shop->SetFakeGold(shop->GetGold() - packet.gold);

			net_offline_shop::GC_gold_packet packets = { shop->GetGold() };
			shop->SendViewerPacket(net_offline_shop::HEADER_GC_GOLD, &packets, sizeof(packets), player, player->GetDesc());

			if (dynamicPacket.GetExtraSize() != headerPacket.extraSize)
			{
				sys_err("invalid offline shop packet size [header %d expected %d recv %d]", headerPacket.subHeader, headerPacket.extraSize, dynamicPacket.GetExtraSize());
			}
		}
		else
		{
			LogManager::Instance().OfflineShopLog(packet.pid, packet.sid, "WIDTHRAW_GOLD_AFTER_DESTROY", 0, 0, 0, 0, packet.gold);
			player->PointChange(POINT_GOLD, packet.gold, true);
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(player, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, static_cast<uint64_t>(packet.gold) });
#endif
		}


		return dynamicPacket.GetExtraSize();
	}
	case net_offline_shop::HEADER_GG_REFRESH_GOLD:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_refresh_gold_packet>();
		auto shopPtr = COfflineShop::Get(packet.sid);

		auto shop = shopPtr ? shopPtr->get() : NULL;

		if (shop)
		{
			auto player = CHARACTER_MANAGER::Instance().FindByPID(shop->GetOwnerPid());
			if (!player)
			{
				sys_err("Player is not online anymore!");
				return dynamicPacket.GetExtraSize();
			}

			shop->SetFakeGold(packet.gold);

			net_offline_shop::GC_gold_packet packets = { packet.gold };
			shop->SendViewerPacket(net_offline_shop::HEADER_GC_GOLD, &packets, sizeof(packets), player, player->GetDesc());
		}

		return dynamicPacket.GetExtraSize();
	}
	}

	auto shopPtr = COfflineShop::Get(headerPacket.id);

	auto shop = shopPtr ? shopPtr->get() : NULL;
	switch (headerPacket.subHeader)
	{
	case net_offline_shop::HEADER_GG_CLOSE:
	{
		if (shop)
		{
			shop->Close();
		}
		break;
	}
	case net_offline_shop::HEADER_GG_DESTROY:
	{
		if (shop)
		{
			COfflineShop::DestroyShop(shop->GetId(), false);
		}
		break;
	}
	case net_offline_shop::HEADER_GG_REOPEN:
	{
		const auto& itemVnum = *dynamicPacket.Get<DWORD>();

		if (shop)
		{
			shop->Reopen(itemVnum);
		}

		break;
	}
	case net_offline_shop::HEADER_GG_NAME:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_name_packet>();

		std::string shopName(packet.shopName.data());
		if (shop)
		{
			shop->SetName(shopName, packet.name_change_time);
		}

		break;
	}
	case net_offline_shop::HEADER_GG_ADD_ITEM:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_add_item_packet>();

		if (shop)
		{
			TOfflineShopItem item;
			item.id = { shop->GetId(), packet.shopPosition };
			item.vnum = packet.vnum;
			item.count = packet.count;
			std::copy(packet.attributes.begin(), packet.attributes.end(), item.attributes.begin());
			std::copy(packet.sockets.begin(), packet.sockets.end(), item.sockets.begin());
			item.price = packet.price;
#ifdef __TRANSMUTATION_SYSTEM__
			item.transmutation = packet.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
			item.refineElement = packet.refineElement;
#endif

			if (!shop->AddItem(item) && shop->IsReal())
			{
				net_offline_shop::GG_packet backHeaderPacket =
				{
					HEADER_GG_OFFLINE_SHOP,
					net_offline_shop::HEADER_GG_GIVE_ITEM,
					sizeof(net_offline_shop::GG_give_item_packet),
					shop->GetId()
				};

				net_offline_shop::GG_give_item_packet backPacket;
				backPacket.vnum = packet.vnum;
				backPacket.count = packet.count;
#ifdef __TRANSMUTATION_SYSTEM__
				backPacket.transmutation = packet.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
				backPacket.refineElement = packet.refineElement;
#endif
				std::copy(packet.attributes.begin(), packet.attributes.end(), backPacket.attributes.begin());
				std::copy(packet.sockets.begin(), packet.sockets.end(), backPacket.sockets.begin());

				desc->BufferedPacket(&backHeaderPacket, sizeof(backHeaderPacket));
				desc->Packet(&backPacket, sizeof(backPacket));
			}
		}

		break;
	}
	case net_offline_shop::HEADER_GG_MOVE_ITEM:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_move_item_packet>();
		if (shop)
		{
			shop->MoveItem(packet.oldShopPosition, packet.newShopPosition);
		}

		break;
	}
	case net_offline_shop::HEADER_GG_REMOVE_ITEM:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_remove_item_packet>();
		if (shop)
		{
			shop->RemoveItem(packet.shopPosition, shop->IsReal());
		}
		break;
	}
	case net_offline_shop::HEADER_GG_GOLD:
	{
		const auto& packet = *dynamicPacket.Get<net_offline_shop::GG_gold_packet>();

		if (packet.gold > 0 && packet.gold <= shop->GetGold() && shop)
		{
			net_offline_shop::GG_packet backHeaderPacket =
			{
				HEADER_GG_OFFLINE_SHOP,
				net_offline_shop::HEADER_GG_GIVE_GOLD,
				sizeof(net_offline_shop::GG_give_gold_packet),
				shop->GetOwnerPid()
			};

			net_offline_shop::GG_give_gold_packet backPacket;
			backPacket.pid = shop->GetOwnerPid();
			backPacket.gold = packet.gold;
			backPacket.sid = shop->GetId();

			shop->SetGold(shop->GetGold() - packet.gold);

			desc->BufferedPacket(&backHeaderPacket, sizeof(backHeaderPacket));
			desc->Packet(&backPacket, sizeof(backPacket));
			if (shop->IsReal())
			{
				net_offline_shop::GG_refresh_gold_packet pkg;
				pkg.sid = shop->GetId();
				pkg.gold = shop->GetGold();
				shop->SendFakeListenerPacket(net_offline_shop::HEADER_GG_REFRESH_GOLD, &pkg, sizeof(pkg));
			}
		}

		break;
	}
	}

	if (dynamicPacket.GetExtraSize() != headerPacket.extraSize)
	{
		sys_err("invalid offline shop packet size [header %d expected %d recv %d]", headerPacket.subHeader, headerPacket.extraSize, dynamicPacket.GetExtraSize());
	}

	return dynamicPacket.GetExtraSize();
}

void COfflineShop::PeerDisconnect(DESC* peer)
{
	for (const auto& entry : shops_)
	{
		auto shop = entry.second.get();

		if (shop->realListener_ == peer)
		{
			shop->realListener_ = NULL;
		}
		if (shop->fakeListener_ == peer)
		{
			shop->fakeListener_ = NULL;
		}
	}
}

COfflineShop::COfflineShop(const TOfflineShop& data, DESC* realListener)
	: id_(data.id), ownerPid_(data.ownerPid), ownerName_(data.ownerName.data()), shopName_(data.shopName.data()), shopNameChangeTime_(data.shopNameChangeTime),
	  channel_(data.channel), mapIndex_(data.mapIndex), x_(data.x), y_(data.y), decoRace_(data.decoRace),
	  decoBoard_(data.decoBoard), openingTime_(data.openingTime), openingTimeRunning_(true), gold_(data.gold),
	  itemGrid_(OFFLINE_SHOP_ITEM_WIDTH, OFFLINE_SHOP_ITEM_HEIGHT),
	  keeper_(nullptr), skipSave_(false), isReal_(realListener == NULL), realListener_(realListener), fakeListener_(nullptr), clearTimedItemEvent_(nullptr)
{
}

COfflineShop::COfflineShop(uint32_t id, const create_queue_info& createInfo)
	: id_(id), ownerPid_(createInfo.ownerPid), ownerName_(createInfo.ownerName),
	  shopName_(createInfo.shopName), shopNameChangeTime_(0), channel_(createInfo.channel), mapIndex_(createInfo.mapIndex), x_(createInfo.x), y_(createInfo.y),
	  decoRace_(createInfo.decoRace), decoBoard_(createInfo.decoBoard),
	  itemGrid_(OFFLINE_SHOP_ITEM_WIDTH, OFFLINE_SHOP_ITEM_HEIGHT), gold_(0),
	  keeper_(nullptr), skipSave_(false), isReal_(true), realListener_(nullptr), fakeListener_(nullptr), clearTimedItemEvent_(nullptr)
{
	SetSkipSave(true);

	SetOpeningTime(get_global_time() + createInfo.openingTime);

	openingTimeRunning_ = CHARACTER_MANAGER::instance().FindByPID(ownerPid_) == NULL && P2P_MANAGER::instance().FindByPID(ownerPid_) == NULL;
	if (!openingTimeRunning_)
	{
		openingTime_ = createInfo.openingTime;
	}

	if (shopName_.size() > SHOP_TAB_NAME_MAX)
	{
		shopName_.erase(SHOP_TAB_NAME_MAX, std::string::npos);
	}

	for (const auto& itemInfo : createInfo.items)
	{
		AddItem(itemInfo.shopPosition, itemInfo.item, itemInfo.price);
	}

	SetSkipSave(false);
	Save();
	for (const auto& entry : items_)
	{
		SaveItem(entry.first);
	}
}

COfflineShop::~COfflineShop()
{
	DespawnKeeper();
	RemoveAllViewers();

	auto owner = CHARACTER_MANAGER::Instance().FindByPID(GetOwnerPid());
	if (owner)
	{
		owner->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShopDestroy %u", GetId());
	}

	event_cancel(&clearTimedItemEvent_);
}

void COfflineShop::SendSpawnPacket(CHARACTER* player)
{
	if (!IsReal() || (IsClosed() && player->GetPlayerID() != GetOwnerPid()))
	{
		return;
	}

	net_offline_shop::GC_spawn_packet packet;
	packet.vid = GetVID();
	packet.decoBoard = GetDecoBoard();

	std::copy(GetName().begin(), GetName().end(), packet.shopName.begin());
	packet.shopName[GetName().size()] = '\0'; // Manually null-terminate

	SendViewerPacket(net_offline_shop::HEADER_GC_SPAWN, &packet, sizeof(packet), player);
}

void COfflineShop::StartTimedItemEvent()
{
	if (clearTimedItemEvent_)
	{
		return;
	}

	if (!IsReal())
	{
		return;
	}

	ptr_event_info* info = AllocEventInfo<ptr_event_info>();
	info->shop = this;
	clearTimedItemEvent_ = event_create(offline_shop_timed_item_event, info, PASSES_PER_SEC(60));
}

bool COfflineShop::OnTimedItemEvent()
{
	std::set<uint8_t> itemsToRemove;

	for (auto it : items_)
	{
		std::shared_ptr<TOfflineShopItem> item = it.second;

		const auto itemTable = GET_ITEM_TABLE(item->vnum);
		if (!itemTable)
		{
			continue;
		}

		for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
		{
			if (itemTable->aLimits[i].bType == LIMIT_REAL_TIME || (itemTable->aLimits[i].bType == LIMIT_REAL_TIME_START_FIRST_USE && item->sockets[0] > 1))
			{
				if (get_global_time() >= item->sockets[0])
				{
					LogManager::Instance().OfflineShopLog(GetOwnerPid(), GetId(), "OFFLINE_REALTIME", 0, 0, item->vnum, item->count, item->price);
					itemsToRemove.insert(it.first);
					break;
				}
			}
		}
	}

	bool removed = false;

	for (uint8_t key : itemsToRemove)
	{
		if (GetItemCount() == 1)
		{
			removed = true;
		}
		RemoveItem(key);
	}

	return removed;
}

void COfflineShop::CancelTimedItemEvent()
{
	event_cancel(&clearTimedItemEvent_);
}

bool COfflineShop::AddItem(const TOfflineShopItem& item)
{
	const auto itemTable = GET_ITEM_TABLE(item.vnum);
	if (!itemTable)
	{
		sys_err("Invalid item %u at position %u.", item.vnum, item.id.second);
		return false;
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (itemTable->aLimits[i].bType == LIMIT_REAL_TIME || (itemTable->aLimits[i].bType == LIMIT_REAL_TIME_START_FIRST_USE && item.sockets[0] > 1))
		{
			StartTimedItemEvent();
			break;
		}
	}

	itemGrid_.Put(item.id.second, 1, itemTable->bSize);
	items_.insert({ item.id.second, std::make_shared<TOfflineShopItem>(item) });

#ifdef __SHOP_SEARCH__
	if (IsReal() && !IsClosed())
	{
		CShopSearchManager::RegisterShopItem(GetOwnerPid(), item, GetShopSearchTime());
	}
#endif

	if (IsReal())
	{
		//Saving item from managment core
		SaveItem(item.id.second);
	}

	if (HasFakeListener())
	{
		net_offline_shop::GG_add_item_packet packet;
		packet.vnum = item.vnum;
		packet.count = item.count;
#ifdef __TRANSMUTATION_SYSTEM__
		packet.transmutation = item.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		packet.refineElement = item.refineElement;
#endif
		std::copy(item.attributes.begin(), item.attributes.end(), packet.attributes.begin());
		std::copy(item.sockets.begin(), item.sockets.end(), packet.sockets.begin());
		packet.shopPosition = item.id.second;
		packet.price = item.price;
		SendFakeListenerPacket(net_offline_shop::HEADER_GG_ADD_ITEM, &packet, sizeof(packet));
	}
	else if (viewers_.size() > 0)
	{
		net_offline_shop::GC_add_item_packet packet;
		packet.vnum = item.vnum;
		packet.count = item.count;
#ifdef __TRANSMUTATION_SYSTEM__
		packet.transmutation = item.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		packet.refineElement = item.refineElement;
#endif
		std::copy(item.attributes.begin(), item.attributes.end(), packet.attributes.begin());
		std::copy(item.sockets.begin(), item.sockets.end(), packet.sockets.begin());
		packet.shopPosition = item.id.second;
		packet.price = item.price;
		SendViewerPacket(net_offline_shop::HEADER_GC_ADD_ITEM, &packet, sizeof(packet));
	}

	return true;
}

bool COfflineShop::AddItem(uint8_t shopPosition, CItem* item, int64_t price)
{
	if (IsClosed() || !item || !CanAddItem(item))
	{
		return false;
	}

	// Ignore equipped items
	if (item->IsEquipped())
	{
		return false;
	}

	if (!itemGrid_.IsEmpty(shopPosition, 1, item->GetSize()))
	{
		return false;
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (item->GetLimitType(i) == LIMIT_REAL_TIME || (item->GetLimitType(i) == LIMIT_REAL_TIME_START_FIRST_USE && item->GetSocket(0) > 1))
		{
			StartTimedItemEvent();
			break;
		}
	}

	itemGrid_.Put(shopPosition, 1, item->GetSize());

	TOfflineShopItem itemData;
	itemData.id = { GetId(), shopPosition };
	itemData.vnum = item->GetVnum();
	itemData.count = item->GetCount();
#ifdef __TRANSMUTATION_SYSTEM__
	itemData.transmutation = item->GetTransmutate();
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	itemData.refineElement = item->GetRefineElement();
#endif

	item->CopySockets(itemData.sockets);
	item->CopyAttributes(itemData.attributes);
	itemData.price = price;
	items_.insert({ shopPosition, std::make_shared<TOfflineShopItem>(itemData) });

#ifdef __SHOP_SEARCH__
	if (IsReal() && !IsClosed())
	{
		CShopSearchManager::RegisterShopItem(GetOwnerPid(), itemData, GetShopSearchTime());
	}
#endif

	LogManager::Instance().OfflineShopLog(GetOwnerPid(), GetId(), "ADD_ITEM", 0, item->GetID(), itemData.vnum, itemData.count, itemData.price);

	item->SetSkipSave(false);
	if (item->GetOwner())
	{
		item->RemoveFromCharacter();
	}
	ITEM_MANAGER::Instance().DestroyItem(item);

	SaveItem(shopPosition);

	net_offline_shop::GC_add_item_packet packet;
	packet.vnum = itemData.vnum;
	packet.count = itemData.count;
#ifdef __TRANSMUTATION_SYSTEM__
	packet.transmutation = itemData.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	packet.refineElement = itemData.refineElement;
#endif
	std::copy(itemData.sockets.begin(), itemData.sockets.end(), packet.sockets.begin());
	std::copy(itemData.attributes.begin(), itemData.attributes.end(), packet.attributes.begin());
	packet.shopPosition = shopPosition;
	packet.price = itemData.price;

	SendViewerPacket(net_offline_shop::HEADER_GC_ADD_ITEM, &packet, sizeof(packet));

	if (HasFakeListener())
	{
		net_offline_shop::GG_add_item_packet packet;
		packet.vnum = itemData.vnum;
		packet.count = itemData.count;
#ifdef __TRANSMUTATION_SYSTEM__
		packet.transmutation = itemData.transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		packet.refineElement = itemData.refineElement;
#endif
		std::copy(itemData.attributes.begin(), itemData.attributes.end(), packet.attributes.begin());
		std::copy(itemData.sockets.begin(), itemData.sockets.end(), packet.sockets.begin());
		packet.shopPosition = itemData.id.second;
		packet.price = itemData.price;

		SendFakeListenerPacket(net_offline_shop::HEADER_GG_ADD_ITEM, &packet, sizeof(packet));
	}

	return true;
}

void COfflineShop::MoveItem(uint8_t oldShopPosition, uint8_t newShopPosition)
{
	if (IsClosed() || !HasItem(oldShopPosition))
	{
		return;
	}

	const auto& item = GetItem(oldShopPosition);
	if (!item) return;

	const auto itemTable = GET_ITEM_TABLE(item->vnum);
	if (!itemTable)
	{
		sys_err("Invalid item %u at position %u.", item->vnum, oldShopPosition);
		return;
	}

	if (!itemGrid_.IsEmpty(newShopPosition, 1, itemTable->bSize))
	{
		return;
	}

#ifdef __SHOP_SEARCH__
	if (IsReal())
	{
		CShopSearchManager::UnregisterShopItem(GetOwnerPid(), *item);
	}
#endif

	itemGrid_.Get(oldShopPosition, 1, itemTable->bSize);
	itemGrid_.Put(newShopPosition, 1, itemTable->bSize);

	auto& itemData = items_.at(oldShopPosition);
	itemData->id.second = newShopPosition;

	items_.insert({ newShopPosition, std::move(itemData) });
	items_.erase(oldShopPosition);

	SaveItem(oldShopPosition);
	SaveItem(newShopPosition);

#ifdef __SHOP_SEARCH__
	if (IsReal())
	{
		const auto& newItem = GetItem(newShopPosition);
		if (!itemTable)
		{
			sys_err("Invalid item search move %u at position %u.", newItem->vnum, newShopPosition);
			return;
		}
		CShopSearchManager::RegisterShopItem(GetOwnerPid(), *newItem, GetShopSearchTime());
	}
#endif

	net_offline_shop::GC_move_item_packet packet = { oldShopPosition, newShopPosition };
	SendViewerPacket(net_offline_shop::HEADER_GC_MOVE_ITEM, &packet, sizeof(packet));

	if (IsReal())
	{
		net_offline_shop::GG_move_item_packet packet = { oldShopPosition, newShopPosition };
		SendFakeListenerPacket(net_offline_shop::HEADER_GG_MOVE_ITEM, &packet, sizeof(packet));
	}
}

bool COfflineShop::RemoveItem(uint8_t shopPosition, bool fromFake)
{
	if (!HasItem(shopPosition))
	{
		sys_err("Tried to remove non-existing item from %d.", shopPosition);
		return false;
	}

	const auto& item = GetItem(shopPosition);

	const auto itemTable = GET_ITEM_TABLE(item->vnum);
	if (!itemTable)
	{
		sys_err("Invalid item %u at position %u.", item->vnum, shopPosition);
		return false;
	}

#ifdef __SHOP_SEARCH__
	if (IsReal())
	{
		CShopSearchManager::UnregisterShopItem(GetOwnerPid(), *item);
	}
#endif

	itemGrid_.Get(shopPosition, 1, itemTable->bSize);

	if (fromFake)
	{
		const auto& item = items_.at(shopPosition).get();

		net_offline_shop::GG_give_item_packet packet;
		packet.pid = GetOwnerPid();
		packet.vnum = item->vnum;
		packet.count = item->count;
#ifdef __TRANSMUTATION_SYSTEM__
		packet.transmutation = item->transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		packet.refineElement = item->refineElement;
#endif
		std::copy(item->attributes.begin(), item->attributes.end(), packet.attributes.begin());
		std::copy(item->sockets.begin(), item->sockets.end(), packet.sockets.begin());

		SendFakeListenerPacket(net_offline_shop::HEADER_GG_GIVE_ITEM, &packet, sizeof(packet));
	}

	items_.erase(shopPosition);

	net_offline_shop::GC_remove_item_packet packet = { shopPosition };
	SendViewerPacket(net_offline_shop::HEADER_GC_REMOVE_ITEM, &packet, sizeof(packet));

	if (GetItemCount() == 0)
	{
		event_cancel(&clearTimedItemEvent_);
	}

	if (IsReal())
	{
		SaveItem(shopPosition);

		net_offline_shop::GG_remove_item_packet packet = { shopPosition };
		SendFakeListenerPacket(net_offline_shop::HEADER_GG_REMOVE_ITEM, &packet, sizeof(packet));

		if (GetItemCount() == 0)
		{
			Close();
		}
	}

	return true;
}

void COfflineShop::AddViewer(CHARACTER* player)
{
	if (!player)
	{
		return;
	}

	auto isMyShop = player->GetPlayerID() == GetOwnerPid();
	if (IsClosed())
	{
		if (!isMyShop)
		{
			return;
		}
	}
	else if (!isMyShop && !IsNear(player))
	{
		return;
	}

	auto desc = player->GetDesc();
	if (!player->GetDesc())
	{
		return;
	}

	viewers_.insert(player);
	player->AddViewingOfflineShop(GetId());

	uint32_t size = sizeof(net_offline_shop::GC_packet) +
					sizeof(net_offline_shop::GC_open_shop_packet) +
					sizeof(net_offline_shop::GC_add_item_packet) * GetItemCount();

	TEMP_BUFFER buffer(size);

	net_offline_shop::GC_packet packet =
	{
		HEADER_GC_OFFLINE_SHOP,
		static_cast<uint16_t>(size),
		net_offline_shop::HEADER_GC_OPEN_SHOP,
		GetId()
	};

	buffer.write(&packet, sizeof(packet));

	net_offline_shop::GC_open_shop_packet openPacket;
	openPacket.isMyShop = isMyShop;
	openPacket.isClosed = IsClosed();
	openPacket.itemCount = GetItemCount();

	std::copy(GetName().begin(), GetName().end(), openPacket.shopName.begin());
	openPacket.shopName[GetName().size()] = '\0'; // Manually null-terminate

	buffer.write(&openPacket, sizeof(openPacket));

	for (const auto& entry : items_)
	{
		const auto& item = entry.second;

		net_offline_shop::GC_add_item_packet packet;
		packet.vnum = item->vnum;
		packet.count = item->count;
#ifdef __TRANSMUTATION_SYSTEM__
		packet.transmutation = item->transmutation;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		packet.refineElement = item->refineElement;
#endif
		std::copy(item->sockets.begin(), item->sockets.end(), packet.sockets.begin());
		std::copy(item->attributes.begin(), item->attributes.end(), packet.attributes.begin());
		packet.shopPosition = entry.first;
		packet.price = item->price;

		buffer.write(&packet, sizeof(packet));
	}

	desc->Packet(buffer.read_peek(), buffer.size());

	if (isMyShop)
	{
		net_offline_shop::GC_gold_packet goldPacket = { GetGold() };
		SendViewerPacket(net_offline_shop::HEADER_GC_GOLD, &goldPacket, sizeof(goldPacket), player);

		net_offline_shop::GC_opening_time_packet timePacket = { GetOpeningTime() };
		SendViewerPacket(net_offline_shop::HEADER_GC_OPENING_TIME, &timePacket, sizeof(timePacket), player);

		LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(player->GetMapIndex());
		if (!pMap)
		{
			return;
		}

		net_offline_shop::GC_offlineshop_position positionPacket = { GetChannel(), GetX(), GetY(), GetX() + pMap->m_setting.iBaseX, GetY() + pMap->m_setting.iBaseY };
		SendViewerPacket(net_offline_shop::HEADER_GC_POSITION, &positionPacket, sizeof(positionPacket), player);
	}
}

void COfflineShop::RemoveViewer(CHARACTER* player)
{
	viewers_.erase(player);
	player->RemoveViewingOfflineShop(GetId());

	if (player->GetDesc())
	{
		net_offline_shop::GC_packet packet =
		{
			HEADER_GC_OFFLINE_SHOP,
			sizeof(packet),
			net_offline_shop::HEADER_GC_CLOSE_SHOP,
			GetId()
		};

		player->GetDesc()->Packet(&packet, sizeof(packet));
	}
}

void COfflineShop::RemoveAllViewers(bool exceptOwner)
{
	auto viewers = viewers_;
	for (const auto& viewer : viewers)
	{
		if (exceptOwner && viewer->GetPlayerID() == GetOwnerPid())
		{
			continue;
		}

		RemoveViewer(viewer);
	}
}

bool COfflineShop::IsViewer(CHARACTER* player) const
{
	return viewers_.find(player) != viewers_.end();
}

void COfflineShop::BuyItem(CHARACTER* buyer, uint8_t shopPosition)
{
	if (!buyer || IsClosed())
	{
		return;
	}

	if (!HasItem(shopPosition))
	{
		return;
	}

	const auto& itemData = GetItem(shopPosition);
	if (!itemData)
	{
		return;
	}

	long long price = itemData->price;

	if (buyer->GetGold() < price)
	{
		buyer->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough gold."));
		return;
	}

	auto item = BuyItem(buyer->GetPlayerID(), shopPosition);
	if (!item)
	{
		return;
	}

	buyer->PointChange(POINT_GOLD, -price);

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(buyer, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(price) });
#endif

	buyer->AutoGiveItem(item);
}

LPITEM COfflineShop::BuyItem(DWORD buyerPID, uint8_t shopPosition)
{
	if (!buyerPID || !IsReal() || !HasItem(shopPosition) || IsClosed())
	{
		return NULL;
	}

	const auto& itemData = GetItem(shopPosition);
	if (!itemData)
	{
		return NULL;
	}

	int64_t reachMax = LLONG_MAX - GetGold();
	if (itemData->price > reachMax)
	{
		sys_log(0, "Gold would overflow. Abort the request.");
		return NULL;
	}

	auto item = ITEM_MANAGER::Instance().CreateItem(itemData->vnum, itemData->count);
	if (!item)
	{
		return NULL;
	}

#ifdef __SHOP_SEARCH__
	CShopSearchManager::OnBuyShopItem(itemData->vnum, itemData->count, itemData->price);
#endif

	item->SetSockets(itemData->sockets);
	item->SetAttributes(itemData->attributes.data());
#ifdef __TRANSMUTATION_SYSTEM__
	item->SetTransmutate(itemData->transmutation);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	item->SetRefineElement(itemData->refineElement);
#endif

	SetGold(GetGold() + itemData->price);

	LogManager::Instance().OfflineShopLog(GetOwnerPid(), GetId(), "BUY_ITEM", buyerPID, item->GetID(), itemData->vnum, itemData->count, itemData->price);

	RemoveItem(shopPosition);
	if (IsReal())
	{
		net_offline_shop::GG_refresh_gold_packet pkg;
		pkg.sid = GetId();
		pkg.gold = GetGold();
		SendFakeListenerPacket(net_offline_shop::HEADER_GG_REFRESH_GOLD, &pkg, sizeof(pkg));
	}
	return item;
}

void COfflineShop::SpawnKeeper()
{
	if (!IsReal())
	{
		return;
	}

	auto race = GetDecoRace();
	if (IsClosed())
	{
		race = COfflineShop::CLOSED_RACE;
	}

	PIXEL_POSITION mapBasePosition;
	if (!SECTREE_MANAGER::Instance().GetMapBasePositionByMapIndex(GetMapIndex(), mapBasePosition))
	{
		return;
	}

	auto name = LC_TEXT("OWNER_SHOP") + std::string(GetOwnerName());

	SetKeeper(CHARACTER_MANAGER::Instance().SpawnMob(race,
			  GetMapIndex(), GetX() + mapBasePosition.x, GetY() + mapBasePosition.y, 0,
			  true, -1, false));

	if (GetKeeper())
	{
		GetKeeper()->SetKeepingOfflineShop(GetId());
		GetKeeper()->SetName(name);
		GetKeeper()->Show(GetMapIndex(), GetX() + mapBasePosition.x, GetY() + mapBasePosition.y);
	}
}

void COfflineShop::DespawnKeeper()
{
	if (GetKeeper())
	{
		CHARACTER_MANAGER::Instance().DestroyCharacter(GetKeeper());
		SetKeeper(nullptr);
	}
}

bool COfflineShop::IsNear(CHARACTER* player) const
{
	if (!GetKeeper() || !player)
	{
		return false;
	}

	return static_cast<uint32_t>(DISTANCE_SQRT(GetKeeper()->GetX() - player->GetX(), GetKeeper()->GetY() - player->GetY())) <= g_offlineShopMaxDistance;
}

void COfflineShop::Reopen(DWORD itemVnum)
{
	const TItemTable * pProto = ITEM_MANAGER::instance().GetTable(itemVnum);
	if (!pProto || pProto->bType != ITEM_USE || pProto->bSubType != USE_OPEN_OFFLINE_SHOP)
	{
		return;
	}

	if (!IsClosed())
	{
		return;
	}

	DespawnKeeper();

	int iNewDuration = pProto->alValues[0];
	SetOpeningTime(openingTimeRunning_ ? get_global_time() + iNewDuration : iNewDuration);

	SpawnKeeper();

	if (IsReal())
	{
		SendFakeListenerPacket(net_offline_shop::HEADER_GG_REOPEN, &itemVnum, sizeof(DWORD));
	}
}

void COfflineShop::RequestClose()
{
	if (IsReal())
	{
		Close();
	}
	else
	{
		SendRealListenerPacket(net_offline_shop::HEADER_GG_CLOSE);
	}
}

void COfflineShop::Close()
{
	DespawnKeeper();

	auto hasToBeDestroyed = GetItemCount() == 0 && GetGold() <= 0;

	RemoveAllViewers(!hasToBeDestroyed);

	if (!hasToBeDestroyed)
	{
		sys_err("!hasToBeDestroyed");
		SetOpeningTime(0);
		SpawnKeeper();

		if (IsReal())
		{
			SendFakeListenerPacket(net_offline_shop::HEADER_GG_CLOSE);
		}
	}
	else
	{
		auto owner = CHARACTER_MANAGER::Instance().FindByPID(GetOwnerPid());
		if (owner)
		{
			owner->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShopDestroy %u", GetId());
		}

		if (IsReal())
		{
			SendFakeListenerPacket(net_offline_shop::HEADER_GG_DESTROY);
			COfflineShop::DestroyShop(GetId(), true);
		}
	}
}

bool COfflineShop::IsClosed() const
{
	return GetOpeningTime() == 0;
}

uint32_t COfflineShop::GetVID() const
{
	if (!GetKeeper())
	{
		return 0;
	}

	return GetKeeper()->GetVID();
}

bool COfflineShop::HasItem(uint8_t shopPosition) const
{
	return items_.find(shopPosition) != items_.end();
}

void COfflineShop::SendViewerPacket(uint8_t header, const void* data, uint32_t size, CHARACTER* specificViewer, DESC* desc_def) const
{
	net_offline_shop::GC_packet packet =
	{
		HEADER_GC_OFFLINE_SHOP,
		static_cast<uint16_t>(sizeof(packet) + size),
		header,
		GetId()
	};

	if (desc_def)
	{
		desc_def->BufferedPacket(&packet, sizeof(packet));
		desc_def->Packet(data, size);
		return;
	}

	if (specificViewer)
	{
		auto desc = specificViewer->GetDesc();
		if (!desc)
		{
			return;
		}

		desc->BufferedPacket(&packet, sizeof(packet));
		desc->Packet(data, size);
	}
	else
	{
		for (const auto& viewer : viewers_)
		{
			auto desc = viewer->GetDesc();
			if (!desc)
			{
				continue;
			}

			desc->BufferedPacket(&packet, sizeof(packet));
			desc->Packet(data, size);
		}
	}
}

void COfflineShop::SendRealListenerPacket(uint8_t header, const void* data, uint32_t size) const
{
	if (!realListener_)
	{
		return;
	}

	net_offline_shop::GG_packet packet =
	{
		HEADER_GG_OFFLINE_SHOP,
		header,
		size,
		GetId()
	};

	TEMP_BUFFER buf(sizeof(packet) + size);
	buf.write(&packet, sizeof(packet));
	if (size > 0)
	{
		buf.write(data, size);
	}

	realListener_->Packet(buf.read_peek(), buf.size());
}

void COfflineShop::SendFakeListenerPacket(uint8_t header, const void* data, uint32_t size) const
{
	if (!fakeListener_)
	{
		return;
	}

	net_offline_shop::GG_packet packet =
	{
		HEADER_GG_OFFLINE_SHOP,
		header,
		size,
		GetId()
	};

	TEMP_BUFFER buf(sizeof(packet) + size);
	buf.write(&packet, sizeof(packet));
	if (size > 0)
	{
		buf.write(data, size);
	}

	fakeListener_->Packet(buf.read_peek(), buf.size());
}

uint32_t COfflineShop::GetId() const
{
	return id_;
}

uint32_t COfflineShop::GetOwnerPid() const
{
	return ownerPid_;
}

const std::string& COfflineShop::GetOwnerName() const
{
	return ownerName_;
}

void COfflineShop::SetName(const std::string& shopName, int32_t nameChangeTime)
{
	if (IsClosed())
	{
		return;
	}

	shopName_ = shopName;

	if (shopName_.size() > SHOP_TAB_NAME_MAX)
	{
		shopName_.erase(SHOP_TAB_NAME_MAX, std::string::npos);
	}

	shopNameChangeTime_ = nameChangeTime;

	Save();

	if (LPCHARACTER owner = CHARACTER_MANAGER::Instance().FindByPID(GetOwnerPid()))
	{
		net_offline_shop::GC_name_packet packet;

		std::copy(GetName().begin(), GetName().end(), packet.shopName.begin());
		packet.shopName[GetName().size()] = '\0'; // Manually null-terminate

		SendViewerPacket(net_offline_shop::HEADER_GC_NAME, &packet, sizeof(packet), owner);
		owner->ChatPacket(CHAT_TYPE_COMMAND, "OfflineShop %u \"%s\"", GetId(), GetName().c_str());
	}

	if (GetKeeper())
	{
		uint32_t size = sizeof(net_offline_shop::GC_packet) + sizeof(net_offline_shop::GC_spawn_packet);

		TEMP_BUFFER buffer(size);

		net_offline_shop::GC_packet headerPacket =
		{
			HEADER_GC_OFFLINE_SHOP,
			static_cast<uint16_t>(size),
			net_offline_shop::HEADER_GC_SPAWN,
			GetId()
		};
		buffer.write(&headerPacket, sizeof(headerPacket));

		net_offline_shop::GC_spawn_packet packet;
		packet.vid = GetVID();
		packet.decoBoard = GetDecoBoard();

		std::copy(GetName().begin(), GetName().end(), packet.shopName.begin());
		packet.shopName[GetName().size()] = '\0'; // Manually null-terminate

		buffer.write(&packet, sizeof(packet));

		GetKeeper()->PacketView(buffer.read_peek(), buffer.size());
	}

	if (IsReal())
	{
		net_offline_shop::GG_name_packet packet;
		copy_stoca(GetName(), packet.shopName);
		packet.name_change_time = shopNameChangeTime_;

		SendFakeListenerPacket(net_offline_shop::HEADER_GG_NAME, &packet, sizeof(packet));
	}
}

const std::string& COfflineShop::GetName() const
{
	return shopName_;
}

int32_t COfflineShop::GetNameChangeTime() const
{
	return shopNameChangeTime_;
}

uint8_t COfflineShop::GetChannel() const
{
	return channel_;
}

uint32_t COfflineShop::GetMapIndex() const
{
	return mapIndex_;
}

uint32_t COfflineShop::GetX() const
{
	return x_;
}

uint32_t COfflineShop::GetY() const
{
	return y_;
}

uint32_t COfflineShop::GetDecoRace() const
{
	return decoRace_;
}

uint8_t COfflineShop::GetDecoBoard() const
{
	return decoBoard_;
}

const COfflineShop::ItemSharedPtr& COfflineShop::GetItem(uint8_t shopPosition) const
{
	auto it = items_.find(shopPosition);
	if (it == items_.end())
	{
		return nullptr;
	}

	return it->second;
}

const std::map<uint8_t, COfflineShop::ItemSharedPtr>& COfflineShop::GetItems() const
{
	return items_;
}

uint32_t COfflineShop::GetItemCount() const
{
	return items_.size();
}

void COfflineShop::SetOpeningTime(uint32_t openingTime)
{
	openingTime_ = openingTime;

#ifdef __SHOP_SEARCH__
	if (IsReal())
	{
		if (IsClosed())
		{
			for (auto& item : GetItems())
			{
				CShopSearchManager::UnregisterShopItem(GetOwnerPid(), *item.second);
			}
		}
		else
		{
			for (auto& item : GetItems())
			{
				CShopSearchManager::RegisterShopItem(GetOwnerPid(), *item.second, GetShopSearchTime());
			}
		}
	}
#endif

	for (const auto& viewer : viewers_)
	{
		if (viewer->GetPlayerID() == GetOwnerPid())
		{
			net_offline_shop::GC_opening_time_packet packet = { GetOpeningTime() };
			SendViewerPacket(net_offline_shop::HEADER_GC_OPENING_TIME, &packet, sizeof(packet), viewer);
		}
	}

	Save();
}

uint32_t COfflineShop::GetOpeningTime() const
{
	if (openingTimeRunning_ || openingTime_ == 0)
	{
		return openingTime_;
	}
	else
	{
		return static_cast<uint32_t>(get_global_time()) + openingTime_;
	}
}

#ifdef __SHOP_SEARCH__
uint32_t COfflineShop::GetShopSearchTime() const
{
	if (!openingTimeRunning_)
	{
		return 0;
	}

	return openingTime_;
}
#endif

void COfflineShop::StopOpeningTime()
{
	if (!openingTimeRunning_)
	{
		return;
	}

	auto time = static_cast<uint32_t>(get_global_time());
	if (time >= openingTime_)
	{
		return;
	}

	openingTimeRunning_ = false;
	if (openingTime_ > 0)
	{
		SetOpeningTime(openingTime_ - time);
	}
}

void COfflineShop::ContinueOpeningTime()
{
	if (openingTimeRunning_)
	{
		return;
	}

	openingTimeRunning_ = true;
	if (openingTime_ > 0)
	{
		SetOpeningTime(openingTime_ + static_cast<uint32_t>(get_global_time()));
	}
}

void COfflineShop::SetGold(int64_t gold)
{
	gold_ = gold;

	if (GetItemCount() == 0 && GetGold() == 0)
	{
		Close();
	}
	else
	{
		for (const auto& viewer : viewers_)
		{
			if (viewer->GetPlayerID() == GetOwnerPid())
			{
				net_offline_shop::GC_gold_packet packet = { GetGold() };
				SendViewerPacket(net_offline_shop::HEADER_GC_GOLD, &packet, sizeof(packet), viewer);
			}
		}



		net_offline_shop::GG_gold_packet packet = { gold };
		SendRealListenerPacket(net_offline_shop::HEADER_GG_GOLD, &packet, sizeof(packet));

		Save();
	}
}

int64_t COfflineShop::GetGold() const
{
	return gold_;
}

void COfflineShop::SetKeeper(CHARACTER* keeper)
{
	keeper_ = keeper;
}

CHARACTER* COfflineShop::GetKeeper() const
{
	return keeper_;
}

void COfflineShop::SetSkipSave(bool skipSave)
{
	skipSave_ = skipSave;
}

bool COfflineShop::IsSkipSave() const
{
	return skipSave_;
}

bool COfflineShop::IsReal() const
{
	return isReal_;
}

bool COfflineShop::HasFakeListener() const
{
	return fakeListener_ != nullptr;
}

void COfflineShop::SetFakeListener(DESC* listener)
{
	fakeListener_ = listener;

	if (!fakeListener_)
	{
		return;
	}

	// Send data
	TEMP_BUFFER buf(sizeof(TOfflineShop) + sizeof(uint32_t) +
					sizeof(TOfflineShopItem) * GetItemCount());

	TOfflineShop data;
	data.id = GetId();
	data.ownerPid = GetOwnerPid();
	data.channel = g_bChannel;
	data.mapIndex = GetMapIndex();
	data.x = GetX();
	data.y = GetY();
	data.decoRace = GetDecoRace();
	data.decoBoard = GetDecoBoard();
	data.openingTime = GetOpeningTime();
	data.gold = GetGold();
	data.shopNameChangeTime = GetNameChangeTime();

	copy_stoca(GetOwnerName(), data.ownerName);
	copy_stoca(GetName(), data.shopName);

	buf.write(&data, sizeof(data));

	uint32_t itemCount = GetItemCount();
	buf.write(&itemCount, sizeof(itemCount));

	for (const auto& entry : GetItems())
	{
		buf.write(entry.second.get(), sizeof(TOfflineShopItem));
	}

	SendFakeListenerPacket(net_offline_shop::HEADER_GG_SHOP, buf.read_peek(), buf.size());
}

void COfflineShop::FakeRequestAddItem(uint8_t shopPosition, CItem* item, int64_t price)
{
	if (IsReal() || IsClosed() || !item || !CanAddItem(item))
	{
		return;
	}

	// Ignore equipped items
	if (item->IsEquipped())
	{
		return;
	}

	if (!itemGrid_.IsEmpty(shopPosition, 1, item->GetSize()))
	{
		return;
	}

	net_offline_shop::GG_add_item_packet packet;
	packet.vnum = item->GetVnum();
	packet.count = item->GetCount();
#ifdef __TRANSMUTATION_SYSTEM__
	packet.transmutation = item->GetTransmutate();
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	packet.refineElement = item->GetRefineElement();
#endif

	item->CopySockets(packet.sockets);
	item->CopyAttributes(packet.attributes);
	packet.shopPosition = shopPosition;
	packet.price = price;

	ITEM_MANAGER::Instance().RemoveItem(item);

	SendRealListenerPacket(net_offline_shop::HEADER_GG_ADD_ITEM, &packet, sizeof(packet));
}

void COfflineShop::FakeRequestMoveItem(uint8_t oldShopPosition, uint8_t newShopPosition)
{
	if (IsReal() || IsClosed() || !HasItem(oldShopPosition))
	{
		return;
	}

	const auto& item = GetItem(oldShopPosition);
	if (!item) return;

	const auto itemTable = GET_ITEM_TABLE(item->vnum);
	if (!itemTable)
	{
		sys_err("Invalid item %u at position %u.", item->vnum, oldShopPosition);
		return;
	}

	if (!itemGrid_.IsEmpty(newShopPosition, 1, itemTable->bSize))
	{
		return;
	}

	net_offline_shop::GG_move_item_packet packet = { oldShopPosition, newShopPosition };
	SendRealListenerPacket(net_offline_shop::HEADER_GG_MOVE_ITEM, &packet, sizeof(packet));
}

void COfflineShop::FakeRequestRemoveItem(uint8_t shopPosition)
{
	if (IsReal() || !HasItem(shopPosition))
	{
		return;
	}

	net_offline_shop::GG_remove_item_packet packet = { shopPosition };
	SendRealListenerPacket(net_offline_shop::HEADER_GG_REMOVE_ITEM, &packet, sizeof(packet));
}

void COfflineShop::FakeRequestClose()
{
	if (IsReal())
	{
		return;
	}

	SendRealListenerPacket(net_offline_shop::HEADER_GG_CLOSE);
}

void COfflineShop::FakeRequestWithdrawGold(int64_t gold)
{
	if (IsReal() || gold < 0 || gold > GetGold())
	{
		return;
	}

	net_offline_shop::GG_gold_packet packet = { gold };
	SendRealListenerPacket(net_offline_shop::HEADER_GG_GOLD, &packet, sizeof(packet));
}

void COfflineShop::FakeRequestChangeName(const std::string& name, int32_t nameChangeTime)
{
	if (IsReal() || !ValidateName(name))
	{
		return;
	}

	net_offline_shop::GG_name_packet packet;
	copy_stoca(name, packet.shopName);
	packet.name_change_time = nameChangeTime;

	SendRealListenerPacket(net_offline_shop::HEADER_GG_NAME, &packet, sizeof(packet));
}

void COfflineShop::Save() const
{
	if (!IsReal() || IsSkipSave())
	{
		return;
	}

	TOfflineShop data;
	data.id = GetId();
	data.ownerPid = GetOwnerPid();
	data.channel = g_bChannel;
	data.mapIndex = GetMapIndex();
	data.x = GetX();
	data.y = GetY();
	data.decoRace = GetDecoRace();
	data.decoBoard = GetDecoBoard();
	data.openingTime = GetOpeningTime();
	data.gold = GetGold();
	data.shopNameChangeTime = GetNameChangeTime();

	copy_stoca(GetOwnerName(), data.ownerName);

	memset(data.shopName.data(), '\0', data.shopName.size());
	std::copy(GetName().begin(), GetName().end(), data.shopName.begin());
	data.shopName[GetName().size()] = '\0'; // Manually null-terminate

	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_SHOP_SAVE, 0, &data, sizeof(data));

	sys_log(0, "SAVE OFFLINE SHOP %u", GetId());
}

void COfflineShop::SaveItem(uint8_t shopPosition) const
{
	if (!IsReal() || IsSkipSave())
	{
		return;
	}

	if (!HasItem(shopPosition))
	{
		TOfflineShopItem data;
		data.id = { GetId(), shopPosition };
		data.vnum = 0;

		db_clientdesc->DBPacket(HEADER_GD_OFFLINE_SHOP_SAVE_ITEM, 0, &data, sizeof(data));

		return;
	}

	const auto& itemPtr = GetItem(shopPosition);

	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_SHOP_SAVE_ITEM, 0, itemPtr.get(), sizeof(TOfflineShopItem));

	sys_log(0, "SAVE OFFLINE SHOP ITEM %u %u", GetId(), shopPosition);
}