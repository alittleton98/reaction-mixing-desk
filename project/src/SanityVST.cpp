/**
 * vst3_loader.cpp
 *
 * Loads a VST3 plugin from disk and initializes it for use.
 * No audio processing loop — just full initialization up to "ready to process".
 *
 * Dependencies: VST3 SDK (pluginterfaces, base, public.sdk)
 */

#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <array>
#include <windows.h>
#include "Common.h"

 // VST3 SDK headers
#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivstcomponent.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "public.sdk/source/vst/hosting/hostclasses.h"
#include "public.sdk/source/common/memorystream.h"
#include "base/source/fobject.h"
#include "pluginterfaces/vst/ivstplugview.h"
#include "pluginterfaces/gui/iplugview.h"

using namespace Steinberg;
using namespace Steinberg::Vst;
using namespace std;

static constexpr int NUM_INSTANCES = 1;
static constexpr const char* PLUGIN_PATH =
"C:\\Program Files\\Common Files\\VST3\\SSL Native Channel Strip 2.vst3";

// ---------------------------------------------------------------------------
// Minimal host application context required by the VST3 SDK.
// ---------------------------------------------------------------------------
class SimpleHostApplication : public FObject, public IHostApplication
{
public:
	tresult PLUGIN_API getName( String128 name ) override
	{
		name = u"Waffles";
		return kResultOk;
	}

	tresult PLUGIN_API createInstance( TUID, TUID, void** ) override
	{
		return kNotImplemented;
	}

	OBJ_METHODS( SimpleHostApplication, FObject )
		REFCOUNT_METHODS( FObject )
		DEF_INTERFACES_1( IHostApplication, FObject )
};

// ---------------------------------------------------------------------------
// RAII wrapper owning one plugin instance.
// The module is loaded once externally and shared across all instances.
// ---------------------------------------------------------------------------
struct Vst3Plugin
{
	IPtr<PlugProvider>    plugProvider;
	IPtr<IComponent>      component;
	IPtr<IAudioProcessor> processor;
	IPtr<IEditController> controller;

	double sampleRate = 44100.0;
	int32  maxSamplesBlock = 512;
	int32  numInputBuses = 1;
	int32  numOutputBuses = 1;

	void initialize( VST3::Hosting::Module& module,
		const VST3::Hosting::ClassInfo& classInfo,
		IHostApplication* host )
	{
		auto& factory = module.getFactory();

		// 1. PlugProvider
		plugProvider = owned( new PlugProvider( factory, classInfo, true ) );
		if ( !plugProvider )
			throw std::runtime_error( "Failed to create PlugProvider." );

		// 2. IComponent + IAudioProcessor
		component = plugProvider->getComponent();
		if ( !component )
			throw std::runtime_error( "Failed to get IComponent." );

		if ( component->initialize( host ) != kResultOk )
			throw std::runtime_error( "IComponent::initialize() failed." );

		if ( component->queryInterface( IAudioProcessor::iid,
			(void**)&processor ) != kResultOk || !processor )
			throw std::runtime_error( "Plugin does not implement IAudioProcessor." );

		// 3. IEditController
		controller = plugProvider->getController();
		if ( controller )
		{
			if ( controller->initialize( host ) != kResultOk )
				std::cerr << "[VST3] Warning: IEditController::initialize() failed.\n";

			IConnectionPoint* compCP = nullptr;
			IConnectionPoint* ctrlCP = nullptr;
			component->queryInterface( IConnectionPoint::iid, (void**)&compCP );
			controller->queryInterface( IConnectionPoint::iid, (void**)&ctrlCP );
			if ( compCP && ctrlCP )
			{
				compCP->connect( ctrlCP );
				ctrlCP->connect( compCP );
			}
			if ( compCP ) compCP->release();
			if ( ctrlCP ) ctrlCP->release();
		}

		// 4. Activate buses
		auto activateBuses = [ & ]( MediaType type, BusDirection dir, int32 count )
			{
				int32 total = component->getBusCount( type, dir );
				for ( int32 i = 0; i < total; ++i )
					component->activateBus( type, dir, i, ( i < count ) );
			};

		activateBuses( kAudio, kInput, numInputBuses );
		activateBuses( kAudio, kOutput, numOutputBuses );
		activateBuses( kEvent, kInput, 1 );

		// 5. Setup processing
		ProcessSetup setup{};
		setup.processMode = kRealtime;
		setup.symbolicSampleSize = kSample32;
		setup.maxSamplesPerBlock = maxSamplesBlock;
		setup.sampleRate = sampleRate;

		if ( processor->setupProcessing( setup ) != kResultOk )
			throw std::runtime_error( "IAudioProcessor::setupProcessing() failed." );

		// 6. Activate component
		if ( component->setActive( true ) != kResultOk )
			throw std::runtime_error( "IComponent::setActive(true) failed." );

		// 7. Attach invisible window so the plugin view initializes
		if ( controller )
		{
			IPlugView* view = controller->createView( ViewType::kEditor );
			if ( view )
			{
				HWND hwnd = CreateWindowExW(
					0, L"STATIC", L"VST3 Host",
					WS_POPUP,
					0, 0, 1, 1,
					nullptr, nullptr, GetModuleHandle( nullptr ), nullptr
				);

				if ( hwnd && view->isPlatformTypeSupported( kPlatformTypeHWND ) == kResultOk )
					view->attached( hwnd, kPlatformTypeHWND );

				view->release();
			}
		}
	}

