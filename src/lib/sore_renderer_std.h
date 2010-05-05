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
