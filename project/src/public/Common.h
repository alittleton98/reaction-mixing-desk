#pragma once
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <iostream>
#include <mutex>
using namespace std;

#define DEFAULT_SAMPLERATE 48000

enum class EAsioDevice : unsigned short
{
	VASIO_64,
	VASIO_256,
	VASIO_512,
	CUSTOM,
	NONE
};

enum class EChannelConfiguration : unsigned short
{
	MONO = 1,
	STEREO = 2,
	QUAD = 4,
	SURROUND = 6,
	SURROUND_REAR = 8,
	ATMOS = 12
};

enum class EChannelStripModel : unsigned short
{
	NATIVE,
	FOUR_THOUSAND_E,
	FOUR_THOUSAND_G
};

namespace ChannelStripColors
{
	const int64_t Default_Blue = 0xFF29A3CC;
	const int64_t Default_Green = 0xFF29CC29;
	const int64_t Default_Red = 0xFFCC2929;
	const int64_t Default_Yellow = 0xFFCCC729;
	const int64_t Default_Orange = 0xFFCC6D29;
	const int64_t Default_Purple = 0xFF8E29CC;

}

enum EMixingDeskOperatingMode
{
	MONO_NATIVE_64,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000G_64,				// Busses are loaded as Mono, 4KG Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000E_64,				// Busses are loaded as Mono, 4KE Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	STEREO_NATIVE_64,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000G_64,			// Busses are loaded as Stereo, 4KG Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000E_64,			// Busses are loaded as Stereo, 4KE Channel Strip, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	MONO_NATIVE_256,			// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000G_256,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	MONO_4000E_256,				// Busses are loaded as Mono, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master
	STEREO_NATIVE_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000G_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
	STEREO_4000E_256,			// Busses are loaded as Stereo, Native Channel Strip 2, Bus Compressor, and 56 Channel I/O with 8 Channel Master. Channels are represented in pairs except the Master 
};

typedef unsigned int uint32;

template <typename T, typename U>
constexpr bool IsSameType( T* inValue, U inOtherValue )
{
	return typeid ( inValue ) == typeid ( inOtherValue );
}

static string FixPath( const char* path )
{
	string result( path );
	string out;
	out.reserve( result.size() );

	for ( char c : result )
	{
		out += c;
		if ( c == '\\' )
			out += '\\';
	}

	return out;
}