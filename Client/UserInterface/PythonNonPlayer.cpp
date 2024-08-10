#include "stdafx.h"
#include "../eterPack/EterPackManager.h"
#include "pythonnonplayer.h"
#include "InstanceBase.h"
#include "PythonCharacterManager.h"

bool CPythonNonPlayer::LoadNonPlayerData(const char * c_szFileName)
{
	static DWORD s_adwMobProtoKey[4] =
	{
		4813894,
		18955,
		552631,
		6822045
	};

	CMappedFile file;
	LPCVOID pvData;

	Tracef("CPythonNonPlayer::LoadNonPlayerData: %s, sizeof(TMobTable)=%u\n", c_szFileName, sizeof(TMobTable));

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
	{
		return false;
	}

	DWORD dwFourCC, dwElements, dwDataSize;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC != MAKEFOURCC('M', 'M', 'P', 'T'))
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid Mob proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	BYTE * pbData = new BYTE[dwDataSize];
	file.Read(pbData, dwDataSize);
	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress(zObj, pbData, s_adwMobProtoKey))
	{
		delete [] pbData;
		return false;
	}

	DWORD structSize = zObj.GetSize() / dwElements;
	DWORD structDiff = zObj.GetSize() % dwElements;
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
	if (structDiff != 0 && !CPythonNonPlayer::TMobTableAll::IsValidStruct(structSize))
#else
	if ((zObj.GetSize() % sizeof(TMobTable)) != 0)
#endif
	{
		TraceError("CPythonNonPlayer::LoadNonPlayerData: invalid size %u check data format. structSize %u, structDiff %u", zObj.GetSize(), structSize, structDiff);
		return false;
	}

	for (DWORD i = 0; i < dwElements; ++i)
	{
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		CPythonNonPlayer::TMobTable t = {0};
		CPythonNonPlayer::TMobTableAll::Process(zObj.GetBuffer(), structSize, i, t);
#else
		CPythonNonPlayer::TMobTable & t = *((CPythonNonPlayer::TMobTable *) zObj.GetBuffer() + i);
#endif
		TMobTable * pTable = &t;

#ifdef INGAME_WIKI
		auto ptr = std::make_unique <TMobTable>();
		*ptr = t;
		m_NonPlayerDataMap[t.dwVnum].mobTable = std::move(ptr);
		m_NonPlayerDataMap[t.dwVnum].isSet = false;
		m_NonPlayerDataMap[t.dwVnum].isFiltered = false;
		m_NonPlayerDataMap[t.dwVnum].dropList.clear();
#else
		TMobTable* pNonPlayerData = new TMobTable;
		memcpy(pNonPlayerData, pTable, sizeof(TMobTable));
		m_NonPlayerDataMap.insert(TNonPlayerDataMap::value_type(pNonPlayerData->dwVnum, pNonPlayerData));
#endif
	}

	delete [] pbData;
	return true;
}

bool CPythonNonPlayer::GetName(DWORD dwVnum, const char ** c_pszName)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
	{
		return false;
	}

	*c_pszName = p->szLocaleName;

	return true;
}

char CPythonNonPlayer::GetName(DWORD dwVnum)
{
	const TMobTable* p = GetTable(dwVnum);

	if (!p)
	{
		return *"UNKNOWN";
	}

	return *p->szLocaleName;
}

bool CPythonNonPlayer::GetInstanceType(DWORD dwVnum, BYTE* pbType)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
	{
		return false;
	}

	*pbType = p->bType;

	return true;
}

const CPythonNonPlayer::TMobTable * CPythonNonPlayer::GetTable(DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find(dwVnum);

	if (itor == m_NonPlayerDataMap.end())
	{
		return NULL;
	}

#ifdef INGAME_WIKI
	return itor->second.mobTable.get();
#else
	return itor->second;
#endif
}

BYTE CPythonNonPlayer::GetEventType(DWORD dwVnum)
{
	const TMobTable * p = GetTable(dwVnum);

	if (!p)
	{
		//Tracef("CPythonNonPlayer::GetEventType - Failed to find virtual number\n");
		return ON_CLICK_EVENT_NONE;
	}

	return p->bOnClickType;
}

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
DWORD CPythonNonPlayer::GetMonsterLevel(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return 0;
	}

	return c_pTable->bLevel;
}
#endif

