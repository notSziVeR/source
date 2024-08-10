#include "StdAfx.h"
#include "../eterBase/CRC32.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"
#include "PythonWindowManager.h"

#include "../EterLib/StateManager.h"

#ifdef INGAME_WIKI
	#include "../eterLib/CWikiRenderTargetManager.h"
#endif

#include "../UserInterface/PythonConfig.h"

BOOL g_bOutlineBoxEnable = FALSE;

namespace UI
{

CWindow::CWindow(PyObject * ppyObject) :
	m_x(0),
	m_y(0),
	m_lWidth(0),
	m_lHeight(0),
	m_poHandler(ppyObject),
	m_bShow(false),
	m_pParent(NULL),
	m_dwFlag(0),
	m_isUpdatingChildren(FALSE)
#ifdef ENABLE_MOUSEWHEEL_EVENT
	, m_bIsScrollable(false)
#endif
#ifdef INGAME_WIKI
	, m_isInsideRender(false)
#endif
	,
	m_clipRect({ 0, 0, 0, 0 })
{
#ifdef _DEBUG
	static DWORD DEBUG_dwGlobalCounter = 0;
	DEBUG_dwCounter = DEBUG_dwGlobalCounter++;

	m_strName = "!!debug";
#endif
	//assert(m_poHandler != NULL);
	m_HorizontalAlign = HORIZONTAL_ALIGN_LEFT;
	m_VerticalAlign = VERTICAL_ALIGN_TOP;
	m_rect.bottom = m_rect.left = m_rect.right = m_rect.top = 0;
	m_limitBiasRect.bottom = m_limitBiasRect.left = m_limitBiasRect.right = m_limitBiasRect.top = 0;
#ifdef INGAME_WIKI
	memset(&m_renderBox, 0, sizeof(m_renderBox));
#endif
	D3DXMatrixIdentity(&m_matScaling);
}

CWindow::~CWindow()
{
}

DWORD CWindow::Type()
{
	static DWORD s_dwType = GetCRC32("CWindow", strlen("CWindow"));
	return (s_dwType);
}

BOOL CWindow::IsType(DWORD dwType)
{
	return OnIsType(dwType);
}

BOOL CWindow::OnIsType(DWORD dwType)
{
	if (CWindow::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}

struct FClear
{
	void operator () (CWindow * pWin)
	{
		pWin->Clear();
	}
};

void CWindow::Clear()
{
	std::for_each(m_pChildList.begin(), m_pChildList.end(), FClear());
	m_pChildList.clear();

	m_pParent = NULL;
	DestroyHandle();
	Hide();
}

void CWindow::DestroyHandle()
{
	m_poHandler = NULL;
}

void CWindow::Show()
{
	if (IsFlag(FLAG_ANIMATED_BOARD) && CPythonConfig::Instance().GetString(CPythonConfig::CLASS_OPTION, "ANIMATION_MODE", "0") == "1")
	{
		this->m_bShow = true;

		m_sizeAnimation = tweeny::from(0.0f)
						  .to(1.0f)
						  .during(310)
						  .via(tweeny::easing::cubicOut)
						  .onStep([this](tweeny::tween<float>& t, float scale)
		{
			if (t.progress() == 0.0f)
			{
				SetScale(0.0f, 0.0f);
				//SetAllAlpha(0.0f);
			}

			SetScale(scale, scale);
			//SetAllAlpha(scale);

			if (t.progress() == 1.0f)
			{
				SetScale(1.0f, 1.0f);
				//SetAllAlpha(1.0f);

				return true;
			}
			else
			{
				return false;
			}
		});
		m_sizeAnimation.value().step(0.0f);
	}
	else
	{
		m_bShow = true;
	}
}

void CWindow::Hide()
{
	if (IsFlag(FLAG_ANIMATED_BOARD) && CPythonConfig::Instance().GetString(CPythonConfig::CLASS_OPTION, "ANIMATION_MODE", "0") == "1")
	{
		m_sizeAnimation = tweeny::from(1.0f)
						  .to(0.0f)
						  .during(140)
						  .via(tweeny::easing::cubicOut)
						  .onStep([this](tweeny::tween<float>& t, float scale)
		{
			if (t.progress() == 0.0f)
			{
				SetScale(1.0f, 1.0f);
				//SetAllAlpha(1.0f);
			}

			SetScale(scale, scale);
			//SetAllAlpha(scale);

			if (t.progress() == 1.0f)
			{
				SetScale(0.0f, 0.0f);
				//SetAllAlpha(0.0f);

				m_bShow = false;
				return true;
			}
			else
			{
				return false;
			}
		});
		m_sizeAnimation.value().step(0.0f);
	}
	else
	{
		m_bShow = false;
	}
}

#ifdef INGAME_WIKI
void CWindow::OnHideWithChilds()
{
	OnHide();
	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::OnHideWithChilds));
}

void CWindow::OnHide()
{
	PyCallClassMemberFunc(m_poHandler, "OnHide", BuildEmptyTuple());
}
#endif

bool CWindow::IsRendering()
{
	if (!IsShow())
	{
		return false;
	}

	if (!m_pParent)
	{
		return true;
	}

	return m_pParent->IsRendering();
}

void CWindow::__RemoveReserveChildren()
{
	if (m_pReserveChildList.empty())
	{
		return;
	}

	TWindowContainer::iterator it;
	for (it = m_pReserveChildList.begin(); it != m_pReserveChildList.end(); ++it)
	{
		m_pChildList.remove(*it);
	}
	m_pReserveChildList.clear();
}

void CWindow::Update()
{
	if (!IsShow())
	{
		return;
	}

	__RemoveReserveChildren();

	OnUpdate();

	if (m_sizeAnimation)
	{
		m_sizeAnimation.value().step(static_cast<uint32_t>(CTimer::instance().GetElapsedMilliecond()));
	}

	m_isUpdatingChildren = TRUE;
	TWindowContainer::iterator it;
	for (it = m_pChildList.begin(); it != m_pChildList.end();)
	{
		TWindowContainer::iterator it_next = it;
		++it_next;
		(*it)->Update();
		it = it_next;
	}
	m_isUpdatingChildren = FALSE;
}

#ifdef INGAME_WIKI
bool CWindow::IsShow()
{
	if (!m_bShow)
	{
		return false;
	}

	if (m_isInsideRender)
		if (m_renderBox.left + m_renderBox.right >= m_lWidth || m_renderBox.top + m_renderBox.bottom >= m_lHeight)
		{
			return false;
		}

	return true;
}
#endif

void CWindow::Render()
{
	if (!IsShow())
	{
		return;
	}

	OnRender();

	D3DXMATRIX currentMat;
	STATEMANAGER.GetTransform(D3DTS_WORLD, &currentMat);

	D3DXMATRIX newMat;
	D3DXMatrixMultiply(&newMat, &currentMat, &m_matScaling);
	STATEMANAGER.SetTransform(D3DTS_WORLD, &newMat);

	if (g_bOutlineBoxEnable)
	{
		CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f);
		CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
	}

	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::Render));

	STATEMANAGER.SetTransform(D3DTS_WORLD, &currentMat);

#ifdef INGAME_WIKI
	OnAfterRender();
#endif

}

#include "directxmath/DirectXMath.h"
void CWindow::SetScale(float fx, float fy)
{
	m_v2Scale.x = fx;
	m_v2Scale.y = fy;
	UpdateRect();

	using namespace DirectX;

	XMFLOAT2 vCenter(m_x + (m_lWidth / 2), m_y + (m_lHeight / 2));
	XMStoreFloat4x4((XMFLOAT4X4*)&m_matScaling, XMMatrixTransformation2D(XMLoadFloat2((XMFLOAT2*)&vCenter), NULL, XMLoadFloat2((XMFLOAT2*)&m_v2Scale), XMLoadFloat2((XMFLOAT2*)&vCenter), NULL, XMVectorZero()));
}

void CWindow::OnUpdate()
{
	if (!m_poHandler)
	{
		return;
	}

	if (!IsShow())
	{
		return;
	}

	static PyObject* poFuncName_OnUpdate = PyString_InternFromString("OnUpdate");

	//PyCallClassMemberFunc(m_poHandler, "OnUpdate", BuildEmptyTuple());
	PyCallClassMemberFunc_ByPyString(m_poHandler, poFuncName_OnUpdate, BuildEmptyTuple());

}

#ifdef ENABLE_RT_EXTENSION
void CWindow::OnUpdateLockedCursor(int xdif, long ydif)
{
	if (!m_poHandler)
	{
		return;
	}

	PyCallClassMemberFunc(m_poHandler, "OnUpdateLockedCursor", Py_BuildValue("ii", xdif, ydif));
}
#endif

void CWindow::OnRender()
{
	if (!m_poHandler)
	{
		return;
	}

	if (!IsShow())
	{
		return;
	}

	//PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
	PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
}

#ifdef INGAME_WIKI
void CWindow::OnAfterRender()
{
	if (!m_poHandler)
	{
		return;
	}

	if (!IsShow())
	{
		return;
	}

	PyCallClassMemberFunc(m_poHandler, "OnAfterRender", BuildEmptyTuple());
}
#endif

void CWindow::SetName(const char * c_szName)
{
	m_strName = c_szName;
}

