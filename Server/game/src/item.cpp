#include "stdafx.h"
#include "utils.h"
#include "config.h"
#include "char.h"
#include "desc.h"
#include "sectree_manager.h"
#include "packet.h"
#include "protocol.h"
#include "log.h"
#include "skill.h"
#include "unique_item.h"
#include "profiler.h"
#include "marriage.h"
#include "item_addon.h"
#include "dev_log.h"
#include "locale_service.h"
#include "item.h"
#include "item_manager.h"
#include "affect.h"
#include "DragonSoul.h"
#include "buff_on_attributes.h"
#include "belt_inventory_helper.h"
#include "../../common/VnumHelper.h"
#include "../../common/CommonDefines.h"

#ifdef __ITEM_TOGGLE_SYSTEM__
	#include "ItemUtils.h"
#endif

#ifdef __LEGENDARY_STONES_ENABLE__
	#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
	#include "SashSystemHelper.hpp"
#endif

#ifdef __POPUP_SYSTEM_ENABLE__
	#include "PopupManager.hpp"
#endif


#include <limits>

CItem::CItem(DWORD dwVnum)
	: m_dwVnum(dwVnum), m_bWindow(0), m_dwID(0), m_bEquipped(false), m_dwVID(0), m_wCell(0), m_dwCount(0), m_lFlag(0), m_dwLastOwnerPID(0),
	  m_bExchanging(false), m_pkDestroyEvent(NULL), m_pkExpireEvent(NULL), m_pkUniqueExpireEvent(NULL),
	  m_pkTimerBasedOnWearExpireEvent(NULL), m_pkRealTimeExpireEvent(NULL),
	  m_pkAccessorySocketExpireEvent(NULL), m_pkOwnershipEvent(NULL), m_dwOwnershipPID(0), m_bSkipSave(false), m_isLocked(false),
	  m_dwMaskVnum(0), m_dwSIGVnum (0)
#ifdef __ADMIN_MANAGER__
	, m_bIsGMOwner(false)
#endif
#ifdef __TRANSMUTATION_SYSTEM__
	, m_dwTransmutate(0)
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	, m_dwRefineElement(0)
#endif
#ifdef __ENABLE_SOUL_SYSTEM__
	, m_pkSoulItemEvent(NULL)
#endif
{
	memset( &m_alSockets, 0, sizeof(m_alSockets) );
	memset( &m_aAttr, 0, sizeof(m_aAttr) );
}

CItem::~CItem()
{
	Destroy();
}

void CItem::Initialize()
{
	CEntity::Initialize(ENTITY_ITEM);

	m_bWindow = RESERVED_WINDOW;
	m_pOwner = NULL;
	m_dwID = 0;
	m_bEquipped = false;
	m_dwVID = m_wCell = m_dwCount = m_lFlag = 0;
	m_pProto = NULL;
	m_bExchanging = false;
	memset(&m_alSockets, 0, sizeof(m_alSockets));
	memset(&m_aAttr, 0, sizeof(m_aAttr));

	m_pkDestroyEvent = NULL;
	m_pkOwnershipEvent = NULL;
	m_dwOwnershipPID = 0;
	m_pkUniqueExpireEvent = NULL;
	m_pkTimerBasedOnWearExpireEvent = NULL;
	m_pkRealTimeExpireEvent = NULL;

	m_pkAccessorySocketExpireEvent = NULL;

	m_bSkipSave = false;
	m_dwLastOwnerPID = 0;
#ifdef __ADMIN_MANAGER__
	m_bIsGMOwner = false;
#endif
#ifdef __TRANSMUTATION_SYSTEM__
	m_dwTransmutate = 0;
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
	m_dwRefineElement = 0;
#endif
#ifdef __ENABLE_SOUL_SYSTEM__
	m_pkSoulItemEvent = NULL;
#endif
}

void CItem::Destroy()
{
	event_cancel(&m_pkDestroyEvent);
	event_cancel(&m_pkOwnershipEvent);
	event_cancel(&m_pkUniqueExpireEvent);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);
	event_cancel(&m_pkRealTimeExpireEvent);
	event_cancel(&m_pkAccessorySocketExpireEvent);
#ifdef __ENABLE_SOUL_SYSTEM__
	event_cancel(&m_pkSoulItemEvent);
#endif

	CEntity::Destroy();

	if (GetSectree())
	{
		GetSectree()->RemoveEntity(this);
	}
}

EVENTFUNC(item_destroy_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "item_destroy_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetOwner())
	{
		sys_err("item_destroy_event: Owner exist. (item %s owner %s)", pkItem->GetName(), pkItem->GetOwner()->GetName());
	}

	pkItem->SetDestroyEvent(NULL);
	M2_DESTROY_ITEM(pkItem);
	return 0;
}

void CItem::SetDestroyEvent(LPEVENT pkEvent)
{
	m_pkDestroyEvent = pkEvent;
}

void CItem::StartDestroyEvent(int iSec)
{
	if (m_pkDestroyEvent)
	{
		return;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetDestroyEvent(event_create(item_destroy_event, info, PASSES_PER_SEC(iSec)));
}

bool CItem::IsStackable() const
{
	return IS_SET(GetFlag(), ITEM_FLAG_STACKABLE) &&
		!IS_SET(GetAntiFlag(), ITEM_ANTIFLAG_STACK);
}

bool CItem::CanStackWith(LPITEM otherItem) const
{
	if (!IsStackable() || IS_SET(GetAntiFlag(), ITEM_ANTIFLAG_STACK))
	{
		return false;
	}

	if (GetVnum() != otherItem->GetVnum())
	{
		return false;
	}

	if (this == otherItem)
	{
		return false;
	}

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		if (GetSocket(i) != otherItem->GetSocket(i))
		{
			return false;
		}

	return true;
}

void CItem::EncodeInsertPacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
	{
		return;
	}

	const PIXEL_POSITION & c_pos = GetXYZ();

	struct packet_item_ground_add pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_ADD;
	pack.x		= c_pos.x;
	pack.y		= c_pos.y;
	pack.z		= c_pos.z;
	pack.dwVnum		= GetVnum();
	pack.dwVID		= m_dwVID;
#ifdef __ENABLE_DROP_ITEM_COUNT__
	pack.count = m_dwCount;
#endif

	d->Packet(&pack, sizeof(pack));

	if (m_pkOwnershipEvent != NULL)
	{
		item_event_info * info = dynamic_cast<item_event_info *>(m_pkOwnershipEvent->info);

		if ( info == NULL )
		{
			sys_err( "CItem::EncodeInsertPacket> <Factor> Null pointer" );
			return;
		}

		TPacketGCItemOwnership p;

		p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
		p.dwVID = m_dwVID;
		strlcpy(p.szName, info->szOwnerName, sizeof(p.szName));

		d->Packet(&p, sizeof(TPacketGCItemOwnership));
	}
}

void CItem::EncodeRemovePacket(LPENTITY ent)
{
	LPDESC d;

	if (!(d = ent->GetDesc()))
	{
		return;
	}

	struct packet_item_ground_del pack;

	pack.bHeader	= HEADER_GC_ITEM_GROUND_DEL;
	pack.dwVID		= m_dwVID;

	d->Packet(&pack, sizeof(pack));
	sys_log(2, "Item::EncodeRemovePacket %s to %s", GetName(), ((LPCHARACTER) ent)->GetName());
}

void CItem::SetProto(const TItemTable * table)
{
	assert(table != NULL);
	m_pProto = table;
	SetFlag(m_pProto->dwFlags);
}

void CItem::UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use *packet)
{
	if (!GetVnum())
	{
		return;
	}

	packet->header 	= HEADER_GC_ITEM_USE;
	packet->ch_vid 	= ch->GetVID();
	packet->victim_vid 	= victim->GetVID();
	packet->Cell = TItemPos(GetWindow(), m_wCell);
	packet->vnum	= GetVnum();
}

void CItem::RemoveFlag(long bit)
{
	REMOVE_BIT(m_lFlag, bit);
}

void CItem::AddFlag(long bit)
{
	SET_BIT(m_lFlag, bit);
}

void CItem::UpdatePacket()
{
	if (!m_pOwner || !m_pOwner->GetDesc())
	{
		return;
	}

#ifdef __ENABLE_SWITCHBOT__
	if (m_bWindow == SWITCHBOT)
	{
		return;
	}
#endif

#ifdef __INVENTORY_BUFFERING__
	if (m_pOwner->IsInvBuffOn())
	{
		// Set it to be processed later
		m_pOwner->AddItemToInvBuff(this);
		return;
	}
#endif

	TPacketGCItemUpdate pack;

	pack.header = HEADER_GC_ITEM_UPDATE;
	pack.Cell = TItemPos(GetWindow(), m_wCell);
	pack.count	= m_dwCount;

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pack.alSockets[i] = m_alSockets[i];
	}

	thecore_memcpy(pack.aAttr, GetAttributes(), sizeof(pack.aAttr));

#ifdef __TRANSMUTATION_SYSTEM__
	pack.transmutate_id = m_dwTransmutate;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	pack.dwRefineElement = m_dwRefineElement;
#endif

	sys_log(2, "UpdatePacket %s -> %s", GetName(), m_pOwner->GetName());
	m_pOwner->GetDesc()->Packet(&pack, sizeof(pack));
}

CountType CItem::GetCount()
{
	if (GetType() == ITEM_ELK)
		return std::min<CountType>(m_dwCount,
			(std::numeric_limits<CountType>::max)());

	return std::min<CountType>(m_dwCount, GetItemMaxCount(this));
}

