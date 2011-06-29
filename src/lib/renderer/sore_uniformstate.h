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

#ifndef SORE_UNIFORMSTATE_H
#define SORE_UNIFORMSTATE_H

#include <sore_assettypes.h>
#include <sore_dll.h>
#include <sore_vector2.h>
#include <sore_vector3.h>
#include <sore_vector4.h>
#include <sore_matrix4x4.h>
#include <sore_glslshader.h>

#include <map>
#include <string>

namespace SORE_Graphics
{
    enum uniform_type {INT, FLOAT, VEC2, VEC3, VEC4, MAT4};

    struct uniform_var
    {
        uniform_type type;

        int i;
        float f;
        SORE_Math::Vector2<float> v2;
        SORE_Math::Vector3<float> v3;
        SORE_Math::Vector4<float> v4;
        SORE_Math::Matrix4<float> m4;
    };

    bool operator==(const uniform_var& one, const uniform_var& two);
    bool operator!=(const uniform_var& one, const uniform_var& two);

    class SORE_EXPORT UniformState
    {
    public:
        void Bind(SORE_Resource::GLSLShaderPtr s) const;

        void SetVariable(std::string name, int i);
        void SetVariable(std::string name, float f);
        void SetVariable(std::string name, SORE_Math::Vector2<float> v);
        void SetVariable(std::string name, SORE_Math::Vector3<float> v);
        void SetVariable(std::string name, SORE_Math::Vector4<float> v);
        void SetVariable(std::string name, SORE_Math::Matrix4<float> m);

        void SetVariable(std::string name, uniform_var v);

        bool Empty() const; //returns true if there are no uniforms

        //returns all uniforms in this not in o
        UniformState GetDiff(const UniformState& o) const;
    private:
        std::map<std::string, uniform_var> uniforms;
    };

    typedef boost::shared_ptr<UniformState> UniformStatePtr;
}
#endif
