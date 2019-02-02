#include "skse64/GameRTTI.h"
#include "skse64/GameExtraData.h"
#include "objects.h"
#include "TESFormEx.h"
#include "BaseExtraListEx.h"
#include "containerLister.h"

bool ContainerLister::GetOrCheckContainerForms(std::unordered_map<TESForm*, UInt32>* out, bool &hasQuestObject, bool &hasEnchItem)
{
	if (!m_refr)
		return false;

	TESContainer *container = m_refr->GetContainer();
	if (container)
	{
		UInt32 idx = 0;
		TESContainer::Entry *entry;
		while (container->GetContainerItemAt(idx++, entry))
		{
			TESForm *item = entry->form;
			if (item->formType == kFormType_LeveledItem)
				continue;

			if (entry->count <= 0)
				continue;

			if (!item->IsPlayable())
				continue;

			TESFullName *fullName = DYNAMIC_CAST(item, TESForm, TESFullName);
			if (!fullName || strlen(fullName->GetName()) == 0)
				continue;

			(*out)[item] = entry->count;
		}
	}

	ExtraContainerChanges* exChanges = static_cast<ExtraContainerChanges*>(m_refr->extraData.GetByType(kExtraData_ContainerChanges));
	if (exChanges && exChanges->data->objList)
	{
		for (int idx = 0; idx < exChanges->data->objList->Count(); idx++)
		{
			InventoryEntryData * entryData = exChanges->data->objList->GetNthItem(idx);
			TESForm *item = entryData->type;

			if (!item->IsPlayable())
				continue;

			TESFullName *fullName = DYNAMIC_CAST(item, TESForm, TESFullName);
			if (!fullName || strlen(fullName->GetName()) == 0)
				continue;

			UInt32 itemCount = ((*out)[item] >= 1) ? (*out)[item] + entryData->countDelta : entryData->countDelta;
			if (itemCount >= 1)
				(*out)[item] = itemCount;

			//CheckSection
			for (int j = 0; j < entryData->extendDataList->Count(); j++)
			{
				BaseExtraList * extraList = entryData->extendDataList->GetNthItem(j);
				if (extraList)
				{

					BaseExtraListEx* exListEx = static_cast<BaseExtraListEx*>(extraList);
					if (exListEx)
					{
						if (!hasQuestObject)
							hasQuestObject = exListEx->IsQuestObject(m_questObjDefinition) ? true : false;

						if (!hasEnchItem)
							hasEnchItem = (exListEx->GetEnchantment()) ? true : false;
					}

					TESFormEx* itemEx = static_cast<TESFormEx*>(item);
					if (itemEx)
					{
						if (!hasEnchItem)
							hasEnchItem = (itemEx->GetEnchantment()) ? true : false;;
					}
				}
			}
		}
	}
	return out->size() > 0;
}
