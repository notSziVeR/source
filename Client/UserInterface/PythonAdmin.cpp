#include "StdAfx.h"

#ifdef ENABLE_ADMIN_MANAGER
#include "PythonAdmin.h"
#include "PythonBackground.h"
#include "PythonMiniMap.h"
#include "PythonApplication.h"

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CPythonAdmin - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CPythonAdmin::CPythonAdmin()
{
	Initialize();
}

CPythonAdmin::~CPythonAdmin()
{
}

void CPythonAdmin::Initialize()
{
	m_poGameWindow = NULL;
	m_bCanOpenAdminWindow = true;
	m_dwAllowFlag = 0;
	m_iOnlinePlayerSortType = -1;
	m_bOnlinePlayerDirection = 0;
	m_map_OnlinePlayer.clear();
	m_vec_OnlinePlayerSorted.clear();
	m_bGMItemTradeBlock = false;
	MapViewer_Clear();
	Observer_Clear();
	Ban_Clear();
}

void CPythonAdmin::Clear()
{
	m_poGameWindow = NULL;
	m_bCanOpenAdminWindow = false;
	m_dwAllowFlag = 0;
	m_iOnlinePlayerSortType = -1;
	m_bOnlinePlayerDirection = 0;
	m_map_OnlinePlayer.clear();
	m_vec_OnlinePlayerSorted.clear();
	m_bGMItemTradeBlock = false;
	MapViewer_Clear();
	Observer_Clear();
	Ban_Clear();
}

/*******************************************************************\
| [PUBLIC] General Sorting Functions
\*******************************************************************/

template <typename _NumberType, int iIgnoreBytes = 0> int General_ComparePlayerByNumber(const void* pInfo1, const void* pInfo2)
{
	_NumberType& num1 = *(_NumberType*)(((const char*)pInfo1) + iIgnoreBytes);
	_NumberType& num2 = *(_NumberType*)(((const char*)pInfo2) + iIgnoreBytes);
	BYTE bSortDirection = CPythonAdmin::Instance().GetOnlinePlayerSortDirection();

	if (num1 < num2)
	{
		return bSortDirection == 0 ? -1 : 1;
	}
	else if (num1 > num2)
	{
		return bSortDirection == 0 ? 1 : -1;
	}
	else
	{
		return 0;
	}
}

int General_ComparePlayerByName(const void* pInfo1, const void* pInfo2)
{
	const TAdminManagerPlayerInfo* pkInfo1 = (const TAdminManagerPlayerInfo*)pInfo1;
	const TAdminManagerPlayerInfo* pkInfo2 = (const TAdminManagerPlayerInfo*)pInfo2;
	BYTE bSortDirection = CPythonAdmin::Instance().GetOnlinePlayerSortDirection();

	int i = 0;
	char c1, c2;

	while (true)
	{
		c1 = pkInfo1->szName[i];
		if (c1 >= 'A' && c1 <= 'Z')
		{
			c1 = c1 - ('A' - 'a');
		}

		c2 = pkInfo2->szName[i];
		if (c2 >= 'A' && c2 <= 'Z')
		{
			c2 = c2 - ('A' - 'a');
		}

		if (c1 == '\0' && c2 == '\0')
		{
			return 0;
		}

		if (c1 < c2)
		{
			return bSortDirection == 0 ? -1 : 1;
		}
		else if (c1 > c2)
		{
			return bSortDirection == 0 ? 1 : -1;
		}

		++i;
	}
}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

bool CPythonAdmin::CanOpenAdminWindow()
{
	return m_bCanOpenAdminWindow;
}

void CPythonAdmin::SetGameWindow(PyObject* poWindow)
{
	m_poGameWindow = poWindow;
}

void CPythonAdmin::SetAllowFlag(DWORD dwAllowFlag)
{
	m_dwAllowFlag = dwAllowFlag;
}

DWORD CPythonAdmin::GetAllowFlag() const
{
	return m_dwAllowFlag;
}

bool CPythonAdmin::HasAllow(DWORD dwAllowFlag) const
{
	return (GetAllowFlag() & dwAllowFlag) != 0;
}

bool CPythonAdmin::AddOnlinePlayer(const TAdminManagerPlayerInfo* c_pPlayerInfo)
{
	Ban_SetPlayerOnline(c_pPlayerInfo->dwPID);

	bool bExists = m_map_OnlinePlayer.find(c_pPlayerInfo->dwPID) != m_map_OnlinePlayer.end();
	memcpy(&m_map_OnlinePlayer[c_pPlayerInfo->dwPID], c_pPlayerInfo, sizeof(TAdminManagerPlayerInfo));

	if (!bExists)
	{
		m_vec_OnlinePlayerSorted.push_back(*c_pPlayerInfo);

		if (m_iOnlinePlayerSortType != -1)
		{
			SortOnlinePlayer(m_iOnlinePlayerSortType, m_bOnlinePlayerDirection);
		}
	}

	return bExists;
}

bool CPythonAdmin::RemoveOnlinePlayer(DWORD dwPID)
{
	Ban_SetPlayerOffline(dwPID);

	auto it = m_map_OnlinePlayer.find(dwPID);
	if (it != m_map_OnlinePlayer.end())
	{
		m_map_OnlinePlayer.erase(it);
		for (int i = 0; i < m_vec_OnlinePlayerSorted.size(); ++i)
		{
			if (m_vec_OnlinePlayerSorted[i].dwPID == dwPID)
			{
				m_vec_OnlinePlayerSorted.erase(m_vec_OnlinePlayerSorted.begin() + i);
				break;
			}
		}

		return true;
	}

	return false;
}

