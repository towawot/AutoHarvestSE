#pragma once

#include <shlobj.h>
#include <string>
#include <vector>

#define FormID UInt32

class TESForm;
class TESObjectREFR;

namespace FileUtils
{
	std::string GetGamePath(void);
	std::string GetDataPath(void);
	std::string GetPluginPath(void);
	bool IsFoundFile(const char* fileName);
	bool WriteSectionKey(LPCTSTR section_name, LPCTSTR key_name, LPCTSTR key_data, LPCTSTR ini_file_path);
	std::vector<std::string> GetSectionKeys(LPCTSTR section_name, LPCTSTR ini_file_path);
	std::vector<std::string> GetIniKeys(std::string section, std::string fileName);
}

namespace utils
{
	double GetDistance(TESObjectREFR* refr);
	void SetGoldValue(TESForm* pForm, UInt32 value);
}

namespace PluginUtils
{
	std::string GetPluginName(TESForm* thisForm);
	std::string GetBaseName(TESForm* thisForm);
	void SetBaseName(TESForm* pForm, const char* str);
	std::string GetPluginName(UInt8 modIndex);
	UInt8 GetModIndex(TESForm* thisForm);
	//UInt8 GetOrderIDByModName(std::string name);
	UInt8 GetLoadedModIndex(const char* espName);
}

namespace StringUtils
{
	std::string ToStringID(FormID id);
	std::string ToString_0f(double num, UInt8 set);
	void ToUpper(std::string &str);
	void ToLower(std::string &str);
	std::vector<std::string> Split(const std::string &str, char sep);
	bool Replace(std::string &str, const std::string target, const std::string replacement);
	std::string Trim(const std::string& str, const char* trimCharacterList);
	void DeleteNl(std::string &str);
}
//namespace GameSettingUtils
//{
//	std::string GetGameLanguage(void);
//}
//
