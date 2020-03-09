#include "skse64/GameReferences.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "skse64/PapyrusVM.h"
#include "skse64/GameExtraData.h"
#include "TESFormEx.h"
#include "dataCase.h"
#include "objects.h"
//#include <math.h>
//#include <list>

#include "TESObjectARMOEx.h"
#include "AlchemyItemEx.h"
#include "TESObjectWEAPEx.h"
#include "TESQuestEx.h"




BGSKeywordForm* TESFormEx::GetKeywordForm()
{
	BGSKeywordForm* result = nullptr;

	switch (this->formType)
	{
	case kFormType_EffectSetting:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, EffectSetting), EffectSetting, BGSKeywordForm);
		break;
	case kFormType_NPC:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESActorBase), TESActorBase, BGSKeywordForm);
		break;
	case kFormType_Race:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESRace), TESRace, BGSKeywordForm);
		break;
	case kFormType_Armor:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESObjectARMO), TESObjectARMO, BGSKeywordForm);
		break;
	case kFormType_Weapon:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESObjectWEAP), TESObjectWEAP, BGSKeywordForm);
		break;
	case kFormType_Location:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, BGSLocation), BGSLocation, BGSKeywordForm);
		break;
	case kFormType_Activator:
	case kFormType_TalkingActivator:
	case kFormType_Flora:
	case kFormType_Furniture:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESObjectACTI), TESObjectACTI, BGSKeywordForm);
		break;
	case kFormType_Enchantment:
	case kFormType_Spell:
	case kFormType_ScrollItem:
	case kFormType_Ingredient:
	case kFormType_Potion:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, MagicItem), MagicItem, BGSKeywordForm);
		break;
	case kFormType_Misc:
	case kFormType_Apparatus:
	case kFormType_Key:
	case kFormType_SoulGem:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESObjectMISC), TESObjectMISC, BGSKeywordForm);
		break;
	case kFormType_Ammo:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESAmmo), TESAmmo, BGSKeywordForm);
		break;
	case kFormType_Book:
		result = DYNAMIC_CAST(DYNAMIC_CAST(this, TESForm, TESObjectBOOK), TESObjectBOOK, BGSKeywordForm);
		break;
	default:
		result = DYNAMIC_CAST(this, TESForm, BGSKeywordForm);
		break;
	}
	return result;
}

EnchantmentItem* TESFormEx::GetEnchantment()
{
	if (!this)
		return false;

	if (formType == kFormType_Weapon)
	{
		TESObjectWEAPEx* thisWeap = static_cast<TESObjectWEAPEx*>(DYNAMIC_CAST(this, TESForm, TESObjectWEAP));
		return (thisWeap) ? thisWeap->GetEnchantment() : nullptr;
	}
	else if (formType == kFormType_Armor)
	{
		TESObjectARMOEx* thisArmor = static_cast<TESObjectARMOEx*>(DYNAMIC_CAST(this, TESForm, TESObjectARMO));
		return (thisArmor) ? thisArmor->GetEnchantment() : nullptr;
	}
	return false;
}

SInt16 TESFormEx::GetMaxCharge()
{
	if (!this)
		return 0;

	if (formType == kFormType_Weapon)
	{
		TESObjectWEAPEx* thisWeap = static_cast<TESObjectWEAPEx*>(DYNAMIC_CAST(this, TESForm, TESObjectWEAP));
		return (thisWeap) ? thisWeap->GetMaxCharge() : 0;
	}
	return 0;
}

UInt32 TESFormEx::GetGoldValue()
{
	if (!this)
		return 0;

	switch (this->formType)
	{
	case kFormType_Armor:
	case kFormType_Weapon:
	case kFormType_Enchantment:
	case kFormType_Spell:
	case kFormType_ScrollItem:
	case kFormType_Ingredient:
	case kFormType_Potion:
	case kFormType_Misc:
	case kFormType_Apparatus:
	case kFormType_Key:
	case kFormType_SoulGem:
	case kFormType_Ammo:
	case kFormType_Book:
		break;
	default:
		return 0;
	}

	TESValueForm* pValue = DYNAMIC_CAST(this, TESForm, TESValueForm);
	if (!pValue)
		return 0;

	return pValue->value;
}

double TESFormEx::GetWeight()
{
	if (!this)
		return 0.0;

	TESWeightForm* pWeight = DYNAMIC_CAST(this, TESForm, TESWeightForm);
	if (!pWeight)
		return 0.0;

	return pWeight->weight;
}

double TESFormEx::GetWorth()
{
	if (!this)
		return 0;

	double result = 0;

	if (formType == kFormType_Ammo || formType == kFormType_Projectile)
	{
		TESAmmo* ammo = nullptr;
		if (formType == kFormType_Ammo)
		{
			ammo = DYNAMIC_CAST(this, TESForm, TESAmmo);

#ifdef _DEBUG
			_MESSAGE("ammo %0.2f", ammo->settings.damage);
#endif

			result = (ammo) ? static_cast<int>(ammo->settings.damage) : 0;
		}
		else if (formType == kFormType_Projectile)
		{
			BGSProjectile* proj = DYNAMIC_CAST(this, TESForm, BGSProjectile);
			if (proj)
			{
				ammo = DataCase::GetInstance()->ProjToAmmo(proj);
				result = (ammo) ? static_cast<int>(ammo->settings.damage) : 0;
			}
		}

		//TESAttackDamageForm* pAttack = DYNAMIC_CAST(this, TESForm, TESAttackDamageForm);
		//if (pAttack)
		//	return pAttack->GetAttackDamage();
	}
	else
	{
		result = GetGoldValue();

		if (formType == kFormType_Weapon)
		{
			TESObjectWEAPEx* thisWeap = static_cast<TESObjectWEAPEx*>(DYNAMIC_CAST(this, TESForm, TESObjectWEAP));
			result = thisWeap->GetGoldValue();
		}
		else if (formType == kFormType_Armor)
		{
			TESObjectARMOEx* thisArmor = static_cast<TESObjectARMOEx*>(DYNAMIC_CAST(this, TESForm, TESObjectARMO));
			result = thisArmor->GetGoldValue();
		}
		else if (formType == kFormType_Enchantment || formType == kFormType_Spell || formType == kFormType_ScrollItem || formType == kFormType_Ingredient || formType == kFormType_Potion)
		{
			AlchemyItemEx* thisAlchemyItem = static_cast<AlchemyItemEx*>(DYNAMIC_CAST(this, TESForm, AlchemyItem));
			if (thisAlchemyItem)
				result = thisAlchemyItem->GetGoldValue();
		}
	}
	return result;
}

bool IsPlayable(TESForm* pForm)
{
	if (!pForm)
		return false;

	if (pForm->formType == kFormType_Ammo)
	{
		TESAmmo* ammo = static_cast<TESAmmo*>(pForm);
		return (ammo) ? ammo->IsPlayable() : false;
	}
	else if (pForm->formType == kFormType_Weapon)
	{
		TESObjectWEAPEx* weapEx = static_cast<TESObjectWEAPEx*>(DYNAMIC_CAST(pForm, TESForm, TESObjectWEAP));
		return (weapEx) ? weapEx->IsPlayable() : false;
	}
	else if (pForm->formType == kFormType_Light)
	{
		TESObjectLIGH* light = (TESObjectLIGH*)pForm;
		return (light->unkE0.unk0C & 2) != 0;
	}
	else if (pForm->formType == kFormType_Key || pForm->formType == kFormType_Armor || pForm->formType == kFormType_Misc)
		return pForm->IsPlayable();

	return true;
}
