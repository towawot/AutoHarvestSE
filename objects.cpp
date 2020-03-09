#include "skse64/PapyrusVM.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameObjects.h"
#include "skse64/GameReferences.h"
#include "skse64/GameExtraData.h"
#include "skse64/NiNodes.h"
#include "GameFormComponents.h"
#include "utils.h"
#include "objects.h"
#include "dataCase.h"
#include "basketfile.h"
#include "containerLister.h"
#include "TESFormEx.h"
#include <string>
#include <vector>
#include <algorithm>    // std::sort
#include "BaseExtraListEx.h"

bool IsBossContainer(TESObjectREFR* refr)
{
	if (!refr)
		return false;

	BaseExtraList* extraList = &refr->extraData;
	if (!extraList)
		return nullptr;

	ExtraLocationRefType* exLocRefType = static_cast<ExtraLocationRefType*>(extraList->GetByType(kExtraData_LocationRefType));
	return (exLocRefType && exLocRefType->refType->formID == 0x0130F8);
}

bool IsContainerLocked(TESObjectREFR* refr)
{
	if (!refr)
		return false;

	BaseExtraList* extraList = &refr->extraData;
	if (!extraList)
		return false;
	if (!extraList->HasType(kExtraData_Lock))
		return false;
		
	ExtraLock* exLock = static_cast<ExtraLock*>(extraList->GetByType(kExtraData_Lock));

	if (exLock->lock->flags == REFR_LOCK::Flag::kLocked)
		return true;

	return false;
	//BaseExtraList* extraList = &refr->extraData;
	//return (extraList && extraList->HasType(kExtraData_Lock)) ? true : false;


}

TESObjectREFR* GetAshPile(TESObjectREFR* refr)
{
	//#ifdef _DEBUG
	//	_MESSAGE("TESObjectREFRx::GetAshPileRefr");
	//#endif

	if (!refr)
		return nullptr;

	NiPointer<TESObjectREFR> ashRef;
	BaseExtraList* extraList = &refr->extraData;
	if (!extraList)
		return nullptr;

	ExtraAshPileRef* exAsh = static_cast<ExtraAshPileRef*>(extraList->GetByType(kExtraData_AshPileRef));
	if (exAsh && exAsh->refHandle != 0)
	{
		LookupREFRByHandle(exAsh->refHandle, ashRef);
	}
	return ashRef;
}

std::string GetModelPath(TESForm* thisForm)
{
//#ifdef _DEBUG
//	_MESSAGE("GetModelPath");
//#endif

	std::string result;
	if (thisForm)
	{
		if (thisForm->formType == kFormType_Misc)
		{
			TESObjectMISC* miscObject = DYNAMIC_CAST(thisForm, TESForm, TESObjectMISC);
			if (miscObject)
				return miscObject->texSwap.GetModelName();
		}
		else if (thisForm->formType == kFormType_Container)
		{
			TESObjectCONT* container = DYNAMIC_CAST(thisForm, TESForm, TESObjectCONT);
			if (container)
				return container->texSwap.GetModelName();
		}
	}
	return result;
}

bool CheckObjectModelPath(TESForm* thisForm, const char* arg)
{
//#ifdef _DEBUG
//	_MESSAGE("CheckObjectModelPath");
//#endif

	if (!thisForm || strlen(arg) == 0)
		return false;
	std::string s = GetModelPath(thisForm);
	if (s.empty())
		return false;
	StringUtils::ToLower(s);
	return (s.find(arg, 0) != std::string::npos) ? true : false;
}

ObjectType TESObjectREFRx::GetObjectType()
{
//#ifdef _DEBUG
//	_MESSAGE("TESObjectREFRx::GetObjectType");
//#endif
	//if (m_objType == ObjectType::unknown)
		//m_objType = ClassifyType(this);
	//return m_objType;
	return ClassifyType(this);
}

