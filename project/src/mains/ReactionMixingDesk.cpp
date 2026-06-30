#include <windows.h>
#include "resources/resource.h"
#include "common.h"

int APIENTRY wWinMain( _In_ HINSTANCE instance, _In_opt_ HINSTANCE /*prevInstance*/,
	_In_ LPWSTR lpCmdLine, _In_ int /*nCmdShow*/ )
{
	AllocConsole();
	FILE* fp;
	freopen_s( &fp, "CONOUT$", "w", stdout );
	freopen_s( &fp, "CONOUT$", "w", stderr );
	freopen_s( &fp, "CONIN$", "r", stdin );
	printf( "Hello World!\n" );

	string message = "";
	while ( message != "exit" )
	{
		printf( "Reaction Mixing Desk: " );
		getline( cin, message );
		printf( "%s\n", message.c_str() );
	}

	this_thread::sleep_for( chrono::milliseconds( 500 ) );

	return 0;
}