#include "skse64/PluginAPI.h"
//#include "skse64_common/skse_version.h"
//#include "skse64/GameForms.h"
//#include "skse64/GameReferences.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusVM.h"
#include "skse64/PapyrusNativeFunctions.h"

#include "papyrus.h"
#include "TESFormEx.h"
#include "tasks.h"
#include "iniSettings.h"
#include "objects.h"
#include "utils.h"
#include "dataCase.h"
#include <list>

#include "GridCellArrayEx.h"
#include "userlistUtil.h"
#include "skse64/ScaleformLoader.h"

bool g_SneakState = false;

namespace papyrus
{
	void DebugTrace(StaticFunctionTag* base, BSFixedString str)
	{
		_MESSAGE("%s", str);
	}

	BSFixedString GetPluginName(StaticFunctionTag* base, TESForm* thisForm)
	{
		if (!thisForm)
			return nullptr;
		return PluginUtils::GetPluginName(thisForm).c_str();
	}

	BSFixedString GetTextFormID(StaticFunctionTag* base, TESForm* thisForm)
	{
		if (!thisForm)
			return nullptr;
		return StringUtils::ToStringID(thisForm->formID).c_str();
	}

	BSFixedString GetTextObjectType(StaticFunctionTag* base, TESForm* thisForm)
	{
		if (!thisForm)
			return nullptr;

		ObjectType objType = ClassifyType(thisForm, true);
		if (objType == ObjectType::unknown)
			return "NON-CLASSIFIED";

		std::string result = GetObjectTypeName(objType);
		StringUtils::ToUpper(result);
		return (!result.empty()) ? result.c_str() : nullptr;
	}

