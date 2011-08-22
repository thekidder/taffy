#include "particle_texture_loader.h"

#include <sore_texture2d.h>
#include <sore_util.h>

ParticleTextureLoader::ParticleTextureLoader(size_t texture_size_w, size_t texture_size_h)
    : texture_size_width(texture_size_w), texture_size_height(texture_size_h)
{
}

ParticleState ParticleTextureLoader::Create()
{
    ParticleState s;
    s.positions = new SORE_Resource::Texture2D(
        0, GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.data = new SORE_Resource::Texture2D(
        0, GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    s.colors = new SORE_Resource::Texture2D(
        0, GL_RGBA32F_ARB, GL_RGBA, texture_size_width, texture_size_height);
    return s;
}