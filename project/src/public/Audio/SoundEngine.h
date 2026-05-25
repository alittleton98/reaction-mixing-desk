// Core engine class for the mixing desk. Audio device and Wwise are managed by this class
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

#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "pluginterfaces/vst/vsttypes.h"

#include "asiosdk/common/asio.h"
#include "asiosdk/common/asiosys.h"
#include "asiosdk/host/asiodrivers.h"

#include <cassert>
#include <string.h>
#include "Common.h"

void OnSampleRateChange( ASIOSampleRate SampleRate );
ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
long OnAsioMessage( long Selector, long Value, void* Message, double* opt );

#define MAX_INPUT_CHANNELS 512
#define MAX_OUTPUT_CHANNELS 512

using namespace Steinberg;
class AudioChannel;

//namespace Steinberg
//{
//	FUnknown* gStandardPluginContext = nullptr;
//}

class SoundEngine : public Vst::HostApplication
{

public:
	static SoundEngine gSoundEngine;
	static SoundEngine* GetSoundEngine() { return &SoundEngine::gSoundEngine; }

	ASIOCallbacks Callbacks;
public:
	// Start up/Shut down
	bool LoadSoundEngineConfiguration( EMixingDeskOperatingMode OperatingMode );
	bool InitializeASIODevice();
	bool InitializeSignalChain();
	bool InitializeControlSurface();
	bool InitializeWwiseSoundEngine();
	bool LinkToWwiseAuthoring();
	bool TerminateWwiseSoundEngine();
	bool TerminateWwiseComms();
	bool TerminateSoundEngine();
	bool TerminateControlSurface();
	bool IsFullyInitialized() { return false; };
	bool ReloadSoundEngine();

	// 
	bool Tick( float DeltaTime );

	// ASIO Implementation
	bool LoadAsioDriver( EAsioDevice ChosenAsioDevice );
	long InitializeAsioStaticData();
	ASIOError CreateAsioBuffers();
	long GetAsioBufferSize() { return preferredSize; };
	ASIOSampleRate GetAsioSampleRate() { return sampleRate; };
	long GetAsioInputLatency() { return inputLatency; };
	long GetAsioOutputLatency() { return outputLatency; };

	// Signal Chain
	bool ProcessSignalChain();

	// Wwise implementation
	AKRESULT SetupListener();
	AKRESULT RegisterGameObject();
	AKRESULT UnregisterGameObject();
	AKRESULT LoadBank();
	AKRESULT UnloadBank();
	AKRESULT PostAudioEvent();
	AKRESULT PauseSoundEngine();
	AKRESULT StopAll();
	AKRESULT SetMultiplePositions();
	AKRESULT SetAuxSends();
	AKRESULT SetInSpatialAudioRoom();
	AKRESULT SetStateValue();
	AKRESULT SetSwitchValue();
	AKRESULT SetRtpcValue();
	// Memory

	// Function
	bool StartSoundEngine();
	/*bool TickSoundEngine( float DeltaTime );*/

private:
	// Wwise
	string ProjectPath;
	CAkDefaultIOHookDeferred wwiseIOHook;

	// Initialization
	bool asioInitialized = false;
	bool signalChainInitialized = false;
	bool wwiseSoundEngineInitialized = false;
	bool wwiseCommsInitialized = false;
	bool bShouldUseAsioAsTimingMechanism = true;
	bool bShouldLoadAudioProcessing = false;
	bool bShouldLoadWwiseSoundEngine = false;

	// Mixing Desk Setup
	EAsioDevice SelectedAsioDevice;
	EChannelConfiguration SelectedChannelConfiguration;
	EChannelStripModel SelectedChannelStripModel;
	vector<AudioChannel*> AudioChannels;

	// ASIO
	ASIODriverInfo	DriverInfo;
	ASIOCallbacks	AsioCallbacks;
	AsioDrivers* AsioDriver = 0;
	long			inputChannels;
	long			outputChannels;

	// ASIOGetBufferSize()
	long           minSize;
	long           maxSize;
	long           preferredSize;
	long           granularity;

	// ASIOGetSampleRate()
	ASIOSampleRate sampleRate;

	// ASIOOutputReady()
	bool           postOutput;

	// ASIOGetLatencies ()
	long           inputLatency;
	long           outputLatency;

	// ASIOCreateBuffers ()
	long inputBuffers;	// becomes number of actual created input buffers
	long outputBuffers;	// becomes number of actual created output buffers
	ASIOBufferInfo bufferInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // buffer info's

	// ASIOGetChannelInfo()
	ASIOChannelInfo channelInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // channel info's
	// The above two arrays share the same indexing, as the data in them are linked together

	// Information from ASIOGetSamplePosition()
	// data is converted to double floats for easier use, however 64 bit integer can be used, too
	double         nanoSeconds;
	double         samples;
	double         tcSamples;	// time code samples

	// bufferSwitchTimeInfo()
	ASIOTime       tInfo;			// time info state
	unsigned long  sysRefTime;      // system reference time, when bufferSwitch() was called

	// Signal the end of processing in this example
	bool           stopped;


	// VST3
	VST3::Hosting::Module::Ptr module_sslNativeChannelStrip2;
	VST3::Hosting::Module::Ptr module_sslFourKEChannelStrip;
	VST3::Hosting::Module::Ptr module_sslFourKGChannelStrip;
	VST3::Hosting::Module::Ptr module_sslBusCompressor2;
	VST3::Hosting::Module::Ptr module_sslMeterPro;
	Vst::HostApplication context;




};

