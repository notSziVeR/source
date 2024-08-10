#include "stdafx.h"
#include <fstream>
#include "constants.h"
#include "buffer_manager.h"
#include "packet.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "char.h"
#include "char_manager.h"
#include "questmanager.h"
#include "text_file_loader.h"
#include "lzo_manager.h"
#include "item.h"
#include "config.h"
#include "target.h"
#include "party.h"
#include "locale_service.h"
#include "dungeon.h"
#include "xmas_event.h"
#ifdef __QUEST_RENEWAL__
	#ifdef _QR_MS_
		#include <vector>
		#include <iostream>
		#include <sstream>
	#else
		#include <boost/tokenizer.hpp>
	#endif
#endif

#ifdef __ENABLE_FIND_LETTERS_EVENT__
	#include "find_letters_event.h"
#endif

DWORD g_GoldDropTimeLimitValue = 0;
#ifdef __ENABLE_NEWSTUFF__
	DWORD g_ItemDropTimeLimitValue = 0;
	DWORD g_BoxUseTimeLimitValue = 0;
	DWORD g_BuySellTimeLimitValue = 0;
	bool g_NoDropMetinStone = false;
	bool g_NoMountAtGuildWar = false;
	bool g_NoPotionsOnPVP = false;
#endif
extern bool DropEvent_CharStone_SetValue(const std::string& name, int value);
extern bool DropEvent_RefineBox_SetValue (const std::string& name, int value);

