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

namespace SORE_Kernel
{
	struct Event
	{
	};
	
	typedef unsigned int event_listener;
	typedef int(*EVENT_LISTENER)(Event*);
	event_listener AddListener(EVENT_LISTENER listener);
	void RemoveListener(event_listener listener);
	
	class InputTask : public Task
	{
		public:
			InputTask();
			~InputTask();
			
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
			
			const char* GetName() const {return "Input task";}
	};
}
