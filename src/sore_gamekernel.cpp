/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#include <cassert>
#include <cstring>

#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include "sore_gamekernel.h"
#include "sore_logger.h"
#include "sore_timing.h"

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