namespace quest
{
using namespace std;

CQuestManager::CQuestManager()
	: m_pSelectedDungeon(NULL), m_dwServerTimerArg(0), m_iRunningEventIndex(0), L(NULL), m_bNoSend (false),
	  m_CurrentRunningState(NULL), m_pCurrentCharacter(NULL), m_pCurrentNPCCharacter(NULL), m_pCurrentPartyMember(NULL),
	  m_pCurrentPC(NULL),  m_iCurrentSkin(0), m_bError(false), m_pOtherPCBlockRootPC(NULL)
{
}

CQuestManager::~CQuestManager()
{
	Destroy();
}

void CQuestManager::Destroy()
{
	if (L)
	{
		lua_close(L);
		L = NULL;
	}
}

bool CQuestManager::Initialize()
{
	if (g_bAuthServer)
	{
		return true;
	}

	if (!InitializeLua())
	{
		return false;
	}

	m_pSelectedDungeon = NULL;

	m_mapEventName.insert(TEventNameMap::value_type("click", QUEST_CLICK_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("kill", QUEST_KILL_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("timer", QUEST_TIMER_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("levelup", QUEST_LEVELUP_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("login", QUEST_LOGIN_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("logout", QUEST_LOGOUT_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("button", QUEST_BUTTON_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("info", QUEST_INFO_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("chat", QUEST_CHAT_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("in", QUEST_ATTR_IN_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("out", QUEST_ATTR_OUT_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("use", QUEST_ITEM_USE_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("server_timer", QUEST_SERVER_TIMER_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("enter", QUEST_ENTER_STATE_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("leave", QUEST_LEAVE_STATE_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("letter", QUEST_LETTER_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("take", QUEST_ITEM_TAKE_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("target", QUEST_TARGET_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("party_kill", QUEST_PARTY_KILL_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("unmount", QUEST_UNMOUNT_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("pick", QUEST_ITEM_PICK_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("sig_use", QUEST_SIG_USE_EVENT));
	m_mapEventName.insert(TEventNameMap::value_type("item_informer", QUEST_ITEM_INFORMER_EVENT));
#ifdef ENABLE_QUEST_DIE_EVENT
	m_mapEventName.insert(TEventNameMap::value_type("die", QUEST_DIE_EVENT));
#endif

	m_bNoSend = false;

	m_iCurrentSkin = QUEST_SKIN_NORMAL;

	{
		ifstream inf((g_stQuestDir + "/questnpc.txt").c_str());
		int line = 0;

		if (!inf.is_open())
		{
			sys_err( "QUEST Cannot open 'questnpc.txt'");
		}
		else
		{
			sys_log(0, "QUEST can open 'questnpc.txt' (%s)", g_stQuestDir.c_str() );
		}

		while (1)
		{
			unsigned int vnum;

			inf >> vnum;

			line++;

			if (inf.fail())
			{
				break;
			}

			string s;
			getline(inf, s);
			unsigned int li = 0, ri = s.size() - 1;
			while (li < s.size() && isspace(s[li])) { li++; }
			while (ri > 0 && isspace(s[ri])) { ri--; }

			if (ri < li)
			{
				sys_err("QUEST questnpc.txt:%d:npc name error", line);
				continue;
			}

			s = s.substr(li, ri - li + 1);

			int	n = 0;
			str_to_number(n, s.c_str());
			if (n)
			{
				continue;
			}

			//cout << '-' << s << '-' << endl;
			if ( test_server )
			{
				sys_log(0, "QUEST reading script of %s(%d)", s.c_str(), vnum);
			}
			m_mapNPC[vnum].Set(vnum, s);
			m_mapNPCNameID[s] = vnum;
		}

		// notarget quest
		m_mapNPC[0].Set(0, "notarget");
	}

	SetEventFlag("guild_withdraw_delay", 1);
	SetEventFlag("guild_disband_delay", 1);
#ifdef __QUEST_RENEWAL__
	ReadQuestCategoryToDict();
#endif
	return true;
}

unsigned int CQuestManager::FindNPCIDByName(const string& name)
{
	map<string, unsigned int>::iterator it = m_mapNPCNameID.find(name);
	return it != m_mapNPCNameID.end() ? it->second : 0;
}

void CQuestManager::SelectItem(unsigned int pc, unsigned int selection)
{
	PC* pPC = GetPC(pc);
	if (pPC && pPC->IsRunning() && pPC->GetRunningQuestState()->suspend_state == SUSPEND_STATE_SELECT_ITEM)
	{
		pPC->SetSendDoneFlag();
		pPC->GetRunningQuestState()->args = 1;
		lua_pushnumber(pPC->GetRunningQuestState()->co, selection);

		if (!RunState(*pPC->GetRunningQuestState()))
		{
			CloseState(*pPC->GetRunningQuestState());
			pPC->EndRunning();
		}
	}
}

void CQuestManager::Confirm(unsigned int pc, EQuestConfirmType confirm, unsigned int pc2)
{
	PC* pPC = GetPC(pc);

	if (!pPC->IsRunning())
	{
		sys_err("no quest running for pc, cannot process input : %u", pc);
		return;
	}

	if (pPC->GetRunningQuestState()->suspend_state != SUSPEND_STATE_CONFIRM)
	{
		sys_err("not wait for a confirm : %u %d", pc, pPC->GetRunningQuestState()->suspend_state);
		return;
	}

	if (pc2 && !pPC->IsConfirmWait(pc2))
	{
		sys_err("not wait for a confirm : %u %d", pc, pPC->GetRunningQuestState()->suspend_state);
		return;
	}

	pPC->ClearConfirmWait();

	pPC->SetSendDoneFlag();

	pPC->GetRunningQuestState()->args = 1;
	lua_pushnumber(pPC->GetRunningQuestState()->co, confirm);

	AddScript("[END_CONFIRM_WAIT]");
	SetSkinStyle(QUEST_SKIN_NOWINDOW);
	SendScript();

	if (!RunState(*pPC->GetRunningQuestState()))
	{
		CloseState(*pPC->GetRunningQuestState());
		pPC->EndRunning();
	}

}

int CQuestManager::ReadQuestCategoryFile(WORD q_index)
{

	ifstream inf((g_stQuestDir + "/questcategory.txt").c_str());
	int line = 0;
	int c_qi = 99;

	if (!inf.is_open())
	{
		sys_err( "QUEST Cannot open 'questcategory.txt'");
	}
	else
	{
		sys_log(0, "QUEST can open 'questcategory.txt' (%s)", g_stQuestDir.c_str() );
	}

	while (1)
	{

		string qn = CQuestManager::instance().GetQuestNameByIndex(q_index);

		unsigned int category_num;

		//enum
		//{
		//	MAIN_QUEST,		//0
		//	SUB_QUEST,		//1
		//	COLLECT_QUEST,	//2
		//	LEVELUP_QUEST,	//3
		//	SCROLL_QUEST,	//4
		//	SYSTEM_QUEST,	//5
		//};

		inf >> category_num;

		line++;

		if (inf.fail())
		{
			break;
		}

		string s;
		getline(inf, s);
		unsigned int li = 0, ri = s.size() - 1;
		while (li < s.size() && isspace(s[li])) { li++; }
		while (ri > 0 && isspace(s[ri])) { ri--; }

		if (ri < li)
		{
			sys_err("QUEST questcategory.txt:%d:npc name error", line);
			continue;
		}

		s = s.substr(li, ri - li + 1);

		int	n = 0;
		str_to_number(n, s.c_str());
		if (n)
		{
			continue;
		}

		//cout << '-' << s << '-' << endl;
		if ( test_server )
		{
			sys_log(0, "QUEST reading script of %s(%d)", s.c_str(), category_num);
		}

		if (qn == s)
		{
			c_qi = category_num;
			break;
		}
	}

	// notarget quest
	//m_mapNPC[0].Set(0, "notarget");



	return c_qi;
}

#ifdef __QUEST_RENEWAL__
int CQuestManager::GetQuestCategoryByQuestIndex(WORD q_index)
{
	if (QuestCategoryIndexMap.find(q_index) != QuestCategoryIndexMap.end())
	{
		return QuestCategoryIndexMap[q_index];
	}
	else
	{
		return 0;    /* DEFAULT_QUEST_CATEGORY */
	}
}

void CQuestManager::ReadQuestCategoryToDict()
{
	if (!QuestCategoryIndexMap.empty())
	{
		QuestCategoryIndexMap.clear();
	}

	ifstream inf((g_stQuestDir + "/questcategory.txt").c_str());

	if (!inf.is_open())
	{
		sys_err("QUEST Cannot open 'questcategory.txt'");
		return;
	}

	string lineFromFile;
	while (getline(inf, lineFromFile))
	{
		if (lineFromFile.empty())
		{
			continue;
		}

#ifdef _QR_MS_
		std::stringstream strstr(lineFromFile);
		std::istream_iterator<std::string> it(strstr);
		std::istream_iterator<std::string> end;
		std::vector<std::string> data(it, end);

		std::ostream_iterator<std::string> oit(std::cout);
		std::copy(data.begin(), data.end(), oit);
#else
		boost::tokenizer<boost::escaped_list_separator<char>> token(lineFromFile, boost::escaped_list_separator<char>('\\', '\t', '\"'));
		vector<string> data(token.begin(), token.end());
#endif

		int category_num = atoi(data[0].c_str());
		string quest_name = data[1];

		unsigned int quest_index = CQuestManager::instance().GetQuestIndexByName(quest_name);

		if (test_server)
		{
			sys_log(0, "QUEST_CATEGORY_LINE: %s => %s, %s", lineFromFile.c_str(), data[0].c_str(), quest_name.c_str());
		}

		if (quest_index != 0)
		{
			QuestCategoryIndexMap[quest_index] = category_num;
		}
		else
		{
			sys_err("QUEST coult not find QuestIndex for name Quest: %s(%d)", quest_name.c_str(), category_num);
		}
	}
}
#endif

void CQuestManager::Input(unsigned int pc, const char* msg)
{
	PC* pPC = GetPC(pc);
	if (!pPC)
	{
		sys_err("no pc! : %u", pc);
		return;
	}

	if (!pPC->IsRunning())
	{
		sys_err("no quest running for pc, cannot process input : %u", pc);
		return;
	}

	if (pPC->GetRunningQuestState()->suspend_state != SUSPEND_STATE_INPUT)
	{
		sys_err("not wait for a input : %u %d", pc, pPC->GetRunningQuestState()->suspend_state);
		return;
	}

	pPC->SetSendDoneFlag();

	pPC->GetRunningQuestState()->args = 1;
	lua_pushstring(pPC->GetRunningQuestState()->co, msg);

	if (!RunState(*pPC->GetRunningQuestState()))
	{
		CloseState(*pPC->GetRunningQuestState());
		pPC->EndRunning();
	}
}

void CQuestManager::Select(unsigned int pc, unsigned int selection)
{
	PC* pPC;

	if ((pPC = GetPC(pc)) && pPC->IsRunning() && pPC->GetRunningQuestState()->suspend_state == SUSPEND_STATE_SELECT)
	{
		pPC->SetSendDoneFlag();

		if (!pPC->GetRunningQuestState()->chat_scripts.empty())
		{



			QuestState& old_qs = *pPC->GetRunningQuestState();
			CloseState(old_qs);

			if (selection >= pPC->GetRunningQuestState()->chat_scripts.size())
			{
				pPC->SetSendDoneFlag();
				GotoEndState(old_qs);
				pPC->EndRunning();
			}
			else
			{
				AArgScript* pas = pPC->GetRunningQuestState()->chat_scripts[selection];
				ExecuteQuestScript(*pPC, pas->quest_index, pas->state_index, pas->script.GetCode(), pas->script.GetSize());
			}
		}
		else
		{
			// on default
			pPC->GetRunningQuestState()->args = 1;
			lua_pushnumber(pPC->GetRunningQuestState()->co, selection + 1);

			if (!RunState(*pPC->GetRunningQuestState()))
			{
				CloseState(*pPC->GetRunningQuestState());
				pPC->EndRunning();
			}
		}
	}
	else
	{
		sys_err("wrong QUEST_SELECT request! : %d", pc);
	}
}

void CQuestManager::Resume(unsigned int pc)
{
	PC * pPC;

	if ((pPC = GetPC(pc)) && pPC->IsRunning() && pPC->GetRunningQuestState()->suspend_state == SUSPEND_STATE_PAUSE)
	{
		pPC->SetSendDoneFlag();
		pPC->GetRunningQuestState()->args = 0;

		if (!RunState(*pPC->GetRunningQuestState()))
		{
			CloseState(*pPC->GetRunningQuestState());
			pPC->EndRunning();
		}
	}
	else
	{
		sys_err("wrong QUEST_WAIT request! : %d", pc);
	}
}

void CQuestManager::EnterState(DWORD pc, DWORD quest_index, int state)
{
	PC* pPC;
	if ((pPC = GetPC(pc)))
	{
		if (!CheckQuestLoaded(pPC))
		{
			return;
		}

		m_mapNPC[QUEST_NO_NPC].OnEnterState(*pPC, quest_index, state);
	}
	else
	{
		sys_err("QUEST no such pc id : %d", pc);
	}
}

void CQuestManager::LeaveState(DWORD pc, DWORD quest_index, int state)
{
	PC* pPC;
	if ((pPC = GetPC(pc)))
	{
		if (!CheckQuestLoaded(pPC))
		{
			return;
		}

		m_mapNPC[QUEST_NO_NPC].OnLeaveState(*pPC, quest_index, state);
	}
	else
	{
		sys_err("QUEST no such pc id : %d", pc);
	}
}

void CQuestManager::Letter(DWORD pc, DWORD quest_index, int state)
{
	PreserveState([this, pc, quest_index, state]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnLetter(*pPC, quest_index, state);
		}
		else
		{
			sys_err("QUEST no such pc id : %lu", pc);
		}

		return true;
	});
}

void CQuestManager::LogoutPC(LPCHARACTER ch)
{
	PC * pPC = GetPC(ch->GetPlayerID());

	if (pPC && pPC->IsRunning())
	{
		CloseState(*pPC->GetRunningQuestState());
		pPC->CancelRunning();
	}


	Logout(ch->GetPlayerID());

	if (ch == m_pCurrentCharacter)
	{
		m_pCurrentCharacter = NULL;
		m_pCurrentPC = NULL;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
//
// Quest Event
//
///////////////////////////////////////////////////////////////////////////////////////////

void CQuestManager::Login(unsigned int pc, const char* c_pszQuest)
{
	PreserveState([this, pc, c_pszQuest]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnLogin(*pPC, c_pszQuest);
		}
		else
		{
			sys_err("QUEST no such pc id : %d", pc);
		}

		return true;
	});
}

void CQuestManager::Logout(unsigned int pc)
{
	PreserveState([this, pc]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnLogout(*pPC);
		}
		else
		{
			sys_err("QUEST no such pc id : %d", pc);
		}

		return true;
	});
}

#define ENABLE_PARTYKILL
struct FuncPartyKill
{
	quest::NPC& questNPC;
	FuncPartyKill(NPC& npc) : questNPC(npc) {}

	void operator() (LPCHARACTER ch) const
	{
		auto pPC = quest::CQuestManager::instance().GetPC(ch->GetPlayerID());
		if (pPC && pPC->GetID() && pPC->IsLoaded())
		{
			questNPC.OnPartyKill(*pPC);
		}
	}
};

void CQuestManager::Kill(unsigned int pc, unsigned int npc)
{
	PreserveState([this, pc, npc]() -> bool
	{
		PC * pPC = GetPC(pc);
		if (pPC)
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			if (npc != QUEST_NO_NPC)
			{
				m_mapNPC[npc].OnKill(*pPC);
			}

			m_mapNPC[QUEST_NO_NPC].OnKill(*pPC);

			LPCHARACTER ch = GetCurrentCharacterPtr();
			LPPARTY pParty = ch->GetParty();

			if (pParty)
			{
				if (npc != QUEST_NO_NPC)
				{
					FuncPartyKill f(m_mapNPC[npc]);
					pParty->ForEachOnlineMember(f);
				}

				FuncPartyKill f(m_mapNPC[QUEST_NO_NPC]);
				pParty->ForEachOnlineMember(f);
			}
			else
			{
				if (npc != QUEST_NO_NPC)
				{
					m_mapNPC[npc].OnPartyKill(*pPC);
				}

				m_mapNPC[QUEST_NO_NPC].OnPartyKill(*pPC);
			}
		}
		else
		{
			sys_err("QUEST: no such pc id : %d", pc);
		}

		return true;
	});
}

#ifdef ENABLE_QUEST_DIE_EVENT
void CQuestManager::Die(unsigned int pc, unsigned int npc)
{
	PC * pPC;

	sys_log(0, "CQuestManager::Kill QUEST_DIE_EVENT (pc=%d, npc=%d)", pc, npc);

	if ((pPC = GetPC(pc)))
	{
		if (!CheckQuestLoaded(pPC))
		{
			return;
		}

		m_mapNPC[QUEST_NO_NPC].OnDie(*pPC);

	}
	else
	{
		sys_err("QUEST: no such pc id : %d", pc);
	}
}
#endif

bool CQuestManager::ServerTimer(unsigned int npc, unsigned int arg)
{
	SetServerTimerArg(arg);
	sys_log(0, "XXX ServerTimer Call NPC %p", GetPCForce(0));
	m_pCurrentPC = GetPCForce(0);
	m_pCurrentCharacter = NULL;
	m_pSelectedDungeon = NULL;
	return m_mapNPC[npc].OnServerTimer(*m_pCurrentPC);
}
bool CQuestManager::PreserveState(const std::function<bool()>& f)
{
	auto cur_pCurrentPC = m_pCurrentPC;
	auto cur_pCurrentCharacter = m_pCurrentCharacter;
	auto cur_pSelectedDungeon = m_pSelectedDungeon;

	bool ret = f();

	m_pCurrentPC = cur_pCurrentPC;
	m_pCurrentCharacter = cur_pCurrentCharacter;
	m_pSelectedDungeon = cur_pSelectedDungeon;

	return ret;
}

bool CQuestManager::Timer(unsigned int pc, unsigned int npc)
{
	return PreserveState([this, pc, npc]() -> bool
	{
		PC* pPC = GetPC(pc);

		if (pPC)
		{
			if (CheckQuestLoaded(pPC))
			{
				return m_mapNPC[npc].OnTimer(*pPC);
			}
		}
		else
		{
			sys_err("QUEST TIMER_EVENT no such pc id : %d", pc);
		}

		return false;
	});
}

void CQuestManager::LevelUp(unsigned int pc)
{
	PreserveState([this, pc]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnLevelUp(*pPC);
		}
		else
		{
			sys_err("QUEST LEVELUP_EVENT no such pc id : %d", pc);
		}

		return true;
	});
}

void CQuestManager::AttrIn(unsigned int pc, LPCHARACTER ch, int attr)
{
	PreserveState([this, pc, ch, attr]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			m_pCurrentPartyMember = ch;
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[attr + QUEST_ATTR_NPC_START].OnAttrIn(*pPC);
		}
		else
		{
			sys_err("QUEST no such pc id : %d", pc);
		}

		return true;
	});
}

void CQuestManager::AttrOut(unsigned int pc, LPCHARACTER ch, int attr)
{
	PreserveState([this, pc, ch, attr]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			m_pCurrentPartyMember = ch;
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[attr + QUEST_ATTR_NPC_START].OnAttrOut(*pPC);
		}
		else
		{
			sys_err("QUEST no such pc id : %d", pc);
		}

		return true;
	});
}

bool CQuestManager::Target(unsigned int pc, DWORD dwQuestIndex, const char* c_pszTargetName, const char* c_pszVerb)
{
	return PreserveState([this, pc, dwQuestIndex, c_pszTargetName, c_pszVerb]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			bool bRet;
			return m_mapNPC[QUEST_NO_NPC].OnTarget(*pPC, dwQuestIndex, c_pszTargetName, c_pszVerb, bRet);
		}

		return false;
	});
}

void CQuestManager::QuestInfo(unsigned int pc, unsigned int quest_index)
{
	PreserveState([this, pc, quest_index]() -> bool
	{
		PC* pPC = GetPC(pc);
		if (pPC)
		{
			if (!CheckQuestLoaded(pPC))
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);

				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
				}

				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnInfo(*pPC, quest_index);
		}
		else
		{
			sys_err("QUEST INFO_EVENT no such pc id : %d", pc);
		}

		return true;
	});
}

void CQuestManager::QuestButton(unsigned int pc, unsigned int quest_index)
{
	PreserveState([this, pc, quest_index]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
				}
				return false;
			}
			m_mapNPC[QUEST_NO_NPC].OnButton(*pPC, quest_index);
		}
		else
		{
			sys_err("QUEST CLICK_EVENT no such pc id : %d", pc);
		}

		return true;
	});
}

bool CQuestManager::TakeItem(unsigned int pc, unsigned int npc, LPITEM item)
{
	return PreserveState([this, pc, npc, item]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
				}
				return false;
			}

			SetCurrentItem(item);
			return m_mapNPC[npc].OnTakeItem(*pPC);
		}
		else
		{
			sys_err("QUEST USE_ITEM_EVENT no such pc id : %d", pc);
			return false;
		}
	});
}

