// Timeline is the structure that holds all elements of the Session
#pragma once
#include "Common.h"
#include "Timekeeping.h"
#include <vector>

#define DEFAULT_TIMELINE_LENGTH 60000 // Default timeline length in milliseconds (1 minute)

struct WwiseAPICall
{
	string Description;
	string GameObjectName;
	uint32 GameObjectID;
	std::vector<std::string> Parameters;

	WwiseAPICall() : Description(""), GameObjectName(""), GameObjectID(0), Parameters({}) {}
	WwiseAPICall(string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters) :
		Description(inDescription), GameObjectName(inGameObjectName), GameObjectID(inGameObjectID), Parameters(inParameters) {}
};

struct WwiseSpatialAudioAPICall : WwiseAPICall
{
	string SpatialAudioFunction;

	WwiseSpatialAudioAPICall() : WwiseAPICall(), SpatialAudioFunction("") {}
	WwiseSpatialAudioAPICall (string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters, string inSpatialAudioFunction) :
		WwiseAPICall(inDescription, inGameObjectName, inGameObjectID, inParameters), SpatialAudioFunction(inSpatialAudioFunction) {
	}
};

// A frame is a single snapshot of the profiling session, containing all Wwise API calls that occurred during that frame. A frame is considered to be a single millisecond of time
struct FFrame
{
	long long FrameNumber;
	uint32 TimestampMS;
	vector<WwiseAPICall> WwiseAPICalls;
	FFrame* PreviousFrame;
	FFrame* NextFrame;
};

class FTimeline
{
public:
	long long totalFrames = 0;
	int FrameRate = 0;

public:
	FTimeline();
	~FTimeline();
	
	// Initialize the timeline
	bool Init();
	bool Cleanup();
	bool AddFrameToTimeline(FFrame inNewFrame);
	bool RemoveFrameFromTimeline(long long inFrameNumber, FFrame& outRemovedFrame);
	bool ClearTimeline();
	FFrame* GetFrame(long long inFrameNumber);
	bool AddCallToFrame(long long FrameNumber, WwiseAPICall& inAPICall);
	FFrame* GetCurrentFrame();
	FFrame* NextFrame();
	FFrame* PreviousFrame();

	bool LoadTimelineFromProfilingData(string ProfilingDataFilePath);
	

private:
	vector<FFrame> FrameArray;
	FFrame* CurrentFrame;
	bool isInitialized = false;	
};
 