#ifdef __ENABLE_MARBLE_CREATOR_SYSTEM__
#include "Parser.hpp"

class CMarbleSystemManager : public singleton<CMarbleSystemManager> {

	const int COOLDOWN_TIME = 60 * 15;

	public:
		CMarbleSystemManager() = default;
		virtual ~CMarbleSystemManager() = default;

	public:
		void Create();
		void Clear();
		bool Load(const std::string& fileName);

		const MarbleCollection::MarbleSet* FindSet(const uint8_t id) const;
		const MarbleCollection::MarbleSet* FindSetByMonsterVnum(const uint32_t vnum) const;

		// general functions
		void	SendMarbleInformation(LPCHARACTER ch, BOOL bUpdate = false);
		void	MissionActive(LPCHARACTER ch, std::uint16_t dwIndex, BYTE iExtandedCount);
		void	MissionDeactive(LPCHARACTER ch, std::uint16_t dwIndex, bool bForceDeactive);
		void	CounterKills(LPCHARACTER ch, LPCHARACTER pkVictim);

		time_t	GetCooldownTime(LPCHARACTER ch, DWORD dwIndex, bool bSubtract = false);
		void	SetCooldownTime(LPCHARACTER ch, DWORD dwIndex, int timeIncrease);

		// incoming packet functions
		int		RecvClientPacket(BYTE bSubHeader, LPCHARACTER ch, const char* c_pData, size_t uiBytes);

		// outgoing packet functions
		void	SendClientPacket(LPDESC pkDesc, BYTE bSubHeader, const void* c_pvData, size_t iSize);
		void	SendClientPacket(DWORD dwPID, BYTE bSubHeader, const void* c_pvData, size_t iSize);

	protected:
		std::map<uint8_t, MarbleCollection::MarbleSet> sets_;

};
#endif
