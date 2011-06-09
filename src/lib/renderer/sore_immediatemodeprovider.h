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
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR        *
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

#ifndef SORE_IMMEDIATE_MODE_PROVIDER_H
#define SORE_IMMEDIATE_MODE_PROVIDER_H

#include <sore_font.h>
#include <sore_geometryprovider.h>
#include <sore_glslshader.h>
#include <sore_simplebuffermanager.h>
#include <sore_texture.h>

#include <boost/unordered_map.hpp>

namespace SORE_Graphics
{
    class ImmediateModeProvider : public SORE_Graphics::GeometryProvider
    {
    public:
        ImmediateModeProvider(SORE_Graphics::Texture2DPtr default_texture, SORE_Graphics::GLSLShaderPtr default_shader);

        void SetTexture(SORE_Graphics::Texture2DPtr texture);
        void SetShader(SORE_Graphics::GLSLShaderPtr shader);
        void SetColor(SORE_Graphics::Color color);
        void SetBlendMode(SORE_Graphics::blend_mode blend_mode);
        void SetPrimitiveType(GLenum type);
        void SetKeywords(const std::string& keywords_);

        void Start();

        void DrawQuad(
            float x1, float y1, float z1,
            float x2, float y2, float z2,
            float x3, float y3, float z3,
            float x4, float y4, float z4);
        void DrawLine(
            float x1, float y1, float z1,
            float x2, float y2, float z2);
        void DrawString(float x, float y, SORE_Font::Font& face, unsigned int height, const std::string& string);

        // geometry provider interface
        void MakeUpToDate();
        std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
        std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

        // buffer manager interface
        void Regenerate();

        SORE_Graphics::BufferManager* GetBufferManager() { return &buffer_manager; }
    private:
        void SetTexCoords(float i, float j);
        void AddVertex(float x, float y, float z);
        void AddVertex(const vertex& v);

        unsigned short SetupDraw(GLenum primitive_type); // do some bookkeeping and return the current vertex index

        // flush the current vertex/index cache to a new renderable and start a new cache
        void CreateRenderableFromData();

        SORE_Graphics::Texture2DPtr current_texture;
        SORE_Graphics::GLSLShaderPtr current_shader;
        SORE_Graphics::Color current_color;
        SORE_Graphics::blend_mode current_blend_mode;
        GLenum current_primitive_type;
        std::pair<float, float> current_texcoords;
        std::string keywords;

        std::vector<SORE_Graphics::vertex> vertices;
        std::vector<unsigned short> indices;

        SORE_Graphics::SimpleBufferManager buffer_manager;
        std::vector<SORE_Graphics::Renderable> renderables;
    };
}

#endif