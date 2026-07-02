#include "ReactionSoundEngine.h"
#include "pluginterfaces/vst/ivstmessage.h"
#include "minwindef.h"

ReactionSoundEngine ReactionSoundEngine::globalSoundEngine;

//#include "public.sdk/source/vst/utility/stringconvert.cpp"
void OnSampleRateChange( ASIOSampleRate SampleRate );
ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
long OnAsioMessage( long Selector, long Value, void* Message, double* opt );

ReactionSoundEngine::ReactionSoundEngine()
{
}

bool ReactionSoundEngine::LoadSoundEngineConfiguration()
{
	return true;
}

bool ReactionSoundEngine::UnloadSoundEngineConfiguration()
{
	return true;
}

bool ReactionSoundEngine::InitializeSoundEngine()
{


	return true;
}

bool ReactionSoundEngine::TerminateSoundEngine()
{
	return true;
}

tresult PLUGIN_API ReactionSoundEngine::getName( Vst::String128 name )
{
	return Vst::StringConvert::convert( "Reaction Mixing Desk", name ) ? kResultTrue : kInternalError;
}

tresult PLUGIN_API ReactionSoundEngine::createInstance( TUID cid, TUID _iid, void** obj )
{
	if ( FUnknownPrivate::iidEqual( cid, Vst::IMessage::iid ) &&
		FUnknownPrivate::iidEqual( _iid, Vst::IMessage::iid ) )
	{
		*obj = new Vst::HostMessage;
		return kResultTrue;
	}
	if ( FUnknownPrivate::iidEqual( cid, Vst::IAttributeList::iid ) &&
		FUnknownPrivate::iidEqual( _iid, Vst::IAttributeList::iid ) )
	{
		if ( auto al = Vst::HostAttributeList::make() )
		{
			*obj = al.take();
			return kResultTrue;
		}
		return kOutOfMemory;
	}
	*obj = nullptr;
	return kResultFalse;
}

tresult PLUGIN_API ReactionSoundEngine::queryInterface( const char* _iid, void** obj )
{
	QUERY_INTERFACE( _iid, obj, FUnknown::iid, IHostApplication )
		QUERY_INTERFACE( _iid, obj, IHostApplication::iid, IHostApplication )



		* obj = nullptr;
	return kResultFalse;
}

uint32 PLUGIN_API ReactionSoundEngine::addRef()
{
	return 1;
}

uint32 PLUGIN_API ReactionSoundEngine::release()
{
	return 1;
}

bool ReactionSoundEngine::InitializeASIODevice()
{
	bAsioDeviceInitialized = false;
	if ( bAsioDeviceInitialized )
	{
#if DEBUG_CONFIG
		printf( "ReactionSoundEngine::InitializeASIODevice | ASIO Device already initialized. Terminate running instance first\n" );
#endif
		return false;
	}

	if ( !mAsioDriver )
		mAsioDriver = new AsioDrivers();

	bool driverLoaded = false;
	char driverName[ 32 ];
	if ( mAsioDriver )
	{
		strcpy( driverName, ASIO_DEVICE_NAME );
#if DEBUG_CONFIG
		printf( "Loading ASIO driver %s for Reaction Mixing Desk\n", driverName );
#endif
		driverLoaded = mAsioDriver->loadDriver( driverName );
	}

	if ( !driverLoaded )
	{
#if DEBUG_CONFIG
		printf( "Failed to load ASIO driver: %s\n", driverName );
#endif
		return bAsioDeviceInitialized;
	}

	bool asioDriverInitialized = false;
	asioDriverInitialized = ( ASIOInit( &mDriverInfo ) == ASE_OK );
	if ( !asioDriverInitialized )
	{
#if DEBUG_CONFIG
		printf( "Failed to initialize ASIO driver" );
#endif
		return bAsioDeviceInitialized;
	}

#if DEBUG_CONFIG
	printf( "asioVersion:   %d\n"
		"driverVersion: %d\n"
		"Name:          %s\n"
		"ErrorMessage:  %s\n",
		mDriverInfo.asioVersion, mDriverInfo.driverVersion,
		mDriverInfo.name, mDriverInfo.errorMessage );
#endif

	if ( InitializeAsioStaticData() != 0 )
	{
		return false;
	}

	if ( CreateAsioBuffers() != ASE_OK )
	{
		return false;
	}

	if ( ASIOStart() != ASE_OK )
		return false;

	bAsioDeviceInitialized = true;
	return bAsioDeviceInitialized;
}

