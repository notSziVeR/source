#include "StdAfx.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"
#include "PythonGridSlotWindow.h"

bool PyTuple_GetWindow(PyObject* poArgs, int pos, UI::CWindow ** ppRetWindow)
{
	int iHandle;
	if (!PyTuple_GetInteger(poArgs, pos, &iHandle))
	{
		return false;
	}
	if (!iHandle)
	{
		return false;
	}

	*ppRetWindow = (UI::CWindow*)iHandle;
	return true;
}

PyObject * wndMgrGetAspect(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("f", UI::CWindowManager::Instance().GetAspect());
}

PyObject * wndMgrOnceIgnoreMouseLeftButtonUpEvent(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindowManager::Instance().OnceIgnoreMouseLeftButtonUpEvent();
	return Py_BuildNone();
}

PyObject * wndMgrSetMouseHandler(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetMouseHandler(poHandler);
	return Py_BuildNone();
}

/////////////////////////////////////////////////////////////////////////////////////////////////
///// Register /////
// Window
PyObject * wndMgrRegister(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindowManager& kWndMgr = UI::CWindowManager::Instance();
	UI::CWindow * pWindow = kWndMgr.RegisterWindow(po, szLayer);
	if (!pWindow)
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWindow);
}

// SlotWindow
PyObject * wndMgrRegisterSlotWindow(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterSlotWindow(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// GridSlotWindow
PyObject * wndMgrRegisterGridSlotWindow(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterGridSlotWindow(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// TextLine
PyObject * wndMgrRegisterTextLine(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterTextLine(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// MarkBox
PyObject * wndMgrRegisterMarkBox(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterMarkBox(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// ImageBox
PyObject * wndMgrRegisterImageBox(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterImageBox(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// ExpandedImageBox
PyObject * wndMgrRegisterExpandedImageBox(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterExpandedImageBox(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// AniImageBox
PyObject * wndMgrRegisterAniImageBox(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterAniImageBox(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// RegisterButton
PyObject * wndMgrRegisterButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterButton(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// RadioButton
PyObject * wndMgrRegisterRadioButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterRadioButton(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// ToggleButton
PyObject * wndMgrRegisterToggleButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterToggleButton(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// DragButton
PyObject * wndMgrRegisterDragButton(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterDragButton(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// Box
PyObject * wndMgrRegisterBox(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBox(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// Bar
PyObject * wndMgrRegisterBar(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBar(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// Line
PyObject * wndMgrRegisterLine(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterLine(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// Slot
PyObject * wndMgrRegisterBar3D(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterBar3D(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

// NumberLine
PyObject * wndMgrRegisterNumberLine(PyObject * poSelf, PyObject * poArgs)
{
	PyObject * po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char * szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow * pWindow = UI::CWindowManager::Instance().RegisterNumberLine(po, szLayer);
	return Py_BuildValue("i", pWindow);
}
///// Register /////
/////////////////////////////////////////////////////////////////////////////////////////////////

PyObject * wndMgrDestroy(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().DestroyWindow(pWin);
	return Py_BuildNone();
}

PyObject * wndMgrIsFocus(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWindow == UI::CWindowManager::Instance().GetActivateWindow());
}

PyObject * wndMgrSetFocus(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().ActivateWindow(pWin);
	return Py_BuildNone();
}

PyObject * wndMgrKillFocus(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		if (pWin == UI::CWindowManager::Instance().GetActivateWindow())
		{
			UI::CWindowManager::Instance().DeactivateWindow();
		}
	}
	else
	{
		UI::CWindowManager::Instance().DeactivateWindow();
	}

	return Py_BuildNone();
}

PyObject * wndMgrLock(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().LockWindow(pWin);
	return Py_BuildNone();
}

PyObject * wndMgrUnlock(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindowManager::Instance().UnlockWindow();
	return Py_BuildNone();
}

PyObject * wndMgrSetName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	char * szName;
	if (!PyTuple_GetString(poArgs, 1, &szName))
	{
		return Py_BuildException();
	}

	pWin->SetName(szName);
	return Py_BuildNone();
}

PyObject * wndMgrSetTop(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetTop(pWin);
	return Py_BuildNone();
}

PyObject* wndMgrIsTop(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}

	bool isTop = UI::CWindowManager::Instance().IsTop(pWin);
	return Py_BuildValue("b", isTop);
}

PyObject * wndMgrShow(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	pWin->Show();
	return Py_BuildNone();
}

PyObject * wndMgrHide(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	pWin->Hide();
	return Py_BuildNone();
}

PyObject * wndMgrIsShow(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWin->IsShow());
}

PyObject * wndMgrIsRTL(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWin->IsFlag(UI::CWindow::FLAG_RTL));
}

#ifdef ENABLE_MOUSEWHEEL_EVENT
PyObject* wndMgrSetScrollable(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (pWin)
	{
		pWin->SetScrollable();
	}

	return Py_BuildNone();
}
#endif

PyObject * wndMgrSetScreenSize(PyObject * poSelf, PyObject * poArgs)
{
	int width;
	if (!PyTuple_GetInteger(poArgs, 0, &width))
	{
		return Py_BuildException();
	}
	int height;
	if (!PyTuple_GetInteger(poArgs, 1, &height))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetScreenSize(width, height);
	return Py_BuildNone();
}

PyObject * wndMgrSetParent(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	UI::CWindow * pParentWin;
	if (!PyTuple_GetWindow(poArgs, 1, &pParentWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetParent(pWin, pParentWin);
	return Py_BuildNone();
}

PyObject * wndMgrSetPickAlways(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetPickAlways(pWin);
	return Py_BuildNone();
}

PyObject * wndMgrSetWndSize(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int width;
	if (!PyTuple_GetInteger(poArgs, 1, &width))
	{
		return Py_BuildException();
	}
	int height;
	if (!PyTuple_GetInteger(poArgs, 2, &height))
	{
		return Py_BuildException();
	}

	pWin->SetSize(width, height);
	return Py_BuildNone();
}

PyObject * wndMgrSetWndPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int x;
	if (!PyTuple_GetInteger(poArgs, 1, &x))
	{
		return Py_BuildException();
	}
	int y;
	if (!PyTuple_GetInteger(poArgs, 2, &y))
	{
		return Py_BuildException();
	}

	pWin->SetPosition(x, y);
	return Py_BuildNone();
}

PyObject * wndMgrGetName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("s", pWin->GetName());
}

PyObject * wndMgrGetWndWidth(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWin->GetWidth());
}

PyObject * wndMgrGetWndHeight(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWin->GetHeight());
}

PyObject * wndMgrGetWndLocalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	long lx, ly;
	pWin->GetPosition(&lx, &ly);

	return Py_BuildValue("ii", lx, ly);
}

PyObject * wndMgrGetWndGlobalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	RECT & rRect = pWindow->GetRect();
	return Py_BuildValue("ii", rRect.left, rRect.top);
}

PyObject * wndMgrGetWindowRect(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	RECT & rRect = pWin->GetRect();
	return Py_BuildValue("iiii", rRect.left, rRect.top, rRect.right - rRect.left, rRect.bottom - rRect.top);
}

PyObject * wndMgrSetWindowHorizontalAlign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iAlign;
	if (!PyTuple_GetInteger(poArgs, 1, &iAlign))
	{
		return Py_BuildException();
	}

	pWin->SetHorizontalAlign(iAlign);

	return Py_BuildNone();
}

PyObject * wndMgrSetWindowVerticalAlign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iAlign;
	if (!PyTuple_GetInteger(poArgs, 1, &iAlign))
	{
		return Py_BuildException();
	}

	pWin->SetVerticalAlign(iAlign);

	return Py_BuildNone();
}

PyObject * wndMgrIsIn(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

#ifdef INGAME_WIKI
	bool bCheckChilds;
	if (!PyTuple_GetBoolean(poArgs, 1, &bCheckChilds))
	{
		bCheckChilds = false;
	}

	UI::CWindow* pWinPoint = UI::CWindowManager::Instance().GetPointWindow();
	if (pWin == pWinPoint)
	{
		return Py_BuildValue("b", true);
	}

	if (bCheckChilds && pWinPoint)
	{
		if (pWin->IsChild(pWinPoint, true))
		{
			return Py_BuildValue("b", true);
		}
	}

	return Py_BuildValue("b", false);
#else
	return Py_BuildValue("i", pWin == UI::CWindowManager::Instance().GetPointWindow());
#endif
}

PyObject * wndMgrGetMouseLocalPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	long lx, ly;
	pWin->GetMouseLocalPosition(lx, ly);
	return Py_BuildValue("ii", lx, ly);
}

PyObject * wndMgrGetHyperlink(PyObject * poSelf, PyObject * poArgs)
{
	char retBuf[1024];
	int retLen = CGraphicTextInstance::Hyperlink_GetText(retBuf, sizeof(retBuf) - 1);
	retBuf[retLen] = '\0';

	return Py_BuildValue("s#", retBuf, retLen);
}

PyObject * wndMgrGetScreenWidth(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().GetScreenWidth());
}

PyObject * wndMgrGetScreenHeight(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().GetScreenHeight());
}

PyObject * wndMgrGetMousePosition(PyObject * poSelf, PyObject * poArgs)
{
	long lx, ly;
	UI::CWindowManager::Instance().GetMousePosition(lx, ly);
	return Py_BuildValue("ii", lx, ly);
}

PyObject * wndMgrIsDragging(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildValue("i", UI::CWindowManager::Instance().IsDragging());
}

PyObject * wndMgrIsPickedWindow(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	UI::CWindow * pPickedWin = UI::CWindowManager::Instance().GetPointWindow();
	return Py_BuildValue("i", pWin == pPickedWin ? 1 : 0);
}

PyObject * wndMgrGetChildCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", pWin->GetChildCount());
}

uint32_t GetFlag(const char* pszFlag)
{
	if (!stricmp(pszFlag, "moveable"))
		return UI::CWindow::FLAG_MOVABLE;
	else if (!stricmp(pszFlag, "dragable"))
		return UI::CWindow::FLAG_DRAGABLE;
	else if (!stricmp(pszFlag, "attach"))
		return UI::CWindow::FLAG_ATTACH;
	else if (!stricmp(pszFlag, "restrict_x"))
		return UI::CWindow::FLAG_RESTRICT_X;
	else if (!stricmp(pszFlag, "restrict_y"))
		return UI::CWindow::FLAG_RESTRICT_Y;
	else if (!stricmp(pszFlag, "float"))
		return UI::CWindow::FLAG_FLOAT;
	else if (!stricmp(pszFlag, "not_pick"))
		return UI::CWindow::FLAG_NOT_PICK;
	else if (!stricmp(pszFlag, "ignore_size"))
		return UI::CWindow::FLAG_IGNORE_SIZE;
	else if (!stricmp(pszFlag, "rtl"))
		return UI::CWindow::FLAG_RTL;
	else if (!stricmp(pszFlag, "animate"))
		return UI::CWindow::FLAG_ANIMATED_BOARD;
	else
		return 0;
}

PyObject * wndMgrAddFlag(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	char * pszFlag;
	if (!PyTuple_GetString(poArgs, 1, &pszFlag))
	{
		return Py_BuildException();
	}

	if (pszFlag && *pszFlag)
	{
		if (!stricmp(pszFlag, "movable"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_MOVABLE);
		}
		else if (!stricmp(pszFlag, "limit"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_LIMIT);
		}
		else if (!stricmp(pszFlag, "dragable"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_DRAGABLE);
		}
		else if (!stricmp(pszFlag, "attach"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_ATTACH);
		}
		else if (!stricmp(pszFlag, "restrict_x"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_RESTRICT_X);
		}
		else if (!stricmp(pszFlag, "restrict_y"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_RESTRICT_Y);
		}
		else if (!stricmp(pszFlag, "float"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_FLOAT);
		}
		else if (!stricmp(pszFlag, "not_pick"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_NOT_PICK);
		}
		else if (!stricmp(pszFlag, "ignore_size"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_IGNORE_SIZE);
		}
		else if (!stricmp(pszFlag, "rtl"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_RTL);
		}
		else if (!stricmp(pszFlag, "ltr"))
		{
			pWin->RemoveFlag(UI::CWindow::FLAG_RTL);
		}
		else if (!stricmp(pszFlag, "animate"))
		{
			pWin->AddFlag(UI::CWindow::FLAG_ANIMATED_BOARD);
		}
		else
		{
			TraceError("Unknown window flag %s", pszFlag);
		}
	}

	return Py_BuildNone();
}

PyObject* wndMgrRemoveFlag(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* win;
	if (!PyTuple_GetWindow(poArgs, 0, &win))
		return Py_BadArgument();

	char* pszFlag;
	if (!PyTuple_GetString(poArgs, 1, &pszFlag))
		return Py_BadArgument();

	if (*pszFlag)
	{
		const uint32_t flag = GetFlag(pszFlag);
		if (!flag)
			TraceError("Unknown window flag %s", pszFlag);
		else
			win->RemoveFlag(flag);
	}

	return Py_BuildNone();
}

PyObject * wndMgrSetLimitBias(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int l;
	if (!PyTuple_GetInteger(poArgs, 1, &l))
	{
		return Py_BuildException();
	}
	int r;
	if (!PyTuple_GetInteger(poArgs, 2, &r))
	{
		return Py_BuildException();
	}
	int t;
	if (!PyTuple_GetInteger(poArgs, 3, &t))
	{
		return Py_BuildException();
	}
	int b;
	if (!PyTuple_GetInteger(poArgs, 4, &b))
	{
		return Py_BuildException();
	}

	pWin->SetLimitBias(l, r, t, b);
	return Py_BuildNone();
}

PyObject * wndMgrUpdateRect(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	pWin->UpdateRect();
	return Py_BuildNone();
}

PyObject * wndMgrAppendSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
	{
		return Py_BuildException();
	}

	int ixPosition;
	if (!PyTuple_GetInteger(poArgs, 2, &ixPosition))
	{
		return Py_BuildException();
	}

	int iyPosition;
	if (!PyTuple_GetInteger(poArgs, 3, &iyPosition))
	{
		return Py_BuildException();
	}

	int ixCellSize;
	if (!PyTuple_GetInteger(poArgs, 4, &ixCellSize))
	{
		return Py_BuildException();
	}

	int iyCellSize;
	if (!PyTuple_GetInteger(poArgs, 5, &iyCellSize))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->AppendSlot(iIndex, ixPosition, iyPosition, ixCellSize, iyCellSize);

	return Py_BuildNone();
}

PyObject * wndMgrArrangeSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iStartIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iStartIndex))
	{
		return Py_BuildException();
	}

	int ixCellCount;
	if (!PyTuple_GetInteger(poArgs, 2, &ixCellCount))
	{
		return Py_BuildException();
	}

	int iyCellCount;
	if (!PyTuple_GetInteger(poArgs, 3, &iyCellCount))
	{
		return Py_BuildException();
	}

	int ixCellSize;
	if (!PyTuple_GetInteger(poArgs, 4, &ixCellSize))
	{
		return Py_BuildException();
	}

	int iyCellSize;
	if (!PyTuple_GetInteger(poArgs, 5, &iyCellSize))
	{
		return Py_BuildException();
	}

	int ixBlank;
	if (!PyTuple_GetInteger(poArgs, 6, &ixBlank))
	{
		return Py_BuildException();
	}

	int iyBlank;
	if (!PyTuple_GetInteger(poArgs, 7, &iyBlank))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CGridSlotWindow::Type()))
	{
		TraceError("wndMgr.ArrangeSlot : not a grid window");
		return Py_BuildException();
	}

	UI::CGridSlotWindow * pGridSlotWin = (UI::CGridSlotWindow *)pWin;
	pGridSlotWin->ArrangeGridSlot(iStartIndex, ixCellCount, iyCellCount, ixCellSize, iyCellSize, ixBlank, iyBlank);

	return Py_BuildNone();
}

PyObject * wndMgrSetSlotBaseImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	float fr;
	if (!PyTuple_GetFloat(poArgs, 2, &fr))
	{
		return Py_BuildException();
	}
	float fg;
	if (!PyTuple_GetFloat(poArgs, 3, &fg))
	{
		return Py_BuildException();
	}
	float fb;
	if (!PyTuple_GetFloat(poArgs, 4, &fb))
	{
		return Py_BuildException();
	}
	float fa;
	if (!PyTuple_GetFloat(poArgs, 5, &fa))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		TraceError("wndMgr.ArrangeSlot : not a slot window");
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotBaseImage(szFileName, fr, fg, fb, fa);

	return Py_BuildNone();
}

PyObject * wndMgrSetCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	char * szUpImageName;
	if (!PyTuple_GetString(poArgs, 2, &szUpImageName))
	{
		return Py_BuildException();
	}
	char * szOverImageName;
	if (!PyTuple_GetString(poArgs, 3, &szOverImageName))
	{
		return Py_BuildException();
	}
	char * szDownImageName;
	if (!PyTuple_GetString(poArgs, 4, &szDownImageName))
	{
		return Py_BuildException();
	}
	char * szDisableImageName;
	if (!PyTuple_GetString(poArgs, 5, &szDisableImageName))
	{
		return Py_BuildException();
	}

	int iLeftButtonEnable;
	if (!PyTuple_GetInteger(poArgs, 6, &iLeftButtonEnable))
	{
		return Py_BuildException();
	}
	int iRightButtonEnable;
	if (!PyTuple_GetInteger(poArgs, 7, &iRightButtonEnable))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->SetCoverButton(iSlotIndex, szUpImageName, szOverImageName, szDownImageName, szDisableImageName, iLeftButtonEnable, iRightButtonEnable);

	return Py_BuildNone();
}

PyObject * wndMgrEnableCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->EnableCoverButton(iSlotIndex);

	return Py_BuildNone();
}

PyObject * wndMgrDisableCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->DisableCoverButton(iSlotIndex);

	return Py_BuildNone();
}

PyObject * wndMgrIsDisableCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	return Py_BuildValue("i", pSlotWin->IsDisableCoverButton(iSlotIndex));
}

