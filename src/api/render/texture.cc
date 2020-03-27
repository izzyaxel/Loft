#include "texture.hh"

#include <glad/glad.h>

Texture::Texture(uint32_t width, uint32_t height, ColorFormat colorFormat, InterpMode mode, bool sRGB)
{
	this->fmt = colorFormat;
	this->width = width;
	this->height = height;
	glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
	int32_t f = 0;
	if(colorFormat == ColorFormat::RGB) //It's more efficient to use an extra 8 bits of VRAM per pixel
	{
		if(sRGB) f = GL_SRGB8;
		else f = GL_RGB8;
	}
	else if(colorFormat == ColorFormat::RGBA)
	{
		if(sRGB) f = GL_SRGB8_ALPHA8;
		else f = GL_RGBA8;
	}
	else if(colorFormat == ColorFormat::GREY)
	{
		f = GL_R8;
	}
	glTextureStorage2D(this->handle, 1, f, width, height);
	this->clear();
	this->setInterpolation(mode, mode);
	this->setAnisotropyLevel(1);
}

Texture::Texture(uint8_t *data, uint32_t width, uint32_t height, ColorFormat colorFormat, InterpMode mode, bool sRGB)
{
	this->fmt = colorFormat;
	this->width = width;
	this->height = height;
	int32_t f = 0, cf = 0;
	if(colorFormat == ColorFormat::RGB) //TODO It's more efficient to use an extra 8 bits of VRAM per pixel
	{
		if(sRGB) f = GL_SRGB8;
		else f = GL_RGB8;
		cf = GL_RGB;
	}
	else if(colorFormat == ColorFormat::RGBA)
	{
		if(sRGB) f = GL_SRGB8_ALPHA8;
		else f = GL_RGBA8;
		cf = GL_RGBA;
	}
	else if(colorFormat == ColorFormat::GREY)
	{
		f = GL_R8;
		cf = GL_RED;
	}
	glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
	glTextureStorage2D(this->handle, 1, f, width, height);
	glTextureSubImage2D(this->handle, 0, 0, 0, this->width, this->height, cf, GL_UNSIGNED_BYTE, data);
	this->setInterpolation(mode, mode);
	this->setAnisotropyLevel(1);
}

Texture::Texture(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha, bool sRGB)
{
	this->fmt = ColorFormat::RGBA;
	this->width = 1;
	this->height = 1;
	uint8_t **data = new uint8_t *;
	data[0] = new uint8_t[4];
	data[0][0] = red;
	data[0][1] = green;
	data[0][2] = blue;
	data[0][3] = alpha;
	glCreateTextures(GL_TEXTURE_2D, 1, &this->handle);
	glTextureStorage2D(this->handle, 1, sRGB ? GL_SRGB8_ALPHA8 : GL_RGBA8, this->width, this->height);
	glTextureSubImage2D(this->handle, 0, 0, 0, this->width, 1, GL_RGBA, GL_UNSIGNED_BYTE, data[0]);
	this->setInterpolation(InterpMode::Linear, InterpMode::Linear);
	this->setAnisotropyLevel(1);
}

Texture::~Texture()
{
	glDeleteTextures(1, &this->handle);
}

Texture::Texture(Texture &other)
{
	this->handle = other.handle;
	other.handle = 0;
}

Texture& Texture::operator=(Texture other)
{
	this->handle = other.handle;
	other.handle = 0;
	return *this;
}

Texture::Texture(Texture &&other)
{
	this->handle = other.handle;
	other.handle = 0;
}

Texture& Texture::operator=(Texture &&other)
{
	this->handle = other.handle;
	other.handle = 0;
	return *this;
}

void Texture::use(uint32_t target)
{
	int32_t curTex;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &curTex);
	glBindTextureUnit(target, this->handle);
}

void Texture::setInterpolation(InterpMode min, InterpMode mag)
{
	GLenum glMin = GL_NEAREST, glMag = GL_NEAREST;
	switch(min)
	{
		case InterpMode::Nearest: glMin = GL_NEAREST; break;
		case InterpMode::Linear: glMin = GL_LINEAR; break;
		default: break;
	}
	
	switch(mag)
	{
		case InterpMode::Nearest: glMag = GL_NEAREST; break;
		case InterpMode::Linear: glMag = GL_LINEAR; break;
		default: break;
	}
	glTextureParameteri(this->handle, GL_TEXTURE_MIN_FILTER, glMin);
	glTextureParameteri(this->handle, GL_TEXTURE_MAG_FILTER, glMag);
}

void Texture::setAnisotropyLevel(uint32_t level)
{
	glTextureParameterf(this->handle, GL_TEXTURE_MAX_ANISOTROPY, level);
}

void Texture::subImage(uint8_t *data, uint32_t w, uint32_t h, uint32_t xPos, uint32_t yPos, ColorFormat format)
{
	int32_t f = 0;
	switch(format)
	{
		case ColorFormat::RGB:
			f = GL_RGB;
			break;
		case ColorFormat::RGBA:
			f = GL_RGBA;
			break;
		case ColorFormat::GREY:
			f = GL_RED;
			break;
	}
	glTextureSubImage2D(this->handle, 0, xPos, yPos, w, h, f, GL_UNSIGNED_BYTE, data);
}

void Texture::clear()
{
	int32_t f = 0;
	switch(this->fmt)
	{
		case ColorFormat::RGB:
			f = GL_RGB;
			break;
		case ColorFormat::RGBA:
			f = GL_RGBA;
			break;
		case ColorFormat::GREY:
			f = GL_RED;
			break;
	}
	glClearTexImage(this->handle, 0, f, GL_UNSIGNED_BYTE, "\0\0\0\0");
}
