#include "StdAfx.h"
#include "../eterBase/CRC32.h"
#include "../eterBase/Filename.h"
#include "PythonWindow.h"
#include "PythonSlotWindow.h"

//#define __RENDER_SLOT_AREA__
#ifdef __ITEM_PICKUP_HIGHTLIGHT__
	#include <boost/format.hpp>
#endif

using namespace UI;

class UI::CSlotWindow::CSlotButton : public CButton
{
public:
	enum ESlotButtonType
	{
		SLOT_BUTTON_TYPE_PLUS,
		SLOT_BUTTON_TYPE_COVER,
	};

public:
	CSlotButton(ESlotButtonType dwType, DWORD dwSlotNumber, CSlotWindow * pParent) : CButton(NULL)
	{
		m_dwSlotButtonType = dwType;
		m_dwSlotNumber = dwSlotNumber;
		m_pParent = pParent;
	}
	virtual ~CSlotButton()
	{
	}

	BOOL OnMouseLeftButtonDown()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		if (UI::CWindowManager::Instance().IsAttaching())
		{
			return TRUE;
		}

		m_isPressed = TRUE;
		Down();

		return TRUE;
	}
	BOOL OnMouseLeftButtonUp()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		if (!IsPressed())
		{
			return TRUE;
		}

		if (IsIn())
		{
			SetCurrentVisual(&m_overVisual);
		}
		else
		{
			SetCurrentVisual(&m_upVisual);
		}

		m_pParent->OnPressedSlotButton(m_dwSlotButtonType, m_dwSlotNumber);

		return TRUE;
	}

	void OnMouseOverIn()
	{
		if (IsEnable())
		{
			SetCurrentVisual(&m_overVisual);
		}

		//m_pParent->OnOverInItem(m_dwSlotNumber);
		//m_pParent->OnOverIn(m_dwSlotNumber);
		TSlot* pSlot;
		if (m_pParent->GetSlotPointer(m_dwSlotNumber, &pSlot))
		{
			if (pSlot->isItem)
			{
				m_pParent->OnOverInItem(m_dwSlotNumber);
			}
			else
			{
				m_pParent->OnOverIn(m_dwSlotNumber);
			}
		}
	}

	void OnMouseOverOut()
	{
		if (IsEnable())
		{
			SetUp();
			SetCurrentVisual(&m_upVisual);
		}

		//m_pParent->OnOverOutItem();
		//m_pParent->OnOverOut();
		TSlot* pSlot;
		if (m_pParent->GetSlotPointer(m_dwSlotNumber, &pSlot))
		{
			if (pSlot->isItem)
			{
				m_pParent->OnOverOutItem();
			}
			else
			{
				m_pParent->OnOverOut();
			}
		}
	}

protected:
	ESlotButtonType m_dwSlotButtonType;
	DWORD m_dwSlotNumber;
	CSlotWindow * m_pParent;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class UI::CSlotWindow::CCoverButton : public CSlotButton
{
public:
	CCoverButton(ESlotButtonType dwType, DWORD dwSlotNumber, CSlotWindow * pParent) : CSlotButton(dwType, dwSlotNumber, pParent)
	{
		m_bLeftButtonEnable = TRUE;
		m_bRightButtonEnable = TRUE;
	}
	virtual ~CCoverButton()
	{
	}

	void SetLeftButtonEnable(BOOL bEnable)
	{
		m_bLeftButtonEnable = bEnable;
	}
	void SetRightButtonEnable(BOOL bEnable)
	{
		m_bRightButtonEnable = bEnable;
	}

	void OnRender()
	{
	}

	void RenderButton()
	{
		CButton::OnRender();
	}

	BOOL OnMouseLeftButtonDown()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		if (m_bLeftButtonEnable)
		{
			CButton::OnMouseLeftButtonDown();
		}
		else
		{
			m_pParent->OnMouseLeftButtonDown();
		}
		return TRUE;
	}
	BOOL OnMouseLeftButtonUp()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		if (m_bLeftButtonEnable)
		{
			CButton::OnMouseLeftButtonUp();
			m_pParent->OnMouseLeftButtonDown();
		}
		else
		{
			m_pParent->OnMouseLeftButtonUp();
		}
		return TRUE;
	}

	BOOL OnMouseRightButtonDown()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		if (m_bRightButtonEnable)
		{
			CButton::OnMouseLeftButtonDown();
		}
		return TRUE;
	}
	BOOL OnMouseRightButtonUp()
	{
		if (!IsEnable())
		{
			return TRUE;
		}
		m_pParent->OnMouseRightButtonDown();
		if (m_bRightButtonEnable)
		{
			CButton::OnMouseLeftButtonUp();
		}
		return TRUE;
	}

protected:
	BOOL m_bLeftButtonEnable;
	BOOL m_bRightButtonEnable;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class UI::CSlotWindow::CCoolTimeFinishEffect : public CAniImageBox
{
public:
	CCoolTimeFinishEffect(CSlotWindow * pParent, DWORD dwSlotIndex) : CAniImageBox(NULL)
	{
		m_pParent = pParent;
		m_dwSlotIndex = dwSlotIndex;
	}
	virtual ~CCoolTimeFinishEffect()
	{
	}

	void OnEndFrame()
	{
		((CSlotWindow *)m_pParent)->ReserveDestroyCoolTimeFinishEffect(m_dwSlotIndex);
	}

protected:
	DWORD m_dwSlotIndex;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// Set & Append

void CSlotWindow::SetSlotType(DWORD dwType)
{
	m_dwSlotType = dwType;
}

void CSlotWindow::SetSlotStyle(DWORD dwStyle)
{
	m_dwSlotStyle = dwStyle;
}

void CSlotWindow::SetSlotBaseImageScale(float fx, float fy)
{
	if (m_pBaseImageInstance)
	{
		m_pBaseImageInstance->SetScale(fx, fy);
	}
}

void CSlotWindow::SetScale(float fx, float fy)
{
	m_v2Scale.x = fx;
	m_v2Scale.y = fy;
}

void CSlotWindow::AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize)
{
	TSlot Slot;
	Slot.pInstance = NULL;
	Slot.pNumberLine = NULL;
	Slot.pCoverButton = NULL;
	Slot.pSlotButton = NULL;
	Slot.pSignImage = NULL;

	Slot.pFinishCoolTimeEffect = NULL;

	ClearSlot(&Slot);
	Slot.dwSlotNumber = dwIndex;
	Slot.dwCenterSlotNumber = dwIndex;
	Slot.ixPosition = ixPosition;
	Slot.iyPosition = iyPosition;
	Slot.ixCellSize = ixCellSize;
	Slot.iyCellSize = iyCellSize;
	m_SlotList.push_back(Slot);
}

