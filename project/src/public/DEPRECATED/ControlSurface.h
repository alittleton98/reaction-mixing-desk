// Setup communicating with the control surface and linking
#include "RtMidi.h"

//typedef struct FControlSurfaceMessage
//{
//	unsigned char StatusByte;
//	unsigned char DataByte1;
//	unsigned char DataByte2;
//} ControlSurfaceMessage;

namespace ControlSurface
{
	namespace Messaging
	{
		typedef struct FControlSurfaceMessage
		{
			unsigned char StatusByte;
			unsigned char DataByte1;
			unsigned char DataByte2;

			// Default constructor initializes all bytes to 0
			FControlSurfaceMessage() : StatusByte(0x00), DataByte1(0x00), DataByte2(0x00) {}
			FControlSurfaceMessage(unsigned char inStatus, unsigned char inData1, unsigned char inData2) :
				StatusByte(inStatus), DataByte1(inData1), DataByte2(inData2) {
			}

			// Get Byte array for the message struct
			vector<unsigned char> ToByteArray() const
			{
				return { StatusByte, DataByte1, DataByte2 };
			}

		} ControlSurfaceMessage;

		// Define this function wherever it needs to be called to receive messages from the control surface
		void ReceiveMessageFromControlSurface(double deltatime, std::vector<unsigned char>* message, void* userData);

		// Send a message to the control surface
		void SendMessageToControlSurface(RtMidiOut* MidiOutput, ControlSurfaceMessage Message)
		{
			if (!MidiOutput)
			{
				return;
			}
			MidiOutput->sendMessage(&Message.ToByteArray());
		}

		// Send a group of messages to the control surface
		void SendMessageGroupToControlSurface(RtMidiOut* MidiOutput, vector<ControlSurfaceMessage> Messages)
		{
			if (!MidiOutput)
			{
				return;
			}
			for (ControlSurfaceMessage Message : Messages)
			{
				MidiOutput->sendMessage(&Message.ToByteArray());
			}
		}

		namespace SSL
		{
			namespace ChannelStrips
			{
				namespace Faders
				{
					// Fader Count
					const unsigned char Fader_01 = 0xE0;
					const unsigned char Fader_02 = 0xE1;
					const unsigned char Fader_03 = 0xE2;
					const unsigned char Fader_04 = 0xE3;
					const unsigned char Fader_05 = 0xE4;
					const unsigned char Fader_06 = 0xE5;
					const unsigned char Fader_07 = 0xE6;
					const unsigned char Fader_08 = 0xE7;
					const unsigned char Fader_09 = 0xE8;
					const unsigned char Fader_10 = 0xE9;
					const unsigned char Fader_11 = 0xEA;
					const unsigned char Fader_12 = 0xEB;
					const unsigned char Fader_13 = 0xEC;
					const unsigned char Fader_14 = 0xED;
					const unsigned char Fader_15 = 0xEE;
					const unsigned char Fader_16 = 0xEF;
					const unsigned char Fader_17 = 0xF0;
					const unsigned char Fader_18 = 0xF1;
					const unsigned char Fader_19 = 0xF2;
					const unsigned char Fader_20 = 0xF3;
					const unsigned char Fader_21 = 0xF4;
					const unsigned char Fader_22 = 0xF5;
					const unsigned char Fader_23 = 0xF6;
					const unsigned char Fader_24 = 0xF7;
					const unsigned char Fader_25 = 0xF8;
					const unsigned char Fader_26 = 0xF9;
					const unsigned char Fader_27 = 0xFA;
					const unsigned char Fader_28 = 0xFB;
					const unsigned char Fader_29 = 0xFC;
					const unsigned char Fader_30 = 0xFD;
					const unsigned char Fader_31 = 0xFE;
					const unsigned char Fader_32 = 0xFF;

					// Lowest position of the fader, which is negative infinity generally
					struct FaderPosition_NegativeInf : ControlSurfaceMessage
					{
						FaderPosition_NegativeInf() : ControlSurfaceMessage(0xE0, 0x00, 0x00) {} // Default constructor initializes to fader 1 at negative infinity position
						FaderPosition_NegativeInf(unsigned char Status_SelectedFader) : ControlSurfaceMessage(Status_SelectedFader, 0x00, 0x00) {}
					};

					// Maximum position of the fader, which is generally +10 or +12 dB
					struct FaderPosition_MaxThrow : ControlSurfaceMessage
					{
						FaderPosition_MaxThrow() : ControlSurfaceMessage(0xE0, 0x7F, 0x7F) {} // Default constructor initializes to fader 1 at maximum throw position
						FaderPosition_MaxThrow(unsigned char Status_SelectedFader) : ControlSurfaceMessage(Status_SelectedFader, 0x7F, 0x7F) {}
					};

					struct FaderPosition_Unity : ControlSurfaceMessage
					{
						FaderPosition_Unity() : ControlSurfaceMessage(0xE0, 0x5B, 0x5B) {} // Default constructor initializes to fader 1 at maximum throw position
						FaderPosition_Unity(unsigned char Status_SelectedFader) : ControlSurfaceMessage(Status_SelectedFader, 0x5B, 0x5B) {}
					};
				}

				namespace PanPots
				{
					const unsigned char PanPot_Status = 0xB0;
					const unsigned char PanPot_Direction_Clockwise = 0x00;

					// Channel
					const unsigned char Pot_01 = 0x10;
					const unsigned char Pot_02 = 0x11;
					const unsigned char Pot_03 = 0x12;
					const unsigned char Pot_04 = 0x13;
					const unsigned char Pot_05 = 0x14;
					const unsigned char Pot_06 = 0x15;
					const unsigned char Pot_07 = 0x16;
					const unsigned char Pot_08 = 0x17;
				}
			}

		}

	}
}
