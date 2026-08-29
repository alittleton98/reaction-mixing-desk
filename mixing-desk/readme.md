# Mixing Desk

The Reaction Mixing Desk is the primary user interface for the the Reaction Mixing System. It is designed for the sound designer and/or engineer to be able to control the sound engine throughout the mixing process. The mixing desk is the heart of the system.&#x20;

## Control Surface

The Control Surface for RMD is built around the Mackie Control Framework and allows for mapping standard MCU commands to the Mixing Desk's control scheme. The Control Surface configuration is designed to link and control software that that can benefit from an expanded control scheme from MCU commands. DAWs may not directly benefit from this expansion as they usually have their own built in mappings than be refined as needed. The most immediate use case are interactive experience authoring tools or other pieces of custom software.&#x20;

The primary test case for the Mixing Desk is the Wwise Authoring Tool.

## Channel Strip

The Channel Strip setup runs the audio processing setup for the Mixing Desk. This is the host application for&#x20;

* ASIO 512 Channel Device
* SSL Channel Strip Plugins
* SSL Mastering Plugins

The Channel Strip model of the Mixing Desk is designed to run like an analog studio desk, but has a Mix Bus capable of up to a 7.1 channel configuration. The Mix bus configuration is designed to work in a similar fashion to traditional analog mixing desks, thus recording the output can create a file directly with whatever the Master bus' channel count is and after the processing from the mastering plugins.

SSL Plugins are the primary VST3s used to build the DSP chain because they are the most stable and ready to be used by the SSL UF Series hardware.
