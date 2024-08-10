#include "StdAfx.h"
#include "../eterPack/EterPackManager.h"
#include "../eterLib/ResourceManager.h"

#include "ItemManager.h"

static DWORD s_adwItemProtoKey[4] =
{
	173217,
	72619434,
	408587239,
	27973291
};

BOOL CItemManager::SelectItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwIndex) && dwIndex < (pTable->dwVnum + pTable->dwVnumRange))
			{
				m_pSelectedItemData = p;

#ifdef ENABLE_FIND_LETTERS_EVENT
				if (m_pSelectedItemData && dwIndex > 90500 && dwIndex < 90600)
				{
					m_pSelectedItemData->SetLetterAscii(dwIndex - pTable->dwVnum);
				}
#endif
				return TRUE;
			}
		}
		Tracef(" CItemManager::SelectItemData - FIND ERROR [%d]\n", dwIndex);
		return FALSE;
	}

	m_pSelectedItemData = f->second;

	return TRUE;
}

CItemData * CItemManager::GetSelectedItemDataPointer()
{
	return m_pSelectedItemData;
}

BOOL CItemManager::GetItemDataPointer(DWORD dwItemID, CItemData ** ppItemData)
{
	if (0 == dwItemID)
	{
		return FALSE;
	}

	TItemMap::iterator f = m_ItemMap.find(dwItemID);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData * p = m_vec_ItemRange[i];
			const CItemData::TItemTable * pTable = p->GetTable();
			if ((pTable->dwVnum < dwItemID) &&
					dwItemID < (pTable->dwVnum + pTable->dwVnumRange))
			{
				*ppItemData = p;
				return TRUE;
			}
		}
		Tracef(" CItemManager::GetItemDataPointer - FIND ERROR [%d]\n", dwItemID);
		return FALSE;
	}

	*ppItemData = f->second;

	return TRUE;
}


CItemData* CItemManager::GetProto(DWORD dwItemID)
{
	if (0 == dwItemID)
	{
		return FALSE;
	}

	TItemMap::iterator f = m_ItemMap.find(dwItemID);

	if (m_ItemMap.end() == f)
	{
		int n = m_vec_ItemRange.size();
		for (int i = 0; i < n; i++)
		{
			CItemData* p = m_vec_ItemRange[i];
			const CItemData::TItemTable* pTable = p->GetTable();
			if ((pTable->dwVnum < dwItemID) &&
					dwItemID < (pTable->dwVnum + pTable->dwVnumRange))
			{
				return p;
			}
		}
		Tracef(" CItemManager::GetProto - FIND ERROR [%d]\n", dwItemID);
		return nullptr;
	}

	return f->second;
}

CItemData * CItemManager::MakeItemData(DWORD dwIndex)
{
	TItemMap::iterator f = m_ItemMap.find(dwIndex);

	if (m_ItemMap.end() == f)
	{
		CItemData * pItemData = CItemData::New();

		m_ItemMap.insert(TItemMap::value_type(dwIndex, pItemData));

		return pItemData;
	}

	return f->second;
}

////////////////////////////////////////////////////////////////////////////////////////
// Load Item Table

bool CItemManager::LoadItemList(const char * c_szFileName)
{
	CMappedFile File;
	LPCVOID pData;

	if (!CEterPackManager::Instance().Get(File, c_szFileName, &pData))
	{
		return false;
	}

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
		{
			continue;
		}

		if (!(TokenVector.size() == 3 || TokenVector.size() == 4))
		{
			TraceError(" CItemManager::LoadItemList(%s) - StrangeLine in %d\n", c_szFileName, i);
			continue;
		}

		const std::string & c_rstrID = TokenVector[0];
		//const std::string & c_rstrType = TokenVector[1];
		const std::string & c_rstrIcon = TokenVector[2];

		DWORD dwItemVNum = atoi(c_rstrID.c_str());

		CItemData * pItemData = MakeItemData(dwItemVNum);

		extern BOOL USE_VIETNAM_CONVERT_WEAPON_VNUM;
		if (USE_VIETNAM_CONVERT_WEAPON_VNUM)
		{
			extern DWORD Vietnam_ConvertWeaponVnum(DWORD vnum);
			DWORD dwMildItemVnum = Vietnam_ConvertWeaponVnum(dwItemVNum);
			if (dwMildItemVnum == dwItemVNum)
			{
				if (4 == TokenVector.size())
				{
					const std::string & c_rstrModelFileName = TokenVector[3];
					pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
				}
				else
				{
					pItemData->SetDefaultItemData(c_rstrIcon.c_str());
				}
			}
			else
			{
				DWORD dwMildBaseVnum = dwMildItemVnum / 10 * 10;
				char szMildIconPath[MAX_PATH];
				sprintf(szMildIconPath, "icon/item/%.5d.tga", dwMildBaseVnum);
				if (4 == TokenVector.size())
				{
					char szMildModelPath[MAX_PATH];
					sprintf(szMildModelPath, "d:/ymir work/item/weapon/%.5d.gr2", dwMildBaseVnum);
					pItemData->SetDefaultItemData(szMildIconPath, szMildModelPath);
				}
				else
				{
					pItemData->SetDefaultItemData(szMildIconPath);
				}
			}
		}
		else
		{
			if (4 == TokenVector.size())
			{
				const std::string & c_rstrModelFileName = TokenVector[3];
				pItemData->SetDefaultItemData(c_rstrIcon.c_str(), c_rstrModelFileName.c_str());
			}
			else
			{
				pItemData->SetDefaultItemData(c_rstrIcon.c_str());
			}
		}
	}

	return true;
}

