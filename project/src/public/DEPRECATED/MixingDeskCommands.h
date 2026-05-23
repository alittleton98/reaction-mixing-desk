// Commands for controlling a mixing desk in Wwise/Reaper

namespace MixingDeskCommands
{
	 int Command_InitializeAll = 0;						// Initialize All Wwise Related Functionality in the proper sequence
	 int Command_InitializeWwiseSoundEngine = 0;			// Initialize Wwise Sound Engine. Swaps out the Reaper audio device with Wwise Sound Engine
	 int Command_DeinitializeWwiseSoundEngine = 0;		// Deinitialize Wwise Sound Engine. Restores the Reaper audio device
	 int Command_LoadWwiseProfilingData = 0;				// Load the profiling session into the timeline
	 int Command_ConnectToWwiseAuthoring = 0;				// Connect to the running instance of the Wwise Authoring tool
	 int Command_LoadWwiseProjectMixingDesk = 0;			// Load the Wwise project's Master-Mixer hierarchy into the Mixing Desk
	 int Command_StartMonitoringReaperSession = 0;		// Start monitoring the channel strip plugin for Wwise busses
	 int Command_StopMonitoringReaperSession = 0;			// Stop monitoring the channel strip plugin for Wwise busses


	bool InitializeWwiseSoundEngine(int Command, int Flag)
	{
		return true;
	}

	bool LoadWwiseProfilingData(int Command, int Flag)
	{
		return true;
	}

	bool LoadWwiseProjectMixingDesk(int Command, int Flag)
	{
		return true;
	}

	bool ConnectToWwiseAuthoring(int Command, int Flag)
	{
		return true;
	}

	bool StartMonitoringChannelStripPlugin(int Command, int Flag)
	{
		return true;
	}

	bool StopMonitoringReaperSession(int Command, int Flag)
	{
		return true;
	}
}

