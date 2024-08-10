#pragma once
#include "stdafx.h"

#ifdef __ADMIN_MANAGER__
#include "../../common/length.h"
#include "desc.h"
#include "p2p.h"

class TEMP_BUFFER;

#define EVENT_FLAG_GM_ITEM_TRADE_BLOCK "gm_item_trade_block"

class CAdminManager : public singleton<CAdminManager>
{
public:
	enum ETeamAllowFlags
	{
		ALLOW_VIEW_ONLINE_LIST = 1 << 0,
		ALLOW_VIEW_ONLINE_COUNT = 1 << 1,
		ALLOW_GM_TRADE_BLOCK_OPTION = 1 << 2,
		ALLOW_MAPVIEWER = 1 << 3,
		ALLOW_VIEW_MAPVIEWER_NPC = 1 << 4,
		ALLOW_VIEW_MAPVIEWER_STONE = 1 << 5,
		ALLOW_VIEW_MAPVIEWER_MONSTER = 1 << 6,
		ALLOW_VIEW_MAPVIEWER_PLAYER = 1 << 7,
		ALLOW_OBSERVER = 1 << 8,
		ALLOW_VIEW_OBSERVER_GOLD = 1 << 9,
		ALLOW_VIEW_OBSERVER_SKILL = 1 << 10,
		ALLOW_VIEW_OBSERVER_EQUIPMENT = 1 << 11,
		ALLOW_VIEW_OBSERVER_INVENTORY = 1 << 12,
		ALLOW_OBSERVER_CHATBAN = 1 << 14,
		ALLOW_OBSERVER_ACCOUNTBAN = 1 << 15,
		ALLOW_OBSERVER_PERMA_ACCOUNTBAN = 1 << 16,
		ALLOW_BAN = 1 << 17,
		ALLOW_VIEW_BAN_CHAT = 1 << 18,
		ALLOW_VIEW_BAN_CHAT_LOG = 1 << 19,
		ALLOW_BAN_CHAT = 1 << 20,
		ALLOW_VIEW_BAN_ACCOUNT = 1 << 21,
		ALLOW_VIEW_BAN_ACCOUNT_LOG = 1 << 22,
		ALLOW_BAN_ACCOUNT = 1 << 23,
		ALLOW_ITEM = 1 << 24,
		ALLOW_VIEW_ITEM = 1 << 25,
	};

	enum ECGSubHeaders
	{
		CG_SUBHEADER_MAPVIEWER_START,
		CG_SUBHEADER_MAPVIEWER_STOP,
		CG_SUBHEADER_OBSERVER_START,
		CG_SUBHEADER_OBSERVER_STOP,
		CG_SUBHEADER_BAN_CHAT_SEARCH,
		CG_SUBHEADER_BAN_REQUEST_LOG,
		CG_SUBHEADER_BAN_ACCOUNT_SEARCH,
		CG_SUBHEADER_BAN_ACCOUNT,
		CG_SUBHEADER_ITEM_SEARCH,

		CG_SUBHEADER_GET_LOGS,
	};

	enum EGCSubHeaders
	{
		GC_SUBHEADER_NONE,
		GC_SUBHEADER_PLAYER_ONLINE,
		GC_SUBHEADER_PLAYER_OFFLINE,
		GC_SUBHEADER_GM_ITEM_TRADE_BLOCK,
		GC_SUBHEADER_MAPVIEWER_LOAD,
		GC_SUBHEADER_MAPVIEWER_PLAYER_MOVE,
		GC_SUBHEADER_MAPVIEWER_PLAYER_STATE,
		GC_SUBHEADER_MAPVIEWER_PLAYER_APPEND,
		GC_SUBHEADER_MAPVIEWER_PLAYER_DESTROY,
		GC_SUBHEADER_MAPVIEWER_MONSTER_MOVE,
		GC_SUBHEADER_MAPVIEWER_MONSTER_STATE,
		GC_SUBHEADER_MAPVIEWER_MONSTER_APPEND,
		GC_SUBHEADER_MAPVIEWER_MONSTER_DESTROY,
		GC_SUBHEADER_OBSERVER_LOAD,
		GC_SUBHEADER_OBSERVER_SKILLGROUP,
		GC_SUBHEADER_OBSERVER_SKILL_UPDATE,
		GC_SUBHEADER_OBSERVER_SKILL_UPDATE_ALL,
		GC_SUBHEADER_OBSERVER_SKILL_COOLDOWN,
		GC_SUBHEADER_OBSERVER_POINT_UPDATE,
		GC_SUBHEADER_OBSERVER_ITEM_SET,
		GC_SUBHEADER_OBSERVER_ITEM_DEL,
		GC_SUBHEADER_OBSERVER_CHATBAN_STATE,
		GC_SUBHEADER_OBSERVER_ACCOUNTBAN_STATE,
		GC_SUBHEADER_OBSERVER_STOP_FORCED,
		GC_SUBHEADER_BAN_CHAT_STATE,
		GC_SUBHEADER_BAN_CHAT_SEARCH_PLAYER,
		GC_SUBHEADER_BAN_PLAYER_LOG,
		GC_SUBHEADER_BAN_ACCOUNT_STATE,
		GC_SUBHEADER_BAN_ACCOUNT_SEARCH,
		GC_SUBHEADER_ITEM_SEARCH,