const std::string& __SnapString(const std::string& c_rstSrc, std::string& rstTemp)
{
	UINT uSrcLen = c_rstSrc.length();
	if (uSrcLen < 2)
	{
		return c_rstSrc;
	}

	if (c_rstSrc[0] != '"')
	{
		return c_rstSrc;
	}

	UINT uLeftCut = 1;

	UINT uRightCut = uSrcLen;
	if (c_rstSrc[uSrcLen - 1] == '"')
	{
		uRightCut = uSrcLen - 1;
	}

	rstTemp = c_rstSrc.substr(uLeftCut, uRightCut - uLeftCut);
	return rstTemp;
}

bool CItemManager::LoadItemDesc(const char* c_szFileName)
{
	const VOID* pvData;
	CMappedFile kFile;
	if (!CEterPackManager::Instance().Get(kFile, c_szFileName, &pvData))
	{
		Tracenf("CItemManager::LoadItemDesc(c_szFileName=%s) - Load Error", c_szFileName);
		return false;
	}

	CMemoryTextFileLoader kTextFileLoader;
	kTextFileLoader.Bind(kFile.Size(), pvData);

	std::string stTemp;

	CTokenVector kTokenVector;
	for (DWORD i = 0; i < kTextFileLoader.GetLineCount(); ++i)
	{
		if (!kTextFileLoader.SplitLineByTab(i, &kTokenVector))
		{
			continue;
		}

		while (kTokenVector.size() < ITEMDESC_COL_NUM)
		{
			kTokenVector.push_back("");
		}

		//assert(kTokenVector.size()==ITEMDESC_COL_NUM);

		DWORD dwVnum = atoi(kTokenVector[ITEMDESC_COL_VNUM].c_str());
		const std::string& c_rstDesc = kTokenVector[ITEMDESC_COL_DESC];
		const std::string& c_rstSumm = kTokenVector[ITEMDESC_COL_SUMM];
		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
		{
			continue;
		}

		CItemData* pkItemDataFind = f->second;

		pkItemDataFind->SetDescription(__SnapString(c_rstDesc, stTemp));
		pkItemDataFind->SetSummary(__SnapString(c_rstSumm, stTemp));
	}
	return true;
}

DWORD GetHashCode( const char* pString )
{
	unsigned long i, len;
	unsigned long ch;
	unsigned long result;

	len     = strlen( pString );
	result = 5381;
	for ( i = 0; i < len; i++ )
	{
		ch = (unsigned long)pString[i];
		result = ((result << 5) + result) + ch; // hash * 33 + ch
	}

	return result;
}

bool CItemManager::LoadItemTable(const char* c_szFileName)
{
	CMappedFile file;
	LPCVOID pvData;

	if (!CEterPackManager::Instance().Get(file, c_szFileName, &pvData))
	{
		return false;
	}

	DWORD dwFourCC, dwElements, dwDataSize;
	DWORD dwVersion = 0;
	DWORD dwStride = 0;

	file.Read(&dwFourCC, sizeof(DWORD));

	if (dwFourCC == MAKEFOURCC('M', 'I', 'P', 'X'))
	{
		file.Read(&dwVersion, sizeof(DWORD));
		file.Read(&dwStride, sizeof(DWORD));

		if (dwVersion != 1)
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] VERSION[%d]", c_szFileName, dwVersion);
			return false;
		}

#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		if (!CItemData::TItemTableAll::IsValidStruct(dwStride))
#else
		if (dwStride != sizeof(CItemData::TItemTable))
