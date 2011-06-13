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

void SORE_Graphics::TextureState::Bind(SORE_Resource::GLSLShaderPtr s) const
{
    std::map<std::string, SORE_Resource::Texture2DPtr>::const_iterator it;
    int i;
    for(i = 0,it = textures.begin(); it!= textures.end(); ++it,++i)
    {
        it->second->Bind(s, it->first, i);
    }
}

void SORE_Graphics::TextureState::AddTexture(
    const std::string& samplerName, SORE_Resource::Texture2DPtr tex)
{
    textures[samplerName] = tex;
    cachedHash = 0;
    std::map<std::string, SORE_Resource::Texture2DPtr>::const_iterator it;
    for(it = textures.begin(); it != textures.end(); ++it)
    {
        if(it->second)
        {
            boost::hash_combine(cachedHash, it->second->GetHandle());
        }
    }
}

bool SORE_Graphics::TextureState::Empty() const
{
    return textures.empty();
}

std::size_t SORE_Graphics::TextureState::GetSortKey() const
{
    return cachedHash;
}

SORE_Graphics::TextureState SORE_Graphics::TextureState::GetDiff(
    const TextureState& o) const
{
    TextureState r;
    std::map<std::string, SORE_Resource::Texture2DPtr>::const_iterator it;
    for(it = textures.begin(); it != textures.end(); ++it)
    {
        if(o.textures.find(it->first) == o.textures.end())
        {
            r.AddTexture(it->first, it->second);
            continue;
        }
        const SORE_Resource::Texture2DPtr& t = o.textures.find(it->first)->second;
        const SORE_Resource::Texture2DPtr& t2 = it->second;
        if(t != t2)
            r.AddTexture(it->first, it->second);
    }
    return r;
}