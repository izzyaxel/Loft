#include "color.hh"
#include "util.hh"
#include "global.hh"

#include <commons/misc.hh>

Color Color::operator+(Color const &other)
{
	Color out;
	out.r = this->r + other.r;
	out.g = this->g + other.g;
	out.b = this->b + other.b;
	out.a = this->a + other.a;
	return out;
}

bool Color::operator==(Color const &other)
{
	return this->r == other.r && this->g == other.g && this->b == other.b && this->a == other.a;
}

void Color::fromRGBf(float r, float g, float b)
{
	this->r = (iFmt)(bound<float>(r, 0.0f, 1.0f) * ui16Max);
	this->g = (iFmt)(bound<float>(g, 0.0f, 1.0f) * ui16Max);
	this->b = (iFmt)(bound<float>(b, 0.0f, 1.0f) * ui16Max);
}

void Color::fromRGBAf(float r, float g, float b, float a)
{
	this->r = (iFmt)(bound<float>(r, 0.0f, 1.0f) * ui16Max);
	this->g = (iFmt)(bound<float>(g, 0.0f, 1.0f) * ui16Max);
	this->b = (iFmt)(bound<float>(b, 0.0f, 1.0f) * ui16Max);
	this->a = (iFmt)(bound<float>(a, 0.0f, 1.0f) * ui16Max);
}

void Color::fromRGBui8(uint8_t r, uint8_t g, uint8_t b)
{
	this->r = (iFmt)(r * 256);
	this->g = (iFmt)(g * 256);
	this->b = (iFmt)(b * 256);
}

void Color::fromRGBAui8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	this->r = (iFmt)(r * 256);
	this->g = (iFmt)(g * 256);
	this->b = (iFmt)(b * 256);
	this->a = (iFmt)(a * 256);
}

void Color::fromRGBui16(uint16_t r, uint16_t g, uint16_t b)
{
	this->r = r;
	this->g = g;
	this->b = b;
}

void Color::fromRGBAui16(uint16_t r, uint16_t g, uint16_t b, uint16_t a)
{
	this->r = r;
	this->g = g;
	this->b = b;
	this->a = a;
}

void Color::fromHex(uint32_t hex)
{
	this->a = (iFmt)(((hex & 0xFF000000) >> 24) * 256);
	this->r = (iFmt)(((hex & 0x00FF0000) >> 16) * 256);
	this->g = (iFmt)(((hex & 0x0000FF00) >> 8) * 256);
	this->b = (iFmt)((hex & 0x000000FF) * 256);
}

void Color::fromWeb(std::string const &color)
{
	if(color.empty() || color[0] != '#')
	{
		logger << Sev::ERR << "Invalid color formatting" << logger.endl();
		return;
	}
	uint32_t conv = (uint32_t)(std::strtol(color.data() + 1, nullptr, 16));
	switch(color.size())
	{
		default: logger << Sev::ERR << "Invalid color formatting" << logger.endl(); break;
		case 4:
			this->b = (iFmt)((conv & 0x00F) * 256);
			this->g = (iFmt)((conv & 0x0F0 << 4) * 256);
			this->r = (iFmt)((conv & 0xF00 << 8) * 256);
			break;
		case 5:
			this->a = (iFmt)((conv & 0x000F) * 256);
			this->b = (iFmt)((conv & 0x00F0 << 4) * 256);
			this->g = (iFmt)((conv & 0x0F00 << 8) * 256);
			this->r = (iFmt)((conv & 0xF000 << 12) * 256);
			break;
		case 7:
			this->r = (iFmt)((conv & 0xFF000000 << 16) * 256);
			this->g = (iFmt)((conv & 0xFF000000 << 8) * 256);
			this->b = (iFmt)((conv & 0xFF000000) * 256);
			break;
		case 9:
			this->r = (iFmt)((conv & 0xFF000000 << 24) * 256);
			this->g = (iFmt)((conv & 0xFF000000 << 16) * 256);
			this->b = (iFmt)((conv & 0xFF000000 << 8) * 256);
			this->a = (iFmt)((conv & 0xFF000000) * 256);
			break;
	}
}

vec3<float> Color::asRGBf() const
{
	return {(float)this->r / ui16Max, (float)this->g / ui16Max, (float)this->b / ui16Max};
}

vec4<float> Color::asRGBAf() const
{
	return {(float)this->r / ui16Max, (float)this->g / ui16Max, (float)this->b / ui16Max, (float)this->a / ui16Max};
}

vec3<uint8_t> Color::asRGBui8() const
{
	return {(uint8_t)(this->r / 256), (uint8_t)(this->g / 256), (uint8_t)(this->b / 256)};
}

vec4<uint8_t> Color::asRGBAui8() const
{
	return {(uint8_t)(this->r / 256), (uint8_t)(this->g / 256), (uint8_t)(this->b / 256), (uint8_t)(this->a / 256)};
}

vec3<uint16_t> Color::asRGBui16() const
{
	return {this->r, this->g, this->b};
}

vec4<uint16_t> Color::asRGBAui16() const
{
	return {this->r, this->g, this->b, this->a};
}

uint32_t Color::asHex() const
{
	return (uint8_t)(this->a / 256) << 24 | (uint8_t)(this->r / 256) << 16 | (uint8_t)(this->g / 256) << 8 | (uint8_t)(this->b / 256);
}

std::string Color::asWeb() const
{
	std::stringstream ss;
	ss << std::hex << ((uint8_t)(this->r / 256) << 24 | (uint8_t)(this->g / 256) << 16 | (uint8_t)(this->b / 256) << 8 | (uint8_t)(this->a / 256));
	return "#" + ss.str();
}
