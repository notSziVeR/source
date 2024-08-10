#include "StdAfx.h"
#include "../eterLib/StateManager.h"
#include "../eterLib/GrpSubImage.h"
#include "../eterlib/Camera.h"
#include "../EterPack/EterPackManager.h"

#include "PythonMiniMap.h"
#include "PythonBackground.h"
#include "PythonCharacterManager.h"
#include "PythonGuild.h"

#include "AbstractPlayer.h"

#include "../eterPythonLib/PythonWindowManager.h"

#if defined(ENABLE_ATLAS_MARK_INFO) || defined(ENABLE_MULTI_LANGUAGE_SYSTEM)
#include "PythonNonPlayer.h"
#endif

#include "PythonNonPlayer.h"
#include "../EterPack/EterPackManager.h"
#include "PythonSystem.h"
#include "PythonPlayer.h"

#ifdef ENABLE_ADMIN_MANAGER
#include "PythonAdmin.h"
#include "PythonNonPlayer.h"
#include "PythonChat.h"
#endif

namespace
{
#ifdef __ENABLE_BOSS_ON_ATLAS__
	long lBossImageWidth = 0;
	long lBossImageHeight = 0;
#endif
}


#ifdef ENABLE_PARTY_ATLAS
void CPythonMiniMap::UpdatePartyMember(DWORD dwPID, DWORD dwX, DWORD dwY)
{
	TPartyMember* pMember = NULL;

	auto it = m_kMap_dwPID_kPartyMember.find(dwPID);
	if (it == m_kMap_dwPID_kPartyMember.end())
	{
		CPythonPlayer::TPartyMemberInfo* pInfo;
		if (!CPythonPlayer::Instance().GetPartyMemberPtr(dwPID, &pInfo))
		{
			TraceError("cannot get party member info by pid %u", dwPID);
			return;
		}

		pMember = &m_kMap_dwPID_kPartyMember[dwPID];
		pMember->stName = pInfo->strName;
	}
	else
	{
		pMember = &it->second;
	}

	pMember->fX = dwX;
	pMember->fY = dwY;

	pMember->fScreenX = pMember->fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
	pMember->fScreenY = pMember->fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;
}

void CPythonMiniMap::RemovePartyMember(DWORD dwPID)
{
	m_kMap_dwPID_kPartyMember.erase(dwPID);
}
#endif

void CPythonMiniMap::AddObserver(DWORD dwVID, float fSrcX, float fSrcY)
{
	std::map<DWORD, SObserver>::iterator f = m_kMap_dwVID_kObserver.find(dwVID);
	if (m_kMap_dwVID_kObserver.end() == f)
	{
		SObserver kObserver;
		kObserver.dwSrcTime = ELTimer_GetMSec();
		kObserver.dwDstTime = kObserver.dwSrcTime + 1000;
		kObserver.fSrcX = fSrcX;
		kObserver.fSrcY = fSrcY;
		kObserver.fDstX = fSrcX;
		kObserver.fDstY = fSrcY;
		kObserver.fCurX = fSrcX;
		kObserver.fCurY = fSrcY;
		m_kMap_dwVID_kObserver.insert(std::map<DWORD, SObserver>::value_type(dwVID, kObserver));
	}
	else
	{
		SObserver& rkObserver = f->second;
		rkObserver.dwSrcTime = ELTimer_GetMSec();
		rkObserver.dwDstTime = rkObserver.dwSrcTime + 1000;
		rkObserver.fSrcX = fSrcX;
		rkObserver.fSrcY = fSrcY;
		rkObserver.fDstX = fSrcX;
		rkObserver.fDstY = fSrcY;
		rkObserver.fCurX = fSrcX;
		rkObserver.fCurY = fSrcY;
	}
}

void CPythonMiniMap::MoveObserver(DWORD dwVID, float fDstX, float fDstY)
{
	std::map<DWORD, SObserver>::iterator f = m_kMap_dwVID_kObserver.find(dwVID);
	if (m_kMap_dwVID_kObserver.end() == f)
	{
		return;
	}

	SObserver& rkObserver = f->second;
	rkObserver.dwSrcTime = ELTimer_GetMSec();
	rkObserver.dwDstTime = rkObserver.dwSrcTime + 1000;
	rkObserver.fSrcX = rkObserver.fCurX;
	rkObserver.fSrcY = rkObserver.fCurY;
	rkObserver.fDstX = fDstX;
	rkObserver.fDstY = fDstY;
}

void CPythonMiniMap::RemoveObserver(DWORD dwVID)
{
	m_kMap_dwVID_kObserver.erase(dwVID);
}

void CPythonMiniMap::SetCenterPosition(float fCenterX, float fCenterY)
{
	m_fCenterX = fCenterX;
	m_fCenterY = fCenterY;

	CMapOutdoor& rkMap = CPythonBackground::Instance().GetMapOutdoorRef();
	for (BYTE byTerrainNum = 0; byTerrainNum < AROUND_AREA_NUM; ++byTerrainNum)
	{
		m_lpMiniMapTexture[byTerrainNum] = NULL;
		CTerrain* pTerrain;
		if (rkMap.GetTerrainPointer(byTerrainNum, &pTerrain))
		{
			m_lpMiniMapTexture[byTerrainNum] = pTerrain->GetMiniMapTexture();
		}
	}

	const TOutdoorMapCoordinate& rOutdoorMapCoord = rkMap.GetCurCoordinate();

	m_fCenterCellX = (m_fCenterX - (float)(rOutdoorMapCoord.m_sTerrainCoordX * CTerrainImpl::TERRAIN_XSIZE)) / (float)(CTerrainImpl::CELLSCALE);
	m_fCenterCellY = (m_fCenterY - (float)(rOutdoorMapCoord.m_sTerrainCoordY * CTerrainImpl::TERRAIN_YSIZE)) / (float)(CTerrainImpl::CELLSCALE);

	__SetPosition();
}

void CPythonMiniMap::Update(float fCenterX, float fCenterY)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	if (!rkBG.IsMapOutdoor())
	{
		return;
	}

	if (m_fCenterX != fCenterX || m_fCenterY != fCenterY)
	{
		SetCenterPosition(fCenterX, fCenterY);
	}

	m_OtherPCPositionVector.clear();
	m_PartyPCPositionVector.clear();
	m_NPCPositionVector.clear();
#ifdef ENABLE_STONES_MINIMAP
	m_MetinPositionVector.clear();
#endif
	m_MonsterPositionVector.clear();
	m_WarpPositionVector.clear();

#ifdef ENABLE_OFFLINE_SHOP
	m_vecOfflineShopPositions.clear();
#endif

	float fooCellScale = 1.0f / ((float)CTerrainImpl::CELLSCALE);

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();

	CInstanceBase* pkInstMain = rkChrMgr.GetMainInstancePtr();
	if (!pkInstMain)
	{
		return;
	}

	CPythonCharacterManager::CharacterIterator i;
	for (i = rkChrMgr.CharacterInstanceBegin(); i != rkChrMgr.CharacterInstanceEnd(); ++i)
	{
		CInstanceBase* pkInstEach = *i;

		TPixelPosition kInstancePosition;
		pkInstEach->NEW_GetPixelPosition(&kInstancePosition);
		float fDistanceFromCenterX = (kInstancePosition.x - m_fCenterX) * fooCellScale * m_fScale;
		float fDistanceFromCenterY = (kInstancePosition.y - m_fCenterY) * fooCellScale * m_fScale;
		if (fabs(fDistanceFromCenterX) >= m_fMiniMapRadius || fabs(fDistanceFromCenterY) >= m_fMiniMapRadius)
		{
			continue;
		}

		float fDistanceFromCenter = sqrtf(fDistanceFromCenterX * fDistanceFromCenterX + fDistanceFromCenterY * fDistanceFromCenterY);
		if (fDistanceFromCenter >= m_fMiniMapRadius)
		{
			continue;
		}

		TMarkPosition aMarkPosition;

		if (pkInstEach->IsPC() && !pkInstEach->IsInvisibility()
#ifdef ENABLE_SHAMAN_SYSTEM
			&& !pkInstEach->IsShaman()
#endif
			)
		{
			if (pkInstEach == CPythonCharacterManager::Instance().GetMainInstancePtr())
			{
				continue;
			}

			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;
			aMarkPosition.m_eNameColor = pkInstEach->GetNameColorIndex();
			if (aMarkPosition.m_eNameColor == CInstanceBase::NAMECOLOR_PARTY)
			{
				m_PartyPCPositionVector.push_back(aMarkPosition);
			}
			else
			{
				m_OtherPCPositionVector.push_back(aMarkPosition);
			}
		}
		else if (pkInstEach->IsNPC()
#ifdef ENABLE_SHAMAN_SYSTEM
			|| pkInstEach->IsShaman()
#endif
			)
		{
			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;

#ifdef ENABLE_OFFLINE_SHOP
			if (pkInstEach->GetRace() >= 30000 && pkInstEach->GetRace() <= 30008)
			{
				this->m_vecOfflineShopPositions.push_back(aMarkPosition);
			}
			else
#endif
				m_NPCPositionVector.push_back(aMarkPosition);
		}
#ifdef ENABLE_STONES_MINIMAP
		else if (pkInstEach->IsStone())
		{
			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;

			m_MetinPositionVector.push_back(aMarkPosition);
		}
#endif
		else if (pkInstEach->IsEnemy())
		{
			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;

			m_MonsterPositionVector.push_back(aMarkPosition);
		}
		else if (pkInstEach->IsWarp())
		{
			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;

			m_WarpPositionVector.push_back(aMarkPosition);
		}
	}

	{
		DWORD dwCurTime = ELTimer_GetMSec();

		std::map<DWORD, SObserver>::iterator i;
		for (i = m_kMap_dwVID_kObserver.begin(); i != m_kMap_dwVID_kObserver.end(); ++i)
		{
			SObserver& rkObserver = i->second;

			float fPos = float(dwCurTime - rkObserver.dwSrcTime) / float(rkObserver.dwDstTime - rkObserver.dwSrcTime);
			if (fPos < 0.0f) { fPos = 0.0f; }
			else if (fPos > 1.0f) { fPos = 1.0f; }

			rkObserver.fCurX = (rkObserver.fDstX - rkObserver.fSrcX) * fPos + rkObserver.fSrcX;
			rkObserver.fCurY = (rkObserver.fDstY - rkObserver.fSrcY) * fPos + rkObserver.fSrcY;

			TPixelPosition kInstancePosition;
			kInstancePosition.x = rkObserver.fCurX;
			kInstancePosition.y = rkObserver.fCurY;
			kInstancePosition.z = 0.0f;

			float fDistanceFromCenterX = (kInstancePosition.x - m_fCenterX) * fooCellScale * m_fScale;
			float fDistanceFromCenterY = (kInstancePosition.y - m_fCenterY) * fooCellScale * m_fScale;
			if (fabs(fDistanceFromCenterX) >= m_fMiniMapRadius || fabs(fDistanceFromCenterY) >= m_fMiniMapRadius)
			{
				continue;
			}

			float fDistanceFromCenter = sqrtf(fDistanceFromCenterX * fDistanceFromCenterX + fDistanceFromCenterY * fDistanceFromCenterY);
			if (fDistanceFromCenter >= m_fMiniMapRadius)
			{
				continue;
			}

			TMarkPosition aMarkPosition;
			aMarkPosition.m_fX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
			aMarkPosition.m_fY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;
			aMarkPosition.m_eNameColor = CInstanceBase::NAMECOLOR_PARTY;
			m_PartyPCPositionVector.push_back(aMarkPosition);
		}
	}

	{
		TAtlasMarkInfoVector::iterator itor = m_AtlasWayPointInfoVector.begin();
		for (; itor != m_AtlasWayPointInfoVector.end(); ++itor)
		{
			TAtlasMarkInfo& rAtlasMarkInfo = *itor;

			if (TYPE_TARGET != rAtlasMarkInfo.m_byType)
			{
				continue;
			}

			if (0 != rAtlasMarkInfo.m_dwChrVID)
			{
				CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(rAtlasMarkInfo.m_dwChrVID);
				if (pInstance)
				{
					TPixelPosition kPixelPosition;
					pInstance->NEW_GetPixelPosition(&kPixelPosition);
					__UpdateWayPoint(&rAtlasMarkInfo, kPixelPosition.x, kPixelPosition.y);
				}
			}

			const float c_fMiniMapWindowRadius = 55.0f;

			float fDistanceFromCenterX = (rAtlasMarkInfo.m_fX - m_fCenterX) * fooCellScale * m_fScale;
			float fDistanceFromCenterY = (rAtlasMarkInfo.m_fY - m_fCenterY) * fooCellScale * m_fScale;
			float fDistanceFromCenter = sqrtf(fDistanceFromCenterX * fDistanceFromCenterX + fDistanceFromCenterY * fDistanceFromCenterY);

			if (fDistanceFromCenter >= c_fMiniMapWindowRadius)
			{
				float fRadianX = acosf(fDistanceFromCenterX / fDistanceFromCenter);
				float fRadianY = asinf(fDistanceFromCenterY / fDistanceFromCenter);
				fDistanceFromCenterX = 55.0f * cosf(fRadianX);
				fDistanceFromCenterY = 55.0f * sinf(fRadianY);
				rAtlasMarkInfo.m_fMiniMapX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX + 2.0f;
				rAtlasMarkInfo.m_fMiniMapY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY + 2.0f;
			}
			else
			{
				rAtlasMarkInfo.m_fMiniMapX = (m_fWidth - (float)m_WhiteMark.GetWidth()) / 2.0f + fDistanceFromCenterX + m_fScreenX;
				rAtlasMarkInfo.m_fMiniMapY = (m_fHeight - (float)m_WhiteMark.GetHeight()) / 2.0f + fDistanceFromCenterY + m_fScreenY;
			}
		}
	}

