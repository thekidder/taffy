//
// C++ Implementation: sore_shaders
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#include "sore_shaders.h"

namespace SORE_Graphics
{
	bool GLSLShader::initCalled = false;
	bool GLSLShader::supported  = false;
	
	int GLSLShader::InitShaders()
	{
		initCalled = true;
		char* version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
		if(version==NULL)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Card reports GLSL not supported");
			supported = false;
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("OpenGL Shading language version: %s") % version);
			supported = true;
		}
		if(!(GLEW_VERSION_2_0 || (GLEW_ARB_vertex_program && GLEW_ARB_fragment_program)))
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "No shader support - check OpenGL version and extensions string");
			supported = false;
		}
		else
		{
			supported = true;
		}
		if(supported)
			ENGINE_LOG(SORE_Logging::LVL_INFO, "System is ready for GLSL shaders");
		else
			return -1;
		return 0;
	}
	
	bool GLSLShader::ShadersSupported()
	{
		if(!initCalled) InitShaders();
		return supported;
	}
	
	void GLSLShader::UnbindShaders()
	{
		if(!ShadersSupported())
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return;
		}
		glUseProgramObjectARB(0);
	}
	
	GLSLShader::GLSLShader(const char* vertex, const char* fragment)
	{
		if(!initCalled)
			InitShaders();
		Init();
		AddVertexFile(vertex);
		AddFragmentFile(fragment);
		ok = true;
		linked = false;
	}
	
	GLSLShader::~GLSLShader()
	{
		if(!ShadersSupported() || program==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return;
		}
		std::vector<GLuint>::iterator it;
		for(it=vertexShaders.begin();it!=vertexShaders.end();it++)
		{
			glDetachObjectARB(program, *it);
			glDeleteObjectARB(*it);
		}
		for(it=fragmentShaders.begin();it!=fragmentShaders.end();it++)
		{
			glDetachObjectARB(program, *it);
			glDeleteObjectARB(*it);
		}
		glDeleteObjectARB(program);
	}
	
	bool GLSLShader::Ready()
	{
		return ok && linked && ShadersSupported() && program!=0;
	}
	
	void GLSLShader::Link()
	{
		if(!ShadersSupported() || program==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return;
		}
		glLinkProgramARB(program);
		int link;
		glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &link);
		if(link!=GL_TRUE)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to link shader program");
			int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;

			glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB ,&infologLength);
		
			if (infologLength > 0)
			{
				infoLog = new char[infologLength];
				glGetInfoLogARB(program, infologLength, &charsWritten, infoLog);
				ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Info log:\n%s") % infoLog);
				delete[] infoLog;
			}
		}
		else
		{
			ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Shader program linked OK");
			linked = true;
		}
	}
	
	int GLSLShader::Init()
	{
		if(!ShadersSupported())
		{
			program = 0;
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Error creating shader program (shaders are not supported on this system)");
			return 1;
		}
		program = glCreateProgramObjectARB();
		if(program == 0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Error creating shader program");
			ok = false;
			return 1; //error
		}
		return 0;
	}
	
	int GLSLShader::AddShader(GLuint type, const char* src)
	{
		if(!(type==GL_VERTEX_SHADER || type==GL_FRAGMENT_SHADER))
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Attempted to create shader of unknown type (type given: %u)") % type);
			return 1;
		}
		if(!ShadersSupported() || program==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return 1;
		}
		GLuint shader;
		shader = glCreateShaderObjectARB(type);
		std::string shaderType, source;
		if(type==GL_VERTEX_SHADER) shaderType = "vertex";
		else if(type==GL_FRAGMENT_SHADER) shaderType = "fragment";
		if(shader==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Error creating %s shader object.") % shaderType.c_str());
			return 1;
		}
		glShaderSourceARB(shader, 1, &src, NULL);
		glCompileShaderARB(shader);
		glAttachObjectARB(program, shader);
		
		//now let's check if everything is ok
		int compile;
		glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compile);
		if(compile!=GL_TRUE)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Failed to compile %s shader") % shaderType.c_str());
			
			int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;
	
			glGetUniformivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB ,&infologLength);
			if (infologLength > 0)
			{
				infoLog = new char[infologLength];
				glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
				ENGINE_LOG(SORE_Logging::LVL_ERROR, boost::format("Info log:\n%s") % infoLog);
				delete[] infoLog;
			}
			else
				ENGINE_LOG(SORE_Logging::LVL_INFO, boost::format("No info log for %s shader") % shaderType.c_str());
			return 1;
		}
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, boost::format("Compiling %s shader successful") % shaderType.c_str());
		
		if(type==GL_VERTEX_SHADER)
			vertexShaders.push_back(shader);
		else if(type==GL_FRAGMENT_SHADER)
			fragmentShaders.push_back(shader);
		return 0;
	}
	
	int GLSLShader::AddVertexFile(const char* vertex)
	{
		char* src;
		SORE_FileIO::file_ref file = SORE_FileIO::Open(vertex);
		unsigned int size = SORE_FileIO::Size(file);
		src = new char[size+1];
		SORE_FileIO::Read(src, sizeof(char), size, file);
		src[size] = '\0';
		int status = AddShader(GL_VERTEX_SHADER, src);
		delete[] src;
		return status;
	}
	
	int GLSLShader::AddVertexString(const char* vertex)
	{
		return AddShader(GL_VERTEX_SHADER, vertex);
	}
	
	int GLSLShader::AddFragmentFile(const char* fragment)
	{
		char* src;
		SORE_FileIO::file_ref file = SORE_FileIO::Open(fragment);
		unsigned int size = SORE_FileIO::Size(file);
		src = new char[size+1];
		SORE_FileIO::Read(src, sizeof(char), size, file);
		src[size] = '\0';
		int status = AddShader(GL_FRAGMENT_SHADER, src);
		delete[] src;
		return status;
	}
	
	int GLSLShader::AddFragmentString(const char* fragment)
	{
		return AddShader(GL_FRAGMENT_SHADER, fragment);
	}
	
	void GLSLShader::Bind()
	{
		if(!ShadersSupported() || program==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return;
		}
		glUseProgramObjectARB(program);
	}
	
	void GLSLShader::SetUniform4f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	{
		if(!ShadersSupported() || program==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
			return;
		}
		std::map<std::string, GLint>::iterator it;
		GLint location;
		if((it=uniforms.find(name))==uniforms.end())
		{
			location = glGetUniformLocationARB(program, name.c_str());
			uniforms.insert(std::pair<std::string,GLint>(name,location));
		}
		else
			location = it->second;
		glUniform4fARB(location,v0,v1,v2,v3);
	}

} //end of namespace SORE_Graphics