void CSlotWindow::SetCoverButton(DWORD dwIndex, const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName, const char * c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	CCoverButton *& rpCoverButton = pSlot->pCoverButton;

	if (!rpCoverButton)
	{
		rpCoverButton = new CCoverButton(CSlotButton::SLOT_BUTTON_TYPE_COVER, pSlot->dwSlotNumber, this);
		CWindowManager::Instance().SetParent(rpCoverButton, this);
	}

	rpCoverButton->SetLeftButtonEnable(bLeftButtonEnable);
	rpCoverButton->SetRightButtonEnable(bRightButtonEnable);
	rpCoverButton->SetUpVisual(c_szUpImageName);
	rpCoverButton->SetOverVisual(c_szOverImageName);
	rpCoverButton->SetDownVisual(c_szDownImageName);
	rpCoverButton->SetDisableVisual(c_szDisableImageName);
	rpCoverButton->Enable();
	rpCoverButton->Show();

	if (pSlot->pSlotButton)
	{
		SetTop(pSlot->pSlotButton);
	}
}

void CSlotWindow::DeleteCoverButton(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (!pSlot->pCoverButton)
	{
		return;
	}

	CWindowManager::Instance().DestroyWindow(pSlot->pCoverButton);
	pSlot->pCoverButton = NULL;
}

void CSlotWindow::EnableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (!pSlot->pCoverButton)
	{
		return;
	}

	pSlot->pCoverButton->Enable();
}

void CSlotWindow::DisableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (!pSlot->pCoverButton)
	{
		return;
	}

	pSlot->pCoverButton->Disable();
}

void CSlotWindow::SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (bAlwaysRender)
	{
		SET_BIT(pSlot->dwState, SLOT_STATE_ALWAYS_RENDER_COVER);
	}
	else
	{
		REMOVE_BIT(pSlot->dwState, SLOT_STATE_ALWAYS_RENDER_COVER);
	}
}

void CSlotWindow::ShowSlotBaseImage(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bRenderBaseSlotImage = true;
}

void CSlotWindow::HideSlotBaseImage(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bRenderBaseSlotImage = false;
}

BOOL CSlotWindow::IsDisableCoverButton(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return FALSE;
	}

	if (!pSlot->pCoverButton)
	{
		return FALSE;
	}

	return pSlot->pCoverButton->IsDisable();
}

void CSlotWindow::SetSlotBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa)
{
	__CreateBaseImage(c_szFileName, fr, fg, fb, fa);
}

/*** Start New Pet System ***/
void CSlotWindow::SetSlotBaseImageScale(const char* c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy)
{
	__CreateBaseImageScale(c_szFileName, fr, fg, fb, fa, sx, sy);
}
/*** End New Pet System ***/

void CSlotWindow::AppendSlotButton(const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;
		CSlotButton *& rpSlotButton = rSlot.pSlotButton;

		if (!rpSlotButton)
		{
			rpSlotButton = new CSlotButton(CSlotButton::SLOT_BUTTON_TYPE_PLUS, rSlot.dwSlotNumber, this);
			rpSlotButton->AddFlag(FLAG_FLOAT);
			CWindowManager::Instance().SetParent(rpSlotButton, this);
		}

		rpSlotButton->SetUpVisual(c_szUpImageName);
		rpSlotButton->SetOverVisual(c_szOverImageName);
		rpSlotButton->SetDownVisual(c_szDownImageName);
		rpSlotButton->SetPosition(rSlot.ixPosition + 1, rSlot.iyPosition + 19);
		rpSlotButton->Hide();
	}
}

void CSlotWindow::AppendRequirementSignImage(const char * c_szImageName)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;
		CImageBox *& rpSignImage = rSlot.pSignImage;

		if (!rpSignImage)
		{
			rpSignImage = new CImageBox(NULL);
			CWindowManager::Instance().SetParent(rpSignImage, this);
		}

		rpSignImage->LoadImage(c_szImageName);
		rpSignImage->Hide();
	}
}

BOOL CSlotWindow::HasSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return FALSE;
	}

	return TRUE;
}

void CSlotWindow::SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (pSlot->isItem)
		if (pSlot->dwItemIndex == dwVirtualNumber)
		{
			pSlot->dwState = 0;
			pSlot->isItem = (dwVirtualNumber > 0) ? TRUE : FALSE;
			if (pImage && pSlot->pInstance)
			{
				pSlot->pInstance->SetImagePointer(pImage);
			}
			return;
		}

	ClearSlot(pSlot);
	pSlot->dwState = 0;
	pSlot->isItem = (dwVirtualNumber > 0) ? TRUE : FALSE;
	pSlot->dwItemIndex = dwVirtualNumber;

	if (pImage)
	{
		assert(NULL == pSlot->pInstance);
		pSlot->pInstance = CGraphicImageInstance::New();
		pSlot->pInstance->SetDiffuseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
		pSlot->pInstance->SetImagePointer(pImage);
	}

	pSlot->byxPlacedItemSize = byWidth;
	pSlot->byyPlacedItemSize = byHeight;

	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Show();
	}
}

/*** Start New Pet System ***/
void CSlotWindow::SetSlotScale(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage* pImage, float sx, float sy, D3DXCOLOR& diffuseColor)
{
	TSlot* pSlot;

	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (pSlot->isItem)
		if (pSlot->dwItemIndex == dwVirtualNumber)
		{
			pSlot->dwState = 0;
			pSlot->isItem = TRUE;
			if (pImage && pSlot->pInstance)
			{
				pSlot->pInstance->SetImagePointer(pImage);
			}
			return;
		}

	ClearSlot(pSlot);
	pSlot->dwState = 0;
	pSlot->isItem = TRUE;
	pSlot->dwItemIndex = dwVirtualNumber;

	if (pImage)
	{
		assert(NULL == pSlot->pInstance);
		pSlot->pInstance = CGraphicImageInstance::New();
		pSlot->pInstance->SetDiffuseColor(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
		pSlot->pInstance->SetImagePointer(pImage);
		pSlot->pInstance->SetScale(sx, sy);
	}

	pSlot->byxPlacedItemSize = byWidth;
	pSlot->byyPlacedItemSize = byHeight;

	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Show();
	}
}
/*** End New Pet System ***/

void CSlotWindow::SetSlotCount(DWORD dwIndex, DWORD dwCount)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (dwCount <= 0)
	{
		if (pSlot->pNumberLine)
		{
			delete pSlot->pNumberLine;
			pSlot->pNumberLine = NULL;
		}
	}
	else
	{
		char szCount[16 + 1];
		_snprintf(szCount, sizeof(szCount), "%d", dwCount);

		if (!pSlot->pNumberLine)
		{
			CNumberLine * pNumberLine = new UI::CNumberLine(this);
			pNumberLine->SetHorizontalAlign(CNumberLine::HORIZONTAL_ALIGN_RIGHT);
			pNumberLine->Show();
			pSlot->pNumberLine = pNumberLine;
		}

		pSlot->pNumberLine->SetNumber(szCount);
	}
}

