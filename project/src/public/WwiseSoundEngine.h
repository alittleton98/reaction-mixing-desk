#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SOundEngine/Common/AkMemoryMgr.h"
#include "AK/SOundEngine/Common/AkMemoryMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"
#include "AK/IBytes.h"
#include <cassert>

AK::StreamMgr::IAkFileLocationResolver* g_fileLocationResolver = nullptr;

// Initializes the Wwise Sound Engine with default memory settings.
bool InitializeWwiseSoundEngine()
{
	// Init memory settings
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
#if DEBUG_CONFIG
		assert(false && "Failed to initialize Wwise Memory Manager");
#endif
		return false;
	}

	// Init streaming manager
	AkStreamMgrSettings streamSettings;
	AK::StreamMgr::GetDefaultSettings(streamSettings);
	if (!AK::StreamMgr::Create(streamSettings))
	{
#if DEBUG_CONFIG
		assert(false && "Failed to create Wwise Streaming Manager");
#endif
		AK::MemoryMgr::Term();
		return false;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	// Init the sound engine
	

	// Init Spatial Audio

	// Init WAAPI comms
	return true;
}