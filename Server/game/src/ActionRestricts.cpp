#include "stdafx.h"
#include "utils.h"
#include "char.h"
#include "log.h"
#include "ActionRestricts.h"

//#define __DEBUG_PRINT
#ifdef __DEBUG_PRINT
	#include <iostream>
	#include <boost/date_time/posix_time/posix_time.hpp>
	#include <boost/date_time/c_local_time_adjustor.hpp>

	using std::cerr;
	using std::endl;
#endif

CActionRestricts::CActionRestricts(LPCHARACTER ch) : m_lpChar(ch)
{
	memset(&counters, 0, sizeof(counters));
	memset(&timers, 0, sizeof(timers));
}

BYTE CActionRestricts::getterB(BYTE key, BYTE index)
{
	if (key >= RESTRICT_DROP_ITEM && key < MAX_ACTION_RESTRICT_NUM && (index == RESTRICT_TYPE_COUNT || index == RESTRICT_TYPE_TIME))
	{
		return eBlocksMap[key][index];
	}

	return 0;
}

bool CActionRestricts::AddCounter(const ERestrictions& restrict)
{
	DWORD actualTime = get_global_time();
	counters[restrict]++;

#ifdef __DEBUG_PRINT
	auto cur_time = boost::posix_time::second_clock::local_time();
	cerr << "Restriction Current time: " << cur_time << endl;
#endif

	if (actualTime > timers[restrict])
	{
		counters[restrict] = 0;
		timers[restrict] = get_global_time() + getterB(restrict, RESTRICT_TYPE_TIME);

		return true;
	}
	else
	{
		if (counters[restrict] >= getterB(restrict, RESTRICT_TYPE_COUNT))
		{
			if (counters[restrict] % 10 == 0) // Log only when +10 actions.
			{
				LogManager::instance().RestrictionsLog(m_lpChar->GetPlayerID(), static_cast<BYTE>(restrict), counters[restrict]);
			}

#ifdef __DEBUG_PRINT
			cerr << "Restriction Next possible action: " << GetFullDateFromTime(timers[restrict]) << endl;;
#endif

			m_lpChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You can't do it that fast. Please try again in %s"), GetFullDateFromTime(timers[restrict]).c_str());
			return false;
		}
	}

	return true;
}