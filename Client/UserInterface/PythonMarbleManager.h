#pragma once
#include "StdAfx.h"

#ifdef ENABLE_MARBLE_CREATOR_SYSTEM
class CPythonMarbleManager : public CSingleton<CPythonMarbleManager>
{
private:
	struct SMarbleInformation
	{
		SMarbleInformation(BYTE c_bID, DWORD c_dwMarbleVnum, WORD c_wRequiredKillCount, DWORD c_bActiveMission, BYTE c_bActiveExtandedCount, WORD c_wKilledMonsters, time_t c_tCooldownTime)
			: bID(c_bID), dwMarbleVnum(c_dwMarbleVnum), wRequiredKillCount(c_wRequiredKillCount), bActiveMission(c_bActiveMission), bActiveExtandedCount(c_bActiveExtandedCount),
			wKilledMonsters(c_wKilledMonsters), tCooldownTime(c_tCooldownTime) {}
		BYTE	bID;
		DWORD	dwMarbleVnum;
		WORD	wRequiredKillCount;
		DWORD	bActiveMission;
		BOOL	bActiveExtandedCount;
		WORD	wKilledMonsters;
		time_t	tCooldownTime;
	};

	std::vector<std::shared_ptr<SMarbleInformation>> vMarbleContainer;

public:
	CPythonMarbleManager();
	virtual ~CPythonMarbleManager();

	void RegisterMarbleInfo(BYTE bID, DWORD dwMarbleVnum, WORD wRequiredKillCount, DWORD bActiveMission, BYTE bActiveExtandedCount, WORD wKilledMonsters, time_t tCooldownTime);
	void ClearMarbleData();

	size_t GetMarblesCount() const;
	SMarbleInformation* GetInfoByIndex(std::uint16_t dwArrayIndex) const;

};
#endif
