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

#ifndef SORE_TEXTURE_H
#define SORE_TEXTURE_H

#include "sore_allgl.h"
#include "sore_resource.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{
    class SORE_EXPORT Texture2D : public SORE_Resource::Resource
    {
    public:
        Texture2D(const SORE_Resource::WatchedFileArray& wfa);
        Texture2D(const unsigned char* data, GLint internalFormat,
                  GLenum format, unsigned int width, unsigned int height);
        void Bind(
            GLSLShaderPtr shader,
            const std::string& sampleName,
            unsigned int textureSlot) const;
        const char* Type() const {return "2D texture";}

        void SaveTGA(const char* filename);
        bool GLContextDependent() const {return true;}
        unsigned int GetHandle() const;

        bool operator<(const Texture2D& o) const;
        bool operator==(const Texture2D& o) const;

        static std::string ProcessFilename(const std::string& file);
    protected:
        void Load();
        void LoadTGA(const char* filename);
        void LoadFromData(const unsigned char* data, GLint internalFormat,
                          GLenum format, unsigned int width, unsigned int height);
        void Unload();

        GLuint handle;
    private:
        unsigned int w, h;
    };

    bool operator!=(const Texture2D& one, const Texture2D& two);

    typedef boost::shared_ptr<Texture2D> Texture2DPtr;
}

#endif
