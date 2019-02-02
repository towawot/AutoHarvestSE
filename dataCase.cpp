
#include "skse64/PapyrusVM.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/GameForms.h"

#include "common/IFileStream.h"
#include "skse64/GameStreams.h"
#include "skse64/ScaleformState.h"

#include "skse64/GameSettings.h"

#include "utils.h"
#include "dataCase.h"
#include "iniSettings.h"
#include <fstream>
#include <iostream>
#include <string>

DataCase* DataCase::s_pInstance = nullptr;

DataCase::DataCase()
{
}

#include "strConv.h"

void DataCase::GetTranslationData()
{
	Setting	* setting = GetINISetting("sLanguage:General");

	std::string path = "Interface\\Translations\\AutoHarvestSE_";
	path += (setting && setting->GetType() == Setting::kType_String) ? setting->data.s : "ENGLISH";
	path += ".txt";

	BSResourceNiBinaryStream fs(path.c_str());
	if (!fs.IsValid())
		return;

//#ifdef _DEBUG
//	_MESSAGE("Reading translations from %s...", path.c_str());
//#endif

	UInt16	bom = 0;
	UInt32	ret = fs.Read(&bom, sizeof(UInt16));
	if (ret == 0)
	{
		_MESSAGE("Empty translation file.");
		return;
	}

	if (bom != 0xFEFF)
	{
		_MESSAGE("BOM Error, file must be encoded in UCS-2 LE.");
		return;
	}

	while (true)
	{
		wchar_t buf[512];

		UInt32	len = fs.ReadLine_w(buf, sizeof(buf) / sizeof(buf[0]), '\n');
		if (len == 0) // End of file
		{
			break;
		}
		// at least $ + wchar_t + \t + wchar_t
		if (len < 4 || buf[0] != '$')
			continue;

		wchar_t last = buf[len - 1];
		if (last == '\r')
			len--;

		// null terminate
		buf[len] = 0;

		UInt32 delimIdx = 0;
		for (UInt32 i = 0; i < len; i++)
			if (buf[i] == '\t')
				delimIdx = i;

		// at least $ + wchar_t
		if (delimIdx < 2)
			continue;

		// replace \t by \0
		buf[delimIdx] = 0;

		wchar_t * key = nullptr;
		wchar_t * translation = nullptr;
		BSScaleformTranslator::GetCachedString(&key, buf, 0);
		BSScaleformTranslator::GetCachedString(&translation, &buf[delimIdx + 1], 0);

//#ifdef _DEBUG
//		TranslationTableItem item(key, translation);
//		item.Dump();
//#endif

		std::string keyS = wide_to_utf8(key);
		std::string translationS = wide_to_utf8(translation);

		lists.translations[keyS] = translationS;

	}
#ifdef _DEBUG
	_MESSAGE("* TranslationData(%d)", lists.translations.size());
#endif

	return;
}

bool DataCase::GetTSV(std::unordered_set<TESForm*> *tsv, const char* fileName)
{
	std::string filepath = FileUtils::GetPluginPath();
	filepath = filepath + "AutoHarvestSE\\" + fileName;

	if (!FileUtils::IsFoundFile(filepath.c_str()))
		return false;

	std::ifstream ifs(filepath);
	if (ifs.fail())
		return false;

	std::string str;
	while (getline(ifs, str))
	{
		if (str[0] == '#' || str[0] == ';' || (str[0] == '/' && str[1] == '/'))
			continue;

		if (str.find_first_not_of("\t") == std::string::npos)
			continue;

		auto vec = StringUtils::Split(str, '\t');
		std::string modName = vec[0];

		UInt8 modIndex = PluginUtils::GetOrderIDByModName(vec[0]);
		if (modIndex == 0xFF)
			continue;

		UInt32 formID = std::stoul(vec[1], nullptr, 16);
		formID |= (modIndex << 24);

		TESForm* pForm = LookupFormByID(formID);
		if (pForm)
			tsv->insert(pForm);
	}

#ifdef _DEBUG
	_MESSAGE("* TSV:%s(%d)", fileName, tsv->size());
#endif
	return true;
}