DWORD CPythonAdmin::GetOnlinePlayerCount()
{
	return m_map_OnlinePlayer.size();
}

const TAdminManagerPlayerInfo* CPythonAdmin::GetOnlinePlayerByIndex(DWORD dwIndex)
{
	auto it = m_map_OnlinePlayer.begin();
	if (it == m_map_OnlinePlayer.end())
	{
		return NULL;
	}

	for (DWORD i = 0; i < dwIndex; ++i)
	{
		if (++it == m_map_OnlinePlayer.end())
		{
			return NULL;
		}
	}

	return &it->second;
}

const TAdminManagerPlayerInfo* CPythonAdmin::GetOnlinePlayerByPID(DWORD dwPID)
{
	auto it = m_map_OnlinePlayer.find(dwPID);
	if (it == m_map_OnlinePlayer.end())
	{
		return NULL;
	}

	return &it->second;
}

void CPythonAdmin::SortOnlinePlayer(BYTE bSortType, BYTE bDirection)
{
	int (* pSortFunc)(const void*, const void*);

	switch (bSortType)
	{
	case GENERAL_ONLINE_PLAYER_SORT_BY_PID:
		pSortFunc = &General_ComparePlayerByNumber<int, 0>;
		break;

	case GENERAL_ONLINE_PLAYER_SORT_BY_NAME:
		pSortFunc = &General_ComparePlayerByName;
		break;

	case GENERAL_ONLINE_PLAYER_SORT_BY_MAP_INDEX:
		pSortFunc = &General_ComparePlayerByNumber < LONG, sizeof(DWORD) + CHARACTER_NAME_MAX_LEN + 1 >;
		break;

	case GENERAL_ONLINE_PLAYER_SORT_BY_CHANNEL:
		pSortFunc = &General_ComparePlayerByNumber < BYTE, sizeof(DWORD) + CHARACTER_NAME_MAX_LEN + 1 + sizeof(LONG) >;
		break;

	case GENERAL_ONLINE_PLAYER_SORT_BY_EMPIRE:
		pSortFunc = &General_ComparePlayerByNumber < BYTE, sizeof(DWORD) + CHARACTER_NAME_MAX_LEN + 1 + sizeof(LONG) + sizeof(BYTE) >;
		break;

	default:
		TraceError("invalid sort type %u", bSortType);
		return;
	}

	m_iOnlinePlayerSortType = bSortType;
	m_bOnlinePlayerDirection = bDirection;

	std::qsort(&m_vec_OnlinePlayerSorted[0], m_vec_OnlinePlayerSorted.size(), sizeof(TAdminManagerPlayerInfo), pSortFunc);
}

bool CPythonAdmin::IsOnlinePlayerSorted() const
{
	return m_iOnlinePlayerSortType != -1;
}

const TAdminManagerPlayerInfo* CPythonAdmin::GetSortOnlinePlayerByIndex(DWORD dwIndex)
{
	return &m_vec_OnlinePlayerSorted[dwIndex];
}

void CPythonAdmin::SetGMItemTradeBlock(bool bIsBlock)
{
	m_bGMItemTradeBlock = bIsBlock;
}

bool CPythonAdmin::IsGMItemTradeBlock() const
{
	return m_bGMItemTradeBlock;
}

/*******************************************************************\
| [PUBLIC] MapViewer Functions
\*******************************************************************/

void CPythonAdmin::MapViewer_Clear()
{
	m_bMapViewer_ShowMap = false;
	m_stMapViewer_MapName = "";
	m_map_MapViewerPlayer.clear();
	m_map_MapViewerMob.clear();
}

void CPythonAdmin::MapViewer_LoadMapInfo(DWORD dwBaseX, DWORD dwBaseY)
{
	CMapManager::TMapInfo* pkInfo = CPythonBackground::Instance().GlobalPositionToMapInfo(dwBaseX, dwBaseY);
	if (!pkInfo)
	{
		m_bMapViewer_ShowMap = false;
		m_stMapViewer_MapName = "";
	}
	else
	{
		m_bMapViewer_ShowMap = true;

		m_stMapViewer_MapName = pkInfo->m_strName;
		m_dwMapViewer_MapBaseX = pkInfo->m_dwBaseX;
		m_dwMapViewer_MapBaseY = pkInfo->m_dwBaseY;
		m_dwMapViewer_MapSizeX = pkInfo->m_dwSizeX;
		m_dwMapViewer_MapSizeY = pkInfo->m_dwSizeY;

		CPythonMiniMap::Instance().LoadAdminManagerAtlas();
	}
}

const std::string& CPythonAdmin::MapViewer_GetMapName()
{
	return m_stMapViewer_MapName;
}

DWORD CPythonAdmin::MapViewer_GetBaseX()
{
	return m_dwMapViewer_MapBaseX;
}

DWORD CPythonAdmin::MapViewer_GetBaseY()
{
	return m_dwMapViewer_MapBaseY;
}

DWORD CPythonAdmin::MapViewer_GetSizeX()
{
	return m_dwMapViewer_MapSizeX;
}

DWORD CPythonAdmin::MapViewer_GetSizeY()
{
	return m_dwMapViewer_MapSizeY;
}

void CPythonAdmin::MapViewer_AddPlayer(const TAdminManagerMapViewerPlayerInfo& rkPlayer)
{
	m_map_MapViewerPlayer[rkPlayer.dwPID] = rkPlayer;
}

