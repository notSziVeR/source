#pragma once
#include <boost/noncopyable.hpp>
#include "../../libxml/Types.hpp"
#include "../../libxml/xml.hpp"

namespace EquipmentCollection
{
	class EquipmentSet {
		public:
			struct Apply {
				uint8_t type;
				int32_t value;
				uint32_t count;
			};

			using apply_collection_t = std::vector<Apply>;

		public:
			EquipmentSet(uint32_t id, std::string name, apply_collection_t applies);
			EquipmentSet(EquipmentSet&& o) = default;
			virtual ~EquipmentSet() = default;

			void ApplyTo(LPCHARACTER ch, uint32_t setPartCount) const;

			const uint32_t GetId() const { return id_; }
			const std::string& GetName() const { return name_; }
			const apply_collection_t& GetApplies() const { return applies_; }

		protected:
			uint32_t id_;
			std::string name_;
			apply_collection_t applies_;
	};

	class Parser
		: boost::noncopyable {
		public:
			using equipment_set_collection_t = std::vector<EquipmentSet>;

		public:
			Parser(const std::string& filename);
			virtual ~Parser() = default;

			bool TryParse();

			const std::string& GetFilename() const { return filename_; }

			equipment_set_collection_t EquipmentSets() { return std::move(sets_); }

		protected:
			void ParseEquipmentSetNode(const xml::Node* node);
			void ParseEquipmentSetApplyNode(const xml::Node* node,
				EquipmentSet::apply_collection_t& applies);

		protected:
			const std::string& filename_;

			equipment_set_collection_t sets_;
		};
}

#ifdef __ENABLE_TELEPORT_SYSTEM__
namespace TeleportCollection
{
	class TeleportSet
	{
		public:
			struct Teleport {
				std::string sHash;
				uint8_t id;
				uint8_t min_level;
				uint8_t max_level;
				uint32_t x;
				uint32_t y;

				uint8_t GetID() const
				{
					return id;
				}

				bool operator==(const Teleport& rObj)
				{
					return GetID() == rObj.id;
				}

				bool operator==(const uint8_t& id_)
				{
					return GetID() == id_;
				}

				void Rehash()
				{
					std::string sReturn(std::to_string(id) + std::string("|") + std::to_string(min_level) + std::string("|") + std::to_string(max_level) + std::string("|") + std::to_string(x) + std::string("|") + std::to_string(y));
					sHash = sReturn;
				}

				std::string GetString() const
				{
					return sHash;
				}
			};

			using teleport_collection_t = std::vector<Teleport>;

		public:
			TeleportSet(uint32_t id, std::string name, bool admin, teleport_collection_t teleports);
			TeleportSet(TeleportSet&& o) = default;
			virtual ~TeleportSet() = default;

			const uint32_t GetId() const { return id_; }
			const std::string& GetName() const { return name_; }
			const bool GetAdmin() const { return admin_; }
			const teleport_collection_t& GetTeleports() const { return teleports_; }

			const std::string GetString() const { return (std::to_string(id_) + std::string("|") + name_); }

		protected:
			uint32_t id_;
			std::string name_;
			bool admin_;
			teleport_collection_t teleports_;
	};

	class Parser
		: boost::noncopyable {
			public:
				using teleport_set_collection_t = std::vector<TeleportSet>;

			public:
				Parser(const std::string& filename);
				virtual ~Parser() = default;

				bool TryParse();

				const std::string& GetFilename() const { return filename_; }

				teleport_set_collection_t TeleportSets() { return std::move(sets_); }

			protected:
				void ParseTeleportSetNode(const xml::Node* node);
				void ParseTeleportSetRowNode(const xml::Node* node,
					TeleportSet::teleport_collection_t& applies);

			protected:
				const std::string& filename_;

				teleport_set_collection_t sets_;
	};
}
#endif

#ifdef __ENABLE_BEGINNER_MANAGER__
namespace BeginnerCollection
{
	class BeginnerSet
	{
		public:
			struct Apply
			{
				uint8_t type;
				int32_t value;
			};

