#include "stdafx.h"
#ifdef __SHIP_DUNGEON_ENABLE__
#include <boost/tokenizer.hpp>
#include "char.h"
#include "dungeon.h"
#include "char_manager.h"
#include "desc_client.h"
#include "desc_manager.h"
#include "mob_manager.h"
#include "ShipDefendDungeon.hpp"

// For typing purpose
template <typename E>
static constexpr typename std::underlying_type<E>::type to_underlying(E e)
{
	return (static_cast<typename std::underlying_type<E>::type>(e) + 1);
}

namespace ShipDefendHelper
{
DWORD ALLIED_MAST_VNUM = 20434;
DWORD ENEMY_HYDRA_VNUM = 3960;
DWORD TREASURE_VNUM = 3965;
int FIRST_WAVE_COUNT = 4;
DWORD ENEMY_GUARD_GROUP_VNUM = 6501;
std::unordered_set<DWORD> s_enemy_guard;
int ENEMY_HYDRA_X = 385;
int ENEMY_HYDRA_Y = 376;
int ALLIED_MAST_X = 385;
int ALLIED_MAST_Y = 400;
int ALLIED_MAST_DIR = 1;
int TREASURE_X = 385;
int TREASURE_Y = 420;
int TREASURE_DIR = 1;
int ENEMY_INVERTED_X = ALLIED_MAST_X;
int ENEMY_INVERTED_Y = ALLIED_MAST_Y + 30;
time_t TIMEOUT = 60 * 30;
time_t EXIT_TIME = 60;
time_t LOGOUT_TIME = 10;

// Multilang support
#ifdef __MULTI_LANGUAGE_SYSTEM__
std::array<std::string, to_underlying(EShipDungeonStateEnum::END)> AShipDefenedNotice =
{
	"SHIP_DUNGEON_ENTRY",
	"SHIP_DUNGEON_WAVE_1",
	"SHIP_DUNGEON_WAVE_2",
	"SHIP_DUNGEON_WAVE_3",
	"SHIP_DUNGEON_WAVE_4",
	"SHIP_DUNGEON_HYDRA_1",
	"SHIP_DUNGEON_HYDRA_2",
	"SHIP_DUNGEON_HYDRA_3",
	"SHIP_DUNGEON_HYDRA_4",
	"SHIP_DUNGEON_END"
};
#else
std::array<std::string, to_underlying(EShipDungeonStateEnum::END)> AShipDefenedNotice =
{
	LC_TEXT("SHIP_DUNGEON_ENTRY"),
	LC_TEXT("SHIP_DUNGEON_WAVE_1"),
	LC_TEXT("SHIP_DUNGEON_WAVE_2"),
	LC_TEXT("SHIP_DUNGEON_WAVE_3"),
	LC_TEXT("SHIP_DUNGEON_WAVE_4"),
	LC_TEXT("SHIP_DUNGEON_HYDRA_1"),
	LC_TEXT("SHIP_DUNGEON_HYDRA_1"),
	LC_TEXT("SHIP_DUNGEON_HYDRA_1"),
	LC_TEXT("SHIP_DUNGEON_HYDRA_1"),
	LC_TEXT("SHIP_DUNGEON_TREASURE"),
	LC_TEXT("SHIP_DUNGEON_END"),
};
#endif

void Initialize()
{
	CMobGroup * group = CMobManager::instance().GetGroup(ENEMY_GUARD_GROUP_VNUM);
	if (!group)
	{
		sys_err("Error! Cannot load group for dungeon: %d!", ENEMY_GUARD_GROUP_VNUM);
		return;
	}

	for (const auto & dwMob : group->GetMemberVector())
	{
		s_enemy_guard.insert(dwMob);
		sys_log(0, "Dungeon mob has been loaded! Vnum: %d, Group: %d", dwMob, ENEMY_GUARD_GROUP_VNUM);
	}
}
}

CShipDefendDungeon::CShipDefendDungeon(const long & _dwMapIndex, LPDUNGEON _pDungeon) : dwMapIndex(_dwMapIndex), pDungeon(_pDungeon), iState(-1), bEnd(false)
{
	// Spawning mast first
	SpawnMonster(ShipDefendHelper::ALLIED_MAST_VNUM, ShipDefendHelper::ALLIED_MAST_X, ShipDefendHelper::ALLIED_MAST_Y, ShipDefendHelper::ALLIED_MAST_DIR);
}

