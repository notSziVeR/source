#include "stdafx.h"
#include "constants.h"
#include "fishing.h"
#include "locale_service.h"
#include "skill.h"
#include "item_manager.h"
#include "config.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "sectree_manager.h"
#include "char.h"
#include "char_manager.h"
#include "db.h"

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

BYTE MAX_ROD_LEVEL = 6;

namespace fishing
{
EVENTFUNC(fishing_event)
{
	fishing_event_info * info = dynamic_cast<fishing_event_info *>( event->info );

	if ( info == NULL )
	{
		sys_err( "fishing_event> <Factor> Null pointer" );
		return 0;
	}

	LPCHARACTER ch = CHARACTER_MANAGER::instance().FindByPID(info->pid);

	if (!ch)
	{
		return 0;
	}

	LPITEM rod = ch->GetWear(WEAR_WEAPON);

	if (!(rod && rod->GetType() == ITEM_ROD))
	{
		ch->m_pkFishingEvent = NULL;
		return 0;
	}

	switch (info->step)
	{
	case 0:
		++info->step;
		info->hang_time = get_dword_time();
		info->fish_id = 0;
		CFishing::instance().FishingReact(ch);

		ch->ChatPacket(CHAT_TYPE_INFO, "Kliknij %d razy spacjê, aby spróbowaæ wy³owiæ rybê.", info->hang_count);
		return (PASSES_PER_SEC(6));

	default:
		ch->m_pkFishingEvent = NULL;
		CFishing::instance().Take(info, ch);
		return 0;
	}
}

LPEVENT CreateFishingEvent(LPCHARACTER ch)
{
	fishing_event_info* info = AllocEventInfo<fishing_event_info>();
	info->pid	= ch->GetPlayerID();
	info->step	= 0;
	info->hang_time	= 0;
	info->hang_count = number(2, 5);

	int time = number(10, 40);

	TPacketGCFishing p;
	p.header	= HEADER_GC_FISHING;
	p.subheader	= FISHING_SUBHEADER_GC_START;
	p.info		= ch->GetVID();
	p.dir		= (BYTE)(ch->GetRotation() / 5);
	ch->PacketAround(&p, sizeof(TPacketGCFishing));

	return event_create(fishing_event, info, PASSES_PER_SEC(time));
}
}

CFishing::CFishing()
{
}

CFishing::~CFishing()
{
	fishing_conf.clear();
}

void CFishing::Reload()
{
	fishing_conf.clear();
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.fishing ORDER by id"));

	if (msg->Get()->uiNumRows == 0)
	{
		sys_err("ClientManagerBoot::InitializeFishing Could not load fishing. No result!");
		return;
	}

	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	fishing_info tmp_struct;

	MYSQL_ROW row;
	DWORD tmp_prob_max = 0;
	while ((row = mysql_fetch_row(pRes)))
	{
		if (row[1] && *row[1])
		{
			tmp_struct.vnum = strtoul(row[1], NULL, 10);
		}
		if (row[2] && *row[2])
		{
			tmp_struct.dead_vnum = strtoul(row[2], NULL, 10);
		}
		if (row[3] && *row[3])
		{
			tmp_struct.grilled_vnum = strtoul(row[3], NULL, 10);
		}

		if (row[4] && *row[4])
		{
			tmp_struct.prob_min = tmp_prob_max;
			tmp_prob_max += strtoul(row[4], NULL, 10);
			tmp_struct.prob_max = tmp_prob_max;
		}

		if (row[5] && *row[5])
		{
			tmp_struct.max_length = strtoul(row[5], NULL, 10);
		}

		fishing_conf.push_back(tmp_struct);
		if (test_server)
		{
			sys_log(0, "New_Fish_System: Fish vnum %d loaded.", tmp_struct.vnum);
		}
	}
	glob_max_prob = tmp_prob_max;
}

