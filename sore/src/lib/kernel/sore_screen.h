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

#include <sore_allgl.h>
#include <sore_font.h>
#include <sore_gamekernel.h>
#include <sore_event.h>
#include <sore_logger.h>
#include <sore_noncopyable.h>
#include <sore_renderer.h>
#include <sore_screeninfo.h>
#include <sore_settings.h>

#include <boost/function.hpp>

namespace SORE_Kernel
{
    class InputTask;

    // Responsible for opening/displaying a window and keeping the OpenGL context up-to-date
    class SORE_EXPORT Screen : public Task, SORE_Utility::Noncopyable
    {
    public:
        Screen(SORE_Graphics::ScreenInfo& _screen,
               const std::string& windowTitle_="SORE App",
               const std::string& iconFilename="",
               SORE_Utility::SettingsManager* _sm=NULL);

        void Frame(int elapsedTime);

        const char* GetName() const {return "Screen";}

        void ChangeScreen(SORE_Graphics::ScreenInfo& _screen);
        std::vector<SORE_Math::Vector2<unsigned int> > ListModes();

        bool OnResize(const Event& event);

        SORE_Graphics::ScreenInfo GetScreen() const;
    private:
        friend class InputTask;

        void CreateSFMLWindow(); // create window based on current screeninfo
        void SetupScreenInfo(SORE_Graphics::ScreenInfo& _screen);
        int  InitializeGL();
        void PrintGLDiagnostics();
        void InitExtensions();
        void ChangeScreenOnSettingsChange();

        sf::Window window;
        std::string windowTitle;
        SORE_Graphics::ScreenInfo screen;
        SORE_Utility::SettingsManager* sm;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