#endif
		{
			TraceError("CPythonItem::LoadItemTable: invalid item_proto[%s] STRIDE[%d] != sizeof(SItemTable)",
					   c_szFileName, dwStride, sizeof(CItemData::TItemTable));
			return false;
		}
	}
	else if (dwFourCC != MAKEFOURCC('M', 'I', 'P', 'T'))
	{
		TraceError("CPythonItem::LoadItemTable: invalid item proto type %s", c_szFileName);
		return false;
	}

	file.Read(&dwElements, sizeof(DWORD));
	file.Read(&dwDataSize, sizeof(DWORD));

	BYTE * pbData = new BYTE[dwDataSize];
	file.Read(pbData, dwDataSize);

	/////

	CLZObject zObj;

	if (!CLZO::Instance().Decompress(zObj, pbData, s_adwItemProtoKey))
	{
		delete [] pbData;
		return false;
	}

	/////

	char szName[64 + 1];
	std::map<DWORD, DWORD> itemNameMap;

	for (DWORD i = 0; i < dwElements; ++i)
	{
#ifdef ENABLE_PROTOSTRUCT_AUTODETECT
		CItemData::TItemTable t = {0};
		CItemData::TItemTableAll::Process(zObj.GetBuffer(), dwStride, i, t);
#else
		CItemData::TItemTable & t = *((CItemData::TItemTable *) zObj.GetBuffer() + i);
#endif
		CItemData::TItemTable * table = &t;

		CItemData * pItemData;
		DWORD dwVnum = table->dwVnum;

		TItemMap::iterator f = m_ItemMap.find(dwVnum);
		if (m_ItemMap.end() == f)
		{
			_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum);

#ifdef INGAME_WIKI
			pItemData = CItemData::New();
#endif

			if (CResourceManager::Instance().IsFileExist(szName) == false)
			{
#ifdef INGAME_WIKI
				pItemData->ValidateImage(false);
#endif

				std::map<DWORD, DWORD>::iterator itVnum = itemNameMap.find(GetHashCode(table->szName));

				if (itVnum != itemNameMap.end())
				{
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", itVnum->second);
				}
				else
				{
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", dwVnum - dwVnum % 10);
				}

				if (CResourceManager::Instance().IsFileExist(szName) == false)
				{
#ifdef _DEBUG
					TraceError("%16s(#%-5d) cannot find icon file. setting to default.", table->szName, dwVnum);
#endif
					const DWORD EmptyBowl = 27995;
					_snprintf(szName, sizeof(szName), "icon/item/%05d.tga", EmptyBowl);
				}
			}

			pItemData = CItemData::New();

			pItemData->SetDefaultItemData(szName);
			m_ItemMap.insert(TItemMap::value_type(dwVnum, pItemData));
#ifdef INGAME_WIKI
			pItemData->SetItemTableData(table);
			if (!CResourceManager::Instance().IsFileExist(pItemData->GetIconFileName().c_str()))
			{
				pItemData->ValidateImage(false);
			}
#endif
		}
		else
		{
			pItemData = f->second;
#ifdef INGAME_WIKI
			pItemData->SetItemTableData(table);
#endif
		}
		if (itemNameMap.find(GetHashCode(table->szName)) == itemNameMap.end())
		{
			itemNameMap.insert(std::map<DWORD, DWORD>::value_type(GetHashCode(table->szName), table->dwVnum));
		}
		pItemData->SetItemTableData(table);
		if (0 != table->dwVnumRange)
		{
			m_vec_ItemRange.push_back(pItemData);
		}
	}

	delete [] pbData;

	CreateSortedItemData();

	return true;
}

void CItemManager::CreateSortedItemData()
{
	m_vecItemDataSorted.clear();

	for (auto it : m_ItemMap)
	{
		m_vecItemDataSorted.push_back(it.second);
	}

	std::qsort(&m_vecItemDataSorted[0], m_vecItemDataSorted.size(), sizeof(CItemData*), [](const void* first, const void* second)
	{
		CItemData* a = (CItemData*) * ((const void**)first);
		CItemData* b = (CItemData*) * ((const void**)second);

		return stricmp(a->GetName(), b->GetName());
	});
}

