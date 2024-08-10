#pragma once
#include "stdafx.h"
#ifdef __ENABLE_ATTENDANCE_EVENT__
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../../libxml/Types.hpp"
#include "../../libxml/xml.hpp"

// Public Declarations
namespace AttendanceNS
{
	using EMonths = boost::date_time::months_of_year;

	struct SReward
	{
		DWORD dwItemVnum;
		CountType wItemCount;
	};
};

class CAttendanceRewardManager : public singleton<CAttendanceRewardManager>
{
	using sAttPool = std::pair<AttendanceNS::SReward, std::vector<AttendanceNS::SReward>>;

	public:
		CAttendanceRewardManager() = default;
		virtual ~CAttendanceRewardManager() = default;

	public:
		void Create();
		void Clear();
		bool Load(const std::string& name);

	public:
		void RegisterPool(const xml::Node* root);
		void ParseDays(const xml::Node* root, sAttPool& rRewards);

	public:
		void Broadcast(LPCHARACTER ch, bool bClear = false);
		void CollectReward(LPCHARACTER ch);

	private:
		AttendanceNS::EMonths GetCurrentMonth() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().month().as_enum(); }
		DWORD GetCurrentYear() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().year(); }
		BYTE GetCurrentDay() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().day().as_number(); }
		
		bool IsCollected(LPCHARACTER ch, const int iNum = -1);
		bool CalculateTotalCollected(LPCHARACTER ch);

	private:
		std::map<AttendanceNS::EMonths, sAttPool> m_att_pool;
};
#endif
