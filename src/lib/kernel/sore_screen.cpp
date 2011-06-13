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

#include <GL/glew.h>

#include <sore_screen.h>
#include <sore_profiler.h>
#include <sore_util.h>
#include <sore_timing.h>
#include <cassert>
#include <boost/format.hpp>
#include <functional>
#include <sore_allgl.h>

namespace SORE_Kernel
{
    Screen::Screen(SORE_Graphics::ScreenInfo& _screen,
        const std::string& windowTitle_, const std::string& iconFilename,
        SORE_Utility::SettingsManager* _sm)
        : windowTitle(windowTitle_), screen(_screen), sm(_sm)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Creating screen");

        if(sm!=NULL)
        {
            screen.width =
                sm->WatchVariable
                ("screen", "width",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
            screen.height =
                sm->WatchVariable
                ("screen", "height",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
            screen.fullscreen =
                sm->WatchVariable
                ("screen", "fullscreen",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
            screen.resizable  =
                sm->WatchVariable
                ("screen", "resizable",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
            screen.showCursor =
                sm->WatchVariable
                ("screen", "showcursor",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
            screen.useNativeResolution  =
                sm->WatchVariable
                ("screen", "native",
                 boost::bind(std::mem_fun(&Screen::ChangeScreenOnSettingsChange),
                             this));
        }

        CreateSFMLWindow();
        
        if(InitializeGL()!=0)
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
            quitFlag = true;
        }
    }

    void Screen::ChangeScreen(SORE_Graphics::ScreenInfo& _screen)
    {
        screen = _screen;
        CreateSFMLWindow();
    }

    std::vector<SORE_Math::Vector2<unsigned int> > Screen::ListModes()
    {
        std::vector<SORE_Math::Vector2<unsigned int> > allModes;
        // TODO: FIXME
        return allModes;
    }

    void Screen::Frame(int elapsedTime)
    {
        window.SetActive();
        window.Display();
    }

    bool Screen::OnResize(const Event& event)
    {
        if(event.type == RESIZE)
        {
            screen.useNativeResolution = false;
            screen.width  = event.resize.w;
            screen.height = event.resize.h;
            SetupScreenInfo(screen);

            // make sure viewport is correct
            glViewport(0, 0, screen.width, screen.height);

            return true;
        }
        else
        {
            return false;
        }
    }

    void Screen::CreateSFMLWindow()
    {
        SetupScreenInfo(screen);

        unsigned long style = sf::Style::Close;
        if(screen.fullscreen)
            style |= sf::Style::Fullscreen;
        if(screen.resizable)
            style |= sf::Style::Resize;
        sf::VideoMode videoMode(screen.width, screen.height);

        window.Create(videoMode, windowTitle, style);

        SORE_Kernel::Event e;
        e.type = SORE_Kernel::RESIZE;
        e.resize.w = screen.width;
        e.resize.h = screen.height;
        //input.InjectEvent(e);
    }

    void Screen::SetupScreenInfo(SORE_Graphics::ScreenInfo& _screen)
    {
        if(_screen.useNativeResolution)
        {
            // TODO: FIXME
            //_screen.width = best_w;
            //_screen.height = best_h;
        }
        _screen.ratio = static_cast<GLfloat>(_screen.width) /
            static_cast<GLfloat>(_screen.height);
    }

    int Screen::InitializeGL()
    {
        /* Enable smooth shading */
        glShadeModel( GL_SMOOTH );
        /* Set the background black */
        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
        /* Depth buffer setup */
        glClearDepth( 1.0f );
        /* Enables Depth Testing */
        glEnable( GL_DEPTH_TEST );
        /* The Type Of Depth Test To Do */
        glDepthFunc( GL_LEQUAL );
        /* Really Nice Perspective Calculations */
        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
        glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST);
        InitExtensions();
#ifdef WIN32
        if(WGLEW_EXT_swap_control)
        {
            wglSwapIntervalEXT(0); //turn off vsync
        }
        else
        {
            ENGINE_LOG(SORE_Logging::LVL_WARNING, "Vsync control not available");
        }
#endif
        PrintGLDiagnostics();

        glViewport( 0, 0, ( GLsizei )screen.width, ( GLsizei )screen.height );

        return 0;
    }

    void Screen::PrintGLDiagnostics()
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("OpenGL Diagnostics\n"
                                 "Renderer   : %s\n"
                                 "Vender     : %s\n"
                                 "API Version: %s")
                   % (char*)glGetString(GL_RENDERER) % (char*)glGetString(GL_VENDOR)
                   % (char*)glGetString(GL_VERSION));
        char* glExtensions = (char*)glGetString(GL_EXTENSIONS);
        char* version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if(version==NULL)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "No GLSL Support Available");
        }
        else
        {
            ENGINE_LOG(SORE_Logging::LVL_INFO,
                       boost::format("OpenGL Shading language version: %s")
                       % version);
        }
        std::string extensions;
        if(glExtensions==NULL) extensions = "";
        else extensions = glExtensions;
        size_t pos;
        while((pos=extensions.find(" "))!=std::string::npos)
        {
            extensions.replace(pos, 1, "\n");
        }
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("OpenGL extension string:\n%s") % extensions);
        int maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Maximum texture size: %dx%d")
                   % maxTextureSize % maxTextureSize);
        int maxTextureUnits;
        int maxFragmentTextureImageUnits;
        int maxVertexTextureImageUnits;
        int maxCombinedTextureImageUnits;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxFragmentTextureImageUnits);
        glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,
                      &maxVertexTextureImageUnits);
        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,
                      &maxCombinedTextureImageUnits);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Maximum (complete) texture units: %d\n"
                                 "Maximum fragment texture units: %d\n"
                                 "Maximum vertex texture units: %d\n"
                                 "Maximum combined texture units: %d")
                   % maxTextureUnits % maxFragmentTextureImageUnits
                   % maxVertexTextureImageUnits % maxCombinedTextureImageUnits);
        int maxMRT;
        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxMRT);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Maximum color attachments: %d")
                   % maxMRT);
        int maxVertexAttribs;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Maximum vertex attributes: %d")
                   % maxVertexAttribs);
        int width[2];
        glGetIntegerv(GL_LINE_WIDTH_RANGE, width);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Line width range: %d - %d")
                   % width[0] % width[1]);
        glGetIntegerv(GL_POINT_SIZE_RANGE, width);
        ENGINE_LOG(SORE_Logging::LVL_INFO,
                   boost::format("Point size range: %d - %d")
                   % width[0] % width[1]);
    }

    void Screen::InitExtensions()
    {
        glewExperimental = GL_TRUE;
        GLenum glewError = glewInit();
        if(glewError != GLEW_OK)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to initialize OpenGL extensions. "
                                     "GLEW Error: %s")
                       % glewGetErrorString(glewError));
        }
    }

    SORE_Graphics::ScreenInfo Screen::GetScreen() const
    {
        return screen;
    }

    void Screen::ChangeScreenOnSettingsChange()
    {
        int iWidth   = (int) sm->GetVariable("screen", "width");
        int iHeight  = (int) sm->GetVariable("screen", "height");
        bool bFull   = (bool)sm->GetVariable("screen", "fullscreen");
        bool bResize = (bool)sm->GetVariable("screen", "resizable");
        bool bCursor = (bool)sm->GetVariable("screen", "showcursor");
        bool native  = (bool)sm->GetVariable("screen", "native");
        static SORE_Graphics::ScreenInfo screen;
        screen.width=iWidth;
        screen.height=iHeight;
        screen.fullscreen=bFull;
        screen.resizable=bResize;
        screen.showCursor=bCursor;
        screen.useNativeResolution = native;
        ChangeScreen(screen);
    }
}
