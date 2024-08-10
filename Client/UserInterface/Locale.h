#pragma once

#include "Locale_inc.h"
#include "../EterBase/Singleton.h"

#define IGNORE_TRANSLATION_STRING "[NOTRANSLATION]"
#define IGNORE_TRANSLATION_STRING_LEN (sizeof(IGNORE_TRANSLATION_STRING) - 1)

#define LSS_SECURITY_KEY "testtesttesttest"

enum
{
	LANGUAGE_ENGLISH,
	LANGUAGE_GERMAN,
	LANGUAGE_ROMANIA,
	LANGUAGE_POLISH,
	LANGUAGE_TURKISH,
	LANGUAGE_CZECH,
	LANGUAGE_SPANISH,
	LANGUAGE_FRENCH,
	LANGUAGE_GREEK,
	LANGUAGE_HUNGARIAN,
	LANGUAGE_ITALY,
	LANGUAGE_PORTUGAL,
	LANGUAGE_DANISH,
	LANGUAGE_MAX_NUM,

	LANGUAGE_DEFAULT = LANGUAGE_ENGLISH,
};

class CLocaleManager : public singleton<CLocaleManager>
{
private:
	typedef struct {
		std::string stName;
		std::string stShortName;
		WORD wCodePage;
	} TLanguageInfo;

	typedef struct {
		WORD wSystemIndex;
		WORD wClientIndex;
	} TLanguageIndexInfo;

public:
	CLocaleManager();
	~CLocaleManager();

	void Initialize();
	void Destroy();

public:
	BYTE GetLanguageByName(const char* szName, bool bShort = true) const;
	const char* GetLanguageNameByID(BYTE bLanguageID, bool bShort = true) const;

public:
	const std::string GetName() const;
	BYTE		GetLanguage() const;
	void		SetLanguage(BYTE bLanguageID);
	const char* GetLanguageName() const;
	const char* GetLanguageShortName() const;

	const std::string& GetLocaleBasePath() const;
	const std::string& GetLocalePath() const;

	WORD		CLocaleManager::GetCodePage() const;

public:
	DWORD		GetSkillPower(BYTE bLevel);

private:
	TLanguageInfo		m_akLanguageInfo[LANGUAGE_MAX_NUM];
	BYTE				m_bLanguage;
};
