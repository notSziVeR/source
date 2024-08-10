#include "stdafx.h"
#include "constants.h"
#include "config.h"
#include "questmanager.h"
#include "start_position.h"
#include "packet.h"
#include "buffer_manager.h"
#include "log.h"
#include "char.h"
#include "char_manager.h"
#include "OXEvent.h"
#include "desc.h"
#include "item.h"

bool COXEventManager::Initialize()
{
	m_timedEvent = NULL;
	m_map_char.clear();
	m_map_attender.clear();
	m_vec_quiz.clear();
	s_super_visior.clear();
	m_vec_main_pool.clear();
	m_vec_sub_pool.clear();
	m_vec_categories.clear();

	SetStatus(OXEVENT_FINISH);

	return true;
}

void COXEventManager::Destroy()
{
	CloseEvent();

	m_map_char.clear();
	m_map_attender.clear();
	m_vec_quiz.clear();
	s_super_visior.clear();
	m_vec_main_pool.clear();
	m_vec_sub_pool.clear();
	m_vec_categories.clear();

	SetStatus(OXEVENT_FINISH);
}

OXEventStatus COXEventManager::GetStatus()
{
	BYTE ret = quest::CQuestManager::instance().GetEventFlag("oxevent_status");

	switch (ret)
	{
	case 0 :
		return OXEVENT_FINISH;

	case 1 :
		return OXEVENT_OPEN;

	case 2 :
		return OXEVENT_CLOSE;

	case 3 :
		return OXEVENT_QUIZ;

	default :
		return OXEVENT_ERR;
	}

	return OXEVENT_ERR;
}

void COXEventManager::SetStatus(OXEventStatus status)
{
	BYTE val = 0;

	switch (status)
	{
	case OXEVENT_OPEN :
		val = 1;
		break;

	case OXEVENT_CLOSE :
		val = 2;
		break;

	case OXEVENT_QUIZ :
		val = 3;
		break;

	case OXEVENT_FINISH :
	case OXEVENT_ERR :
	default :
		val = 0;
		break;
	}
	quest::CQuestManager::instance().RequestSetEventFlag("oxevent_status", val);
}

bool COXEventManager::Enter(LPCHARACTER pkChar)
{
	if (GetStatus() == OXEVENT_FINISH)
	{
		sys_log(0, "OXEVENT : map finished. but char enter. %s", pkChar->GetName());
		return false;
	}

	PIXEL_POSITION pos = pkChar->GetXYZ();

	if (pos.x == 896500 && pos.y == 24600)
	{
		return EnterAttender(pkChar);
	}
	else if (pos.x == 896300 && pos.y == 28900)
	{
		return EnterAudience(pkChar);
	}
	else
	{
		sys_log(0, "OXEVENT : wrong pos enter %d %d", pos.x, pos.y);
		return false;
	}

	return false;
}

bool COXEventManager::EnterAttender(LPCHARACTER pkChar)
{
	DWORD pid = pkChar->GetPlayerID();

	m_map_char.insert(std::make_pair(pid, pid));
	m_map_attender.insert(std::make_pair(pid, pid));

	SendCountInfo();
	return true;
}

bool COXEventManager::EnterAudience(LPCHARACTER pkChar)
{
	DWORD pid = pkChar->GetPlayerID();

	m_map_char.insert(std::make_pair(pid, pid));

	SendCountInfo();
	return true;
}

bool COXEventManager::AddQuiz(unsigned char level, const char* pszQuestion, bool answer)
{
	if (m_vec_quiz.size() < (size_t) level + 1)
	{
		m_vec_quiz.resize(level + 1);
	}

	struct tag_Quiz tmpQuiz;

	tmpQuiz.level = level;
	strlcpy(tmpQuiz.Quiz, pszQuestion, sizeof(tmpQuiz.Quiz));
	tmpQuiz.answer = answer;

	m_vec_quiz[level].push_back(tmpQuiz);
	return true;
}

