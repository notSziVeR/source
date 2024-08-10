
#include "stdafx.h"
#include "Cache.h"

#include "QID.h"
#include "ClientManager.h"
#include "Main.h"

#include <sstream>

extern CPacketInfo g_item_info;
extern int g_iPlayerCacheFlushSeconds;
extern int g_iItemCacheFlushSeconds;
extern int g_test_server;
// MYSHOP_PRICE_LIST
extern int g_iItemPriceListTableCacheFlushSeconds;
// END_OF_MYSHOP_PRICE_LIST
//
extern int g_item_count;

#ifdef __ENABLE_OFFLINE_SHOP__
	extern uint32_t g_offlineShopCacheFlushSeconds;
	extern uint32_t g_offlineShopItemCacheFlushSeconds;
#endif

CItemCache::CItemCache()
{
	m_expireTime = MIN(1800, g_iItemCacheFlushSeconds);
}

CItemCache::~CItemCache()
{
}

// fixme
// by rtsummit
void CItemCache::Delete()
{
	if (m_data.vnum == 0)
	{
		return;
	}

	//char szQuery[QUERY_MAX_LEN];
	//szQuery[QUERY_MAX_LEN] = '\0';
	if (g_test_server)
	{
		sys_log(0, "ItemCache::Delete : DELETE %u", m_data.id);
	}

	m_data.vnum = 0;
	m_bNeedQuery = true;
	m_lastUpdateTime = time(0);
	OnFlush();

	//m_bNeedQuery = false;
}

void CItemCache::OnFlush()
{
	if (m_data.vnum == 0)
	{
		char szQuery[QUERY_MAX_LEN];
		snprintf(szQuery, sizeof(szQuery), "DELETE FROM item%s WHERE id=%u", GetTablePostfix(), m_data.id);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEM_DESTROY, 0, NULL);

		if (g_test_server)
		{
			sys_log(0, "ItemCache::Flush : DELETE %u %s", m_data.id, szQuery);
		}
	}
	else
	{
		long alSockets[ITEM_SOCKET_MAX_NUM];
		TPlayerItemAttribute aAttr[ITEM_ATTRIBUTE_MAX_NUM];
		bool isSocket = false, isAttr = false;

		memset(&alSockets, 0, sizeof(long) * ITEM_SOCKET_MAX_NUM);
		memset(&aAttr, 0, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM);

		TPlayerItem * p = &m_data;

		if (memcmp(alSockets, p->alSockets, sizeof(long) * ITEM_SOCKET_MAX_NUM))
		{
			isSocket = true;
		}

		if (memcmp(aAttr, p->aAttr, sizeof(TPlayerItemAttribute) * ITEM_ATTRIBUTE_MAX_NUM))
		{
			isAttr = true;
		}

		char szColumns[QUERY_MAX_LEN];
		char szValues[QUERY_MAX_LEN];
		char szUpdate[QUERY_MAX_LEN];

#ifdef __ADMIN_MANAGER__
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, window, pos, count, vnum, is_gm_owner");

		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u, %u",
								 p->id, p->owner, p->window, p->pos, p->count, p->vnum, p->is_gm_owner);

		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, window=%d, pos=%d, count=%u, vnum=%u, is_gm_owner=%u",
								  p->owner, p->window, p->pos, p->count, p->vnum, p->is_gm_owner);
#else
		int iLen = snprintf(szColumns, sizeof(szColumns), "id, owner_id, `window`, pos, count, vnum");

		int iValueLen = snprintf(szValues, sizeof(szValues), "%u, %u, %d, %d, %u, %u",
								 p->id, p->owner, p->window, p->pos, p->count, p->vnum);

		int iUpdateLen = snprintf(szUpdate, sizeof(szUpdate), "owner_id=%u, `window`=%d, pos=%d, count=%u, vnum=%u",
								  p->owner, p->window, p->pos, p->count, p->vnum);