	float GetConfigSetting(StaticFunctionTag* base, SInt32 section, BSFixedString key)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);
		if (!INIFile::GetInstance()->IsType(first))
			return 0.0;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		return INIFile::GetInstance()->GetConfigSettings(first, str.c_str());
	}

	void AddConfigSetting(StaticFunctionTag* base, SInt32 section, BSFixedString key, float value)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);

		static INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini || !ini->IsType(first))
			return;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		ini->AddConfigSettings(first, str.c_str(), static_cast<double>(value));
	}

	BSFixedString GetObjectKeyString(StaticFunctionTag* base, SInt32 objectNumber)
	{
		BSFixedString result;
		std::string str = GetObjectTypeName(objectNumber);
		if (str.empty() || str.c_str() == "unknown")
			return result;
		else
			result = str.c_str();
		return result;
	}

	float GetObjectSetting(StaticFunctionTag* base, SInt32 section, SInt32 objectSet, BSFixedString key)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);
		INIFile::SecondaryType second = static_cast<INIFile::SecondaryType>(objectSet);

		INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini || !ini->IsType(first) || !ini->IsType(second))
			return 0.0;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		return ini->GetObjectSetSettings(first, second, str.c_str());
	}

	void AddObjectSetting(StaticFunctionTag* base, SInt32 section, SInt32 objectSet, BSFixedString key, float value)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);
		INIFile::SecondaryType second = static_cast<INIFile::SecondaryType>(objectSet);

		INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini || !ini->IsType(first) || !ini->IsType(second))
			return;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		ini->AddObjectSetSettings(first, second, str.c_str(), static_cast<double>(value));
	}

	void SaveConfigSetting(StaticFunctionTag* base, SInt32 section, BSFixedString key, float value)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);

		INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini || !ini->IsType(first))
			return;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		ini->SaveConfigSettings(first, str.c_str(), static_cast<double>(value));
	}

	void SaveObjectSetting(StaticFunctionTag* base, SInt32 section, SInt32 objectSet, BSFixedString key, float value)
	{
		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(section);
		INIFile::SecondaryType second = static_cast<INIFile::SecondaryType>(objectSet);

		INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini || !ini->IsType(first) || !ini->IsType(second))
			return;

		std::string str = key.c_str();
		StringUtils::ToLower(str);

		ini->SaveObjectSetSettings(first, second, str.c_str(), static_cast<double>(value));
	}

	bool Reconfigure(StaticFunctionTag* base)
	{
		INIFile* ini = INIFile::GetInstance()->GetInstance();
		if (!ini)
			return false;

		ini->DeleteInstance();
		ini = INIFile::GetInstance()->GetInstance();
		return (ini) ? true : false;;
	}

	SInt32 GetCloseReferences(StaticFunctionTag* base, SInt32 type1)
	{
		//typedef UInt32 FormID;

		UInt32 result = 0;

		DataCase* data = DataCase::GetInstance();
		if (!data)
			return 0;

		// 0.9.4‚É‚Äpapyrus‚ÌŽ©“®Ý’è‚É•ÏX
		//static const UInt32 LocTypePlayerHouse = 0x00FC1A3;
		//if ((*g_thePlayer)->locationAC8 && (*g_thePlayer)->locationAC8->keyword.BGSKeywordForm::HasKeyword(LocTypePlayerHouse))
		//	return 0;

		INIFile* ini = INIFile::GetInstance();
		GridCellArrayEx* grid = static_cast<GridCellArrayEx*>((*g_TES)->cellArray);
		if (!grid || !ini)
			return 0;

		INIFile::PrimaryType first = static_cast<INIFile::PrimaryType>(type1);
		if (!ini->IsType(first))
			return 0;

		const int disableInCombat = static_cast<int>(ini->GetConfigSettings(first, "disableInCombat"));
		if (disableInCombat == 1 && (*g_thePlayer)->IsInCombat())
		{
#ifdef _DEBUG
			_MESSAGE("disableInCombat %d", disableInCombat);
#endif
			return 0;
		}

		const int disableDrawingWeapon = static_cast<int>(ini->GetConfigSettings(first, "disableDrawingWeapon"));
		if (disableDrawingWeapon == 1 && (*g_thePlayer)->actorState.IsWeaponDrawn())
		{
#ifdef _DEBUG
			_MESSAGE("disableDrawingWeapon %d", disableDrawingWeapon);
#endif
			return 0;
		}

		bool sneakState = (*g_thePlayer)->actorState.IsSneaking();
		if (g_SneakState != sneakState)
			data->ListsClear();
		g_SneakState = sneakState;

		int crimeCheck = static_cast<int>(ini->GetConfigSettings(first, (sneakState) ? "crimeCheckSneaking" : "crimeCheckNotSneaking"));
		int belongingsCheck = static_cast<int>(ini->GetConfigSettings(first, "playerBelongingsLoot"));

		typedef std::pair<TESObjectREFR*, double> Pair;
		std::list<Pair> lists;

		result = grid->GetReferences(&lists);
		if (result == 0)
			return 0;

		double radius = ini->GetRadius(first);
		for (Pair pair : lists)
		{
			double distance = pair.second;

			if (!(distance < radius))
				break;

			TESObjectREFR* refr = pair.first;
			if (!refr)
				continue;

			if (refr->baseForm->formType == kFormType_Container)
			{
				if (ini->GetConfigSettings(INIFile::PrimaryType::common, "EnableLootContainer") == 0.0)
					continue;
			}
			else if (refr->formType == kFormType_Character)
			{
				if (ini->GetConfigSettings(INIFile::PrimaryType::common, "enableLootDeadbody") == 0.0)
					continue;
			}
			else if (refr->baseForm->formType == kFormType_Activator && GetAshPile(refr))
			{
				if (ini->GetConfigSettings(INIFile::PrimaryType::common, "enableLootDeadbody") == 0.0)
					continue;
			}
			else if (ini->GetConfigSettings(INIFile::PrimaryType::common, "enableAutoHarvest") == 0.0)
				continue;


			//#ifdef _DEBUG
			//			const char* name = CALL_MEMBER_FN(refr, GetReferenceName)();
			//			_MESSAGE("%s(%08x) base(%08x) dist(%0.1f / %0.1f)", name, refr->formID, refr->baseForm->formID, distance, radius);
			//#endif

			bool ownership = false;

//#ifdef _DEBUG
//			const char* str = ownership ? "true" : "false";
//			_MESSAGE("***0 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif

			if (crimeCheck == 1.0)
			{
				ownership = CALL_MEMBER_FN(refr, IsOffLimits)();

//#ifdef _DEBUG
//				const char* str = ownership ? "true" : "false";
//				_MESSAGE("***1 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif

			}
			else if (crimeCheck == 2.0)
			{
				ownership = CALL_MEMBER_FN(refr, IsOffLimits)();

//#ifdef _DEBUG
//				const char* str = ownership ? "true" : "false";
//				_MESSAGE("***2 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif


				if (!ownership)
				{
					TESObjectREFRx* refrEx = static_cast<TESObjectREFRx*>(refr);
					if (refrEx)
					{
						ownership = refrEx->IsPlayerOwned();

//#ifdef _DEBUG
//						const char* str = ownership ? "true" : "false";
//						_MESSAGE("***2-1 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif
					}
				}

				if (!ownership)
				{
					TESObjectCELLEx* cellEx = static_cast<TESObjectCELLEx*>((*g_thePlayer)->parentCell);
					if (cellEx)
					{
						ownership = cellEx->IsPlayerOwned();

//#ifdef _DEBUG
//						const char* str = ownership ? "true" : "false";
//						_MESSAGE("***2-2 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif
					}
				}
			}

			if (!ownership && belongingsCheck == 0)
			{
				TESObjectREFRx* refrEx = static_cast<TESObjectREFRx*>(refr);
				if (refrEx)
					ownership = refrEx->IsPlayerOwned();

//#ifdef _DEBUG
//				const char* str = ownership ? "true" : "false";
//				_MESSAGE("***3 crimeCheck: %d ownership: %s", crimeCheck, str);
//#endif

			}

			if (ownership)
			{
				data->BlockReference(refr);
				continue;
			}

			if (refr->formType == kFormType_Character)
			{
				ActorEx* actorEx = static_cast<ActorEx*>((Actor*)refr);
				if (actorEx)
				{
					if (actorEx->IsPlayerFollower() || actorEx->IsEssential() || !actorEx->IsNotSummonable())
					{
						data->BlockReference(refr);
						return false;
					}
				}
			}


			INIFile::SecondaryType second = INIFile::SecondaryType::NONE2;
			if (refr->baseForm->formType == kFormType_Container)
				second = INIFile::SecondaryType::containers;
			else if (refr->formType == kFormType_Character && refr->IsDead(true))
				second = INIFile::SecondaryType::deadbodies;
			else if ((refr->baseForm)->formType == kFormType_Activator && GetAshPile(refr))
			{
				refr = GetAshPile(refr);
				second = INIFile::SecondaryType::deadbodies;
			}
			else
				second = INIFile::SecondaryType::itemObjects;

			g_task->AddTask(new SearchTask(refr, distance, first, second));
		}

		return result;
	}

	bool LockTask(StaticFunctionTag* base, TESObjectREFR* refr, bool islock)
	{
		DataCase* data = DataCase::GetInstance();
		return (islock) ? data->LockTask(refr) : data->UnlockTask(refr);
	}

	bool BlockReference(StaticFunctionTag* base, TESObjectREFR* refr, bool isBlock)
	{
		DataCase* data = DataCase::GetInstance();
		return (isBlock) ? data->BlockReference(refr) : data->UnblockReference(refr);
	}

	void ListClear(StaticFunctionTag* base)
	{
		DataCase::GetInstance()->ListsClear();
	}

	SInt32 SaveUserlist(StaticFunctionTag* base)
	{
		return UserlistSave();
	}

	SInt32 LoadUserlist(StaticFunctionTag* base)
	{
		return UserlistLoad();
	}

	bool IsInUserlist(StaticFunctionTag* base, TESForm* item)
	{
		DataCase* data = DataCase::GetInstance();
		return (data->lists.userlist.count(item) >= 1);
	}

	bool AddUserlist(StaticFunctionTag* base, TESForm* item)
	{
		static BGSListForm* listForm;
		if (!listForm)
		{
			UInt32 listID = 0x0333c;
			listID |= DataHandler::GetSingleton()->GetLoadedModIndex("AutoHarvestSE.esp") << 24;
			listForm = DYNAMIC_CAST(LookupFormByID(listID), TESForm, BGSListForm);
		}
		if (!listForm)
			return false;

		DataCase* data = DataCase::GetInstance();
		auto result = data->lists.userlist.insert(item);
//		if (IsInUserlist(nullptr, item))
//		{
//#ifdef _DEBUG
//			_MESSAGE("AddFormToList");
//#endif
//			CALL_MEMBER_FN(listForm, AddFormToList)(item);
//		}
		return result.second;
	}

	bool RemoveUserlist(StaticFunctionTag* base, TESForm* item)
	{
		static BGSListForm* listForm;
		if (!listForm)
		{
			UInt32 listID = 0x0333c;
			listID |= DataHandler::GetSingleton()->GetLoadedModIndex("AutoHarvestSE.esp") << 24;
			listForm = DYNAMIC_CAST(LookupFormByID(listID), TESForm, BGSListForm);
		}
		if (!listForm)
			return false;

		DataCase* data = DataCase::GetInstance();
		data->lists.userlist.erase(item);
		//if (data->lists.userlist.erase(item) == 0)
			//CALL_MEMBER_FN(listForm, RemoveFormFromList)(item);

		return (data->lists.userlist.erase(item) == 0);
	}

	bool IsInExcludelist(StaticFunctionTag* base, TESForm* item)
	{
		DataCase* data = DataCase::GetInstance();
		return (data->lists.excludelist.count(item) >= 1);
	}

	bool AddExcludelist(StaticFunctionTag* base, TESForm* item)
	{
		static BGSListForm* listForm;
		if (!listForm)
		{
			UInt32 listID = 0x0333d;
			listID |= DataHandler::GetSingleton()->GetLoadedModIndex("AutoHarvestSE.esp") << 24;
			listForm = DYNAMIC_CAST(LookupFormByID(listID), TESForm, BGSListForm);
		}
		if (!listForm)
			return false;

		DataCase* data = DataCase::GetInstance();
		auto result = data->lists.excludelist.insert(item);
		//		if (IsInUserlist(nullptr, item))
		//		{
		//#ifdef _DEBUG
		//			_MESSAGE("AddFormToList");
		//#endif
		//			CALL_MEMBER_FN(listForm, AddFormToList)(item);
		//		}
		return result.second;
	}

	bool RemoveExcludelist(StaticFunctionTag* base, TESForm* item)
	{
		static BGSListForm* listForm;
		if (!listForm)
		{
			UInt32 listID = 0x0333d;
			listID |= DataHandler::GetSingleton()->GetLoadedModIndex("AutoHarvestSE.esp") << 24;
			listForm = DYNAMIC_CAST(LookupFormByID(listID), TESForm, BGSListForm);
		}
		if (!listForm)
			return false;

		DataCase* data = DataCase::GetInstance();
		data->lists.excludelist.erase(item);
		//if (data->lists.userlist.erase(item) == 0)
		//CALL_MEMBER_FN(listForm, RemoveFormFromList)(item);

		return (data->lists.excludelist.erase(item) == 0);
	}

	BSFixedString GetTranslation(StaticFunctionTag* base, BSFixedString key)
	{
		std::string str = key.c_str();
		DataCase* data = DataCase::GetInstance();
		if (data->lists.translations.count(str) == 0)
			return nullptr;
		return data->lists.translations[str].c_str();
	}

	BSFixedString Replace(StaticFunctionTag* base, BSFixedString str, BSFixedString target, BSFixedString replacement)
	{
		std::string s_str(str.c_str());
		std::string s_target(target.c_str());
		std::string s_replacement(replacement.c_str());
		return (StringUtils::Replace(s_str, s_target, s_replacement)) ? s_str.c_str() : nullptr;
	}

	BSFixedString ReplaceArray(StaticFunctionTag* base, BSFixedString str, VMArray<BSFixedString> targets, VMArray<BSFixedString> replacements)
	{
		std::string result(str.c_str());
		if (result.empty() || targets.Length() != replacements.Length())
			return nullptr;

		BSFixedString target;
		BSFixedString replacement;
		for (int index = 0; index < targets.Length(); index++)
		{
			target = "";
			replacement = "";

			targets.Get(&target, index);
			replacements.Get(&replacement, index);

			std::string s_target(target.c_str());
			std::string s_replacement(replacement.c_str());

			if (!StringUtils::Replace(result, s_target, s_replacement))
				return nullptr;
		}
		return result.c_str();
	}

}