#ifdef __ENABLE_BOSS_ON_ATLAS__
	for (auto& rAtlas : m_AtlasBossInfoVector)
	{
		if (TYPE_BOSS != rAtlas.m_byType)
			continue;
		
		if (0 != rAtlas.m_dwChrVID)
		{
			CInstanceBase* pInstance = CPythonCharacterManager::Instance().GetInstancePtr(rAtlas.m_dwChrVID);
			if (pInstance)
			{
				TPixelPosition kPixelPosition;
				pInstance->NEW_GetPixelPosition(&kPixelPosition);
				__UpdateWayPoint(&rAtlas, kPixelPosition.x, kPixelPosition.y); // Used to update image's location
			}
		}
	}
#endif
}

void CPythonMiniMap::Render(float fScreenX, float fScreenY)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	if (!rkBG.IsMapOutdoor())
	{
		return;
	}

	if (!m_bShow)
	{
		return;
	}

	if (!rkBG.IsMapReady())
	{
		return;
	}

	if (m_fScreenX != fScreenX || m_fScreenY != fScreenY)
	{
		m_fScreenX = fScreenX;
		m_fScreenY = fScreenY;
		__SetPosition();
	}

	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
	STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	STATEMANAGER.SaveSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);


	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SaveTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	STATEMANAGER.SaveTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);

	STATEMANAGER.SetTexture(1, m_MiniMapFilterGraphicImageInstance.GetTexturePointer()->GetD3DTexture());
	STATEMANAGER.SetTransform(D3DTS_TEXTURE1, &m_matMiniMapCover);

	STATEMANAGER.SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	STATEMANAGER.SetStreamSource(0, m_VertexBuffer.GetD3DVertexBuffer(), 20);
	STATEMANAGER.SetIndices(m_IndexBuffer.GetD3DIndexBuffer(), 0);
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorld);

	for (BYTE byTerrainNum = 0; byTerrainNum < AROUND_AREA_NUM; ++byTerrainNum)
	{
		LPDIRECT3DTEXTURE9 pMiniMapTexture = m_lpMiniMapTexture[byTerrainNum];
		STATEMANAGER.SetTexture(0, pMiniMapTexture);
		if (pMiniMapTexture)
		{
			CStateManager& rkSttMgr = CStateManager::Instance();
			rkSttMgr.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, byTerrainNum * 4, 4, byTerrainNum * 6, 2);
		}
		else
		{
			STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
			STATEMANAGER.DrawIndexedPrimitive(D3DPT_TRIANGLELIST, byTerrainNum * 4, 4, byTerrainNum * 6, 2);
			STATEMANAGER.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		}
	}

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAARG2);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAARG1);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_ALPHAOP);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_COLOROP);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);

	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSU);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_ADDRESSV);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXCOORDINDEX);
	STATEMANAGER.RestoreTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS);
	STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSU);
	STATEMANAGER.RestoreSamplerState(1, D3DSAMP_ADDRESSV);

	SetDiffuseOperation();
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matIdentity);

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);

	TInstancePositionVectorIterator aIterator;

	if (m_fScale >= 2.0f)
	{
		// Monster
		STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_MOB));//m_MarkTypeToColorMap[TYPE_MONSTER]);
		aIterator = m_MonsterPositionVector.begin();
		while (aIterator != m_MonsterPositionVector.end())
		{
			TMarkPosition& rPosition = *aIterator;
			m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
			m_WhiteMark.Render();
			++aIterator;
		}

		// Other PC
		aIterator = m_OtherPCPositionVector.begin();
		while (aIterator != m_OtherPCPositionVector.end())
		{
			TMarkPosition& rPosition = *aIterator;
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(rPosition.m_eNameColor));
			m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
			m_WhiteMark.Render();
			++aIterator;
		}

		// Party PC
		if (!m_PartyPCPositionVector.empty())
		{
			float v = (1 + sinf(CTimer::Instance().GetCurrentSecond() * 6)) / 5 + 0.6;
			D3DXCOLOR c(CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_PARTY));//(m_MarkTypeToColorMap[TYPE_PARTY]);
			D3DXCOLOR d(v, v, v, 1);
			D3DXColorModulate(&c, &c, &d);
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, (DWORD)c);
			aIterator = m_PartyPCPositionVector.begin();
			while (aIterator != m_PartyPCPositionVector.end())
			{
				TMarkPosition& rPosition = *aIterator;
				m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
				m_WhiteMark.Render();
				++aIterator;
			}
		}
	}

	// NPC
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_NPC));
	aIterator = m_NPCPositionVector.begin();
	while (aIterator != m_NPCPositionVector.end())
	{
		TMarkPosition& rPosition = *aIterator;
		m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
		m_WhiteMark.Render();
		++aIterator;
	}

#ifdef ENABLE_STONES_MINIMAP
	// Metin
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_METIN));
	aIterator = m_MetinPositionVector.begin();
	while (aIterator != m_MetinPositionVector.end())
	{
		TMarkPosition& rPosition = *aIterator;
		m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
		m_WhiteMark.Render();
		++aIterator;
	}
#endif

	// Warp
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WARP));
	aIterator = m_WarpPositionVector.begin();
	while (aIterator != m_WarpPositionVector.end())
	{
		TMarkPosition& rPosition = *aIterator;
		m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
		m_WhiteMark.Render();
		++aIterator;
	}

#ifdef ENABLE_OFFLINE_SHOP
	if (CPythonSystem::instance().IsShowSalesText())
	{
		STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_OFFLINE_SHOP));
		aIterator = this->m_vecOfflineShopPositions.begin();
		while (aIterator != this->m_vecOfflineShopPositions.end())
		{
			TMarkPosition& rPosition = *aIterator;
			m_WhiteMark.SetPosition(rPosition.m_fX, rPosition.m_fY);
			m_WhiteMark.Render();
			++aIterator;
		}
	}
#endif

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_ALPHAOP);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);

	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MIPFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pkInst)
	{
		float fRotation;
		fRotation = (540.0f - pkInst->GetRotation());
		while (fRotation > 360.0f)
		{
			fRotation -= 360.0f;
		}
		while (fRotation < 0.0f)
		{
			fRotation += 360.0f;
		}

		m_PlayerMark.SetRotation(fRotation);
		m_PlayerMark.Render();
	}

	// Target
	{
		TAtlasMarkInfoVector::iterator itor = m_AtlasWayPointInfoVector.begin();
		for (; itor != m_AtlasWayPointInfoVector.end(); ++itor)
		{
			TAtlasMarkInfo& rAtlasMarkInfo = *itor;

			if (TYPE_TARGET != rAtlasMarkInfo.m_byType)
			{
				continue;
			}
			if (rAtlasMarkInfo.m_fMiniMapX <= 0.0f)
			{
				continue;
			}
			if (rAtlasMarkInfo.m_fMiniMapY <= 0.0f)
			{
				continue;
			}

			__RenderTargetMark(rAtlasMarkInfo.m_fMiniMapX, rAtlasMarkInfo.m_fMiniMapY);
		}
	}

	CCamera* pkCmrCur = CCameraManager::Instance().GetCurrentCamera();

	if (pkCmrCur)
	{
		m_MiniMapCameraraphicImageInstance.SetRotation(pkCmrCur->GetRoll());
		m_MiniMapCameraraphicImageInstance.Render();
	}
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);
}

void CPythonMiniMap::SetScale(float fScale)
{
	if (fScale >= 4.0f)
	{
		fScale = 4.0f;
	}
	if (fScale <= 0.5f)
	{
		fScale = 0.5f;
	}
	m_fScale = fScale;

	__SetPosition();
}

void CPythonMiniMap::ScaleUp()
{
	m_fScale *= 2.0f;
	if (m_fScale >= 4.0f)
	{
		m_fScale = 4.0f;
	}
	__SetPosition();
}

void CPythonMiniMap::ScaleDown()
{
	m_fScale *= 0.5f;
	if (m_fScale <= 0.5f)
	{
		m_fScale = 0.5f;
	}
	__SetPosition();
}

void CPythonMiniMap::SetMiniMapSize(float fWidth, float fHeight)
{
	m_fWidth = fWidth;
	m_fHeight = fHeight;
}

#pragma pack(push)
#pragma pack(1)
typedef struct _MINIMAPVERTEX
{
	float x, y, z;          // position
	float u, v;       // normal
} MINIMAPVERTEX, * LPMINIMAPVERTEX;
#pragma pack(pop)

