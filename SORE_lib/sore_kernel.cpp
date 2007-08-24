
#include "sore_kernel.h"
#include "sore_logger.h"

SORE_Kernel::GameKernel* SORE_Kernel::GameKernel::gk = NULL;

SORE_Kernel::Task::Task()
{
}

SORE_Kernel::GameKernel* SORE_Kernel::GameKernel::GetKernel()
{
	if(gk == NULL)
	{
		gk = new GameKernel();
	}
	return gk;
}

SORE_Kernel::GameKernel::GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel initialized");
}

SORE_Kernel::GameKernel::~GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel destroyed");
}

void SORE_Kernel::GameKernel::Pause()
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		it->second->Pause();
	}
}

void SORE_Kernel::GameKernel::Resume()
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		it->second->Resume();
	}
}

SORE_Kernel::task_ref SORE_Kernel::GameKernel::AddTask(unsigned int priority, SORE_Kernel::Task* task)
{
	tasks.insert(std::pair<unsigned int, Task*>(priority, task));
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(SORE_Kernel::task_ref task)
{
	tasks.erase(task);
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(const char* taskName)
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		if(strcmp(it->second->GetName(), taskName)==0)
		{
			tasks.erase(it);
		}
	}
}