CShipDefendDungeon::~CShipDefendDungeon()
{
	// Getting rid of useless events
	for (auto const & s_event : s_event_collector)
	{
		CEventFunctionHandler::instance().RemoveEvent(s_event);
	}
}

void CShipDefendDungeon::MoveInitialState()
{
	if (iState < 0)
	{
		// Moving to first state
		MoveState();

		// Setting timer
		RegisterEvent([this](SArgumentSupportImpl *)
		{
			std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this](LPDESC d)
			{
				if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
				{
					d->GetCharacter()->GoHome();
				}
			});
		}, ("SHIP_DEFEND_DUNGEON_TIMEOUT_" + std::to_string(dwMapIndex)), ShipDefendHelper::TIMEOUT);

#ifdef __MULTI_LANGUAGE_SYSTEM__
		BroadcastMessage("SHIP_DUNGEON_MESSAGE_ENTRY");
#else
		BroadcastMessage(LC_TEXT("SHIP_DUNGEON_MESSAGE_ENTRY"));
#endif
	}
}

void CShipDefendDungeon::RegisterHitRecord(LPCHARACTER pAttacker, LPCHARACTER pVictim)
{
	// Not started yet
	if (iState < 0)
	{
		return;
	}

	// Mast
	if (pVictim->GetRaceNum() == ShipDefendHelper::ALLIED_MAST_VNUM)
	{
		BroadcastAllieStatus(pVictim);
		if (pVictim->GetHP() <= 0) // Basically means it's dead
		{
			WipeAllMonsters();

#ifdef __MULTI_LANGUAGE_SYSTEM__
			BroadcastMessage("SHIP_DUNGEON_MESSAGE_FAIL");
#else
			BroadcastMessage(LC_TEXT("SHIP_DUNGEON_MESSAGE_FAIL"));
#endif

			bEnd = true;

			RegisterEvent([this](SArgumentSupportImpl *)
			{
				std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this](LPDESC d)
				{
					if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
					{
						d->GetCharacter()->GoHome();
					}
				});
			}, ("SHIP_DEFEND_DUNGEON_FAIL_" + std::to_string(dwMapIndex)), ShipDefendHelper::LOGOUT_TIME);
		}
	}
	// Hydra
	else if (pVictim->GetRaceNum() == ShipDefendHelper::ENEMY_HYDRA_VNUM)
	{
		if (FindFieldMonster(pVictim) && pVictim->GetHP() <= 0)
		{
			for (const auto & dwVnum : ShipDefendHelper::s_enemy_guard)
			{
				KillMonsterByVnum(dwVnum);
			}

			WipeMonster(pVictim);

			if (static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState + 1) == ShipDefendHelper::EShipDungeonStateEnum::TREASURE_BOX)
			{
				MoveState();
			}
			else
			{
#ifdef __MULTI_LANGUAGE_SYSTEM__
				BroadcastMessage("SHIP_DUNGEON_MESSAGE_SUCCESS");
#else
				BroadcastMessage(LC_TEXT("SHIP_DUNGEON_MESSAGE_SUCCESS"));
#endif

				RegisterMoveTimeEvent(ShipDefendHelper::LOGOUT_TIME);
			}
		}
	}
	// Reborn is beeing pull off only for hydra states
	else if (FindFieldMonster(pVictim) && pVictim->GetHP() <= 0 && static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState) >= ShipDefendHelper::EShipDungeonStateEnum::HYDRA_1 && static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState) <= ShipDefendHelper::EShipDungeonStateEnum::HYDRA_4)
	{
		SpawnMonster(pVictim->GetRaceNum(), GetAlliedMast());
		s_monster_list.erase(pVictim->GetVID());
	}
	// Kill the reward box
	else if ((pVictim->GetRaceNum() == ShipDefendHelper::TREASURE_VNUM))
	{
		if (pVictim->GetHP() <= 0)
		{
			if (static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState + 1) == ShipDefendHelper::EShipDungeonStateEnum::END)
			{
				MoveState();
			}
		}
	}
	// For first steps
	else
	{
		if (pVictim->GetHP() <= 0)
		{
			s_monster_list.erase(pVictim->GetVID());
		}

		if (!GetMonsterCountByGroup(ShipDefendHelper::s_enemy_guard))
		{
#ifdef __MULTI_LANGUAGE_SYSTEM__
			BroadcastMessage("SHIP_DUNGEON_MESSAGE_SUCCESS");
#else
			BroadcastMessage(LC_TEXT("SHIP_DUNGEON_MESSAGE_SUCCESS"));
#endif

			RegisterMoveTimeEvent(ShipDefendHelper::LOGOUT_TIME);
		}
	}
}

