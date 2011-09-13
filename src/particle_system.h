#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "app_log.h"
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

class ParticleSystem : public SORE_Graphics::GeometryProvider, public SORE_Graphics::BufferManager
{
public:
    ParticleSystem(
        size_t texture_size_w, size_t texture_size_h,
        SORE_Resource::MaterialPtr shadowMaterial_,
        SORE_Resource::MaterialPtr updateMaterial_,
        SORE_Resource::MaterialPtr drawMaterial_,
        SORE_Resource::MaterialPtr emitterMaterial_);

    ParticleShadowPipe* GetShadowPipe();
    ParticleUpdatePipe* GetUpdatePipe();
    ParticleEmitterPipe* GetEmitterPipe();
    SORE_Graphics::camera_info GetUpdateCamera();

    void Update(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode);

    template<typename T>
    void SetUniform(const std::string& name, const T& v)
    {
        for(std::vector<SORE_Graphics::Renderable>::iterator it = geometry.begin(); it != geometry.end(); ++it)
        {
            it->SetUniform(name, v);
        }
    }

    bool OnResize(const SORE_Kernel::Event& e);

    // renderer stuff interfaces
    void MakeUpToDate() { APP_LOG(SORE_Logging::LVL_INFO, "MakeUpToDate"); updated = spawned = false; buffers.MakeUpToDate(); }
    SORE_Graphics::geometry_entry LookupGC(SORE_Graphics::GeometryChunkPtr gc) { return buffers.LookupGC(gc); }
    bool Contains(SORE_Graphics::GeometryChunkPtr gc) { return buffers.Contains(gc); }
    std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
    std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

    SORE_Graphics::BufferManager* GetBufferManager() { return this; }
private:
    std::vector<SORE_Graphics::Renderable> geometry;
    SORE_Graphics::SimpleBufferManager buffers;

    size_t texture_size_width, texture_size_height;
    float half_width;

    ParticleState state1, state2, spawns;
    ParticleState* current, *last;

    ParticleShadowPipe* shadow_pipe;
    ParticleUpdatePipe* update_pipe;
    ParticleEmitterPipe* emitter_pipe;

    SORE_Resource::MaterialPtr updateMaterial, drawMaterial, emitterMaterial;

    int time_since_update, time_since_spawn_update;
    float last_emit_time, timer;
    float seed;

    bool spawned, updated;
};

#endif