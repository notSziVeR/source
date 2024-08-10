#include "stdafx.h"

#ifdef __ADMIN_MANAGER__
#include "AdminManagerController.h"
#include "char.h"
#include "gm.h"
#include "buffer_manager.h"
#include "desc_manager.h"
#include "config.h"
#include "char_manager.h"
#include "sectree_manager.h"
#include "sectree.h"
#include "map_location.h"
#include "item.h"
#include "db.h"
#include "desc_client.h"
#include "log.h"
#include "p2p.h"
#include "questmanager.h"

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CAdminManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

#define ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY 5

struct FMapViewerLoadPlayer
{
	FMapViewerLoadPlayer(TEMP_BUFFER& rkBuf) : m_rkBuf(rkBuf)
	{
		m_dwCount = 0;
	}

	void operator()(LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
		{
			return;
		}

		LPCHARACTER pkChr = (LPCHARACTER) ent;

		if (!pkChr->IsPC())
		{
			return;
		}

		m_dwCount++;
		m_rkBuf.write(&CAdminManager::Instance().GetMapViewerPlayerInfo(pkChr), sizeof(TAdminManagerMapViewerPlayerInfo));
	}

	TEMP_BUFFER&	m_rkBuf;
	DWORD			m_dwCount;
};

struct FMapViewerLoadMob
{
	FMapViewerLoadMob(TEMP_BUFFER& rkBuf, DWORD dwAllowFlag) : m_rkBuf(rkBuf), m_dwAllowFlag(dwAllowFlag)
	{
		m_dwCount = 0;
	}

	void operator()(LPENTITY ent)
	{
		if (!ent->IsType(ENTITY_CHARACTER))
		{
			return;
		}

		LPCHARACTER pkChr = (LPCHARACTER)ent;

		if (!CAdminManager::instance().CheckMapViewerNPC(pkChr, m_dwAllowFlag))
		{
			return;
		}

		m_dwCount++;
		m_rkBuf.write(&CAdminManager::Instance().GetMapViewerMonsterInfo(pkChr), sizeof(TAdminManagerMapViewerMobInfo));
	}

	TEMP_BUFFER&	m_rkBuf;
	DWORD			m_dwCount;
	DWORD			m_dwAllowFlag;
};

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CAdminManager::CAdminManager()
{
	memset(m_adwConfig, 0, sizeof(m_adwConfig));
}

CAdminManager::~CAdminManager()
{
	Destroy();
}

void CAdminManager::Initialize(DWORD* padwConfig, bool bReload)
{
	memcpy(m_adwConfig, padwConfig, sizeof(m_adwConfig));

	if (!bReload)
	{
		InitializeBan();
	}
}

void CAdminManager::Destroy()
{

}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

TAdminManagerPlayerInfo& CAdminManager::GetPlayerInfo(LPCHARACTER pkChr)
{
	static TAdminManagerPlayerInfo kInfo;

	kInfo.dwPID = pkChr->GetPlayerID();
	strlcpy(kInfo.szName, pkChr->GetName(), sizeof(kInfo.szName));
	kInfo.lMapIndex = pkChr->GetMapIndex();
	kInfo.bChannel = g_bChannel;
	kInfo.bEmpire = pkChr->GetEmpire();

	return kInfo;
}

TAdminManagerPlayerInfo& CAdminManager::GetPlayerInfo(CCI* pCCI)
{
	static TAdminManagerPlayerInfo kInfo;

	kInfo.dwPID = pCCI->dwPID;
	strlcpy(kInfo.szName, pCCI->szName, sizeof(kInfo.szName));
	kInfo.lMapIndex = pCCI->lMapIndex;
	kInfo.bChannel = pCCI->bChannel;
	kInfo.bEmpire = pCCI->bEmpire;

	return kInfo;
}

DWORD CAdminManager::GetPlayerInfoList(std::vector<TAdminManagerPlayerInfo>& rvec_PlayerInfo)
{
	// get user by desc
	const DESC_MANAGER::DESC_SET& rset_ClientDesc = DESC_MANAGER::instance().GetClientSet();
	for (itertype(rset_ClientDesc) it = rset_ClientDesc.begin(); it != rset_ClientDesc.end(); ++it)
	{
		LPCHARACTER pkChr = (*it)->GetCharacter();
		if (pkChr && !(*it)->IsPhase(PHASE_LOADING))
		{
			rvec_PlayerInfo.push_back(GetPlayerInfo(pkChr));
		}
	}

	// get user by p2p manager
	const P2P_MANAGER::TPIDCCIMap* pP2PMap = P2P_MANAGER::instance().GetP2PCCIMap();
	for (itertype(*pP2PMap) it = pP2PMap->begin(); it != pP2PMap->end(); ++it)
	{
		CCI* pCCI = it->second;
		rvec_PlayerInfo.push_back(GetPlayerInfo(pCCI));
	}

	return rvec_PlayerInfo.size();
}

/*******************************************************************\
| [PUBLIC] Team Allow Functions
\*******************************************************************/

BYTE CAdminManager::GetTeamRankByPID(DWORD dwPID) const
{
	static DWORD s_dwOldPID = 0;
	static BYTE s_bOldRank = GM_PLAYER;

	if (s_dwOldPID == dwPID)
	{
		return s_bOldRank;
	}

	s_dwOldPID = dwPID;

	// get rank by pid
	if (LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID))
	{
		s_bOldRank = pkChr->GetGMLevel(true);
	}
	else
	{
		if (CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID))
		{
			s_bOldRank = GM::get_level(pkCCI->szName, NULL, true);
		}
		else
		{
			sys_err("cannot get team rank by pid %u", dwPID);
			s_bOldRank = GM_PLAYER;
		}
	}

	return s_bOldRank;
}

DWORD CAdminManager::GetAllow(BYTE bTeamRank) const
{
	return m_adwConfig[bTeamRank];
}

DWORD CAdminManager::GetAllow(LPCHARACTER pkChr) const
{
	return GetAllow(pkChr->GetGMLevel(true));
}

bool CAdminManager::HasAllow(BYTE bTeamRank, DWORD dwAllowFlag) const
{
	return (GetAllow(bTeamRank) & dwAllowFlag) != 0;
}

bool CAdminManager::HasAllow(LPCHARACTER pkChr, DWORD dwAllowFlag) const
{
	return (GetAllow(pkChr) & dwAllowFlag) != 0;
}

/*******************************************************************\
| [PUBLIC] MapViewer Functions
\*******************************************************************/

TAdminManagerMapViewerPlayerInfo& CAdminManager::GetMapViewerPlayerInfo(LPCHARACTER pkChr)
{
	static TAdminManagerMapViewerPlayerInfo kInfo;

	kInfo.dwPID = pkChr->GetPlayerID();
	strlcpy(kInfo.szName, pkChr->GetName(), sizeof(kInfo.szName));
	kInfo.bEmpire = pkChr->GetEmpire();
	kInfo.is_alive = !pkChr->IsDead();
	kInfo.x = pkChr->GetX();
	kInfo.y = pkChr->GetY();

	return kInfo;
}

TAdminManagerMapViewerMobInfo& CAdminManager::GetMapViewerMonsterInfo(LPCHARACTER pkChr)
{
	static TAdminManagerMapViewerMobInfo kInfo;

	kInfo.dwVID = pkChr->GetVID();
	kInfo.dwRaceNum = pkChr->GetRaceNum();
	kInfo.is_alive = !pkChr->IsDead();
	kInfo.x = pkChr->GetX();
	kInfo.y = pkChr->GetY();
	kInfo.stone_drop_vnum = pkChr->IsStone() ? pkChr->GetDropMetinStoneVnum() : 0;

	return kInfo;
}

DWORD CAdminManager::GetMapViewerNPCAllowFlag(LPCHARACTER pkNPC) const
{
	if (pkNPC->IsStone())
	{
		return ALLOW_VIEW_MAPVIEWER_STONE;
	}
	if (pkNPC->IsMonster() && pkNPC->GetMobRank() >= MOB_RANK_BOSS)
	{
		return ALLOW_VIEW_MAPVIEWER_MONSTER;
	}
	if (pkNPC->GetCharType() == CHAR_TYPE_NPC)
	{
		return ALLOW_VIEW_MAPVIEWER_NPC;
	}

	return 0;
}

bool CAdminManager::CheckMapViewerNPC(LPCHARACTER pkChr, DWORD dwAllowFlag)
{
	return (pkChr->IsStone() && (dwAllowFlag & ALLOW_VIEW_MAPVIEWER_STONE) != 0) ||
		   (pkChr->IsMonster() && pkChr->GetMobRank() >= MOB_RANK_BOSS && (dwAllowFlag & ALLOW_VIEW_MAPVIEWER_MONSTER) != 0) ||
		   (pkChr->GetCharType() == CHAR_TYPE_NPC && (dwAllowFlag & ALLOW_VIEW_MAPVIEWER_NPC) != 0);
}

long CAdminManager::GetMapViewerMapIndex(DWORD dwPID)
{
	itertype(m_map_MapViewerByPID) it = m_map_MapViewerByPID.find(dwPID);
	if (it == m_map_MapViewerByPID.end())
	{
		return 0;
	}
	return it->second;
}

bool CAdminManager::IsRunningMapViewer(DWORD dwPID)
{
	return m_map_MapViewerByPID.find(dwPID) != m_map_MapViewerByPID.end();
}

void CAdminManager::StartMapViewer(LPCHARACTER pkChr, long lMapIndex)
{
	DWORD dwPID = pkChr->GetPlayerID();

	long lCurrentMapIndex = GetMapViewerMapIndex(dwPID);
	if (lCurrentMapIndex != 0)
	{
		if (lCurrentMapIndex == lMapIndex)
		{
			sys_err("cannot start mapviewer for %u %s (already running)", dwPID, pkChr->GetName());
			return;
		}

		StopMapViewer(dwPID);
	}

	// if (!CMapLocation::instance().Exists(lMapIndex))
	// {
	// sys_err("cannot start mapviewer for %u %s (map %ld does not exist)", dwPID, pkChr->GetName(), lMapIndex);
	// return;
	// }

	// check if the map is on other core
	LPDESC pkP2PDesc = P2P_MANAGER::instance().GetP2PDescByMapIndex(lMapIndex);
	if (pkP2PDesc)
	{
		StartMapViewer(dwPID, lMapIndex);

		m_map_PlayerP2PConnections[dwPID].insert(pkP2PDesc);

		TEMP_BUFFER buf;
		buf.write(&dwPID, sizeof(DWORD));
		buf.write(&lMapIndex, sizeof(long));
		SendP2P(pkP2PDesc, P2P_SUBHEADER_START_MAPVIEWER, buf.read_peek(), buf.size());
	}
	else
	{
		pkChr->ChatPacket(CHAT_TYPE_INFO, "The mapviewer was not able to start.");
	}
}

void CAdminManager::StartMapViewer(DWORD dwPID, long lMapIndex)
{
	m_map_MapViewerByPID[dwPID] = lMapIndex;
	m_map_MapViewerByMapIndex[lMapIndex].insert(dwPID);

	LPDESC pkP2PDesc = P2P_MANAGER::instance().GetP2PDescByMapIndex(lMapIndex);
	if (!pkP2PDesc)
	{
		SendMapViewerLoadPacket(dwPID, lMapIndex);
	}
}

void CAdminManager::StopMapViewer(DWORD dwPID)
{
	if (!IsRunningMapViewer(dwPID))
	{
		sys_err("cannot stop mapviewer for pid %u (not running)", dwPID);
		return;
	}

	long lMapIndex = m_map_MapViewerByPID[dwPID];

	m_map_MapViewerByMapIndex[lMapIndex].erase(dwPID);
	m_map_MapViewerByPID.erase(dwPID);

	// check if the map is on other core
	LPDESC pkP2PDesc = P2P_MANAGER::instance().GetP2PDescByMapIndex(lMapIndex);
	if (pkP2PDesc)
	{
		SendP2P(pkP2PDesc, P2P_SUBHEADER_STOP_MAPVIEWER, &dwPID, sizeof(DWORD));
	}
}

/*******************************************************************\
| [PUBLIC] MapViewer Event Functions
\*******************************************************************/

void CAdminManager::MapViewer_OnPlayerLogin(LPCHARACTER pkChr)
{
	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), ALLOW_VIEW_MAPVIEWER_PLAYER))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_PLAYER_APPEND, &GetMapViewerPlayerInfo(pkChr), sizeof(TAdminManagerMapViewerPlayerInfo));
		}
	}
}

void CAdminManager::MapViewer_OnPlayerLogout(LPCHARACTER pkChr)
{
	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwPID = pkChr->GetPlayerID();

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), ALLOW_VIEW_MAPVIEWER_PLAYER))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_PLAYER_DESTROY, &dwPID, sizeof(DWORD));
		}
	}
}

