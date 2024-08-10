#include "StdAfx.h"

#ifdef ENABLE_ADMIN_MANAGER
#include "PythonAdmin.h"
#include "PythonNetworkStream.h"

PyObject * adminCanOpenWindow(PyObject * poSelf, PyObject * poArgs)
{
	bool bCanOpenWindow = CPythonAdmin::Instance().CanOpenAdminWindow();

	if (bCanOpenWindow)
	{
		if (!CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_ONLINE_LIST) &&
				!CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_VIEW_ONLINE_COUNT) &&
				!CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_MAPVIEWER) &&
				!CPythonAdmin::Instance().HasAllow(CPythonAdmin::ALLOW_OBSERVER))
		{
			bCanOpenWindow = false;
		}
	}

	return Py_BuildValue("b", bCanOpenWindow);
}

PyObject * adminHasAllow(PyObject * poSelf, PyObject * poArgs)
{
	int iAllowFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iAllowFlag))
	{
		return Py_BadArgument();
	}

	bool bHasAllow = CPythonAdmin::Instance().HasAllow(iAllowFlag);
	return Py_BuildValue("b", bHasAllow);
}

PyObject * adminGetOnlinePlayerCount(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwCount = CPythonAdmin::Instance().GetOnlinePlayerCount();
	return Py_BuildValue("i", dwCount);
}

PyObject * adminGetOnlinePlayerByIndex(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const TAdminManagerPlayerInfo* c_pPlayerInfo = CPythonAdmin::instance().GetOnlinePlayerByIndex(iIndex);
	if (!c_pPlayerInfo)
	{
		return Py_BuildException();
	}

	return Py_BuildValue("isiii", c_pPlayerInfo->dwPID, c_pPlayerInfo->szName, (long)c_pPlayerInfo->lMapIndex,
						 c_pPlayerInfo->bChannel, c_pPlayerInfo->bEmpire);
}

PyObject * adminGetOnlinePlayerByPID(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	const TAdminManagerPlayerInfo* c_pPlayerInfo = CPythonAdmin::instance().GetOnlinePlayerByPID(iPID);
	if (!c_pPlayerInfo)
	{
		return Py_BuildException();
	}

	return Py_BuildValue("isiii", c_pPlayerInfo->dwPID, c_pPlayerInfo->szName, (long)c_pPlayerInfo->lMapIndex,
						 c_pPlayerInfo->bChannel, c_pPlayerInfo->bEmpire);
}

PyObject * adminSortOnlinePlayer(PyObject * poSelf, PyObject * poArgs)
{
	int iSortType;
	if (!PyTuple_GetInteger(poArgs, 0, &iSortType))
	{
		return Py_BadArgument();
	}
	int iDirection;
	if (!PyTuple_GetInteger(poArgs, 1, &iDirection))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::Instance().SortOnlinePlayer(iSortType, iDirection);
	return Py_BuildNone();
}

PyObject * adminGetOnlinePlayerSortType(PyObject * poSelf, PyObject * poArgs)
{
	int iSortType = CPythonAdmin::Instance().GetOnlinePlayerSortType();
	return Py_BuildValue("i", iSortType);
}

PyObject * adminGetOnlinePlayerSortDirection(PyObject * poSelf, PyObject * poArgs)
{
	int iSortDir = CPythonAdmin::Instance().GetOnlinePlayerSortDirection();
	return Py_BuildValue("i", iSortDir);
}

PyObject * adminGetSortOnlinePlayerByIndex(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const TAdminManagerPlayerInfo* c_pPlayerInfo = CPythonAdmin::instance().GetSortOnlinePlayerByIndex(iIndex);
	if (!c_pPlayerInfo)
	{
		return Py_BuildException();
	}

	return Py_BuildValue("isiii", c_pPlayerInfo->dwPID, c_pPlayerInfo->szName, (long)c_pPlayerInfo->lMapIndex,
						 c_pPlayerInfo->bChannel, c_pPlayerInfo->bEmpire);
}

PyObject * adminIsOnlinePlayerSorted(PyObject * poSelf, PyObject * poArgs)
{
	bool bIsSorted = CPythonAdmin::Instance().IsOnlinePlayerSorted();
	return Py_BuildValue("b", bIsSorted);
}

PyObject * adminIsGMItemTradeBlock(PyObject * poSelf, PyObject * poArgs)
{
	bool bIsBlock = CPythonAdmin::Instance().IsGMItemTradeBlock();
	return Py_BuildValue("b", bIsBlock);
}

PyObject * adminStartMapViewer(PyObject * poSelf, PyObject * poArgs)
{
	int iBaseX;
	if (!PyTuple_GetInteger(poArgs, 0, &iBaseX))
	{
		return Py_BadArgument();
	}
	int iBaseY;
	if (!PyTuple_GetInteger(poArgs, 1, &iBaseY))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_MAPVIEWER_START);
	CPythonNetworkStream::instance().Send(sizeof(int), &iBaseX);
	CPythonNetworkStream::instance().Send(sizeof(int), &iBaseY);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminStopMapViewer(PyObject * poSelf, PyObject * poArgs)
{
	CPythonNetworkStream::instance().SendAdminManager(ADMIN_MANAGER_CG_SUBHEADER_MAPVIEWER_STOP, NULL);
	return Py_BuildNone();
}

PyObject * adminGetMapViewerBaseX(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwBaseX = CPythonAdmin::Instance().MapViewer_GetBaseX();
	return Py_BuildValue("i", dwBaseX);
}

