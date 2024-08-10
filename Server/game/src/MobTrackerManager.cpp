#include "stdafx.h"
#ifdef __ENABLE_MOB_TRAKCER__
#include "char.h"
#include "utils.h"
#include "questmanager.h"
#include "config.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "sectree_manager.h"
#include "MobTrackerManager.hpp"

namespace
{
	const std::string sBroadcastClearInterface("MobTracker_ClearInterface");
	const std::string sBroadcastRegisterTracker("MobTracker_RegisterInfo {} {} {} {} {} {}");
	const std::string sBroadcastOpenInterface("MobTracker_OpenInterface");
}

MobTrackerCollection::TrackerSet::TrackerSet(uint32_t id, tracker_collection_t trackers)
	: id_(std::move(id))
	, trackers_(std::move(trackers))
{}

void MobTrackerManager::Create()
{
	Load("locale/germany/tracker-manager.xml");
}

void MobTrackerManager::Clear()
{
	sets_.clear();
}

void MobTrackerManager::Destroy()
{
	//Clearing data..
	Clear();
}

bool MobTrackerManager::Load(const std::string& filename)
{
	MobTrackerCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	//auto sets = std::move(parser.TrackerSets());
	//for (auto& set : sets)
	//	sets_.emplace(set.GetId(), std::move(set));

	return true;
}

size_t MobTrackerManager::RegisterInstance(const uint8_t id, const uint32_t vnum, const uint16_t x, const uint16_t y, const time_t time)
{
	auto fit = sets_.find(id);
	// That's mean we gonna put just new instance to vector!
	if (fit != sets_.end())
	{
		fit->second.push_back(MobTrackerInstance{ static_cast<uint8_t>(fit->second.size() + 1), vnum, x, y, time});
		sys_log(0, fmt::format("##MobTrackerManager::CreateNewInstance#2! ID: {}, Mob: {} Size: {}", id, vnum, fit->second.size()).c_str());
		return fit->second.size();
	}

	sets_[id].push_back(MobTrackerInstance{ 1, vnum, x, y, time });

	sys_log(0, fmt::format("##MobTrackerManager::CreateNewInstance#1! ID: {}, Mob: {} Size: {}", id, vnum, 1).c_str());
	return 1;
}

void MobTrackerManager::ApplyDelay(const uint32_t map_id, const size_t id)
{
	auto fInstance = GetInstance(map_id, id);
	if (!fInstance) return;

	fInstance->AppendDelay(get_global_time() + fInstance->time);

	// Sending an update to everyone on this map!
	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &map_id, &id](LPDESC d)
	{
		if (d->GetCharacter() && d->GetCharacter()->GetMapIndex() == map_id)
		{
			Broadcast(d->GetCharacter(), false, id);
		}
	});
}

void MobTrackerManager::Reload()
{
	// Clearing old data
	Clear();

	// Loading new data again
	Create();
}

void MobTrackerManager::Broadcast(LPCHARACTER ch, bool bClear, const int iNum)
{
	if (!ch) return;

	auto set = FindSet(ch->GetMapIndex());
	if (!set) return;

	if (bClear)
		ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastClearInterface);

	if (iNum > -1)
	{
		auto instance = GetInstance(ch->GetMapIndex(), iNum);
		if (!instance) return;

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRegisterTracker, instance->vnum, instance->id, instance->time, instance->delay - get_global_time(), instance->x, instance->y));
		return;
	}

	for (const auto& rElement : *set)
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format(sBroadcastRegisterTracker, rElement.vnum, rElement.id, rElement.time, rElement.delay - get_global_time(), rElement.x, rElement.y));
	}

	ch->ChatPacket(CHAT_TYPE_COMMAND, sBroadcastOpenInterface);
}

void MobTrackerManager::RequestTeleport(LPCHARACTER ch, const uint8_t id)
{
	if (!ch) return;

	auto instance = GetInstance(ch->GetMapIndex(), id);
	if (!instance) return;

	PIXEL_POSITION p;

	uint16_t x = instance->x / 100;
	uint16_t y = instance->y / 100;

	if (SECTREE_MANAGER::instance().GetMapBasePosition(ch->GetX(), ch->GetY(), p))
	{
		x += p.x / 100;
		y += p.y / 100;
	}

	ch->Show(ch->GetMapIndex(), x * 100, y * 100, 0);
	ch->Stop();
}

const std::vector<MobTrackerManager::MobTrackerInstance>* MobTrackerManager::FindSet(const uint32_t id) const
{
	auto set = sets_.find(id);
	if (set == sets_.end())
		return nullptr;

	return &(set->second);
}

MobTrackerManager::MobTrackerInstance* MobTrackerManager::GetInstance(const uint32_t map_id, const uint32_t id)
{
	auto set = sets_.find(map_id);
	if (set == sets_.end()) return nullptr;

	auto fItem = std::find_if(set->second.begin(), set->second.end(), [&id](MobTrackerManager::MobTrackerInstance& rItem) { return (rItem.GetID() == id); });
	return (fItem != set->second.end()) ? &(*fItem) : nullptr;
}
#endif
