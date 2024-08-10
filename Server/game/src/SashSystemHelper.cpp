#include "stdafx.h"
#ifdef __SASH_ABSORPTION_ENABLE__
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "refine.h"
#include "SashSystemHelper.hpp"

namespace SashSystemHelper
{
	TSashMapType m_sash_refine_map;
	TSashMapType m_sash_absorption_map;

	/// Effects ///
	static const std::string s_effect_combination = "d:/ymir work/effect/etc/buff/buff_item6.mse";
	static const std::string s_effect_absorption = "d:/ymir work/effect/etc/buff/buff_item7.mse";
	/// END ///

	/// Struct methods ///
	void TSashRecord::UpdateItem(LPCHARACTER ch, const BYTE & iSlotNum, const DWORD & dwID, const WORD & wCell)
	{
		LPITEM item = ITEM_MANAGER::instance().Find(a_records[iSlotNum].id);
		if (item && item->GetOwner() == ch)
			item->Lock(false);

		a_records[iSlotNum] = TSashItemRecord(dwID, wCell);
	}

	void TSashRecord::ReleaseItems(LPCHARACTER ch)
	{
		for (const auto & record : a_records)
		{
			LPITEM item = ITEM_MANAGER::instance().Find(record.id);
			if (item && item->GetOwner() == ch)
				item->Lock(false);
		}
	}
	/// End ///

	/// Local functions ///
	static bool CheckItemsIntegrity(LPCHARACTER ch, std::vector<LPITEM> & v_items, const TSashRecord::TSashArray & a_records)
	{
		for (auto const & sash : a_records)
		{
			LPITEM item;
			if (!(item = ITEM_MANAGER::instance().Find(sash.id)))
				return false;

			if (item->GetOwner() != ch || item->GetCell() != sash.cell || item->GetWindow() != INVENTORY)
				return false;

			v_items.push_back(item);
		}

		return true;
	}

	static LPITEM GetSashByID(LPCHARACTER ch, DWORD dwID)
	{
		auto pItem = ITEM_MANAGER::instance().Find(dwID);
		if (!pItem)
			return nullptr;

		return (pItem->GetOwner() != ch ? nullptr : pItem);
	}
	/// End ///

	/// Global Functions ///
	int GetSashFreeSlot(LPCHARACTER ch)
	{
		int iSlot = -1;
		auto it = m_sash_refine_map.find(ch->GetPlayerID());
		if (it == m_sash_refine_map.end())
			return iSlot;

		return (it->second).GetFreeSlot();
	}

	bool HasAbsorption(LPITEM sash)
	{
		return (sash->GetType() == ITEM_COSTUME && sash->GetSubType() == COSTUME_SASH && sash->GetSocket(SASH_ABSORPTION_SOCKET) > 0);
	}

	DWORD GetAbsorptionBase(LPITEM sash)
	{
		return sash->GetSocket(SASH_ABSORPTION_SOCKET);
	}

	float GetAbsorptionValue(LPITEM sash)
	{
		return static_cast<float>(sash->GetSocket(SASH_TYPE_SOCKET));
	}

	int GetSashType(LPITEM sash)
	{
		if (!sash)
		{
			sys_err("Unknown sash item.");
			return -1;
		}

		return sash->GetValue(0);
	}

	LPITEM CreateSash(const DWORD & sash_vnum)
	{
		LPITEM nItem = ITEM_MANAGER::instance().CreateItem(sash_vnum, 1, 0, false);
		if (!nItem)
		{
			sys_err("Critical error. Couldn't create sash item. Vnum: %d", sash_vnum);
			return nullptr;
		}

		switch (nItem->GetValue(0))
		{
			case SASH_TYPE_NORMAL:
			case SASH_TYPE_UNIQUE:
			case SASH_TYPE_RARE:
				nItem->SetSocket(SASH_TYPE_SOCKET, std::max(nItem->GetValue(0)*SASH_ABSORPTION_BASE_VALUE, (long int) 1));
				break;
			case SASH_TYPE_LEGENDARY:
				nItem->SetSocket(SASH_TYPE_SOCKET, number(SASH_ABSORPTION_LEGENDARY_MIN, SASH_ABSORPTION_LEGENDARY_MAX));
				break;
			default:
				sys_err("Unknown sash type! Vnum: %d", sash_vnum);
				break;
		}

		return nItem;
	}

