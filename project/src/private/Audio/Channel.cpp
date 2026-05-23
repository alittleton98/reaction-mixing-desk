#include "Audio/Channel.h"
#include "Audio/SoundEngine.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "pluginterfaces/gui/iplugview.h"  
#include "pluginterfaces/vst/ivstchannelcontextinfo.h"
#include "public.sdk/source/vst/vstpresetfile.h"
#include "base/source/fstring.h"
#include "pluginterfaces/vst/ivstattributes.h"
#include "pluginterfaces/vst/vstspeaker.h"

void AudioChannel::SetModules( VST3::Hosting::Module::Ptr inChannelStripModule, VST3::Hosting::Module::Ptr inDynamicsModule, VST3::Hosting::Module::Ptr inMeterModule
)
{
#if DEBUG_CONFIG
	printf( "Setting up modules for Channel %s\n", ChannelName );
#endif

	if ( !inChannelStripModule )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::SetModules | ChannelStrip module is nullptr\n" );
		return;
#endif
	}
	if ( !inDynamicsModule )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::SetModules | Dynamics module is nullptr\n" );
		return;
#endif
	}
	if ( !inMeterModule )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::SetModules | Meter module is nullptr\n" );
		return;
#endif
	}

	channelStripModule = inChannelStripModule;
	dynamicsModule = inDynamicsModule;
	meterModule = inMeterModule;
}


bool AudioChannel::InitializeChannel( SoundEngine* inSoundEngine )
{
#if DEBUG_CONFIG
	printf( "Initializing Channel %s\n", ChannelName );
#endif
	auto channelStripFactory = channelStripModule->getFactory();
	auto dynamicsFactory = dynamicsModule->getFactory();
	auto meterFactory = meterModule->getFactory();
	channelStripFactory.setHostContext( inSoundEngine );
	dynamicsFactory.setHostContext( inSoundEngine );
	meterFactory.setHostContext( inSoundEngine );

	if ( !StartChannelStrip( channelStripFactory, inSoundEngine ) )
	{
#if DEBUG_CONFIG
		assert( "Failed to initialize Channel Strip %s", newChannel->ChannelName );
#endif
		return false;
	}


	if ( !StartDynamics( dynamicsFactory, inSoundEngine ) )
	{
#if DEBUG_CONFIG
		assert( "Failed to initialize Dynamics %s", newChannel->ChannelName );
#endif
		return false;
	}

	if ( !StartMeter( meterFactory, inSoundEngine ) )
	{
#if DEBUG_CONFIG
		assert( "Failed to initialize Meter %s", newChannel->ChannelName );
#endif
		return false;
	}

	return true;
}

bool AudioChannel::ProcessAudioForChannel()
{
	return true;
}

bool AudioChannel::StartChannelStrip( PluginFactory inChannelStripFactory, SoundEngine* inSoundEngine )
{
#if DEBUG_CONFIG
	printf( "Starting Channel Strip for Channel %s\n", ChannelName.c_str() );
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
		assert( "AudioChannel::StartChannelStrip | Meter module is nullptr\n" );
		return false;
#endif
	}
	else
	{
		if ( !ChannelStripEffect.pluginProvider->initialize() )
		{
#if DEBUG_CONFIG
			assert( "AudioChannel::StartChannelStrip | plugin provider failed to initialize\n" );
			return false;
#endif
		}
	}

	ChannelStripEffect.componentPtr = ChannelStripEffect.pluginProvider->getComponent();
	if ( !ChannelStripEffect.componentPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartChannelStrip | Channel Strip component is nullptr\n" );
		return false;
#endif
	}

	ChannelStripEffect.editControllerPtr = ChannelStripEffect.pluginProvider->getController();
	if ( !ChannelStripEffect.editControllerPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartChannelStrip | Channel Strip edit controller is nullptr\n" );
		return false;
#endif
	}

	ChannelStripEffect.componentPtr->queryInterface( Vst::IAudioProcessor::iid, (void**)&ChannelStripEffect.audioProcessorPtr );
	if ( !ChannelStripEffect.audioProcessorPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartChannelStrip | Channel Strip audio processor is nullptr\n" );
		return false;
#endif
	}

	Vst::SpeakerArrangement inputArrangement = Vst::SpeakerArr::k51;
	Vst::SpeakerArrangement outputArrangement = Vst::SpeakerArr::k51;
	ChannelStripEffect.audioProcessorPtr->setBusArrangements( &inputArrangement, 1, &outputArrangement, 1 );

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

	for ( int indexParameter = 0; indexParameter < ChannelStripEffect.editControllerPtr->getParameterCount(); indexParameter++ )
	{
		Vst::ParameterInfo parameter;
		if ( ChannelStripEffect.editControllerPtr->getParameterInfo( indexParameter, parameter ) == kResultTrue )
		{
			std::wstring wstr_title( parameter.title, parameter.title + 128 );
			wstr_title = wstr_title.substr( 0, wstr_title.find( L'\0' ) );

			std::wcout << "Param ID: " << parameter.id
				<< " |Name: " << wstr_title
				<< " |Default: " << parameter.defaultNormalizedValue
				<< std::endl;
		}

	}

	//Vst::ChannelContext::IInfoListener* infoListener = nullptr;
	//ChannelStripEffect.componentPtr->queryInterface(
	//	Vst::ChannelContext::IInfoListener::iid,
	//	(void**)&infoListener
	//);

	//if ( !infoListener )
	//{
	//	printf( "[VST3] IInfoListener not supported\n" );
	//	return false;
	//}

	//// Communicate attribute list

	//infoListener->release();

	IPlugView* view = nullptr;
	if ( ChannelStripEffect.editControllerPtr )
	{
		view = ChannelStripEffect.editControllerPtr->createView( Vst::ViewType::kEditor );
		if ( view )
		{
#if DEBUG_CONFIG
			printf( "Creating Channel Strip window for Channel %s\n", ChannelName.c_str() );
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

			view->release();
			//DestroyWindow( hwnd );
		}
	}

	return true;
}

