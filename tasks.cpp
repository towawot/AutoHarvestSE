#include "skse64/PluginAPI.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusArgs.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/GameReferences.h"
#include "skse64/GameExtraData.h"
#include "skse64/GameThreads.h"

#include "events.h"
#include "tasks.h"
#include "TESFormEx.h"
#include "objects.h"
#include "dataCase.h"
#include "iniSettings.h"
#include "containerLister.h"
#include "basketfile.h"
#include "sound.h"
#include "debugs.h"

#include "TESQuestEx.h"


SKSETaskInterface* g_task = nullptr;

INIFile* TaskBase::m_ini = nullptr;
UInt64 TaskBase::m_aliasHandle = 0;

TaskBase::TaskBase()
{
	if (!m_ini)
	{
		m_ini = INIFile::GetInstance();
	}

	if (m_aliasHandle == 0)
	{
		TESQuestEx* quest = GetTargetQuest(ESP_NAME, QUEST_ID);
		m_aliasHandle = (quest) ? quest->GetAliasHandle(0) : 0;
	}
}

void SearchTask::Dispose()
{
	delete this;
}

//void ContainerLootTask::Dispose()
//{
//	delete this;
//}

SearchTask::SearchTask(TESObjectREFR* refr, double distance, INIFile::PrimaryType first, INIFile::SecondaryType second)
	: m_refr(refr), m_distance(distance), m_first(first), m_second(second)
{
}

