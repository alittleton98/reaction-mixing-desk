// Setup communicating with the control surface and linking
#pragma once
#include "rtmidi/RtMidi.h"
#include "Common.h"


namespace ControlSurface
{
	namespace Messaging
	{
		namespace NoteValues
		{
			const unsigned char Note_On = 0x7f;
			const unsigned char Note_Off = 0x00;
			const unsigned char Note_Blink = 0x01;
		}

		namespace StatusBytes
		{
			const unsigned char SB_Note_Off = 0x80;
			const unsigned char SB_Note_On = 0x90;
			const unsigned char SB_Control_Change = 0xB0;
			const unsigned char SB_Channel_Meter = 0xD0;
			const unsigned char SB_Fader_Position = 0xE0;
			const unsigned char SB_PanPot_Rotation = 0xB0;
		}

		namespace SysExCommands
		{
			const unsigned char UpdateTimecodeDisplay = 0x10;
			const unsigned char UpdateAssignmentDisplay = 0x11;
			const unsigned char UpdateLCD = 0x12;
		}

		typedef struct FControlSurfaceMessage
		{
			unsigned char StatusByte;
			unsigned char DataByte1;
			unsigned char DataByte2;

			// Default constructor initializes all bytes to 0
			FControlSurfaceMessage() : StatusByte( 0x00 ), DataByte1( 0x00 ), DataByte2( 0x00 ) {}
			FControlSurfaceMessage( unsigned char inStatus, unsigned char inData1, unsigned char inData2 ) :
				StatusByte( inStatus ), DataByte1( inData1 ), DataByte2( inData2 )
			{
			}

			// Get Byte array for the message struct
			vector<unsigned char> ToByteArray() const
			{
				return { StatusByte, DataByte1, DataByte2 };
			}

		} ControlSurfaceMessage;

		typedef struct FControlSurfaceSysExMessage
		{
			const unsigned char SysExStart = 0xF0;
			const unsigned char SysExEnd = 0xF7;
			const unsigned char SysExMackieID_01 = 0x00;
			const unsigned char SysExMackieID_02 = 0x00;
			const unsigned char SysExMackieID_03 = 0x66;
			const unsigned char SysExMackieControlDeviceID = 0x14;
			unsigned char SysExCommand = 0x10;
			char* SysExPayload;

			// Get Byte array for the message struct
			vector<unsigned char> ToByteArray() const
			{
				vector<unsigned char> midiMsg;
				midiMsg.push_back( SysExStart );  // SysEx start
				midiMsg.push_back( SysExMackieID_01 );  // Mackie ID
				midiMsg.push_back( SysExMackieID_02 );
				midiMsg.push_back( SysExMackieID_03 );
				midiMsg.push_back( SysExMackieControlDeviceID );  // Mackie Control device
				midiMsg.push_back( SysExCommand );  // LCD write command

				for ( int i = 0; i < 12; i++ )
					midiMsg.push_back( (unsigned char)( SysExPayload[ i ] ) );

				midiMsg.push_back( SysExEnd );  // SysEx end

				return midiMsg;
			}
		}ControlSurfaceSysExMessage;


		// Define this function wherever it needs to be called to receive messages from the control surface
		void inline ReceiveMessageFromControlSurface( double deltatime, std::vector<unsigned char>* message, void* userData );

		// Send a message to the control surface
		void inline SendMessageToControlSurface( RtMidiOut* MidiOutput, ControlSurfaceMessage Message )
		{
			if ( !MidiOutput )
			{
				return;
			}
			MidiOutput->sendMessage( &Message.ToByteArray() );
		}

		// Send a group of messages to the control surface
		void inline SendMessageGroupToControlSurface( RtMidiOut* MidiOutput, vector<ControlSurfaceMessage> Messages )
		{
			if ( !MidiOutput )
			{
				return;
			}
			for ( ControlSurfaceMessage Message : Messages )
			{
				MidiOutput->sendMessage( &Message.ToByteArray() );
			}
		}


	}