void papyrus::RegisterFuncs(VMClassRegistry* registry)
{
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, void, BSFixedString>("DebugTrace", "AutoHarvestSE", papyrus::DebugTrace, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, BSFixedString, TESForm*>("GetPluginName", "AutoHarvestSE", papyrus::GetPluginName, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, BSFixedString, TESForm*>("GetTextFormID", "AutoHarvestSE", papyrus::GetTextFormID, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, BSFixedString, TESForm*>("GetTextObjectType", "AutoHarvestSE", papyrus::GetTextObjectType, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, SInt32, SInt32>("GetCloseReferences", "AutoHarvestSE", papyrus::GetCloseReferences, registry));

	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, float, SInt32, BSFixedString>("GetConfigSetting", "AutoHarvestSE", papyrus::GetConfigSetting, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, void, SInt32, BSFixedString, float>("AddConfigSetting", "AutoHarvestSE", papyrus::AddConfigSetting, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, BSFixedString, SInt32>("GetObjectKeyString", "AutoHarvestSE", papyrus::GetObjectKeyString, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, float, SInt32, SInt32, BSFixedString>("GetObjectSetting", "AutoHarvestSE", papyrus::GetObjectSetting, registry));
	registry->RegisterFunction(
		new NativeFunction4<StaticFunctionTag, void, SInt32, SInt32, BSFixedString, float>("AddObjectSetting", "AutoHarvestSE", papyrus::AddObjectSetting, registry));

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, bool>("Reconfigure", "AutoHarvestSE", papyrus::Reconfigure, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, void, SInt32, BSFixedString, float>("SaveConfigSetting", "AutoHarvestSE", papyrus::SaveConfigSetting, registry));
	registry->RegisterFunction(
		new NativeFunction4<StaticFunctionTag, void, SInt32, SInt32, BSFixedString, float>("SaveObjectSetting", "AutoHarvestSE", papyrus::SaveObjectSetting, registry));

	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, bool, TESObjectREFR*, bool>("LockTask", "AutoHarvestSE", papyrus::LockTask, registry));
	registry->RegisterFunction(
		new NativeFunction2<StaticFunctionTag, bool, TESObjectREFR*, bool>("BlockReference", "AutoHarvestSE", papyrus::BlockReference, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, void>("ListClear", "AutoHarvestSE", papyrus::ListClear, registry));

	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, SInt32>("SaveUserlist", "AutoHarvestSE", papyrus::SaveUserlist, registry));
	registry->RegisterFunction(
		new NativeFunction0<StaticFunctionTag, SInt32>("LoadUserlist", "AutoHarvestSE", papyrus::LoadUserlist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsInUserlist", "AutoHarvestSE", papyrus::IsInUserlist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("AddUserlist", "AutoHarvestSE", papyrus::AddUserlist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("RemoveUserlist", "AutoHarvestSE", papyrus::RemoveUserlist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("IsInExcludelist", "AutoHarvestSE", papyrus::IsInExcludelist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("AddExcludelist", "AutoHarvestSE", papyrus::AddExcludelist, registry));
	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, bool, TESForm*>("RemoveExcludelist", "AutoHarvestSE", papyrus::RemoveExcludelist, registry));

	registry->RegisterFunction(
		new NativeFunction1<StaticFunctionTag, BSFixedString, BSFixedString>("GetTranslation", "AutoHarvestSE", papyrus::GetTranslation, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, BSFixedString, BSFixedString>("Replace", "AutoHarvestSE", papyrus::Replace, registry));
	registry->RegisterFunction(
		new NativeFunction3<StaticFunctionTag, BSFixedString, BSFixedString, VMArray<BSFixedString>, VMArray<BSFixedString>>("ReplaceArray", "AutoHarvestSE", papyrus::ReplaceArray, registry));

}