std::string TESObjectREFRx::GetTypeName()
{
//#ifdef _DEBUG
//	_MESSAGE("TESObjectREFRx::GetObjectTypeName");
//#endif
	ObjectType objType = ClassifyType(this);
	return GetObjectTypeName(objType);
}

double TESObjectREFRx::GetPosValue()
{
	if (!this)
		return -1.0;
	double dx = this->pos.x;
	double dy = this->pos.y;
	double dz = this->pos.z;
	return (dx*dx + dy*dy + dz*dz);
}


bool TESObjectREFRx::IsQuestItem(SInt32 questitemDefinition)
{
//#ifdef _DEBUG
//	_MESSAGE("TESObjectREFRx::IsQuestObject");
//#endif

	if (!this)
		return false;

	UInt32 handle = GetOrCreateRefrHandle(this);
	if (handle == *(g_invalidRefHandle.GetPtr()))
		return false;

	NiPointer<TESObjectREFR> targetRef;
	LookupREFRByHandle(handle, targetRef);

	if (!targetRef)
		targetRef = this;

	BaseExtraListEx* extraListEx = static_cast<BaseExtraListEx*>(&targetRef->extraData);
	if (!extraListEx)
		return false;

	return extraListEx->IsQuestObject(questitemDefinition);
}

std::vector<TESObjectREFR*> TESObjectREFRx::GetLinkedRefs(BGSKeyword* keyword)
{
	std::vector<TESObjectREFR*> result;
	ExtraLinkedRef* exLinkRef = static_cast<ExtraLinkedRef*>(this->extraData.GetByType(kExtraData_LinkedRef));
	if (!exLinkRef)
		return result;

	for (UInt32 index = 0; index < exLinkRef->Length(); index++)
	{
		auto pair = exLinkRef->Get(index);
		if (!pair.refr || pair.keyword != keyword)
			continue;
		result.push_back(pair.refr);
	}
	return result;
}

TESContainer* TESObjectREFRx::GetContainer() const
{
	if (!baseForm)
		return nullptr;

	TESContainer *container = nullptr;
	if (baseForm->formType == kFormType_Container)
		container = DYNAMIC_CAST((TESObjectCONT*)baseForm, TESObjectCONT, TESContainer);
	else if (baseForm->formType == kFormType_NPC)
		container = DYNAMIC_CAST((TESActorBase*)baseForm, TESActorBase, TESContainer);

	return container;
}

TESForm * TESObjectREFRx::GetOwner(void)
{
	ExtraOwnership* extraOwner = static_cast<ExtraOwnership*>(this->extraData.GetByType(kExtraData_Ownership));
	return (extraOwner) ? extraOwner->owner : nullptr;
}

bool TESObjectREFRx::IsPlayerOwned()
{
	TESForm* owner = GetOwner();
	if (owner)
	{
		if (owner->formType == kFormType_NPC)
		{
			TESNPC* npc = DYNAMIC_CAST(owner, TESForm, TESNPC);
			TESNPC* playerBase = DYNAMIC_CAST((*g_thePlayer)->baseForm, TESForm, TESNPC);
			return (npc && npc == playerBase);
		}
		else if (owner->formType == kFormType_Faction)
		{
			TESFaction* faction = DYNAMIC_CAST(owner, TESForm, TESFaction);
			if (faction)
			{
				if ((*g_thePlayer)->IsInFaction(faction))
					return true;

				return false;
			}

			
		}
	}
	return false;
}

UInt32 TESObjectREFRx::GetWorth(void)
{
	TESFormEx* itemEx = static_cast<TESFormEx*>(baseForm);
	return (itemEx) ? itemEx->GetWorth() : 0;
}

double TESObjectREFRx::GetWeight(void)
{
	TESFormEx* itemEx = static_cast<TESFormEx*>(baseForm);
	return (itemEx) ? itemEx->GetWeight() : 0;
}

TESObjectREFR* TESObjectREFRx::GetAshPileRefr()
{
	return GetAshPile(this);
}

