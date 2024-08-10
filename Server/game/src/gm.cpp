#include "stdafx.h"
#include "constants.h"
#include "gm.h"
#include "utils.h"
#include "questmanager.h"
#ifdef __ADMIN_MANAGER__
	#include "AdminManagerController.h"
#endif

extern int test_server;

namespace GM
{
DWORD g_adwAdminConfig[GM_MAX_NUM];

//ADMIN_MANAGER
typedef struct
{
	tAdminInfo Info;
} tGM;

std::map<std::string, tGM> g_map_GM;

void init(DWORD* adwAdminConfig)
{
	memcpy(&g_adwAdminConfig[0], adwAdminConfig, sizeof(g_adwAdminConfig));
}

void insert(const tAdminInfo& rAdminInfo)
{
	sys_log(0, "InsertGMList(account:%s, player:%s, auth:%d)",
			rAdminInfo.m_szAccount,
			rAdminInfo.m_szName,
			rAdminInfo.m_Authority);

	tGM t;

	thecore_memcpy(&t.Info, &rAdminInfo, sizeof(rAdminInfo));

	g_map_GM[rAdminInfo.m_szName] = t;
}

void remove(const char* szName)
{
	g_map_GM.erase(szName);
}

unsigned char get_level(const char* name, const char* account, bool ignore_test_server)
{
	if (!ignore_test_server && test_server) { return GM_IMPLEMENTOR; }

	std::map<std::string, tGM >::iterator it = g_map_GM.find(name);

	if (g_map_GM.end() == it)
	{
		return GM_PLAYER;
	}

	if (account)
	{
		if (strcasecmp(it->second.Info.m_szAccount, account) != 0 && strcmp(it->second.Info.m_szAccount, "[ALL]") != 0)
		{
			sys_err("GM::get_level: account compare failed [real account %s need account %s]", account, it->second.Info.m_szAccount);
			return GM_PLAYER;
		}
	}

	sys_log(0, "GM::GET_LEVEL : FIND ACCOUNT");
	return it->second.Info.m_Authority;
}

void clear()
{
	g_map_GM.clear();
}

bool check_allow(unsigned char bGMLevel, DWORD dwCheckFlag)
{
#ifdef __ADMIN_MANAGER__
	if (IS_SET(dwCheckFlag, GM_ALLOW_DROP_GM_ITEM | GM_ALLOW_EXCHANGE_GM_ITEM_TO_GM | GM_ALLOW_EXCHANGE_GM_ITEM_TO_PLAYER))
	{
		if (quest::CQuestManager::instance().GetEventFlag(EVENT_FLAG_GM_ITEM_TRADE_BLOCK) == 0)
		{
			if (IS_SET(dwCheckFlag, GM_ALLOW_DROP_GM_ITEM))
			{
				REMOVE_BIT(dwCheckFlag, GM_ALLOW_DROP_GM_ITEM);
				SET_BIT(dwCheckFlag, GM_ALLOW_DROP_PLAYER_ITEM);
			}
			if (IS_SET(dwCheckFlag, GM_ALLOW_EXCHANGE_GM_ITEM_TO_GM))
			{
				REMOVE_BIT(dwCheckFlag, GM_ALLOW_EXCHANGE_GM_ITEM_TO_GM);
				SET_BIT(dwCheckFlag, GM_ALLOW_EXCHANGE_PLAYER_ITEM_TO_GM);
			}
			if (IS_SET(dwCheckFlag, GM_ALLOW_EXCHANGE_GM_ITEM_TO_PLAYER))
			{
				REMOVE_BIT(dwCheckFlag, GM_ALLOW_EXCHANGE_GM_ITEM_TO_PLAYER);
				SET_BIT(dwCheckFlag, GM_ALLOW_EXCHANGE_PLAYER_ITEM_TO_PLAYER);
			}
		}
	}
#endif

	return IS_SET(g_adwAdminConfig[bGMLevel], dwCheckFlag);
}

bool check_account_allow(const std::string& stAccountName, DWORD dwCheckFlag)
{
	std::map<std::string, tGM >::iterator it = g_map_GM.begin();

	bool bHasGM = false;
	bool bCheck = false;
	while (it != g_map_GM.end() && !bCheck)
	{
		if (!stAccountName.compare(it->second.Info.m_szAccount))
		{
			bHasGM = true;

			unsigned char bGMLevel = it->second.Info.m_Authority;
			bCheck = check_allow(bGMLevel, dwCheckFlag);
		}

		++it;
	}

	return !bHasGM || bCheck;
}
}
