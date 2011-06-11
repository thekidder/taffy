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
    Screen::Screen(SORE_Graphics::ScreenInfo& _screen, InputTask& i,
        const std::string& windowTitle, const std::string& iconFilename,
        SORE_Utility::SettingsManager* _sm)
        : input(i), screen(_screen), sm(_sm)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Creating screen");
        renderer = NULL;
        proj.type = SORE_Graphics::PERSPECTIVE;
        proj.fov = 45.0f;
        proj.znear = 0.1f;
        proj.zfar  = 200.0f;
        proj.useScreenRatio = true;
        screen.ratio = static_cast<GLfloat>(_screen.width) /
            static_cast<GLfloat>(_screen.height);
        
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
        
        if(InitializeGL()!=0)
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
            quitFlag = true;
        }
    }

    void Screen::ChangeScreen(SORE_Graphics::ScreenInfo& _screen)
    {
        if(renderer)
            renderer->SetScreenInfo(_screen);

        screen = _screen;
        InitializeGL();

        glGetIntegerv(GL_VIEWPORT, viewport);
        Event e;
        e.type = RESIZE;
        e.resize.w = _screen.width;
        e.resize.h = _screen.height;
        input.InjectEvent(e);
    }

    std::vector<SORE_Math::Vector2<unsigned int> > Screen::ListModes()
    {
        std::vector<SORE_Math::Vector2<unsigned int> > allModes;
        
        return allModes;
    }

    Screen::~Screen()
    {
        
    }

    void Screen::Frame(int elapsedTime)
    {
        SORE_Profiler::Sample graphics("graphics");
        if(renderer)
            renderer->Render();
    }

    void Screen::SetRenderer(SORE_Graphics::IRenderer* _renderer)
    {
        renderer = _renderer;
        renderer->SetScreenInfo(screen);
    }

    bool Screen::OnResize(Event* event)
    {
        GLint width, height;
        if(event==NULL)
        {
            width = screen.width;
            height = screen.height;
        }
        else
        {
            width  = event->resize.w;
            height = event->resize.h;
        }
        SORE_Graphics::ScreenInfo tempScreen = screen;
        tempScreen.width = width;
        tempScreen.height = height;
        ChangeScreen(tempScreen);
        return true;
    }

    void Screen::SetupScreenInfo(SORE_Graphics::ScreenInfo& _screen)
    {
        if(_screen.useNativeResolution)
        {
            _screen.width = best_w;
            _screen.height = best_h;
        }
        _screen.ratio = static_cast<GLfloat>(_screen.width) /
            static_cast<GLfloat>(_screen.height);
    }

    void Screen::Resize(int width, int height)
    {
        ENGINE_LOG(SORE_Logging::LVL_DEBUG1,
                   boost::format("resizing from (%d, %d) to (%d, %d)")
                   % screen.width % screen.height % width % height);
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

    const GLint* Screen::GetViewport() const
    {
        return viewport;
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