inline bool ActorEx::IsSneaking()
{
	return (this && ((this->actorState.flags04 & ActorState::kState_Sneaking) != 0));
}

bool ActorEx::IsPlayerFollower()
{
	if (!this)
		return false;
	static TESFaction* faction = DYNAMIC_CAST(LookupFormByID(0x05C84E), TESForm, TESFaction);
	return (faction) ? IsInFaction(faction): false;

}

bool ActorEx::IsEssential()
{
	if (!this)
		return false;
	return  (flags2 & Actor::Flags2::kFlags_Essential) != 0;
}

bool ActorEx::IsNotSummonable()
{
	if (this)
	{
		TESNPC* pNPC = DYNAMIC_CAST(baseForm, TESForm, TESNPC);
		if (pNPC && pNPC->actorData.flags >> 14 & 1)
			return false;
	}
	return true;
}

SInt16 TESObjectREFRx::GetItemCount()
{
	if (!this)
		return 1;
	if (!baseForm)
		return 1;

	ExtraCount* exCount = static_cast<ExtraCount*>(this->extraData.GetByType(kExtraData_Count));
	return (exCount) ? exCount->count : 1;
}

SInt16 TESObjectREFRx::GetContainerItemCount(TESForm* thisForm)
{

	if (!this || !thisForm || !baseForm)
		return -1;

	TESContainer * container = DYNAMIC_CAST(baseForm, TESForm, TESContainer);
	if (!container)
		return -1;

	SInt16 result = container->CountItem(thisForm);

	ExtraContainerChanges* exContainerChanges = static_cast<ExtraContainerChanges*>(extraData.GetByType(kExtraData_ContainerChanges));
	if (exContainerChanges)
	{
		InventoryEntryData* entryData = exContainerChanges->data->FindItemEntry(thisForm);
		if (entryData)
			result += entryData->countDelta;
	}

	return result;
}

NiTimeController* TESObjectREFRx::GetTimeController()
{
	NiNode* node = GetNiNode();
	return (node && node->m_controller) ? node->m_controller : nullptr;
}

ObjectType ClassifyType(TESObjectREFRx* refr, bool ignoreUserlist)
{
//#ifdef _DEBUG
//	_MESSAGE("ClassifyType");
//#endif
	if (!refr || !refr->baseForm)
		return ObjectType::unknown;

//#ifdef _DEBUG
//	_MESSAGE("ClassifyType formType %d  bformType %d", refr->formType, refr->baseForm->formType);
//#endif

	if (refr->formType == kFormType_Character)
	{
		//return ObjectType::unknown;
		return ObjectType::actor;
	}
	else if (refr->GetAshPileRefr() && refr->baseForm->formType == kFormType_Activator)
	{
		//TODO:AshPile不要?? papyrus.cpp
		return ObjectType::unknown;
	}
	else if (refr->formType == kFormType_Arrow)
	{
//		BGSProjectile* proj = DYNAMIC_CAST(refr->baseForm, TESForm, BGSProjectile);
//		if (!proj)
//		{
//#ifdef _DEBUG
//			_MESSAGE("proj1");
//#endif
//			return ObjectType::unknown;
//		}
//
//		TESAmmo* ammo = DataCase::GetInstance()->ProjToAmmo(proj);
//		if (!ammo)
//		{
//#ifdef _DEBUG
//			_MESSAGE("ammo2");
//#endif
//			return ObjectType::unknown;
//		}
		return ObjectType::ammo;
	}
	else if (refr->baseForm->formType == kFormType_SoulGem)
	{
		//TODO::リンクされたソウルジェムか？(0.1現在)papyrusで処理
		//return (!refr->extraData.GetLinkedRef(nullptr)) ? ObjectType::soulgem : ObjectType::unknown;
		return ObjectType::soulgem;
	}

	return ClassifyType(refr->baseForm, ignoreUserlist);
}

