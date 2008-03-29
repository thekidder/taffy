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

#include "sore_screeninfo.h"

SORE_Math::Point2D<float> SORE_Graphics::ScreenToProjection(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Point2D<int> pos)
{
	SORE_Math::Point2D<float> projected;
	projected.x = proj.left + (proj.right-proj.left)*(double(pos.x)/double(screen.width));
	projected.y = proj.bottom + (proj.top-proj.bottom)*(double(pos.y)/double(screen.height));
	return projected;
}

SORE_Math::Point2D<int> SORE_Graphics::ProjectionToScreen(ScreenInfo screen, ProjectionInfo proj, SORE_Math::Point2D<float> pos)
{
	SORE_Math::Point2D<int> screenPos;
	screenPos.x = static_cast<int>((screen.width )*(pos.x/(proj.right-proj.left)));
	screenPos.y = static_cast<int>((screen.height)*(pos.y/(proj.top-proj.bottom)));
	return screenPos;
}
