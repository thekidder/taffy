/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
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

#include "sore_screeninfo.h"

namespace SORE_Graphics
{
	SORE_Math::Vector2<float> ScreenToProjection(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<int> pos)
	{
		SORE_Math::Vector2<float> projected;
		projected[0] = proj.left + (proj.right-proj.left)*(static_cast<float>(pos[0])/static_cast<float>(screen.width));
		projected[1] = proj.bottom + (proj.top-proj.bottom)*(static_cast<float>(pos[1])/static_cast<float>(screen.height));
		return projected;
	}

	SORE_Math::Vector2<int> ProjectionToScreen(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<float> pos)
	{
		SORE_Math::Vector2<int> screenPos;
		screenPos[0] = static_cast<int>((screen.width )*(pos[0]/(proj.right-proj.left)));
		screenPos[1] = static_cast<int>((screen.height)*(pos[1]/(proj.top-proj.bottom)));
		return screenPos;
	}
}
