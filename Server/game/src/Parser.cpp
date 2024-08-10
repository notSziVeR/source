#include "stdafx.h"
#include "char.h"
#include "Parser.hpp"

#include <fstream>

#include <boost/algorithm/string.hpp>
#include "../../libxml/Document.hpp"

namespace EquipmentCollection {

	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "EquipmentSets");
			if (!rootNode) {
				sys_err(fmt::format("Root node <EquipmentSets/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "EquipmentSet") {
					ParseEquipmentSetNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("Missiong Manager Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseEquipmentSetNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint32_t id;
		xml::GetAttribute(root, "id", id);

		std::string name;
		xml::GetAttribute(root, "name", name);

		EquipmentSet::apply_collection_t applies;
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Apply") {
				ParseEquipmentSetApplyNode(node, applies);
			}
		}

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const EquipmentSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Equipment set id {} is already in use.", id).c_str());

		sets_.emplace_back(std::move(id), std::move(name), std::move(applies));
	}

	void Parser::ParseEquipmentSetApplyNode(
		const xml::Node* root, EquipmentSet::apply_collection_t& applies) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		EquipmentSet::Apply apply;

		std::string type;
		xml::GetAttribute(root, "type", type);
		apply.type = FN_get_apply_type(type.c_str());

		if (apply.type == APPLY_NONE)
			sys_log(0, fmt::format("Invalid apply type {}.", type).c_str());

		xml::GetAttribute(root, "value", apply.value);
		xml::GetAttribute(root, "count", apply.count);

		applies.emplace_back(std::move(apply));
	}
};

