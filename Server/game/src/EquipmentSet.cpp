#include "stdafx.h"
#include "char.h"
#include <fstream>
#include "EquipmentSet.hpp"

#include "../../libxml/Document.hpp"

EquipmentCollection::EquipmentSet::EquipmentSet(uint32_t id, std::string name, apply_collection_t applies)
	: id_(std::move(id))
	, name_(std::move(name))
	, applies_(std::move(applies))
{}

void EquipmentCollection::EquipmentSet::ApplyTo(LPCHARACTER ch, uint32_t setPartCount) const {
	for (const auto& apply : applies_) {
		if (setPartCount < apply.count)
			continue;

		ch->AddAffect(AFFECT_EQUIPMENT_SET, apply.type, apply.value, 0, INFINITE_AFFECT_DURATION, 0, false, false);
	}
}

bool EquipmentSetSettings::Load(const std::string& filename) {
	EquipmentCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	auto sets = std::move(parser.EquipmentSets());
	for (auto& set : sets)
		sets_.emplace(set.GetId(), std::move(set));

	return true;
}

void EquipmentSetSettings::Clear() {
	sets_.clear();
}

const EquipmentCollection::EquipmentSet* EquipmentSetSettings::FindSet(const uint32_t id) const {
	auto it = sets_.find(id);
	if (it == sets_.end())
		return nullptr;

	return &it->second;
}

void EquipmentSetSettings::Create() {
	Load("locale/germany/equipment-sets.xml");
}
