#pragma once
#include <map>

class CEventManager : public singleton<CEventManager>
{
	public:
		CEventManager();
		virtual ~CEventManager();
		void Destroy();

	public:
		bool RegisterEvent(std::string && e_name, time_t && e_time);
		bool UnregisterEvent(std::string && e_name);
		void RefreshImplementorPanel(LPCHARACTER ch);
		void SendEventList(LPCHARACTER ch, const std::string & e_name = "");
		void BroadcastNewEvent(std::string && e_name, std::string && e_time, time_t && ttEndTime);
		void RecvEventPacket(const TPacketGGEventInfo * pack);
		void Update();

	private:
		const time_t ttEventUpdate = 5;
		std::string s_current_loop_event;
		std::map<std::string, std::pair<time_t, std::string>> m_current_event;
};

