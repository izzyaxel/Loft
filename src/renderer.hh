#pragma once

#include "def.hh"
#include "color.hh"
#include "object.hh"
#include "events.hh"
#include "api/render/framebuffer.hh"
#include "api/render/renderList.hh"
#include "postStack.hh"

#include <commons/math/vec2.hh>
#include <commons/math/mat4.hh>
#include <cstdint>

struct Alternator
{
	inline bool swap(){alt = !alt; return alt;}
	inline bool get(){return alt;} //true: a false: b
private: bool alt = true;
};

struct Renderer
{
	Renderer(UP<EventBus_t> const &eventBus, uint32_t contextWidth, uint32_t contextHeight);
	~Renderer();
	
	/// Render a given list
	void render(RenderList renderList, mat4x4<float> view, mat4x4<float> projection, SP<LayerPostStack> const &layerPostStack, SP<GlobalPostStack> const &globalPostStack);
	
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
	
	RenderList list;

protected:
	void pingPong();

private:
	void postProcessLayer(uint64_t layer, SP<LayerPostStack> const &layerPostStack);
	void postProcessGlobal(SP<GlobalPostStack> const &globalPostStack);
	void drawToScratch();
	void drawToBackBuffer();
	void scratchToPingPong();
	void drawRenderable(Renderable const &entry);
	
	Framebuffer *fboA = nullptr, *fboB = nullptr, *scratch = nullptr;
	Alternator curFBO;
	uint32_t contextWidth, contextHeight;
	Color clearColor;
	mat4x4<float> m, v, p, mvp;
};
