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

#ifndef SORE_RENDERER2D_H
#define SORE_RENDERER2D_H

#include <boost/function.hpp>

#include "sore_batch.h"
#include "sore_fbo.h"
#include "sore_geometrychunk.h"
#include "sore_geometryprovider.h"
#include "sore_graphics.h"
#include "sore_graphicsarray.h"
#include "sore_renderable.h"
#include "sore_renderer.h"
#include "sore_resource.h"
#include "sore_shaders.h"
#include "sore_timing.h"
#ifndef SORE_NO_VBO
#include "sore_vbo.h"
#else
#include "sore_vertexarray.h"
#endif

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
    class SORE_EXPORT Renderer : public SORE_Graphics::IRenderer, boost::noncopyable
    {
    public:
        Renderer(SORE_Resource::ResourcePool& _pool);
        ~Renderer();

        virtual void Render();

        void ClearGeometryProviders();
        void AddGeometryProvider(GeometryProvider* gp);

        //Rendering Statistics
        float GetFPS() const;
        float GetFrameMS() const;
        unsigned int GetDrawCalls() const;
        unsigned int GetNumPolys() const;

        void PushState();
        void PopState();
    protected:
        virtual void OnScreenChange();
    private:
        void ClearGeometry();
        void Build();

        void CalculateFPS();

        ProjectionInfo GetProjection(geometry_layer layer);

        SORE_Resource::ResourcePool& pool;

        //acts as a stack: only providers on the top are called
        std::vector<std::vector<GeometryProvider*> > geometryProviders;
        std::vector<std::vector<GeometryProvider*> >::iterator currentState;

        std::vector<GraphicsArray*> geometry;
        std::vector<RenderBatch> batches;

        //statistics: get info about current rendering
        float fps;
        float ms;
        unsigned int drawCalls;
        unsigned int numPolys;
    };
}

#endif
