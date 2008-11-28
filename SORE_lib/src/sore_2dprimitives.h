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

#include "sore_gfxprimitives.h"

#include "sore_resource.h"
#include "sore_material.h"

namespace SORE_Graphics
{
  class Sprite2D
  {
  public:
    Sprite2D() {}
    Sprite2D(SORE_Resource::ResourcePool* rm, std::string materialName, GLfloat _x, GLfloat _y, GLfloat _depth, GLfloat _width, GLfloat _height, color _c);
    void SetAlpha(float a) {c.a = a; }
    Material* m;
    color c;
    GLfloat x,y,width,height,depth;
  };
}

#endif /*__SORE_2DPRIMITIVES__*/
