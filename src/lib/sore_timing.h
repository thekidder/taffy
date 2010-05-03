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

#ifndef  SORE_TIMING_H
#define  SORE_TIMING_H

#include "sore_dll.h"

namespace SORE_Timing
{
	unsigned int SORE_EXPORT GetGlobalTicks(); // in 1/10000 sec
	unsigned int SORE_EXPORT GetGlobalMS(); //provided for convenience 
	
	void SORE_EXPORT Sleep(unsigned int ms);

	//implements a timer that starts when it comes in scope
	class SORE_EXPORT Timer
	{
	public:
		Timer();

		void Reset();
		float GetSeconds() const;
		unsigned int GetMS() const;
	private:
		unsigned int startTicks;
	};
}

#endif