PyObject * wndMgrSetAlwaysRenderCoverButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	bool bAlwaysRender = false;

	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!PyTuple_GetBoolean(poArgs, 2, &bAlwaysRender))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->SetAlwaysRenderCoverButton(iSlotIndex, bAlwaysRender);

	return Py_BuildNone();
}

PyObject* wndMgrGetSlotIndices(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWindow;
	std::vector<uint32_t> v_slots = pSlotWin->GetSlotIndincies();

	PyObject* pyTuple = PyTuple_New(v_slots.size());
	int idx = 0;
	for (const auto& rItem : v_slots)
	{
		PyTuple_SetItem(pyTuple, idx++, Py_BuildValue("i", rItem));
	}

	return pyTuple;
}

PyObject* wndMgrDeleteCoverButton(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWindow;
	pSlotWin->DeleteCoverButton(iSlotIndex);

	return Py_BuildNone();
}

PyObject * wndMgrAppendSlotButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	char * szUpImageName;
	if (!PyTuple_GetString(poArgs, 1, &szUpImageName))
	{
		return Py_BuildException();
	}
	char * szOverImageName;
	if (!PyTuple_GetString(poArgs, 2, &szOverImageName))
	{
		return Py_BuildException();
	}
	char * szDownImageName;
	if (!PyTuple_GetString(poArgs, 3, &szDownImageName))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->AppendSlotButton(szUpImageName, szOverImageName, szDownImageName);

	return Py_BuildNone();
}

PyObject * wndMgrAppendRequirementSignImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	char * szImageName;
	if (!PyTuple_GetString(poArgs, 1, &szImageName))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->AppendRequirementSignImage(szImageName);

	return Py_BuildNone();
}

PyObject * wndMgrShowSlotButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->ShowSlotButton(iSlotNumber);

	return Py_BuildNone();
}

PyObject * wndMgrHideAllSlotButton(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->HideAllSlotButton();

	return Py_BuildNone();
}

PyObject * wndMgrShowRequirementSign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->ShowRequirementSign(iSlotNumber);

	return Py_BuildNone();
}

PyObject * wndMgrHideRequirementSign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWindow;
	pSlotWin->HideRequirementSign(iSlotNumber);

	return Py_BuildNone();
}

PyObject * wndMgrRefreshSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->RefreshSlot();

	return Py_BuildNone();
}

PyObject * wndMgrSetUseMode(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetUseMode(iFlag);

	return Py_BuildNone();
}

PyObject* wndMgrIsUseMode(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	return Py_BuildValue("b", pSlotWin->IsUseMode());
}

PyObject * wndMgrSetUsableItem(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = static_cast<UI::CSlotWindow*>(pWin);
	pSlotWin->SetUsableItem(iFlag);

	return Py_BuildNone();
}