			struct Item {
				uint32_t vnum;
				uint32_t count;
				bool equip;
				std::vector<Apply> applies;
			};

			struct Affect
			{
				uint8_t type;
				int32_t value;
				time_t time;
			};

			struct Skill
			{
				uint8_t type;
				uint8_t value;
			};

			using item_t = Item;
			using item_apply_t = Apply;

			using affect_t = Affect;

			using skill_t = Skill;

			using beginner_item_collection_t = std::vector<Item>;
			using beginner_affect_collection_t = std::vector<Affect>;
			using beginner_skill_collection_t = std::vector<Skill>;

		public:
			BeginnerSet(uint32_t id, std::string name, beginner_item_collection_t items, beginner_affect_collection_t applys, beginner_skill_collection_t skills);
			BeginnerSet(BeginnerSet&& o) = default;
			virtual ~BeginnerSet() = default;

			const uint32_t GetId() const { return id_; }
			const std::string& GetName() const { return name_; }
			const beginner_item_collection_t& GetItems() const { return items_; }
			const beginner_affect_collection_t& GetAffects() const { return affects_; }
			const beginner_skill_collection_t& GetSkills() const { return skills_; }

		protected:
			uint32_t id_;
			std::string name_;
			beginner_item_collection_t items_;
			beginner_affect_collection_t affects_;
			beginner_skill_collection_t skills_;
	};

	class Parser
		: boost::noncopyable {
		public:
			using beginner_set_collection_t = std::vector<BeginnerSet>;

		public:
			Parser(const std::string& filename);
			virtual ~Parser() = default;

			bool TryParse();

			const std::string& GetFilename() const { return filename_; }

			beginner_set_collection_t BeginnerSets() { return std::move(sets_); }

		protected:
			void ParseTypeNode(const xml::Node* node);
			void ParseItemNode(const xml::Node* node, BeginnerSet::item_t& item);
			void ParseItemApplyNode(const xml::Node* node, std::vector<BeginnerSet::item_apply_t>& item_applies);

			void ParseAffectNode(const xml::Node* root, BeginnerSet::affect_t& apply);
			void ParseSkillNode(const xml::Node* root, BeginnerSet::skill_t& skill);

		protected:
			const std::string& filename_;

			beginner_set_collection_t sets_;
	};
}
#endif

#ifdef __ENABLE_BIOLOG_SYSTEM__
namespace BiologCollection
{
	class BiologSet
	{
		public:
			struct Affect
			{
				uint8_t type;
				int32_t value;
				time_t time;
			};

			struct Reward {
				uint32_t vnum;
				uint32_t count;
				std::pair<bool, std::vector<std::pair<uint8_t, Affect>>> affects;

				std::pair<uint32_t, uint32_t> GetRewardItem() const
				{
					return { vnum, count };
				}

				bool GetRewardSelectable() const
				{
					return affects.first;
				}

				std::vector<std::pair<uint8_t, Affect>> GetRewardAffects() const
				{
					return affects.second;
				}
			};

			struct Monster {
				uint32_t vnum;
				uint8_t chance;
			};

			using reward_t = Reward;
			using reward_affect_t = Affect;
			using monster_t = Monster;

			using biolog_reward_collection_t = Reward;
			using biolog_monster_collection_t = std::vector<Monster>;

		public:
			BiologSet(uint32_t id, uint8_t level, uint32_t item, uint32_t count, time_t cooldown, uint8_t chance, biolog_reward_collection_t reward, biolog_monster_collection_t monsters);
			BiologSet(BiologSet&& o) = default;
			virtual ~BiologSet() = default;

			const uint32_t GetId() const { return id_; }
			const uint8_t GetLevel() const { return level_; }
			const uint32_t GetRequiredItem() const { return item_; }
			const uint32_t GetRequiredCount() const { return count_; }
			const time_t GetCooldown() const { return cooldown_; }
			const uint8_t GetChance() const { return chance_; }
			const biolog_reward_collection_t& GetReward() const { return reward_; }
			const biolog_monster_collection_t& GetMonsters() const { return monsters_; }

