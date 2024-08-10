#include "StdAfx.h"

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
#include "PythonMarbleManager.h"
#include "PythonNetworkStream.h"
#include "Packet.h"

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CPythonMarbleManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CPythonMarbleManager::CPythonMarbleManager()
{
}

CPythonMarbleManager::~CPythonMarbleManager()
{
	vMarbleContainer.clear();
}

void CPythonMarbleManager::RegisterMarbleInfo(BYTE bID, DWORD dwMarbleVnum, WORD wRequiredKillCount, DWORD bActiveMission, BYTE bActiveExtandedCount, WORD wKilledMonsters, time_t tCooldownTime)
{
	vMarbleContainer.emplace_back(std::make_shared<SMarbleInformation>(bID, dwMarbleVnum, wRequiredKillCount, bActiveMission, bActiveExtandedCount, wKilledMonsters, tCooldownTime));
}

void CPythonMarbleManager::ClearMarbleData()
{
	vMarbleContainer.clear();
}

size_t CPythonMarbleManager::GetMarblesCount() const
{
	return vMarbleContainer.size();
}

CPythonMarbleManager::SMarbleInformation* CPythonMarbleManager::GetInfoByIndex(std::uint16_t dwArrayIndex) const
{
	if (dwArrayIndex >= GetMarblesCount())
	{
		return nullptr;
	}

	return vMarbleContainer.at(dwArrayIndex).get();
}

/*******************************************************************\
| [PUBLIC] (De-)Module Functions
\*******************************************************************/

PyObject* marbleSendPacketOpen(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendMarbleManagerAction(CG_MARBLE_MANAGER_OPEN);

	return Py_BuildNone();
}

PyObject* marbleSendPacketAccept(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	int iExtandedCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iExtandedCount))
	{
		iExtandedCount = false;
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendMarbleManagerAction(CG_MARBLE_MANAGER_ACTIVE);
	rkNetStream.Send(sizeof(iIndex), &iIndex);
	rkNetStream.Send(sizeof(iExtandedCount), &iExtandedCount);

	return Py_BuildNone();
}

PyObject* marbleSendPacketCancel(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	bool bForceDeactive;
	if (!PyTuple_GetBoolean(poArgs, 1, &bForceDeactive))
	{
		bForceDeactive = false;
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendMarbleManagerAction(CG_MARBLE_MANAGER_DEACTIVE);
	rkNetStream.Send(sizeof(iIndex), &iIndex);
	rkNetStream.Send(sizeof(bForceDeactive), &bForceDeactive);

	return Py_BuildNone();
}

PyObject* marbleGetMarblesCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonMarbleManager::Instance().GetMarblesCount());
}

PyObject* marbleClear(PyObject* poSelf, PyObject* poArgs)
{
	CPythonMarbleManager::Instance().ClearMarbleData();
	return Py_BuildNone();
}

PyObject* marbleGetInfoByIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const auto Marble = CPythonMarbleManager::Instance().GetInfoByIndex(iIndex);
	if (!Marble)
	{
		return Py_BuildException("Failed to find marble by index %d", iIndex);
	}

	return Py_BuildValue("iiiiii", Marble->bID, Marble->dwMarbleVnum, Marble->wRequiredKillCount, Marble->bActiveMission, Marble->bActiveExtandedCount, Marble->wKilledMonsters);
}

PyObject* marbleGetCooldown(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BadArgument();
	}

	const auto Marble = CPythonMarbleManager::Instance().GetInfoByIndex(iIndex);
	if (!Marble)
	{
		return Py_BuildException("Failed to find marble by index %d", iIndex);
	}

	return Py_BuildValue("i", Marble->tCooldownTime);
}

void initMarbleManager()
{
	static PyMethodDef s_methods[] =
	{
		{ "MarblePacketOpen",		marbleSendPacketOpen,	METH_VARARGS },
		{ "MarblePacketActive",		marbleSendPacketAccept,	METH_VARARGS },
		{ "MarblePacketDeactive",	marbleSendPacketCancel,	METH_VARARGS },
		{ "MarbleGetCount",			marbleGetMarblesCount,	METH_VARARGS },
		{ "MarbleGetByIndex",		marbleGetInfoByIndex,	METH_VARARGS },
		{ "MarbleClearData",		marbleClear,			METH_VARARGS },
		{ "MarbleGetCooldown",		marbleGetCooldown,		METH_VARARGS },

		{ NULL, NULL, NULL }
	};

	PyObject* poModule = Py_InitModule("marblemgr", s_methods);
}

#endif
