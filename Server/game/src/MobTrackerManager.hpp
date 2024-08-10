#include "Parser.hpp"

#ifdef __ENABLE_MOB_TRAKCER__
class MobTrackerManager : public singleton<MobTrackerManager>
{
	struct MobTrackerInstance
	{
		uint8_t id;
		uint32_t vnum;
		uint16_t x, y;
		time_t time;
		time_t delay;

		uint8_t GetID() const
		{
			return id;
		}

		uint32_t GetVnum() const
		{
			return vnum;
		}

		void AppendDelay(const time_t time_)
		{
			delay = time_;
		}

		bool operator==(const MobTrackerInstance& rObj)
		{
			return GetID() == rObj.id;
		}

		bool operator==(const uint8_t& id_)
		{
			return GetID() == id_;
		}
	};

	public:
		MobTrackerManager() = default;
		virtual ~MobTrackerManager() = default;

		void Create();
		void Clear();
		void Destroy();
		bool Load(const std::string& fileName);

		size_t RegisterInstance(const uint8_t id, const uint32_t vnum, const uint16_t x, const uint16_t y, const time_t time);
		void ApplyDelay(const uint32_t map_id, const size_t id);
		// Reload Function
		void Reload();

	public:
		// Connection
		void Broadcast(LPCHARACTER ch, bool bClear = true, const int iNum = -1);
		void RequestTeleport(LPCHARACTER ch, const uint8_t id);

	private:
		const std::vector<MobTrackerInstance>* FindSet(const uint32_t id) const;
		MobTrackerInstance* GetInstance(const uint32_t map_id, const uint32_t id);
	
	protected:
		std::map<uint32_t, std::vector<MobTrackerInstance>> sets_;
};
#endif
