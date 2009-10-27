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

#include <algorithm>

#include "sore_allgl.h"
#include "sore_renderer2d.h"
#include "sore_shaders.h"

namespace SORE_Graphics
{
    inline int next_p2 (int a )
    {
        int rval=1;
        while(rval<a) rval<<=1;
        return rval;
    }

    Renderer2D::Renderer2D(SORE_Resource::ResourcePool& _rm)
        : rm(_rm)
    {
        shad = rm.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
        PushState();
    }

    Renderer2D::~Renderer2D()
    {
        for(std::vector<batch>::iterator dit = batches.begin();dit!=batches.end();++dit)
        {
            delete dit->second;
        }
    }

    void Renderer2D::SetupProjection(SORE_Graphics::ProjectionInfo& pi)
    {
        switch(pi.type)
        {
        case SORE_Graphics::NONE:
            break;
        case SORE_Graphics::ORTHO2D:
            if(pi.useScreenCoords)
            {
                pi.top = 0;
                pi.left = 0;
                pi.bottom = static_cast<GLfloat>(screen.height);
                pi.right = static_cast<GLfloat>(screen.width);
                pi.ratio = screen.ratio;
            }
            else if(pi.useScreenRatio)
            {
                pi.bottom = pi.left / screen.ratio;
                pi.top = pi.right / screen.ratio;
                pi.ratio = screen.ratio;
            }
            else
            {
                pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
            }
            break;
        case SORE_Graphics::ORTHO:
            //TODO: finish ortho projection
            break;
        case SORE_Graphics::PERSPECTIVE:
            if(pi.useScreenRatio)
            {
                pi.ratio = screen.ratio;
            }
            else
            {
                pi.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
            }
            break;
        default:
            break;
        }
    }

    int Renderer2D::ChangeProjectionMatrix(SORE_Graphics::ProjectionInfo& projection)
    {
        SetupProjection(projection);
        int returnCode = 0;
        glMatrixMode( GL_PROJECTION );
        glLoadIdentity( );
        switch(projection.type)
        {
        case SORE_Graphics::NONE:
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                                 "No projection type set, could not initialize projection");
            returnCode = -1;
            break;
        case SORE_Graphics::ORTHO2D:
            gluOrtho2D(projection.left, projection.right, projection.top, projection.bottom);
            break;
        case SORE_Graphics::ORTHO:
            //TODO: finish ortho projection
            break;
        case SORE_Graphics::PERSPECTIVE:
        {
            if(projection.useScreenRatio)
                projection.ratio = static_cast<float>(screen.width) /
                    static_cast<float>(screen.height);
            gluPerspective(projection.fov, projection.ratio, projection.znear, projection.zfar );
            break;
        }
        default:
            returnCode = -1;
            break;
        }
        glMatrixMode( GL_MODELVIEW );

