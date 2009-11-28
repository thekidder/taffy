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

#ifndef SORE_SPRITE_H
#define SORE_SPRITE_H

#include "math/sore_geometry.h"
#include "sore_dll.h"
#include "sore_renderable.h"

namespace SORE_Graphics
{
    Renderable MakeSprite(
        SORE_Math::Rect<float> bounds,
        SORE_Math::Rect<float> texCoords,
        float z,
        Texture2DPtr texture,
        GLSLShaderPtr shader,
        geometry_layer l = LAYER3,
        blend_mode b = SUBTRACTIVE);
}

#endif
