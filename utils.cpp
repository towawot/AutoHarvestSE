#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"

//#include <shlobj.h>
#include "skse64/GameRTTI.h"
#include "skse64/GameForms.h"
#include "skse64/GameData.h"
#include "skse64/GameSettings.h"

#include "common/IFileStream.h"

#include "utils.h"

#include <shlobj.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm> //Trim
#include <math.h>	// pow


namespace FileUtils
{
	std::string GetGamePath(void)
	{
		static std::string s_runtimeDirectory;
		if (s_runtimeDirectory.empty())
		{
			char	runtimePathBuf[MAX_PATH];
			UInt32	runtimePathLength = GetModuleFileName(GetModuleHandle(NULL), runtimePathBuf, sizeof(runtimePathBuf));
			if (runtimePathLength && (runtimePathLength < sizeof(runtimePathBuf)))
			{
				std::string	runtimePath(runtimePathBuf, runtimePathLength);
				std::string::size_type	lastSlash = runtimePath.rfind('\\');
				if (lastSlash != std::string::npos)
					s_runtimeDirectory = runtimePath.substr(0, lastSlash + 1);
			}
		}
		return s_runtimeDirectory;
	}

	std::string GetDataPath()
	{
		static std::string s_dataDirectory;
		if (s_dataDirectory.empty())
		{
			s_dataDirectory = GetGamePath();
			s_dataDirectory += "data\\";
		}
		return s_dataDirectory;
	}
	std::string GetPluginPath()
	{
		static std::string s_skseDirectory;
		if (s_skseDirectory.empty())
		{
			s_skseDirectory = GetGamePath();
			s_skseDirectory += "data\\SKSE\\Plugins\\";
		}
		return s_skseDirectory;
	}

	bool IsFoundFile(const char* fileName)
	{
		std::ifstream ifs(fileName);
		return (ifs.fail()) ? false : true;
	}

	bool WriteSectionKey(LPCTSTR section_name, LPCTSTR key_name, LPCTSTR key_data, LPCTSTR ini_file_path)
	{
		return (WritePrivateProfileString(section_name, key_name, key_data, ini_file_path)) != 0;
	}

	std::vector<std::string> GetSectionKeys(LPCTSTR section_name, LPCTSTR ini_file_path)
	{
		std::vector<std::string> result;
		std::string file_path(ini_file_path);
		if (IsFoundFile(ini_file_path))
		{
			TCHAR buf[32768] = {};
			GetPrivateProfileSection(section_name, buf, sizeof(buf), ini_file_path);
			for (LPTSTR seek = buf; *seek != '\0'; seek++)
			{
				std::string str(seek);
				result.push_back(str);
				while (*seek != '\0')
					seek++;
			}
		}
		return result;
	}

	std::vector<std::string> GetIniKeys(std::string section, std::string fileName)
	{
		std::vector<std::string> result;
		std::string filePath = GetPluginPath();
		filePath += fileName;
		if (IsFoundFile(filePath.c_str()))
		{
			std::vector<std::string> list = GetSectionKeys(section.c_str(), filePath.c_str());
			for (std::string str : list)
			{
				auto vec = StringUtils::Split(str, '=');
				if (!vec.empty())
				{
					std::string key = vec.at(0);
					UInt32 value = std::atoi(vec.at(1).c_str());
					if (!key.empty() && value >= 1)
						result.push_back(key.c_str());
				}
			}
		}
		return result;
	}
}

namespace utils
{
	double GetDistance(TESObjectREFR* refr)
	{
		double dx = refr->pos.x - (*g_thePlayer)->pos.x;
		double dy = refr->pos.y - (*g_thePlayer)->pos.y;
		double dz = refr->pos.z - (*g_thePlayer)->pos.z;
		return pow(dx*dx + dy*dy + dz*dz, 0.5);
	}


