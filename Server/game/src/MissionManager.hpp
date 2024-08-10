#pragma once

#include "stdafx.h"
#include "../../libxml/Types.hpp"
#include "../../libxml/xml.hpp"

#ifdef __ENABLE_MISSION_MANAGER__
// Public declarations
namespace MissionNS
{
	struct SReward
	{
		DWORD dwItemVnum;
		CountType wItemCount;
	};

	struct SObjective
	{
		DWORD dwObjectiveVnum;
		uint64_t sObjectiveCount;

		bool operator==(const SObjective& rObj)
		{
			return (dwObjectiveVnum == rObj.dwObjectiveVnum);
		}
	};
};

class CMissionManager : public singleton<CMissionManager>
{
	using SRewards = std::vector<MissionNS::SReward>;

	struct SMissionObject
	{
		int iTaskNum;
		MissionNS::SObjective sObjective;
		SRewards v_rewards;

		SMissionObject() : iTaskNum(-1), sObjective{}, v_rewards{}
		{}

		SMissionObject(const int _iTaskNum, const MissionNS::SObjective& _rObjective)
			: iTaskNum{ _iTaskNum }, sObjective{ _rObjective }, v_rewards{}
		{}

		// Operators
		bool operator==(const SMissionObject& rObj)
		{
			return (sObjective == rObj.sObjective);
		}

		bool operator==(const MissionNS::SObjective& rObj)
		{
			return (sObjective == rObj);
		}

		// Functions
		void AddReward(const MissionNS::SReward& rReward) { v_rewards.push_back(rReward); }
		BYTE GetTargetPerc(LPCHARACTER ch, const DWORD& dwVnum) const;
		void GiveReward(LPCHARACTER ch);
		void UpdateObjective(LPCHARACTER ch, const DWORD& dwVnum);
		void ClearObjective(LPCHARACTER ch, const DWORD& dwVnum);
	};

	using SQuestsList = std::vector<SMissionObject>;
	using sMissionsPool = std::pair<time_t, SQuestsList>;

	public:
		CMissionManager() = default;
		virtual ~CMissionManager() = default;

	public:
		void Create();
		bool Load(const std::string& filename);
		void Clear();

	public:
		void RegisterPool(const xml::Node* root);
		void ParseMissions(const xml::Node* root, SQuestsList& rObjective);
		void ParseRewards(const xml::Node* root, MissionNS::SReward& rewards);

	public:
		void TriggerEvent(LPCHARACTER ch, const MissionNS::SObjective& eType);
		void BroadcastMission(LPCHARACTER ch, bool bClear = false, const DWORD iElement = 0, const int iNum = -1);
		bool RegisterMission(LPCHARACTER ch, const DWORD& dwVnum);

	private:
		bool GetPoolByTask(const DWORD& dwVnum, SQuestsList** rRef);
		bool HasEnabled(LPCHARACTER ch);
		bool CalculateTotalProgress(const SQuestsList& rPool, LPCHARACTER ch, const DWORD& dwVnum);

	private:
		std::map<DWORD, sMissionsPool> m_task_pool;
};
#endif
