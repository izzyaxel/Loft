#pragma once

#include <commons/events.hh>
#include <SDL2/SDL_events.h>

template <typename ... Args> struct EventBase : public std::function<void(Args ...)>
{
	using std::function<void(Args ...)>::function;
};

//Constructor forwarding
#define _CFWD using EventBase::EventBase;

struct EventExiting : public EventBase<> {_CFWD};
struct EventPaused : public EventBase<> {_CFWD};
struct EventUnpaused : public EventBase<> {_CFWD};
struct EventPreUpdate : public EventBase<> {_CFWD};
struct EventPostUpdate : public EventBase<> {_CFWD};
struct EventPrerender : public EventBase<> {_CFWD};
struct EventPostrender : public EventBase<> {_CFWD};
struct EventScreenshot : public EventBase<std::string const &> {_CFWD};
struct EventMouseButton : public EventBase<bool, uint8_t> {_CFWD};
struct EventKeyboard : public EventBase<bool, SDL_KeyboardEvent>  {_CFWD};
struct EventWindowClose : public EventBase<> {_CFWD};
struct EventWindowSizeChanged : public EventBase<uint32_t, uint32_t> {_CFWD};
struct EventWindowFocusGained : public EventBase<> {_CFWD};
struct EventWindowFocusLost : public EventBase<> {_CFWD};
struct EventWindowEnter : public EventBase<> {_CFWD};
struct EventWindowLeave : public EventBase<> {_CFWD};
struct EventWindowExposed : public EventBase<> {_CFWD};
struct EventWindowHidden : public EventBase<> {_CFWD};
struct EventWindowShown : public EventBase<> {_CFWD};
struct EventWindowHitTest : public EventBase<> {_CFWD};
struct EventWindowRestored : public EventBase<> {_CFWD};
struct EventWindowMinimized : public EventBase<> {_CFWD};
struct EventWindowMaximized : public EventBase<> {_CFWD};
struct EventWindowTakeFocus : public EventBase<> {_CFWD};
struct EventWindowMoved : public EventBase<> {_CFWD};

using EventBus_t = EventBus<EventExiting,
		EventPaused,
		EventUnpaused,
		EventPreUpdate,
		EventPostUpdate,
		EventPrerender,
		EventPostrender,
		EventScreenshot,
		EventMouseButton,
		EventKeyboard,
		EventWindowClose,
		EventWindowSizeChanged,
		EventWindowFocusGained,
		EventWindowFocusLost,
		EventWindowEnter,
		EventWindowLeave,
		EventWindowExposed,
		EventWindowHidden,
		EventWindowShown,
		EventWindowHitTest,
		EventWindowRestored,
		EventWindowMinimized,
		EventWindowMaximized,
		EventWindowTakeFocus,
		EventWindowMoved>;
