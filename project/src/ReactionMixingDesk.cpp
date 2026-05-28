#pragma once
#include "ReactionMixingDesk.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "rtmidi/RtMidi.h"

void midiCallback( double deltatime, std::vector<unsigned char>* message, void* userData )
{
	size_t nBytes = message->size();

	std::cout << "MIDI message received (" << nBytes << " bytes): ";
	for ( unsigned int i = 0; i < nBytes; i++ ) {
		std::cout << "0x" << std::hex << (int)message->at( i ) << " ";
	}
	std::cout << std::dec << "\n";

	if ( ( *message )[ 0 ] == 0xF0 &&
		( *message )[ 1 ] == 0x00 &&
		( *message )[ 2 ] == 0x00 &&
		( *message )[ 3 ] == 0x66 )
	{
		unsigned char deviceID = ( *message )[ 4 ];
		unsigned char command = ( *message )[ 5 ];

		if ( command == 0x01 )
			printf( "Device ID: 0x%02X\n", deviceID );
	}

}

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/ )
{
	AllocConsole();
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );

	// Optional: disable buffering so output appears immediately
	setvbuf( stdout, nullptr, _IONBF, 0 );
	setvbuf( stderr, nullptr, _IONBF, 0 );

	RtMidiIn midiIn;
	RtMidiIn midiInMaster;
	RtMidiOut midiOut;
	RtMidiOut midiOutMaster;

	unsigned int inPortCount = midiIn.getPortCount();
	unsigned int outPortCount = midiOut.getPortCount();

	printf( "Input Port count: %i\n", inPortCount );
	for ( unsigned int i = 0; i < inPortCount; i++ ) {
		std::string portName = midiIn.getPortName( i );
		std::cout << i << ": " << portName << "\n";
	}
	printf( "Output Port count: %i\n", outPortCount );
	for ( unsigned int i = 0; i < outPortCount; i++ ) {
		std::string portName = midiOut.getPortName( i );
		std::cout << i << ": " << portName << "\n";
	}

	midiIn.setCallback( &midiCallback );
	midiIn.openPort( 3 );
	midiIn.ignoreTypes( false, false, false );
	midiOut.openPort( 4 );

	ControlSurface::Messaging::SSL::ResetScribbleStrip( &midiOut );

	SoundEngine* soundEngineInstance = SoundEngine::GetSoundEngine();
	if ( !soundEngineInstance )
		return 1;

	if ( !soundEngineInstance->LoadSoundEngineConfiguration( EMixingDeskOperatingMode::MONO_NATIVE_64 ) )
		return 1;


	if ( !soundEngineInstance->InitializeASIODevice() )
		return 1;
	soundEngineInstance->Callbacks.sampleRateDidChange = &OnSampleRateChange;
	soundEngineInstance->Callbacks.asioMessage = &OnAsioMessage;
	soundEngineInstance->Callbacks.bufferSwitchTimeInfo = &OnBufferSwitchTimeInfo;
	if ( !soundEngineInstance->InitializeSignalChain() )
		return 1;
	//soundEngineInstance->StartSoundEngine();

	long long totalDuration = 15000;;
	long long elapsedTime = 0;
	auto start = std::chrono::system_clock::now();
	while ( elapsedTime < totalDuration )
	{

		auto now = std::chrono::system_clock::now();
		elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>( now - start ).count();
		//printf( "%lld\n", elapsedTime );
		start = now;
		ControlSurface::Messaging::SetTimecodeMilliseconds( &midiOut, elapsedTime );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}

	MSG msg;
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	/*gSoundEngine->TerminateSoundEngine();*/
	return 0;
}

