#pragma once
#ifdef __DUNGEON_INFO_ENABLE__

class CDungeonInfoRankingManager : public singleton<CDungeonInfoRankingManager>
{
	struct SRankingElement
	{
		struct sRecord
		{
			TPacketGDDIRanking tElement;

			sRecord() : tElement{}, bIsValid{false}
			{}

			sRecord(const TPacketGDDIRanking & tElement_) : tElement{tElement_}, bIsValid{true}
			{}

			bool operator<(const sRecord & r);
			bool operator<(const TPacketGDDIRanking & r);
			bool operator>(const TPacketGDDIRanking & r);

			bool IsValid() const { return bIsValid; }

			private:
				bool bIsValid;
		};

		using TArrayValues = std::array<sRecord, static_cast<int>(EDIEnums::DUNGEON_INFO_RANKING_MAX)>;

		public:
			SRankingElement() { a_rankings_record.fill(sRecord{}); }
			SRankingElement(const EDIHighScoreTypes & eType_) : eType(eType_) { a_rankings_record.fill(sRecord{}); }

			void Flush();
			void Fetch(const std::string & sName);
			TArrayValues & GetArray() { return a_rankings_record; }
			const EDIHighScoreTypes & GetType() { return eType; }
			bool IsNewHighScore(const TPacketGDDIRanking * pPacket);
			void ApplyHighScore(const TPacketGDDIRanking * pPacket);
			void BroadcastArray(CPeer * pPeer);

		private:
			TArrayValues a_rankings_record;
			EDIHighScoreTypes eType;
	};

	public:
		CDungeonInfoRankingManager();
		virtual ~CDungeonInfoRankingManager() {};

	public:
		void RecvRankingUpdate(const TPacketGDDIRanking * pPacket);
		void RecvRankingConfig(const std::string & sName, CPeer * pPeer = nullptr);
		void BroadcastRankingUpdate(CPeer * peer = nullptr, const std::string & sKey = "", const EDIHighScoreTypes & eType = EDIHighScoreTypes::MAX_NUM);

		//
		void Flush(bool bSkipUpdate = false);

		friend bool operator<(const TPacketGDDIRanking & l, const SRankingElement::sRecord & r);
		friend bool operator>(const TPacketGDDIRanking & l, const SRankingElement::sRecord & r);

	private:
		SRankingElement * GetRankingElement(const std::string & sKey, const EDIHighScoreTypes & eType = EDIHighScoreTypes::MAX_NUM);
		void BroadcastRankingWithElement(CPeer * pPeer, const std::string & sKey, const EDIHighScoreTypes & eType);

	private:
		bool bInit;
		time_t ttNextUpdate;
		std::map<std::string, std::array<SRankingElement, static_cast<int>(EDIHighScoreTypes::MAX_NUM)>> m_highscores;
};
#endif

