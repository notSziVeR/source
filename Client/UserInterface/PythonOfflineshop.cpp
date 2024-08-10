#include "StdAfx.h"
#include "PythonOfflineShop.h"
#include "PythonNetworkStream.h"
#include "PythonCharacterManager.h"

// Static instantiation
std::map<uint32_t, std::unique_ptr<CPythonOfflineShop>> CPythonOfflineShop::offlineShops_;
PyObject* CPythonOfflineShop::managerHandler_;

CPythonOfflineShop* CPythonOfflineShop::New(uint32_t id, bool isMyShop, bool isClosed)
{
	auto shop = Get(id);
	if (shop)
	{
		return shop;
	}

	shop = new CPythonOfflineShop(id, isMyShop, isClosed);
	offlineShops_.insert({ id, std::unique_ptr<CPythonOfflineShop>(shop) });

	shop->Open();

	return shop;
}

void CPythonOfflineShop::Destroy(CPythonOfflineShop* shop)
{
	if (!shop)
	{
		return;
	}

	auto it = offlineShops_.find(shop->GetId());
	if (it == offlineShops_.end())
	{
		delete shop;
		return;
	}

	offlineShops_.erase(it);
}

CPythonOfflineShop* CPythonOfflineShop::Get(uint32_t id)
{
	auto it = offlineShops_.find(id);
	if (it == offlineShops_.end())
	{
		return nullptr;
	}

	return it->second.get();
}

bool CPythonOfflineShop::ReceivePacket()
{
	net_offline_shop::GC_packet headerPacket;
	if (!CPythonNetworkStream::Instance().Recv(sizeof(headerPacket), &headerPacket))
	{
		return false;
	}

	CPythonOfflineShop* shop = Get(headerPacket.id);

	switch (headerPacket.subHeader)
	{
	case net_offline_shop::HEADER_GC_SPAWN:
	{
		net_offline_shop::GC_spawn_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		PyCallClassMemberFunc(GetManagerHandler(), "OnSpawnShop",
							  Py_BuildValue("(iiis)", headerPacket.id, packet.vid, packet.decoBoard, packet.shopName.data()));

		break;
	}
	case net_offline_shop::HEADER_GC_OPEN_SHOP:
	{
		if (shop)
		{
			CPythonOfflineShop::Destroy(shop);
		}

		net_offline_shop::GC_open_shop_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		std::string shopName(packet.shopName.begin(), packet.shopName.end());

		shop = CPythonOfflineShop::New(headerPacket.id, packet.isMyShop, packet.isClosed);
		shop->SetName(shopName);

		while (packet.itemCount > 0)
		{
			net_offline_shop::GC_add_item_packet item;
			if (!CPythonNetworkStream::Instance().Recv(sizeof(item), &item))
			{
				return false;
			}

			shop->AddItem(item.shopPosition, item.price, item.vnum, item.count,
#ifdef TRANSMUTATION_SYSTEM
					item.transmutation,
#endif

#ifdef ENABLE_REFINE_ELEMENT
					item.refineElement,
#endif
					item.sockets, item.attributes);

			packet.itemCount--;
		}

		break;
	}
	case net_offline_shop::HEADER_GC_CLOSE_SHOP:
	{
		if (shop)
		{
			CPythonOfflineShop::Destroy(shop);
		}

		break;
	}
	case net_offline_shop::HEADER_GC_NAME:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_name_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		std::string shopName(packet.shopName.begin(), packet.shopName.end());
		shop->SetName(shopName);

		break;
	}
	case net_offline_shop::HEADER_GC_ADD_ITEM:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_add_item_packet item;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(item), &item))
		{
			return false;
		}

		shop->AddItem(item.shopPosition, item.price, item.vnum, item.count,
#ifdef TRANSMUTATION_SYSTEM
			item.transmutation,
#endif

#ifdef ENABLE_REFINE_ELEMENT
			item.refineElement,
#endif
			item.sockets, item.attributes);

		break;
	}
	case net_offline_shop::HEADER_GC_MOVE_ITEM:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_move_item_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		shop->MoveItem(packet.oldShopPosition, packet.newShopPosition);

		break;
	}
	case net_offline_shop::HEADER_GC_REMOVE_ITEM:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_remove_item_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		shop->RemoveItem(packet.shopPosition);

		break;
	}
	case net_offline_shop::HEADER_GC_OPENING_TIME:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_opening_time_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		shop->SetOpeningTime(packet.openingTime);

		break;
	}
	case net_offline_shop::HEADER_GC_GOLD:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_gold_packet packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		shop->SetGold(packet.gold);

		break;
	}
	case net_offline_shop::HEADER_GC_POSITION:
	{
		if (!shop)
		{
			return false;
		}

		net_offline_shop::GC_offlineshop_position packet;
		if (!CPythonNetworkStream::Instance().Recv(sizeof(packet), &packet))
		{
			return false;
		}

		shop->SetPosition(packet.channel, packet.local_x, packet.local_y, packet.global_x, packet.global_y);

		break;
	}
	}

	return true;
}

