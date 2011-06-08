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

#ifndef SORE_GEOMETRYCHUNK_H
#define SORE_GEOMETRYCHUNK_H

#include <boost/shared_ptr.hpp>

#include <sore_allgl.h>
#include <sore_color.h>
#include <sore_dll.h>

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
    struct vertex
    {
        float tex0i, tex0j;
        float r, g, b, a;
        float normx, normy, normz;
        float x, y, z;
    };

    class SORE_EXPORT GeometryChunk
    {
    public:
        GeometryChunk(unsigned int nVertices, unsigned int nIndices, GLenum type);
        GeometryChunk(const GeometryChunk& o);
        ~GeometryChunk();

        //sets up vertices to the same color
        void SetColor(const Color& c);

        vertex* const GetVertices();
        vertex& GetVertex(unsigned short i);

        unsigned short* const GetIndices();
        unsigned short& GetIndex(unsigned short i);

        GLenum         Type() const;
        unsigned short NumVertices() const;
        unsigned short NumIndices() const;

        GeometryChunk& operator=(const GeometryChunk& o);
    private:
        void Init(const GeometryChunk& o);

        unsigned short numVertices, numIndices;
        vertex* data;
        unsigned short* indices;
        GLenum type;
    };

    typedef boost::shared_ptr<GeometryChunk> GeometryChunkPtr;
}

#endif
