#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "utils.h"
#include "desc.h"
#include "desc_client.h"
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "locale_service.h"
#include "../../common/length.h"
#include "exchange.h"
#include "DragonSoul.h"
#include "questmanager.h" // @fixme150

#include "buffer_manager.h"

#include <ctime>
#include "config.h"
#ifdef __ADMIN_MANAGER__
	#include "gm.h"
#endif

#ifdef __ENABLE_NEW_LOGS_CHAT__
	#include "ChatLogsHelper.hpp"
#endif

#ifdef __EXTANDED_GOLD_AMOUNT__
	void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, int64_t arg1, TItemPos arg2, CountType arg3, void* pvData = NULL);

	void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, int64_t arg1, TItemPos arg2, CountType arg3, void* pvData)
#else
	void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, CountType arg3, void* pvData = NULL);

	void exchange_packet(LPCHARACTER ch, BYTE sub_header, bool is_me, DWORD arg1, TItemPos arg2, CountType arg3, void* pvData)
#endif
{
	if (!ch->GetDesc())
	{
		return;
	}

	struct packet_exchange pack_exchg;

	pack_exchg.header = HEADER_GC_EXCHANGE;
	pack_exchg.sub_header = sub_header;
	pack_exchg.is_me = is_me;
	pack_exchg.arg1 = arg1;
	pack_exchg.arg2 = arg2;
	pack_exchg.arg3 = arg3;

	if (sub_header == EXCHANGE_SUBHEADER_GC_ITEM_ADD && pvData)
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(((LPITEM)pvData)->GetWindow(), ((LPITEM)pvData)->GetCell());
#endif
		thecore_memcpy(&pack_exchg.alSockets, ((LPITEM)pvData)->GetSockets(), sizeof(pack_exchg.alSockets));
		thecore_memcpy(&pack_exchg.aAttr, ((LPITEM)pvData)->GetAttributes(), sizeof(pack_exchg.aAttr));

#ifdef __TRANSMUTATION_SYSTEM__
		pack_exchg.transmutate_id = ((LPITEM)pvData)->GetTransmutate();
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		pack_exchg.dwRefineElement = ((LPITEM)pvData)->GetRefineElement();
#endif
	}
	else
	{
#ifdef WJ_ENABLE_TRADABLE_ICON
		pack_exchg.arg4 = TItemPos(RESERVED_WINDOW, 0);
#endif
		memset(&pack_exchg.alSockets, 0, sizeof(pack_exchg.alSockets));
		memset(&pack_exchg.aAttr, 0, sizeof(pack_exchg.aAttr));

#ifdef __TRANSMUTATION_SYSTEM__
		pack_exchg.transmutate_id = 0;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		pack_exchg.dwRefineElement = 0;
#endif
	}

	ch->GetDesc()->Packet(&pack_exchg, sizeof(pack_exchg));
}


bool CHARACTER::ExchangeStart(LPCHARACTER victim)
{
	if (this == victim)
	{
		return false;
	}

	if (IsObserverMode())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade while observing."));
		return false;
	}

#ifdef __ADMIN_MANAGER__
	if ((!GM::check_allow(GetGMLevel(), GM_ALLOW_EXCHANGE_TO_GM) && victim->IsGM()) ||
			(!GM::check_allow(GetGMLevel(), GM_ALLOW_EXCHANGE_TO_PLAYER) && !victim->IsGM()))
	{
		if (!GM::check_allow(GetGMLevel(), GM_ALLOW_EXCHANGE_TO_GM) && victim->IsGM())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not allowed to trade to gamemasters."));
		}
		else if (!GM::check_allow(GetGMLevel(), GM_ALLOW_EXCHANGE_TO_PLAYER) && !victim->IsGM())
		{
			ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are not allowed to trade to players."));
		}
		return false;
	}
