#pragma once
#include <unordered_map>
#include <memory>
#include "ShipDefendDungeon.hpp"

class CDungeon;
class CHARACTER;

class CShipDefendDungeonManager : public singleton<CShipDefendDungeonManager>
{
	// Helper
	struct SShipDefendStruct
	{
		bool bParty;
		std::unique_ptr<CShipDefendDungeon> pDungeon;

		SShipDefendStruct() : bParty(false), pDungeon()
		{}

		SShipDefendStruct(const bool & _bParty, CShipDefendDungeon * _pDungeon) : bParty(_bParty), pDungeon(_pDungeon)
		{}

		SShipDefendStruct(SShipDefendStruct && o)
		{
			bParty = std::move(o.bParty);
			pDungeon = std::move(o.pDungeon);
		}

		SShipDefendStruct(const SShipDefendStruct & o) = delete;
	};

	public:
		CShipDefendDungeonManager();
		virtual ~CShipDefendDungeonManager();
		void Initialize();

	public:
		bool RegisterAttender(LPCHARACTER ch, LPDUNGEON pDungeon);
		void EraseAttender(LPCHARACTER ch);
		void RegisterHitRecord(LPCHARACTER pAttacker, LPCHARACTER pVictim);
		void MoveInitialState(LPCHARACTER pPC, LPCHARACTER pAllied);
		bool CheckForAllie(LPCHARACTER pAttacker, LPCHARACTER pVictim);
		void EraseDungeon(LPDUNGEON pDungeon);
		CShipDefendDungeon * FindDungeonByPID(LPCHARACTER ch);
		CShipDefendDungeon * FindDungeonByVID(LPCHARACTER ch);

	private:
		std::unordered_map<DWORD, SShipDefendStruct> m_attenders_list;
};

