//
// C++ Implementation: sore_input
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_input.h"

SORE_Kernel::GlobalInputFunctor* SORE_Kernel::MakeFunctor(bool(*func)(Event*))
{
	return new GlobalInputFunctor(func);
}

SORE_Kernel::InputTask::InputTask(SORE_Kernel::GameKernel* gk) : Task(gk)
{
	event.mouse.buttonState = 0x00;
}

SORE_Kernel::InputTask::~InputTask()
{
}

void SORE_Kernel::InputTask::Frame(int elapsedTime)
{
	SDL_Event sdl_event;
	while(SDL_PollEvent(&sdl_event))
	{
		switch(sdl_event.type)
		{
			case SDL_QUIT:
			{
				gk->quitFlag = true;
				break;
			}
			case SDL_MOUSEMOTION:
				event.type = MOUSEMOVE;
				event.mouse.x = sdl_event.motion.x;
				event.mouse.y = sdl_event.motion.y;
				event.mouse.xmove = sdl_event.motion.xrel;
				event.mouse.ymove = sdl_event.motion.yrel;
				break;
			case SDL_MOUSEBUTTONDOWN:
				ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Button down: buttonState: %u, button: %u", sdl_event.motion.state, sdl_event.button.button);
				event.type = MOUSEBUTTONDOWN;
				if(sdl_event.motion.state & SDL_BUTTON_LEFT) event.mouse.buttonState |= MOUSE_BUTTON1;
				if(sdl_event.motion.state & SDL_BUTTON_MIDDLE) event.mouse.buttonState |= MOUSE_BUTTON3;
				if(sdl_event.motion.state & SDL_BUTTON_RIGHT) event.mouse.buttonState |= MOUSE_BUTTON2;
				//event.mouse.buttonState = sdl_event.button.button;
				break;
			case SDL_MOUSEBUTTONUP:
				ENGINE_LOG(SORE_Logging::LVL_DEBUG2, "Button up: buttonState: %u, button: %u", sdl_event.motion.state, sdl_event.button.button);
				event.type = MOUSEBUTTONUP;
				if(sdl_event.button.button & SDL_BUTTON_LEFT) event.mouse.buttonState -= MOUSE_BUTTON1;
				if(sdl_event.button.button & SDL_BUTTON_RIGHT) event.mouse.buttonState -= MOUSE_BUTTON2;
				if(sdl_event.button.button & SDL_BUTTON_MIDDLE) event.mouse.buttonState -= MOUSE_BUTTON3;
				//event.mouse.buttonState = 0x00;
				//if(sdl_event.motion.state & SDL_BUTTON_LEFT) event.mouse.buttonState |= MOUSE_BUTTON1;
				//if(sdl_event.motion.state & SDL_BUTTON_MIDDLE) event.mouse.buttonState |= MOUSE_BUTTON3;
				//if(sdl_event.motion.state & SDL_BUTTON_RIGHT) event.mouse.buttonState |= MOUSE_BUTTON2;
				//event.mouse.buttonState = sdl_event.button.button;
				break;
			case SDL_KEYDOWN:
				event.type = KEYDOWN;
				event.key.keySym = sdl_event.key.keysym.sym;
				event.key.modState = sdl_event.key.keysym.mod;
				break;
			case SDL_KEYUP:
				event.type = KEYUP;
				event.key.keySym = sdl_event.key.keysym.sym;
				break;
			case SDL_VIDEORESIZE:
				event.type = RESIZE;
				event.resize.w = sdl_event.resize.w;
				event.resize.h = sdl_event.resize.h;
				break;
			default: //don't care about this event, so don't handle it!
				return;
		}
		std::multimap<unsigned int, InputFunctor*>::iterator it;
		for(it=allListeners.begin();it!=allListeners.end();it++)
		{
			if(it->first & event.type)
			{
				//InputFunctor* temp = (it->second);
				//if((it->second)(&event))
				//	break;
				if((*it->second)(&event) && event.type!=RESIZE) //ALWAYS propagate resize events...can be used by more than one function
					break;
			}
		}
	}
}

SORE_Kernel::event_listener_ref SORE_Kernel::InputTask::AddListener(unsigned int eventType, InputFunctor* functor)
{
	event_listener_ref size = allListeners.size();
	allListeners.insert(std::pair<unsigned int, InputFunctor*>(eventType, functor));
	return size;
}

void SORE_Kernel::InputTask::Pause()
{
}

void SORE_Kernel::InputTask::Resume()
{
}