	void CreateSash(LPITEM nItem)
	{
		switch (nItem->GetValue(0))
		{
			case SASH_TYPE_NORMAL:
			case SASH_TYPE_UNIQUE:
			case SASH_TYPE_RARE:
				nItem->SetSocket(SASH_TYPE_SOCKET, std::max(nItem->GetValue(0)*SASH_ABSORPTION_BASE_VALUE, (long int) 1));
				break;
			case SASH_TYPE_LEGENDARY:
				nItem->SetSocket(SASH_TYPE_SOCKET, number(SASH_ABSORPTION_LEGENDARY_MIN, SASH_ABSORPTION_LEGENDARY_MAX));
				break;
			default:
				sys_err("Unknown sash type! Vnum: %d", nItem->GetVnum());
				break;
		}
	}

	long long ComputeRefineFee(LPITEM sash)
	{
		if (!sash)
			return 0;

		switch (sash->GetValue(0))
		{
			case SASH_TYPE_NORMAL:
			case SASH_TYPE_UNIQUE:
			case SASH_TYPE_RARE:
				return std::max(sash->GetValue(0), (long int) 1)*SASH_FEE_BASE_VALUE;
				break;
			case SASH_TYPE_LEGENDARY:
				return SASH_FEE_LEGENDARY_VALUE;
				break;
			default:
				sys_err("Unknown sash type! Vnum: %d", sash->GetVnum());
				return 0;
				break;
		}

		return 0;
	}
	
	// Releasing from all containers
	void ReleasePlayer(const DWORD & pid)
	{
		m_sash_refine_map.erase(pid);
		m_sash_absorption_map.erase(pid);
	}

	// Clearing up in case of shutdown
	void ClearUp()
	{
		m_sash_refine_map.clear();
		m_sash_absorption_map.clear();
	}

