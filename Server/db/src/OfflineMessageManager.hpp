#pragma once
#ifdef __OFFLINE_MESSAGE_SYSTEM__
#include <unordered_map>

class CPeer;
class CMessageOfflineManager : public singleton<CMessageOfflineManager>
{
	struct TOfflineMessageStruct
	{
		std::string sender;
		std::string target;
		char text[CHAT_MAX_LEN-1];
		BYTE sender_level;
		std::string sent_date;
		time_t expiration_time;
	};

	public:
		CMessageOfflineManager();
		virtual ~CMessageOfflineManager();
		void Destroy();

	public:
		void RecvNewMessage(const TPacketGDOfflineMessage * message_packet);
		void DeleteMessage(const TPacketGDOfflineMessage * message_packet);
		void BroadcastResults(CPeer * peer = nullptr);
		void UpdatePlayerName(const TPacketGDOfflineMessage * message_packet);
		void DeletePlayer(const std::string & sName);

		void CleanupOutdatedMessage(bool bSkipTime = false);

	private:
		void Initialize();

	private:
		std::unordered_map<std::string, std::vector<TOfflineMessageStruct>> Query_Message_Container;
		time_t ttNextUpdate;
		bool bDestroyed;
};
#endif

