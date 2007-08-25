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
#include "allgl.h"

namespace SORE_Kernel
{
	struct Event
	{
	};
	
	typedef unsigned int event_listener_ref;
	typedef int(*EVENT_LISTENER)(Event*);
	
	class InputTask : public Task
	{
		public:
			InputTask();
			~InputTask();
			
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
			
			const char* GetName() const {return "Input task";}
			
			event_listener_ref AddListener   (EVENT_LISTENER listener);
			void RemoveListener(event_listener_ref listener);
		protected:
			SDL_Event event;
	};
}
