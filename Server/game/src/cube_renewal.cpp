#include "stdafx.h"
#include "constants.h"
#include "utils.h"
#include "config.h"
#include "log.h"
#include "char.h"
#include "db.h"
#include "lzo_manager.h"
#include "desc_client.h"
#include "buffer_manager.h"
#include "char_manager.h"
#include "packet.h"
#include "questmanager.h"
#include "locale_service.h"
#include "mob_manager.h"
#include "item_manager.h"
#include "cube_renewal.h"
#include "p2p.h"
#include "item.h"

#ifdef __BATTLE_PASS_ENABLE__
	#include "BattlePassManager.hpp"
#endif

#ifdef __ENABLE_CUBE_RENEWAL_GEM__

	//* If you use my gem system use:

	// #define GET_GEM_FUNC pkChr->GetGem()
	// #define CHANGE_GEM_FUNC(value) pkChr->PointChange(POINT_GEM, -value, false);

	//* If you use gaya market like an object use:

	#define GET_GEM_FUNC pkChr->CountSpecifyItem(50926)
	#define CHANGE_GEM_FUNC(value) pkChr->RemoveSpecifyItem(50926, value)

#endif

namespace CCubeRenewalHelper
{
	const std::string sBlockFlag("c_d_npc_{}_catt_{}");
	const std::string sBlockFlagItem(sBlockFlag + "_i_{}");


	std::vector<SCubeDisabledNPC> v_disabled_npcs = {};

	// Functions
	void UpdateEventFlag(const std::string & sFlag, const int& iValue)
	{
		quest::CQuestManager::Instance().RequestSetEventFlag(sFlag, iValue);
		quest::CQuestManager::Instance().SetEventFlag(sFlag, iValue);
	}

	void RegisterDisabledCattegory(const DWORD& dwVnum, const BYTE& bCategory, const DWORD& dwRequiredItem, const WORD& wRequiredItemCount)
	{
		SCubeDisabledNPC sNpc = {};
		sNpc.dwVnum = dwVnum;
		sNpc.bCategory = bCategory;
		sNpc.dwRequiredItem = dwRequiredItem;
		sNpc.wRequiredItemCount = wRequiredItemCount;

		v_disabled_npcs.push_back(sNpc);

		sys_log(0, "The disabled npc %d of cube was registered! Data: Category: %d, Required item: %d, Required count: %d", dwVnum, bCategory, dwRequiredItem, wRequiredItemCount);
	}

	// Getter
	bool IsDisabledCategory(const uint32_t& dwNpcVnum, const BYTE & iCategory)
	{
		auto fIt = std::find_if(v_disabled_npcs.begin(), v_disabled_npcs.end(), [&dwNpcVnum, &iCategory](const SCubeDisabledNPC& rRecord) { return (rRecord.dwVnum == dwNpcVnum, rRecord.bCategory == iCategory); });
		if (fIt == v_disabled_npcs.end())
			return false;

		const auto iCurrCount = quest::CQuestManager::Instance().GetEventFlag(fmt::format(sBlockFlagItem, dwNpcVnum, fIt->bCategory, fIt->dwRequiredItem));
		return iCurrCount != fIt->wRequiredItemCount;
	}

	SCubeDisabledNPC* GetCattegoryToUnlock(const uint32_t& dwNpcVnum)
	{
		for (auto& rRecord : v_disabled_npcs)
		{
			if (IsDisabledCategory(rRecord.dwVnum, rRecord.bCategory))
			{
				return &rRecord;
			}
		}
		
		return nullptr;
	}

	const int GetAmount(const uint32_t& dwNpcVnum, const BYTE& iCategory, const DWORD& dwRequiredItem)
	{
		return quest::CQuestManager::Instance().GetEventFlag(fmt::format(sBlockFlagItem, dwNpcVnum, iCategory, dwRequiredItem));
	}