#endif

	if (victim->IsNPC())
	{
		sys_log(1, "Cannot exchange because of polymorph propably");
		return false;
	}

	//PREVENT_TRADE_WINDOW
	if (IsOpenSafebox() || GetShopOwner() || GetMyShop() || IsCubeOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("CLOSE_OTHER_WINDOW_BEFORE_OPEN"));
		return false;
	}

	if (victim->IsOpenSafebox() || victim->GetShopOwner() || victim->GetMyShop() || victim->IsCubeOpen())
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person is already trading, so you cannot trade right now."));
		return false;
	}

	//END_PREVENT_TRADE_WINDOW
	int iDist = DISTANCE_APPROX(GetX() - victim->GetX(), GetY() - victim->GetY());


	if (iDist >= EXCHANGE_MAX_DISTANCE)
	{
		return false;
	}

	if (GetExchange())
	{
		return false;
	}

	if (victim->GetExchange())
	{
		exchange_packet(this, EXCHANGE_SUBHEADER_GC_ALREADY, 0, 0, NPOS, 0);
		return false;
	}

	if (victim->IsBlockMode(BLOCK_EXCHANGE))
	{
		ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The other person has canceled the trade."));
		return false;
	}

	SetExchange(M2_NEW CExchange(this));
	victim->SetExchange(M2_NEW CExchange(victim));

	victim->GetExchange()->SetCompany(GetExchange());
	GetExchange()->SetCompany(victim->GetExchange());

	SetExchangeTime();
	victim->SetExchangeTime();

	exchange_packet(victim, EXCHANGE_SUBHEADER_GC_START, 0, GetVID(), NPOS, 0);
	exchange_packet(this, EXCHANGE_SUBHEADER_GC_START, 0, victim->GetVID(), NPOS, 0);

	return true;
}

CExchange::CExchange(LPCHARACTER pOwner)
{
	m_pCompany = NULL;

	m_bAccept = false;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		m_apItems[i] = NULL;
		m_aItemPos[i] = NPOS;
		m_abItemDisplayPos[i] = 0;
	}

	m_lGold = 0;
	m_lLastCriticalUpdatePulse = 0;

	m_pOwner = pOwner;
	pOwner->SetExchange(this);

#ifdef __ENABLE_RENEWAL_EXCHANGE__
	m_pGrid = M2_NEW CGrid(6, 4);
#else
	m_pGrid = M2_NEW CGrid(4, 4);
#endif
}

CExchange::~CExchange()
{
	M2_DELETE(m_pGrid);
}

bool CExchange::AddItem(TItemPos item_pos, BYTE display_pos)
{
	assert(m_pOwner != NULL && GetCompany());

	if (item_pos.IsEquipPosition())
	{
		return false;
	}

	LPITEM item;

	if (!(item = m_pOwner->GetItem(item_pos)))
	{
		return false;
	}

	// Exchange item relocation
	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		display_pos = GetEmptySlot(item->GetSize());
	}

	if (!item_pos.IsValidItemPosition())
	{
		return false;
	}

#ifdef __ADMIN_MANAGER__
	LPCHARACTER pkVictim = GetCompany()->GetOwner();
	if (!pkVictim)
	{
		return false;
	}

	if (item->IsGMOwner())
	{
		if (pkVictim->IsGM() && !GM::check_allow(m_pOwner->GetGMLevel(), GM_ALLOW_EXCHANGE_GM_ITEM_TO_GM))
		{
			m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You may not exchange a gm owned item to a gm."));
			return false;
		}
		else if (!pkVictim->IsGM() && !GM::check_allow(m_pOwner->GetGMLevel(), GM_ALLOW_EXCHANGE_GM_ITEM_TO_PLAYER))
		{
			m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You may not exchange a gm owned item to a player."));
			return false;
		}
	}
	else
	{
		if (pkVictim->IsGM() && !GM::check_allow(m_pOwner->GetGMLevel(), GM_ALLOW_EXCHANGE_PLAYER_ITEM_TO_GM))
		{
			m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You may not exchange a player owned item to a gm."));
			return false;
		}
		else if (!pkVictim->IsGM() && !GM::check_allow(m_pOwner->GetGMLevel(), GM_ALLOW_EXCHANGE_PLAYER_ITEM_TO_PLAYER))
		{
			m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You may not exchange a player owned item to a player."));
			return false;
		}
	}
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
		//if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE) && !GM::check_allow(m_pOwner->GetGMLevel(), GM_ALLOW_EXCHANGE_ANTIGIVE) && !GM::check_allow(pkVictim->GetGMLevel(), GM_ALLOW_EXCHANGE_ANTIGIVE))
