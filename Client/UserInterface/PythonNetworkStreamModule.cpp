#include "StdAfx.h"
#include "PythonNetworkStream.h"
//#include "PythonNetworkDatagram.h"
#include "AccountConnector.h"
#include "PythonGuild.h"
#include "Test.h"

#include "AbstractPlayer.h"

#ifdef ENABLE_OFFLINE_SHOP
	#include "PythonOfflineShop.h"
#endif

#ifdef ENABLE_SHOP_SEARCH
	#include "PythonShopSearch.h"
#endif

static std::string gs_stServerInfo;
extern BOOL gs_bEmpireLanuageEnable;
std::list<std::string> g_kList_strCommand;

PyObject* netGetBettingGuildWarValue(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}


	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.EXPORT_GetBettingGuildWarValue(szName));
}

PyObject* netSetServerInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
	{
		return Py_BuildException();
	}

	gs_stServerInfo = szFileName;
	return Py_BuildNone();
}

PyObject* netGetServerInfo(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("s", gs_stServerInfo.c_str());
}

PyObject* netPreserveServerCommand(PyObject* poSelf, PyObject* poArgs)
{
	char* szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
	{
		return Py_BuildException();
	}

	g_kList_strCommand.push_back(szLine);

	return Py_BuildNone();
}

PyObject* netGetPreservedServerCommand(PyObject* poSelf, PyObject* poArgs)
{
	if (g_kList_strCommand.empty())
	{
		return Py_BuildValue("s", "");
	}

	std::string strCommand = g_kList_strCommand.front();
	g_kList_strCommand.pop_front();

	return Py_BuildValue("s", strCommand.c_str());
}

PyObject* netStartGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.StartGame();

	return Py_BuildNone();
}

PyObject* netIsTest(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", __IS_TEST_SERVER_MODE__);
}

PyObject* netWarp(PyObject* poSelf, PyObject* poArgs)
{
	int nX;
	if (!PyTuple_GetInteger(poArgs, 0, &nX))
	{
		return Py_BuildException();
	}

	int nY;
	if (!PyTuple_GetInteger(poArgs, 1, &nY))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.Warp(nX, nY);

	return Py_BuildNone();
}

PyObject* netLoadInsultList(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.LoadInsultList(szFileName));
}

PyObject* netUploadMark(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadMark(szFileName));
}

PyObject* netUploadSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char* szFileName;
	if (!PyTuple_GetString(poArgs, 0, &szFileName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.UploadSymbol(szFileName));
}

PyObject* netGetGuildID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetGuildID());
}

PyObject* netGetEmpireID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetEmpireID());
}

PyObject* netGetMainActorVID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorVID());
}

PyObject* netGetMainActorRace(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorRace());
}

PyObject* netGetMainActorEmpire(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorEmpire());
}

PyObject* netGetMainActorSkillGroup(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMainActorSkillGroup());
}

PyObject* netIsSelectedEmpire(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsSelectedEmpire());
}


PyObject* netGetAccountCharacterSlotDataInteger(PyObject* poSelf, PyObject* poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
	{
		return Py_BuildException();
	}

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	UINT uResult = rkNetStream.GetAccountCharacterSlotDatau(nIndex, nType);
	return Py_BuildValue("i", uResult);
}

PyObject* netGetAccountCharacterSlotDataString(PyObject* poSelf, PyObject* poArgs)
{
	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &nIndex))
	{
		return Py_BuildException();
	}

	int nType;
	if (!PyTuple_GetInteger(poArgs, 1, &nType))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetAccountCharacterSlotDataz(nIndex, nType));
}

// SUPPORT_BGM
PyObject* netGetFieldMusicFileName(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetFieldMusicFileName());
}

PyObject* netGetFieldMusicVolume(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("f", rkNetStream.GetFieldMusicVolume());
}
// END_OF_SUPPORT_BGM

PyObject* netSetPhaseWindow(PyObject* poSelf, PyObject* poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
	{
		return Py_BuildException();
	}

	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netClearPhaseWindow(PyObject* poSelf, PyObject* poArgs)
{
	int ePhaseWnd;
	if (!PyTuple_GetInteger(poArgs, 0, &ePhaseWnd))
	{
		return Py_BuildException();
	}

	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 1, &poPhaseWnd))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ClearPhaseWindow(ePhaseWnd, poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetServerCommandParserWindow(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetServerCommandParserWindow(poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetAccountConnectorHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poPhaseWnd;
	if (!PyTuple_GetObject(poArgs, 0, &poPhaseWnd))
	{
		return Py_BuildException();
	}

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.SetHandler(poPhaseWnd);
	return Py_BuildNone();
}

PyObject* netSetHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;

	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetHandler(poHandler);
	return Py_BuildNone();
}

PyObject* netSetTCPRecvBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetTCPSendBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetSendBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetUDPRecvBufferSize(PyObject* poSelf, PyObject* poArgs)
{
	int bufSize;
	if (!PyTuple_GetInteger(poArgs, 0, &bufSize))
	{
		return Py_BuildException();
	}

	//CPythonNetworkDatagram::Instance().SetRecvBufferSize(bufSize);
	return Py_BuildNone();
}

PyObject* netSetMarkServer(PyObject* poSelf, PyObject* poArgs)
{
	char* szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
	{
		return Py_BuildException();
	}

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetMarkServer(szAddr, port);
	return Py_BuildNone();
}

PyObject* netConnectTCP(PyObject* poSelf, PyObject* poArgs)
{
	char* szAddr;
	if (!PyTuple_GetString(poArgs, 0, &szAddr))
	{
		return Py_BuildException();
	}

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ConnectLoginServer(szAddr, port);
	return Py_BuildNone();
}

PyObject* netConnectUDP(PyObject* poSelf, PyObject* poArgs)
{
	char * c_szIP;
	if (!PyTuple_GetString(poArgs, 0, &c_szIP))
	{
		return Py_BuildException();
	}
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
	{
		return Py_BuildException();
	}

	//CPythonNetworkDatagram::Instance().SetConnection(c_szIP, iPort);
	return Py_BuildNone();
}

PyObject* netConnectToAccountServer(PyObject* poSelf, PyObject* poArgs)
{
	char* addr;
	if (!PyTuple_GetString(poArgs, 0, &addr))
	{
		return Py_BuildException();
	}

	int port;
	if (!PyTuple_GetInteger(poArgs, 1, &port))
	{
		return Py_BuildException();
	}

	char* account_addr;
	if (!PyTuple_GetString(poArgs, 2, &account_addr))
	{
		return Py_BuildException();
	}

	int account_port;
	if (!PyTuple_GetInteger(poArgs, 3, &account_port))
	{
		return Py_BuildException();
	}

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.Connect(addr, port, account_addr, account_port);
	return Py_BuildNone();
}

///////////////////////////////////////////////////////////////////////////////////////////////////

PyObject* netSetLoginInfo(PyObject* poSelf, PyObject* poArgs)
{
	char* szName, * szPwd;
	int locale;
	if (!PyTuple_GetString(poArgs, 0, &szName) ||
		!PyTuple_GetString(poArgs, 1, &szPwd) ||
		!PyTuple_GetInteger(poArgs, 2, &locale))
		return Py_BadArgument();

	CPythonNetworkStream::Instance().SetLoginInfo(szName, szPwd, static_cast<uint8_t>(locale));
	CAccountConnector::Instance().SetLoginInfo(szName, szPwd);
	return Py_BuildNone();
}

PyObject* netSetOfflinePhase(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	return Py_BuildNone();
}

PyObject* netSendSelectEmpirePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iEmpireIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iEmpireIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendSelectEmpirePacket(iEmpireIndex);
	return Py_BuildNone();
}

PyObject* netSendLoginPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	char* szPwd;
	if (!PyTuple_GetString(poArgs, 1, &szPwd))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendLoginPacket(szName, szPwd);
	return Py_BuildNone();
}

PyObject* netSendNEWCIBNPasspodAnswerPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szMatrixCardString;
	if (!PyTuple_GetString(poArgs, 0, &szMatrixCardString))
	{
		return Py_BuildException();
	}

	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rkAccountConnector.SendNEWCIBNPasspodAnswerPacket(szMatrixCardString);
	return Py_BuildNone();
}


PyObject* netDirectEnter(PyObject* poSelf, PyObject* poArgs)
{
	int nChrSlot;
	if (!PyTuple_GetInteger(poArgs, 0, &nChrSlot))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ConnectGameServer(nChrSlot);
	return Py_BuildNone();
}

PyObject* netSendSelectCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int Index;
	if (!PyTuple_GetInteger(poArgs, 0, &Index))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendSelectCharacterPacket((BYTE) Index);
	return Py_BuildNone();
}

PyObject* netSendChangeNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BuildException();
	}
	char* szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChangeNamePacket((BYTE)iIndex, szName);
	return Py_BuildNone();
}

PyObject* netEnableChatInsultFilter(PyObject* poSelf, PyObject* poArgs)
{
	int nEnable;
	if (!PyTuple_GetInteger(poArgs, 0, &nEnable))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.EnableChatInsultFilter(nEnable ? true : false);
	return Py_BuildNone();
}

PyObject* netIsChatInsultIn(PyObject* poSelf, PyObject* poArgs)
{
	char* szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsChatInsultIn(szMsg));
}


PyObject* netIsInsultIn(PyObject* poSelf, PyObject* poArgs)
{
	char* szMsg;
	if (!PyTuple_GetString(poArgs, 0, &szMsg))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsInsultIn(szMsg));
}