bool CItem::SetCount(CountType count)
{
	if (GetType() == ITEM_ELK)
	{
		m_dwCount = std::min<CountType>(count, max_gold);
	}
	else
	{
		m_dwCount = std::min<CountType>(count, GetItemMaxCount(this));
	}

	if (count == 0 && m_pOwner)
	{
		if (GetSubType() == USE_ABILITY_UP || GetSubType() == USE_POTION || GetVnum() == 70020)
		{
			LPCHARACTER pOwner = GetOwner();
			WORD wCell = GetCell();
#ifdef __ITEM_TOGGLE_SYSTEM__
			OnRemoveItem(GetOwner(), this);
#endif
			RemoveFromCharacter();

			if (!IsDragonSoul())
			{
				LPITEM pItem = pOwner->FindSpecifyItem(GetVnum());

				if (NULL != pItem)
				{
					pOwner->ChainQuickslotItem(pItem, QUICKSLOT_TYPE_ITEM, wCell);
				}
				else
				{
					pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, wCell, 255);
				}
			}

			M2_DESTROY_ITEM(this);
		}
		else
		{
#ifdef __ITEM_TOGGLE_SYSTEM__
			OnRemoveItem(GetOwner(), this);
#endif
			if (!IsDragonSoul())
			{
				m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, m_wCell, 255);
			}
			M2_DESTROY_ITEM(RemoveFromCharacter());
		}

		return false;
	}

	UpdatePacket();

	Save();
	return true;
}

LPITEM CItem::RemoveFromCharacter(bool bSwapping)
{
	if (!m_pOwner)
	{
		sys_err("Item::RemoveFromCharacter owner null");
		return (this);
	}

#ifdef __INVENTORY_BUFFERING__
	if (m_pOwner->IsInvBuffOn())
		// Item is removed - no longer need to buffer it
	{
		m_pOwner->RemoveItemFromInvBuff(this);
	}
#endif

	LPCHARACTER pOwner = m_pOwner;

	if (m_bEquipped)
	{
		Unequip(bSwapping);
		//pOwner->UpdatePacket();

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
	else
	{
		if (GetWindow() != SAFEBOX && GetWindow() != MALL)
		{
			if (IsDragonSoul())
			{
				if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
				{
					sys_err("CItem::RemoveFromCharacter: pos >= DRAGON_SOUL_INVENTORY_MAX_NUM");
				}
				else
				{
					pOwner->SetItem(TItemPos(m_bWindow, m_wCell), NULL);
				}
			}
#ifdef __ENABLE_SWITCHBOT__
			else if (m_bWindow == SWITCHBOT)
			{
				if (m_wCell >= SWITCHBOT_SLOT_COUNT)
				{
					sys_err("CItem::RemoveFromCharacter: pos >= SWITCHBOT_SLOT_COUNT");
				}
				else
				{
					pOwner->SetItem(TItemPos(SWITCHBOT, m_wCell), NULL);
				}
			}
#endif
			else
			{
				TItemPos cell(INVENTORY, m_wCell);

#ifndef __SPECIAL_STORAGE_ENABLE__
				if (false == cell.IsDefaultInventoryPosition() && false == cell.IsBeltInventoryPosition())
#else
				if (false == cell.IsDefaultInventoryPosition() && false == cell.IsBeltInventoryPosition() && !cell.IsSpecialStorage())
#endif
					sys_err("CItem::RemoveFromCharacter: Invalid Item Position");
				else
				{
					pOwner->SetItem(cell, NULL);
				}
			}
		}

		m_pOwner = NULL;
		m_wCell = 0;

		SetWindow(RESERVED_WINDOW);
		Save();
		return (this);
	}
}

bool CItem::AddToCharacter(LPCHARACTER ch, TItemPos Cell)
{
	assert(GetSectree() == NULL);
	assert(m_pOwner == NULL);

	WORD pos = Cell.cell;
	BYTE window_type = Cell.window_type;

	if (INVENTORY == window_type)
	{
		if (m_wCell >= INVENTORY_MAX_NUM && BELT_INVENTORY_SLOT_START > m_wCell)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}
	else if (DRAGON_SOUL_INVENTORY == window_type)
	{
		if (m_wCell >= DRAGON_SOUL_INVENTORY_MAX_NUM)
		{
			sys_err("CItem::AddToCharacter: cell overflow: %s to %s cell %d", m_pProto->szName, ch->GetName(), m_wCell);
			return false;
		}
	}

#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
	bool bWereMine = this->GetLastOwnerPID() == ch->GetPlayerID();
#endif

	if (ch->GetDesc())
	{
		m_dwLastOwnerPID = ch->GetPlayerID();
	}

	event_cancel(&m_pkDestroyEvent);

#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
	ch->SetItem(TItemPos(window_type, pos), this, bWereMine);
#else
	ch->SetItem(TItemPos(window_type, pos), this);
#endif
	m_pOwner = ch;

#ifdef __ADMIN_MANAGER__
	if (m_bIsGMOwner == GM_OWNER_UNSET)
	{
		m_bIsGMOwner = ch->IsGM() ? GM_OWNER_GM : GM_OWNER_PLAYER;
	}
#endif

	Save();
	return true;
}

LPITEM CItem::RemoveFromGround()
{
	if (GetSectree())
	{
		SetOwnership(NULL);

		GetSectree()->RemoveEntity(this);

		ViewCleanup();

		Save();
	}

	return (this);
}

bool CItem::AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck)
{
	if (0 == lMapIndex)
	{
		sys_err("wrong map index argument: %d", lMapIndex);
		return false;
	}

	if (GetSectree())
	{
		sys_err("sectree already assigned");
		return false;
	}

	if (!skipOwnerCheck && m_pOwner)
	{
		sys_err("owner pointer not null");
		return false;
	}

	LPSECTREE tree = SECTREE_MANAGER::instance().Get(lMapIndex, pos.x, pos.y);

	if (!tree)
	{
		sys_err("cannot find sectree by %dx%d", pos.x, pos.y);
		return false;
	}

	//tree->Touch();

	SetWindow(GROUND);
	SetXYZ(pos.x, pos.y, pos.z);
	tree->InsertEntity(this);
	UpdateSectree();
	Save();
	return true;
}

bool CItem::DistanceValid(LPCHARACTER ch)
{
	if (!GetSectree())
	{
		return false;
	}

	int iDist = DISTANCE_APPROX(GetX() - ch->GetX(), GetY() - ch->GetY());

	if (iDist > 600) // @fixme173 300 to 600
	{
		return false;
	}

	return true;
}

bool CItem::CanUsedBy(LPCHARACTER ch)
{
	// Anti flag check
	switch (ch->GetJob())
	{
	case JOB_WARRIOR:
		if (GetAntiFlag() & ITEM_ANTIFLAG_WARRIOR)
		{
			return false;
		}
		break;

	case JOB_ASSASSIN:
		if (GetAntiFlag() & ITEM_ANTIFLAG_ASSASSIN)
		{
			return false;
		}
		break;

	case JOB_SHAMAN:
		if (GetAntiFlag() & ITEM_ANTIFLAG_SHAMAN)
		{
			return false;
		}
		break;

	case JOB_SURA:
		if (GetAntiFlag() & ITEM_ANTIFLAG_SURA)
		{
			return false;
		}
		break;
#ifdef __ENABLE_WOLFMAN_CHARACTER__
	case JOB_WOLFMAN:
		if (GetAntiFlag() & ITEM_ANTIFLAG_WOLFMAN)
		{
			return false;
		}
		break;
#endif
	}

	return true;
}

int CItem::FindEquipCell(LPCHARACTER ch, int iCandidateCell)
{
	if (GetType() == ITEM_DS || GetType() == ITEM_SPECIAL_DS)
	{
		if (iCandidateCell < 0)
		{
			return WEAR_MAX_NUM + GetSubType();
		}
		else
		{
			for (int i = 0; i < DRAGON_SOUL_DECK_MAX_NUM; i++)
			{
				if (WEAR_MAX_NUM + i * DS_SLOT_MAX + GetSubType() == iCandidateCell)
				{
					return iCandidateCell;
				}
			}
			return -1;
		}
	}
	
	switch (GetType()) {
		case ITEM_WEAPON:
		{
			switch (GetSubType())
			{
				case WEAPON_ARROW:
				case WEAPON_QUIVER:
					return WEAR_ARROW;

				default:
					return WEAR_WEAPON;
			}
		}

		case ITEM_ARMOR: {
			switch (GetSubType()) {
				case ARMOR_BODY:
					return WEAR_BODY;
				case ARMOR_HEAD:
					return WEAR_HEAD;
				case ARMOR_SHIELD:
					return WEAR_SHIELD;
				case ARMOR_WRIST:
					return WEAR_WRIST;
				case ARMOR_FOOTS:
					return WEAR_FOOTS;
				case ARMOR_NECK:
					return WEAR_NECK;
				case ARMOR_EAR:
					return WEAR_EAR;

#ifdef __ENABLE_12ZI_ELEMENT_SLOT__
				case ARMOR_PENDANT:
					return WEAR_PENDANT;
#endif

				default:
					break;
			}
		}

		case ITEM_UNIQUE:
		case ITEM_RING:
		{
			const auto WEAR_SLOTS = { WEAR_UNIQUE1, WEAR_UNIQUE2, WEAR_UNIQUE3, WEAR_UNIQUE4 };
			for (const auto& wearSlot : WEAR_SLOTS)
			{
				if (ch->GetWear(wearSlot)) continue;
				return wearSlot;
			}
		}

		case ITEM_COSTUME: {
			switch (GetSubType()) {
				case COSTUME_BODY:
					return WEAR_COSTUME_BODY;

				case COSTUME_HAIR:
					return WEAR_COSTUME_HAIR;

				case COSTUME_WEAPON:
					return WEAR_COSTUME_WEAPON;

#ifdef __ENABLE_SASH_SYSTEM__
				case COSTUME_SASH:
					return WEAR_COSTUME_SASH;
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
				case COSTUME_MOUNT:
					return WEAR_COSTUME_MOUNT;
#endif

#ifdef __PET_SYSTEM__
				case COSTUME_PET:
					return WEAR_COSTUME_PET;
#endif

				default:
					break;
			}
		}

		case ITEM_BELT:
			return WEAR_BELT;

#ifdef __ENABLE_AMULET_SYSTEM__
		case ITEM_AMULET:
			return WEAR_AMULET;
#endif

		default:
			break;
	}

	return -1;
}