void CSlotWindow::SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (dwCount <= 0)
	{
		if (pSlot->pNumberLine)
		{
			delete pSlot->pNumberLine;
			pSlot->pNumberLine = NULL;
		}
	}
	else
	{
		char szCount[16 + 1];

		switch (dwGrade)
		{
		case 0:
			_snprintf(szCount, sizeof(szCount), "%d", dwCount);
			break;
		case 1:
			_snprintf(szCount, sizeof(szCount), "m%d", dwCount);
			break;
		case 2:
			_snprintf(szCount, sizeof(szCount), "g%d", dwCount);
			break;
		case 3:
			_snprintf(szCount, sizeof(szCount), "p");
			break;
		}

		if (!pSlot->pNumberLine)
		{
			CNumberLine * pNumberLine = new UI::CNumberLine(this);
			pNumberLine->SetHorizontalAlign(CNumberLine::HORIZONTAL_ALIGN_RIGHT);
			pNumberLine->Show();
			pSlot->pNumberLine = pNumberLine;
		}

		pSlot->pNumberLine->SetNumber(szCount);
	}
}

void CSlotWindow::SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->fCoolTime = fCoolTime;
	pSlot->fStartCoolTime = CTimer::Instance().GetCurrentSecond() - fElapsedTime;
}

void CSlotWindow::ActivateSlot(DWORD dwIndex, const std::vector<float>& v_fColMatrix)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bActive = TRUE;

	// Colour matrix
	for (size_t i = 0; i < v_fColMatrix.size(); ++i)
	{
		pSlot->fActtivateColour[i] = v_fColMatrix[i];
	}

#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	if (!m_pSlotActiveEffect)
#else
	if (!m_pSlotActiveEffect[0] || !m_pSlotActiveEffect[1] || !m_pSlotActiveEffect[2])
#endif
	{
		__CreateSlotEnableEffect();
	}
}

void CSlotWindow::NewActivateSlot(DWORD dwIndex)
{
	TSlot* pSlot;

	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->nbActive = TRUE;

	if (!m_niSlotActiveEffect[0] || !m_niSlotActiveEffect[1] || !m_niSlotActiveEffect[2])
	{
		__CreateNewSlotEnableEffect();
	}
}

void CSlotWindow::DeactivateSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bActive = FALSE;
}

void CSlotWindow::NewDeactivateSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->nbActive = FALSE;
}

#ifdef ENABLE_SLOT_WINDOW_EX
float CSlotWindow::GetSlotCoolTime(DWORD dwIndex, float * fElapsedTime)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return 0.0;
	}

	*fElapsedTime = CTimer::Instance().GetCurrentSecond() - pSlot->fStartCoolTime;
	return pSlot->fCoolTime;
}

bool CSlotWindow::IsActivatedSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return false;
	}
	return pSlot->bActive;
}
#endif

void CSlotWindow::ClearSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	ClearSlot(pSlot);
}

void CSlotWindow::ClearSlot(TSlot * pSlot)
{
	pSlot->bActive = FALSE;
	pSlot->nbActive = FALSE;
	pSlot->byxPlacedItemSize = 1;
	pSlot->byyPlacedItemSize = 1;

	pSlot->isItem = FALSE;
	pSlot->dwState = 0;
	pSlot->fCoolTime = 0.0f;
	pSlot->fStartCoolTime = 0.0f;
	pSlot->dwCenterSlotNumber = 0xffffffff;

	pSlot->dwItemIndex = 0;
	pSlot->bRenderBaseSlotImage = true;

	if (pSlot->pInstance)
	{
		CGraphicImageInstance::Delete(pSlot->pInstance);
		pSlot->pInstance = NULL;
	}
	if (pSlot->pCoverButton)
	{
		pSlot->pCoverButton->Hide();
	}
	if (pSlot->pSlotButton)
	{
		pSlot->pSlotButton->Hide();
	}
	if (pSlot->pSignImage)
	{
		pSlot->pSignImage->Hide();
	}
	if (pSlot->pFinishCoolTimeEffect)
	{
		pSlot->pFinishCoolTimeEffect->Hide();
	}

#ifdef TRANSMUTATION_SYSTEM
	pSlot->bChangeLookActive = FALSE;
	pSlot->bSlotCoverImage = FALSE;
#endif

	memset(pSlot->fLockColour, 0, sizeof(pSlot->fLockColour));
	memset(pSlot->fActtivateColour, 0, sizeof(pSlot->fActtivateColour));
}

void CSlotWindow::ClearAllSlot()
{
	Destroy();
}

void CSlotWindow::RefreshSlot()
{
	OnRefreshSlot();

	if (IsRendering())
	{
		TSlot* pSlot;
		if (GetPickedSlotPointer(&pSlot))
		{
			if (pSlot->isItem)
			{
				OnOverOutItem();
				OnOverInItem(pSlot->dwSlotNumber);
			}
			else
			{
				OnOverOut();
				OnOverIn(pSlot->dwSlotNumber);
			}
		}
	}
}

std::vector<uint32_t> CSlotWindow::GetSlotIndincies()
{
	std::vector<uint32_t> v_slots = { };

	for (auto& rSlots : m_SlotList)
	{
		v_slots.push_back(rSlots.dwSlotNumber);
	}

	return v_slots;
}

void CSlotWindow::OnRefreshSlot()
{
}

DWORD CSlotWindow::GetSlotCount()
{
	return m_SlotList.size();
}

void CSlotWindow::LockSlot(DWORD dwIndex, const float* fLockColour)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	memcpy(pSlot->fLockColour, fLockColour, sizeof(pSlot->fLockColour));
	pSlot->dwState |= SLOT_STATE_LOCK;
}

void CSlotWindow::UnlockSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	if (pSlot->dwState & SLOT_STATE_LOCK)
	{
		pSlot->dwState ^= SLOT_STATE_LOCK;
	}
}
void CSlotWindow::SetCantUseSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->dwState |= SLOT_STATE_CANT_USE;
}
void CSlotWindow::SetUseSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->dwState ^= SLOT_STATE_CANT_USE;
}
void CSlotWindow::EnableSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	REMOVE_BIT(pSlot->dwState, SLOT_STATE_DISABLE);
	//pSlot->dwState |= SLOT_STATE_DISABLE;
}
void CSlotWindow::DisableSlot(DWORD dwIndex)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}
	SET_BIT(pSlot->dwState, SLOT_STATE_DISABLE);
	//pSlot->dwState ^= SLOT_STATE_DISABLE;
}

#ifdef ENABLE_RENEWAL_SHOP_SELLING
void CSlotWindow::SetUnusableSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	SET_BIT(pSlot->dwState, SLOT_STATE_UNUSABLE);
}

void CSlotWindow::SetUsableSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}
	REMOVE_BIT(pSlot->dwState, SLOT_STATE_UNUSABLE);
}
#endif

// Select

#ifdef ENABLE_RENEWAL_EXCHANGE
void CSlotWindow::DisableSlotHighlightedGreen(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	REMOVE_BIT(pSlot->dwState, SLOT_STATE_HIGHLIGHT_GREEN);
}

void CSlotWindow::SetSlotHighlightedGreeen(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	SET_BIT(pSlot->dwState, SLOT_STATE_HIGHLIGHT_GREEN);
}
#endif

