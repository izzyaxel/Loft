#pragma once

#include "framebuffer.hh"

struct RenderPass
{
	virtual ~RenderPass() = default;
	
	/// Process the pixels held by 'in', and write the result to 'out'
	virtual void process(Framebuffer *out, Framebuffer *in) = 0;
	
	bool enabled = true;
	std::string name = "";

protected:
	RenderPass() = default;
};