PyObject * adminGetMapViewerBaseY(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwBaseY = CPythonAdmin::Instance().MapViewer_GetBaseY();
	return Py_BuildValue("i", dwBaseY);
}

PyObject * adminStartObserver(PyObject * poSelf, PyObject * poArgs)
{
	char* c_pszName;
	if (!PyTuple_GetString(poArgs, 0, &c_pszName))
	{
		return Py_BadArgument();
	}
	bool bSendFailMessage;
	if (!PyTuple_GetBoolean(poArgs, 1, &bSendFailMessage))
	{
		bSendFailMessage = true;
	}

	int iLen = strlen(c_pszName);

	CPythonNetworkStream::instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_OBSERVER_START);
	CPythonNetworkStream::instance().Send(sizeof(bSendFailMessage), &bSendFailMessage);
	CPythonNetworkStream::instance().Send(sizeof(iLen), &iLen);
	CPythonNetworkStream::instance().Send(iLen, c_pszName);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminStopObserver(PyObject * poSelf, PyObject * poArgs)
{
	CPythonAdmin::Instance().Observer_Clear();
	CPythonNetworkStream::instance().SendAdminManager(ADMIN_MANAGER_CG_SUBHEADER_OBSERVER_STOP, NULL);

	return Py_BuildNone();
}

PyObject * adminIsObserverRunning(PyObject * poSelf, PyObject * poArgs)
{
	bool bIsRunning = CPythonAdmin::Instance().Observer_IsRunning();
	return Py_BuildValue("b", bIsRunning);
}

PyObject * adminIsObserverStopForced(PyObject * poSelf, PyObject * poArgs)
{
	bool bIsStopForced = CPythonAdmin::Instance().Observer_IsStoppedByForce();
	return Py_BuildValue("b", bIsStopForced);
}

PyObject * adminGetObserverPID(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwPID = CPythonAdmin::Instance().Observer_GetPID();
	return Py_BuildValue("i", dwPID);
}

PyObject * adminGetObserverAID(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwAID = CPythonAdmin::Instance().Observer_GetAID();
	return Py_BuildValue("i", dwAID);
}

PyObject * adminGetObserverLoginName(PyObject * poSelf, PyObject * poArgs)
{
	const std::string& c_rstLoginName = CPythonAdmin::Instance().Observer_GetLoginName();
	return Py_BuildValue("s", c_rstLoginName.c_str());
}

PyObject * adminGetObserverRaceNum(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bRaceNum = CPythonAdmin::Instance().Observer_GetRaceNum();
	return Py_BuildValue("i", bRaceNum);
}

PyObject * adminGetObserverMapName(PyObject * poSelf, PyObject * poArgs)
{
	const std::string& c_rstMapName = CPythonAdmin::Instance().Observer_GetMapName();
	return Py_BuildValue("s", c_rstMapName.c_str());
}

PyObject * adminGetObserverSkillGroup(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bSkillGroup = CPythonAdmin::Instance().Observer_GetSkillGroup();
	return Py_BuildValue("i", bSkillGroup);
}

PyObject * adminGetObserverSkillLevel(PyObject * poSelf, PyObject * poArgs)
{
	int iSkillVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillVnum))
	{
		return false;
	}

	const TPlayerSkill* pkSkillInfo = CPythonAdmin::Instance().Observer_GetSkill(iSkillVnum);
	return Py_BuildValue("i", pkSkillInfo->bLevel);
}

PyObject * adminGetObserverSkillMasterType(PyObject * poSelf, PyObject * poArgs)
{
	int iSkillVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillVnum))
	{
		return false;
	}

	const TPlayerSkill* pkSkillInfo = CPythonAdmin::Instance().Observer_GetSkill(iSkillVnum);
	return Py_BuildValue("i", pkSkillInfo->bMasterType);
}

PyObject * adminGetObserverSkillCoolTime(PyObject * poSelf, PyObject * poArgs)
{
	int iSkillVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillVnum))
	{
		return false;
	}

	DWORD dwCoolTime = CPythonAdmin::Instance().Observer_GetSkillCoolTime(iSkillVnum);
	return Py_BuildValue("i", dwCoolTime);
}

PyObject * adminGetObserverESkillCoolTime(PyObject * poSelf, PyObject * poArgs)
{
	int iSkillVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillVnum))
	{
		return false;
	}

	DWORD dwElapsedCoolTime = CPythonAdmin::Instance().Observer_GetSkillElapsedCoolTime(iSkillVnum);
	return Py_BuildValue("i", dwElapsedCoolTime);
}

PyObject * adminGetObserverPoint(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
	{
		return Py_BadArgument();
	}

	int64_t llValue = CPythonAdmin::Instance().Observer_GetPoint(bType, 0);
	return PyLong_FromLongLong(llValue);
}

PyObject * adminGetObserverItem(PyObject * poSelf, PyObject * poArgs)
{
	int iCell;
	if (!PyTuple_GetInteger(poArgs, 0, &iCell))
	{
		return Py_BadArgument();
	}

	const TAdminManagerObserverItemInfo* pItemInfo = CPythonAdmin::Instance().Observer_GetInventoryItem(iCell);
	if (!pItemInfo)
	{
		return Py_BuildValue("iiib", 0, 0, 0, false);
	}

	return Py_BuildValue("iiib", pItemInfo->id, pItemInfo->vnum, pItemInfo->count, pItemInfo->is_gm_item);
}

