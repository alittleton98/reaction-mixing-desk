#include "ReactionMixingDesk.h"
#include <iostream>
#include <thread>
#include <chrono>

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/ )
{
	AllocConsole();
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );

	// Optional: disable buffering so output appears immediately
	setvbuf( stdout, nullptr, _IONBF, 0 );
	setvbuf( stderr, nullptr, _IONBF, 0 );

	gSoundEngine = new SoundEngine();
	if ( !gSoundEngine )
		return 1;

	if ( !gSoundEngine->LoadSoundEngineConfiguration() )
		return 1;

	if ( !gSoundEngine->InitializeASIODevice( EAsioDevice::VASIO_64 ) )
		return 1;

	gSoundEngine->Callbacks.sampleRateDidChange = &OnSampleRateChange;
	gSoundEngine->Callbacks.asioMessage = &OnAsioMessage;
	gSoundEngine->Callbacks.bufferSwitchTimeInfo = &OnBufferSwitchTimeInfo;

	/*if ( !gSoundEngine->InitializeWwiseSoundEngine() )
		return 1;*/

		//gStandardPluginContext = gSoundEngine;
	if ( !gSoundEngine->InitializeSignalChain() )
		return 1;

	MSG msg;
	while ( GetMessage( &msg, nullptr, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	/*gSoundEngine->TerminateSoundEngine();*/
	return 0;
}

void OnSampleRateChange( ASIOSampleRate SampleRate )
{

}

// Asio device buffer switch. Happens every time the buffers are swapped out
ASIOTime* OnBufferSwitchTimeInfo( ASIOTime* Parameters, long DoubleBufferIndex, ASIOBool DirectProcess )
{
	if ( AK::SoundEngine::IsInitialized() )
		AK::SoundEngine::RenderAudio();

	if ( gSoundEngine && gSoundEngine->IsFullyInitialized() )
		gSoundEngine->ProcessSignalChain();

	return Parameters;
}

long OnAsioMessage( long Selector, long Value, void* Message, double* opt )
{
	return 0;
}