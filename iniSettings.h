#pragma once

#define INI_FILE "AutoHarvestSE.ini"

#include <unordered_map>
#include <unordered_set>

class INIFile
{
public:
	enum PrimaryType
	{
		NONE = 0,
		common,
		autoharvest,
		spell,
		LAST
	};

	enum SecondaryType
	{
		NONE2 = 0,
		itemObjects,
		containers,
		deadbodies,
		valueWeight,
		maxItemCount,
		LAST2
	};

	inline bool IsType(PrimaryType type) { return (type > PrimaryType::NONE && type < PrimaryType::LAST); }
	inline bool IsType(SecondaryType type) { return (type > SecondaryType::NONE2 && type < SecondaryType::LAST2); }
	inline bool GetIsPrimaryTypeString(PrimaryType type, std::string& result)
	{
		switch (type)
		{
		case common:
			result += "common"; break;
		case autoharvest:
			result += "autoharvest"; break;
		case spell:
			result += "spell"; break;
		default:
			return false;
		}
		return true;
	}

	inline bool GetIsSecondaryTypeString(SecondaryType type, std::string& result)
	{
		switch (type)
		{
		case itemObjects:
			result += ":itemobjects"; break;
		case containers:
			result += ":containers"; break;
		case deadbodies:
			result += ":deadbodies"; break;
		case valueWeight:
			result += ":valueWeight"; break;
		case maxItemCount:
			result += ":maxItemCount"; break;
		default:
			return false;
		}
		return true;
	}

	double GetConfigSettings(PrimaryType first, std::string str);
	double GetObjectSetSettings(PrimaryType first, SecondaryType second, std::string str);

	double GetRadius(PrimaryType first);

	void Reconfigure(void);

	static INIFile* GetInstance(void)
	{
		if (s_pInstance == nullptr)
			s_pInstance = new INIFile();
		return s_pInstance;
	}

	static void DeleteInstance(void)
	{
		delete s_pInstance;
		s_pInstance = nullptr;
	}

	void AddConfigSettings(PrimaryType first, std::string key, double value) { configMap[first][key] = value; }
	void AddObjectSetSettings(PrimaryType first, SecondaryType second, std::string key, double value) { objectSetMap[first][second][key] = value; }

	bool SaveConfigSettings(PrimaryType first, std::string key, double value);
	bool SaveObjectSetSettings(PrimaryType first, SecondaryType second, std::string key, double value);

private:
	void ShowSettings(std::string str);

	bool SetINIData(std::vector<std::string> *list, std::unordered_map<std::string, double> *map);

	bool CreateConfigMap(PrimaryType first);
	bool CreateObjectSetSettingsMap(PrimaryType type, SecondaryType sub);

	bool WriteINI(const char* section, const char* key, const char* str);

	std::string iniFilePath;
	std::unordered_map<std::string, double> configMap[PrimaryType::LAST];
	std::unordered_map<std::string, double> objectSetMap[PrimaryType::LAST][SecondaryType::LAST2];

private:
	static INIFile* s_pInstance;
	std::string GetINIlFile(void);
	INIFile(void);
};
