#pragma once

#include "skse64/GameThreads.h"

#include "objects.h"
#include "TESFormEx.h"
#include "iniSettings.h"

#define ESP_NAME	"AutoHarvestSE.esp"
#define QUEST_ID	0x01D8C
#define NGEF_ID		0x00D64

extern SKSETaskInterface* g_task;

class TaskBase : public TaskDelegate
{
public:
	TaskBase(void);
	virtual void Run() = 0;
	virtual void Dispose() = 0;

	static void * operator new(std::size_t size)
	{
		return Heap_Allocate(size);
	}

	static void * operator new(std::size_t size, const std::nothrow_t &)
	{
		return Heap_Allocate(size);
	}

	static void * operator new(std::size_t size, void * ptr)
	{
		return ptr;
	}

	static void operator delete(void * ptr)
	{
		Heap_Free(ptr);
	}

	static void operator delete(void * ptr, const std::nothrow_t &)
	{
		Heap_Free(ptr);
	}

	static void operator delete(void *, void *)
	{
	}
//private:
	static INIFile* m_ini;
	static UInt64 m_aliasHandle;
};

class SearchTask : public TaskBase
{
public:
	SearchTask(TESObjectREFR* refr, double distance, INIFile::PrimaryType first, INIFile::SecondaryType second);
	virtual void Run();
	virtual void Dispose();

private:
	TESObjectREFR* m_refr;
	double m_distance;
	INIFile::PrimaryType m_first;
	INIFile::SecondaryType m_second;
};

//class ContainerLootTask : public TaskBase
//{
//public:
//	ContainerLootTask(TESObjectREFR* refr, TESForm* item, INIFile::PrimaryType first, INIFile::SecondaryType second);
//	virtual void Run();
//	virtual void Dispose();
//
//private:
//	TESObjectREFR* m_refr;
//	TESForm* m_item;
//	INIFile::PrimaryType m_first;
//	INIFile::SecondaryType m_second;
//};

namespace tasks
{
	void Init(void);
}
