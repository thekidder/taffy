#include "particle_texture_loader.h"

#include <sore_texture2d.h>

ParticleTextureLoader::ParticleTextureLoader(size_t texture_size_w, size_t texture_size_h)
    : texture_size_width(texture_size_w), texture_size_height(texture_size_h)
{
}

ParticleState ParticleTextureLoader::Load(const Particle_spawn_func_t& spawn_func)
{
    float* positions  = new float[texture_size_width * texture_size_height * 4];
    float* datas      = new float[texture_size_width * texture_size_height * 4];
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
            positions[index * 4 + 3] = p.size;

            colors[index * 4 + 0] = p.r;
            colors[index * 4 + 1] = p.g;
            colors[index * 4 + 2] = p.b;
            colors[index * 4 + 3] = p.lifetime;

            datas[index * 4 + 0] = p.data0;
            datas[index * 4 + 1] = p.data1;
            datas[index * 4 + 2] = p.data2;
            datas[index * 4 + 3] = p.data3;
        }
    }

    ParticleState s;
    s.positions = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(positions), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.data = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(datas), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.colors = new SORE_Resource::Texture2D(
        reinterpret_cast<void*>(colors), GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);

    delete positions;
    delete datas;
    delete colors;

    return s;
}