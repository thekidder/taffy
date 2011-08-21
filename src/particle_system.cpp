#include "particle_system.h"

#include <boost/bind.hpp>
#include <sore_sprite.h>

#include <cassert>

ParticleSystem::ParticleSystem(
    size_t texture_size_,
    SORE_Resource::Texture2DPtr t, 
    SORE_Resource::GLSLShaderPtr s)
    : texture_size(texture_size_),
      texture(t), shader(s),
      vbo(SORE_Graphics::STATIC, true, true, true)
{
    size_t num_particles = texture_size * texture_size;

    SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(num_particles, num_particles, GL_POINTS));
    SORE_Graphics::TransformationPtr trans(new SORE_Math::Matrix4<float>());

    unsigned short* const indices = g->GetIndices();
    SORE_Graphics::vertex* const vertices = g->GetVertices();

    for(size_t i = 0; i < texture_size; ++i)
    {
        for(size_t j = 0; j < texture_size; ++j)
        {
            indices[i * texture_size + j] = i * texture_size + j;
            vertices[i * texture_size + j].tex0i = i / static_cast<float>(texture_size);
            vertices[i * texture_size + j].tex0j = j / static_cast<float>(texture_size);
            vertices[i * texture_size + j].normx = 0.2f;
        }
    }

    geometry.push_back(SORE_Graphics::Renderable(g, s, trans, SORE_Graphics::BLEND_OPAQUE));

    geometry.front().AddTexture("texture", texture);
    geometry.front().AddTexture("shadowMap", SORE_Graphics::TextureState::TextureObject("particle-shadowmap"));
    geometry.front().AddKeyword("game");
    geometry.front().AddKeyword("particle");

    vbo.Clear();
    vbo.AddObject(geometry.front().GetGeometryChunk());
    vbo.Build();

    geometry_lookup.geometry = &vbo;
    geometry_lookup.offset = 0;
    geometry_lookup.indices = geometry.front().GetGeometryChunk()->NumIndices();
    geometry_lookup.vertices = geometry.front().GetGeometryChunk()->NumVertices();
    geometry_lookup.type = geometry.front().GetGeometryChunk()->Type();
}

void ParticleSystem::SetShader(SORE_Resource::GLSLShaderPtr s)
{
    geometry.front().SetShader(s);
}

void ParticleSystem::SetTexture(SORE_Resource::Texture2DPtr t)
{
    geometry.front().AddTexture("texture", t);
}

void ParticleSystem::MakeUpToDate()
{
    geometry.front().AddTexture("colors", state.colors);
    geometry.front().AddTexture("positions", state.positions);
}

std::vector<SORE_Graphics::Renderable>::iterator ParticleSystem::GeometryBegin()
{
    return geometry.begin();
}

std::vector<SORE_Graphics::Renderable>::iterator ParticleSystem::GeometryEnd()
{
    return geometry.end();
}

SORE_Graphics::geometry_entry ParticleSystem::LookupGC(SORE_Graphics::GeometryChunkPtr gc)
{
    return geometry_lookup;
}

bool ParticleSystem::Contains(SORE_Graphics::GeometryChunkPtr gc)
{
    return gc == geometry.front().GetGeometryChunk();
}

void ParticleSystem::AddParticles(Particle_spawn_func_t spawn_func)
{
    ParticleTextureLoader loader(texture_size);
    state = loader.Load(spawn_func);
}

void ParticleSystem::Update(int elapsed)
{

}

bool ParticleSystem::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        geometry.front().Uniforms().SetVariable("halfWidth", e.resize.w / 2.0f);
        return true;
    }
    return false;
}
