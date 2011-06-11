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

#ifndef  SORE_SCREEN_H
#define  SORE_SCREEN_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <SDL/SDL.h>

#include <sore_renderer.h>
#include <sore_gamekernel.h>
#include <sore_font.h>
#include <sore_allgl.h>
#include <sore_logger.h>
#include <sore_input.h>
#include <sore_noncopyable.h>
#include <sore_settings.h>
#include <sore_screeninfo.h>

namespace SORE_Kernel
{
    class SORE_EXPORT Screen : public Task, SORE_Utility::Noncopyable
    {
    public:
        Screen(SORE_Graphics::ScreenInfo& _screen, InputTask& i,
               const std::string& windowTitle="SORE App",
               const std::string& iconFilename="",
               SORE_Utility::SettingsManager* _sm=NULL);
        ~Screen();

        void Frame(int elapsedTime);

        const char* GetName() const {return "Screen";}

        void SetRenderer(SORE_Graphics::IRenderer* _renderer);

        void ChangeScreen(SORE_Graphics::ScreenInfo& _screen);
        std::vector<SORE_Math::Vector2<unsigned int> > ListModes();

        bool OnResize(Event* event=NULL);

        const GLint* GetViewport() const;
        SORE_Graphics::ScreenInfo GetScreen() const;
    private:
        int  InitializeSDL(std::string windowTitle);
        int  InitializeGL();
        void PrintGLDiagnostics();
        void InitExtensions();

        void SetupScreenInfo(SORE_Graphics::ScreenInfo& _screen);

        void SDLScreenChange(SORE_Graphics::ScreenInfo& _screen);
        void ChangeScreenOnSettingsChange();

        void Resize(int width, int height); //does the actual resizing

        InputTask& input; //for injecting resize events

        SDL_Surface* drawContext;
        Uint32 videoFlags;

        SORE_Graphics::IRenderer* renderer;
        SORE_Graphics::ProjectionInfo proj;
        SORE_Graphics::ScreenInfo screen;
        SORE_Utility::SettingsManager* sm;

        GLint viewport[4];

        std::string uiDataPath;

        int best_w, best_h;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
