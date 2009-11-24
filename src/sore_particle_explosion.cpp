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

#include "sore_particle_explosion.h"
#include "sore_util.h"
#include "sore_math.h"

namespace SORE_Graphics
{
    ExplosionParticle::ExplosionParticle(SORE_Math::Matrix4<float> initial,
                                         const Color& c,
                                         SORE_Graphics::Texture2D* tex,
                                         SORE_Graphics::GLSLShader* shad)
        : Particle(initial, c), alpha(c.GetColor()[3])
    {
        angle = SORE_Utility::getRandomMinMax(0.0f, 2.0f*static_cast<float>(M_PI));
        alphaSpeed = SORE_Utility::getRandomMinMax(-0.00005f, 0.00005f) + 0.0003f;
        speed = SORE_Utility::getRandomMinMax(-0.0015f, 0.0015f) + 0.01f;
        transform *= SORE_Math::Matrix4<float>::GetScale(1.0f/20.0f, 1.0f/20.0f, 0.0f);
        transform *= SORE_Math::Matrix4<float>::GetRotation(angle, SORE_Math::AXIS_Z);
        SORE_Math::Rect<float> bounds(-6.0f, 6.0f, -1.5f, 1.5f);

        gc = new GeometryChunk(tex, shad, bounds,
                               SORE_Graphics::LAYER4, SORE_Graphics::SUBTRACTIVE,
                               defaultTexCoords, color);
    }

    bool ExplosionParticle::IsActive() const
    {
        return alpha > 0.0f;
    }

    void ExplosionParticle::Frame(int elapsed)
    {
        transform *=
            SORE_Math::Matrix4<float>::GetTranslation(elapsed*speed, 0.0f, 0.0f);
        alpha -= alphaSpeed * elapsed;
        gc->SetColor(gc->color() - Color(0.0f, 0.0f, 0.0f, alphaSpeed * elapsed));
    }
}
