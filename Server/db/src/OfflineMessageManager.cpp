#include "stdafx.h"
#ifdef __OFFLINE_MESSAGE_SYSTEM__
#include "Main.h"
#include "DBManager.h"
#include "Peer.h"
#include "ClientManager.h"
#include "OfflineMessageManager.hpp"

CMessageOfflineManager::CMessageOfflineManager() : bDestroyed{false}
{
	Initialize();
}

CMessageOfflineManager::~CMessageOfflineManager()
{}

void CMessageOfflineManager::Initialize()
{
	std::string query = std::string("SELECT sender, target, message, sender_level, sent_date, expiration_time FROM player") + std::string(GetTablePostfix()) + std::string(".whisper_offline");
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(query.c_str()));
	if (msg->Get()->uiNumRows == 0)
	{
		return;
	}

	// Ret Tab Struct
	TOfflineMessageStruct message {};

	MYSQL_ROW row;
	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	while ((row = mysql_fetch_row(pRes)))
	{
		message.sender = row[0];
		message.target = row[1];
		strlcpy(message.text, row[2], sizeof(message.text));
		message.sender_level = atoi(row[3]);
		message.sent_date = row[4];
		message.expiration_time = atoi(row[5]);

		if (message.expiration_time > time(0))
		{
			auto it = Query_Message_Container.find(message.target);
			if (it != Query_Message_Container.end())
			{
				(it->second).push_back(message);
			}
			else
				Query_Message_Container.insert(std::make_pair(message.target, std::vector<TOfflineMessageStruct> {message}));
		}
	}

	BroadcastResults();
	ttNextUpdate = time(0) + OFFLINE_MESSAGE_COLLECTOR;
}

void CMessageOfflineManager::Destroy()
{
	if (bDestroyed)
	{
		return;
	}

	// Truncating old data
	std::string query = std::string("DELETE FROM player") + std::string(GetTablePostfix()) + std::string(".whisper_offline");
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(query.c_str()));

	// Caching up-to-date records
	char buf[256];
	for (const auto & mes : Query_Message_Container)
	{
		for (const auto & message : mes.second)
		{
			if (message.expiration_time <= time(0))
			{
				continue;
			}

			snprintf(buf, sizeof(buf), "INSERT INTO player%s.whisper_offline (sender, target, message, sender_level, sent_date, expiration_time) VALUES ('%s', '%s', '%s', %d, '%s', %d)", GetTablePostfix(), message.sender.c_str(), message.target.c_str(), message.text, message.sender_level, message.sent_date.c_str(), message.expiration_time);
			msg.reset(CDBManager::instance().DirectQuery(buf));
		}
	}

	Query_Message_Container.clear();
	bDestroyed = true;
}

void CMessageOfflineManager::RecvNewMessage(const TPacketGDOfflineMessage * message_packet)
{
	TOfflineMessageStruct message;
	message.sender = message_packet->sender;
	message.target = message_packet->target;
	strlcpy(message.text, message_packet->text, sizeof(message.text));
	message.sender_level = message_packet->sender_level;
	message.sent_date = message_packet->sent_date;
	message.expiration_time = message_packet->expiration_time;

	auto it = Query_Message_Container.find(message.target);
	if (it != Query_Message_Container.end())
	{
		(it->second).push_back(message);
	}
	else
		Query_Message_Container.insert(std::make_pair(message.target, std::vector<TOfflineMessageStruct> {message}));

	// Broadcast to cores
	TPacketGDOfflineMessage retPack;
	memcpy(&retPack, message_packet, sizeof(retPack));
	retPack.header = HEADER_DG_OFFLINE_MESSAGE;
	retPack.subheader = SUBHEADER_OFFLINE_MESSAGE_ADD_RECORD;

	CClientManager::instance().ForwardPacket(HEADER_DG_OFFLINE_MESSAGE, &retPack, sizeof(retPack));

	sys_log(0, "Sender: %s, Target: %s, Mess Count: %d", message_packet->sender, message_packet->target, Query_Message_Container.size());
}

void CMessageOfflineManager::DeleteMessage(const TPacketGDOfflineMessage * message_packet)
{
	auto it = Query_Message_Container.find(message_packet->target);
	if (it != Query_Message_Container.end())
	{
		Query_Message_Container.erase(it);

		// Broadcast to cores
		TPacketGDOfflineMessage retPack;
		memcpy(&retPack, message_packet, sizeof(retPack));
		retPack.header = HEADER_DG_OFFLINE_MESSAGE;
		retPack.subheader = SUBHEADER_OFFLINE_MESSAGE_REMOVE_RECORD;

		CClientManager::instance().ForwardPacket(HEADER_DG_OFFLINE_MESSAGE, &retPack, sizeof(retPack));
	}

	sys_log(0, "Mess Count: %d", Query_Message_Container.size());
}

