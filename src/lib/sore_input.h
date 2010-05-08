/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifndef  SORE_INPUT_H
#define  SORE_INPUT_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <map>
#include <vector>
#include <queue>

#include <boost/function.hpp>
#include <SDL/SDL_keysym.h>
#include <SDL/SDL_events.h>

#include <sore_task.h>

namespace SORE_Kernel
{
	const unsigned int NOEVENT         = 0x000;
	const unsigned int MOUSEMOVE       = 0x001;
	const unsigned int MOUSEBUTTONDOWN = 0x002;
	const unsigned int MOUSEBUTTONUP   = 0x004;
	const unsigned int MOUSECLICK      = 0x008;
	const unsigned int KEYDOWN         = 0x010;
	const unsigned int KEYUP           = 0x020;
	const unsigned int KEYCLICK        = 0x040;

	//used for SORE_GUI
	const unsigned int MOUSEENTER      = 0x080;
	const unsigned int MOUSELEAVE      = 0x100;
	
	//window resize is called when the user physically resizes the window
	//this really should not be called by any external functions
	const unsigned int WINDOWRESIZE    = 0x200;
	//resize is called when the actual screen is resized by *any* method
	const unsigned int RESIZE          = 0x400;
	
	const unsigned int MOUSE_BUTTON1   = 0x01;
	const unsigned int MOUSE_BUTTON2   = 0x02;
	const unsigned int MOUSE_BUTTON3   = 0x04;
	const unsigned int MOUSE_WHEELDOWN = 0x08;
	const unsigned int MOUSE_WHEELUP   = 0x10;

	const unsigned int INPUT_ALL       = MOUSEMOVE | MOUSEBUTTONDOWN | MOUSEBUTTONUP 
		| MOUSECLICK | KEYDOWN | KEYUP | KEYCLICK | MOUSEENTER | MOUSELEAVE;

	const unsigned int INPUT_ALLMOUSE  = MOUSEMOVE | MOUSEBUTTONDOWN | MOUSEBUTTONUP | MOUSECLICK | MOUSEENTER | MOUSELEAVE;
	
	struct MouseInfo
	{
		unsigned int x,y;
		int          xmove,ymove;
		unsigned int buttonState;
	};
	
	struct KeyInfo
	{
		unsigned int keySym;
		unsigned int modState;
		unsigned int unicode;
	};
	
	struct ResizeInfo
	{
		int w,h;
	};
	
	struct Event
	{
		unsigned int type;
		KeyInfo key;
		MouseInfo mouse;
		ResizeInfo resize;
	};
	
	typedef std::vector<std::pair<unsigned int, boost::function<bool (Event*)> > > event_map;

	class SORE_EXPORT InputTask;

	struct event_listener_ref
	{
	private:
		std::vector<event_map>::iterator stackPos;
		event_map::iterator event;
		friend class SORE_EXPORT InputTask;
	};
	
	class SORE_EXPORT InputTask : public Task
	{
	public:
		InputTask();
		~InputTask();
			
		void Frame(int elapsedTime);
		void Pause();
		void Resume();
			
		const char* GetName() const {return "Input task";}
			
		event_listener_ref AddListener(unsigned int eventType, boost::function<bool (Event*)> functor);
		void RemoveListener(event_listener_ref listener);
		void InjectEvent(SORE_Kernel::Event e);

		bool QuitEventReceived() const;

		//This "state" is used by Gamestate. Only input handlers in the current state are respected 
		//(with Resize events as an exception)
		void PushState();
		void PopState();
	private:
		//translate the given event and place it into event
		void TranslateEvent(SDL_Event& sdl_event);
		//processes the member variable event
		void HandleEvent(Event& event);

		std::vector<
			event_map
			> allListeners;
		//reference to the current listener list
		std::vector<event_map>::iterator currentListeners;

		Event event;
		std::queue<Event> injectedEvents;

		bool quitEvent;
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
