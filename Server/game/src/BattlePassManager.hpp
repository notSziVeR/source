#pragma once
#ifdef __BATTLE_PASS_ENABLE__

#include <boost/date_time/posix_time/posix_time.hpp>

// Public declarations
namespace BattlePassNS
{
	enum class EObjectives : int
	{
		TYPE_NONE,
		TYPE_KILL,
		TYPE_KILL_PC,
		TYPE_KILL_MONSTERS,
		TYPE_KILL_BOSSESS,
		TYPE_KILL_STONES,
		TYPE_USE,
		TYPE_REFINE,
		TYPE_REFINE_ALL,
		TYPE_FISHING,
		TYPE_FISHING_ALL,
		TYPE_MINING,
		TYPE_MINING_ALL,
		TYPE_DUNGEON,
		TYPE_CUBE,
		TYPE_CUBE_ALL,
		TYPE_ALL_GET_MONEY,
		TYPE_ALL_SPEND_MONEY,
		TYPE_NPC_GET_MONEY,
		TYPE_NPC_SPEND_MONEY,

		TYPE_NPC_SELL_ITEM,
		TYPE_NPC_SELL_ALL,

		TYPE_DESTROY_ITEM,
		TYPE_GET_GAYA,
	};

	enum class EDiffLevel : int
	{
		TYPE_NONE,
		TYPE_EASY,
		TYPE_MEDIUM,
		TYPE_HARD,
	};

	using EMonths = boost::date_time::months_of_year;

	struct SReward
	{
		DWORD dwItemVnum;
		CountType wItemCount;
	};

	struct SObjective
	{
		EObjectives eObjective;
		DWORD dwObjectiveVnum;
		uint64_t sObjectiveCount;

		bool operator==(const SObjective & rObj)
		{
			return (eObjective == rObj.eObjective && dwObjectiveVnum == rObj.dwObjectiveVnum);
		}
	};
}

class CBattlePassManager : public singleton<CBattlePassManager>
{
	using SRewards = std::vector<BattlePassNS::SReward>;

	struct SQuestObject
	{
		int iTaskNum;
		std::string sTaskName;
		std::string sTaskDesc;
		BattlePassNS::SObjective sObjective;
		SRewards v_rewards;

		SQuestObject() : iTaskNum{-1}, sTaskName{}, sTaskDesc{}, sObjective{}, v_rewards{}
		{}

		SQuestObject(const int _iTaskNum, const std::string & _sTaskName, const std::string & _sTaskDesc, const BattlePassNS::SObjective & _rObjective)
			: iTaskNum{_iTaskNum}, sTaskName{_sTaskName}, sTaskDesc{_sTaskDesc}, sObjective{_rObjective}, v_rewards{}
		{}

		// Operators
		bool operator==(const SQuestObject & rObj)
		{
			return (sObjective == rObj.sObjective);
		}

		bool operator==(const BattlePassNS::SObjective & rObj)
		{
			return (sObjective == rObj);
		}

		// Functions
		void AddReward(const BattlePassNS::SReward & rReward) { v_rewards.push_back(rReward); }
		BYTE GetTargetPerc(LPCHARACTER ch) const;
		void GiveReward(LPCHARACTER ch);
		void UpdateObjective(LPCHARACTER ch, const uint64_t& dwObjectiveCount);
		void ClearObjective(LPCHARACTER ch);
		void PrintData();
	};

	using sQuestsList = std::vector<SQuestObject>;
	using SQuestsPool = std::map<BattlePassNS::EMonths, std::pair<SRewards, sQuestsList>>;
	using VQuestsPtrPool = std::vector<SQuestObject*>;

	public:
		CBattlePassManager();
		virtual ~CBattlePassManager() {};

	public:
		void RegisterPool(const BattlePassNS::EDiffLevel & ePoolNum);
		void RegisterMonth(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth);
		void AddMonthReward(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const BattlePassNS::SReward & rReward);
		void AddNewTask(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const std::string & sTaskName, const std::string & sTaskDesc, const BattlePassNS::SObjective & rObjective);
		void AddTaskReward(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const size_t ttTaskNum, const BattlePassNS::SReward & rReward);
		void PrintTask(const BattlePassNS::EDiffLevel & ePoolNum, const BattlePassNS::EMonths & eMonth, const size_t ttTaskNum);

	public:
		void TriggerEvent(LPCHARACTER ch, const BattlePassNS::SObjective & eType);
		void BroadcastBattlePass(LPCHARACTER ch, BattlePassNS::EDiffLevel eDiffLevel = BattlePassNS::EDiffLevel::TYPE_NONE, bool bClear = false, const int iNum = -1);
		void CollectReward(LPCHARACTER ch);
		bool RegisterBattlePass(LPCHARACTER ch, const BattlePassNS::EDiffLevel & eDiffLevel);
		void RecvHighScoreUpdate(const TBattlePassHighScore * pPack);

	private:
		bool GetQuestObject(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, SQuestsPool::iterator & it);
		bool GetPoolByMonth(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, sQuestsList ** rRef);
		bool GetRewardByMonth(const BattlePassNS::EDiffLevel & eDiffLevel, const BattlePassNS::EMonths & eMonth, SRewards ** rRef);

	private:
		VQuestsPtrPool MisMatchEvents(const BattlePassNS::SObjective & eType, const BattlePassNS::EDiffLevel & eDiffLevel);
		bool HasEnabled(LPCHARACTER ch);
		BattlePassNS::EMonths GetCurrentMonth() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().month().as_enum(); }
		DWORD GetCurrentYear() { auto timeLocal = boost::posix_time::second_clock::local_time(); return timeLocal.date().year(); }
		bool CalculateTotalProgress(const sQuestsList & rPool, LPCHARACTER ch);
		void RegisterHighScore(LPCHARACTER ch, const BattlePassNS::EDiffLevel & eDiffLevel, const time_t ttTime);

	private:
		std::map<BattlePassNS::EDiffLevel, SQuestsPool> m_task_pool;
		std::map<BattlePassNS::EDiffLevel, std::vector<TBattlePassHighScore>> m_highscores;
};
#endif