        glLoadIdentity( );
        return returnCode;
    }

    void Renderer2D::OnScreenChange()
    {
    }

    void Renderer2D::Build()
    {
        if(!currentProvider->size())
            return;

        for(std::vector<batch>::iterator dit = batches.begin();dit!=batches.end();++dit)
        {
            delete dit->second;
        }

        batches.clear();

        std::vector<geometry_provider >::iterator git;
        for(git=currentProvider->begin();git!=currentProvider->end();++git)
        {
            unsigned int numIndices = 0, totalIndices = 0;
            batch newBatch;
            newBatch.projCallback = git->projCallback;
            newBatch.cameraCallback = git->cameraCallback;
            newBatch.effect = git->effect;
            switch(git->blend)
            {
            case ADDITIVE:
                newBatch.blendSFactor = GL_SRC_ALPHA;
                newBatch.blendDFactor = GL_DST_ALPHA;
                break;
            case SUBTRACTIVE:
            default: //treat unknown type as subtractive by default
                newBatch.blendSFactor = GL_SRC_ALPHA;
                newBatch.blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
                break;
            }
            render_list geometry = git->geometryCallback();
            if(geometry.size())
            {
                std::sort(geometry.begin(), geometry.end(), &GeometrySort);
                render_list::iterator it;
                for(it=geometry.begin();it!=geometry.end();++it)
                {
                    newBatch.second->AddObject(it->second->Vertices(), it->second->Indices(),
                        it->second->NumVertices(), it->second->NumIndices(),
                        it->first, it->second->TexCoords(), NULL,
                        it->second->Colors());
                    if(!newBatch.first.size() ||
                       newBatch.first.back().tex->GetHandle() !=
                       it->second->GetTexture()->GetHandle())
                    {
                        if(newBatch.first.size()>0)
                        {
                            newBatch.first.back().triLen = numIndices/3;
                        }
                        newBatch.first.push_back(
                            vbo_tex_order(it->second->GetTexture(), totalIndices/3, 0));
                        numIndices = 0;
                    }
                    numIndices+=it->second->NumIndices();
                    totalIndices+=it->second->NumIndices();
                }
                newBatch.first.back().triLen = totalIndices/3 - newBatch.first.back().triStart;
            }
            batches.push_back(newBatch);
            batches.back().second->Build();
        }
    }

    void Renderer2D::RenderBatch(batch& b)
    {
        if(!b.second->Empty())
        {
            shad->Bind();
            b.second->BeginDraw();
            for(std::vector<vbo_tex_order>::iterator it=b.first.begin();it!=b.first.end();++it)
            {
                glClientActiveTexture(GL_TEXTURE0);
                it->tex->Bind();
                b.second->DrawElements(it->triLen, it->triStart);
                numPolys += it->triLen;
                drawCalls++;
            }
            b.second->EndDraw();
        }
    }

    void Renderer2D::Render()
    {
        Build();

        numPolys = 0;
        drawCalls = 0;

        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_BLEND);
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

        glEnable(GL_TEXTURE_2D);

        for(std::vector<batch>::iterator it=batches.begin();it!=batches.end();++it)
        {
            if(!it->second->Empty())
            {
                glBlendFunc(it->blendSFactor, it->blendDFactor);
                ProjectionInfo proj;
                proj = it->projCallback(screen);
                ChangeProjectionMatrix(proj);

                if(it->effect)
                    it->effect->StartFrame(proj);

                if(it->cameraCallback)
                {
                    glLoadMatrixf(const_cast<const GLfloat*>(it->cameraCallback().GetData()));
                }

                RenderBatch(*it);

                if(it->effect)
                    it->effect->EndFrame(proj);
            }
        }

        static int frames = 0;
        static int T0 = SORE_Timing::GetGlobalTicks();
        static int last;
        frames++;

        GLint t = SORE_Timing::GetGlobalTicks();

        ms = (static_cast<float>(t) - static_cast<float>(last)) / 10.0f;

        if (t - T0 >= 500) //calculate FPS every 50 milliseconds
        {
            GLfloat seconds = (GLfloat)((t - T0) / 10000.0);
            fps = frames / seconds;
            T0 = t;
            frames = 0;
        }
        last = t;

        PrintGLErrors(SORE_Logging::LVL_ERROR);
    }

    float Renderer2D::GetFPS() const
    {
        return fps;
    }

    float Renderer2D::GetFrameMS() const
    {
        return ms;
    }

    unsigned int Renderer2D::GetDrawCalls() const
    {
        return drawCalls;
    }

    unsigned int Renderer2D::GetNumPolys() const
    {
        return numPolys;
    }

    void Renderer2D::ClearGeometryProviders()
    {
        currentProvider->clear();
    }

    void Renderer2D::AddGeometryProvider(geometry_provider c)
    {
        currentProvider->push_back(c);
    }

    void Renderer2D::PushState()
    {
        std::vector<geometry_provider> temp;
        geometryProviders.push_back(temp);
        currentProvider = geometryProviders.end() - 1;
    }

    void Renderer2D::PopState()
    {
        geometryProviders.pop_back();
        currentProvider = geometryProviders.end() - 1;
    }

    inline int TextureSort(const GeometryChunk* one, const GeometryChunk* two)
    {
        if(one->GetTexture()->GetHandle() < two->GetTexture()->GetHandle()) return SORT_LESS;
        else if(one->GetTexture() == two->GetTexture()) return SORT_EQUAL;
        else return SORT_GREATER;
    }

    bool GeometrySort(std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> one,
                      std::pair<const SORE_Math::Matrix4<float>*, const GeometryChunk*> two)
    {
        if( !one.second->IsOpaque() && two.second->IsOpaque()) return true;
        if( !two.second->IsOpaque() && one.second->IsOpaque()) return false;
        if(!one.second->IsOpaque())
        {
            if(one.first->GetData()[14] < two.first->GetData()[14])
                return true;
            else if(two.first->GetData()[14] < one.first->GetData()[14])
                return false;
        }
        if(TextureSort(one.second, two.second) == SORT_LESS) return true;
        return false;
    }
}
