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

#ifndef SORE_UNIFORMSTATE_H
#define SORE_UNIFORMSTATE_H

#include <map>
#include <string>

#include "math/sore_vector2.h"
#include "math/sore_vector3.h"
#include "math/sore_vector4.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{
    enum uniform_type {INT, FLOAT, VEC2, VEC3, VEC4};

    struct uniform_var
    {
        uniform_type type;

        int i;
        float f;
        SORE_Math::Vector2<float> v2;
        SORE_Math::Vector3<float> v3;
        SORE_Math::Vector4<float> v4;
    };

    bool operator==(const uniform_var& one, const uniform_var& two);
    bool operator!=(const uniform_var& one, const uniform_var& two);

    class UniformState
    {
    public:
        void Bind(GLSLShaderPtr s) const;

        void SetVariable(std::string name, int i);
        void SetVariable(std::string name, float f);
        void SetVariable(std::string name, SORE_Math::Vector2<float> v);
        void SetVariable(std::string name, SORE_Math::Vector3<float> v);
        void SetVariable(std::string name, SORE_Math::Vector4<float> v);

        void SetVariable(std::string name, uniform_var v);

        bool Empty() const; //returns true if there are no uniforms

        //returns all uniforms in this not in o
        UniformState GetDiff(const UniformState& o);
    private:
        std::map<std::string, uniform_var> uniforms;
    };
}
#endif
