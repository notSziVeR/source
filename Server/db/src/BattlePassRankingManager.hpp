#pragma once
#ifdef __BATTLE_PASS_ENABLE__

#include <boost/date_time/posix_time/posix_time.hpp>

namespace BattlePassNS
{
	enum class EDiffLevel : int
	{
		TYPE_NONE,
		TYPE_EASY,
		TYPE_MEDIUM,
		TYPE_HARD,
	};

	using EMonths = boost::date_time::months_of_year;
}

class CBattlePassRankingManager : public singleton<CBattlePassRankingManager>
{
	public:
		CBattlePassRankingManager();
		virtual ~CBattlePassRankingManager();

	public:
		void RecvGamePacket(CPeer * peer, const TBattlePassHighScore * pPack);
		void Flush();
		void Save();

	private:
		BattlePassNS::EMonths GetCurrentMonth() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().month().as_enum(); }
		DWORD GetCurrentYear() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().year(); }
		void BroadcastElement(CPeer * peer, const BattlePassNS::EDiffLevel & eLevel, const size_t iElement);

	private:
		std::map<BattlePassNS::EDiffLevel, std::vector<TBattlePassHighScore>> m_highscores;
		BattlePassNS::EMonths eCurrentMonth;
		DWORD dwCurrentYear;
		time_t ttNextTime;
};
#endif

