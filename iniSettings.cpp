
#include "iniSettings.h"
#include "utils.h"

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>

INIFile* INIFile::s_pInstance = nullptr;

INIFile::INIFile()
{
	Reconfigure();
}

double INIFile::GetRadius(PrimaryType first)
{
	return GetConfigSettings(first, "radius");
}


bool INIFile::CreateConfigMap(PrimaryType first)
{
	std::string section;
	if (!IsType(first) || !GetIsPrimaryTypeString(first, section))
		return false;

	section += ":config";

	std::vector<std::string> list = FileUtils::GetSectionKeys(section.c_str(), GetINIlFile().c_str());
	std::sort(list.begin(), list.end());
	if (!SetINIData(&list, &configMap[first]))
	{
		//configMap[first].insert(std::make_pair("harvester_radius", 386));
		//configMap[first].insert(std::make_pair("spell_radius", 4096));
	}
	return !configMap[first].empty();
}


double INIFile::GetConfigSettings(PrimaryType first, std::string str)
{
	std::string tmp = str;;
	StringUtils::ToLower(tmp);
	return 	(configMap[first].count(tmp) >= 1) ? configMap[first].at(tmp) : 0.0;
}

bool INIFile::CreateObjectSetSettingsMap(PrimaryType first, SecondaryType second)
{
	std::string section;
	if (!IsType(first) || !GetIsPrimaryTypeString(first, section))
		return false;

	//section += ":";

	if (!IsType(second) || !GetIsSecondaryTypeString(second, section))
		return false;

	StringUtils::ToLower(section);

	std::vector<std::string> list = FileUtils::GetSectionKeys(section.c_str(), GetINIlFile().c_str());
	std::sort(list.begin(), list.end());
	if (!SetINIData(&list, &objectSetMap[first][second]))
	{
		//objectSetMap.insert(std::make_pair("flora", 0.0));
	}
	return !objectSetMap[first][second].empty();
}
double INIFile::GetObjectSetSettings(PrimaryType first, SecondaryType second, std::string str)
{
	if (!IsType(first) || !IsType(second))
		return 0.0;

	std::string tmp = str;
	StringUtils::ToLower(tmp);
	return 	(objectSetMap[first][second].count(tmp) >= 1) ? objectSetMap[first][second].at(tmp) : 0.0;
}


std::string INIFile::GetINIlFile()
{
	if (iniFilePath.empty())
	{
		iniFilePath = FileUtils::GetPluginPath();
		iniFilePath += INI_FILE;
#ifdef _DEBUG
		_MESSAGE("%s", iniFilePath.c_str());
#endif
	}
	return iniFilePath;
}

bool INIFile::WriteINI(const char* section, const char* key, const char* str)
{
	return FileUtils::WriteSectionKey(section, key, str, GetINIlFile().c_str());
}

void INIFile::Reconfigure(void)
{
	if (CreateConfigMap(PrimaryType::common))
		ShowSettings("Common:Config");

	if (CreateConfigMap(PrimaryType::autoharvest))
		ShowSettings("AutoHarvest:Config");
	if (CreateObjectSetSettingsMap(PrimaryType::autoharvest, SecondaryType::itemObjects))
		ShowSettings("AutoHarvest:ItemObjects");
	if (CreateObjectSetSettingsMap(PrimaryType::autoharvest, SecondaryType::containers))
		ShowSettings("AutoHarvest:Containers");
	if (CreateObjectSetSettingsMap(PrimaryType::autoharvest, SecondaryType::deadbodies))
		ShowSettings("AutoHarvest:Deadbodies");
	if (CreateObjectSetSettingsMap(PrimaryType::autoharvest, SecondaryType::valueWeight))
		ShowSettings("AutoHarvest:ValueWeight");

	if (CreateConfigMap(PrimaryType::spell))
		ShowSettings("Spell:Config");
	if (CreateObjectSetSettingsMap(PrimaryType::spell, SecondaryType::itemObjects))
		ShowSettings("Spell:ItemObjects");
	if (CreateObjectSetSettingsMap(PrimaryType::spell, SecondaryType::containers))
		ShowSettings("Spell:Containers");
	if (CreateObjectSetSettingsMap(PrimaryType::spell, SecondaryType::deadbodies))
		ShowSettings("Spell:Deadbodies");
	if (CreateObjectSetSettingsMap(PrimaryType::spell, SecondaryType::valueWeight))
		ShowSettings("Spell:ValueWeight");
}

