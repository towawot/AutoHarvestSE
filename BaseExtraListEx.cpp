#include "BaseExtraListEx.h"


EnchantmentItem * BaseExtraListEx::GetEnchantment(void)
{
	if (!this)
		return false;

	ExtraEnchantment* exEnchant = static_cast<ExtraEnchantment*>(GetByType(kExtraData_Enchantment));
	return (exEnchant && exEnchant->enchant) ? exEnchant->enchant : nullptr;
}

bool BaseExtraListEx::IsQuestObject(SInt32 definition)
{
	if (!this)
		return false;

	ExtraAliasInstanceArray* exAliasArray = static_cast<ExtraAliasInstanceArray*>(GetByType(kExtraData_AliasInstanceArray));
	if (!exAliasArray)
		return false;

	if (definition == 0)
		return true;

	for (int index = 0; index < exAliasArray->aliases.count; index++)
	{
		ExtraAliasInstanceArray::AliasInfo * aliasInfo = nullptr;
		if (exAliasArray->aliases.GetNthItem(index, aliasInfo))
		{
			if (!aliasInfo)
				continue;

			BGSBaseAlias* baseAlias = aliasInfo->alias;
			if (!baseAlias)
				continue;

			if ((baseAlias->flags >> 2) & 1)
				return true;
		}
	}
	return false;
}

