#include "Audio/Channel.h"
#include "Audio/SoundEngine.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "pluginterfaces/gui/iplugview.h"  
#include "pluginterfaces/vst/ivstchannelcontextinfo.h"
#include "public.sdk/source/vst/vstpresetfile.h"
#include "base/source/fstring.h"
#include "pluginterfaces/vst/ivstattributes.h"
#include "pluginterfaces/vst/vstspeaker.h"
#include "pluginterfaces/vst/ivstmessage.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


void AudioBus::SetModules( VST3::Hosting::Module::Ptr inChannelStripModule, VST3::Hosting::Module::Ptr inDynamicsModule, VST3::Hosting::Module::Ptr inMeterModule
)
{
#if DEBUG_CONFIG
	printf( "Setting up modules for Channel %s\n", Name.c_str() );
#endif

	if ( !inChannelStripModule )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::SetModules | ChannelStrip module is nullptr\n" );
		return;
#endif
	}
	if ( !inDynamicsModule )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::SetModules | Dynamics module is nullptr\n" );
		return;
#endif
	}
	if ( !inMeterModule )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::SetModules | Meter module is nullptr\n" );
		return;
#endif
	}

	channelStripModule = inChannelStripModule;
	dynamicsModule = inDynamicsModule;
	meterModule = inMeterModule;
}


bool AudioBus::InitializeBus( ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig, bool bUseBusCompressor, bool bUseMeter )
{
#if DEBUG_CONFIG
	//printf( "Initializing Bus %s\n", Name.c_str() );
#endif
	auto channelStripFactory = channelStripModule->getFactory();
	auto dynamicsFactory = dynamicsModule->getFactory();
	auto meterFactory = meterModule->getFactory();
	channelStripFactory.setHostContext( inSoundEngine );
	dynamicsFactory.setHostContext( inSoundEngine );
	meterFactory.setHostContext( inSoundEngine );

	// Initialize plugins for each channel
	int numChannelsPerBus = 0;
	SIZE_T totalRingBufferSize = GetRingBufferSize( inSoundEngine->GetAsioBufferSize() );
	switch ( inChannelConfig )
	{
		case EChannelConfiguration::MONO:
			numChannelsPerBus = (int)EChannelConfiguration::MONO;
			// Channel 1 memory
			memoryPage_ch1 = new AudioMemoryPage();
			memoryPage_ch1->header = new AudioSampleBufferHeader();
			memoryPage_ch1->mappingName = memoryPage_ch1->mappingName + to_string( ASIOChannelIndexBase ).c_str();
			memoryPage_ch1->memoryFile = CreateFileMappingA( INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, static_cast<DWORD>( totalRingBufferSize >> 32 ), static_cast<DWORD>( totalRingBufferSize & 0xFFFFFFFF ), memoryPage_ch1->mappingName.c_str() );

			memoryPage_ch1->header->channelIndex = ASIOChannelIndexBase;
			memoryPage_ch1->header->frameCapacity = inSoundEngine->GetAsioBufferSize();
			memoryPage_ch1->header->sampleRate = inSoundEngine->GetAsioSampleRate();
			memoryPage_ch1->header->writePointer.store( 0, memory_order_relaxed );
			memoryPage_ch1->header->readPointer.store( 0, memory_order_relaxed );
			break;
		case EChannelConfiguration::STEREO:
			numChannelsPerBus = (int)EChannelConfiguration::STEREO;
			// Channel 1 memory
			memoryPage_ch1 = new AudioMemoryPage();
			memoryPage_ch1->header = new AudioSampleBufferHeader();
			memoryPage_ch1->mappingName = memoryPage_ch1->mappingName + to_string( ASIOChannelIndexBase ).c_str();
			memoryPage_ch1->memoryFile = CreateFileMappingA( INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, static_cast<DWORD>( totalRingBufferSize >> 32 ), static_cast<DWORD>( totalRingBufferSize & 0xFFFFFFFF ), memoryPage_ch1->mappingName.c_str() );

			memoryPage_ch1->header->channelIndex = ASIOChannelIndexBase;
			memoryPage_ch1->header->frameCapacity = inSoundEngine->GetAsioBufferSize();
			memoryPage_ch1->header->sampleRate = inSoundEngine->GetAsioSampleRate();
			memoryPage_ch1->header->writePointer.store( 0, memory_order_relaxed );
			memoryPage_ch1->header->readPointer.store( 0, memory_order_relaxed );

			// Channel 2 memory
			memoryPage_ch2 = new AudioMemoryPage();
			memoryPage_ch2->mappingName = memoryPage_ch2->mappingName + to_string( ASIOChannelIndexBase + 1 ).c_str();
			memoryPage_ch2->memoryFile = CreateFileMappingA( INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, static_cast<DWORD>( totalRingBufferSize >> 32 ), static_cast<DWORD>( totalRingBufferSize & 0xFFFFFFFF ), memoryPage_ch2->mappingName.c_str() );
			break;
		default:
			numChannelsPerBus = (int)EChannelConfiguration::MONO;
			// Channel 1 memory
			memoryPage_ch1 = new AudioMemoryPage();
			memoryPage_ch1->mappingName = memoryPage_ch1->mappingName + to_string( ASIOChannelIndexBase ).c_str();
			memoryPage_ch1->memoryFile = CreateFileMappingA( INVALID_HANDLE_VALUE, nullptr, PAGE_READONLY, static_cast<DWORD>( totalRingBufferSize >> 32 ), static_cast<DWORD>( totalRingBufferSize & 0xFFFFFFFF ), memoryPage_ch1->mappingName.c_str() );

			memoryPage_ch1->header->channelIndex = ASIOChannelIndexBase;
			memoryPage_ch1->header->frameCapacity = inSoundEngine->GetAsioBufferSize();
			memoryPage_ch1->header->sampleRate = inSoundEngine->GetAsioSampleRate();
			memoryPage_ch1->header->writePointer.store( 0, memory_order_relaxed );
			memoryPage_ch1->header->readPointer.store( 0, memory_order_relaxed );
			break;
	}

	if ( !StartChannelStrip( channelStripFactory, inSoundEngine, inChannelConfig ) )
	{
#if DEBUG_CONFIG
		assert( "Failed to initialize Channel Strip for Bus %s at Channel number %i", newChannel->Name.c_str(), indexTotalChannels );
#endif
		return false;
	}

	if ( bUseMeter )
	{
		if ( !StartMeter( meterFactory, inSoundEngine, inChannelConfig ) )
		{
#if DEBUG_CONFIG
			assert( "Failed to initialize Meter for Bus %s at Channel number %i", newChannel->Name.c_str(), indexTotalChannels );
#endif
			return false;
		}
	}

	return true;
}