DWORD CSlotWindow::GetItemIndex(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return 0;
	}

	return pSlot->dwItemIndex;
}

void CSlotWindow::SelectSlot(DWORD dwSelectingIndex)
{
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end();)
	{
		if (dwSelectingIndex == *itor)
		{
			itor = m_dwSelectedSlotIndexList.erase(itor);
			return;
		}
		else
		{
			++itor;
		}
	}

	TSlot * pSlot;
	if (GetSlotPointer(dwSelectingIndex, &pSlot))
	{
		if (!pSlot->isItem)
		{
			return;
		}

		m_dwSelectedSlotIndexList.push_back(dwSelectingIndex);
	}
}

BOOL CSlotWindow::isSelectedSlot(DWORD dwIndex)
{
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		if (dwIndex == *itor)
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CSlotWindow::ClearSelected()
{
	m_dwSelectedSlotIndexList.clear();
}

DWORD CSlotWindow::GetSelectedSlotCount()
{
	return m_dwSelectedSlotIndexList.size();
}

DWORD CSlotWindow::GetSelectedSlotNumber(DWORD dwIndex)
{
	if (dwIndex >= m_dwSelectedSlotIndexList.size())
	{
		return DWORD(-1);
	}

	DWORD dwCount = 0;
	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		if (dwIndex == dwCount)
		{
			break;
		}

		++dwCount;
	}

	return *itor;
}

void CSlotWindow::ShowSlotButton(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
	{
		return;
	}

	if (pSlot->pSlotButton)
	{
		pSlot->pSlotButton->Show();
	}
}

void CSlotWindow::HideAllSlotButton()
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (rSlot.pSlotButton)
		{
			rSlot.pSlotButton->Hide();
		}
	}
}

void CSlotWindow::ShowRequirementSign(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
	{
		return;
	}

	if (!pSlot->pSignImage)
	{
		return;
	}

	pSlot->pSignImage->Show();
}

void CSlotWindow::HideRequirementSign(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
	{
		return;
	}

	if (!pSlot->pSignImage)
	{
		return;
	}

	pSlot->pSignImage->Hide();
}

// Event

BOOL CSlotWindow::OnMouseLeftButtonDown()
{
	TSlot * pSlot;
	if (!GetPickedSlotPointer(&pSlot))
	{
		UI::CWindowManager::Instance().DeattachIcon();
		return TRUE;
	}

	if (pSlot->isItem && !(pSlot->dwState & SLOT_STATE_LOCK))
	{
		OnSelectItemSlot(pSlot->dwSlotNumber);
	}
	else
	{
		OnSelectEmptySlot(pSlot->dwSlotNumber);
	}

	return TRUE;
}

BOOL CSlotWindow::OnMouseLeftButtonUp()
{
	if (UI::CWindowManager::Instance().IsAttaching())
		if (UI::CWindowManager::Instance().IsDragging())
			if (IsIn())
			{
				UI::CWindow * pWin = UI::CWindowManager::Instance().GetPointWindow();

				if (pWin)
					if (pWin->IsType(UI::CSlotWindow::Type()))
					{
						UI::CSlotWindow * pSlotWin = (UI::CSlotWindow *)pWin;

						TSlot * pSlot;
						if (!pSlotWin->GetPickedSlotPointer(&pSlot))
						{
							UI::CWindowManager::Instance().DeattachIcon();
							return TRUE;
						}

						if (pSlot->isItem)
						{
							pSlotWin->OnSelectItemSlot(pSlot->dwSlotNumber);
						}
						else
						{
							pSlotWin->OnSelectEmptySlot(pSlot->dwSlotNumber);
						}

						return TRUE;
					}

				return FALSE;
			}

	return FALSE;
}

BOOL CSlotWindow::OnMouseRightButtonDown()
{
	TSlot * pSlot;
	if (!GetPickedSlotPointer(&pSlot))
	{
		return TRUE;
	}

	if (pSlot->isItem)
	{
		OnUnselectItemSlot(pSlot->dwSlotNumber);
	}
	else
	{
		OnUnselectEmptySlot(pSlot->dwSlotNumber);
	}

	return TRUE;
}

BOOL CSlotWindow::OnMouseLeftButtonDoubleClick()
{
	OnUseSlot();

	return TRUE;
}

void CSlotWindow::OnMouseOverOut()
{
	OnOverOutItem();
	OnOverOut();
}

void CSlotWindow::OnMouseOver()
{
//	if (UI::CWindowManager::Instance().IsCapture())
//	if (!UI::CWindowManager::Instance().IsAttaching())
//		return;

	TSlot* pSlot;
	CWindow* pPointWindow = UI::CWindowManager::Instance().GetPointWindow();
	if (this == pPointWindow)
	{
		if (GetPickedSlotPointer(&pSlot))
		{
			if (OnOverInItem(pSlot->dwSlotNumber))
			{
				return;
			}
			else
			{
				OnOverOutItem();
			}

			if (OnOverIn(pSlot->dwSlotNumber))
			{
				return;
			}
			else
			{
				OnOverOut();
			}

			return;
		}
	}

	OnOverOutItem();
	OnOverOut();
}

void CSlotWindow::OnSelectEmptySlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnSelectEmptySlot", Py_BuildValue("(i)", iSlotNumber));
}
void CSlotWindow::OnSelectItemSlot(int iSlotNumber)
{
//	OnOverOutItem();
	PyCallClassMemberFunc(m_poHandler, "OnSelectItemSlot", Py_BuildValue("(i)", iSlotNumber));

	if (UI::CWindowManager::Instance().IsAttaching())
	{
		OnOverOutItem();
	}
}
void CSlotWindow::OnUnselectEmptySlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnUnselectEmptySlot", Py_BuildValue("(i)", iSlotNumber));
}
void CSlotWindow::OnUnselectItemSlot(int iSlotNumber)
{
	PyCallClassMemberFunc(m_poHandler, "OnUnselectItemSlot", Py_BuildValue("(i)", iSlotNumber));
}
void CSlotWindow::OnUseSlot()
{
	TSlot * pSlot;
	if (GetPickedSlotPointer(&pSlot))
		if (pSlot->isItem)
		{
			PyCallClassMemberFunc(m_poHandler, "OnUseSlot", Py_BuildValue("(i)", pSlot->dwSlotNumber));
		}
}

BOOL CSlotWindow::OnOverInItem(DWORD dwSlotNumber)
{
	TSlot * pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
	{
		return FALSE;
	}

	if (!pSlot->isItem)
	{
		return FALSE;
	}

	if (pSlot->dwSlotNumber == m_dwToolTipSlotNumber)
	{
		return TRUE;
	}

	m_dwToolTipSlotNumber = dwSlotNumber;
	PyCallClassMemberFunc(m_poHandler, "OnOverInItem", Py_BuildValue("(i)", dwSlotNumber));

	return TRUE;
}

