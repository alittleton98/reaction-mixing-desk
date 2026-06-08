#pragma once
#include "ReactionMixingDesk.h"
#include <iostream>
#include <thread>
#include <chrono>
#include "rtmidi/RtMidi.h"

using namespace ControlSurface;
using namespace ControlSurface::Messaging;


bool LoadMidiDevices()
{
	unsigned int i = 0;
#if defined CONTROL_SURFACE_MIDI_IN
#if DEBUG_CONFIG
	printf( "Connecting to MIDI Input Port: %s\n", CONTROL_SURFACE_MIDI_IN );
#endif

	int inPortCount = gMidiInput_01.getPortCount();
	int inputCount = 1;
	for ( i = 0; i < inPortCount; i++ )
	{
		if ( inputCount >= 4 )
			break;
		std::string portName = gMidiInput_01.getPortName( i );
		string midiInNumbered = CONTROL_SURFACE_MIDI_IN;
		midiInNumbered.append( " " );
		midiInNumbered.append( to_string( inputCount ) );
		portName.pop_back();
		portName.pop_back();
		if ( portName == midiInNumbered )
		{
			printf( "Connecting to MIDI Input Port: %s\n", midiInNumbered.c_str() );
			gInputArray[ inputCount - 1 ]->openPort( i );
			gInputArray[ inputCount - 1 ]->ignoreTypes( false, false, false );
			inputCount++;
		}
	}
#else
#if DEBUG_CONFIG
	printf( "CONTROLSURFACE_MIDI_IN is not defined as an environment variable\n" );
#endif
	return false;
#endif

#if defined CONTROL_SURFACE_MIDI_OUT
#if DEBUG_CONFIG
	printf( "Connecting to MIDI Output Port: %s\n", CONTROL_SURFACE_MIDI_OUT );
#endif
	int outPortCount = gMidiOutput_01.getPortCount();
	int outputCount = 1;
	i = 0;
	for ( i = 0; i < outPortCount; i++ )
	{
		if ( outputCount >= 4 )
			break;
		std::string portName = gMidiOutput_01.getPortName( i );
		string midiOutNumbered = CONTROL_SURFACE_MIDI_OUT;
		midiOutNumbered.append( " " );
		midiOutNumbered.append( to_string( outputCount ) );
		portName.pop_back();
		portName.pop_back();
		if ( portName == midiOutNumbered )
		{
			printf( "Connecting to MIDI Input Port: %s\n", midiOutNumbered.c_str() );
			gOutputArray[ outputCount - 1 ]->openPort( i );
			outputCount++;
		}
	}
#else
#if DEBUG_CONFIG
	printf( "CONTROLSURFACE_MIDI_OUT is not defined as an environment variable\n" );
#endif
	return false;
#endif
	return true;
}

void midiCallback( double deltatime, std::vector<unsigned char>* message, void* userData )
{
	size_t nBytes = message->size();
#if DEBUG_CONFIG
	std::cout << "MIDI message received (" << nBytes << " bytes): ";
	for ( unsigned int i = 0; i < nBytes; i++ )
	{
		std::cout << "0x" << std::hex << (int)message->at( i ) << " ";
	}
	std::cout << std::dec << "\n";
#endif

	switch ( gCurrentRunningPhase )
	{
		case ERunningPhase::SETUP_SELECTION:
			HandleSetupSelection( message );

			break;
	}


	if ( gCurrentRunningPhase == ERunningPhase::SETUP_SELECTION )
	{
		if ( message->at( 0 ) == ControlSurface::Messaging::StatusBytes::SB_Note_On )
		{
			if ( (int)message->at( 1 ) == (int)( ControlSurface::ChannelStrips::Buttons::Buttons_Select + 7 ) )
			{
				gCurrentRunningPhase = ERunningPhase::LAUNCH;

				ControlSurface::Messaging::ControlSurfaceMessage LaunchButton;
				LaunchButton.StatusByte = ControlSurface::Messaging::StatusBytes::SB_Note_On;
				LaunchButton.DataByte1 = ControlSurface::ChannelStrips::Buttons::Buttons_Select + 7;
				LaunchButton.DataByte2 = ControlSurface::Messaging::NoteValues::Note_Off;
				gMidiOutput_01.sendMessage( &LaunchButton.ToByteArray() );
			}

		}

		//if ( (int)message->at( 1 ) = ControlSurface::Messaging::StatusBytes::SB_PanPot_Rotation&& )
	}
}

void HandleSetupSelection( std::vector<unsigned char>* message )
{
	unsigned char statusByte = message->at( 0 );
	unsigned char dataByte01 = message->at( 1 );
	unsigned char dataByte02 = message->at( 2 );

	switch ( statusByte )
	{
		//case StatusBytes::SB_PanPot_Rotation:

		//	switch ( dataByte01 )
		//	{
		//		case ChannelStrips::PanPots::PanPot + 1:
		//			//CycleChannelSetup();
		//			break;
		//	}


		//	break;
		//	case StatusBytes::
	}

}

