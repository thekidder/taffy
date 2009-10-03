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

#ifndef SORE_PARTICLE_EMITTER_H
#define SORE_PARTICLE_EMITTER_H

#include "math/sore_matrix4x4.h"
#include "sore_geometrychunk.h"
#include "sore_particle.h"
#include "sore_task.h"
#include "sore_resource.h"
#include "sore_util.h"

namespace SORE_Graphics
{
	template<typename T>
	class SORE_EXPORT ParticleEmitter : public SORE_Kernel::Task
	{
	public:
		ParticleEmitter(unsigned int numParticles, unsigned int spawnAmount, 
										SORE_Math::Matrix4<float> pos, const Color& min, const Color& max, SORE_Resource::Texture2D* tex)
                                        : particles(0), num(numParticles), spawn(spawnAmount), position(0), transform(pos), texture(tex), cMin(min), cMax(max)
		{
			if(num)
			{
				particles = new T*[numParticles];
				Create(1);
			}
		}

		~ParticleEmitter()
		{
			if(num)
				{
					Destroy();
					delete[] particles;
				}
		}

		void Restart()
		{
			position = 0;
			Destroy();
			Create(1);
		}

		render_list GetGeometry()
		{
			render_list geometry;
			for(unsigned int i=0;i<position;++i)
				{
					if(particles[i]->IsActive())
						geometry.push_back(std::make_pair(particles[i]->GetTransform(), particles[i]->GetGeometry()));
				}
			return geometry;
		}

		bool IsActive() const
		{
			for(unsigned int i=0;i<position;++i)
				{
					if(particles[i]->IsActive())
						return true;
				}
			return false;
		}

		void SetMinColor(const Color& c)
		{
			cMin = c;
		}

		void SetMaxColor(const Color& c)
		{
			cMax = c;
		}

		void Frame(int elapsed)
		{
			if(position < num)
				{
					float secs = elapsed / 10000.0f;
					unsigned int n = static_cast<unsigned int>(secs*spawn);
					if(n+position > num) n = num - position;
					Create(n);
				}
			for(unsigned int i=0;i<position;++i)
				{
					if(particles[i]->IsActive())
						particles[i]->Frame(elapsed);
				}
		}
		const char* GetName() const { return "Particle Emitter";}
	private:
		void Destroy()
		{
			for(unsigned int i=0;i<position;++i)
				{
					delete particles[i];
				}
		}

		void Create(unsigned int n)
		{
			for(unsigned int i=position; i<n+position;i++)
			{
				float r = SORE_Utility::getRandomMinMax(cMin.GetColor()[0], cMax.GetColor()[0]);
				float g = SORE_Utility::getRandomMinMax(cMin.GetColor()[1], cMax.GetColor()[1]);
				float b = SORE_Utility::getRandomMinMax(cMin.GetColor()[2], cMax.GetColor()[2]);

				Color c(r, g, b, 1.0f);

				particles[i] = new T(transform, c, texture);
			}
			position += n;
		}

		T** particles;
		unsigned int num, spawn, position;
		SORE_Math::Matrix4<float> transform;
		SORE_Resource::Texture2D* texture;

		Color cMin, cMax;
	};
}

#endif
