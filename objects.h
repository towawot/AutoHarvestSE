#pragma once

#include "skse64/GameReferences.h"
#include <vector>

enum class ObjectType : UInt8
{
	unknown = 0,
	flora,
	fieldCrops,
	critter,
	ingredients,
	septims,
	gems,
	lockpick,
	animalHide,
	animalParts,
	oreIngot,
	soulgem,
	keys,
	clutter,
	clutterDwemer,
	clutterBroken,
	light,
	books,
	spellbook,
	skillbook,
	booksRead,
	spellbookRead,
	skillbookRead,
	scrolls,
	ammo,
	weapon,
	enchantedWeapon,
	armor,
	enchantedArmor,
	ring,
	enchantedRing,
	necklace,
	enchantedNecklace,
	potion,
	poison,
	food,
	foodIngredients,
	drink,
	oreVein,
	userlist,
	container,
	actor,
	ashPile,
};

class TESObjectREFRx : public TESObjectREFR
{
public:
	TESObjectREFRx(TESObjectREFR*) {};

	TESObjectREFR* GetAshPileRefr(void);
	SInt16 GetItemCount(void);
	SInt16 GetContainerItemCount(TESForm * thisForm);
	NiTimeController* GetTimeController(void);
	ObjectType GetObjectType(void);
	std::string GetTypeName(void);
	bool IsQuestItem(SInt32 definition);
	double GetPosValue(void);
	TESContainer * GetContainer() const;
	std::vector<TESObjectREFR*> GetLinkedRefs(BGSKeyword* keyword);
	TESForm* GetOwner(void);
	bool IsPlayerOwned(void);
	UInt32 GetWorth(void);
	double GetWeight(void);

	inline TESObjectCELL* GetParentCell(void) { return parentCell; }
	inline bool Is3DLoaded(void) { return GetNiNode() != nullptr; }

private:


	template<typename T>
	TESForm* GetIngredients(void)
	{
		if (!m_ingredients)
		{
			T obj = static_cast<T>(baseForm);
			m_ingredients = (obj && obj->produce) ? obj->produce : nullptr;
		}
		return m_ingredients;
	}

private:
	TESForm* m_ingredients;
};

class ActorEx : public Actor
{
public:
	bool IsSneaking(void);
	bool IsPlayerFollower(void);
	bool IsEssential(void);
	bool IsNotSummonable(void);
};

TESObjectREFR* GetAshPile(TESObjectREFR* refr);
bool IsBossContainer(TESObjectREFR * refr);
bool IsContainerLocked(TESObjectREFR * refr);
ObjectType ClassifyType(TESObjectREFRx* refr, bool ignoreUserlist = false);
ObjectType ClassifyType(TESForm * baseForm, bool ignoreUserlist = false);
std::string GetObjectTypeName(SInt32 num);
std::string GetObjectTypeName(TESObjectREFR* refr);
std::string GetObjectTypeName(TESForm* pForm);
std::string GetObjectTypeName(ObjectType type);
bool IsPlayable(TESForm * pForm);
