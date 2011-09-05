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
#include <sore_logger.h>
#include <sore_texture2d.h>

#include <fstream>
#include <stdexcept>

// TODO: FIXME: SORE exception support

namespace SORE_Resource
{
    bool GLSLShader::initCalled = false;
    bool GLSLShader::supported  = false;

    const GLSLShader::glsl_variable_info GLSLShader::none = GLSLShader::glsl_variable_info(-1, GL_NONE, 1);

    GLSLShader::GLSLShader()
        : linked(false), program(0)
    {
        Init();
    }

    GLSLShader::GLSLShader(const char* vertex, const char* fragment)
        : linked(false), program(0)
    {
        Init();

        AddVertexString(vertex);
        AddFragmentString(fragment);

        Link();
    }

    GLSLShader::~GLSLShader()
    {
        Unload();
    }


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

    bool GLSLShader::Loaded() const
    {
        return linked && program!=0;
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

        PopulateUniformLocations();
        PopulateAttributeLocations();
        PrintInfo();
    }

    const char* GLenumToString(GLenum type)
    {
        char* typeStr;
        switch(type)
        {
        case GL_FLOAT:
            typeStr = "GL_FLOAT";
            break;
        case GL_FLOAT_VEC2:
            typeStr = "GL_FLOAT_VEC2";
            break;
        case GL_FLOAT_VEC3:
            typeStr = "GL_FLOAT_VEC3";
            break;
        case GL_FLOAT_VEC4:
            typeStr = "GL_FLOAT_VEC4";
            break;
        case GL_FLOAT_MAT2:
            typeStr = "GL_FLOAT_MAT2";
            break;
        case GL_FLOAT_MAT3:
            typeStr = "GL_FLOAT_MAT3";
            break;
        case GL_FLOAT_MAT4:
            typeStr = "GL_FLOAT_MAT4";
            break;
        case GL_FLOAT_MAT2x3:
            typeStr = "GL_FLOAT_MAT2x3";
            break;
        case GL_FLOAT_MAT2x4:
            typeStr = "GL_FLOAT_MAT2x4";
            break;
        case GL_FLOAT_MAT3x2:
            typeStr = "GL_FLOAT_MAT3x2";
            break;
        case GL_FLOAT_MAT3x4:
            typeStr = "GL_FLOAT_MAT3x4";
            break;
        case GL_FLOAT_MAT4x2:
            typeStr = "GL_FLOAT_MAT4x2";
            break;
        case GL_FLOAT_MAT4x3:
            typeStr = "GL_FLOAT_MAT4x3";
            break;
        case GL_SAMPLER_2D:
            typeStr = "GL_SAMPLER_2D";
            break;
        case GL_NONE:
            typeStr = "GL_NONE";
            break;
        default:
            typeStr = "invalid type";
            break;
        }

        return typeStr;
    }

    void GLSLShader::PrintInfo()
    {
        std::map<std::string, glsl_variable_info>::iterator it;
        for(it = uniforms.begin(); it != uniforms.end(); ++it)
        {
            ENGINE_LOG(
                SORE_Logging::LVL_DEBUG1,
                boost::format(
                "Program %d: Found uniform '%s' of type %s and size %d at index %d")
                % program % it->first % GLenumToString(it->second.type) % it->second.size % it->second.index);
        }

        for(it = attributes.begin(); it != attributes.end(); ++it)
        {
            ENGINE_LOG(
                SORE_Logging::LVL_DEBUG1,
                boost::format(
                "Program %d: Found attribute '%s' of type %s and size %d at index %d")
                % program % it->first % GLenumToString(it->second.type) % it->second.size % it->second.index);
        }
    }

    void GLSLShader::Init()
    {
        if(!initCalled)
            InitShaders();
        if(!ShadersSupported())
        {
            throw std::runtime_error(
                "Error creating shader program (shaders are not "
                "supported on this system)");
        }

        program = glCreateProgramObjectARB(); 
        if(program == 0)
        {
            throw std::runtime_error("Error creating shader program");
        }
    }

