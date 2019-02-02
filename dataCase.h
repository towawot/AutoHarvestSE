#pragma once

#include <unordered_set>
#include <unordered_map>
#include <vector>

class TESForm;
class TESObjectREFR;

class DataCase
{
public:
	static DataCase* GetInstance(void)
	{
		if (s_pInstance == nullptr)
			s_pInstance = new DataCase();
		return s_pInstance;
	}

	struct listData
	{
		std::unordered_map<std::string, std::string> translations;

		std::unordered_map<TESObjectREFR*, double> arrowCheck;
		std::unordered_map<BGSProjectile*, TESAmmo*> ammoList;

		std::unordered_set<TESObjectREFR*> blockContainerRefr;

		std::unordered_set<TESForm*> blockForm;
		std::unordered_set<TESObjectREFR*> blockRefr;
		std::unordered_set<TESObjectREFR*> taskLock;

		std::vector<TESSound*> sound;

		std::unordered_set<TESForm*> flora;
		std::unordered_set<TESForm*> critter;
		std::unordered_set<TESForm*> fieldcrop;
		std::unordered_set<TESForm*> gem;
		std::unordered_set<TESForm*> septim;
		std::unordered_set<TESForm*> clutter;
		std::unordered_set<TESForm*> orevein;

		std::unordered_set<TESForm*> userlist;
		std::unordered_set<TESForm*> excludelist;
	} lists;

	bool UnlockTask(TESObjectREFR* refr);
	bool LockTask(TESObjectREFR* refr);
	bool IsInTasks(TESObjectREFR* refr);

	bool BlockReference(TESObjectREFR* refr);
	bool UnblockReference(TESObjectREFR* refr);

	TESAmmo* ProjToAmmo(BGSProjectile* proj);

	void BuildList(void);
	void ListsClear(void);
private:

	bool GetTSV(std::unordered_set<TESForm*> *tsv, const char* fileName);
	bool GetTSV(std::vector<TESSound*> *tsv, const char* fileName);
	//void GetUserlistData(void);
	void GetBlockContainerData(void);
	void GetAmmoData(void);
	void GetTranslationData(void);

	static DataCase* s_pInstance;

	DataCase(void);
};

