#pragma once
#include "skse64/GameObjects.h"

class TESQuestEx : public TESQuest
{
public:
	UInt64 GetAliasHandle(UInt64 index);
	bool IsRunning(void);
};

TESQuestEx* GetTargetQuest(const char* espName, UInt32 questID);
