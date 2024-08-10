#ifndef __INC_METIN_II_SERVER_LOGINSIM__
#define __INC_METIN_II_SERVER_LOGINSIM__

#include "desc_client.h"

class CLoginSim
{
public:
	CLoginSim()
	{
		memset(&auth, 0, sizeof(TPacketGDAuthLogin));
		memset(&login, 0, sizeof(TPacketGDLoginByKey));
		memset(&load, 0, sizeof(TPlayerLoadPacket));
		memset(&logout, 0, sizeof(TLogoutPacket));
		vecIdx = 0;
		dwHandle = 0;
		loginKey = 0;
		bCheck = false;
	}

	void AddPlayer(DWORD dwID)
	{
		vecID.push_back(dwID);
		sys_log(0, "AddPlayer %lu", dwID);
	}

	bool IsCheck()
	{
		return bCheck;
	}

	void SendLogin()
	{
		bCheck = true;

		if (vecIdx > 0 && IsDone())
			return;

		if (vecIdx == 0)
		{

			db_clientdesc->DBPacket(HEADER_GD_AUTH_LOGIN, dwHandle, &auth, sizeof(TPacketGDAuthLogin));
			sys_log(0, "AuthLoginSend handle: %d", dwHandle);
			return;
		}

		load.player_id = vecID[vecIdx++];
		db_clientdesc->DBPacket(HEADER_GD_LOGIN_BY_KEY, dwHandle, &login, sizeof(TPacketGDLoginByKey));
	}

	void SendLoad()
	{
		db_clientdesc->DBPacket(HEADER_GD_PLAYER_LOAD, dwHandle, &load, sizeof(TPlayerLoadPacket));
	}

	void SendLogout()
	{
		db_clientdesc->DBPacket(HEADER_GD_LOGOUT, dwHandle, &logout, sizeof(TLogoutPacket));
		SendLogin();
	}

	bool IsDone()
	{
		if (vecIdx >= vecID.size())
			return true;

		return false;
	}

	void CopyAuthPacket(TPacketGDAuthLogin newPacket)
	{
		std::memcpy(&auth, &newPacket, sizeof(TPacketGDAuthLogin));
	}

	DWORD GetLoginKey()
	{
		return loginKey;
	}

	void SetLoginKey(DWORD key)
	{
		loginKey = key;
	}

	void SetHandle(DWORD handle)
	{
		dwHandle = handle;
	}


	TPacketGDAuthLogin auth;
	TPacketGDLoginByKey login;
	TPlayerLoadPacket load;
	TLogoutPacket logout;

	std::vector<DWORD> vecID;
	DWORD	loginKey, dwHandle;
	unsigned int vecIdx;
	bool bCheck;
};

#endif
