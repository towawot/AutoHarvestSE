#pragma once
#include "skse64/GameObjects.h"

class TESObjectARMOEx : public TESObjectARMO
{
public:
	EnchantmentItem* GetEnchantment(void);
	UInt32 GetGoldValue(void);
};