bool CShipDefendDungeon::FindFieldMonster(LPCHARACTER pMonster)
{
	return (s_monster_list.find(pMonster->GetVID()) != s_monster_list.end() && pMonster->GetMapIndex() == dwMapIndex);
}

LPCHARACTER CShipDefendDungeon::GetAlliedMast()
{
	for (auto const & vid : s_monster_list)
	{
		auto pMonster = CHARACTER_MANAGER::instance().Find(vid);
		if (pMonster && pMonster->GetMapIndex() == dwMapIndex && pMonster->GetRaceNum() == ShipDefendHelper::ALLIED_MAST_VNUM)
		{
			return pMonster;
		}
	}

	return nullptr;
}

void CShipDefendDungeon::WipeMonster(LPCHARACTER pMonster)
{
	pMonster->Dead();
	s_monster_list.erase(pMonster->GetVID());
}

void CShipDefendDungeon::KillMonsterByVnum(const DWORD & dwVnum)
{
	for (auto it = s_monster_list.begin(); it != s_monster_list.end();)
	{
		auto pMonster = CHARACTER_MANAGER::instance().Find(*it);
		if (pMonster && !pMonster->IsPC() && pMonster->GetMapIndex() == dwMapIndex && pMonster->GetRaceNum() == dwVnum)
		{
			pMonster->Dead();
			it = s_monster_list.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void CShipDefendDungeon::WipeAllMonsters()
{
	pDungeon->KillAll();
	s_monster_list.clear();
}

LPCHARACTER CShipDefendDungeon::SpawnMonster(const DWORD & dwVnum, const int & x, const int & y, const int & dir)
{
	LPCHARACTER ch = pDungeon->SpawnMob(dwVnum, x, y, dir);
	if (ch)
	{
		if (dwVnum == ShipDefendHelper::ENEMY_HYDRA_VNUM)
		{
			auto pMonster = GetAlliedMast();
			if (!pMonster)
			{
				sys_err("Internal error! Mast has not been spawn correctly!");
				return nullptr;
			}

			ch->BeginFight(pMonster);
			ch->UpdatePacket();
		}

		s_monster_list.insert(ch->GetVID());
		return ch;
	}

	return nullptr;
}

void CShipDefendDungeon::SpawnMonster(const DWORD & dwVnum, LPCHARACTER pVictim)
{
	LPCHARACTER ch = SpawnMonster(dwVnum,
								  static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState) >= ShipDefendHelper::EShipDungeonStateEnum::HYDRA_1 ? ShipDefendHelper::ENEMY_INVERTED_X : ShipDefendHelper::ENEMY_HYDRA_X,
								  static_cast<ShipDefendHelper::EShipDungeonStateEnum>(iState) >= ShipDefendHelper::EShipDungeonStateEnum::HYDRA_1 ? ShipDefendHelper::ENEMY_INVERTED_Y : ShipDefendHelper::ENEMY_HYDRA_Y);

	if (ch)
	{
		ch->BeginFight(pVictim);
		ch->UpdatePacket();
	}
}

void CShipDefendDungeon::MoveState()
{
	using namespace ShipDefendHelper;

	// Moving state forward
	iState++;

	switch (static_cast<EShipDungeonStateEnum>(iState))
	{
	// Waves
	case EShipDungeonStateEnum::STATE_INIT:
	case EShipDungeonStateEnum::WAVE_1:
	case EShipDungeonStateEnum::WAVE_2:
	case EShipDungeonStateEnum::WAVE_3:
	case EShipDungeonStateEnum::WAVE_4:
	{
		auto pMonster = GetAlliedMast();
		if (!pMonster)
		{
			sys_err("Internal error! Mast has not been spawn correctly!");
			return;
		}

		for (int i = 0; i < FIRST_WAVE_COUNT; i++)
		{
			for (const auto& dwVnum : s_enemy_guard)
			{
				SpawnMonster(dwVnum, pMonster);
			}
		}
	}
	break;

	// Hydra stage
	case EShipDungeonStateEnum::HYDRA_1:
	case EShipDungeonStateEnum::HYDRA_2:
	case EShipDungeonStateEnum::HYDRA_3:
	case EShipDungeonStateEnum::HYDRA_4:
	{
		auto pMonster = GetAlliedMast();
		if (!pMonster)
		{
			sys_err("Internal error! Mast has not been spawn correctly!");
			return;
		}

		SpawnMonster(ENEMY_HYDRA_VNUM, ENEMY_HYDRA_X, ENEMY_HYDRA_Y, ALLIED_MAST_DIR);

		for (int i = 0; i < FIRST_WAVE_COUNT; i++)
		{
			for (const auto& dwVnum : s_enemy_guard)
			{
				SpawnMonster(dwVnum, pMonster);
			}
		}
	}
	break;

	case EShipDungeonStateEnum::TREASURE_BOX:
	{
		// Erasing mast
		KillMonsterByVnum(ShipDefendHelper::ALLIED_MAST_VNUM);
		// Spawning treasure
		SpawnMonster(TREASURE_VNUM, TREASURE_X, TREASURE_Y, TREASURE_DIR);
	}
	break;

	// Farewell
	case EShipDungeonStateEnum::END:
		bEnd = true;
		CEventFunctionHandler::instance().RemoveEvent(("SHIP_DEFEND_DUNGEON_TIMEOUT_" + std::to_string(dwMapIndex)));
		RegisterEvent([this](SArgumentSupportImpl *)
		{
			std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this](LPDESC d)
			{
				pDungeon->ExitAll();
			});
		}, ("SHIP_DEFEND_DUNGEON_SUCCESS_" + std::to_string(dwMapIndex)), ShipDefendHelper::EXIT_TIME);
		break;
	default:
		return;
		break;
	}

	// Sending notice due to new level
	BroadcastMessage(std::move(AShipDefenedNotice[iState]));
}

