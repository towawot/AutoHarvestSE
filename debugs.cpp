#include "skse64/PapyrusVM.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameObjects.h"
#include "skse64/GameReferences.h"
#include "skse64/GameExtraData.h"
//#include "skse64/NiNodes.h"
#include "GameFormComponents.h"
#include "utils.h"
#include "objects.h"
#include "dataCase.h"
//#include "containerLister.h"
#include "TESFormEx.h"
//#include <string>
//#include <vector>
//#include <algorithm>    // std::sort

#include "debugs.h"

void DumpKeywordForm(BGSKeywordForm* keywordForm)
{
	if (keywordForm)
	{
		gLog.Indent();
		_MESSAGE("keywordForm - %p", keywordForm);
		gLog.Indent();

		for (UInt32 idx = 0; idx < keywordForm->numKeywords; ++idx)
		{
			if (keywordForm->keywords[idx])
				_MESSAGE("%s (%08x)", keywordForm->keywords[idx]->keyword.c_str(), keywordForm->keywords[idx]->formID);
		}

		gLog.Outdent();
		gLog.Outdent();
	}
}

void DumpKeyword(TESForm* pForm)
{
#ifdef _DEBUG
	if (!pForm)
		return;

	TESFormEx* pFormEx = static_cast<TESFormEx*>(pForm);
	if (!pFormEx)
		return;

	BGSKeywordForm* keywordForm = pFormEx->GetKeywordForm();
	if (keywordForm)
		DumpKeywordForm(keywordForm);
#endif
}

