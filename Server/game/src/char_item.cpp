#include "stdafx.h"

#include <stack>

#include "utils.h"
#include "config.h"
#include "char.h"
#include "char_manager.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "packet.h"
#include "affect.h"
#include "skill.h"
#include "start_position.h"
#include "mob_manager.h"
#include "db.h"
#include "log.h"
#include "vector.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "fishing.h"
#include "party.h"
#include "dungeon.h"
#include "refine.h"
#include "unique_item.h"
#include "war_map.h"
#include "xmas_event.h"
#include "marriage.h"
#include "polymorph.h"
#include "blend_item.h"
#include "castle.h"
#include "BattleArena.h"
#include "arena.h"
#include "dev_log.h"
#include "pcbang.h"
#include "threeway_war.h"

#include "safebox.h"
#include "shop.h"

#ifdef __ENABLE_NEWSTUFF__
	#include "pvp.h"
#endif

#ifdef __ENABLE_SWITCHBOT__
	#include "switchbot.h"
#endif

#include "../../common/VnumHelper.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/CommonDefines.h"

#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
	#include "gm.h"
#endif

#ifdef __ABUSE_CONTROLLER_ENABLE__
	#include "AbuseController.h"
#endif

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	#include "OfflineShop.h"
#endif

#ifdef __ENABLE_NEW_LOGS_CHAT__
	#include "ChatLogsHelper.hpp"
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	#include "SashSystemHelper.hpp"
#endif

#include "EquipmentSet.hpp"

const int ITEM_BROKEN_METIN_VNUM = 28960;
#define ENABLE_EFFECT_EXTRAPOT
#define ENABLE_BOOKS_STACKFIX

// CHANGE_ITEM_ATTRIBUTES
// const DWORD CHARACTER::msc_dwDefaultChangeItemAttrCycle = 10;
const char CHARACTER::msc_szLastChangeItemAttrFlag[] = "Item.LastChangeItemAttr";
// const char CHARACTER::msc_szChangeItemAttrCycleFlag[] = "change_itemattr_cycle";
// END_OF_CHANGE_ITEM_ATTRIBUTES
const BYTE g_aBuffOnAttrPoints[] = { POINT_ENERGY, POINT_COSTUME_ATTR_BONUS };

struct FFindStone
{
	std::map<DWORD, LPCHARACTER> m_mapStone;

	void operator()(LPENTITY pEnt)
	{
		if (pEnt->IsType(ENTITY_CHARACTER) == true)
		{
			LPCHARACTER pChar = (LPCHARACTER)pEnt;

			if (pChar->IsStone() == true)
			{
				m_mapStone[(DWORD)pChar->GetVID()] = pChar;
			}
		}
	}
};

static bool IS_SUMMON_ITEM(int vnum)
{
	switch (vnum)
	{
	case 22000:
	case 22010:
	case 22011:
	case 22020:
	case ITEM_MARRIAGE_RING:
		return true;
	}

	return false;
}

static bool IS_MONKEY_DUNGEON(int map_index)
{
	switch (map_index)
	{
	case 5:
	case 25:
	case 45:
	case 108:
	case 109:
		return true;;
	}

	return false;
}

bool IS_SUMMONABLE_ZONE(int map_index)
{

	if (IS_MONKEY_DUNGEON(map_index))
	{
		return false;
	}
	if (IS_CASTLE_MAP(map_index))
	{
		return false;
	}

	switch (map_index)
	{
	case 66 :
	case 71 :
	case 72 :
	case 73 :
	case 193 :
#if 0
	case 184 :
	case 185 :
	case 186 :
	case 187 :
	case 188 :
	case 189 :
#endif

	case 216 :
	case 217 :
	case 208 :

	case 113 :
		return false;
	}

	if (CBattleArena::IsBattleArenaMap(map_index)) { return false; }


	if (map_index > 10000) { return false; }

	return true;
}

bool IS_BOTARYABLE_ZONE(int nMapIndex)
{
	if (!g_bEnableBootaryCheck) { return true; }

	switch (nMapIndex)
	{
	case 1 :
	case 3 :
	case 21 :
	case 23 :
	case 41 :
	case 43 :
		return true;
	}

	return false;
}

#ifdef __ENABLE_FIX_STACK_ITEMS__
static bool FN_compare_item_socket(const LPITEM pkItemSrc, const LPITEM pkItemDest)
{
	if (!pkItemSrc || !pkItemDest)
	{
		return false;
	}

	return memcmp(pkItemSrc->GetSockets(), pkItemDest->GetSockets(), sizeof(long) * ITEM_SOCKET_MAX_NUM) == 0;
}
#endif

static bool FN_check_item_socket_new(LPITEM item1, LPITEM item2)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item1->GetSocket(i) != item2->GetSocket(i))
		{
			return false;
		}
	}

	return true;
}

static bool FN_check_item_socket(LPITEM item)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item->GetSocket(i) != item->GetProto()->alSockets[i])
		{
			return false;
		}
	}

	return true;
}

static void FN_copy_item_socket(LPITEM dest, LPITEM src)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		dest->SetSocket(i, src->GetSocket(i));
	}
}
static bool FN_check_item_sex(LPCHARACTER ch, LPITEM item)
{

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_MALE))
	{
		if (SEX_MALE == GET_SEX(ch))
		{
			return false;
		}
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_FEMALE))
	{
		if (SEX_FEMALE == GET_SEX(ch))
		{
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// ITEM HANDLING
/////////////////////////////////////////////////////////////////////////////
bool CHARACTER::CanHandleItem(bool bSkipCheckRefine, bool bSkipObserver)
{
	if (!bSkipObserver)
		if (m_bIsObserver)
		{
			return false;
		}

	if (GetMyShop())
	{
		return false;
	}

	if (!bSkipCheckRefine)
		if (m_bUnderRefine)
		{
			return false;
		}

	if (IsCubeOpen() || NULL != DragonSoul_RefineWindow_GetOpener())
	{
		return false;
	}

	if (IsWarping())
	{
		return false;
	}

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	if (m_deleteSocket.open)
	{
		return false;
	}
#endif

	return true;
}

LPITEM CHARACTER::GetInventoryItem(WORD wCell) const
{
	return GetItem(TItemPos(INVENTORY, wCell));
}
LPITEM CHARACTER::GetItem(TItemPos Cell) const
{
	if (!IsValidItemPosition(Cell))
	{
		return NULL;
	}

	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;

	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pItems[wCell];
	case DRAGON_SOUL_INVENTORY:
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid DS item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pDSItems[wCell];

#ifdef __ENABLE_SWITCHBOT__
	case SWITCHBOT:
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::GetInventoryItem: invalid switchbot item cell %d", wCell);
			return NULL;
		}
		return m_pointsInstant.pSwitchbotItems[wCell];
#endif

	default:
		return NULL;
	}
	return NULL;
}

#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
	void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem, bool bWereMine)
#else
	void CHARACTER::SetItem(TItemPos Cell, LPITEM pItem)
#endif
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	if ((unsigned long)((CItem*)pItem) == 0xff || (unsigned long)((CItem*)pItem) == 0xffffffff)
	{
		sys_err("!!! FATAL ERROR !!! item == 0xff (char: %s cell: %u)", GetName(), wCell);
		core_dump();
		return;
	}

	if (pItem && pItem->GetOwner())
	{
		assert(!"GetOwner exist");
		return;
	}

	switch (window_type)
	{
	case INVENTORY:
	case EQUIPMENT:
	{
		if (wCell >= INVENTORY_AND_EQUIP_SLOT_MAX)
		{
			sys_err("CHARACTER::SetItem: invalid item cell %d", wCell);
			return;
		}

		LPITEM pOld = m_pointsInstant.pItems[wCell];

		if (pOld)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
					{
						continue;
					}

					if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
					{
						continue;
					}

					m_pointsInstant.bItemGrid[p] = 0;
				}
			}
#ifdef __SPECIAL_STORAGE_ENABLE__
			else if (Cell.IsSpecialStorage())
			{
				auto pPair = TItemPos::GetSpecialStorageRange(Cell.GetSpecialStorageType());
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * SPECIAL_STORAGE_PAGE_LENGTH);

					if (p >= pPair.second)
					{
						continue;
					}

					if (m_pointsInstant.pItems[p] && m_pointsInstant.pItems[p] != pOld)
					{
						continue;
					}

					m_pointsInstant.bItemGrid[p] = 0;
				}
			}
#endif
			else
			{
				m_pointsInstant.bItemGrid[wCell] = 0;
			}
		}

		if (pItem)
		{
			if (wCell < INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * 5);

					if (p >= INVENTORY_MAX_NUM)
					{
						continue;
					}



					m_pointsInstant.bItemGrid[p] = wCell + 1;
				}
			}
#ifdef __SPECIAL_STORAGE_ENABLE__
			else if (Cell.IsSpecialStorage())
			{
				auto pPair = TItemPos::GetSpecialStorageRange(Cell.GetSpecialStorageType());
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * SPECIAL_STORAGE_PAGE_LENGTH);

					if (p >= pPair.second)
					{
						continue;
					}

					m_pointsInstant.bItemGrid[p] = wCell + 1;
				}
			}
#endif
			else
			{
				m_pointsInstant.bItemGrid[wCell] = wCell + 1;
			}
		}

		m_pointsInstant.pItems[wCell] = pItem;
	}
	break;

	case DRAGON_SOUL_INVENTORY:
	{
		LPITEM pOld = m_pointsInstant.pDSItems[wCell];

		if (pOld)
		{
			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pOld->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					{
						continue;
					}

					if (m_pointsInstant.pDSItems[p] && m_pointsInstant.pDSItems[p] != pOld)
					{
						continue;
					}

					m_pointsInstant.wDSItemGrid[p] = 0;
				}
			}
			else
			{
				m_pointsInstant.wDSItemGrid[wCell] = 0;
			}
		}

		if (pItem)
		{
			if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				sys_err("CHARACTER::SetItem: invalid DS item cell %d", wCell);
				return;
			}

			if (wCell < DRAGON_SOUL_INVENTORY_MAX_NUM)
			{
				for (int i = 0; i < pItem->GetSize(); ++i)
				{
					int p = wCell + (i * DRAGON_SOUL_BOX_COLUMN_NUM);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					{
						continue;
					}



					m_pointsInstant.wDSItemGrid[p] = wCell + 1;
				}
			}
			else
			{
				m_pointsInstant.wDSItemGrid[wCell] = wCell + 1;
			}
		}

		m_pointsInstant.pDSItems[wCell] = pItem;
	}
	break;
#ifdef __ENABLE_SWITCHBOT__
	case SWITCHBOT:
	{
		LPITEM pOld = m_pointsInstant.pSwitchbotItems[wCell];
		if (pItem && pOld)
		{
			return;
		}

		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			sys_err("CHARACTER::SetItem: invalid switchbot item cell %d", wCell);
			return;
		}

		if (pItem)
		{
			CSwitchbotManager::Instance().RegisterItem(GetPlayerID(), pItem->GetID(), wCell);
		}
		else
		{
			CSwitchbotManager::Instance().UnregisterItem(GetPlayerID(), wCell);
		}

		m_pointsInstant.pSwitchbotItems[wCell] = pItem;
	}
	break;
#endif
	default:
		sys_err ("Invalid Inventory type %d", window_type);
		return;
	}

	if (GetDesc())
	{

		if (pItem)
		{
			TPacketGCItemSet pack;
			pack.header = HEADER_GC_ITEM_SET;
			pack.Cell = Cell;

			pack.count = pItem->GetCount();
			pack.vnum = pItem->GetVnum();
			pack.flags = pItem->GetFlag();
			pack.anti_flags	= pItem->GetAntiFlag();
#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
			pack.highlight = !bWereMine || (Cell.window_type == DRAGON_SOUL_INVENTORY);
			(Cell.window_type == DRAGON_SOUL_INVENTORY);
#else
			pack.highlight = (Cell.window_type == DRAGON_SOUL_INVENTORY);
#endif

#ifdef __TRANSMUTATION_SYSTEM__
			pack.transmutate_id = pItem->GetTransmutate();
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
			pack.dwRefineElement = pItem->GetRefineElement();
#endif

			thecore_memcpy(pack.alSockets, pItem->GetSockets(), sizeof(pack.alSockets));
			thecore_memcpy(pack.aAttr, pItem->GetAttributes(), sizeof(pack.aAttr));

#ifndef __INVENTORY_BUFFERING__
			GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
#else
			if (!bInvBuff)
			{
				GetDesc()->Packet(&pack, sizeof(TPacketGCItemSet));
			}
			else
				// Put item into set
			{
				AddItemToInvBuff(pItem);
			}
#endif
		}
		else
		{
			TPacketGCItemDelDeprecated pack;
			pack.header = HEADER_GC_ITEM_DEL;
			pack.Cell = Cell;
			pack.count = 0;
			pack.vnum = 0;
			memset(pack.alSockets, 0, sizeof(pack.alSockets));
			memset(pack.aAttr, 0, sizeof(pack.aAttr));

#ifdef __TRANSMUTATION_SYSTEM__
			pack.transmutate_id = 0;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
			pack.dwRefineElement = 0;
#endif

			GetDesc()->Packet(&pack, sizeof(TPacketGCItemDelDeprecated));
		}
	}

	if (pItem)
	{
		pItem->SetCell(this, wCell);
		switch (window_type)
		{
		case INVENTORY:
		case EQUIPMENT:
#ifndef __SPECIAL_STORAGE_ENABLE__
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell))
#else
			if ((wCell < INVENTORY_MAX_NUM) || (BELT_INVENTORY_SLOT_START <= wCell && BELT_INVENTORY_SLOT_END > wCell) || Cell.IsSpecialStorage())
#endif
				pItem->SetWindow(INVENTORY);
			else
			{
				pItem->SetWindow(EQUIPMENT);
			}
			break;
		case DRAGON_SOUL_INVENTORY:
			pItem->SetWindow(DRAGON_SOUL_INVENTORY);
			break;
#ifdef __ENABLE_SWITCHBOT__
		case SWITCHBOT:
			pItem->SetWindow(SWITCHBOT);
			break;
#endif
		}
	}

#ifdef __ADMIN_MANAGER__
	if (GetDesc() && (Cell.window_type == INVENTORY || Cell.window_type == EQUIPMENT))
	{
		CAdminManager::instance().OnPlayerItemSet(this, wCell);
	}
#endif
}

LPITEM CHARACTER::GetWear(BYTE bCell) const
{

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::GetWear: invalid wear cell %d", bCell);
		return NULL;
	}

	return m_pointsInstant.pItems[INVENTORY_MAX_NUM + bCell];
}

void CHARACTER::SetWear(BYTE bCell, LPITEM item)
{

	if (bCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
	{
		sys_err("CHARACTER::SetItem: invalid item cell %d", bCell);
		return;
	}

	SetItem(TItemPos (INVENTORY, INVENTORY_MAX_NUM + bCell), item);

	if (!item && bCell == WEAR_WEAPON && !m_bIsDisconnecting)
	{

		if (IsAffectFlag(AFF_GWIGUM))
		{
			RemoveAffect(SKILL_GWIGEOM);
		}

		if (IsAffectFlag(AFF_GEOMGYEONG))
		{
			RemoveAffect(SKILL_GEOMKYUNG);
		}
	}

	if (!m_bIsDisconnecting)
		RefreshEquipmentSet();
}

void CHARACTER::ClearItem()
{
	int		i;
	LPITEM	item;

	for (i = INVENTORY_AND_EQUIP_SLOT_MAX - 1; i >= 0; --i)
	{
		if ((item = GetInventoryItem(i)))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);

			SyncQuickslot(QUICKSLOT_TYPE_ITEM, i, 255);
		}
	}
	for (i = 0; i < DRAGON_SOUL_INVENTORY_MAX_NUM; ++i)
	{
		if ((item = GetItem(TItemPos(DRAGON_SOUL_INVENTORY, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#ifdef __ENABLE_SWITCHBOT__
	for (i = 0; i < SWITCHBOT_SLOT_COUNT; ++i)
	{
		if ((item = GetItem(TItemPos(SWITCHBOT, i))))
		{
			item->SetSkipSave(true);
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->RemoveFromCharacter();
			M2_DESTROY_ITEM(item);
		}
	}
#endif
}

bool CHARACTER::IsEmptyItemGrid(TItemPos Cell, BYTE bSize, int iExceptionCell) const
{
	switch (Cell.window_type)
	{
	case INVENTORY:
	{
		WORD bCell = Cell.cell;

		++iExceptionCell;

		if (Cell.IsBeltInventoryPosition())
		{
			LPITEM beltItem = GetWear(WEAR_BELT);

			if (NULL == beltItem)
			{
				return false;
			}

			if (false == CBeltInventoryHelper::IsAvailableCell(bCell - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
			{
				return false;
			}

			if (m_pointsInstant.bItemGrid[bCell])
			{
				if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
				{
					return true;
				}

				return false;
			}

			if (bSize == 1)
			{
				return true;
			}

		}
#ifdef __SPECIAL_STORAGE_ENABLE__
		else if (Cell.IsSpecialStorage())
		{
			auto pPair = TItemPos::GetSpecialStorageRange(Cell.GetSpecialStorageType());
			if (pPair.first == pPair.second)
				// Something went wrong
			{
				return false;
			}

			if (bCell < pPair.first)
			{
				return false;
			}

			if (bCell >= pPair.second)
			{
				return false;
			}

			if (m_pointsInstant.bItemGrid[bCell])
			{
				if (m_pointsInstant.bItemGrid[bCell] == (UINT)iExceptionCell)
				{
					if (bSize == 1)
					{
						return true;
					}

					int j = 1;
					UINT bPage = bCell / SPECIAL_STORAGE_PAGE_SIZE;

					do
					{
						UINT p = bCell + (SPECIAL_STORAGE_PAGE_LENGTH * j);

						if (p >= pPair.second)
						{
							return false;
						}

						if (p / SPECIAL_STORAGE_PAGE_SIZE != bPage)
						{
							return false;
						}

						if (m_pointsInstant.bItemGrid[p])
							if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
							{
								return false;
							}
					} while (++j < bSize);

					return true;
				}
				else
				{
					return false;
				}
			}

			if (1 == bSize)
			{
				return true;
			}
			else
			{
				int j = 1;
				WORD bPage = bCell / SPECIAL_STORAGE_PAGE_SIZE;

				do
				{
					WORD p = bCell + (SPECIAL_STORAGE_PAGE_LENGTH * j);

					if (p >= pPair.second)
					{
						return false;
					}

					if (p / SPECIAL_STORAGE_PAGE_SIZE != bPage)
					{
						return false;
					}

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
						{
							return false;
						}
				} while (++j < bSize);

				return true;
			}
		}
#endif
		else if (bCell >= INVENTORY_MAX_NUM)
		{
			return false;
		}

		if (m_pointsInstant.bItemGrid[bCell])
		{
			if (m_pointsInstant.bItemGrid[bCell] == iExceptionCell)
			{
				if (bSize == 1)
				{
					return true;
				}

				int j = 1;
				BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

				do
				{
					BYTE p = bCell + (5 * j);

					if (p >= INVENTORY_MAX_NUM)
					{
						return false;
					}

					if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
					{
						return false;
					}

					if (m_pointsInstant.bItemGrid[p])
						if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
						{
							return false;
						}
				} while (++j < bSize);

				return true;
			}
			else
			{
				return false;
			}
		}


		if (1 == bSize)
		{
			return true;
		}
		else
		{
			int j = 1;
			BYTE bPage = bCell / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT);

			do
			{
				BYTE p = bCell + (5 * j);

				if (p >= INVENTORY_MAX_NUM)
				{
					return false;
				}

				if (p / (INVENTORY_MAX_NUM / INVENTORY_PAGE_COUNT) != bPage)
				{
					return false;
				}

				if (m_pointsInstant.bItemGrid[p])
					if (m_pointsInstant.bItemGrid[p] != iExceptionCell)
					{
						return false;
					}
			} while (++j < bSize);

			return true;
		}
	}
	break;
	case DRAGON_SOUL_INVENTORY:
	{
		WORD wCell = Cell.cell;
		if (wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			return false;
		}



		iExceptionCell++;

		if (m_pointsInstant.wDSItemGrid[wCell])
		{
			if (m_pointsInstant.wDSItemGrid[wCell] == iExceptionCell)
			{
				if (bSize == 1)
				{
					return true;
				}

				int j = 1;

				do
				{
					int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

					if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
					{
						return false;
					}

					if (m_pointsInstant.wDSItemGrid[p])
						if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
						{
							return false;
						}
				} while (++j < bSize);

				return true;
			}
			else
			{
				return false;
			}
		}


		if (1 == bSize)
		{
			return true;
		}
		else
		{
			int j = 1;

			do
			{
				int p = wCell + (DRAGON_SOUL_BOX_COLUMN_NUM * j);

				if (p >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					return false;
				}

				if (m_pointsInstant.bItemGrid[p])
					if (m_pointsInstant.wDSItemGrid[p] != iExceptionCell)
					{
						return false;
					}
			} while (++j < bSize);

			return true;
		}
	}
#ifdef __ENABLE_SWITCHBOT__
	case SWITCHBOT:
	{
		WORD wCell = Cell.cell;
		if (wCell >= SWITCHBOT_SLOT_COUNT)
		{
			return false;
		}

		if (m_pointsInstant.pSwitchbotItems[wCell])
		{
			return false;
		}

		return true;
	}
#endif
	}
	return false;
}

#ifndef __SPECIAL_STORAGE_ENABLE__
	int CHARACTER::GetEmptyInventory(BYTE size) const
#else
	int CHARACTER::GetEmptyInventory(BYTE size, DWORD wWindowType, bool bSkipStandard) const
#endif
{
#ifndef __SPECIAL_STORAGE_ENABLE__
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
		{
			return i;
		}
#else
	// Set priority on special storage..
	auto pRange = TItemPos::GetSpecialStorageRange(wWindowType);
	for (int i = pRange.first; i < pRange.second; ++i)
	{
		if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
		{
			return i;
		}
	}

	if (!bSkipStandard)
	{
		// .. but consider it can be transfer to normal inventory as well
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
			if (IsEmptyItemGrid(TItemPos(INVENTORY, i), size))
			{
				return i;
			}
	}
#endif

	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(const TItemTable* pItemTable) const
{
	if (nullptr == pItemTable || pItemTable->bType != ITEM_DS)
		return -1;

	if (!DragonSoul_IsQualified()) {
		return -1;
	}

	BYTE bSize = pItemTable->bSize;
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItemTable);

	if (WORD_MAX == wBaseCell)
	{
		return -1;
	}

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell),
			bSize))
			return i + wBaseCell;

	return -1;
}

int CHARACTER::GetEmptyDragonSoulInventory(LPITEM pItem) const
{
	if (NULL == pItem || !pItem->IsDragonSoul())
	{
		return -1;
	}
	if (!DragonSoul_IsQualified())
	{
		return -1;
	}
	BYTE bSize = pItem->GetSize();
	WORD wBaseCell = DSManager::instance().GetBasePosition(pItem);

	if (WORD_MAX == wBaseCell)
	{
		return -1;
	}

	for (int i = 0; i < DRAGON_SOUL_BOX_SIZE; ++i)
		if (IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, i + wBaseCell), bSize))
		{
			return i + wBaseCell;
		}

	return -1;
}

void CHARACTER::CopyDragonSoulItemGrid(std::vector<WORD>& vDragonSoulItemGrid) const
{
	vDragonSoulItemGrid.resize(DRAGON_SOUL_INVENTORY_MAX_NUM);

	std::copy(m_pointsInstant.wDSItemGrid, m_pointsInstant.wDSItemGrid + DRAGON_SOUL_INVENTORY_MAX_NUM, vDragonSoulItemGrid.begin());
}

int CHARACTER::CountEmptyInventory() const
{
	int	count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i))
		{
			count += GetInventoryItem(i)->GetSize();
		}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			if (GetInventoryItem(i))
			{
				count += GetInventoryItem(i)->GetSize();
			}
		}
	}
#endif

#ifndef __SPECIAL_STORAGE_ENABLE__
	return (INVENTORY_MAX_NUM - count);
#else
	return (INVENTORY_AND_EQUIP_SLOT_MAX - WEAR_MAX_NUM - count);
#endif
}

void TransformRefineItem(LPITEM pkOldItem, LPITEM pkNewItem)
{
	// ACCESSORY_REFINE
	if (pkOldItem->IsAccessoryForSocket())
	{
		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			pkNewItem->SetSocket(i, pkOldItem->GetSocket(i));
		}
		//pkNewItem->StartAccessorySocketExpireEvent();
	}
	// END_OF_ACCESSORY_REFINE
	else
	{

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			if (!pkOldItem->GetSocket(i))
			{
				break;
			}
			else
			{
				pkNewItem->SetSocket(i, 1);
			}
		}


		int slot = 0;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			long socket = pkOldItem->GetSocket(i);

			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				pkNewItem->SetSocket(slot++, socket);
			}
		}

	}


	pkOldItem->CopyAttributeTo(pkNewItem);
}

#ifdef __ENABLE_FAST_REFINE_OPTION__
void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way, bool IsFastRefine = false)
{
	if (NULL != ch && item != NULL)
	{
		if (IsFastRefine)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Refine] The upgrade has successful!"));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");
		}

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);

#ifdef __BATTLE_PASS_ENABLE__
		CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective { BattlePassNS::EObjectives::TYPE_REFINE, item->GetVnum(), 1 });
		CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_REFINE_ALL, 0, 1 });
#endif
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0, bool IsFastRefine = false)
{
	if (NULL != ch && NULL != item)
	{
		if (IsFastRefine)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("[Refine] The upgrade has been failed."));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");
		}

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}
#else
void NotifyRefineSuccess(LPCHARACTER ch, LPITEM item, const char* way)
{
	if (NULL != ch && item != NULL)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineSuceeded");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), 1, way);
	}
}

void NotifyRefineFail(LPCHARACTER ch, LPITEM item, const char* way, int success = 0)
{
	if (NULL != ch && NULL != item)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailed");

		LogManager::instance().RefineLog(ch->GetPlayerID(), item->GetName(), item->GetID(), item->GetRefineLevel(), success, way);
	}
}
#endif

#ifdef __ENABLE_REFINE_MSG_ADD__
void NotifyRefineFailType(const LPCHARACTER pkChr, const LPITEM pkItem, const BYTE bType, const std::string stRefineType, const BYTE bSuccess = 0, const BYTE autoReopen = 0)
{
	if (pkChr && pkItem)
	{
		pkChr->ChatPacket(CHAT_TYPE_COMMAND, "RefineFailedType %d %d", bType, autoReopen);
		LogManager::instance().RefineLog(pkChr->GetPlayerID(), pkItem->GetName(), pkItem->GetID(), pkItem->GetRefineLevel(), bSuccess, stRefineType.c_str());
	}
}
#endif

void CHARACTER::SetRefineNPC(LPCHARACTER ch)
{
	if ( ch != NULL )
	{
		m_dwRefineNPCVID = ch->GetVID();
	}
	else
	{
		m_dwRefineNPCVID = 0;
	}
}

#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
struct FindBlacksmith
{
	LPCHARACTER m_ch;
	bool found;
	bool m_throughGuild;
	int MAX_BLACKSMITH_DIST = 7500; //Around 5000 is the max distance, but to have a bit of room.

	FindBlacksmith(LPCHARACTER ch, bool throughGuild)
	{
		m_ch = ch;
		found = false;
		m_throughGuild = throughGuild;
	}

	void operator()(LPENTITY ent)
	{
		if (found) //Skip all the next entities - we already got what we wanted
		{
			return;
		}

		if (ent->IsType(ENTITY_CHARACTER))
		{
			LPCHARACTER ch = (LPCHARACTER)ent;
			if (ch->IsNPC())
			{
				DWORD vnum = ch->GetMobTable().dwVnum;
				switch (vnum)
				{
				case BLACKSMITH_WEAPON_MOB:
				case BLACKSMITH_ARMOR_MOB:
				case BLACKSMITH_ACCESSORY_MOB:
				{
					if (!m_throughGuild) //We can ignore proximity to guild blacksmiths if we are not using guild
					{
						return;
					}

					int dist = DISTANCE_APPROX(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
					if (test_server)
					{
						sys_log(0, "Guild blacksmith found within %d distance units.", dist);
					}

					if (dist < MAX_BLACKSMITH_DIST)
					{
						found = true;
					}

					break;
				}

				case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
				case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
				case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
				case BLACKSMITH_MOB:
				case BLACKSMITH2_MOB:
				{
					if (m_throughGuild) //Poximity to non-guild blacksmiths is irrelevant if we are using guild
					{
						return;
					}

					int dist = DISTANCE_APPROX(m_ch->GetX() - ch->GetX(), m_ch->GetY() - ch->GetY());
					if (test_server)
					{
						sys_log(0, "Blacksmith (%lu) found within %d distance units.", vnum, dist);
					}

					if (dist < MAX_BLACKSMITH_DIST)
					{
						found = true;
					}

					break;
				}
				}
			}
		}

		return;
	}
};
#endif

bool CHARACTER::DoRefine(LPITEM item, bool bMoneyOnly, bool autoReopen)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	if (!item)
	{
		return false;
	}

#ifdef __ENABLE_BLACKSMITH_TO_FAR_BLOCKED__
	FindBlacksmith f(this, IsRefineThroughGuild());
	if (LPSECTREE tree = GetSectree())
	{
		tree->ForEachAround(f);
	}

	if (!f.found)
	{
		LogManager::instance().HackLog("REFINE_FAR_BLACKSMITH", this);
		//We let continue - GMs will decide/ban.
	}
#endif


#ifdef __ENABLE_SASH_SYSTEM__
	if (SashSystemHelper::HasAbsorption(item))
	{
		return SashSystemHelper::RefineSashAbsorbed(this, item, SashSystemHelper::ESashSystem::SASH_REFINE_ABSORB_ITEM_NPC, autoReopen);
	}
#endif

	const TRefineTable* prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		return false;
	}

	DWORD result_vnum = item->GetRefinedVnum();

	// REFINE_COST
	int cost = ComputeRefineFee(prt->cost);

	int RefineChance = GetQuestFlag("main_quest_lv7.refine_chance");

	if (RefineChance > 0)
	{
		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The free weapon improvements can only be used on weapons up to level 20."));
			return false;
		}

		cost = 0;
		SetQuestFlag("main_quest_lv7.refine_chance", RefineChance - 1);
	}
	// END_OF_REFINE_COST

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No further improvements possible."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		return false;
	}

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefine NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	// REFINE_COST
	if (GetGold() < cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough Yang to use this item."));
		return false;
	}

	if (!bMoneyOnly && !RefineChance)
	{
		for (int i = 0; i < prt->material_count; ++i)
		{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
			if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
#else
			if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
#endif
			{
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
				}
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not the right material for an upgrade."));
				return false;
			}
		}

		for (int i = 0; i < prt->material_count; ++i)
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());
#else
			RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
