#include "stdafx.h"
#ifdef __GAYA_SHOP_SYSTEM__
#include "../../common/length.h"
#include "constants.h"
#include "char.h"
#include "db.h"
#include "item.h"
#include "char_manager.h"
#include "utils.h"
#include "config.h"
#include "desc.h"
#include "p2p.h"
#include "GayaSystemManager.hpp"

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

EVENTINFO(gaya_empty_info)
{
};

EVENTFUNC(gaya_rotation_event)
{
	CGayaSystemManager::instance().CycleRotation();
	return PASSES_PER_SEC(1);
}

CGayaSystemManager::CGayaSystemManager()
{
	m_gaya_guests.clear();
	m_gaya_items.clear();
	LoadGayaItems();

	gaya_empty_info* info = AllocEventInfo<gaya_empty_info>();
	m_pkGayaRotationEvent = event_create(gaya_rotation_event, info, PASSES_PER_SEC(1));

	// Requesting broadcast from all peers in case of core down
	RequestGayaRecords();
}

CGayaSystemManager::~CGayaSystemManager()
{
}

void CGayaSystemManager::Destroy()
{
	event_cancel(&m_pkGayaRotationEvent);
	m_gaya_guests.clear();
	m_gaya_items.clear();
}

void CGayaSystemManager::LoadGayaItems()
{
	static const int qMaxNum = 4;
	MYSQL_ROW row;
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT slot_num, vnum, count, cost FROM player%s.gaya_shop", get_table_postfix()));
	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	if (!pRes || !msg->Get())
	{
		sys_err("Error during gaya's query.");
		return;
	}

	if (msg->Get()->uiNumRows == 0)
	{
		return;
	}

	auto CheckRows = [](const MYSQL_ROW & row, const int& iCount)->bool
	{
		for (int i = 0; i < iCount; ++i)
		{
			if (!(row[i] && *row[i]))
			{
				return false;
			}
		}

		return true;
	};

	auto VerifyGayaItem = [](const MYSQL_ROW & row, const int& iCount)->bool
	{
		for (int i = 1; i < iCount; ++i)
		{
			if (atoll(row[i]) <= 0)
			{
				return false;
			}
		}

		return true;
	};

	while ((row = mysql_fetch_row(pRes)))
	{
		if (CheckRows(row, qMaxNum))
		{
			try
			{
				m_gaya_items.at(atoi(row[0]));
			}
			catch (...)
			{
				m_gaya_items.emplace(std::piecewise_construct, std::forward_as_tuple(atoi(row[0])), std::make_tuple());
			}

			if (VerifyGayaItem(row, qMaxNum))
			{
				m_gaya_items[atoi(row[0])].emplace_back(atoi(row[0]), atoi(row[1]), atoi(row[2]), atoll(row[3]));
				sys_log(0, "Gaya item has been loaded: Slot_Num: %d, Vnum: %d, Count: %d, Cost: %lld", atoi(row[0]), atoi(row[1]), atoi(row[2]), atoi(row[3]));
			}
			else
			{
				sys_err("Not valid gaya record! Slot_Num: %d, Vnum: %d, Count: %d, Cost: %lld", atoi(row[0]), atoi(row[1]), atoi(row[2]), atoi(row[3]));
			}
		}
	}
}

void CGayaSystemManager::AddGayaGuest(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}

	auto cIt = FindGayaGuest(ch);
	if (cIt != m_gaya_guests.end())
	{
		SendGayaItemList(ch);
		OpenGayaInterface(ch);
		return;
	}

	// Checking slots status (default GAYA_DEFAULT_UNLOCKED_COUNT)
	for (int i = 0; i < GAYA_DEFAULT_UNLOCKED_COUNT; ++i)
	{
		if (!GetGayaSlotStatus(ch, i))
		{
			UpdateGayaSlotStatus(ch, i, 1);
		}
	}

	cIt = RegisterGayaAttender(ch);
	RollGayaItems(ch);
	OpenGayaInterface(ch);

	// Broadcasting new record
	BroadcastGayaRecord(cIt);
}

void CGayaSystemManager::RemoveGayaGuest(LPCHARACTER ch)
{
	/*
		Erased
	*/
}

