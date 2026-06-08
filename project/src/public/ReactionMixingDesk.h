// Primary Reaction Mixing Desk application
// Launches the desk and initializes the core modules
// Allows for use of a config file
#pragma once

#define WWISE_PROJECT_PATH "E:\Dev\reaction-mixing-desk\wwise_projects\2023\2023.wproj"
#define WWISE_GENERATED_ROOT "E:\\Dev\\reaction-mixing-desk\\wwise_projects\\2023\GeneratedSoundBanks"

#include "Audio/SoundEngine.h"
#include "Control Surface/ControlSurface.h"
#include "Control Surface/SSLCommands.h"
#include "Ak/WwiseAuthoringAPI/waapi.h"
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "Control Surface/WwiseController.h"

enum ERunningPhase
{
	FINISHED,
	STOPPED,
	LAUNCH,
	SETUP_SELECTION,
	INITIALIZING,
	RUNNING,
	DEINITIALIZING
};

ReactionSoundEngine gSoundEngine;
WwiseController gWwiseController;

RtMidiIn gMidiInput_01;
RtMidiOut gMidiOutput_01;
RtMidiIn gMidiInput_02;
RtMidiOut gMidiOutput_02;
RtMidiIn gMidiInput_03;
RtMidiOut gMidiOutput_03;
RtMidiIn gMidiInput_04;
RtMidiOut gMidiOutput_04;

RtMidiIn* gInputArray[ 4 ] = { &gMidiInput_01, &gMidiInput_02, &gMidiInput_03, &gMidiInput_04 };
RtMidiOut* gOutputArray[ 4 ] = { &gMidiOutput_01, &gMidiOutput_02, &gMidiOutput_03, &gMidiOutput_04 };

AK::WwiseAuthoringAPI::Client waapiClient;

ERunningPhase gCurrentRunningPhase = ERunningPhase::STOPPED;
std::string LaunchActionTitle = "Action";
std::string LaunchAction[] = { "Launch", "Exit" };
std::string ChannelSelectionTitle = "ChCount";
std::string ChannelConfiguration[] = { "  64  ", "  256  " };
std::string ChannelStripPluginTitle = "ChStrip";
std::string ChannelStripPlugins[] = { "Native", "4000 E", "4000 G" };
std::string WwiseOperatingModeTitle = " Wwise ";
std::string WwiseOperatingModes[] = { "Engine ", "Control", "Record " };
std::string WwiseProjectPath = "";

// Start Up
bool LoadMidiDevices();

void HandleSetupSelection( std::vector<unsigned char>* message );

// SSL - UIX
void CycleLaunchOperation( int index );
void CycleChannelSetup( int index );
void CycleWwiseSoundEngineToggle( int index );


EMixingDeskOperatingMode SetOperatingMode();

void StartUpScreen( RtMidiOut* inMidiOutput )
{
	ControlSurface::ResetScribbleStrip( &gMidiOutput_01 );
	ControlSurface::SetTimecodeMilliseconds( &gMidiOutput_01, 0 );
	ControlSurface::SetTextOnScribbleStrip( &gMidiOutput_01, 0, 0, "DigitalWave   DynamixMixDesk" );
	gCurrentRunningPhase = ERunningPhase::SETUP_SELECTION;
	//ControlSurface
}

void ControlSurfaceUtilityScreen( RtMidiOut* inMidiOutput )
{
	ControlSurface::ResetScribbleStrip( inMidiOutput );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 0, LaunchActionTitle );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 1, LaunchAction[ 0 ] );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 2, ChannelSelectionTitle );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 3, "Native " );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 4, " Wwise " );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 5, "  Off  " );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 6, "" );
	ControlSurface::ChannelStrips::SetChannelName( inMidiOutput, 7, " START " );
	ControlSurface::Messaging::ControlSurfaceMessage LaunchButton;
	LaunchButton.StatusByte = ControlSurface::Messaging::StatusBytes::SB_Note_On;
	LaunchButton.DataByte1 = ControlSurface::ChannelStrips::Buttons::Buttons_Select + 7;
	LaunchButton.DataByte2 = ControlSurface::Messaging::NoteValues::Note_Blink;
	inMidiOutput->sendMessage( &LaunchButton.ToByteArray() );
}

void LoadingScreen( RtMidiOut* inMidiOutput, string inTaskName )
{
	string padded = inTaskName.substr( 0, 7 );
	while ( padded.size() < 7 )
		padded += ' ';

	string message;
	message = inTaskName + "LOADING";

	ControlSurface::SetTextOnScribbleStrip( inMidiOutput, 0, 0, message );
}

bool isUtilityScreenUp = false;
void( *gCurrentScreen )( RtMidiOut* );
void FlipLCD( RtMidiOut* inMidiOutput )
{
	if ( isUtilityScreenUp )
	{
		gCurrentScreen( inMidiOutput );
		isUtilityScreenUp = false;
	}
	else
	{
		ControlSurfaceUtilityScreen( inMidiOutput );
		isUtilityScreenUp = true;
	}
}

void SetMixDeskScreen()
{
}

enum EWwiseBusProperties
{

};

enum EWwiseSoundProperties
{

};

// Wwise Authoring
// USE WWISE IDs header
int totalWwiseBusse = 0;
bool ConnectToWaapi();
bool SetPropertyOnBus( int index );