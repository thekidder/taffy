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

#ifndef SORE_GLSLSHADER_H
#define SORE_GLSLSHADER_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <sore_allgl.h>
#include <sore_assettypes.h>
#include <sore_dll.h>
#include <sore_matrix4x4.h>

#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>
#include <map>

namespace SORE_Resource
{
    class SORE_EXPORT GLSLShader
    {
    public:
        static int  InitShaders();
        static void UnbindShaders();
        static bool ShadersSupported();

        struct glsl_variable_info
        {
            glsl_variable_info(GLuint index_, GLenum type_, GLint size_)
                : index(index_), type(type_), size(size_)
            {}

            const GLint index;
            const GLenum type;
            const GLint size;
        };

        typedef std::map<std::string, glsl_variable_info> Uniform_map_t;
        typedef Uniform_map_t Attribute_map_t;

        GLSLShader();
        GLSLShader(const char* vertex, const char* fragment);
        ~GLSLShader();

        int  AddVertexString(const char* vertex);
        int  AddFragmentString(const char* fragment);

        void Link();
        void Bind() const;
        bool Loaded() const;
        unsigned int GetHandle() const;

        // Uniform operators
        void SetUniform(const std::string& name, GLint i0);
        void SetUniform(const std::string& name, GLfloat f0);
        void SetUniform(const std::string& name, const SORE_Math::Vector2<float>& v);
        void SetUniform(const std::string& name, const SORE_Math::Vector3<float>& v);
        void SetUniform(const std::string& name, const SORE_Math::Vector4<float>& v);

        void SetUniform(
            const std::string& name, const SORE_Math::Matrix4<float>& m);
        void SetUniformTexture(
            const std::string& name, GLuint textureSlot);

        // get table of active uniforms/attributes
        const Uniform_map_t& ActiveUniforms() const;
        const Attribute_map_t& ActiveAttributes() const;

        // for sorting
        bool operator<(const GLSLShader& o) const;
        bool operator==(const GLSLShader& o) const;
    private:
        const static glsl_variable_info none;

        void Init();
        void Unload();
        int  AddShader(GLuint type, const char* src);

        // populate tables of uniform/attribute locations at link time
        void PopulateUniformLocations();
        void PopulateAttributeLocations();

        // check that name is of type, and return the uniform index, doing a lot of error checking
        int GetCheckedIndex(const std::string& name, GLenum type);

        const glsl_variable_info& GetUniform(const std::string& name);
        const glsl_variable_info& GetAttribute(const std::string& name);

        void PrintInfo();

        std::vector<GLuint> vertexShaders, fragmentShaders;
        GLuint program;
        bool linked;
        Uniform_map_t uniforms;
        Attribute_map_t attributes;
        static bool initCalled;
        static bool supported;
    };

    bool operator!=(const GLSLShader& one, const GLSLShader& two);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