void StartUpFaderThrow( RtMidiOut* inMidiOutput )
{
	ControlSurface::Messaging::ControlSurfaceMessage fader;
	ControlSurface::Messaging::ControlSurfaceMessage fader_0;
	ControlSurface::Messaging::ControlSurfaceMessage fader_1;
	ControlSurface::Messaging::ControlSurfaceMessage fader_2;
	ControlSurface::Messaging::ControlSurfaceMessage fader_3;
	ControlSurface::Messaging::ControlSurfaceMessage fader_4;
	ControlSurface::Messaging::ControlSurfaceMessage fader_5;
	ControlSurface::Messaging::ControlSurfaceMessage fader_6;
	ControlSurface::Messaging::ControlSurfaceMessage fader_7;
	const int totalFaders = 9;

	for ( int i = 0; i < totalFaders; i++ )
	{
		fader = ControlSurface::ChannelStrips::Faders::FaderPosition_MaxThrow( i );
		inMidiOutput->sendMessage( &fader.ToByteArray() );
	}

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	for ( int i = 0; i < totalFaders; i++ )
	{
		fader = ControlSurface::ChannelStrips::Faders::FaderPosition_NegativeInf( i );
		inMidiOutput->sendMessage( &fader.ToByteArray() );
	}

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	for ( int i = 0; i < totalFaders; i++ )
	{
		if ( i % 2 == 0 )
		{
			fader = ControlSurface::ChannelStrips::Faders::FaderPosition_MaxThrow( i );
			inMidiOutput->sendMessage( &fader.ToByteArray() );

		}
		else
		{
			fader = ControlSurface::ChannelStrips::Faders::FaderPosition_NegativeInf( i );
			inMidiOutput->sendMessage( &fader.ToByteArray() );

		}
	}

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );

	for ( int i = 0; i < totalFaders; i++ )
	{
		if ( i % 2 == 1 )
		{
			fader = ControlSurface::ChannelStrips::Faders::FaderPosition_MaxThrow( i );
			inMidiOutput->sendMessage( &fader.ToByteArray() );

		}
		else
		{
			fader = ControlSurface::ChannelStrips::Faders::FaderPosition_NegativeInf( i );
			inMidiOutput->sendMessage( &fader.ToByteArray() );

		}
	}

	std::this_thread::sleep_for( std::chrono::milliseconds( 500 ) );
	for ( int i = 0; i < totalFaders; i++ )
	{
		fader = ControlSurface::ChannelStrips::Faders::FaderPosition_Unity( i );
		inMidiOutput->sendMessage( &fader.ToByteArray() );
	}
	std::this_thread::sleep_for( std::chrono::milliseconds( 800 ) );
	for ( int i = 0; i < totalFaders; i++ )
	{
		fader = ControlSurface::ChannelStrips::Faders::FaderPosition_NegativeInf( i );
		inMidiOutput->sendMessage( &fader.ToByteArray() );
	}
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
		case WM_CLOSE:
			// Optional: prompt user to save / confirm quit
			ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
			return 0;

		case WM_DESTROY:
			ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
			ControlSurface::SetTimecodeMilliseconds( &gMidiOutput_01, 0 );
			PostQuitMessage( 0 );          // breaks the GetMessage loop → wWinMain returns
			return 0;

	}
	return DefWindowProc( hWnd, message, wParam, lParam );
}

BOOL WINAPI ConsoleCtrlHandler( DWORD dwCtrlType )
{
	ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
	ControlSurface::SetTimecodeMilliseconds( &gMidiOutput_01, 0 );
	return TRUE;
}

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/ )
{
	AllocConsole();
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );
	if ( !SetConsoleCtrlHandler( ConsoleCtrlHandler, TRUE ) ) {
		std::cerr << "Failed to register control handler\n";
		return 1;
	}


	// Optional: disable buffering so output appears immediately
	setvbuf( stdout, nullptr, _IONBF, 0 );
	setvbuf( stderr, nullptr, _IONBF, 0 );

	if ( !LoadMidiDevices() )
		return 1;

	gCurrentRunningPhase = ERunningPhase::INITIALIZING;
	gCurrentScreen = StartUpScreen;

	std::this_thread::sleep_for( std::chrono::milliseconds( 1000 ) );
	StartUpFaderThrow( &gMidiOutput_01 );
	gCurrentRunningPhase = ERunningPhase::INITIALIZING;
	StartUpScreen( &gMidiOutput_01 );
	gMidiInput_01.setCallback( &midiCallback );

	while ( gCurrentRunningPhase == ERunningPhase::SETUP_SELECTION )
	{
		std::this_thread::sleep_for( std::chrono::milliseconds( 16 ) );
	}


	ReactionSoundEngine* soundEngineInstance = ReactionSoundEngine::GetSoundEngine();
	if ( !soundEngineInstance )
		return 1;

	if ( !soundEngineInstance->LoadSoundEngineConfiguration( EMixingDeskOperatingMode::STEREO_4000E_64 ) )
		return 1;

	ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 0, "Init:" );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 1, "ASIO" );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "Loading" );
	if ( !soundEngineInstance->InitializeASIODevice() )
	{
		ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "FAILED" );
		std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
		return 1;

	}
	soundEngineInstance->Callbacks.sampleRateDidChange = &OnSampleRateChange;
	soundEngineInstance->Callbacks.asioMessage = &OnAsioMessage;
	soundEngineInstance->Callbacks.bufferSwitchTimeInfo = &OnBufferSwitchTimeInfo;
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "SUCCESS" );
	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 1, "dBCHAIN" );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "Loading" );
	if ( !soundEngineInstance->InitializeSignalChain() ) {
		ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "FAILED" );
		std::this_thread::sleep_for( std::chrono::seconds( 5 ) );
		return 1;

	}
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "SUCCESS" );
	std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
	ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 0, "Test:" );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 1, "TC" );
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "Running" );
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
		ControlSurface::SetTimecodeMilliseconds( &gMidiOutput_01, elapsedTime );
		std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
	}
	ControlSurface::ChannelStrips::SetChannelName( &gMidiOutput_01, 2, "Complete" );

	MSG msg;
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	/*gSoundEngine->TerminateSoundEngine();*/
	return 0;
}

