# CMakeList.txt : CMake project for project, include source and define
# React
#
cmake_minimum_required (VERSION 3.10)
include(FetchContent)
find_package(Git QUIET)

if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    set(GIT_COMMIT_COUNT "0")
    set(GIT_COMMIT_HASH "unknown")
endif()

set(RMD_MAJOR_NUMBER "0")
set(RMD_MINOR_NUMBER "1")
set(RMD_BUILD_NUMBER ${GIT_COMMIT_COUNT})

add_compile_definitions(RMD_MAJOR_NUMBER=0)
add_compile_definitions(RMD_MINOR_NUMBER=1)
add_compile_definitions(RMD_BUILD_NUMBER=${GIT_COMMIT_COUNT})
add_compile_definitions(RMD_VERSION_STRING="${RMD_MAJOR_NUMBER}.${RMD_MINOR_NUMBER}.${RMD_BUILD_NUMBER}")
add_compile_definitions(ASIO_DEVICE_NAME="VB-Matrix VASIO-512")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("reaction-mixing-desk")

if (DEFINED ENV{WWISEVERSION})
  set (WWISE_SDK_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
else()
  message(FATAL_ERROR "Environment variable WWISEVERSION is required")
endif()


file(GLOB_RECURSE WWISE_INTEGRATION_SOURCES
    "${WWISE_SDK_PATH}/samples/SoundEngine/Common/*.cpp"
)

if (WIN32)
file(GLOB_RECURSE WWISE_PLATFORM_SOURCES
    "${WWISE_SDK_PATH}/samples/SoundEngine/Win32/*.cpp"  # or Mac, depending on platform
)
else()
file(GLOB_RECURSE WWISE_PLATFORM_SOURCES
	"${WWISE_SDK_PATH}/samples/SoundEngine/POSIX/*.cpp"  # or Mac, depending on platform
)
endif()

# Dependencies
FetchContent_Declare( 
	asio
	GIT_REPOSITORY	https://github.com/audiosdk/asio.git
	GIT_TAG			main
)
FetchContent_MakeAvailable(asio)

FetchContent_Declare( 
	rtmidi
	GIT_REPOSITORY	https://github.com/thestk/rtmidi.git
	GIT_TAG			master
)
FetchContent_MakeAvailable(rtmidi)

FetchContent_Declare( 
	vst3sdk
	GIT_REPOSITORY	https://github.com/steinbergmedia/vst3sdk.git
	GIT_TAG			master
)
FetchContent_MakeAvailable(vst3sdk)

FetchContent_Declare( 
	simpleini
	GIT_REPOSITORY	https://github.com/brofield/simpleini.git
	GIT_TAG			master
)
FetchContent_MakeAvailable(simpleini)

FetchContent_Declare( 
	rapidjson
	GIT_REPOSITORY	https://github.com/Tencent/rapidjson.git
	GIT_TAG			master
)
FetchContent_MakeAvailable(rapidjson)

# MAIN REACTION MIXING DESK APPLICATION

file(GLOB_RECURSE REACTIONMIXINGDESK_SOURCE
"src/controlsurface/*cpp"
"src/soundengine/*cpp"
"src/timeline/*cpp"
"src/wwise-host/*cpp"
)

file(GLOB_RECURSE REACTIONMIXINGDESK_HEADER
"src/controlsurface/*h"
"src/soundengine/*h"
"src/timeline/*h"
"src/wwise-host/*h"
"src/*h"
)

file(GLOB_RECURSE ASIOSDK_SOURCE
"${asio_SOURCE_DIR}/host/*.cpp"
"${asio_SOURCE_DIR}/host/pc/*.cpp"
"${asio_SOURCE_DIR}/common/asio.cpp"
)

add_executable (ReactionMixingDesk "src/mains/ReactionMixingDesk.cpp" "src/resources/reaction-mixing-desk.rc" ${REACTIONMIXINGDESK_HEADER} ${REACTIONMIXINGDESK_SOURCE} ${ASIOSDK_SOURCE})

target_include_directories(ReactionMixingDesk PUBLIC 
"${WWISE_SDK_PATH}/include" 
"${WWISE_SDK_PATH}/samples" 
 "${CMAKE_CURRENT_SOURCE_DIR}/src"
 "${asio_SOURCE_DIR}/common"
 "${asio_SOURCE_DIR}/host"
 "${rtmidi_SOURCE_DIR}"
 "${simpleini_SOURCE_DIR}"
 "${vst3sdk_SOURCE_DIR}"
 "${rapidjson_SOURCE_DIR}/include"
)

target_link_libraries(ReactionMixingDesk PRIVATE
    sdk_hosting
    base
    pluginterfaces
	sdk_common
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ReactionMixingDesk PROPERTY CXX_STANDARD 20)
endif()

# Global Definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(ReactionMixingDesk PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(ReactionMixingDesk PRIVATE DEBUG_CONFIG=0 RELEASE_CONFIG=1)
else()
	message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Debug/lib/*.lib")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(ReactionMixingDesk PRIVATE ${WWISE_LIBS} ws2_32)
	# Use Profile configuration because we need to be able to communicate with the Sound Engine
	elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Profile/lib/*.lib")
		target_link_libraries(ReactionMixingDesk PRIVATE ${WWISE_LIBS} ws2_32)
	else()
		message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
	endif()
else()
	message(FATAL_ERROR "Unsupported architecture. Chaotic pointer size")
endif()
target_compile_definitions(ReactionMixingDesk PRIVATE IEEE754_64FLOAT=1)


configure_file(
    ${CMAKE_SOURCE_DIR}/config/settings.ini
    ${CMAKE_BINARY_DIR}/settings.ini
    COPYONLY
)