void CSlotWindow::OnOverOutItem()
{
	if (SLOT_NUMBER_NONE == m_dwToolTipSlotNumber)
	{
		return;
	}

	m_dwToolTipSlotNumber = SLOT_NUMBER_NONE;
	PyCallClassMemberFunc(m_poHandler, "OnOverOutItem", Py_BuildValue("()"));
}

BOOL CSlotWindow::OnOverIn(DWORD dwSlotNumber)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwSlotNumber, &pSlot))
	{
		return FALSE;
	}

	if (pSlot->isItem)
	{
		return FALSE;
	}

	if (pSlot->dwSlotNumber == m_dwOverInSlotNumber)
	{
		return TRUE;
	}

	m_dwOverInSlotNumber = dwSlotNumber;
	PyCallClassMemberFunc(m_poHandler, "OnOverIn", Py_BuildValue("(i)", dwSlotNumber));
	return TRUE;
}

void CSlotWindow::OnOverOut()
{
	if (SLOT_NUMBER_NONE == m_dwOverInSlotNumber)
	{
		return;
	}

	m_dwOverInSlotNumber = SLOT_NUMBER_NONE;
	PyCallClassMemberFunc(m_poHandler, "OnOverOut", Py_BuildValue("()"));
}

void CSlotWindow::OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft)
{
	if (CSlotButton::SLOT_BUTTON_TYPE_PLUS == dwType)
	{
		PyCallClassMemberFunc(m_poHandler, "OnPressedSlotButton", Py_BuildValue("(i)", dwSlotNumber));
	}
	else if (CSlotButton::SLOT_BUTTON_TYPE_COVER == dwType)
	{
		if (isLeft)
		{
			OnMouseLeftButtonDown();
		}
	}
}

void CSlotWindow::OnUpdate()
{
	for (std::deque<DWORD>::iterator itor = m_ReserveDestroyEffectDeque.begin(); itor != m_ReserveDestroyEffectDeque.end(); ++itor)
	{
		DWORD dwSlotIndex = *itor;

		TSlot* pSlot;
		if (!GetSlotPointer(dwSlotIndex, &pSlot))
		{
			continue;
		}

		__DestroyFinishCoolTimeEffect(pSlot);
	}
	m_ReserveDestroyEffectDeque.clear();

#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	if (m_pSlotActiveEffect)
	{
		m_pSlotActiveEffect->Update();
	}
#else
	if (m_pSlotActiveEffect)
	{
		for (int i = 0; i < 3; ++i)
		{
			if (m_pSlotActiveEffect[i])
			{
				m_pSlotActiveEffect[i]->Update();
			}
		}
	}

	if (m_niSlotActiveEffect)
	{
		for (int i = 0; i < 3; ++i)
		{

			if (m_niSlotActiveEffect[i])
			{
				m_niSlotActiveEffect[i]->Update();
			}
		}
	}
#endif

#ifdef TRANSMUTATION_SYSTEM
	for (int i = 0; i < 3; ++i)
	{
		if (m_pSlotChangeLookActiveEffect[i])
		{
			m_pSlotChangeLookActiveEffect[i]->Update();
		}
	}

	if (m_pSlotCoverImage)
	{
		m_pSlotCoverImage->Update();
	}
#endif
}

void CSlotWindow::OnRender()
{
	RenderSlotBaseImage();

	switch (m_dwSlotStyle)
	{
	case SLOT_STYLE_PICK_UP:
		OnRenderPickingSlot();
		break;
	case SLOT_STYLE_SELECT:
		OnRenderSelectedSlot();
		break;
	}

	std::for_each(m_pChildList.begin(), m_pChildList.end(), std::mem_fn(&CWindow::OnRender));

	TSlotListIterator itor;

	//
	//////////////////////////////////////////////////////////////////////////
#ifdef __RENDER_SLOT_AREA__
	CPythonGraphic::Instance().SetDiffuseColor(0.5f, 0.5f, 0.5f);
	for (itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;
		CPythonGraphic::Instance().RenderBox2d(m_rect.left + rSlot.ixPosition,
											   m_rect.top + rSlot.iyPosition,
											   m_rect.left + rSlot.ixPosition + rSlot.ixCellSize,
											   m_rect.top + rSlot.iyPosition + rSlot.iyCellSize);
	}
	CPythonGraphic::Instance().SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	CPythonGraphic::Instance().RenderBox2d(m_rect.left, m_rect.top, m_rect.right, m_rect.bottom);
#endif
	//////////////////////////////////////////////////////////////////////////

	for (itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (rSlot.pSlotButton)
		{
			rSlot.pSlotButton->SetPosition(rSlot.ixPosition + 1, rSlot.iyPosition + 19);
		}
		if (rSlot.pSignImage)
		{
			rSlot.pSignImage->SetPosition(rSlot.ixPosition - 7, rSlot.iyPosition + 10);
		}

		if (rSlot.pInstance)
		{
			rSlot.pInstance->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
			rSlot.pInstance->Render();
		}

#ifdef TRANSMUTATION_SYSTEM
		if (rSlot.bChangeLookActive)
		{
			int ix = m_rect.left + rSlot.ixPosition;
			int iy = m_rect.top + rSlot.iyPosition;
			int size = rSlot.byyPlacedItemSize - 1;

			if (size >= 0 && size <= 2)
			{
				if (m_pSlotChangeLookActiveEffect[size])
				{
					m_pSlotChangeLookActiveEffect[size]->Show();
					m_pSlotChangeLookActiveEffect[size]->SetPosition(ix, iy);
					m_pSlotChangeLookActiveEffect[size]->Render();
				}
			}
		}

		if (rSlot.bSlotCoverImage)
		{
			if (m_pSlotCoverImage)
			{
				int ix = m_rect.left + rSlot.ixPosition;
				int iy = m_rect.top + rSlot.iyPosition;

				m_pSlotCoverImage->SetPosition(ix, iy);
				m_pSlotCoverImage->Render();
			}
		}
#endif

		if (!rSlot.isItem)
		{
			if (IS_SET(rSlot.dwState, SLOT_STATE_ALWAYS_RENDER_COVER))
			{
				rSlot.pCoverButton->Show();
				rSlot.pCoverButton->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
				rSlot.pCoverButton->RenderButton();
			}

			continue;
		}

		if (IS_SET(rSlot.dwState, SLOT_STATE_DISABLE))
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 0.0f, 0.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
												   m_rect.top + rSlot.iyPosition,
												   m_rect.left + rSlot.ixPosition + rSlot.ixCellSize,
												   m_rect.top + rSlot.iyPosition + rSlot.iyCellSize);
		}

