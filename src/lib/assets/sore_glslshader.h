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
#include <sore_dll.h>

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

        GLSLShader();
        GLSLShader(const char* vertex, const char* fragment);
        ~GLSLShader();

        int  AddVertexString(const char* vertex);
        int  AddFragmentString(const char* fragment);

        void Link();
        void Bind() const;
        bool Loaded() const;
        unsigned int GetHandle() const;

        //Uniform operators
        void SetUniform1i(std::string name, GLuint i0);
        void SetUniform1f(std::string name, GLfloat f0);
        void SetUniform2f(std::string name, GLfloat v0, GLfloat v1);
        void SetUniform3f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2);
        void SetUniform4f(
            std::string name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

        void SetUniform1fv(
            std::string name, unsigned int count, const GLfloat* values);
        void SetUniformMatrix4fv(
            std::string name, const GLfloat * values);

        GLint GetAttributeLocation(std::string name);

        //for sorting
        bool operator<(const GLSLShader& o) const;
        bool operator==(const GLSLShader& o) const;

        static std::string ProcessFilename(const std::string& filename);
    private:
        void Init();
        void Unload();
        int  AddShader(GLuint type, const char* src);
        GLint GetUniformLocation(std::string name);

        std::vector<GLuint> vertexShaders, fragmentShaders;
        GLuint program;
        bool linked;
        std::map<std::string,GLint> uniforms;
        std::map<std::string,GLint> attributes;
        static bool initCalled;
        static bool supported;
    };

    bool operator!=(const GLSLShader& one, const GLSLShader& two);
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