CItemManager::TItemRange FindItemNameRange(CItemManager::TItemRange itRange, char charValue, int charIndex)
{
	int dist = std::distance(itRange.first, itRange.second);
	charValue = tolower(charValue);

	const int alphabetMaxNum = 'z' - 'a' + 1; // +1 because starts with idx 0
	int alphabetIdx = MAX(0, MIN(alphabetMaxNum - 1, charValue - 'a'));
	int startIterIdx = dist * alphabetIdx / alphabetMaxNum;

	// find first iter
	CItemManager::TItemVector::const_iterator itCur = itRange.first + startIterIdx;
	CItemManager::TItemRange tmpRange = itRange;
	while (itCur != tmpRange.second)
	{
		CItemData* pCurItem = *itCur;
		char currentChar = pCurItem->GetNameString().length() > charIndex ? tolower(pCurItem->GetName()[charIndex]) : 0;

		// current character is smaller than searched value
		if (currentChar < charValue)
		{
			itRange.first = itCur;
			tmpRange.first = itCur;
			itCur += MAX(1, std::distance(itCur, tmpRange.second) / 2);
		}
		// current character is equal or larger than searched value
		else if (itCur != tmpRange.first)
		{
			if (
				// current character is equal but the one before this is equal as well
				(
					currentChar == charValue &&
					(*(itCur - 1))->GetNameString().length() > charIndex &&
					tolower((*(itCur - 1))->GetName()[charIndex]) == charValue
				)
				// current character is larger than the searched value
				|| (currentChar > charValue)
			)
			{
				if (currentChar > charValue)
				{
					itRange.second = itCur;
				}
				tmpRange.second = itCur;
				itCur -= MAX(1, std::distance(tmpRange.first, itCur) / 2);
			}
			// current character is equal and the one before this is not equal
			else
			{
				break;
			}
		}
		else if (currentChar == charValue)
		{
			break;
		}
		// current character is larger and the iterator reached begin of vector -> no results found -> set to end iterator
		else
		{
			itCur = itRange.second;
		}
	}

	itRange.first = itCur;

	// find last iter if first iter was found
	while (itCur != itRange.second)
	{
		CItemData* pCurItem = *itCur;

		if (pCurItem->GetNameString().length() < charIndex || tolower(pCurItem->GetName()[charIndex]) != charValue)
		{
			itRange.second = itCur;
			break;
		}

		itCur++;
	}

	return itRange;
}

CItemManager::TItemRange CItemManager::GetProtosByName(const char* name) const
{
	auto it_start = m_vecItemDataSorted.begin();
	auto it_end = m_vecItemDataSorted.end();

	int nameLen = strlen(name);
	for (int i = 0; i < nameLen; ++i)
	{
		TItemRange ret = FindItemNameRange(TItemRange(it_start, it_end), name[i], i);
		it_start = ret.first;
		it_end = ret.second;

		if (it_start == it_end)
		{
			break;
		}
	}

	return TItemRange(it_start, it_end);
}

BYTE CItemManager::GetApplyTypeByNameEquipment(const std::string& c_rstApplyName)
{
	TraceError("CItemData::%s", c_rstApplyName.c_str());
	auto it = m_applyNameMap.find("CItemData::" + c_rstApplyName);
	if (it == m_applyNameMap.end())
	{
		return CItemData::APPLY_NONE;
	}

	return it->second;
}

BYTE CItemManager::GetApplyTypeByName(const std::string& c_rstApplyName)
{
	auto it = m_applyNameMap.find("CItemData::APPLY_" + c_rstApplyName);
	if (it == m_applyNameMap.end())
	{
		return CItemData::APPLY_NONE;
	}

	return it->second;
}

void CItemManager::Destroy()
{
	TItemMap::iterator i;
	for (i = m_ItemMap.begin(); i != m_ItemMap.end(); ++i)
	{
		CItemData::Delete(i->second);
	}

	m_ItemMap.clear();
#ifdef INGAME_WIKI
	m_tempItemVec.clear();
#endif
}

void SpecialMarkToLower(std::string& str)
{
	char marks[] = { ' ', '”', '•', '£', 'Ø', 'è', '∆', '—', '\n' };
	char lower_marks[] = { 'Í', 'Û', 'π', '≥', 'ø', 'ü', 'Ê', 'Ò', '\n' };
	for (BYTE i = 0; marks[i] != '\n'; i++)
	{
		std::string rep;
		rep.push_back(lower_marks[i]);
		while (str.find(marks[i]) != std::string::npos)
		{
			str.replace(str.find(marks[i]), 1, rep);
		}
	}
}

#ifdef OX_EVENT_SYSTEM_ENABLE
std::map<DWORD, std::string> CItemManager::GetItemByName(std::string ItemName)
{
	std::map<DWORD, std::string> ret_map;
	static bool is_case_sensitive = false;

	if (!is_case_sensitive)
	{
		std::transform(ItemName.begin(), ItemName.end(), ItemName.begin(), tolower);
		SpecialMarkToLower(ItemName);
	}

	for (TItemMap::iterator it = m_ItemMap.begin(); it != m_ItemMap.end(); it++)
	{
		std::string tmp_name = it->second->GetName();
		if (!is_case_sensitive)
		{
			std::transform(tmp_name.begin(), tmp_name.end(), tmp_name.begin(), tolower);
			SpecialMarkToLower(tmp_name);
		}

		if (tmp_name.find(ItemName) != std::string::npos)
		{
			ret_map.insert(std::make_pair(it->second->GetTable()->dwVnum, it->second->GetName()));
		}
	}

	return ret_map;
}
#endif