bool DataCase::GetTSV(std::vector<TESSound*> *tsv, const char* fileName)
{
	std::string filepath = FileUtils::GetPluginPath();
	filepath = filepath + "AutoHarvestSE\\" + fileName;

	if (!FileUtils::IsFoundFile(filepath.c_str()))
		return false;

	std::ifstream ifs(filepath);
	if (ifs.fail())
		return false;

	std::string str;
	while (getline(ifs, str))
	{
		if (str[0] == '#' || str[0] == ';' || (str[0] == '/' && str[1] == '/'))
			continue;

		if (str.find_first_not_of("\t") == std::string::npos)
			continue;

		auto vec = StringUtils::Split(str, '\t');
		std::string modName = vec[0];

		UInt8 modIndex = PluginUtils::GetOrderIDByModName(vec[0]);
		if (modIndex == 0xFF)
			continue;

		UInt32 formID = std::stoul(vec[1], nullptr, 16);
		formID |= (modIndex << 24);

		TESForm* pForm = LookupFormByID(formID);
		if (!pForm)
			continue;

		TESSound* sound = DYNAMIC_CAST(pForm, TESForm, TESSound);
		if (!sound)
			continue;
		
		tsv->push_back(sound);
	}

#ifdef _DEBUG
	_MESSAGE("* TSV:%s(%d)", fileName, tsv->size());
#endif
	return true;
}

//void DataCase::GetUserlistData()
//{
//	DataHandler* dhnd = DataHandler::GetSingleton();
//	if (!dhnd)
//		return;
//
//	UInt32 listID = 0x0333c;
//	listID |= dhnd->GetLoadedModIndex("AutoHarvestSE.esp") << 24;
//	BGSListForm* listForm = DYNAMIC_CAST(LookupFormByID(listID), TESForm, BGSListForm);
//	if (!listForm)
//	{
//#ifdef _DEBUG
//		_MESSAGE("ListFormERR %08X", listID);
//#endif
//		return;
//	}
//
//	if (!listForm->addedForms)
//	{
//#ifdef _DEBUG
//		_MESSAGE("addedFormsERR %08X", listID);
//#endif
//		return;
//	}
//
//	for (int index = 0; index < listForm->addedForms->count; index++)
//	{
//		UInt32 formid = 0;
//		if (!listForm->addedForms->GetNthItem(index, formid))
//			continue;
//
//		TESForm* childForm = LookupFormByID(formid);
//		if (!childForm)
//			continue;
//
//		lists.userlist.insert(childForm);
//	}
//
//#ifdef _DEBUG
//	_MESSAGE("* UserlistData(%d)", lists.userlist.size());
//#endif
//
//	return;
//}

enum FactionFlags
{
	kFlag_Vender = 1 << 14,		//  4000
};

void DataCase::GetBlockContainerData()
{
	DataHandler* dhnd = DataHandler::GetSingleton();
	if (dhnd)
	{
		for (int index = 0; index < dhnd->arrFACT.count; index++)
		{
			TESForm* pForm = nullptr;
			TESObjectREFR* containerRef = nullptr;

			if (dhnd->arrFACT.GetNthItem(index, pForm))
			{
				TESFaction* faction = DYNAMIC_CAST(pForm, TESForm, TESFaction);

				if (!faction)
					continue;

				if (faction->factionFlags & FactionFlags::kFlag_Vender)
				{
					containerRef = (faction->vendorData).merchantContainer;
					if (containerRef)
					{
#ifdef _DEBUG2
						_MESSAGE("[ADD:%d] vendor container : %s(%08X)", index, CALL_MEMBER_FN(containerRef, GetReferenceName)(), containerRef->formID);
#endif
						lists.blockContainerRefr.insert(containerRef);
					}
				}

				containerRef = (faction->crimeValues).stolenGoodsContainer;
				if (containerRef)
				{
#ifdef _DEBUG2
					_MESSAGE("[ADD:%d] stolenGoodsContainer : %s(%08X)", index, CALL_MEMBER_FN(containerRef, GetReferenceName)(), containerRef->formID);
#endif
					lists.blockContainerRefr.insert(containerRef);
				}

				containerRef = (faction->crimeValues).playerInventoryContainer;
				if (containerRef)
				{
#ifdef _DEBUG2
					_MESSAGE("[ADD:%d] playerInventoryContainer : %s(%08X)", index, CALL_MEMBER_FN(containerRef, GetReferenceName)(), containerRef->formID);
#endif
					lists.blockContainerRefr.insert(containerRef);
				}
			}
		}
#ifdef _DEBUG
		_MESSAGE("* BlockContainerData(%d)", lists.blockContainerRefr.size());
#endif
	}
}