bool CPythonMiniMap::Create()
{
	const std::string strImageRoot = "D:/ymir work/ui/";
	const std::string strImageFilter = strImageRoot + "minimap_image_filter.dds";
	const std::string strImageCamera = strImageRoot + "minimap_camera.dds";
	const std::string strPlayerMark = strImageRoot + "minimap/playermark.sub";
	const std::string strWhiteMark = strImageRoot + "minimap/whitemark.sub";

#ifdef __ENABLE_BOSS_ON_ATLAS__
	const std::string strBossMark = "assets/ui/minimap/field_boss_mark.tga";
#endif

	CGraphicImage* pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(strImageFilter.c_str());
	m_MiniMapFilterGraphicImageInstance.SetImagePointer(pImage);
	pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(strImageCamera.c_str());
	m_MiniMapCameraraphicImageInstance.SetImagePointer(pImage);

	m_matMiniMapCover._11 = 1.0f / ((float)m_MiniMapFilterGraphicImageInstance.GetWidth());
	m_matMiniMapCover._22 = 1.0f / ((float)m_MiniMapFilterGraphicImageInstance.GetHeight());
	m_matMiniMapCover._33 = 0.0f;

	CGraphicSubImage* pSubImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(strPlayerMark.c_str());
	m_PlayerMark.SetImagePointer(pSubImage);

	pSubImage = (CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(strWhiteMark.c_str());
	m_WhiteMark.SetImagePointer(pSubImage);

#ifdef __ENABLE_BOSS_ON_ATLAS__
	pImage = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(strBossMark.c_str());
	m_BossImageMark.SetImagePointer(pImage);

	::lBossImageWidth = pImage->GetWidth();
	::lBossImageHeight = pImage->GetHeight();
#endif

	char buf[256];
	for (int i = 0; i < MINI_WAYPOINT_IMAGE_COUNT; ++i)
	{
		sprintf(buf, "%sminimap/mini_waypoint%02d.sub", strImageRoot.c_str(), i + 1);
		m_MiniWayPointGraphicImageInstances[i].SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(buf));
		m_MiniWayPointGraphicImageInstances[i].SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	}
	for (int j = 0; j < WAYPOINT_IMAGE_COUNT; ++j)
	{
		sprintf(buf, "%sminimap/waypoint%02d.sub", strImageRoot.c_str(), j + 1);
		m_WayPointGraphicImageInstances[j].SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(buf));
		m_WayPointGraphicImageInstances[j].SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	}
	for (int k = 0; k < TARGET_MARK_IMAGE_COUNT; ++k)
	{
		sprintf(buf, "%sminimap/targetmark%02d.sub", strImageRoot.c_str(), k + 1);
		m_TargetMarkGraphicImageInstances[k].SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(buf));
		m_TargetMarkGraphicImageInstances[k].SetRenderingMode(CGraphicExpandedImageInstance::RENDERING_MODE_SCREEN);
	}

	m_GuildAreaFlagImageInstance.SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer("d:/ymir work/ui/minimap/GuildArea01.sub"));

#pragma pack(push)
#pragma pack(1)
	LPMINIMAPVERTEX		lpMiniMapVertex;
	LPMINIMAPVERTEX		lpOrigMiniMapVertex;
#pragma pack(pop)

	if (!m_VertexBuffer.Create(36, D3DFVF_XYZ | D3DFVF_TEX1, D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED))
	{
		return false;
	}

	if (m_VertexBuffer.Lock((void**)&lpOrigMiniMapVertex))
	{
		char* pchMiniMapVertex = (char*)lpOrigMiniMapVertex;
		memset(pchMiniMapVertex, 0, sizeof(char) * 720);
		lpMiniMapVertex = (LPMINIMAPVERTEX)pchMiniMapVertex;

		for (int iY = -3; iY <= 1; ++iY)
		{
			if (0 == iY % 2)
			{
				continue;
			}
			float fY = 0.5f * ((float)iY);
			for (int iX = -3; iX <= 1; ++iX)
			{
				if (0 == iX % 2)
				{
					continue;
				}
				float fX = 0.5f * ((float)iX);
				lpMiniMapVertex = (LPMINIMAPVERTEX)pchMiniMapVertex;
				lpMiniMapVertex->x = fX;
				lpMiniMapVertex->y = fY;
				lpMiniMapVertex->z = 0.0f;
				lpMiniMapVertex->u = 0.0f;
				lpMiniMapVertex->v = 0.0f;
				pchMiniMapVertex += 20;
				lpMiniMapVertex = (LPMINIMAPVERTEX)pchMiniMapVertex;
				lpMiniMapVertex->x = fX;
				lpMiniMapVertex->y = fY + 1.0f;
				lpMiniMapVertex->z = 0.0f;
				lpMiniMapVertex->u = 0.0f;
				lpMiniMapVertex->v = 1.0f;
				pchMiniMapVertex += 20;
				lpMiniMapVertex = (LPMINIMAPVERTEX)pchMiniMapVertex;
				lpMiniMapVertex->x = fX + 1.0f;
				lpMiniMapVertex->y = fY;
				lpMiniMapVertex->z = 0.0f;
				lpMiniMapVertex->u = 1.0f;
				lpMiniMapVertex->v = 0.0f;
				pchMiniMapVertex += 20;
				lpMiniMapVertex = (LPMINIMAPVERTEX)pchMiniMapVertex;
				lpMiniMapVertex->x = fX + 1.0f;
				lpMiniMapVertex->y = fY + 1.0f;
				lpMiniMapVertex->z = 0.0f;
				lpMiniMapVertex->u = 1.0f;
				lpMiniMapVertex->v = 1.0f;
				pchMiniMapVertex += 20;
			}
		}

		m_VertexBuffer.Unlock();
	}

	if (!m_IndexBuffer.Create(54, D3DFMT_INDEX16))
	{
		return false;
	}

	WORD pwIndices[54] =
	{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,

		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23,

		24, 25, 26, 26, 25, 27,
		28, 29, 30, 30, 29, 31,
		32, 33, 34, 34, 33, 35
	};

	void* pIndices;

	if (m_IndexBuffer.Lock(&pIndices))
	{
		memcpy(pIndices, pwIndices, 54 * sizeof(WORD));
		m_IndexBuffer.Unlock();
	}

	return true;
}

void CPythonMiniMap::__SetPosition()
{
	m_fMiniMapRadius = fMIN(6400.0f / ((float)CTerrainImpl::CELLSCALE) * m_fScale, 64.0f);

	m_matWorld._11 = m_fWidth * m_fScale;
	m_matWorld._22 = m_fHeight * m_fScale;
	m_matWorld._41 = (1.0f + m_fScale) * m_fWidth * 0.5f - m_fCenterCellX * m_fScale + m_fScreenX;
	m_matWorld._42 = (1.0f + m_fScale) * m_fHeight * 0.5f - m_fCenterCellY * m_fScale + m_fScreenY;

	if (!m_MiniMapFilterGraphicImageInstance.IsEmpty())
	{
		m_matMiniMapCover._41 = -(m_fScreenX) / ((float)m_MiniMapFilterGraphicImageInstance.GetWidth());
		m_matMiniMapCover._42 = -(m_fScreenY) / ((float)m_MiniMapFilterGraphicImageInstance.GetHeight());
	}

	if (!m_PlayerMark.IsEmpty())
		m_PlayerMark.SetPosition((m_fWidth - (float)m_PlayerMark.GetWidth()) / 2.0f + m_fScreenX,
			(m_fHeight - (float)m_PlayerMark.GetHeight()) / 2.0f + m_fScreenY);

	if (!m_MiniMapCameraraphicImageInstance.IsEmpty())
		m_MiniMapCameraraphicImageInstance.SetPosition((m_fWidth - (float)m_MiniMapCameraraphicImageInstance.GetWidth()) / 2.0f + m_fScreenX,
			(m_fHeight - (float)m_MiniMapCameraraphicImageInstance.GetHeight()) / 2.0f + m_fScreenY);
}

//////////////////////////////////////////////////////////////////////////
// Atlas

#ifdef __ENABLE_BOSS_ON_ATLAS__
void CPythonMiniMap::ClearAtlasMarkInfo(bool clearStone)
#else
void CPythonMiniMap::ClearAtlasMarkInfo()
#endif
{
	m_AtlasNPCInfoVector.clear();
	m_AtlasWarpInfoVector.clear();
#ifdef __ENABLE_BOSS_ON_ATLAS__
	if (clearStone)
		m_AtlasBossInfoVector.clear();
#endif
}

void CPythonMiniMap::RegisterAtlasMark(BYTE byType, const char* c_szName, long lx, long ly)
{
	TAtlasMarkInfo aAtlasMarkInfo;
//	memset(&aAtlasMarkInfo, 0, sizeof(aAtlasMarkInfo));

	aAtlasMarkInfo.m_fX = float(lx);
	aAtlasMarkInfo.m_fY = float(ly);
	aAtlasMarkInfo.m_strText = c_szName;

	aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
	aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;

	switch (byType)
	{
	case CActorInstance::TYPE_NPC:
		aAtlasMarkInfo.m_byType = TYPE_NPC;
		m_AtlasNPCInfoVector.push_back(aAtlasMarkInfo);
		break;
	case CActorInstance::TYPE_WARP:
		aAtlasMarkInfo.m_byType = TYPE_WARP;
		{
			int iPos = aAtlasMarkInfo.m_strText.find(" ");
			if (iPos >= 0)
			{
				aAtlasMarkInfo.m_strText[iPos] = 0;
			}

			const char* szText = aAtlasMarkInfo.m_strText.c_str();
			const char* szRepPos;
			while (szRepPos = strstr(szText, "_"))
				aAtlasMarkInfo.m_strText[szRepPos - szText] = ' ';
		}
		m_AtlasWarpInfoVector.push_back(aAtlasMarkInfo);
		break;
#ifdef __ENABLE_BOSS_ON_ATLAS__
	case CActorInstance::TYPE_STONE:
		aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)::lBossImageWidth / 2.0f;
		aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)::lBossImageHeight / 2.0f;

		aAtlasMarkInfo.m_byType = TYPE_BOSS;
		m_AtlasBossInfoVector.push_back(aAtlasMarkInfo);
		break;
#endif
	}
}

void CPythonMiniMap::ClearGuildArea()
{
	m_GuildAreaInfoVector.clear();
}

void CPythonMiniMap::RegisterGuildArea(DWORD dwID, DWORD dwGuildID, long x, long y, long width, long height)
{
	TGuildAreaInfo kGuildAreaInfo;
	kGuildAreaInfo.dwGuildID = dwGuildID;
	kGuildAreaInfo.lx = x;
	kGuildAreaInfo.ly = y;
	kGuildAreaInfo.lwidth = width;
	kGuildAreaInfo.lheight = height;
	m_GuildAreaInfoVector.push_back(kGuildAreaInfo);
}

DWORD CPythonMiniMap::GetGuildAreaID(DWORD x, DWORD y)
{
	TGuildAreaInfoVectorIterator itor = m_GuildAreaInfoVector.begin();
	for (; itor != m_GuildAreaInfoVector.end(); ++itor)
	{
		TGuildAreaInfo& rAreaInfo = *itor;

		if (x >= rAreaInfo.lx)
			if (y >= rAreaInfo.ly)
				if (x <= rAreaInfo.lx + rAreaInfo.lwidth)
					if (y <= rAreaInfo.ly + rAreaInfo.lheight)
					{
						return rAreaInfo.dwGuildID;
					}
	}

	return 0xffffffff;
}

