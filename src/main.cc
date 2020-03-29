#include "loft.hh"
#include "global.hh"

#include <chrono>
#include <commons/logger.hh>
#include <commons/misc.hh>
#include <commons/fileio.hh>

int main()
{
	UP<Loft> loft = nullptr;
	
	loft = Loft::create(800, 600, "Loft Engine", false, false);
	createDirectory(getCWD() + "out");
	logger.setOptions(LoggerOptions{LogTarget::STDOUT, LogStamping::TIMESTAMPS, LogVerbosity::HIGH, getCWD() + "out/log.txt", false, true});
	
	loft->input->bindTriggerModeKey(Key::KEY_ESC, [&loft] (bool down)
	{
		if(!down) loft->exiting = true;
	});
	
	loft->renderer->setClearColor(0.0f, 0.01f, 0.05f, 0.0f);
	loft->renderer->setCullFace(true);
	
	//Register event handlers
	loft->eventBus->registerEventHandler<EventMouseButton>([] (bool down, uint8_t button)
	{
		switch(button)
		{
			case 1: //LMB
	
				break;
	
			case 2: //MMB
	
				break;
	
			case 3: //RMB
	
				break;
	
			default: break;
		}
	});
	loft->eventBus->registerEventHandler<EventKeyboard>([&loft] (bool down, SDL_KeyboardEvent event)
	{
		for(auto &bind : loft->input->triggerModeKeybinds) if((int)event.keysym.scancode == (int)bind.key) bind.callback(down);
	});
	
	SDL_Event event;
	auto now = std::chrono::steady_clock::now(), prev = std::chrono::steady_clock::now(), prevFrame = std::chrono::steady_clock::now();
	double delta = 0, deltaT = 0, accumulation = 0;
	do
	{
		now = std::chrono::steady_clock::now();
		delta = std::chrono::duration_cast<std::chrono::duration<double>>(now - prev).count();
		accumulation += delta;
		prev = now;
		double targetFrameTime = 1.0 / (double)loft->updateRate;
		if(accumulation >= targetFrameTime)
		{
			deltaT = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::steady_clock::now() - prevFrame).count();
			if(deltaT > 1.0 / loft->minFPS) deltaT = 1.0 / loft->minFPS;
			accumulation -= targetFrameTime;
			prevFrame = now;
			loft->input->updateImmediateModeKbd(deltaT);
			while(SDL_PollEvent(&event) != 0)
			{
				switch(event.type)
				{
					case SDL_MOUSEBUTTONUP: loft->eventBus->post<EventMouseButton>(false, event.button.button); break;
					case SDL_MOUSEBUTTONDOWN: loft->eventBus->post<EventMouseButton>(true, event.button.button); break;
					case SDL_KEYDOWN: loft->eventBus->post<EventKeyboard>(true, event.key); break;
					case SDL_KEYUP: loft->eventBus->post<EventKeyboard>(false, event.key); break;
					case SDL_QUIT: loft->exiting = true; break; //Event is posted when the engine actually starts shutting down
					case SDL_WINDOWEVENT:
						if(event.window.event == SDL_WINDOWEVENT_CLOSE) loft->eventBus->post<EventWindowClose>();
						if(event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) loft->eventBus->post<EventWindowFocusGained>();
						if(event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) loft->eventBus->post<EventWindowFocusLost>();
						if(event.window.event == SDL_WINDOWEVENT_TAKE_FOCUS) loft->eventBus->post<EventWindowTakeFocus>();
						if(event.window.event == SDL_WINDOWEVENT_ENTER) loft->eventBus->post<EventWindowEnter>();
						if(event.window.event == SDL_WINDOWEVENT_LEAVE) loft->eventBus->post<EventWindowLeave>();
						if(event.window.event == SDL_WINDOWEVENT_EXPOSED) loft->eventBus->post<EventWindowExposed>();
						if(event.window.event == SDL_WINDOWEVENT_HIDDEN) loft->eventBus->post<EventWindowHidden>();
						if(event.window.event == SDL_WINDOWEVENT_SHOWN) loft->eventBus->post<EventWindowShown>();
						if(event.window.event == SDL_WINDOWEVENT_HIT_TEST) loft->eventBus->post<EventWindowHitTest>();
						if(event.window.event == SDL_WINDOWEVENT_RESTORED) loft->eventBus->post<EventWindowRestored>();
						if(event.window.event == SDL_WINDOWEVENT_MINIMIZED) loft->eventBus->post<EventWindowMinimized>();
						if(event.window.event == SDL_WINDOWEVENT_MAXIMIZED) loft->eventBus->post<EventWindowMaximized>();
						if(event.window.event == SDL_WINDOWEVENT_MOVED) loft->eventBus->post<EventWindowMoved>();
						if(event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_RESIZED)
						{
							int32_t w = 0, h = 0;
							SDL_GL_GetDrawableSize(reinterpret_cast<SDL_Window *>(loft->window), &w, &h);
							loft->eventBus->post<EventWindowSizeChanged>((uint32_t)w, (uint32_t)h);
						}
						break;
					default: break;
				}
			}
			loft->eventBus->post<EventPreUpdate>();
			loft->update(deltaT);
			loft->eventBus->post<EventPostUpdate>();
			loft->eventBus->post<EventPrerender>();
			loft->renderFrame();
			loft->eventBus->post<EventPostrender>();
		}
		else SDL_Delay(1);
	} while(!loft->exiting);
	loft->eventBus->post<EventExiting>();
	loft.reset();
	return 0;
}
