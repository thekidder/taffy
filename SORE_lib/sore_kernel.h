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
	class Task
	{
		public:
			Task();
			virtual ~Task() {}
			
			virtual void Frame(int elapsedTime)  = 0;
			virtual void Pause()  = 0;
			virtual void Resume() = 0;
			
			virtual const char* GetName() const = 0;
		protected:
	};
	
	typedef std::multimap<unsigned int, Task*>::iterator task_ref;
	
	class GameKernel
	{
		public:
			static GameKernel* GetKernel();
			static void        Cleanup();
			
			void     Pause();
			void     Frame();
			void     Resume();
			
			task_ref AddTask(unsigned int priority, Task* task);
			Task*    RemoveTask(task_ref task);
			Task*    RemoveTask(const char* taskName);
			void     RemoveAllTasks();
			bool     quitFlag;
		protected:
			std::multimap<unsigned int, Task*> tasks;
			GameKernel();
			~GameKernel();
			static GameKernel* gk;
			int lastTicks;
	};
}

#endif /*__SORE_KERNEL_H__*/