#endif

		if (isSocket)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", socket0, socket1, socket2, socket3, socket4, socket5, socket6, socket7, socket8, socket9");
			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
								  ", %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld, %ld", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5], p->alSockets[6], p->alSockets[7], p->alSockets[8], p->alSockets[9]);
			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
								   ", socket0=%ld, socket1=%ld, socket2=%ld, socket3=%ld, socket4=%ld, socket5=%ld, socket6=%ld, socket7=%ld, socket8=%ld, socket9=%ld", p->alSockets[0], p->alSockets[1], p->alSockets[2], p->alSockets[3], p->alSockets[4], p->alSockets[5], p->alSockets[6], p->alSockets[7], p->alSockets[8], p->alSockets[9]);
		}

		if (isAttr)
		{
			iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen,
							 ", attrtype0, attrvalue0, attrtype1, attrvalue1, attrtype2, attrvalue2, attrtype3, attrvalue3"
							 ", attrtype4, attrvalue4, attrtype5, attrvalue5, attrtype6, attrvalue6");

			iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
								  ", %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d",
								  p->aAttr[0].bType, p->aAttr[0].sValue,
								  p->aAttr[1].bType, p->aAttr[1].sValue,
								  p->aAttr[2].bType, p->aAttr[2].sValue,
								  p->aAttr[3].bType, p->aAttr[3].sValue,
								  p->aAttr[4].bType, p->aAttr[4].sValue,
								  p->aAttr[5].bType, p->aAttr[5].sValue,
								  p->aAttr[6].bType, p->aAttr[6].sValue);

			iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
								   ", attrtype0=%d, attrvalue0=%d"
								   ", attrtype1=%d, attrvalue1=%d"
								   ", attrtype2=%d, attrvalue2=%d"
								   ", attrtype3=%d, attrvalue3=%d"
								   ", attrtype4=%d, attrvalue4=%d"
								   ", attrtype5=%d, attrvalue5=%d"
								   ", attrtype6=%d, attrvalue6=%d",
								   p->aAttr[0].bType, p->aAttr[0].sValue,
								   p->aAttr[1].bType, p->aAttr[1].sValue,
								   p->aAttr[2].bType, p->aAttr[2].sValue,
								   p->aAttr[3].bType, p->aAttr[3].sValue,
								   p->aAttr[4].bType, p->aAttr[4].sValue,
								   p->aAttr[5].bType, p->aAttr[5].sValue,
								   p->aAttr[6].bType, p->aAttr[6].sValue);
		}

#ifdef __TRANSMUTATION_SYSTEM__
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", trans_id");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
							  ", %d", p->transmutate_id);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
							   ", trans_id=%d", p->transmutate_id);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
		iLen += snprintf(szColumns + iLen, sizeof(szColumns) - iLen, ", refine_element");
		iValueLen += snprintf(szValues + iValueLen, sizeof(szValues) - iValueLen,
							  ", %d", p->dwRefineElement);
		iUpdateLen += snprintf(szUpdate + iUpdateLen, sizeof(szUpdate) - iUpdateLen,
							   ", refine_element=%d", p->dwRefineElement);
#endif

		char szItemQuery[QUERY_MAX_LEN * 3];
		snprintf(szItemQuery, sizeof(szItemQuery), "REPLACE INTO item%s (%s) VALUES(%s)", GetTablePostfix(), szColumns, szValues);

		if (g_test_server)
		{
			sys_log(0, "ItemCache::Flush :REPLACE  (%s)", szItemQuery);
		}

		CDBManager::instance().ReturnQuery(szItemQuery, QID_ITEM_SAVE, 0, NULL);

		//g_item_info.Add(p->vnum);
		++g_item_count;
	}

	m_bNeedQuery = false;
}

//
// CPlayerTableCache
//
CPlayerTableCache::CPlayerTableCache()
{
	m_expireTime = MIN(1800, g_iPlayerCacheFlushSeconds);
}

CPlayerTableCache::~CPlayerTableCache()
{
}

void CPlayerTableCache::OnFlush()
{
	if (g_test_server)
	{
		sys_log(0, "PlayerTableCache::Flush : %s", m_data.name);
	}

	char szQuery[QUERY_MAX_LEN];
	CreatePlayerSaveQuery(szQuery, sizeof(szQuery), &m_data);
	CDBManager::instance().ReturnQuery(szQuery, QID_PLAYER_SAVE, 0, NULL);
}