#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
bool CPythonNonPlayer::IsAggressive(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return 0;
	}

	return (IS_SET(c_pTable->dwAIFlag, AIFLAG_AGGRESSIVE));
}
#endif

BYTE CPythonNonPlayer::GetEventTypeByVID(DWORD dwVID)
{
	CInstanceBase * pInstance = CPythonCharacterManager::Instance().GetInstancePtr(dwVID);

	if (NULL == pInstance)
	{
		//Tracef("CPythonNonPlayer::GetEventTypeByVID - There is no Virtual Number\n");
		return ON_CLICK_EVENT_NONE;
	}

	WORD dwVnum = pInstance->GetVirtualNumber();
	return GetEventType(dwVnum);
}

const char*	CPythonNonPlayer::GetMonsterName(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		static const char* sc_szEmpty = "";
		return sc_szEmpty;
	}

	return c_pTable->szLocaleName;
}

DWORD CPythonNonPlayer::GetMonsterColor(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable * c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return 0;
	}

	return c_pTable->dwMonsterColor;
}

void CPythonNonPlayer::GetMatchableMobList(int iLevel, int iInterval, TMobTableList * pMobTableList)
{
	/*
		pMobTableList->clear();

		TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin();
		for (; itor != m_NonPlayerDataMap.end(); ++itor)
		{
			TMobTable * pMobTable = itor->second;

			int iLowerLevelLimit = iLevel-iInterval;
			int iUpperLevelLimit = iLevel+iInterval;

			if ((pMobTable->abLevelRange[0] >= iLowerLevelLimit && pMobTable->abLevelRange[0] <= iUpperLevelLimit) ||
				(pMobTable->abLevelRange[1] >= iLowerLevelLimit && pMobTable->abLevelRange[1] <= iUpperLevelLimit))
			{
				pMobTableList->push_back(pMobTable);
			}
		}
	*/
}

DWORD CPythonNonPlayer::GetMonsterRaceFlag(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		DWORD dwRaceFlag = 0;
		return dwRaceFlag;
	}

	return c_pTable->dwRaceFlag;
}

bool CPythonNonPlayer::IsMonsterRaceFlag(DWORD dwVnum, DWORD dwFlag)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return false;
	}

	if (c_pTable->dwVnum == dwVnum && IS_SET(c_pTable->dwRaceFlag, dwFlag))
	{
		return true;
	}

	return false;
}

DWORD CPythonNonPlayer::GetMonsterResist(DWORD dwVnum, BYTE bResistNum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return 0;
	}

	if (bResistNum >= MOB_RESISTS_MAX_NUM)
	{
		return 0;
	}

	return c_pTable->cResists[bResistNum];
}

#ifdef ENABLE_12ZI_ELEMENT_ADD
bool CPythonNonPlayer::MonsterHasRaceFlag(DWORD dwVnum, const char* szSearchString)
{
	std::string raceFlagList[] =
	{
		"ANIMAL", "UNDEAD", "DEVIL", "HUMAN", "ORC", "MILGYO", "INSECT", "FIRE", "ICE", "DESERT", "TREE",
		"ATT_ELEC", "ATT_FIRE", "ATT_ICE", "ATT_WIND", "ATT_EARTH", "ATT_DARK", "ATT_CZ"
	};

	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return false;
	}

	DWORD dwRaceFlag = c_pTable->dwRaceFlag;

	if (dwRaceFlag == 0)
	{
		return false;
	}

	std::string toSearchString("");
	std::string searchString(szSearchString);
	int tmpFlag;

	for (int i = 0; i < sizeof(raceFlagList) / sizeof(raceFlagList[0]); i++)
	{
		tmpFlag = static_cast<int>(pow(2.0, static_cast<double>(i)));
		if (dwRaceFlag & tmpFlag)
		{
			if (!!toSearchString.compare(""))
			{
				toSearchString += ",";
			}
			toSearchString += raceFlagList[i];
		}
	}

	size_t found = toSearchString.find(searchString);
	if (found != std::string::npos)
	{
		return true;
	}

	return false;
}
#endif

#ifdef ZUO_PANEL_ENABLE
extern void SpecialMarkToLower(std::string& str);

