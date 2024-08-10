#ifndef __INC_METIN_II_GAME_ITEM_H__
#define __INC_METIN_II_GAME_ITEM_H__

#include "entity.h"

class CItem : public CEntity
{
protected:
	// override methods from ENTITY class
	virtual void	EncodeInsertPacket(LPENTITY entity);
	virtual void	EncodeRemovePacket(LPENTITY entity);

public:
	CItem(DWORD dwVnum);
	virtual ~CItem();

	int			GetLevelLimit();

	bool		CheckItemUseLevel(int nLevel);

	bool		IsPCBangItem();

	long		FindApplyValue(BYTE bApplyType);

	bool		IsStackable() const;

	bool		CanStackWith(LPITEM otherItem) const;

	void		Initialize();
	void		Destroy();

	void		Save();

	void		SetWindow(BYTE b)	{ m_bWindow = b; }
	BYTE		GetWindow()		{ return m_bWindow; }

	void		SetID(DWORD id)		{ m_dwID = id;	}
	DWORD		GetID()			{ return m_dwID; }

	void			SetProto(const TItemTable * table);
	TItemTable const *	GetProto()	{ return m_pProto; }

#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t		GetGold();
	int64_t		GetShopBuyPrice();
#else
	int		GetGold();
	int		GetShopBuyPrice();
#endif

#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
	const char* GetName() const;
#else
	const char* GetName() { return m_pProto ? m_pProto->szLocaleName : NULL; }
#endif
	const char *	GetBaseName()		{ return m_pProto ? m_pProto->szName : NULL; }
	BYTE		GetSize()		{ return m_pProto ? m_pProto->bSize : 0;	}

	void		SetFlag(long flag)	{ m_lFlag = flag;	}
	long		GetFlag() const { return m_lFlag; }

	void		AddFlag(long bit);
	void		RemoveFlag(long bit);

	DWORD		GetWearFlag()		{ return m_pProto ? m_pProto->dwWearFlags : 0; }
	DWORD		GetAntiFlag() const		{ return m_pProto ? m_pProto->dwAntiFlags : 0; }
	DWORD		GetImmuneFlag()		{ return m_pProto ? m_pProto->dwImmuneFlag : 0; }

	inline bool IsAntiFlag(uint32_t bit) const
	{
		return (GetAntiFlag() & bit);
	}

	inline bool IsFlag(uint32_t bit) const
	{
		return (GetFlag() & bit);
	}

	void		SetVID(DWORD vid)	{ m_dwVID = vid;	}
	DWORD		GetVID()		{ return m_dwVID;	}

#ifdef __ENABLE_REFINE_ELEMENT__
	void	SetRefineElement(DWORD);
	DWORD	GetRefineElement() const { return m_dwRefineElement; }
	BYTE 	GetRefineElementType() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement / 100000000)); }
	BYTE 	GetRefineElementPlus() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement / 10000000 % 10)); }
	BYTE 	GetRefineElementBonusValue() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement / 100000 % 100)); }
	BYTE 	GetRefineElementAttackValue() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement / 1000 % 100)); }
	BYTE 	GetRefineElementLastIncBonus() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement / 100 % 10)); }
	BYTE 	GetRefineElementLastIncAttack() { return (!m_dwRefineElement) ? 0 : ((BYTE)(m_dwRefineElement % 100)); }
