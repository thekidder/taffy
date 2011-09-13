#include "app_log.h"
#include "particle_system.h"

#include <boost/bind.hpp>

#include <sore_glslshader_loader.h>
#include <sore_profiler.h>
#include <sore_texture2d_loader.h>
#include <sore_timing.h>

#include <cassert>

const unsigned int k_shadowmap_size = 2048;

ParticleSystem::ParticleSystem(
    size_t texture_size_w, size_t texture_size_h,
        SORE_Resource::MaterialPtr shadowMaterial_,
        SORE_Resource::MaterialPtr updateMaterial_,
        SORE_Resource::MaterialPtr drawMaterial_,
        SORE_Resource::MaterialPtr emitterMaterial_)
    : texture_size_width(texture_size_w),
      texture_size_height(texture_size_h),
      current(&state1), last(&state2),
      updateMaterial(updateMaterial_), drawMaterial(drawMaterial_), emitterMaterial(emitterMaterial_),
      time_since_update(10000), time_since_spawn_update(10000), last_emit_time(0.0f), timer(0.0f),
      spawned(false), updated(false)
{
    size_t num_particles = texture_size_width * texture_size_height;
    size_t constructed = 0;
    while(num_particles > 0)
    {
        size_t gc_particles = std::min(static_cast<size_t>(std::numeric_limits<unsigned short>::max()) + 1, num_particles);
        num_particles -= gc_particles;

        SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(gc_particles, gc_particles, GL_POINTS));
        SORE_Graphics::MatrixPtr trans(new SORE_Math::Matrix4<float>());

        unsigned short* const indices = g->GetIndices();
        SORE_Graphics::vertex* const vertices = g->GetVertices();

        for(size_t j = 0; j < gc_particles; ++j, ++constructed)
        {
            float h = static_cast<float>(constructed / texture_size_width);
            float w = static_cast<float>(constructed % texture_size_width);
            indices [j] = j;
            vertices[j].tex0i = w / texture_size_width;
            vertices[j].tex0j = h / texture_size_height;
        }
        buffers.GeometryAdded(g, SORE_Graphics::STATIC);
        geometry.push_back(SORE_Graphics::Renderable(g, trans, drawMaterial));
        geometry.back().AddKeyword("game");
        geometry.back().AddKeyword("particle");
    }

    ParticleTextureLoader loader(texture_size_width, texture_size_height);
    state1 = loader.Create();
    state2 = loader.Create();
    spawns = loader.Create();

    std::vector<SORE_Resource::Texture2DPtr> currentVec;
    currentVec.push_back(current->positions);
    currentVec.push_back(current->colors);
    currentVec.push_back(current->data);

    std::vector<SORE_Resource::Texture2DPtr> lastVec;
    lastVec.push_back(last->positions);
    lastVec.push_back(last->colors);
    lastVec.push_back(last->data);

    std::vector<SORE_Resource::Texture2DPtr> spawnVec;
    spawnVec.push_back(spawns.positions);
    spawnVec.push_back(spawns.colors);
    spawnVec.push_back(spawns.data);

    shadow_pipe = new ParticleShadowPipe(shadowMaterial_, k_shadowmap_size, current->positions, last->positions, 0);
    update_pipe = new ParticleUpdatePipe(currentVec, lastVec, 0);
    emitter_pipe = new ParticleEmitterPipe(spawnVec, 0);

    seed = time(0) / 397641.6234315f;
}

ParticleShadowPipe* ParticleSystem::GetShadowPipe()
{
    return shadow_pipe;
}

ParticleUpdatePipe* ParticleSystem::GetUpdatePipe()
{
    return update_pipe;
}

ParticleEmitterPipe* ParticleSystem::GetEmitterPipe()
{
    return emitter_pipe;
}

SORE_Graphics::camera_info ParticleSystem::GetUpdateCamera()
{
    SORE_Graphics::ProjectionInfo proj;
    proj.type = SORE_Graphics::ORTHO2D;
    proj.left = 0.0f;
    proj.right = 1.0f;
    proj.top = 0.0f;
    proj.bottom = 1.0f;

    SORE_Math::Matrix4<float> view;
    SORE_Graphics::camera_info cam = {proj, view};
    return cam;
}

std::vector<SORE_Graphics::Renderable>::iterator ParticleSystem::GeometryBegin()
{
    return geometry.begin();
}

std::vector<SORE_Graphics::Renderable>::iterator ParticleSystem::GeometryEnd()
{
    return geometry.end();
}

void ParticleSystem::Update(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    time_since_update += elapsed;
    time_since_spawn_update += elapsed;
    timer += static_cast<float>(elapsed);
    if(!updated && update_pipe->Swap())
    {
        shadow_pipe->Swap();
        APP_LOG(SORE_Logging::LVL_INFO, boost::format("UPDATE: render from %d (%f)") % current->positions->Handle() % (time_since_update / 1000.0f));
        for(std::vector<SORE_Graphics::Renderable>::iterator it = geometry.begin(); it != geometry.end(); ++it)
        {
            it->SetTexture("colors", current->colors);
            it->SetTexture("positions", current->positions);
        }

        imm_mode.SetKeywords("particle_update");
        imm_mode.SetMaterial(updateMaterial);
        imm_mode.SetTexture("positions", current->positions);
        imm_mode.SetTexture("colors", current->colors);
        imm_mode.SetTexture("data", current->data);
        imm_mode.SetUniform("elapsed", time_since_update / 1000.0f);
        imm_mode.SetUniform("t", timer);

        imm_mode.SetTexture("positions_spawn", spawns.positions);
        imm_mode.SetTexture("colors_spawn", spawns.colors);
        imm_mode.SetTexture("data_spawn", spawns.data);

        imm_mode.DrawQuad(
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f);

        std::swap(current, last);

        time_since_update = 0;

        updated = true;
    }
    if(!spawned && time_since_spawn_update >= 80)
    //if(!spawned && emitter_shader)
    {
        emitter_pipe->Spawn();

        imm_mode.SetKeywords("particle_emitter");
        imm_mode.SetMaterial(emitterMaterial);

        APP_LOG(SORE_Logging::LVL_INFO, boost::format("SPAWN (%f, %f)") % timer % last_emit_time);

        imm_mode.SetUniform("seed", seed);
        imm_mode.SetUniform("t", timer / 10000.0f);
        imm_mode.SetUniform("old_t", last_emit_time);
        imm_mode.DrawQuad(
            0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f);

        time_since_spawn_update = 0;
        last_emit_time = timer;

        spawned = true;
    }
}

bool ParticleSystem::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        for(std::vector<SORE_Graphics::Renderable>::iterator it = geometry.begin(); it != geometry.end(); ++it)
        {
            it->SetUniform("halfWidth", e.resize.w / 2.0f);
        }
        return true;
    }
    return false;
}
