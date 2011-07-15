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

#include <sore_uniformstate.h>

void SORE_Graphics::UniformState::Bind(SORE_Resource::GLSLShaderPtr s) const
{
    std::map<std::string, uniform_var>::const_iterator it;
    for(it = uniforms.begin(); it!= uniforms.end(); ++it)
    {
        switch(it->second.type)
        {
        case INT:
            s->SetUniform1i(it->first, it->second.i);
            break;
        case FLOAT:
            s->SetUniform1f(it->first, it->second.f);
            break;
        case VEC2:
            s->SetUniform2f(it->first,
                            it->second.v2.GetValue()[0],
                            it->second.v2.GetValue()[1]);
            break;
        case VEC3:
            s->SetUniform3f(it->first,
                            it->second.v3.GetValue()[0],
                            it->second.v3.GetValue()[1],
                            it->second.v3.GetValue()[2]);
            break;
        case VEC4:
            s->SetUniform4f(it->first,
                            it->second.v4.GetValue()[0],
                            it->second.v4.GetValue()[1],
                            it->second.v4.GetValue()[2],
                            it->second.v4.GetValue()[3]);
            break;
        case MAT4:
            s->SetUniformMatrix4fv(it->first, it->second.m4.GetData());
            break;
        }
    }
}

void SORE_Graphics::UniformState::SetVariable(std::string name, int i)
{
    uniforms[name].type = INT;
    uniforms[name].i = i;
}

void SORE_Graphics::UniformState::SetVariable(std::string name, float f)
{
    uniforms[name].type = FLOAT;
    uniforms[name].f = f;
}

void SORE_Graphics::UniformState::SetVariable(
    std::string name, SORE_Math::Vector2<float> v)
{
    uniforms[name].type = VEC2;
    uniforms[name].v2 = v;
}

void SORE_Graphics::UniformState::SetVariable(
    std::string name, SORE_Math::Vector3<float> v)
{
    uniforms[name].type = VEC3;
    uniforms[name].v3 = v;
}

void SORE_Graphics::UniformState::SetVariable(
    std::string name, SORE_Math::Vector4<float> v)
{
    uniforms[name].type = VEC4;
    uniforms[name].v4 = v;
}

void SORE_Graphics::UniformState::SetVariable(
    std::string name, SORE_Math::Matrix4<float> m)
{
    uniforms[name].type = MAT4;
    uniforms[name].m4 = m;
}

void SORE_Graphics::UniformState::SetVariable(std::string name, uniform_var v)
{
    switch(v.type)
    {
    case INT:
        SetVariable(name, v.i);
        break;
    case FLOAT:
        SetVariable(name, v.f);
        break;
    case VEC2:
        SetVariable(name, v.v2);
        break;
    case VEC3:
        SetVariable(name, v.v3);
        break;
    case VEC4:
        SetVariable(name, v.v4);
        break;
    case MAT4:
        SetVariable(name, v.m4);
        break;
    }
}

bool SORE_Graphics::UniformState::Empty() const
{
    return uniforms.empty();
}

SORE_Graphics::UniformState SORE_Graphics::UniformState::GetDiff(
    const UniformState& o) const
{
    UniformState r;
    std::map<std::string, uniform_var>::const_iterator it;
    for(it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        if(o.uniforms.find(it->first) == o.uniforms.end())
        {
            r.SetVariable(it->first, it->second);
            continue;
        }
        const uniform_var& u = o.uniforms.find(it->first)->second;
        const uniform_var& u2 = it->second;
        if(u != u2)
            r.SetVariable(it->first, it->second);
    }
    return r;
}

bool SORE_Graphics::operator==(
    const SORE_Graphics::uniform_var& one, const SORE_Graphics::uniform_var& two)
{
    switch(one.type)
    {
    case SORE_Graphics::INT:
        return (two.type == SORE_Graphics::INT && one.i == two.i);
    case SORE_Graphics::FLOAT:
        return (two.type == SORE_Graphics::FLOAT && one.f == two.f);
    case SORE_Graphics::VEC2:
        return (two.type == SORE_Graphics::VEC2 && one.v2 == two.v2);
    case SORE_Graphics::VEC3:
        return (two.type == SORE_Graphics::VEC3 && one.v3 == two.v3);
    case SORE_Graphics::VEC4:
        return (two.type == SORE_Graphics::VEC4 && one.v4 == two.v4);
    case SORE_Graphics::MAT4:
        return (two.type == SORE_Graphics::MAT4 && one.m4 == two.m4);
    default:
        return false;
    }
}

bool SORE_Graphics::operator!=(
    const SORE_Graphics::uniform_var& one, const SORE_Graphics::uniform_var& two)
{
    return !(one == two);
}