void CMessageOfflineManager::BroadcastResults(CPeer * peer)
{
	TPacketGDOfflineMessage retPack;
	memset(&retPack, 0, sizeof(retPack));
	retPack.header = HEADER_DG_OFFLINE_MESSAGE;
	retPack.subheader = SUBHEADER_OFFLINE_MESSAGE_ADD_RECORD;

	// Performing cleanup for outdated messages
	// Not need to broadcast - each core should do it itself
	CleanupOutdatedMessage(true);

	for (const auto & mes : Query_Message_Container)
	{
		for (const auto & message : mes.second)
		{
			strlcpy(retPack.sender, message.sender.c_str(), sizeof(retPack.sender));
			strlcpy(retPack.target, message.target.c_str(), sizeof(retPack.target));
			strlcpy(retPack.text, message.text, sizeof(retPack.text));
			retPack.sender_level = message.sender_level;
			strlcpy(retPack.sent_date, message.sent_date.c_str(), sizeof(retPack.sent_date));
			retPack.expiration_time = message.expiration_time;

			if (peer)
			{
				peer->EncodeHeader(HEADER_DG_OFFLINE_MESSAGE, 0, sizeof(retPack));
				peer->Encode(&retPack, sizeof(retPack));
			}
			else
			{
				CClientManager::instance().ForwardPacket(HEADER_DG_OFFLINE_MESSAGE, &retPack, sizeof(retPack));
			}
		}
	}

	sys_log(0, "Mess Count: %d", Query_Message_Container.size());
}

void CMessageOfflineManager::UpdatePlayerName(const TPacketGDOfflineMessage * message_packet)
{
	/*
		Sender - old name
		Target - new name
	*/

	std::string old_name(message_packet->sender);
	std::string new_name(message_packet->target);

	// Updating containers
	std::vector<TOfflineMessageStruct> tmp_vec;
	auto it = Query_Message_Container.find(old_name);

	if (it != Query_Message_Container.end())
	{
		tmp_vec = it->second;
		Query_Message_Container.erase(it);
		Query_Message_Container.insert(std::make_pair(new_name, std::move(tmp_vec)));
	}

	for (auto & mes : Query_Message_Container)
	{
		for (auto & message : mes.second)
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

	TPacketGDOfflineMessage retPack;
	memset(&retPack, 0, sizeof(retPack));
	retPack.header = HEADER_DG_OFFLINE_MESSAGE;
	retPack.subheader = SUBHEADER_OFFLINE_MESSAGE_CHANGE_NAME;
	strlcpy(retPack.sender, old_name.c_str(), sizeof(retPack.sender));
	strlcpy(retPack.target, new_name.c_str(), sizeof(retPack.target));

	CClientManager::instance().ForwardPacket(HEADER_DG_OFFLINE_MESSAGE, &retPack, sizeof(retPack));
}

void CMessageOfflineManager::DeletePlayer(const std::string & sName)
{
	// Updating containers
	std::vector<TOfflineMessageStruct> tmp_vec;
	auto it = Query_Message_Container.find(sName);

	if (it != Query_Message_Container.end())
	{
		Query_Message_Container.erase(it);
	}

	TPacketGDOfflineMessage retPack;
	memset(&retPack, 0, sizeof(retPack));
	retPack.header = HEADER_DG_OFFLINE_MESSAGE;
	retPack.subheader = SUBHEADER_OFFLINE_MESSAGE_DELETE_NAME;
	strlcpy(retPack.sender, sName.c_str(), sizeof(retPack.sender));

	CClientManager::instance().ForwardPacket(HEADER_DG_OFFLINE_MESSAGE, &retPack, sizeof(retPack));

	sys_log(0, "Mess Count: %d", Query_Message_Container.size());
}

void CMessageOfflineManager::CleanupOutdatedMessage(bool bSkipTime)
{
	if (!bSkipTime && ttNextUpdate > time(0))
	{
		return;
	}

	for (auto & mes : Query_Message_Container)
	{
		if (mes.second.size())
			mes.second.erase(std::remove_if(mes.second.begin(), mes.second.end(), [] (TOfflineMessageStruct & rElement) -> bool { return time(0) >= rElement.expiration_time; }), mes.second.end());
	}

	if (!bSkipTime)
	{
		ttNextUpdate = time(0) + OFFLINE_MESSAGE_COLLECTOR;
	}
}
#endif

