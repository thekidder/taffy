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
//

#include "sore_shaders.h"

namespace SORE_Graphics
{
	void InitShaders()
	{
		if(!glCreateProgram && glCreateProgramARB)
			glCreateProgram = glCreateProgramARB;
		if(!glCreateShader && glCreateShaderARB)
			glCreateShader = glCreateShaderARB;
		if(!glShaderSource && glShaderSourceARB)
			glShaderSource = glShaderSourceARB;
		if(!glCompileShader && glCompileShaderARB)
			glCompileShader = glCompileShaderARB;
		if(!glAttachShader && glAttachObjectARB)
			glAttachShader = glAttachObjectARB;
		if(!glGetShaderiv && glGetShaderivARB)
			glGetShaderiv = glGetShaderivARB;
		if(!glGetShaderInfoLog && glGetShaderInfoLogARB)
			glGetShaderInfoLog = glGetShaderInfoLogARB;
		if(!glUseProgram && glUseProgramObjectARB)
			glUseProgram = glUseProgramObjectARB;
		if(!glDetachShader && glDetachShaderARB)
			glDetachShader = glDetachShaderARB;
		if(!glDeleteShader && glDeleteShaderARB)
			glDeleteShader = glDeleteShaderARB;
	}
	
	void UnbindShaders()
	{
		glUseProgram(0);
	}
	
	GLSLShader::GLSLShader(const char* vertex, const char* fragment)
	{
		Init();
		AddVertexFile(vertex);
		AddFragmentFile(fragment);
		ok = true;
		linked = false;
	}
	
	GLSLShader::~GLSLShader()
	{
		std::vector<GLuint>::iterator it;
		for(it=vertexShaders.begin();it!=vertexShaders.end();it++)
		{
			glDetachShader(program, *it);
			glDeleteShader(*it);
		}
		for(it=fragmentShaders.begin();it!=fragmentShaders.end();it++)
		{
			glDetachShader(program, *it);
			glDeleteShader(*it);
		}
		glDeleteProgram(program);
	}
	
	bool GLSLShader::Ready()
	{
		return ok && linked;
	}
	
	void GLSLShader::Link()
	{
		glLinkProgram(program);
		int link;
		glGetProgramiv(program, GL_LINK_STATUS, &link);
		if(link!=GL_TRUE)
		{
			ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Failed to link shader program");
			int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;

			glGetProgramiv(program, GL_INFO_LOG_LENGTH,&infologLength);
		
			if (infologLength > 0)
			{
				infoLog = new char[infologLength];
				glGetProgramInfoLog(program, infologLength, &charsWritten, infoLog);
				ENGINE_LOG(SORE_Logging::LVL_ERROR, "Info log:\n%s", infoLog);
				delete[] infoLog;
			}
		}
		else
		{
			ENGINE_LOG_S(SORE_Logging::LVL_DEBUG1, "Shader program linked OK");
			linked = true;
		}
	}
	
	int GLSLShader::Init()
	{
		program = glCreateProgram();
		if(program == 0)
		{
			ENGINE_LOG_S(SORE_Logging::LVL_ERROR, "Error creating shader program");
			ok = false;
			return 1; //error
		}
		return 0;
	}
	
	int GLSLShader::AddShader(GLuint type, const char* src)
	{
		if(!(type==GL_VERTEX_SHADER || type==GL_FRAGMENT_SHADER))
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Attempted to create shader of unknown type (type given: %u)", type);
			return 1;
		}
		GLuint shader;
		shader = glCreateShader(type);
		std::string shaderType, source;
		if(type==GL_VERTEX_SHADER) shaderType = "vertex";
		else if(type==GL_FRAGMENT_SHADER) shaderType = "fragment";
		if(shader==0)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Error creating %s shader object.", shaderType.c_str());
			return 1;
		}
		glShaderSource(shader, 1, &src, NULL);
		glCompileShader(shader);
		glAttachShader(program, shader);
		
		//now let's check if everything is ok
		int compile;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compile);
		if(compile!=GL_TRUE)
		{
			ENGINE_LOG(SORE_Logging::LVL_ERROR, "Failed to compile %s shader", shaderType.c_str(),compile,link,GL_TRUE);
			
			int infologLength = 0;
			int charsWritten  = 0;
			char *infoLog;
	
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH,&infologLength);
			if (infologLength > 0)
			{
				infoLog = new char[infologLength];
				glGetShaderInfoLog(shader, infologLength, &charsWritten, infoLog);
				ENGINE_LOG(SORE_Logging::LVL_ERROR, "Info log:\n%s", infoLog);
				delete[] infoLog;
			}
			else
				ENGINE_LOG(SORE_Logging::LVL_INFO, "No info log for %s shader",shaderType.c_str());
			return 1;
		}
		ENGINE_LOG(SORE_Logging::LVL_DEBUG1, "Compiling %s shader successful", shaderType.c_str());
		
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
		glUseProgram(program);
	}
	
	void GLSLShader::SetUniform4f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
	{
		std::map<std::string, GLint>::iterator it;
		GLint location;
		if((it=uniforms.find(name))==uniforms.end())
		{
			location = glGetUniformLocation(program, name.c_str());
			uniforms.insert(std::pair<std::string,GLint>(name,location));
		}
		else
			location = it->second;
		glUniform4f(location,v0,v1,v2,v3);
	}

} //end of namespace SORE_Graphics