// :p

bool INIFile::SetINIData(std::vector<std::string> *list, std::unordered_map<std::string, double> *map)
{
	for (std::string str : *list)
	{
		if (str.empty())
			continue;
		StringUtils::ToLower(str);
		auto vec = StringUtils::Split(str, '=');
		if (vec.size() != 2)
			continue;
		std::string key = vec.at(0);
		double value = std::atof(vec.at(1).c_str());
		if (!key.empty() && value != 0)
		{
			map->insert(std::make_pair(key.c_str(), value));
		}
	}
	return !map->empty();
}


bool INIFile::SaveConfigSettings(PrimaryType first, std::string key, double value)
{
	std::string section;
	std::string s_key = key;
	if (!IsType(first) || !GetIsPrimaryTypeString(first, section))
		return false;

	section += ":config";
	StringUtils::ToLower(section);
	StringUtils::ToLower(s_key);
	int setp = (s_key == "interval") ? 1 : 0;
	std::string sValue = StringUtils::ToString_0f(value, setp);
	return WriteINI(section.c_str(), s_key.c_str(), sValue.c_str());
}

bool INIFile::SaveObjectSetSettings(PrimaryType first, SecondaryType second, std::string key, double value)
{
	std::string section;
	if (!IsType(first) || !GetIsPrimaryTypeString(first, section))
		return false;

	//section += ":";

	if (!IsType(second) || !GetIsSecondaryTypeString(second, section))
		return false;

	StringUtils::ToLower(section);

	int setp = (second == SecondaryType::valueWeight) ? 1 : 0;
	std::string sValue = StringUtils::ToString_0f(value, setp);
	return WriteINI(section.c_str(), key.c_str(), sValue.c_str());
}

void INIFile::ShowSettings(std::string str)
{
#ifdef _DEBUG
	_MESSAGE("ShowSettings(%s)", str.c_str());
	StringUtils::ToLower(str);
	if (str == "common:config")
	{
		for (auto itr = configMap[PrimaryType::common].begin(); itr != configMap[PrimaryType::common].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "autoharvest:config")
	{
		for (auto itr = configMap[PrimaryType::autoharvest].begin(); itr != configMap[PrimaryType::autoharvest].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "autoharvest:itemobjects")
	{
		for (auto itr = objectSetMap[PrimaryType::autoharvest][SecondaryType::itemObjects].begin(); itr != objectSetMap[PrimaryType::autoharvest][SecondaryType::itemObjects].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "autoharvest:containers")
	{
		for (auto itr = objectSetMap[PrimaryType::autoharvest][SecondaryType::containers].begin(); itr != objectSetMap[PrimaryType::autoharvest][SecondaryType::containers].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "autoharvest:deadbodies")
	{
		for (auto itr = objectSetMap[PrimaryType::autoharvest][SecondaryType::deadbodies].begin(); itr != objectSetMap[PrimaryType::autoharvest][SecondaryType::deadbodies].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "autoharvest:valueweight")
	{
		for (auto itr = objectSetMap[PrimaryType::autoharvest][SecondaryType::valueWeight].begin(); itr != objectSetMap[PrimaryType::autoharvest][SecondaryType::valueWeight].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "spell:config")
	{
		for (auto itr = configMap[PrimaryType::spell].begin(); itr != configMap[PrimaryType::spell].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "spell:itemobjects")
	{
		for (auto itr = objectSetMap[PrimaryType::spell][SecondaryType::itemObjects].begin(); itr != objectSetMap[PrimaryType::spell][SecondaryType::itemObjects].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "spell:containers")
	{
		for (auto itr = objectSetMap[PrimaryType::spell][SecondaryType::containers].begin(); itr != objectSetMap[PrimaryType::spell][SecondaryType::containers].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "spell:deadbodies")
	{
		for (auto itr = objectSetMap[PrimaryType::spell][SecondaryType::deadbodies].begin(); itr != objectSetMap[PrimaryType::spell][SecondaryType::deadbodies].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}
	else if (str == "spell:valueweight")
	{
		for (auto itr = objectSetMap[PrimaryType::spell][SecondaryType::valueWeight].begin(); itr != objectSetMap[PrimaryType::spell][SecondaryType::valueWeight].end(); ++itr)
			_MESSAGE("   %s  =  %0.2f", (itr->first).c_str(), itr->second);
	}

	_MESSAGE("");
#endif
}