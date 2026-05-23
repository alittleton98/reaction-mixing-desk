// Consuming audio device that drives the core clock of the MixingDesk. Also serves 
#pragma once
#include <stdio>
#include "asiosdk/common/asio.h"
#include "asiosdk/common/asiosys.h"
#include "asiosdk/host/asiodrivers.h"
#include "Audio/Channel.h"
#include "WwiseSoundEngine/WwiseSoundEngine.h"

using class Channel;

// internal data storage
typedef struct AudioDeviceDriverInfo
{
	// ASIOInit()
	ASIODriverInfo driverInfo;

	// ASIOGetChannels()
	long           inputChannels;
	long           outputChannels;

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
	ASIOBufferInfo bufferInfos[kMaxInputChannels + kMaxOutputChannels]; // buffer info's

	// ASIOGetChannelInfo()
	ASIOChannelInfo channelInfos[kMaxInputChannels + kMaxOutputChannels]; // channel info's
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
} AudioDeviceDriverInfo;


class AudioDevice
{
public:
	AudioDevice();
	bool Initialize();
	void OnSampleRateChange( ASIOSampleRate SampleRate );
	ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess );
	long OnAsioMessage( long Selector, long Value, void* Message, double* opt );

private:
	AudioDeviceDriverInfo DriverInfo;
};