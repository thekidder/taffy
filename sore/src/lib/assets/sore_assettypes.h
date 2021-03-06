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

#ifndef SORE_ASSETTYPES_H
#define SORE_ASSETTYPES_H

#include <sore_resourcecache.h>
#include <sore_resourceproxy.h>

namespace SORE_Resource
{
    class Font;
    class GLSLShader;
    class Material;
    class Texture2D;

    class FontLoader;
    class GLSLShaderLoader;
    class MaterialLoader;
    class Texture2DLoader;

    typedef ResourceProxyWrapper<Font> FontPtr;
    typedef ResourceProxyWrapper<GLSLShader> GLSLShaderPtr;
    typedef ResourceProxyWrapper<Material> MaterialPtr;
    typedef ResourceProxyWrapper<Texture2D> Texture2DPtr;

    // standard cache types
    typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::Font,       SORE_Resource::FontLoader>       Font_cache_t;
    typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::Material,   SORE_Resource::MaterialLoader>   Material_cache_t;
    typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::GLSLShader, SORE_Resource::GLSLShaderLoader> Shader_cache_t;
    typedef SORE_Resource::ResourceCache<std::string, SORE_Resource::Texture2D,  SORE_Resource::Texture2DLoader>  Texture_cache_t;
        
}

#endif