	int SetAmount(const uint32_t& dwNpcVnum, const BYTE& iCategory, const WORD& iRemovedCount)
	{
		auto fIt = std::find_if(v_disabled_npcs.begin(), v_disabled_npcs.end(), [&dwNpcVnum, &iCategory](const SCubeDisabledNPC& rRecord) { return (rRecord.dwVnum == dwNpcVnum && rRecord.bCategory == iCategory); });
		if (fIt == v_disabled_npcs.end())
			return -1;

		//Checking if this cattegory is still locked to be sure!
		if (!IsDisabledCategory(dwNpcVnum, iCategory))
			return -2;

		//Getting the required count
		const auto iCurrGivedCount = quest::CQuestManager::instance().GetEventFlag(fmt::format(sBlockFlagItem, dwNpcVnum, iCategory, fIt->dwRequiredItem));

		//Checking if we have the pointer of character
		const auto pCharacter = quest::CQuestManager::Instance().GetCurrentCharacterPtr();
		
		if (pCharacter)
		{
			//Getting the count of items from current character
			const auto iCurrItemCount = pCharacter->CountSpecifyItem(fIt->dwRequiredItem);

			//Checking if we have enough items to remove
			if (iCurrItemCount < iRemovedCount)
				return -3;

			//Let's check the currently required number of items to hand over
			const auto iRequired = fIt->wRequiredItemCount - iCurrGivedCount;

			//Let's use mines, it's safer when we want to return more than required.
			const auto iRemoveItemCount = std::min(iRequired, static_cast<int>(iRemovedCount));
			pCharacter->RemoveSpecifyItem(fIt->dwRequiredItem, iRemoveItemCount);

			UpdateEventFlag(fmt::format(sBlockFlagItem, dwNpcVnum, iCategory, fIt->dwRequiredItem), iCurrGivedCount + iRemoveItemCount);
			return 1;
		}

		return -5;
	}
};

#define __ENABLE_CUBE_RENEWAL_BLOCK_CATEGORIES__

CCubeRenewal::CCubeRenewal()
{
}

CCubeRenewal::~CCubeRenewal()
{

}

void CCubeRenewal::Initialize(bool bIsReload)
{
	if (g_bAuthServer)
	{
		return;
	}

	char fileName[256 + 1];
	snprintf(fileName, sizeof(fileName), "%s/cube.txt", GetBasePath().c_str());

	std::string initType = bIsReload ? "Reload Cube Data" : "Initialize Cube Data";

	sys_log(0, "%s: %s", initType.c_str(), fileName);

	// Delete current cube data ( for reload )
	if (bIsReload)
	{
		TCubeData * pCube = NULL;
		TCubeMap::iterator iter;
		TCubeVector::iterator iterV;

		for (iter = sCubeProto.begin(); iter != sCubeProto.end(); iter++)
		{
			TCubeVector itS = (*iter).second;
			if (!itS.empty())
			{
				for (iterV = itS.begin(); iterV != itS.end(); iterV++)
				{
					pCube = *iterV;
					M2_DELETE(pCube);
				}

				itS.clear();
			}
		}

		sCubeProto.clear();
	}

	if (!CubeLoad(fileName))
	{
		sys_err("%s failed", initType.c_str());
	}
}

