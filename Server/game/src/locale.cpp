#include "stdafx.h"

#include <fstream>
#include <rapidxml/rapidxml_utils.hpp>
#include <boost/algorithm/string/compare.hpp>
#include "char.h"
#include "log.h"

#pragma push_macro("max")
#undef max
#include <strasser/csv.hpp>
#pragma pop_macro("max")

LC::TStringLocalizationMap LC::m_stringLocaleMaps[TRANSLATION_TYPE_MAX_NUM][LC_COUNT];

LC::TEntityLocalizationMapArray LC::m_itemLocaleMaps;
LC::TEntityLocalizationMapArray LC::m_mobLocaleMaps;

std::string astLocaleStringNames[LC_COUNT];
std::string astLocaleStringShortNames[LC_COUNT];

void LC::Initialize()
{
	//Clearing if needed
	for (size_t i = 0; i < LC_COUNT; i++)
	{
		astLocaleStringNames[i].clear();
		astLocaleStringShortNames[i].clear();
	}

	//Initialize languages
	InitializeLanguages("locale/germany/languages_proto.xml");

	//Initialize data for every languages
	for (size_t i = 0; i < LC_COUNT; i++)
	{
		std::string stTranslateDir = "locale/lang/" + astLocaleStringShortNames[i];

		LoadTextLocalizationFile((ELocales)i, TRANSLATION_TYPE_GAME, TRANSLATION_ERROR_LOCALE_STRING, stTranslateDir + "/locale_strings.xml");
		LoadTextLocalizationFile((ELocales)i, TRANSLATION_TYPE_QUEST, TRANSLATION_ERROR_LOCALE_QUEST, stTranslateDir + "/locale_quests.xml");
		LoadItemNameLocalizationFile((ELocales)i, stTranslateDir + "/entities/item_names.txt");
		LoadMobNameLocalizationFile((ELocales)i, stTranslateDir + "/entities/mob_names.txt");
	}
}

static inline bool CanParseRecord(MYSQL_ROW& row, int iRowNum)
{
	return (*row && row[iRowNum]);
}

void LC::InitializeLanguages(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, filePath.c_str(), "(Initialize) - Failed to initialize this file!");
		return;
	}

	try
	{
		rapidxml::file<> xmlFile(file);
		rapidxml::xml_document<> xmlDoc;
		try
		{
			xmlDoc.parse<rapidxml::parse_trim_whitespace>(xmlFile.data());
		}
		catch (const std::runtime_error& e)
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, e.what(), "(Initialize) - Runtime rror");
			return;
		}
		catch (const rapidxml::parse_error& e)
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, e.what(), "(Initialize) - Parse error");
			return;
		}

		auto parent = xmlDoc.first_node("language_proto", 0, false);
		if (!parent)
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, "", "(Initialize) - Cannot find language_proto node");
			return;
		}

		int index = 0;
		for (auto category = parent->first_node(); category; category = category->next_sibling())
		{
			if (index >= LC_COUNT) {
				LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, std::to_string(LC_COUNT).c_str(), "(Initialize) - Range is not equal with enum");
				break;
			}

			auto name_attr = category->first_attribute("name", 0, false);
			if (!name_attr)
			{
				LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, "", "(Initialize) - Category without child name are invalid");
				continue;
			}

			astLocaleStringNames[index] = name_attr->value();
			astLocaleStringShortNames[index] = category->value();
			index++;
		}
	}
	catch (const std::exception& e)
	{
		LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_SYSTEM, 0, e.what(), "(Initialize) - An unknown error occurred.");
	}
}

void LC::LoadTextLocalizationFile(ELocales locale, ETranslationTypes type, EErrorTypes errorType, const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LogManager::instance().TranslationErrorLog(errorType, locale, filePath.c_str(), "(LF) - Failed to initialize this file!");
		exit(1);
		return;
	}

	try
	{
		rapidxml::file<> xmlFile(file);
		rapidxml::xml_document<> xmlDoc;
		try
		{
			xmlDoc.parse<rapidxml::parse_trim_whitespace>(xmlFile.data());
		}
		catch (const std::runtime_error& e)
		{
			LogManager::instance().TranslationErrorLog(errorType, locale, e.what(), "(LF) - Runtime error");
			return;
		}
		catch (const rapidxml::parse_error& e)
		{
			char szError[200];
			snprintf(szError, sizeof(szError), "[%s] - [%s]",
				e.what(), e.where<char>());

			LogManager::instance().TranslationErrorLog(errorType, locale, szError, "(LF) - Parse error");
			return;
		}

		auto parent = xmlDoc.first_node("translate_proto", 0, false);
		if (!parent)
		{
			LogManager::instance().TranslationErrorLog(errorType, locale, "", "(LF) - Cannot find translate_proto node");
			return;
		}

		auto& localeMap = m_stringLocaleMaps[type][locale];
		std::string key, value;

		for (auto category = parent->first_node(); category; category = category->next_sibling())
		{
			auto name_attr = category->first_attribute("name", 0, false);
			if (!name_attr)
			{
				LogManager::instance().TranslationErrorLog(errorType, locale, "", "(LF) - Category without child name are invalid");
				continue;
			}

			key = name_attr->value();
			value = category->value();

			localeMap.insert({ key, value });
		}
	}
	catch (const std::exception& e)
	{
		LogManager::instance().TranslationErrorLog(errorType, locale, e.what(), "(LF) - An unknown error occurred.");
	}
}

