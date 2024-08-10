#ifndef __INC_METIN_II_GAME_INPUT_PROCESSOR__
#define __INC_METIN_II_GAME_INPUT_PROCESSOR__

#include "packet_info.h"

enum
{
	INPROC_CLOSE,
	INPROC_HANDSHAKE,
	INPROC_LOGIN,
	INPROC_MAIN,
	INPROC_DEAD,
	INPROC_DB,
	INPROC_UDP,
	INPROC_P2P,
	INPROC_AUTH,
	INPROC_TEEN,
};

void LoginFailure(LPDESC d, const char* c_pszStatus, int iValue = 0, std::string&& sBanReason = "");


extern void SendShout(const char * szText, BYTE bEmpire
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	, BYTE bLocale
#endif
);

class CInputProcessor
{
public:
	CInputProcessor();
	virtual ~CInputProcessor() {};

	virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
	virtual BYTE GetType() = 0;

	void BindPacketInfo(CPacketInfo * pPacketInfo);
	void Pong(LPDESC d);
	void Handshake(LPDESC d, const char * c_pData);
	void Version(LPCHARACTER ch, const char* c_pData);

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) = 0;

	CPacketInfo * m_pPacketInfo;
	int	m_iBufferLeft;

	CPacketInfoCG 	m_packetInfoCG;
};

class CInputClose : public CInputProcessor
{
public:
	virtual BYTE	GetType() { return INPROC_CLOSE; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) { return m_iBufferLeft; }
};

class CInputHandshake : public CInputProcessor
{
public:
	CInputHandshake();
	virtual ~CInputHandshake();