bool CPythonOfflineShop::SendPacket(uint8_t header, uint32_t id, const void* data, uint32_t size)
{
	net_offline_shop::CG_packet packet =
	{
		HEADER_CG_OFFLINE_SHOP,
		header,
		id
	};

	if (!CPythonNetworkStream::Instance().Send(sizeof(packet), &packet))
	{
		return false;
	}

	if (data && size > 0)
	{
		if (!CPythonNetworkStream::Instance().Send(size, data))
		{
			return false;
		}
	}

	CPythonNetworkStream::Instance().SendSequence();
	return true;
}

void CPythonOfflineShop::SetManagerHandler(PyObject* handler)
{
	managerHandler_ = handler;
}

PyObject* CPythonOfflineShop::GetManagerHandler()
{
	return managerHandler_;
}

void CPythonOfflineShop::SendCreatePacket(std::string shopName,
		uint32_t x, uint32_t y, uint32_t decoRace, uint8_t decoBoard,
		const std::vector<net_offline_shop::CG_item_info>& items)
{
	if (items.size() < 1)
	{
		return;
	}

	net_offline_shop::CG_create_packet dataPacket =
	{
		"",
		x, y,
		decoRace, decoBoard,
		items.size()
	};

	std::copy(shopName.begin(), shopName.end(), dataPacket.shopName.begin());

	net_offline_shop::CG_packet headerPacket =
	{
		HEADER_CG_OFFLINE_SHOP,
		net_offline_shop::HEADER_CG_CREATE,
		0
	};

	if (!CPythonNetworkStream::Instance().Send(sizeof(headerPacket), &headerPacket))
	{
		return;
	}

	if (!CPythonNetworkStream::Instance().Send(sizeof(dataPacket), &dataPacket))
	{
		return;
	}

	CPythonNetworkStream::Instance().Send(sizeof(net_offline_shop::CG_item_info) * items.size(), items.data());
	CPythonNetworkStream::Instance().SendSequence();
}

void CPythonOfflineShop::SendAddViewerPacket(uint32_t id)
{
	SendPacket(net_offline_shop::HEADER_CG_ADD_VIEWER, id);
}

CPythonOfflineShop::CPythonOfflineShop(uint32_t id, bool isMyShop, bool isClosed)
	: id_(id), isMyShop_(isMyShop), shopHandler_(nullptr), isClosed_(isClosed)
{
}

CPythonOfflineShop::~CPythonOfflineShop()
{
	PyCallClassMemberFunc(GetManagerHandler(), "OnCloseShop",
						  Py_BuildValue("(i)", GetId()));
}

uint32_t CPythonOfflineShop::GetId() const
{
	return id_;
}

bool CPythonOfflineShop::IsMyShop() const
{
	return isMyShop_;
}

bool CPythonOfflineShop::IsClosed() const
{
	return isClosed_;
}

void CPythonOfflineShop::RegisterHandler(PyObject* handler)
{
	shopHandler_ = handler;
}

PyObject* CPythonOfflineShop::GetHandler() const
{
	return shopHandler_;
}

void CPythonOfflineShop::SendPacket(uint8_t header, const void* data, uint32_t size) const
{
	SendPacket(header, GetId(), data, size);
}

void CPythonOfflineShop::SendReopenPacket() const
{
	if (!IsMyShop())
	{
		return;
	}

	SendPacket(net_offline_shop::HEADER_CG_REOPEN);
}

void CPythonOfflineShop::SendClosePacket() const
{
	if (!IsMyShop())
	{
		return;
	}

	SendPacket(net_offline_shop::HEADER_CG_CLOSE);
}

void CPythonOfflineShop::SendRemoveViewerPacket() const
{
	if (!IsMyShop())
	{
		return;
	}

	SendPacket(net_offline_shop::HEADER_CG_REMOVE_VIEWER);
}

void CPythonOfflineShop::SendBuyItemPacket(uint8_t shopPosition) const
{
	net_offline_shop::CG_buy_item_packet packet = { shopPosition };
	SendPacket(net_offline_shop::HEADER_CG_BUY_ITEM, &packet, sizeof(packet));
}

void CPythonOfflineShop::SendChangeNamePacket(const std::string& shopName) const
{
	if (!IsMyShop())
	{
		return;
	}

	net_offline_shop::CG_change_name_packet packet;
	std::copy(shopName.begin(), shopName.end(), packet.shopName.data());
	packet.shopName[shopName.size()] = '\0'; // Manually null-terminate

	SendPacket(net_offline_shop::HEADER_CG_CHANGE_NAME, &packet, sizeof(packet));
}

