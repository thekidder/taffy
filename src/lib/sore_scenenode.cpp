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

#include <sore_scenenode.h>
#include <sore_logger.h>

namespace SORE_Graphics
{
    SceneNode::SceneNode(Renderable r) : geometry(r)
    {
    }

    SceneNode::~SceneNode()
    {
        for(node_list::iterator it = children.begin(); it != children.end(); ++it)
        {
            delete *it;
        }
    }

    void SceneNode::RemoveChild(node_list::iterator it)
    {
        SceneNode* s = *it;
        delete s;
        *it = 0;
        children.erase(it);
    }

    const node_list & SceneNode::GetChildren() const
    {
        return children;
    }

    const SORE_Math::Matrix4< float > & SceneNode::GetTransform() const
    {
        return *geometry.GetTransform();
    }

    void SceneNode::SetGeometry(Renderable r)
    {
        geometry = r;
    }

    void SceneNode::Translate(float x, float y, float z)
    {
        geometry.GetTransform()->Translate(x, y, z);
        InvalidateCache();
    }

    void SceneNode::Rotate(float rad, unsigned int axis)
    {
        geometry.GetTransform()->Rotate(rad, axis);
        InvalidateCache();
    }

    void SceneNode::SetIdentity()
    {
        geometry.GetTransform()->SetIdentity();
        InvalidateCache();
    }

    node_list::iterator SceneNode::AddChild(Renderable r)
    {
        SceneNode* c = new SceneNode(r);
        return children.insert(children.end(), c);
    }

    void SceneNode::AddToRenderList(std::vector<Renderable>& list)
    {
        if(geometry.GetGeometryChunk())
            list.push_back(geometry);
        for(node_list::iterator it=children.begin();it!=children.end();++it)
            (*it)->AddToRenderList(list);
    }

    void SceneNode::InvalidateCache()
    {
        cacheUpdated = false;
        for(node_list::iterator it=children.begin();it!=children.end();++it)
            (*it)->InvalidateCache();
    }

    void SceneNode::UpdateCache(SceneNode* parent)
    {
        if(!cacheUpdated)
        {
            if(parent)
            {
                if(!parent->cacheUpdated)
                    ENGINE_LOG(SORE_Logging::LVL_WARNING,
                               "Trying to update node with out-of-date parent");

                cachedAbsoluteTransform =
                    parent->cachedAbsoluteTransform * *geometry.GetTransform();
            }
            else
            {
                cachedAbsoluteTransform = *geometry.GetTransform();
            }
            cacheUpdated = true;
        }
        for(node_list::iterator it=children.begin();it!=children.end();++it)
            (*it)->UpdateCache(this);
    }
}