	// Opening proper interface
	void OpenSashInterface(LPCHARACTER ch, const BYTE & bIfType)
	{
		switch (bIfType)
		{
			case SASH_INTERFACE_TYPE_COMBINATION:
			{
				if (m_sash_refine_map.find(ch->GetPlayerID()) != m_sash_refine_map.end())
					return;

				if (m_sash_absorption_map.find(ch->GetPlayerID()) != m_sash_absorption_map.end())
				{
					UnregisterSashAbsorption(ch);
					Sash_Broadcast(ch, ESashBroadcast::TYPE_ABSORPTION_CLOSE);
				}

				RegisterSashRefine(ch);
			}
			break;
			case SASH_INTERFACE_TYPE_ABSORPTION:
			{
				if (m_sash_absorption_map.find(ch->GetPlayerID()) != m_sash_absorption_map.end())
					return;

				if (m_sash_refine_map.find(ch->GetPlayerID()) != m_sash_refine_map.end())
				{
					UnregisterSashRefine(ch);
					Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_CLOSE);
				}	

				RegisterSashAbsorption(ch);
			}
			break;
		}
	}

	// Closing interface
	void CloseSashInterface(LPCHARACTER ch)
	{
		UnregisterSashRefine(ch);
		UnregisterSashAbsorption(ch);

		Sash_Broadcast(ch, ESashBroadcast::TYPE_INVENTORY_REFRESH);
	}
	/// END ///

	/// Combination Functions ///
	void UnregisterSashRefine(LPCHARACTER ch)
	{
		auto r_it = m_sash_refine_map.find(ch->GetPlayerID());
		if (r_it != m_sash_refine_map.end())
		{
			(r_it->second).ReleaseItems(ch);
			m_sash_refine_map.erase(r_it);
		}
	}

	bool RegisterSashRefine(LPCHARACTER ch)
	{
		if (!ch)
			return false;

		// Checking if record exists
		if (m_sash_refine_map.find(ch->GetPlayerID()) != m_sash_refine_map.end())
			return false;

		// Registering pc as sash refiner
		m_sash_refine_map.emplace(std::piecewise_construct, std::forward_as_tuple(ch->GetPlayerID()), std::forward_as_tuple());
		Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_OPEN);
		return true;
	}

	bool RegisterSashToRefine(LPCHARACTER ch, LPITEM sash, const BYTE & iSlotNum)
	{
		if (!sash || !ch)
			return false;

		// Checking if slot belongs
		if (iSlotNum > SASH_CASE_TYPE_ITEM)
			return false;

		// Checking if record exists
		auto it = m_sash_refine_map.find(ch->GetPlayerID());
		if (it == m_sash_refine_map.end())
			return false;

		// Checking if legendary sash has maximum absorption value
		if (iSlotNum == SASH_CASE_TYPE_SASH && sash->GetValue(0) == SASH_TYPE_LEGENDARY && sash->GetSocket(SASH_TYPE_SOCKET) == SASH_ABSORPTION_VALUE_MAX)
		{
			Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_ITEM_CANNOT_UPGRADE);
			return false;
		}

		// Checking item type!
		if ((sash->GetType() != ITEM_COSTUME) && (sash->GetSubType() != COSTUME_SASH)) return false;

		// Checking type integrity if second slot exists
		const DWORD & secSlotID = (it->second).GetItem(iSlotNum == SASH_CASE_TYPE_SASH ? SASH_CASE_TYPE_ITEM : SASH_CASE_TYPE_SASH).id;
		if (secSlotID > 0)
		{
			LPITEM secSash = ITEM_MANAGER::instance().Find(secSlotID);
			if (!secSash)
				return false;

			if (sash->GetValue(0) != SASH_TYPE_LEGENDARY && !sash->GetRefinedVnum())
			{
				Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_ITEM_NOT_MATCH);
				return false;
			}

			if (GetSashType(sash) != GetSashType(secSash))
			{
				Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_ITEM_NOT_MATCH);
				return false;
			}
		}

		(it->second).UpdateItem(ch, iSlotNum, sash->GetID(), sash->GetCell());
		sash->Lock(true);

		Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_ITEM_UPDATE, sash, iSlotNum);

		auto pOriginItem = GetSashByID(ch, (it->second).GetItem(SASH_CASE_TYPE_SASH).id);
		if (pOriginItem && pOriginItem->GetRefinedVnum())
		{
			LPITEM tmpItem = ITEM_MANAGER::instance().CreateItem(pOriginItem->GetRefinedVnum(), 1, 0, false, 0, true);
			if (tmpItem)
			{
				Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_ITEM_UPDATE, tmpItem, SASH_CASE_RESULT_ITEM);
				M2_DESTROY_ITEM(tmpItem);
			}
		}
		else
			Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_ITEM_UPDATE, sash, SASH_CASE_RESULT_ITEM);

		Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_ITEM_ADDED);
		if (iSlotNum == SASH_CASE_TYPE_SASH)
			Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_COST_UPDATE, nullptr, 0, ComputeRefineFee(sash));

		return true;
	}

	bool EraseSashToRefine(LPCHARACTER ch, const BYTE & iSlotNum)
	{
		if (!ch)
			return false;

		// Checking if slot belongs
		if (iSlotNum > SASH_CASE_TYPE_ITEM)
			return false;

		// Checking if record exists
		auto it = m_sash_refine_map.find(ch->GetPlayerID());
		if (it == m_sash_refine_map.end())
			return false;

		(it->second).UpdateItem(ch, iSlotNum, 0, 0);

		Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_ITEM_UPDATE, nullptr, iSlotNum);
		Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_ITEM_REMOVED);

		const auto pItems = (it->second).GetItemList();
		auto cIf = std::count_if(pItems.begin(), pItems.end(), [](const TSashItemRecord& rItem) { return rItem.id > 0; });

		if (cIf == 0)
			Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_CLEAR);

		return true;
	}

	bool RefineSash(LPCHARACTER ch)
	{
		if (!ch)
			return false;

		// Checking of record exists
		auto r_it = m_sash_refine_map.find(ch->GetPlayerID());
		if (r_it == m_sash_refine_map.end())
			return false;

		// Checking sash validity
		std::vector<LPITEM> v_sash;
		if (!CheckItemsIntegrity(ch, v_sash, (r_it->second).GetItemList()))
			return false;

		// Checking if one can pay refine cost
		long long llrFee = ComputeRefineFee(v_sash[SASH_CASE_TYPE_SASH]);
		if (ch->GetGold() < llrFee)
		{
			Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_NOT_ENOUGH_MONEY);
			return false;
		}
		else
			ch->PointChange(POINT_GOLD, -llrFee);

		// Destroying material
		v_sash[SASH_CASE_TYPE_ITEM]->SetCount(0);

		// Pulling refine process
		ch->ChatPacket(CHAT_TYPE_INFO, fmt::format("Current chance for sash refine is {}", v_sash[SASH_CASE_TYPE_SASH]->GetValue(SASH_REFINE_CHANCE_ITEM_VALUE)));
		if (v_sash[SASH_CASE_TYPE_SASH]->GetValue(SASH_REFINE_CHANCE_ITEM_VALUE) >= number(1, 100))
		{
			// Upgrading sash (DO NOT DESTROY IT)
			if (v_sash[SASH_CASE_TYPE_SASH]->GetValue(0) == SASH_TYPE_LEGENDARY)
				v_sash[SASH_CASE_TYPE_SASH]->SetSocket(SASH_TYPE_SOCKET,
								MINMAX(v_sash[SASH_CASE_TYPE_SASH]->GetSocket(SASH_TYPE_SOCKET),
								v_sash[SASH_CASE_TYPE_SASH]->GetSocket(SASH_TYPE_SOCKET)+number(1, SASH_UPGRADE_MAX_VALUE),
								SASH_ABSORPTION_VALUE_MAX));
			else
			{
				LPITEM pkNewItem = CreateSash(v_sash[SASH_CASE_TYPE_SASH]->GetRefinedVnum());
				if (!pkNewItem)
					return false;

				v_sash[SASH_CASE_TYPE_SASH]->CopyAttributeTo(pkNewItem);
				pkNewItem->SetSocket(SASH_ABSORPTION_SOCKET, v_sash[SASH_CASE_TYPE_SASH]->GetSocket(SASH_ABSORPTION_SOCKET));

				auto bCell = v_sash[SASH_CASE_TYPE_SASH]->GetCell();
				ITEM_MANAGER::instance().RemoveItem(v_sash[SASH_CASE_TYPE_SASH], "REMOVE (SASH REFINE SUCCESS)");

				pkNewItem->AddToCharacter(ch, TItemPos(INVENTORY, bCell)); 
				ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			}

			// Sending info
			Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_PROCESS_SUCCESS);
			ch->SpecificEffectPacket(s_effect_combination.c_str());
		}
		else
			// Sending info
			Sash_ChatInfo(ch, ESashChatInfo::INFO_COMBINATION_PROCESS_FAIL);

		// Releasing iterator
		UnregisterSashRefine(ch);
		Sash_Broadcast(ch, ESashBroadcast::TYPE_COMBINATION_CLEAR);

		// Readding pc
		RegisterSashRefine(ch);
		return true;
	}
	/// END ///

	/// Absorption Functions ///
	void UnregisterSashAbsorption(LPCHARACTER ch)
	{
		auto r_it = m_sash_absorption_map.find(ch->GetPlayerID());
		if (r_it != m_sash_absorption_map.end())
		{
			(r_it->second).ReleaseItems(ch);
			m_sash_absorption_map.erase(r_it);
		}
	}

	bool RegisterSashAbsorption(LPCHARACTER ch)
	{
		if (!ch)
			return false;

		// Checking if record exists
		if (m_sash_absorption_map.find(ch->GetPlayerID()) != m_sash_absorption_map.end())
			return false;

		// Registering pc to absorption process
		m_sash_absorption_map.emplace(std::piecewise_construct, std::forward_as_tuple(ch->GetPlayerID()), std::forward_as_tuple());
		Sash_Broadcast(ch, ESashBroadcast::TYPE_ABSORPTION_OPEN);
		return true;
	}

	bool RegisterSashToAbsorption(LPCHARACTER ch, LPITEM item, const BYTE & iSlotNum)
	{
		if (!ch || !item)
			return false;

		// Checking if slot belongs
		if (iSlotNum > SASH_CASE_TYPE_ITEM)
			return false;

		// Checking if record exists
		auto it = m_sash_absorption_map.find(ch->GetPlayerID());
		if (it == m_sash_absorption_map.end())
			return false;

		// Verifying if slot fits
		switch (iSlotNum)
		{
			case SASH_CASE_TYPE_SASH:
			{
				if (HasAbsorption(item))
				{
					Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_NOT_MATCH);
					return false;
				}
			}
			break;
			case SASH_CASE_TYPE_ITEM:
			{
				if (item->GetType() == ITEM_WEAPON)
				{
					if (item->GetSubType() == WEAPON_ARROW || item->GetSubType() == WEAPON_QUIVER)
					{
						Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_NOT_MATCH);
						return false;
					}
				}
				//else if (item->GetType() == ITEM_ARMOR)
				//{
				//	if (item->GetSubType() != ARMOR_BODY)
				//	{
				//		Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_NOT_MATCH);
				//		return false;
				//	}
				//}
				else
				{
					Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_NOT_MATCH);
					return false;
				}
			}
			break;
			default:
				return false;
				break;
		}

		// Updating and locking item
		(it->second).UpdateItem(ch, iSlotNum, item->GetID(), item->GetCell());
		item->Lock(true);

		Sash_Broadcast(ch, ESashBroadcast::TYPE_ABSORPTION_ITEM_UPDATE, item, iSlotNum);
		Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_ADDED);
		return true;
	}

	bool EraseSashToAbsorption(LPCHARACTER ch, const BYTE & iSlotNum)
	{
		if (!ch)
			return false;

		// Checking if slot belongs
		if (iSlotNum > SASH_CASE_TYPE_ITEM)
			return false;

		// Checking if record exists
		auto it = m_sash_absorption_map.find(ch->GetPlayerID());
		if (it == m_sash_absorption_map.end())
			return false;

		(it->second).UpdateItem(ch, iSlotNum, 0, 0);

		Sash_Broadcast(ch, ESashBroadcast::TYPE_ABSORPTION_ITEM_UPDATE, nullptr, iSlotNum);
		Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_ITEM_REMOVED);
		return true;
	}

	bool AbsorbForSash(LPCHARACTER ch)
	{
		if (!ch)
			return false;

		// Checking of record exists
		auto r_it = m_sash_absorption_map.find(ch->GetPlayerID());
		if (r_it == m_sash_absorption_map.end())
			return false;

		// Checking items validity
		std::vector<LPITEM> v_items;
		if (!CheckItemsIntegrity(ch, v_items, (r_it->second).GetItemList()))
			return false;

		// Pulling absorption process
		if (SASH_ABSORPTION_CHANCE >= number(1, 100))
		{
			// Copying attrs
			v_items[SASH_CASE_TYPE_ITEM]->CopyAttributeTo(v_items[SASH_CASE_TYPE_SASH]);

			// Right now we getting saved the last absorption rate
			const auto vAbsorption = v_items[SASH_CASE_TYPE_SASH]->GetSocket(SASH_TYPE_SOCKET);

			//TODO Do this just when you have the STONES!
			// Copy sockets before setting the absorbed vnum
			v_items[SASH_CASE_TYPE_ITEM]->CopySocketTo(v_items[SASH_CASE_TYPE_SASH]);

			// Setting hook for absorption
			v_items[SASH_CASE_TYPE_SASH]->SetSocket(SASH_TYPE_SOCKET, std::max((long)1, vAbsorption));

			// Setting hook for applies
			v_items[SASH_CASE_TYPE_SASH]->SetSocket(SASH_ABSORPTION_SOCKET, v_items[SASH_CASE_TYPE_ITEM]->GetVnum());

			// Sending info
			Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_PROCESS_SUCCESS);
			ch->SpecificEffectPacket(s_effect_absorption.c_str());
		}
		else
			// Sending info
			Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_PROCESS_FAIL);

		// Destroying material
		v_items[SASH_CASE_TYPE_ITEM]->SetCount(0);

		// Releasing iterator
		UnregisterSashAbsorption(ch);
		Sash_Broadcast(ch, ESashBroadcast::TYPE_ABSORPTION_CLEAR);

		// Readding pc
		RegisterSashAbsorption(ch);
		return true;
	}

	void ReleaseSashAbsorption(LPCHARACTER ch, LPITEM sash, LPITEM r_item)
	{
		if (!ch || !sash || !r_item)
			return;

		// Checking if provided item is valid
		if (r_item->GetVnum() != SASH_ABSORPTION_RELEASE_ITEM)
			return;

		// Checking if sash got absorption
		if (!HasAbsorption(sash))
			return;

		r_item->SetCount(0);

		sash->ClearAttribute();
		
#ifdef __LEGENDARY_STONES_ENABLE__
		// Clear the sockets from item!
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (i == SASH_TYPE_SOCKET)
				continue;

			if (i == SASH_ABSORPTION_SOCKET)
				continue;

			sash->SetSocket(i, 0);
		}