bool COXEventManager::ShowQuizList(LPCHARACTER pkChar)
{
	int c = 0;

	for (size_t i = 0; i < m_vec_quiz.size(); ++i)
	{
		for (size_t j = 0; j < m_vec_quiz[i].size(); ++j, ++c)
		{
			pkChar->ChatPacket(CHAT_TYPE_INFO, "%d %s %s", m_vec_quiz[i][j].level, m_vec_quiz[i][j].Quiz, m_vec_quiz[i][j].answer ? LC_TEXT("TRUE") : LC_TEXT("FALSE"));
		}
	}

	pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Total number of the Quiz: %d"), c);
	return true;
}

void COXEventManager::ClearQuiz()
{
	for (unsigned int i = 0; i < m_vec_quiz.size(); ++i)
	{
		m_vec_quiz[i].clear();
	}

	m_vec_quiz.clear();
}

EVENTINFO(OXEventInfoData)
{
	bool answer;

	OXEventInfoData()
		: answer( false )
	{
	}
};

EVENTFUNC(oxevent_timer)
{
	static BYTE flag = 0;
	OXEventInfoData* info = dynamic_cast<OXEventInfoData*>(event->info);

	if ( info == NULL )
	{
		sys_err( "oxevent_timer> <Factor> Null pointer" );
		return 0;
	}

	switch (flag)
	{
	case 0:
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNoticeMap(LC_TEXT("The result will follow in 10 seconds."), OXEVENT_MAP_INDEX, true);
#else
		SendNoticeMap(LC_TEXT("The result will follow in 10 seconds."), OXEVENT_MAP_INDEX, true);
#endif

		flag++;
		return PASSES_PER_SEC(7);

	// Countdown
	case 1:
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNoticeMap(LC_TEXT("OX_COUNT_DOWN_3"), OXEVENT_MAP_INDEX, true);
#else
		SendNoticeMap(LC_TEXT("OX_COUNT_DOWN_3"), OXEVENT_MAP_INDEX, false);
#endif

		flag++;
		return PASSES_PER_SEC(3);

	case 2:
#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNoticeMap(LC_TEXT("The correct answer is:"), OXEVENT_MAP_INDEX, true);
#else
		SendNoticeMap(LC_TEXT("The correct answer is:"), OXEVENT_MAP_INDEX, true);
#endif

		if (info->answer == true)
		{
			COXEventManager::instance().CheckAnswer(true);
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNoticeMap(LC_TEXT("Yes (O)"), OXEVENT_MAP_INDEX, true);
#else
			SendNoticeMap(LC_TEXT("Yes (O)"), OXEVENT_MAP_INDEX, true);
#endif
		}
		else
		{
			COXEventManager::instance().CheckAnswer(false);
#ifdef __MULTI_LANGUAGE_SYSTEM__
			SendNoticeMap(LC_TEXT("No (X)"), OXEVENT_MAP_INDEX, true);
#else
			SendNoticeMap(LC_TEXT("No (X)"), OXEVENT_MAP_INDEX, true);
#endif
		}

#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNoticeMap(LC_TEXT("In 5 sec. everyone who gave an incorrect answer will be removed."), OXEVENT_MAP_INDEX, true);
#else
		SendNoticeMap(LC_TEXT("In 5 sec. everyone who gave an incorrect answer will be removed."), OXEVENT_MAP_INDEX, true);
#endif

		flag++;
		return PASSES_PER_SEC(5);

	case 3:
		COXEventManager::instance().WarpToAudience();
		COXEventManager::instance().SetStatus(OXEVENT_CLOSE);

#ifdef __MULTI_LANGUAGE_SYSTEM__
		SendNoticeMap(LC_TEXT("Ready for the next question?"), OXEVENT_MAP_INDEX, true);
#else
		SendNoticeMap(LC_TEXT("Ready for the next question?"), OXEVENT_MAP_INDEX, true);
#endif

		flag = 0;
		break;
	}
	return 0;
}

