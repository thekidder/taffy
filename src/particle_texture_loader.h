#ifndef PARTICLE_TEXTURE_LOADER_H
#define PARTICLE_TEXTURE_LOADER_H

#include <sore_assettypes.h>
#include <sore_vector3.h>

#include <boost/function.hpp>

struct ParticleSpawn
{
    float x, y, z;
    float xv, yv, zv;
    float r, g, b, a;
};

struct ParticleState
{
    SORE_Resource::Texture2DPtr positions;
    SORE_Resource::Texture2DPtr velocities;
    SORE_Resource::Texture2DPtr colors;
};

typedef boost::function<void (ParticleSpawn&)> Particle_spawn_func_t;

class ParticleTextureLoader
{
public:
    ParticleTextureLoader(size_t texture_size_);

    ParticleState Load(const Particle_spawn_func_t& spawn_func);
private:
    size_t texture_size;
};

#endif