bool CCubeRenewal::CubeLoad(const char *fileName)
{
	FILE *fp;
	char one_line[256];
	unsigned long long value1;
	int value2;
	const char	*delim = " \t\r\n";
	char *v, *token_string;

	int iIndex = 0; // Unique index for every item in cube.txt
	DWORD dwNpcVnum = 0;

	TCubeData *cubeData = NULL;
	TCubeItem tempItem = {0, 0};

	if (0 == fileName || 0 == fileName[0])
	{
		return false;
	}

	if ((fp = fopen(fileName, "r")) == 0)
	{
		return false;
	}

	while (fgets(one_line, 256, fp))
	{
		value1 = value2 = 0;

		if (one_line[0] == '#')
		{
			continue;
		}

		token_string = strtok(one_line, delim);

		if (NULL == token_string)
		{
			continue;
		}

		TOKEN("category")
		{
			if ((v = strtok(NULL, delim)))
			{
				std::string catName = std::string(v);
				if (cubeData)
				{
					cubeData->bCategory = GetCategoryIndexByName(catName);
				}
			}
		}
		else
		{
			if ((v = strtok(NULL, delim)))
			{
				str_to_number(value1, v);
			}

			if ((v = strtok(NULL, delim)))
			{
				str_to_number(value2, v);
			}

			TOKEN("section")
			{
				cubeData = M2_NEW TCubeData;
			}
			else TOKEN("npc")
			{
				dwNpcVnum = (DWORD)value1;
			}
			else TOKEN("item")
			{
				tempItem.dwVnum = value1;
				tempItem.iCount = value2;

				cubeData->materialV.push_back(tempItem);
			}
			else TOKEN("reward")
			{
				tempItem.dwVnum = value1;
				tempItem.iCount = value2;

				cubeData->tReward = tempItem;
			}
			else TOKEN("inc_item")
			{
				tempItem.dwVnum = value1;
				tempItem.iCount = value2;

				cubeData->tIncItem = tempItem;
			}
			else TOKEN("inc_percent")
			{
				cubeData->iIncPercent = value1;
			}
			else TOKEN("percent")
			{
				cubeData->iPercent = value1;
			}
			else TOKEN("gold")
			{
				cubeData->dwGold = value1;
			}
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
			else TOKEN("copy_bonus")
			{
				cubeData->bCopyBonus = value1 ? true : false;
			}
#endif

			else TOKEN("end")
			{
				if (!IsGoodCubeData(cubeData, dwNpcVnum))
				{
					M2_DELETE(cubeData);
					continue;
				}

				cubeData->iIndex = iIndex;
				iIndex++;

				TCubeMap::iterator it = sCubeProto.find(dwNpcVnum);
				if (it != sCubeProto.end())
				{
					it->second.push_back(cubeData);
				}
				else
				{
					TCubeVector emptyVec;
					emptyVec.push_back(cubeData);
					sCubeProto.insert(std::make_pair(dwNpcVnum, emptyVec));
				}
			}
		}
	}

	fclose(fp);
	return true;
}