#ifdef ENABLE_RENEWAL_SHOP_SELLING
		if (IS_SET(rSlot.dwState, SLOT_STATE_UNUSABLE))
		{
			CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
												   m_rect.top + rSlot.iyPosition,
												   m_rect.left + rSlot.ixPosition + rSlot.byxPlacedItemSize * rSlot.ixCellSize,
												   m_rect.top + rSlot.iyPosition + rSlot.byyPlacedItemSize * rSlot.iyCellSize);
		}
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
		if (IS_SET(rSlot.dwState, SLOT_STATE_HIGHLIGHT_GREEN))
		{
			CPythonGraphic::Instance().SetDiffuseColor(74.0f / 255.f, 1.0f, 74.0f / 255.0f, 0.3f);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
												   m_rect.top + rSlot.iyPosition,
												   m_rect.left + rSlot.ixPosition + rSlot.byxPlacedItemSize * rSlot.ixCellSize,
												   m_rect.top + rSlot.iyPosition + rSlot.byyPlacedItemSize * rSlot.iyCellSize);
		}
#endif

		if (rSlot.fCoolTime != 0.0f)
		{
			float fcurTime = CTimer::Instance().GetCurrentSecond();
			float fPercentage = (fcurTime - rSlot.fStartCoolTime) / rSlot.fCoolTime;
			CPythonGraphic::Instance().RenderCoolTimeBox(m_rect.left + rSlot.ixPosition + 16.0f, m_rect.top + rSlot.iyPosition + 16.0f, 16.0f, fPercentage);

			if (fcurTime - rSlot.fStartCoolTime >= rSlot.fCoolTime)
			{
				if ((fcurTime - rSlot.fStartCoolTime) - rSlot.fCoolTime < 1.0f)
				{
					__CreateFinishCoolTimeEffect(&rSlot);
				}

				rSlot.fCoolTime = 0.0f;
				rSlot.fStartCoolTime = 0.0f;
			}
		}

		if (rSlot.pCoverButton)
		{
			rSlot.pCoverButton->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
			rSlot.pCoverButton->RenderButton();
		}

		if (rSlot.pNumberLine)
		{
			int ix = rSlot.byxPlacedItemSize * ITEM_WIDTH + rSlot.ixPosition - 4;
			int iy = rSlot.iyPosition + rSlot.byyPlacedItemSize * ITEM_HEIGHT - 12 + 2;
			rSlot.pNumberLine->SetPosition(ix, iy);
			rSlot.pNumberLine->Update();
			rSlot.pNumberLine->Render();
		}

		if (rSlot.pFinishCoolTimeEffect)
		{
			rSlot.pFinishCoolTimeEffect->SetPosition(rSlot.ixPosition, rSlot.iyPosition);
			rSlot.pFinishCoolTimeEffect->Update();
			rSlot.pFinishCoolTimeEffect->Render();
		}

		if (rSlot.bActive)
#ifndef __ITEM_PICKUP_HIGHTLIGHT__
			if (m_pSlotActiveEffect)
			{
				int ix = m_rect.left + rSlot.ixPosition;
				int iy = m_rect.top + rSlot.iyPosition;
				m_pSlotActiveEffect->SetPosition(ix, iy);
				m_pSlotActiveEffect->Render();
			}
#else
			if (m_pSlotActiveEffect)
			{
				// Layering diffuse if matrix is set
				if (rSlot.fActtivateColour[3] > 0.0f)
					for (auto& img : m_pSlotActiveEffect)
					{
						img->SetSlotActivatedDiffuseColor(std::vector<float>(std::begin(rSlot.fActtivateColour), std::end(rSlot.fActtivateColour)));
					}

				int ix = m_rect.left + rSlot.ixPosition;
				int iy = m_rect.top + rSlot.iyPosition;
				m_pSlotActiveEffect[MINMAX(0, rSlot.byyPlacedItemSize - 1, 2)]->SetPosition(ix, iy);
				m_pSlotActiveEffect[MINMAX(0, rSlot.byyPlacedItemSize - 1, 2)]->Render();
			}
#endif

		if (rSlot.nbActive)
		{
			int iSize = ((int)rSlot.byyPlacedItemSize) - 1;
			if (iSize >= 0 && iSize < 3 && m_niSlotActiveEffect[iSize])
			{
				int ix = m_rect.left + rSlot.ixPosition;
				int iy = m_rect.top + rSlot.iyPosition;
				m_niSlotActiveEffect[iSize]->SetPosition(ix, iy);
				m_niSlotActiveEffect[iSize]->Render();
			}
		}
	}

	RenderLockedSlot();
}

void CSlotWindow::RenderSlotBaseImage()
{
	if (!m_pBaseImageInstance)
	{
		return;
	}

	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (!rSlot.bRenderBaseSlotImage)
		{
			continue;
		}

		m_pBaseImageInstance->SetPosition(m_rect.left + rSlot.ixPosition, m_rect.top + rSlot.iyPosition);
		m_pBaseImageInstance->Render();
	}
}

void CSlotWindow::OnRenderPickingSlot()
{
	if (!UI::CWindowManager::Instance().IsAttaching())
	{
		return;
	}

	if (CWindowManager::Instance().GetPointWindow() != this)
	{
		return;
	}

	TSlot * pSlot;
	if (!GetSelectedSlotPointer(&pSlot))
	{
		return;
	}

	CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.5f);
	CPythonGraphic::Instance().RenderBar2d(m_rect.left + pSlot->ixPosition,
										   m_rect.top + pSlot->iyPosition,
										   m_rect.left + pSlot->ixPosition + pSlot->ixCellSize,
										   m_rect.top + pSlot->iyPosition + pSlot->iyCellSize);
}

void CSlotWindow::OnRenderSelectedSlot()
{
	if (CWindowManager::Instance().GetPointWindow() != this)
	{
		return;
	}

	std::list<DWORD>::iterator itor = m_dwSelectedSlotIndexList.begin();
	for (; itor != m_dwSelectedSlotIndexList.end(); ++itor)
	{
		TSlot * pSlot;
		if (!GetSlotPointer(*itor, &pSlot))
		{
			continue;
		}

		CPythonGraphic::Instance().SetDiffuseColor(1.0f, 1.0f, 1.0f, 0.5f);
		CPythonGraphic::Instance().RenderBar2d(m_rect.left + pSlot->ixPosition,
											   m_rect.top + pSlot->iyPosition,
											   m_rect.left + pSlot->ixPosition + pSlot->ixCellSize,
											   m_rect.top + pSlot->iyPosition + pSlot->iyCellSize);
	}
}

void CSlotWindow::RenderLockedSlot()
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot& rSlot = *itor;

		if (!rSlot.isItem)
		{
			continue;
		}

		if (rSlot.dwState & SLOT_STATE_LOCK)
		{
			CPythonGraphic::Instance().SetDiffuseColor(rSlot.fLockColour[0], rSlot.fLockColour[1], rSlot.fLockColour[2], rSlot.fLockColour[3]);
			CPythonGraphic::Instance().RenderBar2d(m_rect.left + rSlot.ixPosition,
												   m_rect.top + rSlot.iyPosition,
												   m_rect.left + rSlot.ixPosition + rSlot.ixCellSize,
												   m_rect.top + rSlot.iyPosition + (rSlot.iyCellSize * rSlot.byyPlacedItemSize));
		}
	}
}


// Check Slot