	void teardown()
	{
		if ( component )
		{
			component->setActive( false );

			if ( controller )
			{
				IConnectionPoint* compCP = nullptr;
				IConnectionPoint* ctrlCP = nullptr;
				component->queryInterface( IConnectionPoint::iid, (void**)&compCP );
				controller->queryInterface( IConnectionPoint::iid, (void**)&ctrlCP );
				if ( compCP && ctrlCP )
				{
					compCP->disconnect( ctrlCP );
					ctrlCP->disconnect( compCP );
				}
				if ( compCP ) compCP->release();
				if ( ctrlCP ) ctrlCP->release();

				controller->terminate();
			}

			component->terminate();
		}
	}

	~Vst3Plugin() { teardown(); }
};

// ---------------------------------------------------------------------------
// Entry point
// ---------------------------------------------------------------------------
int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/ )
{
	AllocConsole();

	SimpleHostApplication host;
	PluginContextFactory::instance().setPluginContext( static_cast<IHostApplication*>( &host ) );

	// ------------------------------------------------------------------
	// Load the module once — all instances share the same DLL in memory.
	// ------------------------------------------------------------------
	std::string errorMsg;
	auto module = VST3::Hosting::Module::create( PLUGIN_PATH, errorMsg );
	if ( !module )
	{
		std::cerr << "Failed to load VST3 module: " << errorMsg << "\n";
		return 1;
	}
	std::cout << "[VST3] Module loaded.\n";

	// Find the audio effect class once.
	VST3::Hosting::ClassInfo classInfo;
	bool found = false;
	for ( auto& ci : module->getFactory().classInfos() )
	{
		if ( ci.category() == kVstAudioEffectClass )
		{
			classInfo = ci;
			found = true;
			std::cout << "[VST3] Class: " << ci.name() << "\n";
			break;
		}
	}

	if ( !found )
	{
		std::cerr << "No audio effect class found.\n";
		return 1;
	}

	// ------------------------------------------------------------------
	// Create NUM_INSTANCES independent plugin instances.
	// ------------------------------------------------------------------
	std::array<Vst3Plugin, NUM_INSTANCES> plugins;

	for ( int i = 0; i < NUM_INSTANCES; ++i )
	{
		try
		{
			plugins[ i ].initialize( *module, classInfo, static_cast<IHostApplication*>( &host ) );
			std::cout << "[VST3] Instance " << ( i + 1 ) << " / " << NUM_INSTANCES << " ready.\n";
		}
		catch ( const std::exception& ex )
		{
			std::cerr << "[VST3] Instance " << ( i + 1 ) << " failed: " << ex.what() << "\n";
			return 1;
		}
	}

	std::cout << "\nAll " << NUM_INSTANCES << " instances initialized and ready to process.\n";

	this_thread::sleep_for( chrono::seconds( 30 ) );

	// plugins array goes out of scope — all instances torn down via ~Vst3Plugin()
	return 0;
}