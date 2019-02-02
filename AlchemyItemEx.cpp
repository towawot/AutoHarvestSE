#include "AlchemyItemEx.h"

#ifdef _DEBUG
#include <codecvt>
//適当にbitチェックさせる
#define _bit(x, n) (((x) >> (n)) & 1)
void bit(UInt32 flags)
{
	int i;
	UInt32 j = 1;
	for (i = 0; i <= 31; i++)
	{
		_MESSAGE("[%d:%08X(%d)] : %d", i, j, j, _bit(flags, i));
		j += j;
	}
}
#endif

UInt32 AlchemyItemEx::GetGoldValue()
{
	if (!this)
		return 0;

//#ifdef _DEBUG
//	bit(itemData.flags);
//#endif

	if (IsManualCalc())
		return itemData.value;

	double costPP = 0.0;
	for (int index = 0; index < effectItemList.count; index++)
	{
		MagicItem::EffectItem* pEI = nullptr;
		if (!effectItemList.GetNthItem(index, pEI))
			continue;

		costPP += pEI->cost;
	}

	UInt32 result = (costPP > 0) ? static_cast<UInt32>(costPP) : 0;
	return result;
}