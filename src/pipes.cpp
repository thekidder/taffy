#include "pipes.h"

#include <sore_sample.h>

#include <boost/foreach.hpp>

ParticleShadowPipe::ParticleShadowPipe(SORE_Resource::GLSLShaderPtr shader_, int shadowmap_size, SORE_Profiler::Profiler* profiler)
    : Pipe(profiler), shader(shader_), shadowmap(shadowmap_size, shadowmap_size, true, 0)
{
}

void ParticleShadowPipe::doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers)
{
    renderbuffers["particle-shadowmap"] = &shadowmap;
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

    BOOST_FOREACH(SORE_Graphics::Renderable& r, list)
    {
        r.SetShader(shader);
        //r.Uniforms() = SORE_Graphics::UniformState();
        //r.Textures() = SORE_Graphics::TextureState();
    }

    return list;
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