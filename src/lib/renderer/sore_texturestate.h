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

#ifndef SORE_TEXTURESTATE_H
#define SORE_TEXTURESTATE_H

#include <sore_dll.h>
#include <sore_glslshader.h>
#include <sore_texture2d.h>

#include <boost/functional/hash.hpp>

#include <map>
#include <set>
#include <string>

namespace SORE_Graphics
{
    class SORE_EXPORT TextureState
    {
    public:
        struct TextureObject
        {
            TextureObject(const std::string& s = "") : ready(false), name(s) {}
            TextureObject(const std::string& s, SORE_Resource::Texture2DPtr t) 
                : ready(t), name(s), texture(t) {}
            TextureObject(SORE_Resource::Texture2DPtr t) : ready(t), texture(t) {}

            bool ready;
            std::string name;
            SORE_Resource::Texture2DPtr texture;

            bool operator==(const TextureObject& o) const
            {
                return ready == o.ready && name == o.name && texture == o.texture;
            }
        };

        struct TextureObjectComparator
        {
            bool operator()(const TextureObject& one, const TextureObject& two) const
            {
                if(one.ready && !two.ready)
                    return true;
                else if(two.ready && !one.ready)
                    return false;
                else if(one.ready && two.ready)
                    return one.texture->Handle() < two.texture->Handle();
                else
                    return one.name < two.name;
            }
        };

        TextureState();

        void Bind(SORE_Resource::GLSLShaderPtr s) const;

        void AddTexture(const std::string& samplerName, const TextureObject& texture);
        void SetTexture(const std::string& samplerName, SORE_Resource::Texture2DPtr tex);

        // returns true if there are no textures
        bool Empty() const; 
        // returns true if all texture names have textures bound to them
        bool Ready() const { return unreadyTextures.empty(); }

        bool Contains(const std::string& name) const { return textures.find(name) != textures.end(); }
        const TextureObject& Get(const std::string& name) const { return textures.find(name)->second; }

        typedef std::map<std::string, std::string> Unready_texture_map_t;
        const Unready_texture_map_t& UnreadyTextures() const { return unreadyTextures; }

        std::size_t GetSortKey() const;

        // returns all textures in this not in o
        TextureState GetDiff(const TextureState& o) const;

        bool operator==(const TextureState& o) const { return textures == o.textures; }
    private:
        void ComputeHash();

        typedef std::map<std::string, TextureObject, TextureObjectComparator> Texture_map_t;
        Texture_map_t textures;

        std::size_t cachedHash;


        Unready_texture_map_t unreadyTextures;
    };

    typedef boost::shared_ptr<TextureState> TextureStatePtr;
}
#endif