bool AudioBus::ProcessAudioForChannel()
{
	return true;
}

bool AudioBus::StartChannelStrip( PluginFactory inChannelStripFactory, ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig )
{
#if DEBUG_CONFIG
	//printf( "Starting Channel Strip for Channel %s\n", Name.c_str() );
#endif
	for ( int indexClassInfos = 0; indexClassInfos < inChannelStripFactory.classCount(); indexClassInfos++ )
	{
		if ( inChannelStripFactory.classInfos()[ indexClassInfos ].category() == kVstAudioEffectClass )
		{
			ChannelStripEffect.classInfo = inChannelStripFactory.classInfos()[ indexClassInfos ];
			break;
		}
	}
	ChannelStripEffect.pluginProvider = new Vst::PlugProvider( inChannelStripFactory, ChannelStripEffect.classInfo, true );
	if ( !ChannelStripEffect.pluginProvider )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartChannelStrip | Meter module is nullptr\n" );
		return false;
#endif
	}
	else
	{
		if ( !ChannelStripEffect.pluginProvider->initialize() )
		{
#if DEBUG_CONFIG
			assert( "AudioBus::StartChannelStrip | plugin provider failed to initialize\n" );
			return false;
#endif
		}
	}

	ChannelStripEffect.componentPtr = ChannelStripEffect.pluginProvider->getComponent();
	if ( !ChannelStripEffect.componentPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartChannelStrip | Channel Strip component is nullptr\n" );
		return false;
#endif
	}

	ChannelStripEffect.editControllerPtr = ChannelStripEffect.pluginProvider->getController();
	if ( !ChannelStripEffect.editControllerPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartChannelStrip | Channel Strip edit controller is nullptr\n" );
		return false;
#endif
	}

	ChannelStripEffect.componentPtr->queryInterface( Vst::IAudioProcessor::iid, (void**)&ChannelStripEffect.audioProcessorPtr );
	if ( !ChannelStripEffect.audioProcessorPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartChannelStrip | Channel Strip audio processor is nullptr\n" );
		return false;
#endif
	}

	Vst::SpeakerArrangement arrangement = Vst::SpeakerArr::kMono;
	int numInputs = 1;
	int numOutputs = 1;
	switch ( inChannelConfig )
	{
		case EChannelConfiguration::MONO:
			arrangement = Vst::SpeakerArr::kMono;
			break;
		case EChannelConfiguration::STEREO:
			arrangement = Vst::SpeakerArr::kStereo;
			numInputs = 2;
			numOutputs = 2;
			break;
	}

	ChannelStripEffect.audioProcessorPtr->setBusArrangements( &arrangement, numInputs, &arrangement, numOutputs );

	ChannelStripEffect.editControllerPtr->setComponentHandler( this );
	Vst::IConnectionPoint* compCP = nullptr;
	Vst::IConnectionPoint* ctrlCP = nullptr;
	ChannelStripEffect.componentPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&compCP );
	ChannelStripEffect.editControllerPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&ctrlCP );
	if ( compCP && ctrlCP )
	{
		compCP->connect( ctrlCP );
		ctrlCP->connect( compCP );
	}
	if ( compCP ) compCP->release();
	if ( ctrlCP ) ctrlCP->release();

	Vst::ChannelContext::IInfoListener* infoListener = nullptr;
	if ( ChannelStripEffect.editControllerPtr->queryInterface(
		Vst::ChannelContext::IInfoListener::iid, (void**)&infoListener ) == kResultOk
		&& infoListener )
	{
		// 2. Build the attribute list
		IPtr<Vst::IAttributeList> attrList = Vst::HostAttributeList::make();
		if ( attrList )
		{
			// Channel name — STR16 produces a const TChar* (UTF-16)
			Vst::TChar channelName[ 128 ];
			for ( int i = 0; i < 128; i++ ) { channelName[ i ] = static_cast<Vst::TChar>( Name[ i ] ); if ( !Name[ i ] ) break; }

			// Compute character count manually (no wcslen dependency)
			int64 nameLen = 0;
			while ( channelName[ nameLen ] != Vst::TChar( 0 ) ) ++nameLen;

			attrList->setString( Vst::ChannelContext::kChannelNameKey, channelName );
			attrList->setInt( Vst::ChannelContext::kChannelNameLengthKey, nameLen );

			// Channel index (1-based) and color (ARGB)
			attrList->setInt( Vst::ChannelContext::kChannelIndexKey, (int64)Number + 1 );
			attrList->setInt( Vst::ChannelContext::kChannelColorKey, ChannelStripColors::Default_Green );

			// Push to the plugin
			infoListener->setChannelContextInfos( attrList );
		}
		infoListener->release();
	}


	IPlugView* view = nullptr;
	if ( ChannelStripEffect.editControllerPtr )
	{
		view = ChannelStripEffect.editControllerPtr->createView( Vst::ViewType::kEditor );
		if ( view )
		{
#if DEBUG_CONFIG
			//printf( "Creating Channel Strip window for Channel %s\n", Name.c_str() );
#endif
			// Create a native Win32 window to host it
			HWND hwnd = CreateWindowExW(
				0, L"STATIC", L"VST3 Host",
				WS_DISABLED,
				CW_USEDEFAULT, CW_USEDEFAULT,
				800, 600,
				nullptr, nullptr, GetModuleHandle( nullptr ), nullptr
			);

			if ( view->isPlatformTypeSupported( kPlatformTypeHWND ) == kResultOk )
			{
				view->attached( hwnd, kPlatformTypeHWND );
			}
			//ShowWindow( hwnd, SW_SHOW );
			//view->release();
		}
	}

	return true;
}


