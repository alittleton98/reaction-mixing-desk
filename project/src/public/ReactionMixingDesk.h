// Primary Reaction Mixing Desk application
// Launches the desk and initializes the core modules
// Allows for use of a config file
#pragma once
#include "Audio/SoundEngine.h"
// Note that available channels are always channel count - 8 to account for the Master Bus which is always 7.1. Master is always represented as linked multimono
enum EMixingDeskSetup
{
	MONO_NATIVE_64,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000G_64,				// Busses are loaded as Mono, 4KG Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000E_64,				// Busses are loaded as Mono, 4KE Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	STEREO_NATIVE_64,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000G_64,			// Busses are loaded as Stereo, 4KG Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000E_64,			// Busses are loaded as Stereo, 4KE Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	MONO_NATIVE_256,			// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000G_256,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000E_256,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	STEREO_NATIVE_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000G_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000E_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	WWISE_SOUNDENGINE_NOREC,	// Busses are loaded as multi mono channels based on the loaded wwise project. Output device is not initialized as the mixing desk will serve primarily as a control surface
	WWISE_SOUNDENGINE_REC,		// Busses are loaded as multi mono channels based on the loaded wwise project. Output device is initialized, but uses an extra midi layer for control of both ends of the desk. 
	WWISE_CONTROLSURFACE
};

SoundEngine* gSoundEngine = nullptr;