#endif

		sash->SetSocket(SASH_ABSORPTION_SOCKET, 0);
		Sash_ChatInfo(ch, ESashChatInfo::INFO_ABSORPTION_REMOVE_SUCCESS);
	}
	/// END ///
	
	bool RefineSashAbsorbed(LPCHARACTER ch, LPITEM sash, int iRefineType, bool bReopen, LPITEM uItem)
	{
		if (!ch || !sash)
			return false;

		if (iRefineType == -1 || iRefineType > SASH_REFINE_ABSORB_ITEM_MAX_NUM)
			return false;

		const auto pProto = ITEM_MANAGER::instance().GetTable(GetAbsorptionBase(sash));
		if (!pProto)
			return false;
		
		const TRefineTable* sRefTable = CRefineManager::instance().GetRefineRecipe(pProto->wRefineSet);
		if (!sRefTable)
			return false;

		if (pProto->dwRefinedVnum == 0)
			return false;

		// Checking if we have additional item
		DWORD iScrollVnum = 0;
		BYTE iScrollPercent = 0;
		bool bKeepItem = false;

		if (iRefineType == SASH_REFINE_ABSORB_ITEM_ITEM)
		{
			if (!uItem)
				return false;

			if (ch->CountSpecifyItem(uItem->GetVnum()) < 1)
				return false;

			iScrollVnum = uItem->GetVnum();
			iScrollPercent = uItem->GetValue(1);

			const auto eScrollTypes = { 1, 2 };
			for (const auto& rElement : eScrollTypes)
			{
				if (rElement == uItem->GetValue(0)) bKeepItem = true;
			}
		}

		// Lets remove the materials
		for (auto material : sRefTable->materials)
		{
			if (ch->CountSpecifyItem(material.vnum, sash->GetCell()) < material.count)
				return false;
		}

		for (auto material : sRefTable->materials)
		{
			ch->RemoveSpecifyItem(material.vnum, material.count, sash->GetCell());
		}

		// Lets remove the scroll item
		if (iRefineType == SASH_REFINE_ABSORB_ITEM_ITEM)
		{
			uItem->SetCount(uItem->GetCount() - 1);
		}

		if (Random::get(1, 100) <= std::min(sRefTable->prob + iScrollPercent, 100))
		{
			// Lets notify the client
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

			sash->SetSocket(SASH_ABSORPTION_SOCKET, pProto->dwRefinedVnum);

			if (bReopen)
			{
				switch (iRefineType)
				{
					case SASH_REFINE_ABSORB_ITEM_NPC:
					{
						ch->RefineInformation(sash, REFINE_TYPE_NORMAL, nullptr, true);
					}
					break;

					case SASH_REFINE_ABSORB_ITEM_ITEM:
					{
						if (iScrollVnum == 0)
						{
							ch->CloseRefine();
							return false;
						}

						if (ch->CountSpecifyItem(iScrollVnum) <= 0)
						{
							ch->CloseRefine();
							return false;
						}

						ch->RefineItem(ch->FindSpecifyItem(iScrollVnum), sash);
					}
					break;

					default:
					{
						return false;
					}
				}
			}
		}
		else if (!bKeepItem && iRefineType == SASH_REFINE_ABSORB_ITEM_ITEM)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailedType %d %d", REFINE_FAIL_GRADE_DOWN, bReopen);

			// Decrease a current item!
			sash->SetSocket(SASH_ABSORPTION_SOCKET, ITEM_MANAGER::instance().GetRefineFromVnum(GetAbsorptionBase(sash)));

			if (bReopen)
			{
				if (iScrollVnum == 0)
				{
					ch->CloseRefine();
					return false;
				}

				if (ch->CountSpecifyItem(iScrollVnum) <= 0)
				{
					ch->CloseRefine();
					return false;
				}

				ch->RefineItem(ch->FindSpecifyItem(iScrollVnum), sash);
			}
		}
		else if (bKeepItem && iRefineType == SASH_REFINE_ABSORB_ITEM_ITEM)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailedType %d %d", REFINE_FAIL_KEEP_GRADE, bReopen);

			if (bReopen)
			{
				if (iScrollVnum == 0)
				{
					ch->CloseRefine();
					return false;
				}

				if (ch->CountSpecifyItem(iScrollVnum) <= 0)
				{
					ch->CloseRefine();
					return false;
				}

				ch->RefineItem(ch->FindSpecifyItem(iScrollVnum), sash);
			}
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailedType %d %d", REFINE_FAIL_DEL_ITEM, bReopen);

			ITEM_MANAGER::instance().RemoveItem(sash, "Refine absorbed item of sash");
			ch->CloseRefine();
			return false;			
		}

		return true;
	}

	/// Broadcaster ///
	void Sash_Broadcast(LPCHARACTER ch, const ESashBroadcast & broadType, LPITEM item, WORD slot, int cost)
	{
		if (!ch)
			return;

		switch (broadType)
		{
			case ESashBroadcast::TYPE_COMBINATION_OPEN:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Combination_Open");
				break;
			case ESashBroadcast::TYPE_COMBINATION_ITEM_UPDATE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Combination_Update_Slot %d %d %d", slot, item ? item->GetVnum() : 0, item ? item->GetCell() : -1);
				break;
			case ESashBroadcast::TYPE_COMBINATION_COST_UPDATE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Combination_UpdateCost %d", static_cast<int>(cost));
				break;
			case ESashBroadcast::TYPE_COMBINATION_CLEAR:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Combination_Clear");
				break;
			case ESashBroadcast::TYPE_COMBINATION_CLOSE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Combination_Close");
				break;
			case ESashBroadcast::TYPE_ABSORPTION_OPEN:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Absorption_Open");
				break;
			case ESashBroadcast::TYPE_ABSORPTION_ITEM_UPDATE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Absorption_Update_Slot %d %d %d", slot, item ? item->GetVnum() : 0, item ? item->GetCell() : -1);
				break;
			case ESashBroadcast::TYPE_ABSORPTION_CLEAR:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Absorption_Clear");
				break;
			case ESashBroadcast::TYPE_ABSORPTION_CLOSE:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Absorption_Close");
				break;
			case ESashBroadcast::TYPE_INVENTORY_REFRESH:
				ch->ChatPacket(CHAT_TYPE_COMMAND, "Sash_Inventory_Refresh");
				break;
		}
	}

	void Sash_ChatInfo(LPCHARACTER ch, const ESashChatInfo & chatType)
	{
		if (!ch)
			return;

		std::string chatText("");

		switch (chatType)
		{
			case ESashChatInfo::INFO_COMBINATION_ITEM_ADDED:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_ITEM_ADDED");
				break;
			case ESashChatInfo::INFO_COMBINATION_ITEM_REMOVED:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_ITEM_REMOVED");
				break;
			case ESashChatInfo::INFO_COMBINATION_ITEM_NOT_MATCH:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_ITEM_NOT_MATCH");
				break;
			case ESashChatInfo::INFO_COMBINATION_NOT_ENOUGH_MONEY:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_NOT_ENOUGH_MONEY");
				break;
			case ESashChatInfo::INFO_COMBINATION_PROCESS_SUCCESS:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_PROCESS_SUCCESS");
				break;
			case ESashChatInfo::INFO_COMBINATION_PROCESS_FAIL:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_PROCESS_FAIL");
				break;
			case ESashChatInfo::INFO_ABSORPTION_ITEM_ADDED:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_ITEM_ADDED");
				break;
			case ESashChatInfo::INFO_ABSORPTION_ITEM_REMOVED:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_ITEM_REMOVED");
				break;
			case ESashChatInfo::INFO_ABSORPTION_ITEM_NOT_MATCH:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_ITEM_NOT_MATCH");
				break;
			case ESashChatInfo::INFO_ABSORPTION_PROCESS_SUCCESS:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_PROCESS_SUCCESS");
				break;
			case ESashChatInfo::INFO_ABSORPTION_PROCESS_FAIL:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_PROCESS_FAIL");
				break;
			case ESashChatInfo::INFO_ABSORPTION_REMOVE_SUCCESS:
				chatText = LC_TEXT("SASH_SYSTEM_ABSORPTION_REMOVE_SUCCESS");
				break;
			case ESashChatInfo::INFO_COMBINATION_ITEM_CANNOT_UPGRADE:
				chatText = LC_TEXT("SASH_SYSTEM_COMBINATION_ITEM_CANNOT_UPGRADE");
				break;
		}

		if (chatText.size())
			ch->ChatPacket(CHAT_TYPE_INFO, chatText.c_str());
	}
	/// End ///
}
#endif

