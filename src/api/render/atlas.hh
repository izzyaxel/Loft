#pragma once

#include "texture.hh"

#include <commons/math/vec2.hh>
#include <vector>
#include <unordered_map>

struct QuadUVs
{
	vec2<float> upperLeft{}, lowerLeft{}, upperRight{}, lowerRight{};
};

/// A texture atlas for sprites, atlasDims x AtlasDims large, RGBA8, uses 64 MiB of VRAM per atlas
struct Atlas
{
	/// Add a new tile into this atlas
	/// \param name The name of the tile
	/// \param tileData Flat array of pixel data
	/// \param width The width of the new tile
	/// \param height The height of the new tile
	void addTile(std::string const &name, std::vector<uint8_t> const &tileData);
	
	/// Add a new tile into this atlas from raw pixel data
	void addTile(std::string const &name, ColorFormat fmt, std::vector<uint8_t> &&tileData, uint32_t width, uint32_t height);
	
	/// Get the UV coordinates in the atlas for the given tile
	/// \param id The ID of the tile
	/// \return UV coordinates
	[[nodiscard]] QuadUVs getUVsForTile(std::string const &name);
	
	[[nodiscard]] vec2<double> getTileDimensions(std::string const &name);
	
	/// Bind this atlas for rendering use
	void use(uint32_t target = 0);
	
	[[nodiscard]] uint32_t getHandle();
	
	/// Check if this atlas contains a tile of the given name
	[[nodiscard]] bool contains(std::string const &tileName);
	
	/// Create the atlas and send it to the GPU
	void finalize(ColorFormat fmt);
	
	/// Save the atlas from the GPU to a PNG file
	void dump(std::string const &path);

private:
	struct AtlasImg
	{
		AtlasImg() = default;
		AtlasImg(std::string const &name, std::vector<uint8_t> &&data, ColorFormat fmt, vec2<uint32_t> location, uint32_t width, uint32_t height) : name(name), data(data), fmt(fmt), location(location), width(width), height(height) {}
		
		static inline bool comparator(AtlasImg const &a, AtlasImg const &b)
		{
			return a.height * a.width > b.height * b.width;
		}
		
		std::string name;
		std::vector<uint8_t> data{};
		ColorFormat fmt = ColorFormat::RGBA;
		vec2<uint32_t> location{};
		uint32_t width = 0, height = 0;
	};
	
	vec2<float> atlasDims{};
	std::vector<AtlasImg> atlas;
	uint64_t texID;
	bool finalized = false;
};
