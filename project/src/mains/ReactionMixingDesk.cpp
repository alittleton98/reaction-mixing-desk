#include <windows.h>
#include "resources/resource.h"
#include "common.h"
#include "soundengine/ReactionSoundEngine.h"
int main()
//int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
//	_In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
	/*HWND consoleWindow;

	AllocConsole();
	SetConsoleTitleW( L"Reaction Mixing Desk CLI" );
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );*/
	printf( "Reaction Mixing Desk Command Line Interface \n" );
	printf( "Version: %s \n", RMD_VERSION_STRING );

	ReactionSoundEngine* soundEngine = ReactionSoundEngine::Get();
	if ( !soundEngine )
	{
		printf( "Sound Engine ptr returned null \n" );
		return 1;
	}
	if ( !soundEngine->InitializeASIODevice() )
	{
		printf( "Sound Engine failed to intialized asio device \n" );

	}


	string message = "";
	while ( message != "exit" )
	{
		getline( cin, message );
	}

	this_thread::sleep_for( chrono::milliseconds( 500 ) );

	return 0;
}