ObjectType ClassifyType(TESForm * baseForm, bool ignoreUserlist)
{
	if (!baseForm)
		return ObjectType::unknown;

	DataCase* data = DataCase::GetInstance();
	if (baseForm->formType == kFormType_Container)
	{
		//return ObjectType::unknown;
		return ObjectType::container;
	}
	else if (!ignoreUserlist && BasketFile::GetSingleton()->IsinList(BasketFile::USERLIST ,baseForm))
	{
		return ObjectType::userlist;
	}
	else if (data->lists.clutter.count(baseForm))
	{
		return ObjectType::clutter;
	}
	else if (baseForm->formType == kFormType_Flora || baseForm->formType == kFormType_Tree)
	{
		if (data->lists.septim.count(baseForm))
			return ObjectType::septims;
		else if (data->lists.flora.count(baseForm))
			return ObjectType::flora;
		else if (data->lists.fieldcrop.count(baseForm))
			return ObjectType::fieldCrops;
		else if (data->lists.critter.count(baseForm))
			return ObjectType::critter;
		else if (data->lists.orevein.count(baseForm))
			return ObjectType::oreVein;
	}
	else if (baseForm->formType == kFormType_Activator)
	{
		if (data->lists.septim.count(baseForm))
			return ObjectType::septims;
		else if (data->lists.flora.count(baseForm))
			return ObjectType::flora;
		else if (data->lists.fieldcrop.count(baseForm))
			return ObjectType::fieldCrops;
		else if (data->lists.critter.count(baseForm))
			return ObjectType::critter;
		else if (data->lists.orevein.count(baseForm))
			return ObjectType::oreVein;

		//if (pForm->formType == kFormType_Flora)
		//{
		//	TESFlora* flora = static_cast<TESFlora*>(pForm);
		//	if (flora && flora->produce.produce)
		//		return ObjectType::flora;
		//}
		//else if (pForm->formType == kFormType_Tree)
		//{
		//	TESObjectTREE* tree = static_cast<TESObjectTREE*>(pForm);
		//	if (tree && tree->produce.produce)
		//		return ObjectType::flora;
		//}
	}
	else if (baseForm->formType == kFormType_Ingredient)
	{
		return ObjectType::ingredients;
	}
	else if (baseForm->formType == kFormType_SoulGem)
	{
		//return (!refr->extraData.GetLinkedRef(nullptr)) ? ObjectType::soulgem : ObjectType::unknown;
		return ObjectType::soulgem;
	}
	else if (baseForm->formType == kFormType_Misc)
	{
		static const FormID LockPick = 0x0A;
		//static const FormID Gold = 0x00000F;
		//static const FormID Leather01 = 0x000DB5D2;
		//static const FormID LeatherStrips = 0x000800E4;

		static const FormID VendorItemAnimalHide = 0x0914EA;
		static const FormID VendorItemDaedricArtifact = 0x0917E8;
		//static const FormID VendorItemGem = 0x000914ED;
		//static const FormID VendorItemTool = 0x000914EE;
		static const FormID VendorItemAnimalPart = 0x0914EB;
		static const FormID VendorItemOreIngot = 0x0914EC;
		//static const FormID VendorItemClutter = 0x000914E9;
		//static const FormID VendorItemFireword = 0x000BECD7;

		//static const FormID RubyDragonClaw = 0x04B56C;
		//static const FormID IvoryDragonClaw = 0x0AB7BB;
		//static const FormID GlassCraw = 0x07C260;
		//static const FormID EbonyCraw = 0x05AF48;
		//static const FormID EmeraldDragonClaw = 0x0ED417;
		//static const FormID DiamondClaw = 0x0AB375;
		//static const FormID IronClaw = 0x08CDFA;
		//static const FormID CoralDragonClaw = 0x0B634C;
		//static const FormID E3GoldenClaw = 0x0999E7;
		//static const FormID SapphireDragonClaw = 0x0663D7;
		//static const FormID MS13GoldenClaw = 0x039647;

		TESObjectMISC* miscObject = DYNAMIC_CAST(baseForm, TESForm, TESObjectMISC);
		if (!miscObject)
			return ObjectType::unknown;

		if (data->lists.septim.count(baseForm))
			return ObjectType::septims;
		else if (data->lists.gem.count(baseForm))
			return ObjectType::gems;
		else if (miscObject->keyword.HasKeyword(VendorItemAnimalHide))
			return ObjectType::animalHide;
		else if (miscObject->keyword.HasKeyword(VendorItemAnimalPart))
			return ObjectType::animalParts;
		else if (miscObject->formID == LockPick)
			return ObjectType::lockpick;
		else if (CheckObjectModelPath(baseForm, "dwemer"))
			return ObjectType::clutterDwemer;
		else if (CheckObjectModelPath(baseForm, "broken"))
			return ObjectType::clutterBroken;
		else if (miscObject->keyword.HasKeyword(VendorItemOreIngot))
			return ObjectType::oreIngot;
	}
	else if (baseForm->formType == kFormType_Key)
	{
		return ObjectType::keys;
	}
	else if (baseForm->formType == kFormType_Book)
	{
		TESObjectBOOK* book = DYNAMIC_CAST(baseForm, TESForm, TESObjectBOOK);
		if (!book || (book->data.flags & TESObjectBOOK::Data::kType_CantBeTaken) != 0)
			return ObjectType::unknown;
		bool isRead = (book->data.flags & TESObjectBOOK::Data::kType_Read) != 0;
		if ((book->data.flags & TESObjectBOOK::Data::kType_Skill) != 0)
			return (isRead) ? ObjectType::skillbookRead : ObjectType::skillbook;
		else if ((book->data.flags & TESObjectBOOK::Data::kType_Spell) != 0)
			return (isRead) ? ObjectType::spellbookRead : ObjectType::spellbook;
		return (isRead) ? ObjectType::booksRead : ObjectType::books;
	}
	else if (baseForm->formType == kFormType_ScrollItem)
	{
		return ObjectType::scrolls;
	}
	else if (baseForm->formType == kFormType_Ammo)
	{
		if (baseForm->IsPlayable())
			return ObjectType::ammo;
	}
	else if (baseForm->formType == kFormType_Weapon)
	{
		const static FormID Artifacts1 = 0x0A8668;
		const static FormID Artifacts2 = 0x0917E8;

		TESObjectWEAP* weapon = DYNAMIC_CAST(baseForm, TESForm, TESObjectWEAP);
		if (!weapon || !weapon->IsPlayable())
			return ObjectType::unknown;

		if (weapon->keyword.HasKeyword(Artifacts1) || weapon->keyword.HasKeyword(Artifacts2))
			return ObjectType::unknown;

		return (weapon->enchantable.enchantment) ? ObjectType::enchantedWeapon : ObjectType::weapon;
	}
	else if (baseForm->formType == kFormType_Armor)
	{
		const static FormID necklace = 0x10CD0A;
		const static FormID ring = 0x10CD09;

		TESObjectARMO* armor = DYNAMIC_CAST(baseForm, TESForm, TESObjectARMO);
		if (!armor || !armor->IsPlayable())
			return ObjectType::unknown;

		if (armor->keyword.HasKeyword(necklace))
			return (armor->enchantable.enchantment) ? ObjectType::enchantedNecklace : ObjectType::necklace;
		if (armor->keyword.HasKeyword(ring))
			return (armor->enchantable.enchantment) ? ObjectType::enchantedRing : ObjectType::ring;

		return (armor->enchantable.enchantment) ? ObjectType::enchantedArmor : ObjectType::armor;
	}
	else if (baseForm->formType == kFormType_Potion)
	{
		const static FormID drinkSound = 0x0B6435;

		AlchemyItem* potion = DYNAMIC_CAST(baseForm, TESForm, AlchemyItem);
		if (!potion)
			return ObjectType::unknown;

		if (potion->IsFood())
			return (potion->itemData.useSound  && potion->itemData.useSound->formID == drinkSound) ? ObjectType::drink : ObjectType::food;
		else
			return (potion->IsPoison()) ? ObjectType::poison : ObjectType::potion;
	}
	else if (baseForm->formType == kFormType_Light)
	{
		if (baseForm->IsPlayable())
			return ObjectType::light;
	}

	return ObjectType::unknown;
}