		protected:
			uint32_t id_;
			uint8_t level_;
			uint32_t item_, count_;
			time_t cooldown_;
			uint8_t chance_;
			biolog_reward_collection_t reward_;
			biolog_monster_collection_t monsters_;
	};

	class Parser
		: boost::noncopyable {
		public:
			using biolog_set_collection_t = std::vector<BiologSet>;

		public:
			Parser(const std::string& filename);
			virtual ~Parser() = default;

			bool TryParse();

			const std::string& GetFilename() const { return filename_; }

			biolog_set_collection_t BiologSets() { return std::move(sets_); }

		protected:
			void ParseMissionNode(const xml::Node* root);
			void ParseRewardNode(const xml::Node* root, BiologSet::reward_t& reward);
			void ParseRewardAffects(const xml::Node* root, std::pair<bool, std::vector<std::pair<uint8_t, BiologSet::reward_affect_t>>>& reward_affects);
			void ParseMonsterNode(const xml::Node* root, BiologSet::monster_t& monster);

		protected:
			const std::string& filename_;

			biolog_set_collection_t sets_;
	};
}
#endif

#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
namespace MarbleCollection
{
	struct MarbleSet
	{
		public:
			MarbleSet(uint8_t id, uint32_t vnum, uint32_t count);
			MarbleSet(MarbleSet&& o) = default;
			virtual ~MarbleSet() = default;

			const uint8_t GetId() const { return id_; }
			const uint32_t GetVnum() const { return vnum_; }
			const uint32_t GetCount() const { return count_; }

		public:
			uint8_t id_;
			uint32_t vnum_;
			uint32_t count_;
	};

	class Parser
		: boost::noncopyable {
		public:
			using marble_set_collection_t = std::vector<MarbleSet>;

		public:
			Parser(const std::string& filename);
			virtual ~Parser() = default;

			bool TryParse();

			const std::string& GetFilename() const { return filename_; }

			marble_set_collection_t MarbleSets() { return std::move(sets_); }

		protected:
			void ParseMarbleSetNode(const xml::Node* node);

		protected:
			const std::string& filename_;

			marble_set_collection_t sets_;
	};
}
#endif

#ifdef __ENABLE_MOB_TRAKCER__
namespace MobTrackerCollection
{
	class TrackerSet
	{
		public:
			struct Mob {
				uint8_t id;
				uint32_t vnum;
				uint8_t x;
				uint8_t y;

				uint8_t GetID() const
				{
					return id;
				}

				bool operator==(const Mob& rObj)
				{
					return GetID() == rObj.id;
				}

				bool operator==(const uint8_t& id_)
				{
					return GetID() == id_;
				}
		};

		using tracker_collection_t = std::vector<Mob>;

	public:
		TrackerSet(uint32_t id, tracker_collection_t trackers);
		TrackerSet(TrackerSet&& o) = default;
		virtual ~TrackerSet() = default;

		const uint32_t GetId() const { return id_; }
		const tracker_collection_t& GetTrackers() const { return trackers_; }

	protected:
		uint32_t id_;
		tracker_collection_t trackers_;
	};

	class Parser
		: boost::noncopyable {
		public:
			using tracker_set_collection_t = std::vector<TrackerSet>;

		public:
			Parser(const std::string& filename);
			virtual ~Parser() = default;

			bool TryParse();

			const std::string& GetFilename() const { return filename_; }

			tracker_set_collection_t TrackerSets() { return std::move(sets_); }

		protected:
			void ParseTrackerSetNode(const xml::Node* node);
			void ParseTrackerSetRowNode(const xml::Node* node,
				TrackerSet::tracker_collection_t& trackers);

		protected:
			const std::string& filename_;

			tracker_set_collection_t sets_;
	};
}
#endif
