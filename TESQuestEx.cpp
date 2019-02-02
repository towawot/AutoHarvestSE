#include "skse64/GameData.h"
#include "skse64/PapyrusVM.h"
#include "TESQuestEx.h"

TESQuestEx* GetTargetQuest(const char* espName, UInt32 questID)
{
	static TESQuestEx* result = nullptr;
	if (!result)
	{
		UInt32 formID = 0;
		UInt32 idx = DataHandler::GetSingleton()->GetLoadedModIndex(espName);
		if (idx != 0xFF)
			formID = (idx << 24) | questID;

		if (formID != 0)
			result = static_cast<TESQuestEx*>(LookupFormByID(formID));
	}
	return result;
}

UInt64 TESQuestEx::GetAliasHandle(UInt64 index)
{
	static UInt64 result;
	if (result == 0)
	{
		if (this && this->IsRunning())
		{
			BGSBaseAlias* baseAlias;
			if (!this->aliases.GetNthItem(index, baseAlias))
				return 0;

			BGSRefAlias*  refAlias = static_cast<BGSRefAlias*>(baseAlias);
			if (!refAlias)
				return 0;

			IObjectHandlePolicy* policy = (*g_skyrimVM)->GetClassRegistry()->GetHandlePolicy();
			result = (policy) ? policy->Create(refAlias->kTypeID, refAlias) : 0;
		}
	}
	return result;
}

bool TESQuestEx::IsRunning()
{
	return (unk0D8.flags & 1) != 0 && (unk0D8.flags >> 7 & 1) == 0 && unk148 == 0;
}
