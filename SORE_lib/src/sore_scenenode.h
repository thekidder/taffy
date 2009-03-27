/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
//$Id$

#ifndef SORE_SCENENODE_H
#define SORE_SCENENODE_H

#include "sore_geometrychunk.h"
#include <list>

namespace SORE_Graphics
{
	class SceneNode;

	typedef std::list<SceneNode*> node_list;

  /**
     @author Adam Kidder <thekidder@gmail.com>
  */
  class SceneNode
  {
  public:
    SceneNode(GeometryChunk* g = NULL);
    ~SceneNode();
		
    const GeometryChunk* GetChunk() const;
    const node_list& GetChildren() const;
		node_list::iterator AddChild(GeometryChunk* gc = NULL, 
																 SORE_Math::Vector3<float> pos=SORE_Math::zeroVector3f);
		void RemoveChild(node_list::iterator it);

    const SORE_Math::Matrix4<float>& GetTransform() const;
		
		void SetGeometry(GeometryChunk* g);

    void Translate(float x, float y, float z);
    void Rotate(float rad, unsigned int axis);
		void SetIdentity();
		
    void UpdateCache(SceneNode* parent);
    void AddToRenderList(render_list& list);
  protected:
    void InvalidateCache();
  private:
    GeometryChunk* geometry;
    node_list children;
		
    //transformations
    SORE_Math::Matrix4<float> mat;
    SORE_Math::Matrix4<float> cachedAbsoluteTransform;
    bool cacheUpdated;
  };
}
#endif