#endif
	}

	int prob = number(1, 100);

	bool addedPercent = false;
	if (IsRefineThroughGuild() || bMoneyOnly)
	{
		prob -= 10;
		addedPercent = true;
	}

	// END_OF_REFINE_COST

	if (prob <= prt->prob)
	{

		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			// DETAIL_REFINE_LOG
			NotifyRefineSuccess(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER", autoReopen);

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");
			// END_OF_DETAIL_REFINE_LOG

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			sys_log(0, "Refine Success %d", cost);
			pkNewItem->AttrLog();
			sys_log(0, "PayPee %d", cost);
			PayRefineFee(cost);
			sys_log(0, "PayPee End %d", cost);

			if (autoReopen)
			{
				if (!(pkNewItem->GetRefinedVnum()))
				{
					CloseRefine();
					return false;
				}

				RefineInformation(pkNewItem, REFINE_TYPE_NORMAL, nullptr, true, (addedPercent) ? 10 : 0);
			}
		}
		else
		{
			// DETAIL_REFINE_LOG

#ifdef __ENABLE_REFINE_MSG_ADD__
			NotifyRefineFailType(this, item, REFINE_FAIL_KEEP_GRADE, IsRefineThroughGuild() ? "GUILD" : "POWER", autoReopen);
#else
			NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
#endif
			// END_OF_DETAIL_REFINE_LOG

			CloseRefine();
		}
	}
	else
	{

		DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -cost);
#ifdef __ENABLE_REFINE_MSG_ADD__
		NotifyRefineFailType(this, item, REFINE_FAIL_DEL_ITEM, IsRefineThroughGuild() ? "GUILD" : "POWER", autoReopen);
#else
		NotifyRefineFail(this, item, IsRefineThroughGuild() ? "GUILD" : "POWER");
#endif
		item->AttrLog();
		ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

		PayRefineFee(cost);

		CloseRefine();
	}

	return true;
}

enum enum_RefineScrolls
{
	STANDARD_SCROLL = 0,
	HYUNIRON_CHN   = 1,
	YONGSIN_SCROLL = 2,
	MUSIN_SCROLL   = 3,
	YAGONG_SCROLL  = 4,
	MEMO_SCROLL	   = 5,
	BDRAGON_SCROLL	= 6,
#ifdef __ENABLE_SOUL_SYSTEM__
	SOUL_SCROLL = 7,
#endif
};

#ifdef __ENABLE_FAST_REFINE_OPTION__
static LPITEM FindScrollByVnum(DWORD dwVnum, LPCHARACTER ch)
{
	if (!ch)
	{
		return NULL;
	}

	return ch->FindSpecifyItem(dwVnum);
}

bool CHARACTER::DoRefineWithScroll(LPITEM item, bool autoReopen)
#else
bool CHARACTER::DoRefineWithScroll(LPITEM item)
#endif
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	const TRefineTable * prt = CRefineManager::instance().GetRefineRecipe(item->GetRefineSet());

	if (!prt)
	{
		return false;
	}

	LPITEM pkItemScroll = FindSpecifyItem(m_dwRefineScrollVnum);

	if (!pkItemScroll)
	{
		return false;
	}

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
	{
		return false;
	}

	if (pkItemScroll->GetVnum() == item->GetVnum())
	{
		return false;
	}

#ifdef __ENABLE_SASH_SYSTEM__
	if (SashSystemHelper::HasAbsorption(item))
	{
		return SashSystemHelper::RefineSashAbsorbed(this, item, SashSystemHelper::ESashSystem::SASH_REFINE_ABSORB_ITEM_ITEM, autoReopen, pkItemScroll);
	}
#endif

	auto scroll_vnum = pkItemScroll->GetVnum();
	auto scroll_value = pkItemScroll->GetValue(0);
	auto scroll_added_percent = pkItemScroll->GetValue(1);

	auto result_vnum = item->GetRefinedVnum();
	auto result_fail_vnum = item->GetRefineFromVnum();

	if (result_vnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No further improvements possible."));
		return false;
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	if (GetGold() < prt->cost)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough Yang to use this item."));
		return false;
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
		if (CountSpecifyItem(prt->materials[i].vnum, item->GetCell()) < prt->materials[i].count)
#else
		if (CountSpecifyItem(prt->materials[i].vnum) < prt->materials[i].count)
#endif
		{
			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "Find %d, count %d, require %d", prt->materials[i].vnum, CountSpecifyItem(prt->materials[i].vnum), prt->materials[i].count);
			}

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not the right material for an upgrade."));
			return false;
		}
	}

	for (int i = 0; i < prt->material_count; ++i)
	{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count, item->GetCell());
#else
		RemoveSpecifyItem(prt->materials[i].vnum, prt->materials[i].count);
#endif
	}

	auto prob = number(1, 100);
	auto success_prob = prt->prob;
	bool bDestroyWhenFail = false;

	const char* szRefineType = "SCROLL";

	switch (scroll_value)
	{
	case HYUNIRON_CHN:
	{
		bDestroyWhenFail = true;
		szRefineType = "HYUNIRON";
	}
	break;

	case YONGSIN_SCROLL:
	{
		bDestroyWhenFail = true;
		success_prob += scroll_added_percent;
		szRefineType = "YONGSIN";
	}
	break;

	case MUSIN_SCROLL:
	{
		success_prob += scroll_added_percent;
		szRefineType = "MUSIN";
	}
	break;

	default:
		break;
	}

	LPITEM pkItemScrollPtr = pkItemScroll;

	pkItemScrollPtr->SetCount(pkItemScrollPtr->GetCount() - 1);

	pkItemScrollPtr = FindScrollByVnum(scroll_vnum, this);

	if (prob <= success_prob)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE SUCCESS", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			NotifyRefineSuccess(this, item, szRefineType, autoReopen);
			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
			pkNewItem->AttrLog();
			PayRefineFee(prt->cost);

			if (autoReopen)
			{
				if (!(pkNewItem->GetRefinedVnum()) || CountSpecifyItem(scroll_vnum) <= 0)
				{
					CloseRefine();
					return false;
				}

				RefineItem(pkItemScrollPtr, pkNewItem);
			}
		}
		else
		{
#ifdef __ENABLE_REFINE_MSG_ADD__
			NotifyRefineFailType(this, item, REFINE_FAIL_KEEP_GRADE, szRefineType, -1, autoReopen);
#else
			NotifyRefineFail(this, item, szRefineType);
#endif

			if (autoReopen)
			{
				if (CountSpecifyItem(scroll_vnum) <= 0)
				{
					CloseRefine();
					return false;
				}

				RefineItem(pkItemScrollPtr, pkNewItem);
			}
		}
	}
	else if (!bDestroyWhenFail && result_fail_vnum)
	{

		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(result_fail_vnum, 1, 0, false);

		if (pkNewItem)
		{
			ITEM_MANAGER::CopyAllAttrTo(item, pkNewItem);
			LogManager::instance().ItemLog(this, pkNewItem, "REFINE FAIL", pkNewItem->GetName());

			BYTE bCell = item->GetCell();

			DBManager::instance().SendMoneyLog(MONEY_LOG_REFINE, item->GetVnum(), -prt->cost);

#ifdef __ENABLE_REFINE_MSG_ADD__
			NotifyRefineFailType(this, item, REFINE_FAIL_GRADE_DOWN, szRefineType, -1, autoReopen);
#else
			NotifyRefineFail(this, item, szRefineType, -1);
#endif
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE FAIL)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

			pkNewItem->AttrLog();

			PayRefineFee(prt->cost);

			if (autoReopen)
			{
				if (CountSpecifyItem(scroll_vnum) <= 0)
				{
					CloseRefine();
					return false;
				}

				RefineItem(pkItemScrollPtr, pkNewItem);
			}
		}
		else
		{
#ifdef __ENABLE_REFINE_MSG_ADD__
			NotifyRefineFailType(this, item, REFINE_FAIL_KEEP_GRADE, szRefineType, -1, autoReopen);
#else
			NotifyRefineFail(this, item, szRefineType);
#endif

			if (autoReopen)
			{
				if (CountSpecifyItem(scroll_vnum) <= 0)
				{
					CloseRefine();
					return false;
				}

				RefineItem(pkItemScrollPtr, pkNewItem);
			}
		}
	}
	else
	{
#ifdef __ENABLE_REFINE_MSG_ADD__
		NotifyRefineFailType(this, item, REFINE_FAIL_KEEP_GRADE, szRefineType, -1, autoReopen);
#else
		NotifyRefineFail(this, item, szRefineType);
#endif

		PayRefineFee(prt->cost);

		if (autoReopen)
		{
			if (CountSpecifyItem(scroll_vnum) <= 0)
			{
				CloseRefine();
				return false;
			}

			RefineItem(pkItemScrollPtr, item);
		}
	}

	return true;
}

#ifdef __ENABLE_FAST_REFINE_OPTION__
	bool CHARACTER::RefineInformation(CItem* item, uint8_t bType, CItem* additionalItem, bool fast_refine, int addPercentVal)
#else
	bool CHARACTER::RefineInformation(BYTE bCell, BYTE bType, int iAdditionalCell)
#endif
{
	if (!item)
	{
		return false;
	}

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (bCell > INVENTORY_MAX_NUM)
#else
	if (item->GetCell() > INVENTORY_MAX_NUM && !(item->GetCell() >= SPECIAL_STORAGE_START_CELL && item->GetCell() < SPECIAL_STORAGE_END_CELL))
#endif
	{
		return false;
	}

	// 2019.08.20 - Preventing from refining stacked items
	if (item->GetCount() > 1)
	{
		return false;
	}

	// REFINE_COST
	if (bType == REFINE_TYPE_MONEY_ONLY && !GetQuestFlag("demon_tower.can_refine"))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only be rewarded once for the Demon Tower Quest."));
		return false;
	}
	// END_OF_REFINE_COST

	TPacketGCRefineInformation p = {};

#ifdef __ENABLE_SASH_SYSTEM__
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_SASH)
	{
		if (!SashSystemHelper::HasAbsorption(item)) return false;
		auto pProto = ITEM_MANAGER::instance().GetTable(SashSystemHelper::GetAbsorptionBase(item));
		if (pProto && pProto->dwRefinedVnum)
		{
			p.iSashRefine = pProto->dwRefinedVnum;
		}
		else
			return false;
	}
#endif

	p.header = HEADER_GC_REFINE_INFORMATION;
	p.pos = item->GetCell();
	p.src_vnum = item->GetVnum();
#ifdef __ENABLE_SASH_SYSTEM__
	p.result_vnum = p.iSashRefine ? item->GetVnum() : item->GetRefinedVnum();
#else
	p.result_vnum = item->GetRefinedVnum();
#endif
	p.type = bType;
#ifdef __ENABLE_FAST_REFINE_OPTION__
	p.fast_refine = fast_refine;
#endif
	auto addedPercent = 0;
	if (additionalItem)
	{
		addedPercent += additionalItem->GetValue(1);
	}

	addedPercent += addPercentVal;

	p.addedProb = addedPercent;

	if (p.result_vnum == 0)
	{
		sys_err("RefineInformation p.result_vnum == 0");
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	if (item->GetType() == ITEM_USE && item->GetSubType() == USE_TUNING)
	{
		if (bType == 0)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be advanced this way."));
			return false;
		}
		else
		{
			if (!additionalItem || item->GetVnum() == additionalItem->GetVnum())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't combine identical Advancement Scrolls."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can combine the Blessing Scroll with the Magic Iron Ore."));
				return false;
			}
		}
	}

#ifdef __ENABLE_SOUL_SYSTEM__
	if (bType == REFINE_TYPE_SOUL)
	{
		if (!additionalItem)
		{
			return false;
		}

		p.cost = 0;
		p.prob = additionalItem->GetValue(1);
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));

		GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

		SetRefineMode(additionalItem);
		return true;
	}
#endif

	CRefineManager & rm = CRefineManager::instance();

#ifdef __ENABLE_SASH_SYSTEM__
	const TRefineTable* prt = {};

	if (SashSystemHelper::HasAbsorption(item))
	{
		auto pProto = ITEM_MANAGER::instance().GetTable(SashSystemHelper::GetAbsorptionBase(item));
		if (pProto && pProto->dwRefinedVnum)
		{
			prt = rm.GetRefineRecipe(pProto->wRefineSet);
		}
	} else
		prt = rm.GetRefineRecipe(item->GetRefineSet());
#else
	const TRefineTable* prt = rm.GetRefineRecipe(item->GetRefineSet());
#endif

	if (!prt)
	{
		sys_err("RefineInformation NOT GET REFINE SET %d", item->GetRefineSet());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	// REFINE_COST

	//MAIN_QUEST_LV7
	if (GetQuestFlag("main_quest_lv7.refine_chance") > 0)
	{

		if (!item->CheckItemUseLevel(20) || item->GetType() != ITEM_WEAPON)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The free weapon improvements can only be used on weapons up to level 20."));
			return false;
		}
		p.cost = 0;
	}
	else
	{
		p.cost = ComputeRefineFee(prt->cost);
	}

	//END_MAIN_QUEST_LV7
	p.prob = prt->prob;

	if (bType == REFINE_TYPE_MONEY_ONLY)
	{
		p.material_count = 0;
		memset(p.materials, 0, sizeof(p.materials));
	}
	else
	{
		p.material_count = prt->material_count;
		thecore_memcpy(&p.materials, prt->materials, sizeof(prt->materials));
	}
	// END_OF_REFINE_COST

	GetDesc()->Packet(&p, sizeof(TPacketGCRefineInformation));

	SetRefineMode(additionalItem);
	return true;
}

bool CHARACTER::RefineItem(LPITEM pkItem, LPITEM pkTarget)
{
	if (!CanHandleItem())
	{
		return false;
	}

	// 2019.08.20 - Preventing from refining stacked items
	if (pkTarget->GetCount() > 1)
	{
		return false;
	}

	if (pkItem->GetSubType() == USE_TUNING)
	{
		switch (pkItem->GetValue(0))
		{
		case STANDARD_SCROLL:
		case HYUNIRON_CHN:
		case YONGSIN_SCROLL:
		{
			RefineInformation(pkTarget, REFINE_TYPE_SCROLL, pkItem);
#ifdef __ENABLE_FAST_REFINE_OPTION__
			// Using only for fast refine variable.
			SetFastRefineVnum(pkItem->GetVnum());
#endif
		}
		break;

#ifdef __ENABLE_SOUL_SYSTEM__
		case SOUL_SCROLL:
		{
			RefineInformation(pkTarget, REFINE_TYPE_SOUL, pkItem);
#ifdef __ENABLE_FAST_REFINE_OPTION__
			// Using only for fast refine variable.
			SetFastRefineVnum(pkItem->GetVnum());
#endif
		}
		break;
#endif

		default:
		{
			RefineInformation(pkTarget, REFINE_TYPE_SCROLL, pkItem);
#ifdef __ENABLE_FAST_REFINE_OPTION__
			// Using only for fast refine variable.
			SetFastRefineVnum(pkItem->GetVnum());
#endif
		}
		break;
		}
	}
	else if (pkItem->GetSubType() == USE_DETACHMENT && IS_SET(pkTarget->GetFlag(), ITEM_FLAG_REFINEABLE))
	{
		LogManager::instance().ItemLog(this, pkTarget, "USE_DETACHMENT", pkTarget->GetName());

		bool bHasMetinStone = false;

		for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
		{
			long socket = pkTarget->GetSocket(i);
			if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
			{
				bHasMetinStone = true;
				break;
			}
		}

		if (bHasMetinStone)
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				long socket = pkTarget->GetSocket(i);
				if (socket > 2 && socket != ITEM_BROKEN_METIN_VNUM)
				{
					AutoGiveItem(socket);
					//TItemTable* pTable = ITEM_MANAGER::instance().GetTable(pkTarget->GetSocket(i));
					//pkTarget->SetSocket(i, pTable->alValues[2]);

					pkTarget->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
				}
			}
			pkItem->SetCount(pkItem->GetCount() - 1);
			return true;
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no Stone available to take out."));
			return false;
		}
	}

	return false;
}

EVENTFUNC(kill_campfire_event)
{
	char_event_info* info = dynamic_cast<char_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "kill_campfire_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER	ch = info->ch;

	if (ch == NULL)   // <Factor>
	{
		return 0;
	}
	ch->m_pkMiningEvent = NULL;
	M2_DESTROY_CHARACTER(ch);
	return 0;
}

bool CHARACTER::GiveRecallItem(LPITEM item)
{
	int idx = GetMapIndex();
	int iEmpireByMapIndex = -1;

	if (idx < 20)
	{
		iEmpireByMapIndex = 1;
	}
	else if (idx < 40)
	{
		iEmpireByMapIndex = 2;
	}
	else if (idx < 60)
	{
		iEmpireByMapIndex = 3;
	}
	else if (idx < 10000)
	{
		iEmpireByMapIndex = 0;
	}

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot store this location."));
		return false;
	}

	int pos;

	if (item->GetCount() == 1)
	{
		item->SetSocket(0, GetX());
		item->SetSocket(1, GetY());
	}
#ifndef __SPECIAL_STORAGE_ENABLE__
	else if ((pos = GetEmptyInventory(item->GetSize())) != -1)
#else
	else if ((pos = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow())) != -1)
#endif
	{
		LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), 1);

		if (NULL != item2)
		{
			item2->SetSocket(0, GetX());
			item2->SetSocket(1, GetY());
			item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

			item->SetCount(item->GetCount() - 1);
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
		return false;
	}

	return true;
}

void CHARACTER::ProcessRecallItem(LPITEM item)
{
	int idx;

	if ((idx = SECTREE_MANAGER::instance().GetMapIndex(item->GetSocket(0), item->GetSocket(1))) == 0)
	{
		return;
	}

	int iEmpireByMapIndex = -1;

	if (idx < 20)
	{
		iEmpireByMapIndex = 1;
	}
	else if (idx < 40)
	{
		iEmpireByMapIndex = 2;
	}
	else if (idx < 60)
	{
		iEmpireByMapIndex = 3;
	}
	else if (idx < 10000)
	{
		iEmpireByMapIndex = 0;
	}

	switch (idx)
	{
	case 66:
	case 216:
		iEmpireByMapIndex = -1;
		break;

	case 301:
	case 302:
	case 303:
	case 304:
		if ( GetLevel() < 90 )
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to use this item."));
			return;
		}
		else
		{
			break;
		}
	}

	if (iEmpireByMapIndex && GetEmpire() != iEmpireByMapIndex)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot teleport to a safe position in a foreign Kingdom."));
		item->SetSocket(0, 0);
		item->SetSocket(1, 0);
	}
	else
	{
		sys_log(1, "Recall: %s %d %d -> %d %d", GetName(), GetX(), GetY(), item->GetSocket(0), item->GetSocket(1));
		WarpSet(item->GetSocket(0), item->GetSocket(1));
		item->SetCount(item->GetCount() - 1);
	}
}

void CHARACTER::__OpenPrivateShop()
{
#ifdef ENABLE_OPEN_SHOP_WITH_ARMOR
	ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
#else
	unsigned bodyPart = GetPart(PART_MAIN);
	switch (bodyPart)
	{
	case 0:
	case 1:
	case 2:
		ChatPacket(CHAT_TYPE_COMMAND, "OpenPrivateShop");
		break;
	default:
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only open the shop if you take off your armour."));
		break;
	}
#endif
}

// MYSHOP_PRICE_LIST
#ifdef __EXTANDED_GOLD_AMOUNT__
	void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, int64_t dwItemPrice)
#else
	void CHARACTER::SendMyShopPriceListCmd(DWORD dwItemVnum, DWORD dwItemPrice)
#endif
{
	char szLine[256];
#ifdef __EXTANDED_GOLD_AMOUNT__
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %lld", dwItemVnum, dwItemPrice);
#else
	snprintf(szLine, sizeof(szLine), "MyShopPriceList %u %u", dwItemVnum, dwItemPrice);
#endif
	ChatPacket(CHAT_TYPE_COMMAND, szLine);
	sys_log(0, szLine);
}

void CHARACTER::UseSilkBotaryReal(const TPacketMyshopPricelistHeader* p)
{
	const TItemPriceInfo* pInfo = (const TItemPriceInfo*)(p + 1);

	if (!p->byCount)

	{
		SendMyShopPriceListCmd(1, 0);
	}
	else
	{
		for (int idx = 0; idx < p->byCount; idx++)
		{
			SendMyShopPriceListCmd(pInfo[ idx ].dwVnum, pInfo[ idx ].dwPrice);
		}
	}

	__OpenPrivateShop();
}

//


//
void CHARACTER::UseSilkBotary(void)
{
	if (m_bNoOpenedShop)
	{
		DWORD dwPlayerID = GetPlayerID();
		db_clientdesc->DBPacket(HEADER_GD_MYSHOP_PRICELIST_REQ, GetDesc()->GetHandle(), &dwPlayerID, sizeof(DWORD));
		m_bNoOpenedShop = false;
	}
	else
	{
		__OpenPrivateShop();
	}
}
// END_OF_MYSHOP_PRICE_LIST


int CalculateConsume(LPCHARACTER ch)
{
	static const int WARP_NEED_LIFE_PERCENT	= 30;
	static const int WARP_MIN_LIFE_PERCENT	= 10;
	// CONSUME_LIFE_WHEN_USE_WARP_ITEM
	int consumeLife = 0;
	{
		// CheckNeedLifeForWarp
		const int curLife		= ch->GetHP();
		const int needPercent	= WARP_NEED_LIFE_PERCENT;
		const int needLife = ch->GetMaxHP() * needPercent / 100;
		if (curLife < needLife)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough HP."));
			return -1;
		}

		consumeLife = needLife;



		const int minPercent	= WARP_MIN_LIFE_PERCENT;
		const int minLife	= ch->GetMaxHP() * minPercent / 100;
		if (curLife - needLife < minLife)
		{
			consumeLife = curLife - minLife;
		}

		if (consumeLife < 0)
		{
			consumeLife = 0;
		}
	}
	// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM
	return consumeLife;
}

int CalculateConsumeSP(LPCHARACTER lpChar)
{
	static const int NEED_WARP_SP_PERCENT = 30;

	const int curSP = lpChar->GetSP();
	const int needSP = lpChar->GetMaxSP() * NEED_WARP_SP_PERCENT / 100;

	if (curSP < needSP)
	{
		lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough Spell Points (SP) to use this."));
		return -1;
	}

	return needSP;
}

// #define ENABLE_FIREWORK_STUN
#define ENABLE_ADDSTONE_FAILURE
bool CHARACTER::UseItemEx(LPITEM item, TItemPos DestCell)
{
	int iLimitRealtimeStartFirstUseFlagIndex = -1;
	//int iLimitTimerBasedOnWearFlagIndex = -1;

	WORD wDestCell = DestCell.cell;
	BYTE bDestInven = DestCell.window_type;
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limitValue = item->GetProto()->aLimits[i].lValue;

		switch (item->GetProto()->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (GetLevel() < limitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to use this item."));
				return false;
			}
			break;

		case LIMIT_REAL_TIME_START_FIRST_USE:
			iLimitRealtimeStartFirstUseFlagIndex = i;
			break;

		case LIMIT_TIMER_BASED_ON_WEAR:
			//iLimitTimerBasedOnWearFlagIndex = i;
			break;
		}
	}

	if (test_server)
	{
		sys_log(0, "USE_ITEM %s, Inven %d, Cell %d, ItemType %d, SubType %d", item->GetName(), bDestInven, wDestCell, item->GetType(), item->GetSubType());
	}

	if ( CArenaManager::instance().IsLimitedItem( GetMapIndex(), item->GetVnum() ) == true )
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
		return false;
	}
#ifdef __ENABLE_NEWSTUFF__
	else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && IsLimitedPotionOnPVP(item->GetVnum()))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
		return false;
	}
#endif

	// @fixme402 (IsLoadedAffect to block affect hacking)
	if (!IsLoadedAffect())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Affects are not loaded yet!"));
		return false;
	}

	// @fixme141 BEGIN
	if (TItemPos(item->GetWindow(), item->GetCell()).IsBeltInventoryPosition())
	{
		LPITEM beltItem = GetWear(WEAR_BELT);

		if (NULL == beltItem)
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you have no equipped belt.");
			return false;
		}

		if (false == CBeltInventoryHelper::IsAvailableCell(item->GetCell() - BELT_INVENTORY_SLOT_START, beltItem->GetValue(0)))
		{
			ChatPacket(CHAT_TYPE_INFO, "<Belt> You can't use this item if you don't upgrade your belt.");
			return false;
		}
	}
	// @fixme141 END


	if (-1 != iLimitRealtimeStartFirstUseFlagIndex)
	{
		item->OnRealTimeFirstUse();
	}

	switch (item->GetType())
	{
	case ITEM_HAIR:
		return ItemProcess_Hair(item, wDestCell);

	case ITEM_POLYMORPH:
		return ItemProcess_Polymorph(item);

	case ITEM_QUEST:
		if (GetArena() != NULL || IsObserverMode() == true)
		{
			if (item->GetVnum() == 50051 || item->GetVnum() == 50052 || item->GetVnum() == 50053)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
		}

		if (!IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_USE | ITEM_FLAG_QUEST_USE_MULTIPLE))
		{
			if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
			{
				return false;
			}

			if (item->GetSIGVnum() == 0)
			{
				quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
			}
			else
			{
				quest::CQuestManager::instance().SIGUse(GetPlayerID(), item->GetSIGVnum(), item, false);
			}
		}
		break;

	case ITEM_CAMPFIRE:
	{
		float fx, fy;
		GetDeltaByDegree(GetRotation(), 100.0f, &fx, &fy);

		LPSECTREE tree = SECTREE_MANAGER::instance().Get(GetMapIndex(), (long)(GetX() + fx), (long)(GetY() + fy));

		if (!tree)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot build a campfire here."));
			return false;
		}

		if (tree->IsAttr((long)(GetX() + fx), (long)(GetY() + fy), ATTR_WATER))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot build a campfire under water."));
			return false;
		}

		LPCHARACTER campfire = CHARACTER_MANAGER::instance().SpawnMob(fishing::CAMPFIRE_MOB, GetMapIndex(), (long)(GetX() + fx), (long)(GetY() + fy), 0, false, number(0, 359));

		char_event_info* info = AllocEventInfo<char_event_info>();

		info->ch = campfire;

		campfire->m_pkMiningEvent = event_create(kill_campfire_event, info, PASSES_PER_SEC(40));

		item->SetCount(item->GetCount() - 1);
	}
	break;

	case ITEM_UNIQUE:
	{
		switch (item->GetSubType())
		{
		case USE_ABILITY_UP:
		{
			switch (item->GetValue(0))
			{
			case APPLY_MOV_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true, true);
				break;

			case APPLY_STR:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEX:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CON:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_INT:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_CAST_SPEED:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_RESIST_MAGIC:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_RESIST_MAGIC, item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_ATT_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_ATT_GRADE_BONUS,
						  item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;

			case APPLY_DEF_GRADE_BONUS:
				AddAffect(AFFECT_UNIQUE_ABILITY, POINT_DEF_GRADE_BONUS,
						  item->GetValue(2), 0, item->GetValue(1), 0, true, true);
				break;
			}
		}

		if (GetDungeon())
		{
			GetDungeon()->UsePotion(this);
		}

		if (GetWarMap())
		{
			GetWarMap()->UsePotion(this, item);
		}

		item->SetCount(item->GetCount() - 1);
		break;

		default:
		{
			if (item->GetSubType() == USE_SPECIAL)
			{
				sys_log(0, "ITEM_UNIQUE: USE_SPECIAL %u", item->GetVnum());

				switch (item->GetVnum())
				{
				case 71049:
					if (g_bEnableBootaryCheck)
					{
						if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
						{
							UseSilkBotary();
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ������ �� �� ���� �����Դϴ�"));
						}
					}
					else
					{
						UseSilkBotary();
					}
					break;
				}
			}
			else
			{
				if (!item->IsEquipped())
				{
					EquipItem(item);
				}
				else
				{
					UnequipItem(item);
				}
			}
		}
		break;
		}
	}
	break;

	case ITEM_COSTUME:
	case ITEM_WEAPON:
	case ITEM_ARMOR:
	case ITEM_ROD:
	case ITEM_RING:
	case ITEM_BELT:
	case ITEM_PICK:
#ifdef __ENABLE_AMULET_SYSTEM__
	case ITEM_AMULET:
#endif
	{
		if (!item->IsEquipped())
		{
			EquipItem(item);
		}
		else
		{
			UnequipItem(item);
		}
		break;
	}



	case ITEM_DS:
	{
		if (!item->IsEquipped())
		{
			return false;
		}
		return DSManager::instance().PullOut(this, NPOS, item);
		break;
	}
	case ITEM_SPECIAL_DS:
		if (!item->IsEquipped())
		{
			EquipItem(item);
		}
		else
		{
			UnequipItem(item);
		}
		break;

	case ITEM_FISH:
	{
		if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
			return false;
		}
#ifdef __ENABLE_NEWSTUFF__
		else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
			return false;
		}
#endif

		if (item->GetSubType() == FISH_ALIVE)
		{
			CFishing::instance().UseFish(this, item);
		}
	}
	break;

	case ITEM_TREASURE_BOX:
	{
		return false;

	}
	break;

	case ITEM_TREASURE_KEY:
	{
		LPITEM item2;

		if (!GetItem(DestCell) || !(item2 = GetItem(DestCell)))
		{
			return false;
		}

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
		{
			return false;
		}

		if (item2->GetType() != ITEM_TREASURE_BOX)
		{
			ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This item cannot be opened with a key."));
			return false;
		}

		if (item->GetValue(0) == item2->GetValue(0))
		{

			DWORD dwBoxVnum = item2->GetVnum();
			std::vector <DWORD> dwVnums;
			std::vector <DWORD> dwCounts;
			std::vector <LPITEM> item_gets(0);
			int count = 0;

			if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
			{
				ITEM_MANAGER::instance().RemoveItem(item);
				ITEM_MANAGER::instance().RemoveItem(item2);

				for (int i = 0; i < count; i++)
				{
					switch (dwVnums[i])
					{
					case CSpecialItemGroup::GOLD:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
						break;
					case CSpecialItemGroup::EXP:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A mysterious light comes out of the box."));
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
						break;
					case CSpecialItemGroup::MOB:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
						break;
					case CSpecialItemGroup::SLOW:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
						break;
					case CSpecialItemGroup::DRAIN_HP:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
						break;
					case CSpecialItemGroup::POISON:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
						break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
					case CSpecialItemGroup::BLEEDING:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
						break;
#endif
					case CSpecialItemGroup::MOB_GROUP:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
						break;
					default:
						if (item_gets[i])
						{
							if (dwCounts[i] > 1)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Receive: %s - %d"), item_gets[i]->GetName(), dwCounts[i]);
							}
							else
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box contains %s."), item_gets[i]->GetName());
							}
						}
					}
				}
			}
			else
			{
				ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This key does not seem to fit the lock."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("This key does not seem to fit the lock."));
			return false;
		}
	}
	break;

	case ITEM_GIFTBOX:
	{
#ifdef __ENABLE_NEWSTUFF__
		if (0 != g_BoxUseTimeLimitValue)
		{
			if (get_dword_time() < m_dwLastBoxUseTime + g_BoxUseTimeLimitValue)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("���� ��带 ���� �� �����ϴ�."));
				return false;
			}
		}

		m_dwLastBoxUseTime = get_dword_time();
