#pragma once
#include "Common.h"

namespace WwiseAPICalls
{
	// Base structure for a Wwise API call,
	struct WwiseAPICall
	{
		string Description;
		string GameObjectName;
		uint32 GameObjectID;
		std::vector<std::string> Parameters;

		WwiseAPICall() : Description( "" ), GameObjectName( "" ), GameObjectID( 0 ), Parameters( {} ) {}
		WwiseAPICall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters ) :
			Description( inDescription ), GameObjectName( inGameObjectName ), GameObjectID( inGameObjectID ), Parameters( inParameters ) {
		}
	};

	struct RegisterGameObject : WwiseAPICall
	{
		Description = "RegisterGameObject";
		RegisterGameObjectCall() : WwiseAPICall() {}
		RegisterGameObjectCall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters ) :
			WwiseAPICall( inDescription, inGameObjectName, inGameObjectID, inParameters ) {
		}
	};

	struct UnregisterGameObject : WwiseAPICall
	{
		Description = "UnregisterGameObject";
		UnregisterGameObjectCall() : WwiseAPICall() {}
		UnregisterGameObjectCall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters ) :
			WwiseAPICall( inDescription, inGameObjectName, inGameObjectID, inParameters ) {
		}
	};

	struct Set

	namespace SpatialAudio
	{
		// Structure for the subset of Wwise API calls that are related to spatial audio, which have some additional properties that are relevant to the spatial audio system and may be used for more specific handling during timeline playback
		struct WwiseSpatialAudioAPICall : WwiseAPICall
		{
			string SpatialAudioFunction;

			WwiseSpatialAudioAPICall() : WwiseAPICall(), SpatialAudioFunction( "" ) {}
			WwiseSpatialAudioAPICall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters, string inSpatialAudioFunction ) :
				WwiseAPICall( inDescription, inGameObjectName, inGameObjectID, inParameters ), SpatialAudioFunction( inSpatialAudioFunction ) {
			}
		};

		struct SetGameObjectRadius : WwiseSpatialAudioAPICall
		{
			float OuterRadius;
			float InnerRadius;

			SetGameObjectRadiusCall() : WwiseSpatialAudioAPICall() {
				Description = "SetGameObjectRadius";
				SpatialAudioFunction = "SetGameObjectRadius";
			}
			SetGameObjectRadiusCall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters ) :
				WwiseSpatialAudioAPICall( inDescription, inGameObjectName, inGameObjectID, inParameters, "SetGameObjectRadius" ) {
			}
		};

		struct SetPosition : WwiseSpatialAudioAPICall
		{
			float PositionX;
			float PositionY;
			float PositionZ;
			SetGameObjectPositionCall() : WwiseSpatialAudioAPICall() {
				Description = "SetGameObjectPosition";
				SpatialAudioFunction = "SetGameObjectPosition";
			}
			SetGameObjectPositionCall( string inDescription, string inGameObjectName, uint32 inGameObjectID, vector<string> inParameters ) :
				WwiseSpatialAudioAPICall( inDescription, inGameObjectName, inGameObjectID, inParameters, "SetGameObjectPosition" ) {
			}
	}
	
}