#pragma once

#include <list>

typedef std::pair<TESObjectREFR*, double> Pair;

class TESObjectCELL;

class TESObjectCELLEx : public TESObjectCELL
{
private:
	enum
	{
		kFlag_Interior = 1,
		kFlag_Public = 0x20
	};

public:
	TESForm* GetOwner(void);
	bool IsPlayerOwned(void);

	template<typename Fn>
	UInt32 GetReferences(std::list<Pair> *out, Fn& fn)
	{
		UInt32 index = 0;
		UInt32 count = 0;
		TESObjectREFR* refr = nullptr;

		for (UInt32 index = 0; index < refData.maxSize; index++)
		{
			refr = refData.refArray[index].ref;
			if (refr && refData.refArray[index].unk08)
			{
				if (!fn(refr))
					continue;

				double dist = GetDistance(refr);
				Pair pair(refr, dist);

				out->push_back(pair);
				count++;
			}
		}
		return count;
	}

	inline bool IsInterior(void) const { return (unk040 & kFlag_Interior) != 0; }
	inline bool IsPublic(void) const { return (unk040 & kFlag_Public) != 0; }
};

class GridArray
{
public:
	virtual ~GridArray();

	virtual void Unk_01(void);
	virtual void Unk_02(void);
	virtual void Unk_03(void);
	virtual void Unk_04(void);
	virtual void Unk_05(void);
	virtual void Unk_06(void) = 0;
	virtual void Unk_07(void) = 0;
	virtual void Unk_08(void) = 0;
	virtual void Unk_09(void) = 0;
};

class GridCellArray : public GridArray
{
public:
	virtual ~GridCellArray();

	virtual void Unk_03(void) override;
	virtual void Unk_04(void) override;
	virtual void Unk_05(void) override;
	virtual void Unk_06(void) override;
	virtual void Unk_07(void) override;
	virtual void Unk_08(void) override;
	virtual void Unk_09(void) override;
	virtual void Unk_0A(void);

	TESObjectCELL * Get(UInt32 x, UInt32 y);
	bool IsAttached(TESObjectCELL *cell) const;
	inline UInt32 GetSize(void) { return size; }

	UInt32	unk04;
	UInt32	unk08;
	UInt32	size;
	TESObjectCELL**	cells;
};

class TES
{
public:
	virtual ~TES();

	UInt32  unk04;  //04
	UInt32  unk08;  //08
	UInt32  unk0C;  //0C
	UInt32  unk10;  //10
	UInt32  unk14;  //14
	UInt32  unk18;  //18
	UInt32  unk1C;  //1C
	UInt32  unk20;  //20
	UInt32  unk24;  //24
	UInt32  unk28;  //28
	UInt32  unk2C;  //2C
	UInt32  unk30;  //30
	UInt32  unk34;  //34
	UInt32  unk38;  //38
	UInt32  unk3C;  //3C
	UInt32  unk40;  //40
	UInt32  unk44;  //44
	UInt32  unk48;  //48
	UInt32  unk4C;  //4C
	UInt32  unk50;  //50
	UInt32  unk54;  //54
	UInt32  unk58;  //58
	UInt32  unk5C;  //5C
	UInt32  unk60;  //60
	UInt32  unk64;  //64
	UInt32  unk68;  //68
	UInt32  unk6C;  //6C
	UInt32  unk70;  //70 
	GridCellArray* cellArray; // 74
	// ... 
};

//class Main
//{
//public:
//	UInt32	unk00;
//	UInt32	unk01;
//	UInt8	unk04;		// 04 - init'd 0
//	UInt8	unk05;		// 05 - init'd 0
//	UInt8	unk06;		// 06 - init'd 0
//	UInt8	unk07;		// 07 - init'd 0
//	UInt8	unk08;		// 08 - init'd 0
//	UInt8	unk09;		// 09 - init'd 0
//	bool	bPaused;	// 0A - init'd 0
//	//...
//
//	inline bool IsPaused(void) { return bPaused; }
//};

extern RelocPtr <TES*> g_TES;
//extern RelocPtr <Main*> g_Main;

class GridCellArrayEx : public GridCellArray
{
public:
	UInt32 GetReferences(std::list<Pair> *out);
};
