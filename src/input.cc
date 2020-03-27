#include "input.hh"

Input::Input()
{
	for(int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if(SDL_IsGameController(i))
		{
			controller = SDL_GameControllerOpen(i);
			break;
		}
	}
	keystate = SDL_GetKeyboardState(nullptr);
}

void Input::updateImmediateModeKbd(double deltaT)
{
	//TODO handle making deltaTT with global or player specific timescale
	if(SDL_IsTextInputActive() == SDL_TRUE)
	{
		this->immediateModeKeystates[keymap[Key::KEY_ESC]] = this->keystate[SDL_SCANCODE_ESCAPE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F1]] = this->keystate[SDL_SCANCODE_F1] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F2]] = this->keystate[SDL_SCANCODE_F2] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F3]] = this->keystate[SDL_SCANCODE_F3] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F4]] = this->keystate[SDL_SCANCODE_F4] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F5]] = this->keystate[SDL_SCANCODE_F5] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F6]] = this->keystate[SDL_SCANCODE_F6] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F7]] = this->keystate[SDL_SCANCODE_F7] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F8]] = this->keystate[SDL_SCANCODE_F8] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F9]] = this->keystate[SDL_SCANCODE_F9] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F10]] = this->keystate[SDL_SCANCODE_F10] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F11]] = this->keystate[SDL_SCANCODE_F11] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F12]] = this->keystate[SDL_SCANCODE_F12] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_PRNTSCRN]] = this->keystate[SDL_SCANCODE_PRINTSCREEN] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_NUMLCK]] = this->keystate[SDL_SCANCODE_NUMLOCKCLEAR] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_PAUSE]] = this->keystate[SDL_SCANCODE_PAUSE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_INSERT]] = this->keystate[SDL_SCANCODE_INSERT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_HOME]] = this->keystate[SDL_SCANCODE_HOME] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_PGUP]] = this->keystate[SDL_SCANCODE_PAGEUP] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_DEL]] = this->keystate[SDL_SCANCODE_DELETE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_END]] = this->keystate[SDL_SCANCODE_END] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_PGDOWN]] = this->keystate[SDL_SCANCODE_PAGEDOWN] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_LARROW]] = this->keystate[SDL_SCANCODE_LEFT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RARROW]] = this->keystate[SDL_SCANCODE_RIGHT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_UARROW]] = this->keystate[SDL_SCANCODE_UP] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_DARROW]] = this->keystate[SDL_SCANCODE_DOWN] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_GRAVE]] = this->keystate[SDL_SCANCODE_GRAVE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_1]] = this->keystate[SDL_SCANCODE_1] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_2]] = this->keystate[SDL_SCANCODE_2] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_3]] = this->keystate[SDL_SCANCODE_3] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_4]] = this->keystate[SDL_SCANCODE_4] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_5]] = this->keystate[SDL_SCANCODE_5] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_6]] = this->keystate[SDL_SCANCODE_6] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_7]] = this->keystate[SDL_SCANCODE_7] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_8]] = this->keystate[SDL_SCANCODE_8] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_9]] = this->keystate[SDL_SCANCODE_9] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_0]] = this->keystate[SDL_SCANCODE_0] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_MINUS]] = this->keystate[SDL_SCANCODE_MINUS] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_EQUALS]] = this->keystate[SDL_SCANCODE_EQUALS] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_BACKSPACE]] = this->keystate[SDL_SCANCODE_BACKSPACE] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_Q]] = this->keystate[SDL_SCANCODE_Q] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_W]] = this->keystate[SDL_SCANCODE_W] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_E]] = this->keystate[SDL_SCANCODE_E] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_R]] = this->keystate[SDL_SCANCODE_R] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_T]] = this->keystate[SDL_SCANCODE_T] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_Y]] = this->keystate[SDL_SCANCODE_Y] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_U]] = this->keystate[SDL_SCANCODE_U] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_I]] = this->keystate[SDL_SCANCODE_I] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_O]] = this->keystate[SDL_SCANCODE_O] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_P]] = this->keystate[SDL_SCANCODE_P] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RBRACKET]] = this->keystate[SDL_SCANCODE_RIGHTBRACKET] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_LBRACKET]] = this->keystate[SDL_SCANCODE_LEFTBRACKET] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_BSLASH]] = this->keystate[SDL_SCANCODE_BACKSLASH] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_CAPSLCK]] = this->keystate[SDL_SCANCODE_CAPSLOCK] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_A]] = this->keystate[SDL_SCANCODE_A] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_S]] = this->keystate[SDL_SCANCODE_S] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_D]] = this->keystate[SDL_SCANCODE_D] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_F]] = this->keystate[SDL_SCANCODE_F] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_G]] = this->keystate[SDL_SCANCODE_G] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_H]] = this->keystate[SDL_SCANCODE_H] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_J]] = this->keystate[SDL_SCANCODE_J] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_K]] = this->keystate[SDL_SCANCODE_K] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_L]] = this->keystate[SDL_SCANCODE_L] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_SEMICOLON]] = this->keystate[SDL_SCANCODE_SEMICOLON] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_APOSTRAPHE]] = this->keystate[SDL_SCANCODE_APOSTROPHE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RETURN]] = this->keystate[SDL_SCANCODE_RETURN] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_LSHIFT]] = this->keystate[SDL_SCANCODE_LSHIFT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_Z]] = this->keystate[SDL_SCANCODE_Z] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_X]] = this->keystate[SDL_SCANCODE_X] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_C]] = this->keystate[SDL_SCANCODE_C] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_V]] = this->keystate[SDL_SCANCODE_V] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_B]] = this->keystate[SDL_SCANCODE_B] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_N]] = this->keystate[SDL_SCANCODE_N] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_M]] = this->keystate[SDL_SCANCODE_M] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_COMMA]] = this->keystate[SDL_SCANCODE_COMMA] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_PERIOD]] = this->keystate[SDL_SCANCODE_PERIOD] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_FSLASH]] = this->keystate[SDL_SCANCODE_SLASH] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RSHIFT]] = this->keystate[SDL_SCANCODE_RSHIFT] != 0;
		
		this->immediateModeKeystates[keymap[Key::KEY_LCTRL]] = this->keystate[SDL_SCANCODE_LCTRL] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_LALT]] = this->keystate[SDL_SCANCODE_LALT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_SPACE]] = this->keystate[SDL_SCANCODE_SPACE] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RALT]] = this->keystate[SDL_SCANCODE_RALT] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_APP]] = this->keystate[SDL_SCANCODE_APPLICATION] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_MENU]] = this->keystate[SDL_SCANCODE_MENU] != 0;
		this->immediateModeKeystates[keymap[Key::KEY_RCTRL]] = this->keystate[SDL_SCANCODE_RCTRL] != 0;
		
		//TODO scroll lock, break etc
		
		//update press lengths
		if(this->immediateModeKeystates[Action::JUMP]) this->pressLength[Action::JUMP] += deltaT;
		else this->pressLength[Action::JUMP] = 0;
	}
}

void Input::bindImmediateModeKey(Key key, Action action)
{
	this->keymap[key] = action;
}

void Input::bindTriggerModeKey(Key key, std::function<void (bool)> callback)
{
	this->triggerModeKeybinds.push_back(TriggerModePair{key, callback});
}

Action Input::getBoundAction(Key key)
{
	return this->keymap[key];
}

Key Input::getBoundKey(Action action)
{
	for(auto const &kb : this->keymap) if(kb.second == action) return kb.first;
	return Key::KEY_UNKNOWN;
}
