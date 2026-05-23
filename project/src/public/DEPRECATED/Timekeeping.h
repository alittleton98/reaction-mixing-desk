// Timekeeping classes and functions
#pragma once
#include <vector>
#include <chrono>
class TickingObject
{
public:
	virtual bool Tick ( float deltaTime ) = 0;
};

class Clock
{
public:

	Clock ();
	~Clock ();
	// Objects added here will have their Tick functions run
	bool SubscribeToClock ( TickingObject* inTickingObject );
	// This function does the preperation for the clock to know the delta and perform any operations necessary to the other Tick functions
	bool Tick ();
	// iterates through each object and calls their respective Tick functions
	bool Tock ();

	// delay in milliseconds for the thread to wait
	float manualDelay = 0.f;
	vector<TickingObject*> tickingObjects;

private:
	float deltaTime;
	chrono::time_point<chrono::steady_clock> lastFrameTime;

};

