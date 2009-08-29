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
// $Id$

#ifndef  SORE_RENDERER_H
#define  SORE_RENDERER_H

#include "sore_screeninfo.h"

namespace SORE_Graphics
{
	typedef unsigned int gc_id;
	
	class SORE_EXPORT IRenderer
	{
	public:
		IRenderer() {}
		virtual ~IRenderer() {}
			
		virtual void Render() = 0;
		void SetScreenInfo(ScreenInfo _screen);
		ScreenInfo GetScreenInfo() const;

		//Renderer has a "state": used by Gamestate. Only geometry in the current state should be rendered
		virtual void PushState() = 0;
		virtual void PopState() = 0;
	protected:
		virtual void OnScreenChange() {}
		ScreenInfo screen;
	};
}

#endif
