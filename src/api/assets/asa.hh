#pragma once

//.ASA | Asset Streaming Archive
//A ZSTD backed archival format for streaming game assets

#include <cstdint>
#include <memory>
#include <vector>
#include <array>

constexpr uint8_t const cmpFmtNone = 0;
constexpr uint8_t const cmpFmtZSTD = 1;

struct Header
{
	uint64_t numToCEntries;
	uint64_t tocBeginOffset;
};

struct ASAEntry
{
	uint8_t format;
	uint16_t filenameLen;
	std::string filename;
	uint64_t compressedSize;
	uint64_t decompressedSize;
	uint64_t offset;
};

struct ASA
{
	~ASA();
	
	/// Open a .ASA file for reading
	/// Only the header and table of contents are parsed initially
	/// Use ASA::read() to read and decompress files
	/// \param filepath Fully qualified/absolute path to a .ASA file
	[[nodiscard]] static std::unique_ptr<ASA> open(std::string const &filepath);
	
	/// Compress and archive a collection of files
	/// \param asaFilePath Fully qualified/absolute path to the output .ASA file
	/// \param filePathes List of file pathes to be read, compressed, and archived
	static void write(std::string const &asaFilePath, std::vector<std::string> const &filePathes);
	
	/// Append new files to an existing ASA
	/// \param inputASAFilePath Path to the ASA file
	/// \param filePathes List of file pathes to read, compress, and append to the archive
	static void append(std::string const &inputASAFilePath, std::vector<std::string> const &filePathes);
	
	/// Read and decompress the given file
	/// \param entry The desired file's ToC entry
	/// \return The decompressed file
	[[nodiscard]] std::vector<uint8_t> read(std::shared_ptr<ASAEntry> const &entry);
	
	/// Read and decompress the given file
	/// \param fileName The desired file's filename, including extension
	/// \return The decompressed file
	[[nodiscard]] std::vector<uint8_t> read(std::string const &fileName);
	
	/// Find the ToC entry for the given filename
	/// \param filename The filename to search the ToC for, including extension
	/// \return A pointer to the ToC entry, or nullptr if it couldn't be found
	[[nodiscard]] std::shared_ptr<ASAEntry> find(std::string const &filename);
	
	/// Header information
	Header header{};
	
	/// The table of contents
	std::vector<std::shared_ptr<ASAEntry>> toc;

private:
	ASA() = default;
	FILE *in = nullptr;
	void *decompressor = nullptr;
};
