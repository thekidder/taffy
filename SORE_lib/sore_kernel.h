
#include <vector>

namespace SORE_Kernel
{
	typedef int task_ref;
	
	class Task
	{
		public:
			Task();
			virtual ~Task() = 0;
			
			virtual void Frame()  = 0;
			virtual void Pause()  = 0;
			virtual void Resume() = 0;
	};
	
	class GameKernel
	{
		public:
			GameKernel();
			~GameKernel();
			
			void Pause();
			void Resume();
				
			task_ref AddTask(Task* task);
			void     RemoveTask(task_ref task);
		protected:
			std::vector<Task*> taskList;
	};
}
