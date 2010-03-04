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

#ifndef SORE_BATCH_H
#define SORE_BATCH_H

#include "sore_graphicsarray.h"
#include "sore_renderable.h"
#include "sore_texture.h"
#include "sore_screeninfo.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{


    class RenderBatch
    {
    public:
        //sets command to either NONE or BIND_VBO
        RenderBatch(GraphicsArray* vertices, bool bindVBO = false);

        void SetType(GLenum type);
        void SetNumIndices(unsigned int numIndices);
        void SetIndexOffset(unsigned int offset);

        void AddChangeCameraCommand(camera_info cam);
        void AddChangeBlendModeCommand(blend_mode mode);
        void AddBindShaderCommand(GLSLShaderPtr shader);
        void AddBindTextureCommand(GLSLShaderPtr shader, TextureState textures);
        void AddChangeUniformsCommand(GLSLShaderPtr shader, UniformState uniforms);

        void SetLayer(geometry_layer layer);
        geometry_layer GetLayer() const;

        //returns number of polygons rendered
        unsigned int Render();
    private:
        GraphicsArray* geometry;
        unsigned int numberIndices;
        unsigned int indexOffset;
        GLenum type;

        blend_mode blend;
        geometry_layer layer;
        camera_info camera;
        GLSLShaderPtr shader;
        TextureState textures;

        UniformState uniforms;
    };
}

#endif