bool COXEventManager::Quiz(unsigned char level, int timelimit)
{
	

	if (m_vec_main_pool.size() == 0) { return false; }

	tag_Quiz e_question;
	if (!GetNextQuestion(e_question))
	{
		return false;
	}

	if (timelimit < 0) { timelimit = 30; }

#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNoticeMap(LC_TEXT("문제 입니다."), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(e_question.Quiz, OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT("If it's correct, then go to O. If it's wrong, go to X."), OXEVENT_MAP_INDEX, true);
#else
	SendNoticeMap(LC_TEXT("문제 입니다."), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(e_question.Quiz, OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT("If it's correct, then go to O. If it's wrong, go to X."), OXEVENT_MAP_INDEX, true);
#endif

	if (m_timedEvent != NULL)
	{
		event_cancel(&m_timedEvent);
	}

	OXEventInfoData* answer = AllocEventInfo<OXEventInfoData>();

	answer->answer = e_question.answer;

	timelimit -= 15;
	m_timedEvent = event_create(oxevent_timer, answer, PASSES_PER_SEC(timelimit));

	SetStatus(OXEVENT_QUIZ);
	return true;
}

bool COXEventManager::Quiz(std::string question, int timelimit)
{
	if (question.size() <= 0) { return false; }
	if (timelimit < 0) { timelimit = 30; }

#ifdef __MULTI_LANGUAGE_SYSTEM__
	SendNoticeMap(LC_TEXT("문제 입니다."), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT(question.c_str()), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT("If it's correct, then go to O. If it's wrong, go to X."), OXEVENT_MAP_INDEX, true);
#else
	SendNoticeMap(LC_TEXT("문제 입니다."), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT(question.c_str()), OXEVENT_MAP_INDEX, true);
	SendNoticeMap(LC_TEXT("If it's correct, then go to O. If it's wrong, go to X."), OXEVENT_MAP_INDEX, true);
#endif

	if (m_timedEvent != NULL)
	{
		event_cancel(&m_timedEvent);
	}

	OXEventInfoData* answer = AllocEventInfo<OXEventInfoData>();
	answer->answer = number(0, 1);

	timelimit -= 15;
	m_timedEvent = event_create(oxevent_timer, answer, PASSES_PER_SEC(timelimit));

	SetStatus(OXEVENT_QUIZ);
	return true;
}

bool COXEventManager::CheckAnswer(bool answer)
{
	if (m_map_attender.size() <= 0) { return true; }

	itertype(m_map_attender) iter = m_map_attender.begin();
	itertype(m_map_attender) iter_tmp;

	m_map_miss.clear();

	int rect[4];
	if (answer != true)
	{
		rect[0] = 892600;
		rect[1] = 22900;
		rect[2] = 896300;
		rect[3] = 26400;
	}
	else
	{
		rect[0] = 896600;
		rect[1] = 22900;
		rect[2] = 900300;
		rect[3] = 26400;
	}

	LPCHARACTER pkChar = NULL;
	PIXEL_POSITION pos;
	for (; iter != m_map_attender.end();)
	{
		pkChar = CHARACTER_MANAGER::instance().FindByPID(iter->second);
		if (pkChar != NULL)
		{
			pos = pkChar->GetXYZ();

			if (pos.x < rect[0] || pos.x > rect[2] || pos.y < rect[1] || pos.y > rect[3])
			{
				pkChar->EffectPacket(SE_FAIL);
				iter_tmp = iter;
				iter++;
				m_map_attender.erase(iter_tmp);
				m_map_miss.insert(std::make_pair(pkChar->GetPlayerID(), pkChar->GetPlayerID()));
				SendCountInfo();
			}
			else
			{
				pkChar->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("Correct!"));
				// pkChar->CreateFly(number(FLY_FIREWORK1, FLY_FIREWORK6), pkChar);
				char chatbuf[256];
				int len = snprintf(chatbuf, sizeof(chatbuf),
								   "%s %u %u", number(0, 1) == 1 ? "cheer1" : "cheer2", (DWORD)pkChar->GetVID(), 0);

				if (len < 0 || len >= (int) sizeof(chatbuf))
				{
					len = sizeof(chatbuf) - 1;
				}

				++len;

				TPacketGCChat pack_chat;
				pack_chat.header = HEADER_GC_CHAT;
				pack_chat.size = sizeof(TPacketGCChat) + len;
				pack_chat.type = CHAT_TYPE_COMMAND;
				pack_chat.id = 0;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM__
				pack_chat.locale = -1;
#endif

				TEMP_BUFFER buf;
				buf.write(&pack_chat, sizeof(TPacketGCChat));
				buf.write(chatbuf, len);

				pkChar->PacketAround(buf.read_peek(), buf.size());
				pkChar->EffectPacket(SE_SUCCESS);

				++iter;
			}
		}
		else
		{
			itertype(m_map_char) err = m_map_char.find(iter->first);
			if (err != m_map_char.end()) { m_map_char.erase(err); }

			itertype(m_map_miss) err2 = m_map_miss.find(iter->first);
			if (err2 != m_map_miss.end()) { m_map_miss.erase(err2); }

			iter_tmp = iter;
			++iter;
			m_map_attender.erase(iter_tmp);
			SendCountInfo();
		}
	}
	return true;
}

