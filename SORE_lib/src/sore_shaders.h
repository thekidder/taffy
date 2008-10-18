//
// C++ Interface: sore_shaders
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#ifndef  __SORE_SHADERS_H__
#define  __SORE_SHADERS_H__

#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_logger.h"
#include "sore_resource.h"

#include <vector>
#include <string>

namespace SORE_Graphics
{
  class GLSLShader : public SORE_Resource::Resource
    {
    public:
      static int  InitShaders();
      static void UnbindShaders();
      static bool ShadersSupported();
			
      GLSLShader(const char* vertex, const char* fragment);
      GLSLShader(std::string shaderFile, std::string additionalInfo); //loads shader as a INI file specifying vertex/shader files
      ~GLSLShader();
			
      int  AddVertexFile(const char* vertex);
      int  AddVertexString(const char* vertex);
			
      int  AddFragmentFile(const char* fragment);
      int  AddFragmentString(const char* fragment);
			
      void Link();
      void Bind();
			
      bool Ready();
			
      const char* Type() const {return "GLSL shader";}
			
      //Uniform operators
      void SetUniform1i(std::string name, GLuint i0);
      void SetUniform1f(std::string name, GLfloat f0);
      void SetUniform2f(std::string name, GLfloat v0, GLfloat v1);
      void SetUniform3f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2);
      void SetUniform4f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
			
      void SetUniform1fv(std::string name, unsigned int count, GLfloat* values);
			
      GLint GetAttributeLocation(std::string name);
			
    protected:
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
}

#endif /*__SORE_SHADERS_H__*/
