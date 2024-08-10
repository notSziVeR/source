#include "stdafx.h"
#ifdef __OFFLINE_MESSAGE_SYSTEM__
#include "char.h"
#include "db.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include "packet.h"
#include "config.h"
#include "p2p.h"
#include "lzo_manager.h"
#include "message_offline.h"
#include "utils.h"

COfflineMessage::COfflineMessage()
{
	P2P_Message_Container.clear();
	LoadMessages();
}

void COfflineMessage::Destroy()
{
	P2P_Message_Container.clear();
}

void COfflineMessage::LoadMessages()
{
	// Sending boot request
	TPacketGDOfflineMessage pack_db;
	memset(&pack_db, 0, sizeof(pack_db));
	pack_db.subheader = SUBHEADER_OFFLINE_MESSAGE_BOOT;
	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_MESSAGE, 0, &pack_db, sizeof(pack_db));

	ttNextUpdate = get_global_time() + OFFLINE_MESSAGE_COLLECTOR;
}

BYTE COfflineMessage::GetSendMessageCount(const std::string & sSender, const std::string & sTarget)
{
	BYTE bCount = 0;
	auto it = P2P_Message_Container.find(sTarget);

	// Summing up messages
	if (it != P2P_Message_Container.end())
		std::for_each((it->second).begin(), (it->second).end(), [&bCount, &sSender] (const TOfflineMessageStruct & eR) { if (eR.sender == sSender) ++bCount; });

	return bCount;
}

int COfflineMessage::AddNewMessage(LPCHARACTER ch, const TPacketCGWhisper* pInfo, char * message_text, size_t message_len)
{
	/*
		Return Types
		-1 - Maximum limit has been reached
	*/

	// Spam filter is applicable only if limit is set. Otherwise just skip it. (check header)
	if (MESSAGE_MAX_NUM && GetSendMessageCount(std::string(ch->GetName()), pInfo->szNameTo) >= MESSAGE_MAX_NUM)
	{
		return -1;
	}

	// Setting up additional buffer for target name
	WORD buf_size = strlen(pInfo->szNameTo) * 2 + 2;
	char * b_name = new char[buf_size];

	if (!DBManager::instance().EscapeString(b_name, buf_size, pInfo->szNameTo, strlen(pInfo->szNameTo)))
	{
		delete [] b_name;
		return -3;
	}

	// Buffer for message content
	buf_size = message_len * 2 + 2;
	char * b_text = new char[buf_size];

	if (!DBManager::instance().EscapeString(b_text, buf_size, message_text, message_len))
	{
		delete [] b_name;
		delete [] b_text;
		return -3;
	}

	// Sending DB Packet
	TPacketGDOfflineMessage pack;
	memset(&pack, 0, sizeof(pack));
	pack.subheader = SUBHEADER_OFFLINE_MESSAGE_ADD_RECORD;
	strlcpy(pack.sender, ch->GetName(), sizeof(pack.sender));
	strlcpy(pack.target, b_name, sizeof(pack.target));
	strlcpy(pack.text, b_text, sizeof(pack.text));
	pack.sender_level = ch->GetLevel();
	strlcpy(pack.sent_date, GetFullDateFromTime(get_global_time()).c_str(), sizeof(pack.sent_date));
	pack.expiration_time = get_global_time() + OFFLINE_MESSAGE_EXPIRATION_TIME;
	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_MESSAGE, 0, &pack, sizeof(pack));

	delete [] b_name;
	delete [] b_text;
	return 0;
}

void COfflineMessage::SendMessage(LPCHARACTER ch)
{
	if (!ch->GetDesc())
	{
		return;
	}

	auto it = P2P_Message_Container.find(std::string(ch->GetName()));
	if (it == P2P_Message_Container.end())
	{
		return;
	}

	// No need to implemented any kind of event/routine
	// Player login can be set as collector trigger
	CleanupOutdatedMessage(ch);

	TPacketGCWhisper pack;
	memset(&pack, 0, sizeof(pack));
	pack.bHeader = HEADER_GC_WHISPER;
	pack.bType = WHISPER_TYPE_MESSAGE_SENT;
#ifdef __ENABLE_MULTI_LANGUAGE_SYSTEM_FLAG_VISIBILITY__
	pack.iLocale = -1;
#endif

	// Sending cached messages to player
	TEMP_BUFFER tmpbuf;
	for (auto const & message : it->second)
	{
		pack.bLevel = message.sender_level;
		pack.wSize = sizeof(TPacketGCWhisper) + sizeof(message.text);
		strlcpy(pack.szNameFrom, message.sender.c_str(), sizeof(pack.szNameFrom));
		strlcpy(pack.szSentDate, message.sent_date.c_str(), sizeof(pack.szSentDate));
		tmpbuf.write(&pack, sizeof(pack));
		tmpbuf.write(message.text, sizeof(message.text));

		ch->GetDesc()->Packet(tmpbuf.read_peek(), tmpbuf.size());
		tmpbuf.reset();
	}

	// Sending DB Packet
	TPacketGDOfflineMessage pack_db;
	memset(&pack_db, 0, sizeof(pack_db));
	pack_db.subheader = SUBHEADER_OFFLINE_MESSAGE_REMOVE_RECORD;
	strlcpy(pack_db.target, ch->GetName(), sizeof(pack_db.target));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_MESSAGE, 0, &pack_db, sizeof(pack_db));
}