void CAdminManager::MapViewer_OnPlayerMove(LPCHARACTER pkChr)
{
	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwPID = pkChr->GetPlayerID();
	const PIXEL_POSITION& rkPos = pkChr->GetXYZ();

	TEMP_BUFFER buf;
	buf.write(&dwPID, sizeof(DWORD));
	buf.write(&rkPos.x, sizeof(rkPos.x));
	buf.write(&rkPos.y, sizeof(rkPos.y));

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), ALLOW_VIEW_MAPVIEWER_PLAYER))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_PLAYER_MOVE, buf.read_peek(), buf.size());
		}
	}
}

void CAdminManager::MapViewer_OnPlayerStateChange(LPCHARACTER pkChr)
{
	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwPID = pkChr->GetPlayerID();
	bool is_alive = !pkChr->IsDead();

	TEMP_BUFFER buf;
	buf.write(&dwPID, sizeof(DWORD));
	buf.write(&is_alive, sizeof(bool));

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), ALLOW_VIEW_MAPVIEWER_PLAYER))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_PLAYER_STATE, buf.read_peek(), buf.size());
		}
	}
}

void CAdminManager::MapViewer_OnMonsterCreate(LPCHARACTER pkChr)
{
	if (!CheckMapViewerNPC(pkChr))
	{
		return;
	}

	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwAllowFlag = GetMapViewerNPCAllowFlag(pkChr);

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), dwAllowFlag))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_MONSTER_APPEND, &GetMapViewerMonsterInfo(pkChr), sizeof(TAdminManagerMapViewerMobInfo));
		}
	}
}

void CAdminManager::MapViewer_OnMonsterDestroy(LPCHARACTER pkChr)
{
	if (!CheckMapViewerNPC(pkChr))
	{
		return;
	}

	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwAllowFlag = GetMapViewerNPCAllowFlag(pkChr);
	DWORD dwVID = pkChr->GetVID();

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), dwAllowFlag))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_MONSTER_DESTROY, &dwVID, sizeof(DWORD));
		}
	}
}

void CAdminManager::MapViewer_OnMonsterMove(LPCHARACTER pkChr)
{
	if (!CheckMapViewerNPC(pkChr))
	{
		return;
	}

	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwAllowFlag = GetMapViewerNPCAllowFlag(pkChr);
	DWORD dwVID = pkChr->GetVID();
	const PIXEL_POSITION& rkPos = pkChr->GetXYZ();

	TEMP_BUFFER buf;
	buf.write(&dwVID, sizeof(DWORD));
	buf.write(&rkPos.x, sizeof(rkPos.x));
	buf.write(&rkPos.y, sizeof(rkPos.y));

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), dwAllowFlag))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_MONSTER_MOVE, buf.read_peek(), buf.size());
		}
	}
}

void CAdminManager::MapViewer_OnMonsterStateChange(LPCHARACTER pkChr)
{
	if (!CheckMapViewerNPC(pkChr))
	{
		return;
	}

	itertype(m_map_MapViewerByMapIndex) it = m_map_MapViewerByMapIndex.find(pkChr->GetMapIndex());
	if (it == m_map_MapViewerByMapIndex.end() || it->second.size() == 0)
	{
		return;
	}

	DWORD dwAllowFlag = GetMapViewerNPCAllowFlag(pkChr);
	DWORD dwVID = pkChr->GetVID();
	bool is_alive = !pkChr->IsDead();

	TEMP_BUFFER buf;
	buf.write(&dwVID, sizeof(DWORD));
	buf.write(&is_alive, sizeof(bool));

	for (itertype(it->second) set_it = it->second.begin(); set_it != it->second.end(); ++set_it)
	{
		if (HasAllow(GetTeamRankByPID(*set_it), dwAllowFlag))
		{
			SendClientPacket(*set_it, GC_SUBHEADER_MAPVIEWER_MONSTER_STATE, buf.read_peek(), buf.size());
		}
	}
}

/*******************************************************************\
| [PUBLIC] Observer Functions
\*******************************************************************/

bool CAdminManager::IsObserverUpdatePoint(unsigned char bType) const
{
	static const unsigned char s_abPoints[] = { POINT_LEVEL, POINT_EXP, POINT_NEXT_EXP, POINT_HT, POINT_ST, POINT_DX, POINT_IQ,
												POINT_HP, POINT_MAX_HP, POINT_SP, POINT_MAX_SP, POINT_GOLD
											  };
	static const unsigned char* s_pbPointLast = s_abPoints + sizeof(s_abPoints) / sizeof(unsigned char);
	return std::find(s_abPoints, s_pbPointLast, bType) != s_pbPointLast;
}

DWORD CAdminManager::GetObserverPID(DWORD dwPID)
{
	itertype(m_map_ObserverByPID) it = m_map_ObserverByPID.find(dwPID);
	if (it == m_map_ObserverByPID.end())
	{
		return 0;
	}
	return it->second;
}

bool CAdminManager::IsRunningObserver(DWORD dwPID)
{
	return m_map_ObserverByPID.find(dwPID) != m_map_ObserverByPID.end();
}

void CAdminManager::StartObserver(LPCHARACTER pkChr, const char* c_pszPlayerName, bool bSendFailMessage)
{
	DWORD dwPID = pkChr->GetPlayerID();

	LPDESC pkPlayerP2PDesc = NULL;
	DWORD dwPlayerPID;
	if (LPCHARACTER pkPlayer = CHARACTER_MANAGER::instance().FindPC(c_pszPlayerName))
	{
		if (pkPlayer->GetDesc()->IsPhase(PHASE_GAME) || pkPlayer->GetDesc()->IsPhase(PHASE_DEAD))
		{
			dwPlayerPID = pkPlayer->GetPlayerID();
		}
		else
		{
			if (bSendFailMessage)
			{
				pkChr->ChatPacket(CHAT_TYPE_INFO, "Cannot find player by name %s.", c_pszPlayerName);
			}
			return;
		}
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::instance().Find(c_pszPlayerName);
		if (pkCCI)
		{
			pkPlayerP2PDesc = pkCCI->pkDesc;
			dwPlayerPID = pkCCI->dwPID;
		}
		else
		{
			if (bSendFailMessage)
			{
				pkChr->ChatPacket(CHAT_TYPE_INFO, "Cannot find player by name %s.", c_pszPlayerName);
			}
			return;
		}
	}

	if (GetTeamRankByPID(dwPlayerPID) >= pkChr->GetGMLevel(true) && pkChr->GetGMLevel(true) < GM_IMPLEMENTOR)
	{
		if (bSendFailMessage)
		{
			pkChr->ChatPacket(CHAT_TYPE_INFO, "You cannot observe player %s.", c_pszPlayerName);
		}
		return;
	}

	DWORD dwCurrentPID = GetObserverPID(dwPID);
	if (dwCurrentPID != 0)
	{
		if (dwCurrentPID == dwPID)
		{
			sys_err("cannot start observer for %u %s (already running)", dwPID, pkChr->GetName());
			return;
		}

		StopObserver(dwPID);
	}

	StartObserver(dwPID, dwPlayerPID);

	if (pkPlayerP2PDesc)
	{
		m_map_PlayerP2PConnections[dwPID].insert(pkPlayerP2PDesc);

		TEMP_BUFFER buf;
		buf.write(&dwPID, sizeof(DWORD));
		buf.write(&dwPlayerPID, sizeof(DWORD));
		SendP2P(pkPlayerP2PDesc, P2P_SUBHEADER_START_OBSERVER, buf.read_peek(), buf.size());
	}
}

void CAdminManager::StartObserver(DWORD dwPID, DWORD dwPlayerPID)
{
	m_map_ObserverByPID[dwPID] = dwPlayerPID;
	m_map_ObserverSetByPID[dwPlayerPID].insert(dwPID);

	LPCHARACTER pkPlayer = CHARACTER_MANAGER::instance().FindByPID(dwPlayerPID);
	if (pkPlayer)
	{
		SendObserverLoadPacket(dwPID, pkPlayer);
	}
}

bool CAdminManager::StopObserver(DWORD dwPID, bool bIsForced)
{
	if (!IsRunningObserver(dwPID))
	{
		sys_err("cannot stop observer for pid %u (not running)", dwPID);
		return false;
	}

	DWORD dwPlayerPID = m_map_ObserverByPID[dwPID];
	m_map_ObserverByPID.erase(dwPID);

	bool bRemovedSet = false;
	std::set<DWORD>& rkPIDSet = m_map_ObserverSetByPID[dwPlayerPID];
	if (rkPIDSet.size() == 1)
	{
		m_map_ObserverSetByPID.erase(dwPlayerPID);
		bRemovedSet = true;
	}
	else
	{
		rkPIDSet.erase(dwPID);
	}

	if (bIsForced)
	{
		LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);
		if (pkChr)
		{
			SendClientPacket(pkChr->GetDesc(), GC_SUBHEADER_OBSERVER_STOP_FORCED, NULL, 0);
			pkChr->ChatPacket(CHAT_TYPE_INFO, "The observation of %s has been forced to stop.", pkChr->GetName());
			pkChr->ChatPacket(CHAT_TYPE_INFO, "The observation will continue when the player comes back online.");
		}
	}

	CCI* pkP2PCCI;
	// check if the observing player is on other core
	pkP2PCCI = P2P_MANAGER::instance().FindByPID(dwPID);
	if (pkP2PCCI)
	{
		SendP2P(pkP2PCCI->pkDesc, bIsForced ? P2P_SUBHEADER_STOP_OBSERVER_BY_FORCE : P2P_SUBHEADER_STOP_OBSERVER, &dwPID, sizeof(DWORD));
	}
	// check if the observed player is on other core
	pkP2PCCI = P2P_MANAGER::instance().FindByPID(dwPlayerPID);
	if (pkP2PCCI)
	{
		SendP2P(pkP2PCCI->pkDesc, bIsForced ? P2P_SUBHEADER_STOP_OBSERVER_BY_FORCE : P2P_SUBHEADER_STOP_OBSERVER, &dwPID, sizeof(DWORD));
	}

	return bRemovedSet;
}

/*******************************************************************\
| [PUBLIC] Observer Event Functions
\*******************************************************************/

void CAdminManager::Observer_OnPlayerSkillGroupChange(LPCHARACTER pkChr)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	unsigned char bSkillGroup = pkChr->GetSkillGroup();

	SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_SKILLGROUP, &bSkillGroup, sizeof(unsigned char), ALLOW_VIEW_OBSERVER_SKILL);
}

void CAdminManager::Observer_OnPlayerSkillChange(LPCHARACTER pkChr, DWORD dwSkillVnum)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	if (dwSkillVnum)
	{
		const TPlayerSkill* pkSkill = &(pkChr->GetPlayerSkills()[dwSkillVnum]);

		TEMP_BUFFER buf;
		buf.write(&dwSkillVnum, sizeof(DWORD));
		buf.write(pkSkill, sizeof(TPlayerSkill));

		SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_SKILL_UPDATE, buf.read_peek(), buf.size(), ALLOW_VIEW_OBSERVER_SKILL);
	}
	else
	{
		const TPlayerSkill* pkSkill = pkChr->GetPlayerSkills();

		SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_SKILL_UPDATE_ALL, pkSkill, sizeof(TPlayerSkill) * SKILL_MAX_NUM, ALLOW_VIEW_OBSERVER_SKILL);
	}
}

void CAdminManager::Observer_OnPlayerSkillCooldown(LPCHARACTER pkChr, DWORD dwSkillVnum, DWORD dwCoolTime)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	TEMP_BUFFER buf;
	buf.write(&dwSkillVnum, sizeof(DWORD));
	buf.write(&dwCoolTime, sizeof(DWORD));
	SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_SKILL_COOLDOWN, buf.read_peek(), buf.size(), ALLOW_VIEW_OBSERVER_SKILL);
}

void CAdminManager::Observer_OnPlayerPointChange(LPCHARACTER pkChr, unsigned char bType, int64_t llValue)
{
	if (!IsObserverUpdatePoint(bType))
	{
		return;
	}

	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	TEMP_BUFFER buf;
	buf.write(&bType, sizeof(unsigned char));
	buf.write(&llValue, sizeof(int64_t));
	SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_POINT_UPDATE, buf.read_peek(), buf.size(), bType == POINT_GOLD ? ALLOW_VIEW_OBSERVER_GOLD : 0);
}

void CAdminManager::Observer_OnPlayerItemSet(LPCHARACTER pkChr, WORD wCell)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	bool bIsInventory = wCell < INVENTORY_MAX_NUM || wCell >= INVENTORY_MAX_NUM + WEAR_MAX_NUM;
	DWORD dwAllowFlag = bIsInventory ? ALLOW_VIEW_OBSERVER_INVENTORY : ALLOW_VIEW_OBSERVER_EQUIPMENT;

	LPITEM pkItem = pkChr->GetInventoryItem(wCell);
	if (pkItem)
	{
		TAdminManagerObserverItemInfo kInfo;
		EncodeObserverItemPacket(kInfo, pkItem);
		SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_ITEM_SET, &kInfo, sizeof(kInfo), dwAllowFlag);
	}
	else
	{
		SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_ITEM_DEL, &wCell, sizeof(WORD), dwAllowFlag);
	}
}

void CAdminManager::Observer_OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(pkChr->GetPlayerID());
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_CHATBAN_STATE, &dwTimeLeft, sizeof(DWORD));
}

