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

#include <sore_glslshader.h>
#include <sore_graphics.h>
#include <sore_util.h>

namespace SORE_Graphics
{
    bool GLSLShader::initCalled = false;
    bool GLSLShader::supported  = false;

    int GLSLShader::InitShaders()
    {
        initCalled = true;
        char* version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if(version==NULL)
            supported = false;
        else
            supported = true;
        if(supported)
        {
            if(!(GLEW_VERSION_2_0 ||
                 (GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader)))
            {
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           "No shader support - check OpenGL version and extensions "
                           "string");
                supported = false;
            }
            else
            {
                supported = true;
            }
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

    GLSLShader::GLSLShader(const char* vertex, const char* fragment,
                           SORE_FileIO::PackageCache* pc)
        : Resource(SORE_Resource::WatchedFileArrayPtr(
                       new SORE_Resource::WatchedFileArray(pc)))
    {
        Init();
        AddVertexFile(vertex);
        AddFragmentFile(fragment);
        ok = true;
    }

    GLSLShader::GLSLShader(SORE_Resource::WatchedFileArrayPtr wfa) :
        Resource(wfa)
    {
        Load();
    }

    void GLSLShader::Load()
    {
        Init();
        SORE_FileIO::InFile* in = File();
        std::map<std::string, std::map<std::string, std::string> > list =
            SORE_Utility::ParseIniFile(*in);
        delete in;

        std::map<std::string, std::map<std::string, std::string> >::iterator i;
        std::map<std::string, std::string>::iterator i2;

        for(i=list.begin();i!=list.end();i++)
        {
            std::string section = i->first;

            for(i2=i->second.begin();i2!=i->second.end();i2++)
            {
                std::string name = i2->first;
                if(section=="Vertex")
                {
                    AddVertexFile(name.c_str());
                }
                else if(section=="Fragment")
                {
                    AddFragmentFile(name.c_str());
                }
                else
                {
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               boost::format("Invalid material heading: %s") % section);
                }
            }
        }
        ok = true;
        Link();
        Bind();
        SORE_Graphics::PrintGLErrors(SORE_Logging::LVL_ERROR);
        UnbindShaders();
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

    bool GLSLShader::Ready() const
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

            glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                      &infologLength);