void SearchTask::Run()
{
	//#ifdef _DEBUG
	//	_MESSAGE("LootTask::Run");
	//#endif

	DataCase* data = DataCase::GetInstance();
	VMClassRegistry* registry = (*g_skyrimVM)->GetClassRegistry();

	static const BSFixedString eventName = "OnAutoHarvest";
	static const BSFixedString eventNameContainerLoot = "OnAutoHarvestContainerLoot";
	static const BSFixedString supplementEvent1 = "OnAutoHarvestSupplement";
	static const BSFixedString supplementEvent2 = "OnAutoHarvestSupplement2";

	if (!data->LockTask(m_refr))
		return;

	TESObjectREFRx* refrEx = static_cast<TESObjectREFRx*>(m_refr);
	if (!refrEx)
	{
#ifdef _DEBUG
		_MESSAGE("Cast Error");
#endif
		return;
	}

	if (m_second == INIFile::SecondaryType::itemObjects)
	{
		ObjectType objType = refrEx->GetObjectType();
		std::string typeName = GetObjectTypeName(objType);

#ifdef _DEBUG
		_MESSAGE("typeName  %s", typeName.c_str());
		DumpReference(refrEx, typeName.c_str());
#endif

		if (objType == ObjectType::unknown)
		{
#ifdef _DEBUG
			_MESSAGE("objType == ObjectType::unknown\nreturn");
#endif
			data->lists.blockForm.insert(m_refr->baseForm);
			return;
		}

		if (BasketFile::GetSingleton()->IsinList(BasketFile::EXCLUDELIST, m_refr->baseForm))
		{
#ifdef _DEBUG
			_MESSAGE("m_refr->baseForm >= 1\nreturn");
#endif
			data->lists.blockForm.insert(m_refr->baseForm);
			return;
		}

		SInt32 questObjectDefinition = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectDefinition"));
		SInt32 questObjectBehavior = refrEx->IsQuestItem(questObjectDefinition) ? static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectGlow")) : -1;
		if (questObjectBehavior == 0)
			return;
		else if (questObjectBehavior == 1)
		{
			SupplementEventFunctor1 argGlow(m_refr, subEvent_glowObject);
			registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
			return;
		}

		if (objType == ObjectType::ammo)
		{
			double pos = refrEx->GetPosValue();
			if (pos == -1.0)
			{
#ifdef _DEBUG
				_MESSAGE("err %0.2f", pos);
#endif
				data->lists.blockRefr.insert(m_refr);
				return;
			}

			if (data->lists.arrowCheck.count(m_refr) == 0)
			{
#ifdef _DEBUG
				_MESSAGE("pick %0.2f", pos);
#endif
				data->lists.arrowCheck.insert(std::make_pair(m_refr, pos));
				data->UnlockTask(m_refr);
				return;
			}
			else
			{
				double prev = data->lists.arrowCheck.at(m_refr);
				if (prev != pos)
				{
#ifdef _DEBUG
					_MESSAGE("moving %0.2f  prev:%0.2f", pos, prev);
#endif
					data->lists.arrowCheck[m_refr] = pos;
					data->UnlockTask(m_refr);
					return;
				}
				else
				{
#ifdef _DEBUG
					_MESSAGE("catch %0.2f", pos);
#endif
					data->lists.arrowCheck.erase(m_refr);
				}
			}
		}

		SInt32 value = m_ini->GetObjectSetSettings(m_first, m_second, typeName.c_str());
		if (value == 0)
		{
#ifdef _DEBUG
			_MESSAGE("value == 0\nreturn");
#endif
			data->lists.blockRefr.insert(m_refr);
			return;
		}

#ifdef _DEBUG
		_MESSAGE("--------------------");
#endif
		double valueVW = m_ini->GetObjectSetSettings(m_first, INIFile::SecondaryType::valueWeight, typeName.c_str());
#ifdef _DEBUG
		_MESSAGE("--------------------");
#endif

		if (valueVW > 0)
		{
			double worth = refrEx->GetWorth();
			double weight = refrEx->GetWeight();
			weight = (weight > 0) ? weight : 1;

			double vw = (worth > 0 && weight > 0) ? worth / weight : 0.0;

#ifdef _DEBUG
			const char* name = CALL_MEMBER_FN(refrEx, GetReferenceName)();
			TESFormEx* formEx = static_cast<TESFormEx*>(m_refr->baseForm);
			double worthB = formEx->GetWorth();
			double weightB = formEx->GetWeight();

			_MESSAGE("* %s(%08x)", name, refrEx->formID);
			_MESSAGE("base->worth=%0.2f  weight=%0.2f", worthB, weightB);
			_MESSAGE("refr->worth=%0.2f  weight=%0.2f", worth, weight);
			_MESSAGE("VW=%0.2f valueVW=%0.2f",  vw, valueVW);
			_MESSAGE("* ");
#endif

			if (vw < valueVW)
			{
#ifdef _DEBUG
				_MESSAGE("vw < valueVW\nreturn");
#endif
				data->lists.blockRefr.insert(m_refr);
				return;
			}
		}

		bool isSilent = (m_ini->GetObjectSetSettings(m_first, m_second, typeName.c_str()) == 1);

		if (refrEx->GetItemCount() >= 2)
		{
			SupplementEventFunctor2 argCount(m_refr, subEvent2_replaceItems, refrEx->GetItemCount() - 1);
			registry->QueueEvent(m_aliasHandle, &supplementEvent2, &argCount);
		}

#ifdef _DEBUG
		_MESSAGE("queue event");
#endif

		LootEventFunctor args(m_refr, static_cast<SInt32>(objType), refrEx->GetItemCount(), isSilent);
		registry->QueueEvent(m_aliasHandle, &eventName, &args);

		return;
	}
	else if (m_second == INIFile::SecondaryType::containers)
	{
		//__DebugMessage("| Task::Container");

#ifdef _DEBUG
		DumpContainer(refrEx);
#endif
		std::unordered_map<TESForm*, UInt32> itemMap;

		SInt32 questitemDefinition = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectDefinition"));

		bool hasQuestObject = false;
		bool hasEnchantItem = false;
		ContainerLister fnOption(refrEx, questitemDefinition);

		//__DebugMessage("| Task::Container::GetOrCheckContainerForms");

		if (!fnOption.GetOrCheckContainerForms(&itemMap, hasQuestObject, hasEnchantItem))
			return;

		bool glowEventState = false;

		//__DebugMessage("| Task::Container::IsContainerLocked");

		if (IsContainerLocked(m_refr))
		{
			SInt32 lockedChestGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "lockedChestGlow"));
			if (lockedChestGlow == 1)
			{
				glowEventState = true;
				SupplementEventFunctor1 argGlow(m_refr, subEvent_glowObject);
				registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
			}

			SInt32 lockedChestLoot = static_cast<int>(m_ini->GetConfigSettings(m_first, "lockedChestLoot"));
			if (lockedChestLoot == 0)
				return;
		}

		//__DebugMessage("| Task::Container::IsBossContainer");

		if (IsBossContainer(m_refr))
		{
			if (!glowEventState)
			{
				SInt32 bossChestGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "bossChestGlow"));
				if (bossChestGlow == 1)
				{
					glowEventState = true;
					SupplementEventFunctor1 argGlow(m_refr, subEvent_glowObject);
					registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
				}
			}

			SInt32 bossChestLoot = static_cast<int>(m_ini->GetConfigSettings(m_first, "bossChestLoot"));
			if (bossChestLoot == 0)
				return;
		}

		//__DebugMessage("| Task::Container::questObjectGlow");

		SInt32 questObjectGlow = -1;
		if (hasQuestObject)
		{
			if (!glowEventState)
			{
				questObjectGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectGlow"));
				if (questObjectGlow == 1)
				{
					glowEventState = true;
					SupplementEventFunctor1 argGlow(m_refr, subEvent_glowObject);
					registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
				}
			}

			SInt32 questObjectLoot = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectLoot"));
			if (questObjectLoot == 0)
				return;
		}

		//__DebugMessage("| Task::Container::enchantItemGlow");

		SInt32 enchantItemGlow = -1;
		if (hasEnchantItem)
		{
			enchantItemGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "enchantItemGlow"));
			if (enchantItemGlow == 1 && !glowEventState)
			{
				SupplementEventFunctor1 argGlow(m_refr, subEvent_enchantGlow);
				registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
			}
		}

		
		//__DebugMessage("| Task::Container::ItemCheckLoop");


		int count = 0;
		for (auto node : itemMap)
		{
			TESFormEx* itemEx = static_cast<TESFormEx*>(node.first);
			UInt32 maxCount = node.second;

			if (!itemEx)
				continue;

			if (maxCount == 0)
				continue;

			if (BasketFile::GetSingleton()->IsinList(BasketFile::EXCLUDELIST, node.first))
			{
				data->lists.blockForm.insert(node.first);
				continue;
			}

			//__DebugMessage("| Task::Container::ItemCheckLoop::ClassifyType");

			ObjectType objType = ClassifyType(itemEx);
			std::string typeName = GetObjectTypeName(objType);

			SInt32 value = m_ini->GetObjectSetSettings(m_first, m_second, typeName.c_str());
			if (value == 0)
			{
				data->lists.blockRefr.insert(m_refr);
				continue;
			}

			//__DebugMessage("| Task::Container::ItemCheckLoop::valueVW");

			double valueVW = m_ini->GetObjectSetSettings(m_first, INIFile::SecondaryType::valueWeight, typeName.c_str());
				
			if (valueVW > 0)
			{
				UInt32 worth = itemEx->GetWorth();
				UInt32 weight = itemEx->GetWeight();
				weight = (weight > 0) ? weight : 1;
				double vw = (worth > 0 && weight > 0) ? worth / weight : 0.0;

				if (vw < valueVW)
				{
					data->lists.blockRefr.insert(m_refr);
					continue;
				}
			}

			//__DebugMessage("| Task::Container::ItemCheckLoop::CreatePickupSound");

			TESSound* sound = CreatePickupSound(itemEx);
			if (!sound)
			{
#ifdef _DEBUG
				_MESSAGE("  null sound data");
#endif
			}

			bool isSilent = (value == 1);

			//__DebugMessage("| Task::Container::ItemCheckLoop::ContainerLootEventFunctor");

			ContainerLootEventFunctor args(m_refr, itemEx, static_cast<SInt32>(objType), maxCount, sound, isSilent);
			registry->QueueEvent(m_aliasHandle, &eventNameContainerLoot, &args);
			count += 1;

			//__DebugMessage("| Task::Container::ItemCheckLoop::next");
		}

		if (count >= 1)
		{
			//__DebugMessage("| Task::Container::GetTimeController");

			int disableContainerAnimation = static_cast<int>(m_ini->GetConfigSettings(m_first, "disableContainerAnimation"));
			if (disableContainerAnimation != 1)
			{
				if (refrEx->GetTimeController())
				{
					SupplementEventFunctor1 argAnim(m_refr, subEvent_chestAnimation);
					registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argAnim);
				}
			}
		}

		return;
	}
	else if (m_second == INIFile::SecondaryType::deadbodies)
	{
		//__DebugMessage("| Task::deadbodies");

#ifdef _DEBUG
		DumpContainer(refrEx);
#endif
		std::unordered_map<TESForm*, UInt32> itemMap;

		//__DebugMessage("| Task::deadbodies::questitemDefinition");

		SInt32 questitemDefinition = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectDefinition"));

		bool hasQuestObject = false;
		bool hasEnchantItem = false;

		//__DebugMessage("| Task::deadbodies::ContainerLister");

		ContainerLister fnOption(refrEx, questitemDefinition);

		//__DebugMessage("| Task::deadbodies::GetOrCheckContainerForms");

		if (!fnOption.GetOrCheckContainerForms(&itemMap, hasQuestObject, hasEnchantItem))
			return;

		//__DebugMessage("| Task::deadbodies::glowEventState");

		bool glowEventState = false;

		SInt32 questObjectGlow = -1;
		if (hasQuestObject)
		{
			if (!glowEventState)
			{
				questObjectGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectGlow"));
				if (questObjectGlow == 1)
				{
					//__DebugMessage("| Task::deadbodies::glowEventState::SupplementEventFunctor1");

					glowEventState = true;
					SupplementEventFunctor1 argGlow(m_refr, subEvent_glowObject);
					registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
				}
			}

			//__DebugMessage("| Task::deadbodies::glowEventState::questObjectLoot");

			SInt32 questObjectLoot = static_cast<int>(m_ini->GetConfigSettings(m_first, "questObjectLoot"));
			if (questObjectLoot == 0)
				return;
		}

		//__DebugMessage("| Task::deadbodies::glowEventState::enchantItemGlow");

		SInt32 enchantItemGlow = -1;
		if (hasEnchantItem)
		{
			enchantItemGlow = static_cast<int>(m_ini->GetConfigSettings(m_first, "enchantItemGlow"));
			if (enchantItemGlow == 1 && !glowEventState)
			{
				//__DebugMessage("| Task::deadbodies::glowEventState::enchantItemGlow::SupplementEventFunctor1");

				SupplementEventFunctor1 argGlow(m_refr, subEvent_enchantGlow);
				registry->QueueEvent(m_aliasHandle, &supplementEvent1, &argGlow);
			}
		}

		//__DebugMessage("| Task::deadbodies::ItemCheckLoop");

		for (auto node : itemMap)
		{
			TESFormEx* itemEx = static_cast<TESFormEx*>(node.first);
			UInt32 maxCount = node.second;

			if (!itemEx)
				continue;

			if (maxCount == 0)
				continue;

			//__DebugMessage("| Task::deadbodies::ItemCheckLoop::ClassifyType");

			if (BasketFile::GetSingleton()->IsinList(BasketFile::EXCLUDELIST, node.first))
			{
				data->lists.blockForm.insert(node.first);
				continue;
			}

			ObjectType objType = ClassifyType(itemEx);
			std::string typeName = GetObjectTypeName(objType);

			SInt32 value = m_ini->GetObjectSetSettings(m_first, m_second, typeName.c_str());
			if (value == 0)
			{
				data->lists.blockRefr.insert(m_refr);
				continue;
			}

			double valueVW = m_ini->GetObjectSetSettings(m_first, INIFile::SecondaryType::valueWeight, typeName.c_str());
			if (valueVW > 0)
			{
				UInt32 worth = itemEx->GetWorth();
				UInt32 weight = itemEx->GetWeight();
				weight = (weight > 0) ? weight : 1;
				double vw = (worth > 0 && weight > 0) ? worth / weight : 0.0;

				if (vw < valueVW)
				{
					data->lists.blockRefr.insert(m_refr);
					continue;
				}
			}

			//__DebugMessage("| Task::deadbodies::ItemCheckLoop::CreatePickupSound");

			TESSound* sound = CreatePickupSound(itemEx);
			if (!sound)
			{
#ifdef _DEBUG
				_MESSAGE("  null sound data");
#endif
			}
			bool isSilent = (value == 1);

			//__DebugMessage("| Task::deadbodies::ItemCheckLoop::ContainerLootEventFunctor");

			ContainerLootEventFunctor args(m_refr, itemEx, static_cast<SInt32>(objType), maxCount, sound, isSilent);
			registry->QueueEvent(m_aliasHandle, &eventNameContainerLoot, &args);

			//__DebugMessage("| Task::deadbodies::ItemCheckLoop::next");

		}
		//TODO:ActivateRefChildren
	}
	return;
}


//ContainerLootTask::ContainerLootTask(TESObjectREFR* refr, TESForm* item, INIFile::PrimaryType first, INIFile::SecondaryType second) : m_refr(refr), m_item(item), m_first(first), m_second(second)
//{
//}
//
//void ContainerLootTask::Run()
//{
//}


void tasks::Init()
{
	//DataCase::GetInstance();
	DataCase::GetInstance()->ListsClear();
	DataCase::GetInstance()->BuildList();
	//CreateSoundList();
}

