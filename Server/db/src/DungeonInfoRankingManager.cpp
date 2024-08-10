#include "stdafx.h"
#ifdef __DUNGEON_INFO_ENABLE__
#include "Main.h"
#include "DBManager.h"
#include "Peer.h"
#include "ClientManager.h"
#include "DungeonInfoRankingManager.hpp"

bool operator<(const TPacketGDDIRanking & l, const CDungeonInfoRankingManager::SRankingElement::sRecord & r)
{
	switch (static_cast<EDIHighScoreTypes>(l.iType))
	{
		case EDIHighScoreTypes::RUN_COUNT:
		case EDIHighScoreTypes::GREATEST_DAMAGE:
			return (l.lValue < r.tElement.lValue || !r.IsValid());
		case EDIHighScoreTypes::QUICKER_RUN:
			return (l.lValue > r.tElement.lValue || !r.IsValid());
		default:
			sys_err("Cannot process switch for max value!");
			return false;
	}

	return (l.lValue > r.tElement.lValue || !r.IsValid());
}

bool operator>(const TPacketGDDIRanking & l, const CDungeonInfoRankingManager::SRankingElement::sRecord & r)
{
	switch (static_cast<EDIHighScoreTypes>(r.tElement.iType))
	{
		case EDIHighScoreTypes::RUN_COUNT:
		case EDIHighScoreTypes::GREATEST_DAMAGE:
			return (r.tElement.lValue < l.lValue || !r.IsValid());
		case EDIHighScoreTypes::QUICKER_RUN:
			return (r.tElement.lValue > l.lValue || !r.IsValid());
		default:
			sys_err("Cannot process switch for max value!");
			return false;
	}

	return (r.tElement.lValue > l.lValue || !r.IsValid());
}

void CDungeonInfoRankingManager::SRankingElement::Flush()
{
	for (const auto & rStructure : a_rankings_record)
	{
		if (!rStructure.IsValid())
			continue;

		// sKey	iType	wPos	iLevel	sName	llValue
		std::string sQuery = "REPLACE INTO player"+std::string(GetTablePostfix())+".dungeon_info_ranking VALUE (";
		// sKey
		sQuery += "'" + std::string(rStructure.tElement.sKey) + "', ";
		// iType
		sQuery += "'" + std::to_string(rStructure.tElement.iType) + "', ";
		// wPos
		sQuery += "'" + std::to_string(rStructure.tElement.wPos) + "', ";
		// iLevel
		sQuery += "'" + std::to_string(rStructure.tElement.wLevel) + "', ";
		// sName
		sQuery += "'" + std::string(rStructure.tElement.sName) + "', ";
		// llValue
		sQuery += "'" + std::to_string(rStructure.tElement.lValue) + "')";

		std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sQuery.c_str()));
	}
}

void CDungeonInfoRankingManager::SRankingElement::Fetch(const std::string & sKey)
{
	std::string sQuery = std::string("SELECT iType, wPos, wLevel, sName, lValue FROM player")+std::string(GetTablePostfix())+std::string(".dungeon_info_ranking");
	sQuery += " WHERE sKey = '" + sKey + "' LIMIT " + std::to_string(static_cast<int>(EDIEnums::DUNGEON_INFO_RANKING_MAX));
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sQuery.c_str()));

	// Fill array with default values
	a_rankings_record.fill(sRecord{});

	if (!msg || msg->Get()->uiNumRows == 0)
	{
		sys_log(0, "Not data for fetch for %s", sKey.c_str());
		return;
	}

	TPacketGDDIRanking tRec{};
	MYSQL_ROW row;
	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	int i = 0;
	while ((row = mysql_fetch_row(pRes)))
	{
		strlcpy(tRec.sKey, sKey.c_str(), sizeof(tRec.sKey));
		tRec.iType = atoi(row[0]);
		tRec.wPos = static_cast<WORD>(atoi(row[1]));
		tRec.wLevel = static_cast<WORD>(atoi(row[2]));
		strlcpy(tRec.sName, row[3], sizeof(tRec.sName));
		tRec.lValue = atoll(row[4]);

		a_rankings_record[i++] = sRecord{tRec};
	}

	sys_log(0, "%d values were fetched!", msg->Get()->uiNumRows);
}

bool CDungeonInfoRankingManager::SRankingElement::IsNewHighScore(const TPacketGDDIRanking * pPacket)
{
	sRecord & rRecord = a_rankings_record.back();
	if (!rRecord.IsValid())
		return true;

	return ((*pPacket) > rRecord);
}

void CDungeonInfoRankingManager::SRankingElement::ApplyHighScore(const TPacketGDDIRanking * pPacket)
{
	sRecord & rRecord = a_rankings_record.back();
	rRecord = *pPacket;

	std::sort(a_rankings_record.begin(), a_rankings_record.end());
}

bool CDungeonInfoRankingManager::SRankingElement::sRecord::operator<(const CDungeonInfoRankingManager::SRankingElement::sRecord & r)
{
	switch (static_cast<EDIHighScoreTypes>(tElement.iType))
	{
		case EDIHighScoreTypes::RUN_COUNT:
		case EDIHighScoreTypes::GREATEST_DAMAGE:
			return (tElement.lValue < r.tElement.lValue);
		case EDIHighScoreTypes::QUICKER_RUN:
			return (tElement.lValue > r.tElement.lValue);
		default:
			sys_err("Cannot process switch for max value!");
			return false;
	}

	return (tElement.lValue > r.tElement.lValue);
}