void CPythonAdmin::MapViewer_ErasePlayer(DWORD dwPID)
{
	auto it = m_map_MapViewerPlayer.find(dwPID);
	if (it == m_map_MapViewerPlayer.end())
	{
		TraceError("cannot erase player by pid %u", dwPID);
		return;
	}
	m_map_MapViewerPlayer.erase(it);
}

void CPythonAdmin::MapViewer_AddMonster(const TAdminManagerMapViewerMobInfo& rkMob)
{
	m_map_MapViewerMob[rkMob.dwVID] = rkMob;
}

void CPythonAdmin::MapViewer_EraseMonster(DWORD dwVID)
{
	auto it = m_map_MapViewerMob.find(dwVID);
	if (it == m_map_MapViewerMob.end())
	{
		TraceError("cannot erase mob by vid %u", dwVID);
		return;
	}
	m_map_MapViewerMob.erase(it);
}

TAdminManagerMapViewerPlayerInfo* CPythonAdmin::MapViewer_GetPlayerByPID(DWORD dwPID)
{
	auto it = m_map_MapViewerPlayer.find(dwPID);
	if (it == m_map_MapViewerPlayer.end())
	{
		return NULL;
	}
	return &it->second;
}

const std::map<DWORD, TAdminManagerMapViewerPlayerInfo>& CPythonAdmin::MapViewer_GetPlayer()
{
	return m_map_MapViewerPlayer;
}

TAdminManagerMapViewerMobInfo* CPythonAdmin::MapViewer_GetMonsterByVID(DWORD dwVID)
{
	auto it = m_map_MapViewerMob.find(dwVID);
	if (it == m_map_MapViewerMob.end())
	{
		return NULL;
	}
	return &it->second;
}

const std::map<DWORD, TAdminManagerMapViewerMobInfo>& CPythonAdmin::MapViewer_GetMonster()
{
	return m_map_MapViewerMob;
}

/*******************************************************************\
| [PUBLIC] Observer Functions
\*******************************************************************/

void CPythonAdmin::Observer_Clear(bool bClearMessages)
{
	m_bObserver_Running = false;
	m_dwObserver_PID = 0;
	m_dwObserver_AID = 0;
	m_stObserver_LoginName = "";
	m_bObserver_RaceNum = 0;
	m_stObserver_MapName = "";
	m_bObserver_SkillGroup = 0;
	memset(&m_akObserver_Skills[0], 0, sizeof(m_akObserver_Skills));
	memset(&m_adwObserver_SkillStartCoolTimes[0], 0, sizeof(m_adwObserver_SkillStartCoolTimes));
	memset(&m_adwObserver_SkillCoolTimes[0], 0, sizeof(m_adwObserver_SkillCoolTimes));
	m_map_ObserverPoints.clear();
	memset(&m_akObserver_Item[0], 0, sizeof(m_akObserver_Item));
	if (bClearMessages)
	{
		m_map_ObserverWhispers.clear();
	}
	m_dwObserverChatBanTimeout = 0;
	m_dwObserverAccountBanTimeout = 0;
}

void CPythonAdmin::Observer_StopRunning()
{
	m_bObserver_Running = false;
}

bool CPythonAdmin::Observer_IsRunning() const
{
	return m_bObserver_Running;
}

bool CPythonAdmin::Observer_IsStoppedByForce() const
{
	if (Observer_IsRunning())
	{
		return false;
	}

	return m_dwObserver_PID != 0;
}

void CPythonAdmin::Observer_SetPID(DWORD dwPID)
{
	m_bObserver_Running = true;
	m_dwObserver_PID = dwPID;
}

DWORD CPythonAdmin::Observer_GetPID() const
{
	return m_dwObserver_PID;
}

void CPythonAdmin::Observer_SetAID(DWORD dwAID)
{
	m_dwObserver_AID = dwAID;
}

DWORD CPythonAdmin::Observer_GetAID() const
{
	return m_dwObserver_AID;
}

void CPythonAdmin::Observer_SetLoginName(const std::string& c_rstLoginName)
{
	m_stObserver_LoginName = c_rstLoginName;
}

const std::string& CPythonAdmin::Observer_GetLoginName() const
{
	return m_stObserver_LoginName;
}

void CPythonAdmin::Observer_SetRaceNum(BYTE bRaceNum)
{
	m_bObserver_RaceNum = bRaceNum;
}

BYTE CPythonAdmin::Observer_GetRaceNum() const
{
	return m_bObserver_RaceNum;
}

void CPythonAdmin::Observer_LoadMapName(DWORD dwX, DWORD dwY)
{
	CMapManager::TMapInfo* pkInfo = CPythonBackground::Instance().GlobalPositionToMapInfo(dwX, dwY);
	if (pkInfo)
	{
		m_stObserver_MapName = pkInfo->m_strLocaleName;
	}
}

const std::string& CPythonAdmin::Observer_GetMapName() const
{
	return m_stObserver_MapName;
}

void CPythonAdmin::Observer_SetSkillGroup(BYTE bSkillGroup)
{
	m_bObserver_SkillGroup = bSkillGroup;
}

BYTE CPythonAdmin::Observer_GetSkillGroup() const
{
	return m_bObserver_SkillGroup;
}

void CPythonAdmin::Observer_SetSkill(DWORD dwSkillVnum, TPlayerSkill* pkSkillData)
{
	memcpy(&m_akObserver_Skills[dwSkillVnum], pkSkillData, sizeof(TPlayerSkill));
}

