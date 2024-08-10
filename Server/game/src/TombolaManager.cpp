#include "stdafx.h"
#ifdef __TOMBOLA_EVENT_ENABLE__
#include "char.h"
#include "db.h"
#include "utils.h"
#include "log.h"
#include "p2p.h"
#include "questmanager.h"
#include "ItemShopManager.hpp"
#include "TombolaManager.hpp"

extern void BroadcastNotice(const char * c_pszBuf, const BYTE bType, const int & bLang);

namespace TombolaEvent
{
const int TOMBOLA_ITEMS_COUNT = 16;
const int TOMBOLA_SPIN_COST = 100;
const int UNIQUE_POOL_CHANCE = 30;
std::map<BYTE, std::array<STombolaItem, TOMBOLA_ITEMS_COUNT>> m_tombola_config;
std::string sTombolaPoolFlag = "tombola_event.pool";
std::string sEventFlag = "tombola_event";
std::string sCoolDown = "tombola_event.cooldown";
int iCoolDown = 2;

void LoadConfig()
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT iPool, id, dwVnum, wCount, iProb FROM player.tombola ORDER by id"));

	if (msg->Get()->uiNumRows == 0)
	{
		sys_err("Could not load tombola. No result!");
		return;
	}

	MYSQL_RES* pRes = msg->Get()->pSQLResult;
	MYSQL_ROW row;
	int dwProbMax = 0;
	BYTE i = 0;

	while ((row = mysql_fetch_row(pRes)))
	{
		STombolaItem rItem{};
		int iPool = -1;
		if (row[0] && *row[0])
		{
			iPool = strtoul(row[0], NULL, 10);
		}
		if (row[1] && *row[1])
		{
			i = strtoul(row[1], NULL, 10) - (iPool * TOMBOLA_ITEMS_COUNT);
		}
		if (row[2] && *row[2])
		{
			rItem.dwVnum = strtoul(row[2], NULL, 10);
		}
		if (row[3] && *row[3])
		{
			rItem.wCount = strtoul(row[3], NULL, 10);
		}

		// Reset max for first item
		if (!i)
		{
			dwProbMax = 0;
		}

		if (row[4] && *row[4])
		{
			rItem.iProbMin = dwProbMax;
			dwProbMax += strtoul(row[4], NULL, 10);
			rItem.iProbMax = dwProbMax;
		}

		if (iPool < 0)
		{
			sys_err("Could not find pool!");
			continue;
		}

		if (i >= TOMBOLA_ITEMS_COUNT)
		{
			sys_err("Invalid index: %u", i);
			continue;
		}

		m_tombola_config[iPool][i] = rItem;
		if (test_server)
		{
			sys_log(0, "Tombola: Item vnum %u loaded.", rItem.dwVnum);
		}
	}
}

void ReloadConfig()
{
	m_tombola_config.clear(); // clearing out
	LoadConfig();
}

void ChangeEventStatus(bool bStatus)
{
	BroadcastNotice((bStatus ? LC_TEXT("TOMBOLA_EVENT_ENABLE") : LC_TEXT("TOMBOLA_EVENT_DISABLE")));
	quest::CQuestManager::instance().RequestSetEventFlag(sEventFlag, bStatus);
}