void CWindow::SetSize(long width, long height)
{
	m_lWidth = width;
	m_lHeight = height;

	m_rect.right = m_rect.left + m_lWidth;
	m_rect.bottom = m_rect.top + m_lHeight;
#ifdef INGAME_WIKI
	if (m_isInsideRender)
	{
		UpdateRenderBoxRecursive();
	}
#endif
}

void CWindow::SetHorizontalAlign(DWORD dwAlign)
{
	m_HorizontalAlign = (EHorizontalAlign)dwAlign;
	UpdateRect();
}

void CWindow::SetVerticalAlign(DWORD dwAlign)
{
	m_VerticalAlign = (EVerticalAlign)dwAlign;
	UpdateRect();
}

void CWindow::SetPosition(long x, long y)
{
	m_x = x;
	m_y = y;

	UpdateRect();
#ifdef INGAME_WIKI
	if (m_isInsideRender)
	{
		UpdateRenderBoxRecursive();
	}
#endif
}

#ifdef INGAME_WIKI
void CWindow::UpdateRenderBoxRecursive()
{
	UpdateRenderBox();
	for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)
	{
		(*it)->UpdateRenderBoxRecursive();
	}
}
#endif

void CWindow::GetPosition(long * plx, long * ply)
{
	*plx = m_x;
	*ply = m_y;
}

long CWindow::UpdateRect()
{
	m_rect.top		= m_y;
	if (m_pParent)
	{
		switch (m_VerticalAlign)
		{
		case VERTICAL_ALIGN_BOTTOM:
			m_rect.top = m_pParent->GetHeight() - m_rect.top;
			break;
		case VERTICAL_ALIGN_CENTER:
			m_rect.top = (m_pParent->GetHeight() - GetHeight()) / 2 + m_rect.top;
			break;
		}
		m_rect.top += m_pParent->m_rect.top;
	}
	m_rect.bottom	= m_rect.top + m_lHeight;

#if defined( _USE_CPP_RTL_FLIP )
	if ( m_pParent == NULL )
	{
		m_rect.left		= m_x;
		m_rect.right	= m_rect.left + m_lWidth;
	}
	else
	{
		if ( m_pParent->IsFlag(UI::CWindow::FLAG_RTL) == true )
		{
			m_rect.left = m_pParent->GetWidth() - m_lWidth - m_x;
			switch (m_HorizontalAlign)
			{
			case HORIZONTAL_ALIGN_RIGHT:
				m_rect.left = - m_x;
				break;
			case HORIZONTAL_ALIGN_CENTER:
				m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() - m_x;
				break;
			}
			m_rect.left += m_pParent->m_rect.left;
			m_rect.right = m_rect.left + m_lWidth;
		}
		else
		{
			m_rect.left		= m_x;
			switch (m_HorizontalAlign)
			{
			case HORIZONTAL_ALIGN_RIGHT:
				m_rect.left = m_pParent->GetWidth() - m_rect.left;
				break;
			case HORIZONTAL_ALIGN_CENTER:
				m_rect.left = (m_pParent->GetWidth() - GetWidth()) / 2 + m_rect.left;
				break;
			}
			m_rect.left += m_pParent->m_rect.left;
			m_rect.right = m_rect.left + m_lWidth;
		}
	}
#else
	m_rect.left		= m_x;
	if (m_pParent)
	{
		switch (m_HorizontalAlign)
		{
		case HORIZONTAL_ALIGN_RIGHT:
			m_rect.left = ::abs(m_pParent->GetWidth()) - m_rect.left;
			break;
		case HORIZONTAL_ALIGN_CENTER:
			m_rect.left = m_pParent->GetWidth() / 2 - GetWidth() / 2 + m_rect.left;
			break;
		}
		m_rect.left += 0L < m_pParent->GetWidth() ? m_pParent->m_rect.left : m_pParent->m_rect.right + ::abs(m_pParent->GetWidth());
	}
	m_rect.right = m_rect.left + m_lWidth;
#endif
	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

	OnChangePosition();

	return 1;
}

void CWindow::GetLocalPosition(long & rlx, long & rly)
{
	rlx = rlx - m_rect.left;
	rly = rly - m_rect.top;
}

void CWindow::GetMouseLocalPosition(long & rlx, long & rly)
{
	CWindowManager::Instance().GetMousePosition(rlx, rly);
	rlx = rlx - m_rect.left;
	rly = rly - m_rect.top;
}

void CWindow::AddChild(CWindow * pWin)
{
	m_pChildList.push_back(pWin);
	pWin->m_pParent = this;
#ifdef INGAME_WIKI
	if (m_isInsideRender && !pWin->m_isInsideRender)
	{
		pWin->SetInsideRender(m_isInsideRender);
	}
#endif
}

#ifdef INGAME_WIKI
void CWindow::SetInsideRender(BOOL flag)
{
	if (!m_pParent || m_isInsideRender && m_pParent->m_isInsideRender)
	{
		return;
	}

	if (m_isInsideRender && flag)
	{
		return;
	}

	m_isInsideRender = flag;
	UpdateRenderBox();
	for (auto it = m_pChildList.begin(); it != m_pChildList.end(); ++it)
	{
		(*it)->SetInsideRender(m_isInsideRender);
	}
}

void CWindow::GetRenderBox(RECT* box)
{
	memcpy(box, &m_renderBox, sizeof(RECT));
}

void CWindow::UpdateTextLineRenderBox()
{
	int width, height;
	((CTextLine*)this)->GetTextSize(&width, &height);

	int pWidth = m_pParent->GetWidth();
	int pHeight = m_pParent->GetHeight();

	if (m_x - m_pParent->m_renderBox.left < 0)
	{
		m_renderBox.left = -m_x + m_pParent->m_renderBox.left;
	}
	else
	{
		m_renderBox.left = 0;
	}

	if (m_y - m_pParent->m_renderBox.top < 0)
	{
		m_renderBox.top = -m_y + m_pParent->m_renderBox.top;
	}
	else
	{
		m_renderBox.top = 0;
	}

	if (m_x + width > pWidth - m_pParent->m_renderBox.right)
	{
		m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;
	}
	else
	{
		m_renderBox.right = 0;
	}

	if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)
	{
		m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;
	}
	else
	{
		m_renderBox.bottom = 0;
	}
}

void CWindow::UpdateRenderBox()
{
	if (!m_isInsideRender || !m_pParent)
	{
		memset(&m_renderBox, 0, sizeof(m_renderBox));
	}
	else
	{
		int width = m_lWidth;
		int height = m_lHeight;
		int pWidth = m_pParent->GetWidth();
		int pHeight = m_pParent->GetHeight();

		if (m_x - m_pParent->m_renderBox.left < 0)
		{
			m_renderBox.left = -m_x + m_pParent->m_renderBox.left;
		}
		else
		{
			m_renderBox.left = 0;
		}

		if (m_y - m_pParent->m_renderBox.top < 0)
		{
			m_renderBox.top = -m_y + m_pParent->m_renderBox.top;
		}
		else
		{
			m_renderBox.top = 0;
		}

		if (m_x + width > pWidth - m_pParent->m_renderBox.right)
		{
			m_renderBox.right = m_x + width - pWidth + m_pParent->m_renderBox.right;
		}
		else
		{
			m_renderBox.right = 0;
		}

		if (m_y + height > pHeight - m_pParent->m_renderBox.bottom)
		{
			m_renderBox.bottom = m_y + height - pHeight + m_pParent->m_renderBox.bottom;
		}
		else
		{
			m_renderBox.bottom = 0;
		}
	}

	OnUpdateRenderBox();
}
#endif

CWindow * CWindow::GetRoot()
{
	if (m_pParent)
		if (m_pParent->IsWindow())
		{
			return m_pParent->GetRoot();
		}

	return this;
}

CWindow * CWindow::GetParent()
{
	return m_pParent;
}

#ifdef INGAME_WIKI
	bool CWindow::IsChild(CWindow* pWin, bool bCheckRecursive)
#else
	bool CWindow::IsChild(CWindow* pWin)
#endif
{
	std::list<CWindow *>::iterator itor = m_pChildList.begin();

	while (itor != m_pChildList.end())
	{
#ifdef INGAME_WIKI
		if (bCheckRecursive)
			if ((*itor)->IsChild(pWin, true))
			{
				return true;
			}
#endif

		++itor;
	}

	return false;
}

void CWindow::DeleteChild(CWindow * pWin)
{
	if (m_isUpdatingChildren)
	{
		m_pReserveChildList.push_back(pWin);
	}
	else
	{
		m_pChildList.remove(pWin);
	}
}

void CWindow::SetTop(CWindow * pWin)
{
	if (!pWin->IsFlag(CWindow::FLAG_FLOAT))
	{
		return;
	}

	TWindowContainer::iterator itor = std::find(m_pChildList.begin(), m_pChildList.end(), pWin);
	if (m_pChildList.end() != itor)
	{
		m_pChildList.push_back(*itor);
		m_pChildList.erase(itor);

		pWin->OnTop();
	}
	else
	{
		TraceError(" CWindow::SetTop - Failed to find child window\n");
	}
}

