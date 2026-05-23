// Used for Serializing Data to a particular file type
#pragma once
#include "Common.h"

// Class for writing data to a file, with support for different file types and serialization formats
class FFileWriter
{

}

// Class for writing data to a CSV file, with support for serializing data in a tabular format
class FCSVFileWriter : public FFileWriter
{

}

// Class for writing data to a JSON file, with support for serializing data in a hierarchical format
class FJSONFileWriter : public FFileWriter
{

}

// Class for writing data to an XML file, with support for serializing data in a hierarchical format with custom tags and attributes
class FXMLFileWriter : public FFileWriter
{
}

// Class for writing data to a custom file format specific to the ReactionMixingDesk application, with support for serializing data in a format optimized for the application's needs
class FReactionFileWriter : public FFileWriter
{
}

// Class for writing audio data to a WAV file, with support for serializing audio data in the WAV file format and handling the necessary headers and metadata
class FWaveFileWriter : public FFileWriter
{

}