void SpinWheel(LPCHARACTER ch)
{
	// Event is off
	if (!quest::CQuestManager::instance().GetEventFlag(sEventFlag))
	{
		return;
	}

	if (!ch || !ch->GetDesc() || !ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOMBOLA_CANNNOT_SPIN"));
		return;
	}

	if (ch->GetQuestFlag(sCoolDown) > get_global_time())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOMBOLA_COOLDOWN"));
		return;
	}

	// Already has an item pending to colllect
	if (ch->GetTombolaItem())
	{
		return;
	}

	// Doesn't have enough cash
	int iCash = CItemShopManager::instance().GetPlayerCash(ch);
	if (iCash < TOMBOLA_SPIN_COST)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TOMBOLA_DOESNT_HAVE_ENOUGH_COIN"));
		return;
	}

	// Get player pool
	int iPool = ch->GetQuestFlag(sTombolaPoolFlag);
	if (iPool < 0 || iPool >= m_tombola_config.size())
	{
		return;
	}

	auto & rPool = m_tombola_config[iPool];
	int iDice = number(0, rPool[(rPool.size()) - 1].iProbMax);

	for (unsigned int i = 0; i < rPool.size(); ++i)
	{
		auto & rItem = rPool[i];
		if (iDice > rItem.iProbMin && iDice <= rItem.iProbMax)
		{
			ch->SetTombolaItem(&rItem);
			CItemShopManager::instance().ChargePlayer(ch, TOMBOLA_SPIN_COST);
			ch->SetQuestFlag(sCoolDown, get_global_time() + iCoolDown);

			LogManager::instance().TombolaLog(ch->GetPlayerID(), "SPIN", rItem.dwVnum, rItem.wCount, iCash, iCash - TOMBOLA_SPIN_COST);
			ch->ChatPacket(CHAT_TYPE_COMMAND, "TombolaSpin %u", i);

			// Reshuffle rewards
			SetRandomPool(ch);
			return;
		}
	}

	sys_err("Error! Tombola was not configured properly!");
}

void ClaimReward(LPCHARACTER ch)
{
	// Event is off
	if (!quest::CQuestManager::instance().GetEventFlag(sEventFlag))
	{
		return;
	}

	if (!ch || !ch->GetDesc())
	{
		return;
	}

	auto pTombolaItem = ch->GetTombolaItem();
	if (!pTombolaItem)
	{
		return;
	}

	ch->AutoGiveItem(pTombolaItem->dwVnum, pTombolaItem->wCount);
	ch->FlushTombolaItem();

	LogManager::instance().TombolaLog(ch->GetPlayerID(), "CLAIM");
	BroadcastConfig(ch);
}

void SetRandomPool(LPCHARACTER ch)
{
	if (!ch)
	{
		return;
	}

	if (UNIQUE_POOL_CHANCE >= number(1, 100) || m_tombola_config.size() == 1)
	{
		ch->SetQuestFlag(sTombolaPoolFlag, 0);
	}
	else
	{
		ch->SetQuestFlag(sTombolaPoolFlag, number(1, m_tombola_config.size() - 1));
	}
}

void BroadcastConfig(LPCHARACTER ch)
{
	// Event is off
	if (!quest::CQuestManager::instance().GetEventFlag(sEventFlag))
	{
		return;
	}

	if (!ch)
	{
		return;
	}

	int iPool = ch->GetQuestFlag(sTombolaPoolFlag);
	for (unsigned int i = 0; i < TOMBOLA_ITEMS_COUNT; ++i)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TombolaItem %u %u %u", i, m_tombola_config[iPool][i].dwVnum, m_tombola_config[iPool][i].wCount);
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "TombolaStatus %d", quest::CQuestManager::instance().GetEventFlag(sEventFlag));

	// User balance
	ch->ChatPacket(CHAT_TYPE_COMMAND, "TombolaUserBalance %u", CItemShopManager::instance().GetPlayerCash(ch));
}
}

// CHARACTER
void CHARACTER::SetTombolaItem(TombolaEvent::STombolaItem * pTombolaItem)
{
	pTombolaReward = pTombolaItem;
}

const TombolaEvent::STombolaItem * CHARACTER::GetTombolaItem()
{
	return pTombolaReward;
}

void CHARACTER::FlushTombolaItem()
{
	pTombolaReward = nullptr;
}

// CMD
ACMD(do_tombola_reload)
{
	BYTE bHeader = HEADER_GG_TOMBOLA_RELOAD;
	P2P_MANAGER::instance().Send(&bHeader, sizeof(BYTE));
	TombolaEvent::ReloadConfig();
}

ACMD(do_tombola_status)
{
	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Tombola event: 0-1 (Enable/Disable)");
		return;
	}

	TombolaEvent::ChangeEventStatus(atoi(arg1));
}

ACMD(do_tombola_spin)
{
	TombolaEvent::SpinWheel(ch);
}

ACMD(do_tombola_reward)
{
	TombolaEvent::ClaimReward(ch);
}

ACMD(do_tombola_open_dialog)
{
	ch->ChatPacket(CHAT_TYPE_COMMAND, "TombolaOpenDialog");
}
#endif