#ifdef INGAME_WIKI
BOOL CItemManager::CanIncrSelectedItemRefineLevel()
{
	auto* tbl = GetSelectedItemDataPointer();
	if (!tbl)
	{
		return FALSE;
	}

	return (tbl->GetType() == CItemData::ITEM_TYPE_ARMOR || tbl->GetType() == CItemData::ITEM_TYPE_WEAPON || tbl->GetType() == CItemData::ITEM_TYPE_BELT);
}

BOOL CItemManager::CanIncrItemRefineLevel(DWORD itemVnum)
{
	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(itemVnum, &tbl))
	{
		return false;
	}

	if (!tbl)
	{
		return FALSE;
	}

	return (tbl->GetType() == CItemData::ITEM_TYPE_ARMOR || tbl->GetType() == CItemData::ITEM_TYPE_WEAPON);
}

bool CItemManager::CanLoadWikiItem(DWORD dwVnum)
{
	DWORD StartRefineVnum = GetWikiItemStartRefineVnum(dwVnum);

	if (StartRefineVnum != dwVnum)
	{
		return false;
	}

	if (StartRefineVnum % 10 != 0)
	{
		return false;
	}

	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(StartRefineVnum, &tbl))
	{
		return false;
	}

	return true;
}

DWORD CItemManager::GetWikiItemStartRefineVnum(DWORD dwVnum)
{
	auto baseItemName = GetWikiItemBaseRefineName(dwVnum);
	if (!baseItemName.size())
	{
		return 0;
	}

	DWORD manage_vnum = dwVnum;
	while (!(strcmp(baseItemName.c_str(), GetWikiItemBaseRefineName(manage_vnum).c_str())))
	{
		--manage_vnum;
	}

	return (manage_vnum + 1);
}

std::string CItemManager::GetWikiItemBaseRefineName(DWORD dwVnum)
{
	CItemData* tbl = nullptr;
	if (!GetItemDataPointer(dwVnum, &tbl))
	{
		return "";
	}

	auto* p = const_cast<char*>(strrchr(tbl->GetName(), '+'));
	if (!p)
	{
		return "";
	}

	std::string sFirstItemName(tbl->GetName(),
							   (tbl->GetName() + (p - tbl->GetName())));

	return sFirstItemName;
}

bool CItemManager::IsFilteredAntiflag(CItemData* itemData, DWORD raceFilter)
{
	if (raceFilter != 0)
	{
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SHAMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_SHAMAN)
		{
			return false;
		}

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_SURA) && raceFilter & CItemData::ITEM_ANTIFLAG_SURA)
		{
			return false;
		}

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_ASSASSIN) && raceFilter & CItemData::ITEM_ANTIFLAG_ASSASSIN)
		{
			return false;
		}

		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WARRIOR) && raceFilter & CItemData::ITEM_ANTIFLAG_WARRIOR)
		{
			return false;
		}

#ifdef INGAME_WIKI_WOLFMAN
		if (!itemData->IsAntiFlag(CItemData::ITEM_ANTIFLAG_WOLFMAN) && raceFilter & CItemData::ITEM_ANTIFLAG_WOLFMAN)
		{
			return false;
		}
#endif
	}

	return true;
}

size_t CItemManager::WikiLoadClassItems(BYTE classType, DWORD raceFilter)
{
	m_tempItemVec.clear();

	for (TItemMap::iterator it = m_ItemMap.begin(); it != m_ItemMap.end(); ++it)
	{
		if (!it->second->IsValidImage() || it->first < 10 || it->second->IsBlacklisted())
		{
			continue;
		}

		bool _can_load = CanLoadWikiItem(it->first);

		switch (classType)
		{
		case 0: // weapon
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_WEAPON && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 1: // body
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_BODY && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 2:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_EAR && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 3:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_FOOTS && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 4:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_HEAD && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 5:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_NECK && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 6:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_SHIELD && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 7:
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_WRIST && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 8: // chests
			if (it->second->GetType() == CItemData::ITEM_TYPE_GIFTBOX)
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 9: // belts
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_BELT)
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		case 10: // talisman
			if (_can_load && it->second->GetType() == CItemData::ITEM_TYPE_ARMOR && it->second->GetSubType() == CItemData::ARMOR_PENDANT && !IsFilteredAntiflag(it->second, raceFilter))
			{
				m_tempItemVec.push_back(it->first);
			}
			break;
		}
	}

	return m_tempItemVec.size();
}