#endif

	bool			SetCount(CountType count);
	CountType		GetCount();

	//
	DWORD		GetVnum() const		{ return m_dwMaskVnum ? m_dwMaskVnum : m_dwVnum;	}
	DWORD		GetOriginalVnum() const		{ return m_dwVnum;	}
	BYTE		GetType() const		{ return m_pProto ? m_pProto->bType : 0;	}
	BYTE		GetSubType() const	{ return m_pProto ? m_pProto->bSubType : 0;	}
	BYTE		GetLimitType(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].bType : 0;	}
	long		GetLimitValue(DWORD idx) const { return m_pProto ? m_pProto->aLimits[idx].lValue : 0;	}

	long		GetValue(DWORD idx);

	void		SetCell(LPCHARACTER ch, WORD pos)	{ m_pOwner = ch, m_wCell = pos;	}
	WORD		GetCell()				{ return m_wCell;	}

	LPITEM		RemoveFromCharacter(bool bSwapping = false);
	bool		AddToCharacter(LPCHARACTER ch, TItemPos Cell);
	LPCHARACTER	GetOwner()		{ return m_pOwner; }

	LPITEM		RemoveFromGround();
	bool		AddToGround(long lMapIndex, const PIXEL_POSITION & pos, bool skipOwnerCheck = false);

	int			FindEquipCell(LPCHARACTER ch, int bCandidateCell = -1);
	bool		IsEquipped() const		{ return m_bEquipped;	}
	bool		EquipTo(LPCHARACTER ch, BYTE bWearCell);
	bool		IsEquipable() const;

	bool		CanUsedBy(LPCHARACTER ch);

	bool		DistanceValid(LPCHARACTER ch);

	void		UpdatePacket();
	void		UsePacketEncode(LPCHARACTER ch, LPCHARACTER victim, struct packet_item_use * packet);

	void		SetExchanging(bool isOn = true);
	bool		IsExchanging() const { return m_bExchanging; }

	bool		IsTwohanded();

	bool		IsPolymorphItem();

	void		ModifyPoints(bool bAdd);

	bool		CreateSocket(BYTE bSlot, BYTE bGold);
	const long *	GetSockets()	{ return &m_alSockets[0];	}
	long		GetSocket(int i) const { return m_alSockets[i]; }

	void		SetSockets(const long * al);
	void 		SetSockets(const std::array<int32_t, ITEM_SOCKET_MAX_NUM>& sockets);
	void		SetSocket(int i, long v, bool bLog = true);

	int		GetSocketCount();
	bool		AddSocket();

	const TPlayerItemAttribute* GetAttributes()		{ return m_aAttr;	}
	const TPlayerItemAttribute& GetAttribute(int i)	{ return m_aAttr[i];	}

	BYTE		GetAttributeType(int i)	{ return m_aAttr[i].bType;	}
	short		GetAttributeValue(int i) { return m_aAttr[i].sValue;	}

	void		SetAttributes(const TPlayerItemAttribute* c_pAttribute);

	int		FindAttribute(BYTE bType);
	bool		RemoveAttributeAt(int index);
	bool		RemoveAttributeType(BYTE bType);

	bool		HasAttr(BYTE bApply);
	bool		HasRareAttr(BYTE bApply);

	void		SetDestroyEvent(LPEVENT pkEvent);
	void		StartDestroyEvent(int iSec = 300);

	DWORD		GetRefinedVnum()	{ return m_pProto ? m_pProto->dwRefinedVnum : 0; }
	DWORD		GetRefineFromVnum();
	int		GetRefineLevel();

	void		SetSkipSave(bool b)	{ m_bSkipSave = b; }
	bool		GetSkipSave()		{ return m_bSkipSave; }

	bool		IsOwnership(LPCHARACTER ch);
	void		SetOwnership(LPCHARACTER ch, int iSec = 10);
	void		SetOwnershipEvent(LPEVENT pkEvent);

	uint32_t	GetOwnerPid() { return m_dwOwnershipPID; }
	DWORD		GetLastOwnerPID()	{ return m_dwLastOwnerPID; }
#ifdef __ENABLE_HIGHLIGHT_NEW_ITEM__
	void		SetLastOwnerPID(DWORD pid) { m_dwLastOwnerPID = pid; }