void CAdminManager::Observer_OnPlayerAccountban(DWORD dwPID, DWORD dwTimeLeft)
{
	itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(dwPID);
	if (it == m_map_ObserverSetByPID.end() || it->second.size() == 0)
	{
		return;
	}

	SendObserverPacket(it->second, GC_SUBHEADER_OBSERVER_ACCOUNTBAN_STATE, &dwTimeLeft, sizeof(DWORD));
}

/*******************************************************************\
| [PUBLIC] Ban Functions
\*******************************************************************/

CAdminManager::TBanClientPlayerInfo& CAdminManager::GetBanClientPlayerInfo(LPCHARACTER pkChr, DWORD dwDuration)
{
	static TBanClientPlayerInfo kPlayerInfo;

	if (!pkChr)
	{
		return kPlayerInfo;
	}

	kPlayerInfo.dwPID = pkChr->GetPlayerID();
	strlcpy(kPlayerInfo.szName, pkChr->GetName(), sizeof(kPlayerInfo.szName));
	kPlayerInfo.bRace = pkChr->GetRealRaceNum();
	kPlayerInfo.bLevel = pkChr->GetLevel();
	kPlayerInfo.wChatbanCount = pkChr->GetChatBanCount();
	kPlayerInfo.wAccountbanCount = pkChr->GetAccountBanCount();
	kPlayerInfo.dwDuration = dwDuration;
	kPlayerInfo.bIsOnline = true;

	return kPlayerInfo;
}

CAdminManager::TBanClientPlayerInfo& CAdminManager::GetBanClientPlayerInfo(DWORD dwPID, TChatBanPlayerInfo* pPlayerInfo)
{
	static TBanClientPlayerInfo kPlayerInfo;
	kPlayerInfo.dwPID = dwPID;
	strlcpy(kPlayerInfo.szName, pPlayerInfo->szName, sizeof(kPlayerInfo.szName));
	kPlayerInfo.bRace = pPlayerInfo->bRace;
	kPlayerInfo.bLevel = pPlayerInfo->bLevel;
	kPlayerInfo.wChatbanCount = pPlayerInfo->wChatbanCount;
	kPlayerInfo.wAccountbanCount = pPlayerInfo->wAccountbanCount;
	kPlayerInfo.dwDuration = pPlayerInfo->GetDuration();
	kPlayerInfo.bIsOnline = pPlayerInfo->IsOnline();

	return kPlayerInfo;
}

CAdminManager::TBanClientAccountInfo& CAdminManager::GetBanClientAccountInfo(DWORD dwAID, TAccountBanPlayerInfo* pAccountInfo)
{
	static TBanClientAccountInfo kAccountInfo;
	kAccountInfo.dwAID = dwAID;
	strlcpy(kAccountInfo.szLoginName, pAccountInfo->szLoginName, sizeof(kAccountInfo.szLoginName));
	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		kAccountInfo.dwPID[i] = pAccountInfo->dwPID[i];
		strlcpy(kAccountInfo.szName[i], pAccountInfo->szName[i], sizeof(kAccountInfo.szName[i]));
		kAccountInfo.bRace[i] = pAccountInfo->bRace[i];
		kAccountInfo.bLevel[i] = pAccountInfo->bLevel[i];
		kAccountInfo.wChatbanCount[i] = pAccountInfo->wChatbanCount[i];
	}
	kAccountInfo.wAccountbanCount = pAccountInfo->wAccountbanCount;
	kAccountInfo.dwDuration = pAccountInfo->GetDuration();

	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		kAccountInfo.bIsOnline[i] = false;

		if (!pAccountInfo->dwPID[i])
		{
			continue;
		}

		if (CHARACTER_MANAGER::instance().FindByPID(pAccountInfo->dwPID[i]) != NULL ||
				P2P_MANAGER::instance().FindByPID(pAccountInfo->dwPID[i]) != NULL)
		{
			kAccountInfo.bIsOnline[i] = true;
		}
	}

	return kAccountInfo;
}

void CAdminManager::InitializeBan()
{
	m_map_BanChatList.clear();
	m_map_BanChatListByName.clear();
	m_map_BanAccountList.clear();
	m_map_BanAccountListByName.clear();
	m_map_BanAccountListByPlayerName.clear();
	m_map_BanAccountListByPlayerID.clear();

	// load ban chat list
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT affect.dwPID, player.name, player.job, player.level, player.chatban_count, "
									 "account.ban_count, affect.lDuration FROM player.affect "
									 "INNER JOIN player ON player.id = affect.dwPID "
									 "INNER JOIN %s.account ON account.id = player.account_id "
									 "WHERE affect.bType = %u", g_AccountDatabase.c_str(), AFFECT_BLOCK_CHAT));
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int col = 0;

			DWORD dwPID;
			str_to_number(dwPID, row[col++]);

			TChatBanPlayerInfo& rkPlayerInfo = m_map_BanChatList[dwPID];

			strlcpy(rkPlayerInfo.szName, row[col++], sizeof(rkPlayerInfo.szName));
			str_to_number(rkPlayerInfo.bRace, row[col++]);
			str_to_number(rkPlayerInfo.bLevel, row[col++]);
			str_to_number(rkPlayerInfo.wChatbanCount, row[col++]);
			str_to_number(rkPlayerInfo.wAccountbanCount, row[col++]);
			str_to_number(rkPlayerInfo.dwDuration, row[col++]);

			m_map_BanChatListByName[rkPlayerInfo.szName] = dwPID;
		}
	}

	// load ban account list
	{
		std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account.id, account.login, player.id, player.name, player.job, "
									 "player.level, player.chatban_count, account.ban_count, account.status, TIME_TO_SEC(TIMEDIFF(account.availDt, NOW())) "
									 "FROM player.player "
									 "RIGHT JOIN %s.account ON account.id = player.account_id "
									 "WHERE account.`status` = 'BLOCK' OR account.availDt > NOW() "
									 "ORDER BY account.id", g_AccountDatabase.c_str()));

		DWORD dwCurAID = 0;
		DWORD dwCurPlayerIndex = 0;
		TAccountBanPlayerInfo* pAccountInfo = NULL;
		while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
		{
			int col = 0;

			DWORD dwAID;
			str_to_number(dwAID, row[col++]);
			char szAccountLogin[LOGIN_MAX_LEN + 1];
			strlcpy(szAccountLogin, row[col++], sizeof(szAccountLogin));

			if (dwAID != dwCurAID)
			{
				dwCurAID = dwAID;
				dwCurPlayerIndex = 0;

				pAccountInfo = &m_map_BanAccountList[dwAID];
				memset(pAccountInfo, 0, sizeof(TAccountBanPlayerInfo));
				strlcpy(pAccountInfo->szLoginName, szAccountLogin, sizeof(pAccountInfo->szLoginName));

				m_map_BanAccountListByName[szAccountLogin] = dwAID;
			}

			if (row[col])
			{
				str_to_number(pAccountInfo->dwPID[dwCurPlayerIndex], row[col]);
			}
			++col;
			if (row[col])
			{
				strlcpy(pAccountInfo->szName[dwCurPlayerIndex], row[col], sizeof(pAccountInfo->szName[dwCurPlayerIndex]));
			}
			++col;
			if (row[col])
			{
				str_to_number(pAccountInfo->bRace[dwCurPlayerIndex], row[col]);
			}
			++col;
			if (row[col])
			{
				str_to_number(pAccountInfo->bLevel[dwCurPlayerIndex], row[col]);
			}
			++col;
			if (row[col])
			{
				str_to_number(pAccountInfo->wChatbanCount[dwCurPlayerIndex], row[col]);
			}
			++col;

			str_to_number(pAccountInfo->wAccountbanCount, row[col++]);

			char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
			strlcpy(szStatus, row[col++], sizeof(szStatus));
			if (!strcasecmp(szStatus, "BLOCK"))
			{
				pAccountInfo->dwDuration = INT_MAX;
			}
			else
			{
				DWORD dwDuration;
				str_to_number(dwDuration, row[col++]);

				pAccountInfo->SetDuration(dwDuration);
			}

			m_map_BanAccountListByPlayerName[pAccountInfo->szName[dwCurPlayerIndex]] = dwAID;
			m_map_BanAccountListByPlayerID[pAccountInfo->dwPID[dwCurPlayerIndex]] = dwAID;
			++dwCurPlayerIndex;
		}
	}
}

void CAdminManager::SearchBanChatPlayer(LPCHARACTER pkChr, const char* c_pszName)
{
	SearchBanChatPlayer(pkChr->GetPlayerID(), c_pszName);
}

void CAdminManager::SearchBanChatPlayer(DWORD dwGMPid, const char* c_pszName)
{
	TBanClientPlayerInfo* pPlayerInfo = NULL;

	LPCHARACTER pkFind = CHARACTER_MANAGER::instance().FindPC(c_pszName);
	if (pkFind)
	{
		pPlayerInfo = &GetBanClientPlayerInfo(pkFind);

		if (CAffect* pkAff = pkFind->FindAffect(AFFECT_BLOCK_CHAT))
		{
			pPlayerInfo->dwDuration = pkAff->lDuration;
		}

		P2P_SearchBanChatPlayer(dwGMPid, true, pPlayerInfo);
	}
	else
	{
		TEMP_BUFFER buf;
		buf.write(&dwGMPid, sizeof(DWORD));
		int iLen = strlen(c_pszName);
		buf.write(&iLen, sizeof(int));
		buf.write(c_pszName, iLen);

		if (CCI* pkCCI = P2P_MANAGER::instance().Find(c_pszName))
		{
			SendP2P(pkCCI->pkDesc, P2P_SUBHEADER_BAN_CHAT_SEARCH_PLAYER, buf.read_peek(), buf.size());
		}
		else
		{
			db_clientdesc->DBPacket(HEADER_GD_ADMIN_MANAGER_CHAT_SEARCH_PLAYER, 0, buf.read_peek(), buf.size());
		}
	}
}

void CAdminManager::P2P_SearchBanChatPlayer(DWORD dwGMPid, bool bSuccess, TBanClientPlayerInfo* pPlayerInfo)
{
	if (bSuccess && !pPlayerInfo)
	{
		sys_err("invalid call P2P_SearchBanChatPlayer: bSuccess = TRUE pPlayerInfo = NULL");
		return;
	}

	TEMP_BUFFER buf;
	buf.write(&bSuccess, sizeof(bool));
	if (bSuccess)
	{
		buf.write(pPlayerInfo, sizeof(TBanClientPlayerInfo));
	}
	SendClientPacket(dwGMPid, GC_SUBHEADER_BAN_CHAT_SEARCH_PLAYER, buf.read_peek(), buf.size());
}

void CAdminManager::SendBanPlayerLog(LPCHARACTER pkChr, DWORD dwPlayerID)
{
	char szExtraOption[256]{ 0 };
	int iExtraOptionLen = 0;

	if (HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT_LOG))
		iExtraOptionLen += snprintf(szExtraOption + iExtraOptionLen, sizeof(szExtraOption) - iExtraOptionLen, " OR `type`=%u",
									BAN_TYPE_CHAT);
	if (HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT_LOG))
		iExtraOptionLen += snprintf(szExtraOption + iExtraOptionLen, sizeof(szExtraOption) - iExtraOptionLen, " OR `type`=%u",
									BAN_TYPE_ACCOUNT);

	DBManager::instance().ReturnQuery(QID_ADMIN_MANAGER_BAN_LOG, pkChr->GetPlayerID(), new DWORD(dwPlayerID),
									  "SELECT pid, name, gm_pid, gm_name, `type`+0, new_duration, reason, proof, date FROM log.ban_log WHERE pid = %u "
									  "AND (FALSE%s) "
									  "ORDER BY date DESC", dwPlayerID, szExtraOption);

	sys_err("void CAdminManager::SendBanPlayerLog(LPCHARACTER pkChr, DWORD dwPlayerID)");
}

