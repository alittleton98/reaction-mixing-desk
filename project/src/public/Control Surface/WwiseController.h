// Class for setting up the controller for Wwise Authoring
#pragma once
#include "Ak/WwiseAuthoringAPI/waapi.h"

using namespace AK;

class WwiseController
{
public:
	bool ConnectToWwiseAuthoring( string IP = "127.0.0.1", int port = 8080 );

private:
	WwiseAuthoringAPI::Client waapiClient;
	WwiseAuthoringAPI::AkJson wwiseInfo;
};



