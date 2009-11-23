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

#include "sore_particle_circle.h"

namespace SORE_Graphics
{
    CircleParticle::CircleParticle(SORE_Math::Matrix4<float> initial, const Color& c,
                                   SORE_Graphics::Texture2D* tex,
                                   SORE_Graphics::GLSLShader* shad)
  : Particle(initial, c), alpha(1.0f)
    {
        transform *= SORE_Math::Matrix4<float>::GetScale(1.0f/20.0f, 1.0f/20.0f, 0.0f);
        SORE_Math::Rect<float> bounds(-1.0f, 1.0f, -1.0f, 1.0f);
        gc = new GeometryChunk(tex, shad, bounds, defaultTexCoords, color);
    }

    bool CircleParticle::IsActive() const
    {
        return alpha > 0.0f;
    }

    void CircleParticle::Frame(int elapsed)
    {
        transform *= SORE_Math::Matrix4<float>::GetScale
            (1.0f+elapsed*0.0006f, 1.0f+elapsed*0.0006f, 0.0f);
        alpha -= 0.00015f * elapsed;
        gc->SetColor(Color(1.0f, 0.0f, 0.0f, alpha));
    }
}
