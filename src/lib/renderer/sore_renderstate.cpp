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
#include <sore_renderstate.h>

SORE_Graphics::RenderState::RenderState() : commands(0)
{
}

SORE_Graphics::RenderState::RenderState(const Renderable& r, camera_info cam)
  : commands(0)
{
    commands |= RENDER_CMD_CHANGE_CAMERA;
    camera = cam;

    commands |= RENDER_CMD_CHANGE_BLEND_MODE;
    blend = r.GetBlendMode();

    if(r.GetShader())
    {
        commands |= RENDER_CMD_BIND_SHADER;
        shader = r.GetShader();
    }

    if(!r.GetTextures().Empty())
    {
        commands |= RENDER_CMD_BIND_TEXTURE;
        textures = r.GetTextures();
    }

    UniformState copy = r.Uniforms();
    copy.SetVariable("transform", *r.GetTransform());

    if(!copy.Empty())
    {
        commands |= RENDER_CMD_CHANGE_UNIFORMS;
        uniforms = copy;
    }

    primitiveType = r.GetGeometryChunk()->Type();
    commands |= RENDER_CMD_CHANGE_PRIMITIVE;
}

SORE_Graphics::RenderState SORE_Graphics::RenderState::Difference(const RenderState& old) const
{
    RenderState newState;
    newState.camera = camera;
    newState.blend = blend;
    newState.shader = shader;

    if(old.camera != camera)
    {
        newState.commands |= RENDER_CMD_CHANGE_CAMERA;
    }

    if(old.blend != blend)
    {
        newState.commands |= RENDER_CMD_CHANGE_BLEND_MODE;
    }

    if(!old.shader || old.shader != shader)
    {
        newState.commands |= RENDER_CMD_BIND_SHADER;
    }

    TextureState tDiff = textures.GetDiff(old.textures);
    if(!tDiff.Empty())
    {
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
        newState.uniforms = uDiff;
    else
        newState.uniforms = UniformState();

    if(!newState.uniforms.Empty())
        newState.commands |= RENDER_CMD_CHANGE_UNIFORMS;

    if(old.primitiveType != primitiveType)
    {
        newState.commands |= RENDER_CMD_CHANGE_PRIMITIVE;
    }

    return newState;
}

bool SORE_Graphics::RenderState::Empty() const
{
    return commands == 0;
}

void SORE_Graphics::RenderState::Apply() const
{
    if(commands & RENDER_CMD_CHANGE_CAMERA)
    {
        ChangeProjectionMatrix(camera.projection);
        ChangeCameraMatrix(camera.viewMatrix);
    }
    if(commands & RENDER_CMD_CHANGE_BLEND_MODE)
    {
        unsigned int blendSFactor, blendDFactor;
        switch(blend)
        {
        case BLEND_OPAQUE:
            glEnable(GL_DEPTH_TEST);
            blendSFactor = GL_ONE;
            blendDFactor = GL_ZERO;
            break;
        case BLEND_ADDITIVE:
            glDisable(GL_DEPTH_TEST);
            blendSFactor = GL_SRC_ALPHA;
            blendDFactor = GL_DST_ALPHA;
            break;
        case BLEND_SUBTRACTIVE:
        default: //treat unknown type as subtractive by default
            glDisable(GL_DEPTH_TEST);
            blendSFactor = GL_SRC_ALPHA;
            blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        glBlendFunc(blendSFactor, blendDFactor);
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
}

void SORE_Graphics::RenderState::ChangeProjectionMatrix(
    const ProjectionInfo& projection) const
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    switch(projection.type)
    {
    case NONE:
        ENGINE_LOG(SORE_Logging::LVL_ERROR,
                   "No projection type set, could not initialize projection");
        break;
    case ORTHO2D:
        glOrtho(projection.left, projection.right,
                projection.top, projection.bottom, -1.0f, 1.0f);
        break;
    case ORTHO:
        glOrtho(projection.left, projection.right,
                projection.top, projection.bottom, projection.znear, projection.zfar);
        break;
    case PERSPECTIVE:
        {
            gluPerspective(projection.fov, projection.ratio,
                           projection.znear, projection.zfar );
            break;
        }
    default:
        break;
    }
    glMatrixMode(GL_MODELVIEW);
}

void SORE_Graphics::RenderState::ChangeCameraMatrix(
    const SORE_Math::Matrix4<float>& camera) const
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    if(camera != SORE_Math::Matrix4<float>())
    {
        glMultMatrixf(camera.GetData());
    }
}
