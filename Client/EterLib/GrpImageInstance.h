#pragma once

#include "GrpImage.h"
#include "GrpIndexBuffer.h"
#include "GrpVertexBufferDynamic.h"
#include "Pool.h"

class CGraphicImageInstance
{
public:
	static DWORD Type();
	BOOL IsType(DWORD dwType);

public:
	CGraphicImageInstance();
	virtual ~CGraphicImageInstance();

	void Destroy();

	void Render();

	void SetDiffuseColor(float fr, float fg, float fb, float fa);

	/*** Start New Pet System ***/
	// void SetScale(float sx, float sy); // Default without ENABLE_RENDER_TARGET
	void SetScale(float fx, float fy);
	/*** End New Pet System ***/

	void SetPosition(float fx, float fy);

	void SetImagePointer(CGraphicImage* pImage);
	void ReloadImagePointer(CGraphicImage* pImage);
	bool IsEmpty() const;

	int GetWidth();
	int GetHeight();

	CGraphicTexture * GetTexturePointer();
	const CGraphicTexture &	GetTextureReference() const;
	CGraphicImage * GetGraphicImagePointer();

	bool operator == (const CGraphicImageInstance & rhs) const;

protected:
	void Initialize();

	virtual void OnRender();
	virtual void OnSetImagePointer();

	virtual BOOL OnIsType(DWORD dwType);

protected:
	D3DXCOLOR m_DiffuseColor;
	D3DXVECTOR2 m_v2Position;
	CGraphicImage::TRef m_roImage;
	/*** Start New Pet System ***/
	D3DXVECTOR2 m_v2Scale;
	/*** End New Pet System ***/

public:
	static void CreateSystem(UINT uCapacity);
	static void DestroySystem();

	static CGraphicImageInstance* New();
	static void Delete(CGraphicImageInstance* pkImgInst);

	static CDynamicPool<CGraphicImageInstance>		ms_kPool;
};