void CAdminManager::CreateAccountBanInfo(TAccountBanPlayerInfo* pInfo, LPCHARACTER pkChr, DWORD dwAID)
{
	if (!dwAID)
	{
		if (!pkChr)
		{
			sys_err("invalid call of CreateAccountBanInfo (no pkChr and no dwAID given)");
			return;
		}

		dwAID = pkChr->GetAID();
	}

	memset(pInfo, 0, sizeof(TAccountBanPlayerInfo));

	std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account.login, player.id, player.name, player.job, "
								 "player.level, player.chatban_count, account.ban_count, account.status, TIME_TO_SEC(TIMEDIFF(account.availDt, NOW())) "
								 "FROM player.player "
								 "RIGHT JOIN %s.account ON account.id = player.account_id "
								 "WHERE player.account_id = %u", g_AccountDatabase.c_str(), dwAID));

	DWORD dwCurPlayerIndex = 0;
	if (pkChr)
	{
		pInfo->dwPID[dwCurPlayerIndex] = pkChr->GetPlayerID();
		strlcpy(pInfo->szName[dwCurPlayerIndex], pkChr->GetName(), sizeof(pInfo->szName[dwCurPlayerIndex]));
		pInfo->bRace[dwCurPlayerIndex] = pkChr->GetRealRaceNum();
		pInfo->bLevel[dwCurPlayerIndex] = pkChr->GetLevel();
		pInfo->wChatbanCount[dwCurPlayerIndex] = pkChr->GetChatBanCount();
		++dwCurPlayerIndex;
	}

	while (MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult))
	{
		int col = 0;

		strlcpy(pInfo->szLoginName, row[col++], sizeof(pInfo->szLoginName));

		DWORD dwPID = 0;
		if (row[col])
		{
			str_to_number(dwPID, row[col]);
		}
		++col;

		if (dwPID && (!pkChr || dwPID != pkChr->GetPlayerID()))
		{
			pInfo->dwPID[dwCurPlayerIndex] = dwPID;
			if (row[col])
			{
				strlcpy(pInfo->szName[dwCurPlayerIndex], row[col], sizeof(pInfo->szName[dwCurPlayerIndex]));
			}
			++col;
			if (row[col])
			{
				str_to_number(pInfo->bRace[dwCurPlayerIndex], row[col]);
			}
			++col;
			if (row[col])
			{
				str_to_number(pInfo->bLevel[dwCurPlayerIndex], row[col]);
			}
			++col;
			if (row[col])
			{
				str_to_number(pInfo->wChatbanCount[dwCurPlayerIndex], row[col]);
			}
			++col;

			++dwCurPlayerIndex;
		}
		else
		{
			++col; // player name
			++col; // player race
			++col; // player level
			++col; // player chatban count
		}

		str_to_number(pInfo->wAccountbanCount, row[col++]);

		char szStatus[ACCOUNT_STATUS_MAX_LEN + 1];
		strlcpy(szStatus, row[col++], sizeof(szStatus));
		if (!strcasecmp(szStatus, "BLOCK"))
		{
			pInfo->dwDuration = INT_MAX;
		}
		else
		{
			int iDuration = 0;
			str_to_number(iDuration, row[col++]);

			if (iDuration < 0)
			{
				iDuration = 0;
			}

			pInfo->SetDuration(iDuration);
		}
	}
}

void CAdminManager::DoAccountBan(LPCHARACTER pkChrGM, DWORD dwAID, DWORD dwPlayerID, DWORD dwTimeLeft, const char* c_pszReason, const char* c_pszProof, bool bIncreaseBanCounter)
{
	LPCHARACTER pkChrTarget = dwPlayerID ? CHARACTER_MANAGER::instance().FindByPID(dwPlayerID) : NULL;
	TAccountBanPlayerInfo* pAccountInfo = NULL;
	bool bIsLog = false;

	int iTimeChange = dwTimeLeft;

	// get handle on player info
	itertype(m_map_BanAccountList) it = m_map_BanAccountList.find(dwAID);
	if (it != m_map_BanAccountList.end())
	{
		pAccountInfo = &m_map_BanAccountList[dwAID];

		iTimeChange = (int)dwTimeLeft - (int)pAccountInfo->GetDuration();
	}
	else
	{
		if (!dwTimeLeft)
		{
			return;
		}

		// create account info
		if (pkChrTarget)
		{
			if (dwAID != pkChrTarget->GetAID())
			{
				sys_err("invalid aid %u != real aid %u of player %u", dwAID, it->second, dwPlayerID);
				return;
			}
			pAccountInfo = &m_map_BanAccountList[dwAID];
			CreateAccountBanInfo(pAccountInfo, pkChrTarget);

			if (!*pAccountInfo->szLoginName)
			{
				m_map_BanAccountList.erase(dwAID);
				return;
			}
		}
		else
		{
			// log
			char szTargetName[CHARACTER_NAME_MAX_LEN + 1];
			char szLoginName[LOGIN_MAX_LEN + 1];

			{
				if (dwPlayerID)
				{
					DWORD dwCompareAID = 0;

					std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT player.name, account.login, account.id FROM player.player "
												 "INNER JOIN %s.account ON account.id = player.account_id "
												 "WHERE player.id = %u", g_AccountDatabase.c_str(), dwPlayerID));
					if (pMsg->Get()->uiNumRows == 0)
					{
						if (pkChrGM)
						{
							pkChrGM->ChatPacket(CHAT_TYPE_INFO, "The player [%u] could not be found.", dwPlayerID);
						}
						return;
					}

					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
					strlcpy(szTargetName, row[0], sizeof(szTargetName));
					strlcpy(szLoginName, row[1], sizeof(szLoginName));
					str_to_number(dwCompareAID, row[2]);

					if (dwAID != dwCompareAID)
					{
						sys_err("invalid aid %u != real aid %u of player %u", dwAID, dwCompareAID, dwPlayerID);
						return;
					}
				}
				else
				{
					std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account.login FROM %s.account WHERE id = %u",
												 g_AccountDatabase.c_str(), dwAID));
					if (pMsg->Get()->uiNumRows == 0)
					{
						if (pkChrGM)
						{
							pkChrGM->ChatPacket(CHAT_TYPE_INFO, "The account [%u] could not be found.", dwAID);
						}
						return;
					}

					MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
					szTargetName[0] = '\0';
					strlcpy(szLoginName, row[0], sizeof(szLoginName));
				}

				LogManager::instance().AccountBanLog(dwPlayerID, dwAID, szTargetName, pkChrGM, iTimeChange, dwTimeLeft, c_pszReason, c_pszProof, bIncreaseBanCounter);
				bIsLog = true;
			}

			LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(szLoginName);
			CCI* pkCCI;
			if (pkDesc || (!(pkCCI = P2P_MANAGER::instance().FindByPID(dwPlayerID))))
			{
				pAccountInfo = &m_map_BanAccountList[dwAID];
				CreateAccountBanInfo(pAccountInfo, pkDesc ? pkDesc->GetCharacter() : NULL, dwAID);
			}
			else
			{
				DWORD dwGMPid = pkChrGM->GetPlayerID();
				BYTE bRequestorGMLevel = pkChrGM->GetGMLevel(true);

				TEMP_BUFFER buf;
				buf.write(&dwGMPid, sizeof(DWORD));
				buf.write(&bRequestorGMLevel, sizeof(BYTE));
				buf.write(&dwAID, sizeof(DWORD));
				int iLen = strlen(szLoginName);
				buf.write(&iLen, sizeof(int));
				buf.write(szLoginName, iLen);
				buf.write(&dwTimeLeft, sizeof(DWORD));
				buf.write(&bIncreaseBanCounter, sizeof(bool));
				SendP2P(pkCCI->pkDesc, P2P_SUBHEADER_BAN_ACCOUNT_REQUEST, buf.read_peek(), buf.size());

				pkChrGM->ChatPacket(CHAT_TYPE_INFO, "A ban request for %s (account %s) to channel %u (port %u) has been sent.",
									szTargetName, szLoginName, pkCCI->pkDesc->GetP2PChannel(), pkCCI->pkDesc->GetP2PPort());

				return;
			}
		}
	}

	// set new duration
	DWORD dwOldDuration = pAccountInfo->dwDuration;
	pAccountInfo->SetDuration(dwTimeLeft);

	// increase ban counter
	if (bIncreaseBanCounter)
	{
		pAccountInfo->wAccountbanCount++;
	}

	// exec ban
	if (!__DoAccountBan(dwAID, pAccountInfo, pkChrGM->GetPlayerID(), pkChrGM->GetGMLevel()))
	{
		pAccountInfo->dwDuration = dwOldDuration;
		pAccountInfo->wAccountbanCount--;
		return;
	}

	// log
	if (!bIsLog)
	{
		LogManager::Instance().AccountBanLog(dwPlayerID, dwAID, pAccountInfo->szName[0], pkChrGM, iTimeChange, dwTimeLeft, c_pszReason, c_pszProof, bIncreaseBanCounter);
	}
}

void CAdminManager::P2P_DoAccountBan(DWORD dwGMPid, BYTE bRequestorGMLevel, DWORD dwAID, const char* c_pszLoginName, DWORD dwTimeLeft, bool bIncreaseBanCounter)
{
	TAccountBanPlayerInfo* pAccountInfo = NULL;
	itertype(m_map_BanAccountList) it = m_map_BanAccountList.find(dwAID);
	if (it != m_map_BanAccountList.end())
	{
		pAccountInfo = &it->second;
	}
	else
	{
		pAccountInfo = &m_map_BanAccountList[dwAID];

		LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(c_pszLoginName);
		CreateAccountBanInfo(pAccountInfo, pkDesc ? pkDesc->GetCharacter() : NULL, dwAID);
		if (!*pAccountInfo->szLoginName)
		{
			m_map_BanAccountList.erase(dwAID);
			return;
		}
	}

	// set new duration
	DWORD dwOldDuration = pAccountInfo->dwDuration;
	pAccountInfo->SetDuration(dwTimeLeft);

	// increase ban counter
	if (bIncreaseBanCounter)
	{
		pAccountInfo->wAccountbanCount++;
	}

	// exec ban
	if (!__DoAccountBan(dwAID, pAccountInfo, dwGMPid, bRequestorGMLevel))
	{
		pAccountInfo->dwDuration = dwOldDuration;
		pAccountInfo->wAccountbanCount--;
		return;
	}
}

bool CAdminManager::__DoAccountBan(DWORD dwAID, TAccountBanPlayerInfo* pAccountInfo, DWORD dwGMPid, BYTE bRequestorGMLevel)
{;
	// check gm levels
	BYTE bTargetGMLevel = GM_PLAYER;
	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		if (!pAccountInfo->dwPID[i])
		{
			continue;
		}

		BYTE bGMLevel = GM::get_level(pAccountInfo->szName[i], NULL, true);
		if (bGMLevel > bTargetGMLevel)
		{
			bTargetGMLevel = bGMLevel;
		}
	}
	if (bTargetGMLevel > GM_PLAYER)
	{
		if (bTargetGMLevel == GM_IMPLEMENTOR || bRequestorGMLevel < GM_IMPLEMENTOR)
		{
			//pkDesc->GetCharacter()->ChatInfoTrans("You cannot ban accounts of the team.");
			return false;
		}
	}

	// disconnect account
	if (pAccountInfo->GetDuration())
	{
		LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(pAccountInfo->szLoginName);
		if (pkDesc)
		{
			if (ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY)
			{
				if (pkDesc->GetCharacter())
					pkDesc->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "Your account has been blocked. You will be disconnected within %d seconds.",
													   ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY);
				pkDesc->DelayedDisconnect(ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY);
			}
			else
			{
				pkDesc->SetPhase(PHASE_CLOSE);
			}

			if (pkDesc->GetCharacter())
			{
				pkDesc->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "The player [%s] of account [%s] has been disconnected.",
												   pkDesc->GetCharacter()->GetName(), pAccountInfo->szLoginName);
			}
			else
			{
				//SendGMChatPacket("The account [%s] has been disconnected.", pAccountInfo->szLoginName);
			}
		}
		else
		{
			int iLen = strlen(pAccountInfo->szLoginName);

			TEMP_BUFFER buf;
			buf.write(&iLen, sizeof(int));
			buf.write(pAccountInfo->szLoginName, iLen);
			SendP2P(NULL, P2P_SUBHEADER_BAN_ACCOUNT_DISCONNECT, buf.read_peek(), buf.size());

			//pkDesc->GetCharacter()->ChatInfoTrans("A disconnect request for all cores of account [%s] has been sent.", pAccountInfo->szLoginName);
		}
	}

	// update account table
	SQLMsg* pMsg = NULL;
	if (pAccountInfo->GetDuration() < INT_MAX)
	{
		pMsg = DBManager::instance().DirectQuery("UPDATE %s.account SET "
				"account.status = 'OK', account.availDt = DATE_ADD(NOW(), INTERVAL %u SECOND), ban_count = %u "
				"WHERE account.id = %u", g_AccountDatabase.c_str(), pAccountInfo->GetDuration(), pAccountInfo->wAccountbanCount, dwAID);
	}
	else
	{
		pMsg = DBManager::instance().DirectQuery("UPDATE %s.account SET "
				"account.status = 'BLOCK', account.availDt = NOW(), ban_count = %u "
				"WHERE account.id = %u", g_AccountDatabase.c_str(), pAccountInfo->wAccountbanCount, dwAID);
	}

	if (pMsg->Get()->uiAffectedRows == 0)
	{
		//pkDesc->GetCharacter()->ChatInfoTrans("The ban request failed somehow. If the problem happens again report it.");
		delete pMsg;
		return false;
	}
	delete pMsg;

	// get client info table
	TBanClientAccountInfo& rkClientInfo = GetBanClientAccountInfo(dwAID, pAccountInfo);

	// p2p
	SendP2P(NULL, P2P_SUBHEADER_BAN_ACCOUNTBAN, &rkClientInfo, sizeof(TBanClientAccountInfo));

	// event
	OnPlayerAccountban(&rkClientInfo);

	return true;
}
#undef SendGMChatPacket

