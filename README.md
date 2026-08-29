# Reaction Mixing System

The Reaction Mixing System is an all digital studio application suite designed to grant extensive control over the mix process for realtime interactive experiences. RMS is primarily built for the video game industry, but can be utilized for other real time experiences with your authoring tool/engine of choice.

The mixing desk application is built with the SSL UF Series Control Surface hardware. The Desk is purpose built for mixing in the box  for your chosen sound engine and operating as a digital version of an analog mixing Desk. Input for the desk is from shared memory pages that are loaded from audio processing plugins of different formats.

Reaction Mixing Desk runs both as a locally integrated system or as a client to a running instance of the Reaction Audio Server.&#x20;

The Reaction Mixing System has 3 main parts

1. Mixing Desk - The primary client application and audio channel mixing desk. The Mixing Desk utilizes the control surface hardware to control the necessary authoring and playback tools. The desk's audio system utilizes an ASIO device for IO, but allows for loading a mix graph. The Mixing Desk can be run offline using shared memory or online as a Master/Slave to a running Audio Server.&#x20;
2. Audio Server - The server application that controls the transport and property information for each client of the mixing desk. The server is not authoritative on its own, but is instead essentially a broadcast aggregate for the Slave clients and an interpreter for the Master. The server is not required when running the client locally
3. Audio Plugin - The audio plugin sends or receives data from the desired audio applications. The Mixing System does not make sound on its own so it requires a source. The audio plugin is the source of any audio data used by the mixing system. It is also capable of receiving endpoint data from the Mixing Desk's mix graph as well.&#x20;

If the Desk is spun up in offline mode, a Server is not launched and the Mixing Desk is expected to connect to any other applications/sources via LAN or shared memory.

