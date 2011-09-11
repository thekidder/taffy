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

#ifndef SORE_MATERIAL_H
#define SORE_MATERIAL_H

#include <sore_blend_state.h>
#include <sore_glslshader.h>
#include <sore_texture2d.h>
#include <sore_texturestate.h>
#include <sore_uniformstate.h>

#include <string>

namespace SORE_Graphics
{
    class PipelineRenderer;
    class Renderable;
    class RenderState;
}

namespace SORE_Resource
{
    class Material
    {
    public:
        Material(
            const SORE_Graphics::Blend_state& blendState_,
            const GLSLShaderPtr shader_);

        Material GetDiff(const Material& other) const;

        template<typename T>
        void SetUniform(const std::string& name, const T& value)
        {
            uniforms.SetVariable(name, value);
        }

        // need to refactor out this interface
        bool Loaded() const { return true; }

        void SetTexture(const std::string& name, const Texture2DPtr texture);
        const SORE_Graphics::TextureState& Textures() const { return textures; }

        const GLSLShaderPtr GetShader() const { return shader; }

        bool operator==(const Material& other) const;
        bool operator<(const Material& other) const;
    private:
        // let the rendering pipeline access our internals
        friend class SORE_Graphics::PipelineRenderer;
        friend class SORE_Graphics::Renderable;
        friend class SORE_Graphics::RenderState;

        SORE_Graphics::Blend_state blendState;
        GLSLShaderPtr shader;

        SORE_Graphics::TextureState textures;
        SORE_Graphics::UniformState uniforms;
    };

    inline bool operator!=(const Material& one, const Material& two) { return !(one == two); }
}

#endif