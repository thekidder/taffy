#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "particle_texture_loader.h"

#include <sore_buffermanager.h>
#include <sore_color.h>
#include <sore_event.h>
#include <sore_geometryprovider.h>
#include <sore_vbo.h>

#include <vector>

class ParticleSystem : public SORE_Graphics::GeometryProvider, SORE_Graphics::BufferManager
{
public:
    ParticleSystem(
        size_t texture_size_,
        SORE_Resource::Texture2DPtr t = SORE_Resource::Texture2DPtr(), 
        SORE_Resource::GLSLShaderPtr s = SORE_Resource::GLSLShaderPtr());

    void SetShader(SORE_Resource::GLSLShaderPtr s);
    void SetTexture(SORE_Resource::Texture2DPtr t);

    void AddParticles(Particle_spawn_func_t spawn_func);
    void Update(int elapsed);

    SORE_Graphics::UniformState& Uniforms() { return geometry.front().Uniforms(); }

    bool OnResize(const SORE_Kernel::Event& e);

    // renderer stuff interfaces
    void MakeUpToDate();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::geometry_entry LookupGC(SORE_Graphics::GeometryChunkPtr gc);
    bool Contains(SORE_Graphics::GeometryChunkPtr gc);

    SORE_Graphics::BufferManager* GetBufferManager() { return this; }
private:
    SORE_Resource::Texture2DPtr texture;
    SORE_Resource::GLSLShaderPtr shader;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::geometry_entry geometry_lookup;
    SORE_Graphics::VBO vbo;

    size_t texture_size;
    float half_width;

    ParticleState state;
};

#endif