int CGayaSystemManager::CraftItem(LPCHARACTER ch, const WORD& iSlotID)
{
#ifndef __SPECIAL_STORAGE_ENABLE__
	if (iSlotID >= INVENTORY_MAX_NUM)
	{
		return -1;
	}
#else
	if ((iSlotID >= INVENTORY_MAX_NUM && iSlotID < SPECIAL_STORAGE_START_CELL) || iSlotID >= INVENTORY_AND_EQUIP_SLOT_MAX)
	{
		return -1;
	}
#endif

	if (ch->GetGold() < GAYA_CRAFTING_COST)
	{
		return -2;
	}

	LPITEM item = ch->GetInventoryItem(iSlotID);
	if (!item)
	{
		return -1;
	}

	if (std::find(GAYA_STONES_LIST.begin(), GAYA_STONES_LIST.end(), item->GetVnum()) == GAYA_STONES_LIST.end() || ch->CountSpecifyItem(GAYA_CRAFTING_ITEM[0]) < GAYA_CRAFTING_ITEM[1])
	{
		return -3;
	}

	if (item->GetCount() < 100)
	{
		return -4;
	}

	while (item->GetCount() >= 100 && ch->CountSpecifyItem(GAYA_CRAFTING_ITEM[0]) >= GAYA_CRAFTING_ITEM[1])
	{
		item->SetCount(item->GetCount() - 100);
		ch->RemoveSpecifyItem(GAYA_CRAFTING_ITEM[0], GAYA_CRAFTING_ITEM[1]);
		ch->PointChange(POINT_GOLD, -GAYA_CRAFTING_COST);

#ifdef __BATTLE_PASS_ENABLE__
		CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(GAYA_CRAFTING_COST) });
#endif

		if (GAYA_CRAFTING_CHANCE >= number(1, 100))
		{
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_GET_GAYA, 0, 1 });
#endif
			ch->GiveGayaPoints(1);
		}
		else
		{
			return -5;
			break;
		}
	}

	return 0;
}

int CGayaSystemManager::UnlockGayaSlot(LPCHARACTER ch, const WORD& iSlotID, const WORD& iUnlockItemID)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return -1;
	}

	if (GetGayaSlotStatus(ch, iSlotID))
	{
		return -2;
	}

	LPITEM item;
	if (!(item = ch->GetInventoryItem(iUnlockItemID)))
	{
		return -3;
	}

	if (item->GetVnum() != GAYA_UNLOCK_ITEM)
	{
		return -3;
	}

	// if (ch->CountSpecifyItem(GAYA_UNLOCK_ITEM) < 1)
	// return -3;

	item->SetCount(item->GetCount() - 1);
	UpdateGayaSlotStatus(ch, iSlotID, 1);

	SendGayaItemList(ch);
	return 0;
}

int CGayaSystemManager::PurchaseItem(LPCHARACTER ch, const BYTE& iSlotID)
{
	if (iSlotID >= GAYA_SLOT_SIZE)
	{
		return -1;
	}

	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return -1;
	}

	if (!GetGayaSlotStatus(ch, iSlotID))
	{
		return -1;
	}

	const tGayaItem& purItem = std::get<0>(cIt->second)[iSlotID];
	if (!std::get<1>(purItem))
	{
		return -2;
	}

	long long llCost = std::get<3>(purItem) * std::get<2>(purItem); // count * cost
	if (llCost > ch->GetGayaPoints())
	{
		return -3;
	}

	ch->ChargeGayaPoints(llCost);
	ch->AutoGiveItem(std::get<1>(purItem), std::get<2>(purItem));
	return 0;
}

void CGayaSystemManager::UpdateGayaSlotStatus(LPCHARACTER ch, const BYTE& iSlotID, const bool& bStatus)
{
	ch->SetQuestFlag((std::string("gaya_system.slot_enable_") + std::to_string(iSlotID)), bStatus);
}

tGayaGuestIterator CGayaSystemManager::RegisterGayaAttender(LPCHARACTER ch)
{
	auto it = m_gaya_guests.emplace(std::piecewise_construct, std::make_tuple(ch->GetPlayerID()), std::make_tuple()).first;
	std::get<0>(it->second).resize(GAYA_SLOT_SIZE);
	std::get<1>(it->second) = get_global_time() + NEXT_ROTATION_TIME;
	return it;
}

void CGayaSystemManager::AddGayaItem(LPCHARACTER ch, tGayaItem&& gItem)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	try
	{
		std::get<0>((cIt->second)).at(std::get<0>(gItem)) = std::move(gItem);
	}
	catch (...)
	{
		std::get<0>((cIt->second)).push_back(gItem);
	}
}

