#include "app_log.h"
#include "pipes.h"

#include <sore_material.h>
#include <sore_sample.h>

#include <boost/foreach.hpp>

ParticleShadowPipe::ParticleShadowPipe(
    SORE_Resource::MaterialPtr material_, int shadowmap_size,
    SORE_Resource::Texture2DPtr current_, 
    SORE_Resource::Texture2DPtr last_,
    SORE_Profiler::Profiler* profiler)
    : Pipe(profiler), material(material_), shadowmap(shadowmap_size, shadowmap_size, true, 0),
    one(current_), two(last_), current(&one), last(&two)
{
}

void ParticleShadowPipe::Swap()
{
    std::swap(current, last);
}

void ParticleShadowPipe::doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers)
{
    renderbuffers["shadowMap"] = &shadowmap;
    material->SetTexture("positions", *current);
}

SORE_Graphics::render_list& ParticleShadowPipe::beginRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    PROFILE_BLOCK("Particle shadow pipe", profiler);

    renderQueue.SetRenderbuffer(&shadowmap);
    renderQueue.AddCommand(SORE_Graphics::CLEAR_DEPTH_BUFFER);

    myList.clear();
    BOOST_FOREACH(const SORE_Graphics::Renderable& r, list)
    {
        myList.push_back(SORE_Graphics::Renderable(r, material));
    }

    return myList;
}

void ParticleShadowPipe::finishRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    renderQueue.SetRenderbuffer(0);
}

ParticleUpdatePipe::ParticleUpdatePipe(
    const std::vector<SORE_Resource::Texture2DPtr>& current_,
    const std::vector<SORE_Resource::Texture2DPtr>& last_, 
    SORE_Profiler::Profiler* profiler)
    : Pipe(profiler), started(false),
    one(current_), two(last_),
    current(&one), last(&two)
{
}

bool ParticleUpdatePipe::Swap()
{
    if(started)
        return false;

    APP_LOG(SORE_Logging::LVL_INFO, "Swap");

    std::swap(current, last);
    started = true;
    return true;
}

void ParticleUpdatePipe::doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers)
{
}

SORE_Graphics::render_list& ParticleUpdatePipe::beginRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    PROFILE_BLOCK("Particle update pipe", profiler);

    if(!started)
        return empty;

    APP_LOG(SORE_Logging::LVL_INFO, boost::format("RENDER: into %d") % current->Handle());
    renderQueue.SetRenderbuffer(current);
    started = false;

    return list;
}

void ParticleUpdatePipe::finishRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    renderQueue.SetRenderbuffer(0);
}

ParticleEmitterPipe::ParticleEmitterPipe(
    const std::vector<SORE_Resource::Texture2DPtr>& spawnTextures,
    SORE_Profiler::Profiler* profiler)
    : Pipe(profiler), spawning(false),
    spawn(spawnTextures)
{
}

void ParticleEmitterPipe::Spawn()
{
    spawning = true;
}

void ParticleEmitterPipe::doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers)
{
}

SORE_Graphics::render_list& ParticleEmitterPipe::beginRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    PROFILE_BLOCK("Particle emitter pipe", profiler);

    if(!spawning)
        return empty;

    APP_LOG(SORE_Logging::LVL_INFO, boost::format("SPAWN"));

    renderQueue.SetRenderbuffer(&spawn);
    renderQueue.AddCommand(SORE_Graphics::CLEAR_COLOR_AND_DEPTH_BUFFERS);
    spawning = false;

    return list;
}

void ParticleEmitterPipe::finishRender(
    const SORE_Graphics::camera_table& cameras,
    SORE_Graphics::Renderbuffer_map_t& renderbuffers,
    SORE_Graphics::render_list& list,
    SORE_Graphics::GLCommandList& renderQueue,
    SORE_Graphics::BufferManager* bm)
{
    renderQueue.SetRenderbuffer(0);
}