PyObject* netSendWhisperPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szName;
	char* szLine;

	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	if (!PyTuple_GetString(poArgs, 1, &szLine))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendWhisperPacket(szName, szLine);
	return Py_BuildNone();
}

PyObject* netSendCharacterPositionPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPosition;
	if (!PyTuple_GetInteger(poArgs, 0, &iPosition))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendCharacterPositionPacket((BYTE) iPosition);
	return Py_BuildNone();
}

PyObject* netSendChatPacket(PyObject* poSelf, PyObject* poArgs)
{
	char* szLine;
	if (!PyTuple_GetString(poArgs, 0, &szLine))
	{
		return Py_BuildException();
	}
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		iType = CHAT_TYPE_TALKING;
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendChatPacket(szLine, iType);
	return Py_BuildNone();
}

PyObject* netSendEmoticon(PyObject* poSelf, PyObject* poArgs)
{
	int eEmoticon;
	if (!PyTuple_GetInteger(poArgs, 0, &eEmoticon))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendEmoticon(eEmoticon);
	return Py_BuildNone();
}

PyObject* netSendCreateCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BuildException();
	}

	char* name;
	if (!PyTuple_GetString(poArgs, 1, &name))
	{
		return Py_BuildException();
	}

	int job;
	if (!PyTuple_GetInteger(poArgs, 2, &job))
	{
		return Py_BuildException();
	}

	int shape;
	if (!PyTuple_GetInteger(poArgs, 3, &shape))
	{
		return Py_BuildException();
	}

	int stat1;
	if (!PyTuple_GetInteger(poArgs, 4, &stat1))
	{
		return Py_BuildException();
	}
	int stat2;
	if (!PyTuple_GetInteger(poArgs, 5, &stat2))
	{
		return Py_BuildException();
	}
	int stat3;
	if (!PyTuple_GetInteger(poArgs, 6, &stat3))
	{
		return Py_BuildException();
	}
	int stat4;
	if (!PyTuple_GetInteger(poArgs, 7, &stat4))
	{
		return Py_BuildException();
	}

	if (index < 0 && index > 3)
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendCreateCharacterPacket((BYTE) index, name, (BYTE) job, (BYTE) shape, stat1, stat2, stat3, stat4);
	return Py_BuildNone();
}

PyObject* netSendDestroyCharacterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BuildException();
	}

	char * szPrivateCode;
	if (!PyTuple_GetString(poArgs, 1, &szPrivateCode))
	{
		return Py_BuildException();
	}

	if (index < 0 && index > 3)
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendDestroyCharacterPacket((BYTE) index, szPrivateCode);
	return Py_BuildNone();
}

PyObject* netSendEnterGamePacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendEnterGame();
	return Py_BuildNone();
}

PyObject* netOnClickPacket(PyObject* poSelf, PyObject* poArgs)
{
	int index;
	if (!PyTuple_GetInteger(poArgs, 0, &index))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendOnClickPacket(index);

	return Py_BuildNone();
}

PyObject* netSendItemUsePacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		{
			return Py_BuildException();
		}
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemUsePacket(Cell);
	return Py_BuildNone();
}

#ifdef ENABLE_MULTI_USE_PACKET
PyObject* netSendItemMultiUsePacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	BYTE bCount;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetByte(poArgs, 1, &bCount))
		{
			return Py_BuildException();
		}
		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetByte(poArgs, 2, &bCount))
		{
			return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemMultiUsePacket(Cell, bCount);
	return Py_BuildNone();
}
#endif

PyObject* netSendItemUseToItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos SourceCell;
	TItemPos TargetCell;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &SourceCell.cell))
		{
			return Py_BuildException();
		}

		if (!PyTuple_GetInteger(poArgs, 1, &TargetCell.cell))
		{
			return Py_BuildException();
		}
		break;
	case 4:
		if (!PyTuple_GetByte(poArgs, 0, &SourceCell.window_type))
		{
			return Py_BuildException();
		}

		if (!PyTuple_GetInteger(poArgs, 1, &SourceCell.cell))
		{
			return Py_BuildException();
		}

		if (!PyTuple_GetByte(poArgs, 2, &TargetCell.window_type))
		{
			return Py_BuildException();
		}

		if (!PyTuple_GetInteger(poArgs, 3, &TargetCell.cell))
		{
			return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemUseToItemPacket(SourceCell, TargetCell);
	return Py_BuildNone();
}

PyObject* netSendItemDropPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
	case 1:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		{
			return Py_BuildException();
		}
		break;
	case 2:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}


	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(Cell, 0);
	return Py_BuildNone();
}

PyObject* netSendItemDropPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	int count;
	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &count))
		{
			return Py_BuildException();
		}

		break;
	case 3:
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &count))
		{
			return Py_BuildException();
		}

		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(Cell, 0, count);
	return Py_BuildNone();
}

PyObject* netSendElkDropPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacket(TItemPos(RESERVED_WINDOW, 0), (DWORD) iElk);
	return Py_BuildNone();
}

PyObject* netSendGoldDropPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDropPacketNew(TItemPos (RESERVED_WINDOW, 0), (DWORD) iElk, 0);
	return Py_BuildNone();
}

PyObject* netSendItemMovePacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	TItemPos ChangeCell;
	CountType num;

	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &ChangeCell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &num))
		{
			return Py_BuildException();
		}
		break;
	case 5:
	{
		if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetByte(poArgs, 2, &ChangeCell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 3, &ChangeCell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 4, &num))
		{
			return Py_BuildException();
		}
	}
	break;
	default:
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemMovePacket(Cell, ChangeCell, num);
	return Py_BuildNone();
}

PyObject* netSendItemPickUpPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemPickUpPacket(vid);
	return Py_BuildNone();
}

PyObject* netSendGiveItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iTargetVID;
	TItemPos Cell;
	int iItemCount;
	switch (PyTuple_Size(poArgs))
	{
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &iItemCount))
		{
			return Py_BuildException();
		}
		break;
	case 4:
		if (!PyTuple_GetInteger(poArgs, 0, &iTargetVID))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetByte(poArgs, 1, &Cell.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &Cell.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 3, &iItemCount))
		{
			return Py_BuildException();
		}
		break;
	default:
		break;
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendGiveItemPacket(iTargetVID, Cell, iItemCount);
	return Py_BuildNone();
}

#ifdef ENABLE_DESTROY_ITEM_PACKET
PyObject* netSendItemDestroyPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos Cell;
	switch (PyTuple_Size(poArgs))
	{
		case 1:
			if (!PyTuple_GetInteger(poArgs, 0, &Cell.cell))
				return Py_BadArgument();
			break;
		case 2:
			if (!PyTuple_GetByte(poArgs, 0, &Cell.window_type))
				return Py_BadArgument();
			if (!PyTuple_GetInteger(poArgs, 1, &Cell.cell))
				return Py_BadArgument();
			break;
		default:
			return Py_BadArgument();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendItemDestroyPacket(Cell);
	return Py_BuildNone();
}
#endif

PyObject* netSendShopEndPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendShopEndPacket();
	return Py_BuildNone();
}

PyObject* netSendShopBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 0, &iCount))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendShopBuyPacket(iCount);
	return Py_BuildNone();
}

#ifdef ENABLE_MULTIPLE_BUY_SYSTEM
PyObject* netSendShopMultipleBuyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iPos))
	{
		return Py_BuildException();
	}

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iCount))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendShopBuyPacketMultiple(iPos, iCount);
	return Py_BuildNone();
}
#endif

PyObject* netSendShopSellPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
	{
		return Py_BuildException();
	}
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendShopSellPacket(iSlotNumber);
	return Py_BuildNone();
}

PyObject* netSendShopSellPacketNew(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotNumber))
	{
		return Py_BuildException();
	}
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 1, &iCount))
	{
		return Py_BuildException();
	}
	int iType;
	if (!PyTuple_GetInteger(poArgs, 2, &iType))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendShopSellPacketNew(iSlotNumber, iCount, iType);
	return Py_BuildNone();
}

PyObject* netSendExchangeStartPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeStartPacket(vid);
	return Py_BuildNone();
}

PyObject* netSendExchangeElkAddPacket(PyObject* poSelf, PyObject* poArgs)
{
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t iElk;

	if (!PyTuple_GetLongLong(poArgs, 0, &iElk))
	{
		return Py_BuildException();
	}
#else
	int iElk;
	if (!PyTuple_GetInteger(poArgs, 0, &iElk))
	{
		return Py_BuildException();
	}
#endif

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeElkAddPacket(iElk);
	return Py_BuildNone();
}

PyObject* netSendExchangeItemAddPacket(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bWindowType;
	if (!PyTuple_GetInteger(poArgs, 0, &bWindowType))
	{
		return Py_BuildException();
	}
	WORD wSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &wSlotIndex))
	{
		return Py_BuildException();
	}
	int iDisplaySlotIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iDisplaySlotIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeItemAddPacket(TItemPos(bWindowType, wSlotIndex), iDisplaySlotIndex);
	return Py_BuildNone();
}

PyObject* netSendExchangeItemDelPacket(PyObject* poSelf, PyObject* poArgs)
{
	int pos;
	if (!PyTuple_GetInteger(poArgs, 0, &pos))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeItemDelPacket((BYTE) pos);
	return Py_BuildNone();
}

PyObject* netSendExchangeAcceptPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeAcceptPacket();
	return Py_BuildNone();
}