PyObject * adminGetObserverItemSocket(PyObject * poSelf, PyObject * poArgs)
{
	int iCell;
	if (!PyTuple_GetInteger(poArgs, 0, &iCell))
	{
		return Py_BadArgument();
	}

	const TAdminManagerObserverItemInfo* pItemInfo = CPythonAdmin::Instance().Observer_GetInventoryItem(iCell);
	if (!pItemInfo)
	{
		return Py_BuildValue("(iiiii)", 0, 0, 0, 0, 0);
	}

	return Py_BuildValue("(iiiiii)", pItemInfo->alSockets[0], pItemInfo->alSockets[1], pItemInfo->alSockets[2], pItemInfo->alSockets[3], pItemInfo->alSockets[4], pItemInfo->alSockets[5]);
}

PyObject * adminGetObserverItemAttr(PyObject * poSelf, PyObject * poArgs)
{
	int iCell;
	if (!PyTuple_GetInteger(poArgs, 0, &iCell))
	{
		return Py_BadArgument();
	}

	const TAdminManagerObserverItemInfo* pItemInfo = CPythonAdmin::Instance().Observer_GetInventoryItem(iCell);
	if (!pItemInfo)
		return Py_BuildValue("((ii)(ii)(ii)(ii)(ii)(ii)(ii))",
							 0, 0,
							 0, 0,
							 0, 0,
							 0, 0,
							 0, 0,
							 0, 0,
							 0, 0);

	return Py_BuildValue("((ii)(ii)(ii)(ii)(ii)(ii)(ii))",
						 pItemInfo->aAttr[0].bType, pItemInfo->aAttr[0].sValue,
						 pItemInfo->aAttr[1].bType, pItemInfo->aAttr[1].sValue,
						 pItemInfo->aAttr[2].bType, pItemInfo->aAttr[2].sValue,
						 pItemInfo->aAttr[3].bType, pItemInfo->aAttr[3].sValue,
						 pItemInfo->aAttr[4].bType, pItemInfo->aAttr[4].sValue,
						 pItemInfo->aAttr[5].bType, pItemInfo->aAttr[5].sValue,
						 pItemInfo->aAttr[6].bType, pItemInfo->aAttr[6].sValue);
}

PyObject * adminGetObserverWhisperDlgCount(PyObject * poSelf, PyObject * poArgs)
{
	auto& rkMap = CPythonAdmin::Instance().Observer_GetMessageMap();
	return Py_BuildValue("i", rkMap.size());
}

PyObject * adminGetObserverWhisperPIDByIdx(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	auto& rkMap = CPythonAdmin::Instance().Observer_GetMessageMap();
	auto it = rkMap.begin();
	for (int i = 0; i < iIndex; ++i)
	{
		++it;
	}

	return Py_BuildValue("i", it->first);
}

PyObject * adminGetObserverWhisperName(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	auto& rkMap = CPythonAdmin::Instance().Observer_GetMessageMap();
	auto it = rkMap.find(iPID);
	if (it == rkMap.end())
	{
		return Py_BuildValue("s", "");
	}
	const CPythonAdmin::TWhisper& rkWhisper = it->second;

	return Py_BuildValue("s", rkWhisper.stOtherName.c_str());
}

PyObject * adminGetObserverWhisperMsgCount(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	auto& rkMap = CPythonAdmin::Instance().Observer_GetMessageMap();
	auto it = rkMap.find(iPID);
	if (it == rkMap.end())
	{
		return Py_BuildValue("i", 0);
	}
	const CPythonAdmin::TWhisper& rkWhisper = it->second;

	return Py_BuildValue("i", rkWhisper.vec_kMessages.size());
}

#ifdef time_t
	#undef time_t
#endif
PyObject * adminGetObserverWhisperMsg(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
	{
		return Py_BadArgument();
	}

	auto& rkMap = CPythonAdmin::Instance().Observer_GetMessageMap();
	auto it = rkMap.find(iPID);
	if (it == rkMap.end())
	{
		return Py_BuildValue("iis", 0, 0, "");
	}
	const CPythonAdmin::TWhisper& rkWhisper = it->second;

	char szTimeInfo[50];
	time_t msgTime = rkWhisper.vec_kMessages[iIndex].dwSentTime;
	tm* pLocaltime = localtime(&msgTime);
	strftime(szTimeInfo, sizeof(szTimeInfo), "%d.%m %H:%M:%S", pLocaltime);

	return Py_BuildValue("iss", rkWhisper.vec_kMessages[iIndex].dwSenderPID, szTimeInfo,
						 rkWhisper.vec_kMessages[iIndex].szMessage);
}

PyObject * adminGetObserverChatBanTimeLeft(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwTimeLeft = CPythonAdmin::Instance().Observer_GetChatBanTimeLeft();
	return Py_BuildValue("i", dwTimeLeft);
}

PyObject * adminGetObserverAccountBanTimeLeft(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwTimeLeft = CPythonAdmin::Instance().Observer_GetAccountBanTimeLeft();
	return Py_BuildValue("i", dwTimeLeft);
}

PyObject * adminGetBanChatPlayerCount(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwPlayerCount = CPythonAdmin::Instance().Ban_GetChatPlayerCount();
	return Py_BuildValue("i", dwPlayerCount);
}

PyObject * adminGetBanChatPlayerByIndex(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	DWORD dwPID;
	const CPythonAdmin::TChatBanPlayerInfo* pPlayerInfo = CPythonAdmin::Instance().Ban_GetChatPlayerByIndex(iIndex, dwPID);
	if (!pPlayerInfo)
	{
		return Py_BuildValue("isiiiiib", 0, "", 0, 0, 0, 0, 0, false);
	}

	return Py_BuildValue("isiiiiib",
						 dwPID, pPlayerInfo->szName, pPlayerInfo->bRace, pPlayerInfo->bLevel, pPlayerInfo->wChatbanCount, pPlayerInfo->wAccountbanCount,
						 pPlayerInfo->GetDuration(),	pPlayerInfo->IsOnline());
}

