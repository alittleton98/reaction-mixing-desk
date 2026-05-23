// Functions and structures for running synchronization with Wwise via WAAPI
#pragma once
#include "AK/WwiseAuthoringAPI/waapi.h"
#include "AK/WwiseAuthoringAPI/AkAutobahn/Client.h"
#include "AK/SoundEngine/Common/AkTypes.h"
#include "AK/SpatialAudio/Common/AkSpatialAudioTypes.h"

struct GameObject_Position
{
	float X;
	float Y;
	float Z;
};

struct GameObject_FrontOrientation
{
	float X;
	float Y;
	float Z;
};

struct GameObject_TopOrientation
{
	float X;
	float Y;
	float Z;
};

struct SpatialObject_Scale
{
	float X;
	float Y;
	float Z;
};
// setup with structures
namespace ReactionMixingDesk
{
	namespace WAAPI
	{
		AK::WwiseAuthoringAPI::Client WAAPIClient;
		
		// TODO : Objects represented are ones that are controlled through Reaper instead of Wwise
		// Structures that represent persistent objects that exist within the project and can have properties updated
		namespace AuthoringObjects
		{
			static struct AuthoringObject
			{
				AkUniqueID ObjectID;
				std::string ObjectName;
			};

			// Represents and Emitter or a Listener in Wwise, which can have spatial audio properties updated
			static struct GameObject : AuthoringObject
			{
				AkGameObjectID GameObjectID;
				std::string GameObjectName;
			};

			// Represents a bus in Wwise, which can have properties updated
			static struct Bus : AuthoringObject
			{
				AkUniqueID BusID;
				float VoiceVolume;
				float BusVolume;
				bool bSolo;
				bool bMute;
				float LowPassFrequency;
				float HighPassFrequency;
				float HighShelfFrequency;
				float HighShelfGain;
				float MidPeakFrequency;
				float MidPeakGain;
				float LowMidFrequency;
				float LowMidGain;
				float LowShelfFrequency;
				float LowShelfGain;
				float CompressorThreshold;
				float CompressorRatio;
				float CompressorAttack;
				float CompressorRelease;
				
			};
		}

		// Structures that represent calls to the API that can be stored and replayed from the capture log. These are meant to be run during playback of the session
		namespace SoundEngineCalls
		{
			static struct SoundEngineAPICall
			{
				AkUniqueID ParameterObjectID;
				std::string ParameterObjectName;
				double Timestamp;
				const char* waapi_command;
			};

			static struct RegisterGameObject : SoundEngineAPICall
			{
				AkGameObjectID GameObjectID;
				
				RegisterGameObject()
				{
					ParameterObjectID = AK_INVALID_PLAYING_ID;
					waapi_command = ak::soundengine::registerGameObj;
				}
				
				// Command to register a game object in Wwise, which will then be used to control properties of that game object
				bool _RegisterGameObject()
				{

				}
			};

			//
			static struct UnregisterGameObject : SoundEngineAPICall
			{
				AkGameObjectID GameObjectID;

				UnregisterGameObject()
				{
					ParameterObjectID = AK_INVALID_PLAYING_ID;
					waapi_command = ak::soundengine::unregisterGameObj;
				}
			};

			static struct PostAudioEvent : SoundEngineAPICall
			{
				AkUniqueID EventID;
				AkGameObjectID GameObjectID;
			};

			static struct SetRTPCValue : SoundEngineAPICall
			{
				AkGameObjectID GameObjectID;
				AkUniqueID RTPCID;
				float Value;
			};

			static struct SetSwitchValue : SoundEngineAPICall
			{
				AkGameObjectID GameObjectID;
				AkUniqueID SwitchGroupID;
				AkUniqueID SwitchStateID;
			};

			static struct SetStateValue : SoundEngineAPICall
			{
				AkUniqueID StateGroupID;
				AkUniqueID StateID;
			};

			static struct AttenuationScalingFactor : SoundEngineAPICall
			{
				float ScalingFactor;
			};

			namespace SpatialAudio
			{
				static struct SetPosition : SoundEngineAPICall
				{
					float Position_X;
					float Position_Y;
					float Position_Z;
					float OrientationFront_X;
					float OrientationFront_Y;
					float OrientationFront_Z;
					float OrientationTop_X;
					float OrientationTop_Y;
					float OrientationTop_Z;
				};

				static struct SetOrientation
				{
					AkGameObjectID GameObjectID;
					float FrontX;
					float FrontY;
					float FrontZ;
					float TopX;
					float TopY;
					float TopZ;
				};

				static struct SetGeometry
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID ListenerID;
					float OcclusionLevel;
				};

				static struct SetGeometryInstance
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID ListenerID;
					float OcclusionLevel;
				};

				static struct SetRoom
				{
					AkRoomID RoomID;
					AkAuxBusID AuxBusID;
					float ReverbLevel;
					float TransmissionLoss;
					float AuxSendLevelToSelf;
					bool KeepRegistered;
					AkGeometryInstanceID GeometryInstanceID;

				};

				static struct SetPortal
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID ListenerID;
					float ObstructionLevel;
					float OcclusionLevel;
				};

				static struct RemovePortal
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID ListenerID;
					float ObstructionLevel;
					float OcclusionLevel;
				};

				static struct SetObjectObstructionAndOcclusion
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID ListenerID;
					float ObstructionLevel;
					float OcclusionLevel;
				};

				static struct SetObjectToPortalObstruction
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID PortalID;
					float ObstructionLevel;
					float OcclusionLevel;
				};

				static struct SetGameObjectRadius
				{
					AkGameObjectID GameObjectID;
					float InnerRadius;
					float OuterRadius;
					float OuterRadiusFalloff;
				};

				static struct SetGameObjectInRoom
				{
					AkGameObjectID GameObjectID;
					AkGameObjectID RoomID;
				};

			}

		}

		// Structures that represent calls to the Wwise Authoring tool that modifying AuthoringObjects
		namespace AuthoringCalls
		{
			static struct AuthoringAPICall
			{
				AkUniqueID ParameterObjectID;
				std::string ParameterObjectName;
				const char* waapi_command;
			};

			// Authoring Tools calls that mimic the properties in 
			namespace RealtimeSync
			{
				
			};

			namespace StateManagementCommands
			{
				
			};

		}

		

		

		

		

		

		

		

		

		namespace ObjectProperties
		{
			static struct Volume
			{
				AkUniqueID BusID;
				float Volume;
			};
		
			static struct SoloObject
			{
				AkUniqueID BusID;
				bool bSolo;
			};

			static struct MuteObject
			{
				AkUniqueID BusID;
				bool bMute;
			};

			static struct SetLowPassFrequencyOnObject
			{
				AkUniqueID BusID;
				float Frequency;
			};

			static struct SetHighPassFrequencyOnObject
			{
				AkUniqueID BusID;
				float Frequency;
			};

			static struct SetHighShelfFrequencyOnObject
			{
				AkUniqueID BusID;
				float Frequency;
			};

			static struct SetHighShelfGainOnObject
			{
				AkUniqueID BusID;
				float Gain;
			};

			static struct SetMidPeakFrequencyOnObject
			{
				AkUniqueID BusID;
				float Frequency;
			};

			static struct SetMidPeakGainOnObject
			{
				AkUniqueID BusID;
				float Gain;
			};

			static struct SetLowMidFrequencyOnObject
			{
				AkUniqueID BusID;
				float Frequency;
			};

			static struct SetLowMidGainOnObject
			{
				AkUniqueID BusID;
				float Gain;
			};

		}

		
	}
}
