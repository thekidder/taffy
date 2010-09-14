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

#ifndef SORE_GAMESTATE_MANAGER_H
#define SORE_GAMESTATE_MANAGER_H

#include <vector>

#include <sore_pipeline_renderer.h>
#include <sore_input.h>
#include <sore_resource.h>
#include <sore_screen.h>
#include <sore_gamestate.h>
#include <sore_gamekernel.h>

namespace SORE_Game
{
    class SORE_EXPORT GamestateManager
    {
    public:
        GamestateManager(SORE_FileIO::PackageCache* pc = NULL,
                         const std::string& windowTitle = "SORE Framework Application",
                         const std::string& iconFilename = "",
                         const std::string& settingsFile = "");
        ~GamestateManager();

        //use new to instantiate newState; the Manager will handle its deallocation
        void PushState(Gamestate* newState);
        void PopState();
        //run until a task requests exit or until there are no states left
        int Run();

        SORE_Graphics::IRenderer* GetRenderer();
        SORE_Kernel::InputTask& GetInputTask();
        SORE_Resource::ResourcePool& GetPool();
        SORE_Kernel::Screen& GetScreen();

        bool FileNotifySupported() const;
    private:
        void Pop();

        SORE_Kernel::GameKernel kernel;

        SORE_Utility::IniSettingsBackend ini;
        SORE_Utility::SettingsManager sm;

        SORE_Kernel::InputTask input;

        SORE_Kernel::Screen screen; //depends on input, sm
        SORE_Graphics::PipelineRenderer renderer; //must be init after screen

#ifdef FilesystemWatcherTask
        SORE_FileIO::FilesystemWatcherTask watcher;
#endif
        SORE_Resource::ResourcePool pool; //needs watcher (if defined)

        SORE_Kernel::task_ref curr;
        std::vector<std::pair<SORE_Kernel::task_ref, Gamestate*> > states;

        //should we pop the state on frame end?
        bool popFlag;
    };
}

#endif