    void GLSLShader::Unload()
    {
        vertexShaders.clear();
        fragmentShaders.clear();
        uniforms.clear();
        attributes.clear();

        if(program == 0)
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

    int GLSLShader::AddVertexString(const char* vertex)
    {
        return AddShader(GL_VERTEX_SHADER, vertex);
    }

    int GLSLShader::AddFragmentString(const char* fragment)
    {
        return AddShader(GL_FRAGMENT_SHADER, fragment);
    }

    void GLSLShader::Bind() const
    {
        if(!Loaded())
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

    void GLSLShader::SetUniform(const std::string& name, GLint i0)
    {
        GLint location = GetCheckedIndex(name, GL_INT);
        if(location != -1)
            glUniform1iARB(location, i0);
    }

    void GLSLShader::SetUniform(const std::string& name, GLfloat f0)
    {
        GLint location = GetCheckedIndex(name, GL_FLOAT);
        if(location != -1)
            glUniform1fARB(location,f0);
    }

    void GLSLShader::SetUniform(const std::string& name, const SORE_Math::Vector2<float>& v)
    {
        GLint location = GetCheckedIndex(name, GL_FLOAT_VEC2);
        if(location != -1)
            glUniform1fvARB(location, 2, v.GetValue());
    }

    void GLSLShader::SetUniform(const std::string& name, const SORE_Math::Vector3<float>& v)
    {
        GLint location = GetCheckedIndex(name, GL_FLOAT_VEC3);
        if(location != -1)
            glUniform1fvARB(location, 3, v.GetValue());
    }

    void GLSLShader::SetUniform(const std::string& name, const SORE_Math::Vector4<float>& v)
    {
        GLint location = GetCheckedIndex(name, GL_FLOAT_VEC4);
        if(location != -1)
            glUniform1fvARB(location, 4, v.GetValue());
    }

    void GLSLShader::SetUniform(
        const std::string& name, const SORE_Math::Matrix4<float>& m)
    {
        GLint location = GetCheckedIndex(name, GL_FLOAT_MAT4);
        if(location != -1)
            glUniformMatrix4fvARB(location, 1, false, m.GetData());
    }

    void GLSLShader::SetUniformTexture(
            const std::string& name, GLuint textureSlot)
    {
        GLint location = GetCheckedIndex(name, GL_SAMPLER_2D);
        if(location != -1)
            glUniform1i(location, textureSlot);
    }

    const GLSLShader::Uniform_map_t& GLSLShader::ActiveUniforms() const
    {
        return uniforms;
    }

    const GLSLShader::Attribute_map_t& GLSLShader::ActiveAttributes() const
    {
        return attributes;
    }

    const GLSLShader::glsl_variable_info& GLSLShader::GetUniform(const std::string& name)
    {
        Uniform_map_t::iterator it;
        if((it=uniforms.find(name))==uniforms.end())
        {
            ENGINE_LOG(
                SORE_Logging::LVL_ERROR,
                boost::format("Error getting location of GLSL uniform "
                              "'%s'. Variable name probably does not exist")
                     % name);
            // cache this name in the table to avoid repeating error message
            uniforms.insert(std::make_pair(name, none));
            return none;
        }
        else
        {
            return it->second;
        }
    }

    const GLSLShader::glsl_variable_info& GLSLShader::GetAttribute(const std::string& name)
    {
        Attribute_map_t::iterator it;
        if((it=attributes.find(name))==attributes.end())
        {
            ENGINE_LOG(
                SORE_Logging::LVL_ERROR,
                boost::format("Error getting location of GLSL attribute "
                              "'%s'. Variable name probably does not exist")
                     % name);
            // cache this name in the table to avoid repeating error message
            attributes.insert(std::make_pair(name, none));
            return none;
        }
        else
        {
            return it->second;
        }
    }

    void GLSLShader::PopulateUniformLocations()
    {
        GLint numUniforms;
        GLint maxUniformStringLen;
        glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numUniforms);
        glGetProgramiv(program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformStringLen);

        for(GLint i = 0; i < numUniforms; ++i)
        {
            char* name = new char[maxUniformStringLen];
            GLint size;
            GLenum type;

            glGetActiveUniform(program, i, maxUniformStringLen, 0, &size, &type, name);

            uniforms.insert(std::make_pair(name, glsl_variable_info(i, type, size)));

            delete name;
        }
    }

    void GLSLShader::PopulateAttributeLocations()
    {
        GLint numAttributes;
        GLint maxAttributeStringLen;
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeStringLen);

        for(GLint i = 0; i < numAttributes; ++i)
        {
            char* name = new char[maxAttributeStringLen];
            GLint size;
            GLenum type;

            glGetActiveAttrib(program, i, maxAttributeStringLen, 0, &size, &type, name);

            attributes.insert(std::make_pair(name, glsl_variable_info(i, type, size)));

            delete name;
        }
    }

    int GLSLShader::GetCheckedIndex(const std::string& name, GLenum type)
    {
        if(!ShadersSupported() || !linked || !program)
        {
            throw std::runtime_error("Shader is not initialized");
        }
        const glsl_variable_info& var = GetUniform(name);
        // don't check if var.type is none, this means it's an invalid uniform anyways
        // (and we've already logged an error)
        if(var.type != GL_NONE && var.type != type)
            throw std::runtime_error("Setting incorrect uniform type");
        return var.index;
    }
} //end of namespace SORE_Graphics
