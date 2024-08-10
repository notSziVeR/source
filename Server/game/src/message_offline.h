#pragma once
#ifdef __OFFLINE_MESSAGE_SYSTEM__
#include <unordered_map>
#include <unordered_set>

class COfflineMessage : public singleton<COfflineMessage>
{
	struct TOfflineMessageStruct
	{
		std::string sender;
		std::string target;
		char text[CHAT_MAX_LEN - 1];
		BYTE sender_level;
		std::string sent_date;
		time_t expiration_time;
	};

public:
	COfflineMessage();
	virtual ~COfflineMessage() {};

	void Destroy();
	int AddNewMessage(LPCHARACTER ch, const TPacketCGWhisper* pInfo, char * message_text, size_t message_len);
	void SendMessage(LPCHARACTER ch);
	BYTE GetSendMessageCount(const std::string & sSender, const std::string & sTarget);
	void UpdateName(const std::string & old_name, const std::string & new_name);
	// Recv
	void P2PAddMessage(const TPacketGDOfflineMessage * message_packet);
	void P2PDeleteMessage(const std::string & sName);
	void P2PUpdateName(const std::string & old_name, const std::string & new_name);

protected:
	const BYTE MESSAGE_MAX_NUM = 10;

private:
	void LoadMessages();
	void CleanupOutdatedMessage(LPCHARACTER ch);
	std::unordered_map<std::string, std::vector<TOfflineMessageStruct> > P2P_Message_Container;
	std::unordered_set<std::string> s_ExistingPlayers;
	time_t ttNextUpdate;
};
#endif

