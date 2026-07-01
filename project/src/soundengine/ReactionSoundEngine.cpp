#include "ReactionSoundEngine.h"

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
	return 0;
}
tresult PLUGIN_API ReactionSoundEngine::createInstance( TUID cid, TUID _iid, void** obj )
{
	return 0;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API ReactionSoundEngine::queryInterface( const char* _iid, void** obj )
{

	return kResultFalse;
}

//-----------------------------------------------------------------------------
uint32 PLUGIN_API ReactionSoundEngine::addRef()
{
	return 1;
}

//-----------------------------------------------------------------------------
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
		printf( "ReactionSoundEngine::InitializeASIODevice | ASIO Device already initialized. Terminate running instance first" );
#endif
		return false;
	}

	if ( !mAsioDriver )
		mAsioDriver = new AsioDrivers();

	bool driverLoaded = false;
	if ( mAsioDriver )
	{
#if DEBUG_CONFIG
		printf( "Loading ASIO driver for Reaction Mixing Desk" );
#endif
		char driverName[ 32 ];
		strcpy( driverName, ASIO_DEVICE_NAME );
		driverLoaded = mAsioDriver->loadDriver( driverName );
	}

	if ( !driverLoaded )
	{
#if DEBUG_CONFIG
		printf( "Failed to load ASIO driver" );
#endif
		return bAsioDeviceInitialized;
	}

	bool asioDriverInitialized = false;
	asioDriverInitialized = ( ASIOInit( &mDriverInfo ) != ASE_OK );
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
	}

	if ( CreateAsioBuffers() != ASE_OK )
	{
	}

	bAsioDeviceInitialized = driverLoaded;
	return bAsioDeviceInitialized;
}

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
	ASIOBufferInfo* info = mBufferInfos;

	// prepare inputs (Though this is not necessaily required, no opened inputs will work, too
	if ( mInputChannels > MAX_INPUT_CHANNELS )
		mInputBuffers = MAX_INPUT_CHANNELS;
	else
		mInputBuffers = mInputChannels;

	for ( i = 0; i < mInputBuffers; i++, info++ )
	{
		info->isInput = ASIOTrue;
		info->channelNum = i;
		info->buffers[ 0 ] = info->buffers[ 1 ] = 0;
	}

	// prepare outputs
	if ( mOutputChannels > MAX_OUTPUT_CHANNELS )
		mOutputBuffers = MAX_OUTPUT_CHANNELS;
	else
		mOutputBuffers = mOutputChannels;
	for ( i = 0; i < mOutputBuffers; i++, info++ )
	{
		info->isInput = ASIOFalse;
		info->channelNum = i;
		info->buffers[ 0 ] = info->buffers[ 1 ] = 0;
	}

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