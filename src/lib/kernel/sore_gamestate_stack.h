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

#ifndef SORE_GAMESTATE_STACK_H
#define SORE_GAMESTATE_STACK_H

#include <sore_fileio.h>
#include <sore_font_loader.h>
#include <sore_gamestate.h>
#include <sore_glslshader_loader.h>
#include <sore_resourcecache.h>
#include <sore_screen.h>
#include <sore_settings.h>
#include <sore_texture2d_loader.h>
#include <sore_util.h>

#include <string>

namespace SORE_Game
{
    class SORE_EXPORT GamestateStack
    {
    public:
        typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::Font,       SORE_Resource::FontLoader>       Font_cache_t;
        typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::GLSLShader, SORE_Resource::GLSLShaderLoader> Shader_cache_t;
        typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::Texture2D,  SORE_Resource::Texture2DLoader>  Texture_cache_t;
        
        GamestateStack(
            const std::string& windowTitle = "SORE Framework Application",
            const std::string& iconFilename = "",
            const std::string& settingsFile = "");
        ~GamestateStack();

        //use new to instantiate newState; the stack will handle its deallocation
        void PushState(Gamestate* newState);
        void PopState();
        //run until a task requests exit or until there are no states left
        int Run();

        // Accessors
        SORE_Kernel::Screen& Screen() { return screen; }
        SORE_FileIO::PackageCache& PackageCache() { return packageCache; }

        // global resource caches
        Font_cache_t& FontCache() { return fontCache; }
        Shader_cache_t& ShaderCache() { return shaderCache; }
        Texture_cache_t& TextureCache() { return textureCache; }
    private:
        void Pop();

        SORE_Kernel::GameKernel kernel;

        SORE_Utility::IniSettingsBackend ini;
        SORE_Utility::SettingsManager sm;

        // globals accessible from gamestates
        SORE_Kernel::Screen screen;
        SORE_FileIO::PackageCache packageCache;

        // resource caches accessible from gamestates
        Font_cache_t fontCache;
        Shader_cache_t shaderCache;
        Texture_cache_t textureCache;

        SORE_Kernel::task_ref curr;
        std::vector<std::pair<SORE_Kernel::task_ref, Gamestate*> > states;

        //should we pop the state on frame end?
        bool popFlag;
    };
}
#endif