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

#include <sore_logger.h>
#include <sore_pipe.h>

#include <boost/foreach.hpp>

#include <stdexcept>

void SORE_Graphics::Pipe::AddChildPipe(Pipe* child)
{
    children.push_back(child);
}

void SORE_Graphics::Pipe::Setup(Renderbuffer_map_t& renderBuffers)
{
    doSetup(renderBuffers);

    pipe_vector::iterator i;
    for(i = children.begin(); i != children.end(); ++i)
    {
        i->Setup(renderBuffers);
    }
}

void SORE_Graphics::Pipe::Render
(
    const camera_table& cameras,
    Renderbuffer_map_t& renderbuffers,
    render_list& list,
    GLCommandList& renderQueue,
    BufferManager* bm
)
{
    render_list& newList = beginRender(cameras, renderbuffers, list, renderQueue, bm);

    pipe_vector::iterator i;
    for(i = children.begin(); i != children.end(); ++i)
    {
        i->Render(cameras, renderbuffers, newList, renderQueue, bm);
    }

    finishRender(cameras, renderbuffers, list, renderQueue, bm);
}

SORE_Graphics::SortingPipe::SortingPipe(sorting_predicate comp) : comparator(comp)
{
}

void SORE_Graphics::SortingPipe::doSetup(Renderbuffer_map_t& renderBuffers)
{
    sortedList.clear();
}

SORE_Graphics::render_list& SORE_Graphics::SortingPipe::beginRender
(
    const camera_table& cameras,
    Renderbuffer_map_t& renderbuffers,
    render_list& list,
    GLCommandList& renderQueue,
    BufferManager* bm
)
{
    sortedList = list;

    std::sort(sortedList.begin(), sortedList.end(), comparator);

    return sortedList;
}

bool SORE_Graphics::NullFilter(const Renderable& r)
{
    return true;
}

SORE_Graphics::KeywordFilter::KeywordFilter(const std::string& keyword) : keyword(keyword)
{
}

bool SORE_Graphics::KeywordFilter::operator()(const Renderable& r) const
{
    return r.HasKeyword(keyword);
}

SORE_Graphics::render_list& SORE_Graphics::NullPipe::beginRender
(
    const camera_table& cameras,
    Renderbuffer_map_t& renderbuffers,
    render_list& list,
    GLCommandList& renderQueue,
    BufferManager* bm
)
{
    return list;
}


SORE_Graphics::RenderPipe::RenderPipe(const std::string& cameraName) : camera(cameraName)
{
}

SORE_Graphics::render_list& SORE_Graphics::RenderPipe::beginRender
(
    const camera_table& cameras,
    Renderbuffer_map_t& renderBuffers,
    render_list& list,
    GLCommandList& renderQueue,
    BufferManager* bm
)
{
    if(cameras.find(camera) == cameras.end())
    {
        // TODO: FIXME: sore exceptions
        throw std::runtime_error("Could not find named camera " + camera);
    }

    const camera_info& cam = cameras.find(camera)->second;

    BOOST_FOREACH(const Renderable& r, list)
    {
        renderQueue.AddRenderable(r, bm->LookupGC(r.GetGeometryChunk()), cam);
    }

    return list;
}

SORE_Graphics::FilterPipe::FilterPipe(filter_predicate filterFunction) : filter(filterFunction)
{
}

void SORE_Graphics::FilterPipe::doSetup(Renderbuffer_map_t& renderBuffers)
{
    newList.clear();
}

SORE_Graphics::render_list& SORE_Graphics::FilterPipe::beginRender
(
    const camera_table& cameras,
    Renderbuffer_map_t& renderBuffers,
    render_list& list,
    GLCommandList& renderQueue,
    BufferManager* bm
)
{
    BOOST_FOREACH(Renderable r, list)
    {
        if(filter(r))
        {
            newList.push_back(r);
        }
    }
    return newList;
}
