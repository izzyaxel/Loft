#pragma once

#include <cstdint>
#include <initializer_list>
#include <vector>
#include <cstdio>
#include <string>
#include <memory>

enum struct Attachment
{
	Color, Alpha, Depth, Stencil,
};

struct Framebuffer
{
	Framebuffer() = delete;
	Framebuffer(uint32_t width, uint32_t height, std::initializer_list<Attachment> const &options, std::string const &name);
	~Framebuffer();
	
	//copy
	Framebuffer(Framebuffer &other);
	Framebuffer& operator=(Framebuffer other);
	
	//move
	Framebuffer(Framebuffer &&other);
	Framebuffer& operator=(Framebuffer &&other);
	
	void use();
	void bind(Attachment type, uint32_t target);
	void regenerate(uint32_t width, uint32_t height);
	
	uint32_t handle = 0, colorHandle = 0, depthHandle = 0, stencilHandle = 0, width = 0, height = 0;
	bool hasColor = false, hasDepth = false, hasAlpha = false, hasStencil = false;
	std::string name = "";

private:
	void createFBO(Framebuffer &fbo);
	void clearFBO(Framebuffer &fbo);
};

struct FramebufferPool
{
	FramebufferPool() = delete;
	FramebufferPool(FramebufferPool const &other) = delete;
	FramebufferPool(FramebufferPool const &&other) = delete;
	
	FramebufferPool(size_t alloc, uint32_t width, uint32_t height);
	
	[[nodiscard]] std::shared_ptr<Framebuffer> getNextAvailableFBO(uint32_t width, uint32_t height);
	
	void onResize(uint32_t width, uint32_t height);

private:
	std::vector<std::shared_ptr<Framebuffer>> pool;
};