void COfflineMessage::UpdateName(const std::string & old_name, const std::string & new_name)
{
	// Sending DB Packet
	TPacketGDOfflineMessage pack_db;
	memset(&pack_db, 0, sizeof(pack_db));
	pack_db.subheader = SUBHEADER_OFFLINE_MESSAGE_CHANGE_NAME;
	strlcpy(pack_db.sender, old_name.c_str(), sizeof(pack_db.sender));
	strlcpy(pack_db.target, new_name.c_str(), sizeof(pack_db.target));
	db_clientdesc->DBPacket(HEADER_GD_OFFLINE_MESSAGE, 0, &pack_db, sizeof(pack_db));
}

///// RECV /////
void COfflineMessage::P2PAddMessage(const TPacketGDOfflineMessage * message_packet)
{
	// Recv P2P Packet
	TOfflineMessageStruct message;
	message.sender = message_packet->sender;
	message.target = message_packet->target;
	strlcpy(message.text, message_packet->text, sizeof(message.text));
	message.sender_level = message_packet->sender_level;
	message.sent_date = message_packet->sent_date;
	message.expiration_time = message_packet->expiration_time;

	auto it = P2P_Message_Container.find(message.target);
	if (it != P2P_Message_Container.end())
	{
		it->second.push_back(message);
	}
	else
		P2P_Message_Container.insert(std::make_pair(message.target, std::vector<TOfflineMessageStruct> {message}));
}

void COfflineMessage::P2PDeleteMessage(const std::string & sName)
{
	// Recv P2P Packet
	auto it = P2P_Message_Container.find(sName);
	if (it != P2P_Message_Container.end())
	{
		P2P_Message_Container.erase(it);
	}
}

void COfflineMessage::P2PUpdateName(const std::string & old_name, const std::string & new_name)
{
	// Recv P2P Packet
	std::vector<TOfflineMessageStruct> tmp_vec;
	auto it = P2P_Message_Container.find(old_name);
	if (it != P2P_Message_Container.end())
	{
		tmp_vec = it->second;
		P2P_Message_Container.erase(it);
		P2P_Message_Container.insert(std::make_pair(new_name, std::move(tmp_vec)));
	}

	for (auto && mes : P2P_Message_Container)
	{
		for (auto && message : mes.second)
		{
			if (message.sender == old_name)
			{
				message.sender = new_name;
			}

			if (message.target == old_name)
			{
				message.target = new_name;
			}
		}
	}
}

void COfflineMessage::CleanupOutdatedMessage(LPCHARACTER ch)
{
	if (ttNextUpdate > get_global_time())
	{
		// Since there still isn't an update time we only perform checkup for given player
		auto it = P2P_Message_Container.find(std::string(ch->GetName()));
		if (it != P2P_Message_Container.end())
		{
			auto & rMes = it->second;
			if (rMes.size())
				rMes.erase(std::remove_if(rMes.begin(), rMes.end(), [] (TOfflineMessageStruct & rElement) -> bool { return get_global_time() >= rElement.expiration_time; }), rMes.end());
		}

		return;
	}

	for (auto & mes : P2P_Message_Container)
	{
		if (mes.second.size())
			mes.second.erase(std::remove_if(mes.second.begin(), mes.second.end(), [] (TOfflineMessageStruct & rElement) -> bool { return get_global_time() >= rElement.expiration_time; }), mes.second.end());
	}

	ttNextUpdate = get_global_time() + OFFLINE_MESSAGE_COLLECTOR;
}
#endif

