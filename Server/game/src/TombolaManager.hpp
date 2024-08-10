#pragma once

namespace TombolaEvent
{
	struct STombolaItem
	{
		DWORD dwVnum;
		CountType wCount;
		int iProbMin;
		int iProbMax;

		STombolaItem() : dwVnum{0}, wCount{0}, iProbMin{-1}, iProbMax{-1}
		{}
	};

	void LoadConfig();
	void ReloadConfig();
	void ChangeEventStatus(bool bStatus);
	void SpinWheel(LPCHARACTER ch);
	void ClaimReward(LPCHARACTER ch);
	void SetRandomPool(LPCHARACTER ch);
	void BroadcastConfig(LPCHARACTER ch);
}

