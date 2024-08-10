#include "stdafx.h"
#include "PythonExchange.h"

void CPythonExchange::SetSelfName(const char *name)
{
	strncpy(m_self.name, name, CHARACTER_NAME_MAX_LEN);
}

void CPythonExchange::SetTargetName(const char *name)
{
	strncpy(m_victim.name, name, CHARACTER_NAME_MAX_LEN);
}

char * CPythonExchange::GetNameFromSelf()
{
	return m_self.name;
}

char * CPythonExchange::GetNameFromTarget()
{
	return m_victim.name;
}

#ifdef ENABLE_RENEWAL_EXCHANGE
void CPythonExchange::SetSelfRace(DWORD race)
{
	m_self.race = race;
}

void CPythonExchange::SetTargetRace(DWORD race)
{
	m_victim.race = race;
}

DWORD CPythonExchange::GetRaceFromSelf()
{
	return m_self.race;
}

DWORD CPythonExchange::GetRaceFromTarget()
{
	return m_victim.race;
}

void CPythonExchange::SetSelfLevel(DWORD level)
{
	m_self.level = level;
}

void CPythonExchange::SetTargetLevel(DWORD level)
{
	m_victim.level = level;
}

DWORD CPythonExchange::GetLevelFromSelf()
{
	return m_self.level;
}

DWORD CPythonExchange::GetLevelFromTarget()
{
	return m_victim.level;
}
#endif

#ifdef EXTANDED_GOLD_AMOUNT
	void CPythonExchange::SetElkToTarget(int64_t elk)
#else
	void CPythonExchange::SetElkToTarget(DWORD	elk)
#endif
{
	m_victim.elk = elk;
}

#ifdef EXTANDED_GOLD_AMOUNT
	void CPythonExchange::SetElkToSelf(int64_t elk)
#else
	void CPythonExchange::SetElkToSelf(DWORD elk)
#endif
{
	m_self.elk = elk;
}
#ifdef EXTANDED_GOLD_AMOUNT
	int64_t CPythonExchange::GetElkFromTarget()
#else
	DWORD CPythonExchange::GetElkFromTarget()
#endif
{
	return m_victim.elk;
}

#ifdef EXTANDED_GOLD_AMOUNT
	int64_t CPythonExchange::GetElkFromSelf()
#else
	DWORD CPythonExchange::GetElkFromSelf()
#endif
{
	return m_self.elk;
}

void CPythonExchange::SetItemToTarget(DWORD pos, DWORD vnum, CountType count)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.item_vnum[pos] = vnum;
	m_victim.item_count[pos] = count;
}

void CPythonExchange::SetItemToSelf(DWORD pos, DWORD vnum, CountType count)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.item_vnum[pos] = vnum;
	m_self.item_count[pos] = count;
}

void CPythonExchange::SetItemMetinSocketToTarget(int pos, int imetinpos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.item_metin[pos][imetinpos] = vnum;
}

void CPythonExchange::SetItemMetinSocketToSelf(int pos, int imetinpos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.item_metin[pos][imetinpos] = vnum;
}

void CPythonExchange::SetItemAttributeToTarget(int pos, int iattrpos, BYTE byType, short sValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.item_attr[pos][iattrpos].bType = byType;
	m_victim.item_attr[pos][iattrpos].sValue = sValue;
}

void CPythonExchange::SetItemAttributeToSelf(int pos, int iattrpos, BYTE byType, short sValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.item_attr[pos][iattrpos].bType = byType;
	m_self.item_attr[pos][iattrpos].sValue = sValue;
}

void CPythonExchange::DelItemOfTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.item_vnum[pos] = 0;
	m_victim.item_count[pos] = 0;
}

void CPythonExchange::DelItemOfSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.item_vnum[pos] = 0;
	m_self.item_count[pos] = 0;
}

DWORD CPythonExchange::GetItemVnumFromTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_victim.item_vnum[pos];
}