void CItem::ModifyPoints(bool bAdd)
{
	if (!this->GetOwner())
	{
		return;
	}

	int basicValuePercent = 100;

	int accessoryGrade;

	if (false == IsAccessoryForSocket())
	{
#ifdef __ENABLE_QUIVER_SYSTEM__
		if ((m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR) && m_pProto->bSubType != WEAPON_QUIVER)

#else
		if (m_pProto->bType == ITEM_WEAPON || m_pProto->bType == ITEM_ARMOR)
#endif
		{
			for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				DWORD dwVnum;

				if ((dwVnum = GetSocket(i)) <= 2)
				{
					continue;
				}

				TItemTable * p = ITEM_MANAGER::instance().GetTable(dwVnum);

				if (!p)
				{
					sys_err("cannot find table by vnum %u", dwVnum);
					continue;
				}

#ifdef __LEGENDARY_STONES_ENABLE__
				const auto iSocketIT = i;
#endif

				if (ITEM_METIN == p->bType)
				{
					for (const auto& [applyType, applyValue] : p->aApplies)
					{
						if (applyType == APPLY_NONE) continue;

						auto iValue = applyValue;

#ifdef __LEGENDARY_STONES_ENABLE__
						if (iSocketIT <= LegendaryStonesHandler::START_SOCKET_VALUES && GetSocket(LegendaryStonesHandler::START_SOCKET_VALUES + iSocketIT))
							iValue = GetSocket(LegendaryStonesHandler::START_SOCKET_VALUES + iSocketIT);
#endif

						if (applyType == APPLY_SKILL)
						{
							m_pOwner->ApplyPoint(applyType, bAdd ? iValue : iValue ^ 0x00800000);
						}
						else
						{
							m_pOwner->ApplyPoint(applyType, bAdd ? iValue : -iValue);
						}
					}
				}
			}
		}

#ifdef __SASH_ABSORPTION_ENABLE__
		if (SashSystemHelper::HasAbsorption(this))
		{
			for (size_t i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
			{
				DWORD dwVnum;

				if ((dwVnum = GetSocket(i)) <= 2)
				{
					continue;
				}

				const auto* p = ITEM_MANAGER::instance().GetTable(dwVnum);

				if (!p)
				{
					sys_err("cannot find table by vnum %u", dwVnum);
					continue;
				}

#ifdef __LEGENDARY_STONES_ENABLE__ 
				const auto iSocketIT = i;
#endif

				if (ITEM_METIN == p->bType)
				{
					for (auto aApplie : p->aApplies)
					{
						if (aApplie.bType == APPLY_NONE)
							continue;


						auto lValue = aApplie.lValue;

#ifdef __LEGENDARY_STONES_ENABLE__
						if (iSocketIT <= LegendaryStonesHandler::START_SOCKET_VALUES && GetSocket(LegendaryStonesHandler::START_SOCKET_VALUES + iSocketIT))
							lValue = GetSocket(LegendaryStonesHandler::START_SOCKET_VALUES + iSocketIT);
#endif

						lValue = std::max((long)1, static_cast<long>(static_cast<float>(lValue)* SashSystemHelper::GetAbsorptionValue(this) / 100.0f));

						if (lValue == 0)
							continue;

						if (aApplie.bType == APPLY_SKILL)
							m_pOwner->ApplyPoint(aApplie.bType, bAdd ? lValue : lValue ^ 0x00800000);
						else
							m_pOwner->ApplyPoint(aApplie.bType, bAdd ? lValue : -lValue);
					}
				}
			}
		}
#endif

		accessoryGrade = 0;
	}
	else
	{
		accessoryGrade = MIN(GetAccessorySocketGrade(), ITEM_ACCESSORY_SOCKET_MAX_NUM);
	}

	DWORD it = 0;
	for (const auto& apply : m_pProto->aApplies)
	{

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
		if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT) continue;
#endif

#ifdef __PET_SYSTEM__
		if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_PET) continue;
#endif

		long value = apply.lValue;

		if (apply.bType == APPLY_SKILL)
			m_pOwner->ApplyPoint(apply.bType,
				bAdd ? value : value ^ 0x00800000);
		else
		{
			if (0 != accessoryGrade)
			{
#ifdef __ENABLE_ORE_REFACTOR__
				value += GetAccessoryBonusValue(it);
#else
				value += MAX(accessoryGrade, value * aiAccessorySocketEffectivePct[accessoryGrade] / 100);
#endif
			}

			m_pOwner->ApplyPoint(apply.bType, bAdd ? value : -value);
		}
		it++;
	}

#ifdef __SASH_ABSORPTION_ENABLE__
	if (SashSystemHelper::HasAbsorption(this))
	{
		auto a_pProto = ITEM_MANAGER::instance().GetTable_NEW(SashSystemHelper::GetAbsorptionBase(this));
		if (a_pProto)
		{
			for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
			{
				if (a_pProto->aApplies[i].bType == APPLY_NONE)
					continue;

				long value = std::max((long)1, static_cast<long>(static_cast<float>(a_pProto->aApplies[i].lValue) * SashSystemHelper::GetAbsorptionValue(this) / 100.0f));
				if (value <= 0)
					continue;

				if (a_pProto->aApplies[i].bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(a_pProto->aApplies[i].bType, bAdd ? value : value ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(a_pProto->aApplies[i].bType, bAdd ? value : -value);
			}
		}
	}
#endif

	if (true == CItemVnumHelper::IsRamadanMoonRing(GetVnum()) || true == CItemVnumHelper::IsHalloweenCandy(GetVnum())
			|| true == CItemVnumHelper::IsHappinessRing(GetVnum()) || true == CItemVnumHelper::IsLovePendant(GetVnum()))
	{
		// Do not anything.
	}
	else
	{
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
			if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT) continue;
#endif

#ifdef __PET_SYSTEM__
			if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_PET) continue;
#endif

			if (GetAttributeType(i))
			{
				const TPlayerItemAttribute& ia = GetAttribute(i);

#ifdef __SASH_ABSORPTION_ENABLE__
				long iaValue = SashSystemHelper::HasAbsorption(this) ? std::max((long)1, static_cast<long>(static_cast<float>(ia.sValue) * SashSystemHelper::GetAbsorptionValue(this) / 100.0f)) : ia.sValue;
				if (iaValue <= 0)
					continue;

				if (ia.bType == APPLY_SKILL)
					m_pOwner->ApplyPoint(ia.bType, bAdd ? iaValue : iaValue ^ 0x00800000);
				else
					m_pOwner->ApplyPoint(ia.bType, bAdd ? iaValue : -iaValue);
#else
				if (ia.bType == APPLY_SKILL)
				{
					m_pOwner->ApplyPoint(ia.bType, bAdd ? ia.sValue : ia.sValue ^ 0x00800000);
				}
				else
				{
					m_pOwner->ApplyPoint(ia.bType, bAdd ? ia.sValue : -ia.sValue);
				}
#endif
			}
		}
	}

#ifdef __ENABLE_REFINE_ELEMENT__
	if (m_pProto->bType == ITEM_WEAPON && GetRefineElementType())
	{
		long lAttPlus = GetRefineElementAttackValue();
		if (lAttPlus > 0)
		{
			m_pOwner->ApplyPoint(APPLY_ATT_GRADE_BONUS, bAdd ? lAttPlus : -lAttPlus);
		}

		long lBonusValue = GetRefineElementBonusValue();
		if (lBonusValue > 0)
			// Bleah
		{
			m_pOwner->ApplyPoint(APPLY_ENCHANT_ELECT + (GetRefineElementType() - 1), bAdd ? lBonusValue : -lBonusValue);
		}
	}
#endif

	switch (m_pProto->bType)
	{
	case ITEM_PICK:
	case ITEM_ROD:
	{
		if (bAdd)
		{
			if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
			{
				m_pOwner->SetPart(PART_WEAPON, GetVnum());
			}
		}
		else
		{
			if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
			{
				m_pOwner->SetPart(PART_WEAPON, 0);
			}
		}
	}
	break;

	case ITEM_WEAPON:
	{
#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
		if (0 != m_pOwner->GetWear(WEAR_COSTUME_WEAPON))
		{
			break;
		}
#endif

		if (bAdd)
		{
			if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
#ifdef __TRANSMUTATION_SYSTEM__
				m_pOwner->SetPart(PART_WEAPON, GetTransmutate() ? GetTransmutate() : GetVnum());
#else
				m_pOwner->SetPart(PART_WEAPON, GetVnum());
#endif
		}
		else
		{
			if (m_wCell == INVENTORY_MAX_NUM + WEAR_WEAPON)
			{
				m_pOwner->SetPart(PART_WEAPON, 0);
			}
		}
	}
	break;

	case ITEM_ARMOR:
	{

		if (0 != m_pOwner->GetWear(WEAR_COSTUME_BODY))
		{
			break;
		}

		if (GetSubType() == ARMOR_BODY || GetSubType() == ARMOR_HEAD || GetSubType() == ARMOR_FOOTS || GetSubType() == ARMOR_SHIELD)
		{
			if (bAdd)
			{
				if (GetProto()->bSubType == ARMOR_BODY)
				{
#ifdef __TRANSMUTATION_SYSTEM__
					m_pOwner->SetPart(PART_MAIN, GetTransmutate() ? GetTransmutate() : GetVnum());
#else
					m_pOwner->SetPart(PART_MAIN, GetVnum());
#endif
				}
			}
			else
			{
				if (GetProto()->bSubType == ARMOR_BODY)
				{
					m_pOwner->SetPart(PART_MAIN, m_pOwner->GetOriginalPart(PART_MAIN));
				}
			}
		}
	}
	break;


	case ITEM_COSTUME:
	{
		DWORD toSetValue = this->GetVnum();
		EParts toSetPart = PART_MAX_NUM;


		if (GetSubType() == COSTUME_BODY)
		{
			toSetPart = PART_MAIN;

			if (false == bAdd)
			{

				const CItem* pArmor = m_pOwner->GetWear(WEAR_BODY);
				toSetValue = (NULL != pArmor) ? pArmor->GetVnum() : m_pOwner->GetOriginalPart(PART_MAIN);
			}

		}

		else if (GetSubType() == COSTUME_HAIR)
		{
			toSetPart = PART_HAIR;
			toSetValue = (true == bAdd) ? this->GetValue(3) : 0;
		}

#ifdef __ENABLE_SASH_SYSTEM__
		else if (GetSubType() == COSTUME_SASH)
		{
			toSetPart = PART_SASH;
			toSetValue = (true == bAdd) ? this->GetVnum() : 0;
		}
#endif

#ifdef __ENABLE_WEAPON_COSTUME_SYSTEM__
		else if (GetSubType() == COSTUME_WEAPON)
		{
			toSetPart = PART_WEAPON;
			if (false == bAdd)
			{
				const CItem* pWeapon = m_pOwner->GetWear(WEAR_WEAPON);
				if (pWeapon != NULL)
				{
#ifdef __TRANSMUTATION_SYSTEM__
					toSetValue = pWeapon->GetTransmutate() ? pWeapon->GetTransmutate() : pWeapon->GetVnum();
#else
					toSetValue = pWeapon->GetVnum();
#endif
				}
				else
				{
					toSetValue = m_pOwner->GetOriginalPart(PART_WEAPON);
				}
			}
		}
#endif

		if (PART_MAX_NUM != toSetPart)
		{
			m_pOwner->SetPart((BYTE)toSetPart, toSetValue);
			m_pOwner->UpdatePacket();
		}
	}
	break;
	case ITEM_UNIQUE:
	{
		if (0 != GetSIGVnum())
		{
			const CSpecialItemGroup* pItemGroup = ITEM_MANAGER::instance().GetSpecialItemGroup(GetSIGVnum());
			if (NULL == pItemGroup)
			{
				break;
			}
			DWORD dwAttrVnum = pItemGroup->GetAttrVnum(GetVnum());
			const CSpecialAttrGroup* pAttrGroup = ITEM_MANAGER::instance().GetSpecialAttrGroup(dwAttrVnum);
			if (NULL == pAttrGroup)
			{
				break;
			}
			for (itertype (pAttrGroup->m_vecAttrs) it = pAttrGroup->m_vecAttrs.begin(); it != pAttrGroup->m_vecAttrs.end(); it++)
			{
				m_pOwner->ApplyPoint(it->apply_type, bAdd ? it->apply_value : -it->apply_value);
			}
		}
	}
	break;
	}
}