bool CCubeRenewal::IsGoodCubeData(TCubeData *cubeData, DWORD dwNpcVnum)
{
	if (!cubeData)
	{
		sys_err("CubeLoad::Null cube data ");
		return false;
	}

	if (dwNpcVnum == 0)
	{
		sys_err("CubeLoad::Npc Vnum is null ");
		return false;
	}

	const CMob * pkMob = CMobManager::instance().Get(dwNpcVnum);
	if (!pkMob)
	{
		sys_err("CubeLoad::Wrong npc vnum %u ", dwNpcVnum);
		return false;
	}

	if (cubeData->materialV.size() > CUBE_MATERIAL_MAX_NUM)
	{
		sys_err("CubeLoad::Too much material size %d - Npc vnum %u ", cubeData->materialV.size(), dwNpcVnum);
		return false;
	}

	for (int i = 0; i < cubeData->materialV.size(); ++i)
	{
		if (!cubeData->materialV[i].iCount)
		{
			sys_err("CubeLoad::Material count is null - Npc Vnum %u ", dwNpcVnum);
			return false;
		}

		if (!cubeData->materialV[i].dwVnum)
		{
			sys_err("CubeLoad::Material vnum is null - Npc Vnum %u ", dwNpcVnum);
			return false;
		}

		TItemTable * pProto = ITEM_MANAGER::instance().GetTable(cubeData->materialV[i].dwVnum);

		if (!pProto)
		{
			sys_err("CubeLoad::Material is wrong - ItemVnum %u - Npc Vnum %u ", cubeData->materialV[i].dwVnum, dwNpcVnum);
			return false;
		}
	}

	if (!cubeData->tReward.iCount)
	{
		sys_err("CubeLoad::Reward count is null - Npc Vnum %u ", dwNpcVnum);
		return false;
	}

	if (!cubeData->tReward.dwVnum)
	{
		sys_err("CubeLoad::Reward vnum is null - Npc Vnum %u ", dwNpcVnum);
		return false;
	}

	if (cubeData->tIncItem.dwVnum)
	{
		TItemTable * pProtoI = ITEM_MANAGER::instance().GetTable(cubeData->tIncItem.dwVnum);

		if (!pProtoI)
		{
			sys_err("CubeLoad::Increase Percent item is wrong - Vnum %u - Npc Vnum %u ", cubeData->tIncItem.dwVnum, dwNpcVnum);
			return false;
		}

		if (cubeData->tIncItem.iCount <= 0)
		{
			sys_err("CubeLoad::Increase Percent count is null - Npc Vnum %u ", dwNpcVnum);
			return false;
		}

		if (cubeData->iIncPercent <= 0 || cubeData->iIncPercent > 100)
		{
			sys_err("CubeLoad::IncreasePercent value is wrong - Value %d - Npc Vnum %u ", cubeData->iIncPercent, dwNpcVnum);
			return false;
		}
	}

	TItemTable * pProto = ITEM_MANAGER::instance().GetTable(cubeData->tReward.dwVnum);

	if (!pProto)
	{
		sys_err("CubeLoad::Reward is wrong - RewardVnum %u - Npc Vnum %u ", cubeData->tReward.dwVnum, dwNpcVnum);
		return false;
	}

	if (cubeData->bCategory == -1)
	{
		sys_err("CubeLoad::Wrong category index %u - Npc Vnum %u ", cubeData->bCategory, dwNpcVnum);
		return false;
	}

	return true;
}

int CCubeRenewal::GetCategoryIndexByName(std::string stCategoryName)
{
	const std::string stCategoryType[CUBE_CATEGORY_MAX] =
	{
		"WEAPONPVP",
		"ARMORPVM",
		"ARMORPVP",

		"ACCESSORYPVM",
		"ACCESSORYPVP",

		"SHIELDSPVM",
		"SHIELDSPVP",

		"HELMETPVM",
		"HELMETPVP",

		"BRACELETPVM",
		"BRACELETPVP",

		"NECKLACEPVM",
		"NECKLACEPVP",

		"EARRINGSPVM",
		"EARRINGSPVP",

		"SHOESPVM",
		"SHOESPVP",

		"BELTPVM",
		"BELTPVP",

		"EVENT",
		"ETC",

		"PET",
		"MOUNT",
		"SOULS",
		"SOULS_SCROLLS",
		"REFINE_SCROLLS"
	};

	for (int i = 0; i < CUBE_CATEGORY_MAX; i++)
	{
		if (stCategoryType[i] == stCategoryName)
		{
			return i;
		}
	}

	return -1;
}

