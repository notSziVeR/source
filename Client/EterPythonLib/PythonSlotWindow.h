#pragma once

#include "PythonWindow.h"

namespace UI
{
enum
{
	ITEM_WIDTH = 32,
	ITEM_HEIGHT = 32,

	SLOT_NUMBER_NONE = 0xffffffff,
};

enum ESlotStyle
{
	SLOT_STYLE_NONE,
	SLOT_STYLE_PICK_UP,
	SLOT_STYLE_SELECT,
};

enum ESlotState
{
	SLOT_STATE_LOCK		= (1 << 0),
	SLOT_STATE_CANT_USE	= (1 << 1),
	SLOT_STATE_DISABLE	= (1 << 2),
	SLOT_STATE_ALWAYS_RENDER_COVER = (1 << 3),
#ifdef ENABLE_RENEWAL_SHOP_SELLING
	SLOT_STATE_UNUSABLE = (1 << 4),
#endif
#ifdef ENABLE_RENEWAL_EXCHANGE
	SLOT_STATE_HIGHLIGHT_GREEN = (1 << 5),
#endif
};

class CSlotWindow : public CWindow
{
public:
	static DWORD Type();

public:
	class CSlotButton;
	class CCoverButton;
	class CCoolTimeFinishEffect;

	friend class CSlotButton;
	friend class CCoverButton;

	typedef struct SSlot
	{
		DWORD	dwState;
		DWORD	dwSlotNumber;
		DWORD	dwCenterSlotNumber;
		DWORD	dwItemIndex;
		BOOL	isItem;

		// CoolTime
		float	fCoolTime;
		float	fStartCoolTime;

		// Toggle
		BOOL	bActive;
		BOOL	nbActive;

		int		ixPosition;
		int		iyPosition;

		int		ixCellSize;
		int		iyCellSize;

		BYTE	byxPlacedItemSize;
		BYTE	byyPlacedItemSize;

		CGraphicImageInstance * pInstance;
		CNumberLine * pNumberLine;

		bool	bRenderBaseSlotImage;
		CCoverButton * pCoverButton;
		CSlotButton * pSlotButton;
		CImageBox * pSignImage;
		CAniImageBox * pFinishCoolTimeEffect;

#ifdef TRANSMUTATION_SYSTEM
		BOOL	bChangeLookActive;
		BOOL	bSlotCoverImage;
#endif

		float fLockColour[4];
		float fActtivateColour[4];
	} TSlot;
	typedef std::list<TSlot> TSlotList;
	typedef TSlotList::iterator TSlotListIterator;

public:
	CSlotWindow(PyObject * ppyObject);
	virtual ~CSlotWindow();

	void Destroy();

	// Manage Slot
	void SetSlotType(DWORD dwType);
	void SetSlotStyle(DWORD dwStyle);

	void SetSlotBaseImageScale(const char* c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy);
	void SetSlotScale(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage* pImage, float sx, float sy, D3DXCOLOR& diffuseColor);

	void AppendSlot(DWORD dwIndex, int ixPosition, int iyPosition, int ixCellSize, int iyCellSize);
	void SetCoverButton(DWORD dwIndex, const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName, const char * c_szDisableImageName, BOOL bLeftButtonEnable, BOOL bRightButtonEnable);
	void SetSlotBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);
	void AppendSlotButton(const char * c_szUpImageName, const char * c_szOverImageName, const char * c_szDownImageName);
	void AppendRequirementSignImage(const char * c_szImageName);

	void DeleteCoverButton(DWORD dwIndex);

	void EnableCoverButton(DWORD dwIndex);
	void DisableCoverButton(DWORD dwIndex);
	void SetAlwaysRenderCoverButton(DWORD dwIndex, bool bAlwaysRender = false);

	void ShowSlotBaseImage(DWORD dwIndex);
	void HideSlotBaseImage(DWORD dwIndex);
	BOOL IsDisableCoverButton(DWORD dwIndex);
	BOOL HasSlot(DWORD dwIndex);

	void ClearAllSlot();
	void ClearSlot(DWORD dwIndex);
	void SetSlot(DWORD dwIndex, DWORD dwVirtualNumber, BYTE byWidth, BYTE byHeight, CGraphicImage * pImage, D3DXCOLOR& diffuseColor);
	void SetSlotCount(DWORD dwIndex, DWORD dwCount);
	void SetSlotCountNew(DWORD dwIndex, DWORD dwGrade, DWORD dwCount);
	void SetSlotCoolTime(DWORD dwIndex, float fCoolTime, float fElapsedTime = 0.0f);
	void ActivateSlot(DWORD dwIndex, const std::vector<float>& v_fColMatrix);
	void NewActivateSlot(DWORD dwIndex);
	void DeactivateSlot(DWORD dwIndex);
	void NewDeactivateSlot(DWORD dwIndex);
	void RefreshSlot();

	std::vector<uint32_t> GetSlotIndincies();

#ifdef ENABLE_SLOT_WINDOW_EX
	float GetSlotCoolTime(DWORD dwIndex, float * fElapsedTime);
	bool IsActivatedSlot(DWORD dwIndex);
#endif

#ifdef TRANSMUTATION_SYSTEM
	void EnableSlotCoverImage(DWORD dwIndex);
	void DisableSlotCoverImage(DWORD dwIndex);
	void ActivateChangeLookSlot(DWORD dwIndex, BYTE diffuse_type);
	void DeactivateChangeLookSlot(DWORD dwIndex);
#endif

	void SetSlotBaseImageScale(float fx, float fy);
	void SetScale(float fx, float fy);

	DWORD GetSlotCount();

