#include "stdafx.h"
#include "../../libgame/include/grid.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "shop.h"
#include "desc.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "item.h"
#include "item_manager.h"
#include "buffer_manager.h"
#include "packet.h"
#include "log.h"
#include "db.h"
#include "questmanager.h"
#include "mob_manager.h"
#include "locale_service.h"
#include "desc_client.h"
#include "shop_manager.h"
#include "group_text_parse_tree.h"
#include "shopEx.h"
#include <boost/algorithm/string/predicate.hpp>
#include "shop_manager.h"
#include <cctype>

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

CShopManager::CShopManager()
{
}

CShopManager::~CShopManager()
{
	Destroy();
}

bool CShopManager::Initialize(TShopTable * table, int size)
{
	if (!m_map_pkShop.empty())
	{
		return false;
	}

	int i;

	for (i = 0; i < size; ++i, ++table)
	{
		LPSHOP shop = M2_NEW CShop;

		if (!shop->Create(table->dwVnum, table->dwNPCVnum, table->items))
		{
			M2_DELETE(shop);
			continue;
		}

		m_map_pkShop.insert(TShopMap::value_type(table->dwVnum, shop));
		m_map_pkShopByNPCVnum.insert(TShopMap::value_type(table->dwNPCVnum, shop));
	}

	return InitializeShopEx();
}

bool CShopManager::InitializeShopEx()
{
	DestroyShopEx();
	char szShopTableExFileName[256];

	snprintf(szShopTableExFileName, sizeof(szShopTableExFileName),
			 "%s/shop_table_ex.txt", GetBasePath().c_str());

	return ReadShopTableEx(szShopTableExFileName);
}

void CShopManager::DestroyShopEx()
{
	std::set<DWORD> destroyShopVnums;
	for (auto it = m_map_pkShopByNPCVnum.begin(); it != m_map_pkShopByNPCVnum.end(); )
	{
		auto it_cur = it++;

		CShopEx* pkShopEx = dynamic_cast<CShopEx*>(it_cur->second);
		if (!pkShopEx)
		{
			continue;    // no extended shop version
		}

		if (pkShopEx->GetVnum() != 0)
		{
			m_map_pkShop.erase(pkShopEx->GetVnum());
		}

		M2_DELETE(pkShopEx);
		m_map_pkShopByNPCVnum.erase(it_cur);
	}
}

void CShopManager::Destroy()
{
	TShopMap::iterator it = m_map_pkShop.begin();

	while (it != m_map_pkShop.end())
	{
		M2_DELETE(it->second);
		++it;
	}

	m_map_pkShop.clear();

	DestroyShopEx();
}

LPSHOP CShopManager::Get(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShop.find(dwVnum);

	if (it == m_map_pkShop.end())
	{
		return NULL;
	}

	return (it->second);
}

LPSHOP CShopManager::GetByNPCVnum(DWORD dwVnum)
{
	TShopMap::const_iterator it = m_map_pkShopByNPCVnum.find(dwVnum);

	if (it == m_map_pkShopByNPCVnum.end())
	{
		return NULL;
	}

	return (it->second);
}

bool CShopManager::StartShopping(LPCHARACTER pkChr, LPCHARACTER pkChrShopKeeper, int iShopVnum)
{
	if (pkChr->GetShopOwner() == pkChrShopKeeper)
	{
		return false;
	}
	// this method is only for NPC
	if (pkChrShopKeeper->IsPC())
	{
		return false;
	}

	//PREVENT_TRADE_WINDOW
	if (pkChr->IsOpenSafebox() || pkChr->GetExchange() || pkChr->GetMyShop() || pkChr->IsCubeOpen())
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You cannot trade in the shop while another window is open."));
		return false;
	}
	//END_PREVENT_TRADE_WINDOW

	long distance = DISTANCE_APPROX(pkChr->GetX() - pkChrShopKeeper->GetX(), pkChr->GetY() - pkChrShopKeeper->GetY());

	if (distance >= SHOP_MAX_DISTANCE)
	{
		sys_log(1, "SHOP: TOO_FAR: %s distance %d", pkChr->GetName(), distance);
		return false;
	}

	LPSHOP pkShop;

	if (iShopVnum)
	{
		pkShop = Get(iShopVnum);
	}
	else
	{
		pkShop = GetByNPCVnum(pkChrShopKeeper->GetRaceNum());
	}

	if (!pkShop)
	{
		sys_log(1, "SHOP: NO SHOP");
		return false;
	}

	bool bOtherEmpire = false;

	if (pkChr->GetEmpire() != pkChrShopKeeper->GetEmpire())
	{
		bOtherEmpire = true;
	}

	pkShop->AddGuest(pkChr, pkChrShopKeeper->GetVID(), bOtherEmpire);
	pkChr->SetShopOwner(pkChrShopKeeper);
	sys_log(0, "SHOP: START: %s", pkChr->GetName());
	return true;
}

