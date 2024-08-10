#include "StdAfx.h"
#include "PythonNetworkStream.h"
#include "Packet.h"
#include "Test.h"
#include "AccountConnector.h"

#include "Hackshield.h"
#include "WiseLogicXTrap.h"

// Login ---------------------------------------------------------------------------
void CPythonNetworkStream::LoginPhase()
{
	TPacketHeader header;
	if (!CheckPacket(&header))
	{
		return;
	}

#if defined(_DEBUG) && defined(ENABLE_PRINT_RECV_PACKET_DEBUG)
	Tracenf("RECV HEADER : %u , phase %s ", header, m_strPhase.c_str());
#endif

	switch (header)
	{
	case HEADER_GC_PHASE:
		if (RecvPhasePacket())
		{
			return;
		}
		break;

	case HEADER_GC_LOGIN_SUCCESS:
		if (__RecvLoginSuccessPacket())
		{
			return;
		}
		break;

	case HEADER_GC_LOGIN_FAILURE:
		if (__RecvLoginFailurePacket())
		{
			return;
		}
		break;

	case HEADER_GC_EMPIRE:
		if (__RecvEmpirePacket())
		{
			return;
		}
		break;

	case HEADER_GC_CHINA_MATRIX_CARD:
		if (__RecvChinaMatrixCardPacket())
		{
			return;
		}
		break;

	case HEADER_GC_NEWCIBN_PASSPOD_REQUEST:
		if (__RecvNEWCIBNPasspodRequestPacket())
		{
			return;
		}
		break;
	case HEADER_GC_NEWCIBN_PASSPOD_FAILURE:
		if (__RecvNEWCIBNPasspodFailurePacket())
		{
			return;
		}
		break;


	case HEADER_GC_LOGIN_KEY:
		if (__RecvLoginKeyPacket())
		{
			return;
		}
		break;

	case HEADER_GC_PING:
		if (RecvPingPacket())
		{
			return;
		}
		break;

	case HEADER_GC_HYBRIDCRYPT_KEYS:
		RecvHybridCryptKeyPacket();
		return;
		break;

	case HEADER_GC_HYBRIDCRYPT_SDB:
		RecvHybridCryptSDBPacket();
		return;
		break;

	default:
		if (RecvDefaultPacket(header))
		{
			return;
		}
		break;
	}

	RecvErrorPacket(header);
}

void CPythonNetworkStream::SetLoginPhase()
{
#ifndef _IMPROVED_PACKET_ENCRYPTION_
	const char* key = LocaleService_GetSecurityKey();
	SetSecurityMode(true, key);
#endif

	if ("Login" != m_strPhase)
	{
		m_phaseLeaveFunc.Run();
	}

	Tracen("");
	Tracen("## Network - Login Phase ##");
	Tracen("");

	m_strPhase = "Login";

	m_phaseProcessFunc.Set(this, &CPythonNetworkStream::LoginPhase);
	m_phaseLeaveFunc.Set(this, &CPythonNetworkStream::__LeaveLoginPhase);

	m_dwChangingPhaseTime = ELTimer_GetMSec();

	if (__DirectEnterMode_IsSet())
	{
		if (0 != m_dwLoginKey)
		{
			SendLoginPacketNew(m_stID.c_str(), m_stPassword.c_str()
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
			, locale_
#endif
			);
		}
		else
		{
			SendLoginPacket(m_stID.c_str(), m_stPassword.c_str());
		}

		ClearLoginInfo();
		CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
		rkAccountConnector.ClearLoginInfo();
	}
	else
	{
		if (0 != m_dwLoginKey)
		{
			SendLoginPacketNew(m_stID.c_str(), m_stPassword.c_str()
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
				, locale_
#endif
			);
		}
		else
		{
			SendLoginPacket(m_stID.c_str(), m_stPassword.c_str());
		}

		ClearLoginInfo();
		CAccountConnector & rkAccountConnector = CAccountConnector::Instance();
		rkAccountConnector.ClearLoginInfo();

		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "OnLoginStart", Py_BuildValue("()"));

		__ClearSelectCharacterData();
	}
}