void DumpExtraData(BSExtraData* extraData)
{
#ifdef _DEBUG
	gLog.Indent();

	if (extraData)
	{
		_MESSAGE("extraData");

		gLog.Indent();

		while (extraData)
		{
			_MESSAGE("Check:[%03X]", extraData->GetType());

			TESObjectREFR * targetRef = nullptr;
			const char *className = GetObjectClassName(extraData);
			if (extraData->GetType() == kExtraData_Count)
				_MESSAGE("%02X %s (%d)", extraData->GetType(), className, ((ExtraCount*)extraData)->count);
			else if (extraData->GetType() == kExtraData_Charge)
				_MESSAGE("%02X %s (%0.2f)", extraData->GetType(), className, ((ExtraCharge*)extraData)->charge);
			else if (extraData->GetType() == kExtraData_LocationRefType)
			{
				_MESSAGE("%02X %s ([%s] %08X)", extraData->GetType(), className, ((ExtraLocationRefType*)extraData)->refType->keyword.c_str(), ((ExtraLocationRefType*)extraData)->refType->formID);
				//DumpClass(extraData, sizeof(ExtraLocationRefType)/8);
			}
			else if (extraData->GetType() == kExtraData_Ownership)
				_MESSAGE("%02X %s ([%s] %08X)", extraData->GetType(), className, ((ExtraOwnership*)extraData)->owner->GetName(), ((ExtraOwnership*)extraData)->owner->formID);
			else if (extraData->GetType() == kExtraData_AshPileRef)
			{
				UInt32 handle = ((ExtraAshPileRef*)extraData)->refHandle;
				if (LookupREFRByHandle(&handle, &targetRef))
					_MESSAGE("%02X %s AshRef(Handle)=%08X(%d)", extraData->GetType(), className, targetRef->formID, handle);
				else
					_MESSAGE("%02X %s AshRef(Handle)=?", extraData->GetType(), className);
			}
			//else if (extraData->GetType() == kExtraData_ItemDropper)
			//{
			//	UInt32 handle = ((ExtraItemDropper*)extraData)->handle;
			//	if (LookupREFRByHandle(&handle, &targetRef))
			//		_MESSAGE("%02X %s droppedRef(Handle)=%08X(%d)", extraData->GetType(), className, targetRef->formID, handle);
			//	else
			//		_MESSAGE("%02X %s droppedRef(Handle)=?", extraData->GetType(), className);
			//}
			//else if (extraData->GetType() == kExtraData_DroppedItemList)
			//{
			//	tList<UInt32>* list = &((ExtraDroppedItemList*)extraData)->handles;
			//	if (list)
			//	{
			//		for (int index = 0; index < list->Count(); index++)
			//		{
			//			UInt32* handle = list->GetNthItem(index);
			//			if (LookupREFRByHandle(&*handle, &targetRef))
			//				_MESSAGE("%02X %s droppedRef(Handle)=%08X(%d)", extraData->GetType(), className, targetRef->formID, &handle);
			//			else
			//				_MESSAGE("%02X %s droppedRef(Handle)=?", extraData->GetType(), className);
			//		}
			//	}
			//}
			else if (extraData->GetType() == kExtraData_ActivateRef)
			{
				ExtraActivateRef* exActivateRef = static_cast<ExtraActivateRef*>(extraData);
				DumpClass(exActivateRef, sizeof(ExtraActivateRef) / 8);
				//_MESSAGE("---------------");
				//DumpClass(exActivateRef->data, sizeof(ExtraActivateRef));

				_MESSAGE("%02X %s", extraData->GetType(), className);
			}
			else if (extraData->GetType() == kExtraData_ActivateRefChildren)
			{
				ExtraActivateRefChildren* exActivateRefChain = static_cast<ExtraActivateRefChildren*>(extraData);
				DumpClass(exActivateRefChain, sizeof(ExtraActivateRefChildren) / 8);

				_MESSAGE("%02X %s (%08X)", extraData->GetType(), className, ((ExtraActivateRefChildren*)extraData)->data->unk3->formID);
			}
			else if (extraData->GetType() == kExtraData_LinkedRef)
			{
				ExtraLinkedRef* exLinkRef = static_cast<ExtraLinkedRef*>(extraData);
				if (!exLinkRef)
					_MESSAGE("%02X %s  ERR?????", extraData->GetType(), className);
				else
				{
					UInt32 length = exLinkRef->Length();
					for (UInt32 index = 0; index < length; index++)
					{
						auto pair = exLinkRef->Get(index);
						if (!pair.refr)
							continue;

						if (!pair.keyword)
							_MESSAGE("%02X %s ([NULL] %08X) %d / %d", extraData->GetType(), className, pair.refr->formID, (index + 1), length);
						else
							_MESSAGE("%02X %s ([%s] %08X) %d / %d", extraData->GetType(), className, (pair.keyword)->keyword.c_str(), pair.refr->formID, (index + 1), length);
					}
				}

				//DumpClass(exLinkRef, sizeof(ExtraLinkedRef) / 8);

			}
			else
				_MESSAGE("%02X %s", extraData->GetType(), className);

			extraData = extraData->next;
		}

		gLog.Outdent();
	}

	gLog.Outdent();
#endif
}

void DumpItemVW(TESFormEx* itemEx)
{
#ifdef _DEBUG
	UInt32 worth = itemEx->GetWorth();
	UInt32 weight = itemEx->GetWeight();
	weight = weight > 0 ? weight : 1;

	double vw = (worth > 0 && weight > 0) ? worth / weight : 0.0;

	//_MESSAGE("GoldValue(%d)", itemEx->GetGoldValue());
	_MESSAGE("Worth(%d)  Weight(%d)  V/W(%0.2f)", worth, weight, vw);
#endif
}

void DumpReference(TESObjectREFRx *refr, const char* typeName)
{
#ifdef _DEBUG
	_MESSAGE("%08X %02X(%02d) [%s] - %s", refr->formID, refr->baseForm->formType, refr->baseForm->formType, CALL_MEMBER_FN(refr, GetReferenceName)(), typeName);

	TESFormEx* itemEx = static_cast<TESFormEx*>(refr->baseForm);
	DumpItemVW(itemEx);

	DumpKeyword(refr->baseForm);

	BSExtraData *extraData = refr->extraData.m_data;
	DumpExtraData(extraData);

	_MESSAGE("--------------------\n");
#endif
}

