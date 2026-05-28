# Reaction Mixing Desk
Mixing Desk plugin for Interactive Sound Engines. Built with Reaper SDK and native integrations for Wwise Sound Engine. 

# Overview
The general goal of the plugin is to be able to efficiently mix Wwise projects without having to run the original software that loads the sound engine. This is inspired by the need to mix multiplayer games without the requirement of multiple players. The project includes two major binaries:
- WwiseSoundEngineHost.exe : A headless Wwise SoundEngine instance that will play through a desired Wwise Profile from end to end. This is primarily for testing purposes to ensure the Sound Engine is running as desired
- reaper_reaction-mixing-desk.dll : The Reaper extension plugin that will spin up an instance of the Wwise Sound Engine and use Reaper as its interactive control software/game engine.
# Installation
## Binaries
Binaries for the project can be retrieved from the Releases section of the Github repository. The project is built primarily for Windows/Mac. Other platforms can be supported on request. Binaries will indicate which Wwise SDK versions the project has been built against.
## Installing from Source
### Requirements
- The ability to open and build a CMake project
- Wwise installation with the SDK
- Environment Variable `WWISEVERSION=<WwiseSDK Version number following format YEAR.MAJOR.MINOR.BUILD> Ex. WWISEVERSION=2025.1.4.9062`
- Environemnt Variable `REAPERPLUGINPATH=<Location of ReaperPlugin Installation>`
### Install
1. Clone repository and build CMake project
2. Compile Release version of reaper_reaction-mixing-desk target
3. binaries should safely install in the desired location and have been linked against the installed/desired WwiseSDK
