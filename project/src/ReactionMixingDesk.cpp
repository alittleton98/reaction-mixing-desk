#include "ReactionMixingDesk.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "rtmidi/RtMidi.h"
#include "Control Surface/ControlSurface.h"

void midiCallback( double deltatime, std::vector<unsigned char>* message, void* userData )
{
	size_t nBytes = message->size();

	std::cout << "MIDI message received (" << nBytes << " bytes): ";
	for ( unsigned int i = 0; i < nBytes; i++ ) {
		std::cout << "0x" << std::hex << (int)message->at( i ) << " ";
	}
	std::cout << std::dec << "\n";
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
	RtMidiOut midiOut;

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

	// Channel 0-7, each slot is 7 chars wide
	int offset = 0 * 7 + 0x38;  // upper row

	string name = "0.0dB";
	// Pad or truncate name to exactly 7 chars
	std::string padded = name.substr( 0, 7 );
	while ( padded.size() < 7 )
		padded += ' ';

	// Build SysEx message
	std::vector<unsigned char> midiMsg;
	midiMsg.push_back( 0xF0 );  // SysEx start
	midiMsg.push_back( 0x00 );  // Mackie ID
	midiMsg.push_back( 0x00 );
	midiMsg.push_back( 0x66 );
	midiMsg.push_back( 0x14 );  // Mackie Control device
	midiMsg.push_back( 0x12 );  // LCD write command
	midiMsg.push_back( offset );// position on LCD



	for ( char c : padded )
		midiMsg.push_back( (unsigned char)c );

	midiMsg.push_back( 0xF7 );  // SysEx end

	midiOut.sendMessage( &midiMsg );

	// Channel 0-7, each slot is 7 chars wide
	offset = 0 * 7;  // upper row

	name = "Waffles";
	// Pad or truncate name to exactly 7 chars
	padded = name.substr( 0, 7 );
	while ( padded.size() < 7 )
		padded += ' ';

	// Build SysEx message
	midiMsg;
	midiMsg.push_back( 0xF0 );  // SysEx start
	midiMsg.push_back( 0x00 );  // Mackie ID
	midiMsg.push_back( 0x00 );
	midiMsg.push_back( 0x66 );
	midiMsg.push_back( 0x14 );  // Mackie Control device
	midiMsg.push_back( 0x12 );  // LCD write command
	midiMsg.push_back( offset );// position on LCD

	for ( char c : padded )
		midiMsg.push_back( (unsigned char)c );

	midiMsg.push_back( 0xF7 );  // SysEx end

	midiOut.sendMessage( &midiMsg );
	//        FF     SS       MM       HH  (right to left)
	//char buf[ 12 ];
	//snprintf( buf, sizeof( buf ), "0%d%d%d%d%d%d%d%d%d%d",
	//	1, 2, 3, 4, 5, 6, 7, 8, 9, 9 );


	//// Build SysEx message
	//midiMsg;
	//midiMsg.push_back( 0xF0 );  // SysEx start
	//midiMsg.push_back( 0x00 );  // Mackie ID
	//midiMsg.push_back( 0x00 );
	//midiMsg.push_back( 0x66 );
	//midiMsg.push_back( 0x14 );  // Mackie Control device
	//midiMsg.push_back( 0x10 );  // LCD write command

	//for ( int i = 0; i < 12; i++ )
	//	midiMsg.push_back( (unsigned char)( buf[ i ] ) );

	//midiMsg.push_back( 0xF7 );  // SysEx end

	//midiOut.sendMessage( &midiMsg );

	long long totalDuration = 15000;;
	long long elapsedTime = 0;
	auto start = std::chrono::system_clock::now();

	ControlSurface::Messaging::SetChannelName( &midiOut, 1, "Pants" );


	while ( elapsedTime < totalDuration )
	{

		auto now = std::chrono::system_clock::now();
		elapsedTime += std::chrono::duration_cast<std::chrono::milliseconds>( now - start ).count();
		//printf( "%lld\n", elapsedTime );
		start = now;
		ControlSurface::Messaging::SetTimecodeMilliseconds( &midiOut, elapsedTime );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}


	SoundEngine* soundEngineInstance = SoundEngine::GetSoundEngine();
	if ( !soundEngineInstance )
		return 1;

	if ( !soundEngineInstance->LoadSoundEngineConfiguration( EMixingDeskOperatingMode::MONO_4000E_64 ) )
		return 1;


	if ( !soundEngineInstance->InitializeASIODevice() )
		return 1;
	soundEngineInstance->Callbacks.sampleRateDidChange = &OnSampleRateChange;
	soundEngineInstance->Callbacks.asioMessage = &OnAsioMessage;
	soundEngineInstance->Callbacks.bufferSwitchTimeInfo = &OnBufferSwitchTimeInfo;
	/*if ( !soundEngineInstance->InitializeSignalChain() )
		return 1;*/
		//soundEngineInstance->StartSoundEngine();

	MSG msg;
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	/*gSoundEngine->TerminateSoundEngine();*/
	return 0;
}