	void LockSlot(DWORD dwIndex, const float* fLockColour);
	void UnlockSlot(DWORD dwIndex);
	BOOL IsLockSlot(DWORD dwIndex);
	void SetCantUseSlot(DWORD dwIndex);
	void SetUseSlot(DWORD dwIndex);
	BOOL IsCantUseSlot(DWORD dwIndex);
	void EnableSlot(DWORD dwIndex);
	void DisableSlot(DWORD dwIndex);
	BOOL IsEnableSlot(DWORD dwIndex);

#ifdef ENABLE_RENEWAL_SHOP_SELLING
	void SetUnusableSlot(DWORD dwIndex);
	void SetUsableSlot(DWORD dwIndex);
#endif

#ifdef ENABLE_RENEWAL_EXCHANGE
	void DisableSlotHighlightedGreen(DWORD dwIndex);
	void SetSlotHighlightedGreeen(DWORD dwIndex);
#endif

	DWORD GetItemIndex(DWORD dwIndex);

	// Select
	void ClearSelected();
	void SelectSlot(DWORD dwSelectingIndex);
	BOOL isSelectedSlot(DWORD dwIndex);
	DWORD GetSelectedSlotCount();
	DWORD GetSelectedSlotNumber(DWORD dwIndex);

	// Slot Button
	void ShowSlotButton(DWORD dwSlotNumber);
	void HideAllSlotButton();
	void OnPressedSlotButton(DWORD dwType, DWORD dwSlotNumber, BOOL isLeft = TRUE);

	// Requirement Sign
	void ShowRequirementSign(DWORD dwSlotNumber);
	void HideRequirementSign(DWORD dwSlotNumber);

	// ToolTip
	BOOL OnOverInItem(DWORD dwSlotNumber);
	void OnOverOutItem();

	// OverInSlot
	BOOL OnOverIn(DWORD dwSlotNumber);
	void OnOverOut();

	// For Usable Item
	void SetUseMode(BOOL bFlag);
	BOOL IsUseMode() const;
	void SetUsableItem(BOOL bFlag);
	BOOL IsUsableItem() const;
	void SetSwitchMode(BOOL bFlag);
	BOOL IsSwitchMode() const;
	void SetSwitchableItem(BOOL bFlag);
	BOOL IsSwitchableItem() const;

	// CallBack
	void ReserveDestroyCoolTimeFinishEffect(DWORD dwSlotIndex);

protected:
	void __Initialize();
	void __CreateToggleSlotImage();
	void __CreateSlotEnableEffect();
	void __CreateNewSlotEnableEffect();
	void __CreateFinishCoolTimeEffect(TSlot * pSlot);
	void __CreateBaseImage(const char * c_szFileName, float fr, float fg, float fb, float fa);
	/*** Start New Pet System ***/
	void __CreateBaseImageScale(const char* c_szFileName, float fr, float fg, float fb, float fa, float sx, float sy);
	/*** End New Pet System ***/
	void __DestroyToggleSlotImage();
	void __DestroySlotEnableEffect();
	void __DestroyNewSlotEnableEffect();
	void __DestroyFinishCoolTimeEffect(TSlot * pSlot);
	void __DestroyBaseImage();

#ifdef TRANSMUTATION_SYSTEM
	void __CreateSlotChangeLookEnableEffect(BYTE diffuse_type = 0);
	void __CreateSlotCoverImage();
	void __DestroySlotChangeLookEnableEffect();
	void __DestroySlotCoverImage();
#endif

	// Event
	void OnUpdate();
	void OnRender();
	BOOL OnMouseLeftButtonDown();
	BOOL OnMouseLeftButtonUp();
	BOOL OnMouseRightButtonDown();
	BOOL OnMouseLeftButtonDoubleClick();
	void OnMouseOverOut();
	virtual void OnMouseOver();
	void RenderSlotBaseImage();
	void RenderLockedSlot();
	virtual void OnRenderPickingSlot();
	virtual void OnRenderSelectedSlot();

	// Select
	void OnSelectEmptySlot(int iSlotNumber);
	void OnSelectItemSlot(int iSlotNumber);
	void OnUnselectEmptySlot(int iSlotNumber);
	void OnUnselectItemSlot(int iSlotNumber);
	void OnUseSlot();

	// Manage Slot
	BOOL GetSlotPointer(DWORD dwIndex, TSlot ** ppSlot);
	BOOL GetSelectedSlotPointer(TSlot ** ppSlot);
	virtual BOOL GetPickedSlotPointer(TSlot ** ppSlot);
	void ClearSlot(TSlot * pSlot);
	virtual void OnRefreshSlot();

	// ETC
	BOOL OnIsType(DWORD dwType);

protected:
	DWORD m_dwSlotType;
	DWORD m_dwSlotStyle;
	std::list<DWORD> m_dwSelectedSlotIndexList;
	TSlotList m_SlotList;
	DWORD m_dwToolTipSlotNumber;
	DWORD m_dwOverInSlotNumber;

	BOOL m_isUseMode;
	BOOL m_isUsableItem;
	BOOL m_isSwitchMode;
	BOOL m_isSwitchableItem;

	CGraphicImageInstance * m_pBaseImageInstance;
	CImageBox * m_pToggleSlotImage;
#ifndef __ITEM_PICKUP_HIGHTLIGHT__
	CAniImageBox* m_pSlotActiveEffect;
#else
	CAniImageBox* m_pSlotActiveEffect[3];
#endif
	CAniImageBox* m_niSlotActiveEffect[3];
	std::deque<DWORD> m_ReserveDestroyEffectDeque;
#ifdef TRANSMUTATION_SYSTEM
	CAniImageBox* m_pSlotChangeLookActiveEffect[3];
	CAniImageBox* m_pSlotCoverImage;
#endif

};
};