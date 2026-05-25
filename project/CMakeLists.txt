# CMakeList.txt : CMake project for project, include source and define
# project specific logic here.
#

# Version setup
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.10)
if (DEFINED ENV{WWISEVERSION})
  set (WWISE_SDK_PATH "C:/Program Files/Audiokinetic/Wwise$ENV{WWISEVERSION}/SDK") # Wwise version should fit the format MAJOR.MINOR.PATCH.BUILD. See the AK Launcher for the full version number
else()
  message(FATAL_ERROR "Environment variable WWISEVERSION is required")
endif()

if (NOT DEFINED ENV{REAPERPLUGINPATH})
	message(FATAL_ERROR "Environment variable REAPERPLUGINPATH not defined")
endif()

if (NOT DEFINED ENV{VASIO_512_DRIVER})
	message(WARNING "Environment variable VASIO_512_DRIVER not defined")
endif()

if (NOT DEFINED ENV{VASIO_256_DRIVER})
	message(WARNING "Environment variable VASIO_256_DRIVER not defined")
endif()

if (NOT DEFINED ENV{VASIO_64_DRIVER})
	message(FATAL_ERROR "Environment variable VASIO_64_DRIVER not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_NATIVE_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_NATIVE_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_4000E_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_4000E_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_4000G_CHANNELSTRIP})
	message(FATAL_ERROR "Environment variable VST_SSL_4000G_CHANNELSTRIP not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_NATIVE_BUSCOMPRESSOR})
	message(FATAL_ERROR "Environment variable VST_SSL_NATIVE_BUSCOMPRESSOR not defined")
endif()

if (NOT DEFINED ENV{VST_SSL_METER})
	message(FATAL_ERROR "Environment variable VST_SSL_METER not defined")
endif()

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("reaction-digital-mixing-desk")

file(GLOB_RECURSE REACTIONMIXINGDESK_SOURCE
"src/private/*cpp"
)

file(GLOB_RECURSE ASIOSDK_SOURCE
"include/asiosdk/*cpp"
)

file( GLOB_RECURSE VSTSDK_LIBS
"lib/vst3sdk/pluginterfaces/*.lib"
"lib/vst3sdk/base/*.lib"
"lib/vst3sdk/public.sdk/source/vst/*.lib"
)

file(GLOB_RECURSE VSTSDK_SOURCE
"include/vst3sdk/base/*.cpp"
"include/vst3sdk/public.sdk/source/main/*.cpp"
"include/vst3sdk/public.sdk/source/vst/hosting/*.cpp"
"include/vst3sdk/public.sdk/source/vst/utility/*.cpp"
"include/vst3sdk/public.sdk/source/vst/vstinitiids.cpp"
"include/vst3sdk/public.sdk/source/common/*.cpp"
"include/vst3sdk/pluginterfaces/*.cpp"
)


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

# Wwise Sound Engine Host is meant for separately testing the Sound Engine integration. It's not required for the plugin to work in REAPER.
add_executable (WwiseSoundEngineHost 
"src/WwiseSoundEngineHost.cpp" 
"src/public/DEPRECATED/WwiseSoundEngine.h"
${WWISE_INTEGRATION_SOURCES}
${WWISE_PLATFORM_SOURCES}
)

add_executable (ReactionMixingDesk WIN32
"src/public/ReactionMixingDesk.h"
"src/ReactionMixingDesk.cpp"
"src/public/DEPRECATED/WwiseSoundEngine.h"
${REACTIONMIXINGDESK_SOURCE}
${ASIOSDK_SOURCE}
${VSTSDK_SOURCE}
${WWISE_INTEGRATION_SOURCES}
${WWISE_PLATFORM_SOURCES}
)		



target_include_directories(ReactionMixingDesk PUBLIC "${WWISE_SDK_PATH}/include" 
"${WWISE_SDK_PATH}/samples" "src/public" "include/vst3sdk" "include/asiosdk/common" "include/asiosdk/host" "include")
#target_link_libraries(ReactionMixingDesk PRIVATE "libfftw3-3" "libfftw3f-3" "libfftw3l-3" ws2_32)

target_include_directories(WwiseSoundEngineHost PUBLIC 
"${WWISE_SDK_PATH}/include" 
"${WWISE_SDK_PATH}/samples" 
"include/public" 
"src/public" )

# Global Definitions
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions(WwiseSoundEngineHost PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
	target_compile_definitions(ReactionMixingDesk PRIVATE DEBUG_CONFIG=1 RELEASE_CONFIG=0)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions(WwiseSoundEngineHost PRIVATE RELEASE_CONFIG=1 DEBUG_CONFIG=0)
	target_compile_definitions(ReactionMixingDesk PRIVATE DEBUG_CONFIG=0 RELEASE_CONFIG=1)
else()
	message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
endif()

target_compile_definitions(WwiseSoundEngineHost PRIVATE WWISESDKVERSION="$ENV{WWISEVERSION}")
set( VASIO_512_DRIVER "$ENV{VASIO_512_DRIVER}" )
set( VASIO_256_DRIVER "$ENV{VASIO_256_DRIVER}" )
set( VASIO_64_DRIVER "$ENV{VASIO_64_DRIVER}" )
set( VST_SSL_NATIVE_CHANNELSTRIP "$ENV{VST_SSL_NATIVE_CHANNELSTRIP}" )
set( VST_SSL_4000E_CHANNELSTRIP "$ENV{VST_SSL_4000E_CHANNELSTRIP}" )
set( VST_SSL_4000G_CHANNELSTRIP "$ENV{VST_SSL_4000G_CHANNELSTRIP}" )
set( VST_SSL_NATIVE_BUSCOMPRESSOR "$ENV{VST_SSL_NATIVE_BUSCOMPRESSOR}" )
set( VST_SSL_METER "$ENV{VST_SSL_METER}" )

foreach( VAR IN ITEMS
    VASIO_512_DRIVER
    VASIO_256_DRIVER
    VASIO_64_DRIVER
    VST_SSL_NATIVE_CHANNELSTRIP
    VST_SSL_4000E_CHANNELSTRIP
    VST_SSL_4000G_CHANNELSTRIP
    VST_SSL_NATIVE_BUSCOMPRESSOR
    VST_SSL_METER )
    string( REPLACE "\\" "\\\\" ${VAR} "${${VAR}}" )
endforeach()

target_compile_definitions( ReactionMixingDesk PRIVATE
    VASIO_512_DRIVER="${VASIO_512_DRIVER}"
    VASIO_256_DRIVER="${VASIO_256_DRIVER}"
    VASIO_64_DRIVER="${VASIO_64_DRIVER}"
    VST_SSL_NATIVE_CHANNELSTRIP="${VST_SSL_NATIVE_CHANNELSTRIP}"
    VST_SSL_4000E_CHANNELSTRIP="${VST_SSL_4000E_CHANNELSTRIP}"
    VST_SSL_4000G_CHANNELSTRIP="${VST_SSL_4000G_CHANNELSTRIP}"
    VST_SSL_NATIVE_BUSCOMPRESSOR="${VST_SSL_NATIVE_BUSCOMPRESSOR}"
    VST_SSL_METER="${VST_SSL_METER}"
)
target_link_libraries(ReactionMixingDesk  PRIVATE "${CMAKE_SOURCE_DIR}/lib/rtmidi/rtmidi.lib")
# Link Libararies
if (WIN32)
	# 64-bit
	if(CMAKE_SIZEOF_VOID_P EQUAL 8) 
		if(CMAKE_BUILD_TYPE STREQUAL "Debug")
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Debug/lib/*.lib")
			foreach (libfile IN LISTS WWISE_LIBS)
				message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
			endforeach()
			target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS} ws2_32)
			target_link_libraries(ReactionMixingDesk PRIVATE ${WWISE_LIBS} ws2_32)
		# Use Profile configuration because we need to be able to communicate with the Sound Engine
		elseif(CMAKE_BUILD_TYPE STREQUAL "Release")	
			file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/x64_vc170/Profile/lib/*.lib")
			target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS} ws2_32)
			target_link_libraries(ReactionMixingDesk PRIVATE ${WWISE_LIBS} ws2_32)
		else()
			message(FATAL_ERROR "Unsupported build configuration. Use Debug or Release")
		endif()
	else()
		message(FATAL_ERROR "Unsupported architecture. Chaotic pointer size")
	endif()
	target_compile_definitions(ReactionMixingDesk PRIVATE IEEE754_64FLOAT=1)
elseif(APPLE)
	if (CMAKE_BUILD_TYPE STREQUAL "Debug")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/Mac_Xcode1600/Debug/lib/*.a")
		foreach (libfile IN LISTS WWISE_LIBS)
			message(STATUS "Linking Debug Wwise libs to Debug configuration: ${libfile}")
		endforeach()
		target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
	elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
		file(GLOB WWISE_LIBS "${WWISE_SDK_PATH}/Mac_Xcode1600/Debug/lib/*.a")
		target_link_libraries(WwiseSoundEngineHost PRIVATE ${WWISE_LIBS})
	endif()
else()
  message(FATAL_ERROR "Unsupported platform")
endif()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET WwiseSoundEngineHost PROPERTY CXX_STANDARD 20)
endif()
