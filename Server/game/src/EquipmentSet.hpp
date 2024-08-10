#pragma once
#include "../../libxml/Types.hpp"
#include "../../libxml/xml.hpp"

#include "Parser.hpp"


class EquipmentSetSettings : public singleton<EquipmentSetSettings> {
	public:
		EquipmentSetSettings() = default;
		virtual ~EquipmentSetSettings() = default;

		bool Load(const std::string& filename);
		void Clear();

		const EquipmentCollection::EquipmentSet* FindSet(const uint32_t id) const;

		void Create();

	protected:
		std::map<uint32_t, EquipmentCollection::EquipmentSet> sets_;

};