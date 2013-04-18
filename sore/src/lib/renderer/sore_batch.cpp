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

#include <sore_batch.h>
#include <sore_logger.h>

#include <stdexcept>

SORE_Graphics::RenderBatch::RenderBatch()
    : geometry(0)
{
}

SORE_Graphics::RenderBatch::RenderBatch(const RenderState& state)
    : geometry(0), bind(false), state(state)
{
}

SORE_Graphics::RenderBatch::RenderBatch(
    const geometry_entry& geometry,
    const RenderState& state, bool bindVBO)
    : geometry(geometry.geometry), type(geometry.type), bind(bindVBO), state(state)
{
    ranges.insert(std::make_pair(geometry.offset, geometry.indices));
}

void SORE_Graphics::RenderBatch::AddIndices(const geometry_entry& geometry)
{
    Geometry_ranges_t::iterator it = ranges.insert(std::make_pair(geometry.offset, geometry.indices)).first;
    Geometry_ranges_t::iterator after = it;
    ++after;
    if(it != ranges.begin())
    {
        Geometry_ranges_t::iterator before = it;
        --before;

        if(before->first + before->second == it->first)
        {
            // merge the two entries, they are consectutive
            before->second += it->second;
            ranges.erase(it);
            it = before;
        }
    }
    
    if(after != ranges.end())
    {
        if(it->first + it->second == after->first)
        {
            // merge the two entries, they are consectutive
            it->second += after->second;
            ranges.erase(after);
        }
    }
}

unsigned int SORE_Graphics::RenderBatch::Render()
{
    state.Apply();
    if(geometry && ranges.size())
    {
        if(bind)
        {
            geometry->BeginDraw();
        }
        unsigned int trisPerPoly;
        if(type == GL_POINTS)
            trisPerPoly = 1;
        else if(type == GL_LINES)
            trisPerPoly = 2;
        else if(type == GL_TRIANGLES)
            trisPerPoly = 3;
        else
            throw std::runtime_error("Unknown primitive type"); // TODO: sore exceptions

        unsigned int numberIndices = 0;
        Geometry_ranges_t::iterator it;
        for(it = ranges.begin(); it != ranges.end(); ++it)
        {
            numberIndices += it->second;
            geometry->DrawElements(it->second, it->first, type);
        }
        return numberIndices/trisPerPoly;
    }
    else
    {
        return 0;
    }
}

void SORE_Graphics::RenderBatch::EndDraw() const
{
    if(geometry)
        geometry->EndDraw();
}

