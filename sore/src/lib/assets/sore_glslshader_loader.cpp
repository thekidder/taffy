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

#include <sore_glslshader_loader.h>
#include <sore_logger.h>
#include <sore_util.h>

#include <stdexcept>

SORE_Resource::GLSLShaderLoader::GLSLShaderLoader(
    SORE_FileIO::PackageCache& packageCache_, 
    const std::string& basePath_,
    const std::string& proxyName_)
    : FileResourceLoader<GLSLShader>(packageCache_, basePath_, proxyName_)
{
}

SORE_Resource::GLSLShader* SORE_Resource::GLSLShaderLoader::Load(const std::string& path)
{
    SORE_FileIO::InFile* in = LoadFile(path);
    std::map<std::string, std::map<std::string, std::string> > list =
        SORE_Utility::ParseIniFile(*in);
    delete in;

    GLSLShader* shader = new GLSLShader();

    std::map<std::string, std::map<std::string, std::string> >::iterator i;
    std::map<std::string, std::string>::iterator j;

    for(i=list.begin();i!=list.end();i++)
    {
        std::string section = i->first;

        for(j=i->second.begin();j!=i->second.end();j++)
        {
            SORE_FileIO::InFile* file = LoadFileWithoutBasePath(j->first);
            if(!file->strm().good())
            {
                //TODO: FIXME: sore exception handling
                throw std::runtime_error("Could not load shader from " + j->first);
            }
            size_t size = file->size();
            char* src = new char[size+1];
            file->strm().read(src, size);
            delete file;
            src[size] = '\0';
            if(section=="Vertex")
            {
                shader->AddVertexString(src);
            }
            else if(section=="Fragment")
            {
                shader->AddFragmentString(src);
            }
            else
            {
                ENGINE_LOG(SORE_Logging::LVL_WARNING,
                           boost::format("Invalid material heading: %s") % section);
            }
            delete src;
        }
    }

    shader->Link();

    return shader;
}