#endif
		DWORD dwBoxVnum = item->GetVnum();
		std::vector <DWORD> dwVnums;
		std::vector <DWORD> dwCounts;
		std::vector <LPITEM> item_gets(0);
		int count = 0;

		BYTE size = item->GetSize();
		BYTE window = item->GetVirtualWindow();
		int cell = GetEmptyInventory(size, window);

		if (cell == -1)
		{
			return false;
		}

		if ( dwBoxVnum > 51500 && dwBoxVnum < 52000 )
		{
			if ( !(this->DragonSoul_IsQualified()) )
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Before you open the Cor Draconis, you have to complete the Dragon Stone quest and activate the Dragon Stone Alchemy."));
				return false;
			}
		}

		if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
		{
			item->SetCount(item->GetCount() - 1);

			for (int i = 0; i < count; i++)
			{
				switch (dwVnums[i])
				{
				case CSpecialItemGroup::GOLD:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
					break;
				case CSpecialItemGroup::EXP:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A mysterious light comes out of the box."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
					break;
				case CSpecialItemGroup::MOB:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
					break;
				case CSpecialItemGroup::SLOW:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
					break;
				case CSpecialItemGroup::DRAIN_HP:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
					break;
				case CSpecialItemGroup::POISON:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
					break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
				case CSpecialItemGroup::BLEEDING:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
					break;
#endif
				case CSpecialItemGroup::MOB_GROUP:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
					break;
				default:
					if (item_gets[i])
					{
#ifndef __ENABLE_NEW_LOGS_CHAT__
						if (dwCounts[i] > 1)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Receive: %s - %d"), item_gets[i]->GetName(), dwCounts[i]);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box contains %s."), item_gets[i]->GetName());
						}
#endif
					}
				}
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You have not received anything."));
			return false;
		}
	}
	break;

	case ITEM_SKILLFORGET:
	{
		if (!item->GetSocket(0))
		{
			item->SetCount(item->GetCount() - 1);
			return false;
		}

		DWORD dwVnum = item->GetSocket(0);

		if (SkillLevelDown(dwVnum))
		{
			item->SetCount(item->GetCount() - 1);
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have lowered your Skill Level."));
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot lower your Skill Level."));
		}
	}
	break;

	case ITEM_SKILLBOOK:
	{
		if (IsPolymorphed())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
			return false;
		}

		DWORD dwVnum = 0;

		if (item->GetVnum() == 50300)
		{
			dwVnum = item->GetSocket(0);
		}
		else
		{

			dwVnum = item->GetValue(0);
		}

		if (0 == dwVnum)
		{
			item->SetCount(item->GetCount() - 1);

			return false;
		}

		if (true == LearnSkillByBook(dwVnum))
		{
#ifdef ENABLE_BOOKS_STACKFIX
			item->SetCount(item->GetCount() - 1);
#else
			ITEM_MANAGER::instance().RemoveItem(item);
#endif

			int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);

			if (distribution_test_server)
			{
				iReadDelay /= 3;
			}

			SetSkillNextReadTime(dwVnum, get_global_time() + iReadDelay);
#ifdef __ENABLE_SKILLS_INFORMATION__
			BroadcastSkillInformation();
#endif
		}
	}
	break;

	case ITEM_USE:
	{
		if (item->GetVnum() > 50800 && item->GetVnum() <= 50820)
		{
			if (test_server)
			{
				sys_log (0, "ADD addtional effect : vnum(%d) subtype(%d)", item->GetOriginalVnum(), item->GetSubType());
			}

			int affect_type = AFFECT_EXP_BONUS_EURO_FREE;
			int apply_type = aApplyInfo[item->GetValue(0)].bPointType;
			int apply_value = item->GetValue(2);
			int apply_duration = item->GetValue(1);

			switch (item->GetSubType())
			{
			case USE_ABILITY_UP:
				if (FindAffect(affect_type, apply_type))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
					return false;
				}

				{
					switch (item->GetValue(0))
					{
					case APPLY_MOV_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_MOV_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_ATT_SPEED:
						AddAffect(affect_type, apply_type, apply_value, AFF_ATT_SPEED_POTION, apply_duration, 0, true, true);
						break;

					case APPLY_STR:
					case APPLY_DEX:
					case APPLY_CON:
					case APPLY_INT:
					case APPLY_CAST_SPEED:
					case APPLY_RESIST_MAGIC:
					case APPLY_ATT_GRADE_BONUS:
					case APPLY_DEF_GRADE_BONUS:
						AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, true, true);
						break;
					}
				}

				if (GetDungeon())
				{
					GetDungeon()->UsePotion(this);
				}

				if (GetWarMap())
				{
					GetWarMap()->UsePotion(this, item);
				}

				item->SetCount(item->GetCount() - 1);
				break;

			case USE_AFFECT :
			{
				if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				}
				else
				{
					// PC_BANG_ITEM_ADD
					if (item->IsPCBangItem() == true)
					{

						if (CPCBangManager::instance().IsPCBangIP(GetDesc()->GetHostName()) == false)
						{

							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used in an Internet cafe."));
							return false;
						}
					}
					// END_PC_BANG_ITEM_ADD

					AddAffect(AFFECT_EXP_BONUS_EURO_FREE, aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false, true);
					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case USE_POTION_NODELAY:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
						return false;
					}

					switch (item->GetVnum())
					{
					case 70020 :
					case 71018 :
					case 71019 :
					case 71020 :
						if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
						{
							if (m_nPotionLimit <= 0)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
								return false;
							}
						}
						break;

					default :
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
						return false;
						break;
					}
				}
#ifdef __ENABLE_NEWSTUFF__
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#endif

				bool used = false;

				if (item->GetValue(0) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(1) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (item->GetValue(3) != 0)
				{
					if (GetHP() < GetMaxHP())
					{
						PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
						EffectPacket(SE_HPUP_RED);
						used = TRUE;
					}
				}

				if (item->GetValue(4) != 0)
				{
					if (GetSP() < GetMaxSP())
					{
						PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
						EffectPacket(SE_SPUP_BLUE);
						used = TRUE;
					}
				}

				if (used)
				{
					if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
					{
						if (test_server)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Used Moon Cake or Seed."));
						}
						SetUseSeedOrMoonBottleTime();
					}
					if (GetDungeon())
					{
						GetDungeon()->UsePotion(this);
					}

					if (GetWarMap())
					{
						GetWarMap()->UsePotion(this, item);
					}

					m_nPotionLimit--;

					//RESTRICT_USE_SEED_OR_MOONBOTTLE
					item->SetCount(item->GetCount() - 1);
					//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
				}
			}
			break;
			}

			return true;
		}


		if (item->GetVnum() >= 27863 && item->GetVnum() <= 27883)
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#ifdef __ENABLE_NEWSTUFF__
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#endif
		}

		if (test_server)
		{
			sys_log (0, "USE_ITEM %s Type %d SubType %d vnum %d", item->GetName(), item->GetType(), item->GetSubType(), item->GetOriginalVnum());
		}

		switch (item->GetSubType())
		{
		case USE_TIME_CHARGE_PER:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret;
				char buf[128];
				if (item->GetVnum() == DRAGON_HEART_VNUM)
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
				}
				else
				{
					ret = pDestItem->GiveMoreTime_Per((float)item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				}
				if (ret > 0)
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "Inc %ds by item{VN:%d SOC%d:%ld}", ret, item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "Inc %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You charing dragon soul successfull."), ret);
					item->SetCount(item->GetCount() - 1);
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					return true;
				}
				else
				{
					if (item->GetVnum() == DRAGON_HEART_VNUM)
					{
						sprintf(buf, "No change by item{VN:%d SOC%d:%ld}", item->GetVnum(), ITEM_SOCKET_CHARGING_AMOUNT_IDX, item->GetSocket(ITEM_SOCKET_CHARGING_AMOUNT_IDX));
					}
					else
					{
						sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					}

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You charing dragon soul failed."), ret);
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		break;
		case USE_TIME_CHARGE_FIX:
		{
			LPITEM pDestItem = GetItem(DestCell);
			if (NULL == pDestItem)
			{
				return false;
			}

			if (pDestItem->IsDragonSoul())
			{
				int ret = pDestItem->GiveMoreTime_Fix(item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
				char buf[128];
				if (ret)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d�� ��ŭ �����Ǿ����ϴ�."), ret);
					sprintf(buf, "Increase %ds by item{VN:%d VAL%d:%ld}", ret, item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_SUCCESS", buf);
					item->SetCount(item->GetCount() - 1);
					return true;
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("������ �� �����ϴ�."));
					sprintf(buf, "No change by item{VN:%d VAL%d:%ld}", item->GetVnum(), ITEM_VALUE_CHARGING_AMOUNT_IDX, item->GetValue(ITEM_VALUE_CHARGING_AMOUNT_IDX));
					LogManager::instance().ItemLog(this, item, "DS_CHARGING_FAILED", buf);
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		break;

#ifdef __ENABLE_REFINE_ELEMENT__
		case USE_ELEMENT_UPGRADE:
		{
			LPITEM pDestItem;
			if (!IsValidItemPosition(DestCell) || !(pDestItem = GetItem(DestCell)))
			{
				return false;
			}

			if (pDestItem->IsExchanging() || pDestItem->IsEquipped())
			{
				return false;
			}

			if (item->GetValue(0) <= REFINE_ELEMENT_CATEGORY_NONE || item->GetValue(0) >= REFINE_ELEMENT_CATEGORY_MAX)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used with this item."));
				return false;
			}

			if (pDestItem->GetType() != ITEM_WEAPON)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used with this item."));
				return false;
			}

			if (pDestItem->GetRefineLevel() < ELEMENT_MIN_REFINE_LEVEL)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The elemental enchantment is only available for +7 weapons or higher."));
				return false;
			}

			if (pDestItem->GetRefineElementPlus() == REFINE_ELEMENT_MAX)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The maximum elemental enchantment level has already been reached."));
				return false;
			}

			if (pDestItem->GetRefineElementType() > 0 && pDestItem->GetRefineElementType() != item->GetValue(0))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The item already has another element."));
				return false;
			}

			if (GetGold() < REFINE_ELEMENT_UPGRADE_YANG)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang."));
				return false;
			}

			if (!CanHandleItem())
			{
				return false;
			}

			RefineElementInformation(item->GetCell(), pDestItem->GetCell(), REFINE_ELEMENT_TYPE_UPGRADE);
			return true;
		}
		break;

		case USE_ELEMENT_DOWNGRADE:
		{
			LPITEM pDestItem;
			if (!IsValidItemPosition(DestCell) || !(pDestItem = GetItem(DestCell)))
			{
				return false;
			}

			if (pDestItem->IsExchanging() || pDestItem->IsEquipped())
			{
				return false;
			}

			if (pDestItem->GetType() != ITEM_WEAPON)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used with this item."));
				return false;
			}

			if (!pDestItem->GetRefineElementPlus())
			{
				return false;
			}

			if (GetGold() < REFINE_ELEMENT_DOWNGRADE_YANG)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang."));
				return false;
			}

			if (!CanHandleItem())
			{
				return false;
			}

			RefineElementInformation(item->GetCell(), pDestItem->GetCell(), REFINE_ELEMENT_TYPE_DOWNGRADE);
			return true;
		}
		break;

		case USE_ELEMENT_CHANGE:
		{
			LPITEM pDestItem;
			if (!IsValidItemPosition(DestCell) || !(pDestItem = GetItem(DestCell)))
			{
				return false;
			}

			if (pDestItem->IsExchanging() || pDestItem->IsEquipped())
			{
				return false;
			}

			if (pDestItem->GetType() != ITEM_WEAPON)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot be used with this item."));
				return false;
			}

			if (!pDestItem->GetRefineElementPlus())
			{
				return false;
			}

			if (GetGold() < REFINE_ELEMENT_CHANGE_YANG)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough Yang."));
				return false;
			}

			if (!CanHandleItem())
			{
				return false;
			}

			RefineElementInformation(item->GetCell(), pDestItem->GetCell(), REFINE_ELEMENT_TYPE_CHANGE);
			return true;
		}
		break;
#endif

		case USE_SPECIAL:

			switch (item->GetVnum())
			{
			case ITEM_NOG_POCKET:
			{

				if (FindAffect(AFFECT_NOG_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
					return false;
				}
				long time = item->GetValue(0);
				long moveSpeedPer	= item->GetValue(1);
				long attPer	= item->GetValue(2);
				long expPer			= item->GetValue(3);
				AddAffect(AFFECT_NOG_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
				AddAffect(AFFECT_NOG_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;


			case ITEM_RAMADAN_CANDY:
			{

				// @fixme147 BEGIN
				if (FindAffect(AFFECT_RAMADAN_ABILITY))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
					return false;
				}
				// @fixme147 END
				long time = item->GetValue(0);
				long moveSpeedPer	= item->GetValue(1);
				long attPer	= item->GetValue(2);
				long expPer			= item->GetValue(3);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MOV_SPEED, moveSpeedPer, AFF_MOV_SPEED_POTION, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_ATTBONUS, attPer, AFF_NONE, time, 0, true, true);
				AddAffect(AFFECT_RAMADAN_ABILITY, POINT_MALL_EXPBONUS, expPer, AFF_NONE, time, 0, true, true);
				item->SetCount(item->GetCount() - 1);
			}
			break;
			case ITEM_MARRIAGE_RING:
			{
				marriage::TMarriage* pMarriage = marriage::CManager::instance().Get(GetPlayerID());
				if (pMarriage)
				{
					if (pMarriage->ch1 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch1->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
							break;
						}
					}

					if (pMarriage->ch2 != NULL)
					{
						if (CArenaManager::instance().IsArenaMap(pMarriage->ch2->GetMapIndex()) == true)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
							break;
						}
					}

					int consumeSP = CalculateConsumeSP(this);

					if (consumeSP < 0)
					{
						return false;
					}

					PointChange(POINT_SP, -consumeSP, false);

					WarpToPID(pMarriage->GetOther(GetPlayerID()));
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot wear a Wedding Ring if you are not married."));
				}
			}
			break;


			case UNIQUE_ITEM_CAPE_OF_COURAGE:

			case 70057:
			case REWARD_BOX_UNIQUE_ITEM_CAPE_OF_COURAGE:
#ifdef __NEW_AGGREGATE_MONSTERS__
			{
				DWORD Lv = GetLevel();
				bool bEffect = false;
				int64_t dwPrice = 1000;

				if (Lv >= 30)
				{
					if (GetGold() >= dwPrice)
					{
						PointChange(POINT_GOLD, -dwPrice);
#ifdef __BATTLE_PASS_ENABLE__
						CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(dwPrice)});
#endif
						AggregateMonster();
						bEffect = true;
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need %lld gold to use this item."), dwPrice);
					}
				}
				else
				{
					AggregateMonster();
					bEffect = true;
				}

				if (bEffect)
				{
					if (GetCapeEffectPulse() + 100 > thecore_pulse())
					{
						break;
					}

					SpecificEffectPacket("d:/ymir work/effect/etc/buff/bravery_cape.mse");
					SetCapeEffectPulse(thecore_pulse());
				}
			}
#else
			AggregateMonster();
			item->SetCount(item->GetCount() - 1);
#endif
			break;

			case UNIQUE_ITEM_WHITE_FLAG:
				ForgetMyAttacker();
				item->SetCount(item->GetCount() - 1);
				break;

			case UNIQUE_ITEM_TREASURE_BOX:
				break;

			case 30093:
			case 30094:
			case 30095:
			case 30096:

			{
				const int MAX_BAG_INFO = 26;
				static struct LuckyBagInfo
				{
					DWORD count;
					int prob;
					DWORD vnum;
				} b1[MAX_BAG_INFO] =
				{
					{ 1000,	302,	1 },
					{ 10,	150,	27002 },
					{ 10,	75,	27003 },
					{ 10,	100,	27005 },
					{ 10,	50,	27006 },
					{ 10,	80,	27001 },
					{ 10,	50,	27002 },
					{ 10,	80,	27004 },
					{ 10,	50,	27005 },
					{ 1,	10,	50300 },
					{ 1,	6,	92 },
					{ 1,	2,	132 },
					{ 1,	6,	1052 },
					{ 1,	2,	1092 },
					{ 1,	6,	2082 },
					{ 1,	2,	2122 },
					{ 1,	6,	3082 },
					{ 1,	2,	3122 },
					{ 1,	6,	5052 },
					{ 1,	2,	5082 },
					{ 1,	6,	7082 },
					{ 1,	2,	7122 },
					{ 1,	1,	11282 },
					{ 1,	1,	11482 },
					{ 1,	1,	11682 },
					{ 1,	1,	11882 },
				};

				LuckyBagInfo * bi = NULL;
				bi = b1;

				int pct = number(1, 1000);

				int i;
				for (i = 0; i < MAX_BAG_INFO; i++)
				{
					if (pct <= bi[i].prob)
					{
						break;
					}
					pct -= bi[i].prob;
				}
				if (i >= MAX_BAG_INFO)
				{
					return false;
				}

				if (bi[i].vnum == 50300)
				{

					GiveRandomSkillBook();
				}
				else if (bi[i].vnum == 1)
				{
					PointChange(POINT_GOLD, 1000, true);
				}
				else
				{
					AutoGiveItem(bi[i].vnum, bi[i].count);
				}
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50004:
			{
				if (item->GetSocket(0))
				{
					item->SetSocket(0, item->GetSocket(0) + 1);
				}
				else
				{

					int iMapIndex = GetMapIndex();

					PIXEL_POSITION pos;

					if (SECTREE_MANAGER::instance().GetRandomLocation(iMapIndex, pos, 700))
					{
						item->SetSocket(0, 1);
						item->SetSocket(1, pos.x);
						item->SetSocket(2, pos.y);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Event Detector from this position."));
						return false;
					}
				}

				int dist = 0;
				float distance = (DISTANCE_SQRT(GetX() - item->GetSocket(1), GetY() - item->GetSocket(2)));

				if (distance < 1000.0f)
				{

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Event Detector vanished in a mysterious light."));


					struct TEventStoneInfo
					{
						DWORD dwVnum;
						int count;
						int prob;
					};
					const int EVENT_STONE_MAX_INFO = 15;
					TEventStoneInfo info_10[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  8 },
						{ 27004, 10,  6 },
						{ 27002, 10, 12 },
						{ 27005, 10, 12 },
						{ 27100,  1,  9 },
						{ 27103,  1,  9 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 12 },

						{ 25040,  1,  4 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1,  0 },

						{ 50001,  1,  0 },
						{ 50003,  1,  1 },
					};
					TEventStoneInfo info_7[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  1 },
						{ 27004, 10,  1 },
						{ 27004, 10,  9 },
						{ 27005, 10,  9 },
						{ 27100,  1,  5 },
						{ 27103,  1,  5 },
						{ 27101,  1, 10 },
						{ 27104,  1, 10 },
						{ 27999,  1, 14 },

						{ 25040,  1,  5 },

						{ 27410,  1,  5 },
						{ 27600,  1,  5 },
						{ 25100,  1,  5 },

						{ 50001,  1,  0 },
						{ 50003,  1,  5 },

					};
					TEventStoneInfo info_4[EVENT_STONE_MAX_INFO] =
					{
						{ 27001, 10,  0 },
						{ 27004, 10,  0 },
						{ 27002, 10,  0 },
						{ 27005, 10,  0 },
						{ 27100,  1,  0 },
						{ 27103,  1,  0 },
						{ 27101,  1,  0 },
						{ 27104,  1,  0 },
						{ 27999,  1, 25 },

						{ 25040,  1,  0 },

						{ 27410,  1,  0 },
						{ 27600,  1,  0 },
						{ 25100,  1, 15 },

						{ 50001,  1, 10 },
						{ 50003,  1, 50 },

					};

					{
						TEventStoneInfo* info;
						if (item->GetSocket(0) <= 4)
						{
							info = info_4;
						}
						else if (item->GetSocket(0) <= 7)
						{
							info = info_7;
						}
						else
						{
							info = info_10;
						}

						int prob = number(1, 100);

						for (int i = 0; i < EVENT_STONE_MAX_INFO; ++i)
						{
							if (!info[i].prob)
							{
								continue;
							}

							if (prob <= info[i].prob)
							{
								if (info[i].dwVnum == 50001)
								{
									DWORD * pdw = M2_NEW DWORD[2];

									pdw[0] = info[i].dwVnum;
									pdw[1] = info[i].count;


									DBManager::instance().ReturnQuery(QID_LOTTO, GetPlayerID(), pdw,
																	  "INSERT INTO lotto_list VALUES(0, 'server%s', %u, NOW())",
																	  get_table_postfix(), GetPlayerID());
								}
								else
								{
									AutoGiveItem(info[i].dwVnum, info[i].count);
								}

								break;
							}
							prob -= info[i].prob;
						}
					}

					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf), "StoneDetect %u 0 0", (DWORD)GetVID());

					if (len < 0 || len >= (int) sizeof(chatbuf))
					{
						len = sizeof(chatbuf) - 1;
					}

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header	= HEADER_GC_CHAT;
					pack_chat.size		= sizeof(TPacketGCChat) + len;
					pack_chat.type		= CHAT_TYPE_COMMAND;
					pack_chat.id		= 0;
					pack_chat.bEmpire	= GetDesc()->GetEmpire();
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
					pack_chat.locale = GetLocale();
#endif
					//pack_chat.id	= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());

					item->SetCount(item->GetCount() - 1);
					return true;
				}
				else if (distance < 20000)
				{
					dist = 1;
				}
				else if (distance < 70000)
				{
					dist = 2;
				}
				else
				{
					dist = 3;
				}


				const int STONE_DETECT_MAX_TRY = 10;
				if (item->GetSocket(0) >= STONE_DETECT_MAX_TRY)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Event Detector has vanished."));
					item->SetCount(item->GetCount() - 1);
					AutoGiveItem(27002);
					return true;
				}

				if (dist)
				{
					char chatbuf[CHAT_MAX_LEN + 1];
					int len = snprintf(chatbuf, sizeof(chatbuf),
									   "StoneDetect %u %d %d",
									   (DWORD)GetVID(), dist, (int)GetDegreeFromPositionXY(GetX(), item->GetSocket(2), item->GetSocket(1), GetY()));

					if (len < 0 || len >= (int) sizeof(chatbuf))
					{
						len = sizeof(chatbuf) - 1;
					}

					++len;

					TPacketGCChat pack_chat;
					pack_chat.header	= HEADER_GC_CHAT;
					pack_chat.size		= sizeof(TPacketGCChat) + len;
					pack_chat.type		= CHAT_TYPE_COMMAND;
					pack_chat.id		= 0;
					pack_chat.bEmpire	= GetDesc()->GetEmpire();
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
					pack_chat.locale	= GetLocale();
#endif
					//pack_chat.id		= vid;

					TEMP_BUFFER buf;
					buf.write(&pack_chat, sizeof(TPacketGCChat));
					buf.write(chatbuf, len);

					PacketAround(buf.read_peek(), buf.size());
				}

			}
			break;

			case 27989:
			case 76006:
			{
				LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(GetMapIndex());

				if (pMap != NULL)
				{
					item->SetSocket(0, item->GetSocket(0) + 1);

					FFindStone f;

					// <Factor> SECTREE::for_each -> SECTREE::for_each_entity
					pMap->for_each(f);

					if (f.m_mapStone.size() > 0)
					{
						std::map<DWORD, LPCHARACTER>::iterator stone = f.m_mapStone.begin();

						DWORD max = UINT_MAX;
						LPCHARACTER pTarget = stone->second;

						while (stone != f.m_mapStone.end())
						{
							DWORD dist = (DWORD)DISTANCE_SQRT(GetX() - stone->second->GetX(), GetY() - stone->second->GetY());

							if (dist != 0 && max > dist)
							{
								max = dist;
								pTarget = stone->second;
							}
							stone++;
						}

						if (pTarget != NULL)
						{
							int val = 3;

							if (max < 10000) { val = 2; }
							else if (max < 70000) { val = 1; }

							ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u %d %d", (DWORD)GetVID(), val,
									   (int)GetDegreeFromPositionXY(GetX(), pTarget->GetY(), pTarget->GetX(), GetY()));
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Stone not detected."));
						}
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no stone."));
					}

					if (item->GetSocket(0) >= 6)
					{
						ChatPacket(CHAT_TYPE_COMMAND, "StoneDetect %u 0 0", (DWORD)GetVID());
						item->SetCount(item->GetCount() - 1);
					}
				}
				break;
			}
			break;

			case 27996:
				item->SetCount(item->GetCount() - 1);
				AttackedByPoison(NULL); // @warme008
				break;

			case 27987:
			{
				item->SetCount(item->GetCount() - 1);

				int r = number(1, 100);

				if (r <= 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You find a simple Piece of Stone in the Clam."));
					AutoGiveItem(27990);
				}
				else
				{
					const int prob_table_gb2312[] =
					{
						95, 97, 99
					};

					const int * prob_table = prob_table_gb2312;

					if (r <= prob_table[0])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Clam has vanished."));
					}
					else if (r <= prob_table[1])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a White Pearl inside the Clam."));
						AutoGiveItem(27992);
					}
					else if (r <= prob_table[2])
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a Blue Pearl inside the Clam."));
						AutoGiveItem(27993);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is a Blood-Red Pearl inside the Clam."));
						AutoGiveItem(27994);
					}
				}
			}
			break;

			case 71013:
				CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50100:
			case 50101:
			case 50102:
			case 50103:
			case 50104:
			case 50105:
			case 50106:
				CreateFly(item->GetVnum() - 50100 + FLY_FIREWORK1, this);
				item->SetCount(item->GetCount() - 1);
				break;

			case 50200:
				if (g_bEnableBootaryCheck)
				{
					if (IS_BOTARYABLE_ZONE(GetMapIndex()) == true)
					{
						__OpenPrivateShop();
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open a warehouse in this area."));
					}
				}
				else
				{
					__OpenPrivateShop();
				}
				break;

			//case fishing::FISH_MIND_PILL_VNUM:
			//	AddAffect(AFFECT_FISH_MIND_PILL, POINT_NONE, 0, AFF_FISH_MIND, 20*60, 0, true);
			//	item->SetCount(item->GetCount() - 1);
			//	break;

			case 50301:
			case 50302:
			case 50303:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
					return false;
				}

				int lv = GetSkillLevel(SKILL_LEADERSHIP);

				if (lv < item->GetValue(0))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this book."));
					return false;
				}

				if (lv >= item->GetValue(1))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This book will not help you."));
					return false;
				}

				if (LearnSkillByBook(SKILL_LEADERSHIP))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(SKILL_LEADERSHIP, get_global_time() + iReadDelay);
#ifdef __ENABLE_SKILLS_INFORMATION__
					BroadcastSkillInformation();
#endif
				}
			}
			break;

			case 50304:
			case 50305:
			case 50306:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
					return false;

				}
				if (GetSkillLevel(SKILL_COMBO) == 0 && GetLevel() < 30)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to have a minimum level of 30 to understand this book."));
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) == 1 && GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a minimum level of 50 to understand this book."));
					return false;
				}

				if (GetSkillLevel(SKILL_COMBO) >= 2)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't train any more Combos."));
					return false;
				}

				int iPct = item->GetValue(0);

				if (LearnSkillByBook(SKILL_COMBO, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(SKILL_COMBO, get_global_time() + iReadDelay);
				}
			}
			break;
			case 50311:
			case 50312:
			case 50313:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read books while transformed."));
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);
				if (GetSkillLevel(dwSkillVnum) >= 20 || dwSkillVnum - SKILL_LANGUAGE1 + 1 == GetEmpire())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already understand this language."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50061 :
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�����߿��� å�� ������ �����ϴ�."));
					return false;

				}
				DWORD dwSkillVnum = item->GetValue(0);
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 10)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train this skill."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;

			case 50314:
			case 50315:
			case 50316:
			case 50323:
			case 50324:
			case 50325:
			case 50326:
			{
				if (IsPolymorphed() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change your status while you are transformed."));
					return false;
				}

				int iSkillLevelLowLimit = item->GetValue(0);
				int iSkillLevelHighLimit = item->GetValue(1);
				int iLevelLimit = item->GetValue(3);
				DWORD dwSkillVnum = 0;

				switch (item->GetVnum())
				{
				case 50314:
				case 50315:
				case 50316:
					dwSkillVnum = SKILL_POLYMORPH;
					break;

				case 50323:
				case 50324:
					dwSkillVnum = SKILL_ADD_HP;
					break;

				case 50325:
				case 50326:
					dwSkillVnum = SKILL_RESIST_PENETRATE;
					break;

#ifdef __ENABLE_AMULET_SYSTEM__
				case 178107:
				case 178108:
				case 178109:
					dwSkillVnum = SKILL_PASSIVE_CRAFT_AMULETS;
					break;
#endif


				default:
					return false;
				}

				if (0 == dwSkillVnum)
				{
					return false;
				}

				if (GetLevel() < iLevelLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to improve your Level to read this Book."));
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train this skill."));
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) < iSkillLevelLowLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("It isn't easy to understand this book."));
					return false;
				}

				if (GetSkillLevel(dwSkillVnum) >= iSkillLevelHighLimit)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train with this Book any more."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
#ifdef __ENABLE_SKILLS_INFORMATION__
					BroadcastSkillInformation();
#endif
				}
			}
			break;

			case 50902:
			case 50903:
			case 50904:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�����߿��� å�� ������ �����ϴ�."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_CREATE;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train this skill."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);

					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Success to learn skill ");
					}
				}
				else
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, "[TEST_SERVER] Failed to learn skill ");
					}
				}
			}
			break;

			// MINING
			case ITEM_MINING_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�����߿��� å�� ������ �����ϴ�."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_MINING;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetSkillLevel(dwSkillVnum) >= 40)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot train this skill."));
					return false;
				}

				if (LearnSkillByBook(dwSkillVnum, iPct))
				{
#ifdef ENABLE_BOOKS_STACKFIX
					item->SetCount(item->GetCount() - 1);
#else
					ITEM_MANAGER::instance().RemoveItem(item);
#endif

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
				}
			}
			break;
			// END_OF_MINING

			case ITEM_HORSE_SKILL_TRAIN_BOOK:
			{
				if (IsPolymorphed())
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�����߿��� å�� ������ �����ϴ�."));
					return false;

				}
				DWORD dwSkillVnum = SKILL_HORSE;
				int iPct = MINMAX(0, item->GetValue(1), 100);

				if (GetLevel() < 50)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need a minimum level of 50 to get riding training."));
					return false;
				}

				if (!test_server && get_global_time() < GetSkillNextReadTime(dwSkillVnum))
				{
					if (FindAffect(AFFECT_SKILL_NO_BOOK_DELAY))
					{

						RemoveAffect(AFFECT_SKILL_NO_BOOK_DELAY);
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have escaped the evil ghost curse with the help of an Exorcism Scroll."));
					}
					else
					{
						SkillLearnWaitMoreTimeMessage(GetSkillNextReadTime(dwSkillVnum) - get_global_time());
						return false;
					}
				}

				if (GetPoint(POINT_HORSE_SKILL) >= 20 ||
						GetSkillLevel(SKILL_HORSE_WILDATTACK) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60 ||
						GetSkillLevel(SKILL_HORSE_WILDATTACK_RANGE) + GetSkillLevel(SKILL_HORSE_CHARGE) + GetSkillLevel(SKILL_HORSE_ESCAPE) >= 60)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot read any more Riding Guides."));
					return false;
				}

				if (number(1, 100) <= iPct)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You read the Horse Riding Manual and received a Riding Point."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can use this point to improve your riding skill!"));
					PointChange(POINT_HORSE_SKILL, 1);

					int iReadDelay = number(BOOK_DELAY_MIN, BOOK_DELAY_MAX);
					if (distribution_test_server) { iReadDelay /= 3; }

					if (!test_server)
					{
						SetSkillNextReadTime(dwSkillVnum, get_global_time() + iReadDelay);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You did not understand the riding guide."));
				}