bool CWindow::IsTop(CWindow * pWin)
{
	if (m_pChildList.size() == 0)
	{
		return false;
	}

	if (*m_pChildList.rbegin() != pWin)
	{
		return false;
	}

	return true;
}
void CWindow::OnMouseDrag(long lx, long ly)
{
	PyCallClassMemberFunc(m_poHandler, "OnMouseDrag", Py_BuildValue("(ii)", lx, ly));
}

void CWindow::OnMoveWindow(long lx, long ly)
{
	PyCallClassMemberFunc(m_poHandler, "OnMoveWindow", Py_BuildValue("(ii)", lx, ly));
}

void CWindow::OnSetFocus()
{
	//PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
	PyCallClassMemberFunc(m_poHandler, "OnSetFocus", BuildEmptyTuple());
}

void CWindow::OnKillFocus()
{
	PyCallClassMemberFunc(m_poHandler, "OnKillFocus", BuildEmptyTuple());
}

void CWindow::OnMouseOverIn()
{
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
}

void CWindow::OnMouseOverOut()
{
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
}

void CWindow::OnMouseOver()
{
	PyCallClassMemberFunc(m_poHandler, "OnMouseMove", BuildEmptyTuple());
}

void CWindow::OnDrop()
{
	PyCallClassMemberFunc(m_poHandler, "OnDrop", BuildEmptyTuple());
}

void CWindow::OnTop()
{
	PyCallClassMemberFunc(m_poHandler, "OnTop", BuildEmptyTuple());
}

void CWindow::OnIMEUpdate()
{
	PyCallClassMemberFunc(m_poHandler, "OnIMEUpdate", BuildEmptyTuple());
}

BOOL CWindow::RunIMETabEvent()
{
	if (!IsRendering())
	{
		return FALSE;
	}

	if (OnIMETabEvent())
	{
		return TRUE;
	}

	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->RunIMETabEvent())
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWindow::RunIMEReturnEvent()
{
	if (!IsRendering())
	{
		return FALSE;
	}

	if (OnIMEReturnEvent())
	{
		return TRUE;
	}

	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->RunIMEReturnEvent())
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CWindow::RunIMEKeyDownEvent(int ikey)
{
	if (!IsRendering())
	{
		return FALSE;
	}

	if (OnIMEKeyDownEvent(ikey))
	{
		return TRUE;
	}

	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->RunIMEKeyDownEvent(ikey))
		{
			return TRUE;
		}
	}

	return FALSE;
}

CWindow * CWindow::RunKeyDownEvent(int ikey)
{
	if (OnKeyDown(ikey))
	{
		return this;
	}

	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->IsShow())
		{
			CWindow * pProcessedWindow = pWindow->RunKeyDownEvent(ikey);
			if (NULL != pProcessedWindow)
			{
				return pProcessedWindow;
			}
		}
	}

	return NULL;
}

BOOL CWindow::RunKeyUpEvent(int ikey)
{
	if (OnKeyUp(ikey))
	{
		return TRUE;
	}

	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->IsShow())
			if (pWindow->RunKeyUpEvent(ikey))
			{
				return TRUE;
			}
	}

	return FALSE;
}


BOOL CWindow::RunPressReturnKeyEvent()
{
	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow* pWindow = *itor;

		if (pWindow->IsShow())
			if (pWindow->RunPressReturnKeyEvent())
			{
				return TRUE;
			}
	}

	if (OnPressReturnKey())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CWindow::RunPressEscapeKeyEvent()
{
	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->IsShow())
			if (pWindow->RunPressEscapeKeyEvent())
			{
				return TRUE;
			}
	}

	if (OnPressEscapeKey())
	{
		return TRUE;
	}

	return FALSE;
}

BOOL CWindow::RunPressExitKeyEvent()
{
	TWindowContainer::reverse_iterator itor;
	for (itor = m_pChildList.rbegin(); itor != m_pChildList.rend(); ++itor)
	{
		CWindow * pWindow = *itor;

		if (pWindow->RunPressExitKeyEvent())
		{
			return TRUE;
		}

		if (pWindow->IsShow())
			if (pWindow->OnPressExitKey())
			{
				return TRUE;
			}
	}

	return FALSE;
}

BOOL CWindow::OnMouseLeftButtonDown()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseLeftButtonUp()
{
	PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonUp", BuildEmptyTuple());
	return TRUE;
}

BOOL CWindow::OnMouseLeftButtonDoubleClick()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseLeftButtonDoubleClick", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseRightButtonDown()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseRightButtonUp()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonUp", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseRightButtonDoubleClick()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseRightButtonDoubleClick", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseMiddleButtonDown()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonDown", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnMouseMiddleButtonUp()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnMouseMiddleButtonUp", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}


#ifdef ENABLE_MOUSE_WHEEL_EVENT
// ScrollBar Support
BOOL CWindow::OnScrollEvent(const int& nLen)
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnScrollWheelEvent", Py_BuildValue("(i)", nLen), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}
#endif

#ifdef ENABLE_MOUSEWHEEL_EVENT
BOOL CWindow::OnMouseWheelScroll(short wDelta)
{
#ifdef _DEBUG
	Tracenf("Mouse Wheel Scroll : wDelta %d ", wDelta);
#endif

	PyCallClassMemberFunc(m_poHandler, "OnMouseWheelScroll", Py_BuildValue("(s)", wDelta > 0 ? "UP" : "DOWN") );
	return m_bIsScrollable;
}


void CWindow::SetScrollable()
{
	m_bIsScrollable = true;
}
#endif


BOOL CWindow::OnIMETabEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMETab", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEReturnEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEReturn", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEUpdateSelectionEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEUpdateSelection", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEKeyDownEvent(int ikey)
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEKeyDown", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEChangeCodePage()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEChangeCodePage", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEOpenCandidateListEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenCandidateList", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMECloseCandidateListEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseCandidateList", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMEOpenReadingWndEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMEOpenReadingWnd", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnIMECloseReadingWndEvent()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnIMECloseReadingWnd", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnKeyDown(int ikey)
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnKeyDown", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnKeyUp(int ikey)
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnKeyUp", Py_BuildValue("(i)", ikey), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnPressReturnKey()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnPressReturnKey", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnPressEscapeKey()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnPressEscapeKey", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

BOOL CWindow::OnPressExitKey()
{
	long lValue;
	if (PyCallClassMemberFunc(m_poHandler, "OnPressExitKey", BuildEmptyTuple(), &lValue))
		if (0 != lValue)
		{
			return TRUE;
		}

	return FALSE;
}

/////

bool CWindow::IsIn(long x, long y)
{
	if (x >= m_rect.left && x <= m_rect.right)
		if (y >= m_rect.top && y <= m_rect.bottom)
		{
			return true;
		}

	return false;
}

bool CWindow::IsIn()
{
	long lx, ly;
	UI::CWindowManager::Instance().GetMousePosition(lx, ly);

	return IsIn(lx, ly);
}

CWindow * CWindow::PickWindow(long x, long y)
{
	std::list<CWindow *>::reverse_iterator ritor = m_pChildList.rbegin();
	for (; ritor != m_pChildList.rend(); ++ritor)
	{
		CWindow * pWin = *ritor;
		if (pWin->IsShow())
		{
			if (!pWin->IsFlag(CWindow::FLAG_IGNORE_SIZE))
			{
				if (!pWin->IsIn(x, y))
				{
					if (0L <= pWin->GetWidth())
					{
						continue;
					}
				}
			}

			CWindow * pResult = pWin->PickWindow(x, y);
			if (pResult)
			{
				return pResult;
			}
		}
	}

	if (IsFlag(CWindow::FLAG_NOT_PICK))
	{
		return NULL;
	}

	return (this);
}

CWindow * CWindow::PickTopWindow(long x, long y)
{
	std::list<CWindow *>::reverse_iterator ritor = m_pChildList.rbegin();
	for (; ritor != m_pChildList.rend(); ++ritor)
	{
		CWindow * pWin = *ritor;
		if (pWin->IsShow())
			if (pWin->IsIn(x, y))
				if (!pWin->IsFlag(CWindow::FLAG_NOT_PICK))
				{
					return pWin;
				}
	}

	return NULL;
}

#ifdef INGAME_WIKI
void CWindow::iSetRenderingRect(int iLeft, int iTop, int iRight, int iBottom)
{
	m_renderingRect.left = iLeft;
	m_renderingRect.top = iTop;
	m_renderingRect.right = iRight;
	m_renderingRect.bottom = iBottom;

	OnSetRenderingRect();
}

void CWindow::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	float fWidth = float(GetWidth());
	float fHeight = float(GetHeight());
	if (IsType(CTextLine::Type()))
	{
		int iWidth, iHeight;
		((CTextLine*)this)->GetTextSize(&iWidth, &iHeight);
		fWidth = float(iWidth);
		fHeight = float(iHeight);
	}

	iSetRenderingRect(fWidth * fLeft, fHeight * fTop, fWidth * fRight, fHeight * fBottom);
}

int CWindow::GetRenderingWidth()
{
	return max(0, GetWidth() + m_renderingRect.right + m_renderingRect.left);
}

int CWindow::GetRenderingHeight()
{
	return max(0, GetHeight() + m_renderingRect.bottom + m_renderingRect.top);
}

