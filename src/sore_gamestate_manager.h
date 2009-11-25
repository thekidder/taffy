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

#ifndef SORE_GAMESTATE_MANAGER_H
#define SORE_GAMESTATE_MANAGER_H

#include <vector>

#include "sore_renderer_std.h"
#include "sore_input.h"
#include "sore_resource.h"
#include "sore_screen.h"
#include "sore_gamestate.h"
#include "sore_gamekernel.h"

namespace SORE_Game
{
    class SORE_EXPORT GamestateManager
    {
    public:
        GamestateManager(SORE_Kernel::GameKernel& gk,
                         SORE_FileIO::PackageCache* pc = NULL,
                         std::string windowTitle = "SORE Framework Application",
                         std::string settingsFile = "");
        ~GamestateManager();

        //use new to instantiate newState; the Manager will handle its deallocation
        void PushState(Gamestate* newState);
        void PopState();
        //run until a task requests exit or until there are no states left
        int Run();

        SORE_Graphics::Renderer* GetRenderer();
        SORE_Kernel::InputTask* GetInputTask();
        SORE_Resource::ResourcePool& GetPool();
        SORE_Kernel::Screen* GetScreen();
    private:
        void Pop();

        SORE_Kernel::GameKernel& kernel;
        SORE_Graphics::Renderer* renderer;
        SORE_Kernel::InputTask input;
        SORE_Resource::ResourcePool pool;

        SORE_Utility::IniSettingsBackend ini;
        SORE_Utility::SettingsManager sm;

        SORE_Kernel::Screen screen;

        SORE_Kernel::task_ref curr;
        std::vector<std::pair<SORE_Kernel::task_ref, Gamestate*> > states;

        //should we pop the state on frame end?
        bool popFlag;
    };
}

#endif