DWORD CPythonExchange::GetItemVnumFromSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_self.item_vnum[pos];
}

CountType CPythonExchange::GetItemCountFromTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_victim.item_count[pos];
}

CountType CPythonExchange::GetItemCountFromSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_self.item_count[pos];
}

DWORD CPythonExchange::GetItemMetinSocketFromTarget(BYTE pos, int iMetinSocketPos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_victim.item_metin[pos][iMetinSocketPos];
}

DWORD CPythonExchange::GetItemMetinSocketFromSelf(BYTE pos, int iMetinSocketPos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_self.item_metin[pos][iMetinSocketPos];
}

void CPythonExchange::GetItemAttributeFromTarget(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	*pbyType = m_victim.item_attr[pos][iAttrPos].bType;
	*psValue = m_victim.item_attr[pos][iAttrPos].sValue;
}

void CPythonExchange::GetItemAttributeFromSelf(BYTE pos, int iAttrPos, BYTE * pbyType, short * psValue)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	*pbyType = m_self.item_attr[pos][iAttrPos].bType;
	*psValue = m_self.item_attr[pos][iAttrPos].sValue;
}

#ifdef ENABLE_REFINE_ELEMENT
void CPythonExchange::SetItemRefineElementToSelf(BYTE bPos, DWORD dwRefineElement)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.dwRefineElement[bPos] = dwRefineElement;
}

void CPythonExchange::SetItemRefineElementToTarget(BYTE bPos, DWORD dwRefineElement)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.dwRefineElement[bPos] = dwRefineElement;
}

DWORD CPythonExchange::GetItemRefineElementFromSelf(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_self.dwRefineElement[bPos];
}

DWORD CPythonExchange::GetItemRefineElementFromTarget(BYTE bPos)
{
	if (bPos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_victim.dwRefineElement[bPos];
}
#endif

void CPythonExchange::SetAcceptToTarget(BYTE Accept)
{
	m_victim.accept = Accept ? true : false;
}

void CPythonExchange::SetAcceptToSelf(BYTE Accept)
{
	m_self.accept = Accept ? true : false;
}

bool CPythonExchange::GetAcceptFromTarget()
{
	return m_victim.accept ? true : false;
}

bool CPythonExchange::GetAcceptFromSelf()
{
	return m_self.accept ? true : false;
}

bool CPythonExchange::GetElkMode()
{
	return m_elk_mode;
}

void CPythonExchange::SetElkMode(bool value)
{
	m_elk_mode = value;
}

void CPythonExchange::Start()
{
	m_isTrading = true;
}

void CPythonExchange::End()
{
	m_isTrading = false;
}

bool CPythonExchange::isTrading()
{
	return m_isTrading;
}

#ifdef TRANSMUTATION_SYSTEM
void CPythonExchange::SetItemTransmutateToTarget(int pos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_victim.transmutation_id[pos] = vnum;
}

void CPythonExchange::SetItemTransmutateToSelf(int pos, DWORD vnum)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return;
	}

	m_self.transmutation_id[pos] = vnum;
}

DWORD CPythonExchange::GetItemTransmutateFromTarget(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_victim.transmutation_id[pos];
}

DWORD CPythonExchange::GetItemTransmutateFromSelf(BYTE pos)
{
	if (pos >= EXCHANGE_ITEM_MAX_NUM)
	{
		return 0;
	}

	return m_self.transmutation_id[pos];
}
#endif

void CPythonExchange::Clear()
{
	memset(&m_self, 0, sizeof(m_self));
	memset(&m_victim, 0, sizeof(m_victim));
	/*
		m_self.item_vnum[0] = 30;
		m_victim.item_vnum[0] = 30;
		m_victim.item_vnum[1] = 40;
		m_victim.item_vnum[2] = 50;
	*/
}

CPythonExchange::CPythonExchange()
{
	Clear();
	m_isTrading = false;
	m_elk_mode = false;
}
CPythonExchange::~CPythonExchange()
{
}