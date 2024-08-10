#pragma once

#include "../eterBase/Singleton.h"
#include "../eterlib/IME.h"

class CPythonIME :
	public IIMEEventSink,
	public CIME,
	public CSingleton<CPythonIME>
{
public:
	CPythonIME();
	virtual ~CPythonIME();

	void MoveLeft(bool resetSelection);
	void MoveRight(bool resetSelection);
	void MoveHome();
	void MoveEnd();
	void SetCursorPosition(int iPosition);
	int GetCursorPosition();
	void ResetTextSelection();
	void SetTextSelection();
	TSelectionPos GetTextSelected();
	void Delete();

	void Create(HWND hWnd);

protected:
	virtual bool __IsWritable(wchar_t key);

	virtual void OnTab();
	virtual void OnReturn();
	virtual void OnEscape();
	virtual bool OnUpdateSelection(bool checkIfSelected = false);

	virtual bool OnWM_CHAR( WPARAM wParam, LPARAM lParam );
	virtual void OnUpdate();
	virtual void OnChangeCodePage();
	virtual void OnOpenCandidateList();
	virtual void OnCloseCandidateList();
	virtual void OnOpenReadingWnd();
	virtual void OnCloseReadingWnd();
};
