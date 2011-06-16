#include "particle_system.h"

#include <sore_sprite.h>

#include <cassert>

bool operator<(const Particle& one, const Particle& two);

ParticleSystem::ParticleSystem(
    size_t num_particles,
    SORE_Resource::Texture2DPtr t, 
    SORE_Resource::GLSLShaderPtr s)
    : particles(num_particles),
      texture(t), shader(s),
      vbo(SORE_Graphics::STREAM, true, true, false)
{
    SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(0, 0, GL_POINTS));
    SORE_Graphics::TransformationPtr trans(new SORE_Math::Matrix4<float>());

    geometry.push_back(SORE_Graphics::Renderable(g, s, trans, SORE_Graphics::BLEND_ADDITIVE));

    geometry.front().AddTexture("texture", texture);
    geometry.front().AddKeyword("game");

    UpdateGeometry();
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
    UpdateGeometry();

    if(particles.size())
    {
        float size = particles.front().size;
        geometry.front().Uniforms().SetVariable("pointSize", size);
        geometry.front().Uniforms().SetVariable("halfWidth", half_width);
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

void ParticleSystem::SetSize(float size)
{
    particles.front().size = size;
}

void ParticleSystem::Clear()
{
    particles.clear();
}

void ParticleSystem::AddParticles(boost::function<void (Particle&)> create_callback, size_t num_particles)
{
    assert(num_particles <= particles.size());

    Particles_t::iterator it = particles.end() - num_particles;
    for(it; it != particles.end(); ++it)
        create_callback(*it);
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

        vertices[i].r = particles[i].color.GetComponent(SORE_Graphics::RED);
        vertices[i].g = particles[i].color.GetComponent(SORE_Graphics::GREEN);
        vertices[i].b = particles[i].color.GetComponent(SORE_Graphics::BLUE);
        vertices[i].a = particles[i].color.GetComponent(SORE_Graphics::ALPHA);

        indices[i] = i;
    }
}

void ParticleSystem::Update(int elapsed)
{
    std::sort(particles.begin(), particles.end());

    float seconds = elapsed / 1000.0f;

    for(Particles_t::iterator it = particles.begin(); it != particles.end(); ++it)
    {
        it->x += it->xv * seconds;
        it->y += it->yv * seconds;
        it->z += it->zv * seconds;

        it->xv += it->xa * seconds;
        it->yv += it->ya * seconds;
        it->zv += it->za * seconds;

        it->lifetime += seconds;

        it->color += it->colorChange * seconds;
    }
}

bool ParticleSystem::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        half_width = static_cast<float>(e.resize.w);
        return true;
    }
    return false;
}

bool operator<(const Particle& one, const Particle& two)
{
    return one.lifetime < two.lifetime;
}