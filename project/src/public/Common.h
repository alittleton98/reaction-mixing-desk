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
};

enum class EChannelStripModel : unsigned short
{
	NATIVE,
	FOUR_THOUSAND_E,
	FOUR_THOUSAND_G
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