void COXEventManager::WarpToAudience()
{
	if (m_map_miss.size() <= 0) { return; }

	itertype(m_map_miss) iter = m_map_miss.begin();
	LPCHARACTER pkChar = NULL;

	for (; iter != m_map_miss.end(); ++iter)
	{
		pkChar = CHARACTER_MANAGER::instance().FindByPID(iter->second);

		if (pkChar != NULL)
		{
			switch ( number(0, 3))
			{
			case 0 :
				pkChar->Show(OXEVENT_MAP_INDEX, 896300, 28900);
				break;
			case 1 :
				pkChar->Show(OXEVENT_MAP_INDEX, 890900, 28100);
				break;
			case 2 :
				pkChar->Show(OXEVENT_MAP_INDEX, 896600, 20500);
				break;
			case 3 :
				pkChar->Show(OXEVENT_MAP_INDEX, 902500, 28100);
				break;
			default :
				pkChar->Show(OXEVENT_MAP_INDEX, 896300, 28900);
				break;
			}
		}
	}

	m_map_miss.clear();
}

bool COXEventManager::CloseEvent()
{
	if (m_timedEvent != NULL)
	{
		event_cancel(&m_timedEvent);
	}

	itertype(m_map_char) iter = m_map_char.begin();

	LPCHARACTER pkChar = NULL;
	for (; iter != m_map_char.end(); ++iter)
	{
		pkChar = CHARACTER_MANAGER::instance().FindByPID(iter->second);

		if (pkChar != NULL)
		{
			pkChar->WarpSet(EMPIRE_START_X(pkChar->GetEmpire()), EMPIRE_START_Y(pkChar->GetEmpire()));
		}
	}

	Initialize();

	return true;
}

bool COXEventManager::LogWinner()
{
	itertype(m_map_attender) iter = m_map_attender.begin();

	for (; iter != m_map_attender.end(); ++iter)
	{
		LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindByPID(iter->second);

		if (pkChar)
		{
			LogManager::instance().CharLog(pkChar, 0, "OXEVENT", "LastManStanding");
		}
	}

	return true;
}

bool COXEventManager::GiveItemToAttender(DWORD dwItemVnum, WORD count, DWORD item_time)
{
	itertype(m_map_attender) iter = m_map_attender.begin();

	for (; iter != m_map_attender.end(); ++iter)
	{
		LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindByPID(iter->second);

		if (pkChar)
		{
			LPITEM item = pkChar->AutoGiveItem(dwItemVnum, count);

			if (item_time)
			{
				item->SetSocket(0, time(0) + item_time);
			}

			LogManager::instance().ItemLog(pkChar->GetPlayerID(), 0, count, dwItemVnum, "OXEVENT_REWARD", "", pkChar->GetDesc()->GetHostName(), dwItemVnum);
		}
	}

	return true;
}

void COXEventManager::SendCountInfo()
{
	for (const auto & pid : s_super_visior)
	{
		LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindByPID(pid);
		if (pkChar)
		{
			pkChar->ChatPacket(CHAT_TYPE_COMMAND, std::string(("UpdateAttendersCount OnlineAttenders ") + std::to_string(m_map_attender.size())).c_str());
			pkChar->ChatPacket(CHAT_TYPE_COMMAND, std::string(("UpdateAttendersCount OnlineObservers ") + std::to_string(m_map_char.size())).c_str());
		}
	}
}