	void SetGoldValue(TESForm* pForm, UInt32 value)
	{
		if (!pForm)
			return;
		TESValueForm* pValue = DYNAMIC_CAST(pForm, TESForm, TESValueForm);
		if (pValue)
			pValue->value = value;
	}
}

namespace PluginUtils
{
	std::string GetPluginName(TESForm* thisForm)
	{
		std::string result;
		UInt8 loadOrder = (thisForm->formID) >> 24;
		if (loadOrder < 0xFF)
		{
			DataHandler* dhnd = *g_dataHandler;
			ModInfo* modInfo = dhnd->modList.loadedMods[loadOrder];
			if (modInfo)
				result = modInfo->name;
		}
		return result;
	}
	std::string GetPluginName(UInt8 modIndex)
	{
		std::string unknown("Unknown");
		ModInfo* info = nullptr;
		DataHandler* dhnd = *g_dataHandler;
		if (dhnd && modIndex < 0xFF)
			info = dhnd->modList.loadedMods[modIndex];
		return (info) ? info->name : unknown;
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

	void SetBaseName(TESForm* pForm, const char* str)
	{
		if (!pForm)
			return;
		TESFullName* pFullName = DYNAMIC_CAST(pForm, TESForm, TESFullName);
		if (pFullName)
			pFullName->name = str;
	}

	UInt8 GetModIndex(TESForm* thisForm)
	{
		return (thisForm) ? thisForm->formID >> 24 : 0xFF;
	}

	UInt8 GetLoadedModIndex(const char* espName)
	{
		const ModInfo* info = DataHandler::GetSingleton()->LookupModByName(espName);
		if (!info)
			return 0xFF;

		if (info->IsLight())
		{
			//‚¢‚Â‚©‘Î‰ž‚·‚é
			return 0xFF;
		}

		return info->GetPartialIndex();
	}

}

namespace StringUtils
{
	std::string ToStringID(FormID id)
	{
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << id;
		//std::string result("0x");
		//result += ss.str();
		//return result;
		return ss.str();
	}

	std::string ToString_0f(double num, UInt8 setp)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(setp) << num;
		return ss.str();
	}

	void ToUpper(std::string &str)
	{
		for (auto &c : str)
			c = toupper(c);
	}

	void ToLower(std::string &str)
	{
		for (auto &c : str)
			c = tolower(c);
	}

	std::string Trim(const std::string& str, const char* trimCharacterList = " \t\v\r\n")
	{
		std::string result;
		std::string::size_type left = str.find_first_not_of(trimCharacterList);
		if (left != std::string::npos)
		{
			std::string::size_type right = str.find_last_not_of(trimCharacterList);
			result = str.substr(left, right - left + 1);
		}
		return result;
	}

	void DeleteNl(std::string &str)
	{
		const char CR = '\r';
		const char LF = '\n';
		std::string destStr;
		for (const auto c : str) {
			if (c != CR && c != LF) {
				destStr += c;
			}
		}
		str = std::move(destStr);
	}

	std::vector<std::string> Split(const std::string &str, char sep)
	{
		std::vector<std::string> result;

		auto first = str.begin();
		while (first != str.end())
		{
			auto last = first;
			while (last != str.end() && *last != sep)
				++last;
			result.push_back(std::string(first, last));
			if (last != str.end())
				++last;
			first = last;
		}
		return result;
	}

	bool Replace(std::string &str, const std::string target, const std::string replacement)
	{
		if (str.empty() || target.empty())
			return false;

		bool result = false;
		std::string::size_type pos = 0;
		while ((pos = str.find(target, pos)) != std::string::npos)
		{
			if (!result)
				result = true;
			str.replace(pos, target.length(), replacement);
			pos += replacement.length();
		}
		return result;
	}
}

namespace GameSettingUtils
{
	std::string GetGameLanguage()
	{
		Setting	* setting = GetINISetting("sLanguage:General");
		std::string sLanguage = (setting && setting->GetType() == Setting::kType_String) ? setting->data.s : "ENGLISH";
		return sLanguage;
	}
}

