#pragma once
#ifdef __DUNGEON_INFO_ENABLE__

namespace NSDungeonInfo
{
	extern const std::string sBroadcastCommandGlobal;
	extern const std::string sBroadcastCommandPersonal;
	extern const std::string sBroadcastCommandHighscore;

	extern std::map<std::string, const char *> m_translation_unit;
}

class CDungeonInfoManager : public singleton<CDungeonInfoManager>
{
	struct SDungeonInstance
	{
		public:
			// Constructors
			SDungeonInstance();
			SDungeonInstance(const DWORD & _dwMapIndex, const std::string & _sName, const std::string & _sInstanceName, const DWORD & _dwRaceFlag, const BYTE & _bPartyCount, const std::pair<WORD, WORD> & _pRequiredLevel, const DWORD & _dwPassItem, const time_t & _iDelay, const DWORD & _dwVictimVnum);

			// Methods
			const std::string & GetName() const { return sName; }
			const DWORD & GetMapIndex() const { return dwMapIndex; }
			void BroadcastDungeonData(LPCHARACTER ch);
			void JoinDungeon(LPCHARACTER ch);
			#ifdef __DUNGEON_RETURN_ENABLE__
			void ReJoinDungeon(LPCHARACTER ch);
			#endif
			bool CanJoinDungeon(LPCHARACTER ch);
			void FinishDungeon(LPCHARACTER ch);
			void ClearDelay(LPCHARACTER ch);
			DWORD GetVictimVnum() { return dwVictimVnum; }

			// Rankings
			void RegisterPersonalHighscore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue, LPCHARACTER pkVictim = nullptr);
			void RecvHighscoreRecord(const TPacketGDDIRanking & pPacket);

		private:
			time_t GetPersistingDelay(LPCHARACTER ch, bool bSubtract = false);
			void ApplyDelay(LPCHARACTER ch);
			void ApplyMapFlag(LPCHARACTER ch);
			void SetPersonalHighScore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue);
			int GetPersonalHighScore(LPCHARACTER ch, const EDIHighScoreTypes & eType);
			void RegisterGlobalHighscore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue);

		private:
			bool HIGHSCORE_RunCount(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue);
			bool HIGHSCORE_FastestRun(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue);
			bool HIGHSCORE_GreatestDamage(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue);

			bool HIGHSCORE_RunCount(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue);
			bool HIGHSCORE_FastestRun(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue);
			bool HIGHSCORE_GreatestDamage(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue);

		private:
			DWORD dwMapIndex;
			std::string sName;
			std::string sInstanceName;
			DWORD dwRaceFlag;
			BYTE bPartyCount;
			std::pair<WORD, WORD> pRequiredLevel;
			DWORD dwPassItem;
			time_t iDelay;
			time_t ttStartTime;
			DWORD dwVictimVnum;

			std::map<int, std::vector<TPacketGDDIRanking>> m_highscores;
			std::map<int, std::function<bool(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue)>> m_global_highscores_validators;
			std::map<int, std::function<bool(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)>> m_personal_highscores_validators;
	};

	public:
		CDungeonInfoManager() {};
		virtual ~CDungeonInfoManager() {}

	public:
		void RegisterNewDungeon(const DWORD & iKey, const DWORD & dwMapIndex, const std::string & sName, const std::string & sTitleName, const DWORD & dwRaceFlag, const BYTE & bPartyCount, const std::pair<WORD, WORD> & pRequiredLevel, const DWORD & dwPassItem, const time_t & iDelay, const DWORD & dwVictimVnum);
		void JoinDungeon(LPCHARACTER ch, const std::string & sName);
		#ifdef __DUNGEON_RETURN_ENABLE__
		void ReJoinDungeon(LPCHARACTER ch, const std::string & sName);
		#endif
		void SaveResult(LPCHARACTER ch, const DWORD & dwMapIndex);
		void BroadcastConfig(LPCHARACTER ch);
		void ClearDelay(LPCHARACTER ch, const std::string & dwMapIndex);

		// Rankings
		void BroadcastHighscoreRecord(LPCHARACTER ch, const DWORD & dwMapIndex, const EDIHighScoreTypes & eType, const long long & llValue, LPCHARACTER pkVictim = nullptr);
		void RecvHighscoreRecord(const TPacketGDDIRanking * pPacket);

		inline DWORD GetRealMapIndex(DWORD dwMapIndex) { return (dwMapIndex < 10000) ? dwMapIndex : dwMapIndex/10000; }

	private:
		SDungeonInstance * FindDungeonInstance(const std::string & sKey);
		SDungeonInstance * FindDungeonInstance(const DWORD & dwMapIndex);

	private:
		std::map<DWORD, SDungeonInstance> m_dungeon_instances;
};
#endif

