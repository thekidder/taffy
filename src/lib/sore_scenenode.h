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

#ifndef SORE_SCENENODE_H
#define SORE_SCENENODE_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <list>

#include <sore_matrix4x4.h>
#include <sore_vector3.h>
#include <sore_dll.h>
#include <sore_noncopyable.h>
#include <sore_renderable.h>

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
