#include "Audio/SoundEngine.h"
#include "Common.h"
#include "Audio/Channel.h"
#include "public.sdk/source/common/memorystream.h"
#include <windows.h>
#include "public.sdk/source/common/memorystream.h"
#include "base/source/fobject.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "pluginterfaces/gui/iplugview.h"  
#include <format>


bool SoundEngine::LoadSoundEngineConfiguration()
{
	SelectedAsioDevice = EAsioDevice::VASIO_256;
	SelectedChannelConfiguration = EChannelConfiguration::MONO;
	SelectedChannelStripModel = EChannelStripModel::NATIVE;
	return true;
}

bool SoundEngine::InitializeASIODevice( EAsioDevice ChosenAsioDevice )
{
	asioInitialized = false;
	if ( !LoadAsioDriver( ChosenAsioDevice ) )
		return asioInitialized;

	if ( ASIOInit( &DriverInfo ) != ASE_OK )
		return asioInitialized;

	printf( "asioVersion:   %d\n"
		"driverVersion: %d\n"
		"Name:          %s\n"
		"ErrorMessage:  %s\n",
		DriverInfo.asioVersion, DriverInfo.driverVersion,
		DriverInfo.name, DriverInfo.errorMessage );

	if ( InitializeAsioStaticData() != 0 )
		return asioInitialized;

	if ( CreateAsioBuffers() != ASE_OK )
		return asioInitialized;

	asioInitialized = true;

	return asioInitialized;
}

bool SoundEngine::InitializeSignalChain()
{
	signalChainInitialized = false;

	// Check for Wwise or the ASIO
	if ( !asioInitialized )
		return signalChainInitialized;

	string error;
	module_sslNativeChannelStrip2 = VST3::Hosting::Module::create( VST_SSL_NATIVE_CHANNELSTRIP, error );
	if ( !module_sslNativeChannelStrip2.get() )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize SSL Native Channel Strip 2\n" && error.c_str() );
#endif
		return signalChainInitialized;
	}

	module_sslFourKEChannelStrip = VST3::Hosting::Module::create( VST_SSL_4000E_CHANNELSTRIP, error );
	if ( !module_sslFourKEChannelStrip.get() )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize SSL 4000E Channel Strip" );
#endif
		return signalChainInitialized;
	}

	module_sslFourKGChannelStrip = VST3::Hosting::Module::create( VST_SSL_4000G_CHANNELSTRIP, error );
	if ( !module_sslFourKGChannelStrip.get() )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize SSL Native Channel Strip 2" );
#endif
		return signalChainInitialized;
	}

	module_sslBusCompressor2 = VST3::Hosting::Module::create( VST_SSL_NATIVE_BUSCOMPRESSOR, error );
	if ( !module_sslBusCompressor2.get() )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize SSL Native Channel Strip 2" );
#endif
		return signalChainInitialized;
	}

	module_sslMeterPro = VST3::Hosting::Module::create( VST_SSL_METER, error );
	if ( !module_sslMeterPro.get() )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize SSL Native Channel Strip 2" );
#endif
		return signalChainInitialized;
	}

	Vst::PluginContextFactory::instance().setPluginContext( this );

	for ( int indexChannels = 0; indexChannels < 4; indexChannels++ )
	{
		AudioChannel* newChannel = new AudioChannel();
		newChannel->ChannelName = "Track " + indexChannels;

		switch ( SelectedChannelStripModel )
		{
			case EChannelStripModel::NATIVE:
				newChannel->SetModules( module_sslNativeChannelStrip2, module_sslBusCompressor2, module_sslMeterPro );
				break;
			case EChannelStripModel::FOUR_THOUSAND_E:
				newChannel->SetModules( module_sslFourKEChannelStrip, module_sslBusCompressor2, module_sslMeterPro );
				break;
			case EChannelStripModel::FOUR_THOUSAND_G:
				newChannel->SetModules( module_sslFourKGChannelStrip, module_sslBusCompressor2, module_sslMeterPro );
				break;
			default:
				break;
		}

		if ( !newChannel->InitializeChannel( this ) )
		{
#if DEBUG_CONFIG
			assert( "Failed to initialize Channel %i : %s", indexChannels, newChannel->ChannelName );
#endif
			return signalChainInitialized;
		}
	}

	signalChainInitialized = true;
	return signalChainInitialized;
}

bool SoundEngine::ProcessSignalChain()
{
	return false;
}