void CAdminManager::P2P_DoAccountBanDisconnect(const char* c_pszLoginName)
{
	LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(c_pszLoginName);
	if (pkDesc)
	{
		if (ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY)
		{
			if (pkDesc->GetCharacter())
				pkDesc->GetCharacter()->ChatPacket(CHAT_TYPE_INFO, "Your account has been blocked. You will be disconnected within %d seconds.",
												   ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY);
			pkDesc->DelayedDisconnect(ADMIN_MANAGER_DISCONNECT_ON_BAN_DELAY);
		}
		else
		{
			pkDesc->SetPhase(PHASE_CLOSE);
		}
	}
}

void CAdminManager::SearchBanAccount(LPCHARACTER pkChr, const char* c_pszName, BYTE bSearchType)
{
	SearchBanAccount(pkChr->GetPlayerID(), c_pszName, bSearchType);
}

void CAdminManager::SearchBanAccount(DWORD dwGMPid, const char* c_pszName, BYTE bSearchType)
{
	static TAccountBanPlayerInfo skAccountInfo;
	DWORD dwAID = 0;
	TAccountBanPlayerInfo* pAccountInfo = NULL;

	if (bSearchType == BAN_ACCOUNT_SEARCH_ACCOUNT)
	{
		itertype(m_map_BanAccountListByName) it = m_map_BanAccountListByName.find(c_pszName);
		if (it != m_map_BanAccountListByName.end())
		{
			dwAID = it->second;
			pAccountInfo = &m_map_BanAccountList[dwAID];
		}
		else
		{
			pAccountInfo = &skAccountInfo;

			LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(c_pszName);
			if (pkDesc)
			{
				dwAID = pkDesc->GetAccountTable().id;
			}
			else
			{
				char szEscapedLogin[LOGIN_MAX_LEN * 2 + 1];
				DBManager::instance().EscapeString(szEscapedLogin, sizeof(szEscapedLogin), c_pszName, strlen(c_pszName));
				std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT id FROM %s.account WHERE login = '%s'",
											 g_AccountDatabase.c_str(), szEscapedLogin));
				if (pMsg->Get()->uiNumRows == 0)
				{
					P2P_SearchBanAccount(dwGMPid, false, NULL);
					return;
				}
				str_to_number(dwAID, mysql_fetch_row(pMsg->Get()->pSQLResult)[0]);
			}

			CreateAccountBanInfo(pAccountInfo, pkDesc ? pkDesc->GetCharacter() : NULL, dwAID);
		}
	}
	else if (bSearchType == BAN_ACCOUNT_SEARCH_PLAYER)
	{
		itertype(m_map_BanAccountListByPlayerName) it = m_map_BanAccountListByPlayerName.find(c_pszName);
		if (it != m_map_BanAccountListByPlayerName.end())
		{
			dwAID = it->second;
			pAccountInfo = &m_map_BanAccountList[dwAID];
		}
		else
		{
			pAccountInfo = &skAccountInfo;

			LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(c_pszName);
			if (pkChr)
			{
				dwAID = pkChr->GetAID();
				CreateAccountBanInfo(pAccountInfo, pkChr);
			}
			else
			{
				CCI* pkCCI = P2P_MANAGER::instance().Find(c_pszName);
				if (pkCCI)
				{
					TEMP_BUFFER buf;
					buf.write(&dwGMPid, sizeof(DWORD));
					int iLen = strlen(c_pszName);
					buf.write(&iLen, sizeof(int));
					buf.write(c_pszName, iLen);
					buf.write(&bSearchType, sizeof(BYTE));

					SendP2P(pkCCI->pkDesc, P2P_SUBHEADER_BAN_ACCOUNT_SEARCH, buf.read_peek(), buf.size());
					return;
				}
				else
				{
					char szEscapedName[CHARACTER_NAME_MAX_LEN * 2 + 1];
					DBManager::instance().EscapeString(szEscapedName, sizeof(szEscapedName), c_pszName, strlen(c_pszName));
					std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT account_id FROM player WHERE name = '%s'", szEscapedName));
					if (pMsg->Get()->uiNumRows == 0)
					{
						P2P_SearchBanAccount(dwGMPid, false, NULL);
						return;
					}
					str_to_number(dwAID, mysql_fetch_row(pMsg->Get()->pSQLResult)[0]);

					CreateAccountBanInfo(pAccountInfo, NULL, dwAID);
				}
			}
		}
	}

	P2P_SearchBanAccount(dwGMPid, dwAID != 0, &GetBanClientAccountInfo(dwAID, pAccountInfo));
}

void CAdminManager::P2P_SearchBanAccount(DWORD dwGMPid, bool bSuccess, TBanClientAccountInfo* pAccountInfo)
{
	if (bSuccess && !pAccountInfo)
	{
		sys_err("invalid call P2P_SearchBanAccount: bSuccess = TRUE pAccountInfo = NULL");
		return;
	}

	TEMP_BUFFER buf;
	buf.write(&bSuccess, sizeof(bool));
	if (bSuccess)
	{
		buf.write(pAccountInfo, sizeof(TBanClientAccountInfo));
	}
	SendClientPacket(dwGMPid, GC_SUBHEADER_BAN_ACCOUNT_SEARCH, buf.read_peek(), buf.size());
}

void CAdminManager::SendBanAccountLog(LPCHARACTER pkChr, DWORD dwAccountID)
{
	char szExtraOption[256]{ 0 };
	int iExtraOptionLen = 0;

	if (HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT_LOG))
		iExtraOptionLen += snprintf(szExtraOption + iExtraOptionLen, sizeof(szExtraOption) - iExtraOptionLen, " OR `type`=%u",
									BAN_TYPE_CHAT);
	if (HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT_LOG))
		iExtraOptionLen += snprintf(szExtraOption + iExtraOptionLen, sizeof(szExtraOption) - iExtraOptionLen, " OR `type`=%u",
									BAN_TYPE_ACCOUNT);

	DBManager::instance().ReturnQuery(QID_ADMIN_MANAGER_BAN_LOG, pkChr->GetPlayerID(), new DWORD(dwAccountID),
									  "SELECT pid, name, gm_pid, gm_name, `type`+0, new_duration, reason, proof, date FROM log.ban_log WHERE aid = %u "
									  "AND (FALSE%s) "
									  "ORDER BY date DESC", dwAccountID, szExtraOption);

	sys_err("void CAdminManager::SendBanAccountLog(LPCHARACTER pkChr, DWORD dwAccountID)");
}

/*******************************************************************\
| [PUBLIC] Ban Event Functions
\*******************************************************************/

void CAdminManager::Ban_OnPlayerLogin(DWORD dwPID)
{
	itertype(m_map_BanChatList) it = m_map_BanChatList.find(dwPID);
	if (it == m_map_BanChatList.end())
	{
		return;
	}
	it->second.SetOnline();
}

void CAdminManager::Ban_OnPlayerLogout(DWORD dwPID)
{
	itertype(m_map_BanChatList) it = m_map_BanChatList.find(dwPID);
	if (it == m_map_BanChatList.end())
	{
		return;
	}
	it->second.SetOffline();
}

void CAdminManager::Ban_OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft)
{
	TBanClientPlayerInfo& rkPlayerInfo = GetBanClientPlayerInfo(pkChr, dwTimeLeft);

	SendP2P(NULL, P2P_SUBHEADER_BAN_PLAYER_CHATBAN, &rkPlayerInfo, sizeof(TBanClientPlayerInfo));
	Ban_OnPlayerChatban(&rkPlayerInfo);
}

void CAdminManager::Ban_OnPlayerChatban(const char* c_pszName, DWORD dwTimeLeft, bool bIncreaseBanCounter)
{
	TBanClientPlayerInfo* pPlayerInfo = NULL;

	itertype(m_map_BanChatListByName) it = m_map_BanChatListByName.find(c_pszName);
	if (it != m_map_BanChatListByName.end())
	{
		itertype(m_map_BanChatList) it2 = m_map_BanChatList.find(it->second);
		if (it2 != m_map_BanChatList.end())
		{
			pPlayerInfo = &GetBanClientPlayerInfo(it2->first, &it2->second);
		}
	}

	if (!pPlayerInfo)
	{
		if (!dwTimeLeft)
		{
			return;
		}

		LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindPC(c_pszName);
		if (pkChr)
		{
			pPlayerInfo = &GetBanClientPlayerInfo(pkChr, dwTimeLeft);
		}
		else
		{
			char szEscapeName[CHARACTER_NAME_MAX_LEN * 2 + 1];
			DBManager::instance().EscapeString(szEscapeName, sizeof(szEscapeName), c_pszName, strlen(c_pszName));

			std::unique_ptr<SQLMsg> pMsg(DBManager::instance().DirectQuery("SELECT player.id, player.name, player.job, player.level, "
										 "player.chatban_count, account.ban_count "
										 "FROM player INNER JOIN %s.account ON account.id = player.account_id WHERE name = '%s'",
										 g_AccountDatabase.c_str(), szEscapeName));
			if (!pMsg->Get()->uiNumRows)
			{
				sys_err("cannot get player by name %s [%s] - cannot add to chat ban list", c_pszName, szEscapeName);
				return;
			}

			MYSQL_ROW row = mysql_fetch_row(pMsg->Get()->pSQLResult);
			int col = 0;
			pPlayerInfo = &GetBanClientPlayerInfo(NULL);

			str_to_number(pPlayerInfo->dwPID, row[col++]);
			strlcpy(pPlayerInfo->szName, row[col++], sizeof(pPlayerInfo->szName));
			str_to_number(pPlayerInfo->bRace, row[col++]);
			str_to_number(pPlayerInfo->bLevel, row[col++]);
			str_to_number(pPlayerInfo->wChatbanCount, row[col++]);
			str_to_number(pPlayerInfo->wAccountbanCount, row[col++]);
			pPlayerInfo->dwDuration = dwTimeLeft;
		}
	}
	else
	{
		pPlayerInfo->dwDuration = dwTimeLeft;

		if (bIncreaseBanCounter)
		{
			pPlayerInfo->wChatbanCount++;
		}
	}

	SendP2P(NULL, P2P_SUBHEADER_BAN_PLAYER_CHATBAN, pPlayerInfo, sizeof(TBanClientPlayerInfo));
	Ban_OnPlayerChatban(pPlayerInfo);
}

void CAdminManager::Ban_OnPlayerChatban(const TBanClientPlayerInfo* pPlayerInfo)
{
	if (!pPlayerInfo->dwDuration)
	{
		if (m_map_BanChatList.find(pPlayerInfo->dwPID) == m_map_BanChatList.end())
		{
			return;
		}

		m_map_BanChatListByName.erase(pPlayerInfo->szName);
		m_map_BanChatList.erase(pPlayerInfo->dwPID);
	}
	else
	{
		TChatBanPlayerInfo& rkPlayerInfo = m_map_BanChatList[pPlayerInfo->dwPID];
		strlcpy(rkPlayerInfo.szName, pPlayerInfo->szName, sizeof(rkPlayerInfo.szName));
		rkPlayerInfo.bRace = pPlayerInfo->bRace;
		rkPlayerInfo.bLevel = pPlayerInfo->bLevel;
		rkPlayerInfo.wChatbanCount = pPlayerInfo->wChatbanCount;
		rkPlayerInfo.wAccountbanCount = pPlayerInfo->wAccountbanCount;

		if (CHARACTER_MANAGER::instance().FindByPID(pPlayerInfo->dwPID) || P2P_MANAGER::instance().FindByPID(pPlayerInfo->dwPID))
		{
			rkPlayerInfo.SetOnline();
		}
		else if (rkPlayerInfo.IsOnline())
		{
			rkPlayerInfo.SetOffline();
		}
		rkPlayerInfo.SetDuration(pPlayerInfo->dwDuration);

		m_map_BanChatListByName[rkPlayerInfo.szName] = pPlayerInfo->dwPID;
	}

	for (itertype(m_set_ActivePIDs) it = m_set_ActivePIDs.begin(); it != m_set_ActivePIDs.end(); ++it)
	{
		if (HasAllow(GetTeamRankByPID(*it), ALLOW_VIEW_BAN_CHAT))
		{
			SendClientPacket(*it, GC_SUBHEADER_BAN_CHAT_STATE, pPlayerInfo, sizeof(TBanClientPlayerInfo));
		}
	}
}

