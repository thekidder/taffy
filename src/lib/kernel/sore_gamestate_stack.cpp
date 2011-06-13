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

#include <sore_gamestate_stack.h>
#include <sore_timing.h>

namespace SORE_Game
{
    static SORE_Graphics::ScreenInfo defaultScreenInfo =
    {
        1024,
        768,
        1024.0f/768.0f,
        true,
        false,
        false
    };

    GamestateStack::GamestateStack(
        const std::string& windowTitle,
        const std::string& iconFilename,
        const std::string& settingsFile)
        : ini(settingsFile), sm(&ini),
        screen(defaultScreenInfo, windowTitle, iconFilename, &sm),
        input(screen),
        fontCache(SORE_Resource::FontLoader(packageCache)),
        textureCache(SORE_Resource::Texture2DLoader(packageCache)),
        shaderCache(SORE_Resource::GLSLShaderLoader(packageCache)),
        popFlag(false)
    {
        kernel.AddTask(10, &input);
        // sf::window.Display() should be last call
        kernel.AddTask(1000, &screen);
    }

    GamestateStack::~GamestateStack()
    {
    }

    void GamestateStack::Pop()
    {
        kernel.RemoveTask(states.back().first);
        delete states.back().second;
        states.pop_back();

        if(states.size())
            kernel.ResumeTask(states.back().first);

        popFlag = false;
    }

    void GamestateStack::PopState()
    {
        popFlag = true;
    }

    void GamestateStack::PushState(Gamestate* newState)
    {
        if(states.size())
            kernel.PauseTask(states.back().first);

        SORE_Kernel::task_ref curr;
        if(newState->GetInterval() == -1)
            curr = kernel.AddTask(50, newState);
        else
            curr = kernel.AddConstTask(50, newState->GetInterval(), newState);
        states.push_back(std::make_pair(curr, newState));

        // put in a resize event so the state knows the window size
        SORE_Kernel::Event e;
        e.type = SORE_Kernel::RESIZE;
        e.resize.w = screen.GetScreen().width;
        e.resize.h = screen.GetScreen().height;
        newState->OnEvent(e);
    }

    int GamestateStack::Run()
    {
        if(kernel.ShouldQuit())
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize one "
                       "or more subsystems - now exiting");
            return 1;
        }

        unsigned int maxfps = 1000;
        unsigned int lastTicks = SORE_Kernel::GetGlobalTicks();
        while(!kernel.ShouldQuit() && states.size() && !input.QuitEventReceived())
        {
            unsigned int ticks = SORE_Kernel::GetGlobalTicks();
            unsigned int time = ticks - lastTicks;
            double fps = 10000.0/double(time);
            if(fps>static_cast<double>(maxfps))
            {
                SORE_Kernel::Sleep(0);
                continue;
            }
            lastTicks = ticks;

            // distribute events to the gamestates
            while(input.size())
            {
                const SORE_Kernel::Event& e = input.top();
                for(State_stack_t::reverse_iterator it = states.rbegin(); it != states.rend(); ++it)
                {
                    if(it->second->OnEvent(e))
                        break;

                }
                input.pop();
            }

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