bool CDungeonInfoRankingManager::SRankingElement::sRecord::operator<(const TPacketGDDIRanking & r)
{
	switch (static_cast<EDIHighScoreTypes>(tElement.iType))
	{
		case EDIHighScoreTypes::RUN_COUNT:
		case EDIHighScoreTypes::GREATEST_DAMAGE:
			return (tElement.lValue < r.lValue || !IsValid());
		case EDIHighScoreTypes::QUICKER_RUN:
			return (tElement.lValue > r.lValue || !IsValid());
		default:
			sys_err("Cannot process switch for max value!");
			return false;
	}

	return tElement.lValue > r.lValue;
}

void CDungeonInfoRankingManager::SRankingElement::BroadcastArray(CPeer * pPeer)
{
	for (const auto & rRec : a_rankings_record)
	{
		if (!rRec.tElement.wLevel) // If level is set to zero it means no record is found
			break;

		if (pPeer)
		{
			pPeer->EncodeHeader(HEADER_DG_DUNGEONINFO_RANKING, 0, sizeof(TPacketGDDIRanking)); 
			pPeer->Encode(&(rRec.tElement), sizeof(TPacketGDDIRanking));
		}
		else
			CClientManager::instance().ForwardPacket(HEADER_DG_DUNGEONINFO_RANKING, &(rRec.tElement), sizeof(TPacketGDDIRanking));
	}
}

CDungeonInfoRankingManager::CDungeonInfoRankingManager() :  bInit{false}, ttNextUpdate{time(0) + static_cast<time_t>(EDIEnums::DUNGEON_INFO_RANKIGN_CACHE)}
{
}

void CDungeonInfoRankingManager::RecvRankingUpdate(const TPacketGDDIRanking * pPacket)
{
	if (!bInit)
	{
		sys_err("Rankings were not initialized just yet.");
		return;
	}

	auto fIt = GetRankingElement(pPacket->sKey, static_cast<EDIHighScoreTypes>(pPacket->iType));
	if (!fIt)
	{
		sys_err("RecvUpdate-> Invalid record for key: %s", pPacket->sKey);
		return;
	}

	// If one beats last record (the worst of the bests)
	if (fIt->IsNewHighScore(pPacket))
	{
		fIt->ApplyHighScore(pPacket);
		BroadcastRankingUpdate(nullptr, pPacket->sKey);

		sys_log(0, "New ranking update registered! Key: %s, Rank type: %d, New name: %s, New value: %lld", pPacket->sKey, pPacket->iType, pPacket->sName, pPacket->lValue);
	}
}

void CDungeonInfoRankingManager::RecvRankingConfig(const std::string & sName, CPeer * pPeer)
{
	if (GetRankingElement(sName))
	{
		// Ranking was loaded, fine
		// Let's send it now to peer
		BroadcastRankingUpdate(pPeer, sName);
		return;
	}

	m_highscores[sName] = {};
	sys_log(0, "New ranking registered! Rank key: %s", sName.c_str());

	// Fetch existing values
	for (unsigned int i = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); i <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++i)
		m_highscores[sName][i].Fetch(sName);

	// Now, send newly fetch data to processed peer
	BroadcastRankingUpdate(pPeer, sName);

	// Initialize ranking
	bInit = true;
}

void CDungeonInfoRankingManager::BroadcastRankingUpdate(CPeer * peer, const std::string & sKey, const EDIHighScoreTypes & eType)
{
	if (sKey.size())
	{
		if (eType < EDIHighScoreTypes::MAX_NUM)
			BroadcastRankingWithElement(peer, sKey, eType);
		else
		{
			for (unsigned int i = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); i <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++i)
				BroadcastRankingWithElement(peer, sKey, static_cast<EDIHighScoreTypes>(i));
		}
	}
	else
	{
		for (const auto & rPair : m_highscores)
		{
			const std::string & rStr = rPair.first;
			if (eType < EDIHighScoreTypes::MAX_NUM)
				BroadcastRankingWithElement(peer, rStr, eType);
			else
			{
				for (unsigned int i = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); i <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++i)
					BroadcastRankingWithElement(peer, rStr, static_cast<EDIHighScoreTypes>(i));
			}
		}
	}
}

void CDungeonInfoRankingManager::Flush(bool bSkipUpdate)
{
	if (!bSkipUpdate && ttNextUpdate > time(0))
		return;

	for (auto & rPair : m_highscores)
	{
		for (unsigned int i = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); i <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++i)
			rPair.second[i].Flush();
	}

	ttNextUpdate = time(0) + static_cast<time_t>(EDIEnums::DUNGEON_INFO_RANKIGN_CACHE);
}

CDungeonInfoRankingManager::SRankingElement * CDungeonInfoRankingManager::GetRankingElement(const std::string & sKey, const EDIHighScoreTypes & eType)
{
	auto fIt = m_highscores.find(sKey);
	if (fIt == m_highscores.end())
	{
		sys_err("Cannot find key %s for dungeon info ranking.", sKey.c_str());
		return nullptr;
	}

	return ((eType < EDIHighScoreTypes::MAX_NUM) ? &(fIt->second)[static_cast<int>(eType)] : nullptr);
}

void CDungeonInfoRankingManager::BroadcastRankingWithElement(CPeer * pPeer, const std::string & sKey, const EDIHighScoreTypes & eType)
{
	auto pRank = GetRankingElement(sKey, eType);
	if (!pRank)
		return;

	pRank->BroadcastArray(pPeer);
}
#endif

