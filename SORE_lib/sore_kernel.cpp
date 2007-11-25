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

#include <cassert>
#include "sore_kernel.h"
#include "sore_logger.h"
#include "sore_allgl.h"

SORE_Kernel::Task::Task(GameKernel* _gk)
{
	gk = _gk;
}

SORE_Kernel::GameKernel::GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel initialized");
	lastTicks = SDL_GetTicks();
	quitFlag = false;
	lastTicks = 0;
	paused = false;
}

SORE_Kernel::GameKernel::~GameKernel()
{
	ENGINE_LOG_S(SORE_Logging::LVL_INFO, "Kernel destroyed");
}

void SORE_Kernel::GameKernel::Frame()
{
	if(paused) return;
	const int maxDeltaT = 2000; //if frame time is over 2 seconds don't run frame
	task_ref it;
	int ticks = SDL_GetTicks();
	int deltaT = ticks - lastTicks;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		if(deltaT<maxDeltaT)
		{
			if(it->second.ms>0)
			{
				it->second.accum += deltaT;
				while(it->second.accum>it->second.ms)
				{
					it->second.task->Frame(it->second.ms);
					it->second.accum -= it->second.ms;
				}
			}
			else
				it->second.task->Frame(deltaT);
		}
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
		it->second.task->Pause();
	}
	paused = true;
}

void SORE_Kernel::GameKernel::Resume()
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		it->second.task->Resume();
	}
	paused = false;
}

SORE_Kernel::task_ref SORE_Kernel::GameKernel::AddConstTask(unsigned int priority, unsigned int ms, Task* task)
{
	assert(ms!=0 && "Milliseconds must be greater than 0");
	const_task newTask;
	newTask.task = task;
	newTask.ms = ms;
	newTask.accum = 0;
	task_ref size = tasks.insert(std::pair<unsigned int, const_task>(priority, newTask));
	return size;
}

SORE_Kernel::task_ref SORE_Kernel::GameKernel::AddTask(unsigned int priority, SORE_Kernel::Task* task)
{
	const_task newTask;
	newTask.task = task;
	newTask.ms = 0;
	newTask.accum = 0;
	task_ref size = tasks.insert(std::pair<unsigned int, const_task>(priority, newTask));
	return size;
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(SORE_Kernel::task_ref task)
{
	Task* temp = task->second.task;
	tasks.erase(task);
	return temp;
}

SORE_Kernel::Task* SORE_Kernel::GameKernel::RemoveTask(const char* taskName)
{
	task_ref it;
	for(it=tasks.begin();it!=tasks.end();it++)
	{
		if(strcmp(it->second.task->GetName(), taskName)==0)
		{
			Task* temp = it->second.task;
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