#else
	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_GIVE))
#endif
	{
		m_pOwner->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade this item."));
		return false;
	}

	if (true == item->isLocked())
	{
		return false;
	}

	if (item->IsExchanging())
	{
		sys_log(0, "EXCHANGE under exchanging");
		return false;
	}

	if (!m_pGrid->IsEmpty(display_pos, 1, item->GetSize()))
	{
		sys_log(0, "EXCHANGE not empty item_pos %d %d %d", display_pos, 1, item->GetSize());
		return false;
	}

	if (m_pOwner->GetQuestItemPtr() == item)
	{
		sys_log(0, "EXCHANGE %s trying to cheat by using a current quest item in trade", m_pOwner->GetName());
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
		{
			continue;
		}

		m_apItems[i] = item;
		m_aItemPos[i] = item_pos;
		m_abItemDisplayPos[i] = display_pos;
		m_pGrid->Put(display_pos, 1, item->GetSize());

		item->SetExchanging(true);

		exchange_packet(m_pOwner,
						EXCHANGE_SUBHEADER_GC_ITEM_ADD,
						true,
						item->GetVnum(),
						TItemPos(RESERVED_WINDOW, display_pos),
						item->GetCount(),
						item);

		exchange_packet(GetCompany()->GetOwner(),
						EXCHANGE_SUBHEADER_GC_ITEM_ADD,
						false,
						item->GetVnum(),
						TItemPos(RESERVED_WINDOW, display_pos),
						item->GetCount(),
						item);

#ifdef __ENABLE_RENEWAL_EXCHANGE__
		std::string link = item->GetHyperlink();

		if (item->GetCount() > 1)
		{
			SendInfo(false, LC_TEXT("Dodano %d x %s do handlu"), item->GetCount(), link.c_str());
			GetCompany()->SendInfo(false, LC_TEXT("%s dodano %d %s do handlu"), GetOwner()->GetName(), item->GetCount(), link.c_str());
		}
		else
		{
			SendInfo(false, LC_TEXT("You added %s"), link.c_str());
			GetCompany()->SendInfo(false, LC_TEXT("%s added %s"), GetOwner()->GetName(), link.c_str());
		}
#endif

		sys_log(0, "EXCHANGE AddItem success %s pos(%d, %d) %d", item->GetName(), item_pos.window_type, item_pos.cell, display_pos);

		return true;
	}

	return false;
}

bool CExchange::RemoveItem(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return false;
	}

	int k = -1;
	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_abItemDisplayPos[i] == pos)
		{
			k = i;
			break;
		}
	}

	if (k == -1) //Not found
	{
		return false;
	}

	if (!m_apItems[k])
	{
		sys_err("No item on position %d when trying to remove it!", k);
		return false;
	}

	TItemPos PosOfInventory = m_aItemPos[k];
	m_apItems[k]->SetExchanging(false);

	m_pGrid->Get(m_abItemDisplayPos[k], 1, m_apItems[k]->GetSize());

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, true, pos, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ITEM_DEL, false, pos, PosOfInventory, 0);

	m_lLastCriticalUpdatePulse = thecore_pulse();
	Accept(false);
	GetCompany()->Accept(false);

	std::string link = m_apItems[k]->GetHyperlink();

	if (m_apItems[k]->GetCount() > 1)
	{
		SendInfo(false, LC_TEXT("You have removed %d x %s from trading"), m_apItems[k]->GetCount(), link.c_str());
		GetCompany()->SendInfo(false, LC_TEXT("%s has removed %d x %s from trading"), GetOwner()->GetName(), m_apItems[k]->GetCount(), link.c_str());
	}
	else
	{
		SendInfo(false, LC_TEXT("You have removed %s"), link.c_str());
		GetCompany()->SendInfo(false, LC_TEXT("%s has removed %s from trading"), GetOwner()->GetName(), link.c_str());
	}

	m_apItems[pos] = NULL;
	m_aItemPos[pos] = NPOS;
	m_abItemDisplayPos[pos] = 0;
	return true;
}

