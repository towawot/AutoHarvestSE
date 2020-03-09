#include <common/IFileStream.h>
#include "skse64/GameRTTI.h"
#include <skse64/GameData.h>
#include <vector>
#include <string>
#include "basketfile.h"

BasketFile* BasketFile::s_pInstance = nullptr;

UInt32 g_userlist_formid = 0x333c;
UInt32 g_excludelist_formid = 0x0333d;

namespace
{
	ModInfo* GetFirstModInfo(TESForm* thisForm)
	{
		ModInfo* result = nullptr;
		UInt16 modIndex = (thisForm->formID) >> 24;
		if (modIndex == 0xFF)
			return result;

		DataHandler* dhnd = DataHandler::GetSingleton();
		if (!dhnd)
			return result;

		tArray<ModInfo*>* modsList = nullptr;
		if (modIndex < 0xFE)
		{
			modsList = &(dhnd->modList.loadedMods);
		}
		else if (modIndex == 0xFE)
		{
			modsList = &(dhnd->modList.loadedCCMods);
			modIndex = ((thisForm->formID) & 0x00FFF000) / 0x1000;
		}

		return (modsList && modsList->GetNthItem(modIndex, result)) ? result : nullptr;
	}

	std::string GetPluginName(ModInfo* modinfo)
	{
		std::string result;
		return modinfo ? modinfo->name : nullptr;
	}

	std::string GetBaseName(TESForm* thisForm)
	{
		std::string result;
		if (thisForm)
		{
			TESFullName* pFullName = DYNAMIC_CAST(thisForm, TESForm, TESFullName);
			if (pFullName)
				result = (pFullName->name).c_str();
		}
		return result;
	}

	std::vector<std::string> split(const std::string &str, char delim)
	{
		std::vector<std::string> res;
		size_t current = 0, found;
		while((found = str.find_first_of(delim, current)) != std::string::npos){
		res.push_back(std::string(str, current, found - current));
		current = found + 1;
		}
		res.push_back(std::string(str, current, str.size() - current));
		return res;
	}
}

BasketFile::BasketFile()
{
	DataHandler* dhnd = DataHandler::GetSingleton();
	if (!dhnd)
		return;

	SInt32 modIndex = -1;

	const ModInfo* modInfo = DataHandler::GetSingleton()->LookupModByName(MODNAME);
	if (!modInfo)
		return;

	modIndex = modInfo->GetPartialIndex();
	if (modIndex == -1)
		return;
	else if (modIndex < 0xFF)
	{
		g_userlist_formid |= (modIndex << 24);
		g_excludelist_formid |= (modIndex << 24);
	}
	else if (modInfo->IsLight())
	{
		g_userlist_formid |= (modIndex << 8);
		g_excludelist_formid |= (modIndex << 8);
	}

	if (!LookupFormByID(g_userlist_formid || !LookupFormByID(g_excludelist_formid)))
		return;

	formList[USERLIST] = (BGSListForm*)LookupFormByID(g_userlist_formid);
	formList[EXCLUDELIST] = (BGSListForm*)LookupFormByID(g_excludelist_formid);
}

inline UInt32 BasketFile::GetSize(listnum list_number)
{
	return formList[list_number]->GetSize();
}

