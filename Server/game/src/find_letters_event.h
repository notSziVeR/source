#ifndef __FIND_LETTERS_H__
#define __FIND_LETTERS_H__

#include "packet.h"

class CFindLetters : public singleton<CFindLetters>
{
public:
	CFindLetters();
	virtual ~CFindLetters();

	void		Initialize();
	void 		CheckEventEnd();
	bool 		CheckWord();

	void 		InitializeEvent(int iEnable);
	void 		InitializeEventEndTime(int iEndTime) { iEventEndTime = iEndTime; }
	void 		FindLettersEventSendInfo(LPCHARACTER pkChar, bool bIsEnable);

	BYTE 		GetLetterByPos(int iPos);

	std::string	GetWord() { return stFindLettersWord; }
private:
	std::string stFindLettersWord;
	int iEventEndTime;
};
#endif
