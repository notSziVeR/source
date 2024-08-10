#include "stdafx.h"
#ifdef __ITEM_TOGGLE_SYSTEM__
#include "../../common/tables.h"
#include "item.h"
#include "item_manager.h"
#include "char.h"
#include "ItemUtils.h"
#include "../../common/VnumHelper.h"
#include "skill.h"
#include "motion.h"
#include "PetSystem.h"
#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
#include "CharUtil.hpp"
#endif
#include "utils.h"

#ifdef __LEGENDARY_STONES_ENABLE__
#include "LegendaryStonesHandler.hpp"
#endif

#ifdef __SASH_ABSORPTION_ENABLE__
#include "SashSystemHelper.hpp"
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
#include "AmuletSystemHelper.hpp"
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
#include "ShamanSystem.hpp"
#endif

#ifdef __POPUP_SYSTEM_ENABLE__
	#include "PopupManager.hpp"
#endif

#include <variant>
#include "config.h"

bool CanModifyItem(const CItem* item)
{
	if (item->IsExchanging())
	{
		return false;
	}

	if (item->isLocked())
	{
		return false;
	}

	return true;
}

bool CanStack(const CItem* from, const CItem* to)
{
	if (from == to)
	{
		return false;
	}

	if (from->GetVnum() != to->GetVnum())
	{
		return false;
	}

	if (!to->IsStackable())
	{
		return false;
	}

	for (auto i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
		if (from->GetSocket(i) != to->GetSocket(i))
		{
			return false;
		}

	return true;
}

CountType GetItemMaxCount(const CItem* item)
{
	switch (item->GetType()) {

	default:
		break;
	}

	return g_bItemCountLimit;
}

CountType GetItemMaxCount(const uint32_t vnum)
{
	const auto p = ITEM_MANAGER::instance().GetTable(vnum);
	if (!p)
		return g_bItemCountLimit;

	switch (p->bType) {

	default:
		break;
	}

	return g_bItemCountLimit;
}

bool ActivateToggleItem(CHARACTER* ch, CItem* item, bool bIsLoad)
{
	// Checks before activation:
	switch (item->GetSubType())
	{
	case TOGGLE_AUTO_RECOVERY_HP:
	case TOGGLE_AUTO_RECOVERY_SP:
	{
		if (ch->GetArena())
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can not use it in the arena."));
			return false;
		}
		break;
	}

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	case TOGGLE_MOUNT:
	{
		if (bIsLoad)
		{
			ch->SetMountStartToggleItem(item);
			return false;
		}
	}
	break;
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
	case TOGGLE_RED_SOUL:
	case TOGGLE_BLUE_SOUL:
	{
		int iCurrentMinutes = (item->GetSocket(2) / 10000);
		int iCurrentStrike = (item->GetSocket(2) % 10000);

		if (iCurrentMinutes < 60)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This soul is not enought charged."));
			return false;
		}

		if (iCurrentStrike <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This soul have no more strikes."));
			return false;
		}
	}
	break;
#endif
	}

	if (item->GetCount() > 1)
	{
		int pos = ch->GetEmptyInventory(item->GetSize());
		if (-1 == pos)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Not enough space in your inventory"));
			return false;
		}

		item->SetCount(item->GetCount() - 1);

		const auto item2 = ITEM_MANAGER::instance().CreateItem(item->GetVnum());
		item2->AddToCharacter(ch, TItemPos(INVENTORY, pos));

		item = item2;
	}

	item->SetSocket(ITEM_SOCKET_TOGGLE_ACTIVE, true);

	item->Lock(true);

	if (item->FindLimit(LIMIT_TIMER_BASED_ON_WEAR))
	{
		item->StartTimerBasedOnWearExpireEvent();
	}

	switch (item->GetSubType())
	{
	case TOGGLE_AUTO_RECOVERY_HP:
	case TOGGLE_AUTO_RECOVERY_SP:
	{
		item->ModifyPoints(true);
		ch->StartAffectEvent();
		break;
	}

	case TOGGLE_AFFECT:
	{
		item->ModifyPoints(true);
	}
	break;

#ifdef __PET_SYSTEM__
	case TOGGLE_PET:
	{
		auto vnum = item->GetValue(ITEM_VALUE_PET_VNUM);

		if (!vnum)
			return false;

		const auto iOverlay = ch->GetWear(WEAR_COSTUME_PET);
		if (iOverlay && iOverlay->GetValue(ITEM_VALUE_PET_VNUM))
			vnum = iOverlay->GetValue(ITEM_VALUE_PET_VNUM);

		ch->GetPetSystem()->SummonItem(vnum, item->GetID(), false);
		break;
	}
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	case TOGGLE_MOUNT:
	{
		ch->CreateToggleMount(item);
		if (item->GetSocket(ITEM_SOCKET_TOGGLE_RIDING))
		{
			Mount(ch);
		}
		break;
	}
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
	case TOGGLE_RED_SOUL:
	case TOGGLE_BLUE_SOUL:
	{
		int iAffectID = AFFECT_SOUL_RED + (item->GetSubType() - TOGGLE_RED_SOUL);
		int iAffID = AFF_SOUL_RED + (item->GetSubType() - TOGGLE_RED_SOUL);

		const CAffect* pAffect = ch->FindAffect(iAffectID);

		if (!pAffect)
		{
			ch->AddAffect(iAffectID, APPLY_NONE, 0, iAffID, INFINITE_AFFECT_DURATION, item->GetID(), true, false);
		}
	}
	break;
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	case TOGGLE_SHAMAN:
	{
		if (ch->GetShamanSystem())
			ch->GetShamanSystem()->SummonItem(item->GetID(), false);

		item->ModifyPoints(true);
		break;
	}
#endif
	}

	ch->CheckMaximumPoints();
	ch->UpdatePacket();

	return true;
}

