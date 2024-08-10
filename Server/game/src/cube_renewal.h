#ifndef __CUBE_RENEWAL_H__
#define __CUBE_RENEWAL_H__

#include "packet.h"

namespace CCubeRenewalHelper
{
	// Disabled npc's
	struct SCubeDisabledNPC
	{
		DWORD dwVnum;
		BYTE bCategory;
		DWORD dwRequiredItem;
		WORD wRequiredItemCount;
	};

	void UpdateEventFlag(const std::string& sFlag, const int& iValue);
	void RegisterDisabledCattegory(const DWORD& dwVnum, const BYTE& bCategory, const DWORD& dwRequiredItem, const WORD& wRequiredItemCount);

	bool IsDisabledCategory(const uint32_t& dwNpcVnum, const std::string & sCategoryName);
	SCubeDisabledNPC* GetCattegoryToUnlock(const uint32_t& dwNpcVnum);

	const int GetAmount(const uint32_t& dwNpcVnum, const BYTE& iCategory, const DWORD& dwRequiredItem);
	int SetAmount(const uint32_t& dwNpcVnum, const BYTE& iCategory, const WORD& iRemovedCount);
};

class CCubeRenewal : public singleton<CCubeRenewal>
{
public:
	CCubeRenewal();
	virtual ~CCubeRenewal();

	void		Initialize(bool bIsReload = false);
	bool 		CubeLoad(const char *fileName);
	bool 		IsGoodCubeData(TCubeData *cubeData, DWORD dwNpcVnum);
	int 		GetCategoryIndexByName(std::string stCategoryName);
	void 		SendCraftFailPacket(LPCHARACTER pkChr, BYTE bErrorType, DWORD dwErrorArg);

	bool 		CubeOpen(LPCHARACTER pkChr, LPCHARACTER pkCubeNpc);
	void 		CubeClose(LPCHARACTER pkChr);
	void 		CubeMake(LPCHARACTER pkChr, const char * pcData);
private:
	typedef std::vector<TCubeData*>	TCubeVector;
	typedef std::map<DWORD, TCubeVector> TCubeMap;

	TCubeMap sCubeProto;
};
#endif
