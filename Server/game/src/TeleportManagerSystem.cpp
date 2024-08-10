#include "stdafx.h"
#ifdef __ENABLE_TELEPORT_SYSTEM__
#include "TeleportManagerSystem.hpp"
#include "stdafx.h"
#include "char.h"
#include "p2p.h"
#include "utils.h"
#include "sectree_manager.h"

#include <boost/algorithm/string.hpp>

namespace
{
	const std::string sBroadcastClearInterface("TeleportManager_ClearInterface");
	const std::string sBroadcastRegisterCategory("TeleportManager_RegisterCategory {}");
	const std::string sBroadcastRegisterTeleport("TeleportManager_RegisterTeleport {} {} {}");
}

TeleportCollection::TeleportSet::TeleportSet(uint32_t id, std::string name, bool admin, teleport_collection_t teleports)
	: id_(std::move(id))
	, name_(std::move(name))
	, admin_(std::move(admin))
	, teleports_(std::move(teleports))
{}

void CTeleportManager::Create()
{
	Load("locale/germany/teleport-manager.xml");
}

void CTeleportManager::Clear()
{
	sets_.clear();
}

void CTeleportManager::Destroy()
{
	//Clearing data..
	Clear();
}

bool CTeleportManager::Load(const std::string& filename)
{
	TeleportCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	auto sets = std::move(parser.TeleportSets());
	for (auto& set : sets)
		sets_.emplace(set.GetId(), std::move(set));

	return true;
}

void CTeleportManager::Reload()
{
	// Clearing old data
	Clear();

	// Loading new data again
	Create();
}

void CTeleportManager::SendInformation(LPCHARACTER& ch)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface);

	for (const auto& set : sets_)
	{
		if (set.second.GetAdmin() && !ch->IsGM()) continue;
		
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRegisterCategory, set.second.GetString()));

		for (const auto& teleport : set.second.GetTeleports())
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRegisterTeleport, set.first, set.second.GetString(), teleport.GetString()));
		}
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, "TeleportManager_Show");
}

void CTeleportManager::Teleport(LPCHARACTER& ch, DWORD& dwIndex)
{
	if (!ch || !ch->IsPC())
	{
		return;
	}

	if (!ch->CanWarp() || !ch->CanDoAction() || ch->GetWarMap())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TELEPORT_NOT_AVAILABLE"));
		return;
	}

	auto pItem = FindSet(dwIndex);
	if (!pItem) return;

	if (pItem->min_level > ch->GetLevel())
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TELEPORT_LOW_LEVEL"));
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TELEPORT_LOW_LEVEL_INFO %d"), pItem->min_level);
		return;
	}

	if (!pItem->x && !pItem->y)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("TELEPORT_NOT_READY_YET"));
		return;
	}

	ch->WarpSet(pItem->x * 100, pItem->y * 100, 0);
}

bool CTeleportManager::ValidateMap(LPCHARACTER ch, DWORD dwIndex)
{
	if (!ch || !ch->IsPC())
		return false;

	auto pkMap = SECTREE_MANAGER::instance().GetMap(dwIndex);
	if (!pkMap)
	{
		sys_log(0, "Cannot find this map mIndex: %d", dwIndex);
		return false;
	}

	auto pItem = FindSet(dwIndex);
	if (!pItem)
		return true;

	return !((ch->GetLevel() < pItem->min_level) || (ch->GetLevel() > pItem->max_level));
}

const TeleportCollection::TeleportSet::Teleport* CTeleportManager::FindSet(const uint32_t id) const {
	for (const auto& set : sets_)
	{
		auto fitRow = std::find_if(set.second.GetTeleports().begin(), set.second.GetTeleports().end(), [&id](const TeleportCollection::TeleportSet::Teleport& rElement) {
			return (id == rElement.id);
			});

		if (fitRow != set.second.GetTeleports().end())
		{
			return &(*fitRow);
		}
	}

	return nullptr;
}
//CMDS

ACMD(do_teleport_open)
{
	if (!ch)
	{
		return;
	}

	CTeleportManager::instance().SendInformation(ch);
}

ACMD(do_teleport_action)
{
	if (!ch)
	{
		return;
	}

	char arg1[256];
	one_argument(argument, arg1, sizeof(arg1));

	if (!*arg1)
	{
		return;
	}

	DWORD num = 0;
	str_to_number(num, arg1);

	CTeleportManager::instance().Teleport(ch, num);
}
#endif
