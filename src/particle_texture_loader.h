#ifndef PARTICLE_TEXTURE_LOADER_H
#define PARTICLE_TEXTURE_LOADER_H

#include <sore_assettypes.h>
#include <sore_vector3.h>

#include <boost/function.hpp>

struct ParticleSpawn
{
    float x, y, z, size;
    float r, g, b, lifetime;

    float data0, data1, data2, data3;
};

struct ParticleState
{
    SORE_Resource::Texture2DPtr positions; // (x, y, z, size)
    SORE_Resource::Texture2DPtr colors;    // (r, g, b, lifetime)
    SORE_Resource::Texture2DPtr data;      // (data0, data1, data2, data3)
};

typedef boost::function<void (ParticleSpawn&)> Particle_spawn_func_t;

class ParticleTextureLoader
{
public:
    ParticleTextureLoader(size_t texture_size_w, size_t texture_size_h);

    ParticleState Load(const Particle_spawn_func_t& spawn_func);
private:
    size_t texture_size_width, texture_size_height;
};

#endif