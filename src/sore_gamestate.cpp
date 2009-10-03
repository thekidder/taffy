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

#include "sore_logger.h"
#include "sore_gamestate.h"
#include "sore_gamestate_manager.h"

namespace SORE_Game
{
	Gamestate::Gamestate(int ms) : owner(0), interval(ms)
	{
	}

	int Gamestate::GetInterval() const
	{
		return interval;
	}

	void Gamestate::Initialize(GamestateManager* o)
	{
		owner = o;
		Init();
	}

	void Gamestate::PushState(Gamestate*  newState)
	{
		if(owner)
			owner->PushState(newState);
		else
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not push state; no GamestateManager");
	}

	void Gamestate::PopState()
	{
		if(owner)
			owner->PopState();
		else
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not pop state; no GamestateManager");
	}
}
