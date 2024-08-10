#include "stdafx.h"
#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
#include "MarbleCreatorSystem.hpp"
#include "char.h"
#include "packet.h"
#include "buffer_manager.h"
#include "desc_client.h"
#include "char_manager.h"
#include "p2p.h"
#include "item_manager.h"
#include "item.h"
#include "utils.h"

//#define __DEBUG_PRINT
#ifdef __DEBUG_PRINT
	#include <iostream>

	using std::cerr;
	using std::endl;
#endif

/*******************************************************************\
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
|||| CMarbleSystemManager - CLASS
|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
\*******************************************************************/

/*******************************************************************\
| [PUBLIC] (De-)Initialize Functions
\*******************************************************************/

MarbleCollection::MarbleSet::MarbleSet(uint8_t id, uint32_t vnum, uint32_t count)
	: id_(std::move(id))
	, vnum_(std::move(vnum))
	, count_(std::move(count))
{}

const MarbleCollection::MarbleSet* CMarbleSystemManager::FindSet(const uint8_t id) const
{
	auto it = sets_.find(id);
	if (it == sets_.end())
		return nullptr;

	return &it->second;
}

const MarbleCollection::MarbleSet* CMarbleSystemManager::FindSetByMonsterVnum(const uint32_t vnum) const
{
	for (const auto& [iKey, rElement] : sets_)
	{
		if (rElement.GetVnum() == vnum)
			return &rElement;
	}

	return nullptr;
}

void CMarbleSystemManager::Create()
{
	Load("locale/germany/marble-manager.xml");
}

void CMarbleSystemManager::Clear()
{
	sets_.clear();
}

bool CMarbleSystemManager::Load(const std::string& filename)
{
	MarbleCollection::Parser parser(filename);
	if (!parser.TryParse()) {
		sys_err(fmt::format("Failed to parse {}.", filename).c_str());
		return false;
	}

	Clear();

	auto sets = std::move(parser.MarbleSets());
	for (auto& set : sets)
		sets_.emplace(set.GetId(), std::move(set));

	return true;
}

/*******************************************************************\
| [PUBLIC] General Functions
\*******************************************************************/

void CMarbleSystemManager::SendMarbleInformation(LPCHARACTER ch, BOOL bUpdate)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}

	SendClientPacket(ch->GetDesc(), GC_MARBLE_MANAGER_REFRESH, NULL, 0);

	TPacketGCMarbleManagerInfo kInfo { 0 };
	for (const auto& [iKey, tData]: sets_)
	{
		kInfo.bID = iKey;
		kInfo.dwMarbleVnum = tData.GetVnum();
		kInfo.wRequiredKillCount = ch->GetMarbleActiveMission() ? tData.GetCount() * ch->GetMarbleActiveExtandedCount() : tData.GetCount();
		kInfo.bActiveMission = ch->GetMarbleActiveMission();
		kInfo.bActiveExtandedCount = ch->GetMarbleActiveExtandedCount();
		kInfo.wKilledMonsters = ch->GetMarbleKilledMonsters();
		kInfo.tCooldownTime = GetCooldownTime(ch, tData.GetVnum(), true);

		SendClientPacket(ch->GetDesc(), EGCMarbleSubHeaders::GC_MARBLE_MANAGER_DATA, &kInfo, sizeof(kInfo));
	}

	TEMP_BUFFER buff;
	buff.write(&bUpdate, sizeof(BOOL));

	SendClientPacket(ch->GetDesc(), GC_MARBLE_MANAGER_OPEN, buff.read_peek(), buff.size());
}

void CMarbleSystemManager::MissionActive(LPCHARACTER ch, std::uint16_t dwIndex, BYTE iExtandedCount)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}
		
	const auto MarbleData = FindSet(dwIndex);
	if (!MarbleData)
		return;

	if (iExtandedCount < 0 || iExtandedCount > 4)
	{
		return;
	}

	auto activeMission = ch->GetMarbleActiveMission();

	if (GetCooldownTime(ch, MarbleData->GetVnum()) > get_global_time())
	{
		return;
	}

	if (activeMission > 0)
	{
		if (activeMission != MarbleData->GetVnum())
		{
			return;
		}
	}
	else
	{
		ch->SetMarbleActiveMission(MarbleData->GetVnum());
		ch->SetMarbleActiveExtandedCount(iExtandedCount + 1);
		SendMarbleInformation(ch, true);
		return;
	}

	if (activeMission == MarbleData->GetVnum())
	{
		auto bFinished = (ch->GetMarbleKilledMonsters() >= MarbleData->GetCount() * ch->GetMarbleActiveExtandedCount());
		if (bFinished)
		{
			auto iCount = ch->GetMarbleActiveExtandedCount();

			LPITEM mItem = ITEM_MANAGER::Instance().CreateItem(70104, iCount, 0, true);
			if (mItem)
			{
				mItem->SetSocket(0, MarbleData->GetVnum());
				mItem->SetSocket(1, 600);

				int iEmptyPos = ch->GetEmptyInventory(mItem->GetSize());
				if (iEmptyPos != -1)
				{
					ch->SetMarbleActiveMission(0);
					ch->SetMarbleActiveExtandedCount(0);
					ch->SetMarbleKilledMonsters(0);

					SetCooldownTime(ch, MarbleData->GetVnum(), iCount);
					mItem->AddToCharacter(ch, TItemPos(INVENTORY, iEmptyPos));
				}
				else
				{
					M2_DESTROY_ITEM(mItem);
					ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You don't have enough space in your inventory."));
				}
				SendMarbleInformation(ch, true);
			}
		}
	}
}