#ifdef __EXTANDED_GOLD_AMOUNT__
	bool CExchange::AddGold(int64_t gold)
#else
	bool CExchange::AddGold(long gold)
#endif
{
	if (gold <= 0)
	{
		return false;
	}

	if (GetOwner()->GetGold() < gold)
	{
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_LESS_GOLD, 0, 0, NPOS, 0);
		return false;
	}

	if (m_lGold == gold) //Nothing changed.
	{
		return false;
	}

#ifdef __EXTANDED_GOLD_AMOUNT__
	if (gold > GOLD_MAX) //Over max gold allowed
#else
	if (gold > GOLD_MAX) //Over max gold allowed
#endif
	{
		sys_err("EXCHANGE %s trying to set %llu Yang in trade (over max)", GetOwner()->GetName(), gold);
		return false;
	}

	Accept(false);
	GetCompany()->Accept(false);

#ifdef __ENABLE_RENEWAL_EXCHANGE__
	std::string prettyGold = pretty_number(gold);

	if (m_lGold < gold)   //Before < new value: increased
	{
		SendInfo(false, LC_TEXT("You have increased the |Eemoji/money|e in trade to %s"), prettyGold.c_str());
		GetCompany()->SendInfo(false, LC_TEXT("%s has increased the |Eemoji/money|e in trade to %s"), GetOwner()->GetName(), prettyGold.c_str());
	}
	else
	{
		m_lLastCriticalUpdatePulse = thecore_pulse();
		SendInfo(false, LC_TEXT("You changed the |Eemoji/money|e in trade to %s"), prettyGold.c_str());
		GetCompany()->SendInfo(false, LC_TEXT("%s has changed the |Eemoji/money|e in trade to %s"), GetOwner()->GetName(), prettyGold.c_str());
	}
#endif

	m_lGold = gold;

	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, true, m_lGold, NPOS, 0);
	exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_GOLD_ADD, false, m_lGold, NPOS, 0);
	return true;
}

bool CExchange::Check(int* piItemCount)
{
	if (GetOwner()->GetGold() < m_lGold)
	{
		return false;
	}

	int item_count = 0;

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
		{
			continue;
		}

		if (!m_aItemPos[i].IsValidItemPosition())
		{
			return false;
		}

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
		{
			return false;
		}

		++item_count;
	}

	*piItemCount = item_count;
	return true;
}

bool CExchange::SanityCheck()
{
	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
		{
			continue;
		}

		if (!m_aItemPos[i].IsValidItemPosition())
		{
			return false;
		}

		if (m_apItems[i] != GetOwner()->GetItem(m_aItemPos[i]))
		{
			return false;
		}
	}

	return true;
}