bool CQuestManager::UseItem(unsigned int pc, LPITEM item, bool bReceiveAll)
{
	return PreserveState([this, pc, item, bReceiveAll]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
				}
				return false;
			}

			SetCurrentItem(item);
			return m_mapNPC[item->GetVnum()].OnUseItem(*pPC, bReceiveAll);
		}
		else
		{
			sys_err("QUEST USE_ITEM_EVENT no such pc id : %d", pc);
			return false;
		}
	});
}

bool CQuestManager::SIGUse(unsigned int pc, DWORD sig_vnum, LPITEM item, bool bReceiveAll)
{
	return PreserveState([this, pc, sig_vnum, item, bReceiveAll]() -> bool
	{
		PC* pPC;
		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);
				if (ch)
				{
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
				}
				return false;
			}

			SetCurrentItem(item);
			return m_mapNPC[sig_vnum].OnSIGUse(*pPC, bReceiveAll);
		}
		else
		{
			sys_err("QUEST USE_ITEM_EVENT no such pc id : %d", pc);
			return false;
		}
	});
}

bool CQuestManager::GiveItemToPC(unsigned int pc, LPCHARACTER pkChr)
{
	if (!pkChr->IsPC())
	{
		return false;
	}

	return PreserveState([this, pc, pkChr]() -> bool
	{
		PC* pPC = GetPC(pc);

		if (pPC)
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			TargetInfo* pInfo = CTargetManager::instance().GetTargetInfo(pc, TARGET_TYPE_VID, pkChr->GetVID());

			if (pInfo)
			{
				bool bRet;

				if (m_mapNPC[QUEST_NO_NPC].OnTarget(*pPC, pInfo->dwQuestIndex, pInfo->szTargetName, "click", bRet))
				{
					return true;
				}
			}
		}

		return false;
	});
}