std::string GetObjectTypeName(SInt32 num)
{
	ObjectType objType = static_cast<ObjectType>(num);
	return GetObjectTypeName(objType);
}

std::string GetObjectTypeName(TESObjectREFR* refr)
{
	ObjectType objType = ClassifyType(refr);
	return GetObjectTypeName(objType);
}

std::string GetObjectTypeName(TESForm* pForm)
{
	ObjectType objType = ClassifyType(pForm);
	return GetObjectTypeName(objType);
}

std::string GetObjectTypeName(ObjectType type)
{
	//#ifdef _DEBUG
	//	_MESSAGE("TESObjectREFRx::GetObjectTypeName %d", type);
	//#endif

	std::string result;

	if (type == ObjectType::unknown)
		result = "unknown";
	if (type == ObjectType::flora)
		result = "flora";
	if (type == ObjectType::fieldCrops)
		result = "fieldCrops";
	if (type == ObjectType::critter)
		result = "critter";
	if (type == ObjectType::ingredients)
		result = "ingredients";
	if (type == ObjectType::septims)
		result = "septims";
	if (type == ObjectType::gems)
		result = "gems";
	if (type == ObjectType::lockpick)
		result = "lockpick";
	if (type == ObjectType::animalHide)
		result = "animalHide";
	if (type == ObjectType::animalParts)
		result = "animalParts";
	if (type == ObjectType::oreIngot)
		result = "oreIngot";
	if (type == ObjectType::soulgem)
		result = "soulgem";
	if (type == ObjectType::keys)
		result = "keys";
	if (type == ObjectType::clutter)
		result = "clutter";
	if (type == ObjectType::clutterDwemer)
		result = "clutterDwemer";
	if (type == ObjectType::clutterBroken)
		result = "clutterBroken";
	if (type == ObjectType::light)
		result = "light";
	if (type == ObjectType::books)
		result = "books";
	if (type == ObjectType::spellbook)
		result = "spellbook";
	if (type == ObjectType::skillbook)
		result = "skillbook";
	if (type == ObjectType::booksRead)
		result = "booksRead";
	if (type == ObjectType::spellbookRead)
		result = "spellbookRead";
	if (type == ObjectType::skillbookRead)
		result = "skillbookRead";
	if (type == ObjectType::scrolls)
		result = "scrolls";
	if (type == ObjectType::ammo)
		result = "ammo";
	if (type == ObjectType::weapon)
		result = "weapon";
	if (type == ObjectType::enchantedWeapon)
		result = "enchantedWeapon";
	if (type == ObjectType::armor)
		result = "armor";
	if (type == ObjectType::enchantedArmor)
		result = "enchantedArmor";
	if (type == ObjectType::ring)
		result = "ring";
	if (type == ObjectType::enchantedRing)
		result = "enchantedRing";
	if (type == ObjectType::necklace)
		result = "necklace";
	if (type == ObjectType::enchantedNecklace)
		result = "enchantedNecklace";
	if (type == ObjectType::potion)
		result = "potion";
	if (type == ObjectType::poison)
		result = "poison";
	if (type == ObjectType::food)
		result = "food";
	if (type == ObjectType::foodIngredients)
		result = "foodIngredients";
	if (type == ObjectType::drink)
		result = "drink";
	if (type == ObjectType::oreVein)
		result = "oreVein";
	if (type == ObjectType::userlist)
		result = "userlist";
	if (type == ObjectType::container)
		result = "container";
	if (type == ObjectType::actor)
		result = "actor";
	if (type == ObjectType::ashPile)
		result = "ashPile";

	if (result.empty())	
		result = "unknown";

	return result;
}