            if (infologLength > 0)
            {
                infoLog = new char[infologLength];
                glGetInfoLogARB(program, infologLength, &charsWritten, infoLog);
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           boost::format("Info log:\n%s") % infoLog);
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
        if(!initCalled)
            InitShaders();
        if(!ShadersSupported())
        {
            program = 0;
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Error creating shader program (shaders are not "
                       "supported on this system)");
            return 1;
        }
        vertexShaders.clear();
        fragmentShaders.clear();
        program = glCreateProgramObjectARB();
        ok = linked = false;
        uniforms.clear();
        attributes.clear();
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
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Attempted to create shader of unknown "
                                     "type (type given: %u)") % type);
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
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Error creating %s shader object.")
                       % shaderType.c_str());
            return 1;
        }
        glShaderSourceARB(shader, 1, &src, NULL);
        glCompileShaderARB(shader);

        //now let's check if everything is ok
        int compile;
        glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &compile);
        if(compile!=GL_TRUE)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Failed to compile %s shader")
                       % shaderType.c_str());

            int infologLength = 0;
            int charsWritten  = 0;
            char *infoLog;

            glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB,
                                      &infologLength);
            if (infologLength > 0)
            {
                infoLog = new char[infologLength];
                glGetInfoLogARB(shader, infologLength, &charsWritten, infoLog);
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           boost::format("Info log:\n%s") % infoLog);
                delete[] infoLog;
            }
            else
                ENGINE_LOG(SORE_Logging::LVL_INFO,
                           boost::format("No info log for %s shader")
                           % shaderType.c_str());
            return 1;
        }
        ENGINE_LOG(SORE_Logging::LVL_DEBUG1,
                   boost::format("Compiling %s shader successful")
                   % shaderType.c_str());
        glAttachObjectARB(program, shader);
        if(type==GL_VERTEX_SHADER)
            vertexShaders.push_back(shader);
        else if(type==GL_FRAGMENT_SHADER)
            fragmentShaders.push_back(shader);
        return 0;
    }

    char* GLSLShader::LoadFile(const char* filename)
    {
        SORE_FileIO::InFile* file = File(filename);
        if(!file->strm().good())
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       boost::format("Could not load shader filename: %s") % filename);
            return NULL;
        }
        size_t size = file->size();
        char* src = new char[size+1];
        file->strm().read(src, size);
        delete file;
        src[size] = '\0';
        return src;
    }

    int GLSLShader::AddVertexFile(const char* vertex)
    {
        char* src = LoadFile(vertex);
        if(!src)
            return -1;
        int status = AddShader(GL_VERTEX_SHADER, src);
        delete[] src;
        if(status == 0)
        {
            ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                       boost::format("Loaded vertex shader file %s") % vertex);
        }
        return status;
    }

    int GLSLShader::AddVertexString(const char* vertex)
    {
        return AddShader(GL_VERTEX_SHADER, vertex);
    }

    int GLSLShader::AddFragmentFile(const char* fragment)
    {
        char* src = LoadFile(fragment);
        if(!src)
            return -1;
        int status = AddShader(GL_FRAGMENT_SHADER, src);
        delete[] src;
        if(status == 0)
        {
            ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                       boost::format("Loaded fragment shader file %s") % fragment);
        }
        return status;
    }

    int GLSLShader::AddFragmentString(const char* fragment)
    {
        return AddShader(GL_FRAGMENT_SHADER, fragment);
    }

    void GLSLShader::Bind() const
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        glUseProgramObjectARB(program);
    }

    unsigned int GLSLShader::GetHandle() const
    {
        return program;
    }

    bool GLSLShader::operator<(const GLSLShader& o) const
    {
        return program < o.program;
    }

    bool GLSLShader::operator==(const GLSLShader& o) const
    {
        return program == o.program;
    }

    bool operator!=(const GLSLShader& one, const GLSLShader& two)
    {
        return !(one == two);
    }

    GLint GLSLShader::GetUniformLocation(std::string name)
    {
        std::map<std::string, GLint>::iterator it;
        GLint location;
        if((it=uniforms.find(name))==uniforms.end())
        {
            location = glGetUniformLocationARB(program, name.c_str());
            if(location==-1)
            {
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           boost::format("Error getting location of GLSL "
                                         "uniform '%s'. Variable name probably "
                                         "does not exist") % name);
                return -1; //don't insert into uniforms database
            }
            uniforms.insert(std::pair<std::string,GLint>(name,location));
        }
        else
            location = it->second;
        return location;
    }

    GLint GLSLShader::GetAttributeLocation(std::string name)
    {
        std::map<std::string, GLint>::iterator it;
        GLint location;
        if((it=attributes.find(name))==attributes.end())
        {
            location = glGetAttribLocationARB(program, name.c_str());
            if(location==-1)
            {
                ENGINE_LOG
                    (SORE_Logging::LVL_ERROR,
                     boost::format("Error getting location of GLSL attribute "
                                   "'%s'. Variable name probably does not exist")
                     % name);
                return -1; //don't insert into uniforms database
            }
            attributes.insert(std::pair<std::string,GLint>(name,location));
        }
        else
            location = it->second;
        return location;
    }

    void GLSLShader::SetUniform1i(std::string name, GLuint i0)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
            glUniform1iARB(location,i0);
    }

    void GLSLShader::SetUniform1f(std::string name, GLfloat f0)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
            glUniform1fARB(location,f0);
    }

    void GLSLShader::SetUniform2f(std::string name, GLfloat v0, GLfloat v1)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
            glUniform2fARB(location,v0,v1);
    }

    void GLSLShader::SetUniform3f(std::string name, GLfloat v0, GLfloat v1, GLfloat v2)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
            glUniform3fARB(location,v0,v1,v2);
    }

    void GLSLShader::SetUniform4f(std::string name, GLfloat v0, GLfloat v1,
                                  GLfloat v2, GLfloat v3)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
            glUniform4fARB(location,v0,v1,v2,v3);
    }

    void GLSLShader::SetUniform1fv(
        std::string name, unsigned int count, const GLfloat * values)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
        {
            glUniform1fvARB(location, count, values);
        }
    }

    void GLSLShader::SetUniformMatrix4fv(
        std::string name, const GLfloat * values)
    {
        if(!ShadersSupported() || program==0)
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Object is not initialized properly");
            return;
        }
        GLint location = GetUniformLocation(name);
        if(location!=-1)
        {
            glUniformMatrix4fvARB(location, 1, false, values);
        }
    }

    std::string GLSLShader::ProcessFilename(const std::string& filename)
    {
        return filename;
    }
} //end of namespace SORE_Graphics
