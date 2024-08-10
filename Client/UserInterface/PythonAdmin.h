#pragma once

#include "StdAfx.h"

#ifdef ENABLE_ADMIN_MANAGER
#include "Packet.h"

class CPythonAdmin : public CSingleton<CPythonAdmin>
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
		ALLOW_VIEW_OBSERVER_WHISPER = 1 << 13,
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

	enum EBanTypes
	{
		BAN_TYPE_CHAT = 1,
		BAN_TYPE_ACCOUNT = 2,
	};

	enum EBanAccountSearchTypes
	{
		BAN_ACCOUNT_SEARCH_ACCOUNT,
		BAN_ACCOUNT_SEARCH_PLAYER,
		BAN_ACCOUNT_SEARCH_MAX_NUM,
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

	enum EGeneralPlayerSortOptions
	{
		GENERAL_ONLINE_PLAYER_SORT_BY_PID,
		GENERAL_ONLINE_PLAYER_SORT_BY_NAME,
		GENERAL_ONLINE_PLAYER_SORT_BY_MAP_INDEX,
		GENERAL_ONLINE_PLAYER_SORT_BY_CHANNEL,
		GENERAL_ONLINE_PLAYER_SORT_BY_EMPIRE,
	};

	typedef struct SWhisperMessage
	{
		DWORD							dwSenderPID;
		DWORD							dwSentTime;
		char							szMessage[512 + 1];
	} TWhisperMessage;
	typedef struct SWhisper
	{
		DWORD							dwOtherPID;
		std::string						stOtherName;
		std::vector<TWhisperMessage>	vec_kMessages;
	} TWhisper;

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
			dwOnlineTime = CTimer::Instance().GetCurrentSecond();
		}
		void	SetOffline()
		{
			if (!IsOnline())
			{
				return;
			}

			dwDuration = GetDuration();
			dwOnlineTime = 0;
		}
		bool	IsOnline() const
		{
			return dwOnlineTime != 0;
		}
		DWORD	GetDuration() const
		{
			if (!dwDuration)
			{
				return 0;
			}

			if (!dwOnlineTime)
			{
				return dwDuration;
			}

			DWORD dwTimeOnline = CTimer::Instance().GetCurrentSecond() - dwOnlineTime;
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
				dwOnlineTime = CTimer::Instance().GetCurrentSecond();
			}
		}
	} TChatBanPlayerInfo;

	typedef struct SAccountBanInfo
	{
		char	szLoginName[ID_MAX_NUM + 1];
		DWORD	dwPID[PLAYER_PER_ACCOUNT5];
		char	szName[PLAYER_PER_ACCOUNT5][CHARACTER_NAME_MAX_LEN + 1];
		BYTE	bRace[PLAYER_PER_ACCOUNT5];
		BYTE	bLevel[PLAYER_PER_ACCOUNT5];
		WORD	wChatbanCount[PLAYER_PER_ACCOUNT5];
		WORD	wAccountbanCount;
		DWORD	dwDuration;
		bool	bIsOnline[PLAYER_PER_ACCOUNT5];

		DWORD	GetDuration() const
		{
			if (!dwDuration)
			{
				return 0;
			}

			if (dwDuration == INT_MAX)
			{
				return dwDuration;
			}

			int iTimeLeft = dwDuration - CTimer::Instance().GetCurrentSecond();
			if (iTimeLeft < 0)
			{
				return 0;
			}

			return iTimeLeft;
		}
		void	SetDuration(DWORD dwDuration)
		{
			if (!dwDuration)
			{
				this->dwDuration = 0;
			}
			else
			{
				this->dwDuration = CTimer::Instance().GetCurrentSecond() + dwDuration;
			}
		}
	} TAccountBanInfo;

	typedef struct SItemInfo
	{
		BYTE		bOwnerType;
		DWORD		dwOwnerID;
		std::string	stOwnerName;
		TItemPos	kCell;
		DWORD		dwItemVnum;
		BYTE		bCount;
		long		alSockets[ITEM_SOCKET_SLOT_MAX_NUM];
		TPlayerItemAttribute	aAttr[ITEM_ATTRIBUTE_SLOT_MAX_NUM];
		bool		bIsGMItem;
	} TItemInfo;

	typedef struct SHackInfo
	{
		std::string stHackerName;
		std::string stReason;
		int dwHackCount;
		std::string stTime;
	} THackInfo;