std::tuple<const char*, int> CItemManager::SelectByNamePart(const char* namePart)
{
	char searchName[CItemData::ITEM_NAME_MAX_LEN + 1];
	memcpy(searchName, namePart, sizeof(searchName));
	for (size_t j = 0; j < sizeof(searchName); j++)
	{
		searchName[j] = static_cast<char>(tolower(searchName[j]));
	}
	std::string tempSearchName = searchName;

	for (TItemMap::iterator i = m_ItemMap.begin(); i != m_ItemMap.end(); i++)
	{
		const CItemData::TItemTable* tbl = i->second->GetTable();

		if (!i->second->IsBlacklisted())
		{
			DWORD StartRefineVnum = GetWikiItemStartRefineVnum(i->first);
			if (StartRefineVnum != 0)
			{
				CItemData* _sRb = nullptr;
				if (!GetItemDataPointer(StartRefineVnum, &_sRb))
				{
					continue;
				}

				if (_sRb->IsBlacklisted())
				{
					continue;
				}
			}
		}
		else
		{
			continue;
		}

		CItemData* itemData = nullptr;
		if (!GetItemDataPointer(i->first, &itemData))
		{
			continue;
		}

		std::string tempName = itemData->GetName();
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
			return std::make_tuple(itemData->GetName(), i->first);
		}
	}

	return std::make_tuple("", -1);
}
#endif

bool CItemManager::LoadItemScale(const char* szItemScale)
{
	CMappedFile File;
	LPCVOID pData;
	if (!CEterPackManager::Instance().Get(File, szItemScale, &pData))
		return false;

	CMemoryTextFileLoader textFileLoader;
	textFileLoader.Bind(File.Size(), pData);

	CTokenVector TokenVector;
	for (DWORD i = 0; i < textFileLoader.GetLineCount(); ++i)
	{
		if (!textFileLoader.SplitLine(i, &TokenVector, "\t"))
			continue;

		if (!(TokenVector.size() == 6 || TokenVector.size() == 7))
		{
			TraceError(" CItemManager::LoadItemScale(%s) - Error on line %d\n", szItemScale, i);
			continue;
		}

		const std::string& c_rstrID = TokenVector[ITEMSCALE_COL_VNUM];
		const std::string& c_rstrJob = TokenVector[ITEMSCALE_COL_JOB];
		const std::string& c_rstrSex = TokenVector[ITEMSCALE_COL_SEX];
		const std::string& c_rstrScaleX = TokenVector[ITEMSCALE_COL_SCALE_X];
		const std::string& c_rstrScaleY = TokenVector[ITEMSCALE_COL_SCALE_Y];
		const std::string& c_rstrScaleZ = TokenVector[ITEMSCALE_COL_SCALE_Z];

		DWORD dwItemVnum = atoi(c_rstrID.c_str());
		BYTE bJob = 0;
		if (!strcmp(c_rstrJob.c_str(), "JOB_WARRIOR")) bJob = NRaceData::JOB_WARRIOR;
		if (!strcmp(c_rstrJob.c_str(), "JOB_ASSASSIN")) bJob = NRaceData::JOB_ASSASSIN;
		if (!strcmp(c_rstrJob.c_str(), "JOB_SURA")) bJob = NRaceData::JOB_SURA;
		if (!strcmp(c_rstrJob.c_str(), "JOB_SHAMAN")) bJob = NRaceData::JOB_SHAMAN;
#ifdef ENABLE_WOLFMAN_CHARACTER
		if (!strcmp(c_rstrJob.c_str(), "JOB_WOLFMAN")) bJob = NRaceData::JOB_WOLFMAN;
#endif
		BYTE bSex = c_rstrSex[0] == 'M';

		float fScaleX = atof(c_rstrScaleX.c_str()) * 0.01f;
		float fScaleY = atof(c_rstrScaleY.c_str()) * 0.01f;
		float fScaleZ = atof(c_rstrScaleZ.c_str()) * 0.01f;
		float fParticleScale = 1.0f;
		if (TokenVector.size() == 7)
		{
			const std::string& c_rstrParticleScale = TokenVector[ITEMSCALE_COL_PARTICLE_SCALE];
			fParticleScale = atof(c_rstrParticleScale.c_str());
		}

		CItemData* pItemData = MakeItemData(dwItemVnum);
		BYTE bGradeMax = 5;
		//if (pItemData->GetType() == CItemData::ITEM_TYPE_COSTUME && pItemData->GetSubType() == CItemData::COSTUME_AURA)
		//	bGradeMax = 6;

		for (BYTE i = 0; i < bGradeMax; ++i)
		{
			pItemData = MakeItemData(dwItemVnum + i);
			if (pItemData)
				pItemData->SetItemTableScaleData(bJob, bSex, fScaleX, fScaleY, fScaleZ, fParticleScale);
		}
	}

	return true;
}

