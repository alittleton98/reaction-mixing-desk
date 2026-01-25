#pragma once
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SOundEngine/Common/AkMemoryMgr.h"
#include "AK/SOundEngine/Common/AkMemoryMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/Tools/Common/AkPlatformFuncs.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SpatialAudio/Common/AkSpatialAudio.h" 
#include "AK/Comm/AkCommunication.h"
#include "AK/IBytes.h"
#if WIN32
#include "SoundEngine/Win32/AkDefaultIOHookDeferred.h"
#else
#include "SoundEngine/POSIX/AkDefaultIOHookDeferred.h"
#endif
#include <cassert>

CAkDefaultIOHookDeferred g_WwiseIOHook;

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
		return false;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	if (g_WwiseIOHook.Init(deviceSettings) != AK_Success)
	{
#if DEBUG_CONFIG
		assert(false && "Failed to initialize Wwise IO Hook");
#endif
		return false;
	}

	// Init the sound engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
#if DEBUG_CONFIG
		assert(false && "Failed to initialize Wwise Sound Engine");
#endif
		return false;
	}

	// Init Spatial Audio
	AkSpatialAudioInitSettings spatialAudioInitSettings;
	if (AK::SpatialAudio::Init(spatialAudioInitSettings) != AK_Success)
	{
#if DEBUG_CONFIG
		assert(false && "Failed to initialize Wwise Spatial Audio");
#endif
		return false;
	}

	// Init WAAPI comms
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
#if DEBUG_CONFIG
		assert(false && "Failed to initialize Wwise Communication");
#endif
		return false;
	}

	return true;
}

void TerminateWwiseSoundEngine()
{
	// Terminate WAAPI comms. Has to be done first
	AK::Comm::Term();

	// Terminate the Sound Engine
	AK::SoundEngine::Term();

	// Terminate IO Hook
	g_WwiseIOHook.Term();

	// Terminate the Streaming Manager
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager. Has to be done last
	AK::MemoryMgr::Term();
}
