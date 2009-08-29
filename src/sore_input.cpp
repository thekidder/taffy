/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#include "sore_input.h"
#include "sore_logger.h"
namespace SORE_Kernel
{
	InputTask::InputTask() : quitEvent(false)
	{
		event.mouse.buttonState = 0x00;
		PushState();
	}

	InputTask::~InputTask()
	{
	}

	bool InputTask::QuitEventReceived() const
	{
		return quitEvent;
	}

	void InputTask::TranslateEvent(SDL_Event& sdl_event)
	{
		switch(sdl_event.type)
		{
		case SDL_QUIT:
		{
			quitEvent = true;
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
			switch(sdl_event.button.button)
			{
			case SDL_BUTTON_LEFT:
				event.mouse.buttonState = MOUSE_BUTTON1;
				break;
			case SDL_BUTTON_MIDDLE:
				event.mouse.buttonState = MOUSE_BUTTON3;
				break;
			case SDL_BUTTON_RIGHT:
				event.mouse.buttonState = MOUSE_BUTTON2;
				break;
			case SDL_BUTTON_WHEELDOWN:
				event.mouse.buttonState = MOUSE_WHEELDOWN;
				break;
			case SDL_BUTTON_WHEELUP:
				event.mouse.buttonState = MOUSE_WHEELUP;
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			event.type = MOUSEBUTTONUP;
			switch(sdl_event.button.button)
			{
			case SDL_BUTTON_LEFT:
				event.mouse.buttonState = MOUSE_BUTTON1;
				break;
			case SDL_BUTTON_MIDDLE:
				event.mouse.buttonState = MOUSE_BUTTON3;
				break;
			case SDL_BUTTON_RIGHT:
				event.mouse.buttonState = MOUSE_BUTTON2;
				break;
			case SDL_BUTTON_WHEELDOWN:
				event.mouse.buttonState = MOUSE_WHEELDOWN;
				break;
			case SDL_BUTTON_WHEELUP:
				event.mouse.buttonState = MOUSE_WHEELUP;
				break;
			}
			break;
		case SDL_KEYDOWN:
			event.type = KEYDOWN;
			event.key.keySym = sdl_event.key.keysym.sym;
			event.key.modState = sdl_event.key.keysym.mod;
			event.key.unicode = sdl_event.key.keysym.unicode;
			break;
		case SDL_KEYUP:
			event.type = KEYUP;
			event.key.keySym = sdl_event.key.keysym.sym;
			event.key.modState = sdl_event.key.keysym.mod;
			event.key.unicode = sdl_event.key.keysym.unicode;
			break;
		case SDL_VIDEORESIZE:
			event.type = WINDOWRESIZE;
			event.resize.w = sdl_event.resize.w;
			event.resize.h = sdl_event.resize.h;
			break;
		default: //invalid event type
			event.type = NOEVENT;
			return;
		}
	}

	void InputTask::HandleEvent(Event& event)
	{
		if(event.type == NOEVENT) return;
		for(event_map::iterator it=currentListeners->begin();it!=currentListeners->end();it++)
		{
			if(it->first & event.type && event.type != RESIZE)
			{
				bool result = (it->second)(&event); 
				if(result) //ALWAYS propagate resize events...can be used by more than one function
						return;
			}
		}
		if(event.type == RESIZE)
		{
			//separate iteration for resize events in previous stack locations
			std::vector<event_map>::iterator stackIter;
			std::vector<event_map>::iterator stackEnd = allListeners.end();
			for(stackIter=allListeners.begin();stackIter!=stackEnd;++stackIter)
			{
				event_map::iterator it;
				for(it=stackIter->begin();it!=stackIter->end();it++)
				{
					if(it->first & event.type)
					{
						(it->second)(&event);
					}
				}
			}
		}
	}

	void InputTask::Frame(int elapsedTime)
	{
		//handle injected events first
		while(injectedEvents.size())
		{
			Event& e = injectedEvents.front();
			HandleEvent(e);
			injectedEvents.pop();
		}

		SDL_Event sdl_event;
		while(SDL_PollEvent(&sdl_event))
		{
			TranslateEvent(sdl_event);
			HandleEvent(event);
		}
	}

	event_listener_ref InputTask::AddListener(unsigned int eventType, boost::function<bool (Event*)> functor)
	{
		event_listener_ref curr;
		curr.stackPos = currentListeners;

		currentListeners->push_back(std::make_pair(eventType, functor));

		curr.event = currentListeners->end() - 1;
		return curr;
	}

	void InputTask::RemoveListener(event_listener_ref ref)
	{
		ref.stackPos->erase(ref.event);
	}

	void InputTask::InjectEvent(SORE_Kernel::Event e)
	{
		injectedEvents.push(e);
	}

	void InputTask::PushState()
	{
		event_map temp;
		allListeners.push_back(temp);
		currentListeners = allListeners.end() - 1;
	}

	void InputTask::PopState()
	{
		allListeners.pop_back();
		currentListeners = allListeners.end() - 1;
	}

	void InputTask::Pause()
	{
	}

	void InputTask::Resume()
	{
	}
}