CItemManager::CItemManager() : m_pSelectedItemData(NULL)
{
#define APPEND_APPLY(apply) m_applyNameMap[#apply] = apply
	APPEND_APPLY(CItemData::APPLY_MAX_HP);
	APPEND_APPLY(CItemData::APPLY_MAX_SP);
	APPEND_APPLY(CItemData::APPLY_CON);
	APPEND_APPLY(CItemData::APPLY_INT);
	APPEND_APPLY(CItemData::APPLY_STR);
	APPEND_APPLY(CItemData::APPLY_DEX);
	APPEND_APPLY(CItemData::APPLY_ATT_SPEED);
	APPEND_APPLY(CItemData::APPLY_MOV_SPEED);
	APPEND_APPLY(CItemData::APPLY_CAST_SPEED);
	APPEND_APPLY(CItemData::APPLY_HP_REGEN);
	APPEND_APPLY(CItemData::APPLY_SP_REGEN);
	APPEND_APPLY(CItemData::APPLY_POISON_PCT);
	APPEND_APPLY(CItemData::APPLY_STUN_PCT);
	APPEND_APPLY(CItemData::APPLY_SLOW_PCT);
	APPEND_APPLY(CItemData::APPLY_CRITICAL_PCT);
	APPEND_APPLY(CItemData::APPLY_PENETRATE_PCT);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_HUMAN);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_ANIMAL);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_ORC);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_MILGYO);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_UNDEAD);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_DEVIL);
	APPEND_APPLY(CItemData::APPLY_STEAL_HP);
	APPEND_APPLY(CItemData::APPLY_STEAL_SP);
	APPEND_APPLY(CItemData::APPLY_MANA_BURN_PCT);
	APPEND_APPLY(CItemData::APPLY_DAMAGE_SP_RECOVER);
	APPEND_APPLY(CItemData::APPLY_BLOCK);
	APPEND_APPLY(CItemData::APPLY_DODGE);
	APPEND_APPLY(CItemData::APPLY_RESIST_SWORD);
	APPEND_APPLY(CItemData::APPLY_RESIST_TWOHAND);
	APPEND_APPLY(CItemData::APPLY_RESIST_DAGGER);
	APPEND_APPLY(CItemData::APPLY_RESIST_BELL);
	APPEND_APPLY(CItemData::APPLY_RESIST_FAN);
	APPEND_APPLY(CItemData::APPLY_RESIST_BOW);
	APPEND_APPLY(CItemData::APPLY_RESIST_FIRE);
	APPEND_APPLY(CItemData::APPLY_RESIST_ELEC);
	APPEND_APPLY(CItemData::APPLY_RESIST_MAGIC);
	APPEND_APPLY(CItemData::APPLY_RESIST_WIND);
	APPEND_APPLY(CItemData::APPLY_REFLECT_MELEE);
	APPEND_APPLY(CItemData::APPLY_REFLECT_CURSE);
	APPEND_APPLY(CItemData::APPLY_POISON_REDUCE);
	APPEND_APPLY(CItemData::APPLY_KILL_SP_RECOVER);
	APPEND_APPLY(CItemData::APPLY_EXP_DOUBLE_BONUS);
	APPEND_APPLY(CItemData::APPLY_GOLD_DOUBLE_BONUS);
	APPEND_APPLY(CItemData::APPLY_ITEM_DROP_BONUS);
	APPEND_APPLY(CItemData::APPLY_POTION_BONUS);
	APPEND_APPLY(CItemData::APPLY_KILL_HP_RECOVER);
	APPEND_APPLY(CItemData::APPLY_IMMUNE_STUN);
	APPEND_APPLY(CItemData::APPLY_IMMUNE_SLOW);
	APPEND_APPLY(CItemData::APPLY_IMMUNE_FALL);
	APPEND_APPLY(CItemData::APPLY_SKILL);
	APPEND_APPLY(CItemData::APPLY_BOW_DISTANCE);
	APPEND_APPLY(CItemData::APPLY_ATT_GRADE_BONUS);
	APPEND_APPLY(CItemData::APPLY_DEF_GRADE_BONUS);
	APPEND_APPLY(CItemData::APPLY_MAGIC_ATT_GRADE);
	APPEND_APPLY(CItemData::APPLY_MAGIC_DEF_GRADE);
	APPEND_APPLY(CItemData::APPLY_CURSE_PCT);
	APPEND_APPLY(CItemData::APPLY_MAX_STAMINA);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_WARRIOR);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_ASSASSIN);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_SURA);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_SHAMAN);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_MONSTER);
	APPEND_APPLY(CItemData::APPLY_MALL_ATTBONUS);
	APPEND_APPLY(CItemData::APPLY_MALL_DEFBONUS);
	APPEND_APPLY(CItemData::APPLY_MALL_EXPBONUS);
	APPEND_APPLY(CItemData::APPLY_MALL_ITEMBONUS);
	APPEND_APPLY(CItemData::APPLY_MALL_GOLDBONUS);
	APPEND_APPLY(CItemData::APPLY_MAX_HP_PCT);
	APPEND_APPLY(CItemData::APPLY_MAX_SP_PCT);
	APPEND_APPLY(CItemData::APPLY_SKILL_DAMAGE_BONUS);
	APPEND_APPLY(CItemData::APPLY_NORMAL_HIT_DAMAGE_BONUS);
	APPEND_APPLY(CItemData::APPLY_SKILL_DEFEND_BONUS);
	APPEND_APPLY(CItemData::APPLY_NORMAL_HIT_DEFEND_BONUS);
	APPEND_APPLY(CItemData::APPLY_EXTRACT_HP_PCT);
	APPEND_APPLY(CItemData::APPLY_PC_BANG_EXP_BONUS);
	APPEND_APPLY(CItemData::APPLY_PC_BANG_DROP_BONUS);
	APPEND_APPLY(CItemData::APPLY_RESIST_WARRIOR);
	APPEND_APPLY(CItemData::APPLY_RESIST_ASSASSIN);
	APPEND_APPLY(CItemData::APPLY_RESIST_SURA);
	APPEND_APPLY(CItemData::APPLY_RESIST_SHAMAN);
	APPEND_APPLY(CItemData::APPLY_ENERGY);
	APPEND_APPLY(CItemData::APPLY_DEF_GRADE);
	APPEND_APPLY(CItemData::APPLY_COSTUME_ATTR_BONUS);
	APPEND_APPLY(CItemData::APPLY_MAGIC_ATTBONUS_PER);
	APPEND_APPLY(CItemData::APPLY_MELEE_MAGIC_ATTBONUS_PER);
	APPEND_APPLY(CItemData::APPLY_RESIST_ICE);
	APPEND_APPLY(CItemData::APPLY_RESIST_EARTH);
	APPEND_APPLY(CItemData::APPLY_RESIST_DARK);
	APPEND_APPLY(CItemData::APPLY_ANTI_CRITICAL_PCT);
	APPEND_APPLY(CItemData::APPLY_ANTI_PENETRATE_PCT);