bool CExchange::CheckSpace()
{
	LPCHARACTER	victim = GetCompany()->GetOwner();

	std::unique_ptr<CGrid> pGrids[INVENTORY_PAGE_COUNT];
	for (int i = 0; i < INVENTORY_PAGE_COUNT; ++i)
	{
		pGrids[i].reset(new CGrid(5, INVENTORY_PAGE_SIZE / 5));
		pGrids[i]->Clear();
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	std::unique_ptr<CGrid> pGridsSpecialInventory[SPECIAL_STORAGE_END_WINDOW - SPECIAL_STORAGE_START_WINDOW][SPECIAL_STORAGE_PAGE_COUNT];

	for (int j = 0; j < SPECIAL_STORAGE_END_WINDOW - SPECIAL_STORAGE_START_WINDOW; ++j)
	{
		for (int i = 0; i < SPECIAL_STORAGE_PAGE_COUNT; ++i)
		{
			pGridsSpecialInventory[j][i].reset(new CGrid(SPECIAL_STORAGE_PAGE_LENGTH, SPECIAL_STORAGE_PAGE_BREADTH));
			pGridsSpecialInventory[j][i]->Clear();
		}
	}
#endif

	LPITEM item;

	for (int iPage = 0; iPage < INVENTORY_PAGE_COUNT; ++iPage)
	{
		int iAdd = iPage * INVENTORY_PAGE_SIZE;

		for (int i = 0; i < INVENTORY_PAGE_SIZE; ++i)
		{
			if (!(item = victim->GetInventoryItem(iAdd + i)))
			{
				continue;
			}

			pGrids[iPage]->Put(i, 1, item->GetSize());
		}
	}

#ifdef __SPECIAL_STORAGE_ENABLE__
	for (int iSpecialPage = 0; iSpecialPage < SPECIAL_STORAGE_END_WINDOW - SPECIAL_STORAGE_START_WINDOW; ++iSpecialPage)
	{
		for (int iPage = 0; iPage < SPECIAL_STORAGE_PAGE_COUNT; ++iPage)
		{
			int iAdd = (BELT_INVENTORY_SLOT_END + (SPECIAL_STORAGE_PAGE_SIZE * SPECIAL_STORAGE_PAGE_COUNT) * iSpecialPage) + SPECIAL_STORAGE_PAGE_SIZE * iPage;
			for (int i = 0; i < SPECIAL_STORAGE_PAGE_SIZE; ++i)
			{
				if (!(item = victim->GetInventoryItem(iAdd + i)))
				{
					continue;
				}

				pGridsSpecialInventory[iSpecialPage][iPage]->Put(i, 1, item->GetSize());
			}
		}
	}
#endif

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
		{
			continue;
		}

		bool bFree = false;

#ifdef __SPECIAL_STORAGE_ENABLE__
		if (item->GetVirtualWindow() != INVENTORY)
		{
			int iGridType = item->GetVirtualWindow() - SPECIAL_STORAGE_START_WINDOW;
			for (int iPage = 0; iPage < SPECIAL_STORAGE_PAGE_SIZE; ++iPage)
			{
				int iPos = pGridsSpecialInventory[iGridType][iPage]->FindBlank(1, item->GetSize());

				if (iPos >= 0)
				{
					pGridsSpecialInventory[iGridType][iPage]->Put(iPos, 1, item->GetSize());
					bFree = true;
					break;
				}
			}
		}

		// If still slot is not free
		if (!bFree)
		{
			for (int iPage = 0; iPage < INVENTORY_PAGE_COUNT; ++iPage)
			{
				int iPos = pGrids[iPage]->FindBlank(1, item->GetSize());

				if (iPos >= 0)
				{
					pGrids[iPage]->Put(iPos, 1, item->GetSize());
					bFree = true;
					break;
				}
			}
		}
#else
		for (int iPage = 0; iPage < INVENTORY_PAGE_COUNT; ++iPage)
		{
			int iPos = pGrids[iPage]->FindBlank(1, item->GetSize());

			if (iPos >= 0)
			{
				pGrids[iPage]->Put(iPos, 1, item->GetSize());
				bFree = true;
				break;
			}
		}
#endif

		if (!bFree)
		{
			return false;
		}
	}

	return true;
}

bool CExchange::Done(DWORD tradeID, bool firstPlayer)
{
	int		i;
	LPITEM	item;

	LPCHARACTER	victim = GetCompany()->GetOwner();

	for (i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!(item = m_apItems[i]))
		{
			continue;
		}

		m_pOwner->SyncQuickslot(QUICKSLOT_TYPE_ITEM, item->GetCell(), 255);
		item->RemoveFromCharacter();

#ifdef __ENABLE_NEW_LOGS_CHAT__
		CChatLogs::SendChatLogInformation(victim, ELogsType::LOG_TYPE_EXCHANGE, static_cast<int>(item->GetCount()), static_cast<int>(item->GetVnum()));
#endif

		LogManager::instance().ExchangeItemLog(tradeID, item, firstPlayer ? "A" : "B");

		if (item->GetVnum() >= 80003 && item->GetVnum() <= 80007)
		{
			LogManager::instance().GoldBarLog(victim->GetPlayerID(), item->GetID(), EXCHANGE_TAKE, "");
			LogManager::instance().GoldBarLog(GetOwner()->GetPlayerID(), item->GetID(), EXCHANGE_GIVE, "");
		}

		if (!victim->AutoGiveItem(item, true, true))
		{
			item = NULL;
		}

		if (item)
		{
			ITEM_MANAGER::instance().FlushDelayedSave(item);

			item->SetExchanging(false);
		}

		m_apItems[i] = NULL;
	}

	if (m_lGold)
	{
		GetOwner()->PointChange(POINT_GOLD, -m_lGold, true);
		victim->PointChange(POINT_GOLD, m_lGold, true);

		if (m_lGold > 1000)
		{
			char exchange_buf[51];
			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", GetOwner()->GetPlayerID(), GetOwner()->GetName());
			LogManager::instance().CharLog(victim, m_lGold, "EXCHANGE_GOLD_TAKE", exchange_buf);

			snprintf(exchange_buf, sizeof(exchange_buf), "%u %s", victim->GetPlayerID(), victim->GetName());
			LogManager::instance().CharLog(GetOwner(), m_lGold, "EXCHANGE_GOLD_GIVE", exchange_buf);
		}
	}

	m_pGrid->Clear();
	return true;
}

