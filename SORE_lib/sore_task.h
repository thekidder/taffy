// $Id$

#ifndef  __SORE_TASK_H__
#define  __SORE_TASK_H__

namespace SORE_Kernel
{
	class GameKernel;
	
	class Task
	{
		public:
			Task(GameKernel* _gk);
			virtual ~Task() {}
			
			virtual void Frame(int elapsedTime)  = 0;
			virtual void Pause() {}
			virtual void Resume() {}
			
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
}

#endif /*__SORE_TASK_H__*/
