#include "stdafx.h"
#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
#include "char.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "p2p.h"
#include "utils.h"
#include "EventFunctionHandler.hpp"
#include "TechnicalMaintenance.hpp"

#include <boost/algorithm/string/replace.hpp>

extern void Shutdown(int iSec);

// Alert delay (counting from the end of the timeout)
static const int ALERT_TIME = 600;
static const int DELAYED_EMERGENCY_REQUEST = 3;

CTechnicalMaintenance::CTechnicalMaintenance() : sMaintenanceName(""), sMaintenanceTime(""), ttMaintenanceTime(0)
{
	// Delayed request
	CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl *) { RequestMaintenanceInfo(); }, "EMERGENCY_MAINTENANCE_REQUEST", DELAYED_EMERGENCY_REQUEST);
}

CTechnicalMaintenance::~CTechnicalMaintenance()
{}

void CTechnicalMaintenance::Destroy()
{
	// Main Event
	CEventFunctionHandler::instance().RemoveEvent(sMaintenanceName);

	// Alerter
	CEventFunctionHandler::instance().RemoveEvent(sMaintenanceName + "_ALERT");
}

void CTechnicalMaintenance::RegisterMaintenance(const std::string & _sMaintenanceName, const time_t & _ttMaintenanceTime, bool bBroadcast)
{
	if (FindMaintenance(_sMaintenanceName))
	{
		/*
		std::string _sMaintenanceName_tmp(_sMaintenanceName);
		boost::replace_all(_sMaintenanceName_tmp, "_", " ");
		sys_err("Trying to append maintenance when one already exists! Name: %s, Time: %s, Broadcast: %d", _sMaintenanceName_tmp.c_str(),
			GetFullDateFromTime(_ttMaintenanceTime+get_global_time()).c_str(), bBroadcast);
		*/
		return;
	}

	ttMaintenanceTime = _ttMaintenanceTime + get_global_time();
	sMaintenanceName = _sMaintenanceName;
	sMaintenanceTime = GetFullDateFromTime(ttMaintenanceTime, false);

	// Shutdown timer only for broadcaster
	if (bBroadcast)
	{
		CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl *)
		{
			TPacketGGShutdown p;
			p.bHeader = HEADER_GG_SHUTDOWN;
			P2P_MANAGER::instance().Send(&p, sizeof(p));

			Shutdown(10);
		},
		sMaintenanceName, _ttMaintenanceTime);
	}

	// Let's push maintenance's reminder 10 min before it comes
	if (ALERT_TIME < _ttMaintenanceTime)
	{
		CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl *)
		{
			BroadcastAlert();
		},
		(sMaintenanceName + "_ALERT"), _ttMaintenanceTime - ALERT_TIME);
	}

	// Broadcasting to other peers
	if (bBroadcast)
	{
		TPacketGGMaintenance p;
		memset(&p, 0, sizeof(p));
		p.bHeader = HEADER_GG_MAINTENANCE;
		p.subHeader = SUBHEADER_MAINTENANCE_APPEND;
		p.ttMaintenanceTime = _ttMaintenanceTime;
		strlcpy(p.sMaintenanceName, sMaintenanceName.c_str(), sizeof(p.sMaintenanceName));
		P2P_MANAGER::instance().Send(&p, sizeof(p));
	}

	// Sending update to everyone
	BroadcastMaintenanceInfo();
}

void CTechnicalMaintenance::DelayMaintenance(const std::string & _sMaintenanceName, const time_t & ttNewTime, bool bBroadcast)
{
	if (!FindMaintenance(_sMaintenanceName))
	{
		std::string _sMaintenanceName_tmp(_sMaintenanceName);
		boost::replace_all(_sMaintenanceName_tmp, "_", " ");
		sys_err("Trying to delay maintenance when there is no one! Name: %s, Time: %s, Broadcast: %d", _sMaintenanceName_tmp.c_str(),
				GetFullDateFromTime(ttMaintenanceTime + ttNewTime).c_str(), bBroadcast);
		return;
	}

	ttMaintenanceTime += ttNewTime;
	sMaintenanceTime = GetFullDateFromTime(ttMaintenanceTime, false);

	CEventFunctionHandler::instance().DelayEvent(_sMaintenanceName, ttMaintenanceTime - get_global_time());
	CEventFunctionHandler::instance().DelayEvent((_sMaintenanceName + "_ALERT"), ttMaintenanceTime - get_global_time());

	// Broadcasting to other peers
	if (bBroadcast)
	{
		TPacketGGMaintenance p;
		memset(&p, 0, sizeof(p));
		p.bHeader = HEADER_GG_MAINTENANCE;
		p.subHeader = SUBHEADER_MAINTENANCE_DELAY;
		p.ttMaintenanceTime = ttNewTime;
		strlcpy(p.sMaintenanceName, sMaintenanceName.c_str(), sizeof(p.sMaintenanceName));
		P2P_MANAGER::instance().Send(&p, sizeof(p));
	}

	// Sending update to everyone
	BroadcastMaintenanceInfo();
}