PyObject* wndMgrIsUsableItem(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	return Py_BuildValue("b", pSlotWin->IsUsableItem());
}

PyObject* wndMgrSetSwitchMode(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetSwitchMode(iFlag);

	return Py_BuildNone();
}

PyObject* wndMgrIsSwitchMode(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	return Py_BuildValue("b", pSlotWin->IsSwitchMode());
}

PyObject* wndMgrSetSwitchableItem(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetSwitchableItem(iFlag);

	return Py_BuildNone();
}

PyObject* wndMgrIsSwitchableItem(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BadArgument();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	return Py_BuildValue("b", pSlotWin->IsSwitchableItem());
}

PyObject* wndMgrGetPickedSlotNumber(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BadArgument();
	}
	bool focusItem;
	if (!PyTuple_GetBoolean(poArgs, 1, &focusItem))
	{
		focusItem = true;
	}

	UI::CGridSlotWindow* pSlotWin = (UI::CGridSlotWindow*)pWin;
	UI::CSlotWindow::TSlot* pSlot;
	pSlotWin->GetPickedSlotPointer(&pSlot, focusItem);

	return Py_BuildValue("i", pSlot ? (int)pSlot->dwSlotNumber : -1);
}

#ifdef ENABLE_RENEWAL_EXCHANGE
PyObject* wndMgrSetSlotHighlightedGreeen(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}
	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}
	UI::CSlotWindow* pSlotWin = static_cast<UI::CSlotWindow*>(pWin);
	pSlotWin->SetSlotHighlightedGreeen(iSlotIndex);
	return Py_BuildNone();
}

PyObject* wndMgrDisableSlotHighlightedGreen(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}
	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}
	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->DisableSlotHighlightedGreen(iSlotIndex);
	return Py_BuildNone();
}
#endif

PyObject * wndMgrClearSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearSlot(iSlotIndex);

	return Py_BuildNone();
}

PyObject * wndMgrClearAllSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearAllSlot();

	return Py_BuildNone();
}

PyObject * wndMgrHasSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

	return Py_BuildValue("i", pSlotWin->HasSlot(iSlotIndex));
}

PyObject * wndMgrSetSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iItemIndex))
	{
		return Py_BuildException();
	}

	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
	{
		return Py_BuildException();
	}

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
	{
		return Py_BuildException();
	}

	int iImageHandle;
	if (!PyTuple_GetInteger(poArgs, 5, &iImageHandle))
	{
		return Py_BuildException();
	}

	D3DXCOLOR diffuseColor;
	PyObject* pTuple;
	if (!PyTuple_GetObject(poArgs, 6, &pTuple))
	{
		diffuseColor = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
		//return Py_BuildException();
	}
	else
		// get diffuse color from pTuple
	{
		if (PyTuple_Size(pTuple) != 4)
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 0, &diffuseColor.r))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 1, &diffuseColor.g))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 2, &diffuseColor.b))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 3, &diffuseColor.a))
		{
			return Py_BuildException();
		}
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlot(iSlotIndex, iItemIndex, iWidth, iHeight, (CGraphicImage *)iImageHandle, diffuseColor);

	return Py_BuildNone();
}

PyObject * wndMgrSetSlotCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 2, &iCount))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCount(iSlotIndex, iCount);

	return Py_BuildNone();
}

PyObject * wndMgrSetSlotCountNew(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	int iGrade;
	if (!PyTuple_GetInteger(poArgs, 2, &iGrade))
	{
		return Py_BuildException();
	}

	int iCount;
	if (!PyTuple_GetInteger(poArgs, 3, &iCount))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCountNew(iSlotIndex, iGrade, iCount);

	return Py_BuildNone();
}

PyObject * wndMgrSetSlotCoolTime(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	float fCoolTime;
	if (!PyTuple_GetFloat(poArgs, 2, &fCoolTime))
	{
		return Py_BuildException();
	}

	float fElapsedTime = 0.0f;
	PyTuple_GetFloat(poArgs, 3, &fElapsedTime);

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotCoolTime(iSlotIndex, fCoolTime, fElapsedTime);

	return Py_BuildNone();
}

PyObject * wndMgrSetToggleSlot(PyObject * poSelf, PyObject * poArgs)
{
	assert(!"wndMgrSetToggleSlot - Don't use such function");
	return Py_BuildNone();
}

PyObject * wndMgrActivateSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	std::vector<float> diffuseColour(4, 0.0f);
	PyObject* pTuple;
	if (PyTuple_GetObject(poArgs, 2, &pTuple))
	{
		if (PyTuple_Size(pTuple) == 4)
		{
			for (size_t i = 0; i < diffuseColour.size(); ++i)
			{
				if (!PyTuple_GetFloat(pTuple, i, &diffuseColour[i]))
				{
					diffuseColour[i] = 0.0f;
				}
			}
		}
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ActivateSlot(iSlotIndex, diffuseColour);
	return Py_BuildNone();
}

PyObject * wndMgrDeactivateSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->DeactivateSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject* wndMgrNewActivateSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->NewActivateSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject* wndMgrNewDeactivateSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->NewDeactivateSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrEnableSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->EnableSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrDisableSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->DisableSlot(iSlotIndex);
	return Py_BuildNone();
}

#ifdef ENABLE_RENEWAL_SHOP_SELLING
PyObject* wndMgrSetUnusableSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetUnusableSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject* wndMgrSetUsableSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetUsableSlot(iSlotIndex);
	return Py_BuildNone();
}
#endif

PyObject * wndMgrShowSlotBaseImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ShowSlotBaseImage(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrHideSlotBaseImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->HideSlotBaseImage(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndMgrSetSlotType(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotType(iType);

	return Py_BuildNone();
}

PyObject * wndMgrSetSlotStyle(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iStyle;
	if (!PyTuple_GetInteger(poArgs, 1, &iStyle))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SetSlotStyle(iStyle);

	return Py_BuildNone();
}

PyObject * wndMgrSelectSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->SelectSlot(iIndex);

	return Py_BuildNone();
}

PyObject * wndMgrClearSelected(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->ClearSelected();

	return Py_BuildNone();
}

PyObject * wndMgrGetSelectedSlotCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSelectedSlotCount());
}

PyObject * wndMgrGetSelectedSlotNumber(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSelectedSlotNumber(iSlotNumber));
}

PyObject * wndMgrIsSelectedSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotNumber))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->isSelectedSlot(iSlotNumber));
}

PyObject * wndMgrGetSlotCount(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->GetSlotCount());
}

PyObject * wndMgrLockSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	float fLockColour[4];
	for (int i = 0, j = 2; i < sizeof(fLockColour) / sizeof(fLockColour[0]); ++i, ++j)
	{
		float& fColour = fLockColour[i];
		if (!PyTuple_GetFloat(poArgs, j, &fColour))
		{
			memset(fLockColour, 0, sizeof(fLockColour));
			fLockColour[3] = 0.5f;
			break;
		}
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->LockSlot(iSlotIndex, fLockColour);
	return Py_BuildNone();
}

PyObject * wndMgrUnlockSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}
	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	pSlotWin->UnlockSlot(iSlotIndex);
	return Py_BuildNone();
}

PyObject * wndBarSetColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iColor;
	if (!PyTuple_GetInteger(poArgs, 1, &iColor))
	{
		return Py_BuildException();
	}

	if (pWindow->IsType(UI::CBar3D::Type()))
	{
		int iLeftColor = iColor;

		int iRightColor;
		if (!PyTuple_GetInteger(poArgs, 2, &iRightColor))
		{
			return Py_BuildException();
		}
		int iCenterColor;
		if (!PyTuple_GetInteger(poArgs, 3, &iCenterColor))
		{
			return Py_BuildException();
		}

		((UI::CBar3D *)pWindow)->SetColor(iLeftColor, iRightColor, iCenterColor);
	}
	else
	{
		((UI::CWindow *)pWindow)->SetColor(iColor);
	}
	return Py_BuildNone();
}

PyObject * wndMgrAttachIcon(PyObject * poSelf, PyObject * poArgs)
{
	int iType;
	if (!PyTuple_GetInteger(poArgs, 0, &iType))
	{
		return Py_BuildException();
	}
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iIndex))
	{
		return Py_BuildException();
	}
	int iSlotNumber;
	if (!PyTuple_GetInteger(poArgs, 2, &iSlotNumber))
	{
		return Py_BuildException();
	}
	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
	{
		return Py_BuildException();
	}
	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().AttachIcon(iType, iIndex, iSlotNumber, iWidth, iHeight);
	return Py_BuildNone();
}

PyObject * wndMgrDeattachIcon(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindowManager::Instance().DeattachIcon();
	return Py_BuildNone();
}

PyObject * wndMgrSetAttachingFlag(PyObject * poSelf, PyObject * poArgs)
{
	BOOL bFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &bFlag))
	{
		return Py_BuildException();
	}

	UI::CWindowManager::Instance().SetAttachingFlag(bFlag);
	return Py_BuildNone();
}