LPSHOP CShopManager::FindPCShop(DWORD dwVID)
{
	TShopMap::iterator it = m_map_pkShopByPC.find(dwVID);

	if (it == m_map_pkShopByPC.end())
	{
		return NULL;
	}

	return it->second;
}

LPSHOP CShopManager::CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount)
{
	if (FindPCShop(ch->GetVID()))
	{
		return NULL;
	}

	LPSHOP pkShop = M2_NEW CShop;
	pkShop->SetPCShop(ch);
	pkShop->SetShopItems(pTable, bItemCount);

	m_map_pkShopByPC.insert(TShopMap::value_type(ch->GetVID(), pkShop));
	return pkShop;
}

void CShopManager::DestroyPCShop(LPCHARACTER ch)
{
	LPSHOP pkShop = FindPCShop(ch->GetVID());

	if (!pkShop)
	{
		return;
	}

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	m_map_pkShopByPC.erase(ch->GetVID());
	M2_DELETE(pkShop);
}


void CShopManager::StopShopping(LPCHARACTER ch)
{
	LPSHOP shop;

	if (!(shop = ch->GetShop()))
	{
		return;
	}

	//PREVENT_ITEM_COPY;
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

	shop->RemoveGuest(ch);
	sys_log(0, "SHOP: END: %s", ch->GetName());
}


#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	void CShopManager::Buy(LPCHARACTER ch, BYTE pos, CountType wAmount)
#else
	void CShopManager::Buy(LPCHARACTER ch, BYTE pos)
#endif
{
#ifdef __ENABLE_NEWSTUFF__
	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime() + g_BuySellTimeLimitValue)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());
#endif
	if (!ch->GetShop())
	{
		return;
	}

	if (!ch->GetShopOwner())
	{
		return;
	}

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too far away from the shop to buy something."));
		return;
	}

	CShop* pkShop = ch->GetShop();

	if (!pkShop->IsPCShop())
	{
		//if (pkShop->GetVnum() == 0)
		//	return;
		//const CMob* pkMob = CMobManager::instance().Get(pkShop->GetNPCVnum());
		//if (!pkMob)
		//	return;

		//if (pkMob->m_table.bType != CHAR_TYPE_NPC)
		//{
		//	return;
		//}
	}
	else
	{
	}

	//PREVENT_ITEM_COPY
	ch->SetMyShopTime();
	//END_PREVENT_ITEM_COPY

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	int ret = pkShop->Buy(ch, pos, wAmount);
#else
	int ret = pkShop->Buy(ch, pos);
#endif

	if (SHOP_SUBHEADER_GC_OK != ret)
	{
		TPacketGCShop pack;

		pack.header	= HEADER_GC_SHOP;
		pack.subheader	= ret;
		pack.size	= sizeof(TPacketGCShop);

		ch->GetDesc()->Packet(&pack, sizeof(pack));
	}
}

void CShopManager::Sell(LPCHARACTER ch, WORD wCell, CountType bCount, BYTE bType)
{
#ifdef __ENABLE_NEWSTUFF__
	if (0 != g_BuySellTimeLimitValue)
	{
		if (get_dword_time() < ch->GetLastBuySellTime() + g_BuySellTimeLimitValue)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("아직 골드를 버릴 수 없습니다."));
			return;
		}
	}

	ch->SetLastBuySellTime(get_dword_time());
