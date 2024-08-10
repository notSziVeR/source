#include "stdafx.h"
#ifdef __HIT_TRACKER_ENABLE__
#include <iostream>
#include <fstream>

#include "utils.h"
#include "char.h"
#include "EventFunctionHandler.hpp"

void CHARACTER::RegisterHitTrackingInfo(const std::string & sEventName, const std::string & sTestName, const time_t & ttEventDelay)
{
	// Saving data
	sTrackingEventName = sEventName;
	sTrackingTestName = sTestName;
	ttTrackingEventDelay = ttEventDelay;

	// Clearing logs
	iHitCount = 0;

	// Setting trigger
	bTrackingTrigger = true;
}

void CHARACTER::StartHitTracking()
{
	if (bTrackingTrigger && !CEventFunctionHandler::instance().FindEvent(sTrackingEventName))
		CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl *) {this->StopHitTracking();}, sTrackingEventName, ttTrackingEventDelay);
}

void CHARACTER::RegisterPCDamage()
{
	if (bTrackingTrigger)
	{
		iHitCount++;
	}
}

void CHARACTER::StopHitTracking()
{
	bTrackingTrigger = false;

	std::ofstream logFile("HitTracker.log", std::ios_base::app);
	if (logFile.is_open())
	{
		logFile << "Tracking info for player: " << GetName() << "." << std::endl;
		logFile << "Test name: " << sTrackingTestName << ", Test time: " << ttTrackingEventDelay << "s" << std::endl;
		logFile << "Hit count registered during the test: " << iHitCount << "\n" << std::endl;
	}
	else
	{
		std::cerr << "Could not open log file." << std::endl;
	}

	std::cerr << "\nHit Tracking Test: " << sTrackingTestName << " has been done! Check log file to inspect the result." << std::endl;
}

static void StartHitRecordSet(LPCHARACTER ch, const std::string & sEventName, const std::string & sTestName, const time_t & ttEventDelay, bool bRestart)
{
	if (bRestart)
	{
		CEventFunctionHandler::instance().RemoveEvent(sEventName);
	}

	ch->RegisterHitTrackingInfo(sEventName, sTestName, ttEventDelay);
	ch->ChatPacket(CHAT_TYPE_INFO, "Tracking will proceed once you hit your first monster");
}

ACMD(do_hit_record_test)
{
	static const time_t ttStaticTrackTime = 60;

	char arg1[256], arg2[256], arg3[256];
	const char * line = two_arguments(argument, arg1, sizeof(arg1), arg2, sizeof(arg2));

	one_argument(line, arg3, sizeof(arg3));

	std::string sTestName = "";
	std::string sType = "";
	time_t ttTrackTime = 0;

	if (!*arg1 || !*arg2)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "You must provided event name and test option. Sample: /hit_record_test TestName start/restart TestDelay");
		return;
	}

	sTestName = arg1;
	sType = arg2;

	if (!*arg3)
	{
		ttTrackTime = ttStaticTrackTime;
	}
	else
	{
		ttTrackTime = atoi(arg3);
	}

	std::string sEventName = "HIT_RECORD_TEST_" + std::to_string(ch->GetPlayerID());

	if (sType == "start")
	{
		if (CEventFunctionHandler::instance().FindEvent(sEventName))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Hit record tracker is already on run.");
			ch->ChatPacket(CHAT_TYPE_INFO, "Type: /hit_record_test restart to rerun test.");
			return;
		}

		StartHitRecordSet(ch, sEventName, sTestName, ttTrackTime, false);
	}
	else if (sType == "restart")
	{
		if (!CEventFunctionHandler::instance().FindEvent(sEventName))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "You haven't started hit record tracker on this PC.");
			return;
		}

		StartHitRecordSet(ch, sEventName, sTestName, ttTrackTime, true);
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "No flags provided. Sample: /hit_record_test TestName start/restart TestDelay");
	}
}
#endif

