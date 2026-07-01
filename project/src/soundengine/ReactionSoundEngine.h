// Sound Engine for the mixing desk itself. This class manages
// - timing from the ASIO clock
// - running the Wwise Sound Engine instance used by the desk
// - loading the signal chain and passing data through it
// - manage window messages the are required for the VST3 SDKs
#pragma once
#pragma region WWISE_INCLUDES
#include "AK/SoundEngine/Common/AkSoundEngine.h"
#include "AK/SoundEngine/Common/AkMemoryMgr.h"
#include "AK/SoundEngine/Common/AkMemoryMgrModule.h"
#include "AK/SoundEngine/Common/IAkStreamMgr.h"
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/Comm/AkCommunication.h"
#include "AK/IBytes.h"
#if WIN32
#include "SoundEngine/Win32/AkDefaultIOHookDeferred.h"
#else
#include "SoundEngine/POSIX/AkDefaultIOHookDeferred.h"
#endif
#pragma endregion

#pragma region VST3_SDK_INCLUDES
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/vst/utility/stringconvert.h"
#include "pluginterfaces/vst/vsttypes.h"
#pragma endregion

#pragma region ASIO_INCLUDES
#include "common/asio.h"
#include "common/asiosys.h"
#include "host/asiodrivers.h"
#pragma endregion

#include "RtMidi.h"

using namespace Steinberg;

void OnSampleRateChange( ASIOSampleRate SampleRate );
ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
long OnAsioMessage( long Selector, long Value, void* Message, double* opt );

#define MAX_INPUT_CHANNELS 512
#define MAX_OUTPUT_CHANNELS 512
#define DEFAULT_SAMPLERATE 48000

enum class EChannelConfiguration : unsigned short
{
	MONO = 1,
	STEREO = 2,
	QUAD = 4,
	SURROUND = 6,
	SURROUND_REAR = 8,
	ATMOS = 12
};

namespace ChannelStripColors
{
	const int64_t Default_Blue = 0xFF29A3CC;
	const int64_t Default_Green = 0xFF29CC29;
	const int64_t Default_Red = 0xFFCC2929;
	const int64_t Default_Yellow = 0xFFCCC729;
	const int64_t Default_Orange = 0xFFCC6D29;
	const int64_t Default_Purple = 0xFF8E29CC;

}

class ReactionSoundEngine : public Vst::IHostApplication
{
public:
	static ReactionSoundEngine globalSoundEngine;
	static ReactionSoundEngine* Get() { return &ReactionSoundEngine::globalSoundEngine; };

	ASIOCallbacks m_asioCallbacks;

	ReactionSoundEngine();

public:
	bool LoadSoundEngineConfiguration();
	bool UnloadSoundEngineConfiguration();
	bool InitializeSoundEngine();
	bool TerminateSoundEngine();

	// VST Host Interface
	tresult PLUGIN_API getName( Vst::String128 name ) override;
	tresult PLUGIN_API createInstance( TUID cid, TUID _iid, void** obj ) override;

	DECLARE_FUNKNOWN_METHODS

	// Audio Signal 
	bool InitializeASIODevice();
	long InitializeAsioStaticData();
	ASIOError CreateAsioBuffers();
	long GetAsioBufferSize() { return mAsioBufferPreferredSize; };
	ASIOSampleRate GetAsioSampleRate() { return mSampleRate; };
	long GetAsioInputLatency() { return mInputLatency; };
	long GetAsioOutputLatency() { return mOutputLatency; };
	bool SetupSignalChain();

	// Sound Engine Loop
	void SoundEngineLoopFunc( float deltaTime );

private:
	bool bAsioDeviceInitialized = false;

	// ASIO
	ASIODriverInfo mDriverInfo;
	ASIOCallbacks mAsioCallbacks;
	AsioDrivers* mAsioDriver = 0;
	long mInputChannels;
	long mOutputChannels;
	// ASIOGetBufferSize()
	long mAsioBufferMinSize;
	long mAsioBufferMaxSize;
	long mAsioBufferPreferredSize;
	long mAsioBufferGranularity;
	// ASIOGetSampleRate()
	ASIOSampleRate mSampleRate;
	// ASIOOutputReady()
	bool mOutputReady;
	// ASIOGetLatencies ()
	long mInputLatency;
	long mOutputLatency;
	// ASIOCreateBuffers ()
	long mInputBuffers;	// becomes number of actual created input buffers
	long mOutputBuffers;	// becomes number of actual created output buffers
	ASIOBufferInfo mBufferInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // buffer info's
	// ASIOGetChannelInfo()
	ASIOChannelInfo mChannelInfos[ MAX_INPUT_CHANNELS + MAX_OUTPUT_CHANNELS ]; // channel info's
	// The above two arrays share the same indexing, as the data in them are linked together
	// Information from ASIOGetSamplePosition()
	// data is converted to double floats for easier use, however 64 bit integer can be used, too
	double nanoSeconds;
	double samples;
	double tcSamples;	// time code samples

	// bufferSwitchTimeInfo()
	ASIOTime tInfo;			// time info state
	unsigned long sysRefTime;      // system reference time, when bufferSwitch() was called
};