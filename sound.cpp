#include "skse64/GameRTTI.h"
#include "skse64/GameData.h"
#include "sound.h"
#include "dataCase.h"

#define FormID UInt32

BGSSoundDescriptorForm* _GetSoundDescriptorForm(FormID formId)
{
	BGSSoundDescriptorForm* result = nullptr;
	TESForm* pForm = LookupFormByID(formId);
	if (pForm)
		result = DYNAMIC_CAST(pForm, TESForm, BGSSoundDescriptorForm);
	return result;
}

TESSound* LookupSoundByID(FormID formId)
{
	TESSound* result = nullptr;
	TESForm* pForm = LookupFormByID(formId);
	if (pForm)
		result = DYNAMIC_CAST(pForm, TESForm, TESSound);
	return result;
}

BGSSoundDescriptorForm* GetPickUpSoundDescriptor(TESForm* baseForm)
{
	if (!baseForm)
		return nullptr;

	BGSSoundDescriptorForm * result = nullptr;

	if (baseForm->formType == kFormType_Ingredient)
	{
		IngredientItem* item = DYNAMIC_CAST(baseForm, TESForm, IngredientItem);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, IngredientItem, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03C7C2);
		}
	}
	else if (baseForm->formType == kFormType_Activator)
	{
		TESObjectACTI* item = DYNAMIC_CAST(baseForm, TESForm, TESObjectACTI);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, TESObjectACTI, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03C7C2);
		}
	}
	else if (baseForm->formType == kFormType_Potion)
	{
		AlchemyItem* item = DYNAMIC_CAST(baseForm, TESForm, AlchemyItem);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, AlchemyItem, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03C7C2);
		}

		//if (!result)
		//{
		//	const static FormID drinkSound = 0x0B6435;

		//	AlchemyItem* potion = DYNAMIC_CAST(baseForm, TESForm, AlchemyItem);
		//	if (!potion)
		//		return ObjectType::unknown;

		//	if (potion->IsFood())
		//		return (potion->itemData.useSound  && potion->itemData.useSound->formID == drinkSound) ? ObjectType::drink : ObjectType::food;
		//	else
		//		return (potion->IsPoison()) ? ObjectType::poison : ObjectType::potion;
		//}
	}
	else if (baseForm->formType == kFormType_Book)
	{
		TESObjectBOOK* item = DYNAMIC_CAST(baseForm, TESForm, TESObjectBOOK);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, TESObjectBOOK, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03EDDE);
		}
	}
	else if (baseForm->formType == kFormType_Misc)
	{
		TESObjectMISC* item = DYNAMIC_CAST(baseForm, TESForm, TESObjectMISC);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, TESObjectMISC, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03C7BA);
		}
	}
	else if (baseForm->formType == kFormType_ScrollItem)
	{
		ScrollItem* item = DYNAMIC_CAST(baseForm, TESForm, ScrollItem);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, ScrollItem, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03EDDE);
		}
	}
	else if (baseForm->formType == kFormType_Note)
	{
		BGSNote* item = DYNAMIC_CAST(baseForm, TESForm, BGSNote);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, BGSNote, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x0C7A54);
		}
	}
	else if (baseForm->formType == kFormType_Ammo)
	{
		TESAmmo* item = DYNAMIC_CAST(baseForm, TESForm, TESAmmo);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, TESAmmo, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = _GetSoundDescriptorForm(0x03E7B7);
		}
	}
	else if (baseForm->formType == kFormType_Key)
	{
		result = _GetSoundDescriptorForm(0x03ED75);
	}
	else if (baseForm->formType == kFormType_Weapon)
	{
		result = _GetSoundDescriptorForm(0x03C7BE);
	}
	else if (baseForm->formType == kFormType_Armor)
	{
		const static FormID cloth = 0x06BBE8;

		TESObjectARMO* item = DYNAMIC_CAST(baseForm, TESForm, TESObjectARMO);
		if (item)
		{
			FormID formId = (item->keyword.HasKeyword(cloth)) ? 0x03C7BE : 0x03E609;
			result = _GetSoundDescriptorForm(formId);
		}
	}

	if (!result)
	{
		BGSPickupPutdownSounds *pSounds = DYNAMIC_CAST(baseForm, TESForm, BGSPickupPutdownSounds);
		if (pSounds && pSounds->pickUp)
			result = pSounds->pickUp;
		else
			result = _GetSoundDescriptorForm(0x03C7BA);
	}

	return result;
}

