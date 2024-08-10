#pragma once

#ifdef __ITEM_SHOP_ENABLE__
class CItemShopManager : public singleton<CItemShopManager>
{
	public:
		CItemShopManager();
		virtual ~CItemShopManager() {};

	public:
		void Process(bool bSkipCache = false);
		// Initial functions
		void LoadCategories();
		void LoadItems();
		void LoadPromotions();
		void LoadSpecialOffers();

		// Recv
		void RecvItemShopPacket(CPeer * pPeer, const TItemShopUniversalPacket * pPacket);

	private:
		// Configuration manager
		void AddNewItem(const TItemShopUniversalPacket * pPacket);
		void DeleteItem(const TItemShopUniversalPacket * pPacket);
		void AddCategory(const TItemShopCategory * pPacket);
		void DeleteCategory(const TItemShopCategory * pPacket);
		void EditItem(const TItemShopItem * pPacket);
		void AddDiscount(const TItemShopDiscount * pPacket);
		void DeleteDiscount(const TItemShopDiscount * pPacket);
		void AddSpecialOffer(const TItemShopItem * pPacket);
		void DeleteSpecialOffer(const TItemShopItem * pPacket);
		void CheckSpecialOfferAvailability(CPeer * pPeer, const TItemShopItem * pPacket);
		void UpdateSpecialOfferAvailability(const TItemShopItem * pPacket);

	private:
		TItemShopItem * GetItemByHash(const std::string & sHash);
		TItemShopItem * GetSpecialOffer(const std::string & sHash);
		std::map<TItemShopCategory, std::vector<TItemShopItem>, TItemShopCategory::Hasher>::iterator GetCategoryByID(BYTE bID);
		const TItemShopCategory * GetCategoryByItemHash(const std::string & sHash);
		bool CategoryExists(BYTE bID);
		void SendItemShopPacket(const EItemShopPacketType & eType, const void * pPacket, CPeer * pPeer = nullptr);

	private:
		time_t ttCacheNextUpdate;
		std::map<TItemShopCategory, std::vector<TItemShopItem>, TItemShopCategory::Hasher> m_itemshop_items;
		std::vector<TItemShopItem> v_special_offers;
};
#endif