void DumpContainer(TESObjectREFRx *refr)
{
#ifdef _DEBUG
	_MESSAGE("%08X %02X(%02d) [%s]", refr->formID, refr->baseForm->formType, refr->baseForm->formType, CALL_MEMBER_FN(refr, GetReferenceName)());

	_MESSAGE("TESContainer--");

	TESContainer *container = refr->GetContainer();
	if (container)
	{
		gLog.Indent();

		TESContainer::Entry *entry;
		UInt32 idx = 0;

		while (container->GetContainerItemAt(idx++, entry))
		{
			TESFormEx *itemEx = static_cast<TESFormEx*>(entry->form);

			_MESSAGE("itemType:: %d:(%08x)", itemEx->formType, itemEx->formID);

			if (itemEx->formType == kFormType_LeveledItem)
			{
				_MESSAGE("%08X LeveledItem", itemEx->formID);
			}
			else
			{
				bool bPlayable = IsPlayable(itemEx);
				TESFullName *name = DYNAMIC_CAST(itemEx, TESForm, TESFullName);
				std::string typeName = GetObjectTypeName(ClassifyType(itemEx));

				_MESSAGE("%08X [%s] count=%d playable=%d  - %s", itemEx->formID, name->GetName(), entry->count, bPlayable, typeName.c_str());

				TESFormEx* itemEx = static_cast<TESFormEx*>(refr->baseForm);
				DumpItemVW(itemEx);

				DumpKeyword(itemEx);
			}
		}

		gLog.Outdent();
	}

	_MESSAGE("ExtraContainerChanges--");

	ExtraContainerChanges* exChanges = static_cast<ExtraContainerChanges*>(refr->extraData.GetByType(kExtraData_ContainerChanges));
	if (exChanges && exChanges->data->objList)
	{
		gLog.Indent();

		for (int idx = 0; idx < exChanges->data->objList->Count(); idx++)
		{
			InventoryEntryData * entryData = exChanges->data->objList->GetNthItem(idx);

			TESFormEx* itemEx = static_cast<TESFormEx*>(entryData->type);

			bool bPlayable = itemEx->IsPlayable();
			std::string typeName = GetObjectTypeName(ClassifyType(itemEx));
			TESFullName *name = DYNAMIC_CAST(itemEx, TESForm, TESFullName);
			_MESSAGE("- %08X [%s] %p count=%d playable=%d  - %s", itemEx->formID, name->GetName(), entryData, entryData->countDelta, bPlayable, typeName.c_str());

			DumpItemVW(itemEx);
			DumpKeyword(itemEx);

			if (!entryData->extendDataList)
			{
				_MESSAGE("extendDataList - not found");
				continue;
			}

			for (int j = 0; j < entryData->extendDataList->Count(); j++)
			{
				BaseExtraList * extraList = entryData->extendDataList->GetNthItem(j);
				if (!extraList)
					continue;

				_MESSAGE("extraList - %p", extraList);

				BSExtraData *extraData = extraList->m_data;
				if (!extraData)
					continue;

				DumpExtraData(extraData);
			}
		}
		gLog.Outdent();
		_MESSAGE("*");
	}

	_MESSAGE("ExtraDatas--");

	BSExtraData *extraData = refr->extraData.m_data;
	if (extraData)
		DumpExtraData(extraData);

	//if (extraData)
	//	_MESSAGE("| extraData");

	//while (extraData)
	//{
	//	const char *className = GetObjectClassName(extraData);
	//	if (extraData->GetType() == kExtraData_Count)
	//		_MESSAGE("|        %02X %s (%d)  %p", extraData->GetType(), className, ((ExtraCount*)extraData)->count, extraData);
	//	else
	//		_MESSAGE("|        %02X %s  %p", extraData->GetType(), className, extraData);

	//	extraData = extraData->next;
	//}

	_MESSAGE("--------------------\n");
#endif
}
