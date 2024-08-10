#include "stdafx.h"
#ifdef __DUNGEON_INFO_ENABLE__
#include "char.h"
#include "desc_client.h"
#include "utils.h"
#include "config.h"
#include "party.h"
#include "p2p.h"
#include "dungeon.h"
#include "DungeonInfoManager.hpp"

#include <boost/algorithm/string.hpp> // include Boost, a C++ library

// NameSpace
namespace NSDungeonInfo
{
	const std::string sBroadcastCommandGlobal("DungeonInfo_BroadcastGlobal %s %s %u %u %u %u %u %u");
	const std::string sBroadcastCommandPersonal("DungeonInfo_BroadcastPersonal %s %u %d %u");
	const std::string sBroadcastCommandHighscore("DungeonInfo_BroadcastHighscore %s %u %u %s %u %lld");

	std::map<std::string, const char *> m_translation_unit = {
	{"ALREADY_ON_DUNGEON", "DUNGEON_INFO_ON_DUNGEON"},
	{"TOO_LOW_LEVEL", "DUNGEON_INFO_TOO_LOW_LEVEL"},
	{"NO_PASS_ITEM", "DUNGEON_INFO_NO_PASS_ITEM"},
	{"PARTY_CONDITION_NOT_MATCHED", "DUNGEON_INFO_PARTY_CONDITION_NOT_MATCHED"},
	{"DELAY_NOT_MATCHED", "DUNGEON_INFO_DELAY_NOT_MATCHED"},
	};
}

// Structure
CDungeonInfoManager::SDungeonInstance::SDungeonInstance() : dwMapIndex{0}, sName{""}, sInstanceName{""}, dwRaceFlag{0}, bPartyCount{0}, pRequiredLevel{0, 0}, dwPassItem{0}, iDelay{0}, ttStartTime{get_global_time()}, dwVictimVnum{0}
{
	for (int eType = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); eType <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++eType)
		m_highscores[eType].resize(static_cast<int>(EDIEnums::DUNGEON_INFO_RANKING_MAX));

	// Validators
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::RUN_COUNT)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_RunCount(eType, llCurValue, llNewValue); };
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::QUICKER_RUN)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_FastestRun(eType, llCurValue, llNewValue); };
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_GreatestDamage(eType, llCurValue, llNewValue); };

	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::RUN_COUNT)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_RunCount(ch, eType, llValue); };
	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::QUICKER_RUN)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_FastestRun(ch, eType, llValue); };
	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_GreatestDamage(ch, eType, llValue); };
}

CDungeonInfoManager::SDungeonInstance::SDungeonInstance(const DWORD & _dwMapIndex, const std::string & _sName, const std::string & _sInstanceName, const DWORD & _dwRaceFlag, const BYTE & _bPartyCount, const std::pair<WORD, WORD> & _pRequiredLevel, const DWORD & _dwPassItem, const time_t & _iDelay, const DWORD & _dwVictimVnum) : 
dwMapIndex{_dwMapIndex}, sName{_sName}, sInstanceName{_sInstanceName}, dwRaceFlag{_dwRaceFlag}, bPartyCount{_bPartyCount}, pRequiredLevel{_pRequiredLevel}, dwPassItem{_dwPassItem}, iDelay{_iDelay}, ttStartTime{get_global_time()}, dwVictimVnum{_dwVictimVnum}
{
	for (int eType = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); eType <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++eType)
		m_highscores[eType].resize(static_cast<int>(EDIEnums::DUNGEON_INFO_RANKING_MAX));

	// Validators
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::RUN_COUNT)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_RunCount(eType, llCurValue, llNewValue); };
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::QUICKER_RUN)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_FastestRun(eType, llCurValue, llNewValue); };
	m_global_highscores_validators[static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE)] = [this] (const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue){ return HIGHSCORE_GreatestDamage(eType, llCurValue, llNewValue); };

	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::RUN_COUNT)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_RunCount(ch, eType, llValue); };
	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::QUICKER_RUN)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_FastestRun(ch, eType, llValue); };
	m_personal_highscores_validators[static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE)] = [this] (LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue){ return HIGHSCORE_GreatestDamage(ch, eType, llValue); };

	boost::replace_all(sName, " ", ""); // Don't let client misinterpret it
	boost::replace_all(sInstanceName, " ", "|");
}

