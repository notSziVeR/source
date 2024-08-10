#include "StdAfx.h"

#ifdef ENABLE_RT_EXTENSION
#include "GrpRenderTarget.h"

#include "../EterLib/Camera.h"
#include "../EterPythonLib/PythonGraphic.h"

#include "../EterBase/CRC32.h"
#include "../GameLib/GameType.h"
#include "../GameLib/MapType.h"
#include "../GameLib/ItemData.h"
#include "../GameLib/ActorInstance.h"
#include "../UserInterface/InstanceBase.h"
//#ifndef __D3DRM_H__
//#define __D3DRM_H__
//#endif


//#include <dshow.h>
//#include <qedit.h>

//#include "../EterPythonLib/PythonGraphic.h"
//#include "../UserInterface/PythonApplication.h"



CGraphicRenderTarget::CGraphicRenderTarget()
	: model_(nullptr), rotation_(false), motion_(false), created_(false), cam_distance_(1500.0f), light_position_(50.0f, 150.0f, 350.0f)
{}

CGraphicRenderTarget::~CGraphicRenderTarget()
{
	Destroy();
}

void CGraphicRenderTarget::SetRenderTarget(uint32_t race)
{
	if (!model_)
	{
		model_ = new CInstanceBase();

		CInstanceBase::SCreateData createData;
		memset(&createData, 0, sizeof(createData));

		createData.m_dwRace = race;
		createData.m_bType = CActorInstance::TYPE_RENDER_TARGET;

		if (model_->Create(createData))
		{
			model_->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));

			created_ = true;
		}
		else
		{
			delete model_;
			model_ = NULL;
		}
	}

	if (created_)
	{
		model_->SetRace(race);
		model_->SetRotation(0.0f);
		model_->SetArmor(0);
		model_->SetHair(0);
		model_->SetWeapon(0);
#ifdef ENABLE_SASH_COSTUME_SYSTEM
		model_->SetSash(0);
#endif

		/*
		DWORD flags[TOGGLE_SHINING_FLAG_32CNT];
		ZeroMemory(flags, sizeof(flags));
		model_->SetToggleShiningFlag(flags);
		*/
		model_->SetMotionMode(CRaceMotionData::MODE_ONEHAND_SWORD);
		model_->SetLoopMotion(CRaceMotionData::NAME_WAIT);
		model_->Update();

		model_->Refresh(CRaceMotionData::NAME_WAIT, true);
	}
}

void CGraphicRenderTarget::SetRenderHair(uint32_t vnum)
{
	if (created_)
	{
		model_->ChangeHair(vnum);
	}
}

void CGraphicRenderTarget::SetRenderArmor(uint32_t vnum)
{
	if (created_)
	{
		model_->ChangeArmor(vnum);
	}
}

void CGraphicRenderTarget::SetRenderWeapon(uint32_t vnum)
{
	if (created_)
	{
		model_->ChangeWeapon(vnum);
	}
}

#ifdef ENABLE_SASH_COSTUME_SYSTEM
void CGraphicRenderTarget::SetRenderSash(uint32_t vnum)
{
	if (created_)
	{
		model_->ChangeSash(vnum);
	}
}
#endif

#ifdef ENABLE_SHINING_SYSTEM
void CGraphicRenderTarget::SetRenderToggleShining(DWORD flags[TOGGLE_SHINING_FLAG_32CNT])
{
	if (created_)
	{
		model_->SetToggleShiningFlag(flags);
	}
}
#endif

void CGraphicRenderTarget::SetRendererMotion(uint32_t vnum)
{
	if (created_)
	{
		model_->SetSkillPreview(vnum, vnum);
		motion_ = true;
		model_->Update();
	}
}

void CGraphicRenderTarget::SetRenderDistance(float distance)
{
	cam_distance_ = distance;
}

float CGraphicRenderTarget::GetRenderDistance()
{
	return cam_distance_;
}

void CGraphicRenderTarget::SetLightPosition(D3DXVECTOR3 light_position)
{
	light_position_ = light_position;
}

const D3DXVECTOR3& CGraphicRenderTarget::GetLightPosition() const
{
	return light_position_;
}

void CGraphicRenderTarget::SetRotation(float rotation)
{
	if (created_)
	{
		model_->SetRotation(rotation);
		model_->Update();
	}
}

float CGraphicRenderTarget::GetRotation()
{
	if (created_)
	{
		return model_->GetRotation();
	}

	return 0.0f;
}

void CGraphicRenderTarget::SetRotationMode(bool enable)
{
	rotation_ = enable;
}

void CGraphicRenderTarget::Update()
{
	if (!created_ || (!rotation_ && !motion_))
	{
		return;
	}

	if (rotation_)
	{
		float rotation = model_->GetRotation() - 0.5f;
		if (rotation <= 0.0f)
		{
			rotation = 360.0f;
		}

		model_->SetRotation(rotation);
	}

	if (motion_)
	{
		if (!model_->GetGraphicThingInstanceRef().IsUsingMovingSkill())
		{
			model_->NEW_SetPixelPosition(TPixelPosition(0.0f, 0.0f, 0.0f));
			//motion_ = false;
		}
	}

	model_->Update();
}

void CGraphicRenderTarget::Render(float x, float y, float width, float height)
{
	// TODO: Fix off screen rendering.
	//       The model can be cutted so only the part on screen
	//       should be rendered. Another option (dirtier) to
	//       make the rendering work flawless is to allow off
	//       off screen rendering.

	if (!created_)
	{
		return;
	}

	auto& graphic = CPythonGraphic::Instance();
	//auto& application = CPythonApplication::Instance();
	auto& camera = CCameraManager::Instance();

	graphic.ClearDepthBuffer();

	float fov = graphic.GetFOV(),
		  aspect = graphic.GetAspect(),
		  nearY = graphic.GetNear(),
		  farY = graphic.GetFar();

	graphic.SetGameRenderState();
	graphic.PushState();
	graphic.SetOmniLight(light_position_);

	camera.SetCurrentCamera(CCameraManager::RENDER_TARGET_CAMERA);

	graphic.SetViewport(x, y, width, height);

	camera.GetCurrentCamera()->SetViewParams(
		D3DXVECTOR3(0.0f, -cam_distance_, 600.0f),
		D3DXVECTOR3(0.0f, 0.0f, 95.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f)
	);

	graphic.UpdateViewMatrix();
	graphic.SetPerspective(10.0f, width / height, 1000.0f, 3000.0f);

	model_->Transform();
	model_->Deform();
	model_->Render();

	/*
	for (auto& effectInfo : model_->GetGraphicThingInstancePtr()->GetAttachedAffectList())
	{
		auto effect = CEffectManager::Instance().GetEffectInstance(effectInfo.dwEffectIndex);
		if (!effect)
		{
			TraceError("cannot get effect by %u", effectInfo.dwEffectIndex);
			continue;
		}

		effect->Update();
		effect->Transform();
		effect->Deform();
		effect->Render();
	}
	*/

	camera.ResetToPreviousCamera();

	graphic.RestoreViewport();
	graphic.PopState();
	graphic.SetPerspective(fov, aspect, nearY, farY);
	graphic.SetInterfaceRenderState();
}

void CGraphicRenderTarget::Destroy()
{
	if (created_)
	{
		created_ = false;

		model_->Destroy();
		delete model_;

		model_ = NULL;
	}
}
#endif