bool CItem::IsEquipable() const
{
	switch (this->GetType())
	{
	case ITEM_COSTUME:
	case ITEM_ARMOR:
	case ITEM_WEAPON:
	case ITEM_ROD:
	case ITEM_PICK:
	case ITEM_UNIQUE:
	case ITEM_DS:
	case ITEM_SPECIAL_DS:
	case ITEM_RING:
	case ITEM_BELT:
#ifdef __ENABLE_AMULET_SYSTEM__
	case ITEM_AMULET:
#endif
		return true;
	}

	return false;
}

#define ENABLE_IMMUNE_FIX
// return false on error state
bool CItem::EquipTo(LPCHARACTER ch, BYTE bWearCell)
{
	if (!ch)
	{
		sys_err("EquipTo: nil character");
		return false;
	}


	if (IsDragonSoul())
	{
		if (bWearCell < WEAR_MAX_NUM || bWearCell >= WEAR_MAX_NUM + DRAGON_SOUL_DECK_MAX_NUM * DS_SLOT_MAX)
		{
			sys_err("EquipTo: invalid dragon soul cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetSubType(), bWearCell - WEAR_MAX_NUM);
			return false;
		}
	}
	else
	{
		if (bWearCell >= WEAR_MAX_NUM)
		{
			sys_err("EquipTo: invalid wear cell (this: #%d %s wearflag: %d cell: %d)", GetOriginalVnum(), GetName(), GetWearFlag(), bWearCell);
			return false;
		}
	}

	if (ch->GetWear(bWearCell))
	{
		sys_err("EquipTo: item already exist (this: #%d %s cell: %d %s)", GetOriginalVnum(), GetName(), bWearCell, ch->GetWear(bWearCell)->GetName());
		return false;
	}

	if (GetOwner())
	{
		RemoveFromCharacter();
	}

	ch->SetWear(bWearCell, this);

	m_pOwner = ch;
	m_bEquipped = true;
	m_wCell	= INVENTORY_MAX_NUM + bWearCell;

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			// m_pOwner->ChatPacket(CHAT_TYPE_INFO, "unequip immuneflag(%u)", m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag); // always 0
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif

	if (IsDragonSoul())
	{
		DSManager::instance().ActivateDragonSoul(this);
#ifdef __ENABLE_DS_SET__
		GetOwner()->DragonSoul_HandleSetBonus();
#endif
	}
	else
	{
		ModifyPoints(true);
		StartUniqueExpireEvent();
		if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
		{
			StartTimerBasedOnWearExpireEvent();
		}

		// ACCESSORY_REFINE
		StartAccessorySocketExpireEvent();
		// END_OF_ACCESSORY_REFINE
	}

	ch->BuffOnAttr_AddBuffsFromItem(this);

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT)
		m_pOwner->RefreshMount();
#endif

#ifdef __PET_SYSTEM__
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_PET)
	{
		if (m_pOwner->GetPetSystem())
			m_pOwner->GetPetSystem()->RefreshOverlay();
	}
#endif

	m_pOwner->ComputeBattlePoints();

	m_pOwner->UpdatePacket();

	Save();

	return (true);
}

bool CItem::Unequip(bool bSwapping)
{
	if (!m_pOwner || GetCell() < INVENTORY_MAX_NUM)
	{
		// ITEM_OWNER_INVALID_PTR_BUG
		sys_err("%s %u m_pOwner %p, GetCell %d",
				GetName(), GetID(), get_pointer(m_pOwner), GetCell());
		// END_OF_ITEM_OWNER_INVALID_PTR_BUG
		return false;
	}

	if (this != m_pOwner->GetWear(GetCell() - INVENTORY_MAX_NUM))
	{
		sys_err("m_pOwner->GetWear() != this");
		return false;
	}

	if (IsRideItem())
	{
		ClearMountAttributeAndAffect();
	}

	if (IsDragonSoul())
	{
		DSManager::instance().DeactivateDragonSoul(this);
	}
	else
	{
		ModifyPoints(false);
	}

	StopUniqueExpireEvent();

	if (-1 != GetProto()->cLimitTimerBasedOnWearIndex)
	{
		StopTimerBasedOnWearExpireEvent();
	}

	// ACCESSORY_REFINE
	StopAccessorySocketExpireEvent();
	// END_OF_ACCESSORY_REFINE


	m_pOwner->BuffOnAttr_RemoveBuffsFromItem(this);

	m_pOwner->SetWear(GetCell() - INVENTORY_MAX_NUM, NULL);

#ifndef ENABLE_IMMUNE_FIX
	DWORD dwImmuneFlag = 0;

	for (int i = 0; i < WEAR_MAX_NUM; ++i)
	{
		if (m_pOwner->GetWear(i))
		{
			// m_pOwner->ChatPacket(CHAT_TYPE_INFO, "unequip immuneflag(%u)", m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag); // always 0
			SET_BIT(dwImmuneFlag, m_pOwner->GetWear(i)->m_pProto->dwImmuneFlag);
		}
	}

	m_pOwner->SetImmuneFlag(dwImmuneFlag);
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	if ((GetType() == ITEM_COSTUME && GetSubType() == COSTUME_MOUNT) && !bSwapping)
		m_pOwner->RefreshMount(true);
#endif

#ifdef __PET_SYSTEM__
	if (GetType() == ITEM_COSTUME && GetSubType() == COSTUME_PET && !bSwapping)
	{
		if (m_pOwner->GetPetSystem())
			m_pOwner->GetPetSystem()->RefreshOverlay(true);
	}
#endif

	m_pOwner->ComputeBattlePoints();

	m_pOwner->UpdatePacket();

	m_pOwner = NULL;
	m_wCell = 0;
	m_bEquipped	= false;

	return true;
}

long CItem::GetValue(DWORD idx)
{
	assert(idx < ITEM_VALUES_MAX_NUM);
	return GetProto()->alValues[idx];
}

void CItem::SetExchanging(bool bOn)
{
	m_bExchanging = bOn;
}

void CItem::Save()
{
	if (m_bSkipSave)
	{
		return;
	}

	ITEM_MANAGER::instance().DelayedSave(this);
}

bool CItem::CreateSocket(BYTE bSlot, BYTE bGold)
{
	assert(bSlot < ITEM_SOCKET_MAX_NUM);

	if (m_alSockets[bSlot] != 0)
	{
		sys_err("Item::CreateSocket : socket already exist %s %d", GetName(), bSlot);
		return false;
	}

	if (bGold)
	{
		m_alSockets[bSlot] = 2;
	}
	else
	{
		m_alSockets[bSlot] = 1;
	}

	UpdatePacket();

	Save();
	return true;
}

void CItem::SetSockets(const long * c_al)
{
	thecore_memcpy(m_alSockets, c_al, sizeof(m_alSockets));
	Save();
}

void CItem::SetSockets(const std::array<int32_t, ITEM_SOCKET_MAX_NUM>& sockets)
{
	for (size_t i = 0; i < sockets.size(); ++i)
	{
		m_alSockets[i] = sockets[i];
	}

	Save();
}