void CDungeonInfoManager::SDungeonInstance::BroadcastDungeonData(LPCHARACTER ch)
{
	if (!ch)
		return;

	// Name -> RaceFlag, RequiredLevel, PassItem, Delay
	ch->ChatPacket(CHAT_TYPE_COMMAND, NSDungeonInfo::sBroadcastCommandGlobal.c_str(), sName.c_str(), sInstanceName.c_str(), dwRaceFlag, bPartyCount, pRequiredLevel.first, pRequiredLevel.second, dwPassItem, iDelay);

	// Name -> HighScore
	for (int eType = static_cast<int>(EDIHighScoreTypes::RUN_COUNT); eType <= static_cast<int>(EDIHighScoreTypes::GREATEST_DAMAGE); ++eType)
		ch->ChatPacket(CHAT_TYPE_COMMAND, NSDungeonInfo::sBroadcastCommandPersonal.c_str(), sName.c_str(), eType, GetPersonalHighScore(ch, static_cast<EDIHighScoreTypes>(eType)), GetPersistingDelay(ch, true));

	// Name -> HighScores (global)
	for (const auto & rPair : m_highscores)
	{
		for (const auto & rScore : rPair.second)
		{
			if (!rScore.wLevel)
				break;

			ch->ChatPacket(CHAT_TYPE_COMMAND, NSDungeonInfo::sBroadcastCommandHighscore.c_str(), sName.c_str(), rScore.iType, rScore.wPos, rScore.sName, rScore.wLevel, rScore.lValue);
		}
	}
}

void CDungeonInfoManager::SDungeonInstance::JoinDungeon(LPCHARACTER ch)
{
	if (!ch || !CanJoinDungeon(ch))
		return;

	// Remove pass item (if one exists)
	if (dwPassItem != 0)
		ch->RemoveSpecifyItem(dwPassItem, 1);

	// Apply delay&map flag
	if (!ch->GetParty())
	{
		ApplyDelay(ch);
		ApplyMapFlag(ch);
	}
	else
	{
		auto lApply = [this] (LPCHARACTER ch) { ApplyDelay(ch); ApplyMapFlag(ch); };
		ch->GetParty()->ForEachOnMapMember(lApply, ch->GetMapIndex());
	}

	// Set warp process
	LPDUNGEON pDungeon = CDungeonManager::instance().Create(dwMapIndex);
	if (!pDungeon)
	{
		#ifndef __CROSS_CHANNEL_DUNGEON_WARP__
			sys_err("Cross channel dungeon MUST be enable in order to use this feature.");
		#else
			// Send request to all peers if anyone has this map in pool
			// Bear in mind we got to save pid(s) as well
			TPacketGGCreateDungeonInstance p{};
			p.bHeader = HEADER_GG_CREATE_DUNGEON_INSTANCE;
			p.lMapIndex = dwMapIndex;
			p.bChannel = g_bChannel;
			p.bRequest = true;
			p.bCount = 1;

			if (ch->GetParty())
				p.bCount = ch->GetParty()->PopulatePartyPids(p.aPids);
			else
				p.aPids[0] = ch->GetPlayerID();

			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "A request from PID: %d regarding map %d has been sent over cores. Waiting for response.", ch->GetPlayerID(), dwMapIndex);
			return;
		#endif
	}

	if (ch->GetParty() && ch->GetParty()->GetLeaderPID() == ch->GetPlayerID())
		pDungeon->JumpParty(ch->GetParty(), ch->GetMapIndex(), 0, 0);
	else if (!ch->GetParty())
		pDungeon->Join(ch);
}

#ifdef __DUNGEON_RETURN_ENABLE__
void CDungeonInfoManager::SDungeonInstance::ReJoinDungeon(LPCHARACTER ch)
{
	if (!ch || !ch->CanWarp())
		return;

	// Waiting for response
	if (ch->isDungeonRejoinWaiting())
		return;

	auto pDungeon = CDungeonManager::instance().FindDungeonByPID(ch->GetPlayerID(), dwMapIndex);
	if (!pDungeon)
	{
		#ifndef __CROSS_CHANNEL_DUNGEON_WARP__
			sys_err("Cross channel dungeon MUST be enable in order to use this feature.");
		#else
			ch->setDungeonRejoinWaiting(true);

			TPacketGGCreateDungeonInstance p{};
			p.bHeader = HEADER_GG_REJOIN_DUNGEON;
			p.bRequest = true;
			p.aPids[0] = ch->GetPlayerID();

			P2P_MANAGER::instance().Send(&p, sizeof(TPacketGGCreateDungeonInstance));
			sys_log(0, "A request from PID: %d regarding map %d has been sent over cores. Waiting for response.", ch->GetPlayerID(), dwMapIndex);
			return;
		#endif
	}
	else
	{
		long x = 0, y = 0, map_index = 0;
		std::tie(x, y, map_index) = pDungeon->GetDungeonCoords(ch);
		ch->WarpSet(x, y, map_index);
	}
}
#endif

