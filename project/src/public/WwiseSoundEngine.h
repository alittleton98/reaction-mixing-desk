// Header for the inclusion and initialization of the Wwise Sound Engine as a standalone sound engine.
// The engine is not assumed to running alongside the authoring tool, and is not expected to have a connection to the Wwise Authoring API (WAAPI), but is capable of doing so

#pragma once
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "AK/SoundEngine/Common/AkMemoryMgrModule.h"
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
#include <string.h>

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
	strcpy_s(commSettings.szAppNetworkName, "Waffles");
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

// Main Timing Loop of the host application. 
bool MainHostLoop()
{


	AK::SoundEngine::RenderAudio();
	return true;
}

/// Setup Emitter and Listener game objects for spatial audio rendering
/// Only one listener is supported
bool SetupListener()
{
	return true;
}

// For each registered emitter game object, set its starting position and orientation
bool SetupEmitters()
{
	return true;
}


/// Register the game object with Wwise Sound Engine.
bool SetupSpatialAudioEnvironment()
{
	return true;
}

/// Post an AkAudioEvent to the registered game object.
bool PostAkAudioEvent()
{
	return true;
}

/// Pauses the Wwise Sound Engine playback. All audio that is currently playing will be paused.
/// The transport is assumed to have paused at the current position in the timeline.
bool PauseWwisePlayback()
{
	return true;
}


/// Clear existing playback for all registered game objects.
/// Called when the transport is stopped or reset. The transport is assumed to have reset to the beginning of the timeline when this is called. 
/// If the transport is played from a position other than the beginning of the timeline, only the events following the seeked position will be posted.
bool ClearWwisePlayback()
{
	AK::SoundEngine::StopAll();
	return true;
	
}

void SetupWwiseConnection()
{
	AK::Comm::GetCurrentSettings();
}

