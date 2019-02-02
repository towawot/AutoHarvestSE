#pragma once

#include "skse64/GameObjects.h"
//#include "skse64/GameBSExtraData.h"
#include "skse64/GameExtraData.h"

class BaseExtraListEx : public BaseExtraList
{
public:
	EnchantmentItem* GetEnchantment(void);
	bool IsQuestObject(SInt32 definition);
};