bool SoundEngine::InitializeControlSurface()
{
	return false;
}

// Initializes the Wwise Sound Engine with default memory settings.
bool SoundEngine::InitializeWwiseSoundEngine()
{
	wwiseSoundEngineInitialized = false;
	// Init memory settings
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings( memSettings );

	if ( AK::MemoryMgr::Init( &memSettings ) != AK_Success )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize Wwise Memory Manager" );
#endif
		return wwiseSoundEngineInitialized;
	}

	// Init streaming manager
	AkStreamMgrSettings streamSettings;
	AK::StreamMgr::GetDefaultSettings( streamSettings );
	if ( !AK::StreamMgr::Create( streamSettings ) )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to create Wwise Streaming Manager" );
#endif
		return wwiseSoundEngineInitialized;
	}

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings( deviceSettings );

	if ( wwiseIOHook.Init( deviceSettings ) != AK_Success )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize Wwise IO Hook" );
#endif
		return wwiseSoundEngineInitialized;
	}

	// Init the sound engine
	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings( initSettings );
	AK::SoundEngine::GetDefaultPlatformInitSettings( platformInitSettings );

	if ( AK::SoundEngine::Init( &initSettings, &platformInitSettings ) != AK_Success )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize Wwise Sound Engine" );
#endif
		return wwiseSoundEngineInitialized;
	}

	// Init Spatial Audio
	AkSpatialAudioInitSettings spatialAudioInitSettings;
	if ( AK::SpatialAudio::Init( spatialAudioInitSettings ) != AK_Success )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize Wwise Spatial Audio" );
#endif
		return wwiseSoundEngineInitialized;
	}

	// Init WAAPI comms
	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings( commSettings );
	strcpy_s( commSettings.szAppNetworkName, "ReactionMixingDesk" );
	if ( AK::Comm::Init( commSettings ) != AK_Success )
	{
#if DEBUG_CONFIG
		assert( false && "Failed to initialize Wwise Communication" );
#endif
		return wwiseSoundEngineInitialized;
	}

	wwiseSoundEngineInitialized = true;
	return wwiseSoundEngineInitialized;
}

bool SoundEngine::InitializeWwiseComms()
{
	return false;
}

bool SoundEngine::TerminateWwiseComms()
{
	return false;
}

bool SoundEngine::TerminateWwiseSoundEngine()
{
	// Terminate WAAPI comms. Has to be done first
	AK::Comm::Term();

	// Terminate the Sound Engine
	AK::SoundEngine::Term();

	// Terminate IO Hook
	wwiseIOHook.Term();

	// Terminate the Streaming Manager
	if ( AK::IAkStreamMgr::Get() )
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager. Has to be done last
	AK::MemoryMgr::Term();

	return true;
}

bool SoundEngine::TerminateSoundEngine()
{
	// Terminate WAAPI comms. Has to be done first
	AK::Comm::Term();

	// Terminate the Sound Engine
	AK::SoundEngine::Term();

	// Terminate IO Hook
	wwiseIOHook.Term();

	// Terminate the Streaming Manager
	if ( AK::IAkStreamMgr::Get() )
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate the Memory Manager. Has to be done last
	AK::MemoryMgr::Term();
	return true;
}

bool SoundEngine::ReloadSoundEngine()
{
	return false;
}

bool SoundEngine::LoadAsioDriver( EAsioDevice ChosenAsioDevice )
{
	if ( !AsioDriver )
		AsioDriver = new AsioDrivers();

	if ( AsioDriver )
	{
		switch ( ChosenAsioDevice )
		{
			case EAsioDevice::VASIO_64:
				return AsioDriver->loadDriver( VASIO_64_DRIVER );
			case EAsioDevice::VASIO_256:
				return AsioDriver->loadDriver( VASIO_256_DRIVER );
			case EAsioDevice::VASIO_512:
				return AsioDriver->loadDriver( VASIO_512_DRIVER );
			case EAsioDevice::NONE:
				return false;
			default:
				return false;
		}
	}

	return false;
}