void CItem::SetSocket(int i, long v, bool bLog)
{
	assert(i < ITEM_SOCKET_MAX_NUM);
	m_alSockets[i] = v;
	UpdatePacket();
	Save();
	if (bLog)
	{
#ifdef __ENABLE_NEWSTUFF__
		if (g_iDbLogLevel >= LOG_LEVEL_MAX)
#endif
			LogManager::instance().ItemLog(i, v, 0, GetID(), "SET_SOCKET", "", "", GetOriginalVnum());
	}
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t CItem::GetGold()
#else
	int CItem::GetGold()
#endif
{
	if (IS_SET(GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (GetProto()->dwGold == 0)
		{
			return GetCount();
		}
		else
		{
			return GetCount() / GetProto()->dwGold;
		}
	}
	else
	{
		return GetProto()->dwGold;
	}
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t CItem::GetShopBuyPrice()
#else
	int CItem::GetShopBuyPrice()
#endif
{
	return GetProto()->dwShopBuyPrice;
}

bool CItem::IsOwnership(LPCHARACTER ch)
{
	if (!m_pkOwnershipEvent)
	{
		return true;
	}

	return m_dwOwnershipPID == ch->GetPlayerID() ? true : false;
}

EVENTFUNC(ownership_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "ownership_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	pkItem->SetOwnershipEvent(NULL);

	TPacketGCItemOwnership p;

	p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID	= pkItem->GetVID();
	p.szName[0]	= '\0';

	pkItem->PacketAround(&p, sizeof(p));
	return 0;
}

void CItem::SetOwnershipEvent(LPEVENT pkEvent)
{
	m_pkOwnershipEvent = pkEvent;
}

void CItem::SetOwnership(LPCHARACTER ch, int iSec)
{
	if (!ch)
	{
		if (m_pkOwnershipEvent)
		{
			event_cancel(&m_pkOwnershipEvent);
			m_dwOwnershipPID = 0;

			TPacketGCItemOwnership p;

			p.bHeader	= HEADER_GC_ITEM_OWNERSHIP;
			p.dwVID	= m_dwVID;
			p.szName[0]	= '\0';

			PacketAround(&p, sizeof(p));
		}
		return;
	}

	if (m_pkOwnershipEvent)
	{
		return;
	}

	if (iSec <= 10)
	{
		iSec = 30;
	}

	m_dwOwnershipPID = ch->GetPlayerID();

	item_event_info* info = AllocEventInfo<item_event_info>();
	strlcpy(info->szOwnerName, ch->GetName(), sizeof(info->szOwnerName));
	info->item = this;

	SetOwnershipEvent(event_create(ownership_event, info, PASSES_PER_SEC(iSec)));

	TPacketGCItemOwnership p;

	p.bHeader = HEADER_GC_ITEM_OWNERSHIP;
	p.dwVID = m_dwVID;
	strlcpy(p.szName, ch->GetName(), sizeof(p.szName));

	PacketAround(&p, sizeof(p));
}

int CItem::GetSocketCount()
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; i++)
	{
		if (GetSocket(i) == 0)
		{
			return i;
		}
	}
	return ITEM_SOCKET_MAX_NUM;
}

bool CItem::AddSocket()
{
	int count = GetSocketCount();
	if (count == ITEM_SOCKET_MAX_NUM)
	{
		return false;
	}
	m_alSockets[count] = 1;
	return true;
}

void CItem::AlterToSocketItem(int iSocketCount)
{
	if (iSocketCount >= ITEM_SOCKET_MAX_NUM)
	{
		sys_log(0, "Invalid Socket Count %d, set to maximum", ITEM_SOCKET_MAX_NUM);
		iSocketCount = ITEM_SOCKET_MAX_NUM;
	}

	for (int i = 0; i < iSocketCount; ++i)
	{
		SetSocket(i, 1);
	}
}

void CItem::AlterToMagicItem()
{
	int idx = GetAttributeSetIndex();

	if (idx < 0)
	{
		return;
	}

	//      Appeariance Second Third
	// Weapon 50        20     5
	// Armor  30        10     2
	// Acc    20        10     1

	int iSecondPct;
	int iThirdPct;

	switch (GetType())
	{
	case ITEM_WEAPON:
		iSecondPct = 20;
		iThirdPct = 5;
		break;

	case ITEM_ARMOR:
	case ITEM_COSTUME:
		if (GetSubType() == ARMOR_BODY)
		{
			iSecondPct = 10;
			iThirdPct = 2;
		}
		else
		{
			iSecondPct = 10;
			iThirdPct = 1;
		}
		break;

	default:
		return;
	}


	PutAttribute(aiItemMagicAttributePercentHigh);

	if (number(1, 100) <= iSecondPct)
	{
		PutAttribute(aiItemMagicAttributePercentLow);
	}

	if (number(1, 100) <= iThirdPct)
	{
		PutAttribute(aiItemMagicAttributePercentLow);
	}
}

DWORD CItem::GetRefineFromVnum()
{
	return ITEM_MANAGER::instance().GetRefineFromVnum(GetVnum());
}

int CItem::GetRefineLevel()
{
	const char* name = GetBaseName();
	char* p = const_cast<char*>(strrchr(name, '+'));

	if (!p)
	{
		return 0;
	}

	int	rtn = 0;
	str_to_number(rtn, p + 1);

	const char* locale_name = GetName();
	p = const_cast<char*>(strrchr(locale_name, '+'));

	if (p)
	{
		int	locale_rtn = 0;
		str_to_number(locale_rtn, p + 1);
		if (locale_rtn != rtn)
		{
			sys_err("refine_level_based_on_NAME(%d) is not equal to refine_level_based_on_LOCALE_NAME(%d).", rtn, locale_rtn);
		}
	}

	return rtn;
}

bool CItem::IsPolymorphItem()
{
	return GetType() == ITEM_POLYMORPH;
}

EVENTFUNC(unique_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "unique_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	if (pkItem->GetValue(2) == 0)
	{
		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= 1)
		{
			sys_log(0, "UNIQUE_ITEM: expire %s %u", pkItem->GetName(), pkItem->GetID());
			pkItem->SetUniqueExpireEvent(NULL);
#ifdef __POPUP_SYSTEM_ENABLE__
			if (pkItem->GetOwner())
				CPopupManager::instance().ProcessPopup(pkItem->GetOwner(), NSPopupMessage::ECPopupTypes::LIMITYPE_END, pkItem);
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{
			pkItem->SetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME, pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - 1);
			return PASSES_PER_SEC(60);
		}
	}
	else
	{
		time_t cur = get_global_time();

		if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) <= cur)
		{
			pkItem->SetUniqueExpireEvent(NULL);
#ifdef __POPUP_SYSTEM_ENABLE__
			if (pkItem->GetOwner())
				CPopupManager::instance().ProcessPopup(pkItem->GetOwner(), NSPopupMessage::ECPopupTypes::LIMITYPE_END, pkItem);
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "UNIQUE_EXPIRE");
			return 0;
		}
		else
		{


			// by rtsummit
			if (pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur < 600)
			{
				return PASSES_PER_SEC(pkItem->GetSocket(ITEM_SOCKET_UNIQUE_REMAIN_TIME) - cur);
			}
			else
			{
				return PASSES_PER_SEC(600);
			}
		}
	}
}

EVENTFUNC(timer_based_on_wear_expire_event)
{
	item_event_info* info = dynamic_cast<item_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "expire_event <Factor> Null pointer" );
		return 0;
	}

	LPITEM pkItem = info->item;

	int remain_time = pkItem->GetSocket(ITEM_SOCKET_REMAIN_SEC) - 1;
	if (remain_time <= 0)
	{
		sys_log(0, "ITEM EXPIRED : expired %s %u", pkItem->GetName(), pkItem->GetID());
		pkItem->SetTimerBasedOnWearExpireEvent(NULL);
		pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, 0);


		if (pkItem->IsDragonSoul())
		{
			DSManager::instance().DeactivateDragonSoul(pkItem);
#ifdef __POPUP_SYSTEM_ENABLE__
			if (pkItem->GetOwner())
				CPopupManager::instance().ProcessPopup(pkItem->GetOwner(), NSPopupMessage::ECPopupTypes::ALCHEMY_STONE_END, pkItem);
#endif
		}
		else
		{
#ifdef __POPUP_SYSTEM_ENABLE__
			if (pkItem->GetOwner())
				CPopupManager::instance().ProcessPopup(pkItem->GetOwner(), NSPopupMessage::ECPopupTypes::LIMITYPE_END, pkItem);
#endif
			ITEM_MANAGER::instance().RemoveItem(pkItem, "TIMER_BASED_ON_WEAR_EXPIRE");
		}
		return 0;
	}

	pkItem->SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	return PASSES_PER_SEC(1);//(MIN (60, remain_time));
}

void CItem::SetUniqueExpireEvent(LPEVENT pkEvent)
{
	m_pkUniqueExpireEvent = pkEvent;
}

void CItem::SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent)
{
	m_pkTimerBasedOnWearExpireEvent = pkEvent;
}

EVENTFUNC(real_time_expire_event)
{
	const item_vid_event_info* info = reinterpret_cast<const item_vid_event_info*>(event->info);

	if (NULL == info)
	{
		return 0;
	}

	const LPITEM item = ITEM_MANAGER::instance().FindByVID( info->item_vid );

	if (NULL == item)
	{
		return 0;
	}

	const time_t current = get_global_time();

	if (current > item->GetSocket(0))
	{
		if (item->GetVnum() && item->IsNewMountItem()) // @fixme152
		{
			item->ClearMountAttributeAndAffect();
		}

#ifdef __POPUP_SYSTEM_ENABLE__
		if (item->GetOwner())
			CPopupManager::instance().ProcessPopup(item->GetOwner(), NSPopupMessage::ECPopupTypes::LIMITYPE_END, item);
#endif

		ITEM_MANAGER::instance().RemoveItem(item, "REAL_TIME_EXPIRE");

		return 0;
	}

	return PASSES_PER_SEC(1);
}

void CItem::OnRealTimeFirstUse()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; i++)
	{
		if (LIMIT_REAL_TIME_START_FIRST_USE == GetProto()->aLimits[i].bType)
		{
			if (0 == GetSocket(1))
			{
				long duration = (0 != GetSocket(0)) ? GetSocket(0) : GetProto()->aLimits[i].lValue;

				if (0 == duration)
				{
					duration = 60 * 60 * 24 * 7;
				}

				SetSocket(0, time(0) + duration);
				StartRealTimeExpireEvent();
			}

			if (false == IsEquipped())
			{
				SetSocket(1, GetSocket(1) + 1);
			}

			break;
		}
	}
}

