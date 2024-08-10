#ifndef __INC_METIN_II_GAME_SHOP_MANAGER_H__
#define __INC_METIN_II_GAME_SHOP_MANAGER_H__

class CShop;
typedef class CShop * LPSHOP;

class CShopManager : public singleton<CShopManager>
{
public:
	typedef std::map<DWORD, CShop *> TShopMap;

public:
	CShopManager();
	virtual ~CShopManager();

	bool	Initialize(TShopTable * table, int size);
	bool	InitializeShopEx();
	void	Destroy();
	void	DestroyShopEx();

	LPSHOP	Get(DWORD dwVnum);
	LPSHOP	GetByNPCVnum(DWORD dwVnum);

	bool	StartShopping(LPCHARACTER pkChr, LPCHARACTER pkShopKeeper, int iShopVnum = 0);
	void	StopShopping(LPCHARACTER ch);

#ifdef __ENABLE_MULTIPLE_BUY_SYSTEM__
	void	Buy(LPCHARACTER ch, BYTE pos, CountType wAmount = 1);
#else
	void	Buy(LPCHARACTER ch, BYTE pos);
#endif

	void	Sell(LPCHARACTER ch, WORD wCell, CountType bCount = 0, BYTE bType = 0);

	LPSHOP	CreatePCShop(LPCHARACTER ch, TShopItemTable* pTable, BYTE bItemCount);

	LPSHOP	FindPCShop(DWORD dwVID);
	void	DestroyPCShop(LPCHARACTER ch);

private:
	TShopMap	m_map_pkShop;
	TShopMap	m_map_pkShopByNPCVnum;
	TShopMap	m_map_pkShopByPC;

	bool	ReadShopTableEx(const char* stFileName);
};

#endif