// Pool
void COXEventManager::FlushPool()
{
	// Flushing main poll
	m_vec_main_pool.clear();
	// Flushing subpools
	m_vec_sub_pool.clear();

	// Filling subpools
	for (size_t i = 0; i < m_vec_quiz.size(); ++i)
	{
		auto & sub_cont = (m_vec_sub_pool.emplace(std::piecewise_construct, std::forward_as_tuple(i), std::forward_as_tuple()).first)->second;
		std::copy(m_vec_quiz[i].begin(), m_vec_quiz[i].end(), std::back_inserter(sub_cont));
	}

	BroadcastPoolInfo();
}

bool COXEventManager::AddQuestionToPool(const unsigned char & c_subpool_num, WORD & w_count)
{
	if (c_subpool_num >= m_vec_sub_pool.size())
	{
		sys_err("Unknown pool request %d.", c_subpool_num);
		return false;
	}

	while (w_count)
	{
		if (!m_vec_sub_pool[c_subpool_num].size())
		{
			return false;
		}

		// Dicing element
		auto & e_pool = m_vec_sub_pool[c_subpool_num][number(0, (m_vec_sub_pool[c_subpool_num].size() - 1))];
		// Adding element to main pool
		m_vec_main_pool.push_back(e_pool);
		// Erasing element from subpool
		auto fIt = std::find(m_vec_sub_pool[c_subpool_num].begin(), m_vec_sub_pool[c_subpool_num].end(), e_pool);
		if (fIt != m_vec_sub_pool[c_subpool_num].end())
		{
			m_vec_sub_pool[c_subpool_num].erase(fIt);
		}

		BroadcastPoolInfo(c_subpool_num);
		w_count--;
	}

	return true;
}

bool COXEventManager::GetNextQuestion(tag_Quiz & _e_question)
{
	if (!m_vec_main_pool.size())
	{
		return false;
	}

	_e_question = std::move(m_vec_main_pool.back());
	m_vec_main_pool.pop_back();

	BroadcastPoolInfo(-1, true);
	return true;
}

void COXEventManager::BroadcastPoolInfo(const char & c_subpool_num, const bool & bOnlyMain)
{
	for (const auto & pid : s_super_visior)
	{
		LPCHARACTER pkChar = CHARACTER_MANAGER::instance().FindByPID(pid);
		if (pkChar)
		{
			if (!bOnlyMain)
			{
				// Subpools
				if (c_subpool_num == -1)
				{
					for (const auto & e_subpool : m_vec_sub_pool)
						pkChar->ChatPacket(CHAT_TYPE_COMMAND, "UpdateOXQuestionPool %d %s %d", e_subpool.first, m_vec_categories.find(e_subpool.first) != m_vec_categories.end() ? m_vec_categories[e_subpool.first].c_str() : "EMPTY",
										   e_subpool.second.size());
				}
				else
					pkChar->ChatPacket(CHAT_TYPE_COMMAND, "UpdateOXQuestionPool %d %s %d", c_subpool_num, m_vec_categories.find(c_subpool_num) != m_vec_categories.end() ? m_vec_categories[c_subpool_num].c_str() : "EMPTY",
									   m_vec_sub_pool.find(c_subpool_num) != m_vec_sub_pool.end() ? m_vec_sub_pool[c_subpool_num].size() : 0);
			}

			// Main Pool
			pkChar->ChatPacket(CHAT_TYPE_COMMAND, "UpdateOXQuestionPool -1 MAIN %d", m_vec_main_pool.size());
		}
	}
}

void COXEventManager::PrintCurrentPools(LPCHARACTER ch)
{
	ch->ChatPacket(CHAT_TYPE_INFO, "----- SubPools -----");
	for (const auto & e_subpool : m_vec_sub_pool)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Pool num: %d, Count: %d", e_subpool.first, e_subpool.second.size());
		for (const auto & e_question : e_subpool.second)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "Quesion: %s", e_question.Quiz);
		}
	}

	ch->ChatPacket(CHAT_TYPE_INFO, "----- MainPool -----");
	ch->ChatPacket(CHAT_TYPE_INFO, "Pool num: main, Count: %d", m_vec_main_pool.size());
}

void COXEventManager::AddQuizCategoryName(int && iKey, std::string && sCatName)
{
	m_vec_categories.emplace(std::move(iKey), std::move(sCatName));
}