BOOL CSlotWindow::GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot)
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (dwIndex == rSlot.dwSlotNumber)
		{
			*ppSlot = &rSlot;
			return TRUE;
		}
	}

	return FALSE;
}

BOOL CSlotWindow::GetSelectedSlotPointer(TSlot ** ppSlot)
{
	long lx, ly;
	GetMouseLocalPosition(lx, ly);

	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		if (lx >= rSlot.ixPosition)
			if (ly >= rSlot.iyPosition)
				if (lx <= rSlot.ixPosition + rSlot.ixCellSize)
					if (ly <= rSlot.iyPosition + rSlot.iyCellSize)
					{
						*ppSlot = &rSlot;
						return TRUE;
					}
	}

	return FALSE;
}

BOOL CSlotWindow::GetPickedSlotPointer(TSlot ** ppSlot)
{
	long lx, ly;
	CWindowManager::Instance().GetMousePosition(lx, ly);

	int ixLocal = lx - m_rect.left;
	int iyLocal = ly - m_rect.top;

	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		int ixCellSize = rSlot.ixCellSize;
		int iyCellSize = rSlot.iyCellSize;

		if (rSlot.isItem)
		{
			ixCellSize = max(rSlot.ixCellSize, int(rSlot.byxPlacedItemSize * ITEM_WIDTH));
			iyCellSize = max(rSlot.iyCellSize, int(rSlot.byyPlacedItemSize * ITEM_HEIGHT));
		}

		if (ixLocal >= rSlot.ixPosition)
			if (iyLocal >= rSlot.iyPosition)
				if (ixLocal <= rSlot.ixPosition + ixCellSize)
					if (iyLocal <= rSlot.iyPosition + iyCellSize)
					{
						*ppSlot = &rSlot;
						return TRUE;
					}
	}

	return FALSE;
}

void CSlotWindow::SetUseMode(BOOL bFlag)
{
	m_isUseMode = bFlag;
}

BOOL CSlotWindow::IsUseMode() const
{
	return m_isUseMode;
}

void CSlotWindow::SetUsableItem(BOOL bFlag)
{
	m_isUsableItem = bFlag;
}

BOOL CSlotWindow::IsUsableItem() const
{
	return m_isUsableItem;
}

void CSlotWindow::SetSwitchMode(BOOL bFlag)
{
	m_isSwitchMode = bFlag;
}

BOOL CSlotWindow::IsSwitchMode() const
{
	return m_isSwitchMode;
}

void CSlotWindow::SetSwitchableItem(BOOL bFlag)
{
	m_isSwitchableItem = bFlag;
}

BOOL CSlotWindow::IsSwitchableItem() const
{
	return m_isSwitchableItem;
}

void CSlotWindow::ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex)
{
	m_ReserveDestroyEffectDeque.push_back(dwSlotIndex);
}

DWORD CSlotWindow::Type()
{
	static int s_Type = GetCRC32("CSlotWindow", strlen("CSlotWindow"));
	return s_Type;
}

BOOL CSlotWindow::OnIsType(DWORD dwType)
{
	if (CSlotWindow::Type() == dwType)
	{
		return TRUE;
	}

	return CWindow::OnIsType(dwType);
}

void CSlotWindow::__CreateToggleSlotImage()
{
	__DestroyToggleSlotImage();

	m_pToggleSlotImage = new CImageBox(NULL);
	m_pToggleSlotImage->LoadImage("d:/ymir work/ui/public/slot_toggle.sub");
	m_pToggleSlotImage->Show();
}

void CSlotWindow::__CreateSlotEnableEffect()
{
	__DestroySlotEnableEffect();

#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	m_pSlotActiveEffect = new CAniImageBox(NULL);
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/00.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/01.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/02.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/03.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/04.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/05.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/06.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/07.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/08.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/09.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/10.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/11.sub");
	m_pSlotActiveEffect->AppendImage("d:/ymir work/ui/public/slotactiveeffect/12.sub");
	m_pSlotActiveEffect->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	m_pSlotActiveEffect->Show();
#else
	for (int i = 0; i < 3; ++i)
	{
		m_pSlotActiveEffect[i] = new CAniImageBox(NULL);
		for (int k = 0; k <= 12; ++k)
		{
			m_pSlotActiveEffect[i]->AppendImage((boost::format("d:/ymir work/ui/public/slotactiveeffect/slot%d/%02d.sub") % (i + 1) % k).str().c_str());
		}

		m_pSlotActiveEffect[i]->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
		m_pSlotActiveEffect[i]->Show();
	}
#endif
}

void CSlotWindow::__CreateNewSlotEnableEffect()
{
	__DestroyNewSlotEnableEffect();

	for (int i = 0; i < 3; ++i)
	{
		m_niSlotActiveEffect[i] = new CAniImageBox(NULL);

		switch (i)
		{
		case 0:
		{
			for (int j = 1; j <= 22; j++)
			{
				char buf[64 + 1];
				sprintf_s(buf, "d:/ymir work/ui/public/newitemeffect/newitemeffect_%06d.sub", j);
				m_niSlotActiveEffect[i]->AppendImage(buf);
			}
		}
		break;

		case 1:
		{
			for (int j = 23; j <= 54; j++)
			{
				char buf[64 + 1];
				sprintf_s(buf, "d:/ymir work/ui/public/newitemeffect/newitemeffect_%06d.sub", j);
				m_niSlotActiveEffect[i]->AppendImage(buf);
			}
		}
		break;

		case 2:
		{
			for (int j = 55; j <= 93; j++)
			{
				char buf[64 + 1];
				sprintf_s(buf, "d:/ymir work/ui/public/newitemeffect/newitemeffect_%06d.sub", j);
				m_niSlotActiveEffect[i]->AppendImage(buf);
			}
		}
		break;
		}

		m_niSlotActiveEffect[i]->SetDelay(5);
		m_niSlotActiveEffect[i]->Show();

		//m_niSlotActiveEffect[i] = pNEff;
	}
}

void CSlotWindow::__CreateFinishCoolTimeEffect(TSlot * pSlot)
{
	__DestroyFinishCoolTimeEffect(pSlot);

	CAniImageBox * pFinishCoolTimeEffect = new CCoolTimeFinishEffect(this, pSlot->dwSlotNumber);
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/00.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/01.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/02.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/03.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/04.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/05.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/06.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/07.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/08.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/09.sub");
	pFinishCoolTimeEffect->AppendImage("d:/ymir work/ui/public/slotfinishcooltimeeffect/10.sub");
	pFinishCoolTimeEffect->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	pFinishCoolTimeEffect->ResetFrame();
	pFinishCoolTimeEffect->SetDelay(2);
	pFinishCoolTimeEffect->Show();

	pSlot->pFinishCoolTimeEffect = pFinishCoolTimeEffect;
}

void CSlotWindow::__CreateBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa)
{
	__DestroyBaseImage();

	CGraphicImage * pImage = (CGraphicImage *)CResourceManager::Instance().GetResourcePointer(c_szFileName);
	m_pBaseImageInstance = CGraphicImageInstance::New();
	m_pBaseImageInstance->SetImagePointer(pImage);
	m_pBaseImageInstance->SetDiffuseColor(fr, fg, fb, fa);
}

