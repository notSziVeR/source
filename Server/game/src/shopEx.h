#ifndef __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#define __INC_METIN_II_GAME_SHOP_SECONDARY_COIN_H__
#include "typedef.h"
#include "shop.h"

struct SShopTable;
typedef struct SShopTableEx : SShopTable
{
	std::string name;
	EShopCoinType coinType;

	TShopItemExTable itemsEx[SHOP_HOST_ITEM_MAX_NUM]{ 0 };
} TShopTableEx;

class CGroupNode;

class CShopEx: public CShop
{
public:
	bool			Create(DWORD dwVnum, DWORD dwNPCVnum);
	bool			AddShopTable(TShopTableEx& shopTable);

	bool CheckFreeInventory(CHARACTER* ch, uint16_t pos, CountType amount,
		const TItemTable* item);

	virtual bool	AddGuest(LPCHARACTER ch, DWORD owner_vid, bool bOtherEmpire);
	virtual void	SetPCShop(LPCHARACTER ch) { return; }
	virtual bool	IsPCShop() { return false; }
#ifdef __EXTANDED_GOLD_AMOUNT__
	virtual int64_t		Buy(LPCHARACTER ch, BYTE pos, CountType wAmount = 1);
#else
	virtual int		Buy(LPCHARACTER ch, BYTE pos);
#endif
	virtual bool	IsSellingItem(DWORD itemID) { return false; }

	size_t			GetTabCount() { return m_vec_shopTabs.size(); }
private:
	std::vector <TShopTableEx> m_vec_shopTabs;
};
typedef CShopEx* LPSHOPEX;

#endif