PyObject* netSendExchangeExitPacket(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SendExchangeExitPacket();
	return Py_BuildNone();
}

PyObject* netExitApplication(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ExitApplication();
	return Py_BuildNone();
}

PyObject* netExitGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ExitGame();
	return Py_BuildNone();
}

PyObject* netLogOutGame(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.LogOutGame();
	return Py_BuildNone();
}

PyObject* netDisconnect(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetOffLinePhase();
	rkNetStream.Disconnect();

	return Py_BuildNone();
}

PyObject* netIsConnect(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.IsOnline());
}

PyObject* netToggleGameDebugInfo(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.ToggleGameDebugInfo();
	return Py_BuildNone();
}

PyObject* netRegisterEmoticonString(PyObject* poSelf, PyObject* poArgs)
{
	char * pcEmoticonString;
	if (!PyTuple_GetString(poArgs, 0, &pcEmoticonString))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.RegisterEmoticonString(pcEmoticonString);
	return Py_BuildNone();
}

PyObject* netSendMessengerAddByVIDPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerAddByVIDPacket(vid);

	return Py_BuildNone();
}

PyObject* netSendMessengerAddByNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerAddByNamePacket(szName);

	return Py_BuildNone();
}

PyObject* netSendMessengerRemovePacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
	{
		return Py_BuildException();
	}
	char * szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMessengerRemovePacket(szKey, szName);

	return Py_BuildNone();
}

PyObject* netSendPartyInvitePacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartyInvitePacket(vid);

	return Py_BuildNone();
}

PyObject* netSendPartyInviteAnswerPacket(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}
	int answer;
	if (!PyTuple_GetInteger(poArgs, 1, &answer))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartyInviteAnswerPacket(vid, answer);

	return Py_BuildNone();
}

PyObject* netSendPartyExitPacket(PyObject* poSelf, PyObject* poArgs)
{
	IAbstractPlayer& rPlayer = IAbstractPlayer::GetSingleton();

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();

	DWORD dwVID = rPlayer.GetMainCharacterIndex();
	DWORD dwPID;
	if (rPlayer.PartyMemberVIDToPID(dwVID, &dwPID))
	{
		rns.SendPartyRemovePacket(dwPID);
	}

	return Py_BuildNone();
}

PyObject* netSendPartyRemovePacketPID(PyObject* poSelf, PyObject* poArgs)
{
	int pid;
	if (!PyTuple_GetInteger(poArgs, 0, &pid))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartyRemovePacket(pid);

	return Py_BuildNone();
}

PyObject* netSendPartyRemovePacketVID(PyObject* poSelf, PyObject* poArgs)
{
	int vid;
	if (!PyTuple_GetInteger(poArgs, 0, &vid))
	{
		return Py_BuildException();
	}

	IAbstractPlayer& rPlayer = IAbstractPlayer::GetSingleton();
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	DWORD dwPID;
	if (rPlayer.PartyMemberVIDToPID(vid, &dwPID))
	{
		rns.SendPartyRemovePacket(dwPID);
	}

	return Py_BuildNone();
}

PyObject* netSendPartySetStatePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
	{
		return Py_BuildException();
	}
	int iState;
	if (!PyTuple_GetInteger(poArgs, 1, &iState))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 2, &iFlag))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartySetStatePacket(iVID, iState, iFlag);

	return Py_BuildNone();
}

PyObject* netSendPartyUseSkillPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillIndex))
	{
		return Py_BuildException();
	}
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iVID))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartyUseSkillPacket(iSkillIndex, iVID);

	return Py_BuildNone();
}

PyObject* netSendPartyParameterPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendPartyParameterPacket(iMode);

	return Py_BuildNone();
}

PyObject* netSendSafeboxSaveMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_SAVE, iMoney);

	return Py_BuildNone();
}

PyObject* netSendSafeboxWithdrawMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMoney;
	if (!PyTuple_GetInteger(poArgs, 0, &iMoney))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSafeBoxMoneyPacket(SAFEBOX_MONEY_STATE_WITHDRAW, iMoney);

	return Py_BuildNone();
}

PyObject* netSendSafeboxCheckinPacket(PyObject* poSelf, PyObject* poArgs)
{
	TItemPos InventoryPos;
	int iSafeBoxPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		InventoryPos.window_type = INVENTORY;
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &iSafeBoxPos))
		{
			return Py_BuildException();
		}
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &InventoryPos.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &iSafeBoxPos))
		{
			return Py_BuildException();
		}
		break;

	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckinPacket(InventoryPos, iSafeBoxPos);

	return Py_BuildNone();
}

PyObject* netSendSafeboxCheckoutPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSafeBoxPos;
	TItemPos InventoryPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iSafeBoxPos))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		break;
	default:
		break;
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSafeBoxCheckoutPacket(iSafeBoxPos, InventoryPos);

	return Py_BuildNone();
}

PyObject* netSendSafeboxItemMovePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSourcePos;
	if (!PyTuple_GetInteger(poArgs, 0, &iSourcePos))
	{
		return Py_BuildException();
	}
	int iTargetPos;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetPos))
	{
		return Py_BuildException();
	}
	int iCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iCount))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSafeBoxItemMovePacket(iSourcePos, iTargetPos, iCount);

	return Py_BuildNone();
}

PyObject* netSendMallCheckoutPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iMallPos;
	TItemPos InventoryPos;

	switch (PyTuple_Size(poArgs))
	{
	case 2:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		break;
	case 3:
		if (!PyTuple_GetInteger(poArgs, 0, &iMallPos))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 1, &InventoryPos.window_type))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetInteger(poArgs, 2, &InventoryPos.cell))
		{
			return Py_BuildException();
		}
		break;
	default:
		return Py_BuildException();
	}
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendMallCheckoutPacket(iMallPos, InventoryPos);

	return Py_BuildNone();
}

PyObject* netSendAnswerMakeGuildPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szName;
	if (!PyTuple_GetString(poArgs, 0, &szName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendAnswerMakeGuildPacket(szName);

	return Py_BuildNone();
}

PyObject* netSendQuestInputStringPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szString;
	if (!PyTuple_GetString(poArgs, 0, &szString))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendQuestInputStringPacket(szString);

	return Py_BuildNone();
}

PyObject* netSendQuestConfirmPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 0, &iAnswer))
	{
		return Py_BuildException();
	}
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 1, &iPID))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendQuestConfirmPacket(iAnswer, iPID);

	return Py_BuildNone();
}

PyObject* netSendGuildAddMemberPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildAddMemberPacket(iVID);

	return Py_BuildNone();
}

PyObject* netSendGuildRemoveMemberPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szKey;
	if (!PyTuple_GetString(poArgs, 0, &szKey))
	{
		return Py_BuildException();
	}

	CPythonGuild::TGuildMemberData * pGuildMemberData;
	if (!CPythonGuild::Instance().GetMemberDataPtrByName(szKey, &pGuildMemberData))
	{
		TraceError("netSendGuildRemoveMemberPacket(szKey=%s) - Can't Find Guild Member\n", szKey);
		return Py_BuildNone();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildRemoveMemberPacket(pGuildMemberData->dwPID);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeGradeNamePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
	{
		return Py_BuildException();
	}
	char * szGradeName;
	if (!PyTuple_GetString(poArgs, 1, &szGradeName))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeNamePacket(iGradeNumber, szGradeName);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeGradeAuthorityPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &iGradeNumber))
	{
		return Py_BuildException();
	}
	int iAuthority;
	if (!PyTuple_GetInteger(poArgs, 1, &iAuthority))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildChangeGradeAuthorityPacket(iGradeNumber, iAuthority);

	return Py_BuildNone();
}

PyObject* netSendGuildOfferPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iExperience;
	if (!PyTuple_GetInteger(poArgs, 0, &iExperience))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildOfferPacket(iExperience);

	return Py_BuildNone();
}

PyObject* netSnedGuildPostCommentPacket(PyObject* poSelf, PyObject* poArgs)
{
	char * szComment;
	if (!PyTuple_GetString(poArgs, 0, &szComment))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildPostCommentPacket(szComment);

	return Py_BuildNone();
}

PyObject* netSnedGuildDeleteCommentPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildDeleteCommentPacket(iIndex);

	return Py_BuildNone();
}

PyObject* netSendGuildRefreshCommentsPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iHightestIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iHightestIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildRefreshCommentsPacket(iHightestIndex);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeMemberGradePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BuildException();
	}
	int iGradeNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iGradeNumber))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGradePacket(iPID, iGradeNumber);

	return Py_BuildNone();
}

PyObject* netSendGuildUseSkillPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSkillID;
	if (!PyTuple_GetInteger(poArgs, 0, &iSkillID))
	{
		return Py_BuildException();
	}
	int iTargetVID;
	if (!PyTuple_GetInteger(poArgs, 1, &iTargetVID))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildUseSkillPacket(iSkillID, iTargetVID);

	return Py_BuildNone();
}

PyObject* netSendGuildChangeMemberGeneralPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iPID;
	if (!PyTuple_GetInteger(poArgs, 0, &iPID))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildChangeMemberGeneralPacket(iPID, iFlag);

	return Py_BuildNone();
}

PyObject* netSendGuildInviteAnswerPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 0, &iGuildID))
	{
		return Py_BuildException();
	}
	int iAnswer;
	if (!PyTuple_GetInteger(poArgs, 1, &iAnswer))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildInviteAnswerPacket(iGuildID, iAnswer);

	return Py_BuildNone();
}

PyObject* netSendGuildChargeGSPPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildChargeGSPPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendGuildDepositMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildDepositMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendGuildWithdrawMoneyPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iGSP;
	if (!PyTuple_GetInteger(poArgs, 0, &iGSP))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendGuildWithdrawMoneyPacket(iGSP);

	return Py_BuildNone();
}

PyObject* netSendRequestRefineInfoPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
	{
		return Py_BuildException();
	}

//	CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
//	rns.SendRequestRefineInfoPacket(iSlotIndex);
	assert(!"netSendRequestRefineInfoPacket - This function is not used anymore");

	return Py_BuildNone();
}

PyObject* netSendRefinePacket(PyObject* poSelf, PyObject* poArgs)
{
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iSlotIndex))
	{
		return Py_BuildException();
	}
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		return Py_BuildException();
	}
#ifdef ENABLE_FAST_REFINE_OPTION
	bool fast_refine;
	if (!PyTuple_GetBoolean(poArgs, 2, &fast_refine))
	{
		fast_refine = false;
	}
#endif

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
#ifdef ENABLE_FAST_REFINE_OPTION
	rns.SendRefinePacket(iSlotIndex, iType, fast_refine);
#else
	rns.SendRefinePacket(iSlotIndex, iType);
#endif

	return Py_BuildNone();
}

PyObject* netSendSelectItemPacket(PyObject* poSelf, PyObject* poArgs)
{
	int iItemPos;
	if (!PyTuple_GetInteger(poArgs, 0, &iItemPos))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.SendSelectItemPacket(iItemPos);

	return Py_BuildNone();
}

PyObject* netSetPacketSequenceMode(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
	rns.SetPacketSequenceMode(true);
	rkAccountConnector.SetPacketSequenceMode(true);

	return Py_BuildNone();
}

PyObject* netSetEmpireLanguageMode(PyObject* poSelf, PyObject* poArgs)
{
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 0, &iMode))
	{
		return Py_BuildException();
	}

	//CPythonNetworkStream& rns=CPythonNetworkStream::Instance();
	gs_bEmpireLanuageEnable = iMode;

	return Py_BuildNone();
}

PyObject* netSetSkillGroupFake(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rns = CPythonNetworkStream::Instance();
	rns.__TEST_SetSkillGroupFake(iIndex);

	return Py_BuildNone();
}

#include "GuildMarkUploader.h"
#include "GuildMarkDownloader.h"

PyObject* netSendGuildSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char * szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
	{
		return Py_BuildException();
	}
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 2, &szFileName))
	{
		return Py_BuildException();
	}
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 3, &iGuildID))
	{
		return Py_BuildException();
	}

	CNetworkAddress kAddress;
	kAddress.Set(szIP, iPort);

	UINT uiError;

	CGuildMarkUploader& rkGuildMarkUploader = CGuildMarkUploader::Instance();
	if (!rkGuildMarkUploader.ConnectToSendSymbol(kAddress, 0, 0, iGuildID, szFileName, &uiError))
	{
		assert(!"Failed connecting to send symbol");
	}

	return Py_BuildNone();
}

PyObject* netDisconnectUploader(PyObject* poSelf, PyObject* poArgs)
{
	CGuildMarkUploader& rkGuildMarkUploader = CGuildMarkUploader::Instance();
	rkGuildMarkUploader.Disconnect();
	return Py_BuildNone();
}

PyObject* netRecvGuildSymbol(PyObject* poSelf, PyObject* poArgs)
{
	char * szIP;
	if (!PyTuple_GetString(poArgs, 0, &szIP))
	{
		return Py_BuildException();
	}
	int iPort;
	if (!PyTuple_GetInteger(poArgs, 1, &iPort))
	{
		return Py_BuildException();
	}
	int iGuildID;
	if (!PyTuple_GetInteger(poArgs, 2, &iGuildID))
	{
		return Py_BuildException();
	}

	CNetworkAddress kAddress;
	kAddress.Set(szIP, iPort);

	std::vector<DWORD> kVec_dwGuildID;
	kVec_dwGuildID.clear();
	kVec_dwGuildID.push_back(iGuildID);

	// @fixme006
	if (kVec_dwGuildID.size() > 0)
	{
		CGuildMarkDownloader& rkGuildMarkDownloader = CGuildMarkDownloader::Instance();
		if (!rkGuildMarkDownloader.ConnectToRecvSymbol(kAddress, 0, 0, kVec_dwGuildID))
		{
			assert(!"Failed connecting to recv symbol");
		}
	}

	return Py_BuildNone();
}

PyObject* netRegisterErrorLog(PyObject* poSelf, PyObject* poArgs)
{
	char * szLog;
	if (!PyTuple_GetString(poArgs, 0, &szLog))
	{
		return Py_BuildException();
	}

	return Py_BuildNone();
}

#ifdef ENABLE_TREASURE_BOX_LOOT
#include "../gamelib/ItemManager.h"
#include "../gamelib/ItemData.h"
static const std::list<BYTE> s_clItemType { CItemData::ITEM_TYPE_TREASURE_BOX, CItemData::ITEM_TYPE_GIFTBOX, };

PyObject* netSendRequestTreasureBoxLoot(PyObject* poSelf, PyObject* poArgs)
{
	int iItemVnum;
	if (!(PyTuple_GetInteger(poArgs, 0, &iItemVnum)))
	{
		return Py_BuildException();
	}

	CItemData* ptrItem;
	if (CItemManager::Instance().GetItemDataPointer(iItemVnum, &ptrItem))
	{
		if (std::find(s_clItemType.begin(), s_clItemType.end(), ptrItem->GetType()) != s_clItemType.end())
		{
			return Py_BuildValue("b", CPythonNetworkStream::Instance().SendRequestTreasureBoxLoot((DWORD)iItemVnum));
		}
	}
	return Py_BuildValue("b", false);
}
#endif

#ifdef ENABLE_TARGET_MONSTER_LOOT
PyObject* netSendLoadTargetInfo(PyObject* poSelf, PyObject* poArgs)
{
	int iVID;
	if (!PyTuple_GetInteger(poArgs, 0, &iVID))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::instance().SendLoadTargetInfo((DWORD)iVID);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_DELETE_SINGLE_STONE
PyObject* netSendRequestDeleteSocket(PyObject* poSelf, PyObject* poArgs)
{
	int eSubHeader;
	if (!(PyTuple_GetInteger(poArgs, 0, &eSubHeader)))
	{
		return Py_BuildException();
	}

	int iItemVnum = -1;
	if (eSubHeader != SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE)
	{
		if (!(PyTuple_GetInteger(poArgs, 1, &iItemVnum)))
		{
			return Py_BuildException();
		}
	}

	return Py_BuildValue("b", CPythonNetworkStream::Instance().SendRequestDeleteSocket((ESubHeader)eSubHeader, iItemVnum));
}
#endif

#ifdef ENABLE_VOICE_CHAT
PyObject* netSendVoiceStart(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* list;

	if (!PyArg_ParseTuple(poArgs, "O!", &PyTuple_Type, &list))
	{
		return Py_BuildException();
	}

	int length = PyTuple_GET_SIZE(list);
	if (length != 1024)
	{
		return Py_BuildException();
	}

	short* data = (short*)malloc(sizeof(short) * length);

	PyObject* item = nullptr;
	for (int i = 0; i < length; i++)
	{
		item = PyTuple_GET_ITEM(list, i);
		data[i] = (short)PyInt_AsLong(item);

		if (PyErr_Occurred())
		{
			return Py_BuildException();
		}
	}

	CPythonNetworkStream::Instance().SendVoiceStart(data);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_CHANGE_CHANNEL
PyObject* netSetMapIndex(PyObject* poSelf, PyObject* poArgs)
{
	int iMapIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iMapIndex))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetMapIndex(iMapIndex);

	return Py_BuildNone();
}

PyObject* netGetMapIndex(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetMapIndex());
}

PyObject* netSetChannelNumber(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bChannelNumber;
	if (!PyTuple_GetInteger(poArgs, 0, &bChannelNumber))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	rkNetStream.SetChannelNumber(bChannelNumber);

	return Py_BuildNone();
}

PyObject* netGetChannelNumber(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("i", rkNetStream.GetChannelNumber());
}
#endif

#ifdef ENABLE_OFFICIAL_SELECT_GUI
PyObject* netGetLoginID(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream& rkNetStream = CPythonNetworkStream::Instance();
	return Py_BuildValue("s", rkNetStream.GetLoginId().c_str());
}
#endif

#ifdef ENABLE_CUBE_RENEWAL
PyObject* netSendCubeClose(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::Instance().SendCubeClose();
	return Py_BuildNone();
}

PyObject* netSendCubeMake(PyObject* poSelf, PyObject* poArgs)
{
	int iNpcVnum;
	if (!PyTuple_GetInteger(poArgs, 0, &iNpcVnum))
	{
		return Py_BuildException();
	}

	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
	{
		return Py_BuildException();
	}

	int iMakeCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iMakeCount))
	{
		return Py_BuildException();
	}

	bool bIsIncrease;
	if (!PyTuple_GetBoolean(poArgs, 3, &bIsIncrease))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream::Instance().SendCubeMake(iNpcVnum, iIndex, (BYTE)iMakeCount, bIsIncrease ? 1 : 0);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_REFINE_ELEMENT
