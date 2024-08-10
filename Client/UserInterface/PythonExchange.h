#pragma once

#include "Packet.h"


class CPythonExchange : public CSingleton<CPythonExchange>
{
public:
	enum
	{
		EXCHANGE_ITEM_MAX_NUM = 24,
	};

	typedef struct trade
	{
		char					name[CHARACTER_NAME_MAX_LEN + 1];
#ifdef ENABLE_RENEWAL_EXCHANGE
		DWORD					vid;
		DWORD					race;
		DWORD					level;
#endif

		DWORD					item_vnum[EXCHANGE_ITEM_MAX_NUM];
		CountType				item_count[EXCHANGE_ITEM_MAX_NUM];
		DWORD					item_metin[EXCHANGE_ITEM_MAX_NUM][ITEM_SOCKET_SLOT_MAX_NUM];
		TPlayerItemAttribute	item_attr[EXCHANGE_ITEM_MAX_NUM][ITEM_ATTRIBUTE_SLOT_MAX_NUM];

		BYTE					accept;
#ifdef EXTANDED_GOLD_AMOUNT
		int64_t					elk;
#else
		DWORD					elk;
#endif
#ifdef TRANSMUTATION_SYSTEM
		DWORD					transmutation_id[EXCHANGE_ITEM_MAX_NUM];
#endif
#ifdef ENABLE_REFINE_ELEMENT
		DWORD					dwRefineElement[EXCHANGE_ITEM_MAX_NUM];
#endif
	} TExchangeData;

public:
	CPythonExchange();
	virtual ~CPythonExchange();

	void			Clear();

	void			Start();
	void			End();
	bool			isTrading();

	// Interface

#ifdef ENABLE_RENEWAL_EXCHANGE
	void			SetTargetVID(DWORD vid) { m_victim.vid = vid; }
	DWORD			GetTargetVID() const { return m_victim.vid; }
#endif

	void			SetSelfName(const char *name);
	void			SetTargetName(const char *name);

	char			*GetNameFromSelf();
	char			*GetNameFromTarget();

#ifdef ENABLE_RENEWAL_EXCHANGE
	void			SetSelfRace(DWORD race);
	void			SetTargetRace(DWORD race);
	DWORD			GetRaceFromSelf();
	DWORD			GetRaceFromTarget();
	void			SetSelfLevel(DWORD level);
	void			SetTargetLevel(DWORD level);
	DWORD			GetLevelFromSelf();
	DWORD			GetLevelFromTarget();
#endif

#ifdef EXTANDED_GOLD_AMOUNT
	void			SetElkToTarget(int64_t elk);
	void			SetElkToSelf(int64_t elk);

	int64_t			GetElkFromTarget();
	int64_t			GetElkFromSelf();
#else
	void			SetElkToTarget(DWORD elk);
	void			SetElkToSelf(DWORD elk);

	DWORD			GetElkFromTarget();
	DWORD			GetElkFromSelf();
#endif

	void			SetItemToTarget(DWORD pos, DWORD vnum, CountType count);
	void			SetItemToSelf(DWORD pos, DWORD vnum, CountType count);

	void			SetItemMetinSocketToTarget(int pos, int imetinpos, DWORD vnum);
	void			SetItemMetinSocketToSelf(int pos, int imetinpos, DWORD vnum);

	void			SetItemAttributeToTarget(int pos, int iattrpos, BYTE byType, short sValue);
	void			SetItemAttributeToSelf(int pos, int iattrpos, BYTE byType, short sValue);

	void			DelItemOfTarget(BYTE pos);
	void			DelItemOfSelf(BYTE pos);

	DWORD			GetItemVnumFromTarget(BYTE pos);
	DWORD			GetItemVnumFromSelf(BYTE pos);

	CountType		GetItemCountFromTarget(BYTE pos);
	CountType		GetItemCountFromSelf(BYTE pos);

	DWORD			GetItemMetinSocketFromTarget(BYTE pos, int iMetinSocketPos);
	DWORD			GetItemMetinSocketFromSelf(BYTE pos, int iMetinSocketPos);

	void			GetItemAttributeFromTarget(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue);
	void			GetItemAttributeFromSelf(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue);

#ifdef ENABLE_REFINE_ELEMENT
	void 			SetItemRefineElementToSelf(BYTE bPos, DWORD dwRefineElement);
	void 			SetItemRefineElementToTarget(BYTE bPos, DWORD dwRefineElement);
	DWORD 			GetItemRefineElementFromSelf(BYTE bPos);
	DWORD 			GetItemRefineElementFromTarget(BYTE bPos);
#endif

	void			SetAcceptToTarget(BYTE Accept);
	void			SetAcceptToSelf(BYTE Accept);

	bool			GetAcceptFromTarget();
	bool			GetAcceptFromSelf();

	bool			GetElkMode();
	void			SetElkMode(bool value);

#ifdef TRANSMUTATION_SYSTEM
	void			SetItemTransmutateToTarget(int pos, DWORD vnum);
	void			SetItemTransmutateToSelf(int pos, DWORD vnum);

	DWORD			GetItemTransmutateFromTarget(BYTE pos);
	DWORD			GetItemTransmutateFromSelf(BYTE pos);
#endif

protected:
	bool				m_isTrading;

	bool				m_elk_mode;
	TExchangeData		m_self;
	TExchangeData		m_victim;
};
