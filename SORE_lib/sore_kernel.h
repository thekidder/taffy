//
// C++ Interface: sore_kernel
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#ifndef  __SORE_KERNEL_H__
#define  __SORE_KERNEL_H__

#include "sore_task.h"
#include "sore_interpolater.h"
#include <vector>
#include <map>
#include <memory>

namespace SORE_Kernel
{
	typedef std::multimap<unsigned int, const_task> task_list;
	
	typedef task_list::iterator task_ref;
	
	class GameKernel
	{
		public:
			GameKernel();
			~GameKernel();
			
			void      Pause();
			void      Frame();
			void      Resume();
			
			bool      Running();
			
			task_ref  AddTask      (unsigned int priority, Task* task);
			task_ref  AddConstTask (unsigned int priority, unsigned int ms, Task* task);
			Task*     RemoveTask(task_ref task);
			Task*     RemoveTask(const char* taskName);
			void      RemoveAllTasks();
			
			task_list GetTasksByName(const char* taskName);
			void      PrintTasks(int lvl);
			
			bool     quitFlag;
			
			SORE_Utility::interpolater_iterator AddInterpolater(boost::shared_ptr<SORE_Utility::IInterpolater> i);
			void RemoveInterpolater(SORE_Utility::interpolater_iterator i);
		protected:
			task_list tasks;
			//std::multimap<unsigned int, const_task> constTasks;
			int lastTicks;
			bool paused;
			SORE_Utility::InterpolaterTask* itask;
	};
}

#endif /*__SORE_KERNEL_H__*/