void CExchange::Accept(bool bIsAccept /* = true */)
{
	if (!GetCompany())
	{
		sys_err("Invalid company");
		return;
	}

	// Player can't update 5 seconds after a trade decrease.
	if (bIsAccept && GetCompany()->GetLastCriticalUpdatePulse() != 0 && thecore_pulse() < GetCompany()->GetLastCriticalUpdatePulse() + PASSES_PER_SEC(5))
	{
		SendInfo(true, LC_TEXT("You cant accept right now the content has changed.."));
		Accept(false);
		GetCompany()->Accept(false);
		return;
	}

	m_bAccept = !m_bAccept;

	if (!bIsAccept)
	{
		m_bAccept = false;
	}

	//Inform both players
	if (m_bAccept && bIsAccept) // No message on forced removal
	{
		SendInfo(false, LC_TEXT("You have prepared the trade."));
		GetCompany()->SendInfo(false, LC_TEXT("%s has prepared the trade."), GetOwner()->GetName());
	}

	if (!m_bAccept && bIsAccept) // No message on forced removal
	{
		SendInfo(false, LC_TEXT("You have withdrawn your willingness to trade"));
		GetCompany()->SendInfo(false, LC_TEXT("%s has withdrawn his willingness to trade."), GetOwner()->GetName());
	}

	if (!GetAcceptStatus() || !GetCompany()->GetAcceptStatus())
	{
		// Update the 'accepted' information
		exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, true, m_bAccept, NPOS, 0);
		exchange_packet(GetCompany()->GetOwner(), EXCHANGE_SUBHEADER_GC_ACCEPT, false, m_bAccept, NPOS, 0);
		return;
	}

	//Both accepted, run the trade!
	if (!PerformTrade())
	{
		//Something went wrong, unready both sides
		Accept(false);
		GetCompany()->Accept(false);
	}
	else
	{
		Cancel(); //All ok, end the trade.
	}
}

