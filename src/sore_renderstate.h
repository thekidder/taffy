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

#ifndef SORE_RENDERERSTATE_H
#define SORE_RENDERERSTATE_H

#include "sore_renderable.h"
#include "sore_screeninfo.h"

namespace SORE_Graphics
{
    class RenderState
    {
    public:
        //create an empty RenderState
        RenderState();
        //set up state for rendering a single renderable
        RenderState(const Renderable& r, camera_info cam);

        //create state based upon the differences between a previous state 
        //and a renderable
        RenderState Difference(const Renderable& r, camera_info cam) const;
        bool Empty() const;
        void Apply() const;
    private:
        const static unsigned int RENDER_CMD_NONE              = 0;
        const static unsigned int RENDER_CMD_CHANGE_CAMERA     = 1;
        const static unsigned int RENDER_CMD_CHANGE_BLEND_MODE = 2;
        const static unsigned int RENDER_CMD_BIND_SHADER       = 4;
        const static unsigned int RENDER_CMD_BIND_TEXTURE      = 8;
        const static unsigned int RENDER_CMD_CHANGE_UNIFORMS   = 16;

        void ChangeCameraMatrix(const SORE_Math::Matrix4<float>& camera);
        void ChangeProjectionMatrix(const ProjectionInfo& proj);

        unsigned int commands;

        UniformState uniforms;
        TextureState textures;
        GLSLShaderPtr shader;

        camera_info camera;
        blend_mode blend;

        GLenum primitiveType;
    };
}

#endif