PyObject* netSendRefineElementPacket(PyObject* poSelf, PyObject* poArgs)
{
	BYTE bArg;
	if (!PyTuple_GetByte(poArgs, 0, &bArg))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::Instance().SendRefineElementPacket(bArg);
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_LUCKY_BOX
PyObject* netSendLuckyBoxAction(PyObject* poSelf, PyObject* poArgs)
{
	int iAction;
	if (!PyTuple_GetInteger(poArgs, 0, &iAction))
	{
		return Py_BuildException();
	}

	CPythonNetworkStream::Instance().SendLuckyBoxAction(iAction);
	return Py_BuildNone();
}
#endif

#ifdef INGAME_WIKI
PyObject* netToggleWikiWindow(PyObject* poSelf, PyObject* poArgs)
{
	CPythonNetworkStream::Instance().ToggleWikiWindow();
	return Py_BuildNone();
}
#endif

#ifdef ENABLE_OFFLINE_SHOP
PyObject* netSendOfflineShopCreatePacket(PyObject* self, PyObject* args)
{
	char* shopName;
	int32_t x, y;
	int32_t decoRace;
	uint8_t decoBoard;

	if (!PyTuple_GetString(args, 0, &shopName) ||
			!PyTuple_GetInteger(args, 1, &x) ||
			!PyTuple_GetInteger(args, 2, &y) ||
			!PyTuple_GetInteger(args, 3, &decoRace) ||
			!PyTuple_GetByte(args, 4, &decoBoard))
	{
		return Py_BadArgument();
	}

	auto itemDict = PyTuple_GetItem(args, 5);
	if (!itemDict)
	{
		return Py_BadArgument();
	}

	std::vector<net_offline_shop::CG_item_info> items;

	auto keys = PyDict_Keys(itemDict);
	auto values = PyDict_Values(itemDict);

	PyObject* key, * value;
	for (size_t i = 0; i < PyList_GET_SIZE(keys); ++i)
	{
		key = PyList_GET_ITEM(keys, i);
		value = PyList_GET_ITEM(values, i);
		if (!key || !value)
		{
			return Py_BadArgument();
		}

		net_offline_shop::CG_item_info itemInfo;
		itemInfo.shopPosition = PyLong_AsUnsignedLong(key);
		itemInfo.inventoryPosition.window_type = PyLong_AsLong(PyDict_GetItem(value, Py_BuildValue("s", "SLOT_TYPE")));
		itemInfo.inventoryPosition.cell = PyLong_AsLong(PyDict_GetItem(value, Py_BuildValue("s", "SLOT_INDEX")));
		itemInfo.price = PyLong_AsLongLong(PyDict_GetItem(value, Py_BuildValue("s", "PRICE")));

		items.push_back(itemInfo);
	}

	CPythonOfflineShop::SendCreatePacket(shopName, x, y, decoRace, decoBoard, items);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopAddViewerPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	if (!PyTuple_GetInteger(args, 0, &vid))
	{
		return Py_BadArgument();
	}

	CPythonOfflineShop::SendAddViewerPacket(vid);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopReopenPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	if (!PyTuple_GetInteger(args, 0, &vid))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendReopenPacket();

	return Py_BuildNone();
}

PyObject* netSendOfflineShopClosePacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	if (!PyTuple_GetInteger(args, 0, &vid))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendClosePacket();

	return Py_BuildNone();
}

PyObject* netSendOfflineShopRemoveViewerPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	if (!PyTuple_GetInteger(args, 0, &vid))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendRemoveViewerPacket();

	return Py_BuildNone();
}

PyObject* netSendOfflineShopBuyItemPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	uint8_t shopPosition;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetByte(args, 1, &shopPosition))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendBuyItemPacket(shopPosition);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopChangeNamePacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	char* shopName;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetString(args, 1, &shopName))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	std::string name(shopName);
	shop->SendChangeNamePacket(name);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopAddItemPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	uint8_t shopPosition;
	TItemPos inventoryPosition;
	int64_t price;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetByte(args, 1, &shopPosition) ||
			!PyTuple_GetInteger(args, 2, &inventoryPosition.window_type) ||
			!PyTuple_GetInteger(args, 3, &inventoryPosition.cell) ||
			!PyTuple_GetLongLong(args, 4, &price))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendAddItemPacket(shopPosition, inventoryPosition, price);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopMoveItemPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	uint8_t oldShopPosition, newShopPosition;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetByte(args, 1, &oldShopPosition) ||
			!PyTuple_GetByte(args, 2, &newShopPosition))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendMoveItemPacket(oldShopPosition, newShopPosition);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopRemoveItemPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	uint8_t shopPosition;
	TItemPos inventoryPosition;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetByte(args, 1, &shopPosition) ||
			!PyTuple_GetInteger(args, 2, &inventoryPosition.window_type) ||
			!PyTuple_GetInteger(args, 3, &inventoryPosition.cell))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendRemoveItemPacket(shopPosition, inventoryPosition);

	return Py_BuildNone();
}

PyObject* netSendOfflineShopWithdrawPacket(PyObject* self, PyObject* args)
{
	int32_t vid;
	int64_t gold;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetLongLong(args, 1, &gold))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->SendWithdrawPacket(gold);

	return Py_BuildNone();
}

PyObject* netSetOfflineShopManagerHandler(PyObject* self, PyObject* args)
{
	PyObject* handler;
	if (!PyTuple_GetObject(args, 0, &handler))
	{
		return Py_BadArgument();
	}

	CPythonOfflineShop::SetManagerHandler(handler);

	return Py_BuildNone();
}

