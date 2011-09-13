#ifndef PIPES_H
#define PIPES_H

#include "particle_texture_loader.h"

#include <sore_assettypes.h>
#include <sore_fbo.h>
#include <sore_pipe.h>

namespace SORE_Profiler
{
    class Profiler;
}

class ParticleShadowPipe : public SORE_Graphics::Pipe
{
public:
    ParticleShadowPipe(
        SORE_Resource::MaterialPtr material_, int shadowmap_size,
        SORE_Resource::Texture2DPtr current_, 
        SORE_Resource::Texture2DPtr last_,
        SORE_Profiler::Profiler* profiler);

    void Swap();
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
    SORE_Graphics::render_list myList;

    SORE_Resource::MaterialPtr material;
    SORE_Graphics::FBO shadowmap;

    SORE_Resource::Texture2DPtr one, two;
    SORE_Resource::Texture2DPtr* current, *last;
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

class ParticleEmitterPipe : public SORE_Graphics::Pipe
{
public:
    ParticleEmitterPipe(
        const std::vector<SORE_Resource::Texture2DPtr>& spawnTextures,
        SORE_Profiler::Profiler* profiler);

        void Spawn();
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
    bool spawning;

    SORE_Graphics::render_list empty;

    SORE_Graphics::FBO spawn;
};

#endif