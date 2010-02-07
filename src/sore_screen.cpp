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

#include <GL/glew.h>

#include "sore_screen.h"
#include "sore_profiler.h"
#include "sore_util.h"
#include "sore_timing.h"
#include <cassert>
#include <boost/format.hpp>
#include <functional>
#include "sore_allgl.h"

namespace SORE_Kernel
{
    Screen::Screen(SORE_Graphics::ScreenInfo& _screen, InputTask& i,
        const std::string& windowTitle, const std::string& iconFilename,
        SORE_Utility::SettingsManager* _sm)
        : input(i), drawContext(0), screen(_screen), sm(_sm)
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
        if(InitializeSDL(windowTitle)!=0)
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL,
                       boost::format("Could not initialize SDL (SDL error %s)")
                       % SDL_GetError());
            quitFlag = true;
        }
        SDL_EnableUNICODE(1);
        SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
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
        best_w = SDL_GetVideoInfo()->current_w;
        best_h = SDL_GetVideoInfo()->current_h;

        if(iconFilename.size())
        {
            SDL_WM_SetIcon(SDL_LoadBMP(iconFilename.c_str()), NULL);
        }

        SDLScreenChange(screen);
        if(InitializeGL()!=0)
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, "Could not initialize GL");
            quitFlag = true;
        }
    }

    void Screen::SDLScreenChange(SORE_Graphics::ScreenInfo& _screen)
    {
        SetupScreenInfo(_screen);
        screen.fullscreen = _screen.fullscreen;
        screen.showCursor = _screen.showCursor;
        screen.resizable  = _screen.resizable;
        if(screen.fullscreen)
            videoFlags |= SDL_FULLSCREEN;
        else
            if(videoFlags & SDL_FULLSCREEN) videoFlags ^= SDL_FULLSCREEN;
        if(screen.showCursor)
            SDL_ShowCursor(SDL_ENABLE);
        else
            SDL_ShowCursor(SDL_DISABLE);
        if(screen.resizable)
            videoFlags |= SDL_RESIZABLE;
        else
            if(videoFlags & SDL_RESIZABLE) videoFlags ^= SDL_RESIZABLE;
        Resize(_screen.width, _screen.height);
    }

    void Screen::ChangeScreen(SORE_Graphics::ScreenInfo& _screen)
    {
        SDLScreenChange(_screen);
        if(renderer)
            renderer->SetScreenInfo(screen);

        glViewport( 0, 0, ( GLsizei )screen.width, ( GLsizei )screen.height );
        glGetIntegerv(GL_VIEWPORT, viewport);
        Event e;
        e.type = RESIZE;
        e.resize.w = screen.width;
        e.resize.h = screen.height;
        input.InjectEvent(e);
    }

    std::vector<SORE_Math::Vector2<unsigned int> > Screen::ListModes()
    {
        std::vector<SORE_Math::Vector2<unsigned int> > allModes;
        SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
        if(!modes)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "No modes detected by SDL");
            return allModes;
        }
        for(unsigned int i=0;modes[i];++i)
        {
            if(modes[i]->x || modes[i]->y)
                ENGINE_LOG(SORE_Logging::LVL_WARNING,
                           boost::format("ListModes returned invalid Rect: x: "
                                         "%d, y: %d")
                           % modes[i]->x % modes[i]->y);
            if(modes[i]->w >= 640 && modes[i]->h >= 480)
            {
                SORE_Math::Vector2<unsigned int> m(modes[i]->w, modes[i]->h);
                allModes.push_back(m);
            }
        }
        return allModes;
    }

    Screen::~Screen()
    {
        SDL_ShowCursor(SDL_ENABLE);
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_Quit();
    }

    void Screen::Frame(int elapsedTime)
    {
        SORE_Profiler::Sample graphics("graphics");
        if(renderer)
            renderer->Render();
        SDL_GL_SwapBuffers();
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
        if(renderer)
        {
            renderer->SetScreenInfo(screen);
        }
        drawContext = SDL_SetVideoMode(screen.width, screen.height, 0, videoFlags);
    }

    int Screen::InitializeSDL(std::string windowTitle)
    {
        if(SDL_Init(SDL_INIT_VIDEO)!=0)
        {
            return 1;
        }

        const SDL_VideoInfo *videoInfo;
        /* Fetch the video info */
        videoInfo = SDL_GetVideoInfo( );

        if ( !videoInfo )
        {
            ENGINE_LOG(SORE_Logging::SHOW_CRITICAL,
                       boost::format("Video query failed: %s")
                       % SDL_GetError());
            return 1;
        }
        //save current resolution
        width = videoInfo->current_w;
        height = videoInfo->current_h;

        videoFlags = SDL_OPENGL;
        videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
        videoFlags |= SDL_HWPALETTE;
        /* This checks to see if surfaces can be stored in memory */
        if ( videoInfo->hw_available )
            videoFlags |= SDL_HWSURFACE;
        else
            videoFlags |= SDL_SWSURFACE;
        if(videoInfo->blit_hw)
            videoFlags |= SDL_HWACCEL;

        SDL_WM_SetCaption(windowTitle.c_str(), windowTitle.c_str());

        return 0;
    }

    int Screen::InitializeGL()
    {
        //OpenGL setup
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

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
