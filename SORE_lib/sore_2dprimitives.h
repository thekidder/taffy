/***************************************************************************
 *   Copyright (C) 2007 by Adam Kidder   *
 *   thekidder@gmail.com   *
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

#ifndef  __SORE_2DPRIMITIVES__
#define  __SORE_2DPRIMITIVES__

#include "sore_resource.h"
#include "sore_material.h"

namespace SORE_Graphics
{
	struct color
	{
		double r,g,b,a;
	};
	
	class IRenderable
	{
		public:
			IRenderable() {}
			virtual ~IRenderable() {}
			virtual void Draw() {}
	};
	
	class Sprite2D : public IRenderable
	{
		public:
			Sprite2D(SORE_Resource::ResourceManager* rm, std::string materialName, double _x, double _y, double _width, double _height, color _c) {}
			virtual void Draw() {}
		protected:
			Material* m;
			color c;
			double x,y,width,height;
	};
}

#endif /*__SORE_2DPRIMITIVES__*/
