#pragma once

//#include "skse64/GameThreads.h"

//#include "objects.h"
//#include "iniSettings.h"

enum
{
	subEvent_glowObject = 1,
	subEvent_glowLockChest,
	subEvent_enchantGlow,
	subEvent_chestAnimation,
	subEvent_containerUnlock,
};

enum
{
	subEvent2_replaceItems = 1,
};

class LootEventFunctor : public IFunctionArguments
{
public:
	LootEventFunctor(TESObjectREFR* refr, SInt32 type, SInt32 count, bool silent);
	virtual bool Copy(Output* dst);
private:
	TESObjectREFR* m_refr;
	SInt32 m_type;
	SInt32 m_count;
	bool m_silent;
};

class ContainerLootEventFunctor : public IFunctionArguments
{
public:
	ContainerLootEventFunctor(TESObjectREFR* refr, TESForm* item, SInt32 type, SInt32 count, TESSound* sound, bool silent);
	virtual bool Copy(Output* dst);
private:
	TESObjectREFR* m_refr;
	TESForm* m_item;
	SInt32 m_type;
	SInt32 m_count;
	TESSound* m_sound;
	bool m_silent;
};

class SupplementEventFunctor1 : public IFunctionArguments
{
public:
	SupplementEventFunctor1(TESObjectREFR* refr, SInt32 mode);
	virtual bool Copy(Output* dst);
private:
	TESObjectREFR* m_refr;
	SInt32 m_mode;
};

class SupplementEventFunctor2 : public IFunctionArguments
{
public:
	SupplementEventFunctor2(TESObjectREFR* refr, SInt32 mode, SInt32 count);
	virtual bool Copy(Output* dst);
private:
	TESObjectREFR* m_refr;
	SInt32 m_mode;
	SInt32 m_count;
};
