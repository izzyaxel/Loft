#pragma once

// Simple model storage format
//
// Magic char[3]
// Num ToC Entries uint64_t
// Offset to ToC from beginning of file uint64_t
//
// Data blob
//
// ToC Entry:
// Offset to entry from start of blob uint64_t
// Model name length uint16_t
// Model name char const*
// Num Verticies uint64_t
// Num UVs uint64_t
// Num Normals uint64_t

#include <memory>
#include <vector>
#include <functional>

struct MeshData
{
	std::vector<float> vertElements, uvElements, normalElements;
	uint64_t numVertElements = 0, numUVElements = 0, numNormalElements = 0;
	std::string modelName = "";
};

struct MeshEntry
{
	uint64_t offsetIntoData = 0;
	uint16_t modelNameLen = 0;
	std::string modelName = "";
	uint64_t numVertElements = 0, numUVElements = 0, numNormalElements = 0; //How many float values each contains
};

struct MeshFile
{
	/// Parse a mesh from raw data (ie in-memory from an archive)
	/// \param data
	[[nodiscard]] static std::unique_ptr<MeshFile> open(std::vector<uint8_t> const &data);
	
	/// Parse a mesh from a file on disk
	/// \param filePath
	[[nodiscard]] static std::unique_ptr<MeshFile> open(std::string const &filePath); //TODO implement
	
	/// 
	/// \param modelName
	[[nodiscard]] std::shared_ptr<MeshEntry> find(std::string const &modelName);
	
	/// 
	/// \param modelName
	[[nodiscard]] MeshData read(std::string const &modelName);
	
	/// 
	/// \param tocEntry
	[[nodiscard]] MeshData read(std::shared_ptr<MeshEntry> const &tocEntry);
	
	/// 
	/// \param outPath 
	/// \param modelData 
	static void write(std::string const &outPath, std::vector<MeshData> const &modelData);
	
	/// Convert a 3D model format to this one using a callback to process the model
	/// \param outPath Path to the output file
	/// \param conversionFunc A function that reads a model file from disk and converts it to a ModelData struct
	void convert(std::string const &outPath, std::function<void(std::vector<MeshData>&)> const &conversionFunc);
	
	std::vector<uint8_t> data;
	std::vector<std::shared_ptr<MeshEntry>> toc;
};
