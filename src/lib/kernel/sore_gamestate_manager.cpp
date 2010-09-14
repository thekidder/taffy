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
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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

    GamestateManager::GamestateManager(SORE_FileIO::PackageCache* pc,
                                       const std::string& windowTitle,
                                       const std::string& iconFilename,
                                       const std::string& settingsFile)
        : ini(settingsFile), sm(&ini),
        screen(screenInfo, input, windowTitle, iconFilename, &sm),
        pool(pc
#ifdef FilesystemWatcherTask
        , &watcher
#endif
        ), popFlag(false)
    {
        curr = kernel.end();

        kernel.AddTask(0, &input);
        kernel.AddTask(10, &screen);
#ifdef FilesystemWatcherTask
        kernel.AddTask(20, &watcher);
#endif

        screen.SetRenderer(&renderer);
    }

    GamestateManager::~GamestateManager()
    {
    }

    void GamestateManager::Pop()
    {
        input.PopState();
        renderer.PopState();
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
        renderer.PushState();
        newState->Initialize(this);

        kernel.PauseTask(curr);

        if(newState->GetInterval() == -1)
            curr = kernel.AddTask(50, newState);
        else
            curr = kernel.AddConstTask(50, newState->GetInterval(), newState);
        states.push_back(std::make_pair(curr, newState));

        //ugly hack! get proper resize in every state
        input.AddListener(
            SORE_Kernel::WINDOWRESIZE,
            std::bind1st(std::mem_fun(&SORE_Kernel::Screen::OnResize), &screen));
        input.AddListener(
            SORE_Kernel::RESIZE,
            std::bind1st(std::mem_fun(&SORE_Resource::ResourcePool::OnResize),
                          &pool));
    }

    SORE_Graphics::IRenderer* GamestateManager::GetRenderer()
    {
        return &renderer;
    }

    SORE_Kernel::InputTask& GamestateManager::GetInputTask()
    {
        return input;
    }

    SORE_Kernel::Screen& GamestateManager::GetScreen()
    {
        return screen;
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

    bool GamestateManager::FileNotifySupported() const
    {
#ifdef FilesystemWatcherTask
        return true;
#else
        return false;
#endif
    }
}
