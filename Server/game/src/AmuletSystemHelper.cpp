#include "stdafx.h"
#ifdef __ENABLE_AMULET_SYSTEM__
#include "char.h"
#include "item.h"
#include "item_manager.h"
#include "skill.h"
#include "AmuletSystemHelper.hpp"

namespace AmuletSystemHelper
{
	TAmuletMapType m_amulet_combine_map;

	// Configuration of applys and refine
	std::unordered_map<enum EAmuletSubTypes, std::vector<std::vector<sUpgradeInfo>>> m_AmuletUpgradeConfiguration =
	{
		{ AMULET_NORMAL, {
			{ { 27102, 3 }, {27105, 1} },
			{ { 27105, 4 } },
			{ { 28030, 1 } },
			{ { 28035, 1 } },
		}}
	};

	std::unordered_map<enum EAmuletSubTypes, std::unordered_map<BYTE, std::vector<std::vector<long>>>> m_AmuletConfiguration =
	{
		{ AMULET_NORMAL,
			{
				{ APPLY_MAX_HP,
					{
						{ 100, 200, 300 },
						{ 400, 500, 600 },
						{ 700, 800, 900 },
						{ 1000, 1100, 1200 },
						{ 1300, 1400, 1500 },
					}},
				{ APPLY_MAX_SP,
					{
						{ 50, 100, 150 },
						{ 200, 250, 300 },
						{ 350, 400, 450 },
						{ 500, 550, 600 },
						{ 650, 700, 750 },
					}},
				{ APPLY_HP_REGEN,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_SP_REGEN,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_MANA_BURN_PCT,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATT_SPEED,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_MOV_SPEED,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_SLOW_PCT,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_STUN_PCT,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_CRITICAL_PCT,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_PENETRATE_PCT,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ITEM_DROP_BONUS,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_EXP_DOUBLE_BONUS,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_DEF_GRADE,
					{
						{ 4, 8, 12 },
						{ 16, 20, 24 },
						{ 28, 32, 36 },
						{ 40, 44, 48 },
						{ 52, 56, 60 },
					}},
				{ APPLY_MAGIC_DEF_GRADE,
					{
						{ 2, 4, 6 },
						{ 8, 10, 12 },
						{ 14, 16, 18 },
						{ 20, 22, 24 },
						{ 26, 28, 30 },
					}},
				{ APPLY_REFLECT_MELEE,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_POISON_REDUCE,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_ORC,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_MILGYO,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_ANIMAL,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_MONSTER,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_HUMAN,
					{
						{ 1, 2, 3 },
						{ 4, 5, 6 },
						{ 7, 8, 9 },
						{ 10, 11, 12 },
						{ 13, 14, 15 },
					}},
				{ APPLY_CON,
					{
						{ 1, 1, 2 },
						{ 2, 2, 3 },
						{ 3, 3, 4 },
						{ 5, 6, 7 },
						{ 8, 9, 10 },
					}},
				{ APPLY_INT,
					{
						{ 1, 1, 2 },
						{ 2, 2, 3 },
						{ 3, 3, 4 },
						{ 5, 6, 7 },
						{ 8, 9, 10 },
					}},
				{ APPLY_STR,
					{
						{ 1, 1, 2 },
						{ 2, 2, 3 },
						{ 3, 3, 4 },
						{ 5, 6, 7 },
						{ 8, 9, 10 },
					}},
				{ APPLY_DEX,
					{
						{ 1, 1, 2 },
						{ 2, 2, 3 },
						{ 3, 3, 4 },
						{ 5, 6, 7 },
						{ 8, 9, 10 },
					}},
				{ APPLY_ATT_GRADE_BONUS,
					{
						{ 4, 8, 12 },
						{ 16, 20, 24 },
						{ 28, 32, 36 },
						{ 40, 44, 48 },
						{ 52, 56, 60 },
					}},
				{ APPLY_MAGIC_ATT_GRADE,
					{
						{ 4, 8, 12 },
						{ 16, 20, 24 },
						{ 28, 32, 36 },
						{ 40, 44, 48 },
						{ 52, 56, 60 },
					}},
				{ APPLY_ATTBONUS_METIN,
					{
						{ 1, 1, 2 },
						{ 2, 2, 3 },
						{ 3, 3, 4 },
						{ 5, 6, 7 },
						{ 8, 9, 10 },
					}},
			},
		},

		{ AMULET_RARE,
			{
				{ APPLY_MAX_HP,
					{
						{ 100, 200, 300, 400 },
						{ 500, 600, 700, 800 },
						{ 900, 1000, 1100, 1200 },
						{ 1300, 1400, 1500, 1600 },
						{ 1700, 1800, 1900, 200 },
					}},
				{ APPLY_MAX_SP,
					{
						{ 50, 100, 150, 200 },
						{ 250, 300, 350, 400 },
						{ 450, 500, 550, 600 },
						{ 650, 700, 750, 800 },
						{ 850, 900, 950, 1000 },
					}},
				{ APPLY_HP_REGEN,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_SP_REGEN,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_MANA_BURN_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATT_SPEED,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_MOV_SPEED,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_SLOW_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_STUN_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_CRITICAL_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_PENETRATE_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ITEM_DROP_BONUS,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_EXP_DOUBLE_BONUS,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_DEF_GRADE,
					{
						{ 4, 8, 12, 16 },
						{ 20, 24, 28, 32 },
						{ 36, 40, 44, 48 },
						{ 52, 56, 60, 64 },
						{ 68, 72, 76, 80 },
					}},
				{ APPLY_MAGIC_DEF_GRADE,
					{
						{ 2, 4, 6, 8 },
						{ 10, 12, 14, 16 },
						{ 18, 20, 22, 24 },
						{ 26, 28, 30, 32 },
						{ 34, 36, 38, 40 },
					}},
				{ APPLY_REFLECT_MELEE,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_POISON_REDUCE,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_ORC,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_MILGYO,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_ANIMAL,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_MONSTER,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_HUMAN,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_CON,
					{
						{ 1, 2, 2, 3 },
						{ 3, 4, 4, 5 },
						{ 5, 6, 6, 7 },
						{ 8, 9, 10, 11 },
						{ 12, 13, 14, 15 },
					}},
				{ APPLY_INT,
					{
						{ 1, 2, 2, 3 },
						{ 3, 4, 4, 5 },
						{ 5, 6, 6, 7 },
						{ 8, 9, 10, 11 },
						{ 12, 13, 14, 15 },
					}},
				{ APPLY_STR,
					{
						{ 1, 2, 2, 3 },
						{ 3, 4, 4, 5 },
						{ 5, 6, 6, 7 },
						{ 8, 9, 10, 11 },
						{ 12, 13, 14, 15 },
					}},
				{ APPLY_DEX,
					{
						{ 1, 2, 2, 3 },
						{ 3, 4, 4, 5 },
						{ 5, 6, 6, 7 },
						{ 8, 9, 10, 11 },
						{ 12, 13, 14, 15 },
					}},
				{ APPLY_ATT_GRADE_BONUS,
					{
						{ 4, 8, 12, 16 },
						{ 20, 24, 28, 32 },
						{ 36, 40, 44, 48 },
						{ 52, 56, 60, 64 },
						{ 68, 72, 76, 80 },
					}},
				{ APPLY_MAGIC_ATT_GRADE,
					{
						{ 4, 8, 12, 16 },
						{ 20, 24, 28, 32 },
						{ 36, 40, 44, 48 },
						{ 52, 56, 60, 64 },
						{ 68, 72, 76, 80 },
					}},
				{ APPLY_ATTBONUS_METIN,
					{
						{ 1, 2, 2, 3 },
						{ 3, 4, 4, 5 },
						{ 5, 6, 6, 7 },
						{ 8, 9, 10, 11 },
						{ 12, 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_UNDEAD,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_DEVIL,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
				{ APPLY_RESIST_ALL,
					{
						{ 1, 1, 1, 2 },
						{ 2, 2, 2, 3 },
						{ 3, 4, 4, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
					}},
				{ APPLY_ANTI_PENETRATE_PCT,
					{
						{ 1, 2, 3, 4 },
						{ 5, 6, 7, 8 },
						{ 9, 10, 11, 12 },
						{ 13, 14, 15, 16 },
						{ 17, 18, 19, 20 },
					}},
			},
		},

		{ AMULET_LEGENDARY,
			{
				{ APPLY_MAX_HP,
					{
						{ 100, 200, 300, 400, 500 },
						{ 600, 700, 800, 900, 1000 },
						{ 1100, 1200, 1300, 1400, 1500 },
						{ 1600, 1700, 1800, 1900, 2000 },
						{ 2100, 2200, 2300, 2400, 2500 },
					}},
				{ APPLY_MAX_SP,
					{
						{ 50, 100, 150, 200, 250 },
						{ 300, 350, 400, 450, 500 },
						{ 550, 600, 650, 700, 750 },
						{ 800, 850, 900, 950, 1000 },
						{ 1050, 1100, 1150, 1200, 1250 },
					}},
				{ APPLY_HP_REGEN,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_SP_REGEN,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_MANA_BURN_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATT_SPEED,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_MOV_SPEED,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_SLOW_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_STUN_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_CRITICAL_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_PENETRATE_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ITEM_DROP_BONUS,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_EXP_DOUBLE_BONUS,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_DEF_GRADE,
					{
						{ 4, 8, 12, 16, 20 },
						{ 24, 28, 32, 36, 40 },
						{ 44, 48, 52, 56, 60 },
						{ 64, 68, 72, 76, 80 },
						{ 84, 88, 92, 96, 100 },
					}},
				{ APPLY_MAGIC_DEF_GRADE,
					{
						{ 2, 4, 6, 8, 10 },
						{ 12, 14, 16, 18, 20 },
						{ 22, 24, 26, 28, 30 },
						{ 32, 34, 36, 38, 40 },
						{ 42, 44, 46, 48, 50 },
					}},
				{ APPLY_REFLECT_MELEE,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_POISON_REDUCE,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_ORC,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_MILGYO,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_ANIMAL,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_MONSTER,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_HUMAN,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_CON,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_INT,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_STR,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_DEX,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_ATT_GRADE_BONUS,
					{
						{ 4, 8, 12, 16, 20 },
						{ 24, 28, 32, 36, 40 },
						{ 44, 48, 52, 56, 60 },
						{ 64, 68, 72, 76, 80 },
						{ 84, 88, 92, 96, 100 },
					}},
				{ APPLY_MAGIC_ATT_GRADE,
					{
						{ 4, 8, 12, 16, 20 },
						{ 24, 28, 32, 36, 40 },
						{ 44, 48, 52, 56, 60 },
						{ 64, 68, 72, 76, 80 },
						{ 84, 88, 92, 96, 100 },
					}},
				{ APPLY_ATTBONUS_METIN,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_ATTBONUS_UNDEAD,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ATTBONUS_DEVIL,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_RESIST_ALL,
					{
						{ 1, 1, 1, 1, 2 },
						{ 2, 2, 3, 3, 4 },
						{ 5, 5, 6, 6, 7 },
						{ 8, 8, 9, 9, 10 },
						{ 11, 12, 13, 14, 15 },
					}},
				{ APPLY_ANTI_PENETRATE_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_ANTI_CRITICAL_PCT,
					{
						{ 1, 2, 3, 4, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
						{ 21, 22, 23, 24, 25 },
					}},
				{ APPLY_SKILL_DAMAGE_BONUS,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
				{ APPLY_RESIST_BOSS,
					{
						{ 1, 1, 1, 1, 2 },
						{ 2, 2, 3, 3, 4 },
						{ 5, 5, 6, 6, 7 },
						{ 8, 8, 9, 9, 10 },
						{ 11, 12, 13, 14, 15 },
					}},
				{ APPLY_ATTBONUS_BOSS,
					{
						{ 1, 1, 2, 2, 3 },
						{ 3, 4, 4, 5, 5 },
						{ 6, 7, 8, 9, 10 },
						{ 11, 12, 13, 14, 15 },
						{ 16, 17, 18, 19, 20 },
					}},
			},
		},
	};

	std::map<BYTE, std::array<BYTE, 4>> m_AmuletPercentage = {
		{	1,	{   8,	0,	0,	0   }},
		{	2,	{   16,	0,	0,	0   }},
		{	3,	{   24,	0,	0,	0   }},
		{	4,	{   32,	0,	0,	0   }},
		{	5,	{   40,	0,	0,	0   }},
		{	6,	{   49,	0,	0,	0   }},
		{	7,	{   58,	0,	0,	0   }},
		{	8,	{   67,	0,	0,	0   }},
		{	9,	{   76,	0,	0,	0   }},
		{	10,	{   85,	0,	0,	0   }},
		{	11,	{   85,	6,	0,	0   }},
		{	12,	{   85,	12,	0,	0   }},
		{	13,	{   85,	18,	0,	0   }},
		{	14,	{   85,	24,	0,	0   }},
		{	15,	{   85,	30,	0,	0   }},
		{	16,	{   85,	37,	0,	0   }},
		{	17,	{   85,	44,	0,	0   }},
		{	18,	{   85,	51,	0,	0   }},
		{	19,	{   85,	58,	0,	0   }},
		{	20,	{   85,	65,	0,	0   }},
		{	21,	{   85,	65,	5,	0   }},
		{	22,	{   85,	65,	10,	0   }},
		{	23,	{   85,	65,	15,	0   }},
		{	24,	{   85,	65,	20,	0   }},
		{	25,	{   85,	65,	25,	0   }},
		{	26,	{   85,	65,	30,	0   }},
		{	27,	{   85,	65,	35,	0   }},
		{	28,	{   85,	65,	40,	0   }},
		{	29,	{   85,	65,	45,	0   }},
		{	30,	{   85,	65,	50,	0   }},
		{	31,	{   85,	65,	50,	4   }},
		{	32,	{   85,	65,	50,	8   }},
		{	33,	{   85,	65,	50,	12  }},
		{	34,	{   85,	65,	50,	16  }},
		{	35,	{   85,	65,	50,	20  }},
		{	36,	{   85,	65,	50,	24  }},
		{	37,	{   85,	65,	50,	28  }},
		{	38,	{   85,	65,	50,	32  }},
		{	39,	{   85,	65,	50,	36  }},
		{	40,	{   85,	65,	50,	40  }},
	};

	/// Global Functions ///
	int GetAmuletFreeSlot(LPCHARACTER ch)
	{
		int iSlot = -1;
		auto it = m_amulet_combine_map.find(ch->GetPlayerID());
		if (it == m_amulet_combine_map.end())
			return iSlot;

		return (it->second).GetFreeSlot();
	}

	int GetAmuletType(LPITEM item)
	{
		if (!item)
			return -1;

		return item->GetValue(AMULET_TYPE_VALUE);
	}
	/// End ///

	// Private Functions
	BYTE GetRandomApply(CItem* item)
	{
		auto fIt = m_AmuletConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fIt != m_AmuletConfiguration.end())
		{
			auto rApply = *Random::get(fIt->second);

			return rApply.first;
		}

		return 0;
	}

	short GetApplyValue(CItem* item, BYTE iApply)
	{
		auto fIt = m_AmuletConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fIt != m_AmuletConfiguration.end())
		{
			auto fApply = (fIt->second).find(item->GetAttributeType(iApply));
			if (fApply != (fIt->second).end())
			{
				if ((fApply->second).size() >= item->GetValue(AMULET_TYPE_VALUE))
				{
					auto vApplyValues = (fApply->second).at(std::min<int>(item->GetValue(AMULET_TYPE_VALUE), AMULET_MAX_POSSIBLE_REFINE_VALUE));
					if (vApplyValues.size() >= item->GetSocket(iApply))
					{
						return std::max<short>(vApplyValues.at(item->GetSocket(iApply)), 0);
					}
				}
			}
		}

		return 0;
	}

	int GetRandomUpIndex(CItem* item)
	{
		auto fIt = m_AmuletUpgradeConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fIt != m_AmuletUpgradeConfiguration.end())
		{
			auto rUpgrade = Random::get<int>(0, fIt->second.size() - 1);

			return rUpgrade;
		}

		return -1;
	}

	bool HaveRequiredItem(LPCHARACTER ch, CItem* item)
	{
		auto fRS = m_AmuletUpgradeConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fRS != m_AmuletUpgradeConfiguration.end())
		{
			size_t s_vRS = fRS->second.size();

			long currUpIndex = item->GetSocket(AMULET_UPGRADE_SOCKET);
			if (currUpIndex != -1)
			{
				if (currUpIndex >= s_vRS)
				{
					currUpIndex = GetRandomUpIndex(item);
				}

				const auto fIt = fRS->second.at(currUpIndex);
				if (fIt.size())
				{
					for (const auto& [dwVnum, iCount] : fIt)
					{
						if (ch->CountSpecifyItem(dwVnum) < iCount)
						{
							return false;
						}
					}
				}

				return true;
			}

			const auto v_randomRS = Random::get<int>(0, s_vRS - 1);

			auto v_Upgrade = fRS->second.at(v_randomRS);

			for (const auto& [dwVnum, iCount] : v_Upgrade)
			{
				if (ch->CountSpecifyItem(dwVnum) < iCount)
				{
					return false;
				}
			}

			item->SetSocket(AMULET_UPGRADE_SOCKET, v_randomRS);
		}

		return true;
	}

	bool CanAmuletUpgrade(LPCHARACTER ch, LPITEM item, int iApplyIndex)
	{
		if (!ch || !ch->GetDesc()) return false;
		if (!item) return false;

		// Lets check the current value of bonus
		long iCurrentGrade = item->GetSocket(iApplyIndex);

		// In case if we have max refined bonus!
		if (iCurrentGrade == item->GetValue(AMULET_MAX_GRADE_VALUE))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_GRADE_MAX_FAILED");
			return false;
		}

		// Lets check if we have enough gold to refine
		int64_t rGold = item->GetValue(AMULET_GOLD_VALUE) * std::max<int>(1, item->GetValue(AMULET_TYPE_VALUE));

		if (ch->GetGold() < rGold)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_NOT_ENOUGH_GOLD");
			return false;
		}

		if (!HaveRequiredItem(ch, item))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_NOT_ENOUGH_ITEMS");
			return false;
		}

