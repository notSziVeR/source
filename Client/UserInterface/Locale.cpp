#include "StdAfx.h"
#include "Locale.h"
#include "PythonConfig.h"

const std::string MULTI_LOCALE_SERVICE = "EUROPE";
std::string g_stLocalePath = "locale";

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CLocaleManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

CLocaleManager::CLocaleManager()
{
	const TLanguageInfo kNameInitInfo[] = {
		{ "ENGLISH", "en", 1252 },
		{ "GERMAN", "de", 1252 },
		{ "ROMANIA", "ro", 1252 },
		{ "POLISH", "pl", 1252 },
		{ "TURKISH", "tr", 1252 },
		{ "CZECH", "cz", 1252 },
		{ "SPANISH", "es", 1252 },
		{ "FRENCH", "fr", 1252 },
		{ "GREEK", "gr", 1252 },
		{ "HUNGARIAN", "hu", 1252 },
		{ "ITALY", "it", 1252 },
		{ "PORTUGAL", "pt", 1252 },
		{ "DANISH", "dn", 1252 },
	};

	for (int i = 0; i < LANGUAGE_MAX_NUM; ++i)
	{
		m_akLanguageInfo[i].stName = kNameInitInfo[i].stName;
		m_akLanguageInfo[i].stShortName = kNameInitInfo[i].stShortName;
	}

	// default language
	m_bLanguage = LANGUAGE_DEFAULT;

	// get system language and set default is available
	const TLanguageIndexInfo kIndexInitInfo[] = {
		{ LANG_GERMAN, LANGUAGE_GERMAN },
		{ LANG_ROMANIAN, LANGUAGE_ROMANIA },
		{ LANG_POLISH, LANGUAGE_POLISH },
		{ LANG_TURKISH, LANGUAGE_TURKISH },
		{ LANG_CZECH, LANGUAGE_CZECH },
		{ LANG_SPANISH, LANGUAGE_SPANISH },
		{ LANG_FRENCH, LANGUAGE_FRENCH },
		{ LANG_GREEK, LANGUAGE_GREEK },
		{ LANG_HUNGARIAN, LANGUAGE_HUNGARIAN },
		{ LANG_ITALIAN, LANGUAGE_ITALY },
		{ LANG_PORTUGUESE, LANGUAGE_PORTUGAL },
		{ LANG_DANISH, LANGUAGE_DANISH },
	};

	for (int i = 0; i < ARRAYSIZE(kIndexInitInfo); ++i)
	{
		if (PRIMARYLANGID(GetSystemDefaultLangID()) == kIndexInitInfo[i].wSystemIndex)
		{
			m_bLanguage = kIndexInitInfo[i].wClientIndex;
			break;
		}
	}
}

CLocaleManager::~CLocaleManager()
{

}

void CLocaleManager::Initialize()
{
	BYTE bLanguage = GetLanguageByName(CPythonConfig::Instance().GetString(CPythonConfig::CLASS_GENERAL, "language", "").c_str(), false);
	if (bLanguage < LANGUAGE_MAX_NUM)
		m_bLanguage = bLanguage;

	WORD codepage = m_akLanguageInfo[m_bLanguage].wCodePage;
	SetDefaultCodePage(codepage);
}

void CLocaleManager::Destroy()
{
}

/*******************************************************************\
| [PUBLIC] Convert Functions
\*******************************************************************/

BYTE CLocaleManager::GetLanguageByName(const char* szName, bool bShort) const
{
	for (int i = 0; i < LANGUAGE_MAX_NUM; ++i)
	{
		if (bShort)
		{
			if (m_akLanguageInfo[i].stShortName == szName)
				return i;
		}
		else
		{
			if (m_akLanguageInfo[i].stName == szName)
				return i;
		}
	}

	return LANGUAGE_MAX_NUM;
}

const char* CLocaleManager::GetLanguageNameByID(BYTE bLanguageID, bool bShort) const
{
	if (bLanguageID >= LANGUAGE_MAX_NUM)
		return "";

	if (bShort)
		return m_akLanguageInfo[bLanguageID].stShortName.c_str();
	else
		return m_akLanguageInfo[bLanguageID].stName.c_str();
}

/*******************************************************************\
| [PUBLIC] Data Functions
\*******************************************************************/

const std::string CLocaleManager::GetName() const
{
	return MULTI_LOCALE_SERVICE;
}

BYTE CLocaleManager::GetLanguage() const
{
	return m_bLanguage;
}

void CLocaleManager::SetLanguage(BYTE bLanguageID)
{
	m_bLanguage = MIN(bLanguageID, LANGUAGE_MAX_NUM - 1);
	CPythonConfig::Instance().Write(CPythonConfig::CLASS_GENERAL, "language", GetLanguageName());
}

const char* CLocaleManager::GetLanguageName() const
{
	return GetLanguageNameByID(GetLanguage(), false);
}

const char* CLocaleManager::GetLanguageShortName() const
{
	return GetLanguageNameByID(GetLanguage(), true);
}

const std::string& CLocaleManager::GetLocaleBasePath() const
{
	return g_stLocalePath;
}

const std::string& CLocaleManager::GetLocalePath() const
{
	static std::string stLocalePath;
	stLocalePath = g_stLocalePath + "/" + GetLanguageShortName();
	return stLocalePath;
}

WORD CLocaleManager::GetCodePage() const
{
	return m_akLanguageInfo[m_bLanguage].wCodePage;
}

/*******************************************************************\
| [PUBLIC] Locale Functions
\*******************************************************************/

DWORD CLocaleManager::GetSkillPower(BYTE level)
{
	static const BYTE SKILL_POWER_NUM = 50;

	if (level >= SKILL_POWER_NUM)
		return 0;

	// 0 5 6 8 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 50 52 54 56 58 60 63 66 69 72 82 85 88 91 94 98 102 106 110 115 125 125 125 125 125
	static unsigned INTERNATIONAL_SKILL_POWERS[SKILL_POWER_NUM] =
	{
		0,
			5,  6,  8, 10, 12,
			14, 16, 18, 20, 22,
			24, 26, 28, 30, 32,
			34, 36, 38, 40, 50, // master
			52, 54, 56, 58, 60,
			63, 66, 69, 72, 82, // grand_master
			85, 88, 91, 94, 98,
			102,106,110,115,125,// perfect_master
			125,
	};

	return INTERNATIONAL_SKILL_POWERS[level];
}
