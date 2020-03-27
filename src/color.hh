#pragma once

#include <commons/math/vec3.hh>
#include <commons/math/vec4.hh>
#include <string>
#include <cstdint>

struct Color
{
	using iFmt = uint16_t; //Internal color storage type, makes it easy to change, ie to uint32_t if needed
	
	Color() = default;
	
	Color operator + (Color const &other);
	
	bool operator == (Color const &other);
	
	void fromRGBf(float r, float g, float b);
	void fromRGBAf(float r, float g, float b, float a);
	void fromRGBui8(uint8_t r, uint8_t g, uint8_t b);
	void fromRGBAui8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
	void fromRGBui16(uint16_t r, uint16_t g, uint16_t b);
	void fromRGBAui16(uint16_t r, uint16_t g, uint16_t b, uint16_t a);
	void fromHex(uint32_t hex);
	void fromWeb(std::string const &color);
	
	/// 0-1 float RGB representation
	[[nodiscard]] vec3<float> asRGBf() const;
	
	/// 0-1 float RGBA representation
	[[nodiscard]] vec4<float> asRGBAf() const;
	
	/// 0-255 RGB representation
	[[nodiscard]] vec3<uint8_t> asRGBui8() const;
	
	/// 0-255 RGBA representation
	[[nodiscard]] vec4<uint8_t> asRGBAui8() const;
	
	/// 0-65535 RGB representation
	[[nodiscard]] vec3<uint16_t> asRGBui16() const;
	
	/// 0-65535 RGBA representation
	[[nodiscard]] vec4<uint16_t> asRGBAui16() const;
	
	/// 0xAARRGGBB representation
	[[nodiscard]] uint32_t asHex() const;
	
	/// #RRGGBBAA representation
	[[nodiscard]] std::string asWeb() const;

protected:
	iFmt r = 0, g = 0, b = 0, a = 1;
};
