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
#include <sore_simplebuffermanager.h>

#include <vector>

namespace SORE_Profiler
{
    class Profiler;
}

class ParticleSystem : public SORE_Graphics::GeometryProvider
{
public:
    ParticleSystem(
        Particle_spawn_func_t spawn_func,
        size_t texture_size_w, size_t texture_size_h,
        SORE_Resource::Texture_cache_t& texture_cache,
        SORE_Resource::Shader_cache_t& shader_cache);

    ParticleUpdatePipe* GetUpdatePipe();
    ParticleEmitterPipe* GetEmitterPipe();
    SORE_Graphics::camera_info GetUpdateCamera();

    void SetEmitter(SORE_Resource::GLSLShaderPtr shader);
    void Update(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);

    template<typename T>
    void SetUniform(const std::string& name, const T& v)
    {
        for(std::vector<SORE_Graphics::Renderable>::iterator it = geometry.begin(); it != geometry.end(); ++it)
        {
            it->Uniforms().SetVariable(name, v);
        }
    }

    bool OnResize(const SORE_Kernel::Event& e);

    // renderer stuff interfaces
    void MakeUpToDate() { buffers.MakeUpToDate(); }
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::BufferManager* GetBufferManager() { return &buffers; }
private:
    SORE_Resource::GLSLShaderPtr update_shader, emitter_shader;

    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::SimpleBufferManager buffers;

    size_t texture_size_width, texture_size_height;
    float half_width;

    ParticleState state1, state2, spawns;
    ParticleState* current, *last;

    ParticleUpdatePipe* update_pipe;
    ParticleEmitterPipe* emitter_pipe;

    SORE_Resource::Texture_cache_t& texture_cache;
    SORE_Resource::Shader_cache_t& shader_cache;

    int time_since_update, time_since_spawn_update;
    float last_emit_time;
    float seed;
};

#endif