	void inline ControlSurface_MidiCallback( double deltatime, std::vector<unsigned char>* message, void* userData )
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
	}

	void inline SetTimecodeMilliseconds( RtMidiOut* inMidiOutput, int milliseconds )
	{
		Messaging::FControlSurfaceSysExMessage Timecode;
		char buf[ 12 ];
		int hour_place_2 = ( milliseconds / 36000000000LL ) % 10;
		int hour_place_1 = ( milliseconds / 3600000000LL ) % 10;
		int hour_place_0 = ( milliseconds / 360000000LL ) % 10;
		int minute_place_1 = ( milliseconds / 600000LL ) % 10;
		int minute_place_0 = ( milliseconds / 60000LL ) % 10;
		int second_place_1 = ( milliseconds / 10000LL ) % 10;
		int second_place_0 = ( milliseconds / 1000LL ) % 10;
		int millisecond_place_2 = ( milliseconds / 100LL ) % 10;
		int millisecond_place_1 = ( milliseconds / 10LL ) % 10;
		int millisecond_place_0 = ( milliseconds ) % 10;
		Timecode.SysExCommand = 0x10;
		snprintf( buf, sizeof( buf ), "0%d%d%d%d%d%d%d%d%d%d",
			hour_place_1,
			hour_place_0,
			minute_place_1,
			minute_place_0,
			second_place_1,
			second_place_0,
			millisecond_place_2,
			millisecond_place_1,
			millisecond_place_0
		);
		Timecode.SysExPayload = buf;
		inMidiOutput->sendMessage( &Timecode.ToByteArray() );
	}

	void inline SetTextOnScribbleStrip( RtMidiOut* inMidiOutput, int inStartingIndex, int inRowNumber, string text )
	{
		Messaging::FControlSurfaceSysExMessage ScribbleText;
		ScribbleText.SysExCommand = 0x12;
		ScribbleText.SysExPayload = new char[ 8 ];

		if ( inStartingIndex > 56 - text.length() )
		{
			inStartingIndex = 56 - text.length();
		}

		// Assemble full 56 char string
		char rowText[ 56 ];
		int indexCharacter = 0;
		for ( indexCharacter = 0; indexCharacter < inStartingIndex; indexCharacter++ )
		{
			rowText[ indexCharacter ] = *"";
		}
		for ( indexCharacter = inStartingIndex; indexCharacter < text.length() + inStartingIndex; indexCharacter++ )
		{
			rowText[ indexCharacter ] = text.at( indexCharacter - inStartingIndex );
		}
		for ( indexCharacter = text.length() + inStartingIndex; indexCharacter < 56; indexCharacter++ )
		{
			rowText[ indexCharacter ] = *"";
		}

		for ( int i = 0; i < 8; i++ )
		{
			int offset = i * 7 + ( ( inRowNumber ) * 0x38 );

			ScribbleText.SysExPayload[ 0 ] = (unsigned char)offset;
			for ( int indexChar = 0; indexChar < 8; indexChar++ )
			{
				ScribbleText.SysExPayload[ indexChar + 1 ] = rowText[ offset + indexChar ];
			}
			inMidiOutput->sendMessage( &ScribbleText.ToByteArray() );

		}

		//inMidiOutput->sendMessage( &ScribbleText.ToByteArray() );
	}

	void inline ResetScribbleStrip( RtMidiOut* inMidiOutput )
	{
		if ( !inMidiOutput )
			return;

		// Set Channel name to value that is 7 characters exactly. Truncate where required
		Messaging::FControlSurfaceSysExMessage EmptyName;
		EmptyName.SysExCommand = Messaging::SysExCommands::UpdateLCD;

		for ( int i = 0; i < 16; i++ )
		{
			int offset = i * 7;

			// Pad or truncate name to exactly 7 chars
			string padded = "";
			while ( padded.size() < 7 )
				padded += ' ';

			EmptyName.SysExPayload = new char[ 8 ];
			EmptyName.SysExPayload[ 0 ] = (unsigned char)offset;
			for ( int indexChar = 0; indexChar < padded.size(); indexChar++ )
			{
				EmptyName.SysExPayload[ indexChar + 1 ] = padded.at( indexChar );
			}
			inMidiOutput->sendMessage( &EmptyName.ToByteArray() );

		}
	}

	namespace ProgressBar
	{

	}



	void inline SetEmptyProgressBar( RtMidiOut* inMidiOutput )
	{
	}

	void inline IncrementProgressBar( RtMidiOut* inMidiOutput )
	{
	}

	void inline StartProgressBar( RtMidiOut* inMidiOutput, int TotalOperations, string TaskName )
	{
		int steps = 56 / TotalOperations;

	}



	namespace ChannelStrips
	{
		void inline SetChannelName( RtMidiOut* inMidiOutput, int inChannelNumber, string Name )
		{
			// Set Channel name to value that is 7 characters exactly. Truncate where required
			Messaging::FControlSurfaceSysExMessage BusName;
			BusName.SysExCommand = 0x12;
			int offset = inChannelNumber * 7;

			// Pad or truncate name to exactly 7 chars
			string padded = Name.substr( 0, 7 );
			while ( padded.size() < 7 )
				padded += ' ';

			BusName.SysExPayload = new char[ 8 ];
			BusName.SysExPayload[ 0 ] = (unsigned char)offset;
			for ( int indexChar = 0; indexChar < padded.size(); indexChar++ )
			{
				BusName.SysExPayload[ indexChar + 1 ] = padded.at( indexChar );
			}

			inMidiOutput->sendMessage( &BusName.ToByteArray() );

		}

		void inline SetChannelDecibel( RtMidiOut* inMidiOutput, int inChannelNumber, float decibelValue )
		{
			// Set Channel name to value that is 7 characters exactly. Truncate where required
			Messaging::FControlSurfaceSysExMessage Name;
			Name.SysExCommand = 0x12;
			int offset = inChannelNumber * 7 + 0x38;

			// Pad or truncate name to exactly 7 chars
			char buf[ 8 ];
			buf[ 0 ] = (unsigned char)offset;
			snprintf( buf + 1, sizeof( buf ) - 1, "%2.1f ",
				decibelValue
			);

			Name.SysExPayload = buf;

			inMidiOutput->sendMessage( &Name.ToByteArray() );
		}

		namespace ChannelValues
		{
			const unsigned char Channel_01 = 0x00;
			const unsigned char Channel_02 = 0x01;
			const unsigned char Channel_03 = 0x02;
			const unsigned char Channel_04 = 0x03;
			const unsigned char Channel_05 = 0x04;
			const unsigned char Channel_06 = 0x05;
			const unsigned char Channel_07 = 0x06;
			const unsigned char Channel_08 = 0x07;
		}

		namespace Faders
		{
			// Fader Count
			const unsigned char Fader = 0xE0;

			struct FaderPosition : Messaging::ControlSurfaceMessage
			{
				unsigned char msb = 0x00;
				unsigned char lsb = 0x00;
				FaderPosition() : Messaging::ControlSurfaceMessage( 0xE0, lsb, msb ) {} // Default constructor initializes to fader 1 at maximum throw position
				FaderPosition( unsigned char Status_SelectedFader ) : Messaging::ControlSurfaceMessage( Status_SelectedFader, lsb, msb ) {}
			};

			// Lowest position of the fader, which is negative infinity generally
			struct FaderPosition_NegativeInf : Messaging::ControlSurfaceMessage
			{
				FaderPosition_NegativeInf() : Messaging::ControlSurfaceMessage( 0xE0, 0x00, 0x00 ) {} // Default constructor initializes to fader 1 at negative infinity position
				FaderPosition_NegativeInf( int Status_SelectedFader ) : Messaging::ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x00, 0x00 ) {}
			};

			// Maximum position of the fader, which is generally +10 or +12 dB
			struct FaderPosition_MaxThrow : Messaging::ControlSurfaceMessage
			{
				FaderPosition_MaxThrow() : Messaging::ControlSurfaceMessage( 0xE0, 0x7F, 0x7F ) {} // Default constructor initializes to fader 1 at maximum throw position
				FaderPosition_MaxThrow( int Status_SelectedFader ) : Messaging::ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x7F, 0x7F ) {}
			};

			struct FaderPosition_Unity : Messaging::ControlSurfaceMessage
			{
				FaderPosition_Unity() : Messaging::ControlSurfaceMessage( 0xE0, 0x5B, 0x5B ) {} // Default constructor initializes to fader 1 at maximum throw position
				FaderPosition_Unity( int Status_SelectedFader ) : Messaging::ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x5B, 0x5B ) {}
			};

			void inline SetFaderPosition( RtMidiOut* inMidiOutput, int inChannelNumber, uint16_t inPosition )
			{
				Messaging::FControlSurfaceMessage faderPosition;
				faderPosition.StatusByte = Messaging::StatusBytes::SB_Fader_Position | inChannelNumber;
				faderPosition.DataByte1 = inPosition & 0x7F;
				faderPosition.DataByte2 = ( inPosition >> 7 ) & 0x7F;

				inMidiOutput->sendMessage( &faderPosition.ToByteArray() );
			}
		}

		namespace PanPots
		{

			const unsigned char PanPot_Direction_Clockwise = 0x01;
			const unsigned char PanPot_Direction_CounterClockwise = 0x41;
			const unsigned char PanPot = 0x10;
			const unsigned char PanPot_Button = 0x20;
		}

		namespace Buttons
		{
			const unsigned char Buttons_Rec = 0x00;
			const unsigned char Buttons_Solo = 0x08;
			const unsigned char Buttons_Cut = 0x10;
			const unsigned char Buttons_Select = 0x18;
		}

		namespace Display
		{
			const unsigned char Display_Meter_NegInfinity = 0x00;
			const unsigned char Display_Meter_Unity = 0x0C;
			const unsigned char Display_Meter_Clip = 0x0E;
			const unsigned char Display_Meter_ClearClip = 0x0F;
		}
	}



}