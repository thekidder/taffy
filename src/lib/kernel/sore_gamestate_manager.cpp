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

#include <sore_gamestate_manager.h>
#include <sore_timing.h>

namespace SORE_Game
{
    static SORE_Graphics::ScreenInfo screenInfo =
    {
        1024,
        768,
        1024.0f/768.0f,
        true,
        false,
        false
    };

    GamestateManager::GamestateManager(SORE_Kernel::GameKernel& gk,
                                       SORE_FileIO::PackageCache* pc,
                                       const std::string& windowTitle,
									   const std::string& iconFilename,
                                       const std::string& settingsFile)
        : kernel(gk), pool(pc
#ifdef linux
		, &watcher
#endif
		), ini(settingsFile), sm(&ini),
          screen(screenInfo, input, windowTitle, iconFilename, &sm), popFlag(false)
    {
        curr = gk.end();

        gk.AddTask(0, &input);
        gk.AddTask(10, &screen);
#ifdef linux
        gk.AddTask(20, &watcher);
#endif

        renderer = new SORE_Graphics::Renderer(pool);
        screen.SetRenderer(renderer);
        input.AddListener(
            SORE_Kernel::WINDOWRESIZE,
            std::bind1st(std::mem_fun(&SORE_Kernel::Screen::OnResize), &screen));
        input.AddListener(
            SORE_Kernel::RESIZE,
            std::bind1st(std::mem_fun(&SORE_Resource::ResourcePool::OnResize),
                          &pool));
    }

    GamestateManager::~GamestateManager()
    {
        delete renderer;
    }

    void GamestateManager::Pop()
    {
        input.PopState();
        renderer->PopState();
        kernel.RemoveTask(states.back().first);
        delete states.back().second;
        states.pop_back();

        if(states.size())
            curr = states.back().first;
        else
            curr = kernel.end();

        kernel.ResumeTask(curr);

        popFlag = false;
    }

    void GamestateManager::PopState()
    {
        popFlag = true;
    }

    void GamestateManager::PushState(Gamestate* newState)
    {
        input.PushState();
        renderer->PushState();
        newState->Initialize(this);

        kernel.PauseTask(curr);

        if(newState->GetInterval() == -1)
            curr = kernel.AddTask(50, newState);
        else
            curr = kernel.AddConstTask(50, newState->GetInterval(), newState);
        states.push_back(std::make_pair(curr, newState));
    }

    SORE_Graphics::Renderer* GamestateManager::GetRenderer()
    {
        return renderer;
    }

    SORE_Kernel::InputTask* GamestateManager::GetInputTask()
    {
        return &input;
    }

    SORE_Kernel::Screen* GamestateManager::GetScreen()
    {
        return &screen;
    }

    SORE_Resource::ResourcePool& GamestateManager::GetPool()
    {
        return pool;
    }

    int GamestateManager::Run()
    {
        if(kernel.ShouldQuit())
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize one "
                       "or more subsystems - now exiting");
            return 1;
        }

        unsigned int maxfps = 1000;
        unsigned int lastTicks = SORE_Timing::GetGlobalTicks();
        while(!kernel.ShouldQuit() && states.size() && !input.QuitEventReceived())
        {
            unsigned int ticks = SORE_Timing::GetGlobalTicks();
            unsigned int time = ticks - lastTicks;
            double fps = 10000.0/double(time);
            if(fps>static_cast<double>(maxfps))
            {
                SORE_Timing::Sleep(0);
                continue;
            }
            lastTicks = ticks;

            kernel.Frame();

            //if a PopState is requested, we delay it until the end of the
            //frame, to make sure no invalid memory accessed are performed
            if(popFlag)
                Pop();
        }
        //properly clean up all existing states
        while(states.size()) Pop();

        return 0;
    }
}