void CWindow::ResetRenderingRect(bool bCallEvent)
{
	m_renderingRect.bottom = m_renderingRect.left = m_renderingRect.right = m_renderingRect.top = 0;

	if (bCallEvent)
	{
		OnSetRenderingRect();
	}
}

void CWindow::OnSetRenderingRect()
{
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

#ifdef INGAME_WIKI
CUiWikiRenderTarget::~CUiWikiRenderTarget() = default;
CUiWikiRenderTarget::CUiWikiRenderTarget(PyObject * ppyObject) :
	CWindow(ppyObject),
	m_dwIndex(-1) {}

/*----------------------------
--------PUBLIC CLASS FUNCTIONS
-----------------------------*/

bool CUiWikiRenderTarget::SetWikiRenderTargetModule(int iRenderTargetModule)
{
	if (!CWikiRenderTargetManager::Instance().GetRenderTarget(iRenderTargetModule))
	{
		if (!CWikiRenderTargetManager::Instance().CreateRenderTarget(iRenderTargetModule, GetWidth(), GetHeight()))
		{
			TraceError("CWikiRenderTargetManager could not create the texture. w %d h %d", GetWidth(), GetHeight());
			return false;
		}
	}

	m_dwIndex = iRenderTargetModule;

	UpdateRect();
	return true;
}

void CUiWikiRenderTarget::OnUpdateRenderBox()
{
	if (m_dwIndex == -1 /*(CPythonWikiRenderTarget::START_MODULE)*/)
	{
		return;
	}

	auto target = CWikiRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);
	if (!target)
	{
		return;
	}

	target->SetRenderingBox(&m_renderBox);
}

/*----------------------------
-----PROTECTED CLASS FUNCTIONS
-----------------------------*/

void CUiWikiRenderTarget::OnRender()
{
	if (m_dwIndex == -1 /*(CPythonWikiRenderTarget::START_MODULE)*/)
	{
		return;
	}

	auto target = CWikiRenderTargetManager::Instance().GetRenderTarget(m_dwIndex);
	if (!target)
	{
		return;
	}

	target->SetRenderingRect(&m_rect);
	target->RenderTexture();
}
#endif

