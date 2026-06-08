// A Mono input Signal that represents a single channel for the audio devices
#pragma once
#include <vector>
#include <atomic>
#include <windows.h>
#include "Common.h"
#include "Audio/Plugin.h"
#include "Control Surface/ControlSurface.h"

#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/base/ipluginbase.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "public.sdk/source/vst/hosting/module.h"
#include "public.sdk/source/vst/hosting/plugprovider.h"
#include "base/source/fdebug.h"
#include "AK/SoundEngine/Common/AkTypes.h"


using namespace Steinberg;
using namespace VST3::Hosting;
class ReactionSoundEngine;

struct FChannelStripEffect
{
	IPtr<Vst::IComponent> componentPtr;
	IPtr<Vst::IAudioProcessor> audioProcessorPtr;
	IPtr<Vst::IEditController> editControllerPtr;
	IPtr<Vst::PlugProvider> pluginProvider;
	ClassInfo classInfo;
};

class AudioBus : public Vst::IComponentHandler
{
public:
	string Name = "Track";
	float FaderValue = -96.6;
	int Number = -1;
	EChannelConfiguration BusConfig = EChannelConfiguration::MONO;


	void SetModules(
		Module::Ptr inChannelStripModule,
		Module::Ptr inDynamicsModule,
		Module::Ptr inMeterModule
	);

	bool InitializeBus( ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig, bool bUseBusCompressor, bool bUseMeter );
	bool ProcessAudioForChannel();
	bool StartChannelStrip( PluginFactory inChannelStripFactory, ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig );
	bool StartMeter( PluginFactory inMeterFactory, ReactionSoundEngine* inSoundEngine, EChannelConfiguration inChannelConfig );
	FChannelStripEffect ChannelStripEffect;
	FChannelStripEffect MeterEffect;


	// Vst::IComponentHandler interface
	/** To be called before calling a performEdit (e.g. on mouse-click-down event).
	 * This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult  beginEdit( Vst::ParamID id ) override
	{
		SMTG_DBPRT1( "beginEdit called (%d)\n", id );
		return kNotImplemented;
	}

	/** Called between beginEdit and endEdit to inform the handler that a given parameter has a new
	 * value. This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult  performEdit( Vst::ParamID id, Vst::ParamValue valueNormalized ) override
	{
		printf( "performEdit called (%d, %f)\n", id, valueNormalized );
		SMTG_DBPRT2( "performEdit called (%d, %f)\n", id, valueNormalized );
		return kNotImplemented;
	}

	/** To be called after calling a performEdit (e.g. on mouse-click-up event).
	 * This must be called in the UI-Thread context!
	 * \note [UI-thread & Connected] */
	tresult  endEdit( Vst::ParamID id ) override
	{
		SMTG_DBPRT1( "endEdit called (%d)\n", id );
		return kNotImplemented;
	}

	/** Instructs host to restart the component. This must be called in the UI-Thread context!
	 * @param[in] flags is a combination of RestartFlags
	 * \note [UI-thread & Connected] */
	tresult  restartComponent( int32 flags ) override
	{
		SMTG_DBPRT1( "restartComponent called (%d)\n", flags );
		return kNotImplemented;
	}

	tresult  queryInterface( const TUID /*_iid*/, void** /*obj*/ ) override
	{
		return kNoInterface;
	}

	// we do not care here of the ref-counting. A plug-in call of release should not destroy this
	// class!
	uint32  addRef() override { return 1000; }
	uint32  release() override { return 1000; }

private:
	VST3::Hosting::Module::Ptr channelStripModule;
	VST3::Hosting::Module::Ptr dynamicsModule;
	VST3::Hosting::Module::Ptr meterModule;
	Vst::ProcessData ChannelProcessData;
	Vst::SampleRate ChannelSampleRate;
	int ChannelBlockSize;
};

