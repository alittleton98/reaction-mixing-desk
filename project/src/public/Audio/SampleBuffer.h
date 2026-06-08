// Data passed through the audio signal chain
#pragma once
#include <atomic>
#include <cstdint>


static constexpr uint32_t SAMPLE_RING_BUFFER_MAGIC = 0xA510BEEF;
static constexpr uint32_t SAMPLE_RING_BUFFER_VERSION = 1;
static constexpr char SAMPLE_RING_BUFFER_MAPPING_BASE[] = "Global\\AsioSharedRingBuffer_v1_";

// Ring Buffer header structure
#pragma pack(push, 1)				// Remove padding bytes for the buffer so the offsets are the same
struct AudioSampleBufferHeader
{
	// immutable
	uint32_t magic;					// Sanity checker
	uint32_t version;
	uint32_t frameCapacity;			// Number of sample frames. Frame is all channels of interleaved audio. We use 1 right now, but leaving room to use the interleaved as we learn more
	uint32_t sampleRate;			// Samples per second
	uint32_t channelIndex;			// The channel of the audio device using this buffer
	uint32_t _pad[ 11 ];			// Pad the above to 64 bytes

	alignas( 64 ) std::atomic<uint64_t> writePointer; // Advanced by producer application
	alignas( 64 ) std::atomic<uint64_t> readPointer; // Advanced by the consumer

	AudioSampleBufferHeader() : magic( SAMPLE_RING_BUFFER_MAGIC ), version( SAMPLE_RING_BUFFER_VERSION ) {}
};
#pragma pack(pop)

static_assert( offsetof( AudioSampleBufferHeader, writePointer ) == 64, "writePointer is out of alignment" );
static_assert( offsetof( AudioSampleBufferHeader, readPointer ) == 128, "readPointer is out of alignment" );

struct AudioMemoryPage
{
	AudioSampleBufferHeader* header = nullptr;
	string mappingName = SAMPLE_RING_BUFFER_MAPPING_BASE;
	HANDLE memoryFile = nullptr;
};


// Get the total size of the buffer
inline SIZE_T GetRingBufferSize( uint32_t frameCapacity )
{
	return sizeof( AudioSampleBufferHeader ) + ( static_cast<SIZE_T>( frameCapacity ) * sizeof( float ) );
}

// Get the sample buffer by getting the header and the sample data that follows it
inline float* GetSampleBuffer( AudioSampleBufferHeader* header )
{
	return reinterpret_cast<float*>( reinterpret_cast<uint64_t*>( header ) + sizeof( AudioSampleBufferHeader ) );
}