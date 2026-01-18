#define REAPERAPI_IMPLEMENT
#include "MixingDeskCommands.h"
#include "ReactionMixingDesk.h"

extern "C" REAPER_PLUGIN_DLL_EXPORT int REAPER_PLUGIN_ENTRYPOINT(REAPER_PLUGIN_HINSTANCE hInstance, reaper_plugin_info_t* reaperPluginInfo) {
	if (!reaperPluginInfo) 
	{
		// Unregister plugin and clean up
		
		// return if no plugin info (called on unload)
		return 0;
	}
	
	if (reaperPluginInfo->caller_version != REAPER_PLUGIN_VERSION) 
	{
		// Incompatible version
		return 0;
	}

	if (REAPERAPI_LoadAPI(reaperPluginInfo->GetFunc) != 0)
	{
		// Failed to load API
		return 0; 
	}
	
	return 1; // Successful Initialization
}