bool CPythonNetworkStream::__RecvEmpirePacket()
{
	TPacketGCEmpire kPacketEmpire;
	if (!Recv(sizeof(kPacketEmpire), &kPacketEmpire))
	{
		return false;
	}

	m_dwEmpireID = kPacketEmpire.bEmpire;
	return true;
}

bool CPythonNetworkStream::__RecvLoginSuccessPacket()
{
	TPacketGCLoginSuccess kPacketLoginSuccess;

	if (!Recv(sizeof(kPacketLoginSuccess), &kPacketLoginSuccess))
	{
		return false;
	}

	for (int i = 0; i < PLAYER_PER_ACCOUNT4; ++i)
	{
		m_akSimplePlayerInfo[i] = kPacketLoginSuccess.akSimplePlayerInformation[i];
		m_adwGuildID[i] = kPacketLoginSuccess.guild_id[i];
		m_astrGuildName[i] = kPacketLoginSuccess.guild_name[i];
	}

	m_kMarkAuth.m_dwHandle = kPacketLoginSuccess.handle;
	m_kMarkAuth.m_dwRandomKey = kPacketLoginSuccess.random_key;

	if (__DirectEnterMode_IsSet())
	{
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_SELECT], "Refresh", Py_BuildValue("()"));
	}

	return true;
}


void CPythonNetworkStream::OnConnectFailure()
{
	if (__DirectEnterMode_IsSet())
	{
		ClosePhase();
	}
	else
	{
		PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "OnConnectFailure", Py_BuildValue("()"));
	}
}


bool CPythonNetworkStream::__RecvLoginFailurePacket()
{
	TPacketGCLoginFailure packet_failure;
	if (!Recv(sizeof(TPacketGCLoginFailure), &packet_failure))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "OnLoginFailure", Py_BuildValue("(sis)", packet_failure.szStatus, packet_failure.iValue, packet_failure.szReason));
#ifdef _DEBUG
	Tracef(" RecvLoginFailurePacket : [%s]\n", packet_failure.szStatus);
#endif
	return true;
}

bool CPythonNetworkStream::SendLoginPacket(const char* c_szName, const char* c_szPassword)
{
	TPacketCGLogin LoginPacket;
	LoginPacket.header = HEADER_CG_LOGIN;

	strncpy(LoginPacket.name, c_szName, sizeof(LoginPacket.name) - 1);
	strncpy(LoginPacket.pwd, c_szPassword, sizeof(LoginPacket.pwd) - 1);
#ifdef __HARDWARE_BAN__
	extern std::string GetHardwareID();
	std::memcpy(LoginPacket.HDId, &GetHardwareID()[0], sizeof(LoginPacket.HDId) - 1);
#endif

	LoginPacket.name[ID_MAX_NUM] = '\0';
	LoginPacket.pwd[PASS_MAX_NUM] = '\0';
#ifdef __HARDWARE_BAN__
	LoginPacket.HDId[HDID_MAX_LEN] = '\0';
#endif

	if (!Send(sizeof(LoginPacket), &LoginPacket))
	{
		Tracen("SendLogin Error");
		return false;
	}

	return SendSequence();
}

