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

#ifndef  SORE_GAMEKERNEL_H
#define  SORE_GAMEKERNEL_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>
#include <map>

#include <sore_dll.h>
#include <sore_task.h>

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