// Text
PyObject * wndTextSetMax(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iMax;
	if (!PyTuple_GetInteger(poArgs, 1, &iMax))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetMax(iMax);
	return Py_BuildNone();
}
PyObject * wndTextSetHorizontalAlign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetHorizontalAlign(iType);
	return Py_BuildNone();
}
PyObject * wndTextSetVerticalAlign(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iType;
	if (!PyTuple_GetInteger(poArgs, 1, &iType))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetVerticalAlign(iType);
	return Py_BuildNone();
}
PyObject * wndTextSetSecret(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetSecret(iFlag);
	return Py_BuildNone();
}
PyObject * wndTextSetOutline(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetOutline(iFlag);
	return Py_BuildNone();
}
PyObject * wndTextSetFeather(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetFeather(iFlag);
	return Py_BuildNone();
}
PyObject * wndTextSetMultiLine(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 1, &iFlag))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetMultiLine(iFlag);
	return Py_BuildNone();
}
PyObject * wndTextSetFontName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFontName;
	if (!PyTuple_GetString(poArgs, 1, &szFontName))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetFontName(szFontName);
	return Py_BuildNone();
}
PyObject * wndTextSetFontColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	if (2 == PyTuple_Size(poArgs))
	{
		int iColor;
		if (!PyTuple_GetInteger(poArgs, 1, &iColor))
		{
			return Py_BuildException();
		}
		((UI::CTextLine*)pWindow)->SetFontColor(iColor);
	}
	else if (4 == PyTuple_Size(poArgs))
	{
		float fr;
		if (!PyTuple_GetFloat(poArgs, 1, &fr))
		{
			return Py_BuildException();
		}
		float fg;
		if (!PyTuple_GetFloat(poArgs, 2, &fg))
		{
			return Py_BuildException();
		}
		float fb;
		if (!PyTuple_GetFloat(poArgs, 3, &fb))
		{
			return Py_BuildException();
		}
		float fa = 1.0f;

		BYTE argb[4] =
		{
			(BYTE) (255.0f * fb),
			(BYTE) (255.0f * fg),
			(BYTE) (255.0f * fr),
			(BYTE) (255.0f * fa)
		};
		((UI::CTextLine*)pWindow)->SetFontColor(*((DWORD *) argb));
	}
	else
	{
		return Py_BuildException();
	}

	return Py_BuildNone();
}
PyObject * wndTextSetLimitWidth(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fWidth;
	if (!PyTuple_GetFloat(poArgs, 1, &fWidth))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetLimitWidth(fWidth);
	return Py_BuildNone();
}
PyObject * wndTextSetText(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szText;
	if (!PyTuple_GetString(poArgs, 1, &szText))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetText(szText);
	return Py_BuildNone();
}
PyObject* wndTextGetSpecificTextWidth(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BadArgument();
	}
	char* szText;
	if (!PyTuple_GetString(poArgs, 1, &szText))
	{
		return Py_BadArgument();
	}

	int textWidth = ((UI::CTextLine*)pWindow)->GetSpecificTextWidth(szText);
	return Py_BuildValue("i", textWidth);
}
PyObject * wndTextGetText(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("s", ((UI::CTextLine*)pWindow)->GetText());
}

PyObject* wndTextSetTextClip(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* window;
	if (!PyTuple_GetWindow(poArgs, 0, &window))
		return Py_BadArgument();

	RECT r;
	if (!PyTuple_GetLong(poArgs, 1, &r.left))
		return Py_BadArgument();
	if (!PyTuple_GetLong(poArgs, 2, &r.top))
		return Py_BadArgument();
	if (!PyTuple_GetLong(poArgs, 3, &r.right))
		return Py_BadArgument();
	if (!PyTuple_GetLong(poArgs, 4, &r.bottom))
		return Py_BadArgument();

	((UI::CWindow*)window)->SetClipRect(r);
	Py_RETURN_NONE;
}

PyObject * wndTextGetTextSize(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int nWidth;
	int nHeight;

	UI::CTextLine* pkTextLine = (UI::CTextLine*)pWindow;
	pkTextLine->GetTextSize(&nWidth, &nHeight);

	return Py_BuildValue("(ii)", nWidth, nHeight);
}


PyObject * wndTextShowCursor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->ShowCursor();
	return Py_BuildNone();
}
PyObject * wndTextHideCursor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->HideCursor();
	return Py_BuildNone();
}
#ifdef INGAME_WIKI
PyObject* wndTextIsShowCursor(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("b", ((UI::CTextLine*)pWindow)->IsShowCursor());
}
#endif
PyObject * wndTextGetCursorPosition(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", ((UI::CTextLine*)pWindow)->GetCursorPosition());
}
PyObject* wndTextGetCharPositionByCursor(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BadArgument();
	}
	int cursorPos;
	if (!PyTuple_GetInteger(poArgs, 1, &cursorPos))
	{
		return Py_BadArgument();
	}

	auto charPos = ((UI::CTextLine*)pWindow)->GetCharPositionByCursor(cursorPos);
	return Py_BuildValue("iii", std::get<0>(charPos), std::get<1>(charPos), std::get<2>(charPos));
}
PyObject* wndTextPositionToRenderPosition(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BadArgument();
	}
	int textPos;
	if (!PyTuple_GetInteger(poArgs, 1, &textPos))
	{
		return Py_BadArgument();
	}

	return Py_BuildValue("i", ((UI::CTextLine*)pWindow)->TextPositionToRenderPosition(textPos));
}

PyObject * wndNumberSetNumber(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szNumber;
	if (!PyTuple_GetString(poArgs, 1, &szNumber))
	{
		return Py_BuildException();
	}

	((UI::CNumberLine*)pWindow)->SetNumber(szNumber);

	return Py_BuildNone();
}

PyObject * wndNumberSetNumberHorizontalAlignCenter(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CNumberLine*)pWindow)->SetHorizontalAlign(UI::CWindow::HORIZONTAL_ALIGN_CENTER);

	return Py_BuildNone();
}

PyObject * wndNumberSetNumberHorizontalAlignRight(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CNumberLine*)pWindow)->SetHorizontalAlign(UI::CWindow::HORIZONTAL_ALIGN_RIGHT);

	return Py_BuildNone();
}

PyObject* wndNumberSetNumberVerticalAlignCenter(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CNumberLine*)pWindow)->SetVerticalAlign(UI::CWindow::VERTICAL_ALIGN_CENTER);

	return Py_BuildNone();
}

PyObject * wndNumberSetPath(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szPath;
	if (!PyTuple_GetString(poArgs, 1, &szPath))
	{
		return Py_BuildException();
	}

	((UI::CNumberLine*)pWindow)->SetPath(szPath);

	return Py_BuildNone();
}

PyObject * wndMarkBox_SetImageFilename(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CMarkBox*)pWindow)->LoadImage(szFileName);
	return Py_BuildNone();
}

PyObject * wndMarkBox_SetImage(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildNone();
}

PyObject * wndMarkBox_Load(PyObject * poSelf, PyObject * poArgs)
{
	return Py_BuildNone();
}

PyObject * wndMarkBox_SetIndex(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int nIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &nIndex))
	{
		return Py_BuildException();
	}

	((UI::CMarkBox*)pWindow)->SetIndex(nIndex);
	return Py_BuildNone();
}

PyObject * wndMarkBox_SetScale(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	float fScale;
	if (!PyTuple_GetFloat(poArgs, 1, &fScale))
	{
		return Py_BuildException();
	}

	((UI::CMarkBox*)pWindow)->SetScale(fScale);
	return Py_BuildNone();
}


PyObject * wndMarkBox_SetDiffuseColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
	{
		return Py_BuildException();
	}
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
	{
		return Py_BuildException();
	}
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
	{
		return Py_BuildException();
	}
	float fa;
	if (!PyTuple_GetFloat(poArgs, 4, &fa))
	{
		return Py_BuildException();
	}

	((UI::CMarkBox*)pWindow)->SetDiffuseColor(fr, fg, fb, fa);

	return Py_BuildNone();
}


PyObject * wndImageLoadImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	if (!((UI::CImageBox*)pWindow)->LoadImage(szFileName))
	{
		return Py_BuildException("Failed to load image (filename: %s)", szFileName);
	}

	return Py_BuildNone();
}

PyObject * wndImageSetDiffuseColor(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fr;
	if (!PyTuple_GetFloat(poArgs, 1, &fr))
	{
		return Py_BuildException();
	}
	float fg;
	if (!PyTuple_GetFloat(poArgs, 2, &fg))
	{
		return Py_BuildException();
	}
	float fb;
	if (!PyTuple_GetFloat(poArgs, 3, &fb))
	{
		return Py_BuildException();
	}
	float fa;
	if (!PyTuple_GetFloat(poArgs, 4, &fa))
	{
		return Py_BuildException();
	}

	if (pWindow->IsType(UI::CImageBox::Type()) ||
			pWindow->IsType(UI::CExpandedImageBox::Type()))
	{
		((UI::CImageBox*)pWindow)->SetDiffuseColor(fr, fg, fb, fa);
	}
	else if (pWindow->IsType(UI::CNumberLine::Type()))
	{
		((UI::CNumberLine*)pWindow)->SetDiffuseColor(fr, fg, fb, fa);
	}

	return Py_BuildNone();
}

PyObject * wndImageGetWidth(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", ((UI::CImageBox*)pWindow)->GetWidth());
}

PyObject * wndImageGetHeight(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", ((UI::CImageBox*)pWindow)->GetHeight());
}

PyObject * wndImageGetTexturePtr(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BadArgument();
	}

	CGraphicImageInstance* image = ((UI::CImageBox*)pWindow)->GetImageInstance();
	return Py_BuildValue("i", image ? image->GetTexturePointer() : NULL);
}
PyObject * wndImageSetScale(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
	{
		return Py_BuildException();
	}
	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
	{
		return Py_BuildException();
	}

	((UI::CExpandedImageBox*)pWindow)->SetScale(fx, fy);

	return Py_BuildNone();
}
PyObject * wndImageSetOrigin(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fx;
	if (!PyTuple_GetFloat(poArgs, 1, &fx))
	{
		return Py_BuildException();
	}
	float fy;
	if (!PyTuple_GetFloat(poArgs, 2, &fy))
	{
		return Py_BuildException();
	}

	((UI::CExpandedImageBox*)pWindow)->SetOrigin(fx, fy);

	return Py_BuildNone();
}
PyObject * wndImageSetRotation(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fRotation;
	if (!PyTuple_GetFloat(poArgs, 1, &fRotation))
	{
		return Py_BuildException();
	}

	((UI::CExpandedImageBox*)pWindow)->SetRotation(fRotation);

	return Py_BuildNone();
}

PyObject * wndImageSetRenderingMode(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int iMode;
	if (!PyTuple_GetInteger(poArgs, 1, &iMode))
	{
		return Py_BuildException();
	}

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
	{
		((UI::CExpandedImageBox*)pWindow)->SetRenderingMode(iMode);
	}

	return Py_BuildNone();
}
PyObject * wndImageSetDelay(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fDelay;
	if (!PyTuple_GetFloat(poArgs, 1, &fDelay))
	{
		return Py_BuildException();
	}

	((UI::CAniImageBox*)pWindow)->SetDelay(fDelay);

	return Py_BuildNone();
}
PyObject* wndImageGetDelay(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", ((UI::CAniImageBox*)pWindow)->GetDelay());
}
PyObject * wndImageAppendImage(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CAniImageBox*)pWindow)->AppendImage(szFileName);

	return Py_BuildNone();
}

PyObject* wndImageResetFrame(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CAniImageBox*)pWindow)->ResetFrame();

	return Py_BuildNone();
}