bool AudioBus::StartMeter( PluginFactory inMeterFactory, ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig )
{
#if DEBUG_CONFIG
	printf( "Starting Meter for Channel %s\n", Name.c_str() );
#endif

	for ( int indexClassInfos = 0; indexClassInfos < inMeterFactory.classCount(); indexClassInfos++ )
	{
		if ( inMeterFactory.classInfos()[ indexClassInfos ].category() == kVstAudioEffectClass )
		{
			MeterEffect.classInfo = inMeterFactory.classInfos()[ indexClassInfos ];
			break;
		}
	}
	MeterEffect.pluginProvider = new Vst::PlugProvider( inMeterFactory, MeterEffect.classInfo, true );
	if ( !MeterEffect.pluginProvider )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartMeter | plugin provider is nullptr\n" );
		return false;
#endif
	}
	else
	{
		if ( !MeterEffect.pluginProvider->initialize() )
		{
#if DEBUG_CONFIG
			assert( "AudioBus::StartMeter | plugin provider failed to initialize\n" );
			return false;
#endif
		}
	}

	MeterEffect.componentPtr = MeterEffect.pluginProvider->getComponent();
	if ( !MeterEffect.componentPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartMeter | Meter component is nullptr\n" );
		return false;
#endif
	}

	MeterEffect.editControllerPtr = MeterEffect.pluginProvider->getController();
	if ( !MeterEffect.editControllerPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartMeter | Meter edit controller is nullptr\n" );
		return false;
#endif
	}

	MeterEffect.componentPtr->queryInterface( Vst::IAudioProcessor::iid, (void**)&MeterEffect.audioProcessorPtr );
	if ( !MeterEffect.audioProcessorPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioBus::StartMeter | Meter audio processor is nullptr\n" );
		return false;
#endif
	}
	Vst::SpeakerArrangement arrangement = Vst::SpeakerArr::kMono;
	int numInputs = 1;
	int numOutputs = 1;
	switch ( inChannelConfig )
	{
		case EChannelConfiguration::MONO:
			arrangement = Vst::SpeakerArr::kMono;
			break;
		case EChannelConfiguration::STEREO:
			arrangement = Vst::SpeakerArr::kStereo;
			numInputs = 2;
			numOutputs = 2;
			break;
	}

	MeterEffect.audioProcessorPtr->setBusArrangements( &arrangement, numInputs, &arrangement, numOutputs );

	MeterEffect.editControllerPtr->setComponentHandler( this );
	Vst::IConnectionPoint* compCP = nullptr;
	Vst::IConnectionPoint* ctrlCP = nullptr;
	MeterEffect.componentPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&compCP );
	MeterEffect.editControllerPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&ctrlCP );
	if ( compCP && ctrlCP )
	{
		compCP->connect( ctrlCP );
		ctrlCP->connect( compCP );
	}
	if ( compCP ) compCP->release();
	if ( ctrlCP ) ctrlCP->release();

	Vst::ChannelContext::IInfoListener* infoListener = nullptr;
	if ( ChannelStripEffect.editControllerPtr->queryInterface(
		Vst::ChannelContext::IInfoListener::iid, (void**)&infoListener ) == kResultOk
		&& infoListener )
	{
		// 2. Build the attribute list
		IPtr<Vst::IAttributeList> attrList = Vst::HostAttributeList::make();
		if ( attrList )
		{
			// Channel name — STR16 produces a const TChar* (UTF-16)
			Vst::TChar channelName[ 128 ];
			for ( int i = 0; i < 128; i++ ) { channelName[ i ] = static_cast<Vst::TChar>( Name[ i ] ); if ( !Name[ i ] ) break; }

			// Compute character count manually (no wcslen dependency)
			int64 nameLen = 0;
			while ( channelName[ nameLen ] != Vst::TChar( 0 ) ) ++nameLen;

			attrList->setString( Vst::ChannelContext::kChannelNameKey, channelName );
			attrList->setInt( Vst::ChannelContext::kChannelNameLengthKey, nameLen );

			// Channel index (1-based) and color (ARGB)
			attrList->setInt( Vst::ChannelContext::kChannelIndexKey, (int64)Number + 1 );
			attrList->setInt( Vst::ChannelContext::kChannelColorKey, (int64)0xFF29A3CC );

			// Push to the plugin
			infoListener->setChannelContextInfos( attrList );
		}
		infoListener->release();
	}

	IPlugView* view = nullptr;
	if ( MeterEffect.editControllerPtr )
	{
		view = MeterEffect.editControllerPtr->createView( Vst::ViewType::kEditor );
		if ( view )
		{
#if DEBUG_CONFIG
			printf( "Creating Meter window for Channel %s\n", Name.c_str() );
#endif
			// Create a native Win32 window to host it
			HWND hwnd = CreateWindowExW(
				0, L"STATIC", L"VST3 Host",
				WS_DISABLED,
				CW_USEDEFAULT, CW_USEDEFAULT,
				800, 600,
				nullptr, nullptr, GetModuleHandle( nullptr ), nullptr
			);

			if ( view->isPlatformTypeSupported( kPlatformTypeHWND ) == kResultOk )
			{
				view->attached( hwnd, kPlatformTypeHWND );
			}
			InvalidateRect( hwnd, nullptr, TRUE );
			UpdateWindow( hwnd );
			view->release();
			//DestroyWindow( hwnd );
		}
	}

	return true;
}