#include "stdafx.h"
#ifdef __ENABLE_NEW_LOGS_CHAT__
#include "ChatLogsHelper.hpp"
#include "char.h"
#include "char_manager.h"
#include "desc.h"

void CChatLogs::SendChatLogInformation(CHARACTER* player, const ELogsType eNum, int amount_, int val_)
{
	if (!player)
	{
		return;
	}

	auto desc = player->GetDesc();
	if (!player->GetDesc())
	{
		return;
	}

	net_logs_chat::GC_packet packet = { HEADER_GC_LOGS_CHAT, static_cast<int>(eNum), amount_, val_ };

	desc->Packet(&packet, sizeof(net_logs_chat::GC_packet));
}
#endif
