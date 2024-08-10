#include "stdafx.h"
#include "CharUtil.hpp"
#include "char.h"
#include "item.h"
#include "locale.h"
#include "desc.h"

#include "packet.h"
#include "ItemUtils.h"


bool Unmount(CHARACTER* ch)
{
	if (!ch->IsPC()) return false;
	const auto mountItem = FindToggleItem(ch, true, TOGGLE_MOUNT);
	if (mountItem)
	{
		mountItem->SetSocket(ITEM_SOCKET_TOGGLE_RIDING, 0, false);
		ch->MountVnum(0);
		ch->CreateToggleMount(mountItem);
		//mountItem->ModifyPoints(false);

		return true;
	}

	if (ch->IsHorseRiding())
	{
		ch->StopRiding();
		return true;
	}

	return false;
}

bool Mount(CHARACTER* ch)
{
	if (ch->GetHorse())
	{
		ch->StartRiding();
		return true;
	}

	LPITEM armor = ch->GetWear(WEAR_BODY);
	if (armor && (armor->GetVnum() >= 11901 && armor->GetVnum() <= 11904))
	{
		return false;
	}

	auto *item = FindToggleItem(ch, true, TOGGLE_MOUNT);
	if (!!item)
	{
		ch->DestroyToggleMount();
		item->SetSocket(ITEM_SOCKET_TOGGLE_RIDING, 1, false);
		
		DWORD dwMountVnum = item->GetValue(ITEM_VALUE_MOUNT_VNUM);
		
		// Overlay!
		ch->GetSpecialMount(dwMountVnum);

		ch->MountVnum(dwMountVnum);
		
		item->OnRealTimeFirstUse();
		return true;
	}


	auto *newMountItem = FindToggleItem(ch, false, TOGGLE_MOUNT);
	if (newMountItem)
	{
		ActivateToggleItem(ch, newMountItem);
		ch->DestroyToggleMount();
		newMountItem->SetSocket(ITEM_SOCKET_TOGGLE_RIDING, 1, false);
		
		DWORD dwMountVnum = newMountItem->GetValue(ITEM_VALUE_MOUNT_VNUM);

		//// Overlay!
		ch->GetSpecialMount(dwMountVnum);

		ch->MountVnum(dwMountVnum);
		
		newMountItem->OnRealTimeFirstUse();

		return true;
	}

	return false;
}
