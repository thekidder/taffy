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

#ifndef  SORE_SCREEN_H
#define  SORE_SCREEN_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <boost/function.hpp>

#include "sore_renderer.h"
#include "sore_gamekernel.h"
#include "sore_font.h"
#include "sore_allgl.h"
#include "sore_logger.h"
#include "sore_input.h"
#include "sore_settings.h"
#include "sore_screeninfo.h"

namespace SORE_Kernel
{
	class SORE_EXPORT Screen : public Task
	{
	public:
		Screen(SORE_Graphics::ScreenInfo& _screen, InputTask& i,
					 std::string windowTitle="SORE App", SORE_Utility::SettingsManager* _sm=NULL);
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
		void InitExtensions();
			

		void SDLScreenChange(SORE_Graphics::ScreenInfo& _screen);
		void ChangeScreenOnSettingsChange(); 
			
		void Resize(int width, int height); //does the actual resizing
		
		InputTask& input; //for injecting resize events

		SDL_Surface* drawContext;
		int width, height; //user's previous width and height so we can reset their screen after fullscreen mode
		Uint32 videoFlags;
			
		SORE_Graphics::IRenderer* renderer;
		SORE_Graphics::ProjectionInfo proj;
		SORE_Graphics::ScreenInfo screen;
		SORE_Utility::SettingsManager* sm;
			
		GLint viewport[4];
			
		std::string uiDataPath;
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
