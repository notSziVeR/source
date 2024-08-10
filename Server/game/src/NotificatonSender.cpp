#include "stdafx.h"
#ifdef __NOTIFICATON_SENDER_ENABLE__
#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "char_manager.h"
#include "utils.h"
#include "NotificatonSender.hpp"

namespace NotificatonSender
{
std::unordered_map<DWORD, CCollectorCache *> m_collector_cache;
time_t NOTIFICATION_DELAY;

void SendPlayerList(LPCHARACTER ch)
{
	if (!ch->CanDoAction() || ch->GetDungeon())
	{
		return;
	}

	auto v_name_list(std::move(GetPlayerListByMapIndex(ch->GetMapIndex())));
	// Firstly, broadcasting cont. count
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleSendCollectionCount %d", v_name_list.size());

	// Then, broadcasting PCs' names
	for (const auto & p_name : v_name_list)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleSendPlayerName %s", p_name.c_str());
	}

	// Caching to keep cohesion
	CacheCollector(ch, std::move(v_name_list));
}

int SendNotifications(LPCHARACTER ch)
{
	if (!ch->CanDoAction() || ch->GetDungeon())
	{
		return -1;
	}

	CCollectorCache * ch_Cache = GetCollectorCache(ch->GetPlayerID());
	if (!ch_Cache)
	{
		return 0;
	}

	// First, we need to update whole records status (disabling)
	ch->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleUpdateAllPlayerStatus 0");

	// Then, let's send notifcation to all cached peers
	// If one does not exists on map, we are erasing him from collector interface
	for (const auto & p_name : ch_Cache->GetPlayerList())
	{
		LPCHARACTER peer = CHARACTER_MANAGER::instance().FindPC(p_name.c_str());
		if (!peer || peer->GetMapIndex() != ch->GetMapIndex())
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleErasePlayer %s", p_name.c_str());
		}
		else
		{
			peer->RegisterNotificationCache(ch->GetPlayerID(), get_dword_time());
			peer->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleSendNotification");
		}
	}

	return 1;
}

void GetNotificationAnswer(LPCHARACTER ch)
{
	CCollectorCache * ch_Cache = GetCollectorCache(ch->GetNotificationCache());
	if (!ch_Cache)
	{
		return;
	}

	const time_t & dwSentTime = ch->GetNotificationSentTime();
	LPCHARACTER collector = CHARACTER_MANAGER::instance().FindByPID(ch->GetNotificationCache());
	if (collector)
	{
		time_t iAnswTime = get_dword_time() - dwSentTime;
		if (iAnswTime < NOTIFICATION_DELAY)
		{
			collector->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleUpdatePlayerStatus %s 2", ch->GetName());
			collector->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("NOTIFICATION_SENDER_BREAK_DELAY %s %u"), ch->GetName(), iAnswTime);
		}
		else
		{
			collector->ChatPacket(CHAT_TYPE_COMMAND, "GMIdleUpdatePlayerStatus %s 1", ch->GetName());
		}
	}

	// Clearing cache for this PC
	ch->RegisterNotificationCache(0, 0);
}

VPlayerList GetPlayerListByMapIndex(const long & lMapIndex)
{
	VPlayerList ret_vec;
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

	std::for_each(c_ref_set.begin(), c_ref_set.end(), [&lMapIndex, &ret_vec] (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		// Searching by MapIndex, skipping dungeons' attenders, implementors
		if (d->GetCharacter()->GetMapIndex() == lMapIndex && !d->GetCharacter()->GetDungeon() && d->GetCharacter()->GetGMLevel() == GM_PLAYER)
		{
			ret_vec.emplace_back(d->GetCharacter()->GetName());
		}
	});

	return ret_vec;
}

// Cache - Class Functions
CCollectorCache::CCollectorCache(const VPlayerList & _v_player_list) : v_player_list(_v_player_list)
{}

CCollectorCache::CCollectorCache(VPlayerList && _v_player_list) : v_player_list(std::move(_v_player_list))
{}

// Cache - NameSpace Functions
void CacheCollector(LPCHARACTER ch, VPlayerList && v_player_list)
{
	CCollectorCache * ch_Cache = GetCollectorCache(ch->GetPlayerID());
	if (!ch_Cache)
	{
		m_collector_cache.insert(std::make_pair(ch->GetPlayerID(), new CCollectorCache(std::move(v_player_list))));
	}
	else
	{
		ch_Cache->UpdateCache(std::move(v_player_list));
	}
}

CCollectorCache * GetCollectorCache(const DWORD & pid)
{
	auto fIt = m_collector_cache.find(pid);
	if (fIt != m_collector_cache.end())
	{
		return fIt->second;
	}
	else
	{
		return nullptr;
	}
}

void EraseCacheCollector(const DWORD & pid)
{
	auto fIt = m_collector_cache.find(pid);
	if (fIt != m_collector_cache.end())
	{
		delete fIt->second;
		m_collector_cache.erase(fIt);
	}
}

void CleanUp()
{
	for (auto & element : m_collector_cache)
	{
		delete element.second;
	}

	m_collector_cache.clear();
}
}
#endif

