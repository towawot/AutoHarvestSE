#include "skse64/PluginAPI.h"
//#include "skse64/GameRTTI.h"
//#include "skse64/GameData.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/GameEvents.h"
#include "skse64/GameReferences.h"
#include "skse64/PapyrusVM.h"
#include "events.h"


LootEventFunctor::LootEventFunctor(TESObjectREFR* refr, SInt32 type, SInt32 count, bool silent)
	: m_refr(refr), m_type(type), m_count(count), m_silent(silent)
{}

bool LootEventFunctor::Copy(Output* dst)
{
	VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
	dst->Resize(4);
	PackValue(dst->Get(0), &m_refr, registry);
	dst->Get(1)->SetInt(m_type);
	dst->Get(2)->SetInt(m_count);
	dst->Get(3)->SetBool(m_silent);
	return true;
}

ContainerLootEventFunctor::ContainerLootEventFunctor(TESObjectREFR* refr, TESForm* item, SInt32 type, SInt32 count, TESSound* sound, bool silent)
	: m_refr(refr), m_item(item), m_type(type), m_count(count), m_sound(sound), m_silent(silent)
{}

bool ContainerLootEventFunctor::Copy(Output* dst)
{
	VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
	dst->Resize(6);
	PackValue(dst->Get(0), &m_refr, registry);
	PackValue(dst->Get(1), &m_item, registry);
	dst->Get(2)->SetInt(m_type);
	dst->Get(3)->SetInt(m_count);
	PackValue(dst->Get(4), &m_sound, registry);
	dst->Get(5)->SetBool(m_silent);
	return true;
}

SupplementEventFunctor1::SupplementEventFunctor1(TESObjectREFR * refr, SInt32 mode)
	: m_refr(refr), m_mode(mode)
{}

bool SupplementEventFunctor1::Copy(Output * dst)
{
	//#ifdef _DEBUG
	//	_MESSAGE("SupplementEventFunctor1::Copy");
	//#endif

	VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
	dst->Resize(2);
	PackValue(dst->Get(0), &m_refr, registry);
	dst->Get(1)->SetInt(m_mode);

	return true;
}

SupplementEventFunctor2::SupplementEventFunctor2(TESObjectREFR * refr, SInt32 mode, SInt32 count)
	: m_refr(refr), m_mode(mode), m_count(count)
{}

bool SupplementEventFunctor2::Copy(Output * dst)
{
	//#ifdef _DEBUG
	//	_MESSAGE("SupplementEventFunctor1::Copy");
	//#endif

	VMClassRegistry * registry = (*g_skyrimVM)->GetClassRegistry();
	dst->Resize(3);
	PackValue(dst->Get(0), &m_refr, registry);
	dst->Get(1)->SetInt(m_mode);
	dst->Get(2)->SetInt(m_count);

	return true;
}

//namespace Events
//{
//	void init()
//	{
//		_MESSAGE("registers events");
//		static CombatEventHandler s_combatEventHander;
//		GetEventDispatcherList()->combatDispatcher.AddEventSink(&s_combatEventHander);
//		_MESSAGE("add event sink");
//	}
//}