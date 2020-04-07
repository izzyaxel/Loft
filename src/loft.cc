#include "loft.hh"
#include "input.hh"
#include "global.hh"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <commons/logger.hh>

void glDebug(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const *message, void const *userParam)
{
	std::string sev, ty;
	switch(severity)
	{
		case GL_DEBUG_SEVERITY_LOW:
			sev = "Severity: LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			sev = "Severity: MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			sev = "Severity: HIGH";
			break;
		default: break;
	}
	switch(type)
	{
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			ty = "Type: Deprecated Behavior";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			ty = "Type: Undefined Behavior";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			ty = "Type: Portability";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			ty = "Type: Performance";
			break;
		case GL_DEBUG_TYPE_OTHER:
			ty = "Type: Other";
			break;
		default: break;
	}
	logger << Sev::ERR << "An OpenGL error occured: " << sev << ", ID: " << id << ", " << ty << ", Message: " << message << logger.endl();
}

Loft::Loft(uint32_t width, uint32_t height, std::string const &windowTitle, bool resizable, bool startMaximized)
{
	this->width = width;
	this->height = height;
	this->windowTitle = windowTitle;
	this->resizable = resizable;
	this->startMaximized = startMaximized;
	
	this->eventBus = MU<EventBus_t>();
	this->input = MU<Input>();
	
	SDL_Init(SDL_INIT_EVERYTHING);
	this->window = SDL_CreateWindow(this->windowTitle.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->width, this->height, SDL_WINDOW_OPENGL);
	if(!this->window) throw std::runtime_error("Window Setup: Failed to create a window");
	SDL_SetWindowResizable(reinterpret_cast<SDL_Window*>(this->window), this->resizable ? SDL_TRUE : SDL_FALSE);
	if(this->minHeight != 0 && this->minWidth != 0) SDL_SetWindowMinimumSize(reinterpret_cast<SDL_Window*>(this->window), this->minWidth, this->minHeight);
	if(this->startMaximized)
	{
		SDL_MaximizeWindow(reinterpret_cast<SDL_Window*>(this->window));
		int32_t w, h;
		SDL_GL_GetDrawableSize(reinterpret_cast<SDL_Window*>(this->window), &w, &h);
		this->width = (uint32_t)w;
		this->height = (uint32_t)h;
	}
	
	SDL_GL_LoadLibrary(nullptr);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	this->context = SDL_GL_CreateContext(reinterpret_cast<SDL_Window*>(window));
	if(!this->context) throw std::runtime_error("Failed to initialize renderer, OpenGL 4.5 is not supported on your hardware");
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	if(!GLAD_GL_VERSION_4_5) throw std::runtime_error("Failed to initialize renderer, OpenGL 4.5 is not supported on your hardware");
	SDL_GL_SetSwapInterval(this->vsync);
	
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(glDebug, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, GL_TRUE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glViewport(0, 0, this->width, this->height);
	glScissor(0, 0, this->width, this->height);
	this->renderer = MU<Renderer>(this->eventBus, this->width, this->height);
	
	eventBus->registerEventHandler<EventWindowSizeChanged>([this](uint32_t newWidth, uint32_t newHeight)
	{
		this->width = newWidth;
		this->height = newHeight;
	});
}

Loft::~Loft()
{
	this->input.reset();
	this->eventBus.reset();
	this->renderer.reset();
}

void Loft::update(double delta)
{
	this->world.update(delta);
}

void Loft::renderFrame()
{
	this->renderer->render(this->world.getSceneGraph(), this->camera);
	SDL_GL_SwapWindow(reinterpret_cast<SDL_Window*>(this->window));
}
