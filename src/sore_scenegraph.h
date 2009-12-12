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

#ifndef SORE_SCENEGRAPH_H
#define SORE_SCENEGRAPH_H

#include <vector>

#include "math/sore_matrix4x4.h"
#include "math/sore_vector3.h"
#include "sore_geometryprovider.h"
#include "sore_renderable.h"
#include "sore_scenenode.h"

namespace SORE_Graphics
{
    const static ProjectionInfo defaultProjection(-1.0f, 1.0f);

    class SORE_EXPORT SceneGraph : public GeometryProvider, boost::noncopyable
    {
    public:
        SceneGraph(
            geometry_layer l = LAYER5, ProjectionInfo proj = defaultProjection);
        ~SceneGraph();

        node_list::iterator AddNode(Renderable r);

        virtual void MakeUpToDate();
        virtual std::vector<Renderable>::iterator GeometryBegin();
        virtual std::vector<Renderable>::iterator GeometryEnd();

        //A geometry provider can optionally set a projection for any layer
        virtual bool HasProjection(geometry_layer layer);
        virtual ProjectionInfo GetProjection(geometry_layer layer);

        SceneNode& GetParent();

    private:
        SceneNode* parent;
        std::vector<Renderable> renderList;

        geometry_layer layer;
        ProjectionInfo projection;
    };
}
#endif