#pragma region ASIO_FUNCTIONS
size_t GetBytesPerSample( ASIOSampleType sampleType )
{
	switch ( sampleType )
	{
		case ASIOSTInt16LSB:
		case ASIOSTInt16MSB:
			return 2;

		case ASIOSTInt24LSB:
		case ASIOSTInt24MSB:
			return 3;

		case ASIOSTInt32LSB:
		case ASIOSTInt32MSB:
		case ASIOSTFloat32LSB:
		case ASIOSTFloat32MSB:
		case ASIOSTInt32LSB16:
		case ASIOSTInt32LSB18:
		case ASIOSTInt32LSB20:
		case ASIOSTInt32LSB24:
			return 4;

		case ASIOSTFloat64LSB:
		case ASIOSTFloat64MSB:
			return 8;

		default:
			assert( false );
	}
}

void OnSampleRateChange( ASIOSampleRate SampleRate )
{

}

void OnBufferSwitch( long doubleBufferIndex, ASIOBool directProcess )
{
	ReactionSoundEngine* soundEngine = ReactionSoundEngine::Get();
	if ( !soundEngine )
		return;

	if ( AK::SoundEngine::IsInitialized() )
		AK::SoundEngine::RenderAudio();

	long channelCount = ASIO_DEVICE_DEFAULT_CHANNELS;
	for ( int indexChannel = 0; indexChannel < channelCount; indexChannel++ )
	{
		// bufferInfos are 0 to inputChannels + (inputChannels + 1) to (inputChannels + outputChannels)
		// buffers selected are one of the two that the ASIO device is not reading at the time
		void* inputBuffer = soundEngine->mBufferInfos[ channelCount ].buffers[ doubleBufferIndex ];
		void* outputBuffer = soundEngine->mBufferInfos[ soundEngine->mInputChannels + indexChannel ].buffers[ doubleBufferIndex ];

		size_t bytesPerSample = GetBytesPerSample( soundEngine->mChannelInfos[ indexChannel ].type );
		size_t bytesPerBuffer = bytesPerSample * soundEngine->mAsioBufferPreferredSize;

		memcpy( inputBuffer, outputBuffer, bytesPerBuffer );
	}
}

// Asio device buffer switch. Happens every time the buffers are swapped out
ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess )
{
	ReactionSoundEngine* soundEngine = ReactionSoundEngine::Get();
	if ( !soundEngine )
		return Parameters;

	/*if ( AK::SoundEngine::IsInitialized() )
		AK::SoundEngine::RenderAudio();*/

	long channelCount = ASIO_DEVICE_DEFAULT_CHANNELS;
	for ( int indexChannel = 0; indexChannel < channelCount; indexChannel++ )
	{
		// bufferInfos are 0 to inputChannels + (inputChannels + 1) to (inputChannels + outputChannels)
		// buffers selected are one of the two that the ASIO device is not reading at the time
		void* inputBuffer = soundEngine->mBufferInfos[ indexChannel ].buffers[ DoubleBufferIndex ];
		void* outputBuffer = soundEngine->mBufferInfos[ soundEngine->mInputChannels + indexChannel ].buffers[ DoubleBufferIndex ];

		size_t bytesPerSample = GetBytesPerSample( soundEngine->mChannelInfos[ indexChannel ].type );
		size_t bytesPerBuffer = bytesPerSample * soundEngine->mAsioBufferPreferredSize;
#if defined RUN_SIN_TEST
		if ( indexChannel == 0 || indexChannel == 1 )
		{
			static double phase = 0.0;
			const double phaseIncrement = 2.0 * M_PI * 440.0 / soundEngine->mSampleRate; // 440 Hz tone

			float* buffer = reinterpret_cast<float*>( inputBuffer );
			for ( int i = 0; i < 512; i++ )
			{
				buffer[ i ] = 0.5f * static_cast<float>( std::sin( phase ) );
				phase += phaseIncrement;
				if ( phase >= 2.0 * M_PI )
					phase -= 2.0 * M_PI;
			}
		}
#endif
		// Passthrough signal
		memcpy( outputBuffer, inputBuffer, bytesPerBuffer );
	}

	return Parameters;
}

long OnAsioMessage( long Selector, long Value, void* Message, double* opt )
{
	switch ( Selector )
	{
		case kAsioSelectorSupported:
			if ( Value == kAsioSupportsTimeInfo )
				return 1;   // yes, we support it
			return 0;

		case kAsioSupportsTimeInfo:
			return 1;       // <-- THIS is the actual switch to time-info mode

	}
	return 0;
}
#pragma endregion 

