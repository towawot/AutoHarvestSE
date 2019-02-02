#include "skse64/GameRTTI.h"
#include "skse64/GameSettings.h"
#include "TESObjectWEAPEx.h"


SInt16 TESObjectWEAPEx::GetMaxCharge()
{
	return (this) ? enchantable.maxCharge : 0;
}

EnchantmentItem * TESObjectWEAPEx::GetEnchantment(void)
{
	return (this && enchantable.enchantment) ? enchantable.enchantment : nullptr;
}

UInt32 TESObjectWEAPEx::GetGoldValue(void)
{
	if (!this)
		return 0;

	static BSFixedString fEnchantmentPointsMult = "fEnchantmentPointsMult";
	double fEPM = GetGameSettingFloat(fEnchantmentPointsMult);
	static BSFixedString fEnchantmentEffectPointsMult = "fEnchantmentEffectPointsMult";
	double fEEPM = GetGameSettingFloat(fEnchantmentEffectPointsMult);

	TESValueForm* pValue = DYNAMIC_CAST(this, TESForm, TESValueForm);
	if (!pValue)
		return 0;

	EnchantmentItem* ench = this->GetEnchantment();
	if (!ench)
		return pValue->value;

	SInt16 charge = this->GetMaxCharge();
	UInt32 cost = ench->data.calculations.cost;
	return ((pValue->value * 2) + (fEPM * charge) + (fEEPM * cost));
	//return (pValue->value + (fEPM * charge) + (fEEPM * cost));
}

double GetGameSettingFloat(BSFixedString name)
{
	Setting* setting;
	SettingCollectionMap* settings = *g_gameSettingCollection;
	if (settings)
	{
		setting = settings->Get(name.data);
	}

	if (!setting || setting->GetType() != Setting::kType_Float)
		return 0.0;

	return setting->data.f32;
}