PyObject * adminGetBanChatPlayerByPID(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	const CPythonAdmin::TChatBanPlayerInfo* pPlayerInfo = CPythonAdmin::Instance().Ban_GetChatPlayerByPID(iPID);
	if (!pPlayerInfo)
	{
		TraceError("could not get by pid %d", iPID);
		return Py_BuildValue("isiiiiib", 0, "", 0, 0, 0, 0, 0, false);
	}

	return Py_BuildValue("isiiiiib",
						 iPID, pPlayerInfo->szName, pPlayerInfo->bRace, pPlayerInfo->bLevel, pPlayerInfo->wChatbanCount, pPlayerInfo->wAccountbanCount,
						 pPlayerInfo->GetDuration(),	pPlayerInfo->IsOnline());
}

PyObject * adminGetBanChatPlayerPIDByName(PyObject * poSelf, PyObject * poArgs)
{
	char* pszName;
	if (!PyTuple_GetString(poArgs, 0, &pszName))
	{
		return Py_BadArgument();
	}

	return Py_BuildValue("i", CPythonAdmin::Instance().Ban_GetChatPlayerPIDByName(pszName));
}

PyObject * adminGetBanChatSearchResultPID(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwPID = CPythonAdmin::Instance().Ban_GetChatSearchResultPID();
	return Py_BuildValue("i", dwPID);
}

PyObject * adminSetBanChatSearchResultPID(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::Instance().Ban_ClientSetChatSearchResult(iPID);
	return Py_BuildNone();
}

PyObject * adminSearchBanChatPlayer(PyObject * poSelf, PyObject * poArgs)
{
	char* pszName;
	if (!PyTuple_GetString(poArgs, 0, &pszName))
	{
		return Py_BadArgument();
	}

	int iLen = strlen(pszName);

	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_BAN_CHAT_SEARCH);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iLen);
	CPythonNetworkStream::Instance().Send(iLen, pszName);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminGetBanAccountCount(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwPlayerCount = CPythonAdmin::Instance().Ban_GetAccountCount();
	return Py_BuildValue("i", dwPlayerCount);
}

PyObject * adminGetBanAccountByIndex(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	DWORD dwAID;
	const CPythonAdmin::TAccountBanInfo* pAccountInfo = CPythonAdmin::Instance().Ban_GetAccountByIndex(iIndex, dwAID);
	if (!pAccountInfo)
	{
		return Py_BuildValue("isiii", 0, "", 0, 0, 0);
	}

	return Py_BuildValue("isiii",
						 dwAID, pAccountInfo->szLoginName, pAccountInfo->wAccountbanCount, pAccountInfo->GetDuration(), pAccountInfo);
}

PyObject * adminGetBanAccountByAID(PyObject * poSelf, PyObject * poArgs)
{
	int iAID;
	if (!PyTuple_GetInteger(poArgs, 0, &iAID))
	{
		return Py_BadArgument();
	}

	const CPythonAdmin::TAccountBanInfo* pAccountInfo = CPythonAdmin::Instance().Ban_GetAccountByAID(iAID);
	if (!pAccountInfo)
	{
		TraceError("could not get by aid %d", iAID);
		return Py_BuildValue("isiii", 0, "", 0, 0, 0);
	}

	return Py_BuildValue("isiii",
						 iAID, pAccountInfo->szLoginName, pAccountInfo->wAccountbanCount, pAccountInfo->GetDuration(), pAccountInfo);
}

PyObject * adminGetBanAccountAIDByName(PyObject * poSelf, PyObject * poArgs)
{
	char* pszName;
	if (!PyTuple_GetString(poArgs, 0, &pszName))
	{
		return Py_BadArgument();
	}

	return Py_BuildValue("i", CPythonAdmin::Instance().Ban_GetAccountAIDByName(pszName));
}

PyObject * adminGetBanAccountAIDByPlayerName(PyObject * poSelf, PyObject * poArgs)
{
	char* pszName;
	if (!PyTuple_GetString(poArgs, 0, &pszName))
	{
		return Py_BadArgument();
	}

	return Py_BuildValue("i", CPythonAdmin::Instance().Ban_GetAccountAIDByPlayerName(pszName));
}

PyObject * adminGetBanAccountAIDByPID(PyObject * poSelf, PyObject * poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BadArgument();
	}

	return Py_BuildValue("i", CPythonAdmin::Instance().Ban_GetAccountAIDByPID(iPID));
}

PyObject * adminGetBanAccountPlayer(PyObject * poSelf, PyObject * poArgs)
{
	int iAccountInfoPtr;
	if (!PyTuple_GetInteger(poArgs, 0, &iAccountInfoPtr))
	{
		return Py_BadArgument();
	}
	int iPlayerIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iPlayerIndex))
	{
		return Py_BadArgument();
	}

	const CPythonAdmin::TAccountBanInfo* pAccountInfo = (const CPythonAdmin::TAccountBanInfo*) iAccountInfoPtr;
	return Py_BuildValue("isiiib",
						 pAccountInfo->dwPID[iPlayerIndex], pAccountInfo->szName[iPlayerIndex], pAccountInfo->bRace[iPlayerIndex], pAccountInfo->bLevel[iPlayerIndex],
						 pAccountInfo->wChatbanCount[iPlayerIndex], pAccountInfo->bIsOnline[iPlayerIndex]);
}