void CAdminManager::Ban_OnPlayerAccountban(const TBanClientAccountInfo* pAccountInfo)
{
	if (!pAccountInfo->dwDuration)
	{
		if (m_map_BanAccountList.find(pAccountInfo->dwAID) == m_map_BanAccountList.end())
		{
			return;
		}

		for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		{
			if (pAccountInfo->dwPID[i])
			{
				m_map_BanAccountListByPlayerName.erase(pAccountInfo->szName[i]);
				m_map_BanAccountListByPlayerID.erase(pAccountInfo->dwPID[i]);
			}
		}
		m_map_BanAccountListByName.erase(pAccountInfo->szLoginName);
		m_map_BanAccountList.erase(pAccountInfo->dwAID);
	}
	else
	{
		if (LPDESC pkDesc = DESC_MANAGER::instance().FindByLoginName(pAccountInfo->szLoginName))
		{
			pkDesc->GetAccountTable().accban_count = pAccountInfo->wAccountbanCount;
		}

		TAccountBanPlayerInfo& rkAccountInfo = m_map_BanAccountList[pAccountInfo->dwAID];
		strlcpy(rkAccountInfo.szLoginName, pAccountInfo->szLoginName, sizeof(rkAccountInfo.szLoginName));
		for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
		{
			rkAccountInfo.dwPID[i] = pAccountInfo->dwPID[i];
			strlcpy(rkAccountInfo.szName[i], pAccountInfo->szName[i], sizeof(rkAccountInfo.szName));
			rkAccountInfo.bRace[i] = pAccountInfo->bRace[i];
			rkAccountInfo.bLevel[i] = pAccountInfo->bLevel[i];
			rkAccountInfo.wChatbanCount[i] = pAccountInfo->wChatbanCount[i];

			if (rkAccountInfo.dwPID[i])
			{
				m_map_BanAccountListByPlayerName[rkAccountInfo.szName[i]] = pAccountInfo->dwAID;
				m_map_BanAccountListByPlayerID[rkAccountInfo.dwPID[i]] = pAccountInfo->dwAID;
			}
		}
		rkAccountInfo.wAccountbanCount = pAccountInfo->wAccountbanCount;
		rkAccountInfo.SetDuration(pAccountInfo->dwDuration);

		m_map_BanAccountListByName[rkAccountInfo.szLoginName] = pAccountInfo->dwAID;
	}

	for (itertype(m_set_ActivePIDs) it = m_set_ActivePIDs.begin(); it != m_set_ActivePIDs.end(); ++it)
	{
		if (HasAllow(GetTeamRankByPID(*it), ALLOW_VIEW_BAN_ACCOUNT))
		{
			SendClientPacket(*it, GC_SUBHEADER_BAN_ACCOUNT_STATE, pAccountInfo, sizeof(TBanClientAccountInfo));
		}
	}
}

/*******************************************************************\
| [PUBLIC] Item Functions
\*******************************************************************/

void CAdminManager::SearchItem(LPCHARACTER pkChr, BYTE bSearchType, const char* c_pszData)
{
	sys_err("bSearchType: %d, c_pszData: %s", bSearchType, c_pszData);
	char szQuery[2048];
	int iQueryLen = snprintf(szQuery, sizeof(szQuery), "SELECT player.id, player.name, account.id, account.login, "
							 "item.id, item.window+0, item.pos, item.vnum, item.count, item.is_gm_owner, ");
	for (int i = 0; i < ITEM_SOCKET_MAX_NUM; ++i)
	{
		iQueryLen += snprintf(szQuery + iQueryLen, sizeof(szQuery) - iQueryLen, "item.socket%d, ", i);
	}
	for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
	{
		iQueryLen += snprintf(szQuery + iQueryLen, sizeof(szQuery) - iQueryLen, "item.attrtype%d, item.attrvalue%d, ", i, i);
	}
	iQueryLen += snprintf(szQuery + iQueryLen - 2, sizeof(szQuery) - iQueryLen + 2, " "
						  "FROM item LEFT JOIN player ON player.id = item.owner_id LEFT JOIN %s.account ON account.id = item.owner_id "
						  "LEFT JOIN item_proto ON item_proto.vnum = item.vnum WHERE", g_AccountDatabase.c_str());

	char szEscapedData[100];
	if (c_pszData)
	{
		DBManager::instance().EscapeString(szEscapedData, sizeof(szEscapedData), c_pszData, strlen(c_pszData));
	}

	sys_err("szEscapedData: %s", szEscapedData);

	switch (bSearchType)
	{
	case ITEM_SEARCH_IID:
		snprintf(szQuery, sizeof(szQuery), "%s item.id = '%s'", szQuery, szEscapedData);
		break;

	case ITEM_SEARCH_INAME:
		snprintf(szQuery, sizeof(szQuery), "%s item.id = '%s'", szQuery, szEscapedData);
		break;

	case ITEM_SEARCH_PID:
		snprintf(szQuery, sizeof(szQuery), "%s item.owner_id = '%s' AND item.window != %u AND item.window != %u", szQuery, szEscapedData, SAFEBOX, MALL);
		break;

	case ITEM_SEARCH_PNAME:
		snprintf(szQuery, sizeof(szQuery), "%s player.name = '%s'", szQuery, szEscapedData);
		break;

	case ITEM_SEARCH_GM_ITEM:
		snprintf(szQuery, sizeof(szQuery), "%s item.is_gm_owner = 1", szQuery);
		break;

	//selectowanie po graczu lub po reason
	default:
		sys_err("invalid search type %u", bSearchType);
		return;
	}

	sys_err("myQuerystr {%s]", szQuery);

	DBManager::instance().ReturnQuery(QID_ADMIN_MANAGER_ITEM_SEARCH, pkChr->GetPlayerID(), NULL, "%s", szQuery);
}

/*******************************************************************\
| [PUBLIC] Logs Functions
\*******************************************************************/

void CAdminManager::GetLogs(LPCHARACTER pkChr)
{
	char szQuery[1024];

	snprintf(szQuery, sizeof(szQuery), "SELECT name, SUBSTRING_INDEX(why, ' ', 1) as reason, COUNT(name) as COUNT, time FROM log.hack_log GROUP BY name, reason HAVING reason NOT IN ('MOV_SPEED_HACK', 'S_WALL_HACK') OR (reason IN ('MOV_SPEED_HACK', 'S_WALL_HACK') AND COUNT > 5) ORDER BY time DESC");

	DBManager::instance().ReturnQuery(QID_ADMIN_MANAGER_GET_LOG, pkChr->GetPlayerID(), NULL, "%s", szQuery);
}

/*******************************************************************\
| [PUBLIC] Client Packet Functions
\*******************************************************************/

void CAdminManager::SendLoadPacket(LPCHARACTER pkChr)
{
	TPacketGCAdminManagerLoad packet;
	packet.header = HEADER_GC_ADMIN_MANAGER_LOAD;

	// send allow flag
	packet.dwAllowFlag = GetAllow(pkChr);

	// get online player
	std::vector<TAdminManagerPlayerInfo> vec_PlayerInfo;
	vec_PlayerInfo.push_back(GetPlayerInfo(pkChr));
	packet.dwPlayerCount = GetPlayerInfoList(vec_PlayerInfo);
	packet.bIsGMItemTradeBlock = HasAllow(pkChr, ALLOW_GM_TRADE_BLOCK_OPTION) ? quest::CQuestManager::instance().GetEventFlag(EVENT_FLAG_GM_ITEM_TRADE_BLOCK) != 0 : false;
	packet.dwBanChatCount = m_map_BanChatList.size();
	packet.dwBanAccountCount = m_map_BanAccountList.size();
	// check right
	if (!HasAllow(pkChr, ALLOW_VIEW_ONLINE_COUNT))
	{
		packet.dwPlayerCount = 0;
	}
	if (!HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT))
	{
		packet.dwBanChatCount = 0;
	}
	if (!HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT))
	{
		packet.dwBanAccountCount = 0;
	}

	// set packet size
	packet.size = sizeof(TPacketGCAdminManagerLoad) +
				  sizeof(TAdminManagerPlayerInfo) * packet.dwPlayerCount +
				  sizeof(TBanClientPlayerInfo) * packet.dwBanChatCount +
				  sizeof(TBanClientAccountInfo) * packet.dwBanAccountCount;

	// create buffer with entire packet
	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (HasAllow(pkChr, ALLOW_VIEW_ONLINE_LIST))
	{
		buf.write(&vec_PlayerInfo[0], sizeof(TAdminManagerPlayerInfo) * packet.dwPlayerCount);
	}
	if (HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT))
	{
		for (itertype(m_map_BanChatList) it = m_map_BanChatList.begin(); it != m_map_BanChatList.end(); ++it)
		{
			buf.write(&GetBanClientPlayerInfo(it->first, &it->second), sizeof(TBanClientPlayerInfo));
		}
	}
	if (HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT))
	{
		for (itertype(m_map_BanAccountList) it = m_map_BanAccountList.begin(); it != m_map_BanAccountList.end(); ++it)
		{
			buf.write(&GetBanClientAccountInfo(it->first, &it->second), sizeof(TBanClientAccountInfo));
		}
	}

	// send
	pkChr->GetDesc()->Packet(buf.read_peek(), buf.size());
}

void CAdminManager::SendMapViewerLoadPacket(DWORD dwPID, long lMapIndex)
{
	LPSECTREE_MAP pkMap = SECTREE_MANAGER::instance().GetMap(lMapIndex);
	if (!pkMap)
	{
		sys_err("cannot send load packet pid %u [map %ld does not exists]", dwPID, lMapIndex);
		return;
	}

	DWORD dwTempCount = 0;

	TEMP_BUFFER buf;
	PIXEL_POSITION kBasePos;
	char* szWritePoint;

	// write base coord
	if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(lMapIndex, kBasePos))
	{
		sys_err("cannot get base position of map %ld (request pid %u)", lMapIndex, dwPID);
		return;
	}
	buf.write(&kBasePos.x, sizeof(kBasePos.x));
	buf.write(&kBasePos.y, sizeof(kBasePos.y));

	if (HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_MAPVIEWER_PLAYER))
	{
		// write player
		buf.write(&dwTempCount, sizeof(DWORD));
		FMapViewerLoadPlayer fLoadPlayer(buf);
		pkMap->for_each(fLoadPlayer);
		// update count
		szWritePoint = buf.getptr()->write_point - sizeof(TAdminManagerMapViewerPlayerInfo) * fLoadPlayer.m_dwCount - sizeof(DWORD);
		memcpy(szWritePoint, &fLoadPlayer.m_dwCount, sizeof(DWORD));
	}

	if (HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_MAPVIEWER_MONSTER | ALLOW_VIEW_MAPVIEWER_NPC | ALLOW_VIEW_MAPVIEWER_STONE))
	{
		// write mobs
		buf.write(&dwTempCount, sizeof(DWORD));
		FMapViewerLoadMob fLoadMob(buf, GetAllow(GetTeamRankByPID(dwPID)));
		pkMap->for_each(fLoadMob);
		// update count
		szWritePoint = buf.getptr()->write_point - sizeof(TAdminManagerMapViewerMobInfo) * fLoadMob.m_dwCount - sizeof(DWORD);
		memcpy(szWritePoint, &fLoadMob.m_dwCount, sizeof(DWORD));
	}

	// send packet
	SendClientPacket(dwPID, GC_SUBHEADER_MAPVIEWER_LOAD, buf.read_peek(), buf.size());
}

void CAdminManager::EncodeObserverItemPacket(TAdminManagerObserverItemInfo& rkItemInfo, LPITEM pkItem)
{
	rkItemInfo.id = pkItem->GetID();
	rkItemInfo.vnum = pkItem->GetVnum();
	rkItemInfo.count = pkItem->GetCount();
	rkItemInfo.cell = pkItem->GetCell();
	memcpy(rkItemInfo.alSockets, pkItem->GetSockets(), sizeof(rkItemInfo.alSockets));
	memcpy(rkItemInfo.aAttr, pkItem->GetAttributes(), sizeof(rkItemInfo.aAttr));
	rkItemInfo.is_gm_item = pkItem->IsGMOwner();
}