const TPlayerSkill* CPythonAdmin::Observer_GetSkill(DWORD dwSkillVnum) const
{
	return &m_akObserver_Skills[dwSkillVnum];
}

void CPythonAdmin::Observer_SetSkillCoolTime(DWORD dwSkillVnum, DWORD dwCoolTime)
{
	m_adwObserver_SkillStartCoolTimes[dwSkillVnum] = ELTimer_GetMSec();
	m_adwObserver_SkillCoolTimes[dwSkillVnum] = dwCoolTime;
}

DWORD CPythonAdmin::Observer_GetSkillCoolTime(DWORD dwSkillVnum) const
{
	return m_adwObserver_SkillCoolTimes[dwSkillVnum];
}

DWORD CPythonAdmin::Observer_GetSkillElapsedCoolTime(DWORD dwSkillVnum) const
{
	return ELTimer_GetMSec() - m_adwObserver_SkillStartCoolTimes[dwSkillVnum];
}

void CPythonAdmin::Observer_SetPoint(BYTE bType, int64_t llValue)
{
	m_map_ObserverPoints[bType] = llValue;
}

int64_t CPythonAdmin::Observer_GetPoint(BYTE bType, int64_t llDefaultValue) const
{
	auto it = m_map_ObserverPoints.find(bType);
	if (it == m_map_ObserverPoints.end())
	{
		return llDefaultValue;
	}
	return it->second;
}

void CPythonAdmin::Observer_SetInventoryItem(const TAdminManagerObserverItemInfo* pItemInfo)
{
	if (pItemInfo->cell >= c_Inventory_Count)
	{
		TraceError("invalid cell %hu for itemID %u", pItemInfo->cell, pItemInfo->id);
		return;
	}

	memcpy(&m_akObserver_Item[pItemInfo->cell], pItemInfo, sizeof(TAdminManagerObserverItemInfo));
}

void CPythonAdmin::Observer_DelInventoryItem(WORD wCell)
{
	if (wCell >= c_Inventory_Count)
	{
		TraceError("invalid cell %hu", wCell);
		return;
	}

	memset(&m_akObserver_Item[wCell], 0, sizeof(TAdminManagerObserverItemInfo));
}

const TAdminManagerObserverItemInfo* CPythonAdmin::Observer_GetInventoryItem(WORD wCell) const
{
	if (wCell >= c_Inventory_Count)
	{
		TraceError("invalid cell %hu", wCell);
		return NULL;
	}

	return &m_akObserver_Item[wCell];
}

void CPythonAdmin::Observer_AddMessage(bool bIsOtherPIDSender, DWORD dwOtherPID, DWORD dwElapsedTime, const char* szMessage, const char* szOtherName)
{
	TWhisper* pkWhisper;

	auto it = m_map_ObserverWhispers.find(dwOtherPID);
	if (it == m_map_ObserverWhispers.end())
	{
		pkWhisper = &m_map_ObserverWhispers[dwOtherPID];
		pkWhisper->dwOtherPID = dwOtherPID;
		if (szOtherName)
		{
			pkWhisper->stOtherName = szOtherName;
		}
		else
		{
			auto player_it = m_map_OnlinePlayer.find(dwOtherPID);
			if (player_it != m_map_OnlinePlayer.end())
			{
				pkWhisper->stOtherName = player_it->second.szName;
			}
			else
			{
				pkWhisper->stOtherName = "[unkown]";
			}
		}
	}
	else
	{
		pkWhisper = &it->second;
	}

	TWhisperMessage kWhisperMessage;
	kWhisperMessage.dwSenderPID = bIsOtherPIDSender ? dwOtherPID : m_dwObserver_PID;
	kWhisperMessage.dwSentTime = time(0) - dwElapsedTime;

	int iLen = MIN(sizeof(kWhisperMessage.szMessage) - 1, strlen(szMessage));
	strncpy(kWhisperMessage.szMessage, szMessage, iLen);
	kWhisperMessage.szMessage[iLen] = '\0';

	pkWhisper->vec_kMessages.push_back(kWhisperMessage);
}

const std::map<DWORD, CPythonAdmin::TWhisper>& CPythonAdmin::Observer_GetMessageMap() const
{
	return m_map_ObserverWhispers;
}

void CPythonAdmin::Observer_SetChatBanTimeLeft(DWORD dwTimeLeft)
{
	m_dwObserverChatBanTimeout = dwTimeLeft > 0 ? CTimer::Instance().GetCurrentSecond() + dwTimeLeft : 0;
}

DWORD CPythonAdmin::Observer_GetChatBanTimeLeft() const
{
	if (!m_dwObserverChatBanTimeout)
	{
		return 0;
	}

	DWORD dwCurSec = CTimer::Instance().GetCurrentSecond();
	return dwCurSec < m_dwObserverChatBanTimeout ? m_dwObserverChatBanTimeout - dwCurSec : 0;
}

void CPythonAdmin::Observer_SetAccountBanTimeLeft(DWORD dwTimeLeft)
{
	m_dwObserverAccountBanTimeout = dwTimeLeft > 0 ? CTimer::Instance().GetCurrentSecond() + dwTimeLeft : 0;
}

DWORD CPythonAdmin::Observer_GetAccountBanTimeLeft() const
{
	if (!m_dwObserverAccountBanTimeout)
	{
		return 0;
	}

	DWORD dwCurSec = CTimer::Instance().GetCurrentSecond();
	return dwCurSec < m_dwObserverAccountBanTimeout ? m_dwObserverAccountBanTimeout - dwCurSec : 0;
}

/*******************************************************************\
| [PUBLIC] Ban Functions
\*******************************************************************/

