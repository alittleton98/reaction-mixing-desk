# CMakeList.txt : CMake project for project, include source and define
# project specific logic here.
#

# Version setup
cmake_minimum_required (VERSION 3.10)
if (DEFINED ENV{WWISEVERSION})
  set (WWISE_SDK_PATH "C:/Program Files (x86)/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
else()
  set(WWISE_SDK_PATH $ENV{WWISESDK}) # Uses default SDK path if no specific version is set
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("reaction-digital-mixing-desk")

# Add source to this project's executable.
add_executable (WwiseSoundEngine "src/private/WwiseSoundEngineHost.cpp" "src/public/WwiseSoundEngineHost.h")
add_library (reaper_reaction-mixing-desk SHARED "src/private/ReactionMixingDesk.cpp" "src/public/MixingDeskCommands.h" "src/public/ReactionMixingDesk.h")

# Include directories
target_include_directories(reaper_reaction-mixing-desk PUBLIC "${WWISE_SDK_PATH}/include" "include/public" "include/reaper-sdk" "include/reaper-sdk/sdk" "src/public")
target_include_directories(WwiseSoundEngine PUBLIC "${WWISE_SDK_PATH}/include" "include/public" "src/public")

# Link Libararies
if (WIN32)
	# 64-bit
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Debug/lib/*.lib")
			foreach (libfile IN LISTS WWISE_LIBS)
				message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
			endforeach()
			target_link_libraries(WwiseSoundEngine PRIVATE ${WWISE_LIBS})
			target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
		# Use Profile configuration because we need to be able to communicate with the Sound Engine
		elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Profile/lib/*.lib")
			foreach (libfile IN LISTS WWISE_LIBS)
				message(STATUS "Linking Profile Wwise Libs to Release configuration: ${libfile}")
			endforeach()
			target_link_libraries(WwiseSoundEngine PRIVATE ${WWISE_LIBS})
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
		target_link_libraries(WwiseSoundEngine PRIVATE ${WWISE_LIBS})
		target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
	elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/Mac_Xcode1600/Debug/lib/*.a")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(WwiseSoundEngine PRIVATE ${WWISE_LIBS})
		target_link_libraries(reaper_reaction-mixing-desk PRIVATE ${WWISE_LIBS})
	endif()
else()
  message(FATAL_ERROR "Unsupported platform")
endif()


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WwiseSoundEngine PROPERTY CXX_STANDARD 20)
endif()
