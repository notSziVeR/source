#pragma once
#include "stdafx.h"

#ifdef __ENABLE_BIOLOG_SYSTEM__
#include "../../common/tables.h"
#include "affect.h"

#include "Parser.hpp"

class CBiologSystem {
	const DWORD BIOLOG_CHANCE_ITEM = 71035;
	const DWORD BIOLOG_TIME_ITEM = 172001;
	const DWORD BIOLOG_RESET_ITEM = 27105;

	const int BIOLOG_AFF_TIME = 60 * 60 * 60 * 365;

	public:
		enum ECGPackets {
			CG_BIOLOG_OPEN,
			CG_BIOLOG_SEND,
			CG_BIOLOG_TIMER,
		};

		enum EGCPackets {
			GC_BIOLOG_OPEN,
			GC_BIOLOG_ALERT,
			GC_BIOLOG_CLOSE,
		};

		enum EStates {
			BIOLOG_STATE_MISSION,
			BIOLOG_STATE_SELECTOR,
		};

	public:
		CBiologSystem(LPCHARACTER m_pkChar);
		virtual ~CBiologSystem() = default;

		void OnDestroy();

		// update functions
		void	FinishMission();

		// general functions
		void	Broadcast();
		void	BroadcastAffects(BiologCollection::BiologSet::biolog_reward_collection_t reward);

		void	CollectItem(bool bAdditionalChance, bool bAdditionalTime);

		bool	RequestReward(BYTE bMission);
		void	RequestAffect(uint8_t key);

		void	ResetAffect(uint8_t key);

		bool	GetBiologItemByMobVnum(LPCHARACTER pkKiller, WORD MonsterVnum, DWORD& ItemVnum, BYTE& bChance);
		
		// timer functions
		void	ActiveAlert(bool bReminder);
		void	BroadcastAlert();

	private:
		LPCHARACTER m_pkChar;
		std::string s_current_biolog_reminder;
};

class CBiologSystemManager : public singleton<CBiologSystemManager> {
	public:
		CBiologSystemManager() = default;
		virtual ~CBiologSystemManager() = default;
	
	public:
		void Create();
		void Clear();
		bool Load(const std::string& fileName);
	
		void BroadcastSets(LPCHARACTER ch);
		const BiologCollection::BiologSet* FindBiologSet(const uint8_t id) const;
		const bool LastMission(const uint8_t id) { return id == sets_.size(); }

	protected:
		std::map<uint8_t, BiologCollection::BiologSet> sets_;
};
#endif