void DeactivateToggleItem(CHARACTER* ch, CItem* item)
{
	item->SetSocket(ITEM_SOCKET_TOGGLE_ACTIVE, false);
	item->SetSocket(ITEM_SOCKET_TOGGLE_RIDING, 0);

	switch (item->GetSubType())
	{
	case TOGGLE_AUTO_RECOVERY_HP:
	case TOGGLE_AUTO_RECOVERY_SP:
	{
		item->ModifyPoints(false);
		break;
	}

	case TOGGLE_AFFECT:
	{
		item->ModifyPoints(false);
	}
	break;

#ifdef __PET_SYSTEM__
	case TOGGLE_PET:
	{
		ch->GetPetSystem()->UnsummonItem(item->GetID());
		break;
	}
#endif

#ifdef __ENABLE_ALTERNATIVE_MOUNT_SYSTEM__
	case TOGGLE_MOUNT:
	{
		ch->MountVnum(0);
		ch->DestroyToggleMount();
		break;
	}
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
	case TOGGLE_RED_SOUL:
	case TOGGLE_BLUE_SOUL:
	{
		int iAffectID = AFFECT_SOUL_RED + (item->GetSubType() - TOGGLE_RED_SOUL);

		const CAffect* pAffect = ch->FindAffect(iAffectID);

		if (pAffect)
		{
			ch->RemoveAffect(const_cast<CAffect*>(pAffect));
		}
	}
	break;
#endif

#ifdef __ENABLE_SHAMAN_SYSTEM__
	case TOGGLE_SHAMAN:
	{
		if (ch->GetShamanSystem())
			ch->GetShamanSystem()->UnsummonItem(item->GetID());

		ch->ChatPacket(CHAT_TYPE_COMMAND, "ShamanSystem_UnregisterShaman");
		item->ModifyPoints(false);
		break;
	}
#endif
	}

	if (item->FindLimit(LIMIT_TIMER_BASED_ON_WEAR))
	{
		item->StopTimerBasedOnWearExpireEvent();
	}

	item->Lock(false);

	ch->CheckMaximumPoints();
	ch->UpdatePacket();
}

void OnCreateToggleItem(CItem* item)
{
	switch (item->GetSubType())
	{
	case TOGGLE_AUTO_RECOVERY_HP:
	case TOGGLE_AUTO_RECOVERY_SP:
		item->SetSocket(ITEM_SOCKET_AUTORECOVERY_FULL,
			item->GetValue(ITEM_VALUE_AUTORECOVERY_AMOUNT),
			false);
		break;
	}
}

void OnLoadToggleItem(CHARACTER* ch, CItem* item)
{
	// Don't do anything if we're not active.
	if (!item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
	{
		return;
	}

	// If our toggle item has an unique group, check if we already activated
	// another item of the same group.
	const auto group = item->GetValue(ITEM_VALUE_TOGGLE_GROUP);
	if (-1 != group && FindToggleItem(ch, true, item->GetSubType(), group, item))
	{
		item->SetSocket(ITEM_SOCKET_TOGGLE_ACTIVE, false);
		return;
	}

	ActivateToggleItem(ch, item, true);
}

void OnRemoveToggleItem(CHARACTER* ch, CItem* item)
{
	// Don't do anything if we're not active.
	if (!item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
	{
		return;
	}

	DeactivateToggleItem(ch, item);
}

bool OnUseToggleItem(CHARACTER* ch, CItem* item)
{
	if (item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE))
	{
		DeactivateToggleItem(ch, item);
		return true;
	}

	// If our toggle item has an unique group, check if we already activated
	// another item of the same group.
	const auto group = item->GetValue(ITEM_VALUE_TOGGLE_GROUP);
	if (-1 != group && FindToggleItem(ch, true, item->GetSubType(), group))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot activate two items of this kind."));
		return false;
	}

	return ActivateToggleItem(ch, item);
}