// In perc.
void CShipDefendDungeon::BroadcastInitStatus(LPCHARACTER ch)
{
	if (iState == -1)
#ifdef __MULTI_LANGUAGE_SYSTEM__
		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("SHIP_DUNGEON_ENTRY_INFO"));
#else
		ch->ChatPacket(CHAT_TYPE_BIG_NOTICE, LC_TEXT("SHIP_DUNGEON_ENTRY_INFO"));
#endif

	ch->ChatPacket(CHAT_TYPE_COMMAND, "Ship_Defend_Dungeon_Open");
	if (GetAlliedMast())
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "Ship_Defend_Dungeon_Update %d", std::max((int) 0, int(float(GetAlliedMast()->GetHP()) / float(GetAlliedMast()->GetMaxHP()) * 100.0f)));
	}
}

void CShipDefendDungeon::BroadcastAllieStatus(LPCHARACTER pAllied)
{
	int iPerc = std::max((int) 0, int(float(pAllied->GetHP()) / float(pAllied->GetMaxHP()) * 100.0f));
	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &pAllied, &iPerc](LPDESC d)
	{
		if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
		{
			d->GetCharacter()->ChatPacket(CHAT_TYPE_COMMAND, "Ship_Defend_Dungeon_Update %d", iPerc);
		}
	});
}

void CShipDefendDungeon::BroadcastMessage(const std::string & s_message)
{
	//#ifdef __MULTI_LANGUAGE_SYSTEM__
	//	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &s_message](LPDESC d) {
	//			if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
	//			{
	//				boost::tokenizer<boost::char_separator<char>> lines{std::string(LC_TEXT(s_message.c_str(), d->GetCharacter())), boost::char_separator<char>{"|"}};
	//				for (auto const & line : lines)
	//					d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, line.c_str());}});
	//#else
	//	boost::tokenizer<boost::char_separator<char>> lines{s_message, boost::char_separator<char>{"|"}};
	//	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &s_message](LPDESC d) {
	//			if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
	//				for (auto const & line : lines)
	//					d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, line.c_str());});
	//#endif

	std::for_each(DESC_MANAGER::instance().GetClientSet().begin(), DESC_MANAGER::instance().GetClientSet().end(), [this, &s_message](LPDESC d)
	{
		if (d->GetCharacter() != NULL && d->GetCharacter()->IsPC() && d->GetCharacter()->GetMapIndex() == dwMapIndex)
		{
			d->GetCharacter()->ChatPacket(CHAT_TYPE_BIG_NOTICE, s_message.c_str());
		}
	});
}

void CShipDefendDungeon::RegisterMoveTimeEvent(const size_t & t_delay)
{
	std::string s_event_name = "SHIP_DEFEND_DUNGEON_" + std::to_string(dwMapIndex) + "_STATE_" + std::to_string(iState);
	CEventFunctionHandler::instance().AddEvent([this](SArgumentSupportImpl *) {this->MoveState();},
	s_event_name, t_delay);

	s_event_collector.insert(s_event_name);
}
#endif