std::map<DWORD, std::string> CPythonNonPlayer::GetMonsterByName(std::string MonsterName, bool bBoss, bool bMetin, const std::unordered_set<DWORD>& s_banned_list)
{
	std::map<DWORD, std::string> ret_map;
	static bool is_case_sensitive = false;

	if (!is_case_sensitive)
	{
		std::transform(MonsterName.begin(), MonsterName.end(), MonsterName.begin(), tolower);
		SpecialMarkToLower(MonsterName);
	}

	for (TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin(); itor != m_NonPlayerDataMap.end(); ++itor)
	{
		const TMobTable* pMobTable = itor->second.mobTable.get();;
		if (!pMobTable)
			continue;

		// Skipping everything except mob&stones
		if (pMobTable->bType != 0 && pMobTable->bType != 2)
			continue;

		std::string tmp_name = pMobTable->szLocaleName;

		if ((bBoss && (pMobTable->bRank >= 4 && pMobTable->bType == 0)) || (bMetin && (pMobTable->bRank == 5 && pMobTable->bType == 2)) || (!bMetin && !bBoss))
		{
			if ((!bMetin && (pMobTable->bRank == 5 && pMobTable->bType == 2)) || (!bBoss && (pMobTable->bRank >= 4 && pMobTable->bType == 0)))
				continue;

			if (!is_case_sensitive)
			{
				std::transform(tmp_name.begin(), tmp_name.end(), tmp_name.begin(), tolower);
				SpecialMarkToLower(tmp_name);
			}

			if (tmp_name.find(MonsterName) != std::string::npos && s_banned_list.find(pMobTable->dwVnum) == s_banned_list.end())
				ret_map.insert(std::make_pair(itor->second.mobTable->dwVnum, pMobTable->szLocaleName));
		}
	}

	return ret_map;
}
#endif

#ifdef INGAME_WIKI
#include "../GameLib/RaceManager.h"
std::string CPythonNonPlayer::GetNameString(DWORD dwVnum)
{
	const TMobTable* p = GetTable(dwVnum);

	if (!p)
	{
		return "";
	}

	//std::string translatedName = GetNameTranslation(dwVnum);
	//if (translatedName != "")
	//{
	//	return translatedName;
	//}

	return std::string(p->szLocaleName);
}

bool CPythonNonPlayer::CanRenderMonsterModel(DWORD dwMonsterVnum)
{
	CRaceData* pRaceData;
	if (!CRaceManager::Instance().GetRaceDataPointer(dwMonsterVnum, &pRaceData, false))
	{
		return false;
	}

	return true;
}

size_t CPythonNonPlayer::WikiLoadClassMobs(BYTE bType, unsigned short fromLvl, unsigned short toLvl)
{
	m_vecTempMob.clear();
	for (auto it = m_NonPlayerDataMap.begin(); it != m_NonPlayerDataMap.end(); ++it)
	{
		if (!it->second.isFiltered && it->second.mobTable->bLevel >= fromLvl &&
				it->second.mobTable->bLevel < toLvl && CanRenderMonsterModel(it->second.mobTable->dwVnum))
		{
			if (bType == 0 && it->second.mobTable->bType == MONSTER && it->second.mobTable->bRank >= 4)
			{
				m_vecTempMob.push_back(it->first);
			}
			else if (bType == 1 && it->second.mobTable->bType == MONSTER && it->second.mobTable->bRank < 4)
			{
				m_vecTempMob.push_back(it->first);
			}
			else if (bType == 2 && it->second.mobTable->bType == STONE)
			{
				m_vecTempMob.push_back(it->first);
			}
		}
	}

	return m_vecTempMob.size();
}

void CPythonNonPlayer::WikiSetBlacklisted(DWORD vnum)
{
	auto it = m_NonPlayerDataMap.find(vnum);
	if (it != m_NonPlayerDataMap.end())
	{
		it->second.isFiltered = true;
	}
}

