#pragma once

#include <unordered_map>

class TESObjectREFRx;

struct ContainerLister
{
public:
	ContainerLister(TESObjectREFRx* refr, SInt32 questObjDefinition) :m_refr(refr), m_questObjDefinition(questObjDefinition) {};
	bool GetOrCheckContainerForms(std::unordered_map<TESForm*, UInt32>* out, bool &hasQuestObject, bool &hasEnchItem);
private:
	TESObjectREFRx* m_refr;
	SInt32 m_questObjDefinition;
};

