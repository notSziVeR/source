#pragma once
#include "item.h"

struct fishing_info
{
	DWORD vnum;
	DWORD dead_vnum;
	DWORD grilled_vnum;
	DWORD prob_min;
	DWORD prob_max;
	DWORD max_length;
};

namespace fishing
{
enum
{
	CAMPFIRE_MOB = 12000,
};

EVENTINFO(fishing_event_info)
{
	DWORD	pid;
	int		step;
	DWORD	hang_time;
	int		fish_id;
	BYTE	hang_count;

	fishing_event_info()
		: pid(0)
		, step(0)
		, hang_time(0)
		, fish_id(0)
		, hang_count(0)
	{
	}
};
extern LPEVENT CreateFishingEvent(LPCHARACTER ch);
}

extern BYTE MAX_ROD_LEVEL;

class CFishing : public singleton<CFishing>
{
private:
	// vector for confing
	std::vector<fishing_info> fishing_conf;
	DWORD	glob_max_prob;

	// functions
	void	FishingSuccess(LPCHARACTER ch);
	BYTE	GetFishingChance(LPCHARACTER ch);
	BYTE	GetRodBonus(LPCHARACTER ch);
	BYTE	GetRodLevel(DWORD rod_level);
	DWORD	RandomFish();
	void	FishingPractise(LPCHARACTER ch);
	DWORD	GetGrilledVnum(LPITEM item);
	DWORD	GetDeadVnum(LPITEM item);
	DWORD	GetFishMaxLenght(LPITEM item);
	DWORD	FishingSetLenght(LPITEM item, LPCHARACTER ch);
	void	RemoveBait(LPCHARACTER ch);

public:
	CFishing();
	~CFishing();
	void	Initialize();
	void	Reload();
	void	Take(fishing::fishing_event_info* info, LPCHARACTER ch);
	void	FishingReact(LPCHARACTER ch);
	void	Grill(LPCHARACTER ch, LPITEM item);
	void	UseFish(LPCHARACTER ch, LPITEM item);
	void	FishingFail(LPCHARACTER ch);
};