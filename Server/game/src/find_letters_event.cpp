#include "stdafx.h"
#include "config.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "packet.h"
#include "item.h"
#include "item_manager.h"
#include "desc.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "banword.h"
#include "questmanager.h"
#include "questlua.h"
#include "db.h"
#include "p2p.h"
#include "find_letters_event.h"

CFindLetters::CFindLetters()
{
	//Initialize();
}

CFindLetters::~CFindLetters()
{
	//Destroy();
}

void CFindLetters::Initialize()
{
	stFindLettersWord = "";
	iEventEndTime = 0;

	if (g_bAuthServer)
	{
		return;
	}

	std::unique_ptr<SQLMsg> pmsg(DBManager::instance().DirectQuery("SELECT word FROM find_letters LIMIT 1;"));
	if (pmsg->Get()->uiNumRows != 1)
	{
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(pmsg->Get()->pSQLResult);
	if (!row[0])
	{
		return;
	}

	stFindLettersWord = row[0];
	sys_log(0, "FindLetters event load word: %s", stFindLettersWord.c_str());

	CheckWord();
}

bool CFindLetters::CheckWord()
{
	if (stFindLettersWord.length() > FIND_LETTERS_MAX_SIZE)
	{
		sys_err("FindLetters: Max length is %d, your word is %d", FIND_LETTERS_MAX_SIZE, stFindLettersWord.length());
		return false;
	}

	int len = 0;
	int lastPos = 0;

	for (int i = 0; i < stFindLettersWord.length(); ++i)
	{
		if (stFindLettersWord[i] == ' ')
		{
			if (len > FIND_LETTERS_MAX_WORD_SIZE)
			{
				sys_err("FindLetters: Max word length is %d, word \"%s\" is %d", FIND_LETTERS_MAX_WORD_SIZE, stFindLettersWord.substr(lastPos, i - lastPos).c_str(), len);
				return false;
			}

			lastPos = i + 1;
			len = 0;
		}
		else if (i == stFindLettersWord.length() - 1)
		{
			if (len + 1 > FIND_LETTERS_MAX_WORD_SIZE)
			{
				sys_err("FindLetters: Max word length is %d, word \"%s\" is %d", FIND_LETTERS_MAX_WORD_SIZE, stFindLettersWord.substr(lastPos, i - lastPos + 1).c_str(), len + 1);
				return false;
			}
			len = 0;
		}
		else
		{
			len++;
		}
	}

	return true;
}

BYTE CFindLetters::GetLetterByPos(int iPos)
{
	if (iPos >= stFindLettersWord.length())
	{
		return 0;
	}

	if (stFindLettersWord[iPos] == ' ')
	{
		return 95;    // 95 is _
	}

	if (islower(stFindLettersWord[iPos]))
	{
		return toupper(stFindLettersWord[iPos]);
	}

	return stFindLettersWord[iPos];
}

struct FindLettersEventPacketFunc
{
	bool bIsEnable;
	FindLettersEventPacketFunc (bool isEnable) : bIsEnable(isEnable) {}

	void operator () (LPDESC d)
	{
		if (!d->GetCharacter())
		{
			return;
		}

		d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "FindLettersIcon %d", bIsEnable);
	}
};

void CFindLetters::InitializeEvent(int iEnable)
{
	const DESC_MANAGER::DESC_SET & descSet = DESC_MANAGER::instance().GetClientSet();
	std::for_each(descSet.begin(), descSet.end(), FindLettersEventPacketFunc(iEnable ? true : false));
}

void CFindLetters::CheckEventEnd()
{
	if (time(0) > iEventEndTime && iEventEndTime != 0)
	{
		quest::CQuestManager::instance().RequestSetEventFlag("enable_find_letters", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("find_letters_event_end_time", 0);
		quest::CQuestManager::instance().RequestSetEventFlag("find_letters_drop", 0);
		iEventEndTime = 0;
	}
}
