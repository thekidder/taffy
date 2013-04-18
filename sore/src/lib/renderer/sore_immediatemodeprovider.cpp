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

// std::copy warning in MSVC++
#define _SCL_SECURE_NO_WARNINGS

#include <sore_immediatemodeprovider.h>
#include <sore_glslshader.h>

#include <algorithm>
#include <set>
#include <iterator>
#include <sstream>

SORE_Graphics::ImmediateModeProvider::ImmediateModeProvider()
    : current_transform(new SORE_Math::Matrix4<float>()),
    current_primitive_type(GL_TRIANGLES), halfWidth(512.0f)
{
}

void SORE_Graphics::ImmediateModeProvider::SetTexture(const std::string& samplerName, const SORE_Resource::Texture2DPtr texture)
{
    TextureState::TextureObjectComparator comp;
    if(current_material->Textures().Contains(samplerName))
    {
        // if a texture with the same name is already bound and is different than the one we're trying to bind
        const TextureState::TextureObject& textureObject = current_material->Textures().Get(samplerName);
        if(comp(texture, textureObject) || comp(textureObject, texture))
        {
            // we need to create a new renderable from the current state and then set the texture
            CreateRenderableFromData();
        }
    }

    current_material->SetTexture(samplerName, texture);
}

void SORE_Graphics::ImmediateModeProvider::SetMaterial(SORE_Resource::MaterialPtr material)
{
    if(material == current_material)
        return;
    CreateRenderableFromData();
    current_material = material;
}

void SORE_Graphics::ImmediateModeProvider::SetColor(SORE_Graphics::Color color)
{
    current_color = color;
}

void SORE_Graphics::ImmediateModeProvider::SetTransform(SORE_Graphics::MatrixPtr transform)
{
    if(current_transform && transform && transform == current_transform)
        return;
    if(!current_transform && !current_transform)
        return;
    CreateRenderableFromData();
    current_transform = transform;
}

void SORE_Graphics::ImmediateModeProvider::SetPrimitiveType(GLenum type)
{
    if(type == current_primitive_type)
        return;
    CreateRenderableFromData();
    current_primitive_type = type;
}

void SORE_Graphics::ImmediateModeProvider::SetKeywords(const std::string& keywords)
{
    if(current_keywords == keywords)
        return;
    CreateRenderableFromData();
    current_keywords = keywords;
}

void SORE_Graphics::ImmediateModeProvider::Start()
{
    buffer_manager.Clear();
    renderables.clear();
    vertices.clear();
    indices.clear();
}

void SORE_Graphics::ImmediateModeProvider::DrawQuad(
    float x1, float y1, float z1,
    float x2, float y2, float z2,
    float x3, float y3, float z3,
    float x4, float y4, float z4,
    SORE_Math::Rect<float> texCoords)
{
    unsigned short current_index = SetupDraw(GL_TRIANGLES);

    SetTexCoords(texCoords.topLeft.GetValue()[0], texCoords.topLeft.GetValue()[1]);
    AddVertex(x1, y1, z1);

    SetTexCoords(texCoords.topLeft.GetValue()[0], texCoords.bottomRight.GetValue()[1]);
    AddVertex(x2, y2, z2);

    SetTexCoords(texCoords.bottomRight.GetValue()[0], texCoords.topLeft.GetValue()[1]);    
    AddVertex(x3, y3, z3);

    SetTexCoords(texCoords.bottomRight.GetValue()[0], texCoords.bottomRight.GetValue()[1]);
    AddVertex(x4, y4, z4);

    // add two triangles
    indices.push_back(current_index);
    indices.push_back(current_index + 1);
    indices.push_back(current_index + 2);
    indices.push_back(current_index + 2);
    indices.push_back(current_index + 1);
    indices.push_back(current_index + 3);
}

void SORE_Graphics::ImmediateModeProvider::DrawLine(
    float x1, float y1, float z1,
    float x2, float y2, float z2)
{
    unsigned short current_index = SetupDraw(GL_LINES);

    SetTexCoords(0.0f, 0.0f);
    AddVertex(x1, y1, z1);

    SetTexCoords(0.0f, 1.0f);
    AddVertex(x2, y2, z2);

    indices.push_back(current_index);
    indices.push_back(current_index + 1);
}

void SORE_Graphics::ImmediateModeProvider::DrawPoint(
    float x1, float y1, float z1, float size)
{
    unsigned short current_index = SetupDraw(GL_POINTS);

    SetUniform("halfWidth", halfWidth);
    AddVertex(x1, y1, z1);
    vertices.back().normx = size;
    indices.push_back(current_index);
}

