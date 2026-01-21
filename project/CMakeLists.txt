# CMakeList.txt : CMake project for project, include source and define
# project specific logic here.
#

# Version setup
cmake_minimum_required (VERSION 3.10)
if (DEFINED ENV{WWISEVERSION})
  set (WWISE_SDK_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
else()
  message(FATAL_ERROR "Environment variable WWISEVERSION is required")
endif()

if (NOT DEFINED ENV{REAPERPLUGINPATH})
	message(FATAL_ERROR "Environment variable REAPERPLUGINPATH not defined")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("reaction-digital-mixing-desk")

# Wwise Sound Engine Host is meant for separately testing the Sound Engine integration. It's not required for the plugin to work in REAPER.
add_executable (WwiseSoundEngineHost 
"src/private/WwiseSoundEngineHost.cpp" 
"src/public/WwiseSoundEngine.h"
)

# Reaction Mixing Desk Plugin for REAPER. This is the ultimate target and intended to be used within REAPER as an extension.
add_library (reaper_reaction-mixing-desk SHARED 
"src/private/ReactionMixingDesk.cpp" 
"src/public/ReactionMixingDesk.h"
"src/public/MixingDeskCommands.h"
"src/public/WAAPISynchronization.h"
)

# Include directories
target_include_directories(reaper_reaction-mixing-desk PUBLIC "${WWISE_SDK_PATH}/include" "include/wwise" "include/reaper-sdk" "include/reaper-sdk/sdk" "src/public" )
target_include_directories(WwiseSoundEngineHost PUBLIC "${WWISE_SDK_PATH}/include" "include/public" "src/public")

# Global Definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(reaper_reaction-mixing-desk PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
	target_compile_definitions(WwiseSoundEngineHost PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(reaper_reaction-mixing-desk PRIVATE RELEASE_CONFIG=1 DEBUG_CONFIG=0)
	target_compile_definitions(WwiseSoundEngineHost PRIVATE RELEASE_CONFIG=1 DEBUG_CONFIG=0)
else()
	message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
endif()

target_compile_definitions(reaper_reaction-mixing-desk PRIVATE WWISESDKVERSION="$ENV{WWISEVERSION}")
target_compile_definitions(WwiseSoundEngineHost PRIVATE WWISESDKVERSION="$ENV{WWISEVERSION}")

# Set output binary names
set_target_properties(reaper_reaction-mixing-desk PROPERTIES OUTPUT_NAME "reaper_reaction-mixing-desk-$ENV{WWISEVERSION}" PREFIX "")

# Link Libararies
if (WIN32)
	# 64-bit
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Debug/lib/*.lib")
			foreach (libfile IN LISTS WWISE_LIBS)
				message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
			endforeach()
			target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
			target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
		# Use Profile configuration because we need to be able to communicate with the Sound Engine
		elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Profile/lib/*.lib")
			target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
			target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
		else()
			message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
		endif()
	else()
		message(FATAL_ERROR "Unsupported architecture. Chaotic pointer size")
	endif()
elseif(APPLE)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/Mac_Xcode1600/Debug/lib/*.a")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
		target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
	elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/Mac_Xcode1600/Debug/lib/*.a")
		target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
		target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
	endif()
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Release")
	add_custom_command(TARGET reaper_reaction-mixing-desk POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:reaper_reaction-mixing-desk> "$ENV{REAPERPLUGINPATH}/$<TARGET_FILE_NAME:reaper_reaction-mixing-desk>"
	)
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WwiseSoundEngineHost PROPERTY CXX_STANDARD 20)
endif()
