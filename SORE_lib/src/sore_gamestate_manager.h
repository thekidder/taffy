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

#ifndef SORE_GAMESTATE_MANAGER_H
#define SORE_GAMESTATE_MANAGER_H

#include <vector>

#include "sore_renderer2d.h"
#include "sore_input.h"
#include "sore_resource.h"
#include "sore_screen.h"
#include "sore_gamestate.h"
#include "sore_gamekernel.h"

namespace SORE_Game
{
	class GamestateManager
	{
	public:
		GamestateManager(SORE_Kernel::GameKernel& gk, std::string windowTitle = "SORE Framework Application");
		~GamestateManager();

		//use new to instantiate newState; the Manager will handle its deallocation
		void PushState(Gamestate* newState);
		void PopState();
		//run until a task requests exit or until there are no states left
		int Run();

		SORE_Graphics::Renderer2D* GetRenderer();
		SORE_Kernel::InputTask* GetInputTask();
		SORE_Resource::ResourcePool& GetPool();
		SORE_Kernel::Screen* GetScreen();
	private:
		void Pop();

		SORE_Kernel::GameKernel& kernel;
		SORE_Graphics::Renderer2D* renderer;
		SORE_Kernel::InputTask input;
		SORE_Resource::ResourcePool pool;

		SORE_Kernel::Screen screen;

		std::vector<std::pair<SORE_Kernel::task_ref, Gamestate*> > states;

		//should we pop the state on frame end?
		bool popFlag;
	};
}

#endif
