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

#ifndef SORE_SCENENODE_H
#define SORE_SCENENODE_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <list>

#include "math/sore_matrix4x4.h"
#include "math/sore_vector3.h"
#include "sore_dll.h"
#include "sore_noncopyable.h"
#include "sore_renderable.h"

namespace SORE_Graphics
{
    class SORE_EXPORT SceneNode;

    typedef std::list<SceneNode*> node_list;

    /**
       @author Adam Kidder <thekidder@gmail.com>
    */
    class SORE_EXPORT SceneNode : SORE_Utility::Noncopyable
    {
    public:
        SceneNode(Renderable r);
        ~SceneNode();

        const node_list& GetChildren() const;
        node_list::iterator AddChild(Renderable r);
        void RemoveChild(node_list::iterator it);

        const SORE_Math::Matrix4<float>& GetTransform() const;

        void SetGeometry(Renderable r);

        void Translate(float x, float y, float z);
        void Rotate(float rad, unsigned int axis);
        void SetIdentity();

        void UpdateCache(SceneNode* parent);
        void AddToRenderList(std::vector<Renderable>& list);
    protected:
        void InvalidateCache();
    private:
        Renderable geometry;
        node_list children;

        //transformations
        SORE_Math::Matrix4<float> cachedAbsoluteTransform;
        bool cacheUpdated;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
