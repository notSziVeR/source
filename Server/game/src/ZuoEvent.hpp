#pragma once
#ifdef __ZUO_EVENT_SYSTEM_ENABLE__
#include <unordered_set>
#include <unordered_map>

enum class EEventType : int
{
	TYPE_STOP,
	TYPE_START,
};

enum class EMonsterType : int
{
	MONSTER_UNKNOWN,
	MONSTER_BOSS,
	MONSTER_STONE,
};

enum class EBroadcastType : int
{
	BROADCAST_BOSS,
	BROADCAST_STONE,
	BROADCAST_ATTENDERS,
	BROADCAST_TIME,
	BROADCAST_ALL
};

class CZuoEventManager : public singleton <CZuoEventManager>
{
	public:
		CZuoEventManager();
		virtual ~CZuoEventManager();

	public:
		void ChangeEventStatus(const int & iStatus = -1);
		void SpawnMonster(const DWORD & dwVnum, const WORD & wCount);
		void DeadMonster(LPCHARACTER ch);
		void RegisterNewPlayer(LPCHARACTER ch);
		void UnregisterPlayer(LPCHARACTER ch);
		void BroadcastEventInfo(const EBroadcastType & eBroadcastType, std::unordered_set<LPCHARACTER> s_residents = {});

		const EEventType & GetEventStatus() { return iEventStatus; }
		const BYTE & GetMinimalRequiredLevel() { return bMinimalEntryLevel; }
		const long & GetMapXWarp() { return lEntryX; }
		const long & GetMapYWarp() { return lEntryY; }

		static constexpr long GetEventMapIndex() { return 82; } // lEventMapIndex = 82

	private:
		EMonsterType GetMonsterType(LPCHARACTER ch);
		EMonsterType GetMonsterType(const DWORD & dwVnum);
		void Clear();
		bool FindMapResident(std::unordered_set<LPCHARACTER> & s_residents);
		void KillAll();

	private:
		const BYTE bMinimalEntryLevel = 15;
		const long lEntryX = 1024000, lEntryY = 1664000;

		EEventType iEventStatus;
		size_t st_max_player_count;
		time_t tt_start_time;
		std::unordered_map<DWORD, bool> us_player_count;
		std::unordered_map<int, std::unordered_set<DWORD>> us_monster_count;
		std::unordered_map<int, std::unordered_set<DWORD>> us_dead_monster_count;
};
#endif

