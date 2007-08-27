//
// C++ Interface: sore_input
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_kernel.h"
#include "sore_allgl.h"
#include <map>

namespace SORE_Kernel
{
	const unsigned int MOUSEMOVE       = 0x01;
	const unsigned int MOUSEBUTTONDOWN = 0x02;
	const unsigned int MOUSEBUTTONUP   = 0x04;
	const unsigned int MOUSECLICK      = 0x08;
	const unsigned int KEYDOWN         = 0x10;
	const unsigned int KEYUP           = 0x20;
	const unsigned int KEYCLICK        = 0x40;
	
	const unsigned int MOUSE_BUTTON1   = 0x01;
	const unsigned int MOUSE_BUTTON2   = 0x02;
	const unsigned int MOUSE_BUTTON3   = 0x04;
	
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
	};
	
	struct Event
	{
		unsigned int type;
		KeyInfo key;
		MouseInfo mouse;
	};
	
	typedef unsigned int event_listener_ref;
	typedef bool (*EVENT_LISTENER)(Event*);
	
	class InputTask : public Task
	{
		public:
			InputTask();
			~InputTask();
			
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
			
			const char* GetName() const {return "Input task";}
			
			event_listener_ref AddListener(unsigned int eventType, EVENT_LISTENER listener);
			void RemoveListener(event_listener_ref listener);
		protected:
			Event event;
			std::multimap<unsigned int, EVENT_LISTENER> allListeners;
	};
}