void CAdminManager::SendObserverLoadPacket(DWORD dwPID, LPCHARACTER pkPlayer)
{
	DWORD dwTempCount = 0;

	TEMP_BUFFER buf;
	char* szWritePoint;

	// main information
	DWORD dwPlayerPID = pkPlayer->GetPlayerID(); // pid
	buf.write(&dwPlayerPID, sizeof(DWORD));
	DWORD dwAccountID = pkPlayer->GetAID();
	buf.write(&dwAccountID, sizeof(DWORD));
	unsigned char bRaceNum = pkPlayer->GetRealRaceNum();
	buf.write(&bRaceNum, sizeof(unsigned char));
	int iAccountNameLen = strlen(pkPlayer->GetDesc()->GetAccountTable().login);
	buf.write(&iAccountNameLen, sizeof(int));
	buf.write(pkPlayer->GetDesc()->GetAccountTable().login, iAccountNameLen);
	const PIXEL_POSITION& kPos = pkPlayer->GetXYZ();
	buf.write(&kPos.x, sizeof(kPos.x)); // x-position
	buf.write(&kPos.y, sizeof(kPos.y)); // y-position

	// skills
	if (HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_OBSERVER_SKILL))
	{
		unsigned char bSkillGroup = pkPlayer->GetSkillGroup();
		buf.write(&bSkillGroup, sizeof(unsigned char));
		buf.write(pkPlayer->GetPlayerSkills(), sizeof(TPlayerSkill) * SKILL_MAX_NUM);
	}

	// points
	buf.write(&dwTempCount, sizeof(DWORD));
	int iPointCount = 0;
	for (int i = 0; i < POINT_MAX_NUM; ++i)
	{
		if (IsObserverUpdatePoint(i) && (i != POINT_GOLD || HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_OBSERVER_GOLD)))
		{
			unsigned char bType = i;
			buf.write(&bType, sizeof(unsigned char));
			int64_t iValue = pkPlayer->GetConvPoint(bType);
			buf.write(&iValue, sizeof(int64_t));
			++iPointCount;
		}
	}
	szWritePoint = buf.getptr()->write_point - (sizeof(unsigned char) + sizeof(int64_t)) * iPointCount - sizeof(DWORD);
	memcpy(szWritePoint, &iPointCount, sizeof(DWORD));

	// inventory
	if (HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_OBSERVER_INVENTORY | ALLOW_VIEW_OBSERVER_EQUIPMENT))
	{
		buf.write(&dwTempCount, sizeof(DWORD));
		int iItemCount = 0;
		for (int i = 0; i < INVENTORY_AND_EQUIP_SLOT_MAX; ++i)
		{
			bool bIsInventory = i < INVENTORY_MAX_NUM || i >= INVENTORY_MAX_NUM + WEAR_MAX_NUM;
			if ((bIsInventory && !HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_OBSERVER_INVENTORY)) ||
					(!bIsInventory && !HasAllow(GetTeamRankByPID(dwPID), ALLOW_VIEW_OBSERVER_EQUIPMENT)))
			{
				continue;
			}

			LPITEM pkItem = pkPlayer->GetInventoryItem(i);
			if (pkItem)
			{
				TAdminManagerObserverItemInfo kInfo;
				EncodeObserverItemPacket(kInfo, pkItem);
				buf.write(&kInfo, sizeof(kInfo));
				iItemCount++;
			}
		}
		szWritePoint = buf.getptr()->write_point - sizeof(TAdminManagerObserverItemInfo) * iItemCount - sizeof(DWORD);
		memcpy(szWritePoint, &iItemCount, sizeof(DWORD));
	}

	// ban
	CAffect* pAffBlockChat = pkPlayer->FindAffect(AFFECT_BLOCK_CHAT);
	DWORD dwChatBanTimeout = pAffBlockChat ? pAffBlockChat->lDuration : 0;
	buf.write(&dwChatBanTimeout, sizeof(DWORD));

	// send packet
	SendClientPacket(dwPID, GC_SUBHEADER_OBSERVER_LOAD, buf.read_peek(), buf.size());
}

void CAdminManager::SendObserverPacket(const std::set<DWORD>& rset_ObserverPIDs, unsigned char bSubHeader, const void* c_pvData, int iSize, DWORD dwCheckAllow)
{
	for (itertype(rset_ObserverPIDs) it = rset_ObserverPIDs.begin(); it != rset_ObserverPIDs.end(); ++it)
	{
		if (!dwCheckAllow || HasAllow(GetTeamRankByPID(*it), dwCheckAllow))
		{
			SendClientPacket(*it, bSubHeader, c_pvData, iSize);
		}
	}
}

void CAdminManager::SendClientPacket(LPDESC pkDesc, unsigned char bSubHeader, const void* c_pvData, int iSize)
{
	TPacketGCAdminManager packet;
	packet.header = HEADER_GC_ADMIN_MANAGER;
	packet.size = sizeof(packet) + iSize;
	packet.sub_header = bSubHeader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (iSize)
	{
		buf.write(c_pvData, iSize);
	}

	pkDesc->Packet(buf.read_peek(), buf.size());
}

void CAdminManager::SendClientPacket(DWORD dwPID, unsigned char bSubHeader, const void* c_pvData, int iSize)
{
	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);
	if (pkChr)
	{
		SendClientPacket(pkChr->GetDesc(), bSubHeader, c_pvData, iSize);
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pkCCI)
		{
			pkCCI->pkDesc->SetRelay(pkCCI->szName);
			SendClientPacket(pkCCI->pkDesc, bSubHeader, c_pvData, iSize);
		}
		else
		{
			sys_err("cannot send client packet to pid %u subheader %hu [cannot find player]", dwPID, bSubHeader);
		}
	}
}

void CAdminManager::SendClientPacket(unsigned char bSubHeader, const void* c_pvData, int iSize, DWORD dwCheckAllow)
{
	for (itertype(m_set_ActivePlayers) it = m_set_ActivePlayers.begin(); it != m_set_ActivePlayers.end(); ++it)
	{
		if (!dwCheckAllow || HasAllow(*it, dwCheckAllow))
		{
			SendClientPacket((*it)->GetDesc(), bSubHeader, c_pvData, iSize);
		}
	}
}

int CAdminManager::RecvClientPacket(LPCHARACTER pkChr, unsigned char bSubHeader, const char* c_pData, size_t uiBytes)
{
	if (m_set_ActivePlayers.find(pkChr) == m_set_ActivePlayers.end())
	{
		sys_err("cannot process admin packet by non-active-player %u %s (subheader %hu)", pkChr->GetPlayerID(), pkChr->GetName(), bSubHeader);
		return -1;
	}

	switch (bSubHeader)
	{
	case CG_SUBHEADER_MAPVIEWER_START:
	{
		if (uiBytes < sizeof(DWORD) + sizeof(DWORD))
		{
			return -1;
		}

		DWORD dwBaseX = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		DWORD dwBaseY = *(DWORD*) c_pData;
		long lMapIndex = SECTREE_MANAGER::instance().GetMapIndex(dwBaseX, dwBaseY);

		if (HasAllow(pkChr, ALLOW_MAPVIEWER))
		{
			if (lMapIndex == 0)
			{
				pkChr->ChatPacket(CHAT_TYPE_INFO, "Invalid map.");
				sys_err("cannot find map by base coords (%u, %u)", dwBaseX, dwBaseY);
			}
			else
			{
				StartMapViewer(pkChr, lMapIndex);
			}
		}
		else
		{
			sys_err("%s is not allowed to start the mapviewer", pkChr->GetName());
		}

		return sizeof(DWORD) + sizeof(DWORD);
	}

	case CG_SUBHEADER_MAPVIEWER_STOP:
		StopMapViewer(pkChr->GetPlayerID());
		return 0;

	case CG_SUBHEADER_OBSERVER_START:
	{
		if (uiBytes < sizeof(bool) + sizeof(int))
		{
			return -1;
		}

		bool bSendFailMessage = *(bool*) c_pData;
		uiBytes -= sizeof(bool);
		c_pData += sizeof(bool);

		int iLen = *(int*) c_pData;
		uiBytes -= sizeof(int);
		c_pData += sizeof(int);

		if (uiBytes < iLen)
		{
			return -1;
		}

		if (HasAllow(pkChr, ALLOW_OBSERVER))
		{
			char szPlayerName[CHARACTER_NAME_MAX_LEN + 1];
			strlcpy(szPlayerName, c_pData, MIN(iLen, CHARACTER_NAME_MAX_LEN) + 1);

			StartObserver(pkChr, szPlayerName, bSendFailMessage);
		}

		return sizeof(bool) + sizeof(int) + iLen;
	}

	case CG_SUBHEADER_OBSERVER_STOP:
		StopObserver(pkChr->GetPlayerID());
		return 0;

	case CG_SUBHEADER_BAN_CHAT_SEARCH:
	{
		if (uiBytes < sizeof(int))
		{
			return -1;
		}

		int iNameLen = *(int*) c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		if (iNameLen > CHARACTER_NAME_MAX_LEN)
		{
			return -1;
		}

		if (uiBytes < iNameLen)
		{
			return -1;
		}

		char* szName = new char[iNameLen + 1];
		thecore_memcpy(szName, c_pData, iNameLen);
		szName[iNameLen] = '\0';

		if (HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT))
		{
			SearchBanChatPlayer(pkChr, szName);
		}

		delete[] szName;

		return sizeof(int) + iNameLen;
	}

	case CG_SUBHEADER_BAN_REQUEST_LOG:
	{
		;
		if (uiBytes < sizeof(BYTE))
		{
			return -1;
		}

		BYTE bType = *(BYTE*) c_pData;
		c_pData += sizeof(BYTE);
		uiBytes -= sizeof(BYTE);

		if (uiBytes < sizeof(DWORD))
		{
			return -1;
		}

		DWORD dwID = *(DWORD*) c_pData;

		if (bType == BAN_TYPE_CHAT && HasAllow(pkChr, ALLOW_VIEW_BAN_CHAT_LOG))
		{
			SendBanPlayerLog(pkChr, dwID);
		}
		else if (bType == BAN_TYPE_ACCOUNT && HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT_LOG))
		{
			SendBanAccountLog(pkChr, dwID);
		}

		return sizeof(BYTE) + sizeof(DWORD);
	}
	break;

	case CG_SUBHEADER_BAN_ACCOUNT_SEARCH:
	{
		if (uiBytes < sizeof(BYTE) + sizeof(int))
		{
			return -1;
		}

		BYTE bSearchType = *(BYTE*) c_pData;
		c_pData += sizeof(BYTE);
		uiBytes -= sizeof(BYTE);

		int iNameLen = *(int*) c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		if (iNameLen > (CHARACTER_NAME_MAX_LEN > LOGIN_MAX_LEN ? CHARACTER_NAME_MAX_LEN : LOGIN_MAX_LEN))
		{
			return -1;
		}

		if (uiBytes < iNameLen)
		{
			return -1;
		}

		char* szName = new char[iNameLen + 1];
		thecore_memcpy(szName, c_pData, iNameLen);
		szName[iNameLen] = '\0';

		if (HasAllow(pkChr, ALLOW_VIEW_BAN_ACCOUNT))
		{
			SearchBanAccount(pkChr, szName, bSearchType);
		}

		delete[] szName;

		return sizeof(BYTE) + sizeof(int) + iNameLen;
	}
	break;

	case CG_SUBHEADER_BAN_ACCOUNT:
	{
		if (uiBytes < sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(BYTE) + sizeof(int))
		{
			return -1;
		}

		DWORD dwAID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		uiBytes -= sizeof(DWORD);

		DWORD dwPlayerID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		uiBytes -= sizeof(DWORD);

		DWORD dwTimeLeft = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		uiBytes -= sizeof(DWORD);

		bool bIncreaseBanCounter = *(bool*) c_pData;
		c_pData += sizeof(bool);
		uiBytes -= sizeof(bool);

		int iLen1 = *(int*) c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		if (uiBytes < iLen1 || iLen1 > 512)
		{
			return -1;
		}

		char* szReason = new char[iLen1 + 1];
		thecore_memcpy(szReason, c_pData, iLen1);
		szReason[iLen1] = '\0';
		c_pData += iLen1;
		uiBytes -= iLen1;

		if (uiBytes < sizeof(int))
		{
			return -1;
		}

		int iLen2 = *(int*) c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		if (uiBytes < iLen2 || iLen2 > 512)
		{
			return -1;
		}

		char* szProof = new char[iLen2 + 1];
		thecore_memcpy(szProof, c_pData, iLen2);
		szProof[iLen2] = '\0';

		if (HasAllow(pkChr, ALLOW_BAN_ACCOUNT))
		{
			DoAccountBan(pkChr, dwAID, dwPlayerID, dwTimeLeft, szReason, szProof, bIncreaseBanCounter);
		}

		delete[] szReason;
		delete[] szProof;

		return sizeof(DWORD) + sizeof(DWORD) + sizeof(DWORD) + sizeof(bool) + sizeof(int) + iLen1 + sizeof(int) + iLen2;
	}
	break;

	case CG_SUBHEADER_ITEM_SEARCH:
	{
		if (uiBytes < sizeof(BYTE) + sizeof(int))
		{
			return -1;
		}

		BYTE bSearchType = *(BYTE*) c_pData;
		c_pData += sizeof(BYTE);
		uiBytes -= sizeof(BYTE);

		int iDataLen = *(int*) c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		if (iDataLen > (CHARACTER_NAME_MAX_LEN > LOGIN_MAX_LEN ? CHARACTER_NAME_MAX_LEN : LOGIN_MAX_LEN))
		{
			return -1;
		}

		if (iDataLen < 0 || (iDataLen == 0 && bSearchType != ITEM_SEARCH_GM_ITEM))
		{
			return -1;
		}

		if (uiBytes < iDataLen)
		{
			return -1;
		}

		char* szData = NULL;
		if (iDataLen)
		{
			szData = new char[iDataLen + 1];
			thecore_memcpy(szData, c_pData, iDataLen);
			szData[iDataLen] = '\0';
		}
		// if (HasAllow(pkChr, ALLOW_VIEW_ITEM))
		SearchItem(pkChr, bSearchType, szData);

		if (szData)
		{
			delete[] szData;
		}

		return sizeof(BYTE) + sizeof(int) + iDataLen;
	}
	break;

	case CG_SUBHEADER_GET_LOGS:
	{
		GetLogs(pkChr);
		return 0;
	}
	break;
	}

	sys_err("cannot process client packet subheader %hu (player %u %s)", bSubHeader, pkChr->GetPlayerID(), pkChr->GetName());
	return -1;
}

/*******************************************************************\
| [PUBLIC] P2P Functions
\*******************************************************************/

void CAdminManager::SendP2P(LPDESC pkDesc, unsigned char bSubHeader, const void* c_pvData, int iSize)
{
	TPacketGGAdminManager packet;
	packet.header = HEADER_GG_ADMIN_MANAGER;
	packet.sub_header = bSubHeader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (iSize)
	{
		buf.write(c_pvData, iSize);
	}

	if (pkDesc)
	{
		pkDesc->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		P2P_MANAGER::instance().Send(buf.read_peek(), buf.size());
	}
}

