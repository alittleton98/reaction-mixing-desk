// Class that traverses a timeline and performs timekeeping while active
#pragma once
#include "Common.h"
#include "Timekeeping.h"
#include "Timeline.h"

class FTransport : public TickingObject
{
public:
	FTransport ();
	~FTransport () = default;

	// Initialize the transport for the session
	bool Init ();

	// logic for processing a frame within the buffer and performing the necessary actions for the frame
	bool Tick () override;

	// Cleanup the transport and prepare for shutdown
	bool Cleanup ();

	// Start processing of frames within the timeline
	bool Play ();

	// Stop processing of frames within the timeline, but maintain the current position in the timeline for resumption
	bool Pause ();

	// Stop processing of frames within the timeline and reset the current position to the beginning of the timeline
	bool Stop ();


private:
	FTimeline* Timeline;
	vector<vector<FFrame*>> FrameBuffer;
	bool bIsPlaying = false;


	long long CurrentFrameNumber = 0;
};