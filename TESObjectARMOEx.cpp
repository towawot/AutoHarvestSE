#include "skse64/GameRTTI.h"
#include "TESObjectARMOEx.h"


UInt32 TESObjectARMOEx::GetGoldValue()
{
	if (!this)
		return 0;

	TESValueForm* pValue = DYNAMIC_CAST(this, TESForm, TESValueForm);
	if (!pValue)
		return 0;

	EnchantmentItem* ench = this->GetEnchantment();
	if (!ench)
	{
#ifdef _DEBUG
		_MESSAGE("!ench");
#endif

		return pValue->value;
	}

	//	EnchantmentItemEx* enchEx = static_cast<EnchantmentItemEx*>(ench);
	//	if (!enchEx)
	//	{
	//#ifdef _DEBUG
	//		_MESSAGE("!enchEx");
	//#endif
	//		return pValue->value;
	//	}

	double costPP = 0.0;
	for (int index = 0; index < ench->effectItemList.count; index++)
	{
		MagicItem::EffectItem* pEI = nullptr;
		if (!ench->effectItemList.GetNthItem(index, pEI))
			continue;

		costPP += pEI->cost;
	}

	UInt32 result = (costPP > 0) ? static_cast<UInt32>(costPP) : 0;

#ifdef _DEBUG
	_MESSAGE("TESObjectARMOEx::GetGoldValue()  %d  %d", pValue->value, result);
#endif

	return pValue->value + result;
}


EnchantmentItem * TESObjectARMOEx::GetEnchantment(void)
{
	return (this && enchantable.enchantment) ? enchantable.enchantment : nullptr;
}
