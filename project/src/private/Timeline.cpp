#include "Timeline.h"
#include <fstream>
#include <sstream>

FTimeline::FTimeline()
{
	CurrentFrame = nullptr;
	totalFrames = DEFAULT_TIMELINE_LENGTH;
	

	
}

FTimeline::~FTimeline()
{
	Cleanup();
	free(FrameArray.data());
	free(CurrentFrame);
}

bool FTimeline::Init()
{
	if (isInitialized)
	{
		if (!Cleanup())
			return false;
	}

	for (int i = 0; i < totalFrames; i++)
	{
		FFrame NewFrame;
		NewFrame.FrameNumber = i;
		NewFrame.WwiseAPICalls = {};
		NewFrame.PreviousFrame = (i > 0) ? &FrameArray[i - 1] : nullptr;
		NewFrame.NextFrame = nullptr;
		if (i > 0)
		{
			FrameArray[i - 1].NextFrame = &NewFrame;
		}
		FrameArray.push_back(NewFrame);
	}
	totalFrames = FrameArray.size();

	if (FrameArray.size() > 0)
	{
		CurrentFrame = &FrameArray[0];
	}

	isInitialized = true;
}

bool FTimeline::Cleanup()
{
	if (!isInitialized)
	{
		return false;
	}
	FrameArray.clear();
	CurrentFrame = nullptr;
	isInitialized = false;
	return true;
}

bool FTimeline::AddFrameToTimeline(FFrame inNewFrame)
{
	if (!isInitialized)
	{
		return false;
	}

	FrameArray.push_back(inNewFrame);
}

bool FTimeline::RemoveFrameFromTimeline(long long inFrameNumber, FFrame& outRemovedFrame)
{
	if (!isInitialized)
	{
		return false;
	}

	outRemovedFrame = FrameArray[inFrameNumber];
	FrameArray.erase(FrameArray.begin() + inFrameNumber);
}

bool FTimeline::ClearTimeline()
{
	if (!isInitialized)
	{
		return false;
	}

	FrameArray.clear();
}

FFrame* FTimeline::GetFrame(long long inFrameNumber)
{
	if (!isInitialized)
	{
		return nullptr;
	}

	FFrame* FoundFrame = nullptr;
	FoundFrame = &FrameArray[inFrameNumber];
	return FoundFrame;
}

bool FTimeline::AddCallToFrame(long long FrameNumber, WwiseAPICall& inAPICall)
{
	if (!isInitialized)
	{
		return false;
	}

	FrameArray[FrameNumber].WwiseAPICalls.push_back(inAPICall);
}

FFrame* FTimeline::GetCurrentFrame()
{
	if (!isInitialized)
	{
		return nullptr;
	}

	return CurrentFrame;
}

FFrame* FTimeline::NextFrame()
{
	if (!isInitialized)
	{
		return false;
	}

	CurrentFrame = CurrentFrame->NextFrame;
	return CurrentFrame;
}

FFrame* FTimeline::PreviousFrame()
{
	if (!isInitialized)
	{
		return false;
	}

	CurrentFrame = CurrentFrame->PreviousFrame;	
	return CurrentFrame;
}

bool FTimeline::LoadTimelineFromProfilingData(string ProfilingDataFilePath)
{
	if (!isInitialized)
	{
		return false;
	}


	char delimiter = '\t';

	ifstream ProfilingDataFile(ProfilingDataFilePath);
	if (!ProfilingDataFile.is_open())
	{
		return false;
	}	

	string line;
	while(getline(ProfilingDataFile, line))
	{
		vector<string> tokens;
		stringstream ss(line);
		string token;
		while (getline(ss, token, delimiter))
		{
			tokens.push_back(token);
		}
		if (tokens.size() >= 4)
		{
			long long FrameNumber = stoll(tokens[0]);
			string Description = tokens[1];
			string GameObjectName = tokens[2];
			uint32 GameObjectID = stoul(tokens[3]);
			vector<string> Parameters(tokens.begin() + 4, tokens.end());
			WwiseAPICall NewAPICall(Description, GameObjectName, GameObjectID, Parameters);
			AddCallToFrame(FrameNumber, NewAPICall);
		}
	}

}