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

#include "sore_batch.h"

SORE_Graphics::RenderBatch::RenderBatch(GraphicsArray* vertices, bool bindVBO)
    : commands(RENDER_CMD_NONE), geometry(vertices)
{
    if(bindVBO)
        commands |= RENDER_CMD_BIND_VBO;
}

void SORE_Graphics::RenderBatch::SetType(GLenum type)
{
    this->type = type;
}

void SORE_Graphics::RenderBatch::SetNumIndices(unsigned int numIndices)
{
    numberIndices = numIndices;
}

void SORE_Graphics::RenderBatch::SetIndexOffset(unsigned int offset)
{
    indexOffset = offset;
}

void SORE_Graphics::RenderBatch::AddChangeCameraCommand(camera_info cam)
{
    camera = cam;
    commands |= RENDER_CMD_CHANGE_CAMERA;
}

void SORE_Graphics::RenderBatch::AddChangeBlendModeCommand(blend_mode mode)
{
    blend = mode;
    commands |= RENDER_CMD_CHANGE_BLEND_MODE;
}

void SORE_Graphics::RenderBatch::AddBindShaderCommand(GLSLShaderPtr shader)
{
    this->shader = shader;
    commands |= RENDER_CMD_BIND_SHADER;
}

void SORE_Graphics::RenderBatch::AddBindTextureCommand(
    GLSLShaderPtr shader, TextureState textures)
{
    this->shader = shader;
    this->textures = textures;
    commands |= RENDER_CMD_BIND_TEXTURE;
}

void SORE_Graphics::RenderBatch::AddChangeUniformsCommand(
    GLSLShaderPtr shader, UniformState uniforms)
{
    this->shader = shader;
    this->uniforms = uniforms;
    commands |= RENDER_CMD_CHANGE_UNIFORMS;
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


unsigned int SORE_Graphics::RenderBatch::Render()
{
    if(commands & RENDER_CMD_BIND_VBO && geometry)
    {
        geometry->BeginDraw();
    }
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
    if(geometry)
    {
        unsigned int trisPerPoly;
        if(type == GL_POINTS)
            trisPerPoly = 1;
        else if(type == GL_LINES)
            trisPerPoly = 2;
        else if(type == GL_TRIANGLES)
            trisPerPoly = 3;
        else
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Unsupported primitive type");
        geometry->DrawElements(numberIndices, indexOffset, type);
        return numberIndices/trisPerPoly;
    }
    return 0;
}

