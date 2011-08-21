#include "particle_texture_loader.h"

#include <sore_texture2d.h>

ParticleTextureLoader::ParticleTextureLoader(size_t texture_size_w, size_t texture_size_h)
    : texture_size_width(texture_size_w), texture_size_height(texture_size_h)
{
}

ParticleState ParticleTextureLoader::Load(const Particle_spawn_func_t& spawn_func)
{
    float* positions  = new float[texture_size_width * texture_size_height * 4];
    float* velocities = new float[texture_size_width * texture_size_height * 4];
    float* colors     = new float[texture_size_width * texture_size_height * 4];

    for(size_t i = 0; i < texture_size_width; ++i)
    {
        for(size_t j = 0; j < texture_size_height; ++j)
        {
            size_t index = j * texture_size_width + i;

            ParticleSpawn p;
            spawn_func(p);

            positions[index * 4 + 0] = p.x;
            positions[index * 4 + 1] = p.y;
            positions[index * 4 + 2] = p.z;
            positions[index * 4 + 3] = 1.0f;

            velocities[index * 4 + 0] = p.xv;
            velocities[index * 4 + 1] = p.yv;
            velocities[index * 4 + 2] = p.zv;
            velocities[index * 4 + 3] = 1.0;

            colors[index * 4 + 0] = p.r;
            colors[index * 4 + 1] = p.g;
            colors[index * 4 + 2] = p.b;
            colors[index * 4 + 3] = p.a;
        }
    }

    ParticleState s;
    s.positions = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(positions), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.velocities = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(velocities), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.colors = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(colors), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);

    delete positions;
    delete velocities;
    delete colors;

    return s;
}