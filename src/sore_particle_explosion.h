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

#ifndef SORE_PARTICLE_EXPLOSION_H
#define SORE_PARTICLE_EXPLOSION_H

#include "sore_particle.h"
#include "sore_texture.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{
    class SORE_EXPORT ExplosionParticle : public Particle
    {
    public:
        ExplosionParticle(SORE_Math::Matrix4<float> initial, const Color& c,
                          Texture2DPtr tex,
                          GLSLShaderPtr shad);

        bool IsActive() const;
        void Frame(int elapsed);

    private:
        float alpha;
        float angle;
        float speed, alphaSpeed;
    };
}

#endif
