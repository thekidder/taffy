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

#include <sore_texturestate.h>

void SORE_Graphics::TextureState::Bind(GLSLShaderPtr s) const
{
    std::map<std::string, Texture2DPtr>::const_iterator it;
    int i;
    for(i = 0,it = textures.begin(); it!= textures.end(); ++it,++i)
    {
        it->second->Bind(s, it->first, i);
    }
}

void SORE_Graphics::TextureState::AddTexture(
    const std::string& samplerName, Texture2DPtr tex)
{
    textures[samplerName] = tex;
    cachedHash = 0;
    std::map<std::string, Texture2DPtr>::const_iterator it;
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
    std::map<std::string, Texture2DPtr>::const_iterator it;
    for(it = textures.begin(); it != textures.end(); ++it)
    {
        if(o.textures.find(it->first) == o.textures.end())
        {
            r.AddTexture(it->first, it->second);
            continue;
        }
        const Texture2DPtr& t = o.textures.find(it->first)->second;
        const Texture2DPtr& t2 = it->second;
        if(t != t2)
            r.AddTexture(it->first, it->second);
    }
    return r;
}