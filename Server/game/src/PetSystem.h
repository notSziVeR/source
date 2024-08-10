#ifndef	__HEADER_PET_SYSTEM__
#define	__HEADER_PET_SYSTEM__

#include <unordered_map>

class CHARACTER;
class CItem;

class CPetActor //: public CHARACTER
{
	friend class CHARACTER;

public:
	enum EPetOptions
	{
		EPetOption_Followable = 1 << 0,
		EPetOption_Mountable = 1 << 1,
		EPetOption_Summonable = 1 << 2,
		EPetOption_Combatable = 1 << 3,
	};

	CPetActor(CHARACTER& owner, uint32_t vnum, uint32_t options = EPetOption_Followable | EPetOption_Summonable);
	virtual ~CPetActor();

	CHARACTER& GetOwner() const { return m_owner; }
	uint32_t GetVnum() const { return m_dwVnum; }
	CHARACTER* GetCharacter() const { return m_pkChar; }

	bool HasOption(EPetOptions option) const { return m_dwOptions & option; }

	bool Mount();
	void Unmount();

	uint32_t Summon(std::string petName, bool bSpawnFar = false);
	void Unsummon();

	bool IsSummoned() const { return m_pkChar; }

	virtual bool Update();

protected:
	void OnDestroy() { m_pkChar = nullptr; }

	virtual bool UpdateFollowAI(); ///< ??? ????? AI ??
	virtual bool UpdatAloneActionAI(float fMinDist, float fMaxDist); ///< ?? ???? ?? ?? AI ??

	bool Follow(int32_t minDistance = 50) const;

	CHARACTER& m_owner;

	uint32_t m_dwVnum;
	uint32_t m_dwOptions;
	uint32_t m_dwLastActionTime;

	CHARACTER* m_pkChar; // Instance of pet(CHARACTER)
};

class CPetSystem
{
	friend EVENTFUNC(petsystem_update_event);

public:
	CPetSystem(CHARACTER& owner);
	~CPetSystem();

	bool Update();

	CPetActor* SummonItem(uint32_t mobVnum, uint32_t itemId,
						  bool bSpawnFar,
						  uint32_t options = CPetActor::EPetOption_Followable | CPetActor::EPetOption_Summonable);

	void UnsummonItem(uint32_t itemId);

	void RefreshOverlay(bool bTakeOut = false);
	void RefreshBuffs();

private:
	CHARACTER& m_owner; ///< ? ???? Owner

	// Pets owned by items
	std::unordered_map<uint32_t, std::unique_ptr<CPetActor>> m_itemPets;

	// Pet update
	uint32_t m_dwUpdatePeriod; ///< ???? ?? (ms??)
	uint32_t m_dwLastUpdateTime;
	LPEVENT m_pkPetSystemUpdateEvent;
};



#endif