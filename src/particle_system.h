#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <sore_buffermanager.h>
#include <sore_color.h>
#include <sore_geometryprovider.h>
#include <sore_vbo.h>

#include <vector>

struct Particle
{
    float x, y, z;
    float size;
    SORE_Graphics::Color color;
};

class ParticleSystem : public SORE_Graphics::GeometryProvider, SORE_Graphics::BufferManager
{
public:
    ParticleSystem(SORE_Graphics::Texture2DPtr texture, SORE_Graphics::GLSLShaderPtr shader);

    void MakeUpToDate();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::geometry_entry LookupGC(SORE_Graphics::GeometryChunkPtr gc);
    bool Contains(SORE_Graphics::GeometryChunkPtr gc);

    SORE_Graphics::BufferManager* GetBufferManager() { return this; }
private:
    std::vector<Particle> particles;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::geometry_entry geometry_lookup;
    SORE_Graphics::VBO vbo;
};

#endif