	virtual BYTE	GetType() { return INPROC_HANDSHAKE; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:
	void		GuildMarkLogin(LPDESC d, const char* c_pData);

	CPacketInfo *	m_pMainPacketInfo;
};

class CInputLogin : public CInputProcessor
{
public:
	virtual BYTE	GetType() { return INPROC_LOGIN; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:
	void		Login(LPDESC d, const char * data);
	void		LoginByKey(LPDESC d, const char * data);

	void		CharacterSelect(LPDESC d, const char * data);
	void		CharacterCreate(LPDESC d, const char * data);
	void		CharacterDelete(LPDESC d, const char * data);
	void		Entergame(LPDESC d, const char * data);
	void		Empire(LPDESC d, const char * c_pData);
	void		GuildMarkCRCList(LPDESC d, const char* c_pData);
	// MARK_BUG_FIX
	void		GuildMarkIDXList(LPDESC d, const char* c_pData);
	// END_OF_MARK_BUG_FIX
	void		GuildMarkUpload(LPDESC d, const char* c_pData);
	int			GuildSymbolUpload(LPDESC d, const char* c_pData, size_t uiBytes);
	void		GuildSymbolCRC(LPDESC d, const char* c_pData);
	void		ChangeName(LPDESC d, const char * data);
};

class CInputMain : public CInputProcessor
{
public:
	virtual BYTE	GetType() { return INPROC_MAIN; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:
	void		Attack(LPCHARACTER ch, const BYTE header, const char* data);

	int			Whisper(LPCHARACTER ch, const char * data, size_t uiBytes);

	int			Chat(LPCHARACTER ch, const char * data, size_t uiBytes);
	void		ItemUse(LPCHARACTER ch, const char * data);
#ifdef __ENABLE_MULTI_USE_PACKET__
	void		ItemMultiUse(LPCHARACTER ch, const char* data);
#endif
	void		ItemDrop(LPCHARACTER ch, const char * data);
	void		ItemDrop2(LPCHARACTER ch, const char * data);
	void		ItemMove(LPCHARACTER ch, const char * data);
	void		ItemPickup(LPCHARACTER ch, const char * data);
#ifdef __ENABLE_DESTROY_ITEM_PACKET__
	void		ItemDestroy(LPCHARACTER ch, const char* data);
#endif
	void		ItemToItem(LPCHARACTER ch, const char * pcData);
	void		QuickslotAdd(LPCHARACTER ch, const char * data);
	void		QuickslotDelete(LPCHARACTER ch, const char * data);
	void		QuickslotSwap(LPCHARACTER ch, const char * data);
	int			Shop(LPCHARACTER ch, const char * data, size_t uiBytes);
	void		OnClick(LPCHARACTER ch, const char * data);
	void		Exchange(LPCHARACTER ch, const char * data);
	void		Position(LPCHARACTER ch, const char * data);
	void		Move(LPCHARACTER ch, const char * data);
	int			SyncPosition(LPCHARACTER ch, const char * data, size_t uiBytes);
	void		FlyTarget(LPCHARACTER ch, const char * pcData, BYTE bHeader);
	void		UseSkill(LPCHARACTER ch, const char * pcData);

	void		ScriptAnswer(LPCHARACTER ch, const void * pvData);
	void		ScriptButton(LPCHARACTER ch, const void * pvData);
	void		ScriptSelectItem(LPCHARACTER ch, const void * pvData);

	void		QuestInputString(LPCHARACTER ch, const void * pvData);
	void		QuestConfirm(LPCHARACTER ch, const void* pvData);
	void		Target(LPCHARACTER ch, const char * pcData);
	void		Warp(LPCHARACTER ch, const char * pcData);
	void		SafeboxCheckin(LPCHARACTER ch, const char * c_pData);
	void		SafeboxCheckout(LPCHARACTER ch, const char * c_pData, bool bMall);
	void		SafeboxItemMove(LPCHARACTER ch, const char * data);
	int			Messenger(LPCHARACTER ch, const char* c_pData, size_t uiBytes);

	void 		PartyInvite(LPCHARACTER ch, const char * c_pData);
	void 		PartyInviteAnswer(LPCHARACTER ch, const char * c_pData);
	void		PartyRemove(LPCHARACTER ch, const char * c_pData);
	void		PartySetState(LPCHARACTER ch, const char * c_pData);
	void		PartyUseSkill(LPCHARACTER ch, const char * c_pData);
	void		PartyParameter(LPCHARACTER ch, const char * c_pData);

#ifdef __INGAME_WIKI__
	void		RecvWikiPacket(LPCHARACTER ch, const char* c_pData);
#endif

	int			Guild(LPCHARACTER ch, const char * data, size_t uiBytes);
	void		AnswerMakeGuild(LPCHARACTER ch, const char* c_pData);

	void		Fishing(LPCHARACTER ch, const char* c_pData);
	void		ItemGive(LPCHARACTER ch, const char* c_pData);
	void		Hack(LPCHARACTER ch, const char * c_pData);
	int			MyShop(LPCHARACTER ch, const char * c_pData, size_t uiBytes);

	void		Refine(LPCHARACTER ch, const char* c_pData);

#ifdef __ENABLE_TREASURE_BOX_LOOT__
	int			RequestTreasureBoxLoot(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif

#ifdef __ENABLE_TARGET_MONSTER_LOOT__
	void		LoadTargetInfo(LPCHARACTER pkCharacter, TPacketCGTargetLoad* pkPacket);
#endif

#ifdef __ENABLE_DELETE_SINGLE_STONE__
	int			RequestDeleteSocket(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif

#ifdef __ENABLE_SWITCHBOT__
	int			Switchbot(LPCHARACTER ch, const char* data, size_t uiBytes);
#endif

#ifdef __VOICE_CHAT_ENABLE__
	int			ProcessVoicePacket(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif

#ifdef __ENABLE_NEW_FILTERS__
	void		ItemsPickup(LPCHARACTER ch, const char* data);
#endif

#ifdef __ADMIN_MANAGER__
	int			AdminManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
	int			MarbleManager(LPCHARACTER ch, const char* c_pData, size_t uiBytes);
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	void 		RefineElement(LPCHARACTER ch, const char* c_pData);
#endif

#ifdef __ENABLE_LUCKY_BOX__
	int 		LuckyBox(LPCHARACTER ch, const char* data, size_t uiBytes);
#endif

#ifdef __SHOP_SEARCH__
	void		ShopSearchByName(LPCHARACTER ch, const char* data);
	int			ShopSearchByOptions(LPCHARACTER ch, const char* data, size_t uiBytes);
	void		ShopSearchBuy(LPCHARACTER ch, const char* data);
	void		ShopSearchOwnerMessage(LPCHARACTER ch, const char* data);
	void		ShopSearchRequestSoldInfo(LPCHARACTER ch, const char* data);
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	void		WhisperRequestLanguage(LPCHARACTER ch, const char* data);
#endif
};

class CInputDead : public CInputMain
{
public:
	virtual BYTE	GetType() { return INPROC_DEAD; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);
};

class CInputDB : public CInputProcessor
{
public:
	virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
	virtual BYTE GetType() { return INPROC_DB; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:
	void		MapLocations(const char * c_pData);
	void		LoginSuccess(DWORD dwHandle, const char *data);
	void		PlayerCreateFailure(LPDESC d, BYTE bType);
	void		PlayerDeleteSuccess(LPDESC d, const char * data);
	void		PlayerDeleteFail(LPDESC d);
	void		PlayerLoad(LPDESC d, const char* data);
	void		PlayerCreateSuccess(LPDESC d, const char * data);
	void		Boot(const char* data);
	void		QuestLoad(LPDESC d, const char * c_pData);
	void		SafeboxLoad(LPDESC d, const char * c_pData);
	void		SafeboxChangeSize(LPDESC d, const char * c_pData);
	void		SafeboxWrongPassword(LPDESC d);
	void		SafeboxChangePasswordAnswer(LPDESC d, const char* c_pData);
	void		MallLoad(LPDESC d, const char * c_pData);
	void		EmpireSelect(LPDESC d, const char * c_pData);
	void		P2P(const char * c_pData);
	void		ItemLoad(LPDESC d, const char * c_pData);
	void		AffectLoad(LPDESC d, const char * c_pData);
	void		GuildLoad(const char * c_pData);
	void		GuildSkillUpdate(const char* c_pData);
	void		GuildSkillRecharge();
	void		GuildExpUpdate(const char* c_pData);
	void		GuildAddMember(const char* c_pData);
	void		GuildRemoveMember(const char* c_pData);
	void		GuildChangeGrade(const char* c_pData);
	void		GuildChangeMemberData(const char* c_pData);
	void		GuildDisband(const char* c_pData);
	void		GuildLadder(const char* c_pData);
	void		GuildWar(const char* c_pData);
	void		GuildWarScore(const char* c_pData);
	void		GuildSkillUsableChange(const char* c_pData);
	void		GuildMoneyChange(const char* c_pData);
	void		GuildWithdrawMoney(const char* c_pData);
	void		GuildWarReserveAdd(TGuildWarReserve * p);
	void		GuildWarReserveUpdate(TGuildWarReserve * p);
	void		GuildWarReserveDelete(DWORD dwID);
	void		GuildWarBet(TPacketGDGuildWarBet * p);
	void		GuildChangeMaster(TPacketChangeGuildMaster* p);

	void		LoginAlready(LPDESC d, const char * c_pData);

	void		PartyCreate(const char* c_pData);
	void		PartyDelete(const char* c_pData);
	void		PartyAdd(const char* c_pData);
	void		PartyRemove(const char* c_pData);
	void		PartyStateChange(const char* c_pData);
	void		PartySetMemberLevel(const char* c_pData);

	void		Time(const char * c_pData);

	void		ReloadProto(const char * c_pData);
	void		ChangeName(LPDESC d, const char * data);

	void		AuthLogin(LPDESC d, const char * c_pData);
	void		AuthLoginOpenID(LPDESC d, const char * c_pData);
	void		ItemAward(const char * c_pData);

	void		ChangeEmpirePriv(const char* c_pData);
	void		ChangeGuildPriv(const char* c_pData);
	void		ChangeCharacterPriv(const char* c_pData);

	void		MoneyLog(const char* c_pData);

	void		SetEventFlag(const char* c_pData);

	void		BillingRepair(const char * c_pData);
	void		BillingExpire(const char * c_pData);
	void		BillingLogin(const char * c_pData);
	void		BillingCheck(const char * c_pData);
	void		VCard(const char * c_pData);

	void		CreateObject(const char * c_pData);
	void		DeleteObject(const char * c_pData);
	void		UpdateLand(const char * c_pData);

	void		Notice(const char * c_pData);

	void		MarriageAdd(TPacketMarriageAdd * p);
	void		MarriageUpdate(TPacketMarriageUpdate * p);
	void		MarriageRemove(TPacketMarriageRemove * p);

	void		WeddingRequest(TPacketWeddingRequest* p);
	void		WeddingReady(TPacketWeddingReady* p);
	void		WeddingStart(TPacketWeddingStart* p);
	void		WeddingEnd(TPacketWeddingEnd* p);

	void		AddBlockCountryIp(TPacketBlockCountryIp * data);
	void		BlockException(TPacketBlockException * data);

	// MYSHOP_PRICE_LIST


	void		MyshopPricelistRes( LPDESC d, const TPacketMyshopPricelistHeader* p );
	// END_OF_MYSHOP_PRICE_LIST
	//
	//RELOAD_ADMIN
	void ReloadAdmin( const char * c_pData );
	//END_RELOAD_ADMIN

	void		DetailLog(const TPacketNeedLoginLogInfo* info);

	void		ItemAwardInformer(TPacketItemAwardInfromer* data);

	void		RespondChannelStatus(LPDESC desc, const char* pcData);

#ifdef __ENABLE_CHANGE_CHANNEL__
	void		ChannelSwitch(LPDESC pkDesc, TChannelSwitchInfo* pkData);
#endif

#ifdef __ADMIN_MANAGER__
	void		AdminManagerSearchPlayer(const char* c_pData);
#endif

#ifdef __ENABLE_OFFLINE_SHOP__
	void		RespondOfflineShopId(const char* data);
#endif

	#ifdef __DUNGEON_INFO_ENABLE__
		void		DIHighscoreRecord(const char * c_pData);
	#endif

	#ifdef __HARDWARE_BAN__
	void		UpdateHardwareBan(const char * pcData);
	#endif

protected:
	DWORD		m_dwHandle;
};

class CInputUDP : public CInputProcessor
{
public:
	CInputUDP();
	virtual bool Process(LPDESC d, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);

	virtual BYTE GetType() { return INPROC_UDP; }
	void		SetSockAddr(struct sockaddr_in & rSockAddr) { m_SockAddr = rSockAddr; };

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

protected:
	void		Handshake(LPDESC lpDesc, const char * c_pData);
	void		StateChecker(const char * c_pData);

protected:
	struct sockaddr_in	m_SockAddr;
	CPacketInfoUDP 		m_packetInfoUDP;
};

class CInputP2P : public CInputProcessor
{
public:
	CInputP2P();
	virtual BYTE	GetType() { return INPROC_P2P; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);

public:
	void		Setup(LPDESC d, const char * c_pData);
	void		Login(LPDESC d, const char * c_pData);
	void		Logout(LPDESC d, const char * c_pData);
	int			Relay(LPDESC d, const char * c_pData, size_t uiBytes);
	int			Notice(LPDESC d, const char * c_pData, size_t uiBytes);
	int			BigNotice(LPDESC d, const char* c_pData, size_t uiBytes);
	int			Guild(LPDESC d, const char* c_pData, size_t uiBytes);
	void		Shout(const char * c_pData);
	void		Disconnect(const char * c_pData);
	void		MessengerAdd(const char * c_pData);
	void		MessengerRemove(const char * c_pData);
	void		MessengerMobile(const char * c_pData);
	void		FindPosition(LPDESC d, const char* c_pData);
	void		WarpCharacter(const char* c_pData);
	void		GuildWarZoneMapIndex(const char* c_pData);
	void		Transfer(const char * c_pData);
	void		XmasWarpSanta(const char * c_pData);
	void		XmasWarpSantaReply(const char * c_pData);
	void		LoginPing(LPDESC d, const char * c_pData);
	void		BlockChat(const char * c_pData);
	void		PCBangUpdate(const char* c_pData);
	void		IamAwake(LPDESC d, const char * c_pData);

#ifdef __ENABLE_SWITCHBOT__
	void		Switchbot(LPDESC d, const char* c_pData);
#endif

#ifdef __GAYA_SHOP_SYSTEM__
	void		GayaInputPacket(LPDESC inpt_d, const char* c_pData);
#endif

#ifdef __EVENT_MANAGER_ENABLE__
	void		EventManagerPacket(const char * c_pData);
#endif

#ifdef __TECHNICAL_MAINTENANCE_ENABLE__
	void		TechnicalMaintenancePacket(LPDESC d, const char * c_pData);
#endif

#ifdef __ADMIN_MANAGER__
	int			AdminManager(const char* c_pData);
#endif

#ifdef __CYCLIC_MONSTER_RESPAWNER__
	void		ResetSpots(const char* c_pData);
#endif

#ifdef __ENABLE_RIGHTS_CONTROLLER__
	void		UpdateRights(const char* c_pData);
#endif

#ifdef __ENABLE_FIX_CHANGE_NAME__
	void		UpdateGuildPlayerName(const char* c_pData);
#endif

#ifdef __ENABLE_TEAMLER_STATUS__
	int			PlayerPacket(const char* c_pData, size_t uiBytes);
	void		TeamlerStatus(const char* c_pData);
#endif

#ifdef __ENABLE_FIND_LOCATION__
	void		GetPlayerLocation(const char* c_pData);
#endif

	void		ExecReloadCommand(const char* c_pData);

#ifdef __CROSS_CHANNEL_DUNGEON_WARP__
	void		CreateDungeonInstance(LPDESC d, const char* c_pData);
#ifdef __DUNGEON_RETURN_ENABLE__
	void		RejoinDungeon(LPDESC d, const char * c_pData);
	void		CanRejoinDungeon(LPDESC d, const char * c_pData);
#endif
#endif

#ifdef __ENABLE_ADMIN_BAN_PANEL__
	void		DisconnectPlayer(const char* c_pData);
#endif

protected:
	CPacketInfoGG 	m_packetInfoGG;
};

class CInputAuth : public CInputProcessor
{
public:
	CInputAuth();
	virtual BYTE GetType() { return INPROC_AUTH; }

protected:
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData);
	int auth_OpenID(const char *authKey, const char *ipAddr, char *rID);

public:
	void		Login(LPDESC d, const char * c_pData);
	void		LoginOpenID(LPDESC d, const char * c_pData);
	void		PasspodAnswer(LPDESC d, const char * c_pData );

};

class CInputTeen : public CInputProcessor
{
public :
	virtual BYTE GetType() { return INPROC_TEEN; }

	void SetStep(int step);

protected :
	virtual bool Process(LPDESC lpDesc, const void * c_pvOrig, int iBytes, int & r_iBytesProceed);
	virtual int	Analyze(LPDESC d, BYTE bHeader, const char * c_pData) { return 0; };

private:
	int	m_step;

	bool ProcessHandshake(LPDESC lpDesc, const void * c_pvOrig, size_t uiBytes, int & r_iBytesProceed);
	bool ProcessMain(LPDESC lpDesc, const void * c_pvOrig, size_t uiBytes, int & r_iBytesProceed);
};

#endif /* __INC_METIN_II_GAME_INPUT_PROCESSOR__ */

