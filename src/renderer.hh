#pragma once

#include "def.hh"
#include "color.hh"
#include "object.hh"
#include "events.hh"
#include "api/assets/camera.hh"
#include "api/render/renderList.hh"
#include "postStack.hh"

#include <commons/math/vec2.hh>
#include <commons/math/mat4.hh>
#include <cstdint>

/// Access types for compute shader imnage binding
enum struct IO
{
	READ = 0x88B8, WRITE = 0x88B9, READWRITE = 0x88BA
};

/// Color format for compute shader image binding
enum struct CF
{
	R32F = 0x822E, RGB8 = 0x8051, RGBA8 = 0x8058, RGB16 = 0x8054, RGBA16 = 0x805B, RGB32I = 0x8D83, RGBA32I = 0x8D82,
	RGB32UI = 0x8D71, RGBA32UI = 0x8D70, RGB16F = 0x881B, RGBA16F = 0x881A, RGB32F = 0x8815, RGBA32F = 0x8814,
	DEPTH32F = 0x8CAC,
};

/// Mode to draw a VAO in
enum struct DrawMode
{
	TRIS = 0x0004, TRISTRIPS = 0x0005, TRIFANS = 0x0006,
	LINES = 0x0001, LINESTRIPS = 0x0003, LINELOOPS = 0x0002,
	POINTS = 0x0000,
};

struct Renderer
{
	Renderer(UP<EventBus_t> const &eventBus, uint32_t contextWidth, uint32_t contextHeight);
	~Renderer();
	
	/// Render a given list
	void render(RenderList renderList, Camera const &camera);
	
	/// Set the RGBA color to clear the context to
	void setClearColor(float r, float g, float b, float a);
	
	/// Set the RGBA color to clear the context to, these values will be converted into sRGB space
	void setSRGBClearColor(float r, float g, float b, float a);
	
	/// Clear the currently bound framebuffer's depth and color
	void clear();
	
	/// Enable/disable depth testing
	void setDepthTesting(bool depthTest);
	
	/// Enable/disable scissor testing
	void setScissorTesting(bool scissorTest);
	
	/// Enable/disable blending
	void setBlend(bool blend);
	
	/// Set the blending type
	void setBlendMode(uint32_t src, uint32_t dst);
	
	/// Enable/disable backface culling
	void setCullFace(bool culling);
	
	/// Bind the back buffer to draw onto
	void useBackBuffer();
	
	/// Get the refresh rate of the monitor the engine window is on
	[[nodiscard]] uint32_t getCurrentMonitorRefreshRate();
	
	/// Bind a texture for use with compute shaders
	void bindImage(uint32_t target, uint32_t const &handle, IO mode, CF format);
	
	/// Run the currently bound compute shader program
	void startComputeShader(uint32_t contextWidth, uint32_t contextHeight);
	
	/// Run the currently bound vert/frag shader program
	void draw(DrawMode mode, size_t numElements);
	
	RenderList list;
	
	uint32_t workSizeX = 40, workSizeY = 20;

private:
	void drawRenderable(Renderable const &entry);
	
	uint32_t _contextWidth, _contextHeight;
	Color _clearColor;
	mat4x4<float> _m, _v, _p, _mvp;
};
