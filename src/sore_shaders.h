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

#ifndef  SORE_SHADERS_H
#define  SORE_SHADERS_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>
#include <string>

#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_logger.h"
#include "sore_resource.h"

namespace SORE_Graphics
{
    class SORE_EXPORT GLSLShader : public SORE_Resource::Resource
    {
    public:
        static int  InitShaders();
        static void UnbindShaders();
        static bool ShadersSupported();

        GLSLShader(const char* vertex, const char* fragment,
                   SORE_FileIO::PackageCache* pc = NULL);
        //loads shader as a INI file specifying vertex/shader files
        GLSLShader(std::string shaderFile, SORE_FileIO::PackageCache* pc = NULL);
        ~GLSLShader();

        int  AddVertexFile(const char* vertex);
        int  AddVertexString(const char* vertex);

        int  AddFragmentFile(const char* fragment);
        int  AddFragmentString(const char* fragment);

        void Link();
        void Bind() const;
        bool Ready() const;

        const char* Type() const {return "GLSL shader";}
        bool GLContextDependent() const {return true;}

        //Uniform operators
        void SetUniform1i(std::string name, GLuint i0);
        void SetUniform1f(std::string name, GLfloat f0);
        void SetUniform2f(std::string name, GLfloat v0, GLfloat v1);
        void SetUniform3f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2);
        void SetUniform4f(std::string name,
                          GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);

        void SetUniform1fv(std::string name, unsigned int count, GLfloat* values);

        GLint GetAttributeLocation(std::string name);

        //for sorting
        bool operator<(const GLSLShader& o) const;
        bool operator==(const GLSLShader& o) const;
    private:
        char* LoadFile(const char* filename);
        void Load(); //load from *.shad file
        int  Init();
        int  AddShader(GLuint type, const char* src);
        GLint GetUniformLocation(std::string name);
        virtual std::string ProcessFilename(std::string file) {return file;}
        std::vector<GLuint> vertexShaders, fragmentShaders;
        GLuint program;
        bool ok,linked;
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
