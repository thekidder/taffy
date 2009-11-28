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

#include "sore_scenenode.h"
#include "sore_logger.h"

namespace SORE_Graphics
{
    SceneNode::SceneNode(Renderable r) : geometry(r)
    {
    }

    SceneNode::~SceneNode()
    {
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
