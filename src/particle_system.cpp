#include "particle_system.h"

#include <sore_sprite.h>

ParticleSystem::ParticleSystem(SORE_Graphics::Texture2DPtr t, SORE_Graphics::GLSLShaderPtr s)
    : vbo(SORE_Graphics::STREAM, true, false, false), texture(t), shader(s)
{
    SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(0, 0, GL_POINTS));
    SORE_Graphics::TransformationPtr trans(new SORE_Math::Matrix4<float>());

    geometry.push_back(SORE_Graphics::Renderable(g, s, trans, SORE_Graphics::BLEND_ADDITIVE));

    geometry.front().AddTexture("texture", texture);
    geometry.front().AddKeyword("game");

    UpdateGeometry();
}

void ParticleSystem::MakeUpToDate()
{
    UpdateGeometry();

    if(particles.size())
    {
        float size = particles.front().size;
        geometry.front().Uniforms().SetVariable("pointSize", size);
    }

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

void ParticleSystem::UpdateGeometry()
{
    SORE_Graphics::GeometryChunkPtr g(
        new SORE_Graphics::GeometryChunk(particles.size(), particles.size(), GL_POINTS));

    geometry.front().SetGeometryChunk(g);

    SORE_Graphics::vertex* const vertices = geometry.front().GetGeometryChunk()->GetVertices();
    unsigned short* const indices = geometry.front().GetGeometryChunk()->GetIndices();

    for(size_t i = 0; i < particles.size(); ++i)
    {
        vertices[i].x = particles[i].x;
        vertices[i].y = particles[i].y;
        vertices[i].z = particles[i].z;

        indices[i] = i;
    }
}