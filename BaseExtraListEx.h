#pragma once

#include "skse64/GameObjects.h"
#include "skse64/GameExtraData.h"

class BaseExtraListEx : public BaseExtraList
{
public:
	EnchantmentItem* GetEnchantment(void);
	bool IsQuestObject(SInt32 definition);
};


struct REFR_LOCK
{
	enum class Flag : UInt8
	{
		kNone = 0,
		kLocked = 1 << 0,
		kLeveled = 1 << 2
	};

	void		SetLocked(bool a_locked);


	// members
	SInt8	baseLevel;	// 00
	UInt8	pad01;		// 01
	UInt16	pad02;		// 02
	UInt32	pad04;		// 04
	TESKey*	key;		// 08
	Flag	flags;		// 10
	UInt8	pad11;		// 11
	UInt16	pad12;		// 12
	UInt32	numTries;	// 14
	UInt32	unk18;		// 18
	UInt32	pad1C;		// 1C
};
STATIC_ASSERT(sizeof(REFR_LOCK) == 0x20);


class ExtraLock : public BSExtraData
{
public:
	ExtraLock();
	virtual ~ExtraLock();

	REFR_LOCK* lock;  // 10
};