#endif

	int		GetAttributeSetIndex();
	void		AlterToMagicItem();
	void		AlterToSocketItem(int iSocketCount);

	WORD		GetRefineSet()		{ return m_pProto ? m_pProto->wRefineSet : 0;	}

	void		StartUniqueExpireEvent();
	void		SetUniqueExpireEvent(LPEVENT pkEvent);

	void		StartTimerBasedOnWearExpireEvent();
	void		SetTimerBasedOnWearExpireEvent(LPEVENT pkEvent);

	void		OnRealTimeFirstUse();
	void		StartRealTimeExpireEvent();
	bool		IsRealTimeItem();

	void		StopUniqueExpireEvent();
	void		StopTimerBasedOnWearExpireEvent();
	void		StopAccessorySocketExpireEvent();


	int			GetDuration();

	int		GetAttributeCount();
	void		ClearAttribute();
	void		ChangeAttribute(const int* aiChangeProb = NULL);
	void		AddAttribute();
	void		AddAttribute(BYTE bType, short sValue);

	void		ApplyAddon(int iAddonType);

	int		GetSpecialGroup() const;
	bool	IsSameSpecialGroup(const LPITEM item) const;

	// ACCESSORY_REFINE

	bool		IsAccessoryForSocket();

	int		GetAccessorySocketGrade();
	int		GetAccessorySocketMaxGrade();
	int		GetAccessorySocketDownGradeTime();

	void		SetAccessorySocketGrade(int iGrade);
	void		SetAccessorySocketMaxGrade(int iMaxGrade);
	void		SetAccessorySocketDownGradeTime(DWORD time, int iGrade = -1, bool NoRefresh = false);

#ifdef __ENABLE_ORE_REFACTOR__
	void		SetAccessorySocketItem(DWORD iVnum = 0, bool bRemoved = false, bool bClearingItem = false);
	void		RemoveAccessorySocketItem();
	bool		IsAccessoryExpirable();
	bool		AccessoryIsPermament(DWORD iVnum);
	int			GetAccessoryBonusValue(int itApply);
	std::vector<std::pair<DWORD, DWORD>>& GetAccessoryItems(bool bSkipNormal = false, bool bSkipPermament = false);
#endif

	void		AccessorySocketDegrade();


	void		StartAccessorySocketExpireEvent();
	void		SetAccessorySocketExpireEvent(LPEVENT pkEvent);

	bool		CanPutInto(LPITEM item);
	// END_OF_ACCESSORY_REFINE

	void		CopyAttributeTo(LPITEM pItem);
	void		CopySocketTo(LPITEM pItem);

	int			GetRareAttrCount();
	bool		AddRareAttribute();
	bool		ChangeRareAttribute();

#ifdef __BELT_ATTR__
	int			GetBeltAttrCount();
	bool		HasBeltAttr(BYTE bApply);
	bool		AddBeltAttribute();
	bool		ChangeBeltAttribute();
	void		ClearBeltAttr();
#endif

	void		AttrLog();

	void		Lock(bool f) { m_isLocked = f; }
	bool		isLocked() const { return m_isLocked; }

private:
	void		SetAttribute(int i, BYTE bType, short sValue);

public:
	void		SetForceAttribute(int i, BYTE bType, short sValue);

protected:
	bool		EquipEx(bool is_equip);
	bool		Unequip(bool bSwapping = false);

	void		AddAttr(BYTE bApply, BYTE bLevel);
	void		PutAttribute(const int * aiAttrPercentTable);
	void		PutAttributeWithLevel(BYTE bLevel);

public:
	void		AddRareAttribute2(const int * aiAttrPercentTable = NULL);
protected:
	void		AddRareAttr(BYTE bApply, BYTE bLevel);
	void		PutRareAttribute(const int * aiAttrPercentTable);
	void		PutRareAttributeWithLevel(BYTE bLevel);

protected:
	friend class CInputDB;
	bool		OnAfterCreatedItem();

public:
	bool		IsRideItem();
	bool		IsRamadanRing();

	void		ClearMountAttributeAndAffect();
	bool		IsNewMountItem();

	void		SetMaskVnum(DWORD vnum)	{	m_dwMaskVnum = vnum; }
	DWORD		GetMaskVnum()			{	return m_dwMaskVnum; }
	bool		IsMaskedItem()	{	return m_dwMaskVnum != 0;	}


	bool		IsDragonSoul();
	int		GiveMoreTime_Per(float fPercent);
	int		GiveMoreTime_Fix(DWORD dwTime);

