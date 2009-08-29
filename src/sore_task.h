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
// $Id$

#ifndef  SORE_TASK_H
#define  SORE_TASK_H

#include "sore_dll.h"

namespace SORE_Kernel
{	
	class SORE_EXPORT Task
	{
	public:
		Task();
		virtual ~Task() {}
		
		bool RequestQuit() const;
	
		virtual void Frame(int elapsedTime) = 0;
		virtual void Pause() {}
		virtual void Resume() {}
		virtual const char* GetName() const = 0;
	protected:
		bool quitFlag;
	};
	
	struct const_task
	{
		Task* task;
		unsigned int ms;
		unsigned int accum;

		bool paused;
	};
}

#endif