bool CDungeonInfoManager::SDungeonInstance::CanJoinDungeon(LPCHARACTER ch)
{
	using namespace NSDungeonInfo;
	if (!ch)
		return false;

	if (ch->GetDungeon() || !ch->CanDoAction())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["ALREADY_ON_DUNGEON"]));
		return false;
	}

	// Check if required level is matched
	auto lSearch = [this] (LPCHARACTER ch) { return (ch->GetLevel() >= pRequiredLevel.first && ch->GetLevel() <= pRequiredLevel.second);};
	bool bMatched = (!ch->GetParty()) ? (ch->GetLevel() >= pRequiredLevel.first && ch->GetLevel() <= pRequiredLevel.second) : ch->GetParty()->ForEachOnMapMemberBool(lSearch, ch->GetMapIndex());
	if (!bMatched)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["TOO_LOW_LEVEL"]));
		return false;
	}

	// Check if pass item is present
	if (dwPassItem != 0)
	{
		if (ch->CountSpecifyItem(dwPassItem) < 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["NO_PASS_ITEM"]));
			return false;
		}
	}

	// Check if party count is matched
	if (ch->GetParty() && ch->GetParty()->GetLeaderPID() != ch->GetPlayerID())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["PARTY_CONDITION_NOT_MATCHED"]));
		return false;
	}

	if (bPartyCount && (!ch->GetParty() || ch->GetParty()->GetNearMemberCount() < bPartyCount))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["PARTY_CONDITION_NOT_MATCHED"]));
		return false;
	}

	if (!bPartyCount && ch->GetParty())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["PARTY_CONDITION_NOT_MATCHED"]));
		return false;
	}

	// Check if delay is matched
	auto lSearch2 = [this] (LPCHARACTER ch_party) { return GetPersistingDelay(ch_party) < get_global_time();};
	bMatched = (!ch->GetParty()) ? GetPersistingDelay(ch) < get_global_time() : ch->GetParty()->ForEachOnMapMemberBool(lSearch2, ch->GetMapIndex());
	if (!bMatched)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT(m_translation_unit["DELAY_NOT_MATCHED"]));
		return false;
	}

	return true;
}

void CDungeonInfoManager::SDungeonInstance::FinishDungeon(LPCHARACTER ch)
{
	if (!ch)
		return;

	// Count will always we increased
	RegisterPersonalHighscore(ch, EDIHighScoreTypes::RUN_COUNT, GetPersonalHighScore(ch, EDIHighScoreTypes::RUN_COUNT)+1);

	// Quickest run is distinguished from delta
	RegisterPersonalHighscore(ch, EDIHighScoreTypes::QUICKER_RUN, (get_global_time() - ttStartTime));

	// Refresh
	BroadcastDungeonData(ch);
}

void CDungeonInfoManager::SDungeonInstance::ClearDelay(LPCHARACTER ch)
{
	if (!ch)
		return;

	std::string sFlag("dungeon_info.delay_" + std::to_string(dwMapIndex));
	ch->SetQuestFlag(sFlag, 0);

	// Refresh
	BroadcastDungeonData(ch);
}

