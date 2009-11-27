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

#include "sore_material.h"
#include "sore_util.h"
#include <boost/lexical_cast.hpp>

namespace SORE_Graphics
{
    Material::Material(std::string materialFile, std::string additionalInfo,
                       SORE_FileIO::PackageCache* pc) :
        Resource(materialFile, pc, additionalInfo), useShader(false),
        file(materialFile)
    {
        Load();
    }

    Material::~Material()
    {
    }

    void Material::Load()
    {
        textureMap.clear();
        textureOrder.clear();
        std::map<std::string, std::map<std::string, std::string> > list =
            SORE_Utility::ParseIniFile(file.c_str(), packageCache);

        std::map<std::string, std::map<std::string, std::string> >::iterator i;
        std::map<std::string, std::string>::iterator i2;

        for(i=list.begin();i!=list.end();i++)
        {
            std::string section = i->first;

            for(i2=i->second.begin();i2!=i->second.end();i2++)
            {
                std::string name = i2->first;
                std::string value = i2->second;

                if(section == "Flags")
                {
                    flags.push_back(name);
                }
                else if(section=="Textures")
                {
                    boost::shared_ptr<SORE_Graphics::Texture2D> tex =
                        rm->GetResource<SORE_Graphics::Texture2D>(value);
                    if(tex!=NULL)
                    {
                        textureMap[name].second = tex;
                    }
                }
                else if(section=="Shader")
                {
                    if(name=="shader_file")
                    {
                        if(rm)
                            shader = rm->GetResource<GLSLShader>(value);
                        else
                            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                                       "No resource manager set");
                    }
                    else if(name=="use_shader")
                    {
                        std::transform(value.begin(), value.end(),
                                       value.begin(), ::tolower);

                        try
                        {
                            if(value == "true") useShader = true;
                            else if(value == "false")
                            {
                                ENGINE_LOG(SORE_Logging::LVL_WARNING,
                                           "Not using shader: dropping back to"
                                           " incomplete FF pipeline");
                                useShader = false;
                            }
                            else useShader = boost::lexical_cast<bool>(value);
                        }
                        catch(boost::bad_lexical_cast)
                        {
                            useShader = true;
                        }
                        if(useShader && !GLSLShader::ShadersSupported())
                        {
                            ENGINE_LOG(SORE_Logging::LVL_WARNING,
                                       "GLSL Shaders not supported");
                            ENGINE_LOG(SORE_Logging::LVL_WARNING,
                                       "Not using shader: dropping back "
                                       "to incomplete FF pipeline");
                            useShader = false;
                        }
                    }
                }
                else if(section=="Fixed-Function")
                {
                }
                else if(section=="Texture-Combine")
                {
                    std::transform(value.begin(),value.end(),value.begin(),::tolower);

                    if     (value == "modulate")
                        textureMap[name].first = GL_MODULATE;
                    else if(value == "blend")
                        textureMap[name].first = GL_BLEND;
                    else if(value == "decal")
                        textureMap[name].first = GL_DECAL;
                    else if(value == "replace")
                        textureMap[name].first = GL_REPLACE;
                    else
                        textureMap[name].first = GL_MODULATE;

                    textureOrder.push_back(name);
                }
                else
                {
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               boost::format("Invalid material heading: %s") %section);
                }
            }
        }
        GLenum error;
        while((error=glGetError())!=GL_NO_ERROR)
        {
            ENGINE_LOG(SORE_Logging::LVL_DEBUG2,
                       boost::format("Material: GL Error: %d") % error);
        }
    }

    void Material::Bind()
    {
        if(GLSLShader::ShadersSupported() && shader && useShader)
            shader->Bind();
        else
            GLSLShader::UnbindShaders();
        for(size_t i=0;i<textureOrder.size();i++)
        {
            if(( (GLSLShader::ShadersSupported() && shader && useShader) ||
                 textureMap[textureOrder[i]].first!=-1) &&
               textureMap[textureOrder[i]].second!=NULL)
            {
                glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
                textureMap[textureOrder[i]].second->Bind();
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                          textureMap[textureOrder[i]].first);
                if(shader && useShader)
                    shader->SetUniform1i(textureOrder[i], static_cast<GLuint>(i));
            }
        }
    }

    std::vector< std::string > SORE_Graphics::Material::GetFlags()
    {
        return flags;
    }

    void SORE_Graphics::Material::AddFlag(std::string flag)
    {
        flags.push_back(flag);
    }

    bool SORE_Graphics::Material::HasFlag(std::string flag)
    {
        if(find(flags.begin(), flags.end(), flag)==flags.end()) return false;
        return true;
    }
}