bool BasketFile::SaveFile(listnum list_number, const char* basketText)
{
#ifdef _DEBUG
	_MESSAGE("BasketFile::SaveFile");
#endif

	std::string RuntimeDir = GetRuntimeDirectory();
	if(RuntimeDir.empty())
		return false;

	std::string fullPath = RuntimeDir + "Data\\SKSE\\Plugins\\AutoHarvestSE\\" + basketText;
	IFileStream fs;
	if(!fs.Create(fullPath.c_str()))
		return false;

	if(formList[list_number])
	{
		for(int i = 0; i < formList[list_number]->forms.count; i++)
		{
			TESForm* childForm;
			if(formList[list_number]->forms.GetNthItem(i, childForm))
			{
				std::string name = GetPluginName(GetFirstModInfo(childForm));
				UInt32 hexID = childForm->formID & 0x00FFFFFF;
				
				stringEx str;
				str.Format("%s\t0x%06X\t%s\n", name.c_str(), hexID, GetBaseName(childForm).c_str());
				
				#ifdef _DEBUG
				_MESSAGE("%s\t%08X\t%s", name.c_str(), hexID, GetBaseName(childForm).c_str());
				#endif
				
				char buf[512];
				char *a = new char[str.size()+1];
				buf[str.size()] = 0;
				memcpy(buf,str.c_str(),str.size());
				delete[] a;

				fs.WriteBuf(buf, str.length());
			}
			
			if(formList[list_number]->addedForms)
			{
				for(int i = 0; i < formList[list_number]->addedForms->count; i++)
				{
					UInt32 formid;
					if (formList[list_number]->addedForms->GetNthItem(i, formid))
					{
						TESForm* childForm = LookupFormByID(formid);
						if(childForm)
						{
							std::string name = GetPluginName(GetFirstModInfo(childForm));
							UInt32 hexID = childForm->formID & 0x00FFFFFF;

							stringEx str;
							str.Format("%s\t0x%06X\n", name.c_str(), hexID);

							#ifdef _DEBUG
							_MESSAGE("%s\t%08X", name.c_str(), hexID);
							#endif
						
							char buf[512];
							char *a = new char[str.size()+1];
							buf[str.size()] = 0;
							memcpy(buf,str.c_str(),str.size());
							delete[] a;
							
							fs.WriteBuf(buf, str.length());
						}
					}
				}
			}
		}
	}
	fs.Close();

#ifdef _DEBUG
	_MESSAGE("BasketFile::SaveFile end");
#endif
	return true;
}

bool BasketFile::LoadFile(listnum list_number, const char* basketText)
{
#ifdef _DEBUG
	_MESSAGE("BasketFile::LoadFile");
#endif
	std::string RuntimeDir = GetRuntimeDirectory();
	if(RuntimeDir.empty())
		return false;

	std::string fullPath = RuntimeDir + "Data\\SKSE\\Plugins\\AutoHarvestSE\\" + basketText;

	IFileStream fs;
	if(!fs.Open(fullPath.c_str()))
		return false;

	for (int index = 0; !fs.HitEOF(); index++)
	{
		char buf[512];
		fs.ReadString(buf, 512, '\n', '\r');

		// skip comments
		if (buf[0] == '#')
			continue;

		std::vector<std::string> str;
		std::string bufLine = buf;
		str = split(bufLine, '\t');

		//if (str.size() != 2 && str.size() != 3)
		//	continue;
		//if (str[0].empty() || str[1].empty())
		//	continue;

		const ModInfo* modinfo = DataHandler::GetSingleton()->LookupModByName(str[0].c_str());
		if (!modinfo)
			continue;

		UInt32 modIndex = modinfo->GetPartialIndex();
		UInt32 formID = std::stoul(str[1], nullptr, 16);
		formID |= (modIndex << 24);
		
		TESForm* thisForm = (TESForm*)LookupFormByID(formID);
		if (!thisForm)
			continue;
		
		CALL_MEMBER_FN(formList[list_number], AddFormToList)(thisForm);
	}
	fs.Close();
#ifdef _DEBUG
	_MESSAGE("BasketFile::LoadFile end");
#endif
	return true;
}

void BasketFile::SyncList(listnum list_number)
{
	if (formList[list_number])
	{
		list[list_number].clear();

		for (int i = 0; i < formList[list_number]->forms.count; i++)
		{
			if (formList[list_number]->addedForms)
			{
				for (int i = 0; i < formList[list_number]->addedForms->count; i++)
				{
					UInt32 formid;
					if (formList[list_number]->addedForms->GetNthItem(i, formid))
					{
						TESForm* childForm = LookupFormByID(formid);
						if (childForm)
							list[list_number].insert(childForm);
					}
				}
			}
		}
	}

}

class VisitorP : public BGSListForm::Visitor
{
public:
	VisitorP(TESForm* form) : m_pickupform(form) { }

	virtual bool Accept(TESForm * form)
	{
		return (m_pickupform == form);
	}

private:
	TESForm* m_pickupform;
};

bool BasketFile::IsinList(listnum list_number, TESForm * pickupform)
{
	VisitorP visitor(pickupform);
	return formList[list_number]->Visit(visitor);
}