bool AudioChannel::StartDynamics( PluginFactory inDynamicsFactory, SoundEngine* inSoundEngine )
{
#if DEBUG_CONFIG
	printf( "Starting Dynamics for Channel %s\n", ChannelName.c_str() );
#endif

	for ( int indexClassInfos = 0; indexClassInfos < inDynamicsFactory.classCount(); indexClassInfos++ )
	{
		if ( inDynamicsFactory.classInfos()[ indexClassInfos ].category() == kVstAudioEffectClass )
		{
			DynamicsEffect.classInfo = inDynamicsFactory.classInfos()[ indexClassInfos ];
			break;
		}
	}
	DynamicsEffect.pluginProvider = new Vst::PlugProvider( inDynamicsFactory, DynamicsEffect.classInfo, true );
	if ( !DynamicsEffect.pluginProvider )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartDynamics | plugin provider is nullptr\n" );
		return false;
#endif
	}
	else
	{
		if ( !DynamicsEffect.pluginProvider->initialize() )
		{
#if DEBUG_CONFIG
			assert( "AudioChannel::StartDynamics | plugin provider failed to initialize\n" );
			return false;
#endif
		}
	}

	DynamicsEffect.componentPtr = DynamicsEffect.pluginProvider->getComponent();
	if ( !DynamicsEffect.componentPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartDynamics | Dynamics component is nullptr\n" );
		return false;
#endif
	}

	DynamicsEffect.editControllerPtr = DynamicsEffect.pluginProvider->getController();
	if ( !DynamicsEffect.editControllerPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartDynamics | Dynamics edit controller is nullptr\n" );
		return false;
#endif
	}

	DynamicsEffect.componentPtr->queryInterface( Vst::IAudioProcessor::iid, (void**)&DynamicsEffect.audioProcessorPtr );
	if ( !DynamicsEffect.audioProcessorPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartDynamics | Dynamics audio processor is nullptr\n" );
		return false;
#endif
	}

	DynamicsEffect.editControllerPtr->setComponentHandler( this );
	Vst::IConnectionPoint* compCP = nullptr;
	Vst::IConnectionPoint* ctrlCP = nullptr;
	DynamicsEffect.componentPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&compCP );
	DynamicsEffect.editControllerPtr->queryInterface( Vst::IConnectionPoint::iid, (void**)&ctrlCP );
	if ( compCP && ctrlCP )
	{
		compCP->connect( ctrlCP );
		ctrlCP->connect( compCP );
	}
	if ( compCP ) compCP->release();
	if ( ctrlCP ) ctrlCP->release();

	for ( int indexParameter = 0; indexParameter < DynamicsEffect.editControllerPtr->getParameterCount(); indexParameter++ )
	{
		Vst::ParameterInfo parameter;
		if ( DynamicsEffect.editControllerPtr->getParameterInfo( indexParameter, parameter ) == kResultTrue )
		{
			std::wstring wstr_title( parameter.title, parameter.title + 128 );
			wstr_title = wstr_title.substr( 0, wstr_title.find( L'\0' ) );

			std::wcout << "Param ID: " << parameter.id
				<< " |Name: " << wstr_title
				<< " |Default: " << parameter.defaultNormalizedValue
				<< std::endl;
		}

	}

	//Vst::ChannelContext::IInfoListener* infoListener = nullptr;
	//DynamicsEffect.componentPtr->queryInterface(
	//	Vst::ChannelContext::IInfoListener::iid,
	//	(void**)&infoListener
	//);

	//if ( !infoListener )
	//{
	//	printf( "[VST3] IInfoListener not supported\n" );
	//	return false;
	//}

	//// Communicate attribute list

	//infoListener->release();

	IPlugView* view = nullptr;
	if ( DynamicsEffect.editControllerPtr )
	{
		view = DynamicsEffect.editControllerPtr->createView( Vst::ViewType::kEditor );
		if ( view )
		{
#if DEBUG_CONFIG
			printf( "Creating Dynamics window for Channel %s\n", ChannelName.c_str() );
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

			view->release();
			//DestroyWindow( hwnd );
		}
	}

	return true;
}