PyObject* netRegisterOfflineShopHandler(PyObject* self, PyObject* args)
{
	int32_t vid;
	PyObject* handler;
	if (!PyTuple_GetInteger(args, 0, &vid) ||
			!PyTuple_GetObject(args, 1, &handler))
	{
		return Py_BadArgument();
	}

	auto shop = CPythonOfflineShop::Get(vid);
	if (!shop)
	{
		// TraceError(__FUNCTION__ ": No offline shop registered by vid %d.", vid);
		return Py_BuildNone();
	}

	shop->RegisterHandler(handler);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_SHOP_SEARCH
PyObject* netSendShopSearchByName(PyObject* self, PyObject* args)
{
	int32_t page;
	uint8_t entryCountIdx;
	uint8_t sortType;
	char* itemName;
	if (!PyTuple_GetInteger(args, 0, &page) ||
			!PyTuple_GetInteger(args, 1, &entryCountIdx) ||
			!PyTuple_GetInteger(args, 2, &sortType) ||
			!PyTuple_GetString(args, 3, &itemName))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::Instance().SendShopSearchByName(itemName, page, entryCountIdx, sortType);

	return Py_BuildNone();
}

bool SendShopSearchByOption_AppendVnum(std::vector<DWORD>& vnumVector, PyObject* vnumItem)
{
	if (PyInt_Check(vnumItem))
	{
		DWORD value = PyInt_AsLong(vnumItem);
		vnumVector.push_back(value);

		return true;
	}
	else if (PyString_Check(vnumItem))
	{
		std::string valueRange = PyString_AsString(vnumItem);
		if (std::all_of(valueRange.begin(), valueRange.end(), ::isdigit))
		{
			DWORD value = std::stoi(valueRange.c_str());
			vnumVector.push_back(value);

			return true;
		}
		else
		{
			size_t pos = valueRange.find_first_of('~');
			if (pos == std::string::npos)
			{
				return false;
			}

			std::string valueStart, valueEnd;
			valueStart.assign(valueRange.c_str(), valueRange.c_str() + pos);
			valueEnd.assign(valueRange.c_str() + pos + 1);

			if (!valueStart.length() || !std::all_of(valueStart.begin(), valueStart.end(), ::isdigit) ||
					!valueEnd.length() || !std::all_of(valueEnd.begin(), valueEnd.end(), ::isdigit))
			{
				return false;
			}

			DWORD dwValueStart = std::stoi(valueStart.c_str());
			DWORD dwValueEnd = std::stoi(valueEnd.c_str());
			if (dwValueEnd == dwValueStart)
			{
				return false;
			}

			if (dwValueEnd < dwValueStart)
			{
				std::swap(dwValueStart, dwValueEnd);
			}

			// set flag that it's a value range and not a simple value
			SET_BIT(dwValueStart, 1 << (8 * sizeof(DWORD) - 1));

			vnumVector.push_back(dwValueStart);
			vnumVector.push_back(dwValueEnd);
			return true;
		}
	}

	return false;
}

PyObject* netSendShopSearchByOptions(PyObject* self, PyObject* args)
{
	int32_t page;
	uint8_t entryCountIdx;
	uint8_t sortType;
	TShopSearchOptions options;
	ZeroMemory(&options, sizeof(options));

	if (!PyTuple_GetInteger(args, 0, &page))
	{
		return Py_BadArgument();
	}

	if (!PyTuple_GetInteger(args, 1, &entryCountIdx))
	{
		return Py_BadArgument();
	}

	if (!PyTuple_GetInteger(args, 2, &sortType))
	{
		return Py_BadArgument();
	}

	// itemTypes
	PyObject* itemTypesObject;
	if (!PyTuple_GetObject(args, 3, &itemTypesObject))
	{
		return Py_BadArgument();
	}
	// go through optional list of itemtypes
	std::vector<PyObject*> typeObjects;
	if (PyList_Check(itemTypesObject))
	{
		for (int i = 0; i < PyList_Size(itemTypesObject); ++i)
		{
			PyObject* typeObj = PyList_GetItem(itemTypesObject, i);
			if (typeObj)
			{
				typeObjects.push_back(typeObj);
			}
		}
	}
	else
	{
		typeObjects.push_back(itemTypesObject);
	}
	// go through optional tuple of itemtypes + subtypes
	std::vector<TShopSearchItemType> typeFlags;
	for (PyObject* typeObj : typeObjects)
	{
		TShopSearchItemType typeFlagValue;

		if (PyTuple_Check(typeObj))
		{
			int itemType;
			if (!PyTuple_GetInteger(typeObj, 0, &itemType))
			{
				return Py_BuildException("SendShopSearchName : invalid typeObj [not itemType(int) as first element in tuple]");
			}

			typeFlagValue.first = itemType;
			typeFlagValue.second = 0;

			for (int i = 1; i < PyTuple_Size(typeObj); ++i)
			{
				int subType;
				if (!PyTuple_GetInteger(typeObj, i, &subType))
				{
					return Py_BuildException("SendShopSearchName : invalid typeObj [not subItemType(int) as %d. element in tuple for type %d]", i, itemType);
				}

				if (subType >= SHOPSEARCH_ITEM_SUBTYPE_MAX_VALUE) { subType = SHOPSEARCH_ITEM_SUBTYPE_MAX_VALUE - 1; }
				SET_BIT(typeFlagValue.second, 1 << subType);
			}
		}
		else
		{
			int itemType = (int)PyInt_AsLong(typeObj);

			typeFlagValue.first = itemType;
			typeFlagValue.second = UINT_MAX; // support all subtypes
		}

		typeFlags.push_back(typeFlagValue);
	}
	// update type flag count by vector size
	options.typeFlagCount = typeFlags.size();

	// specificVnums
	std::vector<DWORD> specificVnumList;
	PyObject* vnumTuple;
	if (PyTuple_GetObject(args, 4, &vnumTuple))
	{
		if (PyTuple_Check(vnumTuple))
		{
			for (int i = 0; i < PyTuple_Size(vnumTuple); ++i)
			{
				PyObject* curVnumItem = PyTuple_GetItem(vnumTuple, i);
				if (!curVnumItem)
				{
					return Py_BuildException("SendShopSearchName : invalid specificVnumList [no object as %d. element in tuple]", i);
				}

				if (!SendShopSearchByOption_AppendVnum(specificVnumList, curVnumItem))
				{
					return Py_BuildException("SendShopSearchName : invalid specificVnumList [invalid object as %d. element in tuple]", i);
				}
			}
		}
		else if (!SendShopSearchByOption_AppendVnum(specificVnumList, vnumTuple))
		{
			return Py_BuildException("SendShopSearchName : invalid specificVnumList (no tuple) [invalid vnum object]");
		}

		options.specificVnumCount = specificVnumList.size();
	}

	CPythonNetworkStream::Instance().SendShopSearchByOptions(options, &typeFlags[0], &specificVnumList[0], page, entryCountIdx, sortType);

	return Py_BuildNone();
}

PyObject* netSendShopSearchBuy(PyObject* self, PyObject* args)
{
	PyObject* itemIDTuple;
	if (!PyTuple_GetObject(args, 0, &itemIDTuple))
	{
		return Py_BadArgument();
	}

	// extract ID
	TShopSearchItemID itemID;
	if (!PyTuple_GetUnsignedInteger(itemIDTuple, 0, &itemID.first) ||
			!PyTuple_GetInteger(itemIDTuple, 1, &itemID.second))
	{
		return Py_BadArgument();
	}

	int itemVnum;
	if (!PyTuple_GetInteger(args, 1, &itemVnum))
	{
		return Py_BadArgument();
	}

	long long itemPrice;
	if (!PyTuple_GetLongLong(args, 2, &itemPrice))
	{
		return Py_BadArgument();
	}

	CPythonShopSearch::Instance().RemoveResultItem(itemID);
	CPythonNetworkStream::Instance().SendShopSearchBuy(itemID, itemVnum, itemPrice);

	return Py_BuildNone();
}

PyObject* netSendShopSearchOwnerMessage(PyObject* self, PyObject* args)
{
	int ownerID;
	if (!PyTuple_GetInteger(args, 0, &ownerID))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::Instance().SendShopSearchOwnerMessage(ownerID);

	return Py_BuildNone();
}

PyObject* netSendShopSearchRequestSoldInfo(PyObject* self, PyObject* args)
{
	int itemVnum;
	if (!PyTuple_GetInteger(args, 0, &itemVnum))
	{
		return Py_BadArgument();
	}

	CPythonNetworkStream::Instance().SendShopSearchRequestSoldInfo(itemVnum);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_PING_TIME
PyObject* netGetPingTime(PyObject* self, PyObject* args)
{
	DWORD ping_time = CPythonNetworkStream::Instance().GetCurrentPingTime();
	return Py_BuildValue("i", ping_time);
}
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
PyObject* netSendWhisperRequestLanguage(PyObject* self, PyObject* args)
{
	char* targetName;
	if (!PyTuple_GetString(args, 0, &targetName))
		return Py_BadArgument();

	CPythonNetworkStream::Instance().SendWhisperRequestLanguage(targetName);

	return Py_BuildNone();
}
#endif

void initnet()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetBettingGuildWarValue",			netGetBettingGuildWarValue,				METH_VARARGS },
		{ "EnableChatInsultFilter",				netEnableChatInsultFilter,				METH_VARARGS },
		{ "SetServerInfo",						netSetServerInfo,						METH_VARARGS },
		{ "GetServerInfo",						netGetServerInfo,						METH_VARARGS },
		{ "PreserveServerCommand",				netPreserveServerCommand,				METH_VARARGS },
		{ "GetPreservedServerCommand",			netGetPreservedServerCommand,			METH_VARARGS },

		{ "StartGame",							netStartGame,							METH_VARARGS },
		{ "Warp",								netWarp,								METH_VARARGS },
		{ "IsTest",								netIsTest,								METH_VARARGS },
		{ "SetMarkServer",						netSetMarkServer,						METH_VARARGS },
		{ "IsChatInsultIn",						netIsChatInsultIn,						METH_VARARGS },
		{ "IsInsultIn",							netIsInsultIn,							METH_VARARGS },
		{ "LoadInsultList",						netLoadInsultList,						METH_VARARGS },
		{ "UploadMark",							netUploadMark,							METH_VARARGS },
		{ "UploadSymbol",						netUploadSymbol,						METH_VARARGS },
		{ "GetGuildID",							netGetGuildID,							METH_VARARGS },
		{ "GetEmpireID",						netGetEmpireID,							METH_VARARGS },
		{ "GetMainActorVID",					netGetMainActorVID,						METH_VARARGS },
		{ "GetMainActorRace",					netGetMainActorRace,					METH_VARARGS },
		{ "GetMainActorEmpire",					netGetMainActorEmpire,					METH_VARARGS },
		{ "GetMainActorSkillGroup",				netGetMainActorSkillGroup,				METH_VARARGS },
		{ "GetAccountCharacterSlotDataInteger",	netGetAccountCharacterSlotDataInteger,	METH_VARARGS },
		{ "GetAccountCharacterSlotDataString",	netGetAccountCharacterSlotDataString,	METH_VARARGS },

		// FIELD_MUSIC
		{ "GetFieldMusicFileName",				netGetFieldMusicFileName,				METH_VARARGS },
		{ "GetFieldMusicVolume",				netGetFieldMusicVolume,					METH_VARARGS },
		// END_OF_FIELD_MUSIC

		{ "ToggleGameDebugInfo",				netToggleGameDebugInfo,					METH_VARARGS },
		{ "SetLoginInfo",						netSetLoginInfo,						METH_VARARGS },
		{ "SetPhaseWindow",						netSetPhaseWindow,						METH_VARARGS },
		{ "ClearPhaseWindow",					netClearPhaseWindow,					METH_VARARGS },
		{ "SetServerCommandParserWindow",		netSetServerCommandParserWindow,		METH_VARARGS },
		{ "SetAccountConnectorHandler",			netSetAccountConnectorHandler,			METH_VARARGS },
		{ "SetHandler",							netSetHandler,							METH_VARARGS },
		{ "SetTCPRecvBufferSize",				netSetTCPRecvBufferSize,				METH_VARARGS },
		{ "SetTCPSendBufferSize",				netSetTCPSendBufferSize,				METH_VARARGS },
		{ "SetUDPRecvBufferSize",				netSetUDPRecvBufferSize,				METH_VARARGS },
		{ "DirectEnter",						netDirectEnter,							METH_VARARGS },

		{ "LogOutGame",							netLogOutGame,							METH_VARARGS },
		{ "ExitGame",							netExitGame,							METH_VARARGS },
		{ "ExitApplication",					netExitApplication,						METH_VARARGS },
		{ "ConnectTCP",							netConnectTCP,							METH_VARARGS },
		{ "ConnectUDP",							netConnectUDP,							METH_VARARGS },
		{ "ConnectToAccountServer",				netConnectToAccountServer,				METH_VARARGS },

		{ "SendLoginPacket",					netSendLoginPacket,						METH_VARARGS },
		{ "SendNEWCIBNPasspodAnswerPacket",		netSendNEWCIBNPasspodAnswerPacket,		METH_VARARGS },
		{ "SendSelectEmpirePacket",				netSendSelectEmpirePacket,				METH_VARARGS },
		{ "SendSelectCharacterPacket",			netSendSelectCharacterPacket,			METH_VARARGS },
		{ "SendChangeNamePacket",				netSendChangeNamePacket,				METH_VARARGS },
		{ "SendCreateCharacterPacket",			netSendCreateCharacterPacket,			METH_VARARGS },
		{ "SendDestroyCharacterPacket",			netSendDestroyCharacterPacket,			METH_VARARGS },
		{ "SendEnterGamePacket",				netSendEnterGamePacket,					METH_VARARGS },

		{ "SendItemUsePacket",					netSendItemUsePacket,					METH_VARARGS },
#ifdef ENABLE_MULTI_USE_PACKET
		{ "SendItemMultiUsePacket",				netSendItemMultiUsePacket,				METH_VARARGS },
#endif
		{ "SendItemUseToItemPacket",			netSendItemUseToItemPacket,				METH_VARARGS },
		{ "SendItemDropPacket",					netSendItemDropPacket,					METH_VARARGS },
		{ "SendItemDropPacketNew",				netSendItemDropPacketNew,				METH_VARARGS },
		{ "SendElkDropPacket",					netSendElkDropPacket,					METH_VARARGS },
		{ "SendGoldDropPacketNew",				netSendGoldDropPacketNew,				METH_VARARGS },
		{ "SendItemMovePacket",					netSendItemMovePacket,					METH_VARARGS },
		{ "SendItemPickUpPacket",				netSendItemPickUpPacket,				METH_VARARGS },
		{ "SendGiveItemPacket",					netSendGiveItemPacket,					METH_VARARGS },
#ifdef ENABLE_DESTROY_ITEM_PACKET
		{ "SendItemDestroyPacket",				netSendItemDestroyPacket,				METH_VARARGS },
#endif

		{ "SetOfflinePhase",					netSetOfflinePhase,						METH_VARARGS },
		{ "Disconnect",							netDisconnect,							METH_VARARGS },
		{ "IsConnect",							netIsConnect,							METH_VARARGS },

		{ "SendChatPacket",						netSendChatPacket,						METH_VARARGS },
		{ "SendEmoticon",						netSendEmoticon,						METH_VARARGS },
		{ "SendWhisperPacket",					netSendWhisperPacket,					METH_VARARGS },

		{ "SendCharacterPositionPacket",		netSendCharacterPositionPacket,			METH_VARARGS },

		{ "SendShopEndPacket",					netSendShopEndPacket,					METH_VARARGS },
		{ "SendShopBuyPacket",					netSendShopBuyPacket,					METH_VARARGS },
#ifdef ENABLE_MULTIPLE_BUY_SYSTEM
		{ "SendShopMultipleBuyPacket",			netSendShopMultipleBuyPacket,			METH_VARARGS },
#endif
		{ "SendShopSellPacket",					netSendShopSellPacket,					METH_VARARGS },
		{ "SendShopSellPacketNew",				netSendShopSellPacketNew,				METH_VARARGS },

		{ "SendExchangeStartPacket",			netSendExchangeStartPacket,				METH_VARARGS },
		{ "SendExchangeItemAddPacket",			netSendExchangeItemAddPacket,			METH_VARARGS },
		{ "SendExchangeItemDelPacket",			netSendExchangeItemDelPacket,			METH_VARARGS },
		{ "SendExchangeElkAddPacket",			netSendExchangeElkAddPacket,			METH_VARARGS },
		{ "SendExchangeAcceptPacket",			netSendExchangeAcceptPacket,			METH_VARARGS },
		{ "SendExchangeExitPacket",				netSendExchangeExitPacket,				METH_VARARGS },

		{ "SendOnClickPacket",					netOnClickPacket,						METH_VARARGS },

		// Emoticon String
		{ "RegisterEmoticonString",				netRegisterEmoticonString,				METH_VARARGS },

		// Messenger
		{ "SendMessengerAddByVIDPacket",		netSendMessengerAddByVIDPacket,			METH_VARARGS },
		{ "SendMessengerAddByNamePacket",		netSendMessengerAddByNamePacket,		METH_VARARGS },
		{ "SendMessengerRemovePacket",			netSendMessengerRemovePacket,			METH_VARARGS },

		// Party
		{ "SendPartyInvitePacket",				netSendPartyInvitePacket,				METH_VARARGS },
		{ "SendPartyInviteAnswerPacket",		netSendPartyInviteAnswerPacket,			METH_VARARGS },
		{ "SendPartyExitPacket",				netSendPartyExitPacket,					METH_VARARGS },
		{ "SendPartyRemovePacket",				netSendPartyRemovePacketPID,			METH_VARARGS },
		{ "SendPartyRemovePacketPID",			netSendPartyRemovePacketPID,			METH_VARARGS },
		{ "SendPartyRemovePacketVID",			netSendPartyRemovePacketVID,			METH_VARARGS },
		{ "SendPartySetStatePacket",			netSendPartySetStatePacket,				METH_VARARGS },
		{ "SendPartyUseSkillPacket",			netSendPartyUseSkillPacket,				METH_VARARGS },
		{ "SendPartyParameterPacket",			netSendPartyParameterPacket,			METH_VARARGS },

		// Safebox
		{ "SendSafeboxSaveMoneyPacket",			netSendSafeboxSaveMoneyPacket,			METH_VARARGS },
		{ "SendSafeboxWithdrawMoneyPacket",		netSendSafeboxWithdrawMoneyPacket,		METH_VARARGS },
		{ "SendSafeboxCheckinPacket",			netSendSafeboxCheckinPacket,			METH_VARARGS },
		{ "SendSafeboxCheckoutPacket",			netSendSafeboxCheckoutPacket,			METH_VARARGS },
		{ "SendSafeboxItemMovePacket",			netSendSafeboxItemMovePacket,			METH_VARARGS },

		// Mall
		{ "SendMallCheckoutPacket",				netSendMallCheckoutPacket,				METH_VARARGS },

		// Guild
		{ "SendAnswerMakeGuildPacket",				netSendAnswerMakeGuildPacket,				METH_VARARGS },
		{ "SendQuestInputStringPacket",				netSendQuestInputStringPacket,				METH_VARARGS },
		{ "SendQuestConfirmPacket",					netSendQuestConfirmPacket,					METH_VARARGS },
		{ "SendGuildAddMemberPacket",				netSendGuildAddMemberPacket,				METH_VARARGS },
		{ "SendGuildRemoveMemberPacket",			netSendGuildRemoveMemberPacket,				METH_VARARGS },
		{ "SendGuildChangeGradeNamePacket",			netSendGuildChangeGradeNamePacket,			METH_VARARGS },
		{ "SendGuildChangeGradeAuthorityPacket",	netSendGuildChangeGradeAuthorityPacket,		METH_VARARGS },
		{ "SendGuildOfferPacket",					netSendGuildOfferPacket,					METH_VARARGS },
		{ "SendGuildPostCommentPacket",				netSnedGuildPostCommentPacket,				METH_VARARGS },
		{ "SendGuildDeleteCommentPacket",			netSnedGuildDeleteCommentPacket,			METH_VARARGS },
		{ "SendGuildRefreshCommentsPacket",			netSendGuildRefreshCommentsPacket,			METH_VARARGS },
		{ "SendGuildChangeMemberGradePacket",		netSendGuildChangeMemberGradePacket,		METH_VARARGS },
		{ "SendGuildUseSkillPacket",				netSendGuildUseSkillPacket,					METH_VARARGS },
		{ "SendGuildChangeMemberGeneralPacket",		netSendGuildChangeMemberGeneralPacket,		METH_VARARGS },
		{ "SendGuildInviteAnswerPacket",			netSendGuildInviteAnswerPacket,				METH_VARARGS },
		{ "SendGuildChargeGSPPacket",				netSendGuildChargeGSPPacket,				METH_VARARGS },
		{ "SendGuildDepositMoneyPacket",			netSendGuildDepositMoneyPacket,				METH_VARARGS },
		{ "SendGuildWithdrawMoneyPacket",			netSendGuildWithdrawMoneyPacket,			METH_VARARGS },

		// Refine
		{ "SendRequestRefineInfoPacket",			netSendRequestRefineInfoPacket,				METH_VARARGS },
		{ "SendRefinePacket",						netSendRefinePacket,						METH_VARARGS },
		{ "SendSelectItemPacket",					netSendSelectItemPacket,					METH_VARARGS },

		// SYSTEM
		{ "SetPacketSequenceMode",					netSetPacketSequenceMode,					METH_VARARGS },
		{ "SetEmpireLanguageMode",					netSetEmpireLanguageMode,					METH_VARARGS },

		// For Test
		{ "SetSkillGroupFake",						netSetSkillGroupFake,						METH_VARARGS },

		// Guild Symbol
		{ "SendGuildSymbol",						netSendGuildSymbol,							METH_VARARGS },
		{ "DisconnectUploader",						netDisconnectUploader,						METH_VARARGS },
		{ "RecvGuildSymbol",						netRecvGuildSymbol,							METH_VARARGS },

		// Log
		{ "RegisterErrorLog",						netRegisterErrorLog,						METH_VARARGS },

#ifdef ENABLE_TREASURE_BOX_LOOT
		{ "SendRequestTreasureBoxLoot",				netSendRequestTreasureBoxLoot,				METH_VARARGS },
#endif
#ifdef ENABLE_TARGET_MONSTER_LOOT
		{ "SendLoadTargetInfo",						netSendLoadTargetInfo,						METH_VARARGS },
#endif
#ifdef ENABLE_DELETE_SINGLE_STONE
		{ "SendRequestDeleteSocket",				netSendRequestDeleteSocket,					METH_VARARGS },
#endif
#ifdef ENABLE_VOICE_CHAT
		{ "netSendVoiceStart",						netSendVoiceStart,							METH_VARARGS },
#endif

#ifdef ENABLE_CHANGE_CHANNEL
		{ "SetMapIndex",							netSetMapIndex,								METH_VARARGS },
		{ "GetMapIndex",							netGetMapIndex,								METH_VARARGS },
		{ "SetChannelNumber",						netSetChannelNumber,						METH_VARARGS },
		{ "GetChannelNumber",						netGetChannelNumber,						METH_VARARGS },
#endif

#ifdef ENABLE_OFFICIAL_SELECT_GUI
		{ "GetLoginID",								netGetLoginID,								METH_VARARGS },
#endif

#ifdef ENABLE_CUBE_RENEWAL
		{ "SendCubeClose",							netSendCubeClose,							METH_VARARGS },
		{ "SendCubeMake",							netSendCubeMake,							METH_VARARGS },
#endif

#ifdef ENABLE_REFINE_ELEMENT
		{ "SendRefineElementPacket",				netSendRefineElementPacket,					METH_VARARGS },
#endif

#ifdef ENABLE_LUCKY_BOX
		{ "SendLuckyBoxAction",						netSendLuckyBoxAction,						METH_VARARGS },
#endif

#ifdef INGAME_WIKI
		{ "ToggleWikiWindow",						netToggleWikiWindow,						METH_VARARGS },
#endif

#ifdef ENABLE_OFFLINE_SHOP
		{ "SendOfflineShopCreatePacket",			netSendOfflineShopCreatePacket,				METH_VARARGS },
		{ "SendOfflineShopAddViewerPacket",			netSendOfflineShopAddViewerPacket,			METH_VARARGS },
		{ "SendOfflineShopReopenPacket",			netSendOfflineShopReopenPacket,				METH_VARARGS },
		{ "SendOfflineShopClosePacket",				netSendOfflineShopClosePacket,				METH_VARARGS },
		{ "SendOfflineShopRemoveViewerPacket",		netSendOfflineShopRemoveViewerPacket,		METH_VARARGS },
		{ "SendOfflineShopBuyItemPacket",			netSendOfflineShopBuyItemPacket,			METH_VARARGS },
		{ "SendOfflineShopChangeNamePacket",		netSendOfflineShopChangeNamePacket,			METH_VARARGS },
		{ "SendOfflineShopAddItemPacket",			netSendOfflineShopAddItemPacket,			METH_VARARGS },
		{ "SendOfflineShopMoveItemPacket",			netSendOfflineShopMoveItemPacket,			METH_VARARGS },
		{ "SendOfflineShopRemoveItemPacket",		netSendOfflineShopRemoveItemPacket,			METH_VARARGS },
		{ "SendOfflineShopWithdrawPacket",			netSendOfflineShopWithdrawPacket,			METH_VARARGS },
		{ "SetOfflineShopManagerHandler",			netSetOfflineShopManagerHandler,			METH_VARARGS },
		{ "RegisterOfflineShopHandler",				netRegisterOfflineShopHandler,				METH_VARARGS },
#endif

#ifdef ENABLE_SHOP_SEARCH
		{ "SendShopSearchByName",					netSendShopSearchByName,					METH_VARARGS },
		{ "SendShopSearchByOptions",				netSendShopSearchByOptions,					METH_VARARGS },
		{ "SendShopSearchBuy",						netSendShopSearchBuy,						METH_VARARGS },
		{ "SendShopSearchOwnerMessage",				netSendShopSearchOwnerMessage,				METH_VARARGS },
		{ "SendShopSearchRequestSoldInfo",			netSendShopSearchRequestSoldInfo,			METH_VARARGS },
#endif

#ifdef ENABLE_PING_TIME
		{ "GetPingTime",							netGetPingTime,								METH_VARARGS },
#endif

#ifdef __MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
		{ "SendWhisperRequestLanguage",				netSendWhisperRequestLanguage,				METH_VARARGS },
#endif

		{ NULL,										NULL,										NULL },
	};

	PyObject* poModule = Py_InitModule("net", s_methods);

	PyModule_AddIntConstant(poModule, "ERROR_NONE", CPythonNetworkStream::ERROR_NONE);
	PyModule_AddIntConstant(poModule, "ERROR_CONNECT_MARK_SERVER", CPythonNetworkStream::ERROR_CONNECT_MARK_SERVER);
	PyModule_AddIntConstant(poModule, "ERROR_LOAD_MARK", CPythonNetworkStream::ERROR_LOAD_MARK);
	PyModule_AddIntConstant(poModule, "ERROR_MARK_WIDTH", CPythonNetworkStream::ERROR_MARK_WIDTH);
	PyModule_AddIntConstant(poModule, "ERROR_MARK_HEIGHT", CPythonNetworkStream::ERROR_MARK_HEIGHT);

	// MARK_BUG_FIX
	PyModule_AddIntConstant(poModule, "ERROR_MARK_UPLOAD_NEED_RECONNECT", CPythonNetworkStream::ERROR_MARK_UPLOAD_NEED_RECONNECT);
	PyModule_AddIntConstant(poModule, "ERROR_MARK_CHECK_NEED_RECONNECT", CPythonNetworkStream::ERROR_MARK_CHECK_NEED_RECONNECT);
	// END_OF_MARK_BUG_FIX

	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOGIN", CPythonNetworkStream::PHASE_WINDOW_LOGIN);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_SELECT", CPythonNetworkStream::PHASE_WINDOW_SELECT);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_CREATE", CPythonNetworkStream::PHASE_WINDOW_CREATE);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOAD", CPythonNetworkStream::PHASE_WINDOW_LOAD);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_GAME", CPythonNetworkStream::PHASE_WINDOW_GAME);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_EMPIRE", CPythonNetworkStream::PHASE_WINDOW_EMPIRE);
	PyModule_AddIntConstant(poModule, "PHASE_WINDOW_LOGO", CPythonNetworkStream::PHASE_WINDOW_LOGO);

	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_ID", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ID);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_NAME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_NAME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_RACE", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_RACE);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_LEVEL", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_LEVEL);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_STR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_STR);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_DEX", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_DEX);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_INT", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_INT);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_HTH", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HTH);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_PLAYTIME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PLAYTIME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_FORM", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_FORM);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_ADDR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_ADDR);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_PORT", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_PORT);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_GUILD_ID", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_ID);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_GUILD_NAME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_GUILD_NAME);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_CHANGE_NAME_FLAG);
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_HAIR", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_HAIR);
#ifdef ENABLE_SASH_COSTUME_SYSTEM
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_SASH", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_SASH);
#endif
	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_LOG_OUT",	CPythonNetworkStream::SERVER_COMMAND_LOG_OUT);
	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER",	CPythonNetworkStream::SERVER_COMMAND_RETURN_TO_SELECT_CHARACTER);
	PyModule_AddIntConstant(poModule, "SERVER_COMMAND_QUIT",	CPythonNetworkStream::SERVER_COMMAND_QUIT);

	PyModule_AddIntConstant(poModule, "EMPIRE_A", 1);
	PyModule_AddIntConstant(poModule, "EMPIRE_B", 2);
	PyModule_AddIntConstant(poModule, "EMPIRE_C", 3);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL", DS_SUB_HEADER_REFINE_FAIL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE", DS_SUB_HEADER_REFINE_FAIL_MAX_REFINE);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_INVALID_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY", DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MONEY);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_NOT_ENOUGH_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL", DS_SUB_HEADER_REFINE_FAIL_TOO_MUCH_MATERIAL);
	PyModule_AddIntConstant(poModule, "DS_SUB_HEADER_REFINE_SUCCEED", DS_SUB_HEADER_REFINE_SUCCEED);