bool CExchange::PerformTrade()
{
	LPCHARACTER otherPlayer = GetCompany()->GetOwner();

	//PREVENT_PORTAL_AFTER_EXCHANGE
	GetOwner()->SetExchangeTime();
	otherPlayer->SetExchangeTime();
	//END_PREVENT_PORTAL_AFTER_EXCHANGE

	// @fixme150 BEGIN
	if (quest::CQuestManager::instance().GetPCForce(GetOwner()->GetPlayerID())->IsRunning() == true)
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if you're using quests"));
		otherPlayer->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if the other part using quests"));
		return false;
	}
	else if (quest::CQuestManager::instance().GetPCForce(otherPlayer->GetPlayerID())->IsRunning() == true)
	{
		otherPlayer->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if you're using quests"));
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade if the other part using quests"));
		return false;
	}
	// @fixme150 END

	//Check the player's gold to make sure that the sums are correct
	if (GetOwner()->GetGold() < GetExchangingGold())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough gold."));
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s doesn't have enough gold."), GetOwner()->GetName());
		return false;
	}

	if (otherPlayer->GetGold() < GetCompany()->GetExchangingGold())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s doesn't have enough gold."), otherPlayer->GetName());
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough gold."));
		return false;
	}

	// Run a sanity check for items that no longer exist,
	// invalid positions, etc.
	if (!SanityCheck())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unexpected Error."));
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unexpected player error."));
		return false;
	}

	if (!GetCompany()->SanityCheck())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unexpected player error."));
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Unexpected Error."));
		return false;
	}

	// Revise that each player can fit all the items from the trade
	// in their inventory.
	if (!CheckSpace())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is not enough space in the inventory."));
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is not enough space for the trade."));
		return false;
	}

	if (!GetCompany()->CheckSpace())
	{
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is not enough space for the trade."));
		GetCompany()->GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("There is not enough space in the inventory."));
		return false;
	}

	if (db_clientdesc->GetSocket() == INVALID_SOCKET)
	{
		sys_err("Cannot use exchange while DB cache connection is dead.");
		otherPlayer->ChatPacket(0, "Unknown trade error.");
		GetOwner()->ChatPacket(0, "Unknown trade error.");
		Cancel();
		return false;
	}

	//If nothing was traded, cancel things out
	if (CountExchangingItems() + GetCompany()->CountExchangingItems() == 0 && GetExchangingGold() + GetCompany()->GetExchangingGold() == 0)
	{
		otherPlayer->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Trade without goods is complete."));
		GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Trade without goods is complete."));
		Cancel();
		return false;
	}

	DWORD tradeID = LogManager::instance().ExchangeLog(EXCHANGE_TYPE_TRADE, GetOwner()->GetPlayerID(), otherPlayer->GetPlayerID(), GetOwner()->GetX(), GetOwner()->GetY(), m_lGold, GetCompany()->m_lGold);

	if (Done(tradeID, true))
	{
		if (m_lGold)
		{
			GetOwner()->Save();
		}

		if (GetCompany()->Done(tradeID, false))
		{
			if (GetCompany()->m_lGold)
			{
				otherPlayer->Save();
			}

			// INTERNATIONAL_VERSION
			GetOwner()->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s has been successful."), otherPlayer->GetName());
			otherPlayer->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("The trade with %s has been successful."), GetOwner()->GetName());
			// END_OF_INTERNATIONAL_VERSION
		}
	}

	return true;
}

void CExchange::Cancel()
{
	exchange_packet(GetOwner(), EXCHANGE_SUBHEADER_GC_END, 0, 0, NPOS, 0);
	GetOwner()->SetExchange(NULL);

	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_apItems[i])
		{
			m_apItems[i]->SetExchanging(false);
		}
	}

	if (GetCompany())
	{
		GetCompany()->SetCompany(NULL);
		GetCompany()->Cancel();
	}

	M2_DELETE(this);
}

#ifdef __ENABLE_RENEWAL_EXCHANGE__
void CExchange::SendInfo(bool isError, const char* format, ...)
{
	if (!GetOwner())
	{
		return;
	}

	LPDESC d = GetOwner()->GetDesc();
	if (!d)
	{
		return;
	}

	char rawbuf[512 + 1];
	va_list args;

	va_start(args, format);
	int len = vsnprintf(rawbuf, sizeof(rawbuf), format, args);
	va_end(args);

	TPacketGCExchageInfo pack;

	pack.bHeader = HEADER_GC_EXCHANGE_INFO;
	pack.wSize = sizeof(TPacketGCExchageInfo) + len;
	pack.bError = isError;
	pack.iUnixTime = std::time(nullptr);

	TEMP_BUFFER tmpbuf;
	tmpbuf.write(&pack, sizeof(pack));
	tmpbuf.write(rawbuf, len);

	d->Packet(tmpbuf.read_peek(), tmpbuf.size());
}
#endif

int CExchange::CountExchangingItems()
{
	int count = 0;
	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (!m_apItems[i])
		{
			continue;
		}

		++count;
	}

	return count;
}
BYTE CExchange::GetEmptySlot(const BYTE & bItemSize)
{
	for (int i = 0; i < EXCHANGE_ITEM_MAX_NUM; ++i)
	{
		if (m_pGrid->IsEmpty(i, 1, bItemSize))
		{
			return i;
		}
	}

	return -1;
}
