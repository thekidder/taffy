/***************************************************************************
 *   Copyright (C) 2008 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#ifndef  __SORE_SCREENINFO__
#define  __SORE_SCREENINFO__

#include "sore_math.h"

namespace SORE_Graphics
{
	enum ProjectionType {NONE, ORTHO, ORTHO2D, PERSPECTIVE};
	
	struct ProjectionInfo
	{
		ProjectionInfo() {type = NONE; fov = ratio = znear = zfar = top = bottom = left = right = 0.0; useScreenCoords = useScreenRatio = false; }
		ProjectionType type;
		float fov,ratio;
		float znear, zfar;
		float top, bottom, left, right;
		bool useScreenCoords; //if this is true, and type of projection is ortho2d, use width/height for projection
		bool useScreenRatio; //if true, uses screen ratio (for ortho, gets top/bottom by dividing left/right by ratio)
	};
	
	struct ScreenInfo
	{
		int width, height;
		float ratio; //set by SORE_Screen after screen is created
		bool showCursor;
		bool fullscreen;
		bool resizable;
	};
	
	SORE_Math::Vector2<float> ScreenToProjection(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<int> pos);
	SORE_Math::Vector2<int>   ProjectionToScreen(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<float> pos);
}

#endif /*__SORE_SCREENINFO__*/