void CDungeonInfoManager::SDungeonInstance::RegisterPersonalHighscore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue, LPCHARACTER pkVictim)
{
	if (!ch)
		return;

	// Invariant - Check if victim is presented and matches the pattern
	if (pkVictim && GetVictimVnum() && pkVictim->GetRaceNum() != GetVictimVnum())
		return;

	if (m_global_highscores_validators.find(static_cast<int>(eType)) == m_global_highscores_validators.end() ||
		m_personal_highscores_validators.find(static_cast<int>(eType)) == m_personal_highscores_validators.end())
	{
		sys_err("Validator was not register for following type: %d", static_cast<int>(eType));
		return;
	}

	// Checking if one beat his own record
	// if (!(m_personal_highscores_validators[static_cast<int>(eType)](ch, eType, llValue)))
	if (!(m_global_highscores_validators[static_cast<int>(eType)](eType, GetPersonalHighScore(ch, eType), llValue)))
		return;

	SetPersonalHighScore(ch, eType, llValue);

	// Validating if one can join highscores
	auto & rRec = m_highscores[static_cast<int>(eType)].back();

	// Checking if one beat global record
	if ((m_global_highscores_validators[static_cast<int>(eType)](eType, rRec.lValue, llValue)))
		RegisterGlobalHighscore(ch, eType, llValue);
}

void CDungeonInfoManager::SDungeonInstance::RecvHighscoreRecord(const TPacketGDDIRanking & pPacket)
{
	m_highscores[pPacket.iType][pPacket.wPos] = pPacket;
}

time_t CDungeonInfoManager::SDungeonInstance::GetPersistingDelay(LPCHARACTER ch, bool bSubtract)
{
	std::string sFlag("dungeon_info.delay_" + std::to_string(dwMapIndex));
	return (bSubtract) ? std::max<time_t>(ch->GetQuestFlag(sFlag)-get_global_time(), 0) : ch->GetQuestFlag(sFlag);
}

void CDungeonInfoManager::SDungeonInstance::ApplyDelay(LPCHARACTER ch)
{
	std::string sFlag("dungeon_info.delay_" + std::to_string(dwMapIndex));
	ch->SetQuestFlag(sFlag, get_global_time()+iDelay);
}

void CDungeonInfoManager::SDungeonInstance::ApplyMapFlag(LPCHARACTER ch)
{
	std::string sFlag("dungeon_info.map_" + std::to_string(dwMapIndex));
	ch->SetQuestFlag(sFlag, 1);
}

int CDungeonInfoManager::SDungeonInstance::GetPersonalHighScore(LPCHARACTER ch, const EDIHighScoreTypes & eType)
{
	std::string sFlag("dungeon_info.highscore_" + std::to_string(dwMapIndex) + "_" + std::to_string(static_cast<int>(eType)));
	return ch->GetQuestFlag(sFlag);
}

void CDungeonInfoManager::SDungeonInstance::SetPersonalHighScore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)
{
	std::string sFlag("dungeon_info.highscore_" + std::to_string(dwMapIndex) + "_" + std::to_string(static_cast<int>(eType)));
	ch->SetQuestFlag(sFlag, static_cast<int>(llValue));
}

void CDungeonInfoManager::SDungeonInstance::RegisterGlobalHighscore(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)
{
	TPacketGDDIRanking packet{};
	packet.iType = static_cast<int>(eType);
	strlcpy(packet.sKey, sName.c_str(), sizeof(packet.sKey));
	strlcpy(packet.sName, ch->GetName(), sizeof(packet.sName));
	packet.wLevel = ch->GetLevel();
	packet.lValue = llValue;

	db_clientdesc->DBPacket(HEADER_GD_DUNGEONINFO_RANKING, 0, &packet, sizeof(packet));
}

// Functors
bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_RunCount(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)
{
	return (llValue > GetPersonalHighScore(ch, eType) || !GetPersonalHighScore(ch, eType));
}

bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_FastestRun(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)
{
	return (llValue < GetPersonalHighScore(ch, eType) || !GetPersonalHighScore(ch, eType));
}

bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_GreatestDamage(LPCHARACTER ch, const EDIHighScoreTypes & eType, const long long & llValue)
{
	return (llValue > GetPersonalHighScore(ch, eType) || !GetPersonalHighScore(ch, eType));
}

bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_RunCount(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue)
{
	return (llNewValue > llCurValue || !llCurValue);
}

bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_FastestRun(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue)
{
	return (llNewValue < llCurValue || !llCurValue);
}

bool CDungeonInfoManager::SDungeonInstance::HIGHSCORE_GreatestDamage(const EDIHighScoreTypes & eType, const long long & llCurValue, const long long & llNewValue)
{
	return (llNewValue > llCurValue || !llCurValue);
}