int CAdminManager::RecvP2P(unsigned char bSubHeader, const char* c_pData)
{
	if (test_server)
	{
		sys_log(0, "CAdminManager::RecvP2P %u", bSubHeader);
	}

	switch (bSubHeader)
	{
	case P2P_SUBHEADER_LOGOUT_PID:
	{
		OnLogoutPlayer(*(DWORD*) c_pData);
		return sizeof(DWORD);
	}
	break;

	case P2P_SUBHEADER_START_MAPVIEWER:
	{
		DWORD dwPID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		long lMapIndex = *(long*) c_pData;

		StartMapViewer(dwPID, lMapIndex);

		return sizeof(DWORD) + sizeof(long);
	}
	break;

	case P2P_SUBHEADER_STOP_MAPVIEWER:
	{
		StopMapViewer(*(DWORD*) c_pData);
		return sizeof(DWORD);
	}
	break;

	case P2P_SUBHEADER_START_OBSERVER:
	{
		DWORD dwPID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		DWORD dwPlayerPID = *(DWORD*) c_pData;

		StartObserver(dwPID, dwPlayerPID);

		return sizeof(DWORD) + sizeof(DWORD);
	}
	break;

	case P2P_SUBHEADER_STOP_OBSERVER:
	{
		StopObserver(*(DWORD*) c_pData);
		return sizeof(DWORD);
	}
	break;

	case P2P_SUBHEADER_STOP_OBSERVER_BY_FORCE:
	{
		StopObserver(*(DWORD*) c_pData, true);
		return sizeof(DWORD);
	}
	break;

	case P2P_SUBHEADER_BAN_PLAYER_CHATBAN:
	{
		TBanClientPlayerInfo* pPlayerInfo = (TBanClientPlayerInfo *) c_pData;

		Ban_OnPlayerChatban(pPlayerInfo);
		return sizeof(TBanClientPlayerInfo);
	}
	break;

	case P2P_SUBHEADER_BAN_CHAT_SEARCH_PLAYER:
	{
		DWORD dwGMPID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		int iNameLen = *(int*) c_pData;
		c_pData += sizeof(int);
		char* szName = new char[iNameLen + 1];
		thecore_memcpy(szName, c_pData, iNameLen);
		szName[iNameLen] = '\0';

		SearchBanChatPlayer(dwGMPID, szName);
		delete[] szName;

		return sizeof(DWORD) + sizeof(int) + iNameLen;
	}
	break;

	case P2P_SUBHEADER_BAN_ACCOUNT_REQUEST:
	{
		DWORD dwGMPid = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		BYTE bRequestorGMLevel = *(BYTE*) c_pData;
		c_pData += sizeof(BYTE);
		DWORD dwAID = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		int iLoginNameLen = *(int*) c_pData;
		c_pData += sizeof(int);
		char* szLoginName = new char[iLoginNameLen + 1];
		thecore_memcpy(szLoginName, c_pData, iLoginNameLen);
		szLoginName[iLoginNameLen] = '\0';
		c_pData += iLoginNameLen;
		DWORD dwTimeLeft = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		bool bIncreaseBanCounter = *(bool*) c_pData;

		P2P_DoAccountBan(dwGMPid, bRequestorGMLevel, dwAID, szLoginName, dwTimeLeft, bIncreaseBanCounter);
		delete[] szLoginName;

		return sizeof(DWORD) + sizeof(BYTE) + sizeof(DWORD) + sizeof(int) + iLoginNameLen + sizeof(DWORD) + sizeof(bool);
	}
	break;

	case P2P_SUBHEADER_BAN_ACCOUNT_DISCONNECT:
	{
		;
		int iLoginNameLen = *(int*) c_pData;
		c_pData += sizeof(int);
		char* szLoginName = new char[iLoginNameLen + 1];
		thecore_memcpy(szLoginName, c_pData, iLoginNameLen);
		szLoginName[iLoginNameLen] = '\0';

		P2P_DoAccountBanDisconnect(szLoginName);
		delete[] szLoginName;

		return sizeof(int) + iLoginNameLen;
	}
	break;

	case P2P_SUBHEADER_BAN_ACCOUNTBAN:
	{
		TBanClientAccountInfo* pData = (TBanClientAccountInfo*) c_pData;

		OnPlayerAccountban(pData);
		return sizeof(TBanClientAccountInfo);
	}
	break;

	case P2P_SUBHEADER_BAN_ACCOUNT_SEARCH:
	{
		DWORD dwGMPid = *(DWORD*) c_pData;
		c_pData += sizeof(DWORD);
		int iLen = *(int*) c_pData;
		c_pData += sizeof(int);
		char* szName = new char[iLen + 1];
		thecore_memcpy(szName, c_pData, iLen);
		szName[iLen] = '\0';
		c_pData += iLen;
		BYTE bSearchType = *(BYTE*) c_pData;

		SearchBanAccount(dwGMPid, szName, bSearchType);
		delete[] szName;

		return sizeof(DWORD) + sizeof(int) + iLen + sizeof(BYTE);
	}
	break;
	}

	sys_err("cannot process subheader %u", bSubHeader);
	return -1;
}

/*******************************************************************\
| [PUBLIC] Event Functions
\*******************************************************************/

void CAdminManager::OnLoginPlayer(LPCHARACTER pkChr)
{
	// update online player list
	SendClientPacket(GC_SUBHEADER_PLAYER_ONLINE, &GetPlayerInfo(pkChr), sizeof(TAdminManagerPlayerInfo), ALLOW_VIEW_ONLINE_LIST | ALLOW_VIEW_ONLINE_COUNT);

	// events
	MapViewer_OnPlayerLogin(pkChr);
	Ban_OnPlayerLogin(pkChr->GetPlayerID());

	// only allow the admin manager for gms - also on test_server !
	if (!GM::check_allow(pkChr->GetGMLevel(true), GM_ALLOW_ADMIN_MANAGER))
	{
		return;
	}

	m_set_ActivePlayers.insert(pkChr);
	m_set_ActivePIDs.insert(pkChr->GetPlayerID());

	// load packet
	SendLoadPacket(pkChr);
}

void CAdminManager::OnP2PLoginPlayer(DWORD dwPID)
{
	// event
	Ban_OnPlayerLogin(dwPID);

	// get player
	CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID);
	if (!pkCCI)
	{
		sys_err("cannot get cci by pid %u", dwPID);
		return;
	}

	SendClientPacket(GC_SUBHEADER_PLAYER_ONLINE, &GetPlayerInfo(pkCCI), sizeof(TAdminManagerPlayerInfo), ALLOW_VIEW_ONLINE_LIST | ALLOW_VIEW_ONLINE_COUNT);
}

void CAdminManager::OnLogoutPlayer(LPCHARACTER pkChr)
{
	// events
	MapViewer_OnPlayerLogout(pkChr);

	// normal handling
	OnLogoutPlayer(pkChr->GetPlayerID());
}

void CAdminManager::OnLogoutPlayer(DWORD dwPID)
{
	Ban_OnPlayerLogout(dwPID);

	{
		itertype(m_map_ObserverSetByPID) it = m_map_ObserverSetByPID.find(dwPID);
		if (it != m_map_ObserverSetByPID.end() && it->second.size() > 0)
		{
			do
			{
				DWORD dwPID = *(it->second.begin());
				if (StopObserver(dwPID, true))
				{
					break;
				}
			} while (it->second.size() > 0);
		}
	}

	{
		itertype(m_set_ActivePIDs) it = m_set_ActivePIDs.find(dwPID);
		if (it != m_set_ActivePIDs.end())
		{
			// stop mapviewer
			if (IsRunningMapViewer(dwPID))
			{
				StopMapViewer(dwPID);
			}

			// stop observer
			if (IsRunningObserver(dwPID))
			{
				StopObserver(dwPID);
			}

			// remove pid for all p2p connections which are connected to the player somehow
			itertype(m_map_PlayerP2PConnections) p2p_it = m_map_PlayerP2PConnections.find(dwPID);
			if (p2p_it != m_map_PlayerP2PConnections.end())
			{
				for (itertype(p2p_it->second) desc_it = p2p_it->second.begin(); desc_it != p2p_it->second.end(); ++desc_it)
				{
					SendP2P(*desc_it, P2P_SUBHEADER_LOGOUT_PID, &dwPID, sizeof(DWORD));
				}

				m_map_PlayerP2PConnections.erase(p2p_it);
			}

			// remove player if exists
			LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);
			if (pkChr)
			{
				m_set_ActivePlayers.erase(pkChr);
			}

			m_set_ActivePIDs.erase(it);
		}
	}

	SendClientPacket(GC_SUBHEADER_PLAYER_OFFLINE, &dwPID, sizeof(DWORD), ALLOW_VIEW_ONLINE_LIST | ALLOW_VIEW_ONLINE_COUNT);
}

void CAdminManager::OnP2PLogoutPlayer(DWORD dwPID)
{
	// event
	Ban_OnPlayerLogout(dwPID);

	SendClientPacket(GC_SUBHEADER_PLAYER_OFFLINE, &dwPID, sizeof(DWORD), ALLOW_VIEW_ONLINE_LIST | ALLOW_VIEW_ONLINE_COUNT);
}

void CAdminManager::OnP2PDisconnect(LPDESC pkDesc)
{
	// remove each p2p peer pointer that refers to the disconnected peer from the list
	for (itertype(m_map_PlayerP2PConnections) it = m_map_PlayerP2PConnections.begin(); it != m_map_PlayerP2PConnections.end(); ++it)
	{
		itertype(it->second) desc_it = it->second.find(pkDesc);
		if (desc_it != it->second.end())
		{
			it->second.erase(desc_it);
		}
	}
}

void CAdminManager::OnPlayerMove(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnPlayerMove(pkChr);
}

void CAdminManager::OnPlayerStateChange(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnPlayerStateChange(pkChr);
}

void CAdminManager::OnMonsterCreate(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnMonsterCreate(pkChr);
}

void CAdminManager::OnMonsterDestroy(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnMonsterDestroy(pkChr);
}

void CAdminManager::OnMonsterMove(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnMonsterMove(pkChr);
}

void CAdminManager::OnMonsterStateChange(LPCHARACTER pkChr)
{
	// mapviewer
	MapViewer_OnMonsterStateChange(pkChr);
}

void CAdminManager::OnPlayerSkillGroupChange(LPCHARACTER pkChr)
{
	// observer
	Observer_OnPlayerSkillGroupChange(pkChr);
}

void CAdminManager::OnPlayerSkillChange(LPCHARACTER pkChr, DWORD dwSkillVnum)
{
	// observer
	Observer_OnPlayerSkillChange(pkChr, dwSkillVnum);
}

void CAdminManager::OnPlayerSkillCooldown(LPCHARACTER pkChr, DWORD dwSkillVnum, DWORD dwCoolTime)
{
	// observer
	Observer_OnPlayerSkillCooldown(pkChr, dwSkillVnum, dwCoolTime);
}

void CAdminManager::OnPlayerPointChange(LPCHARACTER pkChr, unsigned char bType, int64_t llValue)
{
	// observer
	Observer_OnPlayerPointChange(pkChr, bType, llValue);
}

void CAdminManager::OnPlayerItemSet(LPCHARACTER pkChr, WORD wCell)
{
	// observer
	Observer_OnPlayerItemSet(pkChr, wCell);
}

void CAdminManager::OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft)
{
	// observer
	Observer_OnPlayerChatban(pkChr, dwTimeLeft);
	// ban
	Ban_OnPlayerChatban(pkChr, dwTimeLeft);
}

void CAdminManager::OnPlayerChatban(const char* c_pszName, DWORD dwTimeLeft, bool bIncreaseBanCounter)
{
	// ban
	Ban_OnPlayerChatban(c_pszName, dwTimeLeft, bIncreaseBanCounter);
}

void CAdminManager::OnPlayerAccountban(const TBanClientAccountInfo* pAccountInfo)
{
	// observer
	for (int i = 0; i < PLAYER_PER_ACCOUNT; ++i)
	{
		if (pAccountInfo->dwPID[i])
		{
			Observer_OnPlayerAccountban(pAccountInfo->dwPID[i], pAccountInfo->dwDuration);
		}
	}
	// ban
	Ban_OnPlayerAccountban(pAccountInfo);
}

void CAdminManager::OnEventFlagChange(const std::string& c_rstFlagName, int iNewValue)
{
	if (c_rstFlagName == EVENT_FLAG_GM_ITEM_TRADE_BLOCK)
	{
		bool bIsActive = iNewValue != 0;
		SendClientPacket(GC_SUBHEADER_GM_ITEM_TRADE_BLOCK, &bIsActive, sizeof(bool), ALLOW_GM_TRADE_BLOCK_OPTION);
	}
}

void CAdminManager::OnRefreshHackLog(LPCHARACTER pkChr)
{
	//Hack Log Refresh
	GetLogs(pkChr);
}
#endif