bool CQuestManager::Click(unsigned int pc, LPCHARACTER pkChrTarget)
{
	return PreserveState([this, pc, pkChrTarget]() -> bool
	{
		PC* pPC = GetPC(pc);

		if (!pPC)
		{
			sys_err("QUEST CLICK_EVENT no such pc id : %d", pc);
			return false;
		}

		if (!CheckQuestLoaded(pPC))
		{
			LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);

			if (ch)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
			}

			return false;
		}

		TargetInfo* pInfo = CTargetManager::instance().GetTargetInfo(pc, TARGET_TYPE_VID, pkChrTarget->GetVID());
		if (test_server)
		{
			sys_log(0, "CQuestManager::Click(pid = %u, npc_name=%s) - target_info(%p)", pc, pkChrTarget->GetName(), pInfo);
		}

		if (pInfo)
		{
			bool bRet;
			if (m_mapNPC[QUEST_NO_NPC].OnTarget(*pPC, pInfo->dwQuestIndex, pInfo->szTargetName, "click", bRet))
			{
				return bRet;
			}
		}

		DWORD dwCurrentNPCRace = pkChrTarget->GetRaceNum();

		if (pkChrTarget->IsNPC())
		{
			auto it = m_mapNPC.find(dwCurrentNPCRace);

			if (it == m_mapNPC.end())
			{
				sys_log(0, "CQuestManager::Click(pid = %u, target_npc_name=%s) - NOT EXIST NPC RACE VNUM[%lu]",
						pc,
						pkChrTarget->GetName(),
						dwCurrentNPCRace);
				return false;
			}
			quest::NPC* myNPC = &it->second;

			if (myNPC->HasChat())
			{
				if (test_server)
				{
					sys_log(0, "CQuestManager::Click->OnChat");
				}

				if (!myNPC->OnChat(*pPC))
				{
					if (test_server)
					{
						sys_log(0, "CQuestManager::Click->OnChat Failed");
					}

					return myNPC->OnClick(*pPC);
				}

				return true;
			}
			else
			{
				return myNPC->OnClick(*pPC);
			}
		}
		return false;
	});
}

void CQuestManager::Unmount(unsigned int pc)
{
	PreserveState([this, pc]() -> bool
	{
		PC* pPC;

		if ((pPC = GetPC(pc)))
		{
			if (!CheckQuestLoaded(pPC))
			{
				return false;
			}

			m_mapNPC[QUEST_NO_NPC].OnUnmount(*pPC);
		}
		else
		{
			sys_err("QUEST no such pc id : %d", pc);
		}

		return true;
	});
}

void CQuestManager::ItemInformer(unsigned int pc, unsigned int vnum)
{
	PreserveState([this, pc, vnum]() -> bool
	{
		PC* pPC;
		pPC = GetPC(pc);

		if (!pPC)
			return false;

		m_mapNPC[QUEST_NO_NPC].OnItemInformer(*pPC, vnum);

		return true;
	});
}
///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////
void CQuestManager::LoadStartQuest(const string& quest_name, unsigned int idx)
{
	for (itertype(g_setQuestObjectDir) it = g_setQuestObjectDir.begin(); it != g_setQuestObjectDir.end(); ++it)
	{
		const string& stQuestObjectDir = *it;
		string full_name = stQuestObjectDir + "/begin_condition/" + quest_name;
		ifstream inf(full_name.c_str());

		if (inf.is_open())
		{
			sys_log(0, "QUEST loading begin condition for %s", quest_name.c_str());

			istreambuf_iterator<char> ib(inf), ie;
			copy(ib, ie, back_inserter(m_hmQuestStartScript[idx]));
		}
	}
}

bool CQuestManager::CanStartQuest(unsigned int quest_index, const PC& pc)
{
	return CanStartQuest(quest_index);
}

bool CQuestManager::CanStartQuest(unsigned int quest_index)
{
	THashMapQuestStartScript::iterator it;

	if ((it = m_hmQuestStartScript.find(quest_index)) == m_hmQuestStartScript.end())
	{
		return true;
	}
	else
	{
		int x = lua_gettop(L);
		lua_dobuffer(L, &(it->second[0]), it->second.size(), "StartScript");
		int bStart = lua_toboolean(L, -1);
		lua_settop(L, x);
		return bStart != 0;
	}
}

bool CQuestManager::CanEndQuestAtState(const string& quest_name, const string& state_name)
{
	return false;
}

void CQuestManager::DisconnectPC(LPCHARACTER ch)
{
	m_mapPC.erase(ch->GetPlayerID());
}

PC * CQuestManager::GetPCForce(unsigned int pc)
{
	PCMap::iterator it;

	if ((it = m_mapPC.find(pc)) == m_mapPC.end())
	{
		PC * pPC = &m_mapPC[pc];
		pPC->SetID(pc);
		return pPC;
	}

	return &it->second;
}