private:
	TItemTable const * m_pProto;

	DWORD		m_dwVnum;
	LPCHARACTER	m_pOwner;

	BYTE		m_bWindow;
	DWORD		m_dwID;
	bool		m_bEquipped;
	DWORD		m_dwVID;		// VID
	WORD		m_wCell;
	CountType	m_dwCount;

	long		m_lFlag;
	DWORD		m_dwLastOwnerPID;

	bool		m_bExchanging;

	long		m_alSockets[ITEM_SOCKET_MAX_NUM];
	TPlayerItemAttribute	m_aAttr[ITEM_ATTRIBUTE_MAX_NUM];

	LPEVENT		m_pkDestroyEvent;
	LPEVENT		m_pkExpireEvent;
	LPEVENT		m_pkUniqueExpireEvent;
	LPEVENT		m_pkTimerBasedOnWearExpireEvent;
	LPEVENT		m_pkRealTimeExpireEvent;
	LPEVENT		m_pkAccessorySocketExpireEvent;
	LPEVENT		m_pkOwnershipEvent;

	DWORD		m_dwOwnershipPID;

	bool		m_bSkipSave;

	bool		m_isLocked;

	DWORD		m_dwMaskVnum;
	DWORD		m_dwSIGVnum;

#ifdef __TRANSMUTATION_SYSTEM__
	DWORD		m_dwTransmutate;
#endif

#ifdef __ENABLE_REFINE_ELEMENT__
	DWORD		m_dwRefineElement;
#endif

public:
	uint32_t GetEquipmentSetId() { return GetProto()->equipmentSetId; }

public:
	void SetSIGVnum(DWORD dwSIG)
	{
		m_dwSIGVnum = dwSIG;
	}
	DWORD	GetSIGVnum() const
	{
		return m_dwSIGVnum;
	}

public:
	const TItemApply* FindApply(uint8_t type) const;
	const TItemLimit* FindLimit(uint8_t type) const;
	void CopySockets(std::array<int32_t, ITEM_SOCKET_MAX_NUM>& sockets);
	void CopyAttributes(std::array<TPlayerItemAttribute, ITEM_ATTRIBUTE_MAX_NUM>& attributes);

#ifdef __ENABLE_RENEWAL_EXCHANGE__
public:
	std::string GetHyperlink();
#endif

#ifdef __ADMIN_MANAGER__
private:
	enum EGMOwnerTypes
	{
		GM_OWNER_UNSET,
		GM_OWNER_PLAYER,
		GM_OWNER_GM,
	};

	unsigned char m_bIsGMOwner;


public:
	bool IsGMOwner() const { return m_bIsGMOwner == GM_OWNER_GM; }
	void SetGMOwner(bool bGMOwner) { m_bIsGMOwner = bGMOwner ? GM_OWNER_GM : GM_OWNER_PLAYER; }
#endif

#ifdef __SPECIAL_STORAGE_ENABLE__
public:
	DWORD	GetVirtualWindow();
	static DWORD	GetVirtualWindow(const TItemTable* m_pProto);
#endif

#ifdef __TRANSMUTATION_SYSTEM__
public:
	void	SetTransmutate(DWORD trans_id);
	DWORD	GetTransmutate() const { return m_dwTransmutate; }
#endif

#ifdef __ENABLE_SOUL_SYSTEM__
public:
	void		StartSoulItemEvent();
	void		SetSoulItemEvent(LPEVENT pkEvent);
private:
	LPEVENT		m_pkSoulItemEvent;
#endif
};

EVENTINFO(item_event_info)
{
	LPITEM item;
	char szOwnerName[CHARACTER_NAME_MAX_LEN];

	item_event_info()
		: item( 0 )
	{
		::memset( szOwnerName, 0, CHARACTER_NAME_MAX_LEN );
	}
};

EVENTINFO(item_vid_event_info)
{
	DWORD item_vid;

	item_vid_event_info()
		: item_vid( 0 )
	{
	}
};

#endif
