#include "stdafx.h"
#ifdef __ENABLE_SAVE_POSITION__
#include "PositionsManagerHelper.hpp"
#include "char.h"
#include "desc_client.h"
#include "utils.h"
#include "sectree_manager.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "config.h"

namespace
{
const std::string mIndex("saved_position.map_index_");
const std::string mLocalX("saved_position.local_x");
const std::string mLocalY("saved_position.local_y");
const std::string mGlobalX("saved_position.global_x");
const std::string mGlobalY("saved_position.global_y");
}

PositionsManagerHelper::PositionsManagerHelper() {};
PositionsManagerHelper::~PositionsManagerHelper() {};

void PositionsManagerHelper::EmitPositions(LPCHARACTER& ch)
{
	InterpreterCommand(ch, EPositionsCommandInterpreter::INFO_INITIALIZE);
	InterpreterCommand(ch, EPositionsCommandInterpreter::INFO_POSITIONS_CLEAR);

	for (BYTE i = 0; i < EPositionsManagerHelper::MAX_SAVE_POSITIONS; ++i)
	{
		InterpreterCommand(ch, EPositionsCommandInterpreter::INFO_POSITIONS, i, ch->GetQuestFlag(mIndex + std::to_string(i)), ch->GetQuestFlag(mLocalX + std::to_string(i)), ch->GetQuestFlag(mLocalY + std::to_string(i)),
						   ch->GetQuestFlag(mGlobalX + std::to_string(i)), ch->GetQuestFlag(mGlobalY + std::to_string(i)));
	}
};

void PositionsManagerHelper::InterpreterCommand(LPCHARACTER& ch, const EPositionsCommandInterpreter& broadType, BYTE bPos, DWORD mapIndex, int localX, int localY, int globalX, int globalY)
{
	if (!ch)
	{
		return;
	}

	switch (broadType)
	{
	case EPositionsCommandInterpreter::INFO_INITIALIZE:
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "Positions_Initialize %d", EPositionsManagerHelper::MAX_SAVE_POSITIONS);
	}
	break;

	case EPositionsCommandInterpreter::INFO_POSITIONS:
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "Positions_AppendData %d %d %d %d %d %d", bPos, mapIndex, localX, localY, globalX, globalY);
	}
	break;

	case EPositionsCommandInterpreter::INFO_POSITIONS_CLEAR:
	{
		ch->ChatPacket(CHAT_TYPE_COMMAND, "Positions_ClearInterface");
	}
	break;
	}
}

void PositionsManagerHelper::SavePosition(LPCHARACTER& m_pkChar, BYTE bPos)
{
	if (m_pkChar->GetWarMap() || m_pkChar->GetDungeon() || g_bChannel == 99 || m_pkChar->GetMapIndex() == 113)
	{
		return;
	}

	switch (m_pkChar->GetMapIndex())
	{
	case 113:
		return;
		break;

	default:
		break;
	}

	LPSECTREE_MAP pMap = SECTREE_MANAGER::instance().GetMap(m_pkChar->GetMapIndex());
	if (!pMap)
	{
		return;
	}

#ifdef __ACTION_RESTRICTIONS__
	if (m_pkChar->GetActionRestrictions())
	{
		if (m_pkChar->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_SAVE_LOCATION) == false)
		{
			return;
		}
	}
#endif

	m_pkChar->SetQuestFlag(mIndex + std::to_string(bPos), m_pkChar->GetMapIndex());
	m_pkChar->SetQuestFlag(mLocalX + std::to_string(bPos), (m_pkChar->GetX() - pMap->m_setting.iBaseX) / 100);
	m_pkChar->SetQuestFlag(mLocalY + std::to_string(bPos), (m_pkChar->GetY() - pMap->m_setting.iBaseY) / 100);
	m_pkChar->SetQuestFlag(mGlobalX + std::to_string(bPos), m_pkChar->GetX() / 100);
	m_pkChar->SetQuestFlag(mGlobalY + std::to_string(bPos), m_pkChar->GetY() / 100);
	EmitPositions(m_pkChar);
};

void PositionsManagerHelper::ClearPosition(LPCHARACTER& m_pkChar, BYTE bPos)
{
	m_pkChar->SetQuestFlag(mIndex + std::to_string(bPos), 0);
	m_pkChar->SetQuestFlag(mLocalX + std::to_string(bPos), 0);
	m_pkChar->SetQuestFlag(mLocalY + std::to_string(bPos), 0);
	m_pkChar->SetQuestFlag(mGlobalX + std::to_string(bPos), 0);
	m_pkChar->SetQuestFlag(mGlobalY + std::to_string(bPos), 0);
	EmitPositions(m_pkChar);
};

void PositionsManagerHelper::TeleportPosition(LPCHARACTER& m_pkChar, BYTE bPos)
{
	if (!m_pkChar->CanWarp())
	{
		return;
	}

#ifdef __ACTION_RESTRICTIONS__
	if (m_pkChar->GetActionRestrictions())
	{
		if (m_pkChar->GetActionRestrictions()->AddCounter(CActionRestricts::RESTRICT_SAVE_LOCATION) == false)
		{
			return;
		}
	}
#endif

	if (m_pkChar->GetQuestFlag(mIndex + std::to_string(bPos)) > 0)
	{
		m_pkChar->WarpSet(m_pkChar->GetQuestFlag(mGlobalX + std::to_string(bPos)) * 100, m_pkChar->GetQuestFlag(mGlobalY + std::to_string(bPos)) * 100, m_pkChar->GetQuestFlag(mIndex + std::to_string(bPos)));
	}
};
#endif
