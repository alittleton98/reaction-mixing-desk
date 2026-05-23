// Timeline is the structure that holds all elements of the Session in the specified format and delineation
#pragma once
#include "Common.h"
#include "Timekeeping.h"
#include "WwiseProfile.h"
#include <vector>
#include <map>

#define DEFAULT_TIMELINE_LENGTH 60000 // Default timeline length in milliseconds (1 minute)

using FFrame;
using FTrack;

// A frame is a single snapshot of information in the session. Contains the data for the frame number, timestamp, and the method required to process the frame's information. Frames are contained within tracks.
template <typename T>
struct FFrame
{
	long long FrameNumber = 0;
	uint32 TimestampMS = 0;
	vector<T> FrameActions;
	FFrame* PreviousFrame = nullptr;
	FFrame* NextFrame = nullptr;
	FTrack* ParentTrack = nullptr;
	char* TypeIdentifier = "";
	FFrame () = default;
};

// A track is a collection of frames
template <typename T>
struct FTrack
{
	string TrackName = "";
	vector<FFrame<T>> Frames;
	FFrame<T>* CurrentFrame;
	FTrack () = default;
};

// Allow for different types of API calls or timeline based behaviors
class ITimeline
{
public:
	long long totalFrames = 0;
	int FrameRate = 0;

public:
	ITimeline ();
	~ITimeline () = default;

	// Initialize the timeline
	virtual bool Init () = false;
	virtual bool Cleanup () = false;
	virtual bool AddFrameToTrack ( FFrame<T> inNewFrame, FTrack* inParentTrack );
	virtual bool AddTrackToTimeline ( FTrack<T> inNewTrack );
	virtual bool RemoveTrackFromTimeline ( long long inFrameNumber, FTrack<T>& outRemovedFrame ) = false;
	virtual bool RemoveFrameFromTrack ( long long inFrameNumber, FTrack<T>& outRemovedFrame ) = false;
	virtual bool ClearTimeline () = false;
	virtual bool ClearTrack ( char* inTrackName ) = { return ClearTrack ( TrackArray[inTrackName] ); };
	virtual bool ClearTrack ( FTrack<T>* ) = false;
	virtual FFrame<T>* GetFrame ( long long inFrameNumber, FTrack* inParentTrack ) = nullptr;
	virtual bool AddCallToFrame ( long long FrameNumber, T& inAPICall ) = false;
	virtual FFrame<T> GetCurrentFrame () = nullptr;
	virtual FFrame<T>* NextFrame () = nullptr;
	virtual FFrame<T>* PreviousFrame () = nullptr;

	bool LoadTimelineFromProfilingData ( string ProfilingDataFilePath );


private:
	map<char*, FTrack<T>*> TrackArray;
	FFrame<T>* CurrentFrame;
	vector<FFrame<T>*> CurrentFrames;
	bool isInitialized = false;
};
