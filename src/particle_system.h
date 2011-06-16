#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <sore_buffermanager.h>
#include <sore_color.h>
#include <sore_event.h>
#include <sore_geometryprovider.h>
#include <sore_vbo.h>

#include <vector>

struct Particle
{
    Particle(float x_ = 0.0f, float y_ = 0.0f, float z_ = 0.0f, float s_ = 0.0f) 
        : x(x_), y(y_), z(z_), size(s_), 
          xv(0.0f), yv(0.0f), zv(0.0f),
          xa(0.0f), ya(0.0f), za(0.0f),
          lifetime(0.0f), colorChange(0.0f)
    {}
    float x, y, z;
    float size;

    float xv, yv, zv; // velocity
    float xa, ya, za; // acceleration

    float lifetime;

    SORE_Graphics::Color color, colorChange;
};

class ParticleSystem : public SORE_Graphics::GeometryProvider, SORE_Graphics::BufferManager
{
public:
    ParticleSystem(
        size_t num_particles,
        SORE_Resource::Texture2DPtr t = SORE_Resource::Texture2DPtr(), 
        SORE_Resource::GLSLShaderPtr s = SORE_Resource::GLSLShaderPtr());

    void SetShader(SORE_Resource::GLSLShaderPtr s);
    void SetTexture(SORE_Resource::Texture2DPtr t);

    void Clear();
    void AddParticles(boost::function<void (Particle&)> create_callback, size_t num_particles);
    void Update(int elapsed);
    void SetSize(float size);

    bool OnResize(const SORE_Kernel::Event& e);

    // renderer stuff interfaces
    void MakeUpToDate();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::geometry_entry LookupGC(SORE_Graphics::GeometryChunkPtr gc);
    bool Contains(SORE_Graphics::GeometryChunkPtr gc);

    SORE_Graphics::BufferManager* GetBufferManager() { return this; }
private:
    void UpdateGeometry();

    typedef std::vector<Particle> Particles_t;
    Particles_t particles;

    SORE_Resource::Texture2DPtr texture;
    SORE_Resource::GLSLShaderPtr shader;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::geometry_entry geometry_lookup;
    SORE_Graphics::VBO vbo;

    float half_width;
};

#endif