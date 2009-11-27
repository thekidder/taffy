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
    SceneNode::SceneNode(GeometryChunk* g) : geometry(g)
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

    const GeometryChunk * SceneNode::GetChunk() const
    {
        return geometry;
    }

    const node_list & SceneNode::GetChildren() const
    {
        return children;
    }

    const SORE_Math::Matrix4< float > & SceneNode::GetTransform() const
    {
        return mat;
    }

    void SceneNode::SetGeometry(GeometryChunk* g)
    {
        geometry = g;
    }

    void SceneNode::Translate(float x, float y, float z)
    {
        mat.Translate(x, y, z);
        InvalidateCache();
    }

    void SceneNode::Rotate(float rad, unsigned int axis)
    {
        mat.Rotate(rad, axis);
        InvalidateCache();
    }

    void SceneNode::SetIdentity()
    {
        mat.SetIdentity();
        InvalidateCache();
    }

    node_list::iterator SceneNode::AddChild(GeometryChunk* gc, SORE_Math::Vector3<float> pos)
    {
        SceneNode* c = new SceneNode(gc);
        c->Translate(pos[0], pos[1], pos[2]);
        return children.insert(children.end(), c);
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

                cachedAbsoluteTransform = parent->cachedAbsoluteTransform * mat;
            }
            else
            {
                cachedAbsoluteTransform = mat;
            }
            cacheUpdated = true;
        }
        for(node_list::iterator it=children.begin();it!=children.end();++it)
            (*it)->UpdateCache(this);
    }

    /*void SceneNode::AddToRenderList(render_list & list)
    {
        if(geometry)
        {
            list.push_back(std::make_pair(&cachedAbsoluteTransform, geometry));
        }
        for(node_list::iterator it=children.begin();it!=children.end();++it)
            (*it)->AddToRenderList(list);
            }*/
}