// MYSHOP_PRICE_LIST
//
// CItemPriceListTableCache class implementation
//

const int CItemPriceListTableCache::s_nMinFlushSec = 1800;

CItemPriceListTableCache::CItemPriceListTableCache()
{
	m_expireTime = MIN(s_nMinFlushSec, g_iItemPriceListTableCacheFlushSeconds);
}

void CItemPriceListTableCache::UpdateList(const TItemPriceListTable* pUpdateList)
{
	//

	//

	std::vector<TItemPriceInfo> tmpvec;

	for (uint idx = 0; idx < m_data.byCount; ++idx)
	{
		const TItemPriceInfo* pos = pUpdateList->aPriceInfo;
		for (; pos != pUpdateList->aPriceInfo + pUpdateList->byCount && m_data.aPriceInfo[idx].dwVnum != pos->dwVnum; ++pos)
			;

		if (pos == pUpdateList->aPriceInfo + pUpdateList->byCount)
		{
			tmpvec.push_back(m_data.aPriceInfo[idx]);
		}
	}

	//

	//

	if (pUpdateList->byCount > SHOP_PRICELIST_MAX_NUM)
	{
		sys_err("Count overflow!");
		return;
	}

	m_data.byCount = pUpdateList->byCount;

	thecore_memcpy(m_data.aPriceInfo, pUpdateList->aPriceInfo, sizeof(TItemPriceInfo) * pUpdateList->byCount);

	int nDeletedNum;

	if (pUpdateList->byCount < SHOP_PRICELIST_MAX_NUM)
	{
		size_t sizeAddOldDataSize = SHOP_PRICELIST_MAX_NUM - pUpdateList->byCount;

		if (tmpvec.size() < sizeAddOldDataSize)
		{
			sizeAddOldDataSize = tmpvec.size();
		}
		if (tmpvec.size() != 0)
		{
			thecore_memcpy(m_data.aPriceInfo + pUpdateList->byCount, &tmpvec[0], sizeof(TItemPriceInfo) * sizeAddOldDataSize);
			m_data.byCount += sizeAddOldDataSize;
		}
		nDeletedNum = tmpvec.size() - sizeAddOldDataSize;
	}
	else
	{
		nDeletedNum = tmpvec.size();
	}

	m_bNeedQuery = true;

	sys_log(0,
			"ItemPriceListTableCache::UpdateList : OwnerID[%u] Update [%u] Items, Delete [%u] Items, Total [%u] Items",
			m_data.dwOwnerID, pUpdateList->byCount, nDeletedNum, m_data.byCount);
}

void CItemPriceListTableCache::OnFlush()
{
	char szQuery[QUERY_MAX_LEN];

	//

	//

	snprintf(szQuery, sizeof(szQuery), "DELETE FROM myshop_pricelist%s WHERE owner_id = %u", GetTablePostfix(), m_data.dwOwnerID);
	CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_DESTROY, 0, NULL);

	//

	//

	for (int idx = 0; idx < m_data.byCount; ++idx)
	{
		snprintf(szQuery, sizeof(szQuery),
#ifdef __EXTANDED_GOLD_AMOUNT__
				 "REPLACE myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %lld)", // @fixme204 (INSERT INTO -> REPLACE)
#else
				 "REPLACE myshop_pricelist%s(owner_id, item_vnum, price) VALUES(%u, %u, %u)", // @fixme204 (INSERT INTO -> REPLACE)
#endif
				 GetTablePostfix(), m_data.dwOwnerID, m_data.aPriceInfo[idx].dwVnum, m_data.aPriceInfo[idx].dwPrice);
		CDBManager::instance().ReturnQuery(szQuery, QID_ITEMPRICE_SAVE, 0, NULL);
	}

	sys_log(0, "ItemPriceListTableCache::Flush : OwnerID[%u] Update [%u]Items", m_data.dwOwnerID, m_data.byCount);

	m_bNeedQuery = false;
}

