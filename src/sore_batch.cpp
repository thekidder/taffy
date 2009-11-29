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

void SORE_Graphics::RenderBatch::AddChangeProjectionCommand(ProjectionInfo proj)
{
    projection = proj;
    commands |= RENDER_CMD_CHANGE_PROJECTION;
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

void SORE_Graphics::RenderBatch::SetupProjection(
    ProjectionInfo& pi, const ScreenInfo& si)
{
    switch(pi.type)
    {
    case NONE:
        break;
    case ORTHO2D:
        if(pi.useScreenCoords)
        {
            pi.top = 0;
            pi.left = 0;
            pi.bottom = static_cast<GLfloat>(si.height);
            pi.right = static_cast<GLfloat>(si.width);
            pi.ratio = si.ratio;
        }
        else if(pi.useScreenRatio)
        {
            pi.bottom = pi.left / si.ratio;
            pi.top = pi.right / si.ratio;
            pi.ratio = si.ratio;
        }
        else
        {
            pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
        }
        break;
    case ORTHO:
        //TODO: finish ortho projection
        break;
    case PERSPECTIVE:
        if(pi.useScreenRatio)
        {
            pi.ratio = si.ratio;
        }
        else
        {
            pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
        }
        break;
    default:
        break;
    }
}

void SORE_Graphics::RenderBatch::ChangeProjectionMatrix(
    ProjectionInfo& projection, const ScreenInfo& si)
{
    SetupProjection(projection, si);
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
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
            if(projection.useScreenRatio)
                projection.ratio = static_cast<float>(si.width) /
                    static_cast<float>(si.height);
            gluPerspective(projection.fov, projection.ratio,
                           projection.znear, projection.zfar );
            break;
        }
    default:
        break;
    }
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
}


unsigned int SORE_Graphics::RenderBatch::Render(const ScreenInfo& si)
{
    if(commands & RENDER_CMD_BIND_VBO && geometry)
    {
        geometry->BeginDraw();
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Binding VBO");
    }
    if(commands & RENDER_CMD_CHANGE_PROJECTION)
    {
        ChangeProjectionMatrix(projection, si);
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Changing Projection");
    }
    if(commands & RENDER_CMD_CHANGE_BLEND_MODE)
    {
        unsigned int blendSFactor, blendDFactor;
        switch(blend)
        {
        case OPAQUE:
            glEnable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            blendSFactor = GL_ZERO;
            blendDFactor = GL_ONE;
        case ADDITIVE:
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            blendSFactor = GL_SRC_ALPHA;
            blendDFactor = GL_DST_ALPHA;
            break;
        case SUBTRACTIVE:
        default: //treat unknown type as subtractive by default
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            blendSFactor = GL_SRC_ALPHA;
            blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
            break;
        }
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Changing blend func");
        glBlendFunc(blendSFactor, blendDFactor);
    }
    if(commands & RENDER_CMD_BIND_SHADER)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Binding shader");
        shader->Bind();
    }
    if(commands & RENDER_CMD_BIND_TEXTURE)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, "Binding texture " + texture->GetFilename());
        texture->Bind(shader, "texture", 0);
    }
    if(geometry)
    {
        ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("drawing %d triangles from %d") % numberTriangles % triangleOffset);
        geometry->DrawElements(numberTriangles, triangleOffset);
        return numberTriangles;
    }
    return 0;
}