#endif
	if (!ch->GetShop())
	{
		return;
	}

	if (!ch->GetShopOwner())
	{
		return;
	}

	if (!ch->CanHandleItem())
	{
		return;
	}

	if (ch->GetShop()->IsPCShop())
	{
		return;
	}

	if (DISTANCE_APPROX(ch->GetX() - ch->GetShopOwner()->GetX(), ch->GetY() - ch->GetShopOwner()->GetY()) > 2000)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are too far away from the shop to sell something."));
		return;
	}

	LPITEM item = ch->GetItem(TItemPos(bType, wCell));

	if (!item)
	{
		return;
	}

	if (item->IsEquipped() == true)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("착용 중인 아이템은 판매할 수 없습니다."));
		return;
	}

	if (true == item->isLocked())
	{
		return;
	}

	if (IS_SET(item->GetAntiFlag(), ITEM_ANTIFLAG_SELL))
	{
		return;
	}

	if (bCount == 0 || bCount > item->GetCount())
	{
		bCount = item->GetCount();
	}

	auto dwPrice = item->GetShopBuyPrice();

	// Sell/Buy price difference fix
	//if (dwPrice > item->GetGold())
	//	dwPrice = item->GetGold();
	//else
	//	dwPrice *= static_cast<int64_t>(bCount);

	if (IS_SET(item->GetFlag(), ITEM_FLAG_COUNT_PER_1GOLD))
	{
		if (dwPrice == 0)
		{
			dwPrice = bCount;
		}
		else
		{
			dwPrice = bCount / dwPrice;
		}
	}
	else
	{
		dwPrice *= bCount;
	}

	dwPrice /= 5;

	int iVal = 3;
	int64_t tax = dwPrice * iVal / 100;
	dwPrice -= tax;

	if (test_server)
	{
		sys_log(0, "Sell Item price id %d %s itemid %d", ch->GetPlayerID(), ch->GetName(), item->GetID());
	}

	const auto maxPrice = GOLD_MAX - ch->GetGold();
	if (dwPrice > maxPrice)
	{
		sys_err("[OVERFLOW_GOLD] id %u name %s gold %lld", ch->GetPlayerID(), ch->GetName(), ch->GetGold());
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You carry more than 2 Billion Gold, you cannot trade."));
		return;
	}

	sys_log(0, "SHOP: SELL: %s item name: %s(x%d):%u price: %lld", ch->GetName(), item->GetBaseName(), bCount, item->GetID(), dwPrice);

	if (iVal > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%d%% sales tax deducted."), iVal);
	}

	DBManager::instance().SendMoneyLog(MONEY_LOG_SHOP, item->GetVnum(), dwPrice);

	if (bCount == item->GetCount())
	{
		ITEM_MANAGER::instance().RemoveItem(item, "SELL");
	}
	else
	{
		item->SetCount(item->GetCount() - bCount);
	}

#ifdef __BATTLE_PASS_ENABLE__
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_NPC_SELL_ITEM, item->GetVnum(), bCount });
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_NPC_SELL_ALL, 0, bCount });
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_NPC_GET_MONEY, 0, static_cast<uint64_t>(dwPrice) });
	CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_GET_MONEY, 0, static_cast<uint64_t>(dwPrice) });
#endif

	ch->PointChange(POINT_GOLD, dwPrice, false);
}

bool CompareShopItemName(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	TItemTable* lItem = ITEM_MANAGER::instance().GetTable(lhs.vnum);
	TItemTable* rItem = ITEM_MANAGER::instance().GetTable(rhs.vnum);
	if (lItem && rItem)
	{
		return strcmp(lItem->szLocaleName, rItem->szLocaleName) < 0;
	}
	else
	{
		return true;
	}
}

bool CompareShopItemVnum(const SShopItemTable& lhs, const SShopItemTable& rhs)
{
	return lhs.vnum < rhs.vnum;
}

