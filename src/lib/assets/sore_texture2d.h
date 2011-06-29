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

#ifndef SORE_TEXTURE2D_H
#define SORE_TEXTURE2D_H

#include <boost/utility.hpp>

#include <sore_allgl.h>
#include <sore_assettypes.h>
#include <sore_noncopyable.h>
#include <sore_glslshader.h>

namespace SORE_Resource
{
    class SORE_EXPORT Texture2D : SORE_Utility::Noncopyable
    {
    public:
        Texture2D(const void* data, GLint internalFormat,
                  GLenum format, unsigned int width, unsigned int height);
        Texture2D(GLuint handle_);
        ~Texture2D();

        void LoadFromData(const void* data, GLint internalFormat,
                          GLenum format, unsigned int width, unsigned int height);

        void Bind(
            GLSLShaderPtr shader,
            const std::string& sampleName,
            unsigned int textureSlot) const;

        void SaveTGA(const char* filename);
        unsigned int Handle() const { return handle; }
        bool Loaded() const { return handle != 0; }

        bool operator<(const Texture2D& o) const;
        bool operator==(const Texture2D& o) const;
    private:
        void Unload();

        GLuint handle;
        // external textures are not deleted when the object is destroyed
        bool external;

        unsigned int w, h;
    };

    bool operator!=(const Texture2D& one, const Texture2D& two);
}

#endif
