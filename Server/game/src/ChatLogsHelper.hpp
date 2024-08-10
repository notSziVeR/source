#pragma once
#include "stdafx.h"

#ifdef __ENABLE_NEW_LOGS_CHAT__
namespace net_logs_chat
{
	#pragma pack(1)
	struct GC_packet {
		uint8_t header;

		int type_;
		int amount_;
		int value_;
	};
	#pragma pack()
}

enum class ELogsType : int
{
	LOG_TYPE_YANG,
	LOG_TYPE_PICKUP,
	LOG_TYPE_EXCHANGE,
	LOG_TYPE_SHOP,
	LOG_TYPE_CHEST,
	LOG_TYPE_GAYA,
};

class CChatLogs
{
	public:
		static void SendChatLogInformation(CHARACTER* player, const ELogsType eNum, int am_, int val_);
};
#endif