void CItem::StartRealTimeExpireEvent()
{
	if (m_pkRealTimeExpireEvent) return;

	if (FindLimit(LIMIT_REAL_TIME) ||
		FindLimit(LIMIT_REAL_TIME_START_FIRST_USE)) {
		item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
		info->item_vid = GetVID();

		m_pkRealTimeExpireEvent = event_create(real_time_expire_event, info,
			PASSES_PER_SEC(1));
	}
}

bool CItem::IsRealTimeItem()
{
	if (!GetProto()) return false;

	return FindLimit(LIMIT_REAL_TIME);
}

void CItem::StartUniqueExpireEvent()
{
	if (GetType() != ITEM_UNIQUE)
	{
		return;
	}

	if (m_pkUniqueExpireEvent)
	{
		return;
	}


	if (IsRealTimeItem())
	{
		return;
	}

	if (GetProto()->cLimitTimerBasedOnWearIndex >= 0)
	{
		return;
	}

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
	{
		m_pOwner->ShowAlignment(false);
	}

	int iSec = GetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME);

	if (iSec == 0)
	{
		iSec = 60;
	}
	else
	{
		iSec = MIN(iSec, 60);
	}

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, 0);

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetUniqueExpireEvent(event_create(unique_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StartTimerBasedOnWearExpireEvent()
{
	if (m_pkTimerBasedOnWearExpireEvent)
	{
		return;
	}

	if (IsRealTimeItem())
	{
		return;
	}

	if (-1 == GetProto()->cLimitTimerBasedOnWearIndex)
	{
		return;
	}

	int iSec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
	if (iSec == -1)
	{
		return;
	}

	item_event_info* info = AllocEventInfo<item_event_info>();
	info->item = this;

	SetTimerBasedOnWearExpireEvent(event_create(timer_based_on_wear_expire_event, info, PASSES_PER_SEC(1)));
}

void CItem::StopUniqueExpireEvent()
{
	if (!m_pkUniqueExpireEvent)
	{
		return;
	}

	if (GetValue(2) != 0)
	{
		return;
	}

	// HARD CODING
	if (GetVnum() == UNIQUE_ITEM_HIDE_ALIGNMENT_TITLE)
	{
		m_pOwner->ShowAlignment(true);
	}

	SetSocket(ITEM_SOCKET_UNIQUE_SAVE_TIME, event_time(m_pkUniqueExpireEvent) / passes_per_sec);
	event_cancel(&m_pkUniqueExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::StopTimerBasedOnWearExpireEvent()
{
	if (!m_pkTimerBasedOnWearExpireEvent)
	{
		return;
	}

	int remain_time = GetSocket(ITEM_SOCKET_REMAIN_SEC) - event_processing_time(m_pkTimerBasedOnWearExpireEvent) / passes_per_sec;

	SetSocket(ITEM_SOCKET_REMAIN_SEC, remain_time);
	event_cancel(&m_pkTimerBasedOnWearExpireEvent);

	ITEM_MANAGER::instance().SaveSingleItem(this);
}

void CItem::ApplyAddon(int iAddonType)
{
	CItemAddonManager::instance().ApplyAddonTo(iAddonType, this);
}

int CItem::GetSpecialGroup() const
{
	return ITEM_MANAGER::instance().GetSpecialGroupFromItem(GetVnum());
}

bool CItem::IsAccessoryForSocket()
{
	return (m_pProto->bType == ITEM_ARMOR && (m_pProto->bSubType == ARMOR_WRIST || m_pProto->bSubType == ARMOR_NECK || m_pProto->bSubType == ARMOR_EAR)) ||
		   (m_pProto->bType == ITEM_BELT);
}

void CItem::SetAccessorySocketGrade(int iGrade)
{
	SetSocket(0, MINMAX(0, iGrade, GetAccessorySocketMaxGrade()));

#ifndef __ENABLE_ORE_REFACTOR__
	int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

	SetAccessorySocketDownGradeTime(iDownTime);
#endif
}

void CItem::SetAccessorySocketMaxGrade(int iMaxGrade)
{
	SetSocket(1, MINMAX(0, iMaxGrade, ITEM_ACCESSORY_SOCKET_MAX_NUM));
}

void CItem::SetAccessorySocketDownGradeTime(DWORD time, int iGrade, bool bNoRefresh)
{
#ifdef __ENABLE_ORE_REFACTOR__
	const BYTE START_TIME_SOCKET = 5;
	const BYTE iSocket = START_TIME_SOCKET + std::max<BYTE>(iGrade != -1 ? iGrade : (GetAccessorySocketGrade() - 1), 0);

	if (!bNoRefresh)
		SetSocket(iSocket, time);
	else
		m_alSockets[iSocket] = time;
#else
	SetSocket(2, time);
#endif
}

#ifdef __ENABLE_ORE_REFACTOR__
/*

	\brief Ability to hold various items in item accessories!
		-> Items are stored from socket 2-> socket 4,
		-> Items can be permament or limited time,
		-> Time is stored in item sockets ITEM_SOCKET + 3 => | SOCKET 2 = 56028 | -> | SOCKET 5 = 100 (in seconds) |
*/
void CItem::SetAccessorySocketItem(DWORD iVnum, bool bRemoved, bool bClearingItem)
{
	const int START_VNUM_SOCKET = 2;
	const int END_VNUM_SOCKET = 4;

	auto iCurrGrade = GetAccessorySocketGrade();
	int iSetter = std::min<int>(END_VNUM_SOCKET, START_VNUM_SOCKET + iCurrGrade);

	//We gonna set the vnum to 0 when we have removed (! NOT BY ITEM !) to be sure
	if (!bClearingItem)
	{
		SetSocket(iSetter, bRemoved ? 0 : iVnum);
		
		//We wanna to set time to 0/time for special ore
		if (iVnum > 0 && !AccessoryIsPermament(iVnum))
			SetSocket(iSetter + ITEM_ACCESSORY_SOCKET_MAX_NUM, 3600 * 36);
		else
			SetSocket(iSetter + ITEM_ACCESSORY_SOCKET_MAX_NUM, 0);
	}

	//Getting vector of whole items in item
	auto& vec = GetAccessoryItems();

	if (bClearingItem)
	{
		auto fIt = std::find_if(vec.begin(), vec.end(), [&iVnum](const std::pair<DWORD, DWORD>& rItem) { return (iVnum && iVnum == rItem.first); });
		if (fIt != vec.end())
		{
			vec.erase(fIt);
			vec.resize(vec.size() + 1);
		}
	}

	//Sorting by permament
	std::sort(vec.begin(), vec.end(), [&](std::pair<DWORD, DWORD>& iFirst, std::pair<DWORD, DWORD>& iSecond) {return AccessoryIsPermament(iFirst.first) > AccessoryIsPermament(iSecond.first); });

	size_t i = START_VNUM_SOCKET;
	for (const auto& rPair : vec)
	{
		SetSocket(i, rPair.first);
		SetSocket(i + 3, rPair.second);
		i++;
	}
}

void CItem::RemoveAccessorySocketItem()
{
	const int START_VNUM_SOCKET = 2;
	auto& vec = GetAccessoryItems();
	auto fIt = std::find_if(vec.begin(), vec.end(), [&](const std::pair<DWORD, DWORD>& rPair) {
		return rPair.second <= 1;
		});

	vec.erase(fIt);
	vec.resize(vec.size() + 1);

	size_t i = START_VNUM_SOCKET;
	for (const auto& [key, value] : vec)
	{
		SetSocket(i, key);
		SetSocket(i + 3, value);
		i++;
	}
}

bool CItem::IsAccessoryExpirable()
{
	auto& vec = GetAccessoryItems();
	for (const auto& rPair : vec)
	{
		if (!AccessoryIsPermament(rPair.first))
			return true;
	}

	return false;
}

bool CItem::AccessoryIsPermament(DWORD iVnum)
{
	if (iVnum == 0)
		return false;

	const TItemTable* pProto = ITEM_MANAGER::instance().GetTable(iVnum);
	if (!pProto)
		return false;

	return pProto->alValues[5] > 0;
}

int CItem::GetAccessoryBonusValue(int itApply)
{
	int iValue = 0;

	for (size_t i = 0; i < GetAccessorySocketGrade(); ++i)
	{
		DWORD dwVnum = GetSocket(2 + i);
		if (dwVnum == 0) continue;

		const TItemTable* pProto = ITEM_MANAGER::instance().GetTable(dwVnum);
		if (!pProto) continue;

		iValue += GetValue(itApply) * pProto->alValues[3];
	}

	return std::max<int>(1, iValue);
}

std::vector<std::pair<DWORD, DWORD>>& CItem::GetAccessoryItems(bool bSkipNormal, bool bSkipPermament)
{
	static std::vector<std::pair<DWORD, DWORD>> lVnums;
	lVnums.clear();

	//Declaring where we can find the vnums in the item 
	// ITEM_VNUM OF ORE ITEM | THE TIME FOR SPECIFIC ORE
	const auto ACCESSORIES_ITEMS = { 
		std::make_pair(GetSocket(2), GetSocket(5)),
		std::make_pair(GetSocket(3), GetSocket(6)),
		std::make_pair(GetSocket(4), GetSocket(7))
	};

	for (const auto& rPair : ACCESSORIES_ITEMS)
	{
		if (rPair.first == 0)
			continue;
		    
		if (bSkipNormal)
		{
			if (!AccessoryIsPermament(rPair.first))
				continue;
		}

		if (bSkipPermament)
		{
			if (AccessoryIsPermament(rPair.first))
				continue;
		}

		lVnums.push_back(rPair);
	}
	return lVnums;
}
#endif

void degrade(CItem* item)
{
	if (!item)
		return;

	item->SetAccessorySocketExpireEvent(nullptr);
	item->AccessorySocketDegrade();
}

EVENTFUNC(accessory_socket_expire_event)
{
	item_vid_event_info* info = dynamic_cast<item_vid_event_info*>( event->info );

	if ( info == NULL )
	{
		sys_err( "accessory_socket_expire_event> <Factor> Null pointer" );
		return 0;
	}

	LPITEM item = ITEM_MANAGER::instance().FindByVID(info->item_vid);
	
#ifdef __ENABLE_ORE_REFACTOR__
	auto& vec = item->GetAccessoryItems();

	if (vec.size() == 0)
		return 0;

	// Additional checking because we can have there a permament items
	if (!item->IsAccessoryExpirable())
		return 0;

	int i = -1;

	//TODO Remove whole items if times is 0 in one call
	for (const auto& rItem : vec)
	{
		i++;

		if (item->AccessoryIsPermament(rItem.first))
			continue;

		int tTime = rItem.second;

		if (tTime <= 1)
		{
			item->SetAccessorySocketDownGradeTime(0, i, true);
			degrade(item);
			return 0;
		}

		item->SetAccessorySocketDownGradeTime(tTime - 1, i, true);
	}

	// Refresh now
	item->UpdatePacket();
	item->Save();

	return PASSES_PER_SEC(1);
#else
	if (item->GetAccessorySocketDownGradeTime() <= 1)
		return 0;

	CHARACTER* m_pkOwner = item->GetOwner();
	auto now = get_dword_time();
	// Should expire if: Player was attacked in the last 60s, player attacked in
	// the last 60s.
	bool shouldExpire = !m_pkOwner || now - m_pkOwner->GetLastAttackTime() < 60000 ||
		now - m_pkOwner->GetLastHitReceivedTime() < 60000;

	int iTime = item->GetAccessorySocketDownGradeTime() - 60;

	if (iTime <= 1)
	{
		degrade(item);
		return 0;
	}

	if (shouldExpire)
		item->SetAccessorySocketDownGradeTime(iTime);

	if (iTime > 60)
		return PASSES_PER_SEC(60);

	return PASSES_PER_SEC(iTime);
#endif
}

void CItem::StartAccessorySocketExpireEvent()
{
	if (!IsAccessoryForSocket())
	{
		return;
	}

	if (m_pkAccessorySocketExpireEvent)
	{
		return;
	}

	if (GetAccessorySocketMaxGrade() == 0)
	{
		return;
	}

	if (GetAccessorySocketGrade() == 0)
	{
		return;
	}

#ifdef __ENABLE_ORE_REFACTOR__
	if (!IsAccessoryExpirable())
	{
		return;
	}

	int iSec = 1;
#else
	int iSec = GetAccessorySocketDownGradeTime();
	SetAccessorySocketExpireEvent(nullptr);

	if (iSec <= 1)
	{
		iSec = 5;
	}
	else
	{
		iSec = MIN(iSec, 60);
	}
#endif

	item_vid_event_info* info = AllocEventInfo<item_vid_event_info>();
	info->item_vid = GetVID();

	SetAccessorySocketExpireEvent(event_create(accessory_socket_expire_event, info, PASSES_PER_SEC(iSec)));
}

void CItem::StopAccessorySocketExpireEvent()
{
	if (!m_pkAccessorySocketExpireEvent)
	{
		return;
	}

	if (!IsAccessoryForSocket())
	{
		return;
	}

#ifdef __ENABLE_ORE_REFACTOR__
	if (!IsAccessoryExpirable())
	{
		return;
	}

	event_cancel(&m_pkAccessorySocketExpireEvent);
#else
	int new_time = GetAccessorySocketDownGradeTime() - (60 - event_time(m_pkAccessorySocketExpireEvent) / passes_per_sec);

	event_cancel(&m_pkAccessorySocketExpireEvent);

	if (new_time <= 1)
	{
		AccessorySocketDegrade();
	}
	else
	{
		SetAccessorySocketDownGradeTime(new_time);
	}
#endif
}

bool CItem::IsRideItem()
{
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType())
	{
		return true;
	}
	if (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == GetSubType())
	{
		return true;
	}

	return false;
}

bool CItem::IsRamadanRing()
{
	if (GetVnum() == UNIQUE_ITEM_RAMADAN_RING)
	{
		return true;
	}
	return false;
}

void CItem::ClearMountAttributeAndAffect()
{
	LPCHARACTER ch = GetOwner();
	if (!ch)
		return;

	ch->RemoveAffect(AFFECT_MOUNT);
	ch->RemoveAffect(AFFECT_MOUNT_BONUS);

	ch->MountVnum(0);

	ch->PointChange(POINT_ST, 0);
	ch->PointChange(POINT_DX, 0);
	ch->PointChange(POINT_HT, 0);
	ch->PointChange(POINT_IQ, 0);
}

// fixme

// by rtsummit
bool CItem::IsNewMountItem()
{
	return (
			   (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_RIDE == GetSubType() && IS_SET(GetFlag(), ITEM_FLAG_QUEST_USE))
			   || (ITEM_UNIQUE == GetType() && UNIQUE_SPECIAL_MOUNT_RIDE == GetSubType() && IS_SET(GetFlag(), ITEM_FLAG_QUEST_USE))
		   ); // @fixme152
}

void CItem::SetAccessorySocketExpireEvent(LPEVENT pkEvent)
{
	m_pkAccessorySocketExpireEvent = pkEvent;
}

void CItem::AccessorySocketDegrade()
{
	if (GetAccessorySocketGrade() > 0)
	{
		LPCHARACTER ch = GetOwner();

		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("A gem socketed in the %s has vanished."), GetName());
		}

		ModifyPoints(false);

#ifdef __ENABLE_ORE_REFACTOR__
		//SetAccessorySocketItem(0, true);
		RemoveAccessorySocketItem();
#endif

		SetAccessorySocketGrade(GetAccessorySocketGrade() - 1);

		ModifyPoints(true);

#ifndef __ENABLE_ORE_REFACTOR__
		int iDownTime = aiAccessorySocketDegradeTime[GetAccessorySocketGrade()];

		if (test_server)
		{
			iDownTime /= 60;
		}

		SetAccessorySocketDownGradeTime(iDownTime);

		if (iDownTime)
#else
		
		if (IsAccessoryExpirable())
#endif
		{
			StartAccessorySocketExpireEvent();
		}
	}
}

static const bool CanPutIntoRing(LPITEM ring, LPITEM item)
{
	//const DWORD vnum = item->GetVnum();
	return false;
}

#ifdef __ENABLE_ORE_REFACTOR__
bool CItem::CanPutInto(LPITEM item)
{
	if (item->GetType() == ITEM_BELT && this->GetSubType() != USE_PUT_INTO_BELT_SOCKET)
	{
		return false;
	}
	else if (item->GetType() != ITEM_ARMOR)
		return false;

	const DWORD iFormattedVnum = (item->GetVnum() / 10) * 10;

	const auto ACCESSORIES_ITEMS = { GetValue(0), GetValue(1), GetValue(2) };

	return std::find(ACCESSORIES_ITEMS.begin(), ACCESSORIES_ITEMS.end(), iFormattedVnum) != ACCESSORIES_ITEMS.end();
}
#else
bool CItem::CanPutInto(LPITEM item)
{
	if (item->GetType() == ITEM_BELT)
	{
		return this->GetSubType() == USE_PUT_INTO_BELT_SOCKET;
	}

	else if (item->GetType() == ITEM_RING)
	{
		return CanPutIntoRing(item, this);
	}

	else if (item->GetType() != ITEM_ARMOR)
	{
		return false;
	}

	DWORD vnum = item->GetVnum();

	struct JewelAccessoryInfo
	{
		DWORD jewel;
		DWORD wrist;
		DWORD neck;
		DWORD ear;
	};
	const static JewelAccessoryInfo infos[] =
	{
		{ 50634, 14420, 16220, 17220 },
		{ 50635, 14500, 16500, 17500 },
		{ 50636, 14520, 16520, 17520 },
		{ 50637, 14540, 16540, 17540 },
		{ 50638, 14560, 16560, 17560 },
		{ 50639, 14570, 16570, 17570 },
		{ 50640, 14580,	16580, 17580 },
	};

	DWORD item_type = (item->GetVnum() / 10) * 10;
	for (size_t i = 0; i < sizeof(infos) / sizeof(infos[0]); i++)
	{
		const JewelAccessoryInfo& info = infos[i];
		switch (item->GetSubType())
		{
		case ARMOR_WRIST:
			if (info.wrist == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_NECK:
			if (info.neck == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		case ARMOR_EAR:
			if (info.ear == item_type)
			{
				if (info.jewel == GetVnum())
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			break;
		}
	}
	if (item->GetSubType() == ARMOR_WRIST)
	{
		vnum -= 14000;
	}
	else if (item->GetSubType() == ARMOR_NECK)
	{
		vnum -= 16000;
	}
	else if (item->GetSubType() == ARMOR_EAR)
	{
		vnum -= 17000;
	}
	else
	{
		return false;
	}

	DWORD type = vnum / 20;

	if (type < 0 || type > 11)
	{
		type = (vnum - 170) / 20;

		if (50623 + type != GetVnum())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (item->GetVnum() >= 16210 && item->GetVnum() <= 16219)
	{
		if (50625 != GetVnum())
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	else if (item->GetVnum() >= 16230 && item->GetVnum() <= 16239)
	{
		if (50626 != GetVnum())
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return 50623 + type == GetVnum();
}
#endif

// PC_BANG_ITEM_ADD
bool CItem::IsPCBangItem()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (m_pProto->aLimits[i].bType == LIMIT_PCBANG)
		{
			return true;
		}
	}
	return false;
}
// END_PC_BANG_ITEM_ADD

bool CItem::CheckItemUseLevel(int nLevel)
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			if (this->m_pProto->aLimits[i].lValue > nLevel) { return false; }
			else { return true; }
		}
	}
	return true;
}

long CItem::FindApplyValue(BYTE bApplyType)
{
	if (m_pProto == NULL)
	{
		return 0;
	}

	for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
	{
		if (m_pProto->aApplies[i].bType == bApplyType)
		{
			return m_pProto->aApplies[i].lValue;
		}
	}

	return 0;
}

void CItem::CopySocketTo(LPITEM pItem)
{
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		pItem->m_alSockets[i] = m_alSockets[i];
	}
}

int CItem::GetAccessorySocketGrade()
{
	return MINMAX(0, GetSocket(0), GetAccessorySocketMaxGrade());
}

int CItem::GetAccessorySocketMaxGrade()
{
	return MINMAX(0, GetSocket(1), ITEM_ACCESSORY_SOCKET_MAX_NUM);
}

int CItem::GetAccessorySocketDownGradeTime()
{
	return MINMAX(0, GetSocket(2), aiAccessorySocketDegradeTime[GetAccessorySocketGrade()]);
}

void CItem::AttrLog()
{
	const char * pszIP = NULL;

	if (GetOwner() && GetOwner()->GetDesc())
	{
		pszIP = GetOwner()->GetDesc()->GetHostName();
	}

	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		if (m_alSockets[i])
		{
#ifdef __ENABLE_NEWSTUFF__
			if (g_iDbLogLevel >= LOG_LEVEL_MAX)
#endif
				LogManager::instance().ItemLog(i, m_alSockets[i], 0, GetID(), "INFO_SOCKET", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}

	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		int	type	= m_aAttr[i].bType;
		int value	= m_aAttr[i].sValue;

		if (type)
		{
#ifdef __ENABLE_NEWSTUFF__
			if (g_iDbLogLevel >= LOG_LEVEL_MAX)
#endif
				LogManager::instance().ItemLog(i, type, value, GetID(), "INFO_ATTR", "", pszIP ? pszIP : "", GetOriginalVnum());
		}
	}
}

int CItem::GetLevelLimit()
{
	for (int i = 0; i < ITEM_LIMIT_MAX_NUM; ++i)
	{
		if (this->m_pProto->aLimits[i].bType == LIMIT_LEVEL)
		{
			return this->m_pProto->aLimits[i].lValue;
		}
	}
	return 0;
}

bool CItem::OnAfterCreatedItem()
{
	if (-1 != this->GetProto()->cLimitRealTimeFirstUseIndex)
	{

		if (0 != GetSocket(1))
		{
			StartRealTimeExpireEvent();
		}
	}

#ifdef __ENABLE_SOUL_SYSTEM__
	if (GetType() == ITEM_TOGGLE && (GetSubType() == TOGGLE_RED_SOUL || GetSubType() == TOGGLE_BLUE_SOUL))
	{
		StartSoulItemEvent();
	}
#endif

	return true;
}

bool CItem::IsDragonSoul()
{
	return GetType() == ITEM_DS;
}

int CItem::GiveMoreTime_Per(float fPercent)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		DWORD given_time = fPercent * duration / 100u;
		if (remain_sec == duration)
		{
			return false;
		}
		if ((given_time + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, given_time + remain_sec);
			return given_time;
		}
	}

	else
	{
		return 0;
	}
}