std::tuple<const char*, int> CPythonNonPlayer::GetMonsterDataByNamePart(const char* namePart)
{
	char searchName[CHARACTER_NAME_MAX_LEN + 1];
	memcpy(searchName, namePart, sizeof(searchName));
	for (size_t j = 0; j < sizeof(searchName); j++)
	{
		searchName[j] = static_cast<char>(tolower(searchName[j]));
	}
	std::string tempSearchName = searchName;

	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin();
	for (; itor != m_NonPlayerDataMap.end(); ++itor)
	{
		TMobTable* pMobTable = itor->second.mobTable.get();

		if (itor->second.isFiltered)
		{
			continue;
		}

		const char* mobBaseName = CPythonNonPlayer::Instance().GetNameString(pMobTable->dwVnum).c_str();
		std::string tempName = mobBaseName;
		if (!tempName.size())
		{
			continue;
		}

		std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);

		const size_t tempSearchNameLenght = tempSearchName.length();
		if (tempName.length() < tempSearchNameLenght)
		{
			continue;
		}

		if (!tempName.substr(0, tempSearchNameLenght).compare(tempSearchName))
		{
			return std::make_tuple(mobBaseName, pMobTable->dwVnum);
		}
	}

	return std::make_tuple("", -1);
}

void CPythonNonPlayer::BuildWikiSearchList()
{
	m_vecWikiNameSort.clear();
	for (auto it = m_NonPlayerDataMap.begin(); it != m_NonPlayerDataMap.end(); ++it)
		if (!it->second.isFiltered)
		{
			m_vecWikiNameSort.push_back(it->second.mobTable.get());
		}

	SortMobDataName();
}

void CPythonNonPlayer::SortMobDataName()
{
	std::qsort(&m_vecWikiNameSort[0], m_vecWikiNameSort.size(), sizeof(m_vecWikiNameSort[0]), [](const void* a, const void* b)
	{
		TMobTable* pItem1 = *(TMobTable**)(static_cast<const TMobTable*>(a));
		std::string stRealName1 = pItem1->szLocaleName;
		std::transform(stRealName1.begin(), stRealName1.end(), stRealName1.begin(), ::tolower);

		TMobTable* pItem2 = *(TMobTable**)(static_cast<const TMobTable*>(b));
		std::string stRealName2 = pItem2->szLocaleName;
		std::transform(stRealName2.begin(), stRealName2.end(), stRealName2.begin(), ::tolower);

		int iSmallLen = min(stRealName1.length(), stRealName2.length());
		int iRetCompare = stRealName1.compare(0, iSmallLen, stRealName2, 0, iSmallLen);

		if (iRetCompare != 0)
		{
			return iRetCompare;
		}

		if (stRealName1.length() < stRealName2.length())
		{
			return -1;
		}
		else if (stRealName2.length() < stRealName1.length())
		{
			return 1;
		}

		return 0;
	});
}

CPythonNonPlayer::TWikiInfoTable* CPythonNonPlayer::GetWikiTable(DWORD dwVnum)
{
	TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.find(dwVnum);

	if (itor == m_NonPlayerDataMap.end())
	{
		return NULL;
	}

	return &(itor->second);
}
#endif

short CPythonNonPlayer::GetRaceFlag(DWORD dwVnum)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return -1;
	}

	return c_pTable->dwRaceFlag;
}

bool CPythonNonPlayer::GetEnchant(DWORD dwVnum, BYTE enchant)
{
	const CPythonNonPlayer::TMobTable* c_pTable = GetTable(dwVnum);
	if (!c_pTable)
	{
		return false;
	}

	if (enchant >= MOB_ENCHANTS_MAX_NUM)
	{
		return false;
	}

	if (c_pTable->cEnchants[enchant] > 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

uint32_t CPythonNonPlayer::GetMobRank(uint32_t vnum)
{
	const auto* pTable = GetTable(vnum);
	if (!pTable)
	{
		return 0;
	}

	return pTable->bRank;
}

uint32_t CPythonNonPlayer::GetMobType(uint32_t vnum)
{
	const auto* pTable = GetTable(vnum);
	if (!pTable)
	{
		return 0;
	}

	return pTable->bType;
}

void CPythonNonPlayer::Clear()
{
}

void CPythonNonPlayer::Destroy()
{
	for (TNonPlayerDataMap::iterator itor = m_NonPlayerDataMap.begin(); itor != m_NonPlayerDataMap.end(); ++itor)
	{
#ifdef INGAME_WIKI
		m_NonPlayerDataMap.erase(itor);
#else
		delete itor->second;
#endif
	}
	m_NonPlayerDataMap.clear();
}

CPythonNonPlayer::CPythonNonPlayer()
{
	Clear();
}

CPythonNonPlayer::~CPythonNonPlayer(void)
{
	Destroy();
}