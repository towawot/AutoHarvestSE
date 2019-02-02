#include "skse64/GameRTTI.h"
#include "EnchantmentItemEx.h"

UInt32 EnchantmentItemEx::GetGoldValue()
{
	if (!this)
		return 0;

	//static BSFixedString fEnchantmentPointsMult = "fEnchantmentPointsMult";
	//double fEPM = GetGameSettingFloat(fEnchantmentPointsMult);
	//static BSFixedString fEnchantmentEffectPointsMult = "fEnchantmentEffectPointsMult";
	//double fEEPM = GetGameSettingFloat(fEnchantmentEffectPointsMult);

	//double result = 0.0;
	//for (int index = 0; index < effectItemList.count; index++)
	//{
	//	MagicItem::EffectItem* pEI = nullptr;
	//	if (!effectItemList.GetNthItem(index, pEI))
	//		continue;

	//	if (this->data.deliveryType == 1)
	//		result += fEEPM * pEI->cost;
	//	else
	//		result += pEI->cost;
	//}
	//if (this->data.deliveryType == 1 && result > 0.0)
	//	result += fEPM * m_maxCharge;

	//return (UInt32)result;


	TESValueForm* pValue = DYNAMIC_CAST(this, TESForm, TESValueForm);
	if (!pValue)
		return 0;

#ifdef _DEBUG
	_MESSAGE("EnchantmentItemEx::GetGoldValue()  %d", pValue->value);
#endif

	return pValue->value;
}
