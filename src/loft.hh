#pragma once

#include "events.hh"
#include "def.hh"
#include "input.hh"
#include "renderer.hh"
#include "api/assets/camera.hh"
#include "world.hh"

#include <cstdint>
#include <string>

enum struct WindowMode
{
	WINDOWED, FULLSCREEN, BORDERLESSFULLSCREEN,
};

struct Loft
{
	Loft(uint32_t width, uint32_t height, std::string const &windowTitle, bool resizable, bool startMaximized);
	~Loft();
	
	[[nodiscard]] inline static UP<Loft> create(uint32_t width, uint32_t height, std::string const &windowTitle, bool resizable, bool startMaximized)
	{
		return MU<Loft>(width, height, windowTitle, resizable, startMaximized);
	}
	
	void update(double delta);
	void renderFrame();
	
	uint32_t width = 800, height = 600;
	std::string windowTitle = "Loft Engine";
	bool resizable = false, startMaximized = false;
	
	bool exiting = false;
	uint32_t vsync = 1, updateRate = 60, minFPS = 60, windowedWidth = 800, windowedHeight = 600, minWidth = 100, minHeight = 100;
	void *window = nullptr, *context = nullptr;
	float nearPlane = 0.1f, farPlane = 1.1f;
	
	UP<EventBus_t> eventBus = nullptr;
	UP<Input> input = nullptr;
	UP<Renderer> renderer = nullptr;
	Camera camera;
	World world;
};
