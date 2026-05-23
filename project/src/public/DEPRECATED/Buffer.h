#pragma once
#pragma pack
#include <math.h>
#include <atomic>
#include <malloc.h>
#include <stdint.h>

typedef struct
{
	// stdint.h types used for explicit sizing of the header
	char riff_tag[4]; // RIFF
	int32_t riff_length; // Fixed size so this won't be dynamically assigned in this case
	char wave_tag[4]; // WAVE
	char fmt_tag[4]; // fmt - space at the end is required
	int32_t fmt_length; // legnth of format chunk - should be 16
	int16_t audio_format; // PCM is 1
	int16_t num_channels; // number of channels expected to be written to
	int32_t sample_rate; // number of samples per second 
	int32_t byte_rate; // number of bytes per second 
	int16_t block_align; // expected alignment of blocks that make up one sample for all channels 
	int16_t bits_per_sample; // bit size of each sample
	char data_tag[4]; // data
	int32_t data_length; // fixed size so this won't be dynamically assigned in this case
} FBufferHeader;

template<typename TSample, int NumFrames, int NumSamplesPerFrame>
class CAudioBuffer
{
private:
	// Number of frames that contain samples. Entire frame is pulled by the consumer at the desired frequency
	int mNumFrames;
	int mSamplesPerFrame;
	T mbuffer[NumFrames][NumSamplesPerFrame];
	std::atomic<int> mWriteIndex;
	std::atomic<int> mReadIndex;

public:
	CAudioBuffer() {};
	void AddSample( TSample Sample )
	{

	}
	void PopFrame( TSample* OutputFrame );

	bool IsEmpty() { return mWriteIndex == mReadIndex; };
};