void LC::ReadEntityLocalizationFile(TEntityLocalizationMap& localeMap, const std::string& filePath)
{
	io::CSVReader<2, io::trim_chars<' ', '\t'>, io::no_quote_escape<'\t'>> csv(filePath);

	csv.read_header(io::ignore_extra_column, "VNUM", "NAME");

	DWORD vnum;
	std::string name;
	while (csv.read_row(vnum, name))
	{
		localeMap.insert({ vnum, name });
	}
}

void LC::LoadItemNameLocalizationFile(ELocales locale, const std::string& filePath)
{
	LC::ReadEntityLocalizationFile(LC::m_itemLocaleMaps[locale], filePath);
}

void LC::LoadMobNameLocalizationFile(ELocales locale, const std::string& filePath)
{
	LC::ReadEntityLocalizationFile(LC::m_mobLocaleMaps[locale], filePath);
}

const char* LC::TranslateText(const std::string& text, BYTE bType, int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	const auto& localeText = m_stringLocaleMaps[bType][locale].find(text);
	if (localeText == m_stringLocaleMaps[bType][locale].end())
	{
		if (locale == LC_DEFAULT)
		{
			LogManager::instance().TranslationErrorLog(bType + 1, LC_DEFAULT, text.c_str(), "Failed to find key inside default strings");
			return text.c_str();
		}
		else
		{
			LogManager::instance().TranslationErrorLog(bType + 1, locale, text.c_str(), "Failed to find key in this locale");
			return LC::TranslateText(text, bType, LC_DEFAULT);
		}
	}

	return localeText->second.c_str();
}

const char* LC::TranslateText(LPCHARACTER ch, BYTE type, const std::string& text)
{
	return TranslateText(text, type, ch ? ch->GetLocale() : LC_DEFAULT);
}

const char* LC::TranslateItemName(DWORD vnum, int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	const auto& localeItemName = LC::m_itemLocaleMaps[locale].find(vnum);
	if (localeItemName == LC::m_itemLocaleMaps[locale].end())
	{
		if (locale == LC_DEFAULT)
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_ITEM_NAMES, LC_DEFAULT, std::to_string(vnum).c_str(), "Failed to find key for item inside default strings");
			return "UNKNOWN";
		}
		else
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_ITEM_NAMES, locale, std::to_string(vnum).c_str(), "Failed to find key for item in this locale");
			return LC::TranslateItemName(vnum, LC_DEFAULT);
		}
	}

	return localeItemName->second.c_str();
}

const char* LC::TranslateMobName(DWORD vnum, int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	const auto& localeMobName = LC::m_mobLocaleMaps[locale].find(vnum);
	if (localeMobName == LC::m_mobLocaleMaps[locale].end())
	{
		if (locale == LC_DEFAULT)
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_MOB_NAMES, LC_DEFAULT, std::to_string(vnum).c_str(), "Failed to find key for mob inside default strings");
			return "UNKNOWN";
		}
		else
		{
			LogManager::instance().TranslationErrorLog(TRANSLATION_ERROR_MOB_NAMES, locale, std::to_string(vnum).c_str(), "Failed to find key for mob in this locale");
			return LC::TranslateMobName(vnum, LC_DEFAULT);
		}
	}

	return localeMobName->second.c_str();
}

DWORD LC::FindItemVnumByLocalization(const std::string& name, int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	for (const auto& entry : LC::m_itemLocaleMaps[locale])
	{
		if (!strncasecmp(name.c_str(), entry.second.c_str(), name.size()))
		{
			return entry.first;
		}
	}

	return 0;
}

DWORD LC::FindMobVnumByLocalization(const std::string& name, int locale)
{
	if (locale < 0 || locale >= LC_COUNT)
	{
		locale = LC_DEFAULT;
	}

	for (const auto& entry : LC::m_mobLocaleMaps[locale])
	{
		if (!strncasecmp(name.c_str(), entry.second.c_str(), name.size()))
		{
			return entry.first;
		}
	}

	return 0;
}
