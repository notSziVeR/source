#pragma once
#ifdef __ITEM_TOGGLE_SYSTEM__
class CITEM;

bool CanModifyItem(const CItem* item);
bool CanStack(const CItem* from, const CItem* to);
CountType GetItemMaxCount(const CItem* item);
CountType GetItemMaxCount(const uint32_t vnum);

//
// Toggle items
//

bool ActivateToggleItem(CHARACTER* ch, CItem* item, bool bIsLoad = false);
void DeactivateToggleItem(CHARACTER* ch, CItem* item);

void OnCreateToggleItem(CItem* item);
void OnLoadToggleItem(CHARACTER* ch, CItem* item);
void OnRemoveToggleItem(CHARACTER* ch, CItem* item);
bool OnUseToggleItem(CHARACTER* ch, CItem* item);

void	OnCreateItem(CItem* item, bool bGameMasterCreated = false);
bool	OnUseItem(CHARACTER* ch, CItem* item);
void	OnLoadItem(CHARACTER* ch, CItem* item);
void	OnRemoveItem(CHARACTER* ch, CItem* item);

void	ProcessAutoRecoveryItem(CHARACTER* ch, CItem* item);

CItem* FindToggleItem(CHARACTER* ch, bool active,
					  int32_t subType = -1,
					  int32_t group = -1,
					  CItem* except = nullptr);
#endif