#ifdef ENABLE_BOOKS_STACKFIX
				item->SetCount(item->GetCount() - 1);
#else
				ITEM_MANAGER::instance().RemoveItem(item);
#endif
			}
			break;

			case 70102:
			case 70103:
			{
				if (GetAlignment() >= 0)
				{
					return false;
				}

				int delta = MIN(-GetAlignment(), item->GetValue(0));

				sys_log(0, "%s ALIGNMENT ITEM %d", GetName(), delta);

				UpdateAlignment(delta);
				item->SetCount(item->GetCount() - 1);

				if (delta / 10 > 0)
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("Your mind is clear. You can concentrate really well now."));
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank has increased by %d points."), delta / 10);
				}
			}
			break;

			case 71107:
			{
				int val = item->GetValue(0);
				int interval = item->GetValue(1);
				quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());
				int last_use_time = pPC->GetFlag("mythical_peach.last_use_time");

				if (get_global_time() - last_use_time < interval * 60 * 60)
				{
					if (test_server == false)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use it now."));
						return false;
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Test server time limit passed "));
					}
				}

				if (GetAlignment() == 200000)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank  cannot raise any more."));
					return false;
				}

				if (200000 - GetAlignment() < val * 10)
				{
					val = (200000 - GetAlignment()) / 10;
				}

				int old_alignment = GetAlignment() / 10;

				UpdateAlignment(val * 10);

				item->SetCount(item->GetCount() - 1);
				pPC->SetFlag("mythical_peach.last_use_time", get_global_time());

				ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("Your mind is clear. You can concentrate really well now."));
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your rank has increased by %d points."), val);

				char buf[256 + 1];
				snprintf(buf, sizeof(buf), "%d %d", old_alignment, GetAlignment() / 10);
				LogManager::instance().CharLog(this, val, "MYTHICAL_PEACH", buf);
			}
			break;

			case 71109:
			case 72719:
			{
				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				{
					return false;
				}

				if (item2->GetSocketCount() == 0)
				{
					return false;
				}

				switch ( item2->GetType() )
				{
				case ITEM_WEAPON:
					break;
				case ITEM_ARMOR:
					switch (item2->GetSubType())
					{
					case ARMOR_EAR:
					case ARMOR_WRIST:
					case ARMOR_NECK:
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No metin stone to take out."));
						return false;
					}
					break;

				default:
					return false;
				}

				std::stack<long> socket;

				for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
				{
					socket.push(item2->GetSocket(i));
				}

				int idx = ITEM_SOCKET_MAX_NUM - 1;

				while (socket.size() > 0)
				{
					if (socket.top() > 2 && socket.top() != ITEM_BROKEN_METIN_VNUM)
					{
						break;
					}

					idx--;
					socket.pop();
				}

				if (socket.size() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No metin stone to take out."));
					return false;
				}

				LPITEM pItemReward = AutoGiveItem(socket.top());

				if (pItemReward != NULL)
				{
					item2->SetSocket(idx, 1);

					char buf[256 + 1];
					snprintf(buf, sizeof(buf), "%s(%u) %s(%u)",
							 item2->GetName(), item2->GetID(), pItemReward->GetName(), pItemReward->GetID());
					LogManager::instance().ItemLog(this, item, "USE_DETACHMENT_ONE", buf);

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case 70201:
			case 70202:
			case 70203:
			case 70204:
			case 70205:
			case 70206:
			{
				// NEW_HAIR_STYLE_ADD
				if (GetPart(PART_HAIR) >= 1001)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot dye or bleach your current hairstyle."));
				}
				// END_NEW_HAIR_STYLE_ADD
				else
				{
					quest::CQuestManager& q = quest::CQuestManager::instance();
					quest::PC* pPC = q.GetPC(GetPlayerID());

					if (pPC)
					{
						int last_dye_level = pPC->GetFlag("dyeing_hair.last_dye_level");

						if (last_dye_level == 0 ||
								last_dye_level + 3 <= GetLevel() ||
								item->GetVnum() == 70201)
						{
							SetPart(PART_HAIR, item->GetVnum() - 70201);

							if (item->GetVnum() == 70201)
							{
								pPC->SetFlag("dyeing_hair.last_dye_level", 0);
							}
							else
							{
								pPC->SetFlag("dyeing_hair.last_dye_level", GetLevel());
							}

							item->SetCount(item->GetCount() - 1);
							UpdatePacket();
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You need to have reached level %d to be able to dye your hair again."), last_dye_level + 3);
						}
					}
				}
			}
			break;

			case ITEM_NEW_YEAR_GREETING_VNUM:
			{
				DWORD dwBoxVnum = ITEM_NEW_YEAR_GREETING_VNUM;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets;
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						if (dwVnums[i] == CSpecialItemGroup::GOLD)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
						}
					}

					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case ITEM_VALENTINE_ROSE:
			case ITEM_VALENTINE_CHOCOLATE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;


				if (((item->GetVnum() == ITEM_VALENTINE_ROSE) && (SEX_MALE == GET_SEX(this))) ||
						((item->GetVnum() == ITEM_VALENTINE_CHOCOLATE) && (SEX_FEMALE == GET_SEX(this))))
				{

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be opened by the another gender."));
					return false;
				}


				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case ITEM_WHITEDAY_CANDY:
			case ITEM_WHITEDAY_ROSE:
			{
				DWORD dwBoxVnum = item->GetVnum();
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;


				if (((item->GetVnum() == ITEM_WHITEDAY_CANDY) && (SEX_MALE == GET_SEX(this))) ||
						((item->GetVnum() == ITEM_WHITEDAY_ROSE) && (SEX_FEMALE == GET_SEX(this))))
				{

					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be opened by the another gender."));
					return false;
				}


				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					item->SetCount(item->GetCount() - 1);
				}
			}
			break;

			case 50011:
			{
				DWORD dwBoxVnum = 50011;
				std::vector <DWORD> dwVnums;
				std::vector <DWORD> dwCounts;
				std::vector <LPITEM> item_gets(0);
				int count = 0;

				if (GiveItemFromSpecialItemGroup(dwBoxVnum, dwVnums, dwCounts, item_gets, count))
				{
					for (int i = 0; i < count; i++)
					{
						char buf[50 + 1];
						snprintf(buf, sizeof(buf), "%u %u", dwVnums[i], dwCounts[i]);
						LogManager::instance().ItemLog(this, item, "MOONLIGHT_GET", buf);

						//ITEM_MANAGER::instance().RemoveItem(item);
						item->SetCount(item->GetCount() - 1);

						switch (dwVnums[i])
						{
						case CSpecialItemGroup::GOLD:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), dwCounts[i]);
							break;

						case CSpecialItemGroup::EXP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A mysterious light comes out of the box."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d experience points."), dwCounts[i]);
							break;

						case CSpecialItemGroup::MOB:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
							break;

						case CSpecialItemGroup::SLOW:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the red smoke coming out of the box, your speed will increase!"));
							break;

						case CSpecialItemGroup::DRAIN_HP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box suddenly exploded! You have lost Hit Points (HP)."));
							break;

						case CSpecialItemGroup::POISON:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
							break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
						case CSpecialItemGroup::BLEEDING:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("If you inhale the green smoke that is coming out of the box, the poison will spread through your body!"));
							break;
#endif
						case CSpecialItemGroup::MOB_GROUP:
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Look what came out of the box!"));
							break;

						default:
							if (item_gets[i])
							{
								if (dwCounts[i] > 1)
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Receive: %s - %d"), item_gets[i]->GetName(), dwCounts[i]);
								}
								else
								{
									ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The box contains %s."), item_gets[i]->GetName());
								}
							}
							break;
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_TALKING, LC_TEXT("You have not received anything."));
					return false;
				}
			}
			break;

			case ITEM_GIVE_STAT_RESET_COUNT_VNUM:
			{
				PointChange(POINT_STAT_RESET_COUNT, 1);
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50107:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#ifdef __ENABLE_NEWSTUFF__
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#endif

				EffectPacket(SE_CHINA_FIREWORK);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case 50108:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#ifdef __ENABLE_NEWSTUFF__
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#endif

				EffectPacket(SE_SPIN_TOP);
#ifdef ENABLE_FIREWORK_STUN

				AddAffect(AFFECT_CHINA_FIREWORK, POINT_STUN_PCT, 30, AFF_CHINA_FIREWORK, 5 * 60, 0, true);
#endif
				item->SetCount(item->GetCount() - 1);
			}
			break;

			case ITEM_WONSO_BEAN_VNUM:
				PointChange(POINT_HP, GetMaxHP() - GetHP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_SUGAR_VNUM:
				PointChange(POINT_SP, GetMaxSP() - GetSP());
				item->SetCount(item->GetCount() - 1);
				break;

			case ITEM_WONSO_FRUIT_VNUM:
				PointChange(POINT_STAMINA, GetMaxStamina() - GetStamina());
				item->SetCount(item->GetCount() - 1);
				break;

			case 90008: // VCARD
			case 90009: // VCARD
				VCardUse(this, this, item);
				break;

			case ITEM_ELK_VNUM:
			{
				int iGold = item->GetSocket(0);
				item->SetCount(item->GetCount() - 1);
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have received %d Yang."), iGold);
				PointChange(POINT_GOLD, iGold);
			}
			break;

			case 71092 :
			{
				if (m_pkChrTarget != NULL)
				{
					if (m_pkChrTarget->IsPolymorphed())
					{
						m_pkChrTarget->SetPolymorph(0);
						m_pkChrTarget->RemoveAffect(AFFECT_POLYMORPH);
					}
				}
				else
				{
					if (IsPolymorphed())
					{
						SetPolymorph(0);
						RemoveAffect(AFFECT_POLYMORPH);
					}
				}
			}
			break;

			case 71051 :
			{

				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetInventoryItem(wDestCell)))
				{
					return false;
				}

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				{
					return false;
				}

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				if (item2->AddRareAttribute() == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have successfully added stats."));
					int iAddedIdx = item2->GetRareAttrCount() + 4;
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					LogManager::instance().ItemLog(
						GetPlayerID(),
						item2->GetAttributeType(iAddedIdx),
						item2->GetAttributeValue(iAddedIdx),
						item->GetID(),
						"ADD_RARE_ATTR",
						buf,
						GetDesc()->GetHostName(),
						item->GetOriginalVnum());

					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this Item for other Upgrades."));
				}
			}
			break;

			case 71052 :
			{

				LPITEM item2;

				if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
				{
					return false;
				}

				if (ITEM_COSTUME == item2->GetType()) // @fixme124
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
				{
					return false;
				}

				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				if (item2->ChangeRareAttribute() == true)
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_RARE_ATTR", buf);

					item->SetCount(item->GetCount() - 1);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No stats to change."));
				}
			}
			break;

			case ITEM_AUTO_HP_RECOVERY_S:
			case ITEM_AUTO_HP_RECOVERY_M:
			case ITEM_AUTO_HP_RECOVERY_L:
			case ITEM_AUTO_HP_RECOVERY_X:
			case ITEM_AUTO_SP_RECOVERY_S:
			case ITEM_AUTO_SP_RECOVERY_M:
			case ITEM_AUTO_SP_RECOVERY_L:
			case ITEM_AUTO_SP_RECOVERY_X:


			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
			case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
			case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
			{
				if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}
#ifdef __ENABLE_NEWSTUFF__
				else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
					return false;
				}
#endif

				EAffectTypes type = AFFECT_NONE;
				bool isSpecialPotion = false;

				switch (item->GetVnum())
				{
				case ITEM_AUTO_HP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_HP_RECOVERY_S:
				case ITEM_AUTO_HP_RECOVERY_M:
				case ITEM_AUTO_HP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_HP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_HP_RECOVERY_S:
					type = AFFECT_AUTO_HP_RECOVERY;
					break;

				case ITEM_AUTO_SP_RECOVERY_X:
					isSpecialPotion = true;

				case ITEM_AUTO_SP_RECOVERY_S:
				case ITEM_AUTO_SP_RECOVERY_M:
				case ITEM_AUTO_SP_RECOVERY_L:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_XS:
				case REWARD_BOX_ITEM_AUTO_SP_RECOVERY_S:
				case FUCKING_BRAZIL_ITEM_AUTO_SP_RECOVERY_S:
					type = AFFECT_AUTO_SP_RECOVERY;
					break;
				}

				if (AFFECT_NONE == type)
				{
					break;
				}

				if (item->GetCount() > 1)
				{
#ifndef __SPECIAL_STORAGE_ENABLE__
					int pos = GetEmptyInventory(item->GetSize());
#else
					int pos = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

					if (-1 == pos)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There isn't enough space in your inventory."));
						break;
					}

					item->SetCount( item->GetCount() - 1 );

					LPITEM item2 = ITEM_MANAGER::instance().CreateItem( item->GetVnum(), 1 );
					item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

					if (item->GetSocket(1) != 0)
					{
						item2->SetSocket(1, item->GetSocket(1));
					}

					item = item2;
				}

				CAffect* pAffect = FindAffect( type );

				if (NULL == pAffect)
				{
					EPointTypes bonus = POINT_NONE;

					if (true == isSpecialPotion)
					{
						if (type == AFFECT_AUTO_HP_RECOVERY)
						{
							bonus = POINT_MAX_HP_PCT;
						}
						else if (type == AFFECT_AUTO_SP_RECOVERY)
						{
							bonus = POINT_MAX_SP_PCT;
						}
					}

					AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

					item->Lock(true);
					item->SetSocket(0, true);

					AutoRecoveryItemProcess( type );
				}
				else
				{
					if (item->GetID() == pAffect->dwFlag)
					{
						RemoveAffect( pAffect );

						item->Lock(false);
						item->SetSocket(0, false);
					}
					else
					{
						LPITEM old = FindItemByID( pAffect->dwFlag );

						if (NULL != old)
						{
							old->Lock(false);
							old->SetSocket(0, false);
						}

						RemoveAffect( pAffect );

						EPointTypes bonus = POINT_NONE;

						if (true == isSpecialPotion)
						{
							if (type == AFFECT_AUTO_HP_RECOVERY)
							{
								bonus = POINT_MAX_HP_PCT;
							}
							else if (type == AFFECT_AUTO_SP_RECOVERY)
							{
								bonus = POINT_MAX_SP_PCT;
							}
						}

						AddAffect( type, bonus, 4, item->GetID(), INFINITE_AFFECT_DURATION, 0, true, false);

						item->Lock(true);
						item->SetSocket(0, true);

						AutoRecoveryItemProcess( type );
					}
				}
			}
			break;
			}
			break;

		case USE_CLEAR:
		{
			switch (item->GetVnum())
			{
#ifdef __ENABLE_WOLFMAN_CHARACTER__
			case 27124: // Bandage
				RemoveBleeding();
				break;
#endif
			case 27874: // Grilled Perch
			default:
				RemoveBadAffect();
				break;
			}
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_INVISIBILITY:
		{
			if (item->GetVnum() == 70026)
			{
				quest::CQuestManager& q = quest::CQuestManager::instance();
				quest::PC* pPC = q.GetPC(GetPlayerID());

				if (pPC != NULL)
				{
					int last_use_time = pPC->GetFlag("mirror_of_disapper.last_use_time");

					if (get_global_time() - last_use_time < 10 * 60)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use it now."));
						return false;
					}

					pPC->SetFlag("mirror_of_disapper.last_use_time", get_global_time());
				}
			}

			AddAffect(AFFECT_INVISIBILITY, POINT_NONE, 0, AFF_INVISIBILITY, 300, 0, true);
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_POTION_NODELAY:
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}

				switch (item->GetVnum())
				{
				case 70020 :
				case 71018 :
				case 71019 :
				case 71020 :
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
							return false;
						}
					}
					break;

				default :
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}
			}
#ifdef __ENABLE_NEWSTUFF__
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#endif

			bool used = false;

			if (item->GetValue(0) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					PointChange(POINT_HP, item->GetValue(0) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_HPUP_RED);
					used = TRUE;
				}
			}

			if (item->GetValue(1) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(1) * (100 + GetPoint(POINT_POTION_BONUS)) / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (item->GetValue(3) != 0)
			{
				if (GetHP() < GetMaxHP())
				{
					PointChange(POINT_HP, item->GetValue(3) * GetMaxHP() / 100);
					EffectPacket(SE_HPUP_RED);
					used = TRUE;
				}
			}

			if (item->GetValue(4) != 0)
			{
				if (GetSP() < GetMaxSP())
				{
					PointChange(POINT_SP, item->GetValue(4) * GetMaxSP() / 100);
					EffectPacket(SE_SPUP_BLUE);
					used = TRUE;
				}
			}

			if (used)
			{
				if (item->GetVnum() == 50085 || item->GetVnum() == 50086)
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Used Moon Cake or Seed."));
					}
					SetUseSeedOrMoonBottleTime();
				}
				if (GetDungeon())
				{
					GetDungeon()->UsePotion(this);
				}

				if (GetWarMap())
				{
					GetWarMap()->UsePotion(this, item);
				}

				m_nPotionLimit--;

				//RESTRICT_USE_SEED_OR_MOONBOTTLE
				item->SetCount(item->GetCount() - 1);
				//END_RESTRICT_USE_SEED_OR_MOONBOTTLE
			}
		}
		break;

		case USE_POTION:
		{
			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit") > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}

				switch (item->GetVnum())
				{
				case 27001:
				case 27002:
				case 27003:
				case 27004:
				case 27005:
				case 27006:
					if (quest::CQuestManager::instance().GetEventFlag("arena_potion_limit_count") < 10000)
					{
						if (m_nPotionLimit <= 0)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That is over the limit."));
							return false;
						}
					}
					break;

				default:
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this in the duel arena."));
					return false;
				}
			}
#ifdef __ENABLE_NEWSTUFF__
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#endif

			if (item->GetValue(1) != 0)
			{
				if (GetPoint(POINT_SP_RECOVERY) + GetSP() >= GetMaxSP())
				{
					return false;
				}

				PointChange(POINT_SP_RECOVERY, item->GetValue(1) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_SPUP_BLUE);
			}

			if (item->GetValue(0) != 0)
			{
				if (GetPoint(POINT_HP_RECOVERY) + GetHP() >= GetMaxHP())
				{
					return false;
				}

				PointChange(POINT_HP_RECOVERY, item->GetValue(0) * MIN(200, (100 + GetPoint(POINT_POTION_BONUS))) / 100);
				StartAffectEvent();
				EffectPacket(SE_HPUP_RED);
			}

			if (GetDungeon())
			{
				GetDungeon()->UsePotion(this);
			}

			if (GetWarMap())
			{
				GetWarMap()->UsePotion(this, item);
			}

			bool potionGetCount = item->GetValue(5) == 0;


			if (potionGetCount)
			{
				item->SetCount(item->GetCount() - 1);
				m_nPotionLimit--;
			}
		}
		break;

		case USE_POTION_CONTINUE:
		{
			if (item->GetValue(0) != 0)
			{
				AddAffect(AFFECT_HP_RECOVER_CONTINUE, POINT_HP_RECOVER_CONTINUE, item->GetValue(0), 0, item->GetValue(2), 0, true);
			}
			else if (item->GetValue(1) != 0)
			{
				AddAffect(AFFECT_SP_RECOVER_CONTINUE, POINT_SP_RECOVER_CONTINUE, item->GetValue(1), 0, item->GetValue(2), 0, true);
			}
			else
			{
				return false;
			}
		}

		if (GetDungeon())
		{
			GetDungeon()->UsePotion(this);
		}

		if (GetWarMap())
		{
			GetWarMap()->UsePotion(this, item);
		}

		item->SetCount(item->GetCount() - 1);
		break;

		case USE_ABILITY_UP:
		{
			switch (item->GetValue(0))
			{
			case APPLY_MOV_SPEED:
				AddAffect(AFFECT_MOV_SPEED, POINT_MOV_SPEED, item->GetValue(2), AFF_MOV_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
				EffectPacket(SE_DXUP_PURPLE);
#endif
				break;

			case APPLY_ATT_SPEED:
				AddAffect(AFFECT_ATT_SPEED, POINT_ATT_SPEED, item->GetValue(2), AFF_ATT_SPEED_POTION, item->GetValue(1), 0, true);
#ifdef ENABLE_EFFECT_EXTRAPOT
				EffectPacket(SE_SPEEDUP_GREEN);
#endif
				break;

			case APPLY_STR:
				AddAffect(AFFECT_STR, POINT_ST, item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_DEX:
				AddAffect(AFFECT_DEX, POINT_DX, item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_CON:
				AddAffect(AFFECT_CON, POINT_HT, item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_INT:
				AddAffect(AFFECT_INT, POINT_IQ, item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_CAST_SPEED:
				AddAffect(AFFECT_CAST_SPEED, POINT_CASTING_SPEED, item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_ATT_GRADE_BONUS:
				AddAffect(AFFECT_ATT_GRADE, POINT_ATT_GRADE_BONUS,
						  item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;

			case APPLY_DEF_GRADE_BONUS:
				AddAffect(AFFECT_DEF_GRADE, POINT_DEF_GRADE_BONUS,
						  item->GetValue(2), 0, item->GetValue(1), 0, true);
				break;
			}
		}

		if (GetDungeon())
		{
			GetDungeon()->UsePotion(this);
		}

		if (GetWarMap())
		{
			GetWarMap()->UsePotion(this, item);
		}

		item->SetCount(item->GetCount() - 1);
		break;

		case USE_TALISMAN:
		{
			const int TOWN_PORTAL	= 1;
			const int MEMORY_PORTAL = 2;



			if (GetMapIndex() == 200 || GetMapIndex() == 113)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this from your current position."));
				return false;
			}

			if (CArenaManager::instance().IsArenaMap(GetMapIndex()) == true)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#ifdef __ENABLE_NEWSTUFF__
			else if (g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(item->GetVnum()))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item in a duel."));
				return false;
			}
#endif

			if (m_pkWarpEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are ready to warp, so you cannot use the Scroll of the Location."));
				return false;
			}

			// CONSUME_LIFE_WHEN_USE_WARP_ITEM
			int consumeLife = CalculateConsume(this);

			if (consumeLife < 0)
			{
				return false;
			}
			// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

			if (item->GetValue(0) == TOWN_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (!GetDungeon())
						if (!GiveRecallItem(item))
						{
							return false;
						}

					PIXEL_POSITION posWarp;

					if (SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp))
					{
						// CONSUME_LIFE_WHEN_USE_WARP_ITEM
						PointChange(POINT_HP, -consumeLife, false);
						// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

						WarpSet(posWarp.x, posWarp.y);
					}
					else
					{
						sys_err("CHARACTER::UseItem : cannot find spawn position (name %s, %d x %d)", GetName(), GetX(), GetY());
					}
				}
				else
				{
					if (test_server)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are being brought back to the place of origin."));
					}

					ProcessRecallItem(item);
				}
			}
			else if (item->GetValue(0) == MEMORY_PORTAL)
			{
				if (item->GetSocket(0) == 0)
				{
					if (GetDungeon())
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s%s cannot be used in a dungeon."),
								   item->GetName(),
								   "");
						return false;
					}

					if (!GiveRecallItem(item))
					{
						return false;
					}
				}
				else
				{
					// CONSUME_LIFE_WHEN_USE_WARP_ITEM
					PointChange(POINT_HP, -consumeLife, false);
					// END_OF_CONSUME_LIFE_WHEN_USE_WARP_ITEM

					ProcessRecallItem(item);
				}
			}
		}
		break;

		case USE_TUNING:
		case USE_DETACHMENT:
		{
			LPITEM item2;

			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			{
				return false;
			}

			if (item2->GetVnum() >= 28330 && item2->GetVnum() <= 28343)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("+3 spirit stones can not be upgraded by this item."));
				return false;
			}

			if (item2->GetVnum() >= 28430 && item2->GetVnum() <= 28443)
			{
				if (item->GetVnum() == 71056)
				{
					RefineItem(item, item2);
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The spirit stone can not be advanced by this item."));
				}
			}
			else
			{
				RefineItem(item, item2);
			}
		}
		break;

#ifdef __ENABLE_NEW_COSTUME_BONUS__
		case USE_ADD_COSTUME_ATTR_SPECIAL:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_COSTUME != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
				return false;
			}

			const auto iRequiredSubType = item->GetValue(0);
			if (iRequiredSubType < 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Please contact with administration!"));
				return false;
			}

			// Lets check if subType is fitt
			if (item2->GetSubType() != iRequiredSubType)
			{
				return false;
			}

			// Lets declare locked subTypes
			const std::set<BYTE> lLockedSubTypes = { COSTUME_SASH, COSTUME_MOUNT, COSTUME_PET };
			if (lLockedSubTypes.find(item2->GetSubType()) != lLockedSubTypes.end())
			{
				return false;
			}

			// Right now lets copy bonuses to new item!
			std::vector<std::pair<BYTE, long>> pApplys;
			const auto* pTable = ITEM_MANAGER::instance().GetTable(item->GetVnum());
			if (pTable)
			{
				for (const auto& [applyType, applyValue] : pTable->aApplies)
				{
					if (applyType == APPLY_NONE || applyValue == 0) continue;

					pApplys.push_back(std::make_pair(applyType, applyValue));
				}

				if ((item2->GetAttributeCount() + pApplys.size()) > ITEM_COSTUME_MAX_ATTR)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't put this item to this costume due to limit of values!"));
					return false;
				}

				// Lets check if we have already any bonus in item!
				if (std::count_if(pApplys.begin(), pApplys.end(), [&](const std::pair<BYTE, long> rPair) { return (item2->HasAttr(rPair.first)); }) > 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already this kind of bonus!"));
					return false;
				}

				for (const auto& rApply : pApplys)
				{
					item2->SetForceAttribute(item2->GetAttributeCount(), rApply.first, rApply.second);
				}
			}

			item->SetCount(item->GetCount() - 1);

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You changed the item bonus."));
		}
		break;
		case USE_REMOVE_COSTUME_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_COSTUME != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the bonus of this Item."));
				return false;
			}

			// Lets declare locked subTypes
			const std::set<BYTE> lLockedSubTypes = { COSTUME_SASH, COSTUME_MOUNT, COSTUME_PET };
			if (lLockedSubTypes.find(item2->GetSubType()) != lLockedSubTypes.end())
			{
				return false;
			}

			item2->ClearAttribute();
			item->SetCount(item->GetCount() - 1);

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You changed the item bonus."));
		}
		break;
#endif

#ifdef __BELT_ATTR__
		case USE_ADD_BELT_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_BELT != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add a bonus to this item."));
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			{
				return false;
			}

			if (item2->GetAttributeSetIndex() == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add a bonus to this item."));
				return false;
			}

			item2->ClearAttribute();
			auto randCount = Random::get(1, ITEM_MANAGER::MAX_BELT_ATTR_NUM);
			for (size_t i = 0; i < randCount; i++)
			{
				item2->AddBeltAttribute();
			}

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));

			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_CHANGE_BELT_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_BELT != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			{
				return false;
			}

			if (item2->GetAttributeSetIndex() == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
				return false;
			}

			if (item2->GetAttributeCount() == 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no upgrade that you can change."));
				return false;
			}

			item2->ChangeBeltAttribute();
			char buf[21];
			snprintf(buf, sizeof(buf), "%u", item2->GetID());
			LogManager::instance().ItemLog(this, item, "CHANGE_BELT_ATTR", buf);

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the upgrade."));

			item->SetCount(item->GetCount() - 1);
			break;
		}
#endif

		case USE_CHANGE_COSTUME_ATTR:
		case USE_RESET_COSTUME_ATTR:
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			if (ITEM_COSTUME != item2->GetType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
				return false;
			}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			{
				return false;
			}

			if (item2->GetAttributeSetIndex() == -1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
				return false;
			}

			if (item2->GetAttributeCount() == 0)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no upgrade that you can change."));
				return false;
			}

			switch (item->GetSubType())
			{
			case USE_CHANGE_COSTUME_ATTR:
				item2->ChangeAttribute();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_COSTUME_ATTR", buf);
				}
				break;
			case USE_RESET_COSTUME_ATTR:
				item2->ClearAttribute();
				item2->AlterToMagicItem();
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "RESET_COSTUME_ATTR", buf);
				}
				break;
			}

			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the upgrade."));

			item->SetCount(item->GetCount() - 1);
			break;
		}

		//  ACCESSORY_REFINE & ADD/CHANGE_ATTRIBUTES
		case USE_PUT_INTO_BELT_SOCKET:
		case USE_PUT_INTO_RING_SOCKET:
		case USE_PUT_INTO_ACCESSORY_SOCKET:
		case USE_ADD_ACCESSORY_SOCKET:
		case USE_CLEAN_SOCKET:
		case USE_CHANGE_ATTRIBUTE:
		case USE_CHANGE_ATTRIBUTE2 :
		case USE_ADD_ATTRIBUTE:
		case USE_ADD_ATTRIBUTE2:
#ifdef __ENABLE_ORE_REFACTOR__
		case USE_REMOVE_ACCESSORY_SOCKET:
#endif
		{
			LPITEM item2;
			if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
			{
				return false;
			}

			if (item2->IsEquipped())
			{
				BuffOnAttr_RemoveBuffsFromItem(item2);
			}

			const auto LOCKED_LIST = { ITEM_COSTUME, ITEM_BELT };
			for (const auto& type : LOCKED_LIST)
			{
				if (item2->GetType() == type)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}
			}

			//if (ITEM_BELT == item2->GetType() && item->GetSubType() != USE_ADD_ACCESSORY_SOCKET && item->GetSubType() != USE_PUT_INTO_BELT_SOCKET)
			//{
			//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
			//	return false;
			//}

			if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
			{
				return false;
			}

			switch (item->GetSubType())
			{
#ifdef __ENABLE_ORE_REFACTOR__
				case USE_REMOVE_ACCESSORY_SOCKET:
				{
					//Checking if we have any ores in item && if we can do any changes with curr item
					if (!item2->IsAccessoryForSocket() || item2->GetAccessorySocketGrade() == 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "Remove erros fail # This item has no ores!");
						return false;
					}

					//Define the variables for wchich items will be returned
					const auto& bReturningItems = item->GetValue(4) == 1;
					const auto& bReturningNormalOres = item->GetValue(5) == 1;
					const auto& bReturningPermOres = item->GetValue(5) == 2;

					const auto vReturnList = item2->GetAccessoryItems(bReturningNormalOres, bReturningPermOres);

					if (vReturnList.size() == 0)
					{
						ChatPacket(CHAT_TYPE_INFO, "Remove error fail # The returning list is empty, contact with admin!");
						return false;
					}

					for (const auto& rItem : vReturnList)
					{
						//Reset some of items!
						item2->SetAccessorySocketItem(rItem.first, true, true);

						if (bReturningItems)
							AutoGiveItem(rItem.first, 1);
					}

					int pCount = item2->GetAccessorySocketGrade() - vReturnList.size();
					item2->SetAccessorySocketGrade(std::max(0, pCount));

					item->SetCount(item->GetCount() - 1);

					ChatPacket(CHAT_TYPE_INFO, "Remove ores success!");
				}
				break;
#endif
				case USE_CLEAN_SOCKET:
				{
					int i;
					for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					{
						if (item2->GetSocket(i) == ITEM_BROKEN_METIN_VNUM)
						{
							break;
						}
					}

					if (i == ITEM_SOCKET_MAX_NUM)
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There aren't any Pieces of Broken Stone available for removal."));
						return false;
					}

					int j = 0;

					for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
					{
						if (item2->GetSocket(i) != ITEM_BROKEN_METIN_VNUM && item2->GetSocket(i) != 0)
						{
							item2->SetSocket(j++, item2->GetSocket(i));
						}
					}

					for (; j < ITEM_SOCKET_MAX_NUM; ++j)
					{
						if (item2->GetSocket(j) > 0)
						{
							item2->SetSocket(j, 1);
						}
					}

					{
						char buf[21];
						snprintf(buf, sizeof(buf), "%u", item2->GetID());
						LogManager::instance().ItemLog(this, item, "CLEAN_SOCKET", buf);
					}

					item->SetCount(item->GetCount() - 1);

				}
				break;

			case USE_CHANGE_ATTRIBUTE :
			case USE_CHANGE_ATTRIBUTE2 : // @fixme123
				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				if (item2->GetAttributeCount() == 0)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is no upgrade that you can change."));
					return false;
				}

				if ((GM_PLAYER == GetGMLevel()) && (false == test_server) && (g_dwItemBonusChangeTime > 0))
				{
					DWORD dwChangeItemAttrCycle = g_dwItemBonusChangeTime;

					quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID());

					if (pPC)
					{
						DWORD dwNowSec = get_global_time();

						DWORD dwLastChangeItemAttrSec = pPC->GetFlag(msc_szLastChangeItemAttrFlag);

						if (dwLastChangeItemAttrSec + dwChangeItemAttrCycle > dwNowSec)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only do this %d minutes after an upgrade. (%d minutes left)"),
									   dwChangeItemAttrCycle, dwChangeItemAttrCycle - (dwNowSec - dwLastChangeItemAttrSec));
							return false;
						}

						pPC->SetFlag(msc_szLastChangeItemAttrFlag, dwNowSec);
					}
				}

				if (item->GetSubType() == USE_CHANGE_ATTRIBUTE2)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 0, 30, 40, 3
					};

					item2->ChangeAttribute(aiChangeProb);
				}
				else if (item->GetVnum() == 76014)
				{
					int aiChangeProb[ITEM_ATTRIBUTE_MAX_LEVEL] =
					{
						0, 10, 50, 39, 1
					};

					item2->ChangeAttribute(aiChangeProb);
				}

				else
				{


					if (item->GetVnum() == 71151 || item->GetVnum() == 76023)
					{
						if ((item2->GetType() == ITEM_WEAPON)
								|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() != ARMOR_PENDANT))
						{
							bool bCanUse = true;
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
							{
								if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
								{
									bCanUse = false;
									break;
								}
							}
							if (false == bCanUse)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used on weapons or armor up to level 40."));
								break;
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be used on accesories."));
							break;
						}
					}
					item2->ChangeAttribute();
				}

				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have changed the upgrade."));
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());
					LogManager::instance().ItemLog(this, item, "CHANGE_ATTRIBUTE", buf);
				}

#ifdef __BATTLE_PASS_ENABLE__
				CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_USE, item->GetVnum(), 1 });
#endif

				item->SetCount(item->GetCount() - 1);
				break;

			case USE_ADD_ATTRIBUTE :
			{
				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}

				bool isAddAll = item->GetSubType() == USE_ADD_ATTRIBUTE && item->GetValue(1) == 1;

				if ((item2->GetAttributeCount() < 4 || (isAddAll && item2->GetAttributeCount() < 5)))
				{
					if (item->GetVnum() == 71152 || item->GetVnum() == 76024)
					{
						if ((item2->GetType() == ITEM_WEAPON)
								|| (item2->GetType() == ITEM_ARMOR && item2->GetSubType() != ARMOR_PENDANT))
						{
							bool bCanUse = true;
							for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
							{
								if (item2->GetLimitType(i) == LIMIT_LEVEL && item2->GetLimitValue(i) > 40)
								{
									bCanUse = false;
									break;
								}
							}
							if (false == bCanUse)
							{
								ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used on weapons or armor up to level 40."));
								break;
							}
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be used on accesories."));
							break;
						}
					}
					char buf[50];
					snprintf(buf, sizeof(buf), "%u add_all %d", item2->GetID(), isAddAll);

					if (isAddAll || number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
					{
						if (isAddAll)
						{
							while (item2->GetAttributeCount() < 5)
							{
								item2->AddAttribute();
							}
						}
						else
						{
							item2->AddAttribute();
						}
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));

						int iAddedIdx = item2->GetAttributeCount() - 1;
						LogManager::instance().ItemLog(
							GetPlayerID(),
							item2->GetAttributeType(iAddedIdx),
							item2->GetAttributeValue(iAddedIdx),
							item->GetID(),
							"ADD_ATTRIBUTE_SUCCESS",
							buf,
							GetDesc()->GetHostName(),
							item->GetOriginalVnum());
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No upgrade added."));
						LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE_FAIL", buf);
					}

					if (!isAddAll)
					{
						item->SetCount(item->GetCount() - 1);
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You must use the Blessing Marble in order to add another bonus to this item."));
				}
			}
			break;

			case USE_ADD_ATTRIBUTE2 :


				if (item2->GetAttributeSetIndex() == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the upgrade of this item."));
					return false;
				}


				if (item2->GetAttributeCount() == 4)
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					if (number(1, 100) <= aiItemAttributeAddPercent[item2->GetAttributeCount()])
					{
						item2->AddAttribute();
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Upgrade successfully added."));

						int iAddedIdx = item2->GetAttributeCount() - 1;
						LogManager::instance().ItemLog(
							GetPlayerID(),
							item2->GetAttributeType(iAddedIdx),
							item2->GetAttributeValue(iAddedIdx),
							item->GetID(),
							"ADD_ATTRIBUTE2_SUCCESS",
							buf,
							GetDesc()->GetHostName(),
							item->GetOriginalVnum());
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No upgrade added."));
						LogManager::instance().ItemLog(this, item, "ADD_ATTRIBUTE2_FAIL", buf);
					}

					item->SetCount(item->GetCount() - 1);
				}
				else if (item2->GetAttributeCount() == 5)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can no longer be improved. The maximum number of bonuses has been reached."));
				}
				else if (item2->GetAttributeCount() < 4)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can only use the Blessing Marble with an item which already has 4 bonuses."));
				}
				else
				{
					// wtf ?!
					sys_err("ADD_ATTRIBUTE2 : Item has wrong AttributeCount(%d)", item2->GetAttributeCount());
				}
				break;

			case USE_ADD_ACCESSORY_SOCKET:
			{
				char buf[21];
				snprintf(buf, sizeof(buf), "%u", item2->GetID());

				if (item2->IsAccessoryForSocket())
				{
					if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
					{
#ifdef ENABLE_ADDSTONE_FAILURE
						if (number(1, 100) <= ACCESSORY_PERCENT_VALUE)
#else
						if (1)
#endif
						{
							item2->SetAccessorySocketMaxGrade(item2->GetAccessorySocketMaxGrade() + 1);
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Socket successfully added."));
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No socket added."));
							LogManager::instance().ItemLog(this, item, "ADD_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No additional sockets could be added to this item."));
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot add a socket to this item."));
				}
			}
			break;

			case USE_PUT_INTO_BELT_SOCKET:
			case USE_PUT_INTO_ACCESSORY_SOCKET:
				if (item2->IsAccessoryForSocket() && item->CanPutInto(item2))
				{
					char buf[21];
					snprintf(buf, sizeof(buf), "%u", item2->GetID());

					if (item2->GetAccessorySocketGrade() < item2->GetAccessorySocketMaxGrade())
					{
						if (number(1, 100) <= aiAccessorySocketPutPct[item2->GetAccessorySocketGrade()])
						{
#ifdef __ENABLE_ORE_REFACTOR__
							item2->SetAccessorySocketItem(item->GetVnum());
#endif

							item2->SetAccessorySocketGrade(item2->GetAccessorySocketGrade() + 1);

							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming successful."));
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_SUCCESS", buf);
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Arming has failed."));
							LogManager::instance().ItemLog(this, item, "PUT_SOCKET_FAIL", buf);
						}

						item->SetCount(item->GetCount() - 1);
					}
					else
					{
						if (item2->GetAccessorySocketMaxGrade() == 0)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to add a socket first. Use a diamond in order to do this."));
						}
						else if (item2->GetAccessorySocketMaxGrade() < ITEM_ACCESSORY_SOCKET_MAX_NUM)
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There are no sockets for gemstones in this item."));
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to add a socket if you want to use a Diamond."));
						}
						else
						{
							ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No more gems can be added to this item."));
						}
					}
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("These items cannot be used together."));
				}
				break;
			}
			if (item2->IsEquipped())
			{
				BuffOnAttr_AddBuffsFromItem(item2);
			}
		}
		break;
		//  END_OF_ACCESSORY_REFINE & END_OF_ADD_ATTRIBUTES & END_OF_CHANGE_ATTRIBUTES

		case USE_BAIT:
		{

			if (m_pkFishingEvent)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the Bait whilst fishing."));
				return false;
			}

			LPITEM weapon = GetWear(WEAR_WEAPON);

			if (!weapon || weapon->GetType() != ITEM_ROD)
			{
				return false;
			}

			if (weapon->GetSocket(2))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are exchanging the current Bait for %s."), item->GetName());
			}
			else
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You attached %s to the hook as bait."), item->GetName());
			}

			weapon->SetSocket(2, item->GetValue(0));
			item->SetCount(item->GetCount() - 1);
		}
		break;

		case USE_MOVE:
		case USE_TREASURE_BOX:
		case USE_MONEYBAG:
			break;

		case USE_AFFECT :
		{
			if (FindAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			}
			else
			{
				// PC_BANG_ITEM_ADD
				if (item->IsPCBangItem() == true)
				{

					if (CPCBangManager::instance().IsPCBangIP(GetDesc()->GetHostName()) == false)
					{

						ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item can only be used in an Internet cafe."));
						return false;
					}
				}
				// END_PC_BANG_ITEM_ADD

				AddAffect(item->GetValue(0), aApplyInfo[item->GetValue(1)].bPointType, item->GetValue(2), 0, item->GetValue(3), 0, false);
				item->SetCount(item->GetCount() - 1);
			}
		}
		break;

		case USE_CREATE_STONE:
			AutoGiveItem(number(28000, 28013));
			item->SetCount(item->GetCount() - 1);
			break;


		case USE_RECIPE :
		{
			LPITEM pSource1 = FindSpecifyItem(item->GetValue(1));
			DWORD dwSourceCount1 = item->GetValue(2);

			LPITEM pSource2 = FindSpecifyItem(item->GetValue(3));
			DWORD dwSourceCount2 = item->GetValue(4);

			if (dwSourceCount1 != 0)
			{
				if (pSource1 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are missing some ingredients to make the potion."));
					return false;
				}
			}

			if (dwSourceCount2 != 0)
			{
				if (pSource2 == NULL)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are missing some ingredients to make the potion."));
					return false;
				}
			}

			if (pSource1 != NULL)
			{
				if (pSource1->GetCount() < dwSourceCount1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough material (%s)."), pSource1->GetName());
					return false;
				}

				pSource1->SetCount(pSource1->GetCount() - dwSourceCount1);
			}

			if (pSource2 != NULL)
			{
				if (pSource2->GetCount() < dwSourceCount2)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough material (%s)."), pSource2->GetName());
					return false;
				}

				pSource2->SetCount(pSource2->GetCount() - dwSourceCount2);
			}

			LPITEM pBottle = FindSpecifyItem(50901);

			if (!pBottle || pBottle->GetCount() < 1)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You do not have enough empty bottles."));
				return false;
			}

			pBottle->SetCount(pBottle->GetCount() - 1);

			if (number(1, 100) > item->GetValue(5))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The potion production has failed."));
				return false;
			}

			AutoGiveItem(item->GetValue(0));
		}
		break;

#ifdef __ENABLE_OFFLINE_SHOP__
		case USE_OPEN_OFFLINE_SHOP:
			if (!UseItemOpenOfflineShop(item))
			{
				return false;
			}
			break;
#endif
		}
	}
	break;

	case ITEM_METIN:
	{
		LPITEM item2;

		if (!IsValidItemPosition(DestCell) || !(item2 = GetItem(DestCell)))
		{
			return false;
		}

		if (item2->IsExchanging() || item2->IsEquipped()) // @fixme114
		{
			return false;
		}

		if (item2->GetType() == ITEM_PICK) { return false; }
		if (item2->GetType() == ITEM_ROD) { return false; }

		int i;

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		{
			DWORD dwVnum;

			if ((dwVnum = item2->GetSocket(i)) <= 2)
			{
				continue;
			}

			TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

			if (!p)
			{
				continue;
			}

			if (item->GetValue(5) == p->alValues[5])
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach several stones of the same type."));
				return false;
			}
		}

		if (item2->GetType() == ITEM_ARMOR)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_BODY) || !IS_SET(item2->GetWearFlag(), WEARABLE_BODY))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This Spirit Stone cannot be attached to this type of item."));
				return false;
			}
		}
		else if (item2->GetType() == ITEM_WEAPON)
		{
			if (!IS_SET(item->GetWearFlag(), WEARABLE_WEAPON))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot attach this Spirit Stone to a weapon."));
				return false;
			}
		}
		else
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No slot free."));
			return false;
		}

		for (i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			if (item2->GetSocket(i) >= 1 && item2->GetSocket(i) <= 2 && item2->GetSocket(i) >= item->GetValue(2))
			{

#ifdef ENABLE_ADDSTONE_FAILURE
				if (number(1, 100) <= STONE_PERCENT_VALUE)
#else
				if (1)
#endif
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have attached the Spirit Stone successfully."));
					item2->SetSocket(i, item->GetVnum());
#ifdef __LEGENDARY_STONES_ENABLE__
					if (item->GetSocket(0))
						item2->SetSocket(LegendaryStonesHandler::START_SOCKET_VALUES + i, item->GetSocket(0));
#endif
				}
				else
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The Spirit Stone broke while being attached."));
#ifndef __LEGENDARY_STONES_ENABLE__
					item2->SetSocket(i, ITEM_BROKEN_METIN_VNUM);
#endif
				}

				LogManager::instance().ItemLog(this, item2, "SOCKET", item->GetName());
#ifdef __ENABLE_GET_ONE_ITEM_FROM_STACK__
				item->SetCount(item->GetCount() - 1);
#else
				ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (METIN)");
#endif
				break;
			}

		if (i == ITEM_SOCKET_MAX_NUM)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No slot free."));
		}
	}
	break;

	case ITEM_AUTOUSE:
	case ITEM_MATERIAL:
	case ITEM_SPECIAL:
	case ITEM_TOOL:
	case ITEM_LOTTERY:
		break;

	case ITEM_TOTEM:
	{
		if (!item->IsEquipped())
		{
			EquipItem(item);
		}
	}
	break;

	case ITEM_BLEND:

		sys_log(0, "ITEM_BLEND!!");
		if (Blend_Item_find(item->GetVnum()))
		{
			int		affect_type		= AFFECT_BLEND;
			int		apply_type		= aApplyInfo[item->GetSocket(0)].bPointType;
			int		apply_value		= item->GetSocket(1);
			int		apply_duration	= item->GetSocket(2);

			if (FindAffect(affect_type, apply_type))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
			}
			else
			{
				if (FindAffect(AFFECT_EXP_BONUS_EURO_FREE, POINT_RESIST_MAGIC))
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This effect is already activated."));
				}
				else
				{
					AddAffect(affect_type, apply_type, apply_value, 0, apply_duration, 0, false);
					item->SetCount(item->GetCount() - 1);
				}
			}
		}
		break;
	case ITEM_EXTRACT:
	{
		LPITEM pDestItem = GetItem(DestCell);
		if (NULL == pDestItem)
		{
			return false;
		}
		switch (item->GetSubType())
		{
		case EXTRACT_DRAGON_SOUL:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().PullOut(this, NPOS, pDestItem, item);
			}
			return false;
		case EXTRACT_DRAGON_HEART:
			if (pDestItem->IsDragonSoul())
			{
				return DSManager::instance().ExtractDragonHeart(this, pDestItem, item);
			}
			return false;
		default:
			return false;
		}
	}
	break;

#ifdef __ENABLE_LUCKY_BOX__
	case ITEM_LUCKY_BOX:
	{
		if (dwLuckyBoxVnum)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already have a lucky box opened."));
			return false;
		}

		DWORD dwBoxVnum = item->GetVnum();
		const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwBoxVnum);

		if (!pGroup)
		{
			sys_err("LUCKY_BOX: cannot find special item group %d", dwBoxVnum);
			return false;
		}

		int iDefaultPrice = item->GetValue(0);

		TPacketGCLuckyBox pack;
		pack.bHeader = HEADER_GC_LUCKY_BOX;
		pack.dwBoxVnum = dwBoxVnum;
		pack.dwPrice = iDefaultPrice;
		pack.bIsOpen = true;

		int iRewardIndex = pGroup->GetOneIndex();
		if (iRewardIndex > LUCKY_BOX_MAX_NUM)
		{
			sys_err("LUCKY_BOX: wrong lucky box: reward index %d", iRewardIndex);
			return false;
		}

		dwLuckyBoxVnum = dwBoxVnum;
		bLuckyBoxRewardIndex = iRewardIndex;
		bLuckyBoxOpenCount = 1;
		dwLuckyBoxPrice = iDefaultPrice;

		//Fix not remove all in stack, just one
		//ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (LUCKY_BOX)");
		item->SetCount(item->GetCount() - 1);

		for (int i = 0; i < LUCKY_BOX_MAX_NUM; i++) // Original = pGroup->GetGroupSize()
		{
			pack.luckyItems[i].dwVnum = (i < pGroup->GetGroupSize()) ? pGroup->GetVnum(i) : 0;
			pack.luckyItems[i].bCount = (i < pGroup->GetGroupSize()) ? pGroup->GetCount(i) : 0;
			pack.luckyItems[i].bIsReward = (i < pGroup->GetGroupSize()) ? ((iRewardIndex == i ? true : false)) : false;
		}

		GetDesc()->Packet(&pack, sizeof(TPacketGCLuckyBox));
	}
	break;
#endif

#ifdef __ITEM_TOGGLE_SYSTEM__
	case ITEM_TOGGLE:
		if (!OnUseItem(this, item))
		{
			return false;
		}
		break;
#endif

	case ITEM_NONE:
		sys_err("Item type NONE %s", item->GetName());
		break;

	default:
		sys_log(0, "UseItemEx: Unknown type %s %d", item->GetName(), item->GetType());
		return false;
	}

	return true;
}

int g_nPortalLimitTime = 10;

bool CHARACTER::UseItem(TItemPos Cell, TItemPos DestCell)
{
	WORD wCell = Cell.cell;
	BYTE window_type = Cell.window_type;
	LPITEM item;

	if (!CanHandleItem())
	{
		return false;
	}

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
	{
		return false;
	}

	sys_log(0, "%s: USE_ITEM %s (inven %d, cell: %d)", GetName(), item->GetName(), window_type, wCell);

	if (item->IsExchanging())
	{
		return false;
	}

	// We don't want to use it if we are dragging it over another item of the
	// same type...
	auto* destItem = GetItem(DestCell);
	if (destItem && item != destItem && destItem->IsStackable() &&
		!IS_SET(destItem->GetAntiFlag(), ITEM_ANTIFLAG_STACK) &&
		destItem->GetVnum() == item->GetVnum()) {
		if (MoveItem(Cell, DestCell, 0))
			return false;
	}

#ifdef __ACTION_RESTRICTIONS__
	static const DWORD m_tableItems[] =
	{
		80003
	};

	static const DWORD m_tableItemTypes[] = { 23, 3 };

	bool bPassed = true;

	for (auto it : m_tableItems) if (it == item->GetVnum()) { bPassed = false; }
	for (auto it : m_tableItemTypes) if (it == item->GetType()) { bPassed = false; }
	if (bPassed)
	{
		if (GetActionRestrictions())
		{
			if (GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_USE_ITEM) == false) { return false; }
		}
	}
#endif

#ifdef __ENABLE_SWITCHBOT__
	if (Cell.IsSwitchbotPosition())
	{
		CSwitchbot* pkSwitchbot = CSwitchbotManager::Instance().FindSwitchbot(GetPlayerID());
		if (pkSwitchbot && pkSwitchbot->IsActive(Cell.cell))
		{
			return false;
		}

#ifndef __SPECIAL_STORAGE_ENABLE__
		int iEmptyCell = GetEmptyInventory(item->GetSize());
#else
		int iEmptyCell = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

		if (iEmptyCell == -1)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot remove item from switchbot. Inventory is full."));
			return false;
		}

		MoveItem(Cell, TItemPos(INVENTORY, iEmptyCell), item->GetCount());
		return true;
	}
#endif

	if (!item->CanUsedBy(this))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item because you do not fulfil all the requirements."));
		return false;
	}

	if (IsStun())
	{
		return false;
	}

	if (false == FN_check_item_sex(this, item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not able to use that item because you do not have the right gender."));
		return false;
	}

	//PREVENT_TRADE_WINDOW
	if (IS_SUMMON_ITEM(item->GetVnum()))
	{
		if (false == IS_SUMMONABLE_ZONE(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This function is not available right now."));
			return false;
		}




		if (CThreeWayWar::instance().IsThreeWayWarMapIndex(GetMapIndex()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a Scroll of the Location whilst taking part in a kingdom battle."));
			return false;
		}
		int iPulse = thecore_pulse();


		if (iPulse - GetSafeboxLoadTime() < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening the Storeroom you cannot use a Scroll of the Location for %d seconds."), g_nPortalLimitTime);

			if (test_server)
			{
				ChatPacket(CHAT_TYPE_INFO, "[TestOnly]Pulse %d LoadTime %d PASS %d", iPulse, GetSafeboxLoadTime(), PASSES_PER_SEC(g_nPortalLimitTime));
			}
			return false;
		}

		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use a Scroll of the Location while another window is open."));
			return false;
		}

		//PREVENT_REFINE_HACK

		{
			if (iPulse - GetRefineTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade, you cannot use a scroll for another %d seconds."), g_nPortalLimitTime);
				return false;
			}
		}
		//END_PREVENT_REFINE_HACK


		//PREVENT_ITEM_COPY
		{
			if (iPulse - GetMyShopTime() < PASSES_PER_SEC(g_nPortalLimitTime))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After opening a storeroom you cannot use a Scroll of the Location for another %d seconds."), g_nPortalLimitTime);
				return false;
			}

		}
		//END_PREVENT_ITEM_COPY



		if (item->GetVnum() != 70302)
		{
			PIXEL_POSITION posWarp;

			int x = 0;
			int y = 0;

			double nDist = 0;
			const double nDistant = 5000.0;

			if (item->GetVnum() == 22010)
			{
				x = item->GetSocket(0) - GetX();
				y = item->GetSocket(1) - GetY();
			}

			else if (item->GetVnum() == 22000)
			{
				SECTREE_MANAGER::instance().GetRecallPositionByEmpire(GetMapIndex(), GetEmpire(), posWarp);

				if (item->GetSocket(0) == 0)
				{
					x = posWarp.x - GetX();
					y = posWarp.y - GetY();
				}
				else
				{
					x = item->GetSocket(0) - GetX();
					y = item->GetSocket(1) - GetY();
				}
			}

			nDist = sqrt(pow((float)x, 2) + pow((float)y, 2));

			if (nDistant > nDist)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use the Scroll of the Location because the distance is too small."));
				if (test_server)
				{
					ChatPacket(CHAT_TYPE_INFO, "PossibleDistant %f nNowDist %f", nDistant, nDist);
				}
				return false;
			}
		}

		//PREVENT_PORTAL_AFTER_EXCHANGE

		if (iPulse - GetExchangeTime()  < PASSES_PER_SEC(g_nPortalLimitTime))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("After a trade you cannot use a Scroll of the Location for %d seconds."), g_nPortalLimitTime);
			return false;
		}
		//END_PREVENT_PORTAL_AFTER_EXCHANGE

	}


	if ((item->GetVnum() == 50200) || (item->GetVnum() == 71049))
	{
		if (GetExchange() || GetMyShop() || GetShopOwner() || IsOpenSafebox() || IsCubeOpen())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot open the Storeroom if another window is already open."));
			return false;
		}

	}
	//END_PREVENT_TRADE_WINDOW

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item if you're using quests"));
		return false;
	}
	// @fixme150 END

#ifdef __ENABLE_TICKET_COINS__
	InsertCoins(item);
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	InsertGayaPoints(item);
#endif

#ifdef __BATTLE_PASS_ENABLE__
	BattlePassActivation(item);
#endif

#ifdef __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
	switch (UnlockEquipmentSlot(item->GetVnum()))
	{
		case -1:
			ChatPacket(CHAT_TYPE_INFO, "Equipment unlocking internal error");
			break;

		case -2:
			ChatPacket(CHAT_TYPE_INFO, "You have already unlocked slot by this item.");
			break;

		case -3:
			ChatPacket(CHAT_TYPE_INFO, "You doesn't have required item to unlock slot.");
			break;

		case 1:
			ChatPacket(CHAT_TYPE_INFO, "You unlocked slot successfull.");
			break;

		default:
			break;
	}
#endif


	bool ret = false;
	DWORD dwVnum = item->GetVnum();

	if (IS_SET(item->GetFlag(), ITEM_FLAG_LOG))
	{
		DWORD vid = item->GetVID();
		DWORD oldCount = item->GetCount();
		DWORD vnum = item->GetVnum();

		char hint[ITEM_NAME_MAX_LEN + 32 + 1];
		int len = snprintf(hint, sizeof(hint) - 32, "%s", item->GetName());

		if (len < 0 || len >= (int) sizeof(hint) - 32)
		{
			len = (sizeof(hint) - 32) - 1;
		}

		ret = UseItemEx(item, DestCell);

		if (NULL == ITEM_MANAGER::instance().FindByVID(vid))
		{
			LogManager::instance().ItemLog(this, vid, vnum, "REMOVE", hint);
		}
		else if (oldCount != item->GetCount())
		{
			snprintf(hint + len, sizeof(hint) - len, " %u", oldCount - 1);
			LogManager::instance().ItemLog(this, vid, vnum, "USE_ITEM", hint);
		}
	}
	else
	{
		ret = UseItemEx(item, DestCell);
	}

#ifdef __BATTLE_PASS_ENABLE__
	if (ret)
		CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_USE, dwVnum, 1 });
#endif

	return ret;
}

bool CHARACTER::DropItem(TItemPos Cell, CountType bCount)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}
		return false;
	}

#ifdef __ENABLE_NEWSTUFF__
	if (0 != g_ItemDropTimeLimitValue)
	{
		if (get_dword_time() < m_dwLastItemDropTime + g_ItemDropTimeLimitValue)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot drop item right now."));
			return false;
		}
	}

	m_dwLastItemDropTime = get_dword_time();
#endif

	if (IsDead())
	{
		return false;
	}

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
	{
		return false;
	}

#ifdef __ADMIN_MANAGER__
	if ((!item->IsGMOwner() && !GM::check_allow(GetGMLevel(), GM_ALLOW_DROP_PLAYER_ITEM)) ||
			(item->IsGMOwner() && !GM::check_allow(GetGMLevel(), GM_ALLOW_DROP_GM_ITEM)))
	{
		ChatPacket(CHAT_TYPE_INFO, "You cannot do this with this gamemaster rank.");
		return false;
	}
#endif

	if (item->IsExchanging())
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP | ITEM_ANTIFLAG_GIVE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot drop this item."));
		return false;
	}

#ifdef __ACTION_RESTRICTIONS__
	if (GetActionRestrictions())
	{
		if (GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_DROP_ITEM) == false) { return false; }
	}
#endif

	if (bCount == 0 || bCount > item->GetCount())
		bCount = item->GetCount();

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	LPITEM pkItemToDrop;

	if (bCount == item->GetCount())
	{
		item->RemoveFromCharacter();
		pkItemToDrop = item;
	}
	else
	{
		if (bCount == 0)
		{
			if (test_server)
			{
				sys_log(0, "[DROP_ITEM] drop item count == 0");
			}
			return false;
		}

		item->SetCount(item->GetCount() - bCount);
		ITEM_MANAGER::instance().FlushDelayedSave(item);

		pkItemToDrop = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), bCount);

		// copy item socket -- by mhh
		FN_copy_item_socket(pkItemToDrop, item);

		char szBuf[51 + 1];
		snprintf(szBuf, sizeof(szBuf), "%u %u", pkItemToDrop->GetID(), pkItemToDrop->GetCount());
		LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
	}

	PIXEL_POSITION pxPos = GetXYZ();

	if (pkItemToDrop->AddToGround(GetMapIndex(), pxPos))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The dropped item will vanish in 5 minutes."));
#ifdef __ENABLE_NEWSTUFF__
		pkItemToDrop->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_DROPITEM]);
#else
		pkItemToDrop->StartDestroyEvent();
#endif

		ITEM_MANAGER::instance().FlushDelayedSave(pkItemToDrop);

		char szHint[32 + 1];
		snprintf(szHint, sizeof(szHint), "%s %u %u", pkItemToDrop->GetName(), pkItemToDrop->GetCount(), pkItemToDrop->GetOriginalVnum());
		LogManager::instance().ItemLog(this, pkItemToDrop, "DROP", szHint);
		//Motion(MOTION_PICKUP);
	}

	return true;
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	bool CHARACTER::DropGold(int64_t gold)
#else
	bool CHARACTER::DropGold(int gold)
