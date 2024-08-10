#include "stdafx.h"
#include "char.h"
#include "item.h"
#include "desc.h"
#include "DragonSoul.h"
#include "log.h"

#include <numeric>
#include <algorithm>







void CHARACTER::DragonSoul_Initialize()
{
	for (int i = INVENTORY_MAX_NUM + WEAR_MAX_NUM; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		LPITEM pItem = GetItem(TItemPos(INVENTORY, i));
		if (NULL != pItem)
		{
			pItem->SetSocket(ITEM_SOCKET_DRAGON_SOUL_ACTIVE_IDX, 0);
		}
	}

	if (FindAffect(AFFECT_DRAGON_SOUL_DECK_0))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_0);
	}
	else if (FindAffect(AFFECT_DRAGON_SOUL_DECK_1))
	{
		DragonSoul_ActivateDeck(DRAGON_SOUL_DECK_1);
	}
}

int CHARACTER::DragonSoul_GetActiveDeck() const
{
	return m_pointsInstant.iDragonSoulActiveDeck;
}

bool CHARACTER::DragonSoul_IsDeckActivated() const
{
	return m_pointsInstant.iDragonSoulActiveDeck >= 0;
}

bool CHARACTER::DragonSoul_IsQualified() const
{
	return true;
	// return FindAffect(AFFECT_DRAGON_SOUL_QUALIFIED) != NULL;
}

void CHARACTER::DragonSoul_GiveQualification()
{
	if (NULL == FindAffect(AFFECT_DRAGON_SOUL_QUALIFIED))
	{
		LogManager::instance().CharLog(this, 0, "DS_QUALIFIED", "");
	}
	AddAffect(AFFECT_DRAGON_SOUL_QUALIFIED, APPLY_NONE, 0, AFF_NONE, INFINITE_AFFECT_DURATION, 0, false, false);
	//SetQuestFlag("dragon_soul.is_qualified", 1);
	//PointChange(POINT_DRAGON_SOUL_IS_QUALIFIED, 1 - GetPoint(POINT_DRAGON_SOUL_IS_QUALIFIED));
}

bool CHARACTER::DragonSoul_ActivateDeck(int deck_idx)
{
	if (deck_idx < DRAGON_SOUL_DECK_0 || deck_idx >= DRAGON_SOUL_DECK_MAX_NUM)
	{
		return false;
	}

	if (DragonSoul_GetActiveDeck() == deck_idx)
	{
		return true;
	}

	DragonSoul_DeactivateAll();

	if (!DragonSoul_IsQualified())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("용혼석 상자가 활성화되지 않았습니다."));
		return false;
	}

	AddAffect(AFFECT_DRAGON_SOUL_DECK_0 + deck_idx, APPLY_NONE, 0, 0, INFINITE_AFFECT_DURATION, 0, false);

	m_pointsInstant.iDragonSoulActiveDeck = deck_idx;

	for (int i = DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * deck_idx;
			i < DRAGON_SOUL_EQUIP_SLOT_START + DS_SLOT_MAX * (deck_idx + 1); i++)
	{
		LPITEM pItem = GetInventoryItem(i);
		if (NULL != pItem)
		{
			DSManager::instance().ActivateDragonSoul(pItem);
		}
	}

#ifdef __ENABLE_DS_SET__
	DragonSoul_HandleSetBonus();
#endif

	return true;
}

void CHARACTER::DragonSoul_DeactivateAll()
{
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		DSManager::instance().DeactivateDragonSoul(GetInventoryItem(i), true);
	}

#ifdef __ENABLE_DS_SET__
	DragonSoul_HandleSetBonus();
#endif

	m_pointsInstant.iDragonSoulActiveDeck = -1;
	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_0);
	RemoveAffect(AFFECT_DRAGON_SOUL_DECK_1);

#ifdef __ENABLE_DS_SET__
	RemoveAffect(AFFECT_DS_SET);
#endif
}

