#include "stdafx.h"
#include <set>

#include "char.h"
#include "desc.h"
#include "desc_manager.h"
#include "p2p.h"
#include "packet.h"
#include "EventFunctionHandler.hpp"
#include "EventManager.hpp"
#include "utils.h"

CEventManager::CEventManager()
{
	s_current_loop_event = "EVENT_MANAGER_LOOP_" + std::to_string(get_global_time());
	CEventFunctionHandler::instance().AddEvent([] ( const SArgumentSupportImpl * empty_arg) { CEventManager::instance().Update(); }, s_current_loop_event, ttEventUpdate);
}

CEventManager::~CEventManager()
{}

void CEventManager::Destroy()
{
	if (s_current_loop_event.size())
	{
		CEventFunctionHandler::instance().RemoveEvent(s_current_loop_event);
	}
}

// Interface
bool CEventManager::RegisterEvent(std::string && e_name, time_t && e_time)
{
	if (m_current_event.find(e_name) == m_current_event.end())
	{
		std::string s_eTime = GetFullDateFromTime(std::move(e_time), false);
		m_current_event.emplace(e_name, std::make_pair(std::move(e_time), s_eTime));
		BroadcastNewEvent(std::move(e_name), std::move(s_eTime), std::move(e_time));
		return true;
	}

	return false;
}

bool CEventManager::UnregisterEvent(std::string && e_name)
{
	if (m_current_event.find(e_name) != m_current_event.end())
	{
		m_current_event[e_name].first = 0;
		BroadcastNewEvent(std::move(e_name), "EMPTY", 0);
		return true;
	}

	return false;
}

void CEventManager::RefreshImplementorPanel(LPCHARACTER ch)
{
	for (const auto & m_element : m_current_event)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "EventManagerBroadcastStatus %s %d", m_element.first.c_str(), std::max((int) 0, int(m_element.second.first - get_global_time())));
	}
}

void CEventManager::SendEventList(LPCHARACTER ch, const std::string & e_name)
{
	if (e_name.size())
	{
		if (m_current_event.find(e_name) != m_current_event.end())
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "EventManagerUpdateEvent %s %s %d", e_name.c_str(), m_current_event[e_name].second.c_str(), (m_current_event[e_name].first >= get_global_time()));
		}
	}
	else
	{
		for (const auto & m_element : m_current_event)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, "EventManagerUpdateEvent %s %s %d", m_element.first.c_str(), m_element.second.second.c_str(), (m_element.second.first >= get_global_time()));
		}
	}
}

// Broadcasters
void CEventManager::BroadcastNewEvent(std::string && e_name, std::string && e_time, time_t && ttEndTime)
{
	// Sending update to players
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), [&e_name](LPDESC d)
	{
		if (d->GetCharacter())
		{
			CEventManager::instance().SendEventList(d->GetCharacter(), e_name);
		}
	});

	// Broadcasting to other peers
	TPacketGGEventInfo pack;
	memset(&pack, 0, sizeof(pack));
	pack.bHeader = HEADER_GG_EVENT_BROADCAST;
	pack.ttEndTime = ttEndTime;
	strlcpy(pack.sEventName, e_name.c_str(), sizeof(pack.sEventName));
	strlcpy(pack.sEventTime, e_time.c_str(), sizeof(pack.sEventTime));
	P2P_MANAGER::instance().Send(&pack, sizeof(pack));
}

void CEventManager::RecvEventPacket(const TPacketGGEventInfo * pack)
{
	// Setting up iterator and creating new object if not exists
	auto it = m_current_event.find(pack->sEventName);
	if (it == m_current_event.end())
	{
		std::tie(it, std::ignore) = m_current_event.emplace(pack->sEventName, std::pair<time_t, std::string>());
	}

	it->second = std::move(std::make_pair(std::move(pack->ttEndTime), std::move(pack->sEventTime)));

	// Sending update to players
	const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
	std::for_each(c_ref_set.begin(), c_ref_set.end(), [pack](LPDESC d)
	{
		if (d->GetCharacter())
		{
			CEventManager::instance().SendEventList(d->GetCharacter(), pack->sEventName);
		}
	});
}

// Updater
void CEventManager::Update()
{
	s_current_loop_event = "EVENT_MANAGER_LOOP_" + std::to_string(get_global_time());
	CEventFunctionHandler::instance().AddEvent([] ( const SArgumentSupportImpl * empty_arg) { CEventManager::instance().Update(); }, s_current_loop_event, ttEventUpdate);

	if (!m_current_event.size())
	{
		return;
	}

	// Collecting outdated events
	std::unordered_set<std::string> s_garbage;
	for (auto & m_element : m_current_event)
	{
		if (get_global_time() > m_element.second.first)
		{
			m_element.second.first = 0;
			s_garbage.insert(m_element.first);
		}
	}

	// Sending update to players if any garbage has been collected
	if (s_garbage.size())
	{
		const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();
		std::for_each(c_ref_set.begin(), c_ref_set.end(), [](LPDESC d)
		{
			if (d->GetCharacter())
			{
				CEventManager::instance().SendEventList(d->GetCharacter(), "");
			}
		});
	}

	// Wiping out junked events
	for (const auto & s_event : s_garbage)
	{
		m_current_event.erase(s_event);
	}
}