#endif
{
	return false;
}

bool CHARACTER::MoveItem(TItemPos Cell, TItemPos DestCell, CountType count)
{
	LPITEM item = NULL;

	if (!IsValidItemPosition(Cell))
	{
		return false;
	}

	if (!(item = GetItem(Cell)))
	{
		return false;
	}

	if (item->IsExchanging())
	{
		return false;
	}

	if (item->GetCount() < count)
	{
		return false;
	}

#ifndef __SPECIAL_STORAGE_ENABLE__
	if (INVENTORY == Cell.window_type && Cell.cell >= INVENTORY_MAX_NUM && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		return false;
	}
#else
	if (INVENTORY == Cell.window_type && (Cell.cell >= INVENTORY_MAX_NUM && Cell.cell < SPECIAL_STORAGE_START_CELL) && IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		return false;
	}
#endif

	if (DestCell == Cell)
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (!IsValidItemPosition(DestCell))
	{

		return false;
	}

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}
		return false;
	}


	if (DestCell.IsBeltInventoryPosition() && false == CBeltInventoryHelper::CanMoveIntoBeltInventory(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this item in your belt inventory."));
		return false;
	}

#ifdef __ENABLE_SWITCHBOT__
	if (Cell.IsSwitchbotPosition() && CSwitchbotManager::Instance().IsActive(GetPlayerID(), Cell.cell))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move active switchbot item."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && !SwitchbotHelper::IsValidItem(item))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Invalid item type for switchbot."));
		return false;
	}

	if (Cell.IsSwitchbotPosition() && DestCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot equip items directly from switchbot."));
		return false;
	}

	if (DestCell.IsSwitchbotPosition() && Cell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Cannot move equipped items to switchbot."));
		return false;
	}
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
	// Checking if item can be transferred to this window
	if (DestCell.IsSpecialStorage() && DestCell.GetSpecialStorageType() != item->GetVirtualWindow())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_TYPE_MISMATCH"));
		return false;
	}
	// Special storage but not same type
	else if (DestCell.IsSpecialStorage() && Cell.IsSpecialStorage() && DestCell.GetSpecialStorageType() != Cell.GetSpecialStorageType())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_TYPE_MISMATCH"));
		return false;
	}
	// Cannot unequip to special storage
	else if (DestCell.IsSpecialStorage() && Cell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_CANNOT_UNEQUIP"));
		return false;
	}
	// Cannot equip from special storage
	else if (Cell.IsSpecialStorage() && DestCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_CANNOT_EQUIP"));
		return false;
	}
#endif

	if (Cell.IsEquipPosition())
	{
		if (!CanUnequipNow(item))
		{
			return false;
		}

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
		int iWearCell = item->FindEquipCell(this);
		if (iWearCell == WEAR_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}

			if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
			{
				return UnequipItem(item);
			}
		}
#endif
	}

	if (DestCell.IsEquipPosition())
	{
		if (GetItem(DestCell))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already equipped this kind of Dragon Stone."));

			return false;
		}

		EquipItem(item, DestCell.cell - INVENTORY_MAX_NUM);
	}
	else
	{
		if (item->IsDragonSoul())
		{
			if (item->IsEquipped())
			{
				return DSManager::instance().PullOut(this, DestCell, item);
			}
			else
			{
				if (DestCell.window_type != DRAGON_SOUL_INVENTORY)
				{
					return false;
				}

				if (!DSManager::instance().IsValidCellForThisItem(item, DestCell))
				{
					return false;
				}
			}
		}

		else if (DRAGON_SOUL_INVENTORY == DestCell.window_type)
		{
			return false;
		}

		LPITEM item2;

		if ((item2 = GetItem(DestCell)) && item->CanStackWith(item2))
		{
			if (count == 0)
			{
				count = item->GetCount();
			}

			count = MIN(g_bItemCountLimit - item2->GetCount(), count);

			sys_log(0, "%s: ITEM_STACK %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d itemSrcCount %d itemDstCount %d", GetName(), item->GetBaseName(), Cell.window_type, Cell.cell,
					DestCell.window_type, DestCell.cell, count, item->GetCount(), item2->GetCount());

			item->SetCount(item->GetCount() - count);
			item2->SetCount(item2->GetCount() + count);
			return true;
		}

		if (!IsEmptyItemGrid(DestCell, item->GetSize(), Cell.cell))
		{
			if (!Cell.IsDefaultInventoryPosition())
			{
				return false;
			}

			// check if switching is possible
			if (count == 0 || count >= item->GetCount())
			{
				// check if there is an item out of bounds in top side
				if (!IsEmptyItemGrid(DestCell, 1, Cell.cell) && !GetItem(DestCell))
				{
					return false;
				}
				// check if there is an item out of bounds in bottom side
				int row = (DestCell.cell % INVENTORY_PAGE_SIZE) / INVENTORY_PAGE_COLUMN;
				TItemPos belowDstPos(DestCell.window_type, DestCell.cell + INVENTORY_PAGE_COLUMN * item->GetSize());
				if (row + item->GetSize() < INVENTORY_PAGE_ROW &&
						!IsEmptyItemGrid(belowDstPos, 1, Cell.cell) &&
						!GetItem(belowDstPos))
				{
					return false;
				}
				// check if dstPos overlaps srcPos
				if (belowDstPos.cell - INVENTORY_PAGE_COLUMN == Cell.cell ||
						(
							(item->GetSize() > 1 || GetItem(DestCell)->GetSize() > 1) &&
							(belowDstPos.cell - INVENTORY_PAGE_COLUMN == Cell.cell + INVENTORY_PAGE_COLUMN)
						))
				{
					return false;
				}

				// check if item can be moved to dest position
				if (row + item->GetSize() > INVENTORY_PAGE_ROW)
				{
					return false;
				}

				// switch items
				// get items at dest pos
				std::vector<LPITEM> vecDestItems;
				for (int i = 0; i < item->GetSize(); ++i)
				{
					LPITEM itemCur = GetItem(TItemPos(DestCell.window_type, DestCell.cell + i * INVENTORY_PAGE_COLUMN));
					if (itemCur && (itemCur->IsExchanging() || itemCur->isLocked()))
					{
						return false;
					}

					vecDestItems.push_back(itemCur);
				}

				// remove items from character
				for (LPITEM itemCur : vecDestItems)
				{
					if (itemCur)
					{
						itemCur->RemoveFromCharacter();
					}
				}
				item->RemoveFromCharacter();

				// add items to character
				for (int i = 0; i < vecDestItems.size(); ++i)
				{
					LPITEM itemCur = vecDestItems[i];
					if (itemCur)
					{
						itemCur->AddToCharacter(this, TItemPos(Cell.window_type, Cell.cell + i * INVENTORY_PAGE_COLUMN));
					}
				}
				item->AddToCharacter(this, DestCell);

				return true;
			}

			return false;
		}


		if (count == 0 || count >= item->GetCount() || !item->IsStackable() || IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
		{
			sys_log(0, "%s: ITEM_MOVE %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
					DestCell.window_type, DestCell.cell, count);

			item->RemoveFromCharacter();
#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
			SetItem(DestCell, item, true);
#else
			SetItem(DestCell, item);
#endif
			if (INVENTORY == Cell.window_type && INVENTORY == DestCell.window_type)
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, DestCell.cell);
			}
		}
		else if (count < item->GetCount())
		{

			sys_log(0, "%s: ITEM_SPLIT %s (window: %d, cell : %d) -> (window:%d, cell %d) count %d", GetName(), item->GetName(), Cell.window_type, Cell.cell,
					DestCell.window_type, DestCell.cell, count);

			item->SetCount(item->GetCount() - count);
			LPITEM item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum(), count);

			// copy socket -- by mhh
			FN_copy_item_socket(item2, item);

			item2->AddToCharacter(this, DestCell);

			char szBuf[51 + 1];
			snprintf(szBuf, sizeof(szBuf), "%u %u %u %u ", item2->GetID(), item2->GetCount(), item->GetCount(), item->GetCount() + item2->GetCount());
			LogManager::instance().ItemLog(this, item, "ITEM_SPLIT", szBuf);
		}
	}

	return true;
}

namespace NPartyPickupDistribute
{
struct FFindOwnership
{
	uint32_t pid;
	CHARACTER* owner;

	FFindOwnership(uint32_t pid)
		: pid(pid), owner(NULL)
	{
	}

	void operator () (LPCHARACTER ch)
	{
		if (ch->GetPlayerID() == pid)
		{
			owner = ch;
		}
	}
};

struct FCountNearMember
{
	int		total;
	int		x, y;

	FCountNearMember(LPCHARACTER center )
		: total(0), x(center->GetX()), y(center->GetY())
	{
	}

	void operator () (LPCHARACTER ch)
	{
		if (DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
		{
			total += 1;
		}
	}
};

struct FMoneyDistributor
{
	int		total;
	LPCHARACTER	c;
	int		x, y;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t	iMoney;
#else
	int		iMoney;
#endif

	FMoneyDistributor(LPCHARACTER center, int iMoney)
		: total(0), c(center), x(center->GetX()), y(center->GetY()), iMoney(iMoney)
	{
	}

	void operator ()(LPCHARACTER ch)
	{
		if (ch != c && DISTANCE_APPROX(ch->GetX() - x, ch->GetY() - y) <= PARTY_DEFAULT_RANGE)
		{
			ch->PointChange(POINT_GOLD, iMoney, true);
		}
	}
};
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	void CHARACTER::GiveGold(int64_t iAmount)
#else
	void CHARACTER::GiveGold(int iAmount)
#endif
{
	if (iAmount <= 0)
	{
		return;
	}

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, static_cast<uint64_t>(iAmount) });
#endif

#ifdef __EXTANDED_GOLD_AMOUNT__
	sys_log(0, "GIVE_GOLD: %s %lld", GetName(), iAmount);
#else
	sys_log(0, "GIVE_GOLD: %s %d", GetName(), iAmount);
#endif

	if (GetParty())
	{
		LPPARTY pParty = GetParty();

#ifdef __EXTANDED_GOLD_AMOUNT__
		int64_t myAmount = iAmount;
#else
		DWORD dwMyAmount = dwTotal;
#endif

		NPartyPickupDistribute::FCountNearMember funcCountNearMember(this);
		pParty->ForEachOnlineMember(funcCountNearMember);

		if (funcCountNearMember.total > 1)
		{
#ifdef __EXTANDED_GOLD_AMOUNT__
			int64_t share = iAmount / funcCountNearMember.total;
#else
			DWORD dwShare = iAmount / funcCountNearMember.total;
#endif
			myAmount -= share * (funcCountNearMember.total - 1);

			NPartyPickupDistribute::FMoneyDistributor funcMoneyDist(this, share);

			pParty->ForEachOnlineMember(funcMoneyDist);
		}

		PointChange(POINT_GOLD, myAmount, true);
	}
	else
	{
		PointChange(POINT_GOLD, iAmount, true);
	}
}

bool CHARACTER::PickupItem(DWORD dwVID)
{
	auto item = ITEM_MANAGER::instance().FindByVID(dwVID);
	if (!item || !item->GetSectree())
	{
		return false;
	}

	if (IsObserverMode())
	{
		return false;
	}

#ifdef __ABUSE_CONTROLLER_ENABLE__
	if (IsPC())
	{
		const auto abuseController = GetAbuseController();
		if (IsDead())
		{
			abuseController->DeadPickup();
			return false;
		}

		if (!abuseController->CanPickupItem())
		{
			abuseController->AutoPickupSuspect();
			return false;
		}
	}
#endif

	if (!(item->DistanceValid(this)))
		return false;

	const auto ownerPid = item->GetOwnerPid();
	CHARACTER* owner = nullptr;

	if (ownerPid == 0 || ownerPid == GetPlayerID()) {
		owner = this;
	}
	else {
		if (!GetParty() || IS_SET(item->GetAntiFlag(),
			ITEM_ANTIFLAG_GIVE | ITEM_ANTIFLAG_DROP))
			return false;

		NPartyPickupDistribute::FFindOwnership f(ownerPid);
		GetParty()->ForEachOnlineMember(f);

		if (!f.owner && GetParty()->IsMember(ownerPid))
			owner = this;
		else if (f.owner)
			owner = f.owner;
		else
			return false;
	}

	if (item->GetType() == ITEM_ELK) {
		owner->GiveGold(item->GetCount());

		item->RemoveFromGround();
		M2_DESTROY_ITEM(item);

		owner->Save();
		return true;
	}

	// @fixme150 BEGIN
	if (item->GetType() == ITEM_QUEST)
	{
		if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot pickup this item if you're using quests"));
			return false;
	}
	}
	// @fixme150 END

	CountType bCount = item->GetCount();

	if (item->IsStackable()) {
#ifndef __SPECIAL_STORAGE_ENABLE__
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#else
	for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#endif
	{

		CItem* item2 = GetInventoryItem(i);
		if (!item2)
		{
			continue;
		}

#ifdef __SPECIAL_STORAGE_ENABLE__
		// We ought to skip equipped items
		if (item2->GetCell() >= INVENTORY_MAX_NUM && item2->GetCell() < SPECIAL_STORAGE_START_CELL)
		{
			continue;
		}
#endif

		if (!CanStack(item, item2))
			continue;

		CountType count2 = std::min<CountType>(GetItemMaxCount(item2) - item2->GetCount(), bCount);
		bCount -= count2;

		item2->SetCount(item2->GetCount() + count2);

		if (bCount == 0)
		{
#ifdef __ENABLE_NEW_LOGS_CHAT__
			CChatLogs::SendChatLogInformation(owner, ELogsType::LOG_TYPE_PICKUP, static_cast<int>(count2), static_cast<int>(item->GetVnum()));
#else
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item2->GetName());
#endif
			M2_DESTROY_ITEM(item);
			item = item2;
			break;
		}
	}

	// Only update the count if the original item isn't gone already
	if (0 != bCount)
		item->SetCount(bCount);
	}

	if (0 != bCount)
	{
		if (item->IsDragonSoul())
		{
			int iEmptyCell = owner->GetEmptyDragonSoulInventory(item);
			if (iEmptyCell == -1)
			{
				sys_log(0, "No empty ds inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
				return false;
			}

			item->RemoveFromGround();
			item->AddToCharacter(owner, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
		}
		else
		{
#ifndef __SPECIAL_STORAGE_ENABLE__
			int iEmptyCell = GetEmptyInventory(item->GetSize();
			if (iEmptyCell == -1)
#else
			int iEmptyCell = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
			if (iEmptyCell == -1)
#endif
			{
				sys_log(0, "No empty inventory pid %u size %ud itemid %u", GetPlayerID(), item->GetSize(), item->GetID());
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have too many items in your inventory."));
				return false;
			}

			item->RemoveFromGround();
			item->AddToCharacter(owner, TItemPos(INVENTORY, iEmptyCell));
		}

		LogManager::instance().ItemLog(owner, item, "GET",
			fmt::format("{} {} {}", item->GetName(),
				item->GetCount(),
				item->GetVnum())
			.c_str());

		if (owner == this)
		{
#ifdef __ENABLE_NEW_LOGS_CHAT__
			CChatLogs::SendChatLogInformation(this, ELogsType::LOG_TYPE_PICKUP, static_cast<int>(item->GetCount()), static_cast<int>(item->GetVnum()));
#else
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());
#endif
		}
		else
		{
#ifdef __ENABLE_NEW_LOGS_CHAT__
			CChatLogs::SendChatLogInformation(owner, ELogsType::LOG_TYPE_PICKUP, static_cast<int>(item->GetCount()), static_cast<int>(item->GetVnum()));
#endif
			owner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s receives %s."), GetName(), item->GetName());
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Item Trade: %s, %s"), owner->GetName(), item->GetName());
		}
	}

	if (item->GetType() == ITEM_QUEST)
		quest::CQuestManager::instance().PickupItem(owner->GetPlayerID(), item);

	return true;
}

#ifdef __ENABLE_DESTROY_ITEM_PACKET__
bool CHARACTER::DestroyItem(TItemPos Cell)
{
	LPITEM item = NULL;

	if (!CanHandleItem())
	{
		if (NULL != DragonSoul_RefineWindow_GetOpener())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot move the item within the refinement window."));
		}
		return false;
	}

	if (IsDead())
	{
		return false;
	}

	if (!IsValidItemPosition(Cell) || !(item = GetItem(Cell)))
	{
		return false;
	}

	if (item->IsExchanging())
	{
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (item->GetCount() <= 0)
	{
		return false;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DESTROY))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot destroy this item."));
		return false;
	}

	if (quest::PC* pPC = quest::CQuestManager::instance().GetPC(GetPlayerID()))
	{
		if (pPC->IsRunning() && GetQuestItemPtr() == item)
		{
			sys_err("cannot delete item that is used in quest");
			return false;
		}
	}

	SyncQuickslot(QUICKSLOT_TYPE_ITEM, Cell.cell, 255);

	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("ITEM_DESTROYED %s"), item->GetName());

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_DESTROY_ITEM, 0, item->GetCount() });
#endif

	ITEM_MANAGER::instance().RemoveItem(item, "RECV_REMOVE");

	return true;
}
#endif

bool CHARACTER::SwapItem(BYTE bCell, BYTE bDestCell)
{
	if (!CanHandleItem())
	{
		return false;
	}

	TItemPos srcCell(INVENTORY, bCell), destCell(INVENTORY, bDestCell);



	//if (bCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM || bDestCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM)
	if (srcCell.IsDragonSoulEquipPosition() || destCell.IsDragonSoulEquipPosition())
	{
		return false;
	}


	if (bCell == bDestCell)
	{
		return false;
	}


	if (srcCell.IsEquipPosition() && destCell.IsEquipPosition())
	{
		return false;
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	// Special storage but not same type
	if (destCell.IsSpecialStorage() && srcCell.IsSpecialStorage() && destCell.GetSpecialStorageType() != srcCell.GetSpecialStorageType())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_TYPE_MISMATCH"));
		return false;
	}
	// Cannot unequip to special storage
	else if (destCell.IsSpecialStorage() && srcCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_CANNOT_UNEQUIP"));
		return false;
	}
	// Cannot equip from special storage
	else if (srcCell.IsSpecialStorage() && destCell.IsEquipPosition())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("SPECIAL_STORAGE_CANNOT_EQUIP"));
		return false;
	}
#endif

	LPITEM item1, item2;


	if (srcCell.IsEquipPosition())
	{
		item1 = GetInventoryItem(bDestCell);
		item2 = GetInventoryItem(bCell);
	}
	else
	{
		item1 = GetInventoryItem(bCell);
		item2 = GetInventoryItem(bDestCell);
	}

	if (!item1 || !item2)
	{
		return false;
	}

	if (item1 == item2)
	{
		sys_log(0, "[WARNING][WARNING][HACK USER!] : %s %d %d", m_stName.c_str(), bCell, bDestCell);
		return false;
	}


	if (!IsEmptyItemGrid(TItemPos (INVENTORY, item1->GetCell()), item2->GetSize(), item1->GetCell()))
	{
		return false;
	}


	if (TItemPos(EQUIPMENT, item2->GetCell()).IsEquipPosition())
	{
		BYTE bEquipCell = item2->GetCell() - INVENTORY_MAX_NUM;
		BYTE bInvenCell = item1->GetCell();


		if (item2->IsDragonSoul() || item2->GetType() == ITEM_BELT) // @fixme117
		{
			if (false == CanUnequipNow(item2) || false == CanEquipNow(item1))
			{
				return false;
			}
		}

		if (bEquipCell != item1->FindEquipCell(this))
		{
			return false;
		}

		item2->RemoveFromCharacter(true);

		if (item1->EquipTo(this, bEquipCell))
		{
			item2->AddToCharacter(this, TItemPos(INVENTORY, bInvenCell));
		}
		else
		{
			sys_err("SwapItem cannot equip %s! item1 %s", item2->GetName(), item1->GetName());
		}
	}
	else
	{
		BYTE bCell1 = item1->GetCell();
		BYTE bCell2 = item2->GetCell();

		item1->RemoveFromCharacter();
		item2->RemoveFromCharacter();

		item1->AddToCharacter(this, TItemPos(INVENTORY, bCell2));
		item2->AddToCharacter(this, TItemPos(INVENTORY, bCell1));
	}

	return true;
}

bool CHARACTER::UnequipItem(LPITEM item)
{
#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	int iWearCell = item->FindEquipCell(this);
	if (iWearCell == WEAR_WEAPON)
	{
		LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
		if (costumeWeapon && !UnequipItem(costumeWeapon))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
			return false;
		}
	}
#endif

	if (false == CanUnequipNow(item))
	{
		return false;
	}

	int pos;
	if (item->IsDragonSoul())
	{
		pos = GetEmptyDragonSoulInventory(item);
	}
	else
#ifndef __SPECIAL_STORAGE_ENABLE__
		pos = GetEmptyInventory(item->GetSize());
#else
		pos = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

	// HARD CODING
	if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
	{
		ShowAlignment(true);
	}

	item->RemoveFromCharacter();
	if (item->IsDragonSoul())
	{
		item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, pos));
	}
	else
	{
		item->AddToCharacter(this, TItemPos(INVENTORY, pos));
	}

	CheckMaximumPoints();

	return true;
}

//

//
bool CHARACTER::EquipItem(LPITEM item, int iCandidateCell)
{
	if (item->IsExchanging())
	{
		return false;
	}

	if (false == item->IsEquipable())
	{
		return false;
	}

	if (false == CanEquipNow(item))
	{
		return false;
	}

	int iWearCell = item->FindEquipCell(this, iCandidateCell);

	if (iWearCell < 0)
	{
		return false;
	}

#ifdef __ENABLE_EQUIPMENT_SLOT_LOCK_SYSTEM__
	if (!EquipmentSlotStatus(iWearCell))
	{
		ChatPacket(CHAT_TYPE_INFO, "You must unlock the slot to use this item.");
		return false;
	}
#endif

#ifdef __ENABLE_FIX_USING_STACK_ITEMS__
	if ((item->GetCount() > 1) && (item->GetSubType() != WEAPON_ARROW))
	{
		int pos = GetEmptyInventory(item->GetSize());
		if (-1 == pos)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough space in your inventory"));
			return false;
		}

		item->SetCount(item->GetCount() - 1);

		const auto item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum());
		item2->AddToCharacter(this, TItemPos(INVENTORY, pos));

		item = item2;
	}
#endif

	if (iWearCell == WEAR_BODY && IsRiding() && (item->GetVnum() >= 11901 && item->GetVnum() <= 11904))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot wear Tuxido on a horse."));
		return false;
	}

#ifndef __ENABLE_EQUIP_ITEMS_WHILE_ATTACKING__
	if (iWearCell != WEAR_ARROW && IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot change the equipped item while you are transformed."));
		return false;
	}
#endif

	if (FN_check_item_sex(this, item) == false)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not able to use that item because you do not have the right gender."));
		return false;
	}

#ifndef __ENABLE_EQUIP_ITEMS_WHILE_ATTACKING__
	if (item->IsRideItem() && IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You're already riding. Get off first."));
		return false;
	}
#endif

	DWORD dwCurTime = get_dword_time();

	if (iWearCell != WEAR_ARROW
			&& (dwCurTime - GetLastAttackTime() <= 1500 || dwCurTime - m_dwLastSkillTime <= 1500))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have to stand still to equip the item."));
		return false;
	}

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
	if (iWearCell == WEAR_WEAPON)
	{
		if (item->GetType() == ITEM_WEAPON)
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && costumeWeapon->GetValue(3) != item->GetSubType() && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
		else //fishrod/pickaxe
		{
			LPITEM costumeWeapon = GetWear(WEAR_COSTUME_WEAPON);
			if (costumeWeapon && !UnequipItem(costumeWeapon))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot unequip the costume weapon. Not enough space."));
				return false;
			}
		}
	}
	else if (iWearCell == WEAR_COSTUME_WEAPON)
	{
		if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
		{
			LPITEM pkWeapon = GetWear(WEAR_WEAPON);
			if (!pkWeapon || pkWeapon->GetType() != ITEM_WEAPON || item->GetValue(3) != pkWeapon->GetSubType())
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip the costume weapon. Wrong equipped weapon."));
				return false;
			}
		}
	}
#endif


	if (item->IsDragonSoul())
	{
		if (GetInventoryItem(INVENTORY_MAX_NUM + iWearCell))
		{
			ChatPacket(CHAT_TYPE_INFO, "You are already carrying a Dragon Stone of this type.");
			return false;
		}

		if (!item->EquipTo(this, iWearCell))
		{
			return false;
		}
	}

	else
	{
		if (GetWear(iWearCell) && !IS_SET(GetWear(iWearCell)->GetFlag(), ITEM_FLAG_IRREMOVABLE))
		{

			if (item->GetWearFlag() == WEARABLE_ABILITY)
			{
				return false;
			}

			if (false == SwapItem(item->GetCell(), INVENTORY_MAX_NUM + iWearCell))
			{
				return false;
			}
		}
		else
		{
			BYTE bOldCell = item->GetCell();

			if (item->EquipTo(this, iWearCell))
			{
				SyncQuickslot(QUICKSLOT_TYPE_ITEM, bOldCell, iWearCell);
			}
		}
	}

	if (true == item->IsEquipped())
	{

		if (-1 != item->GetProto()->cLimitRealTimeFirstUseIndex)
		{

			if (0 == item->GetSocket(1))
			{

				long duration = (0 != item->GetSocket(0)) ? item->GetSocket(0) : item->GetProto()->aLimits[(unsigned char)(item->GetProto()->cLimitRealTimeFirstUseIndex)].lValue;

				if (0 == duration)
				{
					duration = 60 * 60 * 24 * 7;
				}

				item->SetSocket(0, time(0) + duration);
				item->StartRealTimeExpireEvent();
			}

			item->SetSocket(1, item->GetSocket(1) + 1);
		}

		if (item->GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
		{
			ShowAlignment(false);
		}

		const DWORD& dwVnum = item->GetVnum();


		if (true == CItemVnumHelper::IsRamadanMoonRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_RAMADAN_RING);
		}

		else if (true == CItemVnumHelper::IsHalloweenCandy(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HALLOWEEN_CANDY);
		}

		else if (true == CItemVnumHelper::IsHappinessRing(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_HAPPINESS_RING);
		}

		else if (true == CItemVnumHelper::IsLovePendant(dwVnum))
		{
			this->EffectPacket(SE_EQUIP_LOVE_PENDANT);
		}
		//
		else if (ITEM_UNIQUE == item->GetType() && 0 != item->GetSIGVnum())
		{
			const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(item->GetSIGVnum());
			if (NULL != pGroup)
			{
				const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(pGroup->GetAttrVnum(item->GetVnum()));
				if (NULL != pAttrGroup)
				{
					const std::string& std = pAttrGroup->m_stEffectFileName;
					SpecificEffectPacket(std.c_str());
				}
			}
		}
		if (item->IsNewMountItem()) // @fixme152
		{
			quest::CQuestManager::instance().UseItem(GetPlayerID(), item, false);
		}

	}

	return true;
}

void CHARACTER::BuffOnAttr_AddBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->AddBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_RemoveBuffsFromItem(LPITEM pItem)
{
	for (size_t i = 0; i < sizeof(g_aBuffOnAttrPoints) / sizeof(g_aBuffOnAttrPoints[0]); i++)
	{
		TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(g_aBuffOnAttrPoints[i]);
		if (it != m_map_buff_on_attrs.end())
		{
			it->second->RemoveBuffFromItem(pItem);
		}
	}
}

void CHARACTER::BuffOnAttr_ClearAll()
{
	for (TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.begin(); it != m_map_buff_on_attrs.end(); it++)
	{
		CBuffOnAttributes* pBuff = it->second;
		if (pBuff)
		{
			pBuff->Initialize();
		}
	}
}

void CHARACTER::BuffOnAttr_ValueChange(BYTE bType, BYTE bOldValue, BYTE bNewValue)
{
	TMapBuffOnAttrs::iterator it = m_map_buff_on_attrs.find(bType);

	if (0 == bNewValue)
	{
		if (m_map_buff_on_attrs.end() == it)
		{
			return;
		}
		else
		{
			it->second->Off();
		}
	}
	else if (0 == bOldValue)
	{
		CBuffOnAttributes* pBuff = NULL;
		if (m_map_buff_on_attrs.end() == it)
		{
			switch (bType)
			{
			case POINT_ENERGY:
			{
				static BYTE abSlot[] = { WEAR_BODY, WEAR_HEAD, WEAR_FOOTS, WEAR_WRIST, WEAR_WEAPON, WEAR_NECK, WEAR_EAR, WEAR_SHIELD };
				static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			case POINT_COSTUME_ATTR_BONUS:
			{
				static BYTE abSlot[] =
				{
					WEAR_COSTUME_BODY,
					WEAR_COSTUME_HAIR,
#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
					WEAR_COSTUME_WEAPON,
#endif
				};
				static std::vector <BYTE> vec_slots (abSlot, abSlot + _countof(abSlot));
				pBuff = M2_NEW CBuffOnAttributes(this, bType, &vec_slots);
			}
			break;
			default:
				break;
			}
			m_map_buff_on_attrs.insert(TMapBuffOnAttrs::value_type(bType, pBuff));

		}
		else
		{
			pBuff = it->second;
		}
		if (pBuff != NULL)
		{
			pBuff->On(bNewValue);
		}
	}
	else
	{
		assert (m_map_buff_on_attrs.end() != it);
		it->second->ChangeBuffValue(bNewValue);
	}
}


LPITEM CHARACTER::FindSpecifyItem(DWORD vnum) const
{
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
		if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
		{
			return GetInventoryItem(i);
		}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			if (GetInventoryItem(i) && GetInventoryItem(i)->GetVnum() == vnum)
			{
				return GetInventoryItem(i);
			}
		}
	}
#endif

	return NULL;
}