bool AudioChannel::StartMeter( PluginFactory inMeterFactory, SoundEngine* inSoundEngine )
{
#if DEBUG_CONFIG
	printf( "Starting Meter for Channel %s\n", ChannelName.c_str() );
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
		assert( "AudioChannel::StartMeter | plugin provider is nullptr\n" );
		return false;
#endif
	}
	else
	{
		if ( !MeterEffect.pluginProvider->initialize() )
		{
#if DEBUG_CONFIG
			assert( "AudioChannel::StartMeter | plugin provider failed to initialize\n" );
			return false;
#endif
		}
	}

	MeterEffect.componentPtr = MeterEffect.pluginProvider->getComponent();
	if ( !MeterEffect.componentPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartMeter | Meter component is nullptr\n" );
		return false;
#endif
	}

	MeterEffect.editControllerPtr = MeterEffect.pluginProvider->getController();
	if ( !MeterEffect.editControllerPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartMeter | Meter edit controller is nullptr\n" );
		return false;
#endif
	}

	MeterEffect.componentPtr->queryInterface( Vst::IAudioProcessor::iid, (void**)&MeterEffect.audioProcessorPtr );
	if ( !MeterEffect.audioProcessorPtr )
	{
#if DEBUG_CONFIG
		assert( "AudioChannel::StartMeter | Meter audio processor is nullptr\n" );
		return false;
#endif
	}

	Vst::SpeakerArrangement arrangement = Vst::SpeakerArr::k51;
	MeterEffect.audioProcessorPtr->setBusArrangements( &arrangement, 6, &arrangement, 6 );

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

	for ( int indexParameter = 0; indexParameter < MeterEffect.editControllerPtr->getParameterCount(); indexParameter++ )
	{
		Vst::ParameterInfo parameter;
		if ( MeterEffect.editControllerPtr->getParameterInfo( indexParameter, parameter ) == kResultTrue )
		{
			std::wstring wstr_title( parameter.title, parameter.title + 128 );
			wstr_title = wstr_title.substr( 0, wstr_title.find( L'\0' ) );

			std::wcout << "Param ID: " << parameter.id
				<< " |Name: " << wstr_title
				<< " |Default: " << parameter.defaultNormalizedValue
				<< std::endl;
		}

	}

	//Vst::ChannelContext::IInfoListener* infoListener = nullptr;
	//MeterEffect.componentPtr->queryInterface(
	//	Vst::ChannelContext::IInfoListener::iid,
	//	(void**)&infoListener
	//);

	//if ( !infoListener )
	//{
	//	printf( "[VST3] IInfoListener not supported\n" );
	//	return false;
	//}

	//// Communicate attribute list

	//infoListener->release();

	IPlugView* view = nullptr;
	if ( MeterEffect.editControllerPtr )
	{
		view = MeterEffect.editControllerPtr->createView( Vst::ViewType::kEditor );
		if ( view )
		{
#if DEBUG_CONFIG
			printf( "Creating Meter window for Channel %s\n", ChannelName.c_str() );
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