void CPythonAdmin::Ban_Clear()
{
	m_dwBanChatSearchPlayerPID = 0;
	ZeroMemory(&m_kBanChatSearchPlayer, sizeof(m_kBanChatSearchPlayer));
	m_map_BanChatList.clear();
	m_map_BanChatPIDMap.clear();

	m_vec_BanLogInfo.clear();

	m_dwBanAccountSearchResultAID = 0;
	ZeroMemory(&m_kBanAccountSearchResult, sizeof(m_kBanAccountSearchResult));
	m_map_BanAccountList.clear();
	m_map_BanAccountMapByName.clear();
	m_map_BanAccountMapByPlayerName.clear();
	m_map_BanAccountMapByPID.clear();
}

void CPythonAdmin::Ban_SetPlayerOnline(DWORD dwPID)
{
	{
		bool bUpdatePy = false;

		auto it = m_map_BanChatList.find(dwPID);
		if (it != m_map_BanChatList.end())
		{
			it->second.SetOnline();
			bUpdatePy = true;
		}

		if (m_dwBanChatSearchPlayerPID == dwPID)
		{
			m_kBanChatSearchPlayer.SetOnline();
			bUpdatePy = true;
		}

		if (bUpdatePy)
		{
			PyCallClassMemberFunc(m_poGameWindow, "BINARY_AdminBanChatUpdate", Py_BuildValue("(i)", dwPID));
		}
	}

	{
		bool bUpdatePy = false;

		auto it = m_map_BanAccountMapByPID.find(dwPID);
		if (it != m_map_BanAccountMapByPID.end())
		{
			TAccountBanInfo* pInfo = &m_map_BanAccountList[it->second];
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (pInfo->dwPID[i] == dwPID)
				{
					pInfo->bIsOnline[i] = true;
					bUpdatePy = true;
					break;
				}
			}
		}

		if (m_dwBanAccountSearchResultAID != 0)
		{
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (m_kBanAccountSearchResult.dwPID[i] == dwPID)
				{
					m_kBanAccountSearchResult.bIsOnline[i] = true;
					bUpdatePy = true;
					break;
				}
			}
		}

		if (bUpdatePy)
		{
			PyCallClassMemberFunc(m_poGameWindow, "BINARY_AdminBanAccountUpdate", Py_BuildValue("(i)", dwPID));
		}
	}
}

void CPythonAdmin::Ban_SetPlayerOffline(DWORD dwPID)
{
	{
		bool bUpdatePy = false;

		auto it = m_map_BanChatList.find(dwPID);
		if (it != m_map_BanChatList.end())
		{
			it->second.SetOffline();
			bUpdatePy = true;
		}

		if (m_dwBanChatSearchPlayerPID == dwPID)
		{
			m_kBanChatSearchPlayer.SetOffline();
			bUpdatePy = true;
		}

		if (bUpdatePy)
		{
			PyCallClassMemberFunc(m_poGameWindow, "BINARY_AdminBanChatUpdate", Py_BuildValue("(i)", dwPID));
		}
	}

	{
		bool bUpdatePy = false;

		auto it = m_map_BanAccountMapByPID.find(dwPID);
		if (it != m_map_BanAccountMapByPID.end())
		{
			TAccountBanInfo* pInfo = &m_map_BanAccountList[it->second];
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (pInfo->dwPID[i] == dwPID)
				{
					pInfo->bIsOnline[i] = false;
					bUpdatePy = true;
					break;
				}
			}
		}

		if (m_dwBanAccountSearchResultAID != 0)
		{
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (m_kBanAccountSearchResult.dwPID[i] == dwPID)
				{
					m_kBanAccountSearchResult.bIsOnline[i] = false;
					bUpdatePy = true;
					break;
				}
			}
		}

		if (bUpdatePy)
		{
			PyCallClassMemberFunc(m_poGameWindow, "BINARY_AdminBanAccountUpdate", Py_BuildValue("(i)", dwPID));
		}
	}
}

void CPythonAdmin::Ban_UpdateChatInfo(const TAdminManagerBanClientPlayerInfo* pPlayerInfo)
{
	bool bIsSearchPlayer = false;
	if (m_dwBanChatSearchPlayerPID == pPlayerInfo->dwPID)
	{
		bIsSearchPlayer = true;
		TChatBanPlayerInfo& rkPlayerInfo = m_kBanChatSearchPlayer;

		strcpy_s(rkPlayerInfo.szName, pPlayerInfo->szName);
		rkPlayerInfo.bRace = pPlayerInfo->bRace;
		rkPlayerInfo.bLevel = pPlayerInfo->bLevel;
		rkPlayerInfo.wChatbanCount = pPlayerInfo->wChatbanCount;
		rkPlayerInfo.wAccountbanCount = pPlayerInfo->wAccountbanCount;
		if (pPlayerInfo->bIsOnline)
		{
			rkPlayerInfo.SetOnline();
		}
		else
		{
			rkPlayerInfo.SetOffline();
		}
		rkPlayerInfo.SetDuration(pPlayerInfo->dwDuration);
	}

	if (pPlayerInfo->dwDuration == 0)
	{
		m_map_BanChatPIDMap.erase(pPlayerInfo->szName);
		m_map_BanChatList.erase(pPlayerInfo->dwPID);
	}
	else if (!bIsSearchPlayer || m_map_BanChatList.find(pPlayerInfo->dwPID) != m_map_BanChatList.end())
	{
		TChatBanPlayerInfo& rkPlayerInfo = m_map_BanChatList[pPlayerInfo->dwPID];
		strcpy_s(rkPlayerInfo.szName, pPlayerInfo->szName);
		rkPlayerInfo.bRace = pPlayerInfo->bRace;
		rkPlayerInfo.bLevel = pPlayerInfo->bLevel;
		rkPlayerInfo.wChatbanCount = pPlayerInfo->wChatbanCount;
		rkPlayerInfo.wAccountbanCount = pPlayerInfo->wAccountbanCount;
		if (pPlayerInfo->bIsOnline)
		{
			rkPlayerInfo.SetOnline();
		}
		else
		{
			rkPlayerInfo.SetOffline();
		}
		rkPlayerInfo.SetDuration(pPlayerInfo->dwDuration);

		m_map_BanChatPIDMap[pPlayerInfo->szName] = pPlayerInfo->dwPID;
	}
}

