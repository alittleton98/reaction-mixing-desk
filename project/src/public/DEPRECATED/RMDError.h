#pragma once
#include <cstring>
#include <iostream>

// Error Specifications for the ReactionMixingDesk program

using namespace std;
typedef struct RMDError
{
	int code;
	char* title;
	char* description;
}RMDError;

namespace RMDErrors
{
	RMDError NO_ERROR{ 0, "NO ERROR", "Program is runnign as expected" };
	RMDError CAUGHT_EXCEPTION{ 1, "CAUGHT EXCEPTION", "Program caught an exception and force closed." };
};

// Implemented across deferent executables so defined here
void TerminationHandler()
{
	printf("Program shutting down from Error %i\n%s\t%s", RMDErrors::CAUGHT_EXCEPTION.code, RMDErrors::CAUGHT_EXCEPTION.title, RMDErrors::CAUGHT_EXCEPTION.description);
	abort();
}