void CPythonMiniMap::CreateTarget(int iID, const char* c_szName)
{
	AddWayPoint(TYPE_TARGET, iID, 0.0f, 0.0f, c_szName);
}

void CPythonMiniMap::UpdateTarget(int iID, int ix, int iy)
{
	TAtlasMarkInfo* pkInfo;
	if (!__GetWayPoint(iID, &pkInfo))
	{
		return;
	}

	if (0 != pkInfo->m_dwChrVID)
	{
		if (CPythonCharacterManager::Instance().GetInstancePtr(pkInfo->m_dwChrVID))
		{
			return;
		}
	}

	if (ix < m_dwAtlasBaseX)
	{
		return;
	}
	if (iy < m_dwAtlasBaseY)
	{
		return;
	}
	if (ix > m_dwAtlasBaseX + DWORD(m_fAtlasMaxX))
	{
		return;
	}
	if (iy > m_dwAtlasBaseY + DWORD(m_fAtlasMaxY))
	{
		return;
	}

	__UpdateWayPoint(pkInfo, ix - int(m_dwAtlasBaseX), iy - int(m_dwAtlasBaseY));
}

void CPythonMiniMap::CreateTarget(int iID, const char* c_szName, DWORD dwVID)
{
	AddWayPoint(TYPE_TARGET, iID, 0.0f, 0.0f, c_szName, dwVID);
}

void CPythonMiniMap::DeleteTarget(int iID)
{
	RemoveWayPoint(iID);
}

#define ENABLE_NEW_ATLAS_MARK_INFO
#ifdef ENABLE_NEW_ATLAS_MARK_INFO
#include "PythonNonPlayer.h"
#endif
void CPythonMiniMap::__LoadAtlasMarkInfo()
{
	ClearAtlasMarkInfo();
	ClearGuildArea();

	CPythonBackground& rkBG = CPythonBackground::Instance();
	if (!rkBG.IsMapOutdoor())
	{
		return;
	}

	CMapOutdoor& rkMap = rkBG.GetMapOutdoorRef();

	// LOCALE
	char szAtlasMarkInfoFileName[64 + 1];
	_snprintf(szAtlasMarkInfoFileName, sizeof(szAtlasMarkInfoFileName), "locale/common/map/%s.txt", rkMap.GetName().c_str());
	// END_OF_LOCALE

	CTokenVectorMap stTokenVectorMap;

	if (!LoadMultipleTextData(szAtlasMarkInfoFileName, stTokenVectorMap))
	{
		Tracef(" CPythonMiniMap::__LoadAtlasMarkInfo File Load %s ERROR\n", szAtlasMarkInfoFileName);
		return;
	}

#ifndef __ENABLE_BOSS_ON_ATLAS__
	const std::string strType[TYPE_COUNT] = { "OPC", "OPCPVP", "OPCPVPSELF", "NPC", "MONSTER", "WARP", "WAYPOINT" };
#else
	const std::string strType[TYPE_COUNT] = { "OPC", "OPCPVP", "OPCPVPSELF", "NPC", "MONSTER", "WARP", "WAYPOINT", "PARTY", "EMPIRE", "BOSS" };
#endif

	for (DWORD i = 0; i < stTokenVectorMap.size(); ++i)
	{
		char szMarkInfoName[32 + 1];
		_snprintf(szMarkInfoName, sizeof(szMarkInfoName), "%d", i);

		if (stTokenVectorMap.end() == stTokenVectorMap.find(szMarkInfoName))
		{
			continue;
		}

		const CTokenVector& rVector = stTokenVectorMap[szMarkInfoName];

		TAtlasMarkInfo aAtlasMarkInfo;

#ifdef ENABLE_NEW_ATLAS_MARK_INFO
		if (rVector.size() == 3)
		{
			const std::string& c_rstrType = strType[3];
			const std::string& c_rstrPositionX = rVector[0].c_str();
			const std::string& c_rstrPositionY = rVector[1].c_str();
			const std::string& c_rstrText = rVector[2].c_str();
			auto iVnum = atoi(c_rstrText.c_str());

			aAtlasMarkInfo.m_fX = atof(c_rstrPositionX.c_str());
			aAtlasMarkInfo.m_fY = atof(c_rstrPositionY.c_str());
			aAtlasMarkInfo.m_strText = CPythonNonPlayer::Instance().GetMonsterName(iVnum);
		}
		else
#endif
		{
			const std::string& c_rstrType = rVector[0].c_str();
			const std::string& c_rstrPositionX = rVector[1].c_str();
			const std::string& c_rstrPositionY = rVector[2].c_str();
			const std::string& c_rstVnum = rVector[3].c_str();
			auto iVnum = atoi(c_rstVnum.c_str());
			const std::string& c_rstrText = rVector[4].c_str();

			//memset(&aAtlasMarkInfo, 0, sizeof(aAtlasMarkInfo));

			for (int i = 0; i < TYPE_COUNT; ++i)
			{
				if (0 == c_rstrType.compare(strType[i]))
				{
					aAtlasMarkInfo.m_byType = (BYTE)i;
				}
			}
			aAtlasMarkInfo.m_fX = atof(c_rstrPositionX.c_str());
			aAtlasMarkInfo.m_fY = atof(c_rstrPositionY.c_str());
			aAtlasMarkInfo.m_strText = CPythonNonPlayer::Instance().GetMonsterName(iVnum);
			aAtlasMarkInfo.m_strDuration = c_rstrText.c_str();
		}

		aAtlasMarkInfo.m_fScreenX = aAtlasMarkInfo.m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_WhiteMark.GetWidth() / 2.0f;
		aAtlasMarkInfo.m_fScreenY = aAtlasMarkInfo.m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_WhiteMark.GetHeight() / 2.0f;

		switch (aAtlasMarkInfo.m_byType)
		{
		case TYPE_NPC:
			m_AtlasNPCInfoVector.push_back(aAtlasMarkInfo);
			break;
		case TYPE_WARP:
			m_AtlasWarpInfoVector.push_back(aAtlasMarkInfo);
			break;
#ifdef __ENABLE_BOSS_ON_ATLAS__
		case TYPE_BOSS:
			m_AtlasBossInfoVector.push_back(aAtlasMarkInfo);
			break;
#endif
		}
	}
}

bool CPythonMiniMap::LoadAtlas()
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	if (!rkBG.IsMapOutdoor())
	{
		return false;
	}

	CMapOutdoor& rkMap = rkBG.GetMapOutdoorRef();

	const char* playerMarkFileName = "d:/ymir work/ui/minimap/playermark.sub";

	char atlasFileName[1024 + 1];
	snprintf(atlasFileName, sizeof(atlasFileName), "%s/atlas.sub", rkMap.GetName().c_str());
	if (!CEterPackManager::Instance().isExist(atlasFileName))
	{
		snprintf(atlasFileName, sizeof(atlasFileName), "d:/ymir work/ui/atlas/%s/atlas.sub", rkMap.GetName().c_str());
	}

	m_AtlasImageInstance.Destroy();
	m_AtlasPlayerMark.Destroy();
	CGraphicImage* pkGrpImgAtlas = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(atlasFileName);
	if (pkGrpImgAtlas)
	{
		m_AtlasImageInstance.SetImagePointer(pkGrpImgAtlas);

		if (pkGrpImgAtlas->IsEmpty())
		{
			m_bAtlas = false;
		}
		else
		{
			m_bAtlas = true;
		}
	}
	else
	{
	}
	m_AtlasPlayerMark.SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(playerMarkFileName));
	short sTerrainCountX, sTerrainCountY;
	rkMap.GetBaseXY(&m_dwAtlasBaseX, &m_dwAtlasBaseY);
	rkMap.GetTerrainCount(&sTerrainCountX, &sTerrainCountY);
	m_fAtlasMaxX = (float)sTerrainCountX * CTerrainImpl::TERRAIN_XSIZE;
	m_fAtlasMaxY = (float)sTerrainCountY * CTerrainImpl::TERRAIN_YSIZE;

	m_fAtlasImageSizeX = (float)m_AtlasImageInstance.GetWidth();
	m_fAtlasImageSizeY = (float)m_AtlasImageInstance.GetHeight();

	__LoadAtlasMarkInfo();

	if (m_bShowAtlas)
	{
		OpenAtlasWindow();
	}

	return true;
}

void CPythonMiniMap::__GlobalPositionToAtlasPosition(long lx, long ly, float* pfx, float* pfy)
{
	*pfx = lx / m_fAtlasMaxX * m_fAtlasImageSizeX;
	*pfy = ly / m_fAtlasMaxY * m_fAtlasImageSizeY;
}

void CPythonMiniMap::UpdateAtlas()
{
	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pkInst)
	{
		TPixelPosition kInstPos;
		pkInst->NEW_GetPixelPosition(&kInstPos);

		float fRotation;
		fRotation = (540.0f - pkInst->GetRotation());
		while (fRotation > 360.0f)
		{
			fRotation -= 360.0f;
		}
		while (fRotation < 0.0f)
		{
			fRotation += 360.0f;
		}

		m_AtlasPlayerMark.SetPosition(kInstPos.x / m_fAtlasMaxX * m_fAtlasImageSizeX - (float)m_AtlasPlayerMark.GetWidth() / 2.0f,
			kInstPos.y / m_fAtlasMaxY * m_fAtlasImageSizeY - (float)m_AtlasPlayerMark.GetHeight() / 2.0f);
		m_AtlasPlayerMark.SetRotation(fRotation);
	}

	{
		TGuildAreaInfoVectorIterator itor = m_GuildAreaInfoVector.begin();
		for (; itor != m_GuildAreaInfoVector.end(); ++itor)
		{
			TGuildAreaInfo& rInfo = *itor;
			__GlobalPositionToAtlasPosition(rInfo.lx, rInfo.ly, &rInfo.fsxRender, &rInfo.fsyRender);
			__GlobalPositionToAtlasPosition(rInfo.lx + rInfo.lwidth, rInfo.ly + rInfo.lheight, &rInfo.fexRender, &rInfo.feyRender);
		}
	}
}

void CPythonMiniMap::RenderAtlas(float fScreenX, float fScreenY)
{
	if (!m_bShowAtlas)
	{
		return;
	}

	if (m_fAtlasScreenX != fScreenX || m_fAtlasScreenY != fScreenY)
	{
		m_matWorldAtlas._41 = fScreenX;
		m_matWorldAtlas._42 = fScreenY;
		m_fAtlasScreenX = fScreenX;
		m_fAtlasScreenY = fScreenY;
	}

	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldAtlas);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	m_AtlasImageInstance.Render();

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_NPC));
	m_AtlasMarkInfoVectorIterator = m_AtlasNPCInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasNPCInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		m_WhiteMark.SetPosition(rAtlasMarkInfo.m_fScreenX, rAtlasMarkInfo.m_fScreenY);
		m_WhiteMark.Render();
		++m_AtlasMarkInfoVectorIterator;
	}

	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WARP));
	m_AtlasMarkInfoVectorIterator = m_AtlasWarpInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasWarpInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		m_WhiteMark.SetPosition(rAtlasMarkInfo.m_fScreenX, rAtlasMarkInfo.m_fScreenY);
		m_WhiteMark.Render();
		++m_AtlasMarkInfoVectorIterator;
	}

