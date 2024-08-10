#include "stdafx.h"
#include "locale_service.h"
#include "constants.h"
#include "banword.h"
#include "utils.h"
#include "mob_manager.h"
#include "empire_text_convert.h"
#include "config.h"
#include "skill_power.h"

using namespace std;

extern string		g_stQuestDir;
extern set<string> 	g_setQuestObjectDir;

string g_stServiceBasePath = ".";
string g_stServiceMapPath = "data/map";

BYTE PK_PROTECT_LEVEL = 15;

string g_stLocal = "";

bool is_twobyte(const char* str)
{
	return ishan(*str);
}

// Do not change due to checks for SQL injections.
// Only allows alpha-numeric characters.
bool check_name(const char* str)
{
	if (!str || !*str)
	{
		return false;
	}

	size_t length = strlen(str);
	if (length < 2)
	{
		return false;
	}

	for (const char* tmp = str; *tmp; ++tmp)
	{
		if (!isalnum(*tmp))
		{
			return false;
		}
	}

	if (CBanwordManager::instance().CheckString(str, length))
	{
		return false;
	}

	char szTmp[256];
	str_lower(str, szTmp, sizeof(szTmp));

	if (CMobManager::instance().Get(szTmp, false))
	{
		return false;
	}

	return true;
}

void InitializeLocalizationFiles()
{
	LC::Initialize();
}

void LocaleService_LoadEmpireTextConvertTables()
{
	char szFileName[256];

	for (int iEmpire = 1; iEmpire <= 3; ++iEmpire)
	{
		snprintf(szFileName, sizeof(szFileName), "%s/lang%d.cvt", GetBasePath().c_str(), iEmpire);
		sys_log(0, "Load %s", szFileName);

		LoadEmpireTextConvertTable(iEmpire, szFileName);
	}
}

void InitializeSystemPaths()
{
	g_stServiceBasePath = "locale/germany";
	g_stQuestDir = "locale/germany/quest";
	g_stServiceMapPath = "locale/germany/map";

	g_setQuestObjectDir.clear();
	g_setQuestObjectDir.insert("locale/germany/quest/object");
}

void LocaleService_TransferDefaultSetting()
{
	if (!exp_table)
	{
		exp_table = exp_table_common;
	}

	if (!CTableBySkill::instance().Check())
	{
		exit(1);
	}
}

const std::string& GetBasePath()
{
	return g_stServiceBasePath;
}

const std::string& GetMapPath()
{
	return g_stServiceMapPath;
}

const std::string& GetQuestPath()
{
	return g_stQuestDir;
}