/*** Start New Pet System ***/
void CSlotWindow::__CreateBaseImageScale(const char* c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy)
{
	__DestroyBaseImage();
	CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(c_szFileName);
	m_pBaseImageInstance = CGraphicImageInstance::New();
	m_pBaseImageInstance->SetImagePointer(pImage);
	m_pBaseImageInstance->SetDiffuseColor(fr, fg, fb, fa);
	m_pBaseImageInstance->SetScale(sx, sy);
}
/*** End New Pet System ***/

void CSlotWindow::__DestroyToggleSlotImage()
{
	if (m_pToggleSlotImage)
	{
		delete m_pToggleSlotImage;
		m_pToggleSlotImage = NULL;
	}
}

void CSlotWindow::__DestroySlotEnableEffect()
{
#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	if (m_pSlotActiveEffect)
	{
		delete m_pSlotActiveEffect;
		m_pSlotActiveEffect = NULL;
	}
#else
	for (int i = 0; i < 3; ++i)
	{
		if (m_pSlotActiveEffect[i])
		{
			delete m_pSlotActiveEffect[i];
			m_pSlotActiveEffect[i] = NULL;
		}
	}
#endif
}

void CSlotWindow::__DestroyNewSlotEnableEffect()
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_niSlotActiveEffect[i])
		{
			delete m_niSlotActiveEffect[i];
			m_niSlotActiveEffect[i] = NULL;
		}
	}
}

void CSlotWindow::__DestroyFinishCoolTimeEffect(TSlot * pSlot)
{
	if (pSlot->pFinishCoolTimeEffect)
	{
		delete pSlot->pFinishCoolTimeEffect;
		pSlot->pFinishCoolTimeEffect = NULL;
	}
}

void CSlotWindow::__DestroyBaseImage()
{
	if (m_pBaseImageInstance)
	{
		CGraphicImageInstance::Delete(m_pBaseImageInstance);
		m_pBaseImageInstance = NULL;
	}
}

void CSlotWindow::__Initialize()
{
	m_dwSlotType = 0;
	m_dwSlotStyle = SLOT_STYLE_PICK_UP;
	m_dwToolTipSlotNumber = SLOT_NUMBER_NONE;
	m_dwOverInSlotNumber = SLOT_NUMBER_NONE;

	m_isUseMode = FALSE;
	m_isUsableItem = FALSE;
	m_isSwitchMode = FALSE;
	m_isSwitchableItem = FALSE;

	m_pToggleSlotImage = NULL;

#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	m_pSlotActiveEffect = NULL;
#else
	for (int i = 0; i < 3; ++i)
	{
		m_pSlotActiveEffect[i] = NULL;
		m_niSlotActiveEffect[i] = NULL;
	}
#endif
	m_pBaseImageInstance = NULL;

#ifdef TRANSMUTATION_SYSTEM
	for (int i = 0; i < 3; ++i)
	{
		m_pSlotChangeLookActiveEffect[i] = NULL;
	}

	m_pSlotCoverImage = NULL;
#endif
}

void CSlotWindow::Destroy()
{
	for (TSlotListIterator itor = m_SlotList.begin(); itor != m_SlotList.end(); ++itor)
	{
		TSlot & rSlot = *itor;

		ClearSlot(&rSlot);

		if (rSlot.pNumberLine)
		{
			delete rSlot.pNumberLine;
			rSlot.pNumberLine = NULL;
		}
		if (rSlot.pCoverButton)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pCoverButton);
		}
		if (rSlot.pSlotButton)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pSlotButton);
		}
		if (rSlot.pSignImage)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pSignImage);
		}
		if (rSlot.pFinishCoolTimeEffect)
		{
			CWindowManager::Instance().DestroyWindow(rSlot.pFinishCoolTimeEffect);
		}
	}

	m_SlotList.clear();

	__DestroyToggleSlotImage();
	__DestroySlotEnableEffect();
	__DestroyBaseImage();

#ifdef TRANSMUTATION_SYSTEM
	__DestroySlotChangeLookEnableEffect();
	__DestroySlotCoverImage();
#endif

	__Initialize();
}

CSlotWindow::CSlotWindow(PyObject * ppyObject) : CWindow(ppyObject)
{
	__Initialize();
}

CSlotWindow::~CSlotWindow()
{
	Destroy();
}

#ifdef TRANSMUTATION_SYSTEM
void CSlotWindow::EnableSlotCoverImage(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bSlotCoverImage = TRUE;

	if (!m_pSlotCoverImage)
	{
		__CreateSlotCoverImage();
	}
}

void CSlotWindow::DisableSlotCoverImage(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bSlotCoverImage = FALSE;
}

void CSlotWindow::ActivateChangeLookSlot(DWORD dwIndex, BYTE diffuse_type)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bChangeLookActive = TRUE;

	if (!m_pSlotChangeLookActiveEffect[0] || !m_pSlotChangeLookActiveEffect[1] || !m_pSlotChangeLookActiveEffect[2])
	{
		__CreateSlotChangeLookEnableEffect(diffuse_type);
	}
}

void CSlotWindow::DeactivateChangeLookSlot(DWORD dwIndex)
{
	TSlot* pSlot;
	if (!GetSlotPointer(dwIndex, &pSlot))
	{
		return;
	}

	pSlot->bChangeLookActive = FALSE;
}

void CSlotWindow::__CreateSlotCoverImage()
{
	__DestroySlotCoverImage();

	m_pSlotCoverImage = new CAniImageBox(NULL);
	m_pSlotCoverImage->AppendImage("icon/item/ingame_convert_mark.tga");
	m_pSlotCoverImage->Show();
}

void CSlotWindow::__DestroySlotCoverImage()
{
	if (m_pSlotCoverImage)
	{
		delete m_pSlotCoverImage;
		m_pSlotCoverImage = NULL;
	}
}

void CSlotWindow::__CreateSlotChangeLookEnableEffect(BYTE diffuse_type)
{
	__DestroySlotChangeLookEnableEffect();

	for (int i = 0; i < 3; ++i)
	{
		CAniImageBox* pEff = new CAniImageBox(NULL);
		for (int j = 0; j <= 12; ++j)
		{
			char buf[64 + 1];
			sprintf_s(buf, "d:/ymir work/ui/public/slotactiveeffect/slot%d/%02d.sub", (i + 1), j);
			pEff->SetSlotDiffuseColor(buf, diffuse_type);
		}

		pEff->SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
		m_pSlotChangeLookActiveEffect[i] = pEff;
	}
}

void CSlotWindow::__DestroySlotChangeLookEnableEffect()
{
	for (int i = 0; i < 3; ++i)
	{
		if (m_pSlotChangeLookActiveEffect[i])
		{
			delete m_pSlotChangeLookActiveEffect[i];
			m_pSlotChangeLookActiveEffect[i] = NULL;
		}
	}
}
#endif
