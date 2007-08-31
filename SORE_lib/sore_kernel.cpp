//
// C++ Implementation: sore_kernel
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
#include "sore_logger.h"
#include "sore_allgl.h"

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

void SORE_Kernel::GameKernel::Cleanup()
{
	delete gk;
}

SORE_Kernel::GameKernel::GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel initialized");
	lastTicks = SDL_GetTicks();
	quitFlag = false;
}

SORE_Kernel::GameKernel::~GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel destroyed");
}

void SORE_Kernel::GameKernel::Frame()
{
	int ticks = SDL_GetTicks();
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		it->second->Frame(ticks - lastTicks);
	}
#ifdef DEBUG
	SORE_Logging::sore_log.Flush();
#endif
	lastTicks = ticks;
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
	task_ref size = tasks.insert(std::pair<unsigned int, Task*>(priority, task));
	return size;
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(SORE_Kernel::task_ref task)
{
	Task* temp = task->second;
	tasks.erase(task);
	return temp;
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(const char* taskName)
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		if(strcmp(it->second->GetName(), taskName)==0)
		{
			Task* temp = it->second;
			tasks.erase(it);
			return temp;
		}
	}
	return NULL;
}

void SORE_Kernel::GameKernel::RemoveAllTasks()
{
	tasks.clear();
}
