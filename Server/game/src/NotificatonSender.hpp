#pragma once
#ifdef __NOTIFICATON_SENDER_ENABLE__
#include <vector>
#include <unordered_map>

namespace NotificatonSender
{
	using VPlayerList = std::vector<std::string>;

	class CCollectorCache
	{
		public:
			CCollectorCache() = delete;
			~CCollectorCache() = default;

			CCollectorCache(const VPlayerList & _v_player_list);
			CCollectorCache(VPlayerList && _v_player_list);

		public:
			const VPlayerList & GetPlayerList() { return v_player_list; }
			void UpdateCache(const VPlayerList & _v_player_list) { v_player_list = _v_player_list; }
			void UpdateCache(VPlayerList && _v_player_list) { v_player_list = std::move(_v_player_list); }

		private:
			VPlayerList v_player_list;
	};

	void SendPlayerList(LPCHARACTER ch);
	int SendNotifications(LPCHARACTER ch);
	void GetNotificationAnswer(LPCHARACTER ch);
	VPlayerList GetPlayerListByMapIndex(const long & lMapIndex);
	void CacheCollector(LPCHARACTER ch, VPlayerList && v_player_list);
	CCollectorCache * GetCollectorCache(const DWORD & pid);
	void EraseCacheCollector(const DWORD & pid);
	void CleanUp();

	extern std::unordered_map<DWORD, CCollectorCache *> m_collector_cache;
	extern time_t NOTIFICATION_DELAY;
}
#endif

