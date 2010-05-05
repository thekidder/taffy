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
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#ifndef SORE_GRAPHICS_ARRAY_H
#define SORE_GRAPHICS_ARRAY_H

#include <vector>

#include <sore_matrix4x4.h>
#include <sore_allgl.h>
#include <sore_geometrychunk.h>

namespace SORE_Graphics
{
    /**
       @author Adam Kidder <thekidder@gmail.com>
       Class abstracting the details of Vertex Arrays and VBOs
    */
    class SORE_EXPORT GraphicsArray
    {
    public:
        GraphicsArray(bool isStatic, bool t = false, bool c = false, bool n = false);
        virtual ~GraphicsArray();

        virtual void Build() = 0;

        void Clear();
        void AddObject(GeometryChunkPtr geometry,
                       boost::shared_ptr<SORE_Math::Matrix4<float> > transform);

        void BeginDraw();
        void EndDraw();

        void DrawAll(GLenum type);
        void DrawElements(unsigned int numIndices, unsigned short indexOffset, GLenum type);

        bool Empty() const;
    protected:
        virtual void BeginDrawHook() = 0;
        virtual void* GetOffset(void* pointer, unsigned int offset) = 0;

        std::vector<vertex> vertices;
        std::vector<unsigned short> indices;

        bool hasTexCoords, hasNormals, hasColors;
        bool isStatic;
    };
}

#endif //SORE_GRAPHICS_ARRAY_H