bool CPythonNetworkStream::SendLoginPacketNew(const char* c_szName, const char* c_szPassword
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	, uint8_t locale
#endif
)
{
	TPacketCGLogin2 LoginPacket;
	LoginPacket.header = HEADER_CG_LOGIN2;
	LoginPacket.login_key = m_dwLoginKey;
#ifdef ENABLE_MULTI_LANGUAGE_SYSTEM
	LoginPacket.locale = locale;
#endif

	strncpy(LoginPacket.name, c_szName, sizeof(LoginPacket.name) - 1);
	LoginPacket.name[ID_MAX_NUM] = '\0';

	extern DWORD g_adwEncryptKey[4];
	extern DWORD g_adwDecryptKey[4];
	for (DWORD i = 0; i < 4; ++i)
	{
		LoginPacket.adwClientKey[i] = g_adwEncryptKey[i];
	}

	if (!Send(sizeof(LoginPacket), &LoginPacket))
	{
		Tracen("SendLogin Error");
		return false;
	}

	if (!SendSequence())
	{
		Tracen("SendLogin Error");
		return false;
	}

	__SendInternalBuffer();

#ifndef _IMPROVED_PACKET_ENCRYPTION_
	SetSecurityMode(true, (const char *) g_adwEncryptKey, (const char *) g_adwDecryptKey);
#endif
	return true;
}

bool CPythonNetworkStream::__RecvNEWCIBNPasspodRequestPacket()
{
	TPacketGCNEWCIBNPasspodRequest kRequestPacket;
	if (!Recv(sizeof(kRequestPacket), &kRequestPacket))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "BINARY_OnNEWCIBNPasspodRequest", Py_BuildValue("()"));
	return true;
}

bool CPythonNetworkStream::__RecvNEWCIBNPasspodFailurePacket()
{
	TPacketGCNEWCIBNPasspodFailure kFailurePacket;
	if (!Recv(sizeof(kFailurePacket), &kFailurePacket))
	{
		return false;
	}

	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "BINARY_OnNEWCIBNPasspodFailure", Py_BuildValue("()"));
	return true;
}


bool CPythonNetworkStream::SendNEWCIBNPasspodAnswerPacket(const char * answer)
{
	TPacketCGNEWCIBNPasspodAnswer answerPacket;
	answerPacket.bHeader = HEADER_CG_NEWCIBN_PASSPOD_ANSWER;
	strncpy(answerPacket.szAnswer, answer, NEWCIBN_PASSPOD_ANSWER_MAX_LEN);
	answerPacket.szAnswer[NEWCIBN_PASSPOD_ANSWER_MAX_LEN] = '\0';
	if (!Send(sizeof(answerPacket), &answerPacket))
	{
		TraceError("SendNEWCIBNPasspodAnswerPacket");
		return false;
	}
	return SendSequence();
}


#define ROW(rows, i) ((rows >> ((4 - i - 1) * 8)) & 0x000000FF)
#define COL(cols, i) ((cols >> ((4 - i - 1) * 8)) & 0x000000FF)

bool CPythonNetworkStream::__RecvChinaMatrixCardPacket()
{
	TPacketGCChinaMatrixCard kMatrixCardPacket;
	if (!Recv(sizeof(TPacketGCChinaMatrixCard), &kMatrixCardPacket))
	{
		return false;
	}

	PyObject * pyValue = Py_BuildValue("(iiiiiiii)",	ROW(kMatrixCardPacket.dwRows, 0),
									   ROW(kMatrixCardPacket.dwRows, 1),
									   ROW(kMatrixCardPacket.dwRows, 2),
									   ROW(kMatrixCardPacket.dwRows, 3),
									   COL(kMatrixCardPacket.dwCols, 0),
									   COL(kMatrixCardPacket.dwCols, 1),
									   COL(kMatrixCardPacket.dwCols, 2),
									   COL(kMatrixCardPacket.dwCols, 3));
	PyCallClassMemberFunc(m_apoPhaseWnd[PHASE_WINDOW_LOGIN], "OnMatrixCard", pyValue);
	return true;
}

bool CPythonNetworkStream::__RecvLoginKeyPacket()
{
	TPacketGCLoginKey kLoginKeyPacket;
	if (!Recv(sizeof(TPacketGCLoginKey), &kLoginKeyPacket))
	{
		return false;
	}

	m_dwLoginKey = kLoginKeyPacket.dwLoginKey;
	m_isWaitLoginKey = FALSE;

	return true;
}