#ifdef __ENABLE_TELEPORT_SYSTEM__
namespace TeleportCollection {
	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "Teleport-Manager");
			if (!rootNode) {
				sys_err(fmt::format("Root node <Teleport-Manager/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "Category") {
					ParseTeleportSetNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("TeleportCollection Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseTeleportSetNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint32_t id;
		xml::GetAttribute(root, "id", id);

		std::string name;
		xml::GetAttribute(root, "name", name);
		boost::replace_all(name, " ", "_");

		bool admin;
		xml::GetAttribute(root, "admin", admin);

		TeleportSet::teleport_collection_t teleports;
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Teleport") {
				ParseTeleportSetRowNode(node, teleports);
			}
		}

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const TeleportSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Teleport set id {} is already in use.", id).c_str());

		sets_.emplace_back(std::move(id), std::move(name), std::move(admin), std::move(teleports));
	}

	void Parser::ParseTeleportSetRowNode(
		const xml::Node* root, TeleportSet::teleport_collection_t& teleports) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		TeleportSet::Teleport teleport;

		xml::GetAttribute(root, "id", teleport.id);
		xml::GetAttribute(root, "min_lv", teleport.min_level);
		xml::GetAttribute(root, "max_lv", teleport.max_level);
		xml::GetAttribute(root, "x", teleport.x);
		xml::GetAttribute(root, "y", teleport.y);

		teleport.Rehash();
		teleports.emplace_back(std::move(teleport));
	}
};
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
namespace BeginnerCollection
{
	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "Beginner-Manager");
			if (!rootNode) {
				sys_err(fmt::format("Root node <Beginner-Manager/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "Type") {
					ParseTypeNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("BeginnerCollection Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseTypeNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint32_t id;
		xml::GetAttribute(root, "id", id);

		std::string name;
		xml::GetAttribute(root, "name", name);

		BeginnerSet::beginner_item_collection_t items;
		BeginnerSet::beginner_affect_collection_t affects;
		BeginnerSet::beginner_skill_collection_t skills;

		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			const auto& name = xml::GetName(node);

			if (name == "Item") {
				BeginnerSet::item_t item;
				ParseItemNode(node, item);

				items.emplace_back(std::move(item));
			}
			else if (name == "Affect") {
				BeginnerSet::affect_t affect;
				ParseAffectNode(node, affect);

				affects.emplace_back(std::move(affect));
			}
			else if (name == "Skill") {
				BeginnerSet::skill_t skill;
				ParseSkillNode(node, skill);

				skills.emplace_back(std::move(skill));
			}
		}

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const BeginnerSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Beginner set id {} is already in use.", id).c_str());

		sets_.emplace_back(std::move(id), std::move(name), std::move(items), std::move(affects), std::move(skills));
	}

	void Parser::ParseItemNode(const xml::Node* root, BeginnerSet::item_t& item) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");
		xml::GetAttribute(root, "vnum", item.vnum);
		xml::GetAttribute(root, "count", item.count);
		xml::GetAttribute(root, "equip", false, item.equip);

		auto node = xml::TryGetNode(root, "Applys");
		if (node)
			ParseItemApplyNode(node, item.applies);
	}

	void Parser::ParseItemApplyNode(const xml::Node* root, std::vector<BeginnerSet::item_apply_t>& item_applies) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		BeginnerSet::item_apply_t apply;
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Apply") {
				std::string type;
				xml::GetAttribute(node, "type", type);
				apply.type = FN_get_apply_type(type.c_str());

				if (apply.type == APPLY_NONE)
					sys_log(0, fmt::format("Invalid apply type {}.", type).c_str());

				xml::GetAttribute(node, "value", apply.value);

				item_applies.emplace_back(std::move(apply));
			}
		}
	}

	void Parser::ParseAffectNode(const xml::Node* root, BeginnerSet::affect_t& apply) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		std::string type;
		xml::GetAttribute(root, "type", type);
		apply.type = FN_get_apply_type(type.c_str());

		if (apply.type == APPLY_NONE)
			sys_log(0, fmt::format("Invalid apply type {}.", type).c_str());

		xml::GetAttribute(root, "value", apply.value);
		xml::GetAttribute(root, "time", apply.time);
	}

	void Parser::ParseSkillNode(const xml::Node* root, BeginnerSet::skill_t& skill) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		xml::GetAttribute(root, "type", skill.type);
		xml::GetAttribute(root, "value", skill.value);
	}
}
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
namespace BiologCollection
{
	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "Biolog-Manager");
			if (!rootNode) {
				sys_err(fmt::format("Root node <Biolog-Manager/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "Mission") {
					ParseMissionNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("BiologCollection Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseMissionNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint32_t id;
		xml::GetAttribute(root, "id", id);

		uint8_t level;
		xml::GetAttribute(root, "level", level);

		uint32_t item, count;
		xml::GetAttribute(root, "item", item);
		xml::GetAttribute(root, "count", count);

		time_t cooldown;
		xml::GetAttribute(root, "cooldown", cooldown);

		uint8_t chance;
		xml::GetAttribute(root, "chance", chance);

		BiologSet::biolog_reward_collection_t reward;
		BiologSet::biolog_monster_collection_t monsters;
		
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			const auto& name = xml::GetName(node);

			if (name == "Rewards") {
				ParseRewardNode(node, reward);
			}
			else if (name == "Monsters")
			{
				BiologSet::monster_t monster;
				ParseMonsterNode(node, monster);

				monsters.emplace_back(std::move(monster));
			}
		}

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const BiologSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Biolog set id {} is already in use.", id).c_str());

		sys_log(0, fmt::format("Biolog Mission: Pool {} sucessfully registred!", id).c_str());
		sys_log(0, fmt::format("Biolog Mission: Pool {} information! Required level: {}, Required item: {}/{}, Cooldown: {}, Chance: {}!", id, level, item, count, cooldown, chance).c_str());
		sys_log(0, fmt::format("Biolog Mission: Pool {} rewards:", id).c_str());
		sys_log(0, fmt::format("Biolog Reward Item : {}/{}", reward.GetRewardItem().first, reward.GetRewardItem().second).c_str());
		sys_log(0, fmt::format("Biolog Affect Status: {}", reward.GetRewardSelectable()).c_str());

		for (const auto& affect : reward.GetRewardAffects())
		{
			sys_log(0, fmt::format("Biolog Affect : KEY: {} {}/{}, Time: {}", affect.first, affect.second.type, affect.second.value, affect.second.time).c_str());
		}

		sets_.emplace_back(std::move(id), std::move(level), std::move(item), std::move(count), std::move(cooldown), std::move(chance), std::move(reward), std::move(monsters));
	}

	void Parser::ParseRewardNode(const xml::Node* root, BiologSet::reward_t& reward) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			const auto& name = xml::GetName(node);

			if (name == "Item") {
				xml::GetAttribute(node, "vnum", reward.vnum);
				xml::GetAttribute(node, "count", reward.count);
			}
			else if (name == "Affects")
			{
				ParseRewardAffects(node, reward.affects);
			}
		}
	}

	void Parser::ParseRewardAffects(const xml::Node* root, std::pair<bool, std::vector<std::pair<uint8_t, BiologSet::reward_affect_t>>>& reward_affects) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		xml::GetAttribute(root, "selectable", false, reward_affects.first);

		BiologSet::reward_affect_t affect;
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Affect") {
				std::string type;
				xml::GetAttribute(node, "type", type);
				affect.type = FN_get_apply_type(type.c_str());

				if (affect.type == APPLY_NONE)
					sys_log(0, fmt::format("Invalid apply type {}.", type).c_str());

				xml::GetAttribute(node, "value", affect.value);

				xml::GetAttribute(node, "time", affect.time);

				reward_affects.second.emplace_back(reward_affects.second.size(), std::move(affect));
			}
		}
	}

	void Parser::ParseMonsterNode(const xml::Node* root, BiologSet::monster_t& monster) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Monster") {
				xml::GetAttribute(node, "vnum", monster.vnum);
				xml::GetAttribute(node, "chance", monster.chance);
			}
		}
	}
}
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
namespace MarbleCollection
{
	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "Marble-Manager");
			if (!rootNode) {
				sys_err(fmt::format("Root node <Marble-Manager/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "Mission") {
					ParseMarbleSetNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("MarbleCollection Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseMarbleSetNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint8_t id;
		xml::GetAttribute(root, "id", id);

		uint32_t vnum, count;
		xml::GetAttribute(root, "vnum", vnum);
		xml::GetAttribute(root, "count", count);

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const MarbleSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Marble set id {} is already in use.", id).c_str());

		sets_.emplace_back(std::move(id), std::move(vnum), std::move(count));
	}
}
#endif

#ifdef __ENABLE_MOB_TRAKCER__
namespace MobTrackerCollection {
	Parser::Parser(const std::string& filename)
		: filename_(filename)
	{}

	bool Parser::TryParse()
	{
		try {
			xml::Document document(GetFilename());

			auto rootNode = xml::TryGetNode(&document, "Tracker-Manager");
			if (!rootNode) {
				sys_err(fmt::format("Root node <Tracker-Manager/> in {} not found.",
					GetFilename().c_str()).c_str());
				return false;
			}

			for (auto node = rootNode->first_node();
				node;
				node = node->next_sibling()) {
				const auto& name = xml::GetName(node);
				if (name == "Map") {
					ParseTrackerSetNode(node);
				}
			}
		}
		catch (const std::exception& e) {
			sys_err(fmt::format("MobTrackerCollection Parse Error: {}", e.what()).c_str());
			return false;
		}

		return true;
	}

	void Parser::ParseTrackerSetNode(const xml::Node* root) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		uint32_t id;
		xml::GetAttribute(root, "id", id);

		TrackerSet::tracker_collection_t sets;
		for (auto node = root->first_node();
			node;
			node = node->next_sibling()) {
			if (xml::GetName(node) == "Monster") {
				ParseTrackerSetRowNode(node, sets);
			}
		}

		// Print warning to logs if the id is already in use.
		auto it = std::find_if(std::begin(sets_), std::end(sets_),
			[id](const TrackerSet& set) { return set.GetId() == id; });
		if (it != sets_.end())
			sys_log(0, fmt::format("Tracker set id {} is already in use.", id).c_str());

		sets_.emplace_back(std::move(id), std::move(sets));
	}

	void Parser::ParseTrackerSetRowNode(
		const xml::Node* root, TrackerSet::tracker_collection_t& monsters) {
		BOOST_ASSERT_MSG(root, "Node nullptr.");

		TrackerSet::Mob mob;

		mob.id = monsters.size();
		xml::GetAttribute(root, "vnum", mob.vnum);
		xml::GetAttribute(root, "x", mob.x);
		xml::GetAttribute(root, "y", mob.y);

		monsters.emplace_back(std::move(mob));
	}
};
#endif
