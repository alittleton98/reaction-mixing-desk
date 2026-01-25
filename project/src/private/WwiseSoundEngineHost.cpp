#include "WwiseSoundEngine.h"
#include <iostream>

int main()
{
	if (!InitializeWwiseSoundEngine())
	{
		assert(false && "Failed to initialize Wwise Sound Engine");
		return 1;
	}
	else
	{
		printf("Wwise Sound Engine initiated successfully.\n");
		TerminateWwiseSoundEngine();
		printf("Wwise Sound Engine terminated successfully.\n");
		return 0;
	}
}
