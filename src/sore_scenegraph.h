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
//$Id$

#ifndef SORE_SCENEGRAPH_H
#define SORE_SCENEGRAPH_H

#include "sore_scenenode.h"
#include "sore_geometrychunk.h"
#include "math/sore_vector3.h"
#include "math/sore_matrix4x4.h"
#include <vector>

namespace SORE_Graphics
{
  class SceneGraph
  {
  public:
    SceneGraph();
    ~SceneGraph();
		
		node_list::iterator AddNode(GeometryChunk* gc = NULL, 
																SORE_Math::Vector3<float> pos=SORE_Math::zeroVector3f);

		SceneNode& GetParent();
		
    render_list GetRenderList();
  protected:
  private:
    SceneNode* parent;
  };
}
#endif