void CFishing::Initialize()
{
	std::unique_ptr<SQLMsg> msg(DBManager::instance().DirectQuery("SELECT * FROM player.fishing ORDER by id"));

	if (msg->Get()->uiNumRows == 0)
	{
		sys_err("ClientManagerBoot::InitializeFishing Could not load fishing. No result!");
		return;
	}

	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	fishing_info tmp_struct;

	MYSQL_ROW row;
	DWORD tmp_prob_max = 0;
	while ((row = mysql_fetch_row(pRes)))
	{
		if (row[1] && *row[1])
		{
			tmp_struct.vnum = strtoul(row[1], NULL, 10);
		}
		if (row[2] && *row[2])
		{
			tmp_struct.dead_vnum = strtoul(row[2], NULL, 10);
		}
		if (row[3] && *row[3])
		{
			tmp_struct.grilled_vnum = strtoul(row[3], NULL, 10);
		}

		if (row[4] && *row[4])
		{
			tmp_struct.prob_min = tmp_prob_max;
			tmp_prob_max += strtoul(row[4], NULL, 10);
			tmp_struct.prob_max = tmp_prob_max;
		}

		if (row[5] && *row[5])
		{
			tmp_struct.max_length = strtoul(row[5], NULL, 10);
		}

		fishing_conf.push_back(tmp_struct);
		if (test_server)
		{
			sys_log(0, "New_Fish_System: Fish vnum %d loaded.", tmp_struct.vnum);
		}
	}
	glob_max_prob = tmp_prob_max;
}

void CFishing::FishingSuccess(LPCHARACTER ch)
{
	TPacketGCFishing p;
	p.header = HEADER_GC_FISHING;
	p.subheader = FISHING_SUBHEADER_GC_SUCCESS;
	p.info = ch->GetVID();
	ch->PacketAround(&p, sizeof(p));
}

void CFishing::FishingFail(LPCHARACTER ch)
{
	TPacketGCFishing p;
	p.header = HEADER_GC_FISHING;
	p.subheader = FISHING_SUBHEADER_GC_FAIL;
	p.info = ch->GetVID();
	ch->PacketAround(&p, sizeof(p));
}

void CFishing::FishingReact(LPCHARACTER ch)
{
	TPacketGCFishing p;
	p.header = HEADER_GC_FISHING;
	p.subheader = FISHING_SUBHEADER_GC_REACT;
	p.info = ch->GetVID();
	ch->PacketAround(&p, sizeof(p));
}

BYTE CFishing::GetFishingChance(LPCHARACTER ch)
{
	BYTE base_chance = 25; // tak chcieliscie
	base_chance += GetRodBonus(ch);
	if (ch->GetSkillLevel(123) >= 40)
	{
		base_chance += 10;
	}
	if (ch->GetSkillLevel(123) >= 30)
	{
		base_chance += 10;
	}
	if (ch->GetSkillLevel(123) >= 20)
	{
		base_chance += 10;
	}

	base_chance += ch->GetPremiumRemainSeconds(PREMIUM_FISH_MIND) ? 25 : 0;

	if (test_server)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Szansa na wy³owienie: %d%%", base_chance);
	}
	return base_chance;
}

BYTE CFishing::GetRodBonus(LPCHARACTER ch)
{
	LPITEM rod = ch->GetWear(WEAR_WEAPON);

	if (!rod)
	{
		return 0;
	}

	if (rod->GetType() != ITEM_ROD)
	{
		return 0;
	}

	BYTE rod_type = rod->GetVnum() - 27400;
	return rod_type * 5;
}

BYTE CFishing::GetRodLevel(DWORD rod_level)
{
	return (rod_level - 27400);
}

DWORD CFishing::RandomFish()
{
	DWORD los = number(0, glob_max_prob);
	for (std::vector<fishing_info>::iterator it = fishing_conf.begin(); it != fishing_conf.end(); it++)
	{
		if (los > it->prob_min && los <= it->prob_max)
		{
			return it->vnum;
		}
	}
	return 0;
}

void CFishing::FishingPractise(LPCHARACTER ch)
{
	LPITEM rod = ch->GetWear(WEAR_WEAPON);

	if (!rod)
	{
		return;
	}

	if (rod->GetType() != ITEM_ROD)
	{
		return;
	}

	static int FishingUpgrade[6] = {10, 20, 40, 80, 120};

	// sec
	BYTE level = GetRodLevel(rod->GetVnum());
	if (level >= MAX_ROD_LEVEL)
	{
		return;
	}

	if (rod->GetSocket(1) > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Twoja wêdka osi¹gnê³a maksimum punktów na tym poziomie! Udaj siê do Rybaka aby j¹ ulepszyæ.");
		return;
	}

	rod->SetSocket(0, rod->GetSocket(0) + 1);
	ch->ChatPacket(CHAT_TYPE_INFO, "Twoja wêdka zdoby³a 1 punkt!");

	if (rod->GetSocket(0) >= FishingUpgrade[level])
	{
		rod->SetSocket(1, 1);
		ch->ChatPacket(CHAT_TYPE_INFO, "Twoja wêdka osi¹gnê³a maksymaln¹ liczbê punktów na tym poziomie!");
		ch->ChatPacket(CHAT_TYPE_INFO, "Udaj siê do Rybaka aby j¹ ulepszyæ");
	}
}