		return true;
	}

	bool UpgradeClearUp(LPCHARACTER ch, LPITEM item, int iApplyIndex)
	{
		auto fRS = m_AmuletUpgradeConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fRS != m_AmuletUpgradeConfiguration.end())
		{
			size_t s_vRS = fRS->second.size();

			long currUpIndex = item->GetSocket(AMULET_UPGRADE_SOCKET);
			if (currUpIndex != -1)
			{
				if (currUpIndex >= s_vRS)
				{
					return false;
				}

				const auto fIt = fRS->second.at(currUpIndex);
				if (fIt.size())
				{
					for (const auto& [dwVnum, iCount] : fIt)
					{
						ch->RemoveSpecifyItem(dwVnum, iCount);
					}
				}

				int64_t rGold = item->GetValue(AMULET_GOLD_VALUE) * std::max<int>(1, item->GetValue(AMULET_TYPE_VALUE));
				ch->PointChange(POINT_GOLD, -rGold);
				return true;
			}
		}
		
		return false;
	}

	BYTE GetPercentage(LPCHARACTER ch, LPITEM item)
	{
		auto fIt = m_AmuletPercentage.find(ch->GetSkillLevel(SKILL_PASSIVE_CRAFT_AMULETS));
		if (fIt != m_AmuletPercentage.end())
		{
			return fIt->second.at(std::min<BYTE>(item->GetValue(AMULET_TYPE_VALUE), AMULET_MAX_POSSIBLE_REFINE_VALUE - 1));
		}

		return 0;
	}

	//Public Functions
	void OnCreate(CItem* item)
	{
		// Lets set upgrade index socket
		item->SetSocket(AMULET_UPGRADE_SOCKET, GetRandomUpIndex(item));

		auto fIt = m_AmuletConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fIt != m_AmuletConfiguration.end())
		{
			for (int i = 0; i < (AMULET_BASE_BONUS_COUNT + std::min<int>(item->GetValue(AMULET_TYPE_VALUE), AMULET_ADDITIONAL_BONUS_COUNT)); ++i)
			{
				// We doesn't wanna change bonus after refine!
				if (item->GetAttributeType(i))
				{
					continue;
				}

				auto rApply = GetRandomApply(item);

				while (item->HasAttr(rApply)) {
					rApply = GetRandomApply(item);
				}

				item->SetForceAttribute(i, rApply, 0);

				// We have to set socket!
				item->SetSocket(std::min<int>(i, AMULET_TOTAL_BONUS_COUNT), -1);
			}
		}
	}

	void AmuletUpgradeInfo(LPCHARACTER ch, CItem* item)
	{
		if (!ch || !ch->IsPC()) return;

		auto fRS = m_AmuletUpgradeConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fRS != m_AmuletUpgradeConfiguration.end())
		{
			size_t s_vRS = fRS->second.size();

			long currUpIndex = item->GetSocket(AMULET_UPGRADE_SOCKET);

			if (currUpIndex > s_vRS) currUpIndex = GetRandomUpIndex(item);

			const auto fIt = fRS->second.at(currUpIndex);
			if (fIt.size())
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, "AmuletInformation_UpgradeClear");

				for (const auto& [dwVnum, iCount] : fIt)
				{
					ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletInformation_UpgradeInfo {} {}", dwVnum, iCount));
				}
			}
		}
	}

	void AmuletUpgrade(LPCHARACTER ch, CItem* item, int iApplyIndex)
	{
		// Lets check if we have bonus on this index.
		if (item->GetAttributeType(iApplyIndex) == 0) return;

		// Lets check if we have required item!
		if (!CanAmuletUpgrade(ch, item, iApplyIndex)) return;

		// To be sure that's will never be smaller than 0
		if (item->GetValue(AMULET_TYPE_VALUE) < 0) return;

		auto fIt = m_AmuletConfiguration.find(static_cast<EAmuletSubTypes>(item->GetSubType()));
		if (fIt != m_AmuletConfiguration.end())
		{
			// Lets check the current value of bonus
			long iCurrentGrade = item->GetSocket(iApplyIndex);

			item->SetSocket(iApplyIndex, std::min<long>(iCurrentGrade + 1, item->GetValue(AMULET_MAX_GRADE_VALUE)));
			
			// After unlocking we wanna have value equal to 0
			if (item->GetSocket(iApplyIndex) != 0)
			{
				auto fApply = (fIt->second).find(item->GetAttributeType(iApplyIndex));
				if (fApply != (fIt->second.end()))
				{
					if ((fApply->second).size() >= item->GetValue(AMULET_TYPE_VALUE))
					{
						auto vApplyValues = (fApply->second).at(std::min<int>(item->GetValue(AMULET_TYPE_VALUE), AMULET_MAX_POSSIBLE_REFINE_VALUE));
						if (vApplyValues.size() >= item->GetSocket(iApplyIndex))
						{
							item->SetForceAttribute(iApplyIndex, item->GetAttributeType(iApplyIndex), vApplyValues.at(item->GetSocket(iApplyIndex) - 1));
						}
					}
				}
			}
		}

		// Lets remove required items!
		UpgradeClearUp(ch, item, iApplyIndex);

		// Lets set upgrade index socket
		item->SetSocket(AMULET_UPGRADE_SOCKET, GetRandomUpIndex(item));
		AmuletUpgradeInfo(ch, item);
	}

	void AmuletRoll(LPCHARACTER ch, CItem* item, BYTE iBit)
	{
		if (!ch || !ch->GetDesc() || !item) return;

		size_t applyCount = item->GetValue(AMULET_TYPE_VALUE);

		uint32_t lCount = __builtin_popcount(iBit);

		// That's meaning you have nothing to roll
		if (applyCount - lCount <= 0)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_FAILED_ROLL_MISSING_ATTRIBUTES");
			return;
		}

		// Lets check if we required and have enough lock items!
		if (lCount)
		{
			if (ch->CountSpecifyItem(AMULET_ROLL_VNUM) < lCount)
			{
				ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_FAILED_ROLL_MISSING_LOCK_ITEM");
				return;
			}
		}

		if (ch->CountSpecifyItem(AMULET_LOCK_VNUM) < lCount + 1)
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_FAILED_ROLL_MISSING_ROLL_ITEM");
			return;
		}

		for (size_t i = 0; i < applyCount; ++i)
		{
			// We wanna skip base bonuses
			int applyIteratoring = i + AMULET_BASE_BONUS_COUNT;

			if (item->GetAttributeType(applyIteratoring) == 0) continue;

			// Lets check if we have it locked!
			if (iBit & (1 << i)) continue;

			auto rApply = GetRandomApply(item);

			while (item->HasAttr(rApply)) {
				rApply = GetRandomApply(item);
			}

			item->SetForceAttribute(applyIteratoring, rApply, 0);

			// We wanna to have level == 0
			item->SetSocket(applyIteratoring, std::min<long>(0, item->GetSocket(applyIteratoring)));
		}

		// Lets remove required items
		ch->RemoveSpecifyItem(AMULET_ROLL_VNUM, lCount);
		ch->RemoveSpecifyItem(AMULET_LOCK_VNUM, lCount + 1);
	}

	// Crafting
	int AmuletCraftProcess(LPCHARACTER ch, CItem* item)
	{
		if (item->GetType() != ITEM_AMULET) return -1;

		if (ch->GetGold() < item->GetValue(AMULET_GOLD_CRAFT_VALUE)) return -2;

		// Lets remove required things from character
		ch->PointChange(POINT_GOLD, -static_cast<int64_t>(item->GetValue(AMULET_GOLD_CRAFT_VALUE)));
		ITEM_MANAGER::instance().RemoveItem(item, "AMULET_CRAFT");

		ch->AutoGiveItem(AMULET_DUST_ITEM, std::max<CountType>(1, item->GetValue(AMULET_CRAFT_REWARD_COUNT)));
		return 0;
	}

	// Combination / Passive
	void BroadcastPassiveInformation(LPCHARACTER ch)
	{
		for (const auto& rElement : m_AmuletPercentage)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletPassive_Info {} {} {} {} {}", rElement.first, rElement.second.at(0), rElement.second.at(1), rElement.second.at(2), rElement.second.at(3)));
		}
	}

	/// Struct methods ///
	void TAmuletRecord::UpdateItem(LPCHARACTER ch, const BYTE& iSlotNum, const DWORD& dwID, const WORD& wCell)
	{
		LPITEM item = ITEM_MANAGER::instance().Find(a_records[iSlotNum].id);
		if (item && item->GetOwner() == ch)
			item->Lock(false);

		a_records[iSlotNum] = TAmuletItemRecord(dwID, wCell);
	}

	void TAmuletRecord::ReleaseItems(LPCHARACTER ch)
	{
		for (const auto& record : a_records)
		{
			LPITEM item = ITEM_MANAGER::instance().Find(record.id);
			if (item && item->GetOwner() == ch)
				item->Lock(false);
		}
	}
	/// End ///
	
	/// Local Functions ///
	static LPITEM GetAmuletByID(LPCHARACTER ch, DWORD dwID)
	{
		auto pItem = ITEM_MANAGER::instance().Find(dwID);
		if (!pItem)
			return nullptr;

		return (pItem->GetOwner() != ch ? nullptr : pItem);
	}

	static bool CheckItemsIntegrity(LPCHARACTER ch, std::vector<LPITEM>& v_items, const TAmuletRecord::TAmuletArray& a_records)
	{
		for (auto const& sash : a_records)
		{
			LPITEM item;
			if (!(item = ITEM_MANAGER::instance().Find(sash.id)))
				return false;

			if (item->GetOwner() != ch || item->GetCell() != sash.cell || item->GetWindow() != INVENTORY)
				return false;

			v_items.push_back(item);
		}

		return true;
	}

	static uint64_t GetRequiredGold(uint64_t iBaseValue, BYTE iStep)
	{
		return (iBaseValue * 5) * std::max<BYTE>(1, iStep);
	}
	/// End ///

	// Releasing from container
	void ReleasePlayer(const DWORD& pid)
	{
		m_amulet_combine_map.erase(pid);
	}

	// Clearing up in case of shutdown
	void ClearUp()
	{
		m_amulet_combine_map.clear();
	}

	// Registry things
	bool RegisterCombine(LPCHARACTER ch)
	{
		if (!ch)
			return false;

		// Checking if record exist
		if (m_amulet_combine_map.find(ch->GetPlayerID()) != m_amulet_combine_map.end())
			return false;

		// Registering pc as sash refiner
		m_amulet_combine_map.emplace(std::piecewise_construct, std::forward_as_tuple(ch->GetPlayerID()), std::forward_as_tuple());
		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_Open {}", AMULET_DUST_ITEM));
		return true;
	}

	void UnregisterCombine(LPCHARACTER ch)
	{
		auto r_it = m_amulet_combine_map.find(ch->GetPlayerID());
		if (r_it != m_amulet_combine_map.end())
		{
			(r_it->second).ReleaseItems(ch);
			m_amulet_combine_map.erase(r_it);
		}
	}

	// Opening interface
	void AmuletCombineOpen(LPCHARACTER ch)
	{
		if (m_amulet_combine_map.find(ch->GetPlayerID()) != m_amulet_combine_map.end())
			return;

		RegisterCombine(ch);
	}

	// Closing interface
	void AmuletCombineClose(LPCHARACTER ch)
	{
		UnregisterCombine(ch);

		ch->ChatPacket(CHAT_TYPE_COMMAND, "AmuletCombine_Reset");
	}

	// Register item
	bool AmuletCombineRegisterItem(LPCHARACTER ch, LPITEM item, const BYTE& iSlotNum)
	{
		if (!ch || !item)
			return false;

		// Checking belong slots
		if (iSlotNum > COMBINE_TYPE_ITEM)
			return false;

		// Checking if record exist
		auto it = m_amulet_combine_map.find(ch->GetPlayerID());
		if (it == m_amulet_combine_map.end())
			return false;

		// Checking if item is not reach max level
		if (item->GetValue(AMULET_TYPE_VALUE) == AMULET_MAX_POSSIBLE_REFINE_VALUE)
			return false;

		// Checking if item has required values of bonuses
		for (size_t i = 0; i < item->GetAttributeCount(); ++i)
		{
			if (item->GetAttributeValue(i) < item->GetValue(AMULET_MAX_GRADE_VALUE)) return false;
		}

		// Checking item type!
		if (item->GetType() != ITEM_AMULET) return false;
		
		// If we doesn't have any chance to upgrade item, return!
		if (GetPercentage(ch, item) == 0) return false;

		// Checking type integrity if second slot exists
		const DWORD& secSlotId = (it->second).GetItem(iSlotNum == COMBINE_TYPE_AMULET ? COMBINE_TYPE_ITEM : COMBINE_TYPE_AMULET).id;
		if (secSlotId > 0)
		{
			auto secAmulet = ITEM_MANAGER::instance().Find(secSlotId);
			if (!secAmulet) return false;

			// Checking if items was same
			if (GetAmuletType(item) != GetAmuletType(secAmulet)) return false;
			
			// Checking item sub type
			if (item->GetSubType() != secAmulet->GetSubType()) return false;
		}

		(it->second).UpdateItem(ch, iSlotNum, item->GetID(), item->GetCell());
		item->Lock(true);

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterItem {} {}", iSlotNum, item->GetCell()));

		// Alright right now we wanna send the reward item!
		auto pOriginItem = GetAmuletByID(ch, (it->second).GetItem(COMBINE_TYPE_AMULET).id);
		if (pOriginItem && pOriginItem->GetRefinedVnum())
		{
			LPITEM tmpItem = ITEM_MANAGER::instance().CreateItem(pOriginItem->GetRefinedVnum(), 1, 0, false, 0, true);
			if (tmpItem)
			{
				ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterItem {} {} {}", COMBINE_TYPE_RESULT, -1, pOriginItem->GetRefinedVnum()));
				M2_DESTROY_ITEM(tmpItem);
			}
		}

		if (iSlotNum == COMBINE_TYPE_AMULET)
		{
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterChance {}", GetPercentage(ch, item)));
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterGold {}", GetRequiredGold(item->GetValue(AMULET_GOLD_VALUE), item->GetValue(AMULET_TYPE_VALUE))));
		}

		return true;
	}

	bool AmuletCombineEraseItem(LPCHARACTER ch, const BYTE& iSlotNum)
	{
		if (!ch) return false;

		// Checking if slot belongs
		if (iSlotNum > COMBINE_TYPE_ITEM) return false;

		// Checking if record exists
		auto it = m_amulet_combine_map.find(ch->GetPlayerID());
		if (it == m_amulet_combine_map.end()) return false;

		(it->second).UpdateItem(ch, iSlotNum, 0, 0);

		ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterItem {} {}", iSlotNum, -1));

		const auto pItems = (it->second).GetItemList();
		auto cIf = std::count_if(pItems.begin(), pItems.end(), [](const TAmuletItemRecord& rItem) { return rItem.id > 0; });

		if (cIf == 0)
			ch->ChatPacket(CHAT_TYPE_COMMAND, fmt::format("AmuletCombine_RegisterItem {} {}", COMBINE_TYPE_RESULT, -1));

		return true;
	}

	bool AmuletCombineProcess(LPCHARACTER ch)
	{
		if (!ch) return false;

		// Checking of record exists
		auto r_it = m_amulet_combine_map.find(ch->GetPlayerID());
		if (r_it == m_amulet_combine_map.end())
			return false;

		// Checking amulet validate
		std::vector<LPITEM> v_amulets;
		if (!CheckItemsIntegrity(ch, v_amulets, (r_it->second).GetItemList()))
			return false;

		// Checking if one can pay refine cost
		if (ch->GetGold() < GetRequiredGold(v_amulets[COMBINE_TYPE_AMULET]->GetValue(AMULET_GOLD_VALUE), v_amulets[COMBINE_TYPE_AMULET]->GetValue(AMULET_TYPE_VALUE)))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_NOT_ENOUGH_GOLD");
			return false;
		}
		else
			ch->PointChange(POINT_GOLD, -GetRequiredGold(v_amulets[COMBINE_TYPE_AMULET]->GetValue(AMULET_GOLD_VALUE), v_amulets[COMBINE_TYPE_AMULET]->GetValue(AMULET_TYPE_VALUE)));

		// Destroying material!
		v_amulets[COMBINE_TYPE_ITEM]->SetCount(0);
		
		// Just in case if we failure!
		if (GetPercentage(ch, v_amulets[COMBINE_TYPE_ITEM]) < Random::get(1, 100))
		{
			ch->ChatPacket(CHAT_TYPE_INFO, "AMULET_SYSTEM_COMBINATION_FAILED");
			ITEM_MANAGER::instance().RemoveItem(v_amulets[COMBINE_TYPE_AMULET], "REMOVE (AMULET REFINE FAILED)");
			return false;
		}

		// Creating new item!
		LPITEM pkNewItem = ITEM_MANAGER::instance().CreateItem(v_amulets[COMBINE_TYPE_AMULET]->GetRefinedVnum(), 1, 0, false, -1, false, false, true);
		if (!pkNewItem)
			return false;
		
		// Lets copy current bonuses to new item!
		v_amulets[COMBINE_TYPE_AMULET]->CopyAttributeTo(pkNewItem);

		// After refine we wanna set whole applys to be on lv 0
		for (size_t i = 0; i < v_amulets[COMBINE_TYPE_AMULET]->GetAttributeCount(); i++)
		{
			pkNewItem->SetSocket(i, 0);
		}

		// Uncomment just in case if we wanna have it on max
		//v_amulets[COMBINE_TYPE_AMULET]->CopySocketTo(pkNewItem);

		// Lets add additional bonus!
		OnCreate(pkNewItem);

		auto bCell = v_amulets[COMBINE_TYPE_AMULET]->GetCell();
		ITEM_MANAGER::instance().RemoveItem(v_amulets[COMBINE_TYPE_AMULET], "REMOVE (AMULET REFINE SUCCESS)");
	
		pkNewItem->AddToCharacter(ch, TItemPos(INVENTORY, bCell));
		ITEM_MANAGER::instance().FlushDelayedSave(pkNewItem);

		// Releasing iterator
		UnregisterCombine(ch);

		// Propably we should need RESET there!
		ch->ChatPacket(CHAT_TYPE_COMMAND, "AmuletCombine_Reset");

		// Readding pc
		RegisterCombine(ch);
		return true;
	}
}
#endif