void CGayaSystemManager::SendGayaItemList(LPCHARACTER ch)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	for (auto&& gItem : std::get<0>(cIt->second))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GayaAddItem %d %d %d %lld %d", std::get<0>(gItem), std::get<1>(gItem), std::get<2>(gItem), std::get<3>(gItem), GetGayaSlotStatus(ch, std::get<0>(gItem)));
	}
}

void CGayaSystemManager::RollGayaItems(LPCHARACTER ch)
{
	for (auto&& iGayaItem : GetRandomItems(ch))
	{
		AddGayaItem(ch, std::move(iGayaItem));
	}

	SendGayaItemList(ch);
}

void CGayaSystemManager::EraseGayaItems(LPCHARACTER ch)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	std::get<0>((cIt->second)).clear();
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GayaEraseDesc");
}

void CGayaSystemManager::EraseGayaItems(const DWORD& dwPID)
{
	auto cIt = FindGayaGuest(dwPID);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	std::get<0>((cIt->second)).clear();
}

void CGayaSystemManager::OpenGayaInterface(LPCHARACTER ch)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "GayaRotationTime %d", (std::get<1>(cIt->second) - get_global_time()));
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GayaOpenInterface");
}

void CGayaSystemManager::PullGayaRotation(LPCHARACTER ch)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return;
	}

	EraseGayaItems(ch);
	RollGayaItems(ch);

	// Broadcasting new record
	BroadcastGayaRecord(cIt);

	// Updating time
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GayaRotationTime %d", (std::get<1>(cIt->second) - get_global_time()));
}

int CGayaSystemManager::RequestGayaRotation(LPCHARACTER ch)
{
	auto cIt = FindGayaGuest(ch);
	if (cIt == m_gaya_guests.end())
	{
		return -1;
	}

	if (ch->CountSpecifyItem(GAYA_ROTATION_ITEM) < 1)
	{
		return -2;
	}

	ch->RemoveSpecifyItem(GAYA_ROTATION_ITEM, 1);
	std::get<1>(cIt->second) = get_global_time() + NEXT_ROTATION_TIME;
	PullGayaRotation(ch);
	return 0;
}

tGayaGuestIterator CGayaSystemManager::FindGayaGuest(LPCHARACTER ch)
{
	return m_gaya_guests.find(ch->GetPlayerID());
}

tGayaGuestIterator CGayaSystemManager::FindGayaGuest(const DWORD& dwPID)
{
	return m_gaya_guests.find(dwPID);
}

aGayaItems CGayaSystemManager::GetRandomItems(LPCHARACTER ch)
{
	aGayaItems v_items;
	for (int i = 0; i < GAYA_SLOT_SIZE; ++i)
	{
		try
		{
			auto v_gaya_cat = m_gaya_items.at(i);
			if (v_gaya_cat.size())
			{
				v_items[i] = v_gaya_cat[number(0, (v_gaya_cat.size() - 1))];
			}
			else
			{
				v_items[i] = std::make_tuple(i, 0, 0, 0);
			}
		}
		catch (...)
		{
			sys_err("Unknown gaya category! Cat num: %d", i);
			continue;
		}
	}

	return v_items;
}

bool CGayaSystemManager::GetGayaSlotStatus(LPCHARACTER ch, const BYTE& iSlotID)
{
	return (ch->GetQuestFlag((std::string("gaya_system.slot_enable_") + std::to_string(iSlotID))) > 0);
}

void CGayaSystemManager::CycleRotation()
{
	std::set<DWORD> s_garbage_c;
	for (auto&& gGuest : m_gaya_guests)
	{
		if (static_cast<size_t>(get_global_time()) >= std::get<1>(gGuest.second))
		{
			LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(gGuest.first);
			if (!ch)
			{
				EraseGayaItems(gGuest.first);
				s_garbage_c.insert(gGuest.first);
			}
			else
			{
				std::get<1>(gGuest.second) = get_global_time() + NEXT_ROTATION_TIME;
				PullGayaRotation(ch);
			}
		}
	}

	for (auto&& gInvGuest : s_garbage_c)
	{
		m_gaya_guests.erase(gInvGuest);
	}
}