PyObject * wndButtonSetUpVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetUpVisual(szFileName);

	return Py_BuildNone();
}
PyObject * wndButtonSetOverVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetOverVisual(szFileName);

	return Py_BuildNone();
}
PyObject * wndButtonSetDownVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetDownVisual(szFileName);

	return Py_BuildNone();
}
PyObject * wndButtonSetDisableVisual(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	char * szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetDisableVisual(szFileName);

	return Py_BuildNone();
}
PyObject * wndButtonGetUpVisualFileName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetUpVisualFileName());
}
PyObject * wndButtonGetOverVisualFileName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetOverVisualFileName());
}
PyObject * wndButtonGetDownVisualFileName(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("s", ((UI::CButton*)pWindow)->GetDownVisualFileName());
}
PyObject * wndButtonFlash(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->Flash();

	return Py_BuildNone();
}
PyObject * wndButtonEnable(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->Enable();

	return Py_BuildNone();
}
PyObject * wndButtonDisable(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->Disable();

	return Py_BuildNone();
}
PyObject* wndButtonIsEnabled(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("b", ((UI::CButton*)pWindow)->IsDisable());
}
PyObject * wndButtonDown(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->Down();

	return Py_BuildNone();
}
PyObject * wndButtonSetUp(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CButton*)pWindow)->SetUp();

	return Py_BuildNone();
}
PyObject * wndButtonSetRestrictMovementArea(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int ix;
	if (!PyTuple_GetInteger(poArgs, 1, &ix))
	{
		return Py_BuildException();
	}
	int iy;
	if (!PyTuple_GetInteger(poArgs, 2, &iy))
	{
		return Py_BuildException();
	}
	int iwidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iwidth))
	{
		return Py_BuildException();
	}
	int iheight;
	if (!PyTuple_GetInteger(poArgs, 4, &iheight))
	{
		return Py_BuildException();
	}

	((UI::CDragButton*)pWindow)->SetRestrictMovementArea(ix, iy, iwidth, iheight);

	return Py_BuildNone();
}

PyObject * wndButtonIsDown(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	return Py_BuildValue("i", ((UI::CButton*)pWindow)->IsPressed());
}

extern BOOL g_bOutlineBoxEnable;
extern BOOL g_bShowOverInWindowName;

PyObject * wndMgrSetOutlineFlag(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
	{
		return Py_BuildException();
	}

	g_bOutlineBoxEnable = iFlag;

	return Py_BuildNone();
}

PyObject * wndMgrShowOverInWindowName(PyObject * poSelf, PyObject * poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
	{
		return Py_BuildException();
	}

	g_bShowOverInWindowName = iFlag;

	return Py_BuildNone();
}

#ifdef ENABLE_SLOT_WINDOW_EX
PyObject * wndMgrIsActivatedSlot(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;
	return Py_BuildValue("i", pSlotWin->IsActivatedSlot(iSlotIndex));
}

PyObject * wndMgrGetSlotCoolTime(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

	float fElapsedTime = 0.0f;
	float fCoolTime = pSlotWin->GetSlotCoolTime(iSlotIndex, &fElapsedTime);
	return Py_BuildValue("ff", fCoolTime, fElapsedTime);
}
#endif

#ifdef TRANSMUTATION_SYSTEM
PyObject* wndMgrEnableSlotCoverImage(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->EnableSlotCoverImage(iSlotIndex);

	return Py_BuildNone();
}

PyObject* wndMgrDisableSlotCoverImage(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->DisableSlotCoverImage(iSlotIndex);

	return Py_BuildNone();
}

PyObject* wndMgrActivateChangeLookSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	BYTE bDiffuseType;
	if (!PyTuple_GetByte(poArgs, 2, &bDiffuseType))
	{
		bDiffuseType = 0;
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->ActivateChangeLookSlot(iSlotIndex, bDiffuseType);
	return Py_BuildNone();
}

PyObject* wndMgrDeactivateChangeLookSlot(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->DeactivateChangeLookSlot(iSlotIndex);
	return Py_BuildNone();
}
#endif

PyObject * wndMgrGetItemIndex(PyObject * poSelf, PyObject * poArgs)
{
	UI::CWindow * pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

	return Py_BuildValue("i", pSlotWin->GetItemIndex(iSlotIndex));
}

#ifdef INGAME_WIKI
PyObject* wndMgrRegisterWikiRenderTarget(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* po;
	if (!PyTuple_GetObject(poArgs, 0, &po))
	{
		return Py_BuildException();
	}
	char* szLayer;
	if (!PyTuple_GetString(poArgs, 1, &szLayer))
	{
		return Py_BuildException();
	}

	UI::CWindow* pWindow = UI::CWindowManager::Instance().RegisterWikiRenderTarget(po, szLayer);
	return Py_BuildValue("i", pWindow);
}

PyObject* wndMgrSetInsideRender(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	bool val;
	if (!PyTuple_GetBoolean(poArgs, 1, &val))
	{
		return Py_BuildException();
	}

	pWin->SetInsideRender(val);

	//if (pWin->IsType(UI::CTextLine::Type()))
	//	((UI::CTextLine*)pWin)->SetFlagRenderingWiki(val);

	return Py_BuildNone();
}

PyObject* wndSetRenderingRect(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	float fLeft;
	if (!PyTuple_GetFloat(poArgs, 1, &fLeft))
	{
		return Py_BuildException();
	}
	float fTop;
	if (!PyTuple_GetFloat(poArgs, 2, &fTop))
	{
		return Py_BuildException();
	}
	float fRight;
	if (!PyTuple_GetFloat(poArgs, 3, &fRight))
	{
		return Py_BuildException();
	}
	float fBottom;
	if (!PyTuple_GetFloat(poArgs, 4, &fBottom))
	{
		return Py_BuildException();
	}

	if (pWindow->IsType(UI::CExpandedImageBox::Type()))
		((UI::CExpandedImageBox*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CAniImageBox::Type()))
		((UI::CAniImageBox*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CButton::Type()))
		((UI::CButton*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	else if (pWindow->IsType(UI::CTextLine::Type()))
		((UI::CTextLine*)pWindow)->SetRenderingRect(fLeft, fTop, fRight, fBottom);

	return Py_BuildNone();
}

PyObject* wndMgrGetRenderBox(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	RECT val;
	pWin->GetRenderBox(&val);
	return Py_BuildValue("iiii", val.left, val.top, val.right, val.bottom);
}

PyObject* wndMgrSetWikiRenderTarget(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int renderWindow = -1;
	if (!PyTuple_GetInteger(poArgs, 1, &renderWindow))
	{
		return Py_BuildException();
	}

	((UI::CUiWikiRenderTarget*)pWin)->SetWikiRenderTargetModule(renderWindow);
	return Py_BuildNone();
}

PyObject* wndTextSetFixedRenderPos(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	int startPos, endPos;
	if (!PyTuple_GetInteger(poArgs, 1, &startPos))
	{
		return Py_BuildException();
	}
	if (!PyTuple_GetInteger(poArgs, 2, &endPos))
	{
		return Py_BuildException();
	}

	((UI::CTextLine*)pWindow)->SetFixedRenderPos(startPos, endPos);
	return Py_BuildNone();
}

PyObject* wndTextGetRenderPos(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}
	WORD startPos, endPos;

	((UI::CTextLine*)pWindow)->GetRenderPositions(startPos, endPos);
	return Py_BuildValue("ii", startPos, endPos);
}

PyObject* wndImageUnloadImage(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	((UI::CImageBox*)pWindow)->UnloadImage();
	return Py_BuildNone();
}
#endif


#ifdef ENABLE_RT_EXTENSION
PyObject* wndMgrRegisterRenderTarget(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* po;
	char* layer;

	if (!PyTuple_GetObject(poArgs, 0, &po) ||
			!PyTuple_GetString(poArgs, 1, &layer))
	{
		return Py_BadArgument();
	}

	auto pWindow = UI::CWindowManager::Instance().RegisterRenderTarget(po, layer);
	return Py_BuildValue("i", pWindow);
}

PyObject* wndRenderTargetLoadImage(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	char* fileName;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetString(args, 1, &fileName))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	if (!renderTarget->LoadImage(fileName))
	{
		TraceError("Failed to load image: %s", fileName);
		return Py_BadArgument();
	}

	return Py_BuildNone();
}

PyObject* wndRenderTargetSetRenderTarget(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t race;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetInteger(args, 1, &race))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderTarget(race);

	return Py_BuildNone();
}

PyObject* wndRenderTargetSetRenderHair(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t vnum;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetInteger(args, 1, &vnum))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderHair(vnum);

	return Py_BuildNone();
}

PyObject* wndRenderTargetSetRenderArmor(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t vnum;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetInteger(args, 1, &vnum))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderArmor(vnum);

	return Py_BuildNone();
}

PyObject* wndRenderTargetSetRenderWeapon(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t vnum;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetInteger(args, 1, &vnum))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderWeapon(vnum);

	return Py_BuildNone();
}

#ifdef ENABLE_SASH_COSTUME_SYSTEM
PyObject* wndRenderTargetSetRenderSash(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t vnum;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetInteger(args, 1, &vnum))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderSash(vnum);

	return Py_BuildNone();
}
#endif

#ifdef ENABLE_SHINING_SYSTEM
PyObject* wndRenderTargetSetRenderToggleShining(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int flags[TOGGLE_SHINING_FLAG_32CNT];

	if (!PyTuple_GetWindow(args, 0, &window))
	{
		return Py_BadArgument();
	}

	for (int i = 0; i < TOGGLE_SHINING_FLAG_32CNT; ++i)
	{
		if (!PyTuple_GetInteger(args, 1 + i, &flags[i]))
		{
			return Py_BadArgument();
		}
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderToggleShining((DWORD*)flags);

	return Py_BuildNone();
}
#endif

PyObject* wndRenderTargetSetRendererMotion(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	int32_t vnum;

	if (!PyTuple_GetWindow(args, 0, &window) ||
		!PyTuple_GetInteger(args, 1, &vnum))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRendererMotion(vnum);

	return Py_BuildNone();
}

PyObject* wndRenderTargetSetRenderDistance(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	float distance;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetFloat(args, 1, &distance))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRenderDistance(distance);

	return Py_BuildNone();
}

PyObject* wndRenderTargetGetRenderDistance(PyObject* self, PyObject* args)
{
	UI::CWindow* window;

	if (!PyTuple_GetWindow(args, 0, &window))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	float rotation = renderTarget->GetRenderDistance();

	return Py_BuildValue("f", rotation);
}

PyObject* wndRenderTargetSetLightPosition(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	float x, y, z;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetFloat(args, 1, &x) ||
			!PyTuple_GetFloat(args, 2, &y) ||
			!PyTuple_GetFloat(args, 3, &z))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetLightPosition(D3DXVECTOR3(x, y, z));

	return Py_BuildNone();
}