#ifdef ENABLE_DELETE_SINGLE_STONE
	PyModule_AddIntConstant(poModule, "SUBHEADER_REQUEST_DELETE_SOCKET_OPEN", SUBHEADER_REQUEST_DELETE_SOCKET_OPEN);
	PyModule_AddIntConstant(poModule, "SUBHEADER_REQUEST_DELETE_SOCKET_DELETE", SUBHEADER_REQUEST_DELETE_SOCKET_DELETE);
	PyModule_AddIntConstant(poModule, "SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE", SUBHEADER_REQUEST_DELETE_SOCKET_CLOSE);
#endif

#ifdef ENABLE_ADMIN_MANAGER
	PyModule_AddIntConstant(poModule, "PLAYER_PER_ACCOUNT5", PLAYER_PER_ACCOUNT5);
#endif

#ifdef ENABLE_BIOLOG_SYSTEM
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_OPEN", CG_BIOLOG_MANAGER_OPEN);
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_SEND", CG_BIOLOG_MANAGER_SEND);
	PyModule_AddIntConstant(poModule, "BIOLOG_MANAGER_TIMER", CG_BIOLOG_MANAGER_TIMER);
#endif

#ifdef ENABLE_OFFICIAL_SELECT_GUI
	PyModule_AddIntConstant(poModule, "ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME", CPythonNetworkStream::ACCOUNT_CHARACTER_SLOT_LAST_PLAYTIME);
#endif
}
