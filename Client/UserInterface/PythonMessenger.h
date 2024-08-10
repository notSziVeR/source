#pragma once

class CPythonMessenger : public CSingleton<CPythonMessenger>
{
public:
	typedef std::set<std::string> TFriendNameMap;
	typedef std::map<std::string, BYTE> TGuildMemberStateMap;
#ifdef ENABLE_TEAMLER_STATUS
	typedef std::set<std::string> TTeamNameMap;
#endif

	enum EMessengerGroupIndex
	{
		MESSENGER_GRUOP_INDEX_FRIEND,
		MESSENGER_GRUOP_INDEX_GUILD,
#ifdef ENABLE_TEAMLER_STATUS
		MESSENGER_GROUP_INDEX_TEAM,
#endif
	};

public:
	CPythonMessenger();
	virtual ~CPythonMessenger();

	void Destroy();

	// Friend
	void RemoveFriend(const char * c_szKey);
	void OnFriendLogin(const char * c_szKey);
	void OnFriendLogout(const char * c_szKey);
	void SetMobile(const char * c_szKey, BYTE byState);
	BOOL IsFriendByKey(const char * c_szKey);
	BOOL IsFriendByName(const char * c_szName);

	// Guild
	void AppendGuildMember(const char * c_szName);
	void RemoveGuildMember(const char * c_szName);
	void RemoveAllGuildMember();
	void LoginGuildMember(const char * c_szName);
	void LogoutGuildMember(const char * c_szName);
	void RefreshGuildMember();

#ifdef ENABLE_TEAMLER_STATUS
	// Team
	void OnTeamLogin(const char * c_szKey);
	void OnTeamLogout(const char * c_szKey);
	void RefreshTeamlerState();
#endif

	void SetMessengerHandler(PyObject* poHandler);

protected:
	TFriendNameMap m_FriendNameMap;
	TGuildMemberStateMap m_GuildMemberStateMap;
#ifdef ENABLE_TEAMLER_STATUS
	TTeamNameMap			m_TeamNameMap;
#endif

private:
	PyObject * m_poMessengerHandler;
};