PyObject* wndRenderTargetGetLightPosition(PyObject* self, PyObject* args)
{
	UI::CWindow* window;

	if (!PyTuple_GetWindow(args, 0, &window))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	auto& position = renderTarget->GetLightPosition();

	return Py_BuildValue("fff", position.x, position.y, position.z);
}

PyObject* wndRenderTargetSetRotation(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	float rotation;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetFloat(args, 1, &rotation))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRotation(rotation);

	return Py_BuildNone();
}

PyObject* wndRenderTargetGetRotation(PyObject* self, PyObject* args)
{
	UI::CWindow* window;

	if (!PyTuple_GetWindow(args, 0, &window))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	float rotation = renderTarget->GetRotation();

	return Py_BuildValue("f", rotation);
}

PyObject* wndRenderTargetSetRotationMode(PyObject* self, PyObject* args)
{
	UI::CWindow* window;
	bool rotation;

	if (!PyTuple_GetWindow(args, 0, &window) ||
			!PyTuple_GetBoolean(args, 1, &rotation))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->SetRotationMode(rotation);

	return Py_BuildNone();
}

PyObject* wndRenderTargetDestroyRender(PyObject* self, PyObject* args)
{
	UI::CWindow* window;

	if (!PyTuple_GetWindow(args, 0, &window))
	{
		return Py_BadArgument();
	}

	auto renderTarget = static_cast<UI::CRenderTarget*>(window);
	renderTarget->DestroyRender();

	return Py_BuildNone();
}

PyObject* wndMgrLockCursor(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* window;
	if (!PyTuple_GetWindow(poArgs, 0, &window))
	{
		return Py_BadArgument();
	}

	UI::CWindowManager::Instance().LockCursorOnWindow(window);
	return Py_BuildNone();
}

PyObject* wndMgrUnlockCursor(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* window;
	if (!PyTuple_GetWindow(poArgs, 0, &window))
	{
		return Py_BadArgument();
	}

	UI::CWindowManager::Instance().UnlockCursor(window);
	return Py_BuildNone();
}
#endif

/*** Start New Pet System ***/
PyObject* wndMgrSetSlotBaseImageScale(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	char* szFileName;
	if (!PyTuple_GetString(poArgs, 1, &szFileName))
	{
		return Py_BuildException();
	}

	float fr;
	if (!PyTuple_GetFloat(poArgs, 2, &fr))
	{
		return Py_BuildException();
	}

	float fg;
	if (!PyTuple_GetFloat(poArgs, 3, &fg))
	{
		return Py_BuildException();
	}

	float fb;
	if (!PyTuple_GetFloat(poArgs, 4, &fb))
	{
		return Py_BuildException();
	}

	float fa;
	if (!PyTuple_GetFloat(poArgs, 5, &fa))
	{
		return Py_BuildException();
	}

	float scale_x, scale_y;
	if (!PyTuple_GetFloat(poArgs, 6, &scale_x))
	{
		return Py_BuildException();
	}

	if (!PyTuple_GetFloat(poArgs, 7, &scale_y))
	{
		return Py_BuildException();
	}

	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		TraceError("wndMgr.ArrangeSlot:not a slot window");
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetSlotBaseImageScale(szFileName, fr, fg, fb, fa, scale_x, scale_y);
	return Py_BuildNone();
}

PyObject* wndMgrSetSlotScale(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWin;
	if (!PyTuple_GetWindow(poArgs, 0, &pWin))
	{
		return Py_BuildException();
	}

	int iSlotIndex;
	if (!PyTuple_GetInteger(poArgs, 1, &iSlotIndex))
	{
		return Py_BuildException();
	}

	int iItemIndex;
	if (!PyTuple_GetInteger(poArgs, 2, &iItemIndex))
	{
		return Py_BuildException();
	}

	int iWidth;
	if (!PyTuple_GetInteger(poArgs, 3, &iWidth))
	{
		return Py_BuildException();
	}

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 4, &iHeight))
	{
		return Py_BuildException();
	}

	int iImageHandle;
	if (!PyTuple_GetInteger(poArgs, 5, &iImageHandle))
	{
		return Py_BuildException();
	}

	D3DXCOLOR diffuseColor;

	PyObject* pTuple;
	if (!PyTuple_GetObject(poArgs, 6, &pTuple))
	{
		diffuseColor = D3DXCOLOR(1.0, 1.0, 1.0, 1.0);
	}
	else
	{
		if (PyTuple_Size(pTuple) != 4)
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 0, &diffuseColor.r))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 1, &diffuseColor.g))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 2, &diffuseColor.b))
		{
			return Py_BuildException();
		}
		if (!PyTuple_GetFloat(pTuple, 3, &diffuseColor.a))
		{
			return Py_BuildException();
		}
	}

	float sx, sy;
	if (!PyTuple_GetFloat(poArgs, 7, &sx))
	{

		return Py_BuildException();
	}

	if (!PyTuple_GetFloat(poArgs, 8, &sy))
	{

		return Py_BuildException();
	}


	if (!pWin->IsType(UI::CSlotWindow::Type()))
	{
		return Py_BuildException();
	}

	UI::CSlotWindow* pSlotWin = (UI::CSlotWindow*)pWin;
	pSlotWin->SetSlotScale(iSlotIndex, iItemIndex, iWidth, iHeight, (CGraphicImage*)iImageHandle, sx, sy, diffuseColor);

	return Py_BuildNone();
}
/*** End New Pet System ***/

PyObject* wndMgrSetCoolTimeImageBox(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	int iPercent;
	if (!PyTuple_GetInteger(poArgs, 1, &iPercent))

	{
		iPercent = 1;
	}

	char szFileName[256 + 1];
	_snprintf(szFileName, sizeof(szFileName), "d:/ymir work/ui/game/12zi/time_image/time_0%d.tga", iPercent);

	if (szFileName == NULL)
	{
		return Py_BuildException();
	}

	if (!((UI::CImageBox*)pWindow)->LoadImage(szFileName))
	{
		return Py_BuildException();
	}

	return Py_BuildNone();
}

PyObject* wndMgrSetStartCoolTimeImageBox(PyObject* poSelf, PyObject* poArgs)
{
	UI::CWindow* pWindow;
	if (!PyTuple_GetWindow(poArgs, 0, &pWindow))
	{
		return Py_BuildException();
	}

	float fPercent;
	if (!PyTuple_GetFloat(poArgs, 1, &fPercent))
	{
		return Py_BuildException();
	}

	const int img_value_reverse[] =
	{
		94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75,
		74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55,
		54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35,
		34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
		14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
	};

	int iPercent = fPercent * 95.0f;
	int images = img_value_reverse[iPercent - 1];

	char szBuf[256 + 1];
	if (images < 10)
	{
		_snprintf(szBuf, sizeof(szBuf), "d:/ymir work/ui/game/12zi/time_image/time_0%d.tga", images);
	}
	else
	{
		_snprintf(szBuf, sizeof(szBuf), "d:/ymir work/ui/game/12zi/time_image/time_%d.tga", images);
	}

	if (szBuf == NULL)
	{
		return Py_BuildException();
	}

	if (!((UI::CImageBox*)pWindow)->LoadImage(szBuf))
	{
		return Py_BuildException();
	}

	return Py_BuildNone();
}