void OnCreateItem(CItem* item, bool bGameMasterCreated)
{
	switch (item->GetType())
	{
#ifdef __LEGENDARY_STONES_ENABLE__
	case ITEM_METIN:
	{
		// We gonna do this just in case if we are using the command to createItem, and just for legendary stones!
		if (bGameMasterCreated && LegendaryStonesHandler::IsLegendaryStone(item->GetVnum()))
		{
			if (item->GetSocket(0) == 0)
				item->SetSocket(0, Random::get<long>(item->GetValue(3), item->GetValue(4)));
		}
	}
	break;
#endif

	case ITEM_TOGGLE:
		OnCreateToggleItem(item);
		break;

#ifdef __SASH_ABSORPTION_ENABLE__
	case ITEM_COSTUME:
	{
		if (item->GetSubType() == COSTUME_SASH)
		{
			SashSystemHelper::CreateSash(item);
		}
	}
	break;
#endif

#ifdef __ENABLE_AMULET_SYSTEM__
	case ITEM_AMULET:
	{
		AmuletSystemHelper::OnCreate(item);
	}
	break;
#endif
	}
}

void OnLoadItem(CHARACTER* ch, CItem* item)
{
	switch (item->GetType())
	{
	case ITEM_TOGGLE:
		OnLoadToggleItem(ch, item);
		break;
	}
}

void OnRemoveItem(CHARACTER* ch, CItem* item)
{
	switch (item->GetType())
	{
	case ITEM_TOGGLE:
		OnRemoveToggleItem(ch, item);
		break;
	}
}

bool OnUseItem(CHARACTER* ch, CItem* item)
{
	switch (item->GetType())
	{
	case ITEM_TOGGLE:
		return OnUseToggleItem(ch, item);
	}

	return true;
}

void ProcessAutoRecoveryItem(CHARACTER* ch, CItem* item)
{
	if (ch->IsDead() || ch->IsStun())
	{
		return;
	}

	static const uint32_t kStunSkills[] = { AFFECT_STUN, SKILL_TANHWAN, SKILL_GEOMPUNG, SKILL_BYEURAK, SKILL_GIGUNG };
	for (auto& skill : kStunSkills)
		if (ch->FindAffect(skill))
		{
			return;
		}

	if (ch->GetArena())
	{
		DeactivateToggleItem(ch, item);
		return;
	}

	bool isPerma = item->GetValue(4) == 1;

	const int32_t amountUsed = item->GetSocket(ITEM_SOCKET_AUTORECOVERY_USED);
	const int32_t amountFull = item->GetSocket(ITEM_SOCKET_AUTORECOVERY_FULL);
	const int32_t avail = amountFull - amountUsed;

	int32_t amount = 0;

	if (TOGGLE_AUTO_RECOVERY_HP == item->GetSubType())
	{
		amount = ch->GetMaxHP() - (ch->GetHP() + ch->GetPoint(POINT_HP_RECOVERY));
	}
	else if (TOGGLE_AUTO_RECOVERY_SP == item->GetSubType())
	{
		amount = ch->GetMaxSP() - (ch->GetSP() + ch->GetPoint(POINT_SP_RECOVERY));
	}

	if (amount <= 0)
	{
		return;
	}

	if (!isPerma)
	{
		if (avail > amount)
		{
			const int pct_of_used = amountUsed * 100 / amountFull;
			const int pct_of_will_used = (amountUsed + amount) * 100 / amountFull;

			bool log = false;
			if ((pct_of_will_used / 10) - (pct_of_used / 10) >= 1)
			{
				log = true;
			}

			item->SetSocket(ITEM_SOCKET_AUTORECOVERY_USED,
				amountUsed + amount, log);
		}
		else
		{
			amount = avail;
		}
	}

	if (TOGGLE_AUTO_RECOVERY_HP == item->GetSubType())
	{
		ch->PointChange(POINT_HP_RECOVERY, amount);
		ch->EffectPacket(SE_AUTO_HPUP);
	}
	else if (TOGGLE_AUTO_RECOVERY_SP == item->GetSubType())
	{
		ch->PointChange(POINT_SP_RECOVERY, amount);
		ch->EffectPacket(SE_AUTO_SPUP);
	}

	if (amount == avail && !isPerma)
	{
		// RemoveItem() will deactivate |item|
		ITEM_MANAGER::instance().RemoveItem(item);
	}
}

CItem* FindToggleItem(CHARACTER* ch, bool active,
	int32_t subType, int32_t group, CItem* except)
{
	if (ch == NULL)
	{
		return nullptr;
	}

	for (int i = 0; i < INVENTORY_MAX_NUM; ++i)
	{
		const auto item = ch->GetInventoryItem(i);
		if (!item || item == except || item->GetType() != ITEM_TOGGLE)
		{
			continue;
		}

		if (subType != -1 && item->GetSubType() != subType)
		{
			continue;
		}

		if (active != static_cast<bool>(item->GetSocket(ITEM_SOCKET_TOGGLE_ACTIVE)))
		{
			continue;
		}

		if (group != -1 &&
			group != item->GetValue(ITEM_VALUE_TOGGLE_GROUP))
		{
			continue;
		}

		return item;
	}

	return nullptr;
}
#endif
