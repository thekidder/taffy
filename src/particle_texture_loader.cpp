#include "particle_texture_loader.h"

#include <sore_texture2d.h>

ParticleTextureLoader::ParticleTextureLoader(size_t texture_size_)
    : texture_size(texture_size_)
{
}

ParticleState ParticleTextureLoader::Load(const Particle_spawn_func_t& spawn_func)
{
    float* positions  = new float[texture_size * texture_size * 3];
    float* velocities = new float[texture_size * texture_size * 3];
    float* colors     = new float[texture_size * texture_size * 4];

    for(size_t i = 0; i < texture_size * texture_size; ++i)
    {
        ParticleSpawn p;
        spawn_func(p);

        positions[i * 3 + 0] = p.x;
        positions[i * 3 + 1] = p.y;
        positions[i * 3 + 2] = p.z;

        velocities[i * 3 + 0] = p.xv;
        velocities[i * 3 + 1] = p.yv;
        velocities[i * 3 + 2] = p.zv;

        colors[i * 4 + 0] = p.r;
        colors[i * 4 + 1] = p.g;
        colors[i * 4 + 2] = p.b;
        colors[i * 4 + 3] = p.a;
    }

    SORE_Resource::Texture2D* t = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(positions), GL_RGB32F_ARB, GL_RGB, texture_size, texture_size);
    SORE_Resource::Texture2DPtr positions_tex(new SORE_Resource::ResourceProxy<SORE_Resource::Texture2D>(t));

    t = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(velocities), GL_RGB32F_ARB, GL_RGB, texture_size, texture_size);
    SORE_Resource::Texture2DPtr velocities_tex(new SORE_Resource::ResourceProxy<SORE_Resource::Texture2D>(t));

    t = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(colors), GL_RGBA32F_ARB, GL_RGBA, texture_size, texture_size);
    SORE_Resource::Texture2DPtr colors_tex(new SORE_Resource::ResourceProxy<SORE_Resource::Texture2D>(t));
    
    delete positions;
    delete velocities;
    delete colors;

    ParticleState s;
    s.positions = positions_tex;
    s.velocities = velocities_tex;
    s.colors = colors_tex;

    return s;
}