void CFishing::RemoveBait(LPCHARACTER ch)
{
	LPITEM rod = ch->GetWear(WEAR_WEAPON);

	if (!rod)
	{
		return;
	}

	if (rod->GetType() != ITEM_ROD)
	{
		return;
	}

	rod->SetSocket(2, 0);
	ch->ChatPacket(CHAT_TYPE_INFO, "Przynêta zosta³a zu¿yta.");
}

DWORD CFishing::GetFishMaxLenght(LPITEM item)
{
	for (std::vector<fishing_info>::iterator it = fishing_conf.begin(); it != fishing_conf.end(); it++)
	{
		if (it->vnum == item->GetVnum())
		{
			return it->max_length;
		}
	}
	return 0;
}

DWORD CFishing::FishingSetLenght(LPITEM item, LPCHARACTER ch)
{
	DWORD length = number(1, GetFishMaxLenght(item));
	item->SetSocket(2, length);
	return length;
}

void CFishing::Take(fishing::fishing_event_info* info, LPCHARACTER ch)
{
	RemoveBait(ch);

	if (info->hang_count != 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Klikn¹³eœ spacjê nieodpowiedni¹ iloœæ razy. Po³ów nieudany.");
		FishingFail(ch);
		return;
	}

	if (GetFishingChance(ch) < number(1, 100))
	{
		ch->ChatPacket(CHAT_TYPE_INFO, "Niestety, po³ów nieudany!");
		FishingFail(ch);
		return;
	}

	if (info->step == 1)
	{
		long ms = (long) ((get_dword_time() - info->hang_time));
		if (ms > 6000)
		{
			FishingFail(ch);
			return;
		}
		DWORD item_vnum = RandomFish();
		if (item_vnum)
		{

			LPITEM item = ch->AutoGiveItem(item_vnum, 1, -1, false);
			FishingPractise(ch);

			DWORD length = 0;
			if (item)
			{
				length = FishingSetLenght(item, ch);
			}

#ifdef __BATTLE_PASS_ENABLE__
			if (item)
				CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{BattlePassNS::EObjectives::TYPE_FISHING, item->GetVnum(), 1});
			CBattlePassManager::instance().TriggerEvent(ch, BattlePassNS::SObjective{BattlePassNS::EObjectives::TYPE_FISHING_ALL, 0, 1});
#endif

			FishingSuccess(ch);

			TPacketGCFishing p;
			p.header = HEADER_GC_FISHING;
			p.subheader = FISHING_SUBHEADER_GC_FISH;
			p.info = item_vnum;
			p.length = length;
			ch->GetDesc()->Packet(&p, sizeof(TPacketGCFishing));
		}
		else
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "B³¹d serwera! Skontaktuj siê z administracj¹!");
		}
	}
	else if (info->step > 1)
	{
		FishingFail(ch);
	}
	else
	{
		TPacketGCFishing p;
		p.header = HEADER_GC_FISHING;
		p.subheader = FISHING_SUBHEADER_GC_STOP;
		p.info = ch->GetVID();
		ch->PacketAround(&p, sizeof(p));
	}
}

void CFishing::UseFish(LPCHARACTER ch, LPITEM item)
{
	CountType count = item->GetCount();
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("%s¸¦ ±¸¿ü½À´Ï´Ùgsgswegewge."), item->GetName());
	ch->AutoGiveItem(GetDeadVnum(item), count);
	item->SetCount(0);
}

DWORD CFishing::GetGrilledVnum(LPITEM item)
{
	for (std::vector<fishing_info>::iterator it = fishing_conf.begin(); it != fishing_conf.end(); it++)
	{
		if (it->vnum == item->GetVnum())
		{
			return it->grilled_vnum;
		}
	}
	return 0;
}

DWORD CFishing::GetDeadVnum(LPITEM item)
{
	for (std::vector<fishing_info>::iterator it = fishing_conf.begin(); it != fishing_conf.end(); it++)
	{
		if (it->vnum == item->GetVnum())
		{
			return it->dead_vnum;
		}
	}
	return 0;
}

void CFishing::Grill(LPCHARACTER ch, LPITEM item)
{
	CountType count = item->GetCount();
	ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You are roasting %s over the fire."), item->GetName());
	ch->AutoGiveItem(GetGrilledVnum(item), count);
	item->SetCount(0);
}