public:
	CPythonAdmin();
	~CPythonAdmin();

	void	Initialize();
	void	Clear();

	// general
	bool	CanOpenAdminWindow();

	void	SetGameWindow(PyObject* poWindow);

	void	SetAllowFlag(DWORD dwAllowFlag);
	DWORD	GetAllowFlag() const;
	bool	HasAllow(DWORD dwAllowFlag) const;

	bool	AddOnlinePlayer(const TAdminManagerPlayerInfo* c_pPlayerInfo);
	bool	RemoveOnlinePlayer(DWORD dwPID);

	DWORD	GetOnlinePlayerCount();
	const TAdminManagerPlayerInfo*	GetOnlinePlayerByIndex(DWORD dwIndex);
	const TAdminManagerPlayerInfo*	GetOnlinePlayerByPID(DWORD dwPID);

	void	SortOnlinePlayer(BYTE bSortType, BYTE bDirection);
	bool	IsOnlinePlayerSorted() const;
	int		GetOnlinePlayerSortType() const { return m_iOnlinePlayerSortType; }
	BYTE	GetOnlinePlayerSortDirection() const { return m_bOnlinePlayerDirection; }
	const TAdminManagerPlayerInfo*	GetSortOnlinePlayerByIndex(DWORD dwIndex);

	void	SetGMItemTradeBlock(bool bIsBlock);
	bool	IsGMItemTradeBlock() const;

	// mapviewer
	void				MapViewer_Clear();

	void				MapViewer_LoadMapInfo(DWORD dwBaseX, DWORD dwBaseY);
	const std::string&	MapViewer_GetMapName();
	DWORD				MapViewer_GetBaseX();
	DWORD				MapViewer_GetBaseY();
	DWORD				MapViewer_GetSizeX();
	DWORD				MapViewer_GetSizeY();

	void				MapViewer_AddPlayer(const TAdminManagerMapViewerPlayerInfo& rkPlayer);
	void				MapViewer_ErasePlayer(DWORD dwPID);
	void				MapViewer_AddMonster(const TAdminManagerMapViewerMobInfo& rkMonster);
	void				MapViewer_EraseMonster(DWORD dwVID);
	TAdminManagerMapViewerPlayerInfo*							MapViewer_GetPlayerByPID(DWORD dwPID);
	const std::map<DWORD, TAdminManagerMapViewerPlayerInfo>&	MapViewer_GetPlayer();
	TAdminManagerMapViewerMobInfo*								MapViewer_GetMonsterByVID(DWORD dwVID);
	const std::map<DWORD, TAdminManagerMapViewerMobInfo>&		MapViewer_GetMonster();

	// observer
	void				Observer_Clear(bool bClearMessages = true);

	void				Observer_StopRunning();
	bool				Observer_IsRunning() const;
	bool				Observer_IsStoppedByForce() const;
	void				Observer_SetPID(DWORD dwPID);
	DWORD				Observer_GetPID() const;
	void				Observer_SetAID(DWORD dwAID);
	DWORD				Observer_GetAID() const;
	void				Observer_SetLoginName(const std::string& c_rstLoginName);
	const std::string&	Observer_GetLoginName() const;
	void				Observer_SetRaceNum(BYTE bRaceNum);
	BYTE				Observer_GetRaceNum() const;
	void				Observer_LoadMapName(DWORD dwX, DWORD dwY);
	const std::string&	Observer_GetMapName() const;

	void				Observer_SetSkillGroup(BYTE bSkillGroup);
	BYTE				Observer_GetSkillGroup() const;
	void				Observer_SetSkill(DWORD dwSkillVnum, TPlayerSkill* pkSkillData);
	const TPlayerSkill*	Observer_GetSkill(DWORD dwSkillVnum) const;
	void				Observer_SetSkillCoolTime(DWORD dwSkillVnum, DWORD dwCoolTime);
	DWORD				Observer_GetSkillCoolTime(DWORD dwSkillVnum) const;
	DWORD				Observer_GetSkillElapsedCoolTime(DWORD dwSkillVnum) const;

	void				Observer_SetPoint(BYTE bType, int64_t llValue);
	int64_t				Observer_GetPoint(BYTE bType, int64_t llDefaultValue = -1) const;

	void									Observer_SetInventoryItem(const TAdminManagerObserverItemInfo* pItemInfo);
	void									Observer_DelInventoryItem(WORD wCell);
	const TAdminManagerObserverItemInfo*	Observer_GetInventoryItem(WORD wCell) const;

	void								Observer_AddMessage(bool bIsOtherPIDSender, DWORD dwOtherPID, DWORD dwElapsedTime, const char* szMessage, const char* szOtherName = NULL);
	const std::map<DWORD, TWhisper>&	Observer_GetMessageMap() const;

	void				Observer_SetChatBanTimeLeft(DWORD dwTimeLeft);
	DWORD				Observer_GetChatBanTimeLeft() const;

	void				Observer_SetAccountBanTimeLeft(DWORD dwTimeLeft);
	DWORD				Observer_GetAccountBanTimeLeft() const;

	// ban
	void				Ban_Clear();

	void				Ban_SetPlayerOnline(DWORD dwPID);
	void				Ban_SetPlayerOffline(DWORD dwPID);
	void				Ban_UpdateChatInfo(const TAdminManagerBanClientPlayerInfo* pPlayerInfo);
	void				Ban_SetChatSearchResult(const TAdminManagerBanClientPlayerInfo* pPlayerInfo);
	void				Ban_ClientSetChatSearchResult(DWORD dwPID);
	void				Ban_UpdateAccountInfo(const TAdminManagerBanClientAccountInfo* pAccountInfo);
	void				Ban_SetAccountSearchResult(const TAdminManagerBanClientAccountInfo* pAccountInfo);
	void				Ban_ClientSetAccountSearchResult(DWORD dwAID);

	DWORD				Ban_GetChatPlayerCount();
	const TChatBanPlayerInfo*	Ban_GetChatPlayerByIndex(DWORD dwIndex, DWORD& rdwPID);
	const TChatBanPlayerInfo*	Ban_GetChatPlayerByPID(DWORD dwPID);
	DWORD						Ban_GetChatPlayerPIDByName(const std::string& c_rstName);
	DWORD						Ban_GetChatSearchResultPID() const;

	DWORD				Ban_GetAccountCount();
	const TAccountBanInfo*		Ban_GetAccountByIndex(DWORD dwIndex, DWORD& rdwAID);
	const TAccountBanInfo*		Ban_GetAccountByAID(DWORD dwAID);
	DWORD						Ban_GetAccountAIDByName(const std::string& c_rstName);
	DWORD						Ban_GetAccountAIDByPlayerName(const std::string& c_rstName);
	DWORD						Ban_GetAccountAIDByPID(DWORD dwPID);
	DWORD						Ban_GetAccountSearchResultAID() const;

	void				Ban_ClearLogInfo();
	void				Ban_AddLogInfo(const TAdminManagerBanClientLogInfo* pLogInfo, const char* szReason, const char* szProof);
	const TAdminManagerBanLogInfo*	Ban_GetLogInfo(DWORD dwIndex);
	DWORD				Ban_GetLogInfoCount() const;
	void				Ban_SetLogInfoType(BYTE bType);
	BYTE				Ban_GetLogInfoType() const;

	// item
	void				Item_Clear();

	void				Item_ClearInfo();
	void				Item_AddInfo(TAdminManagerItemInfo* pInfo, const char* szOwnerName);
	DWORD				Item_GetInfoCount() const;
	TItemInfo*			Item_GetInfoByIndex(DWORD dwIndex, DWORD& rdwID);
	TItemInfo*			Item_GetInfoByID(DWORD dwID);

	// hack
	void				Hack_Clear();

	void				Hack_AddInfo(TAdminManagerHackLog* pInfo, int index, const char* szHacker, const char* szReason, const char* szTime);
	void				Hack_ClearInfo();
	DWORD				Hack_GetInfoCount() const;
	const THackInfo*	Hack_GetLogInfo(DWORD dwIndex);

