#include "RtMidi.h"
#include "Common.h"
#include "RMDError.h"
//#include "Timeline.h"
#include "Timekeeping.h"
#include "ControlSurface.h"

using namespace std;

using TickCallback = bool( TickingObject::* )( float deltaTime );
// Function pointer def for a bool Tick(float deltaTime) function signature
//typedef struct TickCallbackObject { TickingObject* Object;  TickCallback& Callback;};
//vector<TickCallbackObject> g_TickCallbacks;
RMDError g_ApplicationErrorStatus;



void midiCallback( double deltatime, std::vector<unsigned char>* message, void* userData )
{
	unsigned int nBytes = message->size();

	std::cout << "MIDI message received (" << nBytes << " bytes): ";
	for ( unsigned int i = 0; i < nBytes; i++ ) {
		std::cout << "0x" << std::hex << (int)message->at( i ) << " ";
	}
	std::cout << std::dec << "\n";
}

int midiTesting()
{
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
	midiIn.openPort( 6 );
	midiIn.ignoreTypes( false, false, false );
	midiOut.openPort( 7 );

	using namespace ControlSurface::Messaging;
	ControlSurfaceMessage fader1;
	ControlSurfaceMessage fader2;
	ControlSurfaceMessage fader3;
	ControlSurfaceMessage fader4;
	ControlSurfaceMessage fader5;
	ControlSurfaceMessage fader6;
	ControlSurfaceMessage fader7;
	ControlSurfaceMessage fader8;


	fader1 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_MaxThrow( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_NegativeInf( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	this_thread::sleep_for( chrono::milliseconds( 750 ) );

	fader1 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_01 );
	midiOut.sendMessage( &fader1.ToByteArray() );
	fader2 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_02 );
	midiOut.sendMessage( &fader2.ToByteArray() );
	fader3 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_03 );
	midiOut.sendMessage( &fader3.ToByteArray() );
	fader4 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_04 );
	midiOut.sendMessage( &fader4.ToByteArray() );
	fader5 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_05 );
	midiOut.sendMessage( &fader5.ToByteArray() );
	fader6 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_06 );
	midiOut.sendMessage( &fader6.ToByteArray() );
	fader7 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_07 );
	midiOut.sendMessage( &fader7.ToByteArray() );
	fader8 = SSL::ChannelStrips::Faders::FaderPosition_Unity( SSL::ChannelStrips::Faders::Fader_08 );
	midiOut.sendMessage( &fader8.ToByteArray() );

	std::string line;
	std::getline( std::cin, line );
	midiIn.closePort();
	return 0;
}

bool LogDeltaTime( float DeltaTime )
{
	printf( "MainThread Delta Time: %f\n", DeltaTime );
	std::cout << "\033[2J\033[H";
	return true;
}

int main()
{
	//midiTesting ();

	g_ApplicationErrorStatus = RMDErrors::NO_ERROR;
	chrono::time_point<chrono::high_resolution_clock> lastknownTime = chrono::high_resolution_clock::now();

	// Tick Watchers
	while ( g_ApplicationErrorStatus.code == RMDErrors::NO_ERROR.code )
	{

		chrono::time_point<chrono::high_resolution_clock> now = chrono::high_resolution_clock::now();
		chrono::duration<float, milli> elapsed = now - lastknownTime;
		float deltaTime = elapsed.count();

		lastknownTime = chrono::high_resolution_clock::now();
	}
}