void CTechnicalMaintenance::CancelMaintenance(const std::string & _sMaintenanceName, bool bBroadcast)
{
	if (!FindMaintenance(_sMaintenanceName))
	{
		std::string _sMaintenanceName_tmp(_sMaintenanceName);
		boost::replace_all(_sMaintenanceName_tmp, "_", " ");
		sys_err("Trying to cancel maintenance when there is no one! Name: %s, Broadcast: %d", _sMaintenanceName_tmp.c_str(), bBroadcast);
		return;
	}

	ttMaintenanceTime = 0;
	sMaintenanceName.clear();
	sMaintenanceTime.clear();
	CEventFunctionHandler::instance().RemoveEvent(_sMaintenanceName);
	CEventFunctionHandler::instance().RemoveEvent((_sMaintenanceName + "_ALERT"));

	// Broadcasting to other peers
	if (bBroadcast)
	{
		TPacketGGMaintenance p;
		memset(&p, 0, sizeof(p));
		p.bHeader = HEADER_GG_MAINTENANCE;
		p.subHeader = SUBHEADER_MAINTENANCE_CANCEL;
		strlcpy(p.sMaintenanceName, _sMaintenanceName.c_str(), sizeof(p.sMaintenanceName));
		P2P_MANAGER::instance().Send(&p, sizeof(p));
	}

	// Sending update to everyone
	BroadcastMaintenanceInfo();
}

void CTechnicalMaintenance::BroadcastMaintenanceInfo(LPCHARACTER ch)
{
	if (!ch)
		std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this] (LPDESC d)
	{
		if (d->GetCharacter())
		{
			d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "TechnicalMaintenanceInfo %s %s", (sMaintenanceName.size() ? sMaintenanceName.c_str() : "CANCELED"), (sMaintenanceTime.size() ? sMaintenanceTime.c_str() : "0"));
		}
	});
	else
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "TechnicalMaintenanceInfo %s %s", (sMaintenanceName.size() ? sMaintenanceName.c_str() : "CANCELED"), (sMaintenanceTime.size() ? sMaintenanceTime.c_str() : "0"));
	}
}

void CTechnicalMaintenance::BroadcastAlert()
{
	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this] (LPDESC d)
	{
		if (d->GetCharacter())
		{
			d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("TECHNICAL_MAINTENANCE_ALTERT %d"), (ALERT_TIME / 60));
		}
	});
}

bool CTechnicalMaintenance::FindMaintenance(const std::string & _sMaintenanceName)
{
	return (sMaintenanceName == _sMaintenanceName);
}

// Emergency broadcast
void CTechnicalMaintenance::RequestMaintenanceInfo()
{
	TPacketGGMaintenance p;
	memset(&p, 0, sizeof(p));
	p.bHeader = HEADER_GG_MAINTENANCE;
	p.subHeader = SUBHEADER_MAINTENANCE_REQUEST;
	P2P_MANAGER::instance().Send(&p, sizeof(p));
}

void CTechnicalMaintenance::SendMaintenanceInfo(LPDESC d)
{
	if (ttMaintenanceTime >= get_global_time())
	{
		TPacketGGMaintenance p;
		memset(&p, 0, sizeof(p));
		p.bHeader = HEADER_GG_MAINTENANCE;
		p.subHeader = SUBHEADER_MAINTENANCE_APPEND;
		p.ttMaintenanceTime = ttMaintenanceTime - get_global_time();
		strlcpy(p.sMaintenanceName, sMaintenanceName.c_str(), sizeof(p.sMaintenanceName));
		d->Packet(&p, sizeof(TPacketGGMaintenance));
	}
}
#endif

