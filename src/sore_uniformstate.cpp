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

#include "sore_uniformstate.h"

void SORE_Graphics::UniformState::Bind(GLSLShaderPtr s) const
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
            s->SetUniformfv(it->first, 2, it->second.v2.GetValue());
            break;
        case VEC3:
            s->SetUniformfv(it->first, 3, it->second.v3.GetValue());
            break;
        case VEC4:
            s->SetUniformfv(it->first, 4, it->second.v4.GetValue());
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
    uniforms[name].type = VEC2;
    uniforms[name].v3 = v;
}

void SORE_Graphics::UniformState::SetVariable(
    std::string name, SORE_Math::Vector4<float> v)
{
    uniforms[name].type = VEC4;
    uniforms[name].v4 = v;
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
    }
}

bool SORE_Graphics::UniformState::Empty() const
{
    return uniforms.empty();
}

SORE_Graphics::UniformState SORE_Graphics::UniformState::GetDiff(
    const UniformState& o)
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
    default:
        return false;
    }
}

bool SORE_Graphics::operator!=(
    const SORE_Graphics::uniform_var& one, const SORE_Graphics::uniform_var& two)
{
    return !(one == two);
}