void CMarbleSystemManager::MissionDeactive(LPCHARACTER ch, std::uint16_t dwIndex, bool bForceDeactive)
{
	if (!ch || !ch->GetDesc())
	{
		return;
	}

	const auto MarbleData = FindSet(dwIndex);
	if (!MarbleData) return;

	auto activeMission = ch->GetMarbleActiveMission();

	// We have to be sure, if you delete any marble from mysql, you are able to deactive this anyway
	if (bForceDeactive && activeMission > 0)
	{
		ch->SetMarbleActiveMission(0);
		ch->SetMarbleActiveExtandedCount(0);
		ch->SetMarbleKilledMonsters(0);
		SendMarbleInformation(ch, true);
		return;
	}

	if (activeMission > 0 && activeMission == MarbleData->GetVnum())
	{
		ch->SetMarbleActiveMission(0);
		ch->SetMarbleActiveExtandedCount(0);
		ch->SetMarbleKilledMonsters(0);
		SendMarbleInformation(ch, true);

		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have successfully canceled your mission."));
		return;
	}

	if (activeMission > 0)
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("This mission is not active"));
	}
	else
	{
		ch->ChatPacket(CHAT_TYPE_INFO, LC_TEXT("You have no active mission."));
	}
}

void CMarbleSystemManager::CounterKills(LPCHARACTER ch, LPCHARACTER pkVictim)
{
	if (!ch || !pkVictim)
	{
		return;
	}

	auto activeMission = ch->GetMarbleActiveMission();
	auto bExtandedCount = ch->GetMarbleActiveExtandedCount();
	auto actuallyKilled = ch->GetMarbleKilledMonsters();

	auto marble = FindSetByMonsterVnum(pkVictim->GetRaceNum());
	if (!marble)
		return;

	if (activeMission == pkVictim->GetRaceNum())
	{
		if (actuallyKilled < marble->GetCount() * bExtandedCount)
		{
			ch->SetMarbleKilledMonsters(actuallyKilled + 1);
		}
	}

	SendMarbleInformation(ch, true);
}

time_t CMarbleSystemManager::GetCooldownTime(LPCHARACTER ch, DWORD dwIndex, bool bSubtract)
{
	std::string sFlag("marble_creator._delay_" + std::to_string(dwIndex));
	return (bSubtract) ? std::max<time_t>(ch->GetQuestFlag(sFlag) - get_global_time(), 0) : ch->GetQuestFlag(sFlag);
}

void CMarbleSystemManager::SetCooldownTime(LPCHARACTER ch, DWORD dwIndex, int timeIncrease)
{
	std::string sFlag("marble_creator._delay_" + std::to_string(dwIndex));
	const auto totalTime = (COOLDOWN_TIME * timeIncrease);

	return ch->SetQuestFlag(sFlag, get_global_time() + totalTime);
}

/*******************************************************************\
| [PUBLIC] Incoming Packet Functions
\*******************************************************************/

int CMarbleSystemManager::RecvClientPacket(BYTE bSubHeader, LPCHARACTER ch, const char* c_pData, size_t uiBytes)
{
	switch (bSubHeader)
	{
	case ECGMarbleSubHeaders::CG_MARBLE_MANAGER_OPEN:
	{
		SendMarbleInformation(ch);
		return 0;
	}
	break;

	case ECGMarbleSubHeaders::CG_MARBLE_MANAGER_ACTIVE:
	{
		if (uiBytes < sizeof(int) + sizeof(bool))
		{
			return -1;
		}

		int bCategory = *(int*)c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		int iExtandedCount = *(int*)c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		MissionActive(ch, bCategory, iExtandedCount);
		return sizeof(int) + sizeof(int);
	}
	break;

	case ECGMarbleSubHeaders::CG_MARBLE_MANAGER_DEACTIVE:
	{
		if (uiBytes < sizeof(int) + sizeof(bool))
		{
			return -1;
		}

		int bCategory = *(int*)c_pData;
		c_pData += sizeof(int);
		uiBytes -= sizeof(int);

		bool bForceDeactive = *(bool*)c_pData;
		c_pData += sizeof(bool);
		uiBytes -= sizeof(bool);

		MissionDeactive(ch, bCategory, bForceDeactive);
		return sizeof(int) + sizeof(bool);
	}
	break;
	}

	sys_err("invalid subheader %u", bSubHeader);
	return -1;
}

/*******************************************************************\
| [PUBLIC] Outgoing Packet Functions
\*******************************************************************/

void CMarbleSystemManager::SendClientPacket(LPDESC pkDesc, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	TPacketGCMarbleManager packet;
	packet.bHeader = HEADER_GC_MARBLE_MANAGER;
	packet.wSize = sizeof(packet) + iSize;
	packet.bSubHeader = bSubHeader;

	TEMP_BUFFER buf;
	buf.write(&packet, sizeof(packet));
	if (iSize)
	{
		buf.write(c_pvData, iSize);
	}

	pkDesc->Packet(buf.read_peek(), buf.size());
}

void CMarbleSystemManager::SendClientPacket(DWORD dwPID, BYTE bSubHeader, const void* c_pvData, size_t iSize)
{
	LPCHARACTER pkChr = CHARACTER_MANAGER::instance().FindByPID(dwPID);

	if (pkChr)
	{
		SendClientPacket(pkChr->GetDesc(), bSubHeader, c_pvData, iSize);
	}
	else
	{
		CCI* pkCCI = P2P_MANAGER::instance().FindByPID(dwPID);
		if (pkCCI)
		{
			pkCCI->pkDesc->SetRelay(pkCCI->szName);
			SendClientPacket(pkCCI->pkDesc, bSubHeader, c_pvData, iSize);
		}
		else
		{
			sys_err("cannot send client packet to pid %u subheader %hu [cannot find player]", dwPID, bSubHeader);
		}
	}
}
#endif
