#pragma once

//#include "../UserInterface/InstanceBase.h"
#ifdef ENABLE_RT_EXTENSION
class CInstanceBase;
class CGraphicRenderTarget
{
private:
	CInstanceBase* model_;
	bool rotation_;
	bool motion_;
	bool created_;
	float cam_distance_;

	D3DXVECTOR3 light_position_;

public:
	CGraphicRenderTarget();
	~CGraphicRenderTarget();

	void SetRenderTarget(uint32_t race);
	void SetRenderHair(uint32_t vnum);
	void SetRenderArmor(uint32_t vnum);
	void SetRenderWeapon(uint32_t vnum);
#ifdef ENABLE_SASH_COSTUME_SYSTEM
	void SetRenderSash(uint32_t vnum);
#endif
	void SetRendererMotion(uint32_t vnum);
#ifdef ENABLE_SHINING_SYSTEM
	void SetRenderToggleShining(DWORD flags[TOGGLE_SHINING_FLAG_32CNT]);
#endif
	void SetLightPosition(D3DXVECTOR3 light_position);
	const D3DXVECTOR3& GetLightPosition() const;

	void SetRenderDistance(float distance);
	float GetRenderDistance();

	void SetRotation(float rotation);
	float GetRotation();

	void SetRotationMode(bool enable);

	void Update();
	void Render(float x, float y, float width, float height);

	void Destroy();
};
#endif
