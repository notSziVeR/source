#pragma once
class CActionRestricts
{
public:
	enum ERestrictions
	{
		RESTRICT_DROP_ITEM = 1, RESTRICT_USE_ITEM, RESTRICT_MESSENGER,
		RESTRICT_EXCHANGE, RESTRICT_GUILD_ADD, RESTRICT_PARTY_ADD,
		RESTRICT_WHISPER, RESTRICT_SAVE_LOCATION, MAX_ACTION_RESTRICT_NUM
	};
	enum ERestrictionTypes { RESTRICT_TYPE_COUNT, RESTRICT_TYPE_TIME, RESTRICT_TYPE_MAX };

	CActionRestricts(LPCHARACTER ch);
	~CActionRestricts() {};

	BYTE getterB(BYTE key, BYTE index);
	bool AddCounter(const ERestrictions& restrict);

	std::map<BYTE, std::array<BYTE, RESTRICT_TYPE_MAX>> eBlocksMap =
	{
		// Type, Count, blockTime
		{RESTRICT_DROP_ITEM, {10, 10}},
		{RESTRICT_USE_ITEM,  {25, 10}},
		{RESTRICT_MESSENGER, {5, 10}},
		{RESTRICT_EXCHANGE,  {20, 10}},
		{RESTRICT_GUILD_ADD, {5, 10}},
		{RESTRICT_PARTY_ADD, {5, 10}},
		{RESTRICT_WHISPER, {10, 10}},
		{RESTRICT_SAVE_LOCATION, {1, 30}},
	};

private:
	LPCHARACTER m_lpChar;
	std::array<BYTE, MAX_ACTION_RESTRICT_NUM> counters;
	std::array<DWORD, MAX_ACTION_RESTRICT_NUM> timers;

	const DWORD actionTimeLimit = 5;
	const BYTE	actionCounterLimit = 10;
};