void DataCase::GetAmmoData()
{
	DataHandler* dhnd = DataHandler::GetSingleton();
	if (!dhnd)
		return;

	for (int index = 0; index < dhnd->ammo.count; index++)
	{
		TESAmmo* ammo = nullptr;
		if (!dhnd->ammo.GetNthItem(index, ammo))
			continue;

		if (!ammo)
			continue;

		if (!ammo->IsPlayable())
			continue;

		std::string name;
		name = PluginUtils::GetBaseName(ammo);
		if (name.empty())
			continue;

		BGSProjectile* proj = ammo->settings.projectile;
		if (!proj)
			continue;

		//if (!proj->fullName.name)
		//	continue;

		//name = proj->fullName.name.c_str();
		//if (name.empty())
		//	continue;

		lists.ammoList[proj] = ammo;
	}

#ifdef _DEBUG
	_MESSAGE("* AmmoData(%d)", lists.ammoList.size());
#endif
}

bool DataCase::LockTask(TESObjectREFR* refr)
{
//#ifdef _DEBUG
//	_MESSAGE("DataCase::LockTask");
//#endif

	if (!refr)
		return false;
	if (lists.taskLock.count(refr) >= 1)
		return false;
	return ((lists.taskLock.insert(refr)).second);
}

bool DataCase::UnlockTask(TESObjectREFR* refr)
{
	//#ifdef _DEBUG
	//	_MESSAGE("DataCase::UnlockTask");
	//#endif

	if (!refr)
		return false;
	if (lists.taskLock.count(refr) == 0)
		return false;
	return (lists.taskLock.erase(refr) != 0);
}

bool DataCase::BlockReference(TESObjectREFR* refr)
{
	//#ifdef _DEBUG
	//	_MESSAGE("DataCase::BlockReference");
	//#endif

	if (!refr)
		return false;
	if (lists.blockRefr.count(refr) >= 1)
		return false;
	return ((lists.blockRefr.insert(refr)).second);
}

bool DataCase::UnblockReference(TESObjectREFR* refr)
{
	//#ifdef _DEBUG
	//	_MESSAGE("DataCase::UnblockReference");
	//#endif

	if (!refr)
		return false;
	if (lists.blockRefr.count(refr) == 0)
		return false;
	return (lists.blockRefr.erase(refr) != 0);
}

TESAmmo* DataCase::ProjToAmmo(BGSProjectile* proj)
{
	return (proj && lists.ammoList.find(proj) != lists.ammoList.end()) ? lists.ammoList[proj] : nullptr;
}

bool DataCase::IsInTasks(TESObjectREFR* refr)
{
//#ifdef _DEBUG
//	_MESSAGE("DataCase::IsInTasks");
//#endif
//
	return (refr && lists.taskLock.count(refr) >= 1);
}


void DataCase::ListsClear()
{
//#ifdef _DEBUG
//	_MESSAGE("DataCase::ListsClear");
//#endif
	lists.blockRefr.clear();
	lists.blockForm.clear();
	lists.taskLock.clear();
	lists.arrowCheck.clear();
}

void DataCase::BuildList()
{
//#ifdef _DEBUG
//	_MESSAGE("DataCase::BuildList");
//#endif

	GetTSV(&lists.flora, "flora.tsv");
	GetTSV(&lists.critter, "critter.tsv");
	GetTSV(&lists.fieldcrop, "fieldcrop.tsv");
	GetTSV(&lists.gem, "gem.tsv");
	GetTSV(&lists.septim, "septim.tsv");
	GetTSV(&lists.orevein, "orevein.tsv");
	GetTSV(&lists.blockForm, "blocklist.tsv");
	GetTSV(&lists.sound, "sound.tsv");
	GetAmmoData();
	GetBlockContainerData();
	//GetUserlistData();
	// TODO::skse64ÉoÉO?Ç†ÇÈÇ∆évÇ§ 0.7.2åªç›papyrusÇ≈èàóù

	GetTranslationData();
}

