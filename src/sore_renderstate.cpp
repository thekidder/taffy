/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "sore_renderstate.h"

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
        textures = &r.GetTextures();
    }

    if(!r.Uniforms().Empty())
    {
        commands |= RENDER_CMD_CHANGE_UNIFORMS;
        uniforms = &r.Uniforms();
    }
}

SORE_Graphics::RenderState SORE_Graphics::RenderState::Difference(
    const Renderable& r, camera_info cam) const
{
    RenderState newState;
    if(camera != cam)
    {
        newState.commands |= RENDER_CMD_CHANGE_CAMERA;
        newState.camera = cam;
    }

    if(r.GetBlendMode() != blend)
    {
        newState.commands |= RENDER_CMD_CHANGE_BLEND_MODE;
        newState.blend = r.GetBlendMode();
    }

    if(r.GetShader() != shader)
    {
        newState.commands |= RENDER_CMD_BIND_SHADER;
        newState.shader = r.GetShader();
    }

    if(r.GetTextures() != textures)
    {
        newState.commands |= RENDER_CMD_BIND_TEXTURE;
        newState.textures = r.GetTextures();
    }

    if(r.Uniforms() !] uniforms)
    {
        newState.commands |= RENDER_CMD_CHANGE_UNIFORMS;
        newState.uniforms = r.Uniforms();
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

void SORE_Graphics::RenderBatch::ChangeProjectionMatrix(
    const ProjectionInfo& projection)
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
        gluOrtho2D(projection.left, projection.right,
                   projection.top, projection.bottom);
        break;
    case ORTHO:
        //TODO: finish ortho projection
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

void SORE_Graphics::RenderBatch::ChangeCameraMatrix(
    const SORE_Math::Matrix4<float>& camera)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(camera.GetData());
}