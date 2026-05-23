#include "Audio/WwiseSoundEngine.h"
#include <iostream>
#include <thread>

//int main()
//{
//	if ( !InitializeWwiseSoundEngine() )
//	{
//		assert( false && "Failed to initialize Wwise Sound Engine" );
//		return 1;
//	}
//	else
//	{
//		printf( "Wwise Sound Engine initiated successfully.\n" );
//		g_WwiseIOHook.SetBasePath( AKTEXT( "E:\\Dev\\ReactionDigital\\UnrealEngine\\Nexus\\Wwise\\Nexus\\GeneratedSoundBanks\\Windows\\" ) );
//		AkBankID bankID;
//		AK::SoundEngine::LoadBank( "Init.bnk", bankID );
//		AK::SoundEngine::RegisterGameObj( 0, "GlobalListener" );
//		AK::SoundEngine::AddDefaultListener( 0 );
//		AK::SoundEngine::RegisterGameObj( 1, "GlobalEmitter" );
//		AkOutputSettings outputSettings;
//		AK::SoundEngine::AddOutput( outputSettings );
//		std::this_thread::sleep_for( std::chrono::seconds( 30 ) );
//		AK::SoundEngine::LoadBank( "Event\\Play_SpeakerCheck.bnk", bankID );
//
//		if ( bankID == AK_INVALID_BANK_ID )
//		{
//			printf( "Failed to load Play_SpeakerCheck.bnk\n" );
//		}
//		else
//		{
//			printf( "Play_SpeakerCheck.bnk loaded successfully with Bank ID: %u\n", bankID );
//			AK::SoundEngine::PostEvent( "Play_SpeakerCheck", 1 );
//			AK::SoundEngine::RenderAudio();
//		}
//		std::this_thread::sleep_for( std::chrono::seconds( 60 ) );
//		TerminateWwiseSoundEngine();
//		printf( "Wwise Sound Engine terminated successfully.\n" );
//		return 0;
//	}
//}
