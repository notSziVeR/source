#ifdef __TRANSMUTATION_SYSTEM__
#ifndef __TRANSMUTATION_HEADER__
#define __TRANSMUTATION_HEADER__
#include "Transmutation_extra.h"

namespace Transmutation
{
enum TRANSMITATION_GOLD : long long
{
	TRANSMITATION_COST = 1000000000LL,
};

bool CheckPair(TransMutation_Struct * trans_item_point, TransMutation_Struct * base_item_point);
bool CheckItem(LPITEM trans_item, BYTE gSlot);
void DoTransmutation(TransMutation_Struct * trans_item_point, TransMutation_Struct * base_item_point);
void RemoveTransmutation(LPITEM item_use, LPITEM item_trans);
}
#endif
#endif