LPITEM CHARACTER::FindItemByID(DWORD id) const
{
	for (int i = 0 ; i < INVENTORY_MAX_NUM ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
		{
			return GetInventoryItem(i);
		}
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END ; ++i)
	{
		if (NULL != GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
		{
			return GetInventoryItem(i);
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			if (GetInventoryItem(i) && GetInventoryItem(i)->GetID() == id)
			{
				return GetInventoryItem(i);
			}
		}
	}
#endif

	return NULL;
}

#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
	int CHARACTER::CountSpecifyItem(DWORD vnum, int iExceptionCell) const
#else
	int CHARACTER::CountSpecifyItem(DWORD vnum) const
#endif
{
	int	count = 0;
	LPITEM item;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
		if (i == iExceptionCell)
		{
			continue;
		}
#endif

		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{
			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		item = GetInventoryItem(i);
		if (NULL != item && item->GetVnum() == vnum)
		{

			if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
			{
				continue;
			}
			else
			{
				count += item->GetCount();
			}
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			item = GetInventoryItem(i);
			if (item && item->GetVnum() == vnum)
			{
				if (m_pkMyShop && m_pkMyShop->IsSellingItem(item->GetID()))
				{
					continue;
				}
				else
				{
					count += item->GetCount();
				}
			}
		}
	}
#endif

	return count;
}

#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
	void CHARACTER::RemoveSpecifyItem(DWORD vnum, CountType count, int iExceptionCell)
#else
	void CHARACTER::RemoveSpecifyItem(DWORD vnum, CountType count)
#endif
{
	if (0 == count)
	{
		return;
	}

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
		if (i == iExceptionCell)
		{
			continue;
		}
#endif

		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetVnum() != vnum)
		{
			continue;
		}


		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (vnum >= 80003 && vnum <= 80007)
		{
			LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetVnum() != vnum)
		{
			continue;
		}


		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			if (NULL == GetInventoryItem(i))
			{
				continue;
			}

			if (GetInventoryItem(i)->GetVnum() != vnum)
			{
				continue;
			}

			if (m_pkMyShop)
			{
				bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
				if (isItemSelling)
				{
					continue;
				}
			}

			if (vnum >= 80003 && vnum <= 80007)
			{
				LogManager::instance().GoldBarLog(GetPlayerID(), GetInventoryItem(i)->GetID(), QUEST, "RemoveSpecifyItem");
			}

			if (count >= GetInventoryItem(i)->GetCount())
			{
				count -= GetInventoryItem(i)->GetCount();
				GetInventoryItem(i)->SetCount(0);

				if (0 == count)
				{
					return;
				}
			}
			else
			{
				GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
				return;
			}
		}
	}
#endif

	if (count)
	{
		sys_log(0, "CHARACTER::RemoveSpecifyItem cannot remove enough item vnum %u, still remain %d", vnum, count);
	}
}

CountType CHARACTER::CountSpecifyTypeItem(BYTE type) const
{
	CountType count = 0;

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (pItem != NULL && pItem->GetType() == type)
		{
			count += pItem->GetCount();
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			LPITEM pItem = GetInventoryItem(i);
			if (pItem != NULL && pItem->GetType() == type)
			{
				count += pItem->GetCount();
			}
		}
	}
#endif

	return count;
}

void CHARACTER::RemoveSpecifyTypeItem(BYTE type, CountType count)
{
	if (0 == count)
	{
		return;
	}

	for (UINT i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetType() != type)
		{
			continue;
		}


		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

	for (int i = BELT_INVENTORY_SLOT_START; i < BELT_INVENTORY_SLOT_END; ++i)
	{
		if (NULL == GetInventoryItem(i))
		{
			continue;
		}

		if (GetInventoryItem(i)->GetType() != type)
		{
			continue;
		}


		if (m_pkMyShop)
		{
			bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
			if (isItemSelling)
			{
				continue;
			}
		}

		if (count >= GetInventoryItem(i)->GetCount())
		{
			count -= GetInventoryItem(i)->GetCount();
			GetInventoryItem(i)->SetCount(0);

			if (0 == count)
			{
				return;
			}
		}
		else
		{
			GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
			return;
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			if (NULL == GetInventoryItem(i))
			{
				continue;
			}

			if (GetInventoryItem(i)->GetType() != type)
			{
				continue;
			}

			if (m_pkMyShop)
			{
				bool isItemSelling = m_pkMyShop->IsSellingItem(GetInventoryItem(i)->GetID());
				if (isItemSelling)
				{
					continue;
				}
			}

			if (count >= GetInventoryItem(i)->GetCount())
			{
				count -= GetInventoryItem(i)->GetCount();
				GetInventoryItem(i)->SetCount(0);

				if (0 == count)
				{
					return;
				}
			}
			else
			{
				GetInventoryItem(i)->SetCount(GetInventoryItem(i)->GetCount() - count);
				return;
			}
		}
	}
#endif
}

// returns true if item pointer is still valid
bool CHARACTER::AutoGiveItem(LPITEM item, bool longOwnerShip, bool autoStack)
{
	if (NULL == item)
	{
		sys_err ("NULL point.");
		return false;
	}

	if (item->GetOwner())
	{
		sys_err ("item %d 's owner exists!", item->GetID());
		return false;
	}

	if (autoStack && !item->IsDragonSoul() && item->IsStackable() && !IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_STACK))
	{
		LogManager::instance().ItemLog(this, item, "SYSTEM_STACK", item->GetBaseName());

#ifndef __SPECIAL_STORAGE_ENABLE__
		for (int slot = 0; slot < INVENTORY_MAX_NUM; ++slot)
#else
		for (int slot = 0; slot < INVENTORY_AND_EQUIP_SLOT_MAX; ++slot)
#endif
		{
			if (LPITEM curItem = GetInventoryItem(slot))
			{

#ifdef __SPECIAL_STORAGE_ENABLE__
				// We ought to skip equipped items
				if (curItem->GetCell() >= INVENTORY_MAX_NUM && curItem->GetCell() < SPECIAL_STORAGE_START_CELL)
				{
					continue;
				}
#endif

				if (!item->CanStackWith(curItem))
				{
					continue;
				}
				if (curItem->GetCount() >= g_bItemCountLimit)
				{
					continue;
				}

				DWORD itemCount = item->GetCount();
				DWORD countAdd = MIN(g_bItemCountLimit - curItem->GetCount(), itemCount);

				curItem->SetCount(curItem->GetCount() + countAdd);
				item->SetCount(itemCount - countAdd);

				if (itemCount == countAdd)
				{
					return false;
				}
			}
		}
	}

	int cell;
	if (item->IsDragonSoul())
	{
		cell = GetEmptyDragonSoulInventory(item);
	}
	else
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		cell = GetEmptyInventory(item->GetSize());
#else
		cell = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif
	}

	if (cell != -1)
	{
		if (item->IsDragonSoul())
		{
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, cell));
		}
		else
		{
			item->AddToCharacter(this, TItemPos(INVENTORY, cell));
		}

		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = cell;
				SetQuickslot(0, slot);
			}
		}
	}
	else
	{
		item->AddToGround (GetMapIndex(), GetXYZ());
#ifdef __ENABLE_NEWSTUFF__
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (longOwnerShip)
		{
			item->SetOwnership (this, 300);
		}
		else
		{
			item->SetOwnership (this, 60);
		}
		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	return true;
}

LPITEM CHARACTER::AutoGiveItem(DWORD dwItemVnum, CountType bCount, int iRarePct, bool bMsg, bool bCheckDSGrid, bool bCommand)
{
	TItemTable * p = ITEM_MANAGER::instance().GetTable(dwItemVnum);

	if (!p)
	{
		return NULL;
	}

	if (IS_SET(p->dwFlags, ITEM_FLAG_MAKECOUNT)) {
		if (bCount < p->alValues[1])
			bCount = p->alValues[1];
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_DROP, dwItemVnum, bCount);

	auto* item = ITEM_MANAGER::instance().CreateItem(dwItemVnum, bCount, 0, true, -1, false, bCommand);
	if (!item)
	{
		sys_err("cannot create item by vnum %u (name: %s)", dwItemVnum, GetName());
		return NULL;
	}
	
	if (item->IsStackable())
	{
#ifndef __SPECIAL_STORAGE_ENABLE__
		for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
#else
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
#endif
		{
			auto* item2 = GetInventoryItem(i);
			if (!item2)
			{
				continue;
			}

#ifdef __SPECIAL_STORAGE_ENABLE__
			// We ought to skip equipped items
			if (item2->GetCell() >= INVENTORY_MAX_NUM && item2->GetCell() < SPECIAL_STORAGE_START_CELL)
			{
				continue;
			}
#endif

			if (!CanStack(item, item2) || !CanModifyItem(item2))
				continue;

			const auto count2 = std::min<CountType>(GetItemMaxCount(item2) - item2->GetCount(), bCount);
			bCount -= count2;

			item2->SetCount(item2->GetCount() + count2);

			if (item2->GetType() == ITEM_QUEST)
				quest::CQuestManager::instance().PickupItem(GetPlayerID(),
					item2);

			if (bCount == 0) {
				M2_DESTROY_ITEM(item);
				return item2;
			}
		}
	}

	item->SetCount(bCount);

	int iEmptyCell;
	if (item->IsDragonSoul())
	{
		if (bCheckDSGrid && DragonSoul_GetEmptyInventoryTypeCount() < (DS_SLOT_MAX))
			iEmptyCell = -1;
		else
			iEmptyCell = GetEmptyDragonSoulInventory(item);
	}
	else
#ifndef __SPECIAL_STORAGE_ENABLE__
		iEmptyCell = GetEmptyInventory(item->GetSize());
#else
		iEmptyCell = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

	if (iEmptyCell != -1)
	{
		if (bMsg)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s received"), item->GetName());
		}

		if (item->IsDragonSoul())
		{
			item->AddToCharacter(this, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyCell));
		}
		else
		{
			item->AddToCharacter(this, TItemPos(INVENTORY, iEmptyCell));
		}
		LogManager::instance().ItemLog(this, item, "SYSTEM", item->GetName());

		if (item->GetType() == ITEM_USE && item->GetSubType() == USE_POTION)
		{
			TQuickslot * pSlot;

			if (GetQuickslot(0, &pSlot) && pSlot->type == QUICKSLOT_TYPE_NONE)
			{
				TQuickslot slot;
				slot.type = QUICKSLOT_TYPE_ITEM;
				slot.pos = iEmptyCell;
				SetQuickslot(0, slot);
			}
		}

		if (item->GetType() == ITEM_QUEST)
			quest::CQuestManager::instance().PickupItem(GetPlayerID(), item);
	}
	else
	{
		item->AddToGround(GetMapIndex(), GetXYZ());
#ifdef __ENABLE_NEWSTUFF__
		item->StartDestroyEvent(g_aiItemDestroyTime[ITEM_DESTROY_TIME_AUTOGIVE]);
#else
		item->StartDestroyEvent();
#endif

		if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_DROP))
		{
			item->SetOwnership(this, 300);
		}
		else
		{
			item->SetOwnership(this, 60);
		}

		LogManager::instance().ItemLog(this, item, "SYSTEM_DROP", item->GetName());
	}

	sys_log(0,
			"7: %d %d", dwItemVnum, bCount);
	return item;
}

bool CHARACTER::GiveItem(LPCHARACTER victim, TItemPos Cell)
{
	if (!CanHandleItem())
	{
		return false;
	}

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetPlayerID())->IsRunning() == true)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot take this item if you're using quests"));
		return false;
	}
	// @fixme150 END

	LPITEM item = GetItem(Cell);

	if (item && !item->IsExchanging())
	{
		if (victim->CanReceiveItem(this, item))
		{
			victim->ReceiveItem(this, item);
			return true;
		}
	}

	return false;
}

bool CHARACTER::CanReceiveItem(LPCHARACTER from, LPITEM item, bool ignoreDist /*= false*/) const
{
	if (IsPC())
	{
		return false;
	}

	// TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX
	if (DISTANCE_APPROX(GetX() - from->GetX(), GetY() - from->GetY()) > 2000)
	{
		return false;
	}
	// END_OF_TOO_LONG_DISTANCE_EXCHANGE_BUG_FIX

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH &&
				(item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
		{
			return true;
		}
		break;

	//case fishing::FISHER_MOB:
	//	if (item->GetType() == ITEM_ROD)
	//		return true;
	//	break;

	// BUILDING_NPC
	case BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
		if (item->GetType() == ITEM_WEAPON &&
				item->GetRefinedVnum())
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	case BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
		if (item->GetType() == ITEM_ARMOR &&
				(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
				item->GetRefinedVnum())
		{
			return true;
		}
		else
		{
			return false;
		}
		break;

	case BLACKSMITH_ACCESSORY_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetType() == ITEM_ARMOR &&
				!(item->GetSubType() == ARMOR_BODY || item->GetSubType() == ARMOR_SHIELD || item->GetSubType() == ARMOR_HEAD) &&
				item->GetRefinedVnum())
		{
			return true;
		}
		else
		{
			return false;
		}
		break;
	// END_OF_BUILDING_NPC

	case BLACKSMITH_MOB:
		if (item->GetRefinedVnum() && item->GetRefineSet() < 3000)
		{
			return true;
		}
		else
		{
			return false;
		}

	case BLACKSMITH2_MOB:
		if (item->GetRefineSet() >= 3001)
		{
			return true;
		}
		else
		{
			return false;
		}

	case ALCHEMIST_MOB:
		if (item->GetRefinedVnum())
		{
			return true;
		}
		break;

	case 20101:
	case 20102:
	case 20103:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_1)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20104:
	case 20105:
	case 20106:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_2)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			return false;
		}
		break;
	case 20107:
	case 20108:
	case 20109:

		if (item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			if (!IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Monkey Herbs cannot be fed to living horses. It is used to revive dead horses."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			if (IsDead())
			{
				from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot feed a dead Horse."));
				return false;
			}
			return true;
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 || item->GetVnum() == ITEM_HORSE_FOOD_2)
		{
			return false;
		}
		break;
	}

	//if (IS_SET(item->GetFlag(), ITEM_FLAG_QUEST_GIVE))
	{
		return true;
	}

	return false;
}

void CHARACTER::ReceiveItem(LPCHARACTER from, LPITEM item)
{
	if (IsPC())
	{
		return;
	}

	switch (GetRaceNum())
	{
	case fishing::CAMPFIRE_MOB:
		if (item->GetType() == ITEM_FISH && (item->GetSubType() == FISH_ALIVE || item->GetSubType() == FISH_DEAD))
		{
			CFishing::instance().Grill(from, item);
		}
		else
		{
			// TAKE_ITEM_BUG_FIX
			from->SetQuestNPCID(GetVID());
			// END_OF_TAKE_ITEM_BUG_FIX
			quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		}
		break;

	// DEVILTOWER_NPC
	case DEVILTOWER_BLACKSMITH_WEAPON_MOB:
	case DEVILTOWER_BLACKSMITH_ARMOR_MOB:
	case DEVILTOWER_BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum() != 0 && item->GetRefineSet() != 0 && item->GetRefineSet() < 3000)
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item, REFINE_TYPE_MONEY_ONLY, nullptr, false, 10);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		}
		break;
	// END_OF_DEVILTOWER_NPC

	case BLACKSMITH_MOB:
	case BLACKSMITH2_MOB:
		if (item->GetRefinedVnum())
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item, REFINE_TYPE_NORMAL);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		}
		break;

	case BLACKSMITH_WEAPON_MOB:
	case BLACKSMITH_ARMOR_MOB:
	case BLACKSMITH_ACCESSORY_MOB:
		if (item->GetRefinedVnum())
		{
			from->SetRefineNPC(this);
			from->RefineInformation(item, REFINE_TYPE_NORMAL, nullptr, true, 10);
		}
		else
		{
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		}
		break;

	case 20101:
	case 20102:
	case 20103:
	case 20104:
	case 20105:
	case 20106:
	case 20107:
	case 20108:
	case 20109:
		if (item->GetVnum() == ITEM_REVIVE_HORSE_1 ||
				item->GetVnum() == ITEM_REVIVE_HORSE_2 ||
				item->GetVnum() == ITEM_REVIVE_HORSE_3)
		{
			from->ReviveHorse();
			item->SetCount(item->GetCount() - 1);
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You fed the Horse with Herbs."));
		}
		else if (item->GetVnum() == ITEM_HORSE_FOOD_1 ||
				 item->GetVnum() == ITEM_HORSE_FOOD_2 ||
				 item->GetVnum() == ITEM_HORSE_FOOD_3)
		{
			from->FeedHorse();
			from->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have fed the Horse."));
			item->SetCount(item->GetCount() - 1);
			EffectPacket(SE_HPUP_RED);
		}
		break;

	default:
		sys_log(0, "TakeItem %s %d %s", from->GetName(), GetRaceNum(), item->GetName());
		from->SetQuestNPCID(GetVID());
		quest::CQuestManager::instance().TakeItem(from->GetPlayerID(), GetRaceNum(), item);
		break;
	}
}

bool CHARACTER::IsEquipUniqueItem(DWORD dwItemVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetVnum() == dwItemVnum)
		{
			return true;
		}
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetVnum() == dwItemVnum)
		{
			return true;
		}
	}


	if (dwItemVnum == UNIQUE_ITEM_RING_OF_LANGUAGE)
	{
		return IsEquipUniqueItem(UNIQUE_ITEM_RING_OF_LANGUAGE_SAMPLE);
	}

	return false;
}

// CHECK_UNIQUE_GROUP
bool CHARACTER::IsEquipUniqueGroup(DWORD dwGroupVnum) const
{
	{
		LPITEM u = GetWear(WEAR_UNIQUE1);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
		{
			return true;
		}
	}

	{
		LPITEM u = GetWear(WEAR_UNIQUE2);

		if (u && u->GetSpecialGroup() == (int) dwGroupVnum)
		{
			return true;
		}
	}

	return false;
}
// END_OF_CHECK_UNIQUE_GROUP

void CHARACTER::SetRefineMode(CItem* additionalItem)
{
	m_iRefineAdditionalCell = additionalItem ? additionalItem->GetCell() : -1;
	m_bUnderRefine = true;
}

void CHARACTER::ClearRefineMode()
{
	m_bUnderRefine = false;
	SetRefineNPC( NULL );
}

bool CHARACTER::GiveItemFromSpecialItemGroup(DWORD dwGroupNum, std::vector<DWORD> &dwItemVnums,
		std::vector<DWORD> &dwItemCounts, std::vector <LPITEM> &item_gets, int &count)
{
	const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwGroupNum);

	if (!pGroup)
	{
		sys_err("cannot find special item group %d", dwGroupNum);
		return false;
	}

	std::vector <int> idxes;
	int n = pGroup->GetMultiIndex(idxes);

	bool bSuccess;

	for (int i = 0; i < n; i++)
	{
		bSuccess = false;
		int idx = idxes[i];
		DWORD dwVnum = pGroup->GetVnum(idx);
		DWORD dwCount = pGroup->GetCount(idx);
		int	iRarePct = pGroup->GetRarePct(idx);
		LPITEM item_get = NULL;
		switch (dwVnum)
		{
		case CSpecialItemGroup::GOLD:
		{
			PointChange(POINT_GOLD, dwCount);
			LogManager::instance().CharLog(this, dwCount, "TREASURE_GOLD", "");
#ifdef __BATTLE_PASS_ENABLE__
			CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, dwCount });
#endif

			bSuccess = true;
		}
		break;
		case CSpecialItemGroup::EXP:
		{
			PointChange(POINT_EXP, dwCount);
			LogManager::instance().CharLog(this, dwCount, "TREASURE_EXP", "");

			bSuccess = true;
		}
		break;

		case CSpecialItemGroup::MOB:
		{
			sys_log(0, "CSpecialItemGroup::MOB %d", dwCount);
			int x = GetX() + number(-500, 500);
			int y = GetY() + number(-500, 500);

			LPCHARACTER ch = CHARACTER_MANAGER::instance().SpawnMob(dwCount, GetMapIndex(), x, y, 0, true, -1);
			if (ch)
			{
				ch->SetAggressive();
			}
			bSuccess = true;
		}
		break;
		case CSpecialItemGroup::SLOW:
		{
			sys_log(0, "CSpecialItemGroup::SLOW %d", -(int)dwCount);
			AddAffect(AFFECT_SLOW, POINT_MOV_SPEED, -(int)dwCount, AFF_SLOW, 300, 0, true);
			bSuccess = true;
		}
		break;
		case CSpecialItemGroup::DRAIN_HP:
		{
			int iDropHP = GetMaxHP() * dwCount / 100;
			sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
			iDropHP = MIN(iDropHP, GetHP() - 1);
			sys_log(0, "CSpecialItemGroup::DRAIN_HP %d", -iDropHP);
			PointChange(POINT_HP, -iDropHP);
			bSuccess = true;
		}
		break;
		case CSpecialItemGroup::POISON:
		{
			AttackedByPoison(NULL);
			bSuccess = true;
		}
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
		case CSpecialItemGroup::BLEEDING:
		{
			AttackedByBleeding(NULL);
			bSuccess = true;
		}
		break;
#endif
		case CSpecialItemGroup::MOB_GROUP:
		{
			int sx = GetX() - number(300, 500);
			int sy = GetY() - number(300, 500);
			int ex = GetX() + number(300, 500);
			int ey = GetY() + number(300, 500);
			CHARACTER_MANAGER::instance().SpawnGroup(dwCount, GetMapIndex(), sx, sy, ex, ey, NULL, true);

			bSuccess = true;
		}
		break;
		default:
		{
			// We have back there, right now its not logic at all..
			/*
			TItemTable* p = ITEM_MANAGER::instance().GetTable(dwVnum);

			if (!p)
			{
				return false;
			}

			if (p->bType == ITEM_DS)
			{
				auto emptyDS = GetEmptyDragonSoulInventory(p);
				if (emptyDS == -1)
				{
					ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough free inventory space."));
					return false;
				}
			}
			*/

#ifdef __ENABLE_NEW_LOGS_CHAT__
			item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, false, true);
#else
			item_get = AutoGiveItem(dwVnum, dwCount, iRarePct, true, true);
#endif

			if (item_get && item_get->GetOwner() == this)
			{
				bSuccess = true;
			}
			else if (item_get && item_get->GetOwner() != this)
				// Delete item to prevent it from being taken by player
				item_get->RemoveFromGround();
		}
		break;
		}

		if (bSuccess)
		{
#ifdef __ENABLE_NEW_LOGS_CHAT__
			if (!bInvBuff)
				CChatLogs::SendChatLogInformation(this, ELogsType::LOG_TYPE_CHEST, static_cast<int>(dwCount), static_cast<int>(dwVnum));
			else
				// Cache items to keep count (in case of stacking)
				AddInventoryBufferLog(item_get->GetName(), dwVnum, dwCount);
#endif

			dwItemVnums.push_back(dwVnum);
			dwItemCounts.push_back(dwCount);
			item_gets.push_back(item_get);
			count++;

		}
		else
		{
			return false;
		}
	}
	return bSuccess;
}

// NEW_HAIR_STYLE_ADD
bool CHARACTER::ItemProcess_Hair(LPITEM item, int iDestCell)
{
	if (item->CheckItemUseLevel(GetLevel()) == false)
	{

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to wear this Hairstyle."));
		return false;
	}

	DWORD hair = item->GetVnum();

	switch (GetJob())
	{
	case JOB_WARRIOR :
		hair -= 72000;
		break;

	case JOB_ASSASSIN :
		hair -= 71250;
		break;

	case JOB_SURA :
		hair -= 70500;
		break;

	case JOB_SHAMAN :
		hair -= 69750;
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case JOB_WOLFMAN:
		break;
#endif
	default :
		return false;
		break;
	}

	if (hair == GetPart(PART_HAIR))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You already have this Hairstyle."));
		return true;
	}

	item->SetCount(item->GetCount() - 1);

	SetPart(PART_HAIR, hair);
	UpdatePacket();

	return true;
}
// END_NEW_HAIR_STYLE_ADD

bool CHARACTER::ItemProcess_Polymorph(LPITEM item)
{
	if (IsPolymorphed())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have already transformed."));
		return false;
	}

	if (true == IsRiding())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("�а��� �� ���� �����Դϴ�."));
		return false;
	}

	if (GetWarMap())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NOT_ALLOWED_TO_SPAWN"));
		return false;
	}

	DWORD dwVnum = item->GetSocket(0);

	if (dwVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
		item->SetCount(item->GetCount() - 1);
		return false;
	}

	const CMob* pMob = CMobManager::instance().Get(dwVnum);

	if (pMob == NULL)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("That's the wrong trading item."));
		item->SetCount(item->GetCount() - 1);
		return false;
	}

	switch (item->GetVnum())
	{
	case 70104 :
	case 70105 :
	case 70106 :
	case 70107 :
	case 71093 :
	{

		sys_log(0, "USE_POLYMORPH_BALL PID(%d) vnum(%d)", GetPlayerID(), dwVnum);


		//int iPolymorphLevelLimit = MAX(0, 20 - GetLevel() * 3 / 10);
		//if (pMob->m_table.bLevel >= GetLevel() + iPolymorphLevelLimit)
		//{
		//	ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot transform into a monster who has a higher level than you."));
		//	return false;
		//}

		auto iDuration = 0;
		switch (GetSkillMasterType(POLYMORPH_SKILL_ID))
		{
			case SKILL_NORMAL:
				iDuration = 5;
				break;

			case SKILL_MASTER:
				iDuration = 15;
				break;

			case SKILL_GRAND_MASTER:
				iDuration = 25;
				break;

			case SKILL_PERFECT_MASTER:
				iDuration = 35;
				break;
		}

		iDuration *= 60;

		AddAffect(AFFECT_POLYMORPH, POINT_POLYMORPH, dwVnum, AFF_POLYMORPH, iDuration, 0, true);
		item->SetCount(item->GetCount() - 1);
	}
	break;

	case 50322:
	{





		sys_log(0, "USE_POLYMORPH_BOOK: %s(%u) vnum(%u)", GetName(), GetPlayerID(), dwVnum);

		if (CPolymorphUtils::instance().PolymorphCharacter(this, item, pMob) == true)
		{
			CPolymorphUtils::instance().UpdateBookPracticeGrade(this, item);
		}
		else
		{
		}
	}
	break;

	default :
		sys_err("POLYMORPH invalid item passed PID(%d) vnum(%d)", GetPlayerID(), item->GetOriginalVnum());
		return false;
	}

	return true;
}

// bool CHARACTER::CanDoCube() const
// {
// if (m_bIsObserver)	return false;
// if (GetShop())		return false;
// if (GetMyShop())	return false;
// if (m_bUnderRefine)	return false;
// if (IsWarping())	return false;

// return true;
// }

bool CHARACTER::UnEquipSpecialRideUniqueItem()
{
	LPITEM Unique1 = GetWear(WEAR_UNIQUE1);
	LPITEM Unique2 = GetWear(WEAR_UNIQUE2);

	if ( NULL != Unique1 )
	{
		if ( UNIQUE_GROUP_SPECIAL_RIDE == Unique1->GetSpecialGroup() )
		{
			return UnequipItem(Unique1);
		}
	}

	if ( NULL != Unique2 )
	{
		if ( UNIQUE_GROUP_SPECIAL_RIDE == Unique2->GetSpecialGroup() )
		{
			return UnequipItem(Unique2);
		}
	}

	return true;
}

void CHARACTER::AutoRecoveryItemProcess(const EAffectTypes type)
{
	if (true == IsDead() || true == IsStun())
	{
		return;
	}

	if (false == IsPC())
	{
		return;
	}

	if (AFFECT_AUTO_HP_RECOVERY != type && AFFECT_AUTO_SP_RECOVERY != type)
	{
		return;
	}

	if (NULL != FindAffect(AFFECT_STUN))
	{
		return;
	}

	{
		const DWORD stunSkills[] = { SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };

		for (size_t i = 0 ; i < sizeof(stunSkills) / sizeof(DWORD) ; ++i)
		{
			const CAffect* p = FindAffect(stunSkills[i]);

			if (NULL != p && AFF_STUN == p->dwFlag)
			{
				return;
			}
		}
	}

	const CAffect* pAffect = FindAffect(type);
	const size_t idx_of_amount_of_used = 1;
	const size_t idx_of_amount_of_full = 2;

	if (NULL != pAffect)
	{
		LPITEM pItem = FindItemByID(pAffect->dwFlag);

		if (NULL != pItem && true == pItem->GetSocket(0))
		{
			if (!CArenaManager::instance().IsArenaMap(GetMapIndex())
#ifdef __ENABLE_NEWSTUFF__
					&& !(g_NoPotionsOnPVP && CPVPManager::instance().IsFighting(GetPlayerID()) && !IsAllowedPotionOnPVP(pItem->GetVnum()))
#endif
			   )
			{
				const long amount_of_used = pItem->GetSocket(idx_of_amount_of_used);
				const long amount_of_full = pItem->GetSocket(idx_of_amount_of_full);

				const int32_t avail = amount_of_full - amount_of_used;

				int32_t amount = 0;

				if (AFFECT_AUTO_HP_RECOVERY == type)
				{
					amount = GetMaxHP() - (GetHP() + GetPoint(POINT_HP_RECOVERY));
				}
				else if (AFFECT_AUTO_SP_RECOVERY == type)
				{
					amount = GetMaxSP() - (GetSP() + GetPoint(POINT_SP_RECOVERY));
				}

				if (amount > 0)
				{
					if (avail > amount)
					{
						const int pct_of_used = amount_of_used * 100 / amount_of_full;
						const int pct_of_will_used = (amount_of_used + amount) * 100 / amount_of_full;

						bool bLog = false;


						if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
						{
							bLog = true;
						}
						pItem->SetSocket(idx_of_amount_of_used, amount_of_used + amount, bLog);
					}
					else
					{
						amount = avail;

						pItem->SetCount(pItem->GetCount() - 1);
					}

					if (AFFECT_AUTO_HP_RECOVERY == type)
					{
						PointChange( POINT_HP_RECOVERY, amount );
						EffectPacket( SE_AUTO_HPUP );
					}
					else if (AFFECT_AUTO_SP_RECOVERY == type)
					{
						PointChange( POINT_SP_RECOVERY, amount );
						EffectPacket( SE_AUTO_SPUP );
					}
				}
			}
			else
			{
				pItem->Lock(false);
				pItem->SetSocket(0, false);
				RemoveAffect( const_cast<CAffect*>(pAffect) );
			}
		}
		else
		{
			RemoveAffect( const_cast<CAffect*>(pAffect) );
		}
	}
}

bool CHARACTER::IsValidItemPosition(TItemPos Pos) const
{
	BYTE window_type = Pos.window_type;
	WORD cell = Pos.cell;

	switch (window_type)
	{
	case RESERVED_WINDOW:
		return false;

	case INVENTORY:
	case EQUIPMENT:
		return cell < (INVENTORY_AND_EQUIP_SLOT_MAX);

	case DRAGON_SOUL_INVENTORY:
		return cell < (DRAGON_SOUL_INVENTORY_MAX_NUM);

	case SAFEBOX:
		if (NULL != m_pkSafebox)
		{
			return m_pkSafebox->IsValidPosition(cell);
		}
		else
		{
			return false;
		}

	case MALL:
		if (NULL != m_pkMall)
		{
			return m_pkMall->IsValidPosition(cell);
		}
		else
		{
			return false;
		}

#ifdef __ENABLE_SWITCHBOT__
	case SWITCHBOT:
		return cell < SWITCHBOT_SLOT_COUNT;
#endif

	default:
		return false;
	}

	return false;
}



#define VERIFY_MSG(exp, msg)  \
	if (true == (exp)) { \
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT(msg)); \
			return false; \
	}


