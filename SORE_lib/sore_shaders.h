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
//

#ifndef  __SORE_SHADERS_H__
#define  __SORE_SHADERS_H__

#include "sore_fileio.h"
#include "sore_allgl.h"
#include "sore_glext.h"
#include "sore_logger.h"

#include <vector>
#include <string>

namespace SORE_Graphics
{
	void InitShaders();
	void UnbindShaders();
	
	class GLSLShader
	{
		public:
			GLSLShader(const char* vertex, const char* fragment);
			~GLSLShader();
			
			int  AddVertexFile(const char* vertex);
			int  AddVertexString(const char* vertex);
			
			int  AddFragmentFile(const char* fragment);
			int  AddFragmentString(const char* fragment);
			
			void Link();
			void Bind();
			
			bool Ready();
			
			//Uniform operators
			void SetUniform4f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		protected:
			int  Init();
			int  AddShader(GLuint type, const char* src);
			std::vector<GLuint> vertexShaders, fragmentShaders;
			GLuint program;
			bool ok,linked;
			std::map<std::string,GLint> uniforms;
	};
}

#endif /*__SORE_SHADERS_H__*/
