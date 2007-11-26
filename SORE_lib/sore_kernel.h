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
//

#ifndef  __SORE_KERNEL_H__
#define  __SORE_KERNEL_H__

#include <vector>
#include <map>

namespace SORE_Kernel
{
	class GameKernel;
	
	class Task
	{
		public:
			Task(GameKernel* _gk);
			virtual ~Task() {}
			
			virtual void Frame(int elapsedTime)  = 0;
			virtual void Pause()  = 0;
			virtual void Resume() = 0;
			
			virtual const char* GetName() const = 0;
		protected:
			GameKernel* gk;
	};
	
	struct const_task
	{
		Task* task;
		unsigned int ms;
		unsigned int accum;
	};
	
	typedef std::multimap<unsigned int, const_task>::iterator task_ref;
	
	class GameKernel
	{
		public:
			GameKernel();
			~GameKernel();
			
			void     Pause();
			void     Frame();
			void     Resume();
			
			bool     Running();
			
			task_ref AddTask      (unsigned int priority, Task* task);
			task_ref AddConstTask (unsigned int priority, unsigned int ms, Task* task);
			Task*    RemoveTask(task_ref task);
			Task*    RemoveTask(const char* taskName);
			void     RemoveAllTasks();
			bool     quitFlag;
		protected:
			std::multimap<unsigned int, const_task> tasks;
			//std::multimap<unsigned int, const_task> constTasks;
			int lastTicks;
			bool paused;
	};
}

#endif /*__SORE_KERNEL_H__*/