#ifdef ENABLE_WOLFMAN_CHARACTER
	APPEND_APPLY(CItemData::APPLY_BLEEDING_REDUCE);
	APPEND_APPLY(CItemData::APPLY_BLEEDING_PCT);
	APPEND_APPLY(CItemData::APPLY_ATT_BONUS_TO_WOLFMAN);
	APPEND_APPLY(CItemData::APPLY_RESIST_WOLFMAN);
	APPEND_APPLY(CItemData::APPLY_RESIST_CLAW);
#endif
#ifdef ENABLE_MAGIC_REDUCTION_SYSTEM
	APPEND_APPLY(CItemData::APPLY_RESIST_MAGIC_REDUCTION);
#endif
#ifdef ENABLE_SASH_COSTUME_SYSTEM
	APPEND_APPLY(CItemData::APPLY_ACCEDRAIN_RATE);
#endif
#ifdef ENABLE_12ZI_ELEMENT_ADD
	APPEND_APPLY(CItemData::APPLY_ENCHANT_ELECT);
	APPEND_APPLY(CItemData::APPLY_ENCHANT_FIRE);
	APPEND_APPLY(CItemData::APPLY_ENCHANT_ICE);
	APPEND_APPLY(CItemData::APPLY_ENCHANT_WIND);
	APPEND_APPLY(CItemData::APPLY_ENCHANT_EARTH);
	APPEND_APPLY(CItemData::APPLY_ENCHANT_DARK);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_INSECT);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_DESERT);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_CZ);
#endif
	APPEND_APPLY(CItemData::APPLY_RESIST_ALL);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_METIN);
	APPEND_APPLY(CItemData::APPLY_ATTBONUS_BOSS);
	APPEND_APPLY(CItemData::APPLY_RESIST_MONSTER);
	APPEND_APPLY(CItemData::APPLY_RESIST_BOSS);
	APPEND_APPLY(CItemData::APPLY_PRECISION);

	APPEND_APPLY(CItemData::APPLY_DUNGEON_DAMAGE_BONUS);
	APPEND_APPLY(CItemData::APPLY_DUNGEON_RECV_DAMAGE_BONUS);
	APPEND_APPLY(CItemData::APPLY_AGGRO_MONSTER_BONUS);
	APPEND_APPLY(CItemData::APPLY_DOUBLE_ITEM_DROP_BONUS);
#undef APPEND_APPLY
	// initialize apply names end
}
CItemManager::~CItemManager()
{
	Destroy();
}
