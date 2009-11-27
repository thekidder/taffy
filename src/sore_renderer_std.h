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

#include "sore_fbo.h"
#include "sore_font.h"
#include "sore_geometrychunk.h"
#include "sore_graphics.h"
#include "sore_graphicsarray.h"
#include "sore_postprocess.h"
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
    typedef std::pair<proj_callback, boost::function<render_list ()> >
    geometry_callback;

    struct vbo_tex_order
    {
        vbo_tex_order(Texture2DPtr t,
                      boost::shared_ptr<GLSLShader> sh,
                      unsigned s, unsigned l)
            : tex(t), shad(sh), triStart(s), triLen(l) {}
        Texture2DPtr tex;
        boost::shared_ptr<GLSLShader> shad;
        unsigned int triStart;
        unsigned int triLen;
    };

    class SORE_EXPORT Renderer;

    typedef std::vector<vbo_tex_order> texture_stack;

    struct geometry_provider
    {
        boost::function<render_list ()> geometryCallback;
        proj_callback projCallback;
        boost::function<const SORE_Math::Matrix4<float>& ()> cameraCallback;
        PostProcessEffect* effect;
        blend_mode blend;
    };

    struct batch
    {
        batch()
        {
#ifndef SORE_NO_VBO
            second = new VBO(true, true);
#else
            second = new VertexArray(true, true);
#endif
        }
        texture_stack first;
        GraphicsArray* second;
        PostProcessEffect* effect;
        GLenum blendSFactor, blendDFactor;
        proj_callback projCallback;
        boost::function<const SORE_Math::Matrix4<float>& ()> cameraCallback;
    };

    class SORE_EXPORT Renderer : public SORE_Graphics::IRenderer
    {
    public:
        Renderer(SORE_Resource::ResourcePool& _rm);
        ~Renderer();

        virtual void Render();

        void ClearGeometryProviders();
        void AddGeometryProvider(geometry_provider c);
        void Build();

        //Rendering Statistics
        float GetFPS() const;
        float GetFrameMS() const;
        unsigned int GetDrawCalls() const;
        unsigned int GetNumPolys() const;

        void PushState();
        void PopState();

        void SetupProjection(SORE_Graphics::ProjectionInfo& pi);
    protected:
        void RenderBatch(batch& b);
        virtual void OnScreenChange();

        //returns 0 on success
        int ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection);
    private:
        SORE_Resource::ResourcePool& rm;

        //acts as a stack: only providers on the top are called
        std::vector<
            std::vector<geometry_provider>
            > geometryProviders;

        std::vector<std::vector<geometry_provider> >::iterator currentProvider;

        std::vector<batch> batches;

        //statistics: get info about current rendering
        float fps;
        float ms;
        unsigned int drawCalls;
        unsigned int numPolys;
    };

    bool GeometrySort(std::pair<const SORE_Math::Matrix4<float>*,
                      const GeometryChunk*> one,
                      std::pair<const SORE_Math::Matrix4<float>*,
                      const GeometryChunk*> two);
}

#endif
