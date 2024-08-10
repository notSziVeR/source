#include "stdafx.h"
#ifdef __ENABLE_BEGINNER_MANAGER__
#include "char.h"
#include "item.h"

#include "BeginnerHelper.hpp"

BeginnerCollection::BeginnerSet::BeginnerSet(uint32_t id, std::string name, beginner_item_collection_t items, beginner_affect_collection_t affects, beginner_skill_collection_t skills)
	: id_(std::move(id))
	, name_(std::move(name))
	, items_(std::move(items))
	, affects_(std::move(affects))
	, skills_(std::move(skills))
{}

namespace
{
	const std::string sFinishFlag("beginner_manager.granted_items");
	const size_t REQUIRED_LEVEL = 5;
};

void BeginnerHelper::Create()
{
	Load("locale/germany/beginner-manager.xml");
}

void BeginnerHelper::Clear()
{
	sets_.clear();
}

bool BeginnerHelper::Load(const std::string& filename)
{
	BeginnerCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	auto sets = std::move(parser.BeginnerSets());
	for (auto& set : sets)
		sets_.emplace(set.GetId(), std::move(set));

	return true;
}

void BeginnerHelper::Broadcast(LPCHARACTER ch)
{
	if (!ch || !ch->GetDesc() || !ch->IsPC() || ch->GetSkillGroup()) return;

	if (ch->GetLevel() < REQUIRED_LEVEL) return;

	ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("RECV_SkillSelect {}", ch->GetJob()));
}

void BeginnerHelper::RecvChoice(LPCHARACTER ch, uint8_t iChoice)
{
	if (!ch || !ch->IsPC() || ch->GetSkillGroup()) return;

	if (iChoice == 0) return;

	ch->SetSkillGroup(iChoice);
	ch->ClearSkill();
	ch->SkillLevelPacket();
}

void BeginnerHelper::Collect(LPCHARACTER ch)
{
	if (!ch) return;

	if (ch->GetQuestFlag(sFinishFlag)) return;

	for (auto iKey : { ch->GetJob(), static_cast<BYTE>(JOB_MAX_NUM) })
	{
		auto iFit = sets_.find(iKey);
		if (iFit != sets_.end())
		{
			// Lets give whole items!
			for (const auto& rItem : iFit->second.GetItems())
			{
				auto pItem = ch->AutoGiveItem(rItem.vnum, rItem.count);
				if (!pItem) continue;

				for (const auto& [apply, value] : rItem.applies)
				{
					if (apply == APPLY_NONE) continue;
					pItem->SetForceAttribute(pItem->GetAttributeCount(), apply, value);
				}

				if (rItem.equip)
					pItem->EquipTo(ch, pItem->FindEquipCell(ch));
			}

			// Lets set whole affects
			for (const auto& rAffect : iFit->second.GetAffects())
			{
				ch->AddAffect(AFFECT_COLLECT, aApplyInfo[rAffect.type].bPointType, rAffect.value, 0, rAffect.time, 0, false);
			}

			// Lets set whole skills
			{
				for (const auto& rSkill : iFit->second.GetSkills())
				{
					ch->SetSkillLevel(rSkill.type, rSkill.value);
				}
			}
		}
	}

	ch->SetQuestFlag(sFinishFlag, 1);
}
#endif