#ifdef ENABLE_PARTY_ATLAS
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_PARTY));
	for (auto it = m_kMap_dwPID_kPartyMember.begin(); it != m_kMap_dwPID_kPartyMember.end(); ++it)
	{
		TPartyMember& rMember = it->second;
		m_WhiteMark.SetPosition(rMember.fScreenX, rMember.fScreenY);
		m_WhiteMark.Render();
	}
#endif

	STATEMANAGER.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WAYPOINT));
	m_AtlasMarkInfoVectorIterator = m_AtlasWayPointInfoVector.begin();
	for (; m_AtlasMarkInfoVectorIterator != m_AtlasWayPointInfoVector.end(); ++m_AtlasMarkInfoVectorIterator)
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;

		if (rAtlasMarkInfo.m_fScreenX <= 0.0f)
		{
			continue;
		}
		if (rAtlasMarkInfo.m_fScreenY <= 0.0f)
		{
			continue;
		}

		if (TYPE_TARGET == rAtlasMarkInfo.m_byType)
		{
			__RenderMiniWayPointMark(rAtlasMarkInfo.m_fScreenX, rAtlasMarkInfo.m_fScreenY);
		}
		else
		{
			__RenderWayPointMark(rAtlasMarkInfo.m_fScreenX, rAtlasMarkInfo.m_fScreenY);
		}
	}

#ifdef __ENABLE_BOSS_ON_ATLAS__
	m_AtlasMarkInfoVectorIterator = m_AtlasBossInfoVector.begin();

	for (const auto& rAtlas : m_AtlasBossInfoVector)
	{
		m_BossImageMark.SetPosition(rAtlas.m_fScreenX - lBossImageWidth / 2, rAtlas.m_fScreenY - lBossImageWidth / 2);
		m_BossImageMark.Render();
	}
#endif

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);

	if ((ELTimer_GetMSec() / 500) % 2)
	{
		m_AtlasPlayerMark.Render();
	}

	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MINFILTER);
	STATEMANAGER.RestoreSamplerState(0, D3DSAMP_MAGFILTER);

	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matIdentity);

	{
		TGuildAreaInfoVectorIterator itor = m_GuildAreaInfoVector.begin();
		for (; itor != m_GuildAreaInfoVector.end(); ++itor)
		{
			TGuildAreaInfo& rInfo = *itor;

			m_GuildAreaFlagImageInstance.SetPosition(fScreenX + (rInfo.fsxRender + rInfo.fexRender) / 2.0f - m_GuildAreaFlagImageInstance.GetWidth() / 2,
				fScreenY + (rInfo.fsyRender + rInfo.feyRender) / 2.0f - m_GuildAreaFlagImageInstance.GetHeight() / 2);
			m_GuildAreaFlagImageInstance.Render();

			//			CScreen::RenderBar2d(fScreenX+rInfo.fsxRender,
			//								 fScreenY+rInfo.fsyRender,
			//								 fScreenX+rInfo.fexRender,
			//								 fScreenY+rInfo.feyRender);
		}
	}
}

#ifdef ENABLE_ADMIN_MANAGER
bool CPythonMiniMap::LoadAdminManagerAtlas()
{
	const char* playerMarkFileName = "d:/ymir work/ui/minimap/playermark.sub";
	const char* otherPlayerMarkFileName = "d:/ymir work/ui/minimap/whitecirclemark.sub";

	const std::string& stBaseMapName = CPythonAdmin::Instance().MapViewer_GetMapName();
	std::string stMapName;

	//if (stBaseMapName.find('/') != std::string::npos)
	//	stMapName = (stBaseMapName.c_str() + stBaseMapName.find_last_of('/') + 1);
	//else
	stMapName = stBaseMapName;

	char atlasFileName[1024 + 1];
	snprintf(atlasFileName, sizeof(atlasFileName), "%s/atlas.sub", stMapName.c_str());
#ifdef ENABLE_FOX_FS
	if (!CFileSystem::Instance().isExist(atlasFileName, __FUNCTION__, false))
#else
	if (!CEterPackManager::Instance().isExist(atlasFileName))
#endif
		snprintf(atlasFileName, sizeof(atlasFileName), "d:/ymir work/ui/atlas/%s/atlas.sub", stMapName.c_str());

	m_AdminManagerAtlasImageInstance.Destroy();
	m_AdminManagerAtlasPlayerMark.Destroy();
	m_AdminManagerAtlasOtherPlayerMark.Destroy();
	CGraphicImage* pkGrpImgAtlas = (CGraphicImage*)CResourceManager::Instance().GetResourcePointer(atlasFileName);

	if (pkGrpImgAtlas)
	{
		m_AdminManagerAtlasImageInstance.SetImagePointer(pkGrpImgAtlas);

		if (pkGrpImgAtlas->IsEmpty())
		{
			m_bAdminManagerAtlas = false;
		}
		else
		{
			m_bAdminManagerAtlas = true;
		}
	}
	else
	{
		TraceError("cannot load atlas file %s", atlasFileName);
		m_bAdminManagerAtlas = false;
	}

	m_AdminManagerAtlasPlayerMark.SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(playerMarkFileName));
	m_AdminManagerAtlasOtherPlayerMark.SetImagePointer((CGraphicSubImage*)CResourceManager::Instance().GetResourcePointer(otherPlayerMarkFileName));

	m_fAdminManagerAtlasMaxX = (float)CPythonAdmin::Instance().MapViewer_GetSizeX() * CTerrainImpl::TERRAIN_XSIZE;
	m_fAdminManagerAtlasMaxY = (float)CPythonAdmin::Instance().MapViewer_GetSizeY() * CTerrainImpl::TERRAIN_YSIZE;

	m_fAdminManagerAtlasImageSizeX = (float)m_AdminManagerAtlasImageInstance.GetWidth();
	m_fAdminManagerAtlasImageSizeY = (float)m_AdminManagerAtlasImageInstance.GetHeight();
	m_fAdminManagerAtlasImageSizeScale = 1.0f;

	return true;
}

void CPythonMiniMap::ConvertGlobalPositionToAdminManagerAtlasData(long lX, long lY, float& fRetX, float& fRetY)
{
	lX -= CPythonAdmin::Instance().MapViewer_GetBaseX();
	lY -= CPythonAdmin::Instance().MapViewer_GetBaseY();
	ConvertPositionToAdminManagerAtlasData(lX, lY, fRetX, fRetY);
}

void CPythonMiniMap::ConvertPositionToAdminManagerAtlasData(long lX, long lY, float& fRetX, float& fRetY)
{
	float& fScale = m_fAdminManagerAtlasImageSizeScale;

	if (IsAdminManagerAtlasRotated())
	{
		float fXDif = m_matWorldAdminManagerAtlas._41 - m_fAdminManagerAtlasScreenX;
		float fYDif = m_matWorldAdminManagerAtlas._42 - m_fAdminManagerAtlasScreenY;
		fRetX = m_fAdminManagerAtlasImageSizeY * fScale - ((float)lY / m_fAdminManagerAtlasMaxY * m_fAdminManagerAtlasImageSizeY * fScale) - fXDif;
		fRetY = (float)lX / m_fAdminManagerAtlasMaxX * m_fAdminManagerAtlasImageSizeX * fScale - fYDif;
	}
	else
	{
		fRetX = (float)lX / m_fAdminManagerAtlasMaxX * m_fAdminManagerAtlasImageSizeX * fScale;
		fRetY = (float)lY / m_fAdminManagerAtlasMaxY * m_fAdminManagerAtlasImageSizeY * fScale;
	}
}

void CPythonMiniMap::ConvertAdminManagerAtlasDataToPosition(float fX, float fY, float& fRetX, float& fRetY)
{
	float& fScale = m_fAdminManagerAtlasImageSizeScale;

	if (IsAdminManagerAtlasRotated())
	{
		float fXDif = m_matWorldAdminManagerAtlas._41 - m_fAdminManagerAtlasScreenX;
		float fYDif = m_matWorldAdminManagerAtlas._42 - m_fAdminManagerAtlasScreenY;
		fRetY = (m_fAdminManagerAtlasImageSizeY * fScale - fX - fXDif) * m_fAdminManagerAtlasMaxY / m_fAdminManagerAtlasMaxY / fScale;
		fRetX = (fY + fYDif) * m_fAdminManagerAtlasMaxX / m_fAdminManagerAtlasImageSizeX / fScale;
	}
	else
	{
		fRetX = fX * m_fAdminManagerAtlasMaxX / m_fAdminManagerAtlasImageSizeX / fScale;
		fRetY = fY * m_fAdminManagerAtlasMaxY / m_fAdminManagerAtlasImageSizeY / fScale;
	}
}

void CPythonMiniMap::UpdateAdminManagerAtlas()
{
	// player mark
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_OBSERVING_PLAYER))
	{
		// EMPTY
	}
}