void initwndMgr()
{
	static PyMethodDef s_methods[] =
	{
		// WindowManager
		{ "SetMouseHandler",			wndMgrSetMouseHandler,				METH_VARARGS },
		{ "SetScreenSize",				wndMgrSetScreenSize,				METH_VARARGS },
		{ "GetScreenWidth",				wndMgrGetScreenWidth,				METH_VARARGS },
		{ "GetScreenHeight",			wndMgrGetScreenHeight,				METH_VARARGS },
		{ "AttachIcon",					wndMgrAttachIcon,					METH_VARARGS },
		{ "DeattachIcon",				wndMgrDeattachIcon,					METH_VARARGS },
		{ "SetAttachingFlag",			wndMgrSetAttachingFlag,				METH_VARARGS },
		{ "GetAspect",					wndMgrGetAspect,					METH_VARARGS },
		{ "GetHyperlink",				wndMgrGetHyperlink,					METH_VARARGS },
		{ "OnceIgnoreMouseLeftButtonUpEvent",	wndMgrOnceIgnoreMouseLeftButtonUpEvent,		METH_VARARGS },

		// Window
		{ "Register",					wndMgrRegister,						METH_VARARGS },
		{ "RegisterSlotWindow",			wndMgrRegisterSlotWindow,			METH_VARARGS },
		{ "RegisterGridSlotWindow",		wndMgrRegisterGridSlotWindow,		METH_VARARGS },
		{ "RegisterTextLine",			wndMgrRegisterTextLine,				METH_VARARGS },
		{ "RegisterMarkBox",			wndMgrRegisterMarkBox,				METH_VARARGS },
		{ "RegisterImageBox",			wndMgrRegisterImageBox,				METH_VARARGS },
		{ "RegisterExpandedImageBox",	wndMgrRegisterExpandedImageBox,		METH_VARARGS },
		{ "RegisterAniImageBox",		wndMgrRegisterAniImageBox,			METH_VARARGS },
		{ "RegisterButton",				wndMgrRegisterButton,				METH_VARARGS },
		{ "RegisterRadioButton",		wndMgrRegisterRadioButton,			METH_VARARGS },
		{ "RegisterToggleButton",		wndMgrRegisterToggleButton,			METH_VARARGS },
		{ "RegisterDragButton",			wndMgrRegisterDragButton,			METH_VARARGS },
		{ "RegisterBox",				wndMgrRegisterBox,					METH_VARARGS },
		{ "RegisterBar",				wndMgrRegisterBar,					METH_VARARGS },
		{ "RegisterLine",				wndMgrRegisterLine,					METH_VARARGS },
		{ "RegisterBar3D",				wndMgrRegisterBar3D,				METH_VARARGS },
		{ "RegisterNumberLine",			wndMgrRegisterNumberLine,			METH_VARARGS },
		{ "Destroy",					wndMgrDestroy,						METH_VARARGS },
		{ "AddFlag",					wndMgrAddFlag,						METH_VARARGS },
		{"RemoveFlag",					wndMgrRemoveFlag,					METH_VARARGS},
		{ "IsRTL",						wndMgrIsRTL,						METH_VARARGS },

#ifdef ENABLE_MOUSEWHEEL_EVENT
		{ "SetScrollable",				wndMgrSetScrollable,				METH_VARARGS },
#endif

		// Base Window
		{ "SetName",					wndMgrSetName,						METH_VARARGS },
		{ "GetName",					wndMgrGetName,						METH_VARARGS },

		{ "SetTop",						wndMgrSetTop,						METH_VARARGS },
		{ "IsTop",						wndMgrIsTop,						METH_VARARGS },

		{ "Show",						wndMgrShow,							METH_VARARGS },
		{ "Hide",						wndMgrHide,							METH_VARARGS },
		{ "IsShow",						wndMgrIsShow,						METH_VARARGS },
		{ "SetParent",					wndMgrSetParent,					METH_VARARGS },
		{ "SetPickAlways",				wndMgrSetPickAlways,				METH_VARARGS },

		{ "IsFocus",					wndMgrIsFocus,						METH_VARARGS },
		{ "SetFocus",					wndMgrSetFocus,						METH_VARARGS },
		{ "KillFocus",					wndMgrKillFocus,					METH_VARARGS },
		{ "Lock",						wndMgrLock,							METH_VARARGS },
		{ "Unlock",						wndMgrUnlock,						METH_VARARGS },

		{ "SetWindowSize",				wndMgrSetWndSize,					METH_VARARGS },
		{ "SetWindowPosition",			wndMgrSetWndPosition,				METH_VARARGS },
		{ "GetWindowWidth",				wndMgrGetWndWidth,					METH_VARARGS },
		{ "GetWindowHeight",			wndMgrGetWndHeight,					METH_VARARGS },
		{ "GetWindowLocalPosition",		wndMgrGetWndLocalPosition,			METH_VARARGS },
		{ "GetWindowGlobalPosition",	wndMgrGetWndGlobalPosition,			METH_VARARGS },
		{ "GetWindowRect",				wndMgrGetWindowRect,				METH_VARARGS },
		{ "SetWindowHorizontalAlign",	wndMgrSetWindowHorizontalAlign,		METH_VARARGS },
		{ "SetWindowVerticalAlign",		wndMgrSetWindowVerticalAlign,		METH_VARARGS },

		{ "GetChildCount",				wndMgrGetChildCount,				METH_VARARGS },

		{ "IsPickedWindow",				wndMgrIsPickedWindow,				METH_VARARGS },
		{ "IsIn",						wndMgrIsIn,							METH_VARARGS },
		{ "GetMouseLocalPosition",		wndMgrGetMouseLocalPosition,		METH_VARARGS },
		{ "GetMousePosition",			wndMgrGetMousePosition,				METH_VARARGS },
		{ "IsDragging",					wndMgrIsDragging,					METH_VARARGS },

		{ "SetLimitBias",				wndMgrSetLimitBias,					METH_VARARGS },

		{ "UpdateRect",					wndMgrUpdateRect,					METH_VARARGS },

		// Slot Window
		{ "AppendSlot",					wndMgrAppendSlot,					METH_VARARGS },
		{ "ArrangeSlot",				wndMgrArrangeSlot,					METH_VARARGS },
		{ "ClearSlot",					wndMgrClearSlot,					METH_VARARGS },
		{ "ClearAllSlot",				wndMgrClearAllSlot,					METH_VARARGS },
		{ "HasSlot",					wndMgrHasSlot,						METH_VARARGS },
		{ "SetSlot",					wndMgrSetSlot,						METH_VARARGS },
		{ "SetSlotCount",				wndMgrSetSlotCount,					METH_VARARGS },
		{ "SetSlotCountNew",			wndMgrSetSlotCountNew,				METH_VARARGS },
		{ "SetSlotCoolTime",			wndMgrSetSlotCoolTime,				METH_VARARGS },
		{ "SetToggleSlot",				wndMgrSetToggleSlot,				METH_VARARGS },
		{ "ActivateSlot",				wndMgrActivateSlot,					METH_VARARGS },
		{ "DeactivateSlot",				wndMgrDeactivateSlot,				METH_VARARGS },

		{ "NewActivateSlot",			wndMgrNewActivateSlot,				METH_VARARGS },
		{ "NewDeactivateSlot",			wndMgrNewDeactivateSlot,			METH_VARARGS },

		{ "EnableSlot",					wndMgrEnableSlot,					METH_VARARGS },
		{ "DisableSlot",				wndMgrDisableSlot,					METH_VARARGS },
#ifdef ENABLE_RENEWAL_SHOP_SELLING
		{ "SetUnusableSlot",			wndMgrSetUnusableSlot,				METH_VARARGS },
		{ "SetUsableSlot",				wndMgrSetUsableSlot,				METH_VARARGS },
#endif
		{ "ShowSlotBaseImage",			wndMgrShowSlotBaseImage,			METH_VARARGS },
		{ "HideSlotBaseImage",			wndMgrHideSlotBaseImage,			METH_VARARGS },
		{ "SetSlotType",				wndMgrSetSlotType,					METH_VARARGS },
		{ "SetSlotStyle",				wndMgrSetSlotStyle,					METH_VARARGS },
		{ "SetSlotBaseImage",			wndMgrSetSlotBaseImage,				METH_VARARGS },

		{ "SetCoverButton",				wndMgrSetCoverButton,				METH_VARARGS },
		{ "EnableCoverButton",			wndMgrEnableCoverButton,			METH_VARARGS },
		{ "DisableCoverButton",			wndMgrDisableCoverButton,			METH_VARARGS },
		{ "IsDisableCoverButton",		wndMgrIsDisableCoverButton,			METH_VARARGS },
		{ "SetAlwaysRenderCoverButton",	wndMgrSetAlwaysRenderCoverButton,	METH_VARARGS },
		{ "DeleteCoverButton",			wndMgrDeleteCoverButton,			METH_VARARGS },

		{ "GetSlotIndices",				wndMgrGetSlotIndices,				METH_VARARGS },

		{ "AppendSlotButton",			wndMgrAppendSlotButton,				METH_VARARGS },
		{ "AppendRequirementSignImage",	wndMgrAppendRequirementSignImage,	METH_VARARGS },
		{ "ShowSlotButton",				wndMgrShowSlotButton,				METH_VARARGS },
		{ "HideAllSlotButton",			wndMgrHideAllSlotButton,			METH_VARARGS },
		{ "ShowRequirementSign",		wndMgrShowRequirementSign,			METH_VARARGS },
		{ "HideRequirementSign",		wndMgrHideRequirementSign,			METH_VARARGS },
		{ "RefreshSlot",				wndMgrRefreshSlot,					METH_VARARGS },
		{ "SetUseMode",					wndMgrSetUseMode,					METH_VARARGS },
		{ "IsUseMode",					wndMgrIsUseMode,					METH_VARARGS },
		{ "SetUsableItem",				wndMgrSetUsableItem,				METH_VARARGS },
		{ "IsUsableItem",				wndMgrIsUsableItem,					METH_VARARGS },
		{ "SetSwitchMode",				wndMgrSetSwitchMode,				METH_VARARGS },
		{ "IsSwitchMode",				wndMgrIsSwitchMode,					METH_VARARGS },
		{ "SetSwitchableItem",			wndMgrSetSwitchableItem,			METH_VARARGS },
		{ "IsSwitchableItem",			wndMgrIsSwitchableItem,				METH_VARARGS },
		{ "GetPickedSlotNumber",		wndMgrGetPickedSlotNumber,			METH_VARARGS },

#ifdef ENABLE_RENEWAL_EXCHANGE
		{ "SetSlotHighlightedGreeen",	wndMgrSetSlotHighlightedGreeen,		METH_VARARGS },
		{ "DisableSlotHighlightedGreen", wndMgrDisableSlotHighlightedGreen,	METH_VARARGS },
#endif

		{ "SelectSlot",					wndMgrSelectSlot,					METH_VARARGS },
		{ "ClearSelected",				wndMgrClearSelected,				METH_VARARGS },
		{ "GetSelectedSlotCount",		wndMgrGetSelectedSlotCount,			METH_VARARGS },
		{ "GetSelectedSlotNumber",		wndMgrGetSelectedSlotNumber,		METH_VARARGS },
		{ "IsSelectedSlot",				wndMgrIsSelectedSlot,				METH_VARARGS },
		{ "GetSlotCount",				wndMgrGetSlotCount,					METH_VARARGS },
		{ "LockSlot",					wndMgrLockSlot,						METH_VARARGS },
		{ "UnlockSlot",					wndMgrUnlockSlot,					METH_VARARGS },

		// Bar
		{ "SetColor",					wndBarSetColor,						METH_VARARGS },

		// TextLine
		{ "SetMax",						wndTextSetMax,						METH_VARARGS },
		{ "SetHorizontalAlign",			wndTextSetHorizontalAlign,			METH_VARARGS },
		{ "SetVerticalAlign",			wndTextSetVerticalAlign,			METH_VARARGS },
		{ "SetSecret",					wndTextSetSecret,					METH_VARARGS },
		{ "SetOutline",					wndTextSetOutline,					METH_VARARGS },
		{ "SetFeather",					wndTextSetFeather,					METH_VARARGS },
		{ "SetMultiLine",				wndTextSetMultiLine,				METH_VARARGS },
		{ "SetText",					wndTextSetText,						METH_VARARGS },
		{ "GetSpecificTextWidth",		wndTextGetSpecificTextWidth,		METH_VARARGS },
		{ "SetFontName",				wndTextSetFontName,					METH_VARARGS },
		{ "SetFontColor",				wndTextSetFontColor,				METH_VARARGS },
		{ "SetLimitWidth",				wndTextSetLimitWidth,				METH_VARARGS },
		{ "GetText",					wndTextGetText,						METH_VARARGS },
		{ "SetClipRect", wndTextSetTextClip, METH_VARARGS },
		{ "GetTextSize",				wndTextGetTextSize,					METH_VARARGS },
		{ "ShowCursor",					wndTextShowCursor,					METH_VARARGS },
		{ "HideCursor",					wndTextHideCursor,					METH_VARARGS },
		{ "GetCursorPosition",			wndTextGetCursorPosition,			METH_VARARGS },
		{ "GetCharPositionByCursor",	wndTextGetCharPositionByCursor,		METH_VARARGS },
		{ "TextPositionToRenderPosition",	wndTextPositionToRenderPosition,	METH_VARARGS },
		// NumberLine
		{ "SetNumber",						wndNumberSetNumber,							METH_VARARGS },
		{ "SetNumberHorizontalAlignCenter",	wndNumberSetNumberHorizontalAlignCenter,	METH_VARARGS },
		{ "SetNumberHorizontalAlignRight",	wndNumberSetNumberHorizontalAlignRight,		METH_VARARGS },
		{ "SetNumberVerticalAlignCenter",	wndNumberSetNumberVerticalAlignCenter,		METH_VARARGS },
		{ "SetPath",						wndNumberSetPath,							METH_VARARGS },

		// ImageBox
		{ "MarkBox_SetImage",			wndMarkBox_SetImage,				METH_VARARGS },
		{ "MarkBox_SetImageFilename",	wndMarkBox_SetImageFilename,		METH_VARARGS },
		{ "MarkBox_Load",				wndMarkBox_Load,					METH_VARARGS },
		{ "MarkBox_SetIndex",			wndMarkBox_SetIndex,				METH_VARARGS },
		{ "MarkBox_SetScale",			wndMarkBox_SetScale,				METH_VARARGS },
		{ "MarkBox_SetDiffuseColor",	wndMarkBox_SetDiffuseColor,			METH_VARARGS },

		// ImageBox
		{ "LoadImage",					wndImageLoadImage,					METH_VARARGS },
		{ "SetDiffuseColor",			wndImageSetDiffuseColor,			METH_VARARGS },
		{ "GetWidth",					wndImageGetWidth,					METH_VARARGS },
		{ "GetHeight",					wndImageGetHeight,					METH_VARARGS },
		{ "GetTexturePtr",				wndImageGetTexturePtr,				METH_VARARGS },
		// ExpandedImageBox
		{ "SetScale",					wndImageSetScale,					METH_VARARGS },
		{ "SetOrigin",					wndImageSetOrigin,					METH_VARARGS },
		{ "SetRotation",				wndImageSetRotation,				METH_VARARGS },
		{ "SetRenderingMode",			wndImageSetRenderingMode,			METH_VARARGS },
		// AniImageBox
		{ "SetDelay",					wndImageSetDelay,					METH_VARARGS },
		{ "GetDelay",					wndImageGetDelay,					METH_VARARGS },
		{ "AppendImage",				wndImageAppendImage,				METH_VARARGS },
		{ "ResetFrame",					wndImageResetFrame,					METH_VARARGS },

		// Button
		{ "SetUpVisual",				wndButtonSetUpVisual,				METH_VARARGS },
		{ "SetOverVisual",				wndButtonSetOverVisual,				METH_VARARGS },
		{ "SetDownVisual",				wndButtonSetDownVisual,				METH_VARARGS },
		{ "SetDisableVisual",			wndButtonSetDisableVisual,			METH_VARARGS },
		{ "GetUpVisualFileName",		wndButtonGetUpVisualFileName,		METH_VARARGS },
		{ "GetOverVisualFileName",		wndButtonGetOverVisualFileName,		METH_VARARGS },
		{ "GetDownVisualFileName",		wndButtonGetDownVisualFileName,		METH_VARARGS },
		{ "Flash",						wndButtonFlash,						METH_VARARGS },
		{ "Enable",						wndButtonEnable,					METH_VARARGS },
		{ "Disable",					wndButtonDisable,					METH_VARARGS },
		{ "IsEnabled",					wndButtonIsEnabled,					METH_VARARGS },
		{ "Down",						wndButtonDown,						METH_VARARGS },
		{ "SetUp",						wndButtonSetUp,						METH_VARARGS },
		{ "IsDown",						wndButtonIsDown,					METH_VARARGS },

		// DragButton
		{ "SetRestrictMovementArea",	wndButtonSetRestrictMovementArea,	METH_VARARGS },

		// For Debug
		{ "SetOutlineFlag",				wndMgrSetOutlineFlag,				METH_VARARGS },
		{ "ShowOverInWindowName",		wndMgrShowOverInWindowName,			METH_VARARGS },

#ifdef ENABLE_SLOT_WINDOW_EX
		{ "IsActivatedSlot",			wndMgrIsActivatedSlot, METH_VARARGS },
		{ "GetSlotCoolTime",			wndMgrGetSlotCoolTime, METH_VARARGS },
#endif

#ifdef TRANSMUTATION_SYSTEM
		{ "EnableSlotCoverImage",		wndMgrEnableSlotCoverImage,			METH_VARARGS },
		{ "DisableSlotCoverImage",		wndMgrDisableSlotCoverImage,		METH_VARARGS },
		{ "ActivateChangeLookSlot",		wndMgrActivateChangeLookSlot,		METH_VARARGS },
		{ "DeactivateChangeLookSlot",	wndMgrDeactivateChangeLookSlot,		METH_VARARGS },
#endif
		{ "GetItemIndex",				wndMgrGetItemIndex,					METH_VARARGS },

#ifdef INGAME_WIKI
		{ "RegisterWikiRenderTarget",	wndMgrRegisterWikiRenderTarget,		METH_VARARGS },
		{ "SetInsideRender",			wndMgrSetInsideRender,				METH_VARARGS },
		{ "SetRenderingRect",			wndSetRenderingRect,				METH_VARARGS },
		{ "GetRenderBox",				wndMgrGetRenderBox,					METH_VARARGS },
		{ "SetWikiRenderTarget",		wndMgrSetWikiRenderTarget,			METH_VARARGS },
		{ "SetFixedRenderPos",			wndTextSetFixedRenderPos,			METH_VARARGS },
		{ "GetRenderPos",				wndTextGetRenderPos,				METH_VARARGS },
		{ "UnloadImage",				wndImageUnloadImage,				METH_VARARGS },
		{ "IsShowCursor",				wndTextIsShowCursor,				METH_VARARGS },
#endif

#ifdef ENABLE_RT_EXTENSION
		{ "RegisterRenderTarget",	wndMgrRegisterRenderTarget,			METH_VARARGS },
		{ "RT_LoadImage",			wndRenderTargetLoadImage,			METH_VARARGS },
		{ "RT_SetRenderTarget",		wndRenderTargetSetRenderTarget,		METH_VARARGS },
		{ "RT_SetRenderHair",		wndRenderTargetSetRenderHair,		METH_VARARGS },
		{ "RT_SetRenderArmor",		wndRenderTargetSetRenderArmor,		METH_VARARGS },
		{ "RT_SetRenderWeapon",		wndRenderTargetSetRenderWeapon,		METH_VARARGS },
#ifdef ENABLE_SASH_COSTUME_SYSTEM
		{ "RT_SetRenderSash",		wndRenderTargetSetRenderSash,		METH_VARARGS },
#endif
#ifdef ENABLE_SHINING_SYSTEM
		{ "RT_SetRenderToggleShining",	wndRenderTargetSetRenderToggleShining,	METH_VARARGS },
#endif
		{ "RT_SetRenderMotion",		wndRenderTargetSetRendererMotion,	METH_VARARGS },
		{ "RT_SetRenderDistance",	wndRenderTargetSetRenderDistance,	METH_VARARGS },
		{ "RT_GetRenderDistance",	wndRenderTargetGetRenderDistance,	METH_VARARGS },
		{ "RT_SetLightPosition",	wndRenderTargetSetLightPosition,	METH_VARARGS },
		{ "RT_GetLightPosition",	wndRenderTargetGetLightPosition,	METH_VARARGS },
		{ "RT_SetRotation",			wndRenderTargetSetRotation,			METH_VARARGS },
		{ "RT_GetRotation",			wndRenderTargetGetRotation,			METH_VARARGS },
		{ "RT_SetRotationMode",		wndRenderTargetSetRotationMode,		METH_VARARGS },
		{ "RT_DestroyRender",		wndRenderTargetDestroyRender,		METH_VARARGS },

		{ "LockCursor",				wndMgrLockCursor,					METH_VARARGS },
		{ "UnlockCursor",			wndMgrUnlockCursor,					METH_VARARGS },
#endif

		/*** Start New Pet System ***/
		{ "SetSlotScale", wndMgrSetSlotScale, METH_VARARGS },
		{ "SetSlotBaseImageScale", wndMgrSetSlotBaseImageScale, METH_VARARGS },
		/*** End New Pet System ***/

		{ "SetCoolTimeImageBox", wndMgrSetCoolTimeImageBox, METH_VARARGS },
		{ "SetStartCoolTimeImageBox", wndMgrSetStartCoolTimeImageBox, METH_VARARGS },
		{ NULL,							NULL,								NULL },
	};

	PyObject * poModule = Py_InitModule("wndMgr", s_methods);

//	PyObject * poMgrModule = Py_InitModule("wndMgr", s_methods);
//	PyObject * poTextModule = Py_InitModule("wndText", s_methods);
//	PyObject * poSlotModule = Py_InitModule("wndSlot", s_methods);

	PyModule_AddIntConstant(poModule, "SLOT_STYLE_NONE",				UI::SLOT_STYLE_NONE);
	PyModule_AddIntConstant(poModule, "SLOT_STYLE_PICK_UP",				UI::SLOT_STYLE_PICK_UP);
	PyModule_AddIntConstant(poModule, "SLOT_STYLE_SELECT",				UI::SLOT_STYLE_SELECT);

	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_LEFT",		CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_RIGHT",	CGraphicTextInstance::HORIZONTAL_ALIGN_RIGHT);
	PyModule_AddIntConstant(poModule, "TEXT_HORIZONTAL_ALIGN_CENTER",	CGraphicTextInstance::HORIZONTAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_TOP",		CGraphicTextInstance::VERTICAL_ALIGN_TOP);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_BOTTOM",		CGraphicTextInstance::VERTICAL_ALIGN_BOTTOM);
	PyModule_AddIntConstant(poModule, "TEXT_VERTICAL_ALIGN_CENTER",		CGraphicTextInstance::VERTICAL_ALIGN_CENTER);

	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_LEFT",			UI::CWindow::HORIZONTAL_ALIGN_LEFT);
	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_CENTER",		UI::CWindow::HORIZONTAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "HORIZONTAL_ALIGN_RIGHT",			UI::CWindow::HORIZONTAL_ALIGN_RIGHT);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_TOP",				UI::CWindow::VERTICAL_ALIGN_TOP);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_CENTER",			UI::CWindow::VERTICAL_ALIGN_CENTER);
	PyModule_AddIntConstant(poModule, "VERTICAL_ALIGN_BOTTOM",			UI::CWindow::VERTICAL_ALIGN_BOTTOM);

	PyModule_AddIntConstant(poModule, "RENDERING_MODE_MODULATE",		CGraphicExpandedImageInstance::RENDERING_MODE_MODULATE);
}
