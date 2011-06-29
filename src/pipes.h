#ifndef PIPES_H
#define PIPES_H

#include <sore_fbo.h>
#include <sore_glslshader.h>
#include <sore_pipe.h>

class ParticleShadowPipe : public SORE_Graphics::Pipe
{
public:
    ParticleShadowPipe(SORE_Resource::GLSLShaderPtr shader_, int shadowmap_size);
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

#endif