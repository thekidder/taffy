/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#include <sore_logger.h>
#include <sore_material.h>
#include <sore_renderstate.h>

SORE_Graphics::RenderState::RenderState() : commands(0), renderbuffer(0)
{
}

SORE_Graphics::RenderState::RenderState(const Renderable& r, camera_info cam)
  : commands(0), renderbuffer(0)
{
    camera = cam;

    commands |= RENDER_CMD_CHANGE_BLEND_MODE;
    blendState = r.material->blendState;
    shader = r.material->shader;

    if(shader)
    {
        commands |= RENDER_CMD_BIND_SHADER;
    }

    textures = r.material->textures;
    if(!textures.Empty())
    {
        commands |= RENDER_CMD_BIND_TEXTURE;
    }

    UniformState copy = r.material->uniforms;
    copy.SetVariable("projection", GetProjectionMatrix(cam.projection));
    copy.SetVariable("view", cam.viewMatrix);
    copy.SetVariable("model", *r.GetModelMatrix());

    if(!copy.Empty())
    {
        commands |= RENDER_CMD_CHANGE_UNIFORMS;
        uniforms = copy;
    }

    primitiveType = r.geometry->Type();
    commands |= RENDER_CMD_CHANGE_PRIMITIVE;
}

SORE_Graphics::RenderState SORE_Graphics::RenderState::Difference(const RenderState& old) const
{
    RenderState newState;
    newState.blendState = blendState;
    newState.shader = shader;
    newState.renderbuffer = renderbuffer;
    newState.camera = camera;
    newState.primitiveType = primitiveType;

    if(old.blendState != blendState)
    {
        newState.commands |= RENDER_CMD_CHANGE_BLEND_MODE;
    }

    if(shader && (!old.shader || old.shader != shader))
    {
        newState.commands |= RENDER_CMD_BIND_SHADER;
    }
    
    // ALWAYS rebind full texture state if the shader changes
    TextureState tDiff;
    if(newState.commands & RENDER_CMD_BIND_SHADER)
        tDiff = textures;
    else
        tDiff = textures.GetDiff(old.textures);

    if(!tDiff.Empty())
    {
        if(!shader)
            throw std::runtime_error("Trying to bind texture with nonexistent shader");
        newState.commands |= RENDER_CMD_BIND_TEXTURE;
        newState.textures = tDiff;
    }
    else
    {
        newState.textures = TextureState();
    }

    UniformState uDiff = uniforms.GetDiff(old.uniforms);
    // reset uniforms if they have changed OR if there is a new shader bound
    if(newState.commands & RENDER_CMD_BIND_SHADER)
        newState.uniforms = uniforms;
    else if(!uDiff.Empty())
    {
        if(!shader)
            throw std::runtime_error("Trying to bind uniform with nonexistent shader");
        newState.uniforms = uDiff;
    }
    else
        newState.uniforms = UniformState();

    if(!newState.uniforms.Empty())
        newState.commands |= RENDER_CMD_CHANGE_UNIFORMS;

    if(old.primitiveType != primitiveType)
    {
        newState.commands |= RENDER_CMD_CHANGE_PRIMITIVE;
    }

    if(old.renderbuffer != renderbuffer)
    {
        newState.commands |= RENDER_CMD_CHANGE_FBO;
    }

    return newState;
}

bool SORE_Graphics::RenderState::Empty() const
{
    return commands == 0;
}

void SORE_Graphics::RenderState::Apply() const
{
    if(camera.projection.type != NONE)
        GetProjectionMatrix(camera.projection);
    if(commands & RENDER_CMD_CHANGE_BLEND_MODE)
    {
        if(blendState.depthTest)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        glBlendFunc(blendState.srcFactor, blendState.dstFactor);
    }
    if(commands & RENDER_CMD_BIND_SHADER)
    {
        shader->Bind();
    }
    if(commands & RENDER_CMD_BIND_TEXTURE)
    {
        textures.Bind(shader);
    }
    if(commands & RENDER_CMD_CHANGE_UNIFORMS)
    {
        uniforms.Bind(shader);
    }
    if(commands & RENDER_CMD_CHANGE_FBO)
    {
        if(!renderbuffer)
        {
            FBO::Unbind();
        }
        else
        {
            renderbuffer->Bind();
            renderbuffer->Draw();
        }
    }
}

void SORE_Graphics::RenderState::SetRenderbuffer(FBO* const renderbuffer_)
{
    renderbuffer = renderbuffer_;

    commands |= RENDER_CMD_CHANGE_FBO;
}

SORE_Math::Matrix4<float> SORE_Graphics::RenderState::GetProjectionMatrix(
    const ProjectionInfo& projection) const
{
    switch(projection.type)
    {
    case ORTHO2D:
    case ORTHO:
        return SORE_Math::Matrix4<float>::GetOrtho(
            projection.left, projection.right, projection.bottom, projection.top, projection.znear, projection.zfar);
    case PERSPECTIVE:
        return SORE_Math::Matrix4<float>::GetPerspective(projection.fov, projection.ratio, projection.znear, projection.zfar);
    case NONE:
    default:
        // todo: SORE exceptions
        throw std::runtime_error("No projection type set, could not initialize projection");
        break;
    }
}