long ReactionSoundEngine::InitializeAsioStaticData( /*AudioDeviceDriverInfo* DriverInfo*/ )
{
	// collect the informational data of the driver
		// get the number of available channels
	if ( ASIOGetChannels( &mInputChannels, &mOutputChannels ) == ASE_OK )
	{
		printf( "ASIOGetChannels (inputs: %d, outputs: %d);\n", mInputChannels, mOutputChannels );

		// get the usable buffer sizes
		if ( ASIOGetBufferSize( &mAsioBufferMinSize, &mAsioBufferMaxSize, &mAsioBufferPreferredSize, &mAsioBufferGranularity ) == ASE_OK )
		{
			printf( "ASIOGetBufferSize (min: %d, max: %d, preferred: %d, mAsioBufferGranularity: %d);\n",
				mAsioBufferMinSize, mAsioBufferMaxSize,
				mAsioBufferPreferredSize, mAsioBufferGranularity );

			// get the currently selected sample rate
			if ( ASIOGetSampleRate( &mSampleRate ) == ASE_OK )
			{
				printf( "ASIOGetSampleRate (mSampleRate: %f);\n", mSampleRate );
				if ( mSampleRate <= 0.0 || mSampleRate > 96000.0 )
				{

					if ( ASIOCanSampleRate( DEFAULT_SAMPLERATE ) )
						return -7;

					// Driver does not store it's internal sample rate, so set it to a know one.
					// Usually you should check beforehand, that the selected sample rate is valid
					// with ASIOCanSampleRate().
					if ( ASIOSetSampleRate( DEFAULT_SAMPLERATE ) == ASE_OK )
					{
						if ( ASIOGetSampleRate( &mSampleRate ) == ASE_OK )
							printf( "ASIOGetSampleRate (mSampleRate: %f);\n", mSampleRate );
						else
							return -6;
					}
					else
						return -5;
				}

				// check wether the driver requires the ASIOOutputReady() optimization
				// (can be used by the driver to reduce output latency by one block)
				if ( ASIOOutputReady() == ASE_OK )
					mOutputReady = true;
				else
					mOutputReady = false;
#if DEBUG_CONFIG
				printf( "ASIOOutputReady(); - %s\n", mOutputReady ? "Supported" : "Not supported" );
#endif
				return 0;
			}
			return -3;
		}
		return -2;
	}
	return -1;
}

ASIOError ReactionSoundEngine::CreateAsioBuffers()
{
	// create buffers for all inputs and outputs of the card with the 
	// mAsioBufferPreferredSize from ASIOGetBufferSize() as buffer size
	long i;
	ASIOError result;

	// fill the mBufferInfos from the start without a gap

	// prepare inputs (Though this is not necessaily required, no opened inputs will work, too
	if ( mInputChannels > MAX_INPUT_CHANNELS )
		mInputBuffers = MAX_INPUT_CHANNELS;
	else
		mInputBuffers = mInputChannels;

	for ( i = 0; i < mInputBuffers; i++ )
	{
		mBufferInfos[ i ].isInput = ASIOTrue;
		mBufferInfos[ i ].channelNum = i;
		mBufferInfos[ i ].buffers[ 0 ] = mBufferInfos[ i ].buffers[ 1 ] = 0;
	}

	// prepare outputs
	if ( mOutputChannels > MAX_OUTPUT_CHANNELS )
		mOutputBuffers = MAX_OUTPUT_CHANNELS;
	else
		mOutputBuffers = mOutputChannels;
	for ( i = mInputBuffers; i < mInputBuffers + mOutputBuffers; i++ )
	{
		mBufferInfos[ i ].isInput = ASIOFalse;
		mBufferInfos[ i ].channelNum = i - mInputBuffers;
		mBufferInfos[ i ].buffers[ 0 ] = mBufferInfos[ i ].buffers[ 1 ] = 0;
	}

	mAsioCallbacks.bufferSwitchTimeInfo = &OnBufferSwitchTimeInfo;
	mAsioCallbacks.asioMessage = &OnAsioMessage;
	mAsioCallbacks.sampleRateDidChange = &OnSampleRateChange;

	// create and activate buffers
	result = ASIOCreateBuffers( mBufferInfos,
		mInputBuffers + mOutputBuffers,
		mAsioBufferPreferredSize, &mAsioCallbacks );
	if ( result == ASE_OK )
	{
		// now get all the buffer details, sample word length, name, word clock group and activation
		for ( i = 0; i < mInputBuffers + mOutputBuffers; i++ )
		{
			mChannelInfos[ i ].channel = mBufferInfos[ i ].channelNum;
			mChannelInfos[ i ].isInput = mBufferInfos[ i ].isInput;
			result = ASIOGetChannelInfo( &mChannelInfos[ i ] );
			if ( result != ASE_OK )
				break;
		}

		if ( result == ASE_OK )
		{
			// get the input and output latencies
			// Latencies often are only valid after ASIOCreateBuffers()
			// (input latency is the age of the first sample in the currently returned audio block)
			// (output latency is the time the first sample in the currently returned audio block requires to get to the output)
			result = ASIOGetLatencies( &mInputLatency, &mOutputLatency );
			if ( result == ASE_OK )
			{
#if DEBUG_CONFIG
				printf( "ASIOGetLatencies (input: %d, output: %d);\n", mInputLatency, mOutputLatency );

#endif
			}
		}
	}
	return result;
}


bool ReactionSoundEngine::SetupSignalChain()
{
	return true;
}

void ReactionSoundEngine::SoundEngineLoopFunc( float deltaTime )
{
}