void SORE_Graphics::ImmediateModeProvider::DrawString(float x, float y, float z, SORE_Resource::FontPtr face, unsigned int height, const std::string& string)
{
    SetupDraw(GL_TRIANGLES);

    float advance = 0.0f;
    for(std::string::const_iterator it = string.begin(); it != string.end(); ++it)
    {
        const SORE_Resource::CharInfo& char_info = face->GetCharacter(height, *it);

        if(char_info.texture)
        {
            vertex temp[4];
            const vertex* src = char_info.vertices;
            std::copy(src, src + 4, temp);

            for(int i = 0; i < 4; ++i)
            {
                temp[i].r = current_color.GetComponent(RED);
                temp[i].g = current_color.GetComponent(GREEN);
                temp[i].b = current_color.GetComponent(BLUE);
                temp[i].a = current_color.GetComponent(ALPHA);

                temp[i].x += x + advance;
                temp[i].y += y;
                temp[i].z = z;
            }

            SetTexture("texture", char_info.texture);
            unsigned short current_index = vertices.size();
            AddVertex(temp[0]);
            AddVertex(temp[1]);
            AddVertex(temp[2]);
            AddVertex(temp[3]);

            // add two triangles
            indices.push_back(current_index);
            indices.push_back(current_index + 1);
            indices.push_back(current_index + 2);
            indices.push_back(current_index + 2);
            indices.push_back(current_index + 1);
            indices.push_back(current_index + 3);
        }
        advance += char_info.advance;
    }
}

void SORE_Graphics::ImmediateModeProvider::MakeUpToDate()
{
    CreateRenderableFromData();
    buffer_manager.MakeUpToDate();
}

std::vector<SORE_Graphics::Renderable>::iterator SORE_Graphics::ImmediateModeProvider::GeometryBegin()
{
    return renderables.begin();
}

std::vector<SORE_Graphics::Renderable>::iterator SORE_Graphics::ImmediateModeProvider::GeometryEnd()
{
    return renderables.end();
}

bool SORE_Graphics::ImmediateModeProvider::OnResize(const SORE_Kernel::Event& e)
{
    if(e.type == SORE_Kernel::RESIZE)
    {
        halfWidth = e.resize.w / 2.0f;
        return true;
    }
    return false;
}

void SORE_Graphics::ImmediateModeProvider::SetTexCoords(float i, float j)
{
    current_texcoords.first = i;
    current_texcoords.second = j;
}

void SORE_Graphics::ImmediateModeProvider::AddVertex(float x, float y, float z)
{
    vertex temp;
    temp.r = current_color.GetComponent(RED);
    temp.g = current_color.GetComponent(GREEN);
    temp.b = current_color.GetComponent(BLUE);
    temp.a = current_color.GetComponent(ALPHA);

    temp.tex0i = current_texcoords.first;
    temp.tex0j = current_texcoords.second;

    temp.x = x;
    temp.y = y;
    temp.z = z;

    vertices.push_back(temp);
}

void SORE_Graphics::ImmediateModeProvider::AddVertex(const vertex& v)
{
    vertices.push_back(v);
}

unsigned short SORE_Graphics::ImmediateModeProvider::SetupDraw(GLenum primitive_type)
{
    if(current_primitive_type != primitive_type)
        SetPrimitiveType(primitive_type);

    if(indices.size() + 6 >= 65535)
        CreateRenderableFromData();

    if(vertices.size() + 6 >= 65535)
        CreateRenderableFromData();

    return vertices.size();
}

void SORE_Graphics::ImmediateModeProvider::CreateRenderableFromData()
{
    if(!indices.size())
        return;

    GeometryChunkPtr geometry(new SORE_Graphics::GeometryChunk(vertices.size(), indices.size(), current_primitive_type));

    std::copy(vertices.begin(), vertices.end(), geometry->GetVertices());
    std::copy(indices.begin(),  indices.end(),  geometry->GetIndices());

    Renderable r(geometry, current_transform, current_material);

    std::set<std::string> keyword_list;
    std::istringstream iss(current_keywords);
    std::copy(
        std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>(),
        std::insert_iterator<std::set<std::string> >(keyword_list,keyword_list.begin()));
    for(std::set<std::string>::iterator it = keyword_list.begin(); it != keyword_list.end(); ++it)
        r.AddKeyword(*it);

    buffer_manager.GeometryAdded(r, SORE_Graphics::STREAM);

    renderables.push_back(r);

    vertices.clear();
    indices.clear();
}