int CItem::GiveMoreTime_Fix(DWORD dwTime)
{
	if (IsDragonSoul())
	{
		DWORD duration = DSManager::instance().GetDuration(this);
		DWORD remain_sec = GetSocket(ITEM_SOCKET_REMAIN_SEC);
		if (remain_sec == duration)
		{
			return false;
		}
		if ((dwTime + remain_sec) >= duration)
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, duration);
			return duration - remain_sec;
		}
		else
		{
			SetSocket(ITEM_SOCKET_REMAIN_SEC, dwTime + remain_sec);
			return dwTime;
		}
	}

	else
	{
		return 0;
	}
}

int	CItem::GetDuration()
{
	if (!GetProto())
	{
		return -1;
	}

	auto limit = FindLimit(LIMIT_REAL_TIME_START_FIRST_USE);
	if (limit)
		return limit->lValue;

	limit = FindLimit(LIMIT_REAL_TIME);
	if (limit)
		return limit->lValue;

	limit = FindLimit(LIMIT_TIMER_BASED_ON_WEAR);
	if (limit)
		return limit->lValue;

	return -1;
}

bool CItem::IsSameSpecialGroup(const LPITEM item) const
{
	if (this->GetVnum() == item->GetVnum())
	{
		return true;
	}

#ifdef __ENABLE_FIX_GIFTBOX_GROUP__
	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
	{
		TItemTable* p = ITEM_MANAGER::instance().GetTable(GetSpecialGroup());
		if (!p)
		{
			return true;
		}

		if (p->bType != ITEM_GIFTBOX)
		{
			return true;
		}
	}
#else
	if (GetSpecialGroup() && (item->GetSpecialGroup() == GetSpecialGroup()))
	{
		return true;
	}
#endif

	return false;
}

