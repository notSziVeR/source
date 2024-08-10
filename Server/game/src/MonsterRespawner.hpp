#pragma once
#ifdef __CYCLIC_MONSTER_RESPAWNER__
#include <unordered_set>
#include <unordered_map>

namespace MonsterRespawner
{
	struct EnumClassHash
	{
		template <typename T>
		int operator()(T t) const
		{
			return static_cast<int>(t);
		}
	};

	enum class ESpotRefreshType : int
	{
		TYPE_MONSTER,
		TYPE_BOSS,
		TYPE_STONE,
	};

	using TRespawnPair = std::pair<time_t, ESpotRefreshType>;

	extern const std::set<long> v_lRespawnMonsterDelay;
	extern const long lRespawnBossDelay;
	extern const long lRespawnStoneDelay;
	extern const std::unordered_set<DWORD> s_respawn_blacklist;
	extern const std::unordered_set<ESpotRefreshType> s_type_blacklist;
	extern std::unordered_map<std::string, TRespawnPair> m_respawn_times;
	extern const std::unordered_map<ESpotRefreshType, std::string, EnumClassHash> m_say_dictionary;

	void Initialize();
	void RespawnSingleMonster(std::set<DWORD> && s_respawn, const long & lMapIndex, const bool & bBroadcast);
	const long GetRespawnDelayByType(const ESpotRefreshType & eType);
	void ProcessRespawn();
}
#endif

