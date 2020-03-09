#pragma once

#include "skse64/GameObjects.h"

class TESFormEx : public TESForm
{
public:
	BGSKeywordForm* GetKeywordForm(void);
	EnchantmentItem* GetEnchantment(void);
	SInt16 GetMaxCharge(void);
	UInt32 GetGoldValue(void);
	double GetWeight(void);
	double GetWorth(void);
};

bool IsPlayable(TESForm* pForm);
