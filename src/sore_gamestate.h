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


#ifndef SORE_GAMESTATE_H
#define SORE_GAMESTATE_H

#include "sore_task.h"

namespace SORE_Game
{
	class GamestateManager;

	class Gamestate : public SORE_Kernel::Task
	{
	public:
		Gamestate(int ms = -1); //ms is the time interval for const tasks: -1 if a regular task

		void Initialize(GamestateManager* o);
		int GetInterval() const;
	protected:
		void PushState(Gamestate* newState);
		//make sure to return directly after calling PopState
		void PopState();

		GamestateManager* owner;
	private:
		//called when we receive our GamestateManager pointer
		virtual void Init() = 0;

		int interval;
	};
}

#endif
