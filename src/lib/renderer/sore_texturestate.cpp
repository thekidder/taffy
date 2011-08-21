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

#include <sore_texturestate.h>

SORE_Graphics::TextureState::TextureState() : cachedHash(0)
{
}

void SORE_Graphics::TextureState::Bind(SORE_Resource::GLSLShaderPtr s) const
{
    Texture_map_t::const_iterator it;
    int i;
    for(i = 0,it = textures.begin(); it != textures.end(); ++it)
    {
        if(it->second.ready)
        {
            it->second.texture->Bind(s, it->first, i);
            ++i;
        }
    }
}

void SORE_Graphics::TextureState::AddTexture(
    const std::string& samplerName, 
    const TextureObject& texture)
{
    textures[samplerName] = texture;
    
    ComputeHash();

    if(!texture.ready)
        unreadyTextures.insert(std::make_pair(samplerName, texture.name));
}

void SORE_Graphics::TextureState::SetTexture(
    const std::string& samplerName, 
    SORE_Resource::Texture2DPtr tex)
{
    Texture_map_t::iterator it = textures.find(samplerName);
    if(it != textures.end())
    {
        it->second.ready = true;
        it->second.texture = tex;
    }

    ComputeHash();

    if(unreadyTextures.find(samplerName) != unreadyTextures.end())
        unreadyTextures.erase(samplerName);
}

bool SORE_Graphics::TextureState::Empty() const
{
    return textures.empty();
}

std::size_t SORE_Graphics::TextureState::GetSortKey() const
{
    return cachedHash;
}

// This function is only valid if the shader is constant
// You always need to bind textures on shader change
SORE_Graphics::TextureState SORE_Graphics::TextureState::GetDiff(
    const TextureState& o) const
{
    TextureState r;
    Texture_map_t::const_iterator it;
    int index_this = 0;
    for(it = textures.begin(); it != textures.end(); ++it, ++index_this)
    {
        if(o.textures.find(it->first) == o.textures.end())
        {
            r.AddTexture(it->first, it->second);
            continue;
        }
        // the texture exists in the both states, need to check which texture
        // unit it is bound to
        // FIXME: we should clean this up so this case is more efficient. We don't
        // always need to rebind, we just need a way to keep track of which texture
        // unit we bind what to
        else
        {
            Texture_map_t::const_iterator jt;
            int index_o;
            for(index_o = 0, jt = o.textures.begin(); jt->first != it->first; ++jt, ++index_o);

            if(index_this != index_o)
                r.AddTexture(it->first, it->second);
        }

        TextureObjectComparator comp;

        const TextureObject& t = o.textures.find(it->first)->second;
        const TextureObject& t2 = it->second;
        // if the two textures are unequal, add to the difference
        if(comp(t, t2) || comp(t2, t))
        {
            r.AddTexture(it->first, it->second);
        }
    }
    return r;
}

void SORE_Graphics::TextureState::ComputeHash()
{
    cachedHash = 0;
    Texture_map_t::const_iterator it;
    for(it = textures.begin(); it != textures.end(); ++it)
    {
        if(it->second.ready)
            boost::hash_combine(cachedHash, it->second.texture->Handle());
        else
            boost::hash_combine(cachedHash, it->second.name);
    }
}