bool CCubeRenewal::CubeOpen(LPCHARACTER pkChr, LPCHARACTER pkCubeNpc)
{
	if (!pkChr)
	{
		return false;
	}

	if (!pkChr->GetDesc())
	{
		return false;
	}

	if (pkChr->IsCubeOpen())
	{
		return false;
	}

	pkChr->SetCubeOpen(true);

	TEMP_BUFFER buf;

	TPacketGCCubeItem packet;
	packet.bHeader = HEADER_GC_CUBE_ITEM;
	packet.dwNpcVnum = pkCubeNpc->GetRaceNum();

	TCubeDataP sendItem = {};
	TCubeData * pCube = NULL;
	TCubeMap::iterator iter = sCubeProto.find(pkCubeNpc->GetRaceNum());
	TCubeVector::iterator iterV;

	int i = 0;

	if (iter != sCubeProto.end())
	{
		TCubeVector itS = (*iter).second;
		if (!itS.empty())
		{
			for (iterV = itS.begin(); iterV != itS.end(); iterV++)
			{
				pCube = *iterV;

#ifdef __ENABLE_CUBE_RENEWAL_BLOCK_CATEGORIES__
				if (CCubeRenewalHelper::IsDisabledCategory(pkCubeNpc->GetRaceNum(), pCube->bCategory)) continue;
#endif

				memset(&sendItem, 0, sizeof(sendItem));

				sendItem.iIndex = pCube->iIndex;

				for (i = 0; i < CUBE_MATERIAL_MAX_NUM && i < pCube->materialV.size(); ++i)
				{
					sendItem.materialV[i] = pCube->materialV.at(i);
				}

				sendItem.tReward = pCube->tReward;
				sendItem.tIncItem = pCube->tIncItem;
				sendItem.iIncPercent = pCube->iIncPercent;
				sendItem.iPercent = pCube->iPercent;
				sendItem.dwGold = pCube->dwGold;
				sendItem.bCategory = pCube->bCategory;
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
				sendItem.bCopyBonus = pCube->bCopyBonus;
#endif
				buf.write(&sendItem, sizeof(sendItem));
			}
		}
	}

	packet.wSize = sizeof(packet) + buf.size();

	if (buf.size())
	{
		pkChr->GetDesc()->BufferedPacket(&packet, sizeof(TPacketGCCubeItem));
		pkChr->GetDesc()->Packet(buf.read_peek(), buf.size());
	}
	else
	{
		pkChr->GetDesc()->Packet(&packet, sizeof(TPacketGCCubeItem));
	}

	return true;
}

void CCubeRenewal::CubeClose(LPCHARACTER pkChr)
{
	if (!pkChr)
	{
		return;
	}

	pkChr->SetCubeOpen(false);
}

void CCubeRenewal::SendCraftFailPacket(LPCHARACTER pkChr, BYTE bErrorType, DWORD dwErrorArg)
{
	if (!pkChr)
	{
		return;
	}

	TPacketGCCubeCraft pack;
	pack.bHeader = HEADER_GC_CUBE_CRAFT;
	pack.bErrorType = bErrorType;
	pack.dwErrorArg = dwErrorArg;

	pkChr->GetDesc()->Packet(&pack, sizeof(TPacketGCCubeCraft));
}

