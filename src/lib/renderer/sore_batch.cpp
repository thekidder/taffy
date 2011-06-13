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

SORE_Graphics::RenderBatch::RenderBatch()
    : geometry(0), numberIndices(0)
{
}

SORE_Graphics::RenderBatch::RenderBatch(
    const geometry_entry& geometry,
    const RenderState& state, bool bindVBO)
    : geometry(geometry.geometry), numberIndices(geometry.indices),
      indexOffset(geometry.offset), type(geometry.type), bind(bindVBO), state(state)
{
}

void SORE_Graphics::RenderBatch::AddIndices(unsigned int numIndices)
{
    numberIndices += numIndices;
}

unsigned int SORE_Graphics::RenderBatch::Render()
{
    state.Apply();
    if(geometry && numberIndices)
    {
        if(bind)
        {
            geometry->BeginDraw();
        }
        unsigned int trisPerPoly = 1;
        if(type == GL_POINTS)
            trisPerPoly = 1;
        else if(type == GL_LINES)
            trisPerPoly = 2;
        else if(type == GL_TRIANGLES)
            trisPerPoly = 3;
        else
            ENGINE_LOG(SORE_Logging::LVL_ERROR, "Unsupported primitive type");
        geometry->DrawElements(numberIndices, indexOffset, type);
        return numberIndices/trisPerPoly;
    }
    else
    {
        return 0;
    }
}

void SORE_Graphics::RenderBatch::EndDraw() const
{
    geometry->EndDraw();
}