void CPythonAdmin::Ban_SetChatSearchResult(const TAdminManagerBanClientPlayerInfo* pPlayerInfo)
{
	m_dwBanChatSearchPlayerPID = pPlayerInfo->dwPID;
	strcpy_s(m_kBanChatSearchPlayer.szName, pPlayerInfo->szName);
	m_kBanChatSearchPlayer.bRace = pPlayerInfo->bRace;
	m_kBanChatSearchPlayer.bLevel = pPlayerInfo->bLevel;
	m_kBanChatSearchPlayer.wChatbanCount = pPlayerInfo->wChatbanCount;
	m_kBanChatSearchPlayer.wAccountbanCount = pPlayerInfo->wAccountbanCount;
	if (pPlayerInfo->bIsOnline)
	{
		m_kBanChatSearchPlayer.SetOnline();
	}
	else
	{
		m_kBanChatSearchPlayer.SetOffline();
	}
	m_kBanChatSearchPlayer.SetDuration(pPlayerInfo->dwDuration);
}

void CPythonAdmin::Ban_ClientSetChatSearchResult(DWORD dwPID)
{
	const TChatBanPlayerInfo* pInfo = Ban_GetChatPlayerByPID(dwPID);
	if (!pInfo)
	{
		TraceError("invalid search result pid %u", dwPID);
		return;
	}

	if (pInfo == &m_kBanChatSearchPlayer)
	{
		return;
	}

	m_dwBanChatSearchPlayerPID = dwPID;
	memcpy(&m_kBanChatSearchPlayer, pInfo, sizeof(TChatBanPlayerInfo));
}

void CPythonAdmin::Ban_UpdateAccountInfo(const TAdminManagerBanClientAccountInfo* pAccountInfo)
{
	if (m_dwBanAccountSearchResultAID == pAccountInfo->dwAID)
	{
		strcpy_s(m_kBanAccountSearchResult.szLoginName, pAccountInfo->szLoginName);
		for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
		{
			if (pAccountInfo->dwPID[i])
			{
				m_kBanAccountSearchResult.dwPID[i] = pAccountInfo->dwPID[i];
				strcpy_s(m_kBanAccountSearchResult.szName[i], pAccountInfo->szName[i]);
				m_kBanAccountSearchResult.bRace[i] = pAccountInfo->bRace[i];
				m_kBanAccountSearchResult.bLevel[i] = pAccountInfo->bLevel[i];
				m_kBanAccountSearchResult.wChatbanCount[i] = pAccountInfo->wChatbanCount[i];
				m_kBanAccountSearchResult.bIsOnline[i] = pAccountInfo->bIsOnline[i];
			}
			else if (m_kBanAccountSearchResult.dwPID[i])
			{
				m_kBanAccountSearchResult.dwPID[i] = 0;
			}
		}
		m_kBanAccountSearchResult.wAccountbanCount = pAccountInfo->wAccountbanCount;
		m_kBanAccountSearchResult.SetDuration(pAccountInfo->dwDuration);
	}

	if (pAccountInfo->dwDuration == 0)
	{
		auto it = m_map_BanAccountList.find(pAccountInfo->dwAID);
		if (it == m_map_BanAccountList.end())
		{
			return;
		}

		TAccountBanInfo* pInfo = &it->second;
		for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
		{
			if (pInfo->dwPID[i])
			{
				m_map_BanAccountMapByPlayerName.erase(pInfo->szName[i]);
				m_map_BanAccountMapByPID.erase(pInfo->dwPID[i]);
			}
		}

		m_map_BanAccountMapByName.erase(pInfo->szLoginName);
		m_map_BanAccountList.erase(it);
	}
	else
	{
		TAccountBanInfo* pInfo = NULL;
		auto it = m_map_BanAccountList.find(pAccountInfo->dwAID);
		if (it != m_map_BanAccountList.end())
		{
			pInfo = &it->second;
		}
		else
		{
			pInfo = &m_map_BanAccountList[pAccountInfo->dwAID];
			ZeroMemory(pInfo, sizeof(TAccountBanInfo));
		}

		strcpy_s(pInfo->szLoginName, pAccountInfo->szLoginName);
		for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
		{
			if (pAccountInfo->dwPID[i])
			{
				pInfo->dwPID[i] = pAccountInfo->dwPID[i];
				strcpy_s(pInfo->szName[i], pAccountInfo->szName[i]);
				pInfo->bRace[i] = pAccountInfo->bRace[i];
				pInfo->bLevel[i] = pAccountInfo->bLevel[i];
				pInfo->wChatbanCount[i] = pAccountInfo->wChatbanCount[i];
				pInfo->bIsOnline[i] = pAccountInfo->bIsOnline[i];

				m_map_BanAccountMapByPlayerName[pInfo->szName[i]] = pAccountInfo->dwAID;
				m_map_BanAccountMapByPID[pInfo->dwPID[i]] = pAccountInfo->dwAID;
			}
			else if (pInfo->dwPID[i])
			{
				m_map_BanAccountMapByPlayerName.erase(pInfo->szName[i]);
				m_map_BanAccountMapByPID.erase(pInfo->dwPID[i]);

				pInfo->dwPID[i] = 0;
			}
		}
		pInfo->wAccountbanCount = pAccountInfo->wAccountbanCount;
		pInfo->SetDuration(pAccountInfo->dwDuration);

		m_map_BanAccountMapByName[pInfo->szLoginName] = pAccountInfo->dwAID;
	}
}