CBox::CBox(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
{
}
CBox::~CBox()
{
}

void CBox::SetColor(DWORD dwColor)
{
	m_dwColor = dwColor;
}

void CBox::OnRender()
{
	CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
	CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CBar::CBar(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
{
}
CBar::~CBar()
{
}

void CBar::SetColor(DWORD dwColor)
{
	m_dwColor = dwColor;
}

void CBar::OnRender()
{
	CPythonGraphic::Instance().SetDiffuseColor(m_dwColor);
#ifdef INGAME_WIKI
	CPythonGraphic::Instance().RenderBar2d(m_rect.left + m_renderBox.left, m_rect.top + m_renderBox.top, m_rect.right - m_renderBox.right, m_rect.bottom - m_renderBox.bottom);
#else
	CPythonGraphic::Instance().RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CLine::CLine(PyObject * ppyObject) : CWindow(ppyObject), m_dwColor(0xff000000)
{
}
CLine::~CLine()
{
}

void CLine::SetColor(DWORD dwColor)
{
	m_dwColor = dwColor;
}

void CLine::OnRender()
{
	CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();
	rkpyGraphic.SetDiffuseColor(m_dwColor);
	rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CBar3D::Type()
{
	static DWORD s_dwType = GetCRC32("CBar3D", strlen("CBar3D"));
	return (s_dwType);
}

CBar3D::CBar3D(PyObject * ppyObject) : CWindow(ppyObject)
{
	m_dwLeftColor = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_dwRightColor = D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f);
	m_dwCenterColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
}
CBar3D::~CBar3D()
{
}

void CBar3D::SetColor(DWORD dwLeft, DWORD dwRight, DWORD dwCenter)
{
	m_dwLeftColor = dwLeft;
	m_dwRightColor = dwRight;
	m_dwCenterColor = dwCenter;
}

void CBar3D::OnRender()
{
	CPythonGraphic & rkpyGraphic = CPythonGraphic::Instance();

	rkpyGraphic.SetDiffuseColor(m_dwCenterColor);
	rkpyGraphic.RenderBar2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);

	rkpyGraphic.SetDiffuseColor(m_dwLeftColor);
	rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.right, m_rect.top);
	rkpyGraphic.RenderLine2d(m_rect.left, m_rect.top, m_rect.left, m_rect.bottom);

	rkpyGraphic.SetDiffuseColor(m_dwRightColor);
	rkpyGraphic.RenderLine2d(m_rect.left, m_rect.bottom, m_rect.right, m_rect.bottom);
	rkpyGraphic.RenderLine2d(m_rect.right, m_rect.top, m_rect.right, m_rect.bottom);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////


BOOL CTextLine::OnIsType(DWORD dwType)
{
	if (CTextLine::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}
CTextLine::CTextLine(PyObject * ppyObject) : CWindow(ppyObject)
{
	m_TextInstance.SetColor(0.78f, 0.78f, 0.78f);
	m_TextInstance.SetHorizonalAlign(CGraphicTextInstance::HORIZONTAL_ALIGN_LEFT);
	m_TextInstance.SetVerticalAlign(CGraphicTextInstance::VERTICAL_ALIGN_TOP);
}
CTextLine::~CTextLine()
{
	m_TextInstance.Destroy();
}

#ifdef INGAME_WIKI
DWORD CTextLine::Type()
{
	static DWORD s_dwType = GetCRC32("CTextLine", strlen("CTextLine"));
	return (s_dwType);
}
#endif

void CTextLine::SetMax(int iMax)
{
	m_TextInstance.SetMax(iMax);
}
void CTextLine::SetHorizontalAlign(int iType)
{
	m_TextInstance.SetHorizonalAlign(iType);
}
void CTextLine::SetVerticalAlign(int iType)
{
	m_TextInstance.SetVerticalAlign(iType);
}
void CTextLine::SetSecret(BOOL bFlag)
{
	m_TextInstance.SetSecret(bFlag ? true : false);
}
void CTextLine::SetOutline(BOOL bFlag)
{
	m_TextInstance.SetOutline(bFlag ? true : false);
}
void CTextLine::SetFeather(BOOL bFlag)
{
	m_TextInstance.SetFeather(bFlag ? true : false);
}
void CTextLine::SetMultiLine(BOOL bFlag)
{
	m_TextInstance.SetMultiLine(bFlag ? true : false);
}
void CTextLine::SetFontName(const char * c_szFontName)
{
	std::string stFontName = c_szFontName;
	stFontName += ".fnt";

	CResourceManager& rkResMgr = CResourceManager::Instance();
	CResource* pkRes = rkResMgr.GetTypeResourcePointer(stFontName.c_str());
	CGraphicText* pkResFont = static_cast<CGraphicText*>(pkRes);
	m_TextInstance.SetTextPointer(pkResFont);
}
void CTextLine::SetFontColor(DWORD dwColor)
{
	m_TextInstance.SetColor(dwColor);
}
void CTextLine::SetLimitWidth(float fWidth)
{
	m_TextInstance.SetLimitWidth(fWidth);
}
void CTextLine::SetText(const char * c_szText)
{
	OnSetText(c_szText);
}
void CTextLine::GetTextSize(int* pnWidth, int* pnHeight)
{
	m_TextInstance.GetTextSize(pnWidth, pnHeight);
}
void CTextLine::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	m_TextInstance.SetRenderingRect(fLeft, fTop, fRight, fBottom);
}
//void CTextLine::SetFlagRenderingWiki(bool val)
//{
//	m_TextInstance.SetRenderingWiki(val);
//}

const char * CTextLine::GetText()
{
	return m_TextInstance.GetValueStringReference().c_str();
}
int CTextLine::GetSpecificTextWidth(const char* c_szText)
{
	return m_TextInstance.GetSpecificTextWidth(c_szText);
}
void CTextLine::ShowCursor()
{
	m_TextInstance.ShowCursor();
}
void CTextLine::HideCursor()
{
	m_TextInstance.HideCursor();
}
#ifdef INGAME_WIKI
bool CTextLine::IsShowCursor()
{
	return m_TextInstance.IsShowCursor();
}
#endif
int CTextLine::GetCursorPosition()
{
	long lx, ly;
	CWindow::GetMouseLocalPosition(lx, ly);
	return m_TextInstance.PixelPositionToCharacterPosition(lx);
}
int CTextLine::GetMultilineCursorPosition()
{
	long lx, ly;
	CWindow::GetMouseLocalPosition(lx, ly);
	return m_TextInstance.PixelPositionToCharacterPosition(lx, ly);
}
std::tuple<int, int, int> CTextLine::GetCharPositionByCursor(int cursorPos)
{
	return m_TextInstance.CharacterPositionToPixelPosition(cursorPos);
}
int CTextLine::TextPositionToRenderPosition(int textPos)
{
	return m_TextInstance.TextPositionToRenderPosition(textPos);
}
void CTextLine::OnSetText(const char * c_szText)
{
	m_TextInstance.SetValue(c_szText);
	m_TextInstance.Update();
#ifdef INGAME_WIKI
	if (m_isInsideRender)
	{
		UpdateRenderBoxRecursive();
	}
#endif
}
#ifdef INGAME_WIKI
bool CTextLine::IsShow()
{
	if (!m_bShow)
	{
		return false;
	}

	if (m_isInsideRender)
	{
		int cW, cH;
		GetTextSize(&cW, &cH);
		if (m_renderBox.left + m_renderBox.right >= cW || m_renderBox.top + m_renderBox.bottom >= cH)
		{
			return false;
		}
	}

	return true;
}
#endif
void CTextLine::OnUpdate()
{
	if (IsShow())
	{
		m_TextInstance.Update();
	}

	CWindow::OnUpdate();
}
void CTextLine::OnRender()
{
	PyCallClassMemberFunc(m_poHandler, "OnRenderSelected", BuildEmptyTuple());

	RECT* clipRect = nullptr;
	if (m_clipRect.left != m_clipRect.right || m_clipRect.top != m_clipRect.bottom)
		clipRect = &m_clipRect;

	if (IsShow())
	{
		m_TextInstance.Render(clipRect);
	}

	CWindow::OnRender();
}
#ifdef INGAME_WIKI
int CTextLine::GetRenderingWidth()
{
	int iTextWidth, iTextHeight;
	GetTextSize(&iTextWidth, &iTextHeight);

	return iTextWidth + m_renderingRect.right + m_renderingRect.left;
}

int CTextLine::GetRenderingHeight()
{
	int iTextWidth, iTextHeight;
	GetTextSize(&iTextWidth, &iTextHeight);

	return iTextHeight + m_renderingRect.bottom + m_renderingRect.top;
}

void CTextLine::OnSetRenderingRect()
{
	int iTextWidth, iTextHeight;
	GetTextSize(&iTextWidth, &iTextHeight);

	m_TextInstance.iSetRenderingRect(m_renderingRect.left, -m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
}
#endif
void CTextLine::OnChangePosition()
{
	// FOR_ARABIC_ALIGN
	//if (m_TextInstance.GetHorizontalAlign() == CGraphicTextInstance::HORIZONTAL_ALIGN_ARABIC)
	{
		m_TextInstance.SetPosition(m_rect.left, m_rect.top);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CNumberLine::Type()
{
	static DWORD s_dwType = GetCRC32("CNumberLine", strlen("CNumberLine"));
	return (s_dwType);
}

BOOL CNumberLine::OnIsType(DWORD dwType)
{
	if (CNumberLine::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}

CNumberLine::CNumberLine(PyObject * ppyObject) : CWindow(ppyObject)
{
	m_strPath = "d:/ymir work/ui/game/taskbar/";
	m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
	m_iVerticalAlign = VERTICAL_ALIGN_TOP;
	m_dwWidthSummary = 0;
	m_dwMaxHeight = 0;
	m_diffuseColor.r = m_diffuseColor.g = m_diffuseColor.b = m_diffuseColor.a = 1.0f;
}
CNumberLine::CNumberLine(CWindow * pParent) : CWindow(NULL)
{
	m_strPath = "d:/ymir work/ui/game/taskbar/";
	m_iHorizontalAlign = HORIZONTAL_ALIGN_LEFT;
	m_iVerticalAlign = VERTICAL_ALIGN_TOP;
	m_dwWidthSummary = 0;
	m_dwMaxHeight = 0;
	m_diffuseColor.r = m_diffuseColor.g = m_diffuseColor.b = m_diffuseColor.a = 1.0f;

	m_pParent = pParent;
}
CNumberLine::~CNumberLine()
{
	ClearNumber();
}

void CNumberLine::SetPath(const char * c_szPath)
{
	m_strPath = c_szPath;
}
void CNumberLine::SetHorizontalAlign(int iType)
{
	m_iHorizontalAlign = iType;
}
void CNumberLine::SetVerticalAlign(int iType)
{
	m_iVerticalAlign = iType;
}
void CNumberLine::SetNumber(const char * c_szNumber)
{
	if (0 == m_strNumber.compare(c_szNumber))
	{
		return;
	}

	ClearNumber();

	m_strNumber = c_szNumber;

	for (DWORD i = 0; i < m_strNumber.size(); ++i)
	{
		char cChar = m_strNumber[i];
		std::string strImageFileName;

		if (':' == cChar)
		{
			strImageFileName = m_strPath + "colon.sub";
		}
		else if ('?' == cChar)
		{
			strImageFileName = m_strPath + "questionmark.sub";
		}
		else if ('/' == cChar)
		{
			strImageFileName = m_strPath + "slash.sub";
		}
		else if ('%' == cChar)
		{
			strImageFileName = m_strPath + "percent.sub";
		}
		else if ('+' == cChar)
		{
			strImageFileName = m_strPath + "plus.sub";
		}
		else if ('m' == cChar)
		{
			strImageFileName = m_strPath + "m.sub";
		}
		else if ('g' == cChar)
		{
			strImageFileName = m_strPath + "g.sub";
		}
		else if ('p' == cChar)
		{
			strImageFileName = m_strPath + "p.sub";
		}
		else if (cChar >= '0' && cChar <= '9')
		{
			strImageFileName = m_strPath;
			strImageFileName += cChar;
			strImageFileName += ".sub";
		}
		else
		{
			continue;
		}

		if (!CResourceManager::Instance().IsFileExist(strImageFileName.c_str()))
		{
			continue;
		}

		CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(strImageFileName.c_str());

		CGraphicImageInstance * pInstance = CGraphicImageInstance::New();
		pInstance->SetImagePointer(pImage);
		pInstance->SetDiffuseColor(m_diffuseColor.r, m_diffuseColor.g, m_diffuseColor.b, m_diffuseColor.a);
		m_ImageInstanceVector.push_back(pInstance);

		m_dwWidthSummary += pInstance->GetWidth();
		m_dwMaxHeight = max(m_dwMaxHeight, pInstance->GetHeight());
	}

	OnChangePosition();
}

void CNumberLine::SetDiffuseColor(float r, float g, float b, float a)
{
	m_diffuseColor.r = r;
	m_diffuseColor.g = g;
	m_diffuseColor.b = b;
	m_diffuseColor.a = a;

	for (CGraphicImageInstance* pInst : m_ImageInstanceVector)
	{
		pInst->SetDiffuseColor(m_diffuseColor.r, m_diffuseColor.g, m_diffuseColor.b, m_diffuseColor.a);
	}
}

void CNumberLine::ClearNumber()
{
	m_ImageInstanceVector.clear();
	m_dwWidthSummary = 0;
	m_dwMaxHeight = 0;
	m_strNumber = "";
}

void CNumberLine::OnRender()
{
	for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
	{
		m_ImageInstanceVector[i]->Render();
	}
}

void CNumberLine::OnChangePosition()
{
	int ix = m_x;
	int iy = m_y;

	if (m_pParent)
	{
		ix = m_rect.left;
		iy = m_rect.top;
	}

	switch (m_iHorizontalAlign)
	{
	case HORIZONTAL_ALIGN_LEFT:
		break;
	case HORIZONTAL_ALIGN_CENTER:
		ix -= int(m_dwWidthSummary) / 2;
		break;
	case HORIZONTAL_ALIGN_RIGHT:
		ix -= int(m_dwWidthSummary);
		break;
	}

	switch (m_iVerticalAlign)
	{
	case VERTICAL_ALIGN_TOP:
		break;
	case VERTICAL_ALIGN_CENTER:
		iy -= int(m_dwMaxHeight) / 2;
		break;
	case VERTICAL_ALIGN_BOTTOM:
		iy -= int(m_dwMaxHeight);
		break;
	}
	for (DWORD i = 0; i < m_ImageInstanceVector.size(); ++i)
	{
		m_ImageInstanceVector[i]->SetPosition(ix, iy);
		ix += m_ImageInstanceVector[i]->GetWidth();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CImageBox::Type()
{
	static DWORD s_dwType = GetCRC32("CImageBox", strlen("CImageBox"));
	return (s_dwType);
}

BOOL CImageBox::OnIsType(DWORD dwType)
{
	if (CImageBox::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}
CImageBox::CImageBox(PyObject * ppyObject) : CWindow(ppyObject)
{
	m_pImageInstance = NULL;

	fCoolTime = 0.0f;
	fStartCoolTime = 0.0f;
}
CImageBox::~CImageBox()
{
	OnDestroyInstance();
}

void CImageBox::OnCreateInstance()
{
	OnDestroyInstance();

	m_pImageInstance = CGraphicImageInstance::New();
}
void CImageBox::OnDestroyInstance()
{
	if (m_pImageInstance)
	{
		CGraphicImageInstance::Delete(m_pImageInstance);
		m_pImageInstance = NULL;
	}
}

BOOL CImageBox::LoadImage(const char * c_szFileName)
{
	if (!c_szFileName[0])
	{
		return FALSE;
	}

	OnCreateInstance();

	CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource)
	{
		return FALSE;
	}
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return FALSE;
	}

	m_pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (m_pImageInstance->IsEmpty())
	{
		return FALSE;
	}

	SetSize(m_pImageInstance->GetWidth(), m_pImageInstance->GetHeight());
	UpdateRect();

	return TRUE;
}

void CImageBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	if (!m_pImageInstance)
	{
		return;
	}

	m_pImageInstance->SetDiffuseColor(fr, fg, fb, fa);
}

int CImageBox::GetWidth()
{
	if (!m_pImageInstance)
	{
		return 0;
	}

	return m_pImageInstance->GetWidth();
}

int CImageBox::GetHeight()
{
	if (!m_pImageInstance)
	{
		return 0;
	}

	return m_pImageInstance->GetHeight();
}

void CImageBox::OnUpdate()
{
}
void CImageBox::OnRender()
{
	if (!m_pImageInstance)
	{
		return;
	}

	if (IsShow())
	{
		m_pImageInstance->Render();
	}

	if (fCoolTime != 0.0f)
	{
		float fcurTime = CTimer::Instance().GetCurrentSecond();
		float fPercentage = (fcurTime - fStartCoolTime) / fCoolTime;
		CPythonGraphic::Instance().RenderCoolTimeBox(m_rect.left + GetWidth() + 16.0f, m_rect.top + GetHeight() + 16.0f, 16.0f, fPercentage);

		if (fcurTime - fStartCoolTime >= fCoolTime)
		{
			fCoolTime = 0.0f;
			fStartCoolTime = 0.0f;
		}
	}
}
void CImageBox::OnChangePosition()
{
	if (!m_pImageInstance)
	{
		return;
	}

	m_pImageInstance->SetPosition(m_rect.left, m_rect.top);
}

void CImageBox::SetCoolTimeImageBox(float CT)
{
	if (!m_pImageInstance)
	{
		return;
	}

	fCoolTime = CT;
}

void CImageBox::SetStartCoolTimeImageBox(float SCT)
{
	if (!m_pImageInstance)
	{
		return;
	}

	fStartCoolTime = SCT;
}

#ifdef ENABLE_RT_EXTENSION
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CRenderTarget::CRenderTarget(PyObject* object)
	: CImageBox(object)
{}

void CRenderTarget::SetRenderTarget(uint32_t race)
{
	renderTarget_.SetRenderTarget(race);
}

void CRenderTarget::SetRenderHair(uint32_t vnum)
{
	renderTarget_.SetRenderHair(vnum);
}

void CRenderTarget::SetRenderArmor(uint32_t vnum)
{
	renderTarget_.SetRenderArmor(vnum);
}

void CRenderTarget::SetRenderWeapon(uint32_t vnum)
{
	renderTarget_.SetRenderWeapon(vnum);
}

#ifdef ENABLE_SASH_COSTUME_SYSTEM
void CRenderTarget::SetRenderSash(uint32_t vnum)
{
	renderTarget_.SetRenderSash(vnum);
}
#endif

#ifdef ENABLE_SHINING_SYSTEM
void CRenderTarget::SetRenderToggleShining(DWORD flags[TOGGLE_SHINING_FLAG_32CNT])
{
	renderTarget_.SetRenderToggleShining(flags);
}
#endif

void CRenderTarget::SetRendererMotion(uint32_t vnum)
{
	renderTarget_.SetRendererMotion(vnum);
}

void CRenderTarget::SetRenderDistance(float distance)
{
	renderTarget_.SetRenderDistance(distance);
}

float CRenderTarget::GetRenderDistance()
{
	return renderTarget_.GetRenderDistance();
}

void CRenderTarget::SetLightPosition(D3DXVECTOR3 position)
{
	renderTarget_.SetLightPosition(position);
}

const D3DXVECTOR3& CRenderTarget::GetLightPosition() const
{
	return renderTarget_.GetLightPosition();
}

void CRenderTarget::SetRotation(float rotation)
{
	renderTarget_.SetRotation(rotation);
}

float CRenderTarget::GetRotation()
{
	return renderTarget_.GetRotation();
}

void CRenderTarget::SetRotationMode(bool enable)
{
	renderTarget_.SetRotationMode(enable);
}

void CRenderTarget::DestroyRender()
{
	renderTarget_.Destroy();
}

void CRenderTarget::OnUpdate()
{
	CImageBox::OnUpdate();

	renderTarget_.Update();
}

void CRenderTarget::OnRender()
{
	CImageBox::OnRender();

	const auto& globalPosition = GetRect();
	renderTarget_.Render(globalPosition.left, globalPosition.top,
						 globalPosition.right - globalPosition.left,
						 globalPosition.bottom - globalPosition.top);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CMarkBox::CMarkBox(PyObject * ppyObject) : CWindow(ppyObject)
{
	m_pMarkInstance = NULL;
}

CMarkBox::~CMarkBox()
{
	OnDestroyInstance();
}

void CMarkBox::OnCreateInstance()
{
	OnDestroyInstance();
	m_pMarkInstance = CGraphicMarkInstance::New();
}

void CMarkBox::OnDestroyInstance()
{
	if (m_pMarkInstance)
	{
		CGraphicMarkInstance::Delete(m_pMarkInstance);
		m_pMarkInstance = NULL;
	}
}

void CMarkBox::LoadImage(const char * c_szFilename)
{
	OnCreateInstance();

	m_pMarkInstance->SetImageFileName(c_szFilename);
	m_pMarkInstance->Load();
	SetSize(m_pMarkInstance->GetWidth(), m_pMarkInstance->GetHeight());

	UpdateRect();
}

void CMarkBox::SetScale(FLOAT fScale)
{
	if (!m_pMarkInstance)
	{
		return;
	}

	m_pMarkInstance->SetScale(fScale);
}

void CMarkBox::SetIndex(UINT uIndex)
{
	if (!m_pMarkInstance)
	{
		return;
	}

	m_pMarkInstance->SetIndex(uIndex);
}

void CMarkBox::SetDiffuseColor(float fr, float fg, float fb, float fa)
{
	if (!m_pMarkInstance)
	{
		return;
	}

	m_pMarkInstance->SetDiffuseColor(fr, fg, fb, fa);
}

void CMarkBox::OnUpdate()
{
}

void CMarkBox::OnRender()
{
	if (!m_pMarkInstance)
	{
		return;
	}

	if (IsShow())
	{
		m_pMarkInstance->Render();
	}
}

void CMarkBox::OnChangePosition()
{
	if (!m_pMarkInstance)
	{
		return;
	}

	m_pMarkInstance->SetPosition(m_rect.left, m_rect.top);
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CExpandedImageBox::Type()
{
	static DWORD s_dwType = GetCRC32("CExpandedImageBox", strlen("CExpandedImageBox"));
	return (s_dwType);
}

BOOL CExpandedImageBox::OnIsType(DWORD dwType)
{
	if (CExpandedImageBox::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}

CExpandedImageBox::CExpandedImageBox(PyObject * ppyObject) : CImageBox(ppyObject)
{
}
CExpandedImageBox::~CExpandedImageBox()
{
	OnDestroyInstance();
}

void CExpandedImageBox::OnCreateInstance()
{
	OnDestroyInstance();

	m_pImageInstance = CGraphicExpandedImageInstance::New();
}
void CExpandedImageBox::OnDestroyInstance()
{
	if (m_pImageInstance)
	{
		CGraphicExpandedImageInstance::Delete((CGraphicExpandedImageInstance*)m_pImageInstance);
		m_pImageInstance = NULL;
	}
}

void CExpandedImageBox::SetScale(float fx, float fy)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetScale(fx, fy);
	CWindow::SetSize(long(float(GetWidth())*fx), long(float(GetHeight())*fy));
}
void CExpandedImageBox::SetOrigin(float fx, float fy)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetOrigin(fx, fy);
}
void CExpandedImageBox::SetRotation(float fRotation)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRotation(fRotation);
}
void CExpandedImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingRect(fLeft, fTop, fRight, fBottom);
}

void CExpandedImageBox::SetRenderingMode(int iMode)
{
	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderingMode(iMode);
}

#ifdef INGAME_WIKI
int CExpandedImageBox::GetRenderingWidth()
{
	return CWindow::GetWidth() + m_renderingRect.right + m_renderingRect.left;
}

int CExpandedImageBox::GetRenderingHeight()
{
	return CWindow::GetHeight() + m_renderingRect.bottom + m_renderingRect.top;
}

void CExpandedImageBox::OnSetRenderingRect()
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
}

void CExpandedImageBox::SetExpandedRenderingRect(float fLeftTop, float fLeftBottom, float fTopLeft, float fTopRight, float fRightTop, float fRightBottom, float fBottomLeft, float fBottomRight)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetExpandedRenderingRect(fLeftTop, fLeftBottom, fTopLeft, fTopRight, fRightTop, fRightBottom, fBottomLeft, fBottomRight);
}

void CExpandedImageBox::SetTextureRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetTextureRenderingRect(fLeft, fTop, fRight, fBottom);
}

DWORD CExpandedImageBox::GetPixelColor(DWORD x, DWORD y)
{
	return ((CGraphicExpandedImageInstance*)m_pImageInstance)->GetPixelColor(x, y);
}

void CExpandedImageBox::OnUpdateRenderBox()
{
	if (!m_pImageInstance)
	{
		return;
	}

	((CGraphicExpandedImageInstance*)m_pImageInstance)->SetRenderBox(m_renderBox);
}
#endif


void CExpandedImageBox::OnUpdate()
{
}
void CExpandedImageBox::OnRender()
{
	if (!m_pImageInstance)
	{
		return;
	}

	if (IsShow())
	{
		m_pImageInstance->Render();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CAniImageBox::Type()
{
	static DWORD s_dwType = GetCRC32("CAniImageBox", strlen("CAniImageBox"));
	return (s_dwType);
}

BOOL CAniImageBox::OnIsType(DWORD dwType)
{
	if (CAniImageBox::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}

CAniImageBox::CAniImageBox(PyObject * ppyObject)
	:	CWindow(ppyObject),
	  m_bycurDelay(0),
	  m_byDelay(4),
	  m_bycurIndex(0)
{
	m_ImageVector.clear();
}
CAniImageBox::~CAniImageBox()
{
	for_each(m_ImageVector.begin(), m_ImageVector.end(), CGraphicExpandedImageInstance::DeleteExpandedImageInstance);
}

void CAniImageBox::SetDelay(int iDelay)
{
	m_byDelay = iDelay;
}

void CAniImageBox::AppendImage(const char * c_szFileName)
{
	CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return;
	}

	CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();

	pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (pImageInstance->IsEmpty())
	{
		CGraphicExpandedImageInstance::Delete(pImageInstance);
		return;
	}

	m_ImageVector.push_back(pImageInstance);

	m_bycurIndex = static_cast<BYTE>(rand() % m_ImageVector.size());
	SetSize(pImageInstance->GetWidth(), pImageInstance->GetHeight());
	UpdateRect();
}

struct FSetRenderingRect
{
	float fLeft, fTop, fRight, fBottom;
	void operator () (CGraphicExpandedImageInstance * pInstance)
	{
		pInstance->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	}
};
void CAniImageBox::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	FSetRenderingRect setRenderingRect;
	setRenderingRect.fLeft = fLeft;
	setRenderingRect.fTop = fTop;
	setRenderingRect.fRight = fRight;
	setRenderingRect.fBottom = fBottom;
	for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingRect);
}

struct FSetRenderingMode
{
	int iMode;
	void operator () (CGraphicExpandedImageInstance * pInstance)
	{
		pInstance->SetRenderingMode(iMode);
	}
};
void CAniImageBox::SetRenderingMode(int iMode)
{
	FSetRenderingMode setRenderingMode;
	setRenderingMode.iMode = iMode;
	for_each(m_ImageVector.begin(), m_ImageVector.end(), setRenderingMode);
}

void CAniImageBox::ResetFrame()
{
	m_bycurIndex = 0;
}

long CAniImageBox::GetRealWidth()
{
	if (m_ImageVector.size() == 0)
	{
		return GetWidth();
	}

	return m_ImageVector[0]->GetWidth();
}

long CAniImageBox::GetRealHeight()
{
	if (m_ImageVector.size() == 0)
	{
		return GetHeight();
	}

	return m_ImageVector[0]->GetHeight();
}
void CAniImageBox::OnUpdate()
{
	++m_bycurDelay;
	if (m_bycurDelay < m_byDelay)
	{
		return;
	}

	m_bycurDelay = 0;

	OnKeyFrame(m_bycurIndex);

	++m_bycurIndex;
	if (m_bycurIndex >= m_ImageVector.size())
	{
		m_bycurIndex = 0;

		OnEndFrame();
	}
}
void CAniImageBox::OnRender()
{
	if (m_bycurIndex < m_ImageVector.size())
	{
		CGraphicExpandedImageInstance * pImage = m_ImageVector[m_bycurIndex];
		pImage->Render();
	}
}

struct FChangePosition
{
	float fx, fy;
	void operator () (CGraphicExpandedImageInstance * pInstance)
	{
		pInstance->SetPosition(fx, fy);
	}
};

void CAniImageBox::OnChangePosition()
{
	FChangePosition changePosition;
	changePosition.fx = m_rect.left;
	changePosition.fy = m_rect.top;
	for_each(m_ImageVector.begin(), m_ImageVector.end(), changePosition);
}

void CAniImageBox::OnEndFrame()
{
	PyCallClassMemberFunc(m_poHandler, "OnEndFrame", BuildEmptyTuple());
}

void CAniImageBox::OnKeyFrame(BYTE bFrame)
{
	PyCallClassMemberFunc(m_poHandler, "OnKeyFrame", Py_BuildValue("(i)", bFrame));
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

DWORD CButton::Type()
{
	static DWORD s_dwType = GetCRC32("CButton", strlen("CButton"));
	return (s_dwType);
}

BOOL CButton::OnIsType(DWORD dwType)
{
	if (CButton::Type() == dwType)
	{
		return TRUE;
	}

	return FALSE;
}

CButton::CButton(PyObject * ppyObject)
	:	CWindow(ppyObject),
	  m_pcurVisual(NULL),
	  m_bEnable(TRUE),
	  m_isPressed(FALSE),
	  m_isFlash(FALSE)
{
	CWindow::AddFlag(CWindow::FLAG_NOT_CAPTURE);
}
CButton::~CButton()
{
}

BOOL CButton::SetUpVisual(const char * c_szFileName)
{
	CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return FALSE;
	}

	m_upVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (m_upVisual.IsEmpty())
	{
		return FALSE;
	}

	SetSize(m_upVisual.GetWidth(), m_upVisual.GetHeight());
	//
	SetCurrentVisual(&m_upVisual);
	//

	return TRUE;
}
BOOL CButton::SetOverVisual(const char * c_szFileName)
{
	CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return FALSE;
	}

	m_overVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (m_overVisual.IsEmpty())
	{
		return FALSE;
	}

	SetSize(m_overVisual.GetWidth(), m_overVisual.GetHeight());

	return TRUE;
}
BOOL CButton::SetDownVisual(const char * c_szFileName)
{
	CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return FALSE;
	}

	m_downVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (m_downVisual.IsEmpty())
	{
		return FALSE;
	}

	SetSize(m_downVisual.GetWidth(), m_downVisual.GetHeight());

	return TRUE;
}
BOOL CButton::SetDisableVisual(const char * c_szFileName)
{
	CResource * pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return FALSE;
	}

	m_disableVisual.SetImagePointer(static_cast<CGraphicImage*>(pResource));
	if (m_downVisual.IsEmpty())
	{
		return FALSE;
	}

	SetSize(m_disableVisual.GetWidth(), m_disableVisual.GetHeight());

	return TRUE;
}