private:
	PyObject*	m_poGameWindow;
	bool		m_bCanOpenAdminWindow;

	DWORD	m_dwAllowFlag;

	std::map<DWORD, TAdminManagerPlayerInfo>	m_map_OnlinePlayer;
	std::vector<TAdminManagerPlayerInfo>		m_vec_OnlinePlayerSorted;
	int		m_iOnlinePlayerSortType;
	BYTE	m_bOnlinePlayerDirection;

	bool	m_bGMItemTradeBlock;

	bool		m_bMapViewer_ShowMap;
	std::string	m_stMapViewer_MapName;
	DWORD		m_dwMapViewer_MapBaseX;
	DWORD		m_dwMapViewer_MapBaseY;
	DWORD		m_dwMapViewer_MapSizeX;
	DWORD		m_dwMapViewer_MapSizeY;
	std::map<DWORD, TAdminManagerMapViewerPlayerInfo>	m_map_MapViewerPlayer;
	std::map<DWORD, TAdminManagerMapViewerMobInfo>		m_map_MapViewerMob;

	bool							m_bObserver_Running;
	DWORD							m_dwObserver_PID;
	DWORD							m_dwObserver_AID;
	std::string						m_stObserver_LoginName;
	BYTE							m_bObserver_RaceNum;
	std::string						m_stObserver_MapName;
	BYTE							m_bObserver_SkillGroup;
	TPlayerSkill					m_akObserver_Skills[SKILL_MAX_NUM];
	DWORD							m_adwObserver_SkillStartCoolTimes[SKILL_MAX_NUM];
	DWORD							m_adwObserver_SkillCoolTimes[SKILL_MAX_NUM];
	std::map<BYTE, int64_t>				m_map_ObserverPoints;
	TAdminManagerObserverItemInfo	m_akObserver_Item[c_Inventory_Count];
	std::map<DWORD, TWhisper>		m_map_ObserverWhispers;
	DWORD							m_dwObserverChatBanTimeout;
	DWORD							m_dwObserverAccountBanTimeout;

	DWORD									m_dwBanChatSearchPlayerPID;
	TChatBanPlayerInfo						m_kBanChatSearchPlayer;
	std::map<DWORD, TChatBanPlayerInfo>		m_map_BanChatList;
	std::map<std::string, DWORD>			m_map_BanChatPIDMap;

	DWORD									m_dwBanAccountSearchResultAID;
	TAccountBanInfo							m_kBanAccountSearchResult;
	std::map<DWORD, TAccountBanInfo>		m_map_BanAccountList;
	std::map<std::string, DWORD>			m_map_BanAccountMapByName;
	std::map<std::string, DWORD>			m_map_BanAccountMapByPlayerName;
	std::map<DWORD, DWORD>					m_map_BanAccountMapByPID;

	BYTE									m_bBanLogType;
	std::vector<TAdminManagerBanLogInfo>	m_vec_BanLogInfo;

	std::map<DWORD, TItemInfo>				m_map_ItemInfo;

	std::vector<THackInfo>				m_map_HackLog;
};
#endif