PC * CQuestManager::GetPC(unsigned int pc)
{
	PCMap::iterator it;

	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(pc);

	if (!pkChr)
	{
		return NULL;
	}

	m_pCurrentPC = GetPCForce(pc);
	m_pCurrentCharacter = pkChr;
	m_pSelectedDungeon = NULL;
	return (m_pCurrentPC);
}

void CQuestManager::ClearScript()
{
	m_strScript.clear();
	m_iCurrentSkin = QUEST_SKIN_NORMAL;
}

void CQuestManager::AddScript(const string& str)
{
	m_strScript += str;
}

void CQuestManager::SendScript()
{
	if (m_bNoSend || !GetCurrentCharacterPtr() || !GetCurrentCharacterPtr()->GetDesc()) // @fixme174 (ch check)
	{
		m_bNoSend = false;
		ClearScript();
		return;
	}

	if (m_strScript == "[DONE]" || m_strScript == "[NEXT]")
	{
		if (m_pCurrentPC && !m_pCurrentPC->GetAndResetDoneFlag() && m_strScript == "[DONE]" && m_iCurrentSkin == QUEST_SKIN_NORMAL && !IsError())
		{
			ClearScript();
			return;
		}
		m_iCurrentSkin = QUEST_SKIN_NOWINDOW;
	}

	//sys_log(0, "Send Quest Script to %s", GetCurrentCharacterPtr()->GetName());
	//send -_-!
	struct ::packet_script packet_script;

	packet_script.header = HEADER_GC_SCRIPT;
	packet_script.skin = m_iCurrentSkin;
	packet_script.src_size = m_strScript.size();
	packet_script.size = packet_script.src_size + sizeof(struct packet_script);
#ifdef ENABLE_QUEST_CATEGORY
	packet_script.quest_flag = 0;


	if (m_bQuestInfoFlag == 1)
	{
		packet_script.quest_flag = 1;
	}
#endif

	TEMP_BUFFER buf;
	buf.write(&packet_script, sizeof(struct packet_script));
	buf.write(&m_strScript[0], m_strScript.size());

	GetCurrentCharacterPtr()->GetDesc()->Packet(buf.read_peek(), buf.size());

	if (test_server)
	{
		sys_log(0, "m_strScript %s size %d", m_strScript.c_str(), buf.size());
	}
#ifdef ENABLE_QUEST_CATEGORY
	sys_log(0, "SendScript=====================On Quest flag %d", packet_script.quest_flag);
#endif

	ClearScript();
	m_bQuestInfoFlag = 0;
}

const char* CQuestManager::GetQuestStateName(const string& quest_name, const int state_index)
{
	int x = lua_gettop(L);
	lua_getglobal(L, quest_name.c_str());
	if (lua_isnil(L, -1))
	{
		sys_err("QUEST wrong quest state file %s.%d", quest_name.c_str(), state_index);
		lua_settop(L, x);
		return "";
	}
	lua_pushnumber(L, state_index);
	lua_gettable(L, -2);

	const char* str = lua_tostring(L, -1);
	lua_settop(L, x);
	return str;
}

int CQuestManager::GetQuestStateIndex(const string& quest_name, const string& state_name)
{
	int x = lua_gettop(L);
	lua_getglobal(L, quest_name.c_str());
	if (lua_isnil(L, -1))
	{
		sys_err("QUEST wrong quest state file %s.%s", quest_name.c_str(), state_name.c_str()  );
		lua_settop(L, x);
		return 0;
	}
	lua_pushstring(L, state_name.c_str());
	lua_gettable(L, -2);

	int v = (int)rint(lua_tonumber(L, -1));
	lua_settop(L, x);
	if ( test_server )
	{
		sys_log( 0, "[QUESTMANAGER] GetQuestStateIndex x(%d) v(%d) %s %s", v, x, quest_name.c_str(), state_name.c_str() );
	}
	return v;
}

void CQuestManager::SetSkinStyle(int iStyle)
{
	if (iStyle < 0 || iStyle >= QUEST_SKIN_COUNT)
	{
		m_iCurrentSkin = QUEST_SKIN_NORMAL;
	}
	else
	{
		m_iCurrentSkin = iStyle;
	}
}

unsigned int CQuestManager::LoadTimerScript(const string& name)
{
	map<string, unsigned int>::iterator it;
	if ((it = m_mapTimerID.find(name)) != m_mapTimerID.end())
	{
		return it->second;
	}
	else
	{
		unsigned int new_id = UINT_MAX - m_mapTimerID.size();

		m_mapNPC[new_id].Set(new_id, name);
		m_mapTimerID.insert(make_pair(name, new_id));

		return new_id;
	}
}

unsigned int CQuestManager::GetCurrentNPCRace()
{
	return GetCurrentNPCCharacterPtr() ? GetCurrentNPCCharacterPtr()->GetRaceNum() : 0;
}

LPITEM CQuestManager::GetCurrentItem()
{
	return GetCurrentCharacterPtr() ? GetCurrentCharacterPtr()->GetQuestItemPtr() : NULL;
}

void CQuestManager::ClearCurrentItem()
{
	if (GetCurrentCharacterPtr())
	{
		GetCurrentCharacterPtr()->ClearQuestItemPtr();
	}
}

void CQuestManager::SetCurrentItem(LPITEM item)
{
	if (GetCurrentCharacterPtr())
	{
		GetCurrentCharacterPtr()->SetQuestItemPtr(item);
	}
}

LPCHARACTER CQuestManager::GetCurrentNPCCharacterPtr()
{
	return GetCurrentCharacterPtr() ? GetCurrentCharacterPtr()->GetQuestNPC() : NULL;
}

const string & CQuestManager::GetCurrentQuestName()
{
	return GetCurrentPC()->GetCurrentQuestName();
}

LPDUNGEON CQuestManager::GetCurrentDungeon()
{
	LPCHARACTER ch = GetCurrentCharacterPtr();

	if (!ch)
	{
		if (m_pSelectedDungeon)
		{
			return m_pSelectedDungeon;
		}
		return NULL;
	}

	return ch->GetDungeonForce();
}

void CQuestManager::RegisterQuest(const string & stQuestName, unsigned int idx)
{
	assert(idx > 0);

	itertype(m_hmQuestName) it;

	if ((it = m_hmQuestName.find(stQuestName)) != m_hmQuestName.end())
	{
		return;
	}

	m_hmQuestName.insert(make_pair(stQuestName, idx));
	LoadStartQuest(stQuestName, idx);
	m_mapQuestNameByIndex.insert(make_pair(idx, stQuestName));

	sys_log(0, "QUEST: Register %4u %s", idx, stQuestName.c_str());
}

unsigned int CQuestManager::GetQuestIndexByName(const string& name)
{
	THashMapQuestName::iterator it = m_hmQuestName.find(name);

	if (it == m_hmQuestName.end())
	{
		return 0;    // RESERVED
	}

	return it->second;
}

const string & CQuestManager::GetQuestNameByIndex(unsigned int idx)
{
	itertype(m_mapQuestNameByIndex) it;

	if ((it = m_mapQuestNameByIndex.find(idx)) == m_mapQuestNameByIndex.end())
	{
		sys_err("cannot find quest name by index %u", idx);
		assert(!"cannot find quest name by index");

		static std::string st = "";
		return st;
	}

	return it->second;
}