// Class
void CDungeonInfoManager::RegisterNewDungeon(const DWORD & iKey, const DWORD & dwMapIndex, const std::string & sName, const std::string & sTitleName, const DWORD & dwRaceFlag, const BYTE & bPartyCount, const std::pair<WORD, WORD> & pRequiredLevel, const DWORD & dwPassItem, const time_t & iDelay, const DWORD & dwVictimVnum)
{
	m_dungeon_instances.emplace(iKey, SDungeonInstance(dwMapIndex, sName, sTitleName, dwRaceFlag, bPartyCount, pRequiredLevel, dwPassItem, iDelay, dwVictimVnum));
	sys_log(0, "The dungeon %s was registered! Data: MapIndex: %u, RaceFlag: %u, Required level: %u, Pass Item: %u, Delay between: %d, Victim vnum: %u, Start time: %u", sName.c_str(), dwMapIndex, bPartyCount, pRequiredLevel.first, dwPassItem, iDelay, dwVictimVnum, get_global_time());

	// Send DB packet in order to register config in there
	TPacketGDDIRankingBoot dbPack{};
	strlcpy(dbPack.sKey, sName.c_str(), sizeof(dbPack.sKey));
	db_clientdesc->DBPacket(HEADER_GD_DUNGEONINFO_RANKING_BOOT, 0, &dbPack, sizeof(dbPack));
}

void CDungeonInfoManager::JoinDungeon(LPCHARACTER ch, const std::string & sName)
{
	auto pInstance = FindDungeonInstance(sName);
	if (!pInstance)
		return;

	pInstance->JoinDungeon(ch);
}

#ifdef __DUNGEON_RETURN_ENABLE__
void CDungeonInfoManager::ReJoinDungeon(LPCHARACTER ch, const std::string & sName)
{
	auto pInstance = FindDungeonInstance(sName);
	if (!pInstance)
		return;

	pInstance->ReJoinDungeon(ch);
}
#endif

void CDungeonInfoManager::SaveResult(LPCHARACTER ch, const DWORD & dwMapIndex)
{
	auto pInstance = FindDungeonInstance(GetRealMapIndex(dwMapIndex));
	if (!pInstance)
		return;

	pInstance->FinishDungeon(ch);
}

void CDungeonInfoManager::BroadcastConfig(LPCHARACTER ch)
{
	if (!ch)
		return;

	for (auto & rPair : m_dungeon_instances)
		rPair.second.BroadcastDungeonData(ch);
}

void CDungeonInfoManager::ClearDelay(LPCHARACTER ch, const std::string & sKey)
{
	auto pInstance = FindDungeonInstance(sKey);
	if (!pInstance)
		return;

	pInstance->ClearDelay(ch);
}

void CDungeonInfoManager::BroadcastHighscoreRecord(LPCHARACTER ch, const DWORD & dwMapIndex, const EDIHighScoreTypes & eType, const long long & llValue, LPCHARACTER pkVictim)
{
	auto pInstance = FindDungeonInstance(GetRealMapIndex(dwMapIndex));
	if (!pInstance)
		return;

	pInstance->RegisterPersonalHighscore(ch, eType, llValue, pkVictim);
}

void CDungeonInfoManager::RecvHighscoreRecord(const TPacketGDDIRanking * pPacket)
{
	auto pInstance = FindDungeonInstance(pPacket->sKey);
	if (!pInstance)
		return;

	pInstance->RecvHighscoreRecord(*pPacket);
}

CDungeonInfoManager::SDungeonInstance * CDungeonInfoManager::FindDungeonInstance(const DWORD & dwMapIndex)
{
	auto fIte = std::find_if(m_dungeon_instances.begin(), m_dungeon_instances.end(), [&dwMapIndex] (const std::pair<DWORD, SDungeonInstance> & rPair) { return dwMapIndex == rPair.second.GetMapIndex(); });
	return (fIte != m_dungeon_instances.end()) ? &(fIte->second) : nullptr;
}

CDungeonInfoManager::SDungeonInstance * CDungeonInfoManager::FindDungeonInstance(const std::string & sKey)
{
	auto fIte = std::find_if(m_dungeon_instances.begin(), m_dungeon_instances.end(), [&sKey] (const std::pair<DWORD, SDungeonInstance> & rPair) { return sKey == rPair.second.GetName(); });
	return (fIte != m_dungeon_instances.end()) ? &(fIte->second) : nullptr;
}
#endif