void CPythonAdmin::Ban_SetAccountSearchResult(const TAdminManagerBanClientAccountInfo* pAccountInfo)
{
	m_dwBanAccountSearchResultAID = pAccountInfo->dwAID;
	Ban_UpdateAccountInfo(pAccountInfo);
}

void CPythonAdmin::Ban_ClientSetAccountSearchResult(DWORD dwAID)
{
	if (!dwAID)
	{
		m_dwBanAccountSearchResultAID = 0;
		ZeroMemory(&m_kBanAccountSearchResult, sizeof(m_kBanAccountSearchResult));
		return;
	}

	const TAccountBanInfo* pInfo = Ban_GetAccountByAID(dwAID);
	if (!pInfo)
	{
		TraceError("invalid search result aid %u", dwAID);
		return;
	}

	if (pInfo == &m_kBanAccountSearchResult)
	{
		return;
	}

	m_dwBanAccountSearchResultAID = dwAID;
	memcpy(&m_kBanAccountSearchResult, pInfo, sizeof(TAccountBanInfo));
}

DWORD CPythonAdmin::Ban_GetChatPlayerCount()
{
	return m_map_BanChatList.size();
}

const CPythonAdmin::TChatBanPlayerInfo* CPythonAdmin::Ban_GetChatPlayerByIndex(DWORD dwIndex, DWORD& rdwPID)
{
	auto it = m_map_BanChatList.begin();
	if (it == m_map_BanChatList.end())
	{
		return NULL;
	}

	for (DWORD i = 0; i < dwIndex; ++i)
	{
		if (++it == m_map_BanChatList.end())
		{
			return NULL;
		}
	}

	rdwPID = it->first;
	return &it->second;
}

const CPythonAdmin::TChatBanPlayerInfo* CPythonAdmin::Ban_GetChatPlayerByPID(DWORD dwPID)
{
	auto it = m_map_BanChatList.find(dwPID);
	if (it == m_map_BanChatList.end())
	{
		return m_dwBanChatSearchPlayerPID == dwPID ? &m_kBanChatSearchPlayer : NULL;
	}
	return &it->second;
}

DWORD CPythonAdmin::Ban_GetChatPlayerPIDByName(const std::string& c_rstName)
{
	auto it = m_map_BanChatPIDMap.find(c_rstName);
	if (it == m_map_BanChatPIDMap.end())
	{
		return 0;
	}
	return it->second;
}

DWORD CPythonAdmin::Ban_GetChatSearchResultPID() const
{
	return m_dwBanChatSearchPlayerPID;
}

DWORD CPythonAdmin::Ban_GetAccountCount()
{
	return m_map_BanAccountList.size();
}

const CPythonAdmin::TAccountBanInfo* CPythonAdmin::Ban_GetAccountByIndex(DWORD dwIndex, DWORD& rdwAID)
{
	auto it = m_map_BanAccountList.begin();
	if (it == m_map_BanAccountList.end())
	{
		return NULL;
	}

	for (DWORD i = 0; i < dwIndex; ++i)
	{
		if (++it == m_map_BanAccountList.end())
		{
			return NULL;
		}
	}

	rdwAID = it->first;
	return &it->second;
}

const CPythonAdmin::TAccountBanInfo* CPythonAdmin::Ban_GetAccountByAID(DWORD dwAID)
{
	auto it = m_map_BanAccountList.find(dwAID);
	if (it == m_map_BanAccountList.end())
	{
		return m_dwBanAccountSearchResultAID == dwAID ? &m_kBanAccountSearchResult : NULL;
	}
	return &it->second;
}

DWORD CPythonAdmin::Ban_GetAccountAIDByName(const std::string& c_rstName)
{
	auto it = m_map_BanAccountMapByName.find(c_rstName);
	if (it == m_map_BanAccountMapByName.end())
	{
		if (m_dwBanAccountSearchResultAID != 0)
		{
			if (!stricmp(m_kBanAccountSearchResult.szLoginName, c_rstName.c_str()))
			{
				return m_dwBanAccountSearchResultAID;
			}
		}
		return 0;
	}
	return it->second;
}

DWORD CPythonAdmin::Ban_GetAccountAIDByPlayerName(const std::string& c_rstName)
{
	auto it = m_map_BanAccountMapByPlayerName.find(c_rstName);
	if (it == m_map_BanAccountMapByPlayerName.end())
	{
		if (m_dwBanAccountSearchResultAID != 0)
		{
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (m_kBanAccountSearchResult.dwPID[i] && !stricmp(m_kBanAccountSearchResult.szName[i], c_rstName.c_str()))
				{
					return m_dwBanAccountSearchResultAID;
				}
			}
		}
		return 0;
	}
	return it->second;
}

