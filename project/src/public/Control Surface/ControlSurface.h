// Setup communicating with the control surface and linking
#pragma once
#include "rtmidi/RtMidi.h"

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


		void inline SetMasterChannelName( RtMidiOut* inMidiOutput, int inChannelNumber, string Name )
		{
			// Set Channel name to value that is 7 characters exactly. Truncate where required
			FControlSurfaceSysExMessage ChannelName;
			ChannelName.SysExCommand = 0x12;
			int offset = 0;

			// Pad or truncate name to exactly 7 chars
			string padded = Name.substr( 0, 7 );
			while ( padded.size() < 7 )
				padded += ' ';


			ChannelName.SysExPayload = new char[ 8 ];
			ChannelName.SysExPayload[ 0 ] = (unsigned char)offset;
			for ( int indexChar = 0; indexChar < padded.size(); indexChar++ )
			{
				ChannelName.SysExPayload[ indexChar + 1 ] = padded.at( indexChar );
			}

			inMidiOutput->sendMessage( &ChannelName.ToByteArray() );

		}


		void inline SetTimecodeMilliseconds( RtMidiOut* inMidiOutput, int milliseconds )
		{
			FControlSurfaceSysExMessage Timecode;
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
			int millisecond_place_0 = ( milliseconds / 1 ) % 10;
			Timecode.SysExCommand = 0x10;
			snprintf( buf, sizeof( buf ), "00%d%d%d%d%d%d%d%d%d%d",
				hour_place_2,
				hour_place_1,
				hour_place_0,
				minute_place_1,
				minute_place_0,
				second_place_1,
				second_place_0,
				millisecond_place_2,
				millisecond_place_1
			);
			Timecode.SysExPayload = buf;
			inMidiOutput->sendMessage( &Timecode.ToByteArray() );
		}



		namespace SSL
		{
			void inline ResetScribbleStrip( RtMidiOut* inMidiOutput )
			{
				// Set Channel name to value that is 7 characters exactly. Truncate where required
				FControlSurfaceSysExMessage ChannelName;
				ChannelName.SysExCommand = 0x12;

				for ( int i = 0; i < 16; i++ )
				{
					int offset = i * 7;

					// Pad or truncate name to exactly 7 chars
					string padded = "";
					while ( padded.size() < 7 )
						padded += ' ';

					ChannelName.SysExPayload = new char[ 8 ];
					ChannelName.SysExPayload[ 0 ] = (unsigned char)offset;
					for ( int indexChar = 0; indexChar < padded.size(); indexChar++ )
					{
						ChannelName.SysExPayload[ indexChar + 1 ] = padded.at( indexChar );
					}
				}

				inMidiOutput->sendMessage( &ChannelName.ToByteArray() );
			}

			namespace ChannelStrips
			{
				void inline SetChannelName( RtMidiOut* inMidiOutput, int inChannelNumber, string Name )
				{
					// Set Channel name to value that is 7 characters exactly. Truncate where required
					FControlSurfaceSysExMessage ChannelName;
					ChannelName.SysExCommand = 0x12;
					int offset = inChannelNumber * 7;

					// Pad or truncate name to exactly 7 chars
					string padded = Name.substr( 0, 7 );
					while ( padded.size() < 7 )
						padded += ' ';

					ChannelName.SysExPayload = new char[ 8 ];
					ChannelName.SysExPayload[ 0 ] = (unsigned char)offset;
					for ( int indexChar = 0; indexChar < padded.size(); indexChar++ )
					{
						ChannelName.SysExPayload[ indexChar + 1 ] = padded.at( indexChar );
					}

					inMidiOutput->sendMessage( &ChannelName.ToByteArray() );

				}

				void inline SetChannelDecibel( RtMidiOut* inMidiOutput, int inChannelNumber, float decibelValue )
				{
					// Set Channel name to value that is 7 characters exactly. Truncate where required
					FControlSurfaceSysExMessage ChannelName;
					ChannelName.SysExCommand = 0x12;
					int offset = inChannelNumber * 7 + 0x38;

					// Pad or truncate name to exactly 7 chars
					char buf[ 8 ];
					buf[ 0 ] = (unsigned char)offset;
					snprintf( buf + 1, sizeof( buf ) - 1, "%2.1f ",
						decibelValue
					);

					ChannelName.SysExPayload = buf;

					inMidiOutput->sendMessage( &ChannelName.ToByteArray() );
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

					struct FaderPosition : ControlSurfaceMessage
					{
						unsigned char msb = 0x00;
						unsigned char lsb = 0x00;
						FaderPosition() : ControlSurfaceMessage( 0xE0, lsb, msb ) {} // Default constructor initializes to fader 1 at maximum throw position
						FaderPosition( unsigned char Status_SelectedFader ) : ControlSurfaceMessage( Status_SelectedFader, lsb, msb ) {}
					};

					// Lowest position of the fader, which is negative infinity generally
					struct FaderPosition_NegativeInf : ControlSurfaceMessage
					{
						FaderPosition_NegativeInf() : ControlSurfaceMessage( 0xE0, 0x00, 0x00 ) {} // Default constructor initializes to fader 1 at negative infinity position
						FaderPosition_NegativeInf( int Status_SelectedFader ) : ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x00, 0x00 ) {}
					};

					// Maximum position of the fader, which is generally +10 or +12 dB
					struct FaderPosition_MaxThrow : ControlSurfaceMessage
					{
						FaderPosition_MaxThrow() : ControlSurfaceMessage( 0xE0, 0x7F, 0x7F ) {} // Default constructor initializes to fader 1 at maximum throw position
						FaderPosition_MaxThrow( int Status_SelectedFader ) : ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x7F, 0x7F ) {}
					};

					struct FaderPosition_Unity : ControlSurfaceMessage
					{
						FaderPosition_Unity() : ControlSurfaceMessage( 0xE0, 0x5B, 0x5B ) {} // Default constructor initializes to fader 1 at maximum throw position
						FaderPosition_Unity( int Status_SelectedFader ) : ControlSurfaceMessage( 0xE0 | Status_SelectedFader, 0x5B, 0x5B ) {}
					};

					void inline SetFaderPosition( RtMidiOut* inMidiOutput, int inChannelNumber, uint16_t inPosition )
					{
						FControlSurfaceMessage faderPosition;
						faderPosition.StatusByte = StatusBytes::SB_Fader_Position | inChannelNumber;
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

				namespace Effects
				{

					typedef struct FEffectParameter
					{
						uint32 ParameterID = 0;;
						double ParameterValue = 0.f;

					public:
						FEffectParameter( uint32 id ) : ParameterID( id ) {};
					};

					const double Param_Min = 0.f;
					const double Param_Max = 1.f;

					namespace NCS2
					{
						inline FEffectParameter Gain_Input( 1563454188 );
						inline FEffectParameter Gain_Output( 867502030 );
						inline FEffectParameter ChannelActive( 2004703496 );

						namespace Filters
						{
							inline FEffectParameter LowPassFilter( 1473943933 );
							inline FEffectParameter HighPassFilter( 225137163 );
						}
						namespace Eq
						{
							inline FEffectParameter Enabled( 2166545 );
							inline FEffectParameter E_Model_Enabled( 69881 );

							inline FEffectParameter HighShelf_Gain( 1240660653 );
							inline FEffectParameter HighShelf_Freq( 1240647078 );
							inline FEffectParameter HighShelf_Bell( 1240515633 );

							inline FEffectParameter HighMidBand_Gain( 1015610129 );
							inline FEffectParameter HighMidBand_Freq( 1015596554 );
							inline FEffectParameter HighMidBand_Q( 1463142943 );

							inline FEffectParameter LowMidBand_Gain( 1073914335 );
							inline FEffectParameter LowMidBand_Freq( 1073900760 );
							inline FEffectParameter LowMidBand_Q( 794052113 );

							inline FEffectParameter LowShelf_Gain( 571569823 );
							inline FEffectParameter LowShelf_Freq( 571556248 );
							inline FEffectParameter LowShelf_Bell( 571424803 );
						}
						namespace Dynamics
						{

							inline FEffectParameter Compressor_FastAttack_Enabled( 76418643 );
							inline FEffectParameter Compressor_Peak_Enabled( 1611847854 );
							inline FEffectParameter Compressor_Enabled( 1708054649 );
							inline FEffectParameter Compressor_Ratio( 576905756 );
							inline FEffectParameter Compressor_Threshold( 589759996 );
							inline FEffectParameter Compressor_Release( 462649816 );

							inline FEffectParameter Expander_Enabled( 1299822290 );
							inline FEffectParameter Expander_FastAttack_Enabled( 1357033743 );
							inline FEffectParameter Expander_Range( 615146258 );
							inline FEffectParameter Expander_Threshold( 76880832 );
							inline FEffectParameter Expander_Release( 710160540 );
							inline FEffectParameter Expander_Hold( 643021898 );
						}
					}

					namespace FourKE
					{
						namespace Filters
						{
							inline FEffectParameter Gain_Input( 1563454188 );
							inline FEffectParameter Gain_Output( 867502030 );

						}
						namespace Eq
						{
						}
						namespace Dynamics
						{
						}
					}

					namespace FourKG
					{
						namespace Filters
						{
							inline FEffectParameter UC1_Gain_Input( 1563454188 );
							inline FEffectParameter UC1_Gain_Output( 867502030 );

						}
						namespace Eq
						{
						}
						namespace Dynamics
						{
						}
					}
				}
			}

		}

	}
}