#ifdef __SPECIAL_STORAGE_ENABLE__
DWORD CItem::GetVirtualWindow()
{
	DWORD dwVirtualWindow = m_pProto ? m_pProto->dwVirtualWindow : 0;
	if (dwVirtualWindow >= SPECIAL_STORAGE_START_WINDOW && dwVirtualWindow < SPECIAL_STORAGE_END_WINDOW)
	{
		return dwVirtualWindow;
	}

	// Return INVENTORY by default
	return INVENTORY;
}

DWORD CItem::GetVirtualWindow(const TItemTable * m_pProto)
{
	DWORD dwVirtualWindow = m_pProto ? m_pProto->dwVirtualWindow : 0;
	if (dwVirtualWindow >= SPECIAL_STORAGE_START_WINDOW && dwVirtualWindow < SPECIAL_STORAGE_END_WINDOW)
	{
		return dwVirtualWindow;
	}

	// Return INVENTORY by default
	return INVENTORY;
}
#endif

#ifdef __ENABLE_RENEWAL_EXCHANGE__
std::string CItem::GetHyperlink()
{
	char itemlink[256];
	std::string attrString = "";
	bool hasAttribute = false;

	for (int i = 0; i < 5; ++i)
	{
		if (GetAttributeType(i) != 0)
		{
			//Reuse buffer from start every time, we are storing at attrString
			snprintf(itemlink, sizeof(itemlink), ":%x:%d",
					 GetAttributeType(i), GetAttributeValue(i));

			attrString += itemlink;
			hasAttribute = true;
		}
	}

	std::string color = hasAttribute ? "cffffc700" : "cfff1e6c0";
	snprintf(itemlink, sizeof(itemlink), "|%s|Hitem:%x:%ld:%ld:%ld:%ld%s|h[%s]|h|r",
			 color.c_str(),
			 GetVnum(), GetFlag(),
			 GetSocket(0), GetSocket(1), GetSocket(2), attrString.c_str(),
			 GetName());

	return itemlink;
}
#endif

#ifdef __TRANSMUTATION_SYSTEM__
void CItem::SetTransmutate(DWORD trans_id)
{
	m_dwTransmutate = trans_id;
	UpdatePacket();
	Save();
}
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
void CItem::SetRefineElement(DWORD dwRefineElement)
{
	m_dwRefineElement = dwRefineElement;
	UpdatePacket();
	Save();
}
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
EVENTFUNC(soul_item_event)
{
	const item_vid_event_info* pInfo = reinterpret_cast<item_vid_event_info*>(event->info);
	if (!pInfo)
	{
		return 0;
	}

	const LPITEM pItem = ITEM_MANAGER::instance().FindByVID(pInfo->item_vid);
	if (!pItem)
	{
		return 0;
	}

	int iCurrentMinutes = (pItem->GetSocket(2) / 10000);
	int iCurrentStrike = (pItem->GetSocket(2) % 10000);
	int iNextMinutes = iCurrentMinutes + 1;

	if (iNextMinutes >= pItem->GetLimitValue(1))
	{
		pItem->SetSocket(2, (pItem->GetLimitValue(1) * 10000 + iCurrentStrike)); // just in case
		pItem->SetSoulItemEvent(NULL);
		return 0;
	}

	pItem->SetSocket(2, (iNextMinutes * 10000 + iCurrentStrike));

	if (test_server)
	{
		return PASSES_PER_SEC(5);
	}

	return PASSES_PER_SEC(60);
}

void CItem::SetSoulItemEvent(LPEVENT pkEvent)
{
	m_pkSoulItemEvent = pkEvent;
}

void CItem::StartSoulItemEvent()
{
	if (GetType() != ITEM_TOGGLE && (GetSubType() != TOGGLE_RED_SOUL || GetSubType() != TOGGLE_BLUE_SOUL))
	{
		return;
	}

	if (m_pkSoulItemEvent)
	{
		return;
	}

	int iMinutes = (GetSocket(2) / 10000);
	if (iMinutes >= GetLimitValue(1))
	{
		return;
	}

	item_vid_event_info* pInfo = AllocEventInfo<item_vid_event_info>();
	pInfo->item_vid = GetVID();
	SetSoulItemEvent(event_create(soul_item_event, pInfo, PASSES_PER_SEC(test_server ? 5 : 60)));
}
#endif

const TItemLimit* CItem::FindLimit(uint8_t type) const
{
	for (const auto& limit : m_pProto->aLimits)
	{
		if (limit.bType == type)
		{
			return &limit;
		}
	}

	return nullptr;
}

const TItemApply* CItem::FindApply(uint8_t type) const
{
	for (const auto& apply : m_pProto->aApplies)
	{
		if (apply.bType == type)
		{
			return &apply;
		}
	}

	return nullptr;
}

void CItem::CopySockets(std::array<int32_t, ITEM_SOCKET_MAX_NUM>& sockets)
{
	std::copy(std::begin(m_alSockets), std::end(m_alSockets), sockets.begin());
}

void CItem::CopyAttributes(std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM>& attributes)
{
	std::copy(std::begin(m_aAttr), std::end(m_aAttr), attributes.begin());
}

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
const char* CItem::GetName() const
{
	const auto* proto = ITEM_MANAGER::instance().GetTable(this->GetVnum());
	if (proto)
		return LC::TranslateItemName(this->GetVnum(), m_pOwner ? m_pOwner->GetLocale() : LC_DEFAULT);

	return "";
}
#endif
