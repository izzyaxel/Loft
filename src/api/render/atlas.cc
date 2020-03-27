#include "atlas.hh"

#include "../../bsp.hh"
#include "../../assets.hh"
#include "../assets/pngw.hh"
#include "../../global.hh"

#include <glad/glad.h>
#include <algorithm>

void Atlas::addTile(std::string const &name, std::vector<uint8_t> const &tileData)
{
	if(this->contains(name))
	{
		logger << Sev::ERR << "Altas already contains a tile with the name " << name << logger.endl();
		return;
	}
	if(this->finalized)
	{
		logger << Sev::ERR << "Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize" << logger.endl();
		return;
	}
	if(tileData.empty())
	{
		logger << Sev::ERR << "Tile data is empty" << logger.endl();
		return;
	}
	PNG decoded = decodePNG(tileData);
	ColorFormat f = ColorFormat::RGB;
	switch(decoded.colorFormat)
	{
		case PNG::COLOR_FMT_GREY:
			f = ColorFormat::GREY;
			break;
		case PNG::COLOR_FMT_RGB:
			f = ColorFormat::RGB;
			break;
		case PNG::COLOR_FMT_RGBA:
			f = ColorFormat::RGBA;
			break;
	}
	this->atlas.push_back(AtlasImg{name, std::move(decoded.imageData), f, vec2<uint32_t>{0, 0}, decoded.width, decoded.height});
}

void Atlas::addTile(std::string const &name, ColorFormat fmt, std::vector<uint8_t> &&tileData, uint32_t width, uint32_t height)
{
	if(this->contains(name))
	{
		logger << Sev::ERR << "Altas already contains a tile with the name " << name << logger.endl();
		return;
	}
	if(this->finalized)
	{
		logger << Sev::ERR << "Atlas has already been uploaded to the GPU, add new tiles to it before calling finalize" << logger.endl();
		return;
	}
	if(tileData.empty())
	{
		logger << Sev::ERR << "Tile data is empty" << logger.endl();
		return;
	}
	this->atlas.push_back(AtlasImg{name, std::move(tileData), fmt, vec2<uint32_t>{0, 0}, width, height});
}

QuadUVs Atlas::getUVsForTile(std::string const &name)
{
	if(!this->finalized || !this->contains(name))
	{
		return QuadUVs{};
	}
	vec2<uint32_t> location{};
	uint32_t width = 0, height = 0;
	for(auto &tile : this->atlas)
	{
		if(tile.name == name)
		{
			location = tile.location;
			width = tile.width;
			height = tile.height;
			break;
		}
	}
	vec2<float> ll = vec2<float>{(float)location.x(), (float)location.y()};
	vec2<float> ul = vec2<float>{(float)location.x(), (float)(location.y() + height)};
	vec2<float> lr = vec2<float>{(float)(location.x() + width), (float)location.y()};
	vec2<float> ur = vec2<float>{(float)(location.x() + width), (float)(location.y() + height)};
	ll = ll / this->atlasDims;
	ul = ul / this->atlasDims;
	lr = lr / this->atlasDims;
	ur = ur / this->atlasDims;
	return QuadUVs{ul, ll, ur, lr};
}

vec2<double> Atlas::getTileDimensions(std::string const &name)
{
	if(!this->contains(name)) return {0.0f, 0.0f};
	for(auto const &tile : this->atlas)
	{
		if(tile.name == name)
		{
			return vec2<double>{(float)tile.width, (float)tile.height};
		}
	}
	return vec2<double>{0.0f, 0.0f};
}

void Atlas::use(uint32_t target)
{
	if(!this->finalized) logger << Sev::ERR << "Trying to bind atlas before it's been finalized" << logger.endl();
	else AR::getTexture(this->texID)->use(target);
}

uint32_t Atlas::getHandle()
{
	return AR::getTexture(this->texID)->handle;
}

bool Atlas::contains(std::string const &tileName)
{
	for(auto const &img : this->atlas) if(img.name == tileName) return true;
	return false;
}

void Atlas::finalize(ColorFormat fmt)
{
	if(this->finalized)
	{
		logger << Sev::ERR << "Atlas has already been uploaded to the GPU, finalization failed" << logger.endl();
		return;
	}
	if(this->atlas.empty())
	{
		logger << Sev::ERR << "Atlas doesn't contain anything, finalization failed" << logger.endl();
		return;
	}
	BSPLayout<uint32_t> layout;
	std::sort(this->atlas.begin(), this->atlas.end(), AtlasImg::comparator);
	for(auto &tile : this->atlas)
	{
		if(tile.width == 0 || tile.height == 0)
		{
			logger << Sev::ERR << "Atlas encountered a tile with 0 width or height: \"" << tile.name << "\" finalization failed" << logger.endl();
			continue;
		}
		tile.location = layout.pack(tile.width, tile.height);
	}
	if(layout.height() == 0 || layout.width() == 0)
	{
		logger << Sev::ERR << "After layout, this atlas would have 0 width or height, finalization failed" << logger.endl();
		return;
	}
	this->texID = AR::newTexture(layout.width(), layout.height(), fmt, InterpMode::Nearest);
	AR::getTexture(this->texID)->clear();
	for(auto &tile : this->atlas)
	{
		AR::getTexture(this->texID)->subImage(tile.data.data(), tile.width, tile.height, tile.location.x(), tile.location.y(), tile.fmt);
	}
	this->atlasDims = {(float)layout.width(), (float)layout.height()};
	this->finalized = true;
}

void Atlas::dump(std::string const &path)
{
	if(!this->finalized) return;
	int32_t cpp = 0, f = 0, pngF = 0; //channels per pixel, format, png format
	switch(AR::getTexture(this->texID)->fmt)
	{
		case ColorFormat::GREY:
			cpp = 1;
			f = GL_RED;
			pngF = PNG::COLOR_FMT_GREY;
			break;
		case ColorFormat::RGB:
			cpp = 3;
			f = GL_RGB;
			pngF = PNG::COLOR_FMT_RGB;
			break;
		case ColorFormat::RGBA:
			cpp = 4;
			f = GL_RGBA;
			pngF = PNG::COLOR_FMT_RGBA;
			break;
	}
	
	std::vector<uint8_t> imageData;
	imageData.resize((size_t)(this->atlasDims.x() * this->atlasDims.y() * cpp));
	glGetTextureImage(AR::getTexture(this->texID)->handle, 0, f, GL_UNSIGNED_BYTE, imageData.size(), imageData.data());
	writePNG(path, (uint32_t)this->atlasDims.x(), (uint32_t)this->atlasDims.y(), imageData.data(), pngF);
}