void CQuestManager::SendEventFlagList(LPCHARACTER ch)
{
	itertype(m_mapEventFlag) it;
	for (it = m_mapEventFlag.begin(); it != m_mapEventFlag.end(); ++it)
	{
		const string& flagname = it->first;
		int value = it->second;

		if (!test_server && value == 1 && flagname == "valentine_drop")
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s %d prob 800", flagname.c_str(), value);
		}
		else if (!test_server && value == 1 && flagname == "newyear_wonso")
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s %d prob 500", flagname.c_str(), value);
		}
		else if (!test_server && value == 1 && flagname == "newyear_fire")
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s %d prob 1000", flagname.c_str(), value);
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "%s %d", flagname.c_str(), value);
		}
	}
}

void CQuestManager::RequestSetEventFlag(const string& name, int value)
{
	TPacketSetEventFlag p;
	strlcpy(p.szFlagName, name.c_str(), sizeof(p.szFlagName));
	p.lValue = value;
	db_clientdesc->DBPacket(HEADER_GD_SET_EVENT_FLAG, 0, &p, sizeof(TPacketSetEventFlag));
}

void CQuestManager::SetEventFlag(const string& name, int value)
{
	static const char*	DROPEVENT_CHARTONE_NAME		= "drop_char_stone";
	static const int	DROPEVENT_CHARTONE_NAME_LEN = strlen(DROPEVENT_CHARTONE_NAME);

	int prev_value = m_mapEventFlag[name];

	sys_log(0, "QUEST eventflag %s %d prev_value %d", name.c_str(), value, m_mapEventFlag[name]);
	m_mapEventFlag[name] = value;

	if (name == "mob_item")
	{
		CHARACTER_MANAGER::instance().SetMobItemRate(value);
	}
	else if (name == "mob_dam")
	{
		CHARACTER_MANAGER::instance().SetMobDamageRate(value);
	}
	else if (name == "mob_gold")
	{
		CHARACTER_MANAGER::instance().SetMobGoldAmountRate(value);
	}
	else if (name == "mob_gold_pct")
	{
		CHARACTER_MANAGER::instance().SetMobGoldDropRate(value);
	}
	else if (name == "user_dam")
	{
		CHARACTER_MANAGER::instance().SetUserDamageRate(value);
	}
	else if (name == "user_dam_buyer")
	{
		CHARACTER_MANAGER::instance().SetUserDamageRatePremium(value);
	}
	else if (name == "mob_exp")
	{
		CHARACTER_MANAGER::instance().SetMobExpRate(value);
	}
	else if (name == "mob_item_buyer")
	{
		CHARACTER_MANAGER::instance().SetMobItemRatePremium(value);
	}
	else if (name == "mob_exp_buyer")
	{
		CHARACTER_MANAGER::instance().SetMobExpRatePremium(value);
	}
	else if (name == "mob_gold_buyer")
	{
		CHARACTER_MANAGER::instance().SetMobGoldAmountRatePremium(value);
	}
	else if (name == "mob_gold_pct_buyer")
	{
		CHARACTER_MANAGER::instance().SetMobGoldDropRatePremium(value);
	}
	else if (name == "crcdisconnect")
	{
		DESC_MANAGER::instance().SetDisconnectInvalidCRCMode(value != 0);
	}
	else if (!name.compare(0, 5, "xmas_"))
	{
		xmas::ProcessEventFlag(name, prev_value, value);
	}
	else if (name == "newyear_boom")
	{
		const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

		for (itertype(c_ref_set) it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
		{
			LPCHARACTER ch = (*it)->GetCharacter();

			if (!ch)
			{
				continue;
			}

			ch->ChatPacket(CHAT_TYPE_COMMAND, "newyear_boom %d", value);
		}
	}
	else if ( name == "eclipse" )
	{
		std::string mode("");

		if ( value == 1 )
		{
			mode = "dark";
		}
		else
		{
			mode = "light";
		}

		const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

		for (itertype(c_ref_set) it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
		{
			LPCHARACTER ch = (*it)->GetCharacter();
			if (!ch)
			{
				continue;
			}

			ch->ChatPacket(CHAT_TYPE_COMMAND, "DayMode %s", mode.c_str());
		}
	}
	else if (name == "day")
	{
		const DESC_MANAGER::DESC_SET & c_ref_set = DESC_MANAGER::instance().GetClientSet();

		for (itertype(c_ref_set) it = c_ref_set.begin(); it != c_ref_set.end(); ++it)
		{
			LPCHARACTER ch = (*it)->GetCharacter();
			if (!ch)
			{
				continue;
			}
			if (value)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "DayMode dark");
			}
			else
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "DayMode light");
			}
		}

		if (value && !prev_value)
		{

			struct SNPCSellFireworkPosition
			{
				long lMapIndex;
				int x;
				int y;
			} positions[] =
			{
				{	1,	615,	618 },
				{	3,	500,	625 },
				{	21,	598,	665 },
				{	23,	476,	360 },
				{	41,	318,	629 },
				{	43,	478,	375 },
				{	0,	0,	0   },
			};

			SNPCSellFireworkPosition* p = positions;
			while (p->lMapIndex)
			{
				if (map_allow_find(p->lMapIndex))
				{
					PIXEL_POSITION posBase;
					if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(p->lMapIndex, posBase))
					{
						sys_err("cannot get map base position %d", p->lMapIndex);
						++p;
						continue;
					}

					CHARACTER_MANAGER::instance().SpawnMob(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM, p->lMapIndex, posBase.x + p->x * 100, posBase.y + p->y * 100, 0, false, -1);
				}
				p++;
			}
		}
		else if (!value && prev_value)
		{

			CharacterVectorInteractor i;

			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(xmas::MOB_XMAS_FIRWORK_SELLER_VNUM, i))
			{
				CharacterVectorInteractor::iterator it = i.begin();

				while (it != i.end())
				{
					M2_DESTROY_CHARACTER(*it++);
				}
			}
		}
	}
	else if (name == "pre_event_hc")
	{
		const DWORD EventNPC = 20090;

		struct SEventNPCPosition
		{
			long lMapIndex;
			int x;
			int y;
		} positions[] =
		{
			{ 3, 588, 617 },
			{ 23, 397, 250 },
			{ 43, 567, 426 },
			{ 0, 0, 0 },
		};

		if (value && !prev_value)
		{
			SEventNPCPosition* pPosition = positions;

			while (pPosition->lMapIndex)
			{
				if (map_allow_find(pPosition->lMapIndex))
				{
					PIXEL_POSITION pos;

					if (!SECTREE_MANAGER::instance().GetMapBasePositionByMapIndex(pPosition->lMapIndex, pos))
					{
						sys_err("cannot get map base position %d", pPosition->lMapIndex);
						++pPosition;
						continue;
					}

					CHARACTER_MANAGER::instance().SpawnMob(EventNPC, pPosition->lMapIndex, pos.x + pPosition->x * 100, pos.y + pPosition->y * 100, 0, false, -1);
				}
				pPosition++;
			}
		}
		else if (!value && prev_value)
		{
			CharacterVectorInteractor i;

			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(EventNPC, i))
			{
				CharacterVectorInteractor::iterator it = i.begin();

				while (it != i.end())
				{
					LPCHARACTER ch = *it++;

					switch (ch->GetMapIndex())
					{
					case 3:
					case 23:
					case 43:
						M2_DESTROY_CHARACTER(ch);
						break;
					}
				}
			}
		}
	}
	else if (name.compare(0, DROPEVENT_CHARTONE_NAME_LEN, DROPEVENT_CHARTONE_NAME) == 0)
	{
		DropEvent_CharStone_SetValue(name, value);
	}
	else if (name.compare(0, strlen("refine_box"), "refine_box") == 0)
	{
		DropEvent_RefineBox_SetValue(name, value);
	}
	else if (name == "gold_drop_limit_time")
	{
		g_GoldDropTimeLimitValue = value * 1000;
	}
