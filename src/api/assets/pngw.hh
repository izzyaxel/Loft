#pragma once

#include <string>
#include <cstdint>
#include <vector>

struct PNG
{
	static int32_t constexpr COLOR_FMT_GREY = 0;
	static int32_t constexpr COLOR_FMT_PALETTE = 2 | 1;
	static int32_t constexpr COLOR_FMT_RGB = 2;
	static int32_t constexpr COLOR_FMT_RGBA = 2 | 4;
	static int32_t constexpr COLOR_FMT_GREY_ALPHA = 4;
	
	PNG(uint32_t width, uint32_t height, char colorFormat, char bitDepth, std::vector<uint8_t> &&imageData);
	
	///The width and height of the image in pixels
	uint32_t width, height;
	
	///The color format and depth width the decoded data is in
	char colorFormat, bitDepth;
	
	/// The decoded pixel data
	std::vector<uint8_t> imageData;
};

/// Read and decode a PNG image
[[nodiscard]] PNG decodePNG(std::string const &filePath);

/// Decode a PNG image
[[nodiscard]] PNG decodePNG(std::vector<uint8_t> const &file);

/// Write a PNG to disk from a flat array of pixel data
/// @param reverseRows Reverse the order of the pixel rows, useful for correcting images obtained with OpenGL
void writePNG(std::string const &filePath, uint32_t width, uint32_t height, uint8_t *imageData, int32_t fmt, bool reverseRows = false);
