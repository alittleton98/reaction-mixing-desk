// Timekeeping classes and functions
#pragma once
class WordClock
{
	float manualDelay = 0.f;

	bool Tick()
	{

	}
};

class TickingObject
{
public:
	virtual bool Tick(float deltaTime) { return false; };
};