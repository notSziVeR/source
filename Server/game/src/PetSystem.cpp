#include "stdafx.h"
#include "utils.h"
#include "vector.h"
#include "char.h"
#include "sectree_manager.h"
#include "char_manager.h"
#include "mob_manager.h"
#include "PetSystem.h"
#include "../../common/VnumHelper.h"
#include "packet.h"
#include "item_manager.h"
#include "item.h"
#include <Eigen/Core>

#include "ItemUtils.h"

extern int passes_per_sec;
EVENTINFO(petsystem_event_info)
{
	CPetSystem* pPetSystem;
};

// An event that updates PetSystem.
// PetSystem differs from existing chracters that update from CHRACTER_MANAGER to existing FSM,
// Update the owner's STATE with _UpdateFollowAI function.
// However, since CHRACTER_MANAGER will update the owner's state,
// Update the petsystem and there was a problem in unsummon the pet.
// (Update in CHRACTER_MANAGER will pend chracter destroy, CPetSystem will have dangling pointer.)
// So only PetSystem update event is generated.
EVENTFUNC(petsystem_update_event)
{
	auto info = static_cast<petsystem_event_info*>(event->info);
	if (!info)
	{
		sys_err("petsystem_update_event> <Factor> Null pointer");
		return 0;
	}

	auto pPetSystem = info->pPetSystem;
	if (!pPetSystem)
	{
		return 0;
	}

	if (!pPetSystem->Update())
	{
		pPetSystem->m_pkPetSystemUpdateEvent = nullptr;
		return 0;
	}

	return 3;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CPetActor
///////////////////////////////////////////////////////////////////////////////////////

CPetActor::CPetActor(CHARACTER& owner, uint32_t vnum, uint32_t options)
	: m_owner(owner)
	, m_dwVnum(vnum)
	, m_dwOptions(options)
	, m_dwLastActionTime(0)
	, m_pkChar(nullptr)
{
	// ctor
}

CPetActor::~CPetActor()
{
	Unsummon();
}

bool CPetActor::Mount()
{
	if (HasOption(EPetOption_Mountable))
	{
		m_owner.MountVnum(m_dwVnum);
	}

	return m_owner.GetMountVnum() == m_dwVnum;
}

void CPetActor::Unmount()
{
	if (m_owner.GetMountVnum() == m_dwVnum)
	{
		m_owner.MountVnum(0);
	}
}

uint32_t CPetActor::Summon(std::string petName, bool bSpawnFar)
{
	int32_t x = m_owner.GetX();
	int32_t y = m_owner.GetY();
	int32_t z = m_owner.GetZ();

	if (!m_owner.GetDungeon() && m_owner.GetMapIndex() < 10000)
	{
		if (bSpawnFar)
		{
			x += (number(0, 1) * 2 - 1) * number(2000, 2500);
			y += (number(0, 1) * 2 - 1) * number(2000, 2500);
		}
		else
		{
			x += number(-100, 100);
			y += number(-100, 100);
		}
	}

	if (m_pkChar)
	{
		m_pkChar->Show(m_owner.GetMapIndex(), x, y);
		return m_pkChar->GetVID();
	}

	m_pkChar = CHARACTER_MANAGER::instance().SpawnMob(m_dwVnum,
			   m_owner.GetMapIndex(),
			   x, y, z,
			   false,
			   (int)(m_owner.GetRotation() + 180),
			   false);

	if (!m_pkChar)
	{
		sys_err("[CPetSystem::Summon] Failed to summon the pet. (vnum: %u)", m_dwVnum);
		return 0;
	}

	m_pkChar->SetPet(this);

	//m_owner.DetailLog();
	//m_pkChar->DetailLog();

	m_pkChar->SetEmpire(m_owner.GetEmpire());
	m_pkChar->SetName(petName);
	m_pkChar->Show(m_owner.GetMapIndex(), x, y, z);
	return m_pkChar->GetVID();
}

void CPetActor::Unsummon()
{
	if (!m_pkChar)
	{
		return;
	}

	m_pkChar->SetPet(nullptr);
	M2_DESTROY_CHARACTER(m_pkChar);
	m_pkChar = nullptr;
}

bool CPetActor::Update()
{
	if (IsSummoned())
	{
		return UpdateFollowAI();
	}

	return true;
}

bool CPetActor::UpdatAloneActionAI(float fMinDist, float fMaxDist)
{
	float fDist = fnumber(fMinDist, fMaxDist);
	float r = static_cast<float>(fnumber(0.0f, 359.0f));
	float dest_x = m_owner.GetX() + fDist * std::cos(r);
	float dest_y = m_owner.GetY() + fDist * std::sin(r);

	//m_pkChar->SetRotation(GetRandomInt(0, 359));

	//GetDeltaByDegree(m_pkChar->GetRotation(), fDist, &fx, &fy);

	// Check for loose property; Do not go if the final position and the intermediate position can not be reached.
	//if (!(SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx, m_pkChar->GetY() + (int) fy)
	//			&& SECTREE_MANAGER::instance().IsMovablePosition(m_pkChar->GetMapIndex(), m_pkChar->GetX() + (int) fx/2, m_pkChar->GetY() + (int) fy/2)))
	//	return true;

	m_pkChar->SetNowWalking(true);

	if (!m_pkChar->IsStateMove())
	{
		m_pkChar->Goto(dest_x, dest_y);
	}

	m_dwLastActionTime = get_dword_time();
	return true;
}

bool CPetActor::UpdateFollowAI()
{
	int START_FOLLOW_DISTANCE = 400;
	int START_RUN_DISTANCE = 750;
	int RESPAWN_DISTANCE = 4500;
	int APPROACH = 250;

	bool bRun = false;

	uint32_t currentTime = get_dword_time();

	int32_t ownerX = m_owner.GetX();
	int32_t ownerY = m_owner.GetY();

	int32_t charX = m_pkChar->GetX();
	int32_t charY = m_pkChar->GetY();

	const auto dist = DISTANCE_APPROX(charX - ownerX, charY - ownerY);

	if (dist >= RESPAWN_DISTANCE)
	{
		float fx, fy;
		GetDeltaByDegree(m_owner.GetRotation(), -APPROACH, &fx, &fy);

		if (m_pkChar->Show(m_owner.GetMapIndex(), ownerX + fx, ownerY + fy))
		{
			return true;
		}
	}

	if (dist >= START_FOLLOW_DISTANCE && !m_pkChar->IsStateMove())
	{
		if (dist >= START_RUN_DISTANCE)
		{
			bRun = true;
		}

		m_pkChar->SetNowWalking(!bRun);

		Follow(APPROACH);

		m_pkChar->SetLastAttacked(currentTime);
		m_dwLastActionTime = currentTime;
	}

	return true;
}

bool CPetActor::Follow(int32_t minDistance) const
{
	if (!m_pkChar)
	{
		return false;
	}

	int32_t ownerX = m_owner.GetX();
	int32_t ownerY = m_owner.GetY();

	int32_t petX = m_pkChar->GetX();
	int32_t petY = m_pkChar->GetY();

	auto dist = DISTANCE_APPROX(ownerX - petX, ownerY - petY);
	if (dist <= minDistance)
	{
		return false;
	}

	Eigen::Vector2f owner(ownerX + number(-100, 100), ownerY + number(-100, 100));
	Eigen::Vector2f pet(petX, petY);

	Eigen::Vector2f pos = pet + (owner - pet).normalized() * (dist - minDistance);
	if (!m_pkChar->Goto(pos.x() + 0.5f, pos.y() + 0.5f))
	{
		return false;
	}

	m_pkChar->SendMovePacket(FUNC_WAIT, 0, 0, 0, 0);
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////
//  CPetSystem
///////////////////////////////////////////////////////////////////////////////////////

CPetSystem::CPetSystem(CHARACTER& owner)
	: m_owner(owner)
	, m_dwUpdatePeriod(400)
	, m_dwLastUpdateTime(0)
{
	// ctor
}

CPetSystem::~CPetSystem()
{
	event_cancel(&m_pkPetSystemUpdateEvent);
}

bool CPetSystem::Update()
{
	uint32_t now = get_dword_time();


	if (m_dwUpdatePeriod + m_dwLastUpdateTime > now)
	{
		return true;
	}

	for (auto it = m_itemPets.begin(), end = m_itemPets.end(); it != end; )
	{
		const auto& petActor = it->second;
		if (!petActor->IsSummoned() || !petActor->Update())
		{
			it = m_itemPets.erase(it);
		}
		else
		{
			++it;
		}
	}

	m_dwLastUpdateTime = now;
	return !m_itemPets.empty();
}

CPetActor* CPetSystem::SummonItem(uint32_t mobVnum, uint32_t itemId,
								  bool bSpawnFar,
								  uint32_t options)
{
	const auto it = m_itemPets.find(itemId);
	if (it != m_itemPets.end())
	{
		sys_err("Item %u already has a pet", itemId);
		return nullptr;
	}

	auto petActor = new CPetActor(m_owner, mobVnum, options);

	std::string name = m_owner.GetName();

	const CMob* pMobInfo = CMobManager::Instance().Get(mobVnum);
	if (pMobInfo)
	{
		name += LC_TEXT("'s Pet");
	}
	else
	{
		name += "'s ";
		name += LC_TEXT("'s Pet");
	}

	petActor->Summon(std::move(name), bSpawnFar);

	if (!m_pkPetSystemUpdateEvent)
	{
		auto info = AllocEventInfo<petsystem_event_info>();
		info->pPetSystem = this;
		m_pkPetSystemUpdateEvent = event_create(petsystem_update_event, info, 3);
	}

	m_itemPets.emplace(itemId, std::unique_ptr<CPetActor>(petActor));

	m_owner.ComputePoints();
	return petActor;
}

void CPetSystem::UnsummonItem(uint32_t itemId)
{
	const auto it = m_itemPets.find(itemId);
	if (it == m_itemPets.end())
	{
		sys_err("Item %u not found", itemId);
		return;
	}

	m_itemPets.erase(it);

	if (m_itemPets.empty())
	{
		event_cancel(&m_pkPetSystemUpdateEvent);
	}

	m_owner.ComputePoints();
}

void CPetSystem::RefreshOverlay(bool bTakeOut)
{
	auto pkItem = FindToggleItem(&m_owner, true, TOGGLE_PET);

	// We gonna refresh anything just in case if we have active pet!
	if (!pkItem) return;

	OnUseToggleItem(&m_owner, pkItem);
	OnUseToggleItem(&m_owner, pkItem);
}

void CPetSystem::RefreshBuffs()
{
	auto pItem = FindToggleItem(&m_owner, true, TOGGLE_PET);
	if (!pItem) return;

	LPITEM pOverlayItem = m_owner.GetWear(WEAR_COSTUME_PET);

	// In case wrong item is placed
	if (pOverlayItem && (pOverlayItem->GetType() != ITEM_COSTUME && pOverlayItem->GetSubType() != COSTUME_PET))
		return;

	// Applies
	for (const auto& pItem : { pItem, pOverlayItem })
	{
		if (!pItem)
		{
			continue;
		}

		auto pItemTable = pItem->GetProto();
		for (int i = 0; i < ITEM_APPLY_MAX_NUM; ++i)
		{
			if (pItemTable->aApplies[i].bType != APPLY_NONE)
			{
				m_owner.ApplyPoint(pItemTable->aApplies[i].bType, pItemTable->aApplies[i].lValue);
			}
		}

		// Attrs
		for (int i = 0; i < ITEM_ATTRIBUTE_MAX_NUM; ++i)
		{
			if (pItem->GetAttributeType(i) != POINT_NONE)
			{
				m_owner.ApplyPoint(pItem->GetAttributeType(i), pItem->GetAttributeValue(i));
			}
		}
	}
}
