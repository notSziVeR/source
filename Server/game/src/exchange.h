#ifndef __INC_METIN_II_GAME_EXCHANGE_H__
#define __INC_METIN_II_GAME_EXCHANGE_H__

class CGrid;

enum EExchangeValues
{
	EXCHANGE_ITEM_MAX_NUM = 24,
	EXCHANGE_MAX_DISTANCE = 1000
};

class CExchange
{
public:
	CExchange(LPCHARACTER pOwner);
	~CExchange();

	void		Accept(bool bIsAccept = true);
	void		Cancel();

#ifdef __EXTANDED_GOLD_AMOUNT__
	bool		AddGold(int64_t lGold);
#else
	bool		AddGold(long lGold);
#endif

	bool		AddItem(TItemPos item_pos, BYTE display_pos);
	bool		RemoveItem(BYTE pos);

	LPCHARACTER	GetOwner() { return m_pOwner; }
	CExchange* GetCompany() { return m_pCompany; }

	bool		GetAcceptStatus() { return m_bAccept; }

	void		SetCompany(CExchange* pExchange) { m_pCompany = pExchange; }
	BYTE		GetEmptySlot(const BYTE & bItemSize);

	int64_t		GetExchangingGold() const { return m_lGold; }

	void		SendInfo(bool isError, const char* format, ...);
	int			CountExchangingItems();

	int			GetLastCriticalUpdatePulse() const { return m_lLastCriticalUpdatePulse; };

private:
	bool		SanityCheck();
	bool		PerformTrade();
	bool		Done(DWORD tradeID, bool firstPlayer);
	bool		Check(int* piItemCount);
	bool		CheckSpace();

private:
	CExchange* m_pCompany;

	LPCHARACTER	m_pOwner;

	TItemPos		m_aItemPos[EXCHANGE_ITEM_MAX_NUM];
	LPITEM		m_apItems[EXCHANGE_ITEM_MAX_NUM];
	BYTE		m_abItemDisplayPos[EXCHANGE_ITEM_MAX_NUM];

	bool 		m_bAccept;
#ifdef __EXTANDED_GOLD_AMOUNT__
	int64_t		m_lGold;
#else
	long		m_lGold;
#endif
	int			m_lLastCriticalUpdatePulse;

	CGrid* m_pGrid;

};

#endif
