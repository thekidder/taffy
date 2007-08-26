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

SORE_Kernel::InputTask::InputTask()
{
}

SORE_Kernel::InputTask::~InputTask()
{
}

void SORE_Kernel::InputTask::Frame(int elapsedTime)
{
	SDL_Event sdl_event;
	while(SDL_PollEvent(&sdl_event))
	{
		switch(event.type)
		{
			case SDL_QUIT:
			{
				GameKernel* gk = GameKernel::GetKernel();
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
				event.type = MOUSEBUTTONDOWN;
				event.mouse.buttonState = sdl_event.button.button;
				break;
			case SDL_MOUSEBUTTONUP:
				event.type = MOUSEBUTTONUP;
				event.mouse.buttonState = sdl_event.button.button;
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
		}
		std::multimap<unsigned int, EVENT_LISTENER>::iterator it;
		for(it=allListeners.begin();it!=allListeners.end();it++)
		{
			if(it->first & event.type)
			{
				it->second(&event);
			}
		}
	}
}

void SORE_Kernel::InputTask::Pause()
{
}

void SORE_Kernel::InputTask::Resume()
{
}
