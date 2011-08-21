#ifndef PIPES_H
#define PIPES_H

#include "particle_texture_loader.h"

#include <sore_fbo.h>
#include <sore_glslshader.h>
#include <sore_pipe.h>

namespace SORE_Profiler
{
    class Profiler;
}

class ParticleShadowPipe : public SORE_Graphics::Pipe
{
public:
    ParticleShadowPipe(SORE_Resource::GLSLShaderPtr shader_, int shadowmap_size, SORE_Profiler::Profiler* profiler);
protected:
    virtual void doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers);
    virtual SORE_Graphics::render_list& beginRender(
        const SORE_Graphics::camera_table& cameras,
        SORE_Graphics::Renderbuffer_map_t& renderbuffers,
        SORE_Graphics::render_list& list,
        SORE_Graphics::GLCommandList& renderQueue,
        SORE_Graphics::BufferManager* bm);
    virtual void finishRender(
        const SORE_Graphics::camera_table& cameras,
        SORE_Graphics::Renderbuffer_map_t& renderbuffers,
        SORE_Graphics::render_list& list,
        SORE_Graphics::GLCommandList& renderQueue,
        SORE_Graphics::BufferManager* bm);
private:
    SORE_Resource::GLSLShaderPtr shader;
    SORE_Graphics::FBO shadowmap;
};

class ParticleUpdatePipe : public SORE_Graphics::Pipe
{
public:
    ParticleUpdatePipe(
        const std::vector<SORE_Resource::Texture2DPtr>& current_, 
        const std::vector<SORE_Resource::Texture2DPtr>& last_,
        SORE_Profiler::Profiler* profiler);

        bool Swap();
protected:
    virtual void doSetup(SORE_Graphics::Renderbuffer_map_t& renderbuffers);
    virtual SORE_Graphics::render_list& beginRender(
        const SORE_Graphics::camera_table& cameras,
        SORE_Graphics::Renderbuffer_map_t& renderbuffers,
        SORE_Graphics::render_list& list,
        SORE_Graphics::GLCommandList& renderQueue,
        SORE_Graphics::BufferManager* bm);
    virtual void finishRender(
        const SORE_Graphics::camera_table& cameras,
        SORE_Graphics::Renderbuffer_map_t& renderbuffers,
        SORE_Graphics::render_list& list,
        SORE_Graphics::GLCommandList& renderQueue,
        SORE_Graphics::BufferManager* bm);
private:
    bool started;

    SORE_Graphics::render_list empty;

    SORE_Graphics::FBO one, two;
    SORE_Graphics::FBO* current, *last;
};

#endif