#pragma once

#include <cstdint>
#include <vector>

enum struct ColorFormat
{
	RGB, RGBA, GREY
};

enum struct InterpMode
{
	Nearest, Linear
};

struct Texture
{
	Texture() = delete;
	
	/// Allocate VRAM for a texture without assigning data to it
	Texture(uint32_t width, uint32_t height, ColorFormat colorFormat, InterpMode mode = InterpMode::Linear, bool sRGB = false);
	
	/// Create a texture from a flat array
	Texture(uint8_t *data, uint32_t width, uint32_t height, ColorFormat colorFormat, InterpMode mode = InterpMode::Linear, bool sRGB = false);
	
	/// Generates a single color 1x1 texture
	explicit Texture(uint8_t red = 255, uint8_t green = 255, uint8_t blue = 255, uint8_t alpha = 255, bool sRGB = false);
	
	~Texture();
	
	//copy
	Texture(Texture &other);
	Texture& operator=(Texture other);
	
	//move
	Texture(Texture &&other);
	Texture& operator=(Texture &&other);
	
	void use(uint32_t target);
	void setInterpolation(InterpMode min, InterpMode mag);
	void setAnisotropyLevel(uint32_t level);
	void subImage(uint8_t *data, uint32_t w, uint32_t h, uint32_t xPos, uint32_t yPos, ColorFormat format);
	void clear();
	
	uint32_t handle = 0, width = 0, height = 0;
	ColorFormat fmt;
};