void CPythonMiniMap::RenderAdminManagerAtlas(float fScreenX, float fScreenY, float fMaxWidth, float fMaxHeight)
{
	if (!m_bShowAdminManagerAtlas)
	{
		return;
	}

	if (!m_bAdminManagerAtlas)
	{
		TraceError("Cannot render AdminManager atlas");
		return;
	}

	float* pfSizeX, * pfSizeY;
	if (IsAdminManagerAtlasRotated())
	{
		pfSizeX = &m_fAdminManagerAtlasImageSizeY;
		pfSizeY = &m_fAdminManagerAtlasImageSizeX;
	}
	else
	{
		pfSizeX = &m_fAdminManagerAtlasImageSizeX;
		pfSizeY = &m_fAdminManagerAtlasImageSizeY;
	}

	float fScale = m_fAdminManagerAtlasImageSizeScale;
	if (*pfSizeX * fScale > fMaxWidth)
	{
		fScale = fMaxWidth / *pfSizeX;
	}
	if (*pfSizeY * fScale > fMaxHeight)
	{
		fScale = fMaxHeight / *pfSizeY;
	}
	if (fScale != m_fAdminManagerAtlasImageSizeScale)
	{
		m_fAdminManagerAtlasImageSizeScale = fScale;
		m_AdminManagerAtlasImageInstance.SetScale(fScale, fScale);
	}

	m_AdminManagerAtlasImageInstance.SetScale(m_fAdminManagerAtlasImageSizeScale, m_fAdminManagerAtlasImageSizeScale);

	if (m_fAdminManagerAtlasScreenX != fScreenX || m_fAdminManagerAtlasScreenY != fScreenY)
	{
		if (IsAdminManagerAtlasRotated())
		{
			m_matWorldAdminManagerAtlas._41 = fScreenX + m_fAdminManagerAtlasImageSizeY / 2 - m_fAdminManagerAtlasImageSizeX / 2;
			m_matWorldAdminManagerAtlas._42 = fScreenY + m_fAdminManagerAtlasImageSizeX / 2 - m_fAdminManagerAtlasImageSizeY / 2;
		}
		else
		{
			m_matWorldAdminManagerAtlas._41 = fScreenX;
			m_matWorldAdminManagerAtlas._42 = fScreenY;
		}
		m_fAdminManagerAtlasScreenX = fScreenX;
		m_fAdminManagerAtlasScreenY = fScreenY;
	}

	// main map
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matWorldAdminManagerAtlas);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	STATEMANAGER.SaveSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	if (IsAdminManagerAtlasRotated())
	{
		m_AdminManagerAtlasImageInstance.SetRotation(90);
	}
	m_AdminManagerAtlasImageInstance.Render();

	STATEMANAGER.SaveRenderState(D3DRS_TEXTUREFACTOR, 0xFFFFFFFF);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	STATEMANAGER.SaveTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// mobs if shown
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_MOB | ADMIN_MANAGER_SHOW_STONE | ADMIN_MANAGER_SHOW_NPC))
	{
		const auto& r_monsterMap = CPythonAdmin::Instance().MapViewer_GetMonster();
		for (auto it = r_monsterMap.begin(); it != r_monsterMap.end(); ++it)
		{
			const auto& rMonsterInfo = it->second;
			const CPythonNonPlayer::TMobTable* pProto = CPythonNonPlayer::instance().GetTable(rMonsterInfo.dwRaceNum);
			UINT eNameColor = CInstanceBase::NAMECOLOR_DEAD;

			if (rMonsterInfo.is_alive && pProto)
			{
				switch (pProto->bType)
				{
				case CPythonNonPlayer::MONSTER:
					if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_MOB))
					{
						continue;
					}
					eNameColor = CInstanceBase::NAMECOLOR_MOB;
					break;

				case CPythonNonPlayer::STONE:
					if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_STONE))
					{
						continue;
					}
					eNameColor = CInstanceBase::NAMECOLOR_METIN;
					break;

				default:
					if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_NPC))
					{
						continue;
					}
					eNameColor = CInstanceBase::NAMECOLOR_NPC;
					break;
				}
			}

			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(eNameColor));

			// draw
			float fX, fY;
			ConvertGlobalPositionToAdminManagerAtlasData(rMonsterInfo.x, rMonsterInfo.y, fX, fY);

			m_WhiteMark.SetPosition(fX - (float)m_WhiteMark.GetWidth() / 2.0f, fY - (float)m_WhiteMark.GetHeight() / 2.0f);
			m_WhiteMark.Render();
		}
	}

	// other pcs if shown
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_PC))
	{
		const auto& r_playerMap = CPythonAdmin::Instance().MapViewer_GetPlayer();
		for (auto it = r_playerMap.begin(); it != r_playerMap.end(); ++it)
		{
			const auto& rPlayerInfo = it->second;
			STATEMANAGER.SetRenderState(D3DRS_TEXTUREFACTOR, CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_EMPIRE_PC + rPlayerInfo.bEmpire));

			float fX, fY;
			ConvertGlobalPositionToAdminManagerAtlasData(rPlayerInfo.x, rPlayerInfo.y, fX, fY);

			m_WhiteMark.SetPosition(fX - (float)m_WhiteMark.GetWidth() / 2.0f,
				fY - (float)m_WhiteMark.GetHeight() / 2.0f);
			m_WhiteMark.Render();
		}
	}

	STATEMANAGER.RestoreRenderState(D3DRS_TEXTUREFACTOR);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG1);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLORARG2);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_COLOROP);

	// observing player
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_OBSERVING_PLAYER) && (ELTimer_GetMSec() / 500) % 2)
	{
		m_AdminManagerAtlasPlayerMark.Render();
	}

	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MINFILTER);
	STATEMANAGER.RestoreTextureStageState(0, D3DTSS_MAGFILTER);
	STATEMANAGER.SetTransform(D3DTS_WORLD, &m_matIdentity);
}

bool CPythonMiniMap::IsAdminManagerAtlasRotated() const
{
	return false;
}

bool CPythonMiniMap::GetAdminManagerAtlasInfo(float fScreenX, float fScreenY, std::string& rReturnString, float* pReturnPosX, float* pReturnPosY, DWORD* pdwTextColor, DWORD* pdwStoneDropVnum)
{
	// rotation dif
	float fBaseXDif = m_matWorldAdminManagerAtlas._41 - m_fAdminManagerAtlasScreenX;
	float fBaseYDif = m_matWorldAdminManagerAtlas._42 - m_fAdminManagerAtlasScreenY;
	// draw XY
	float fRealX = fScreenX - m_fAdminManagerAtlasScreenX - fBaseXDif;
	float fRealY = fScreenY - m_fAdminManagerAtlasScreenY - fBaseYDif;

	float fCheckWidth, fCheckHeight;
	float fMapX, fMapY;

	// check player mark
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_OBSERVING_PLAYER))
	{
		// EMPTY
	}

	// check other player if enabled
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_PC))
	{
		fCheckWidth = m_AdminManagerAtlasOtherPlayerMark.GetWidth();
		fCheckHeight = m_AdminManagerAtlasOtherPlayerMark.GetHeight();

		const auto& r_playerMap = CPythonAdmin::Instance().MapViewer_GetPlayer();
		for (auto it = r_playerMap.begin(); it != r_playerMap.end(); ++it)
		{
			const auto& rPlayerInfo = it->second;
			ConvertGlobalPositionToAdminManagerAtlasData(rPlayerInfo.x, rPlayerInfo.y, fMapX, fMapY);

			if (fMapX - fCheckWidth / 2 <= fRealX && fMapX + fCheckWidth / 2 >= fRealX &&
				fMapY - fCheckHeight / 2 <= fRealY && fMapY + fCheckHeight / 2 >= fRealY)
			{
				rReturnString = rPlayerInfo.szName;
				*pReturnPosX = rPlayerInfo.x - CPythonAdmin::Instance().MapViewer_GetBaseX();
				*pReturnPosY = rPlayerInfo.y - CPythonAdmin::Instance().MapViewer_GetBaseY();
				*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_EMPIRE_PC + rPlayerInfo.bEmpire);
				*pdwStoneDropVnum = 0;
				return true;
			}
		}
	}

	// check mob if enabled
	if (IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_MOB | ADMIN_MANAGER_SHOW_STONE | ADMIN_MANAGER_SHOW_NPC))
	{
		fCheckWidth = m_WhiteMark.GetWidth();
		fCheckHeight = m_WhiteMark.GetHeight();

		const auto& r_monsterMap = CPythonAdmin::Instance().MapViewer_GetMonster();
		if (r_monsterMap.size() > 0)
		{
			auto it = r_monsterMap.end();
			do
			{
				const auto& rMonsterInfo = (--it)->second;
				const CPythonNonPlayer::TMobTable* pProto = CPythonNonPlayer::Instance().GetTable(rMonsterInfo.dwRaceNum);
				if (pProto)
				{
					switch (pProto->bType)
					{
					case CPythonNonPlayer::MONSTER:
						if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_MOB))
						{
							continue;
						}
						*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_MOB);
						*pdwStoneDropVnum = 0;
						break;

					case CPythonNonPlayer::STONE:
						if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_STONE))
						{
							continue;
						}
						*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_METIN);
						*pdwStoneDropVnum = rMonsterInfo.stone_drop_vnum;
						break;

					default:
						if (!IsAdminManagerFlagShown(ADMIN_MANAGER_SHOW_NPC))
						{
							continue;
						}
						*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_NPC);
						*pdwStoneDropVnum = 0;
						break;
					}
				}

				ConvertGlobalPositionToAdminManagerAtlasData(rMonsterInfo.x, rMonsterInfo.y, fMapX, fMapY);

				if (fMapX - fCheckWidth / 2 <= fRealX && fMapX + fCheckWidth / 2 >= fRealX &&
					fMapY - fCheckHeight / 2 <= fRealY && fMapY + fCheckHeight / 2 >= fRealY)
				{
					rReturnString = CPythonNonPlayer::Instance().GetMonsterName(rMonsterInfo.dwRaceNum);
					*pReturnPosX = rMonsterInfo.x - CPythonAdmin::Instance().MapViewer_GetBaseX();
					*pReturnPosY = rMonsterInfo.y - CPythonAdmin::Instance().MapViewer_GetBaseY();
					return true;
				}
			} while (it != r_monsterMap.begin());
		}
	}

	return false;
}

bool CPythonMiniMap::GetAdminManagerAtlasInfoNew(float fScreenX, float fScreenY, float* fReturnPosX, float* fReturnPosY)
{
	// rotation dif
	float fBaseXDif = m_matWorldAdminManagerAtlas._41 - m_fAdminManagerAtlasScreenX;
	float fBaseYDif = m_matWorldAdminManagerAtlas._42 - m_fAdminManagerAtlasScreenY;
	// draw XY
	*fReturnPosX = fScreenX - m_fAdminManagerAtlasScreenX - fBaseXDif;
	*fReturnPosY = fScreenY - m_fAdminManagerAtlasScreenY - fBaseYDif;
	ConvertAdminManagerAtlasDataToPosition(*fReturnPosX, *fReturnPosY, *fReturnPosX, *fReturnPosY);

	if (*fReturnPosX > 0 && *fReturnPosX <= m_fAdminManagerAtlasMaxX && *fReturnPosY > 0 && *fReturnPosY <= m_fAdminManagerAtlasMaxY)
	{
		return true;
	}

	return false;
}

void CPythonMiniMap::ShowAdminManagerAtlas()
{
	m_bShowAdminManagerAtlas = true;
}

void CPythonMiniMap::HideAdminManagerAtlas()
{
	m_bShowAdminManagerAtlas = false;
}

bool CPythonMiniMap::CanShowAdminManagerAtlas() const
{
	return m_bShowAdminManagerAtlas;
}

void CPythonMiniMap::ShowAdminManagerFlag(DWORD dwFlag)
{
	SET_BIT(m_dwShowAdminManagerFlag, dwFlag);
}

void CPythonMiniMap::HideAdminManagerFlag(DWORD dwFlag)
{
	REMOVE_BIT(m_dwShowAdminManagerFlag, dwFlag);
}

bool CPythonMiniMap::IsAdminManagerFlagShown(DWORD dwFlag) const
{
	return IS_SET(m_dwShowAdminManagerFlag, dwFlag);
}
#endif