#ifdef __ENABLE_NEWSTUFF__
	else if (name == "item_drop_limit_time")
	{
		g_ItemDropTimeLimitValue = value * 1000;
	}
	else if (name == "box_use_limit_time")
	{
		g_BoxUseTimeLimitValue = value * 1000;
	}
	else if (name == "buysell_limit_time")
	{
		g_BuySellTimeLimitValue = value * 1000;
	}
	else if (name == "no_drop_metin_stone")
	{
		g_NoDropMetinStone = !!value;
	}
	else if (name == "no_mount_at_guild_war")
	{
		g_NoMountAtGuildWar = !!value;
	}
	else if (name == "no_potions_on_pvp")
	{
		g_NoPotionsOnPVP = !!value;
	}
#endif
#ifdef __ENABLE_FIND_LETTERS_EVENT__
	else if (name == "enable_find_letters")
	{
		CFindLetters::instance().InitializeEvent(value);
	}
	else if (name == "find_letters_event_end_time")
	{
		CFindLetters::instance().InitializeEventEndTime(value);
	}
#endif

	else if (name == "new_xmas_event")
	{
		static DWORD new_santa = 20126;
		if (value != 0)
		{
			CharacterVectorInteractor i;
			bool map1_santa_exist = false;
			bool map21_santa_exist = false;
			bool map41_santa_exist = false;

			if (CHARACTER_MANAGER::instance().GetCharactersByRaceNum(new_santa, i))
			{
				CharacterVectorInteractor::iterator it = i.begin();

				while (it != i.end())
				{
					LPCHARACTER tch = *(it++);

					if (tch->GetMapIndex() == 1)
					{
						map1_santa_exist = true;
					}
					else if (tch->GetMapIndex() == 21)
					{
						map21_santa_exist = true;
					}
					else if (tch->GetMapIndex() == 41)
					{
						map41_santa_exist = true;
					}
				}
			}

			if (map_allow_find(1) && !map1_santa_exist)
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(1);
				CHARACTER_MANAGER::instance().SpawnMob(new_santa, 1, pkSectreeMap->m_setting.iBaseX + 60800, pkSectreeMap->m_setting.iBaseY + 61700, 0, false, 90, true);
			}
			if (map_allow_find(21) && !map21_santa_exist)
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(21);
				CHARACTER_MANAGER::instance().SpawnMob(new_santa, 21, pkSectreeMap->m_setting.iBaseX + 59600, pkSectreeMap->m_setting.iBaseY + 61000, 0, false, 110, true);
			}
			if (map_allow_find(41) && !map41_santa_exist)
			{
				LPSECTREE_MAP pkSectreeMap = SECTREE_MANAGER::instance().GetMap(41);
				CHARACTER_MANAGER::instance().SpawnMob(new_santa, 41, pkSectreeMap->m_setting.iBaseX + 35700, pkSectreeMap->m_setting.iBaseY + 74300, 0, false, 140, true);
			}
		}
		else
		{
			CharacterVectorInteractor i;
			CHARACTER_MANAGER::instance().GetCharactersByRaceNum(new_santa, i);

			for (CharacterVectorInteractor::iterator it = i.begin(); it != i.end(); it++)
			{
				M2_DESTROY_CHARACTER(*it);
			}
		}
	}
}

int	CQuestManager::GetEventFlag(const string& name)
{
	map<string, int>::iterator it = m_mapEventFlag.find(name);

	if (it == m_mapEventFlag.end())
	{
		return 0;
	}

	return it->second;
}

void CQuestManager::BroadcastEventFlagOnLogin(LPCHARACTER ch)
{
	int iEventFlagValue;

	if ((iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("xmas_snow")))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "xmas_snow %d", iEventFlagValue);
	}

	if ((iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("xmas_boom")))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "xmas_boom %d", iEventFlagValue);
	}

	if ((iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("xmas_tree")))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "xmas_tree %d", iEventFlagValue);
	}

	if ((iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("day")))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "DayMode dark");
	}

	if ((iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("newyear_boom")))
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "newyear_boom %d", iEventFlagValue);
	}

	if ( (iEventFlagValue = quest::CQuestManager::instance().GetEventFlag("eclipse")) )
	{
		std::string mode;

		if ( iEventFlagValue == 1 ) { mode = "dark"; }
		else { mode = "light"; }

		ch->ChatPacket(CHAT_TYPE_COMMAND, "DayMode %s", mode.c_str());
	}
}

void CQuestManager::Reload()
{
	lua_close(L);
	m_mapNPC.clear();
	m_mapNPCNameID.clear();
	m_hmQuestName.clear();
	m_mapTimerID.clear();
	m_hmQuestStartScript.clear();
	m_mapEventName.clear();
	L = NULL;
	Initialize();

	for (itertype(m_registeredNPCVnum) it = m_registeredNPCVnum.begin(); it != m_registeredNPCVnum.end(); ++it)
	{
		char buf[256];
		DWORD dwVnum = *it;
		snprintf(buf, sizeof(buf), "%u", dwVnum);
		m_mapNPC[dwVnum].Set(dwVnum, buf);
	}
}

bool CQuestManager::ExecuteQuestScript(PC& pc, DWORD quest_index, const int state, const char* code, const int code_size, vector<AArgScript*>* pChatScripts, bool bUseCache)
{
	return ExecuteQuestScript(pc, CQuestManager::instance().GetQuestNameByIndex(quest_index), state, code, code_size, pChatScripts, bUseCache);
}

bool CQuestManager::ExecuteQuestScript(PC& pc, const string& quest_name, const int state, const char* code, const int code_size, vector<AArgScript*>* pChatScripts, bool bUseCache)
{

	QuestState qs = CQuestManager::instance().OpenState(quest_name, state);
	if (pChatScripts)
	{
		qs.chat_scripts.swap(*pChatScripts);
	}


	if (bUseCache)
	{
		lua_getglobal(qs.co, "__codecache");
		// stack : __codecache
		lua_pushnumber(qs.co, (long)code);
		// stack : __codecache (codeptr)
		lua_rawget(qs.co, -2);
		// stack : __codecache (compiled-code)
		if (lua_isnil(qs.co, -1))
		{
			// cache miss

			// load code to lua,
			// save it to cache
			// and only function remain in stack
			lua_pop(qs.co, 1);
			// stack : __codecache
			luaL_loadbuffer(qs.co, code, code_size, quest_name.c_str());
			// stack : __codecache (compiled-code)
			lua_pushnumber(qs.co, (long)code);
			// stack : __codecache (compiled-code) (codeptr)
			lua_pushvalue(qs.co, -2);
			// stack : __codecache (compiled-code) (codeptr) (compiled_code)
			lua_rawset(qs.co, -4);
			// stack : __codecache (compiled-code)
			lua_remove(qs.co, -2);
			// stack : (compiled-code)
		}
		else
		{
			// cache hit
			lua_remove(qs.co, -2);
			// stack : (compiled-code)
		}
	}
	else
	{
		luaL_loadbuffer(qs.co, code, code_size, quest_name.c_str());
	}


	pc.SetQuest(quest_name, qs);


	QuestState& rqs = *pc.GetRunningQuestState();
	if (!CQuestManager::instance().RunState(rqs))
	{
		CQuestManager::instance().CloseState(rqs);
		pc.EndRunning();
		return false;
	}
	return true;
}

