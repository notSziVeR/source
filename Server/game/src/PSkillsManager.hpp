#pragma once
namespace PSkillsManager
{
	// Sending the data
	void BroadcastData(LPCHARACTER& ch, bool bClear = true, int iNum = -1);

	// Character actions
	void RegisterHighscore(LPCHARACTER& ch, const uint8_t iType);
	void ComputePoints(LPCHARACTER ch);
	
	// Personal Data
	void SetPersonalHighScore(LPCHARACTER& ch, const uint8_t iType);
	int GetPersonalHighScore(LPCHARACTER& ch, const uint8_t iType);
}
