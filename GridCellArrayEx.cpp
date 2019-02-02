#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#include "skse64/GameRTTI.h"
#include "skse64/GameObjects.h"
#include "skse64/GameReferences.h"
#include "dataCase.h"
#include "objects.h"
#include "GridCellArrayEx.h"
#include "skse64/GameExtraData.h"


#if SKSE_VERSION_RELEASEIDX == 55	// RUNTIME_VERSION_1_5_23
#define ADDR_TES	0x02F4CAA0
#elif SKSE_VERSION_RELEASEIDX == 56 || SKSE_VERSION_RELEASEIDX == 57 \
	|| SKSE_VERSION_RELEASEIDX == 58|| SKSE_VERSION_RELEASEIDX == 59 \
	|| SKSE_VERSION_RELEASEIDX == 60 // RUNTIME_VERSION_1_5_39||50||53||53||62
#define ADDR_TES	0x02F4DB20
#else
#error unsupported runtime version
#endif

RelocPtr <TES*> g_TES(ADDR_TES);
//RelocPtr <Main*> g_Main(ADDR_Main);

TESForm* TESObjectCELLEx::GetOwner(void)
{
	BSExtraData* extraData = unk048.extraData;
	while (extraData)
	{
		if (extraData->GetType() == kExtraData_Ownership)
		{
#ifdef _DEBUG
			_MESSAGE("TESObjectCELLEx::GetOwner Hit %08x", ((ExtraOwnership*)extraData)->owner->formID);
#endif
			return ((ExtraOwnership*)extraData)->owner;
		}
		extraData = extraData->next;
	}
	return nullptr;
}

bool TESObjectCELLEx::IsPlayerOwned()
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

TESObjectCELL * GridCellArray::Get(UInt32 x, UInt32 y)
{
	if (!cells || x >= size || y >= size)
		return nullptr;
	return cells[x + y*size];
}

bool GridCellArray::IsAttached(TESObjectCELL *cell) const
{
	if (!cells || !cell)
		return false;
	const UInt32 length = size * size;
	for (UInt32 i = 0; i < length; ++i)
	{
		if (cells[i] == cell)
			return true;
	}
	return false;
}

double GetDistance(TESObjectREFR* refr)
{
	double dx = refr->pos.x - (*g_thePlayer)->pos.x;
	double dy = refr->pos.y - (*g_thePlayer)->pos.y;
	double dz = refr->pos.z - (*g_thePlayer)->pos.z;
	return pow(dx*dx + dy*dy + dz*dz, 0.5);
}

struct cmp_t {
	bool operator()(TESObjectREFR* refr)
	{
		//#ifdef _DEBUG
		//		const char* name = CALL_MEMBER_FN(refr, GetReferenceName)();
		//		_MESSAGE("GetReference %s(%08x)", name, refr->formID);
		//#endif

		if (!refr || refr == *g_thePlayer)
			return false;

		DataCase* data = DataCase::GetInstance();
		if (data->IsInTasks(refr))
			return false;
		if (data->lists.blockRefr.count(refr))
			return false;
		if (data->lists.blockContainerRefr.count(refr))
			return false;
		if (data->lists.blockForm.count(refr->baseForm))
			return false;


		TESObjectREFRx* refrx = static_cast<TESObjectREFRx*>(refr);
		if (!refrx || !refrx->Is3DLoaded())
			return false;

		TESFullName* fullName = DYNAMIC_CAST(refr->baseForm, TESForm, TESFullName);
		if (!fullName || !fullName->name.c_str() || strlen(fullName->name.c_str()) == 0)
		{
			data->lists.blockForm.insert(refr->baseForm);
			return false;
		}

		if (refr->formType == kFormType_Character)
		{
			if (!refr->IsDead(true))
				return false;

			//			ActorEx* actorEx = static_cast<ActorEx*>(refr);
			//			if (actorEx)
			//			{
			//#ifdef _DEBUG
			//				_MESSAGE("ActorEx OK");
			//#endif
			//				if (actorEx->IsPlayerFollower() || actorEx->IsEssential())
			//				{
			//					data->BlockReference(refr);
			//					return false;
			//				}
			//			}
		}

		if ((refr->baseForm->formType == kFormType_Flora || refr->baseForm->formType == kFormType_Tree) && (!(refr->flags & TESObjectREFR::kFlag_Harvested) == 0))
			return false;

		return true;
	}
} cmp;

UInt32 GridCellArrayEx::GetReferences(std::list<Pair> *out)
{
	int x, y;
	UInt32 count = 0;

	TESObjectCELLEx* parent = static_cast<TESObjectCELLEx*>((*g_thePlayer)->parentCell);
	if (parent->IsInterior())
	{
		count += parent->TESObjectCELLEx::GetReferences(out, cmp);
	}
	else
	{
		for (x = 0; x < GetSize(); x++)
		{
			for (y = 0; y < GetSize(); y++)
			{
				TESObjectCELLEx* cell = static_cast<TESObjectCELLEx*>(Get(x, y));
				if (cell && cell->unk044 == 7)
				{
					count += cell->GetReferences(out, cmp);
				}
			}
		}
	}

	if (count >= 2)
	{
		out->sort([](Pair a, Pair b) {return a.second < b.second; });
	}
	return count;
}