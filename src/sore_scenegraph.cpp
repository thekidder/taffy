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

#include "sore_scenegraph.h"

namespace SORE_Graphics
{
    SceneGraph::SceneGraph(geometry_layer l, ProjectionInfo proj)
        : layer(l), projection(proj)
    {
        parent = new SceneNode(Renderable());
    }

    SceneGraph::~SceneGraph()
    {
        delete parent;
    }

    void SceneGraph::MakeUpToDate()
    {
        renderList.clear();
        parent->AddToRenderList(renderList);
    }

    std::vector<Renderable>::iterator SceneGraph::GeometryBegin()
    {
        return renderList.begin();
    }

    std::vector<Renderable>::iterator SceneGraph::GeometryEnd()
    {
        return renderList.end();
    }

    bool SceneGraph::HasProjection(geometry_layer layer)
    {
        if(layer == this->layer)
            return true;
        return false;
    }

    ProjectionInfo SceneGraph::GetProjection(geometry_layer layer)
    {
        if(layer != this->layer)
            return ProjectionInfo();
        return projection;
    }

    SceneNode& SceneGraph::GetParent()
    {
        return *parent;
    }

    node_list::iterator SceneGraph::AddNode(Renderable r)
    {
        return parent->AddChild(r);
    }
}
