#include "skse64/PluginAPI.h"
#include "skse64_common/skse_version.h"
#include "skse64/PapyrusVM.h"
#include "papyrus.h"
#include "tasks.h"
#include <shlobj.h>

IDebugLog	gLog;
UInt32 g_skseVersion;
SKSEMessagingInterface	* g_messaging = nullptr;

void SKSEMessageHandler(SKSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case SKSEMessagingInterface::kMessage_DataLoaded:
		//tasks::Init();
		papyrus::RegisterFuncs((*g_skyrimVM)->GetClassRegistry());
		break;
	case SKSEMessagingInterface::kMessage_NewGame:
	case SKSEMessagingInterface::kMessage_PostLoadGame:
		g_FirstRunForceDisable = true;
		tasks::Init();
		break;
	}
}

extern "C"
{

bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)
{
	SInt32	logLevel = IDebugLog::kLevel_DebugMessage;
	gLog.SetLogLevel((IDebugLog::LogLevel)logLevel);

	gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim Special Edition\\SKSE\\AutoHarvestSE.log");

	_MESSAGE("AutoHarvestSE.dll");

	g_skseVersion = skse->skseVersion;

	info->infoVersion = PluginInfo::kInfoVersion;
	info->name =	"AutoHarvestSE";
	info->version = CURRENT_RELEASE_RUNTIME;

	if (skse->isEditor)
	{
		return false;
	}
	
	//_FormEx/GridCellArrayEx.cppのreleaseIDXも直す事!!
	if (skse->runtimeVersion != CURRENT_RELEASE_RUNTIME)
	{
		_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);
		return false;
	}

	g_messaging = (SKSEMessagingInterface *)skse->QueryInterface(kInterface_Messaging);
	if (!g_messaging)
	{
		return false;
	}

	g_task = (SKSETaskInterface *)skse->QueryInterface(kInterface_Task);
	if (!g_task)
	{
		return false;
	}

	return true;
}

bool SKSEPlugin_Load(const SKSEInterface * skse)
{
	if (g_messaging)
	{
		g_messaging->RegisterListener(skse->GetPluginHandle(), "SKSE", SKSEMessageHandler);
	}
	return true;
}

};
