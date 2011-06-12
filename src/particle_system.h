#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <sore_buffermanager.h>
#include <sore_color.h>
#include <sore_geometryprovider.h>
#include <sore_vbo.h>

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#include <vector>

struct Particle
{
    Particle(float x_, float y_, float z_, float s_) : x(x_), y(y_), z(z_), size(s_) {}
    float x, y, z;
    float size;
    SORE_Graphics::Color color;
};

class ParticleSystem : public SORE_Graphics::GeometryProvider, SORE_Graphics::BufferManager
{
public:
    ParticleSystem(SORE_Graphics::Texture2DPtr t, SORE_Graphics::GLSLShaderPtr s);

    void MakeUpToDate();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::geometry_entry LookupGC(SORE_Graphics::GeometryChunkPtr gc);
    bool Contains(SORE_Graphics::GeometryChunkPtr gc);

    SORE_Graphics::BufferManager* GetBufferManager() { return this; }

    void ClearParticles();
    void AddParticle(const Particle& p);
private:
    void UpdateGeometry();
    std::vector<Particle> particles;

    SORE_Graphics::Texture2DPtr texture;
    SORE_Graphics::GLSLShaderPtr shader;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::geometry_entry geometry_lookup;
    SORE_Graphics::VBO vbo;

    boost::mutex particle_mutex;
};

#endif