PyObject * adminGetBanAccountSearchResultAID(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwAID = CPythonAdmin::Instance().Ban_GetAccountSearchResultAID();
	return Py_BuildValue("i", dwAID);
}

PyObject * adminSetBanAccountSearchResultAID(PyObject * poSelf, PyObject * poArgs)
{
	int iAID;
	if (!PyTuple_GetInteger(poArgs, 0, &iAID))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::Instance().Ban_ClientSetAccountSearchResult(iAID);
	return Py_BuildNone();
}

PyObject * adminSearchBanAccount(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bSearchType;
	if (!PyTuple_GetByte(poArgs, 0, &bSearchType))
	{
		return Py_BadArgument();
	}
	char* pszName;
	if (!PyTuple_GetString(poArgs, 1, &pszName))
	{
		return Py_BadArgument();
	}

	int iLen = strlen(pszName);

	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_BAN_ACCOUNT_SEARCH);
	CPythonNetworkStream::Instance().Send(sizeof(BYTE), &bSearchType);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iLen);
	CPythonNetworkStream::Instance().Send(iLen, pszName);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminBanAccount(PyObject * poSelf, PyObject * poArgs)
{
	int iAID;
	if (!PyTuple_GetInteger(poArgs, 0, &iAID))
	{
		return Py_BadArgument();
	}
	int iPlayerID;
	if (!PyTuple_GetInteger(poArgs, 1, &iPlayerID))
	{
		return Py_BadArgument();
	}
	int iTimeLeft;
	if (!PyTuple_GetInteger(poArgs, 2, &iTimeLeft))
	{
		return Py_BadArgument();
	}
	bool bIncreaseCounter;
	if (!PyTuple_GetBoolean(poArgs, 3, &bIncreaseCounter))
	{
		return Py_BadArgument();
	}
	char* pszReason;
	if (!PyTuple_GetString(poArgs, 4, &pszReason))
	{
		return Py_BadArgument();
	}
	char* pszProof;
	if (!PyTuple_GetString(poArgs, 5, &pszProof))
	{
		return Py_BadArgument();
	}

	int iLen1 = strlen(pszReason);
	int iLen2 = strlen(pszProof);

	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_BAN_ACCOUNT);
	CPythonNetworkStream::Instance().Send(sizeof(DWORD), &iAID);
	CPythonNetworkStream::Instance().Send(sizeof(DWORD), &iPlayerID);
	CPythonNetworkStream::Instance().Send(sizeof(DWORD), &iTimeLeft);
	CPythonNetworkStream::Instance().Send(sizeof(bool), &bIncreaseCounter);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iLen1);
	CPythonNetworkStream::Instance().Send(iLen1, pszReason);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iLen2);
	CPythonNetworkStream::Instance().Send(iLen2, pszProof);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminGetBanLogInfoCount(PyObject * poSelf, PyObject * poArgs)
{
	DWORD dwCount = CPythonAdmin::Instance().Ban_GetLogInfoCount();
	return Py_BuildValue("i", dwCount);
}

PyObject * adminGetBanLogInfo(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const TAdminManagerBanLogInfo* pInfo = CPythonAdmin::Instance().Ban_GetLogInfo(iIndex);
	if (!pInfo)
	{
		return Py_BuildValue("isisiisss", 0, "", 0, "", 0, 0, "", "", "");
	}

	return Py_BuildValue("isisiisss", pInfo->dwPlayerID, pInfo->szPlayerName, pInfo->dwGMPlayerID, pInfo->szGMName, pInfo->bType,
						 pInfo->iNewDuration, pInfo->szReason, pInfo->szProof, pInfo->szDate);
}

PyObject * adminGetLogInfoType(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonAdmin::Instance().Ban_GetLogInfoType());
}

PyObject * adminRequestBanLogInfo(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bType;
	if (!PyTuple_GetByte(poArgs, 0, &bType))
	{
		return Py_BadArgument();
	}
	int iID;
	if (!PyTuple_GetInteger(poArgs, 1, &iID))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::Instance().Ban_SetLogInfoType(bType);

	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_BAN_REQUEST_LOG);
	CPythonNetworkStream::Instance().Send(sizeof(BYTE), &bType);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iID);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}

PyObject * adminGetItemInfoCount(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", CPythonAdmin::Instance().Item_GetInfoCount());
}

PyObject * adminGetItemInfoByIndex(PyObject * poSelf, PyObject * poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	DWORD dwItemID = 0;
	CPythonAdmin::TItemInfo* pInfo = CPythonAdmin::Instance().Item_GetInfoByIndex(iIndex, dwItemID);

	if (!dwItemID)
	{
		return Py_BuildValue("ii", 0, 0);
	}
	else
	{
		return Py_BuildValue("ii", pInfo, dwItemID);
	}
}

PyObject * adminGetItemInfoByID(PyObject * poSelf, PyObject * poArgs)
{
	int iID;
	if (!PyTuple_GetInteger(poArgs, 0, &iID))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::TItemInfo* pInfo = CPythonAdmin::Instance().Item_GetInfoByID(iID);

	if (!pInfo)
	{
		return Py_BuildValue("i", 0);
	}
	else
	{
		return Py_BuildValue("i", pInfo);
	}
}

PyObject * adminGetItemMainInfo(PyObject * poSelf, PyObject * poArgs)
{
	int iInfoPtr;
	if (!PyTuple_GetInteger(poArgs, 0, &iInfoPtr))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::TItemInfo* pInfo = (CPythonAdmin::TItemInfo*) iInfoPtr;
	return Py_BuildValue("iisiiiib", pInfo->bOwnerType, pInfo->dwOwnerID, pInfo->stOwnerName.c_str(), pInfo->kCell.window_type,
						 pInfo->kCell.cell, pInfo->dwItemVnum, pInfo->bCount, pInfo->bIsGMItem);
}

