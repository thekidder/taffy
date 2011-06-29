#include "particle_system.h"

#include <boost/bind.hpp>
#include <sore_sprite.h>

#include <cassert>

ParticleSystem::ParticleSystem(
    size_t num_particles,
    SORE_Resource::Texture2DPtr t, 
    SORE_Resource::GLSLShaderPtr s)
    : particles(num_particles),
      texture(t), shader(s),
      vbo(SORE_Graphics::STREAM, true, true, true),
      size_multiplier(1.0f)
{
    SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(0, 0, GL_POINTS));
    SORE_Graphics::TransformationPtr trans(new SORE_Math::Matrix4<float>());

    geometry.push_back(SORE_Graphics::Renderable(g, s, trans, SORE_Graphics::BLEND_OPAQUE));

    geometry.front().AddTexture("texture", texture);
    geometry.front().AddTexture("shadowMap", SORE_Graphics::TextureState::TextureObject("particle-shadowmap"));
    geometry.front().AddKeyword("game");
    geometry.front().AddKeyword("particle");

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
        geometry.front().Uniforms().SetVariable("halfWidth", half_width);
    }

    vbo.Clear();
    vbo.AddObject(geometry.front().GetGeometryChunk());
    vbo.Build();

    geometry_lookup.geometry = &vbo;
    geometry_lookup.offset = 0;
    geometry_lookup.indices = num_to_render;
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
    size_multiplier = size;
}

void ParticleSystem::Clear()
{
    particles.clear();
}

struct AlphaComparator
{
    bool operator()(const Particle& one, const Particle& two) const
    {
        return one.color.GetColor()[3] > two.color.GetColor()[3];
    }
};

void ParticleSystem::AddParticles(boost::function<void (Particle&)> create_callback, size_t num_particles)
{
    assert(num_particles <= particles.size());

    std::sort(particles.begin(), particles.end(), AlphaComparator());

    Particles_t::iterator it = particles.end() - num_particles;
    for(it; it != particles.end(); ++it)
        create_callback(*it);
}

struct CameraDepthCalculator
{
    CameraDepthCalculator(const SORE_Math::Matrix4<float>& v) : view(v) {}

    void operator()(Particle& one) const
    {
        one.camera_depth = one.x*view.GetData()[2] + 
            one.y*view.GetData()[6] + 
            one.z*view.GetData()[10] +
            view.GetData()[14]; 
    }

    const SORE_Math::Matrix4<float>& view;
};

struct Invisible
{
    bool operator()(const Particle& one) const
    {
        return one.color.GetColor()[3] <= 0.0f || one.size <= 0.0f;
    }
};

struct CameraDepthComparator
{
    bool operator()(const Particle& one, const Particle& two) const
    {
        if(!Invisible()(one) && Invisible()(two))
            return true;
        else if(Invisible()(one) && !Invisible()(two))
            return false;
        return one.camera_depth < two.camera_depth;
    }
};

void ParticleSystem::UpdateGeometry()
{
    //std::for_each(particles.begin(), particles.end(), CameraDepthCalculator(view_matrix));
    //std::sort(particles.begin(), particles.end(), CameraDepthComparator());

    //Particles_t::iterator it = std::find_if(particles.begin(), particles.end(), Invisible());
    num_to_render = particles.size();//it - particles.begin();

    SORE_Graphics::GeometryChunkPtr g(
        new SORE_Graphics::GeometryChunk(particles.size(), particles.size(), GL_POINTS));

    geometry.front().SetGeometryChunk(g);

    SORE_Graphics::vertex* const vertices = geometry.front().GetGeometryChunk()->GetVertices();
    unsigned short* const indices = geometry.front().GetGeometryChunk()->GetIndices();

    for(int i = 0; i < num_to_render; ++i)
    {
        vertices[i].x = particles[i].x;
        vertices[i].y = particles[i].y;
        vertices[i].z = particles[i].z;

        vertices[i].r = particles[i].color.GetComponent(SORE_Graphics::RED);
        vertices[i].g = particles[i].color.GetComponent(SORE_Graphics::GREEN);
        vertices[i].b = particles[i].color.GetComponent(SORE_Graphics::BLUE);
        vertices[i].a = particles[i].color.GetComponent(SORE_Graphics::ALPHA);
        vertices[i].normx = particles[i].size * size_multiplier;

        indices[i] = i;
    }
}

void ParticleSystem::Update(int elapsed)
{
    //float seconds = elapsed / 1000.0f;

    //for(Particles_t::iterator it = particles.begin(); it != particles.end(); ++it)
    //{
    //    it->x += it->xv * seconds;
    //    it->y += it->yv * seconds;
    //    it->z += it->zv * seconds;

    //    it->xv += it->xa * seconds;
    //    it->yv += it->ya * seconds;
    //    it->zv += it->za * seconds;

    //    it->lifetime += seconds;

    //    it->color += it->colorChange * seconds;
    //}
}

void ParticleSystem::SetView(const SORE_Math::Matrix4<float>& view)
{
    view_matrix = view;
}

bool ParticleSystem::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        half_width = static_cast<float>(e.resize.w / 2.0f);
        return true;
    }
    return false;
}