const char * CButton::GetUpVisualFileName()
{
	return m_upVisual.GetGraphicImagePointer()->GetFileName();
}
const char * CButton::GetOverVisualFileName()
{
	return m_overVisual.GetGraphicImagePointer()->GetFileName();
}
const char * CButton::GetDownVisualFileName()
{
	return m_downVisual.GetGraphicImagePointer()->GetFileName();
}

void CButton::Flash()
{
	m_isFlash = TRUE;
}

void CButton::Enable()
{
	if (!IsDisable())
	{
		SetUp();
		m_bEnable = TRUE;
	}

}

void CButton::Disable()
{
	if (IsDisable())
	{
		m_bEnable = FALSE;
		if (!m_disableVisual.IsEmpty())
		{
			SetCurrentVisual(&m_disableVisual);
		}
	}
}

BOOL CButton::IsDisable()
{
	return m_bEnable;
}

void CButton::SetUp()
{
	SetCurrentVisual(&m_upVisual);
	m_isPressed = FALSE;
}
void CButton::Up()
{
	if (IsIn())
	{
		SetCurrentVisual(&m_overVisual);
	}
	else
	{
		SetCurrentVisual(&m_upVisual);
	}

	PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
}
void CButton::Over()
{
	SetCurrentVisual(&m_overVisual);
}
void CButton::Down()
{
	m_isPressed = TRUE;
	SetCurrentVisual(&m_downVisual);
	PyCallClassMemberFunc(m_poHandler, "DownEvent", BuildEmptyTuple());
}