		GC_SUBHEADER_RECV_LOGS,
	};

	enum EP2PSubHeaders
	{
		P2P_SUBHEADER_LOGOUT_PID,
		P2P_SUBHEADER_START_MAPVIEWER,
		P2P_SUBHEADER_STOP_MAPVIEWER,
		P2P_SUBHEADER_START_OBSERVER,
		P2P_SUBHEADER_STOP_OBSERVER,
		P2P_SUBHEADER_STOP_OBSERVER_BY_FORCE,
		P2P_SUBHEADER_BAN_PLAYER_CHATBAN,
		P2P_SUBHEADER_BAN_CHAT_SEARCH_PLAYER,
		P2P_SUBHEADER_BAN_ACCOUNT_REQUEST,
		P2P_SUBHEADER_BAN_ACCOUNT_DISCONNECT,
		P2P_SUBHEADER_BAN_ACCOUNTBAN,
		P2P_SUBHEADER_BAN_ACCOUNT_SEARCH,
	};

	enum EBanTypes
	{
		BAN_TYPE_CHAT = 1,
		BAN_TYPE_ACCOUNT = 2,
	};

	enum EBanAccountSearchTypes
	{
		BAN_ACCOUNT_SEARCH_ACCOUNT,
		BAN_ACCOUNT_SEARCH_PLAYER,
	};

	enum EItemOwnerTypes
	{
		ITEM_OWNER_PLAYER,
		ITEM_OWNER_ACCOUNT,
	};

	enum EItemSearchTypes
	{
		ITEM_SEARCH_IID,
		ITEM_SEARCH_INAME,
		ITEM_SEARCH_PID,
		ITEM_SEARCH_PNAME,
		ITEM_SEARCH_GM_ITEM,
	};

	typedef struct SChatBanPlayerInfo
	{
		char	szName[CHARACTER_NAME_MAX_LEN + 1];
		BYTE	bRace;
		BYTE	bLevel;
		WORD	wChatbanCount;
		WORD	wAccountbanCount;
		DWORD	dwDuration;
		DWORD	dwOnlineTime;

		SChatBanPlayerInfo() : dwDuration(0), dwOnlineTime(0)
		{
		}
		void	SetOnline()
		{
			dwOnlineTime = get_dword_time() / 1000;
		}
		void	SetOffline()
		{
			if (!IsOnline())
			{
				sys_err("cannot set offline for already offline player %s", szName);
				return;
			}

			dwDuration = GetDuration();
			dwOnlineTime = 0;
		}
		bool	IsOnline() const
		{
			return dwOnlineTime != 0;
		}
		DWORD	GetDuration()
		{
			if (!dwDuration)
			{
				return 0;
			}

			if (!dwOnlineTime)
			{
				return dwDuration;
			}

			DWORD dwTimeOnline = get_dword_time() / 1000 - dwOnlineTime;
			if (dwTimeOnline >= dwDuration)
			{
				return 0;
			}

			return dwDuration - dwTimeOnline;
		}
		void	SetDuration(DWORD dwDuration)
		{
			this->dwDuration = dwDuration;

			if (dwOnlineTime)
			{
				dwOnlineTime = get_dword_time();
			}
		}
	} TChatBanPlayerInfo;