CItemPriceListTableCache::~CItemPriceListTableCache()
{
}

#ifdef __ENABLE_OFFLINE_SHOP__
COfflineShopCache::COfflineShopCache()
{
	m_expireTime = g_offlineShopCacheFlushSeconds;
}

void COfflineShopCache::OnFlush()
{
	std::array < char, CHARACTER_NAME_MAX_LEN * 2 + 1 > escapedOwnerName;
	CDBManager::Instance().EscapeString(escapedOwnerName.data(), m_data.ownerName.data(), m_data.ownerName.size());

	std::array < char, SHOP_TAB_NAME_MAX * 2 + 1 > escapedShopName;
	CDBManager::Instance().EscapeString(escapedShopName.data(), m_data.shopName.data(), m_data.shopName.size());

	std::stringstream query;
	query << "REPLACE INTO offline_shop" << GetTablePostfix() << "("
		  << "id, owner_pid, owner_name, name, name_change_time, channel, "
		  << "map_index, x, y, deco_race, deco_board, opening_time, gold"
		  << ") VALUES("
		  << m_data.id << ", "
		  << m_data.ownerPid << ", "
		  << "'" << escapedOwnerName.data() << "', "
		  << "'" << escapedShopName.data() << "', "
		  << "'" << m_data.shopNameChangeTime << "', "
		  << m2::to_string(m_data.channel) << ", "
		  << m_data.mapIndex << ", "
		  << m_data.x << ", "
		  << m_data.y << ", "
		  << m_data.decoRace << ", "
		  << m2::to_string(m_data.decoBoard) << ", "
		  << m_data.openingTime << ", "
		  << m_data.gold
		  << ")";

	sys_err("OFFLINE SHOP: %s", query.str().c_str());
	CDBManager::Instance().AsyncQuery(query.str().c_str());
	m_bNeedQuery = false;
}

COfflineShopItemCache::COfflineShopItemCache()
{
	m_expireTime = g_offlineShopItemCacheFlushSeconds;
}

void COfflineShopItemCache::OnFlush()
{
	if (m_data.vnum == 0)
	{
		std::stringstream query;
		query << "DELETE FROM offline_shop_item" << GetTablePostfix() << " WHERE owner_pid = " << m_data.id.first << " AND position = " << m2::to_string(m_data.id.second);

		sys_err("OFFLINE SHOP ITEM: %s", query.str().c_str());
		CDBManager::Instance().AsyncQuery(query.str().c_str());
	}
	else
	{
		std::stringstream fields, values;
		fields << "owner_pid, position, vnum, count, ";
#ifdef __TRANSMUTATION_SYSTEM__
		fields << "transmutation, ";
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
		fields << "refineElement, ";
#endif
		values << m_data.id.first << ", " << m2::to_string(m_data.id.second) << ", "
			   << m_data.vnum << ", " << m2::to_string(m_data.count) << ", "
#ifdef __TRANSMUTATION_SYSTEM__
			   << m2::to_string(m_data.transmutation) << ", "
#endif
#ifdef __ENABLE_REFINE_ELEMENT__
			   << m2::to_string(m_data.refineElement) << ", "
#endif
			   ;

		size_t i = 0;
		for (const auto& socket : m_data.sockets)
		{
			fields << "socket" << i << ", ";
			values << socket << ", ";
			++i;
		}

		i = 0;
		for (const auto& attribute : m_data.attributes)
		{
			fields << "attrtype" << i << ", attrvalue" << i << ", ";
			values << m2::to_string(attribute.bType) << ", " << attribute.sValue << ", ";
			++i;
		}

		fields << "price";
		values << m_data.price;

		std::stringstream query;
		query << "REPLACE INTO offline_shop_item" << GetTablePostfix() << "("
			  << fields.str() << ") VALUES(" << values.str() << ")";

		sys_err("OFFLINE SHOP ITEM: %s", query.str().c_str());
		CDBManager::Instance().AsyncQuery(query.str().c_str());
	}

	m_bNeedQuery = false;
}
#endif