#ifdef __ENABLE_DS_SET__
void CHARACTER::DragonSoul_HandleSetBonus()
{
	bool bAdd = true;
	uint8_t iSetGrade;;
	if (!DSManager::instance().GetDSSetGrade(this, iSetGrade))
	{
		auto pkAffect = FindAffect(AFFECT_DS_SET);
		if (!pkAffect) { return; }

		iSetGrade = static_cast<uint8_t>(pkAffect->lApplyValue);
		bAdd = false;
	}
	else { AddAffect(AFFECT_DS_SET, APPLY_NONE, iSetGrade, 0, INFINITE_AFFECT_DURATION, 0, true); }

	const uint8_t iDeckIdx = DragonSoul_GetActiveDeck();
	const uint8_t iStartSlotIndex = WEAR_MAX_NUM + (iDeckIdx * DS_SLOT_MAX);
	const uint8_t iEndSlotIndex = iStartSlotIndex + DS_SLOT_MAX;

	for (uint8_t iSlotIndex = iStartSlotIndex; iSlotIndex < iEndSlotIndex; ++iSlotIndex)
	{
		const auto pkItem = GetWear(iSlotIndex);
		if (!pkItem) { return; }

		for (uint8_t i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pkItem->GetAttributeType(i))
			{
				const TPlayerItemAttribute& ia = pkItem->GetAttribute(i);

				static float dsSetValues[DRAGON_SOUL_STEP_MAX] = {
					20.0, 20.0, 20.0, 20.0, 25.0
				};

				auto fWeight = dsSetValues[iSetGrade] / 100.0f;

				int iSetValue = ia.sValue * fWeight;
				ApplyPoint(ia.bType, bAdd ? iSetValue : -iSetValue);
			}
		}
	}
}
#endif

void CHARACTER::DragonSoul_CleanUp()
{
	for (int i = DRAGON_SOUL_EQUIP_SLOT_START; i < DRAGON_SOUL_EQUIP_SLOT_END; i++)
	{
		DSManager::instance().DeactivateDragonSoul(GetInventoryItem(i), true);
	}
}

bool CHARACTER::DragonSoul_RefineWindow_Open(LPENTITY pEntity)
{
	if (NULL == m_pointsInstant.m_pDragonSoulRefineWindowOpener)
	{
		m_pointsInstant.m_pDragonSoulRefineWindowOpener = pEntity;
	}

	TPacketGCDragonSoulRefine PDS;
	PDS.header = HEADER_GC_DRAGON_SOUL_REFINE;
	PDS.bSubType = DS_SUB_HEADER_OPEN;
	LPDESC d = GetDesc();

	if (NULL == d)
	{
		sys_err ("User(%s)'s DESC is NULL POINT.", GetName());
		return false;
	}

	d->Packet(&PDS, sizeof(PDS));
	return true;
}

bool CHARACTER::DragonSoul_RefineWindow_Close()
{
	m_pointsInstant.m_pDragonSoulRefineWindowOpener = NULL;
	return true;
}

bool CHARACTER::DragonSoul_RefineWindow_CanRefine()
{
	return NULL != m_pointsInstant.m_pDragonSoulRefineWindowOpener;
}

int CHARACTER::DragonSoul_GetEmptyInventoryTypeCount(BYTE bSize, BYTE bGrade)
{
	if (!DragonSoul_IsQualified())
		return 0;

	std::array<int, DS_SLOT_MAX> vSlots; std::iota(vSlots.begin(), vSlots.end(), 0);
	std::array<int, DRAGON_SOUL_BOX_SIZE> vItems; std::iota(vItems.begin(), vItems.end(), 0);
	
	return std::count_if(vSlots.begin(), vSlots.end(), [&] (const int iType) { return std::any_of(vItems.begin(), vItems.end(), [&] (const int iSlot) { return IsEmptyItemGrid(TItemPos(DRAGON_SOUL_INVENTORY, (iType * DRAGON_SOUL_STEP_MAX * DRAGON_SOUL_BOX_SIZE) + (bGrade * DRAGON_SOUL_BOX_SIZE) + iSlot),
			bSize); }); });
}

