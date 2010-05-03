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
#include <boost/unordered_map.hpp>

#include <sore_batch.h>
#include <sore_fbo.h>
#include <sore_geometrychunk.h>
#include <sore_geometryprovider.h>
#include <sore_graphics.h>
#include <sore_graphicsarray.h>
#include <sore_noncopyable.h>
#include <sore_renderable.h>
#include <sore_renderer.h>
#include <sore_resource.h>
#include <sore_shaders.h>
#include <sore_timing.h>
#ifndef SORE_NO_VBO
#include <sore_vbo.h>
#else
#include <sore_vertexarray.h>
#endif

/**
   @author Adam Kidder <thekidder@gmail.com>
*/
namespace SORE_Graphics
{
    class SORE_EXPORT Renderer : public SORE_Graphics::IRenderer, SORE_Utility::Noncopyable
    {
    public:
        Renderer(SORE_Resource::ResourcePool& _pool);
        ~Renderer();

        virtual void Render();

        void ClearGeometryProviders();
        void AddStaticGeometry(Renderable r);
        void AddGeometryProvider(GeometryProvider* gp);
        void SetCamera(geometry_layer layer, camera_callback camera);

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
        void ClearGeometry(std::vector<GraphicsArray*>& geometry);
        void Build();
        void BuildStatic();
        void MakeBatches(
            std::vector<Renderable>& allRenderables, 
            std::vector<RenderBatch>& batches,
            boost::unordered_map<Renderable, geometry_entry>& geometryMap,
            bool isStatic);
        void BuildGeometryBuffers(
            std::vector<Renderable>& allRenderables,
            std::vector<GraphicsArray*>& geometry,
            bool isStatic,
            boost::unordered_map<Renderable, geometry_entry>& result);

        void CalculateFPS();

        camera_info GetCamera(geometry_layer layer);

        SORE_Resource::ResourcePool& pool;

        struct render_info
        {
            std::vector<GeometryProvider*> providers;
            std::map<geometry_layer, camera_callback> cameras;
        };
        //acts as a stack: only providers on the top are called
        std::vector<render_info> geometryProviders;
        std::vector<render_info>::iterator currentState;

        std::vector<GraphicsArray*> geometry;
        std::vector<RenderBatch> batches;

        std::vector<GraphicsArray*> staticGeometry;
        boost::unordered_map<Renderable, geometry_entry> staticMap;
        std::vector<Renderable> staticRenderables;
        std::vector<RenderBatch> staticBatches;

        //statistics: get info about current rendering
        float fps;
        float ms;
        unsigned int drawCalls;
        unsigned int numPolys;
    };
}

#endif
