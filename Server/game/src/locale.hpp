#pragma once

class LC
{
public:
	enum EErrorTypes
	{
		TRANSLATION_ERROR_NONE,
		TRANSLATION_ERROR_SYSTEM,
		TRANSLATION_ERROR_LOCALE_STRING,
		TRANSLATION_ERROR_LOCALE_QUEST,
		TRANSLATION_ERROR_ITEM_NAMES,
		TRANSLATION_ERROR_MOB_NAMES,
		TRANSLATION_ERROR_MAX_NUM,
	};

	enum ETranslationTypes
	{
		TRANSLATION_TYPE_NONE,
		TRANSLATION_TYPE_GAME,
		TRANSLATION_TYPE_QUEST,
		TRANSLATION_TYPE_MAX_NUM,
	};

	typedef std::map<const std::string, const std::string> TStringLocalizationMap;

	typedef std::map<DWORD, const std::string> TEntityLocalizationMap;
	typedef std::array<TEntityLocalizationMap, LC_COUNT> TEntityLocalizationMapArray;

protected:
	static TStringLocalizationMap m_stringLocaleMaps[TRANSLATION_TYPE_MAX_NUM][LC_COUNT];

	static TEntityLocalizationMapArray m_itemLocaleMaps;
	static TEntityLocalizationMapArray m_mobLocaleMaps;

public:
	static void Initialize();
	static void InitializeLanguages(const std::string& filePath);

	static void	LoadTextLocalizationFile(ELocales locale, ETranslationTypes type, EErrorTypes errorType, const std::string& filePath);

	static void LoadItemNameLocalizationFile(ELocales locale, const std::string& filePath);
	static void LoadMobNameLocalizationFile(ELocales locale, const std::string& filePath);

	static const char* TranslateText(const std::string& text, BYTE type = TRANSLATION_TYPE_GAME, int locale = LC_DEFAULT);
	static const char* TranslateText(LPCHARACTER ch, BYTE type, const std::string& text);

	static const char* TranslateItemName(DWORD vnum, int locale = LC_DEFAULT);
	static const char* TranslateMobName(DWORD vnum, int locale = LC_DEFAULT);

	static DWORD FindItemVnumByLocalization(const std::string& name, int locale = LC_DEFAULT);
	static DWORD FindMobVnumByLocalization(const std::string& name, int locale = LC_DEFAULT);

protected:
	static void ReadEntityLocalizationFile(TEntityLocalizationMap& localeMap, const std::string& filePath);
};

#define LC_TRANS_TEXT(text_or_ch, type, locale_or_text) (LC::TranslateText(text_or_ch, type, locale_or_text))
#define LC_TEXT_TYPE(text, type, locale) (LC::TranslateText(text, type, locale))
#define LC_ITEM_NAME(vnum, locale) (LC::TranslateItemName(vnum, locale))
#define LC_MOB_NAME(vnum, locale) (LC::TranslateMobName(vnum, locale))

// Backwards compatiblity
#define LC_TEXT(str) str
