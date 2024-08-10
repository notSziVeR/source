#pragma once
#include <set>
#include <array>

#include "EventFunctionHandler.hpp"
#include "char_manager.h"

class CDungeon;
class CHARACTER;

using LTranslationFunction = std::function<const std::string &(LPCHARACTER)>;

namespace ShipDefendHelper
{
	enum class EShipDungeonStateEnum : int
	{
		STATE_INIT,
		WAVE_1,
		WAVE_2,
		WAVE_3,
		WAVE_4,
		HYDRA_1,
		HYDRA_2,
		HYDRA_3,
		HYDRA_4,
		TREASURE_BOX,
		END,
	};

	extern DWORD ALLIED_MAST_VNUM;
	extern DWORD ENEMY_HYDRA_VNUM;
	extern DWORD TREASURE_VNUM;
	extern int FIRST_WAVE_COUNT;
	extern DWORD ENEMY_GUARD_GROUP_VNUM;
	extern std::unordered_set<DWORD> s_enemy_guard;
	extern int ENEMY_HYDRA_X;
	extern int ENEMY_HYDRA_Y;
	extern int ALLIED_MAST_X;
	extern int ALLIED_MAST_Y;
	extern int ENEMY_INVERTED_X;
	extern int ENEMY_INVERTED_Y;
	extern time_t TIMEOUT;
	extern time_t LOGOUT_TIME;

	extern void Initialize();
}

class CShipDefendDungeon
{
	public:
		CShipDefendDungeon(const long & _dwMapIndex, LPDUNGEON _pDungeon);
		~CShipDefendDungeon();

	public:
		void MoveInitialState();
		void BroadcastInitStatus(LPCHARACTER ch);
		void RegisterHitRecord(LPCHARACTER pAttacker, LPCHARACTER pVictim);
		bool FindFieldMonster(LPCHARACTER pMonster);
		void KillMonsterByVnum(const DWORD & dwVnum);
		bool IsEnded() const { return bEnd; }
		LPDUNGEON GetDungeon() const { return pDungeon; }

	private:
		LPCHARACTER GetAlliedMast();
		void WipeAllMonsters();
		LPCHARACTER SpawnMonster(const DWORD & dwVnum, const int & x, const int & y, const int & dir = 0);
		void SpawnMonster(const DWORD & dwVnum, LPCHARACTER pVictim);
		void MoveState();
		void BroadcastAllieStatus(LPCHARACTER pAllied);
		void BroadcastMessage(const std::string & s_message);
		void RegisterMoveTimeEvent(const size_t & t_delay);
		void WipeMonster(LPCHARACTER pMonster);
		template <typename F>
		void RegisterEvent(F func, const std::string & s_event_name, const size_t & t_delay);
		template <typename T>
		size_t GetMonsterCountByGroup(const T & dwGroup);

	private:
		std::set<DWORD> s_monster_list;
		std::set<std::string> s_event_collector;
		long dwMapIndex;
		LPDUNGEON pDungeon;
		int iState;
		bool bEnd;
};

// Wrapper for event handler
template <typename F>
void CShipDefendDungeon::RegisterEvent(F func, const std::string & s_event_name, const size_t & t_delay)
{
	CEventFunctionHandler::instance().AddEvent(func, s_event_name, t_delay);
	s_event_collector.insert(s_event_name);
}

template <typename T>
size_t CShipDefendDungeon::GetMonsterCountByGroup(const T & dwGroup)
{
	size_t count = 0;
	for (const auto & dwVnum : dwGroup)
	{
		for (auto it = s_monster_list.begin(); it != s_monster_list.end(); ++it)
		{
			auto pMonster = CHARACTER_MANAGER::instance().Find(*it);
			if (pMonster && !pMonster->IsPC() && pMonster->GetMapIndex() == dwMapIndex && pMonster->GetRaceNum() == dwVnum)
				count++;
		}
	}

	return count;
}

