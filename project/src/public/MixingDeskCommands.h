// Commands for controlling a mixing desk in Wwise/Reaper

static int Command_InitializeAll = 0;						// Initialize All Wwise Related Functionality in the proper sequence
static int Command_InitializeWwiseSoundEngine = 0;			// Initialize Wwise Sound Engine. Swaps out the Reaper audio device with Wwise Sound Engine
static int Command_DeinitializeWwiseSoundEngine = 0;		// Deinitialize Wwise Sound Engine. Restores the Reaper audio device
static int Command_LoadWwiseProfilingData = 0;				// Load the profiling session into the timeline
static int Command_ConnectToWwiseAuthoring = 0;				// Connect to the running instance of the Wwise Authoring tool
static int Command_LoadWwiseProjectMixingDesk = 0;			// Load the Wwise project's Master-Mixer hierarchy into the Mixing Desk
static int Command_StartMonitoringReaperSession = 0;		// Start monitoring the channel strip plugin for Wwise busses
static int Command_StopMonitoringReaperSession = 0;			// Stop monitoring the channel strip plugin for Wwise busses

static bool InitializeWwiseSoundEngine(int Command, int Flag)
{
	
}

static bool LoadWwiseProfilingData(int Command, int Flag)
{

}

static bool LoadWwiseProjectMixingDesk(int Command, int Flag)
{

}

static bool ConnectToWwiseAuthoring(int Command, int Flag)
{

}

static bool StartMonitoringChannelStripPlugin(int Command, int Flag)
{

}

static bool StopMonitoringReaperSession(int Command, int Flag)
{

}