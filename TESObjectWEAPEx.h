#pragma once
#include "skse64/GameObjects.h"

class TESObjectWEAPEx : public TESObjectWEAP
{
public:
	SInt16 GetMaxCharge(void);
	EnchantmentItem* GetEnchantment(void);
	UInt32 GetGoldValue(void);
	bool IsPlayable(void) { return ((gameData.flags2 & gameData.kFlags_NotPlayable) == 0); }
};

double GetGameSettingFloat(BSFixedString name);
