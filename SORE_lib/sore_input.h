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

namespace SORE_Input
{
	
	typedef unsigned int event_listener;
	typedef int(*EVENT_LISTENER)(Event*);
	event_listener AddListener(EVENT_LISTENER listener);
	void RemoveListener(event_listener listener);
}