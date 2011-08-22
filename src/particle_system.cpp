#include "app_log.h"
#include "particle_system.h"

#include <boost/bind.hpp>

#include <sore_glslshader_loader.h>
#include <sore_profiler.h>
#include <sore_sprite.h>
#include <sore_texture2d_loader.h>

#include <cassert>

ParticleSystem::ParticleSystem(
    Particle_spawn_func_t spawn_func,
    size_t texture_size_w, size_t texture_size_h,
    SORE_Resource::Texture_cache_t& texture_cache_,
    SORE_Resource::Shader_cache_t& shader_cache_)
    : texture_size_width(texture_size_w),
      texture_size_height(texture_size_h),
      vbo(SORE_Graphics::STATIC, true, false, false),
      current(&state1), last(&state2),
      texture_cache(texture_cache_), shader_cache(shader_cache_),
      time_since_update(0), time_since_spawn_update(0)
{
    size_t num_particles = texture_size_width * texture_size_height;

    SORE_Graphics::GeometryChunkPtr g(new SORE_Graphics::GeometryChunk(num_particles, num_particles, GL_POINTS));
    SORE_Graphics::TransformationPtr trans(new SORE_Math::Matrix4<float>());

    unsigned short* const indices = g->GetIndices();
    SORE_Graphics::vertex* const vertices = g->GetVertices();

    for(size_t i = 0; i < texture_size_width; ++i)
    {
        for(size_t j = 0; j < texture_size_height; ++j)
        {
            indices [j * texture_size_width + i] = j * texture_size_width + i;
            vertices[j * texture_size_width + i].tex0i = i / static_cast<float>(texture_size_width);
            vertices[j * texture_size_width + i].tex0j = j / static_cast<float>(texture_size_height);
        }
    }

    geometry.push_back(SORE_Graphics::Renderable(g, SORE_Resource::GLSLShaderPtr(), trans, SORE_Graphics::BLEND_OPAQUE));

    geometry.front().AddTexture("shadowMap", SORE_Graphics::TextureState::TextureObject("particle-shadowmap"));
    geometry.front().AddKeyword("game");
    geometry.front().AddKeyword("particle");

    vbo.Clear();
    vbo.AddObject(geometry.front().GetGeometryChunk());
    vbo.Build();

    geometry_lookup.geometry = &vbo;
    geometry_lookup.offset = 0;
    geometry_lookup.indices = geometry.front().GetGeometryChunk()->NumIndices();
    geometry_lookup.vertices = geometry.front().GetGeometryChunk()->NumVertices();
    geometry_lookup.type = geometry.front().GetGeometryChunk()->Type();

    update_shader = shader_cache.Get("particles_update.shad");

    geometry.front().AddTexture("texture", texture_cache.Get("particle.tga"));

    AddParticles(spawn_func);

    std::vector<SORE_Resource::Texture2DPtr> currentVec;
    currentVec.push_back(current->positions);
    currentVec.push_back(current->colors);
    currentVec.push_back(current->data);

    std::vector<SORE_Resource::Texture2DPtr> lastVec;
    lastVec.push_back(last->positions);
    lastVec.push_back(last->colors);
    lastVec.push_back(last->data);

    updatePipe = new ParticleUpdatePipe(currentVec, lastVec, 0);
}

ParticleUpdatePipe* ParticleSystem::GetUpdatePipe()
{
    return updatePipe;
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

void ParticleSystem::MakeUpToDate()
{
    geometry.front().SetShader(shader_cache.Get("particles.shad"));

    geometry.front().AddTexture("colors", current->colors);
    geometry.front().AddTexture("positions", current->positions);
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

void NullSpawner(ParticleSpawn& p)
{
}

void ParticleSystem::AddParticles(Particle_spawn_func_t spawn_func_)
{
    spawn_func = spawn_func_;

    ParticleTextureLoader loader(texture_size_width, texture_size_height);
    state1 = loader.Load(spawn_func);
    state2 = loader.Load(spawn_func);
    spawns = loader.Load(spawn_func);
}

void ParticleSystem::Update(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
{
    time_since_update += elapsed;
    time_since_spawn_update += elapsed;
    if(updatePipe->Swap())
    {
        imm_mode.SetKeywords("particle_update");
        imm_mode.SetShader(update_shader);
        imm_mode.SetTexture("positions", current->positions);
        imm_mode.SetTexture("colors", current->colors);
        imm_mode.SetTexture("data", current->data);
        imm_mode.SetUniform("elapsed", time_since_update / 1000.0f);

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
    }
    if(time_since_spawn_update > 2000)
    {
        ParticleTextureLoader loader(texture_size_width, texture_size_height);
        spawns = loader.Load(spawn_func);

        time_since_spawn_update = 0;
    }
}

bool ParticleSystem::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        geometry.front().Uniforms().SetVariable("halfWidth", e.resize.w / 2.0f);
        return true;
    }
    return false;
}
