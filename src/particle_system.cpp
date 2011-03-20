#include "particle_system.h"

#include <sore_sprite.h>

ParticleSystem::ParticleSystem(SORE_Graphics::Texture2DPtr texture, SORE_Graphics::GLSLShaderPtr shader)
    : vbo(SORE_Graphics::STREAM, true, false, false)
{
    geometry.push_back(
        SORE_Graphics::MakePointSprite(SORE_Math::Vector3<float>(0.0f, 0.0f, 0.0f),
        64.0f,
        texture,
        shader));

    geometry.front().AddKeyword("game");
}

void ParticleSystem::MakeUpToDate()
{
    vbo.Clear();
    vbo.AddObject(geometry.front().GetGeometryChunk());
    vbo.Build();

    geometry_lookup.geometry = &vbo;
    geometry_lookup.offset = 0;
    geometry_lookup.indices = geometry.front().GetGeometryChunk()->NumIndices();
    geometry_lookup.vertices = geometry.front().GetGeometryChunk()->NumVertices();
    geometry_lookup.type = geometry.front().GetGeometryChunk()->Type();
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