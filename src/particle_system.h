#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "particle_texture_loader.h"
#include "pipes.h"

#include <sore_assettypes.h>
#include <sore_buffermanager.h>
#include <sore_color.h>
#include <sore_event.h>
#include <sore_geometryprovider.h>
#include <sore_immediatemodeprovider.h>
#include <sore_vbo.h>

#include <vector>

namespace SORE_Profiler
{
    class Profiler;
}

class ParticleSystem : public SORE_Graphics::GeometryProvider, SORE_Graphics::BufferManager
{
public:
    ParticleSystem(
        Particle_spawn_func_t spawn_func,
        size_t texture_size_w, size_t texture_size_h,
        SORE_Resource::Texture_cache_t& texture_cache,
        SORE_Resource::Shader_cache_t& shader_cache);

    ParticleUpdatePipe* GetUpdatePipe();
    SORE_Graphics::camera_info GetUpdateCamera();

    void Update(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);

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
    void AddParticles(Particle_spawn_func_t spawn_func);

    SORE_Resource::GLSLShaderPtr update_shader;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::geometry_entry geometry_lookup;
    SORE_Graphics::VBO vbo;

    size_t texture_size_width, texture_size_height;
    float half_width;

    ParticleState state1, state2;
    ParticleState* current, *last;

    ParticleUpdatePipe* updatePipe;

    SORE_Resource::Texture_cache_t& texture_cache;
    SORE_Resource::Shader_cache_t& shader_cache;
};

#endif