bool CPythonMiniMap::GetPickedInstanceInfo(float fScreenX, float fScreenY, std::string& rReturnName, float* pReturnPosX, float* pReturnPosY, DWORD* pdwTextColor)
{
	float fDistanceFromMiniMapCenterX = fScreenX - m_fScreenX - m_fWidth * 0.5f;
	float fDistanceFromMiniMapCenterY = fScreenY - m_fScreenY - m_fHeight * 0.5f;

	if (sqrtf(fDistanceFromMiniMapCenterX * fDistanceFromMiniMapCenterX + fDistanceFromMiniMapCenterY * fDistanceFromMiniMapCenterY) > m_fMiniMapRadius)
	{
		return false;
	}

	float fRealX = m_fCenterX + fDistanceFromMiniMapCenterX / m_fScale * ((float)CTerrainImpl::CELLSCALE);
	float fRealY = m_fCenterY + fDistanceFromMiniMapCenterY / m_fScale * ((float)CTerrainImpl::CELLSCALE);

	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pkInst)
	{
		TPixelPosition kInstPos;
		pkInst->NEW_GetPixelPosition(&kInstPos);

		if (fabs(kInstPos.x - fRealX) < ((float)CTerrainImpl::CELLSCALE) * 6.0f / m_fScale &&
			fabs(kInstPos.y - fRealY) < ((float)CTerrainImpl::CELLSCALE) * 6.0f / m_fScale)
		{
			rReturnName = pkInst->GetNameString();
			*pReturnPosX = kInstPos.x;
			*pReturnPosY = kInstPos.y;
			*pdwTextColor = pkInst->GetNameColor();
			return true;
		}
	}

	if (m_fScale < 1.0f)
	{
		return false;
	}

	CPythonCharacterManager& rkChrMgr = CPythonCharacterManager::Instance();
	CPythonCharacterManager::CharacterIterator i;
	for (i = rkChrMgr.CharacterInstanceBegin(); i != rkChrMgr.CharacterInstanceEnd(); ++i)
	{
		CInstanceBase* pkInstEach = *i;
		if (pkInstEach->IsInvisibility())
		{
			continue;
		}
		if (m_fScale < 2.0f && (pkInstEach->IsEnemy() || (pkInstEach->IsPC()
#ifdef ENABLE_SHAMAN_SYSTEM
			&& !pkInstEach->IsShaman()
#endif
			)))
		{
			continue;
		}
		TPixelPosition kInstancePosition;
		pkInstEach->NEW_GetPixelPosition(&kInstancePosition);

		if (fabs(kInstancePosition.x - fRealX) < ((float)CTerrainImpl::CELLSCALE) * 3.0f / m_fScale &&
			fabs(kInstancePosition.y - fRealY) < ((float)CTerrainImpl::CELLSCALE) * 3.0f / m_fScale)
		{
			rReturnName = pkInstEach->GetNameString();
			*pReturnPosX = kInstancePosition.x;
			*pReturnPosY = kInstancePosition.y;
			*pdwTextColor = pkInstEach->GetNameColor();
			return true;
		}
	}
	return false;
}


#ifdef __ENABLE_BOSS_ON_ATLAS__
bool CPythonMiniMap::GetAtlasInfo(float fScreenX, float fScreenY, std::string& rReturnString, std::string& rReturnDurationString, float* pReturnPosX, float* pReturnPosY, DWORD* pdwTextColor, DWORD* pdwGuildID)
#else
bool CPythonMiniMap::GetAtlasInfo(float fScreenX, float fScreenY, std::string& rReturnString, float* pReturnPosX, float* pReturnPosY, DWORD* pdwTextColor, DWORD* pdwGuildID)
#endif
{
	float fRealX = (fScreenX - m_fAtlasScreenX) * (m_fAtlasMaxX / m_fAtlasImageSizeX);
	float fRealY = (fScreenY - m_fAtlasScreenY) * (m_fAtlasMaxY / m_fAtlasImageSizeY);

	//((float) CTerrainImpl::CELLSCALE) * 10.0f
	float fCheckWidth = (m_fAtlasMaxX / m_fAtlasImageSizeX) * 5.0f;
	float fCheckHeight = (m_fAtlasMaxY / m_fAtlasImageSizeY) * 5.0f;

	CInstanceBase* pkInst = CPythonCharacterManager::Instance().GetMainInstancePtr();

	if (pkInst)
	{
		TPixelPosition kInstPos;
		pkInst->NEW_GetPixelPosition(&kInstPos);

		if (kInstPos.x - fCheckWidth < fRealX && kInstPos.x + fCheckWidth > fRealX &&
			kInstPos.y - fCheckHeight < fRealY && kInstPos.y + fCheckHeight > fRealY)
		{
			rReturnString = pkInst->GetNameString();
			*pReturnPosX = kInstPos.x;
			*pReturnPosY = kInstPos.y;
			*pdwTextColor = pkInst->GetNameColor();
			return true;
		}
	}

#ifdef ENABLE_PARTY_ATLAS
	auto it = m_kMap_dwPID_kPartyMember.begin();
	while (it != m_kMap_dwPID_kPartyMember.end())
	{
		TPartyMember& rkMember = it->second;

		if (rkMember.fX - fCheckWidth / 2 <= fRealX && rkMember.fX + fCheckWidth / 2 >= fRealX &&
			rkMember.fY - fCheckHeight / 2 <= fRealY && rkMember.fY + fCheckHeight / 2 >= fRealY)
		{
			rReturnString = rkMember.stName;
			*pReturnPosX = rkMember.fX;
			*pReturnPosY = rkMember.fY;
			*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_PARTY);
			return true;
		}

		++it;
	}
#endif

	m_AtlasMarkInfoVectorIterator = m_AtlasNPCInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasNPCInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;

		if (rAtlasMarkInfo.m_fX - fCheckWidth / 2 < fRealX && rAtlasMarkInfo.m_fX + fCheckWidth > fRealX &&
			rAtlasMarkInfo.m_fY - fCheckWidth / 2 < fRealY && rAtlasMarkInfo.m_fY + fCheckHeight > fRealY)
		{
			rReturnString = rAtlasMarkInfo.m_strText;
			*pReturnPosX = rAtlasMarkInfo.m_fX;
			*pReturnPosY = rAtlasMarkInfo.m_fY;
			*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_NPC);//m_MarkTypeToColorMap[rAtlasMarkInfo.m_byType];
			return true;
		}
		++m_AtlasMarkInfoVectorIterator;
	}

	m_AtlasMarkInfoVectorIterator = m_AtlasWarpInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasWarpInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		if (rAtlasMarkInfo.m_fX - fCheckWidth / 2 < fRealX && rAtlasMarkInfo.m_fX + fCheckWidth > fRealX &&
			rAtlasMarkInfo.m_fY - fCheckWidth / 2 < fRealY && rAtlasMarkInfo.m_fY + fCheckHeight > fRealY)
		{
			rReturnString = rAtlasMarkInfo.m_strText;
			*pReturnPosX = rAtlasMarkInfo.m_fX;
			*pReturnPosY = rAtlasMarkInfo.m_fY;
			*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WARP);//m_MarkTypeToColorMap[rAtlasMarkInfo.m_byType];
			return true;
		}
		++m_AtlasMarkInfoVectorIterator;
	}

	m_AtlasMarkInfoVectorIterator = m_AtlasWayPointInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasWayPointInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		if (rAtlasMarkInfo.m_fScreenX > 0.0f)
			if (rAtlasMarkInfo.m_fScreenY > 0.0f)
				if (rAtlasMarkInfo.m_fX - fCheckWidth / 2 < fRealX && rAtlasMarkInfo.m_fX + fCheckWidth > fRealX &&
					rAtlasMarkInfo.m_fY - fCheckWidth / 2 < fRealY && rAtlasMarkInfo.m_fY + fCheckHeight > fRealY)
				{
					rReturnString = rAtlasMarkInfo.m_strText;
					*pReturnPosX = rAtlasMarkInfo.m_fX;
					*pReturnPosY = rAtlasMarkInfo.m_fY;
					*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WAYPOINT);//m_MarkTypeToColorMap[rAtlasMarkInfo.m_byType];
					return true;
				}
		++m_AtlasMarkInfoVectorIterator;
	}

	TGuildAreaInfoVector::iterator itor = m_GuildAreaInfoVector.begin();
	for (; itor != m_GuildAreaInfoVector.end(); ++itor)
	{
		TGuildAreaInfo& rInfo = *itor;
		if (fScreenX - m_fAtlasScreenX >= rInfo.fsxRender)
			if (fScreenY - m_fAtlasScreenY >= rInfo.fsyRender)
				if (fScreenX - m_fAtlasScreenX <= rInfo.fexRender)
					if (fScreenY - m_fAtlasScreenY <= rInfo.feyRender)
					{
						if (CPythonGuild::Instance().GetGuildName(rInfo.dwGuildID, &rReturnString))
						{
							*pdwGuildID = rInfo.dwGuildID;
						}
						else
						{
							rReturnString = "empty_guild_area";
						}

						*pReturnPosX = rInfo.lx + rInfo.lwidth / 2;
						*pReturnPosY = rInfo.ly + rInfo.lheight / 2;
						*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_PARTY);
						return true;
					}
	}

#ifdef __ENABLE_BOSS_ON_ATLAS__
	float n_fCheckWidth = (m_fAtlasMaxX / lBossImageWidth);
	float n_fCheckHeight = (m_fAtlasMaxY / lBossImageHeight);

	m_AtlasMarkInfoVectorIterator = m_AtlasBossInfoVector.begin();

	for (const auto& rAtlasMarkInfo : m_AtlasBossInfoVector)
	{
		if (rAtlasMarkInfo.m_fScreenX > 0.0f)
		{
			if (rAtlasMarkInfo.m_fScreenY > 0.0f)
			{
				if (rAtlasMarkInfo.m_fX - n_fCheckWidth / 2 <= fRealX && rAtlasMarkInfo.m_fX + n_fCheckWidth / 2 >= fRealX &&
					rAtlasMarkInfo.m_fY - n_fCheckHeight / 2 <= fRealY && rAtlasMarkInfo.m_fY + n_fCheckHeight / 2 >= fRealY)
				{
					rReturnString = rAtlasMarkInfo.m_strText;
					rReturnDurationString = rAtlasMarkInfo.m_strDuration;
					*pReturnPosX = rAtlasMarkInfo.m_fX;
					*pReturnPosY = rAtlasMarkInfo.m_fY;
					*pdwTextColor = CInstanceBase::GetIndexedNameColor(CInstanceBase::NAMECOLOR_WAYPOINT);//m_MarkTypeToColorMap[rAtlasMarkInfo.m_byType];
					return true;
				}
			}
		}
	}
#endif

	return false;
}

bool CPythonMiniMap::GetAtlasSize(float* pfSizeX, float* pfSizeY)
{
	CPythonBackground& rkBG = CPythonBackground::Instance();
	if (!rkBG.IsMapOutdoor())
	{
		return false;
	}

	*pfSizeX = m_fAtlasImageSizeX;
	*pfSizeY = m_fAtlasImageSizeY;

	return true;
}

// Atlas
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// WayPoint
void CPythonMiniMap::AddWayPoint(BYTE byType, DWORD dwID, float fX, float fY, std::string strText, DWORD dwChrVID)
{
	m_AtlasMarkInfoVectorIterator = m_AtlasWayPointInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasWayPointInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		if (rAtlasMarkInfo.m_dwID == dwID)
		{
			return;
		}
		++m_AtlasMarkInfoVectorIterator;
	}

	TAtlasMarkInfo aAtlasMarkInfo;
	aAtlasMarkInfo.m_byType = byType;
	aAtlasMarkInfo.m_dwID = dwID;
	aAtlasMarkInfo.m_fX = fX;
	aAtlasMarkInfo.m_fY = fY;
	aAtlasMarkInfo.m_fScreenX = 0.0f;
	aAtlasMarkInfo.m_fScreenY = 0.0f;
	aAtlasMarkInfo.m_fMiniMapX = 0.0f;
	aAtlasMarkInfo.m_fMiniMapY = 0.0f;
	aAtlasMarkInfo.m_strText = strText;
	aAtlasMarkInfo.m_dwChrVID = dwChrVID;
	__UpdateWayPoint(&aAtlasMarkInfo, fX, fY);
	m_AtlasWayPointInfoVector.push_back(aAtlasMarkInfo);

}