void CQuestManager::RegisterNPCVnum(DWORD dwVnum)
{
	if (m_registeredNPCVnum.find(dwVnum) != m_registeredNPCVnum.end())
	{
		return;
	}

	m_registeredNPCVnum.insert(dwVnum);

	char buf[256];
	DIR* dir;

	for (itertype(g_setQuestObjectDir) it = g_setQuestObjectDir.begin(); it != g_setQuestObjectDir.end(); ++it)
	{
		const string& stQuestObjectDir = *it;
		snprintf(buf, sizeof(buf), "%s/%u", stQuestObjectDir.c_str(), dwVnum);
		sys_log(0, "%s", buf);

		if ((dir = opendir(buf)))
		{
			closedir(dir);
			snprintf(buf, sizeof(buf), "%u", dwVnum);
			sys_log(0, "%s", buf);

			m_mapNPC[dwVnum].Set(dwVnum, buf);
		}
	}
}

void CQuestManager::WriteRunningStateToSyserr()
{
	const char * state_name = GetQuestStateName(GetCurrentQuestName(), GetCurrentState()->st);

	string event_index_name = "";
	for (itertype(m_mapEventName) it = m_mapEventName.begin(); it != m_mapEventName.end(); ++it)
	{
		if (it->second == m_iRunningEventIndex)
		{
			event_index_name = it->first;
			break;
		}
	}

	sys_err("LUA_ERROR: quest %s.%s %s", GetCurrentQuestName().c_str(), state_name, event_index_name.c_str() );
	if (GetCurrentCharacterPtr() && test_server)
	{
		GetCurrentCharacterPtr()->ChatPacket(CHAT_TYPE_PARTY, "LUA_ERROR: quest %s.%s %s", GetCurrentQuestName().c_str(), state_name, event_index_name.c_str() );
	}
}

#ifndef __WIN32__
void CQuestManager::QuestError(const char* func, int line, const char* fmt, ...)
{
	char szMsg[4096];
	va_list args;

	va_start(args, fmt);
	vsnprintf(szMsg, sizeof(szMsg), fmt, args);
	va_end(args);

	_sys_err(func, line, "%s", szMsg);
	if (test_server)
	{
		LPCHARACTER ch = GetCurrentCharacterPtr();
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_PARTY, "error occurred on [%s:%d]", func, line);
			ch->ChatPacket(CHAT_TYPE_PARTY, "%s", szMsg);
		}
	}
}
#else
void CQuestManager::QuestError(const char* func, int line, const char* fmt, ...)
{
	char szMsg[4096];
	va_list args;

	va_start(args, fmt);
	vsnprintf(szMsg, sizeof(szMsg), fmt, args);
	va_end(args);

	_sys_err(func, line, "%s", szMsg);
	if (test_server)
	{
		LPCHARACTER ch = GetCurrentCharacterPtr();
		if (ch)
		{
			ch->ChatPacket(CHAT_TYPE_PARTY, "error occurred on [%s:%d]", func, line);
			ch->ChatPacket(CHAT_TYPE_PARTY, "%s", szMsg);
		}
	}
}
#endif

void CQuestManager::AddServerTimer(const std::string& name, DWORD arg, LPEVENT event)
{
	sys_log(0, "XXX AddServerTimer %s %d %p", name.c_str(), arg, get_pointer(event));
	if (m_mapServerTimer.find(make_pair(name, arg)) != m_mapServerTimer.end())
	{
		sys_err("already registered server timer name:%s arg:%u", name.c_str(), arg);
		return;
	}
	m_mapServerTimer.insert(make_pair(make_pair(name, arg), event));
}

void CQuestManager::ClearServerTimerNotCancel(const std::string& name, DWORD arg)
{
	m_mapServerTimer.erase(make_pair(name, arg));
}

void CQuestManager::ClearServerTimer(const std::string& name, DWORD arg)
{
	itertype(m_mapServerTimer) it = m_mapServerTimer.find(make_pair(name, arg));
	if (it != m_mapServerTimer.end())
	{
		LPEVENT event = it->second;
		event_cancel(&event);
		m_mapServerTimer.erase(it);
	}
}

void CQuestManager::CancelServerTimers(DWORD arg)
{
	itertype(m_mapServerTimer) it = m_mapServerTimer.begin();
	for ( ; it != m_mapServerTimer.end();)
	{
		if (it->first.second == arg)
		{
			LPEVENT event = it->second;
			event_cancel(&event);
			m_mapServerTimer.erase(it++);
		}
		else
		{
			++it;
		}
	}
}

void CQuestManager::SetServerTimerArg(DWORD dwArg)
{
	m_dwServerTimerArg = dwArg;
}

DWORD CQuestManager::GetServerTimerArg()
{
	return m_dwServerTimerArg;
}

void CQuestManager::SelectDungeon(LPDUNGEON pDungeon)
{
	m_pSelectedDungeon = pDungeon;
}

bool CQuestManager::PickupItem(unsigned int pc, LPITEM item)
{
	if (test_server)
	{
		sys_log( 0, "questmanager::PickupItem Start : itemVnum : %d PC : %d", item->GetOriginalVnum(), pc);
	}
	PC* pPC;
	if ((pPC = GetPC(pc)))
	{
		if (!CheckQuestLoaded(pPC))
		{
			LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(pc);
			if (ch)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Your request is loading. Please wait."));
			}
			return false;
		}
		// call script
		SetCurrentItem(item);

		return m_mapNPC[item->GetVnum()].OnPickupItem(*pPC);
	}
	else
	{
		sys_err("QUEST PICK_ITEM_EVENT no such pc id : %d", pc);
		return false;
	}
}
void CQuestManager::BeginOtherPCBlock(DWORD pid)
{
	LPCHARACTER ch = GetCurrentCharacterPtr();
	if (NULL == ch)
	{
		sys_err("NULL?");
		return;
	}
	/*
	# 1. current pid = pid0 <- It will be m_pOtherPCBlockRootPC.
	begin_other_pc_block(pid1)
		# 2. current pid = pid1
		begin_other_pc_block(pid2)
			# 3. current_pid = pid2
		end_other_pc_block()
	end_other_pc_block()
	*/
	// when begin_other_pc_block(pid1)
	if (m_vecPCStack.empty())
	{
		m_pOtherPCBlockRootPC = GetCurrentPC();
	}
	m_vecPCStack.push_back(GetCurrentCharacterPtr()->GetPlayerID());
	GetPC(pid);
}

void CQuestManager::EndOtherPCBlock()
{
	if (m_vecPCStack.size() == 0)
	{
		sys_err("m_vecPCStack is alread empty. CurrentQuest{Name(%s), State(%s)}", GetCurrentQuestName().c_str(), GetCurrentState()->_title.c_str());
		return;
	}
	DWORD pc = m_vecPCStack.back();
	m_vecPCStack.pop_back();
	GetPC(pc);

	if (m_vecPCStack.empty())
	{
		m_pOtherPCBlockRootPC = NULL;
	}
}

bool CQuestManager::IsInOtherPCBlock()
{
	return !m_vecPCStack.empty();
}

PC*	CQuestManager::GetOtherPCBlockRootPC()
{
	return m_pOtherPCBlockRootPC;
}
}