long SoundEngine::InitializeAsioStaticData( /*AudioDeviceDriverInfo* DriverInfo*/ )
{
	// collect the informational data of the driver
		// get the number of available channels
	if ( ASIOGetChannels( &inputChannels, &outputChannels ) == ASE_OK )
	{
		printf( "ASIOGetChannels (inputs: %d, outputs: %d);\n", inputChannels, outputChannels );

		// get the usable buffer sizes
		if ( ASIOGetBufferSize( &minSize, &maxSize, &preferredSize, &granularity ) == ASE_OK )
		{
			printf( "ASIOGetBufferSize (min: %d, max: %d, preferred: %d, granularity: %d);\n",
				minSize, maxSize,
				preferredSize, granularity );

			// get the currently selected sample rate
			if ( ASIOGetSampleRate( &sampleRate ) == ASE_OK )
			{
				printf( "ASIOGetSampleRate (sampleRate: %f);\n", sampleRate );
				if ( sampleRate <= 0.0 || sampleRate > 96000.0 )
				{

					if ( ASIOCanSampleRate( DEFAULT_SAMPLERATE ) )
						return -7;

					// Driver does not store it's internal sample rate, so set it to a know one.
					// Usually you should check beforehand, that the selected sample rate is valid
					// with ASIOCanSampleRate().
					if ( ASIOSetSampleRate( DEFAULT_SAMPLERATE ) == ASE_OK )
					{
						if ( ASIOGetSampleRate( &sampleRate ) == ASE_OK )
							printf( "ASIOGetSampleRate (sampleRate: %f);\n", sampleRate );
						else
							return -6;
					}
					else
						return -5;
				}

				// check wether the driver requires the ASIOOutputReady() optimization
				// (can be used by the driver to reduce output latency by one block)
				if ( ASIOOutputReady() == ASE_OK )
					postOutput = true;
				else
					postOutput = false;
				printf( "ASIOOutputReady(); - %s\n", postOutput ? "Supported" : "Not supported" );

				return 0;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}

ASIOError SoundEngine::CreateAsioBuffers()
{
	// create buffers for all inputs and outputs of the card with the 
		// preferredSize from ASIOGetBufferSize() as buffer size
	long i;
	ASIOError result;

	// fill the bufferInfos from the start without a gap
	ASIOBufferInfo* info = bufferInfos;

	// prepare inputs (Though this is not necessaily required, no opened inputs will work, too
	if ( inputChannels > MAX_INPUT_CHANNELS )
		inputBuffers = MAX_INPUT_CHANNELS;
	else
		inputBuffers = inputChannels;
	for ( i = 0; i < inputBuffers; i++, info++ )
	{
		info->isInput = ASIOTrue;
		info->channelNum = i;
		info->buffers[ 0 ] = info->buffers[ 1 ] = 0;
	}

	// prepare outputs
	if ( outputChannels > MAX_OUTPUT_CHANNELS )
		outputBuffers = MAX_OUTPUT_CHANNELS;
	else
		outputBuffers = outputChannels;
	for ( i = 0; i < outputBuffers; i++, info++ )
	{
		info->isInput = ASIOFalse;
		info->channelNum = i;
		info->buffers[ 0 ] = info->buffers[ 1 ] = 0;
	}

	// create and activate buffers
	result = ASIOCreateBuffers( bufferInfos,
		inputBuffers + outputBuffers,
		preferredSize, &AsioCallbacks );
	if ( result == ASE_OK )
	{
		// now get all the buffer details, sample word length, name, word clock group and activation
		for ( i = 0; i < inputBuffers + outputBuffers; i++ )
		{
			channelInfos[ i ].channel = bufferInfos[ i ].channelNum;
			channelInfos[ i ].isInput = bufferInfos[ i ].isInput;
			result = ASIOGetChannelInfo( &channelInfos[ i ] );
			if ( result != ASE_OK )
				break;
		}

		if ( result == ASE_OK )
		{
			// get the input and output latencies
			// Latencies often are only valid after ASIOCreateBuffers()
			// (input latency is the age of the first sample in the currently returned audio block)
			// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
			result = ASIOGetLatencies( &inputLatency, &outputLatency );
			if ( result == ASE_OK )
				printf( "ASIOGetLatencies (input: %d, output: %d);\n", inputLatency, outputLatency );
		}
	}
	return result;
}

AKRESULT SoundEngine::SetupListener()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::RegisterGameObject()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::UnregisterGameObject()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::LoadBank()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::UnloadBank()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::PostAudioEvent()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::PauseSoundEngine()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::StopAll()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetMultiplePositions()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetAuxSends()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetInSpatialAudioRoom()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetStateValue()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetSwitchValue()
{
	return AKRESULT::AK_Success;
}

AKRESULT SoundEngine::SetRtpcValue()
{
	return AKRESULT::AK_Success;
}