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

#ifndef  __SORE_GFXPRIMITIVES__
#define  __SORE_GFXPRIMITIVES__

namespace SORE_Graphics
{
  struct color
  {
    float r,g,b,a;
    color operator+=(color& c)
    {
      r+=c.r;
      g+=c.g;
      b+=c.b;
      return *this;
    }
  color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
  color() : r(0.0f),g(0.0f),b(0.0f),a(0.0f) {}
  };
	
  struct viewport
  {
    float x,y,width,height;
  };
}

#endif /*__SORE_GFXPRIMITIVES__*/
