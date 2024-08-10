#include "StdAfx.h"
#include "PythonIME.h"
#include "AbstractApplication.h"
#include "PythonApplication.h"

CPythonIME::CPythonIME()
	: CIME()
{
	ms_pEvent = this;
}

CPythonIME::~CPythonIME()
{
	Tracen("PythonIME Clear");
}

void CPythonIME::Create(HWND hWnd)
{
	Initialize(hWnd);
}

void CPythonIME::MoveLeft(bool resetSelection)
{
	DecCurPos(resetSelection);
}

void CPythonIME::MoveRight(bool resetSelection)
{
	IncCurPos(resetSelection);
}

void CPythonIME::MoveHome()
{
	ms_curpos = 0;
	OnUpdateSelection();
}

void CPythonIME::MoveEnd()
{
	ms_curpos = ms_lastpos;
	OnUpdateSelection();
}

void CPythonIME::SetCursorPosition(int iPosition)
{
	SetCurPos(iPosition);
}

int CPythonIME::GetCursorPosition()
{
	return ms_curpos;
}

void CPythonIME::ResetTextSelection()
{
	ResetCurSelection();
}

void CPythonIME::SetTextSelection()
{
	SetCurSelection();
}

CPythonIME::TSelectionPos CPythonIME::GetTextSelected()
{
	return GetCurSelection();
}

void CPythonIME::Delete()
{
	DelCurPos();
}

void CPythonIME::OnUpdate()
{
	IAbstractApplication::GetSingleton().RunIMEUpdate();
}

void CPythonIME::OnTab()
{
	IAbstractApplication::GetSingleton().RunIMETabEvent();
}

void CPythonIME::OnReturn()
{
	IAbstractApplication::GetSingleton().RunIMEReturnEvent();
}

void CPythonIME::OnEscape()
{
//	IAbstractApplication::GetSingleton().RunIMEEscapeEvent();
}

bool CPythonIME::OnUpdateSelection(bool checkIfSelected)
{
	if (!CIME::OnUpdateSelection(checkIfSelected))
	{
		return false;
	}

	IAbstractApplication::GetSingleton().RunIMEUpdateSelection();
	return true;
}

bool CPythonIME::__IsWritable(wchar_t key)
{
	if (!CIME::__IsWritable(key))
	{
		return false;
	}

	if (CPythonApplication::Instance().IsPressed(DIK_LCONTROL) || CPythonApplication::Instance().IsPressed(DIK_RCONTROL))
	{
		if (!CPythonApplication::Instance().IsPressed(DIK_LALT) && !CPythonApplication::Instance().IsPressed(DIK_RALT))
		{
			return false;
		}
	}

	return true;
}

bool CPythonIME::OnWM_CHAR( WPARAM wParam, LPARAM lParam )
{
	unsigned char c = unsigned char(wParam & 0xff);

	switch (c)
	{
	case VK_RETURN:
		OnReturn();
		return true;

	case VK_TAB:
		if (ms_bCaptureInput == false)
		{
			return 0;
		}
		OnTab();
		return true;

	case VK_ESCAPE:
		if (ms_bCaptureInput == false)
		{
			return 0;
		}
		OnEscape();
		return true;
	}
	return false;
}

void CPythonIME::OnChangeCodePage()
{
	IAbstractApplication::GetSingleton().RunIMEChangeCodePage();
}

void CPythonIME::OnOpenCandidateList()
{
	IAbstractApplication::GetSingleton().RunIMEOpenCandidateListEvent();
}

void CPythonIME::OnCloseCandidateList()
{
	IAbstractApplication::GetSingleton().RunIMECloseCandidateListEvent();
}

void CPythonIME::OnOpenReadingWnd()
{
	IAbstractApplication::GetSingleton().RunIMEOpenReadingWndEvent();
}

void CPythonIME::OnCloseReadingWnd()
{
	IAbstractApplication::GetSingleton().RunIMECloseReadingWndEvent();
}