bool ConvertToShopItemTable(IN CGroupNode* pNode, OUT TShopTableEx& shopTable)
{
	if (!pNode->GetValue("vnum", 0, shopTable.dwVnum))
	{
		sys_err("Group %s does not have vnum.", pNode->GetNodeName().c_str());
		return false;
	}

	if (!pNode->GetValue("name", 0, shopTable.name))
	{
		sys_err("Group %s does not have name.", pNode->GetNodeName().c_str());
		return false;
	}

	if (shopTable.name.length() >= SHOP_TAB_NAME_MAX)
	{
		sys_err("Shop name length must be less than %d. Error in Group %s, name %s", SHOP_TAB_NAME_MAX, pNode->GetNodeName().c_str(), shopTable.name.c_str());
		return false;
	}

	std::string stCoinType;
	if (!pNode->GetValue("cointype", 0, stCoinType))
	{
		stCoinType = "Gold";
	}

	int iCoinItemVnum = 0;

	if (boost::iequals(stCoinType, "Gold"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_GOLD;
	}
	else if (boost::iequals(stCoinType, "Item"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SPECIFIC_ITEM;

		if (!pNode->GetValue("coinvalue", 0, iCoinItemVnum))
		{
			sys_err("Group %s had undefine coinvalue with Item as cointype", pNode->GetNodeName().c_str());
			return false;
		}
	}
	else if (boost::iequals(stCoinType, "SecondaryCoin"))
	{
		shopTable.coinType = SHOP_COIN_TYPE_SECONDARY_COIN;
	}
	else
	{
		sys_err("Group %s has undefine cointype(%s).", pNode->GetNodeName().c_str(), stCoinType.c_str());
		return false;
	}

	CGroupNode* pItemGroup = pNode->GetChildNode("items");
	if (!pItemGroup)
	{
		sys_err("Group %s does not have 'group items'.", pNode->GetNodeName().c_str());
		return false;
	}

	int itemGroupSize = pItemGroup->GetRowCount();
	std::vector <TShopItemExTable> shopItems(itemGroupSize);
	if (itemGroupSize >= SHOP_HOST_ITEM_MAX_NUM)
	{
		sys_err("count(%d) of rows of group items of group %s must be smaller than %d", itemGroupSize, pNode->GetNodeName().c_str(), SHOP_HOST_ITEM_MAX_NUM);
		return false;
	}

	for (int i = 0; i < itemGroupSize; i++)
	{
		std::string idxVal = std::to_string(1 + i);

		if (!pItemGroup->GetValue(idxVal, "vnum", shopItems[i].vnum))
		{
			sys_err("row(%d) of group items of group %s does not have vnum column", i, pNode->GetNodeName().c_str());
			return false;
		}
		sys_log(0, "GetShopExItem[%d/%d] => vnum %d", i, itemGroupSize, shopItems[i].vnum);

		if (!pItemGroup->GetValue(idxVal, "count", shopItems[i].count))
		{
			sys_err("row(%d) of group items of group %s does not have count column", i, pNode->GetNodeName().c_str());
			return false;
		}
		if (!pItemGroup->GetValue(idxVal, "price", shopItems[i].price))
		{
			shopItems[i].price = 0;
		}
		if (!pItemGroup->GetValue(idxVal, "coinvnum", shopItems[i].coin_vnum))
		{
			shopItems[i].coin_vnum = iCoinItemVnum;
		}
	}
	std::string stSort;
	if (!pNode->GetValue("sort", 0, stSort))
	{
		stSort = "None";
	}

	if (boost::iequals(stSort, "Asc"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemName);
	}
	else if (boost::iequals(stSort, "Desc"))
	{
		std::sort(shopItems.rbegin(), shopItems.rend(), CompareShopItemName);
	}
	else if (boost::iequals(stSort, "Vnum"))
	{
		std::sort(shopItems.begin(), shopItems.end(), CompareShopItemVnum);
	}

	CGrid grid = CGrid(5, 9);
	int iPos;

	memset(&shopTable.items[0], 0, sizeof(shopTable.items));
	for (size_t i = 0; i < shopItems.size(); i++)
	{
		TItemTable * item_table = ITEM_MANAGER::instance().GetTable(shopItems[i].vnum);
		if (!item_table)
		{
			sys_err("vnum(%d) of group items of group %s does not exist", shopItems[i].vnum, pNode->GetNodeName().c_str());
			return false;
		}

		iPos = grid.FindBlank(1, item_table->bSize);

		sys_log(0, "AddShopExItem [%d] => %u pos %d", i, shopItems[i].vnum, iPos);

		grid.Put(iPos, 1, item_table->bSize);
		shopTable.items[iPos] = shopItems[i];
		shopTable.itemsEx[iPos] = shopItems[i];
	}

	shopTable.byItemCount = shopItems.size();
	return true;
}

bool CShopManager::ReadShopTableEx(const char* stFileName)
{


	FILE* fp = fopen(stFileName, "rb");
	if (NULL == fp)
	{
		return true;
	}
	fclose(fp);

	CGroupTextParseTreeLoader loader;
	if (!loader.Load(stFileName))
	{
		sys_err("%s Load fail.", stFileName);
		return false;
	}

	CGroupNode* pShopNPCGroup = loader.GetGroup("shopnpc");
	if (NULL == pShopNPCGroup)
	{
		sys_err("Group ShopNPC is not exist.");
		return false;
	}

	typedef std::multimap <DWORD, TShopTableEx> TMapNPCshop;
	TMapNPCshop map_npcShop;
	for (int i = 0; i < pShopNPCGroup->GetRowCount(); i++)
	{
		DWORD npcVnum;
		std::string shopName;
		if (!pShopNPCGroup->GetValue(i, "npc", npcVnum) || !pShopNPCGroup->GetValue(i, "group", shopName))
		{
			sys_err("Invalid row(%d). Group ShopNPC rows must have 'npc', 'group' columns", i);
			return false;
		}
		std::transform(shopName.begin(), shopName.end(), shopName.begin(), (int(*)(int))std::tolower);
		CGroupNode* pShopGroup = loader.GetGroup(shopName.c_str());
		if (!pShopGroup)
		{
			sys_err("Group %s is not exist.", shopName.c_str());
			return false;
		}
		TShopTableEx table;
		if (!ConvertToShopItemTable(pShopGroup, table))
		{
			sys_err("Cannot read Group %s.", shopName.c_str());
			return false;
		}
		if (m_map_pkShopByNPCVnum.find(npcVnum) != m_map_pkShopByNPCVnum.end())
		{
			sys_err("%d cannot have both original shop and extended shop", npcVnum);
			return false;
		}

		map_npcShop.insert(TMapNPCshop::value_type(npcVnum, table));
	}

	for (TMapNPCshop::iterator it = map_npcShop.begin(); it != map_npcShop.end(); ++it)
	{
		DWORD npcVnum = it->first;
		TShopTableEx& table = it->second;
		if (m_map_pkShop.find(table.dwVnum) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exists", table.dwVnum);
			return false;
		}
		TShopMap::iterator shop_it = m_map_pkShopByNPCVnum.find(npcVnum);

		LPSHOPEX pkShopEx = NULL;
		if (m_map_pkShopByNPCVnum.end() == shop_it)
		{
			pkShopEx = M2_NEW CShopEx;
			pkShopEx->Create(0, npcVnum);
			m_map_pkShopByNPCVnum.insert(TShopMap::value_type(npcVnum, pkShopEx));
		}
		else
		{
			pkShopEx = dynamic_cast <CShopEx*> (shop_it->second);
			if (NULL == pkShopEx)
			{
				sys_err("WTF!!! It can't be happend. NPC(%d) Shop is not extended version.", shop_it->first);
				return false;
			}
		}

		if (pkShopEx->GetTabCount() >= SHOP_TAB_COUNT_MAX)
		{
			sys_err("ShopEx cannot have tab more than %d", SHOP_TAB_COUNT_MAX);
			return false;
		}

		if (pkShopEx->GetVnum() != 0 && m_map_pkShop.find(pkShopEx->GetVnum()) != m_map_pkShop.end())
		{
			sys_err("Shop vnum(%d) already exist.", pkShopEx->GetVnum());
			return false;
		}
		if (pkShopEx->GetVnum() != 0)
		{
			m_map_pkShop.insert(TShopMap::value_type(pkShopEx->GetVnum(), pkShopEx));
		}

		pkShopEx->AddShopTable(table);
	}

	return true;
}
