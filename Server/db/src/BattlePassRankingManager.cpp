#include "stdafx.h"
#ifdef __BATTLE_PASS_ENABLE__
#include "Main.h"
#include "DBManager.h"
#include "Peer.h"
#include "ClientManager.h"
#include "BattlePassRankingManager.hpp"

#ifdef BOOST_NO_EXCEPTIONS
namespace boost
{
void throw_exception( std::exception const & e ) {};
}
#endif

namespace
{
const size_t stHighScoreCount = 10;
const time_t ttNextUpdateElapse = 60;
}

CBattlePassRankingManager::CBattlePassRankingManager() : eCurrentMonth{GetCurrentMonth()}, dwCurrentYear{GetCurrentYear()}, ttNextTime{time(0) + ttNextUpdateElapse}
{
	for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
		m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)] = std::vector<TBattlePassHighScore>(stHighScoreCount, TBattlePassHighScore{});

	std::string sQuery = "SELECT iDiffLevel, iMonth, stPos, sName, ttTime FROM player" + std::string(GetTablePostfix()) + ".battle_pass_ranking";
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sQuery.c_str()));

	MYSQL_ROW row;
	MYSQL_RES* pRes = msg->Get()->pSQLResult;

	if (msg->Get()->uiNumRows == 0)
	{
		return;
	}

	while ((row = mysql_fetch_row(pRes)))
	{
		TBattlePassHighScore pack{};
		pack.bSubHeader = SUBHEADER_BATTLEPASS_DG_ADD_ELEMENT;

		pack.iDiffLevel = strtoul(row[0], NULL, 10);
		pack.iMonth = strtoul(row[1], NULL, 10);
		pack.stPos = strtoul(row[2], NULL, 10);
		strlcpy(pack.sName, row[3], sizeof(pack.sName));
		pack.ttTime = strtoul(row[4], NULL, 10);

		m_highscores[static_cast<BattlePassNS::EDiffLevel>(pack.iDiffLevel)][pack.stPos] = pack;
	}
}

CBattlePassRankingManager::~CBattlePassRankingManager()
{
	Save();
}

void CBattlePassRankingManager::RecvGamePacket(CPeer * peer, const TBattlePassHighScore * pPack)
{
	if (!peer)
	{
		return;
	}

	// First, flush
	Flush();

	// Validate packet
	if (static_cast<BattlePassNS::EMonths>(pPack->iMonth) != eCurrentMonth && pPack->bSubHeader != SUBHEADER_BATTLEPASS_GD_REQUEST_ITEMS)
	{
		return;
	}

	switch (pPack->bSubHeader)
	{
	case SUBHEADER_BATTLEPASS_GD_ADD_ITEM:
	{
		auto & rRank = m_highscores[static_cast<BattlePassNS::EDiffLevel>(pPack->iDiffLevel)];
		auto fIt = std::find_if(rRank.begin(), rRank.end(), [&pPack] (const TBattlePassHighScore & rElement) { return (rElement.ttTime > pPack->ttTime || !rElement.ttTime); });

		if (fIt != rRank.end())
		{
			*fIt = *pPack;
			fIt->bSubHeader = SUBHEADER_BATTLEPASS_DG_ADD_ELEMENT;
			fIt->stPos = std::distance(rRank.begin(), fIt);
		}

		BroadcastElement(peer, static_cast<BattlePassNS::EDiffLevel>(pPack->iDiffLevel), fIt->stPos);
	}
	break;
	case SUBHEADER_BATTLEPASS_GD_REQUEST_ITEMS:
	{
		for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
		{
			for (size_t j = 0; j < m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)].size(); ++j)
			{
				BroadcastElement(peer, static_cast<BattlePassNS::EDiffLevel>(i), j);
			}
		}
	}
	break;
	default:
		sys_err("Unknown subheader: %d", pPack->bSubHeader);
		break;
	}
}

void CBattlePassRankingManager::BroadcastElement(CPeer * peer, const BattlePassNS::EDiffLevel & eLevel, const size_t iElement)
{
	if (iElement >= m_highscores[eLevel].size())
	{
		sys_err("Ranking overflow! Size: %u, Element: %d", m_highscores[eLevel].size(), iElement);
		return;
	}

	TBattlePassHighScore & pack = m_highscores[eLevel][iElement];
	if (!pack.ttTime)
		// Don't send empty records
	{
		return;
	}

	peer->EncodeHeader(HEADER_DG_BATTLEPASS_RANKING, 0, sizeof(pack));
	peer->Encode(&pack, sizeof(pack));
}

void CBattlePassRankingManager::Save()
{
	// Truncating old data
	std::string sQuery = "TRUNCATE player" + std::string(GetTablePostfix()) + ".battle_pass_ranking";
	std::unique_ptr<SQLMsg> msg(CDBManager::instance().DirectQuery(sQuery.c_str()));

	// Saving current
	for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
	{
		for (const auto & rElement : m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)])
		{
			if (!rElement.ttTime)
			{
				break;
			}

			sQuery = "INSERT INTO player" + std::string(GetTablePostfix()) + ".battle_pass_ranking VALUE (";
			// Diff Level
			sQuery += "'" + std::to_string(rElement.iDiffLevel) + "', ";
			// Month
			sQuery += "'" + std::to_string(rElement.iMonth) + "', ";
			// Position
			sQuery += "'" + std::to_string(rElement.stPos) + "', ";
			// Name
			sQuery += "'" + std::string(rElement.sName) + "', ";
			// Time
			sQuery += "'" + std::to_string(rElement.ttTime) + "')";

			msg.reset(CDBManager::instance().DirectQuery(sQuery.c_str()));
		}
	}
}

void CBattlePassRankingManager::Flush()
{
	if (ttNextTime <= time(0))
	{
		if (eCurrentMonth != GetCurrentMonth())
		{
			eCurrentMonth = GetCurrentMonth();
			dwCurrentYear = GetCurrentYear();

			// Wipe out all outdated records
			for (auto i = static_cast<int>(BattlePassNS::EDiffLevel::TYPE_EASY); i <= static_cast<int>(BattlePassNS::EDiffLevel::TYPE_HARD); ++i)
				m_highscores[static_cast<BattlePassNS::EDiffLevel>(i)] = std::vector<TBattlePassHighScore>(stHighScoreCount, TBattlePassHighScore{});

			TBattlePassHighScore pack{};
			pack.bSubHeader = SUBHEADER_BATTLEPASS_DG_WIPE_RANKING;
			CClientManager::instance().ForwardPacket(HEADER_DG_BATTLEPASS_RANKING, &pack, sizeof(pack));

			// Save it
			Save();
		}

		ttNextTime = time(0) + ttNextUpdateElapse;
	}
}
#endif

