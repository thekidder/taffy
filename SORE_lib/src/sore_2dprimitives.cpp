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

#include "sore_2dprimitives.h"

namespace SORE_Graphics
{
	Sprite2D::Sprite2D(SORE_Resource::ResourcePool* rm,std::string materialName, GLfloat _x, GLfloat _y, GLfloat _depth, GLfloat _width, GLfloat _height, color _c) : m(NULL),c(_c),x(_x),y(_y),width(_width),height(_height),depth(_depth)
	{
		if(rm)
			m = rm->GetResource<Material>("data/Materials/"+materialName+".mat");
	}
}