void CCubeRenewal::CubeMake(LPCHARACTER pkChr, const char * pcData)
{
	if (!pkChr)
	{
		return;
	}

	if (!pkChr->IsCubeOpen() || pkChr->IsObserverMode() || pkChr->IsWarping())
	{
		return;
	}

	TPacketCGCubeMake * p = (TPacketCGCubeMake *) pcData;
	sys_log(0, "CCubeRenewal::CubeMake ==> NpcVnum %u - ItemIndex %d - Repeat %d - IsIncrease %d", p->dwNpcVnum, p->iIndex, p->bMakeCount, p->bIsIncrease);

	if (sCubeProto.find(p->dwNpcVnum) == sCubeProto.end())
	{
		return;
	}

	TCubeData * pCube = NULL;
	TCubeMap::iterator iter = sCubeProto.find(p->dwNpcVnum);
	TCubeVector::iterator iterV;
	int i = 0, k = 0;
	int iPercentRandom = 0;
	BYTE bSuccesCount = 0;
	bool bIncreaseChance = false;
	int iRealPercent = 0;
	BYTE bMakeCount = p->bMakeCount;
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
	TPlayerItemAttribute	materialItemAttr[ITEM_ATTRIBUTE_MAX_NUM];
	long	alSockets[ITEM_SOCKET_MAX_NUM];
#endif
	LPITEM item;

	if (iter != sCubeProto.end())
	{
		TCubeVector itS = (*iter).second;
		if (!itS.empty())
		{
			for (iterV = itS.begin(); iterV != itS.end(); iterV++)
			{
				pCube = *iterV;
				if (pCube->iIndex == p->iIndex)
				{
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
					// Do not allow multiple craft when the bonuses need to be copy
					// because of the popup from client
					if (pCube->bCopyBonus)
					{
						bMakeCount = 1;
					}
#endif

					for (i = 0; i < bMakeCount; ++i)
					{
						if (pkChr->GetGold() < pCube->dwGold)
						{
							SendCraftFailPacket(pkChr, CUBE_CRAFT_NOT_ENOUGH_MONEY, 0);
							return;
						}

#ifdef __ENABLE_CUBE_RENEWAL_GEM__
						if (GET_GEM_FUNC < pCube->dwGem)
						{
							SendCraftFailPacket(pkChr, CUBE_CRAFT_NOT_ENOUGH_GEM, 0);
							return;
						}
#endif

						for (k = 0; k < pCube->materialV.size(); ++k)
						{
							LPITEM cubeMaterial = pkChr->FindSpecifyItem(pCube->materialV[k].dwVnum);

							if (cubeMaterial)
							{
#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
								if (pCube->bCopyBonus)
								{
									if (cubeMaterial->GetType() == ITEM_WEAPON)
									{
										thecore_memcpy(materialItemAttr, cubeMaterial->GetAttributes(), sizeof(materialItemAttr));
										thecore_memcpy(alSockets, cubeMaterial->GetSockets(), sizeof(alSockets));
									}
									else if (cubeMaterial->GetType() == ITEM_ARMOR && (cubeMaterial->GetSubType() == ARMOR_WRIST || cubeMaterial->GetSubType() == ARMOR_NECK || cubeMaterial->GetSubType() == ARMOR_EAR))
									{
										thecore_memcpy(materialItemAttr, cubeMaterial->GetAttributes(), sizeof(materialItemAttr));
										thecore_memcpy(alSockets, cubeMaterial->GetSockets(), sizeof(alSockets));
									}
								}
#endif
							}

							if (pkChr->CountSpecifyItem(pCube->materialV[k].dwVnum) < pCube->materialV[k].iCount)
							{
								SendCraftFailPacket(pkChr, CUBE_CRAFT_NOT_ENOUGH_OBJECT, pCube->materialV[k].dwVnum);
								return;
							}
						}

						bIncreaseChance = false;
						if (p->bIsIncrease)
						{
							if (pCube->tIncItem.dwVnum && pCube->tIncItem.iCount)
							{
								if (pkChr->CountSpecifyItem(pCube->tIncItem.dwVnum) >= pCube->tIncItem.iCount)
								{
									__ENABLE_CUBE_RENEWAL_COPY_BONUS__
									bIncreaseChance = true;
								}
							}
						}

						item = ITEM_MANAGER::instance().CreateItem(pCube->tReward.dwVnum, pCube->tReward.iCount);
						if (!item)
						{
							SendCraftFailPacket(pkChr, CUBE_CRAFT_CREATE_ITEM, pCube->tReward.dwVnum);
							return;
						}

						int iEmptyPos;
						if (item->IsDragonSoul())
						{
							iEmptyPos = pkChr->GetEmptyDragonSoulInventory(item);
						}
						else
						{
							iEmptyPos = pkChr->GetEmptyInventory(item->GetSize());
						}

						if (iEmptyPos < 0)
						{
							M2_DESTROY_ITEM(item);

							SendCraftFailPacket(pkChr, CUBE_CRAFT_INVENTORY_FULL, 0);
							return;
						}

						if (pCube->dwGold)
						{
							pkChr->PointChange(POINT_GOLD, -pCube->dwGold, false);
#ifdef __BATTLE_PASS_ENABLE__
							CBattlePassManager::instance().TriggerEvent(pkChr, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_ALL_SPEND_MONEY, 0, pCube->dwGold });
#endif
						}
#ifdef __ENABLE_CUBE_RENEWAL_GEM__
						if (pCube->dwGem)
						{
							CHANGE_GEM_FUNC(pCube->dwGem);
						}
#endif

						for (k = 0; k < pCube->materialV.size(); ++k)
						{
							if (pCube->materialV[k].iCount)
							{
								pkChr->RemoveSpecifyItem(pCube->materialV[k].dwVnum, pCube->materialV[k].iCount);
							}
						}

						iPercentRandom = number(1, 100);
						iRealPercent = pCube->iPercent;

						if (bIncreaseChance)
						{
							iRealPercent = MIN(100, iRealPercent + pCube->iIncPercent);

							if (pCube->tIncItem.iCount)
							{
								pkChr->RemoveSpecifyItem(pCube->tIncItem.dwVnum, pCube->tIncItem.iCount);
							}
						}

						if (iPercentRandom <= iRealPercent)
						{
							bSuccesCount += 1;

#ifdef __ENABLE_CUBE_RENEWAL_COPY_BONUS__
							if (pCube->bCopyBonus && materialItemAttr && alSockets)
							{
								item->ClearAttribute();
								item->SetAttributes(materialItemAttr);
								item->SetSockets(alSockets);
							}
#endif

							if (item->IsDragonSoul())
							{
								item->AddToCharacter(pkChr, TItemPos(DRAGON_SOUL_INVENTORY, iEmptyPos));
							}
							else
							{
								item->AddToCharacter(pkChr, TItemPos(INVENTORY, iEmptyPos));
							}

							ITEM_MANAGER::instance().FlushDelayedSave(item);

#ifdef __BATTLE_PASS_ENABLE__
							CBattlePassManager::instance().TriggerEvent(pkChr, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_CUBE, pCube->tReward.dwVnum, 1 });
							CBattlePassManager::instance().TriggerEvent(pkChr, BattlePassNS::SObjective{ BattlePassNS::EObjectives::TYPE_CUBE_ALL, 0, 1 });
#endif
							if (bIncreaseChance)
							{
								sys_log(0, "CCubeRenewal::CubeMake ==> Succes with %d - Need %d (+%d) ==> RewardVnum %d, RewardCount %d, ItemId %u",
										iPercentRandom, pCube->iPercent, pCube->iIncPercent, pCube->tReward.dwVnum, pCube->tReward.iCount, item->GetID());
							}
							else
							{
								sys_log(0, "CCubeRenewal::CubeMake ==> Succes with %d - Need %d ==> RewardVnum %d, RewardCount %d, ItemId %u",
										iPercentRandom, pCube->iPercent, pCube->tReward.dwVnum, pCube->tReward.iCount, item->GetID());
							}
						}
						else
						{
							if (bIncreaseChance)
							{
								sys_log(0, "CCubeRenewal::CubeMake ==> Fail with %d - Need %d (+%d) ==> RewardVnum %d, RewardCount %d",
										iPercentRandom, pCube->iPercent, pCube->iIncPercent, pCube->tReward.dwVnum, pCube->tReward.iCount);
							}
							else
							{
								sys_log(0, "CCubeRenewal::CubeMake ==> Fail with %d - Need %d ==> RewardVnum %d, RewardCount %d",
										iPercentRandom, pCube->iPercent, pCube->tReward.dwVnum, pCube->tReward.iCount);
							}

							M2_DESTROY_ITEM(item);
							continue;
						}
					}

					SendCraftFailPacket(pkChr, CUBE_CRAFT_SUCCES, bSuccesCount);

					pkChr->Save();
					break;
				}
			}
		}
		else
		{
			SendCraftFailPacket(pkChr, CUBE_CRAFT_END, 0);
			return;
		}
	}
	else
	{
		SendCraftFailPacket(pkChr, CUBE_CRAFT_END, 0);
		return;
	}
}