DWORD CPythonAdmin::Ban_GetAccountAIDByPID(DWORD dwPID)
{
	auto it = m_map_BanAccountMapByPID.find(dwPID);
	if (it == m_map_BanAccountMapByPID.end())
	{
		if (m_dwBanAccountSearchResultAID != 0)
		{
			for (int i = 0; i < PLAYER_PER_ACCOUNT5; ++i)
			{
				if (m_kBanAccountSearchResult.dwPID[i] == dwPID)
				{
					return m_dwBanAccountSearchResultAID;
				}
			}
		}
		return 0;
	}
	return it->second;
}

DWORD CPythonAdmin::Ban_GetAccountSearchResultAID() const
{
	return m_dwBanAccountSearchResultAID;
}

void CPythonAdmin::Ban_ClearLogInfo()
{
	m_vec_BanLogInfo.clear();
}

void CPythonAdmin::Ban_AddLogInfo(const TAdminManagerBanClientLogInfo* pLogInfo, const char* szReason, const char* szProof)
{
	TAdminManagerBanLogInfo kInfo;
	kInfo.dwPlayerID = pLogInfo->dwPlayerID;
	strcpy(kInfo.szPlayerName, pLogInfo->szPlayerName);
	kInfo.dwGMPlayerID = pLogInfo->dwGMPlayerID;
	strcpy(kInfo.szGMName, pLogInfo->szGMName);
	kInfo.bType = pLogInfo->bType;
	kInfo.iNewDuration = pLogInfo->iNewDuration;
	strcpy(kInfo.szReason, szReason);
	strcpy(kInfo.szProof, szProof);
	strcpy(kInfo.szDate, pLogInfo->szDate);

	m_vec_BanLogInfo.push_back(kInfo);
}

const TAdminManagerBanLogInfo* CPythonAdmin::Ban_GetLogInfo(DWORD dwIndex)
{
	if (m_vec_BanLogInfo.size() < dwIndex)
	{
		return NULL;
	}

	return &m_vec_BanLogInfo[dwIndex];
}

DWORD CPythonAdmin::Ban_GetLogInfoCount() const
{
	return m_vec_BanLogInfo.size();
}

void CPythonAdmin::Ban_SetLogInfoType(BYTE bType)
{
	m_bBanLogType = bType;
}

BYTE CPythonAdmin::Ban_GetLogInfoType() const
{
	return m_bBanLogType;
}

/*******************************************************************\
| [PUBLIC] Ban Functions
\*******************************************************************/

void CPythonAdmin::Item_Clear()
{
	Item_ClearInfo();
}

void CPythonAdmin::Item_ClearInfo()
{
	m_map_ItemInfo.clear();
}

void CPythonAdmin::Item_AddInfo(TAdminManagerItemInfo* pInfo, const char* szOwnerName)
{
	TItemInfo& kInfo = m_map_ItemInfo[pInfo->dwItemID];
	kInfo.bOwnerType = pInfo->bOwnerType;
	kInfo.dwOwnerID = pInfo->dwOwnerID;
	kInfo.stOwnerName = szOwnerName;
	kInfo.kCell = pInfo->kCell;
	kInfo.dwItemVnum = pInfo->dwItemVnum;
	kInfo.bCount = pInfo->bCount;
	memcpy(kInfo.alSockets, pInfo->alSockets, sizeof(kInfo.alSockets));
	memcpy(kInfo.aAttr, pInfo->aAttr, sizeof(kInfo.aAttr));
	kInfo.bIsGMItem = pInfo->bIsGMItem;
}

DWORD CPythonAdmin::Item_GetInfoCount() const
{
	return m_map_ItemInfo.size();
}

CPythonAdmin::TItemInfo* CPythonAdmin::Item_GetInfoByIndex(DWORD dwIndex, DWORD& rdwID)
{
	auto it = m_map_ItemInfo.begin();
	if (it == m_map_ItemInfo.end())
	{
		return NULL;
	}

	for (DWORD i = 0; i < dwIndex; ++i)
	{
		if (++it == m_map_ItemInfo.end())
		{
			return NULL;
		}
	}

	rdwID = it->first;
	return &it->second;
}

CPythonAdmin::TItemInfo* CPythonAdmin::Item_GetInfoByID(DWORD dwID)
{
	auto it = m_map_ItemInfo.find(dwID);
	if (it == m_map_ItemInfo.end())
	{
		return NULL;
	}
	return &it->second;
}

/*******************************************************************\
| [PUBLIC] Logs Function
\*******************************************************************/

void CPythonAdmin::Hack_AddInfo(TAdminManagerHackLog* pInfo, int index, const char* szHacker, const char* szReason, const char* szTime)
{
	THackInfo kHackInfo;
	kHackInfo.stHackerName = szHacker;
	kHackInfo.stReason = szReason;
	kHackInfo.dwHackCount = pInfo->dwHackCount;
	kHackInfo.stTime = szTime;

	m_map_HackLog.push_back(kHackInfo);
}

void CPythonAdmin::Hack_Clear()
{
	Hack_ClearInfo();
}

void CPythonAdmin::Hack_ClearInfo()
{
	m_map_HackLog.clear();
}

DWORD CPythonAdmin::Hack_GetInfoCount() const
{
	return m_map_HackLog.size();
}

const CPythonAdmin::THackInfo* CPythonAdmin::Hack_GetLogInfo(DWORD dwIndex)
{
	if (m_map_HackLog.size() < dwIndex)
	{
		return NULL;
	}

	return &m_map_HackLog[dwIndex];
}
#endif