void CButton::OnUpdate()
{
}
void CButton::OnRender()
{
	if (!IsShow())
	{
		return;
	}

	if (m_pcurVisual)
	{
		if (m_isFlash)
			if (!IsIn())
				if (int(timeGetTime() / 500) % 2)
				{
					return;
				}

		m_pcurVisual->Render();
	}

	PyCallClassMemberFunc(m_poHandler, "OnRender", BuildEmptyTuple());
}
void CButton::OnChangePosition()
{
	if (m_pcurVisual)
	{
		m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
	}
}

BOOL CButton::OnMouseLeftButtonDown()
{
	if (!IsEnable())
	{
		return TRUE;
	}

	m_isPressed = TRUE;
	Down();

	return TRUE;
}
BOOL CButton::OnMouseLeftButtonDoubleClick()
{
	if (!IsEnable())
	{
		return TRUE;
	}

	OnMouseLeftButtonDown();

	return TRUE;
}
BOOL CButton::OnMouseLeftButtonUp()
{
	if (!IsEnable())
	{
		return TRUE;
	}
	if (!IsPressed())
	{
		return TRUE;
	}

	m_isPressed = FALSE;
	Up();

	return TRUE;
}
//BOOL CButton::OnMouseRightButtonDown()
//{
//	if (!IsEnable())
//		return TRUE;
//
//	m_isPressed = TRUE;
//	Down();
//
//	return TRUE;
//}
BOOL CButton::OnMouseRightButtonDoubleClick()
{
	if (!IsEnable())
	{
		return TRUE;
	}

	OnMouseLeftButtonDown();

	return TRUE;
}
BOOL CButton::OnMouseRightButtonUp()
{
	if (!IsEnable())
	{
		return TRUE;
	}
	if (!IsPressed())
	{
		return TRUE;
	}

	m_isPressed = FALSE;
	Up();

	return TRUE;
}
void CButton::OnMouseOverIn()
{
	if (!IsEnable())
	{
		return;
	}

	Over();
	PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
}
void CButton::OnMouseOverOut()
{
	if (!IsEnable())
	{
		return;
	}

	SetUp();
	PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
}