	typedef struct SAccountBanPlayerInfo
	{
		char	szLoginName[LOGIN_MAX_LEN + 1];
		DWORD	dwPID[PLAYER_PER_ACCOUNT];
		char	szName[PLAYER_PER_ACCOUNT][CHARACTER_NAME_MAX_LEN + 1];
		BYTE	bRace[PLAYER_PER_ACCOUNT];
		BYTE	bLevel[PLAYER_PER_ACCOUNT];
		WORD	wChatbanCount[PLAYER_PER_ACCOUNT];
		WORD	wAccountbanCount;
		DWORD	dwDuration;

		DWORD	GetDuration()
		{
			if (!dwDuration)
			{
				return 0;
			}

			if (dwDuration == INT_MAX)
			{
				return dwDuration;
			}

			int iTimeLeft = (dwDuration - get_dword_time()) / 1000;
			if (iTimeLeft < 0)
			{
				return 0;
			}

			return iTimeLeft;
		}
		void	SetDuration(DWORD dwDuration = 0)
		{
			if (dwDuration == 0)
			{
				this->dwDuration = 0;
			}
			else
				this->dwDuration = get_dword_time() + dwDuration ? dwDuration : 0 * 1000;
		}
	} TAccountBanPlayerInfo;

	typedef TAdminManagerBanClientPlayerInfo TBanClientPlayerInfo;
	typedef TAdminManagerBanClientAccountInfo TBanClientAccountInfo;

public:
	CAdminManager();
	~CAdminManager();

	void	Initialize(DWORD* padwConfig, bool bReload = false);
	void	Destroy();

	// general
	TAdminManagerPlayerInfo&	GetPlayerInfo(LPCHARACTER pkChr);
	TAdminManagerPlayerInfo&	GetPlayerInfo(CCI* pCCI);
	DWORD	GetPlayerInfoList(std::vector<TAdminManagerPlayerInfo>& rvec_PlayerInfo);

	// team allow
	BYTE	GetTeamRankByPID(DWORD dwPID) const;
	DWORD	GetAllow(BYTE bTeamRank) const;
	DWORD	GetAllow(LPCHARACTER pkChr) const;
	bool	HasAllow(BYTE bTeamRank, DWORD dwAllowFlag) const;
	bool	HasAllow(LPCHARACTER pkChr, DWORD dwAllowFlag) const;

	// mapviewer
	TAdminManagerMapViewerPlayerInfo&	GetMapViewerPlayerInfo(LPCHARACTER pkChr);
	TAdminManagerMapViewerMobInfo&		GetMapViewerMonsterInfo(LPCHARACTER pkChr);
	DWORD	GetMapViewerNPCAllowFlag(LPCHARACTER pkNPC) const;
	bool	CheckMapViewerNPC(LPCHARACTER pkChr, DWORD dwAllowFlag = ALLOW_VIEW_MAPVIEWER_MONSTER | ALLOW_VIEW_MAPVIEWER_NPC | ALLOW_VIEW_MAPVIEWER_STONE);
	long	GetMapViewerMapIndex(DWORD dwPID);
	bool	IsRunningMapViewer(DWORD dwPID);
	void	StartMapViewer(LPCHARACTER pkChr, long lMapIndex);
	void	StartMapViewer(DWORD dwPID, long lMapIndex);
	void	StopMapViewer(DWORD dwPID);
	// mapviewer events
	void	MapViewer_OnPlayerLogin(LPCHARACTER pkChr);
	void	MapViewer_OnPlayerLogout(LPCHARACTER pkChr);
	void	MapViewer_OnPlayerMove(LPCHARACTER pkChr);
	void	MapViewer_OnPlayerStateChange(LPCHARACTER pkChr);
	void	MapViewer_OnMonsterCreate(LPCHARACTER pkChr);
	void	MapViewer_OnMonsterDestroy(LPCHARACTER pkChr);
	void	MapViewer_OnMonsterMove(LPCHARACTER pkChr);
	void	MapViewer_OnMonsterStateChange(LPCHARACTER pkChr);