int soundIdx = 0;
TESSound* CreateSound(BGSSoundDescriptorForm* soundDesc)
{

	if (!soundDesc)
		return nullptr;

	DataCase* data = DataCase::GetInstance();
	static UInt32 max = data->lists.sound.size();
	if (max == 0)
		return nullptr;

	if (soundIdx >= max - 1)
		soundIdx = 0;

	TESSound* sound = data->lists.sound.at(soundIdx++);
	if (!sound)
		return nullptr;

	sound->descriptor = soundDesc;
	return sound;
}

//TESSound* CreateSound(TESForm* thisForm)
//{
//	DataCase* data = DataCase::GetInstance();
//	static UInt32 max = data->lists.sound.size();
//	if (max == 0)
//		return nullptr;
//
//	if (soundIdx >= max - 1)
//		soundIdx = 0;
//
//	BGSSoundDescriptorForm* soundDesc = GetPickUpSoundDescriptor(thisForm);
//	if (!soundDesc)
//		return nullptr;
//
//	TESSound* sound = data->lists.sound.at(soundIdx++);
//	if (!sound)
//		return nullptr;
//
//	sound->descriptor = soundDesc;
//	return sound;
//}

TESSound* CreatePickupSound(TESForm* baseForm)
{
	if (!baseForm)
		return nullptr;

	TESSound * result = nullptr;

	if (baseForm->formID == 0xF)
	{
		result = LookupSoundByID(0x333AA);
	}
	else if (baseForm->formType == kFormType_Ingredient)
	{
		IngredientItem* item = DYNAMIC_CAST(baseForm, TESForm, IngredientItem);
		if (item)
		{
			BGSPickupPutdownSounds* pSounds = DYNAMIC_CAST(item, IngredientItem, BGSPickupPutdownSounds);
			if (pSounds && pSounds->pickUp)
				result = CreateSound(pSounds->pickUp);
		}
	}
	//else if (baseForm->formType == kFormType_Activator)
	//{
	//	result = LookupSoundByID(0x014115);
	//}
	else if (baseForm->formType == kFormType_Potion)
	{
		result = LookupSoundByID(0x35227);

		//if (!result)
		//{
		//	const static FormID drinkSound = 0x0B6435;

		//	AlchemyItem* potion = DYNAMIC_CAST(baseForm, TESForm, AlchemyItem);
		//	if (!potion)
		//		return ObjectType::unknown;

		//	if (potion->IsFood())
		//		return (potion->itemData.useSound  && potion->itemData.useSound->formID == drinkSound) ? ObjectType::drink : ObjectType::food;
		//	else
		//		return (potion->IsPoison()) ? ObjectType::poison : ObjectType::potion;
		//}
	}
	else if (baseForm->formType == kFormType_Book || baseForm->formType == kFormType_ScrollItem || baseForm->formType == kFormType_Note)
	{
		result = LookupSoundByID(0x036C0F);
	}
	//else if (baseForm->formType == kFormType_Misc)
	//{
	//	result = LookupSoundByID(0x014115);
	//}
	else if (baseForm->formType == kFormType_Ammo)
	{
		result = LookupSoundByID(0x0334A6);
	}
	else if (baseForm->formType == kFormType_Key)
	{
		result = LookupSoundByID(0x035225);
	}
	else if (baseForm->formType == kFormType_Weapon)
	{
		result = LookupSoundByID(0x014116);
	}
	else if (baseForm->formType == kFormType_Armor)
	{
		const static FormID clothKeyword = 0x06BBE8;
		TESObjectARMO* item = DYNAMIC_CAST(baseForm, TESForm, TESObjectARMO);
		if (item)
			result = LookupSoundByID((!item->keyword.HasKeyword(clothKeyword)) ? 0x32876 : 0x32878);
	}

	if (!result)
		result = LookupSoundByID(0x014115);

	return result;
}