bool CHARACTER::CanEquipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{
	const TItemTable* itemTable = item->GetProto();
	//BYTE itemType = item->GetType();
	//BYTE itemSubType = item->GetSubType();

	switch (GetJob())
	{
	case JOB_WARRIOR:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
		{
			return false;
		}
		break;

	case JOB_ASSASSIN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
		{
			return false;
		}
		break;

	case JOB_SHAMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
		{
			return false;
		}
		break;

	case JOB_SURA:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_SURA)
		{
			return false;
		}
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case JOB_WOLFMAN:
		if (item->GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
		{
			return false;
		}
		break;
#endif
	}

	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		long limit = itemTable->aLimits[i].lValue;
		switch (itemTable->aLimits[i].bType)
		{
		case LIMIT_LEVEL:
			if (GetLevel() < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your level is too low to equip this item."));
				return false;
			}
			break;

		case LIMIT_STR:
			if (GetPoint(POINT_ST) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not strong enough to equip yourself with this item."));
				return false;
			}
			break;

		case LIMIT_INT:
			if (GetPoint(POINT_IQ) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your intelligence is too low to equip yourself with this item."));
				return false;
			}
			break;

		case LIMIT_DEX:
			if (GetPoint(POINT_DX) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your dexterity is too low to equip yourself with this item."));
				return false;
			}
			break;

		case LIMIT_CON:
			if (GetPoint(POINT_HT) < limit)
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your vitality is too low to equip yourself with this item."));
				return false;
			}
			break;
		}
	}

	if (item->GetWearFlag() & WEARABLE_UNIQUE)
	{
		const auto WEAR_SLOTS = { WEAR_UNIQUE1, WEAR_UNIQUE2, WEAR_UNIQUE3, WEAR_UNIQUE4 };
		const int TOTAL_RINGS_WEAR = 2;
		for (const auto& wearSlot : WEAR_SLOTS)
		{
			if (GetWear(wearSlot) && GetWear(wearSlot)->IsSameSpecialGroup(item))
			{
				ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot equip this Item twice."));
				return false;
			}
		}

		if (item->GetType() == ITEM_RING)
		{
			auto iCF = std::count_if(WEAR_SLOTS.begin(), WEAR_SLOTS.end(), [&](const BYTE iSlot) { return (GetWear(iSlot) && GetWear(iSlot)->GetType() == ITEM_RING); });
			if (iCF == TOTAL_RINGS_WEAR)
			{
				ChatPacket(CHAT_TYPE_INFO, "You can't have more than 2 rings equipped.");
				return false;
			}
		}

		if (marriage::CManager::instance().IsMarriageUniqueItem(item->GetVnum()) &&
				!marriage::CManager::instance().IsMarried(GetPlayerID()))
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot use this item because you are not married."));
			return false;
		}

	}

	return true;
}


bool CHARACTER::CanUnequipNow(const LPITEM item, const TItemPos& srcCell, const TItemPos& destCell) /*const*/
{

	if (ITEM_BELT == item->GetType())
	{
		VERIFY_MSG(CBeltInventoryHelper::IsExistItemInBeltInventory(this), "You can only discard the belt when there are no longer any items in its inventory.");
	}


	if (IS_SET(item->GetFlag(), ITEM_FLAG_IRREMOVABLE))
	{
		return false;
	}


	{
		int pos = -1;

		if (item->IsDragonSoul())
		{
			pos = GetEmptyDragonSoulInventory(item);
		}
		else
#ifndef __SPECIAL_STORAGE_ENABLE__
			pos = GetEmptyInventory(item->GetSize());
#else
			pos = GetEmptyInventory(item->GetSize(), item->GetVirtualWindow());
#endif

		VERIFY_MSG( -1 == pos, "There isn't enough space in your inventory." );
	}


	return true;
}

#ifdef __ENABLE_DELETE_SINGLE_STONE__
void CHARACTER::OpenDestroyItemSocket(WORD wIndex)
{
	if (!CanHandleItem() || !CanWarp() || IsHack(true, 10, 10))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-open-error"));
		return;
	}

	LPITEM item;
	if ((item = GetItem(TItemPos(INVENTORY, wIndex))) == nullptr)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-item-not-found"));
		return;
	}

	if (!(item->GetType() == ITEM_WEAPON || (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_BODY)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-item-invalid-type"));
		return;
	}
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
	{
		return;
	}

	if (item->IsEquipped())
	{
		return;
	}

	size_t i;
	for (i = 0; i < ITEM_SOCKET_MAX_NUM - 3; ++i)
	{
		if (test_server)
		{

			ChatPacket(CHAT_TYPE_INFO, "========");
			ChatPacket(CHAT_TYPE_INFO, std::to_string(item->GetSocket(i)).c_str());
			ChatPacket(CHAT_TYPE_INFO, std::to_string(ITEM_BROKEN_METIN_VNUM).c_str());
		}

		if (item->GetSocket(i) != ITEM_BROKEN_METIN_VNUM)
		{
			break;
		}
	}

	if (i == ITEM_SOCKET_MAX_NUM - 3)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-no-socket"));
		return;
	}

	m_deleteSocket.open = true;
	m_deleteSocket.itemPos.cell = wIndex;

	const size_t size = sizeof(BYTE) + sizeof(WORD);
	TEMP_BUFFER buff(size);

	const uint8_t subheader = SUBHEADER_REQUEST_DELETE_SOCKET_OPEN;

	buff.write(&subheader, sizeof(BYTE));
	buff.write(&wIndex, sizeof(WORD));

	TPacketCGDefault p(HEADER_GC_REQUEST_DELETE_SOCKET);
	p.size += size;

	GetDesc()->BufferedPacket(&p, sizeof(TPacketCGDefault));
	GetDesc()->Packet(buff.read_peek(), buff.size());
}

void CHARACTER::DestroyItemSocket(WORD wIndex)
{
	LPITEM item;
	if ((item = GetItem(m_deleteSocket.itemPos)) == nullptr)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-item-not-found"));
		return;
	}

	if (!(item->GetType() == ITEM_WEAPON || (item->GetType() == ITEM_ARMOR && item->GetSubType() == ARMOR_BODY)))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("destroy-socket-item-invalid-type"));
		return;
	}
	if (item->IsEquipped())
	{
		return;
	}
	if (item->GetType() == ITEM_COSTUME && item->GetSubType() == COSTUME_WEAPON)
	{
		return;
	}
	for (size_t i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (wIndex & (1 << i))
		{
			if (item->GetSocket(i) != ITEM_BROKEN_METIN_VNUM)
			{
				AutoGiveItem(item->GetSocket(i));
				item->SetSocket(i, 1);
			}
		}
	}

	CloseDestroyItemSocket();
}

void CHARACTER::CloseDestroyItemSocket()
{
	m_deleteSocket.open = false;
	m_deleteSocket.cooltime = int(thecore_pulse()) + 10;
	m_deleteSocket.itemPos.cell = WORD_MAX;

	const size_t size = sizeof(BYTE);
	TEMP_BUFFER buff(size);

	const uint8_t subheader = SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE;
	buff.write(&subheader, sizeof(BYTE));

	TPacketCGDefault p(HEADER_GC_REQUEST_DELETE_SOCKET);
	p.size += size;

	GetDesc()->BufferedPacket(&p, sizeof(TPacketCGDefault));
	GetDesc()->Packet(buff.read_peek(), buff.size());
}
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
static bool FN_compare_items(LPITEM item1, LPITEM item2)
{
	// Vnum
	if (item1->GetVnum() != item2->GetVnum())
	{
		return false;
	}

	// Sockets
	for (unsigned int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (item1->GetSocket(i) != item2->GetSocket(i))
		{
			return false;
		}
	}

	// Attributes
	for (unsigned int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		if ((item1->GetAttributeType(i) != item2->GetAttributeType(i)) || (item1->GetAttributeValue(i) != item2->GetAttributeValue(i)))
		{
			return false;
		}
	}

	return true;
}
void CHARACTER::SortInventory()
{
	// Gather items
	std::vector<LPITEM> v_items;
	auto lbAdd = [&v_items] (LPITEM item)
	{
		if (item && !item->isLocked())
		{
			auto fRes = std::find_if(v_items.begin(), v_items.end(), [&item] (const LPITEM & rItem) { return FN_compare_items(rItem, item) && (rItem->GetCount() < g_bItemCountLimit); });
			if (item->IsStackable() && fRes != v_items.end())
			{
				auto pItem = *fRes;
				auto stDiff = std::min<CountType>(item->GetCount(), g_bItemCountLimit - pItem->GetCount());

				pItem->SetCount(pItem->GetCount() + stDiff);
				stDiff = item->GetCount() - stDiff;
				item->SetCount(stDiff);

				if (!stDiff)
				{
					item = nullptr;
				}
			}

			if (item)
			{
				item->RemoveFromCharacter();
				v_items.push_back(item);
			}
		}
	};

	// Normal inventory
	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		lbAdd(GetInventoryItem(i));
	}

	// Special Inventory
#ifdef __SPECIAL_STORAGE_ENABLE__
	for (DWORD j = SPECIAL_STORAGE_START_WINDOW; j < SPECIAL_STORAGE_END_WINDOW; ++j)
	{
		auto pRange = TItemPos::GetSpecialStorageRange(j);
		for (int i = pRange.first; i < pRange.second; ++i)
		{
			lbAdd(GetInventoryItem(i));
		}
	}
#endif

	// Sort container
	std::sort(v_items.begin(), v_items.end(), [] (const LPITEM & l, const LPITEM & r) { return l->GetVnum() < r->GetVnum(); });

	// Iterate over
	for (auto it = v_items.begin(); it != v_items.end(); )
	{
		LPITEM pItem = *it;
#ifndef __SPECIAL_STORAGE_ENABLE__
		int iCell = GetEmptyInventory(pItem->GetSize());
#else
		int iCell = GetEmptyInventory(pItem->GetSize(), pItem->GetVirtualWindow());
#endif

		pItem->AddToCharacter(this, TItemPos(INVENTORY, iCell));
		it = v_items.erase(it);
	}

	if (v_items.size())
	{
		for (const auto & pItem : v_items)
		{
			sys_err("What the fuck did just happen? Item will be stashed. Item ID: %u, Owner PID: %u", pItem->GetID(), GetPlayerID());
		}
	}
}
#endif

#ifdef __ENABLE_TICKET_COINS__
void CHARACTER::InsertCoins(LPITEM item)
{
	if (!item)
	{
		return;
	}

	bool bAvaliable = false;
	const auto dwVnum = item->GetVnum();
	const auto AVALIABLE_VNUMS = { 80014, 80015, 80016, 80017 };
	for (const auto& it : AVALIABLE_VNUMS) if (it == dwVnum) { bAvaliable = true; }

	if (bAvaliable)
	{
		const auto dwAmount = item->GetValue(0);

		if (dwAmount <= 0)
		{
			if (test_server)
			{
				sys_log(0, "Amount of coin ticket == 0");
			}

			return;
		}

		TRequestChargeCash p;
		p.dwAID = GetDesc()->GetAccountTable().id;
		p.dwAmount = static_cast<DWORD>(dwAmount);
		p.eChargeType = ERequestCharge_Cash;
		db_clientdesc->DBPacketHeader(HEADER_GD_REQUEST_CHARGE_CASH, 0, sizeof(TRequestChargeCash));
		db_clientdesc->Packet(&p, sizeof(p));

		RemoveSpecifyItem(dwVnum, 1);
		LogManager::instance().CharLog(this, dwAmount, "USE_TICKET_COINS", "");

		if (test_server)
		{
			ChatPacket(CHAT_TYPE_NOTICE, "CHARACTER::InsertCoins: vnum %d | coins %d", dwVnum, dwAmount);
		}

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your account has been topped up at %d coins"), dwAmount);
	}
}
#endif

#ifdef __GAYA_SHOP_SYSTEM__
void CHARACTER::InsertGayaPoints(LPITEM item)
{
	if (!item)
	{
		return;
	}

	bool bAvaliable = false;
	const auto dwVnum = item->GetVnum();
	const auto AVALIABLE_VNUMS = { 190001, 190002, 190003 };
	for (const auto& it : AVALIABLE_VNUMS) if (it == dwVnum) { bAvaliable = true; }

	if (bAvaliable)
	{
		const auto dwAmount = item->GetValue(0);

		if (dwAmount <= 0)
		{
			if (test_server)
			{
				sys_log(0, "Amount of gaya ticket == 0");
			}

			return;
		}

		GiveGayaPoints(dwAmount);
		RemoveSpecifyItem(dwVnum, 1);
		LogManager::instance().CharLog(this, dwAmount, "USE_GAYA_TICKET", "");

		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your account has been topped up at %d gaya"), dwAmount);
	}
}
#endif

#ifdef __BATTLE_PASS_ENABLE__
void CHARACTER::BattlePassActivation(LPITEM item)
{
	if (!item)
	{
		return;
	}

	bool bAvaliable = false;
	const auto dwVnum = item->GetVnum();
	const auto AVALIABLE_VNUMS = { 200001 };
	for (const auto& it : AVALIABLE_VNUMS) if (it == dwVnum) { bAvaliable = true; }

	if (bAvaliable)
	{
		const auto dwLevel = item->GetValue(0);

		if (dwLevel <= 0)
		{
			if (test_server)
			{
				sys_log(0, "Level of BP == 0");
			}

			return;
		}

		auto eDiff = static_cast<BattlePassNS::EDiffLevel>(dwLevel);
		if (CBattlePassManager::instance().RegisterBattlePass(this, eDiff))
		{
			RemoveSpecifyItem(dwVnum, 1);
			LogManager::instance().CharLog(this, dwLevel, "USE_BP_TICKET", "");
		}
	}
}
#endif

#ifdef __ENABLE_FAST_REFINE_OPTION__
void CHARACTER::SetFastRefineVnum(DWORD dwVnum)
{
	m_dwRefineScrollVnum = dwVnum;
}

void CHARACTER::CloseRefine()
{
	ChatPacket(CHAT_TYPE_COMMAND, "CloseRefine");

	ClearRefineMode();
}
#endif

#ifdef __ENABLE_HIDE_COSTUMES__
bool CHARACTER::HasCostumeFlag(DWORD dwCostumeFlag)
{
	if (IS_SET(m_pointsInstant.dwCostumeFlag, dwCostumeFlag))
	{
		return true;
	}

	return false;
}
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
BYTE CHARACTER::GetRefineElementType()
{
	LPITEM pkWeapon = GetWear(WEAR_WEAPON);
	if (!pkWeapon)
	{
		return 0;
	}

	if (pkWeapon->GetRefineElementPlus() < 3)
	{
		return 0;
	}

	return pkWeapon->GetRefineElementType();
}

void CHARACTER::ClearRefineElement()
{
	m_sRefineElementSrcCell = -1;
	m_sRefineElementDstCell = -1;
	m_cRefineElementType = -1;
	m_bUnderRefine = false;
}

bool CHARACTER::DoRefineElement(BYTE bArg)
{
	if (!CanHandleItem(true))
	{
		ClearRefineElement();
		return false;
	}

	// Save the values local and clear them
	// in case someone click multiple times
	// or a return false ocurs and the values need to be cleaned
	short sRefineElementSrcCell = m_sRefineElementSrcCell;
	short sRefineElementDstCell = m_sRefineElementDstCell;
	char  cRefineElementType = m_cRefineElementType;
	ClearRefineElement();

	if (sRefineElementSrcCell < 0 || sRefineElementDstCell < 0 || cRefineElementType < 0)
	{
		return false;
	}

	LPITEM srcItem = GetInventoryItem(sRefineElementSrcCell);
	LPITEM dstItem = GetInventoryItem(sRefineElementDstCell);

	if (!srcItem || !dstItem)
	{
		return false;
	}

	if (dstItem->IsExchanging() || dstItem->IsEquipped())
	{
		return false;
	}

	if (srcItem->GetType() != ITEM_USE)
	{
		return false;
	}

	if (dstItem->GetType() != ITEM_WEAPON)
	{
		return false;
	}

	if (dstItem->GetRefineLevel() < ELEMENT_MIN_REFINE_LEVEL)
	{
		return false;
	}

	if (srcItem->GetSubType() == USE_ELEMENT_UPGRADE)
	{
		if (cRefineElementType != REFINE_ELEMENT_TYPE_UPGRADE)
		{
			return false;
		}

		if (srcItem->GetValue(0) <= REFINE_ELEMENT_CATEGORY_NONE || srcItem->GetValue(0) >= REFINE_ELEMENT_CATEGORY_MAX)
		{
			return false;
		}

		if (dstItem->GetRefineElementPlus() == REFINE_ELEMENT_MAX)
		{
			return false;
		}

		if (dstItem->GetRefineElementType() > 0 && dstItem->GetRefineElementType() != srcItem->GetValue(0))
		{
			return false;
		}
	}
	else if (srcItem->GetSubType() == USE_ELEMENT_DOWNGRADE)
	{
		if (cRefineElementType != REFINE_ELEMENT_TYPE_DOWNGRADE)
		{
			return false;
		}
	}
	else if (srcItem->GetSubType() == USE_ELEMENT_CHANGE)
	{
		if (cRefineElementType != REFINE_ELEMENT_TYPE_CHANGE)
		{
			return false;
		}

		if (bArg <= REFINE_ELEMENT_CATEGORY_NONE || bArg >= REFINE_ELEMENT_CATEGORY_MAX)
		{
			return false;
		}
	}
	else // It's not a good subtype
	{
		return false;
	}

	int64_t dwRefineCost = 10000000; // Default value in case some shit happen
	int iSuccessProb = 0;
	switch (cRefineElementType)
	{
	case REFINE_ELEMENT_TYPE_UPGRADE:
		dwRefineCost = REFINE_ELEMENT_UPGRADE_YANG;
		iSuccessProb = REFINE_ELEMENT_UPGRADE_PROBABILITY;
		break;

	case REFINE_ELEMENT_TYPE_DOWNGRADE:
		dwRefineCost = REFINE_ELEMENT_DOWNGRADE_YANG;
		iSuccessProb = REFINE_ELEMENT_DOWNGRADE_PROBABILITY;
		break;

	case REFINE_ELEMENT_TYPE_CHANGE:
		dwRefineCost = REFINE_ELEMENT_CHANGE_YANG;
		iSuccessProb = REFINE_ELEMENT_DOWNGRADE_PROBABILITY;
		break;

	default:
		dwRefineCost = 10000000;
		break;
	}

	if (GetGold() < dwRefineCost)
	{
		return false;
	}

	// Save and delete after
	BYTE bElementType = srcItem->GetValue(0);
	srcItem->SetCount(srcItem->GetCount() - 1);

	PointChange(POINT_GOLD, -dwRefineCost);

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(dwRefineCost) });
#endif

	int iRandomProb = number(1, 100);
	if (iRandomProb <= iSuccessProb)
	{
		// Succes
		// For upgrade / change
		BYTE bRefineElementType = dstItem->GetRefineElementType();
		BYTE bRefineElementPlus = dstItem->GetRefineElementPlus();
		BYTE bRefineElementBonusValue = dstItem->GetRefineElementBonusValue();
		BYTE bRefineElementAttackValue = dstItem->GetRefineElementAttackValue();

		// For downgrade
		BYTE bRefineElementLastIncBonus = dstItem->GetRefineElementLastIncBonus();
		BYTE bRefineElementLastIncAttack = dstItem->GetRefineElementLastIncAttack();

		if (cRefineElementType == REFINE_ELEMENT_TYPE_UPGRADE)
		{
			bRefineElementType = bElementType;
			bRefineElementPlus++;

			BYTE bIncreaseValue = number(REFINE_ELEMENT_RANDOM_VALUE_MIN, REFINE_ELEMENT_RANDOM_VALUE_MAX);
			bRefineElementBonusValue += bIncreaseValue;
			bRefineElementLastIncBonus = bIncreaseValue;

			BYTE bIncreaseAttackValue = number(REFINE_ELEMENT_RANDOM_BONUS_VALUE_MIN, REFINE_ELEMENT_RANDOM_BONUS_VALUE_MAX);
			bRefineElementAttackValue += bIncreaseAttackValue;
			bRefineElementLastIncAttack = bIncreaseAttackValue;

			char szRefineElement[10 + 1];
			snprintf(szRefineElement, sizeof(szRefineElement), "%d%d%02d%02d%d%02d",
					 bRefineElementType, bRefineElementPlus,
					 bRefineElementBonusValue, bRefineElementAttackValue,
					 bRefineElementLastIncBonus, bRefineElementLastIncAttack
					);

			DWORD dwRefineElement = atoi(szRefineElement);
			dstItem->SetRefineElement(dwRefineElement);

			SendRefineElementPacket(sRefineElementSrcCell, sRefineElementDstCell, REFINE_ELEMENT_TYPE_UPGRADE_SUCCES);
		}
		else if (cRefineElementType == REFINE_ELEMENT_TYPE_DOWNGRADE)
		{
			if (bRefineElementPlus == 1)
			{
				dstItem->SetRefineElement(0);
				SendRefineElementPacket(sRefineElementSrcCell, sRefineElementDstCell, REFINE_ELEMENT_TYPE_DOWNGRADE_SUCCES);
			}
			else
			{
				bRefineElementPlus--;
				bRefineElementBonusValue -= bRefineElementLastIncBonus;
				bRefineElementAttackValue -= bRefineElementLastIncAttack;
				bRefineElementLastIncBonus = MINMAX(0, bRefineElementLastIncBonus, bRefineElementBonusValue - 1);
				bRefineElementLastIncAttack = MINMAX(0, bRefineElementLastIncAttack, bRefineElementAttackValue - 1);

				char szRefineElement[10 + 1];
				snprintf(szRefineElement, sizeof(szRefineElement), "%d%d%02d%02d%d%02d",
						 bRefineElementType, bRefineElementPlus,
						 bRefineElementBonusValue, bRefineElementAttackValue,
						 bRefineElementLastIncBonus, bRefineElementLastIncAttack
						);

				DWORD dwRefineElement = atoi(szRefineElement);
				dstItem->SetRefineElement(dwRefineElement);

				SendRefineElementPacket(sRefineElementSrcCell, sRefineElementDstCell, REFINE_ELEMENT_TYPE_DOWNGRADE_SUCCES);
			}
		}
		else if (cRefineElementType == REFINE_ELEMENT_TYPE_CHANGE)
		{
			bRefineElementType = bArg;

			char szRefineElement[10 + 1];
			snprintf(szRefineElement, sizeof(szRefineElement), "%d%d%02d%02d%d%02d",
					 bRefineElementType, bRefineElementPlus,
					 bRefineElementBonusValue, bRefineElementAttackValue,
					 bRefineElementLastIncBonus, bRefineElementLastIncAttack
					);

			DWORD dwRefineElement = atoi(szRefineElement);
			dstItem->SetRefineElement(dwRefineElement);

			SendRefineElementPacket(sRefineElementSrcCell, sRefineElementDstCell, REFINE_ELEMENT_TYPE_CHANGE_SUCCES);
		}
	}
	else
	{
		// Fail
		// No change actually,
		// but send a notification in client
		SendRefineElementPacket(sRefineElementSrcCell, sRefineElementDstCell, REFINE_ELEMENT_TYPE_UPGRADE_FAIL);
	}

	return true;
}

void CHARACTER::SendRefineElementPacket(WORD wSrcCell, WORD wDstCell, BYTE bType)
{
	if (!GetDesc())
	{
		return;
	}

	if (!GetDesc()->IsPhase(PHASE_GAME) && !GetDesc()->IsPhase(PHASE_DEAD))
	{
		return;
	}

	TPacketGCRefineElement pack;
	pack.bHeader = HEADER_GC_REFINE_ELEMENT;
	pack.wSrcCell = wSrcCell;
	pack.wDstCell = wDstCell;
	pack.bType = bType;
	GetDesc()->Packet(&pack, sizeof(TPacketGCRefineElement));
}

bool CHARACTER::RefineElementInformation(WORD wSrcCell, WORD wDstCell, BYTE bType)
{
	if (wSrcCell >= INVENTORY_MAX_NUM || wDstCell >= INVENTORY_MAX_NUM)
	{
		return false;
	}

	if (bType > REFINE_ELEMENT_TYPE_CHANGE)
	{
		return false;
	}

	LPITEM srcItem = GetInventoryItem(wSrcCell);
	LPITEM dstItem = GetInventoryItem(wDstCell);

	if (!srcItem || !dstItem)
	{
		return false;
	}

	SendRefineElementPacket(wSrcCell, wDstCell, bType);

	m_cRefineElementType = bType;
	m_sRefineElementSrcCell = wSrcCell;
	m_sRefineElementDstCell = wDstCell;
	m_bUnderRefine = true;
	return true;
}
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
bool CHARACTER::DoRefineItemSoul(LPITEM item)
{
	if (!CanHandleItem(true))
	{
		ClearRefineMode();
		return false;
	}

	ClearRefineMode();

	LPITEM pkItemScroll;

	if (m_iRefineAdditionalCell < 0)
	{
		return false;
	}

	pkItemScroll = GetInventoryItem(m_iRefineAdditionalCell);

	if (!pkItemScroll)
	{
		return false;
	}

	if (!(pkItemScroll->GetType() == ITEM_USE && pkItemScroll->GetSubType() == USE_TUNING))
	{
		return false;
	}

	if (pkItemScroll->GetVnum() == item->GetVnum())
	{
		return false;
	}

	DWORD resultVnum = item->GetRefinedVnum();

	if (resultVnum == 0)
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("No further improvements possible."));
		return false;
	}

	TItemTable* pProto = ITEM_MANAGER::instance().GetTable(item->GetRefinedVnum());

	if (!pProto)
	{
		sys_err("DoRefineWithScroll NOT GET ITEM PROTO %d", item->GetRefinedVnum());
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This item cannot be improved."));
		return false;
	}

	int prob = number(1, 100);
	int successProb = pkItemScroll->GetValue(1);

	pkItemScroll->SetCount(pkItemScroll->GetCount() - 1);

	if (prob <= successProb)
	{
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(resultVnum, 1, 0, false);
		if (pkNewItem)
		{
			BYTE bCell = item->GetCell();
			ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulSuceeded");
			ITEM_MANAGER::instance().RemoveItem(item, "REMOVE (REFINE SUCCESS)");

			pkNewItem->AddToCharacter(this, TItemPos(INVENTORY, bCell));
			ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);
		}
		else
		{
			sys_err("Cannot create item soul %u", resultVnum);
			ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulFailed");
		}
	}
	else
	{
		ChatPacket(CHAT_TYPE_COMMAND, "RefineSoulFailed");
	}

	return true;
}
#endif

#ifdef __ENABLE_LUCKY_BOX__
void CHARACTER::LuckyBox(BYTE bAction)
{
	if (bAction > 1)
	{
		return;
	}

	if (!dwLuckyBoxVnum)
	{
		return;
	}

	if (bAction == 0)
	{
		if (bLuckyBoxOpenCount >= 15)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can change reward just 15 times."));
			return;
		}

		const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwLuckyBoxVnum);

		if (!pGroup)
		{
			sys_err("LUCKY_BOX: cannot find special item group %d", dwLuckyBoxVnum);
			return;
		}

		if (GetGold() < (int)dwLuckyBoxPrice)
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enought yang to refresh the prize."));
			return;
		}

		PointChange(POINT_GOLD, -dwLuckyBoxPrice);

#ifdef __BATTLE_PASS_ENABLE__
		CBattlePassManager::instance().TriggerEvent(this, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, static_cast<uint64_t>(dwLuckyBoxPrice) });
#endif

		bLuckyBoxOpenCount++;
		dwLuckyBoxPrice *= 2;

		TPacketGCLuckyBox pack;
		pack.bHeader = HEADER_GC_LUCKY_BOX;
		pack.dwBoxVnum = dwLuckyBoxVnum;
		pack.dwPrice = dwLuckyBoxPrice;
		pack.bIsOpen = false;

		int iRewardIndex = pGroup->GetOneIndex();
		if (iRewardIndex > LUCKY_BOX_MAX_NUM)
		{
			sys_err("LUCKY_BOX: wrong lucky box: reward index %d", iRewardIndex);
			return;
		}

		bLuckyBoxRewardIndex = iRewardIndex;

		for (int i = 0; i < LUCKY_BOX_MAX_NUM; i++)
		{
			pack.luckyItems[i].dwVnum = (i < pGroup->GetGroupSize()) ? pGroup->GetVnum(i) : 0;
			pack.luckyItems[i].bCount = (i < pGroup->GetGroupSize()) ? pGroup->GetCount(i) : 0;
			pack.luckyItems[i].bIsReward = (i < pGroup->GetGroupSize()) ? ((iRewardIndex == i ? true : false)) : false;
		}

		GetDesc()->Packet(&pack, sizeof(TPacketGCLuckyBox));
	}
	else
	{
		const CSpecialItemGroup* pGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(dwLuckyBoxVnum);

		if (!pGroup)
		{
			sys_err("LUCKY_BOX: cannot find special item group %d", dwLuckyBoxVnum);
			return;
		}

		ChatPacket(CHAT_TYPE_COMMAND, "CloseLuckyBox");

		AutoGiveItem(pGroup->GetVnum(bLuckyBoxRewardIndex), (BYTE)pGroup->GetCount(bLuckyBoxRewardIndex));

		dwLuckyBoxVnum = 0;
		bLuckyBoxRewardIndex = 0;
		bLuckyBoxOpenCount = 0;
		dwLuckyBoxPrice = 0;
	}
}
#endif

void CHARACTER::PickupClosesItems()
{
	std::unordered_set<DWORD> us_closest_items;
	ITEM_MANAGER::instance().GetClosestItems(this, us_closest_items);

	for (const auto& dwVID : us_closest_items)
	{
		PickupItem(dwVID);
	}
}

#ifdef __ENABLE_OFFLINE_SHOP__
bool CHARACTER::UseItemOpenOfflineShop(CItem* item)
{
	if (!item)
	{
		return false;
	}

	if (IsOpeningOfflineShop())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are already opening an offline shop."));
		return false;
	}

	item->Lock(true);
	SetOpeningOfflineShopState(true);
	SetOfflineShopOpeningItem(item);

	ChatPacket(CHAT_TYPE_COMMAND, fmt::format("StartOpeningOfflineShop {} {}",
			   item->GetValue(COfflineShop::ITEM_TIME_IDX), item->GetValue(COfflineShop::ITEM_GOLD_IDX)));

	return true;
}
#endif

void CHARACTER::RefreshEquipmentSet() {
	if (!IsItemLoaded())
		return;

	RemoveAffect(AFFECT_EQUIPMENT_SET);

	// Determine set id.
	static std::vector<EWearPositions> EQUIPMENT_ITEMS = {
		WEAR_BODY,         WEAR_HEAD,           WEAR_FOOTS,
		WEAR_WRIST,        WEAR_WEAPON,         WEAR_NECK,
		WEAR_EAR,          WEAR_SHIELD,         WEAR_COSTUME_BODY,
		WEAR_COSTUME_HAIR, WEAR_COSTUME_WEAPON,
	};

	std::map<uint32_t, uint32_t> counts;
	for (const auto& pos : EQUIPMENT_ITEMS) {
		auto item = GetWear(pos);
		if (!item)
			continue;

		auto id = item->GetEquipmentSetId();
		if (id == 0)
			continue;

		++counts[id];
	}

	for (const auto& kv : counts) {
		auto set = EquipmentSetSettings::instance().FindSet(kv.first);
		if (!set)
			return;

		set->ApplyTo(this, kv.second);
	}
}