PyObject * adminGetItemSocket(PyObject * poSelf, PyObject * poArgs)
{
	int iInfoPtr;
	if (!PyTuple_GetInteger(poArgs, 0, &iInfoPtr))
	{
		return Py_BadArgument();
	}
	int iSocketIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSocketIndex))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::TItemInfo* pInfo = (CPythonAdmin::TItemInfo*) iInfoPtr;
	return Py_BuildValue("i", pInfo->alSockets[iSocketIndex]);
}

PyObject * adminGetItemAttribute(PyObject * poSelf, PyObject * poArgs)
{
	int iInfoPtr;
	if (!PyTuple_GetInteger(poArgs, 0, &iInfoPtr))
	{
		return Py_BadArgument();
	}
	int iAttributeIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iAttributeIndex))
	{
		return Py_BadArgument();
	}

	CPythonAdmin::TItemInfo* pInfo = (CPythonAdmin::TItemInfo*) iInfoPtr;
	return Py_BuildValue("ii", pInfo->aAttr[iAttributeIndex].bType, pInfo->aAttr[iAttributeIndex].sValue);
}

PyObject * adminSearchItemInfo(PyObject * poSelf, PyObject * poArgs)
{
	BYTE bSearchType;
	if (!PyTuple_GetByte(poArgs, 0, &bSearchType))
	{
		return Py_BadArgument();
	}
	char* szSearchData;
	if (!PyTuple_GetString(poArgs, 1, &szSearchData))
	{
		return Py_BadArgument();
	}
	int iDataLen = strlen(szSearchData);

	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_ITEM_SEARCH);
	CPythonNetworkStream::Instance().Send(sizeof(BYTE), &bSearchType);
	CPythonNetworkStream::Instance().Send(sizeof(int), &iDataLen);
	CPythonNetworkStream::Instance().Send(iDataLen, szSearchData);
	//CPythonNetworkStream::instance().SendSequence();

	return Py_BuildNone();
}


PyObject* adminGetLogs(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::Instance().SendAdminManagerHead(ADMIN_MANAGER_CG_SUBHEADER_GET_LOGS);

	return Py_BuildNone();
}

PyObject* adminGetHackLogInfoCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonAdmin::Instance().Hack_GetInfoCount());
}

PyObject* adminGetHackLogMainInfo(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const CPythonAdmin::THackInfo* pInfo = CPythonAdmin::Instance().Hack_GetLogInfo(iIndex);

	return Py_BuildValue("ssis", pInfo->stHackerName.c_str(), pInfo->stReason.c_str(), pInfo->dwHackCount, pInfo->stTime.c_str());
}