void CPythonMiniMap::RemoveWayPoint(DWORD dwID)
{
	m_AtlasMarkInfoVectorIterator = m_AtlasWayPointInfoVector.begin();
	while (m_AtlasMarkInfoVectorIterator != m_AtlasWayPointInfoVector.end())
	{
		TAtlasMarkInfo& rAtlasMarkInfo = *m_AtlasMarkInfoVectorIterator;
		if (rAtlasMarkInfo.m_dwID == dwID)
		{
			m_AtlasMarkInfoVectorIterator = m_AtlasWayPointInfoVector.erase(m_AtlasMarkInfoVectorIterator);
			return;
		}
		++m_AtlasMarkInfoVectorIterator;
	}
}

bool CPythonMiniMap::__GetWayPoint(DWORD dwID, TAtlasMarkInfo** ppkInfo)
{
	TAtlasMarkInfoVectorIterator itor = m_AtlasWayPointInfoVector.begin();
	for (; itor != m_AtlasWayPointInfoVector.end(); ++itor)
	{
		TAtlasMarkInfo& rInfo = *itor;
		if (dwID == rInfo.m_dwID)
		{
			*ppkInfo = &rInfo;
			return true;
		}
	}

	return false;
}

void CPythonMiniMap::__UpdateWayPoint(TAtlasMarkInfo* pkInfo, int ix, int iy)
{
	pkInfo->m_fX = float(ix);
	pkInfo->m_fY = float(iy);
	pkInfo->m_fScreenX = pkInfo->m_fX / m_fAtlasMaxX * m_fAtlasImageSizeX;
	pkInfo->m_fScreenY = pkInfo->m_fY / m_fAtlasMaxY * m_fAtlasImageSizeY;
}

// WayPoint
//////////////////////////////////////////////////////////////////////////

void CPythonMiniMap::__RenderWayPointMark(int ixCenter, int iyCenter)
{
	int iNum = (ELTimer_GetMSec() / 67) % WAYPOINT_IMAGE_COUNT;

	CGraphicImageInstance& rInstance = m_WayPointGraphicImageInstances[iNum];
	rInstance.SetPosition(ixCenter - rInstance.GetWidth() / 2, iyCenter - rInstance.GetHeight() / 2);
	rInstance.Render();
}

void CPythonMiniMap::__RenderMiniWayPointMark(int ixCenter, int iyCenter)
{
	int iNum = (ELTimer_GetMSec() / 67) % MINI_WAYPOINT_IMAGE_COUNT;

	CGraphicImageInstance& rInstance = m_MiniWayPointGraphicImageInstances[iNum];
	rInstance.SetPosition(ixCenter - rInstance.GetWidth() / 2, iyCenter - rInstance.GetHeight() / 2);
	rInstance.Render();
}

void CPythonMiniMap::__RenderTargetMark(int ixCenter, int iyCenter)
{
	int iNum = (ELTimer_GetMSec() / 80) % TARGET_MARK_IMAGE_COUNT;

	CGraphicImageInstance& rInstance = m_TargetMarkGraphicImageInstances[iNum];
	rInstance.SetPosition(ixCenter - rInstance.GetWidth() / 2, iyCenter - rInstance.GetHeight() / 2);
	rInstance.Render();
}

void CPythonMiniMap::AddSignalPoint(float fX, float fY)
{
	static unsigned int g_id = 255;

	TSignalPoint sp;
	sp.id = g_id;
	sp.v2Pos.x = fX;
	sp.v2Pos.y = fY;

	m_SignalPointVector.push_back(sp);

	AddWayPoint(TYPE_WAYPOINT, g_id, fX, fY, "");

	g_id++;
}

void CPythonMiniMap::ClearAllSignalPoint()
{
	std::vector<TSignalPoint>::iterator it;
	for (it = m_SignalPointVector.begin(); it != m_SignalPointVector.end(); ++it)
	{
		RemoveWayPoint(it->id);
	}
	m_SignalPointVector.clear();
}

#ifdef __ENABLE_BOSS_ON_ATLAS__
void CPythonMiniMap::ClearBossList()
{
	m_AtlasBossInfoVector.clear();
}

void CPythonMiniMap::AddBossList(BYTE byType, DWORD dwID, float fX, float fY, std::string strText, DWORD dwChrVID)
{
	if (std::find_if(m_AtlasBossInfoVector.begin(), m_AtlasBossInfoVector.end(), [&dwID](const TAtlasMarkInfo& rMark)
		{
			return (rMark.m_dwID == dwID);
		}) != m_AtlasBossInfoVector.end())
		return;

		TAtlasMarkInfo aAtlasMarkInfo;
		aAtlasMarkInfo.m_byType = byType;
		aAtlasMarkInfo.m_dwID = dwID;
		aAtlasMarkInfo.m_fX = fX;
		aAtlasMarkInfo.m_fY = fY;
		aAtlasMarkInfo.m_fScreenX = 0.0f;
		aAtlasMarkInfo.m_fScreenY = 0.0f;
		aAtlasMarkInfo.m_fMiniMapX = 0.0f;
		aAtlasMarkInfo.m_fMiniMapY = 0.0f;
		aAtlasMarkInfo.m_strText = strText;
		aAtlasMarkInfo.m_dwChrVID = dwChrVID;
		__UpdateWayPoint(&aAtlasMarkInfo, fX, fY);
		m_AtlasBossInfoVector.push_back(aAtlasMarkInfo);

}

void CPythonMiniMap::RemoveBossAtlas(DWORD dwID)
{
	std::remove_if(m_AtlasBossInfoVector.begin(), m_AtlasBossInfoVector.end(), [&dwID](const TAtlasMarkInfo& rMark)
		{
			return (rMark.m_dwID == dwID);
		});
}
#endif

void CPythonMiniMap::RegisterAtlasWindow(PyObject* poHandler)
{
	m_poHandler = poHandler;
}

void CPythonMiniMap::UnregisterAtlasWindow()
{
	m_poHandler = 0;
}

void CPythonMiniMap::OpenAtlasWindow()
{
	if (m_poHandler)
	{
		PyCallClassMemberFunc(m_poHandler, "Show", Py_BuildValue("()"));
	}
}

void CPythonMiniMap::SetAtlasCenterPosition(int x, int y)
{
	if (m_poHandler)
	{
		//int sw = UI::CWindowManager::Instance().GetScreenWidth();
		//int sh = UI::CWindowManager::Instance().GetScreenHeight();
		//PyCallClassMemberFunc(m_poHandler,"SetPosition", Py_BuildValue("(ii)",sw/2+x,sh/2+y));
		PyCallClassMemberFunc(m_poHandler, "SetCenterPositionAdjust", Py_BuildValue("(ii)", x, y));
	}
}

bool CPythonMiniMap::IsAtlas()
{
	return m_bAtlas;
}

void CPythonMiniMap::ShowAtlas()
{
	m_bShowAtlas = true;
}

void CPythonMiniMap::HideAtlas()
{
	m_bShowAtlas = false;
}

bool CPythonMiniMap::CanShowAtlas()
{
	return m_bShowAtlas;
}

bool CPythonMiniMap::CanShow()
{
	return m_bShow;
}

void CPythonMiniMap::Show()
{
	m_bShow = true;
}

void CPythonMiniMap::Hide()
{
	m_bShow = false;
}

void CPythonMiniMap::__Initialize()
{
	m_poHandler = 0;

	SetMiniMapSize(128.0f, 128.0f);

	m_fScale = 2.0f;

	m_fCenterX = m_fWidth * 0.5f;
	m_fCenterY = m_fHeight * 0.5f;

	m_fScreenX = 0.0f;
	m_fScreenY = 0.0f;

	m_fAtlasScreenX = 0.0f;
	m_fAtlasScreenY = 0.0f;

	m_dwAtlasBaseX = 0;
	m_dwAtlasBaseY = 0;

	m_fAtlasMaxX = 0.0f;
	m_fAtlasMaxY = 0.0f;

	m_fAtlasImageSizeX = 0.0f;
	m_fAtlasImageSizeY = 0.0f;

	m_bAtlas = false;

	m_bShow = false;
	m_bShowAtlas = false;

	D3DXMatrixIdentity(&m_matIdentity);
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matMiniMapCover);
	D3DXMatrixIdentity(&m_matWorldAtlas);
#ifdef ENABLE_ADMIN_MANAGER
	m_fAdminManagerAtlasScreenX = 0.0f;
	m_fAdminManagerAtlasScreenY = 0.0f;
	m_dwAdminManagerAtlasBaseX = 0;
	m_dwAdminManagerAtlasBaseY = 0;
	m_fAdminManagerAtlasMaxX = 0.0f;
	m_fAdminManagerAtlasMaxY = 0.0f;
	m_fAdminManagerAtlasImageSizeX = 0.0f;
	m_fAdminManagerAtlasImageSizeY = 0.0f;
	m_fAdminManagerAtlasImageSizeScale = 1.0f;
	m_bAdminManagerAtlas = false;
	m_bShowAdminManagerAtlas = false;
	m_dwShowAdminManagerFlag = ADMIN_MANAGER_SHOW_PC | ADMIN_MANAGER_SHOW_MOB | ADMIN_MANAGER_SHOW_STONE | ADMIN_MANAGER_SHOW_NPC;
	D3DXMatrixIdentity(&m_matWorldAdminManagerAtlas);
#endif
#ifdef ENABLE_PARTY_ATLAS
	m_kMap_dwPID_kPartyMember.clear();
#endif
}

void CPythonMiniMap::Destroy()
{
	ClearAllSignalPoint();
	m_poHandler = 0;

	m_VertexBuffer.Destroy();
	m_IndexBuffer.Destroy();

	m_PlayerMark.Destroy();

	m_MiniMapFilterGraphicImageInstance.Destroy();
	m_MiniMapCameraraphicImageInstance.Destroy();

	m_AtlasWayPointInfoVector.clear();
	m_AtlasImageInstance.Destroy();
	m_AtlasPlayerMark.Destroy();
#ifdef ENABLE_ADMIN_MANAGER
	m_AdminManagerAtlasImageInstance.Destroy();
	m_AdminManagerAtlasPlayerMark.Destroy();
	m_AdminManagerAtlasOtherPlayerMark.Destroy();
#endif
	m_WhiteMark.Destroy();

	for (int i = 0; i < MINI_WAYPOINT_IMAGE_COUNT; ++i)
	{
		m_MiniWayPointGraphicImageInstances[i].Destroy();
	}
	for (int j = 0; j < WAYPOINT_IMAGE_COUNT; ++j)
	{
		m_WayPointGraphicImageInstances[j].Destroy();
	}
	for (int k = 0; k < TARGET_MARK_IMAGE_COUNT; ++k)
	{
		m_TargetMarkGraphicImageInstances[k].Destroy();
	}

#ifdef __ENABLE_BOSS_ON_ATLAS__
	m_AtlasBossInfoVector.clear();
	m_BossImageMark.Destroy();
#endif

	m_GuildAreaFlagImageInstance.Destroy();

	__Initialize();
}

CPythonMiniMap::CPythonMiniMap()
{
	__Initialize();
}

CPythonMiniMap::~CPythonMiniMap()
{
	Destroy();
}
