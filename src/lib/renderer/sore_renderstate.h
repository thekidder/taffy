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

#ifndef SORE_RENDERERSTATE_H
#define SORE_RENDERERSTATE_H

#include <sore_camera.h>
#include <sore_fbo.h>
#include <sore_renderable.h>

namespace SORE_Graphics
{
    class RenderState
    {
    public:
        // create an empty RenderState
        RenderState();
        // set up state for rendering a single renderable
        RenderState(const Renderable& r, camera_info cam);

        // create state based upon the differences between a previous state
        // and a renderable
        RenderState Difference(const RenderState& old) const;
        
        // returns true if there is no state to apply
        bool Empty() const;
        // apply the current state
        void Apply() const;

        // change this renderstate:
        // set the renderbuffer to render to
        // colorbuffer is the index of the color buffer to select - if this is
        // -1, no color buffer
        void SetRenderbuffer(FBO* const renderbuffer_, int colorbufferIndex_ = -1);
    private:
        enum Render_cmd_e
        {
            RENDER_CMD_NONE              = 0,
            RENDER_CMD_CHANGE_CAMERA     = 1,
            RENDER_CMD_CHANGE_BLEND_MODE = 2,
            RENDER_CMD_BIND_SHADER       = 4,
            RENDER_CMD_BIND_TEXTURE      = 8,
            RENDER_CMD_CHANGE_UNIFORMS   = 16,
            RENDER_CMD_CHANGE_PRIMITIVE  = 32,
            RENDER_CMD_CHANGE_FBO        = 64
        };

        void ChangeCameraMatrix(const SORE_Math::Matrix4<float>& camera) const;
        void ChangeProjectionMatrix(const ProjectionInfo& proj) const;

        unsigned int commands;

        UniformState uniforms;
        TextureState textures;
        SORE_Resource::GLSLShaderPtr shader;

        camera_info camera;
        blend_mode blend;

        GLenum primitiveType;

        FBO* renderbuffer;
        int colorbufferIndex;
    };
}

#endif