	// observer
	bool	IsObserverUpdatePoint(unsigned char bType) const;
	DWORD	GetObserverPID(DWORD dwPID);
	bool	IsRunningObserver(DWORD dwPID);
	void	StartObserver(LPCHARACTER pkChr, const char* c_pszPlayerName, bool bSendFailMessage);
	void	StartObserver(DWORD dwPID, DWORD dwPlayerPID);
	bool	StopObserver(DWORD dwPID, bool bIsForced = false);
	// observer events
	void	Observer_OnPlayerSkillGroupChange(LPCHARACTER pkChr);
	void	Observer_OnPlayerSkillChange(LPCHARACTER pkChr, DWORD dwSkillVnum);
	void	Observer_OnPlayerSkillCooldown(LPCHARACTER pkChr, DWORD dwSkillVnum, DWORD dwCoolTime);
	void	Observer_OnPlayerPointChange(LPCHARACTER pkChr, unsigned char bType, int64_t llValue);
	void	Observer_OnPlayerItemSet(LPCHARACTER pkChr, WORD wCell);
	void	Observer_OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft);
	void	Observer_OnPlayerAccountban(DWORD dwPID, DWORD dwTimeLeft);

	// ban
	TBanClientPlayerInfo&			GetBanClientPlayerInfo(LPCHARACTER pkChr, DWORD dwDuration = 0);
	TBanClientPlayerInfo&			GetBanClientPlayerInfo(DWORD dwPID, TChatBanPlayerInfo* pPlayerInfo);
	TBanClientAccountInfo&			GetBanClientAccountInfo(DWORD dwAID, TAccountBanPlayerInfo* pAccountInfo);
	void	InitializeBan();
	void	SearchBanChatPlayer(LPCHARACTER pkChr, const char* c_pszName);
	void	SearchBanChatPlayer(DWORD dwGMPid, const char* c_pszName);
	void	P2P_SearchBanChatPlayer(DWORD dwGMPid, bool bSuccess, TBanClientPlayerInfo* pPlayerInfo);
	void	SendBanPlayerLog(LPCHARACTER pkChr, DWORD dwPlayerID);
	void	CreateAccountBanInfo(TAccountBanPlayerInfo* pInfo, LPCHARACTER pkChr, DWORD dwAID = 0);
	void	DoAccountBan(LPCHARACTER pkChrGM, DWORD dwAID, DWORD dwPlayerID, DWORD dwTimeLeft, const char* c_pszReason, const char* c_pszProof, bool bIncreaseBanCounter);
	void	P2P_DoAccountBan(DWORD dwGMPid, BYTE bRequestorGMLevel, DWORD dwAID, const char* c_pszLoginName, DWORD dwTimeLeft, bool bIncreaseBanCounter);
	bool	__DoAccountBan(DWORD dwAID, TAccountBanPlayerInfo* pAccountInfo, DWORD dwGMPid, BYTE bRequestorGMLevel);
	void	P2P_DoAccountBanDisconnect(const char* c_pszLoginName);
	void	SearchBanAccount(LPCHARACTER pkChr, const char* c_pszName, BYTE bSearchType);
	void	SearchBanAccount(DWORD dwGMPid, const char* c_pszName, BYTE bSearchType);
	void	P2P_SearchBanAccount(DWORD dwGMPid, bool bSuccess, TBanClientAccountInfo* pAccountInfo);
	void	SendBanAccountLog(LPCHARACTER pkChr, DWORD dwAccountID);
	// ban events
	void	Ban_OnPlayerLogin(DWORD dwPID);
	void	Ban_OnPlayerLogout(DWORD dwPID);
	void	Ban_OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft);
	void	Ban_OnPlayerChatban(const char* c_pszName, DWORD dwTimeLeft, bool bIncreaseBanCounter);
	void	Ban_OnPlayerChatban(const TBanClientPlayerInfo* pPlayerInfo);
	void	Ban_OnPlayerAccountban(const TBanClientAccountInfo* pAccountInfo);

	// item
	void	SearchItem(LPCHARACTER pkChr, BYTE bSearchType, const char* c_pszData);

	// logs
	void	GetLogs(LPCHARACTER pkChr);

	// client packet
	void	SendLoadPacket(LPCHARACTER pkChr);
	void	SendMapViewerLoadPacket(DWORD dwPID, long lMapIndex);
	void	EncodeObserverItemPacket(TAdminManagerObserverItemInfo& rkItemInfo, LPITEM pkItem);
	void	SendObserverLoadPacket(DWORD dwPID, LPCHARACTER pkPlayer);
	void	SendObserverPacket(const std::set<DWORD>& rset_ObserverPIDs, unsigned char bSubHeader, const void* c_pvData, int iSize, DWORD dwCheckAllow = 0);
	void	SendClientPacket(LPDESC pkDesc, unsigned char bSubHeader, const void* c_pvData, int iSize);
	void	SendClientPacket(DWORD dwPID, unsigned char bSubHeader, const void* c_pvData, int iSize);
	void	SendClientPacket(unsigned char bSubHeader, const void* c_pvData, int iSize, DWORD dwCheckAllow);
	int		RecvClientPacket(LPCHARACTER pkChr, unsigned char bSubHeader, const char* c_pData, size_t uiBytes);

	// p2p
	void	SendP2P(LPDESC pkDesc, unsigned char bSubHeader, const void* c_pvData, int iSize);
	int		RecvP2P(unsigned char bSubHeader, const char* c_pData);

	// events
	void	OnLoginPlayer(LPCHARACTER pkChr);
	void	OnP2PLoginPlayer(DWORD dwPID);
	void	OnLogoutPlayer(LPCHARACTER pkChr);
	void	OnLogoutPlayer(DWORD dwPID);
	void	OnP2PLogoutPlayer(DWORD dwPID);
	void	OnP2PDisconnect(LPDESC pkDesc);

	void	OnPlayerMove(LPCHARACTER pkChr);
	void	OnPlayerStateChange(LPCHARACTER pkChr);
	void	OnMonsterCreate(LPCHARACTER pkMob);
	void	OnMonsterDestroy(LPCHARACTER pkMob);
	void	OnMonsterMove(LPCHARACTER pkMob);
	void	OnMonsterStateChange(LPCHARACTER pkMob);

	void	OnPlayerSkillGroupChange(LPCHARACTER pkChr);
	void	OnPlayerSkillChange(LPCHARACTER pkChr, DWORD dwSkillVnum = 0);
	void	OnPlayerSkillCooldown(LPCHARACTER pkChr, DWORD dwSkillVnum, DWORD dwCoolTime);
	void	OnPlayerPointChange(LPCHARACTER pkChr, unsigned char bType, int64_t llValue);
	void	OnPlayerItemSet(LPCHARACTER pkChr, WORD wCell);
	void	OnPlayerChatban(LPCHARACTER pkChr, DWORD dwTimeLeft);
	void	OnPlayerChatban(const char* c_pszName, DWORD dwTimeLeft, bool bIncreaseBanCounter);
	void	OnPlayerAccountban(const TBanClientAccountInfo* pAccountInfo);

	void	OnEventFlagChange(const std::string& c_rstFlagName, int iNewValue);

	void	OnRefreshHackLog(LPCHARACTER pkChr);

private:
	DWORD									m_adwConfig[GM_MAX_NUM];

	std::set<LPCHARACTER>					m_set_ActivePlayers;
	std::set<DWORD>							m_set_ActivePIDs;
	std::map<DWORD, std::set<LPDESC> >		m_map_PlayerP2PConnections;

	// mapviewer
	std::map<DWORD, long>					m_map_MapViewerByPID;
	std::map<long, std::set<DWORD> >		m_map_MapViewerByMapIndex;

	// observer
	std::map<DWORD, DWORD>					m_map_ObserverByPID;
	std::map<DWORD, std::set<DWORD> >		m_map_ObserverSetByPID;

	// ban
	std::map<DWORD, TChatBanPlayerInfo>		m_map_BanChatList;
	std::map<std::string, DWORD>			m_map_BanChatListByName;
	std::map<DWORD, TAccountBanPlayerInfo>	m_map_BanAccountList;
	std::map<std::string, DWORD>			m_map_BanAccountListByName;
	std::map<std::string, DWORD>			m_map_BanAccountListByPlayerName;
	std::map<DWORD, DWORD>					m_map_BanAccountListByPlayerID;
};
#endif