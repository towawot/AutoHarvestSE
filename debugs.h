#pragma once
#include "skse64/GameReferences.h"

class TESObjectREFRx;

void DumpExtraData(BSExtraData * extraData);
void DumpReference(TESObjectREFRx * refr, const char * typeName);
void DumpContainer(TESObjectREFRx * ref);