void CPythonOfflineShop::SendAddItemPacket(uint8_t shopPosition, const TItemPos& inventoryPosition, int64_t price) const
{
	if (!IsMyShop())
	{
		return;
	}

	net_offline_shop::CG_add_item_packet packet = { { shopPosition, inventoryPosition, price } };
	SendPacket(net_offline_shop::HEADER_CG_ADD_ITEM, &packet, sizeof(packet));
}

void CPythonOfflineShop::SendMoveItemPacket(uint8_t oldShopPosition, uint8_t newShopPosition) const
{
	if (!IsMyShop())
	{
		return;
	}

	net_offline_shop::CG_move_item_packet packet = { oldShopPosition, newShopPosition };
	SendPacket(net_offline_shop::HEADER_CG_MOVE_ITEM, &packet, sizeof(packet));
}

void CPythonOfflineShop::SendRemoveItemPacket(uint8_t shopPosition, const TItemPos& inventoryPosition) const
{
	if (!IsMyShop())
	{
		return;
	}

	net_offline_shop::CG_remove_item_packet packet = { shopPosition, inventoryPosition };
	SendPacket(net_offline_shop::HEADER_CG_REMOVE_ITEM, &packet, sizeof(packet));
}

void CPythonOfflineShop::SendWithdrawPacket(int64_t gold) const
{
	if (!IsMyShop())
	{
		return;
	}

	net_offline_shop::CG_withdraw_packet packet = { gold };
	SendPacket(net_offline_shop::HEADER_CG_WITHDRAW, &packet, sizeof(packet));
}

void CPythonOfflineShop::SetName(const std::string& shopName)
{
	Handler("SetShopName", Py_BuildValue("(s)", shopName.c_str()));
}

void CPythonOfflineShop::AddItem(uint8_t shopPosition, int64_t price, uint32_t vnum, uint32_t count,
#ifdef TRANSMUTATION_SYSTEM
								uint32_t transmutation,
#endif

#ifdef ENABLE_REFINE_ELEMENT
								uint32_t refineElement,
#endif
								 const std::array<int32_t, ITEM_SOCKET_SLOT_MAX_NUM>& sockets,
								 const std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_SLOT_MAX_NUM>& attributes)
{
	Handler("AddItem",
			Py_BuildValue(
				"(iLii"
#ifdef TRANSMUTATION_SYSTEM
				"i"
#endif
#ifdef ENABLE_REFINE_ELEMENT
				"i"
#endif
				"(iiiiii)"
				"((ii)(ii)(ii)(ii)(ii)(ii)(ii)))",
				shopPosition, price, vnum, count,
#ifdef TRANSMUTATION_SYSTEM
				transmutation,
#endif

#ifdef ENABLE_REFINE_ELEMENT
				refineElement,
#endif
				sockets[0], sockets[1], sockets[2],
				sockets[3], sockets[4], sockets[5],
				attributes[0].bType, attributes[0].sValue,
				attributes[1].bType, attributes[1].sValue,
				attributes[2].bType, attributes[2].sValue,
				attributes[3].bType, attributes[3].sValue,
				attributes[4].bType, attributes[4].sValue,
				attributes[5].bType, attributes[5].sValue,
				attributes[6].bType, attributes[6].sValue
			)
		   );
}

void CPythonOfflineShop::MoveItem(uint8_t oldShopPosition, uint8_t newShopPosition)
{
	Handler("MoveItem", Py_BuildValue("(ii)", oldShopPosition, newShopPosition));
}

void CPythonOfflineShop::RemoveItem(uint8_t shopPosition)
{
	Handler("RemoveItem", Py_BuildValue("(i)", shopPosition));
}

void CPythonOfflineShop::SetOpeningTime(uint32_t openingTime)
{
	Handler("SetOpeningTime", Py_BuildValue("(i)", openingTime));
}

void CPythonOfflineShop::SetPosition(uint8_t channel, uint32_t local_x, uint32_t local_y, uint32_t global_x, uint32_t global_y)
{
	Handler("SetShopPosition", Py_BuildValue("(iiiii)", channel, local_x, local_y, global_x, global_y));

}

void CPythonOfflineShop::SetGold(int64_t gold)
{
	Handler("SetGold", Py_BuildValue("(L)", gold));
}

void CPythonOfflineShop::Open() const
{
	PyCallClassMemberFunc(GetManagerHandler(), "OnOpenShop",
						  Py_BuildValue("(iii)", GetId(), IsMyShop() ? 1 : 0, IsClosed() ? 1 : 0));
}

extern void Traceback();
void CPythonOfflineShop::Handler(std::string functionName, PyObject* args)
{
	if (!PyCallClassMemberFunc(GetHandler(), functionName.c_str(), args))
	{
		TraceError("<DEBUG> Failed to call %s.", functionName.c_str());
	}
}