void initAdmin()
{
	static PyMethodDef s_methods[] =
	{
		{ "CanOpenWindow",				adminCanOpenWindow,					METH_VARARGS },

		{ "HasAllow",					adminHasAllow,						METH_VARARGS },

		{ "GetOnlinePlayerCount",		adminGetOnlinePlayerCount,			METH_VARARGS },
		{ "GetOnlinePlayerByIndex",		adminGetOnlinePlayerByIndex,		METH_VARARGS },
		{ "GetOnlinePlayerByPID",		adminGetOnlinePlayerByPID,			METH_VARARGS },
		{ "SortOnlinePlayer",			adminSortOnlinePlayer,				METH_VARARGS },
		{ "GetOnlinePlayerSortType",	adminGetOnlinePlayerSortType,		METH_VARARGS },
		{ "GetOnlinePlayerSortDir",		adminGetOnlinePlayerSortDirection,	METH_VARARGS },
		{ "GetSortOnlinePlayerByIndex",	adminGetSortOnlinePlayerByIndex,	METH_VARARGS },
		{ "IsOnlinePlayerSorted",		adminIsOnlinePlayerSorted,			METH_VARARGS },
		{ "IsGMItemTradeBlock",			adminIsGMItemTradeBlock,			METH_VARARGS },

		{ "StartMapViewer",				adminStartMapViewer,				METH_VARARGS },
		{ "StopMapViewer",				adminStopMapViewer,					METH_VARARGS },
		{ "GetMapViewerBaseX",			adminGetMapViewerBaseX,				METH_VARARGS },
		{ "GetMapViewerBaseY",			adminGetMapViewerBaseY,				METH_VARARGS },

		{ "StartObserver",					adminStartObserver,					METH_VARARGS },
		{ "StopObserver",					adminStopObserver,					METH_VARARGS },
		{ "IsObserverRunning",				adminIsObserverRunning,				METH_VARARGS },
		{ "IsObserverStopForced",			adminIsObserverStopForced,			METH_VARARGS },
		{ "GetObserverPID",					adminGetObserverPID,				METH_VARARGS },
		{ "GetObserverAID",					adminGetObserverAID,				METH_VARARGS },
		{ "GetObserverLoginName",			adminGetObserverLoginName,			METH_VARARGS },
		{ "GetObserverRaceNum",				adminGetObserverRaceNum,			METH_VARARGS },
		{ "GetObserverMapName",				adminGetObserverMapName,			METH_VARARGS },
		{ "GetObserverSkillGroup",			adminGetObserverSkillGroup,			METH_VARARGS },
		{ "GetObserverSkillLevel",			adminGetObserverSkillLevel,			METH_VARARGS },
		{ "GetObserverSkillMasterType",		adminGetObserverSkillMasterType,	METH_VARARGS },
		{ "GetObserverSkillCoolTime",		adminGetObserverSkillCoolTime,		METH_VARARGS },
		{ "GetObserverESkillCoolTime",		adminGetObserverESkillCoolTime,		METH_VARARGS },
		{ "GetObserverPoint",				adminGetObserverPoint,				METH_VARARGS },
		{ "GetObserverItem",				adminGetObserverItem,				METH_VARARGS },
		{ "GetObserverItemSocket",			adminGetObserverItemSocket,			METH_VARARGS },
		{ "GetObserverItemAttr",			adminGetObserverItemAttr,			METH_VARARGS },
		{ "GetObserverWhisperDlgCount",		adminGetObserverWhisperDlgCount,	METH_VARARGS },
		{ "GetObserverWhisperPIDByIdx",		adminGetObserverWhisperPIDByIdx,	METH_VARARGS },
		{ "GetObserverWhisperName",			adminGetObserverWhisperName,		METH_VARARGS },
		{ "GetObserverWhisperMsgCount",		adminGetObserverWhisperMsgCount,	METH_VARARGS },
		{ "GetObserverWhisperMsg",			adminGetObserverWhisperMsg,			METH_VARARGS },
		{ "GetObserverChatBanTimeLeft",		adminGetObserverChatBanTimeLeft,	METH_VARARGS },
		{ "GetObserverAccountBanTimeLeft",	adminGetObserverAccountBanTimeLeft,	METH_VARARGS },

		{ "GetBanChatPlayerCount",			adminGetBanChatPlayerCount,			METH_VARARGS },
		{ "GetBanChatPlayerByIndex",		adminGetBanChatPlayerByIndex,		METH_VARARGS },
		{ "GetBanChatPlayerByPID",			adminGetBanChatPlayerByPID,			METH_VARARGS },
		{ "GetBanChatPlayerPIDByName",		adminGetBanChatPlayerPIDByName,		METH_VARARGS },
		{ "GetBanChatSearchResultPID",		adminGetBanChatSearchResultPID,		METH_VARARGS },
		{ "SetBanChatSearchResultPID",		adminSetBanChatSearchResultPID,		METH_VARARGS },
		{ "SearchBanChatPlayer",			adminSearchBanChatPlayer,			METH_VARARGS },
		{ "GetBanAccountCount",				adminGetBanAccountCount,			METH_VARARGS },
		{ "GetBanAccountByIndex",			adminGetBanAccountByIndex,			METH_VARARGS },
		{ "GetBanAccountByAID",				adminGetBanAccountByAID,			METH_VARARGS },
		{ "GetBanAccountAIDByName",			adminGetBanAccountAIDByName,		METH_VARARGS },
		{ "GetBanAccountAIDByPlayerName",	adminGetBanAccountAIDByPlayerName,	METH_VARARGS },
		{ "GetBanAccountAIDByPID",			adminGetBanAccountAIDByPID,			METH_VARARGS },
		{ "GetBanAccountPlayer",			adminGetBanAccountPlayer,			METH_VARARGS },
		{ "GetBanAccountSearchResultAID",	adminGetBanAccountSearchResultAID,	METH_VARARGS },
		{ "SetBanAccountSearchResultAID",	adminSetBanAccountSearchResultAID,	METH_VARARGS },
		{ "SearchBanAccount",				adminSearchBanAccount,				METH_VARARGS },
		{ "BanAccount",						adminBanAccount,					METH_VARARGS },
		{ "GetBanLogInfoCount",				adminGetBanLogInfoCount,			METH_VARARGS },
		{ "GetBanLogInfo",					adminGetBanLogInfo,					METH_VARARGS },
		{ "GetBanLogInfoType",				adminGetLogInfoType,				METH_VARARGS },
		{ "RequestBanLogInfo",				adminRequestBanLogInfo,				METH_VARARGS },

		{ "GetItemInfoCount",				adminGetItemInfoCount,				METH_VARARGS },
		{ "GetItemInfoByIndex",				adminGetItemInfoByIndex,			METH_VARARGS },
		{ "GetItemInfoByID",				adminGetItemInfoByID,				METH_VARARGS },
		{ "GetItemMainInfo",				adminGetItemMainInfo,				METH_VARARGS },
		{ "GetItemSocket",					adminGetItemSocket,					METH_VARARGS },
		{ "GetItemAttribute",				adminGetItemAttribute,				METH_VARARGS },
		{ "SearchItemInfo",					adminSearchItemInfo,				METH_VARARGS },

		{ "GetLogs",						adminGetLogs,						METH_VARARGS },
		{ "GetHackInfoCount",				adminGetHackLogInfoCount,			METH_VARARGS },
		{ "GetHackMainInfo",				adminGetHackLogMainInfo,			METH_VARARGS },

		{ NULL, NULL, NULL },
	};

	PyObject * poModule = Py_InitModule("admin", s_methods);

	PyModule_AddIntConstant(poModule, "GENERAL_ONLINE_PLAYER_SORT_BY_PID",			CPythonAdmin::GENERAL_ONLINE_PLAYER_SORT_BY_PID);
	PyModule_AddIntConstant(poModule, "GENERAL_ONLINE_PLAYER_SORT_BY_NAME",			CPythonAdmin::GENERAL_ONLINE_PLAYER_SORT_BY_NAME);
	PyModule_AddIntConstant(poModule, "GENERAL_ONLINE_PLAYER_SORT_BY_MAP_INDEX",	CPythonAdmin::GENERAL_ONLINE_PLAYER_SORT_BY_MAP_INDEX);
	PyModule_AddIntConstant(poModule, "GENERAL_ONLINE_PLAYER_SORT_BY_CHANNEL",		CPythonAdmin::GENERAL_ONLINE_PLAYER_SORT_BY_CHANNEL);
	PyModule_AddIntConstant(poModule, "GENERAL_ONLINE_PLAYER_SORT_BY_EMPIRE",		CPythonAdmin::GENERAL_ONLINE_PLAYER_SORT_BY_EMPIRE);

	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_ONLINE_LIST",						CPythonAdmin::ALLOW_VIEW_ONLINE_LIST);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_ONLINE_COUNT",					CPythonAdmin::ALLOW_VIEW_ONLINE_COUNT);
	PyModule_AddIntConstant(poModule, "ALLOW_GM_TRADE_BLOCK_OPTION",				CPythonAdmin::ALLOW_GM_TRADE_BLOCK_OPTION);
	PyModule_AddIntConstant(poModule, "ALLOW_MAPVIEWER",							CPythonAdmin::ALLOW_MAPVIEWER);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_MAPVIEWER_NPC",					CPythonAdmin::ALLOW_VIEW_MAPVIEWER_NPC);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_MAPVIEWER_STONE",					CPythonAdmin::ALLOW_VIEW_MAPVIEWER_STONE);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_MAPVIEWER_MONSTER",				CPythonAdmin::ALLOW_VIEW_MAPVIEWER_MONSTER);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_MAPVIEWER_PLAYER",				CPythonAdmin::ALLOW_VIEW_MAPVIEWER_PLAYER);
	PyModule_AddIntConstant(poModule, "ALLOW_OBSERVER",								CPythonAdmin::ALLOW_OBSERVER);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_OBSERVER_GOLD",					CPythonAdmin::ALLOW_VIEW_OBSERVER_GOLD);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_OBSERVER_SKILL",					CPythonAdmin::ALLOW_VIEW_OBSERVER_SKILL);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_OBSERVER_EQUIPMENT",				CPythonAdmin::ALLOW_VIEW_OBSERVER_EQUIPMENT);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_OBSERVER_INVENTORY",				CPythonAdmin::ALLOW_VIEW_OBSERVER_INVENTORY);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_OBSERVER_WHISPER",				CPythonAdmin::ALLOW_VIEW_OBSERVER_WHISPER);
	PyModule_AddIntConstant(poModule, "ALLOW_OBSERVER_CHATBAN",						CPythonAdmin::ALLOW_OBSERVER_CHATBAN);
	PyModule_AddIntConstant(poModule, "ALLOW_OBSERVER_ACCOUNTBAN",					CPythonAdmin::ALLOW_OBSERVER_ACCOUNTBAN);
	PyModule_AddIntConstant(poModule, "ALLOW_OBSERVER_PERMA_ACCOUNTBAN",			CPythonAdmin::ALLOW_OBSERVER_PERMA_ACCOUNTBAN);
	PyModule_AddIntConstant(poModule, "ALLOW_BAN",									CPythonAdmin::ALLOW_BAN);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_BAN_CHAT",						CPythonAdmin::ALLOW_VIEW_BAN_CHAT);
	PyModule_AddIntConstant(poModule, "ALLOW_VIEW_BAN_CHAT_LOG",					CPythonAdmin::ALLOW_VIEW_BAN_CHAT_LOG);
	PyModule_AddIntConstant(poModule, "ALLOW_BAN_CHAT",								CPythonAdmin::ALLOW_BAN_CHAT);

	PyModule_AddIntConstant(poModule, "BAN_TYPE_CHAT",								CPythonAdmin::BAN_TYPE_CHAT);
	PyModule_AddIntConstant(poModule, "BAN_TYPE_ACCOUNT",							CPythonAdmin::BAN_TYPE_ACCOUNT);

	PyModule_AddIntConstant(poModule, "BAN_ACCOUNT_SEARCH_ACCOUNT",					CPythonAdmin::BAN_ACCOUNT_SEARCH_ACCOUNT);
	PyModule_AddIntConstant(poModule, "BAN_ACCOUNT_SEARCH_PLAYER",					CPythonAdmin::BAN_ACCOUNT_SEARCH_PLAYER);
	PyModule_AddIntConstant(poModule, "BAN_ACCOUNT_SEARCH_MAX_NUM",					CPythonAdmin::BAN_ACCOUNT_SEARCH_MAX_NUM);

	PyModule_AddIntConstant(poModule, "ITEM_OWNER_PLAYER",							CPythonAdmin::ITEM_OWNER_PLAYER);
	PyModule_AddIntConstant(poModule, "ITEM_OWNER_ACCOUNT",							CPythonAdmin::ITEM_OWNER_ACCOUNT);

	PyModule_AddIntConstant(poModule, "ITEM_SEARCH_IID",							CPythonAdmin::ITEM_SEARCH_IID);
	PyModule_AddIntConstant(poModule, "ITEM_SEARCH_INAME",							CPythonAdmin::ITEM_SEARCH_INAME);
	PyModule_AddIntConstant(poModule, "ITEM_SEARCH_PID",							CPythonAdmin::ITEM_SEARCH_PID);
	PyModule_AddIntConstant(poModule, "ITEM_SEARCH_PNAME",							CPythonAdmin::ITEM_SEARCH_PNAME);
	PyModule_AddIntConstant(poModule, "ITEM_SEARCH_GM_ITEM",						CPythonAdmin::ITEM_SEARCH_GM_ITEM);
}
#endif