#ifdef INGAME_WIKI
	void CButton::SetCurrentVisual(CGraphicExpandedImageInstance* pVisual)
#else
	void CButton::SetCurrentVisual(CGraphicImageInstance* pVisual)
#endif
{
	m_pcurVisual = pVisual;
	m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
#ifdef INGAME_WIKI
	if (m_pcurVisual == &m_upVisual)
	{
		PyCallClassMemberFunc(m_poHandler, "OnSetUpVisual", BuildEmptyTuple());
	}
	else if (m_pcurVisual == &m_overVisual)
	{
		PyCallClassMemberFunc(m_poHandler, "OnSetOverVisual", BuildEmptyTuple());
	}
	else if (m_pcurVisual == &m_downVisual)
	{
		PyCallClassMemberFunc(m_poHandler, "OnSetDownVisual", BuildEmptyTuple());
	}
	else if (m_pcurVisual == &m_disableVisual)
	{
		PyCallClassMemberFunc(m_poHandler, "OnSetDisableVisual", BuildEmptyTuple());
	}
#endif
}

BOOL CButton::IsEnable()
{
	return m_bEnable;
}

BOOL CButton::IsPressed()
{
	return m_isPressed;
}

#ifdef INGAME_WIKI
void CButton::OnSetRenderingRect()
{
	m_upVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	m_overVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	m_downVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
	m_disableVisual.iSetRenderingRect(m_renderingRect.left, m_renderingRect.top, m_renderingRect.right, m_renderingRect.bottom);
}
#endif

void CButton::OnUpdateRenderBox()
{
	if (m_upVisual.IsEmpty())
	{
		return;
	}

	CGraphicExpandedImageInstance* imageList[] = { &m_upVisual, &m_overVisual, &m_downVisual, &m_disableVisual };
	for (CGraphicExpandedImageInstance* image : imageList)
	{
		image->SetRenderBox(m_renderBox);
	}
}

void CButton::SetRenderingRect(float fLeft, float fTop, float fRight, float fBottom)
{
	CGraphicExpandedImageInstance* imageList[] = { &m_upVisual, &m_overVisual, &m_downVisual, &m_disableVisual };
	for (CGraphicExpandedImageInstance* image : imageList)
	{
		image->SetRenderingRect(fLeft, fTop, fRight, fBottom);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CRadioButton::CRadioButton(PyObject * ppyObject) : CButton(ppyObject)
{
}
CRadioButton::~CRadioButton()
{
}

BOOL CRadioButton::OnMouseLeftButtonDown()
{
	if (!IsEnable())
	{
		return TRUE;
	}

	if (!m_isPressed)
	{
		Down();
		PyCallClassMemberFunc(m_poHandler, "CallEvent", BuildEmptyTuple());
	}

	return TRUE;
}
BOOL CRadioButton::OnMouseLeftButtonUp()
{
	return TRUE;
}
void CRadioButton::OnMouseOverIn()
{
	if (!IsEnable())
	{
		return;
	}

	if (!m_isPressed)
	{
		SetCurrentVisual(&m_overVisual);
	}

	PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
}
void CRadioButton::OnMouseOverOut()
{
	if (!IsEnable())
	{
		return;
	}

	if (!m_isPressed)
	{
		SetCurrentVisual(&m_upVisual);
	}

	PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CToggleButton::CToggleButton(PyObject * ppyObject) : CButton(ppyObject)
{
}
CToggleButton::~CToggleButton()
{
}

BOOL CToggleButton::OnMouseLeftButtonDown()
{
	if (!IsEnable())
	{
		return TRUE;
	}

	if (m_isPressed)
	{
		SetUp();
		if (IsIn())
		{
			SetCurrentVisual(&m_overVisual);
		}
		else
		{
			SetCurrentVisual(&m_upVisual);
		}
		PyCallClassMemberFunc(m_poHandler, "OnToggleUp", BuildEmptyTuple());
	}
	else
	{
		Down();
		PyCallClassMemberFunc(m_poHandler, "OnToggleDown", BuildEmptyTuple());
	}

	return TRUE;
}
BOOL CToggleButton::OnMouseLeftButtonUp()
{
	return TRUE;
}

void CToggleButton::OnMouseOverIn()
{
	if (!IsEnable())
	{
		return;
	}

	if (!m_isPressed)
	{
		SetCurrentVisual(&m_overVisual);
	}

	PyCallClassMemberFunc(m_poHandler, "ShowToolTip", BuildEmptyTuple());
}
void CToggleButton::OnMouseOverOut()
{
	if (!IsEnable())
	{
		return;
	}

	if (!m_isPressed)
	{
		SetCurrentVisual(&m_upVisual);
	}

	PyCallClassMemberFunc(m_poHandler, "HideToolTip", BuildEmptyTuple());
}

///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

CDragButton::CDragButton(PyObject * ppyObject) : CButton(ppyObject)
{
	CWindow::RemoveFlag(CWindow::FLAG_NOT_CAPTURE);
	m_restrictArea.left = 0;
	m_restrictArea.top = 0;
	m_restrictArea.right = CWindowManager::Instance().GetScreenWidth();
	m_restrictArea.bottom = CWindowManager::Instance().GetScreenHeight();
}
CDragButton::~CDragButton()
{
}

void CDragButton::SetRestrictMovementArea(int ix, int iy, int iwidth, int iheight)
{
	m_restrictArea.left = ix;
	m_restrictArea.top = iy;
	m_restrictArea.right = ix + iwidth;
	m_restrictArea.bottom = iy + iheight;
}

void CDragButton::OnChangePosition()
{
	m_x = max(m_x, m_restrictArea.left);
	m_y = max(m_y, m_restrictArea.top);
	m_x = min(m_x, max(0, m_restrictArea.right - m_lWidth));
	m_y = min(m_y, max(0, m_restrictArea.bottom - m_lHeight));

	m_rect.left = m_x;
	m_rect.top = m_y;

	if (m_pParent)
	{
		const RECT & c_rRect = m_pParent->GetRect();
		m_rect.left += c_rRect.left;
		m_rect.top += c_rRect.top;
	}

	m_rect.right = m_rect.left + m_lWidth;
	m_rect.bottom = m_rect.top + m_lHeight;

	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::UpdateRect));

	if (m_pcurVisual)
	{
		m_pcurVisual->SetPosition(m_rect.left, m_rect.top);
	}

	if (IsPressed())
	{
		PyCallClassMemberFunc(m_poHandler, "OnMove", BuildEmptyTuple());
	}
}

void CDragButton::OnMouseOverIn()
{
	if (!IsEnable())

	{
		return;
	}

	CButton::OnMouseOverIn();
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverIn", BuildEmptyTuple());
}

void CDragButton::OnMouseOverOut()
{
	if (!IsEnable())
	{
		return;
	}

	CButton::OnMouseOverIn();
	PyCallClassMemberFunc(m_poHandler, "OnMouseOverOut", BuildEmptyTuple());
}

#ifdef TRANSMUTATION_SYSTEM
void CAniImageBox::SetSlotDiffuseColor(const char* c_szFileName, BYTE diffuse_type)
{
	static const float DIFFUSE_COLOR[][4] =
	{
		{ 238.00f / 255.0f, 11.00f / 255.0f, 11.00f / 255.0f, 1.0f },
		{ 255.00f / 255.0f, 255.00f / 255.0f, 0.00f / 255.0f, 1.0f },
	};

	CResource* pResource = CResourceManager::Instance().GetResourcePointer(c_szFileName);
	if (!pResource->IsType(CGraphicImage::Type()))
	{
		return;
	}

	CGraphicExpandedImageInstance* pImageInstance = CGraphicExpandedImageInstance::New();

	pImageInstance->SetImagePointer(static_cast<CGraphicImage*>(pResource));

	pImageInstance->SetDiffuseColor(DIFFUSE_COLOR[diffuse_type][0], DIFFUSE_COLOR[diffuse_type][1], DIFFUSE_COLOR[diffuse_type][2], DIFFUSE_COLOR[diffuse_type][3]);

	if (pImageInstance->IsEmpty())
	{
		CGraphicExpandedImageInstance::Delete(pImageInstance);
		return;
	}

	m_ImageVector.push_back(pImageInstance);

	m_bycurIndex = static_cast<BYTE>(rand() % m_ImageVector.size());
}
#endif

void CAniImageBox::SetSlotActivatedDiffuseColor(const std::vector<float>& dxColour)
{
	for (auto& img : m_ImageVector)
	{
		img->SetDiffuseColor(dxColour[0], dxColour[1], dxColour[2], dxColour[3]);
	}
}
};
