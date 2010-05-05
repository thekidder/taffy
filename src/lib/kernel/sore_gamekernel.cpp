/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <cassert>
#include <cstring>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <sore_gamekernel.h>
#include <sore_logger.h>
#include <sore_timing.h>

namespace SORE_Kernel
{
	GameKernel::GameKernel()
	{
		ENGINE_LOG(SORE_Logging::LVL_INFO, "Kernel initialized");
		lastTicks = SORE_Timing::GetGlobalTicks();
		lastTicks = 0;
		paused = false;
	}

	GameKernel::~GameKernel()
	{
		ENGINE_LOG(SORE_Logging::LVL_INFO, "Kernel destroyed");
	}

	void GameKernel::Frame()
	{
		if(paused) return;
		const int maxDeltaT = 20000; //if frame time is over 2 seconds don't run frame
		task_ref it;
		int ticks = SORE_Timing::GetGlobalTicks();
		int deltaT = ticks - lastTicks;
		if(deltaT<maxDeltaT)
		{
			for(it=tasks.begin();it!=tasks.end();it++)
			{
				if(it->second.ms>0)
				{
					it->second.accum += deltaT;
					while(it->second.accum>it->second.ms*10)
					{
						if(!it->second.paused)
						{
							it->second.task->Frame(it->second.ms*10);
							it->second.accum -= it->second.ms*10;
						}
					}
				}
				else
					if(!it->second.paused)
						it->second.task->Frame(deltaT);
			}
		}
#ifdef DEBUG
		SORE_Logging::sore_log.Flush();
#endif
		lastTicks = ticks;
	}

	void GameKernel::Pause()
	{
		task_ref it;
		for(it=tasks.begin();it!=tasks.end();it++)
		{
			PauseTask(it);
		}
		paused = true;
	}

	void GameKernel::Resume()
	{
		task_ref it;
		for(it=tasks.begin();it!=tasks.end();it++)
		{
			ResumeTask(it);
		}
		paused = false;
	}

	bool GameKernel::Running()
	{
		return !paused;
	}

	task_ref GameKernel::AddConstTask(unsigned int priority, unsigned int ms, Task* task)
	{
		assert(ms!=0 && "Milliseconds must be greater than 0");
		const_task newTask;
		newTask.task = task;
		newTask.ms = ms;
		newTask.accum = 0;
		newTask.paused = false;
		task_ref size = tasks.insert(std::pair<unsigned int, const_task>(priority, newTask));
		return size;
	}

	task_ref GameKernel::AddTask(unsigned int priority, SORE_Kernel::Task* task)
	{
		const_task newTask;
		newTask.task = task;
		newTask.ms = 0;
		newTask.accum = 0;
		newTask.paused = false;
		task_ref size = tasks.insert(std::pair<unsigned int, const_task>(priority, newTask));
		return size;
	}

	void GameKernel::PauseTask(SORE_Kernel::task_ref task)
	{
		if(task == tasks.end()) return;
		task->second.paused = true;
		task->second.task->Pause();
	}

	void GameKernel::ResumeTask(SORE_Kernel::task_ref task)
	{
		if(task == tasks.end()) return;
		task->second.paused = false;
		task->second.task->Resume();
	}

	task_ref GameKernel::end()
	{
		return tasks.end();
	}

	Task* GameKernel::RemoveTask(SORE_Kernel::task_ref task)
	{
		if(task == tasks.end()) return NULL;
		Task* temp = task->second.task;
		tasks.erase(task);
		return temp;
	}

	Task* GameKernel::RemoveTask(const char* taskName)
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

	task_list GameKernel::GetTasksByName(const char* taskName)
	{
		task_list list;
		for(task_ref it=tasks.begin();it!=tasks.end();it++)
		{
			if(strcmp(it->second.task->GetName(), taskName)==0)
			{
				list.insert(std::pair<unsigned int, const_task>(it->first, it->second));
			}
		}
		return list;
	}

	void GameKernel::PrintTasks(int lvl)
	{
		std::string taskList  = "Printing all registered tasks...\n";
		taskList             += "----------------------------------------\n";
		taskList             += "Priority\tUpdate (ms)\tName\n";
		for(task_ref it=tasks.begin();it!=tasks.end();it++)
		{
			taskList += boost::lexical_cast<std::string>(it->first);
			taskList += "\t\t";
			taskList += boost::lexical_cast<std::string>(it->second.ms/10);
			taskList += "\t\t";
			taskList += it->second.task->GetName();
			taskList += "\n";
		}
		taskList             += "----------------------------------------";
		ENGINE_LOG(lvl, taskList);
	}

	void GameKernel::RemoveAllTasks()
	{
		tasks.clear();
	}

	bool GameKernel::ShouldQuit() const
	{
		for(task_list::const_iterator it=tasks.begin();it!=tasks.end();++it)
		{
			if(it->second.task->RequestQuit())
			{
				return true;
			}
		}
		return false;
	}
}
