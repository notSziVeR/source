#include "stdafx.h"
#ifdef __TRANSMUTATION_SYSTEM__
#include "constants.h"
#include "Transmutation.h"
#include "item.h"
#include "item_manager.h"

namespace Transmutation
{
bool CheckPair(TransMutation_Struct * trans_item_point, TransMutation_Struct * base_item_point)
{
	if (trans_item_point->item_id == base_item_point->item_id)
	{
		return false;
	}

	LPITEM trans_item = ITEM_MANAGER::instance().Find(trans_item_point->item_id);
	if (!trans_item)
	{
		return false;
	}

	LPITEM base_item = ITEM_MANAGER::instance().Find(base_item_point->item_id);
	if (!base_item)
	{
		return false;
	}

	if (base_item->GetCell() != base_item_point->pos || trans_item->GetCell() != trans_item_point->pos)
	{
		return false;
	}

	if (trans_item->GetVnum() == base_item->GetVnum())
	{
		return false;
	}

	if (trans_item->GetType() != base_item->GetType())
	{
		return false;
	}

	if (trans_item->GetSubType() != base_item->GetSubType())
	{
		return false;
	}

	if (trans_item->GetAntiFlag() != base_item->GetAntiFlag())
	{
		return false;
	}

	switch (base_item->GetType())
	{
	case ITEM_WEAPON:
		if (base_item->GetSubType() == WEAPON_ARROW)
		{
			return false;
		}
		else
		{
			break;
		}
	case ITEM_ARMOR:
		if (base_item->GetSubType() == ARMOR_BODY)
		{
			break;
		}
		else
		{
			return false;
		}
	default:
		return false;
	}

	return true;
}

bool CheckItem(LPITEM trans_item, BYTE gSlot)
{
	if (gSlot == 0)
	{
		if (trans_item->GetTransmutate())
		{
			return false;
		}
	}

	switch (trans_item->GetType())
	{
	case ITEM_WEAPON:
		if (trans_item->GetSubType() == WEAPON_ARROW)
		{
			return false;
		}
		else
		{
			break;
		}
	case ITEM_ARMOR:
		if (trans_item->GetSubType() == ARMOR_BODY)
		{
			break;
		}
		else
		{
			return false;
		}
	default:
		return false;
	}

	return true;
}

void DoTransmutation(TransMutation_Struct * trans_item_point, TransMutation_Struct * base_item_point)
{
	if (trans_item_point->item_id == base_item_point->item_id)
	{
		return;
	}

	LPITEM trans_item = ITEM_MANAGER::instance().Find(trans_item_point->item_id);
	if (!trans_item)
	{
		return;
	}

	LPITEM base_item = ITEM_MANAGER::instance().Find(base_item_point->item_id);
	if (!base_item)
	{
		return;
	}

	if (base_item->GetCell() == base_item_point->pos)
	{
		DWORD item_vnum = base_item->GetVnum();
		// Deleting Base Item
		base_item->SetCount(0);
		trans_item->SetTransmutate(item_vnum);
		trans_item->Lock(false);
	}
}

void RemoveTransmutation(LPITEM item_use, LPITEM item_trans)
{
	if (!item_use || !item_trans)
	{
		return;
	}

	if (!item_trans->GetTransmutate())
	{
		return;
	}

	item_trans->SetTransmutate(0);
	item_use->SetCount(item_use->GetCount() - 1);
	return;
}
}
#endif

