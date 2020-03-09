#pragma once

#include "skse64_common/Utilities.h"
#include "skse64/GameTypes.h"
#include "skse64/GameRTTI.h"

extern bool g_FirstRunForceDisable;

namespace papyrus
{
	void RegisterFuncs(VMClassRegistry* vm);
}
