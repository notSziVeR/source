#pragma once

#ifdef __ITEM_SHOP_ENABLE__
class CItemShopManager : public singleton<CItemShopManager>
{
	public:
		CItemShopManager();
		virtual ~CItemShopManager() {};

	public:
		// Initial functions
		void LoadCategories();
		void LoadItems();
		void LoadPromotions();
		void LoadSpecialOffers();

		// Reload function
		void ReloadService();

		// Player functions
		int GetPlayerCash(LPCHARACTER ch);
		void BuyItem(LPCHARACTER ch, const std::string & sItemHash);
		void BroadcastItemShop(LPCHARACTER ch);
		void AddViewer(LPCHARACTER ch);
		void RemoveViewer(LPCHARACTER ch);
		bool IsViewer(LPCHARACTER ch);

		// Configuration manager
		void RecvInputItemShopPacket(LPCHARACTER ch, const TItemShopUniversalPacket * pPacket);

		// Recv
		void RecvItemShopPacket(const TItemShopUniversalPacket * pPacket);

		//
		void ChargePlayer(LPCHARACTER ch, int iPrice);

	private:
		void AddNewItem(const TItemShopUniversalPacket * pPacket);
		void DeleteItem(const TItemShopUniversalPacket * pPacket);
		void AddCategory(const TItemShopCategory * pPacket);
		void DeleteCategory(const TItemShopCategory * pPacket);
		void EditItem(const TItemShopItem * pPacket);
		void AddDiscount(const TItemShopDiscount * pPacket);
		void DeleteDiscount(const TItemShopDiscount * pPacket);
		void AddSpecialOffer(const TItemShopItem * pPacket);
		void DeleteSpecialOffer(const TItemShopItem * pPacket);

		TItemShopItem * GetItemByHash(const std::string & sHash);
		TItemShopItem * GetSpecialOffer(const std::string & sHash);
		std::map<TItemShopCategory, std::vector<TItemShopItem>, TItemShopCategory::Hasher>::iterator GetCategoryByID(BYTE bID);
		const TItemShopCategory * GetCategoryByItemHash(const std::string & sHash);
		bool CategoryExists(BYTE bID);
		void SendItemShopPacket(const EItemShopPacketType & eType, const void * pPacket);
		bool CanBuyItem(LPCHARACTER ch);
		void NoticeSpecialOffer(const TItemShopUniversalPacket * pPacket);
		bool GiveItem(LPCHARACTER ch, TItemShopItem * pItem);
		void BroadcastViewerPacket(const EItemShopPacketType & eType, const TItemShopUniversalPacket * pPacket, const void * oldPack = nullptr);

	private:
		std::map<TItemShopCategory, std::vector<TItemShopItem>, TItemShopCategory::Hasher> m_itemshop_items;
		std::vector<TItemShopItem> v_special_offers;
		std::unordered_set<LPCHARACTER> us_viewers;
};
#endif

