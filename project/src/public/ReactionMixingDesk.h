/*
Digital Wave Research Lab - Reaction Mixing Desk
Reaper Plugin to run a Wwise Sound Engine simulation and lineraly track a Wwise Profiling session.
Copyright (C) 2026 Reaction Digital LLC
*/ 

#include "reaper_plugin.h"
#include "reaper_plugin_functions.h"

namespace ReactionMixingDesk
{
	// SSL UC1 Channel Strip Plugin Values Mapping. These values are the ones monitored for the selected track's channel strip plugin instance.
	static struct ChannelStripPluginValues
	{
		float InputGain;				// Wwise Bus Voice Volume
		float OutputGain;				// Wwise Bus EQ Output Gain
		float LowPassFrequency;			// Wwise Bus LPF
		float HighPassFrequency;		// Wwise Bus HPF
		float HighShelfFrequency;		// Wwise Bus EQ High Shelf Frequency
		float HighShelfGain;			// Wwise Bus EQ High Shelf Gain
		float MidPeakFrequency;			// Wwise Bus EQ Mid Peak Frequency
		float MidPeakGain;				// Wwise Bus EQ Mid Peak Gain
		float MidPeakQ;					// Wwise Bus EQ Mid Peak Q	
		float LowShelfFrequency;		// Wwise Bus EQ Low Shelf Frequency
		float LowShelfGain;				// Wwise Bus EQ Low Shelf Gain
		float CompressorThreshold;		// Wwise Bus Compressor Threshold
		float CompressorRatio;			// Wwise Bus Compressor Ratio
		float CompressorAttack;			// Wwise Bus Compressor Attack
		float CompressorRelease;		// Wwise Bus Compressor Release
		float CompressorMakeUpGain;		// Wwise Bus Compressor Output Gain
		bool bBypassEQ;					// Wwise Bus EQ Bypass
		bool bBypassCompressor;			// Wwise Bus Compressor Bypass
	};

	// Reaper Actions
	custom_action_register_t Action_InitializeAll =
	{
		0,
		"REACTION_MIXING_DESK_INITIALIZE_ALL",
		"Reaction Mixing Desk: Initialize All Wwise Functionality",
		nullptr
	};

	custom_action_register_t Action_InitializeWwiseSoundEngine =
	{
		0,
		"REACTION_MIXING_DESK_INITIALIZE_WWISE_SOUND_ENGINE",
		"Reaction Mixing Desk: Initialize Wwise Sound Engine",
		nullptr
	};

	custom_action_register_t Action_DeinitializeWwiseSoundEngine =
	{
		0,
		"REACTION_MIXING_DESK_DEINITIALIZE_WWISE_SOUND_ENGINE",
		"Reaction Mixing Desk: Deinitialize Wwise Sound Engine",
		nullptr
	};

	custom_action_register_t Action_LoadWwiseProfilingData =
	{
		0,
		"REACTION_MIXING_DESK_LOAD_WWISE_PROFILING_DATA",
		"Reaction Mixing Desk: Load Wwise Profiling Data",
		nullptr
	};

	custom_action_register_t Action_ConnectToWwiseAuthoring =
	{
		0,
		"REACTION_MIXING_DESK_CONNECT_TO_WWISE_AUTHORING",
		"Reaction Mixing Desk: Connect to Wwise Authoring",
		nullptr
	};

	custom_action_register_t Action_LoadWwiseProjectMixingDesk =
	{
		0,
		"REACTION_MIXING_DESK_LOAD_WWISE_PROJECT_MIXING_DESK",
		"Reaction Mixing Desk: Load Wwise Project Mixing Desk",
		nullptr
	};

	custom_action_register_t Action_StartMonitoringReaperSession =
	{
		0,
		"REACTION_MIXING_DESK_START_MONITORING_REAPER_SESSION",
		"Reaction Mixing Desk: Start Monitoring Reaper Session",
		nullptr
	};

	custom_action_register_t Action_StopMonitoringReaperSession =
	{
		0,
		"REACTION_MIXING_DESK_STOP_MONITORING_REAPER_SESSION",
		"Reaction Mixing Desk: Stop Monitoring Reaper Session",
		nullptr
	};
}


