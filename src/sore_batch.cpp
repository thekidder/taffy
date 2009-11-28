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

void SORE_Graphics::RenderBatch::SetNumTriangles(unsigned int numTris)
{
    numberTriangles = numTris;
}

void SORE_Graphics::RenderBatch::SetTriangleOffset(unsigned int offset)
{
    triangleOffset = offset;
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
    GLSLShaderPtr shader, Texture2DPtr texture)
{
    this->shader = shader;
    this->texture = texture;
    commands |= RENDER_CMD_BIND_TEXTURE;
}

void SORE_Graphics::RenderBatch::Render() const
{
    if(commands & RENDER_CMD_BIND_VBO && geometry)
    {
        geometry->BeginDraw();
    }
    if(commands & RENDER_CMD_CHANGE_BLEND_MODE)
    {
        unsigned int blendSFactor, blendDFactor;
        switch(blend)
        {
        case OPAQUE:
            glDisable(GL_BLEND);
            blendSFactor = GL_ZERO;
            blendDFactor = GL_ONE;
        case ADDITIVE:
            glEnable(GL_BLEND);
            blendSFactor = GL_SRC_ALPHA;
            blendDFactor = GL_DST_ALPHA;
            break;
        case SUBTRACTIVE:
        default: //treat unknown type as subtractive by default
            glEnable(GL_BLEND);
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
        texture->Bind(shader, "texture", 0);
    }
    if(geometry)
    {
        geometry->DrawElements(numberTriangles, triangleOffset);
    }
}