// P2P
// Out
void CGayaSystemManager::BroadcastGayaRecord(tGayaGuestIterator it, LPDESC inpt_d)
{
	TPacketGGGayaInfo info;
	memset(&info, 0, sizeof(info));
	info.bHeader = HEADER_GG_GAYA_BROADCAST;
	info.bSubHeader = SUBHEADER_GG_GAYA_ADD_ITEM;
	info.dwPID = it->first;
	info.uiRotationTime = std::get<1>(it->second);

	for (auto&& gItem : std::get<0>(it->second))
	{
		info.sNum = std::get<0>(gItem);
		info.dwVnum = std::get<1>(gItem);
		info.wCount = std::get<2>(gItem);
		info.llPrice = std::get<3>(gItem);

		if (!inpt_d)
		{
			P2P_MANAGER::instance().Send(&info, sizeof(TPacketGGGayaInfo));
		}
		else
		{
			inpt_d->Packet(&info, sizeof(TPacketGGGayaInfo));
		}
	}
}

void CGayaSystemManager::EraseGayaRecord(const DWORD& dwPID)
{
	TPacketGGGayaInfo info;
	memset(&info, 0, sizeof(info));
	info.bHeader = HEADER_GG_GAYA_BROADCAST;
	info.bSubHeader = SUBHEADER_GG_GAYA_REMOVE_GUEST;
	info.dwPID = dwPID;
	P2P_MANAGER::instance().Send(&info, sizeof(TPacketGGGayaInfo));
}

void CGayaSystemManager::RequestGayaRecords()
{
	TPacketGGGayaInfo info;
	memset(&info, 0, sizeof(info));
	info.bHeader = HEADER_GG_GAYA_BROADCAST;
	info.bSubHeader = SUBHEADER_GG_GAYA_REQUEST_RECORDS;
	P2P_MANAGER::instance().Send(&info, sizeof(TPacketGGGayaInfo));
}

// In
void CGayaSystemManager::BroadcastAllGayaRecords(LPDESC inpt_d)
{
	for (auto&& gGuest : m_gaya_guests)
	{
		TPacketGGGayaInfo info;
		memset(&info, 0, sizeof(info));
		info.bHeader = HEADER_GG_GAYA_BROADCAST;
		info.bSubHeader = SUBHEADER_GG_GAYA_ADD_ITEM;
		info.dwPID = gGuest.first;
		info.uiRotationTime = std::get<1>(gGuest.second);

		for (auto&& gItem : std::get<0>(gGuest.second))
		{
			info.sNum = std::get<0>(gItem);
			info.dwVnum = std::get<1>(gItem);
			info.wCount = std::get<2>(gItem);
			info.llPrice = std::get<3>(gItem);

			inpt_d->Packet(&info, sizeof(TPacketGGGayaInfo));
		}
	}
}

void CGayaSystemManager::RegisterGayaRecord(LPDESC inpt_d, TPacketGGGayaInfo* rec)
{
	switch (rec->bSubHeader)
	{
	case SUBHEADER_GG_GAYA_ADD_ITEM:
	{
		auto it = m_gaya_guests.find(rec->dwPID);
		if (it == m_gaya_guests.end())
		{
			it = m_gaya_guests.emplace(std::piecewise_construct, std::make_tuple(rec->dwPID), std::make_tuple()).first;
			std::get<0>(it->second).resize(GAYA_SLOT_SIZE);
		}

		// Updating time
		std::get<1>(it->second) = rec->uiRotationTime;

		// Updating item
		try
		{
			std::get<0>((it->second)).at(rec->sNum) = std::make_tuple(rec->sNum, rec->dwVnum, rec->wCount, rec->llPrice);
			sys_log(0, "Record has been send! PID: %d, Num: %d, Vnum: %d", it->first, rec->sNum, rec->dwVnum);
		}
		catch (...)
		{
			sys_err("Unknown sNum: %d. Couldn't proceed record process.", rec->sNum);
		}
	}
	break;
	case SUBHEADER_GG_GAYA_REMOVE_GUEST:
	{
		auto it = m_gaya_guests.find(rec->dwPID);
		if (it != m_gaya_guests.end())
		{
			EraseGayaItems(rec->dwPID);
			m_gaya_guests.erase(it);
		}
		else
		{
			sys_err("No record for PID: %d", rec->dwPID);
		}
	}
	break;
	default:
		sys_err("Unknown subheader: %d", rec->bSubHeader);
		break;
	}
}
#endif
