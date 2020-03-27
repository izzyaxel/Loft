#include "framebuffer.hh"

#include "../../global.hh"
#include <glad/glad.h>

Framebuffer::Framebuffer(uint32_t width, uint32_t height, std::initializer_list<Attachment> const &options, std::string const &name)
{
	this->width = width;
	this->height = height;
	for(auto const &option : options)
	{
		switch(option)
		{
			case Attachment::Color: this->hasColor = true; break;
			case Attachment::Alpha: this->hasAlpha = true; break;
			case Attachment::Depth: this->hasDepth = true; break;
			case Attachment::Stencil: this->hasStencil = true; break;
			default: break;
		}
	}
	this->name = name;
	createFBO(*this);
}

Framebuffer::~Framebuffer()
{
	clearFBO(*this);
}

Framebuffer::Framebuffer(Framebuffer &other)
{
	this->handle = other.handle;
	other.handle = 0;
	
	this->colorHandle = other.colorHandle;
	other.colorHandle = 0;
	
	this->depthHandle = other.depthHandle;
	other.depthHandle = 0;
	
	this->stencilHandle = other.stencilHandle;
	other.stencilHandle = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer other)
{
	this->handle = other.handle;
	other.handle = 0;
	
	this->colorHandle = other.colorHandle;
	other.colorHandle = 0;
	
	this->depthHandle = other.depthHandle;
	other.depthHandle = 0;
	
	this->stencilHandle = other.stencilHandle;
	other.stencilHandle = 0;
	return *this;
}

Framebuffer::Framebuffer(Framebuffer &&other)
{
	this->handle = other.handle;
	other.handle = 0;
	
	this->colorHandle = other.colorHandle;
	other.colorHandle = 0;
	
	this->depthHandle = other.depthHandle;
	other.depthHandle = 0;
	
	this->stencilHandle = other.stencilHandle;
	other.stencilHandle = 0;
}

Framebuffer& Framebuffer::operator=(Framebuffer &&other)
{
	this->handle = other.handle;
	other.handle = 0;
	
	this->colorHandle = other.colorHandle;
	other.colorHandle = 0;
	
	this->depthHandle = other.depthHandle;
	other.depthHandle = 0;
	
	this->stencilHandle = other.stencilHandle;
	other.stencilHandle = 0;
	return *this;
}

void Framebuffer::use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->handle);
}

void Framebuffer::bind(Attachment type, uint32_t target)
{
	switch(type)
	{
		case Attachment::Color: glBindTextureUnit(target, this->colorHandle); break;
		case Attachment::Depth: glBindTextureUnit(target, this->depthHandle); break;
		case Attachment::Stencil: glBindTextureUnit(target, this->stencilHandle); break;
		default: break;
	}
}

void Framebuffer::regenerate(uint32_t width, uint32_t height)
{
	this->width = width;
	this->height = height;
	clearFBO(*this);
	createFBO(*this);
}

void Framebuffer::createFBO(Framebuffer &fbo)
{
	glCreateFramebuffers(1, &fbo.handle);
	fbo.use();
	glViewport(0, 0, fbo.width, fbo.height);
	glScissor(0, 0, fbo.width, fbo.height);
	if(fbo.hasColor) glCreateTextures(GL_TEXTURE_2D, 1, &fbo.colorHandle);
	if(fbo.hasDepth) glCreateTextures(GL_TEXTURE_2D, 1, &fbo.depthHandle);
	glTextureStorage2D(fbo.colorHandle, 1, fbo.hasAlpha ? GL_RGBA32F : GL_RGB32F, fbo.width, fbo.height);
	glNamedFramebufferTexture(fbo.handle, GL_COLOR_ATTACHMENT0, fbo.colorHandle, 0);
	if(fbo.hasDepth)
	{
		glTextureStorage2D(fbo.depthHandle, 1, GL_DEPTH_COMPONENT32F, fbo.width, fbo.height);
		glNamedFramebufferTexture(fbo.handle, GL_DEPTH_ATTACHMENT, fbo.depthHandle, 0);
	}
	std::vector<GLenum> drawBuffers;
	drawBuffers.emplace_back(GL_COLOR_ATTACHMENT0);
	if(fbo.hasDepth) drawBuffers.emplace_back(GL_COLOR_ATTACHMENT1);
	glNamedFramebufferDrawBuffers(fbo.handle, static_cast<int32_t>(drawBuffers.size()), drawBuffers.data());
	GLenum error = glCheckNamedFramebufferStatus(fbo.handle, GL_FRAMEBUFFER);
	if(error != GL_FRAMEBUFFER_COMPLETE)
	{
		std::string er = "FBO Creation Error: ";
		switch(error)
		{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: er += "incomplete attachment"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT: er += "incomplete dimensions"; break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: er += "missing attachment"; break;
			case GL_FRAMEBUFFER_UNSUPPORTED: er += "FBOs are not supported"; break;
			default: break;
		}
		logger << Sev::ERR << er << logger.endl();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::clearFBO(Framebuffer &fbo)
{
	glDeleteFramebuffers(1, &fbo.handle);
	glDeleteTextures(1, &fbo.colorHandle);
	glDeleteTextures(1, &fbo.depthHandle);
}

FramebufferPool::FramebufferPool(size_t alloc, uint32_t width, uint32_t height)
{
	this->pool.resize(alloc);
	for(size_t i = 0; i < alloc; i++)
	{
		this->pool[i] = std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::Color, Attachment::Alpha, Attachment::Depth}, "Pool " + std::to_string(i));
	}
}

std::shared_ptr<Framebuffer> FramebufferPool::getNextAvailableFBO(uint32_t width, uint32_t height)
{
	std::shared_ptr<Framebuffer> out;
	for(auto &fbo : this->pool)
	{
		if(fbo.use_count() == 1)
		{
			if(fbo->width != width || fbo->height != height)
			{
				fbo = std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::Color, Attachment::Alpha, Attachment::Depth}, "Pool " + std::to_string(this->pool.size() + 1));
			}
			fbo->use();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			return fbo;
		}
	}
	this->pool.push_back(std::make_shared<Framebuffer>(width, height, std::initializer_list<Attachment>{Attachment::Color, Attachment::Alpha, Attachment::Depth}, "Pool " + std::to_string(this->pool.size() + 1)));
	this->pool.back()->use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return this->pool.back();
}

void FramebufferPool::onResize(uint32_t width, uint32_t height)
{
	for(auto const &fbo : this->pool)
	{
		fbo->regenerate(width, height);
	}
}
