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
#include <sore_material_loader.h>
#include <sore_texture2d_loader.h>

#include <json/json.h>

#include <string>

SORE_Resource::MaterialLoader::MaterialLoader(
    Shader_cache_t& shaderCache_,
    Texture_cache_t& textureCache_,
    SORE_FileIO::PackageCache& packageCache_, 
    const std::string& basePath_,
    const std::string& proxyName_)
    : FileResourceLoader<Material>(packageCache_, basePath_, proxyName_),
    shaderCache(shaderCache_), textureCache(textureCache_)
{
}

SORE_Resource::Material* SORE_Resource::MaterialLoader::Load(const std::string& path)
{
    SORE_FileIO::InFile* in = LoadFile(path);
    if(!in->strm().good())
    {
        delete in;
        //TODO: FIXME: sore exception handling
        throw std::runtime_error("Could not load material from '" + path + "'");
    }
    Json::Reader reader;
    Json::Value root;
    reader.parse(in->strm(), root, false);
    delete in;

    std::string blendMode = root["blend"].asString();

    SORE_Graphics::Blend_state blendState;
    if(blendMode == "subtractive")
    {
        blendState.depthTest = true;
        blendState.srcFactor = GL_SRC_ALPHA;
        blendState.dstFactor = GL_ONE_MINUS_SRC_ALPHA;
    }
    else if(blendMode == "additive")
    {
        blendState.depthTest = false;
        blendState.srcFactor = GL_SRC_ALPHA;
        blendState.dstFactor = GL_DST_ALPHA;
    }
    // default is opaque
    else
    {
        blendState.depthTest = true;
        blendState.srcFactor = GL_ONE;
        blendState.dstFactor = GL_ZERO;
    }

    GLSLShaderPtr shader = shaderCache.Get(root["shader"].asString());

    Material* material = new Material(blendState, shader);

    // set default uniforms
    Json::Value::Members uniforms = root["uniforms"].getMemberNames();
    for(Json::Value::Members::const_iterator it = uniforms.begin(); it != uniforms.end(); ++it)
    {
        const std::string& name = *it;
        const Json::Value& value = root["uniforms"][name];

        SORE_Resource::GLSLShader::Uniform_map_t::const_iterator uniform = shader->ActiveUniforms().find(name);
        if(uniform == shader->ActiveUniforms().end())
        {
            ENGINE_LOG(SORE_Logging::LVL_ERROR, path + ": failed to set uniform " + name + ": shader has no uniform of that name");
            continue;
        }

        switch(uniform->second.type)
        {
        case GL_SAMPLER_2D:
            material->SetTexture(name, textureCache.Get(value.asString()));
            break;
        default:
            ENGINE_LOG(SORE_Logging::LVL_ERROR, path + ": failed to set uniform " + name + ": unknown type");
        }
    }

    return material;
}