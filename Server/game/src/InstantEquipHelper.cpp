#include "stdafx.h"
#include "constants.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "sectree_manager.h"
#include "unique_item.h"
#include "utils.h"
#include "packet.h"
#include "log.h"
#include "quest.h"
#include "db.h"
#include "config.h"
#include "cmd.h"
#include "p2p.h"
#include "buffer_manager.h"
#include "questmanager.h"
#include "InstantEquipHelper.hpp"

#ifdef __ENABLE_INSTANT_EQUIP__
namespace InstantEquipHelper
{
void __instantEquip(LPCHARACTER dwChar, DWORD dwVnum)
{
	if (!dwVnum || !dwChar)
	{
		return;
	}

	LPITEM dwItem;

	dwItem = ITEM_MANAGER::instance().CreateItem(dwVnum);

	if (!dwItem || !dwItem->EquipTo(dwChar, dwItem->FindEquipCell(dwChar)))
	{
		M2_DESTROY_ITEM(dwItem);
	}

	return;
}
}
#endif
