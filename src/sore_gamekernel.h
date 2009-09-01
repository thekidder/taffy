/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#ifndef  SORE_GAMEKERNEL_H
#define  SORE_GAMEKERNEL_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>
#include <map>

#include "sore_dll.h"
#include "sore_task.h"

namespace SORE_Kernel
{
	typedef std::multimap<unsigned int, const_task> task_list;
	typedef task_list::iterator task_ref;
	
	class SORE_EXPORT GameKernel
	{
	public:
		GameKernel();
		~GameKernel();
			
		void      Pause();
		void      Frame();
		void      Resume();
		void      PauseTask(task_ref task);
		void      ResumeTask(task_ref task);
			
		bool      Running();
			
		task_ref  AddTask      (unsigned int priority, Task* task);
		task_ref  AddConstTask (unsigned int priority, unsigned int ms, Task* task);
		Task*     RemoveTask(task_ref task);
		Task*     RemoveTask(const char* taskName);
		void      RemoveAllTasks();

		task_ref  end();
			
		task_list GetTasksByName(const char* taskName);
		void      PrintTasks(int lvl);

		bool ShouldQuit() const;
	private:
		task_list tasks;
		int lastTicks;
		bool paused;
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
