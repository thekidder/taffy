/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id$

#ifndef SCENENODE_H
#define SCENENODE_H

#include "sore_geometrychunk.h"
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

using boost::shared_ptr;
using boost::weak_ptr;

namespace SORE_Graphics
{
  /**
     @author Adam Kidder <thekidder@gmail.com>
  */
  class SceneNode
  {
  public:
    SceneNode(weak_ptr<SceneNode> p, GeometryChunk* g = NULL);
    SceneNode(GeometryChunk* g = NULL); //root-level
    ~SceneNode();
		
    const GeometryChunk* GetChunk() const;
    const std::vector<shared_ptr<SceneNode> >& GetChildren() const;
    void AddChild(shared_ptr<SceneNode> c);
		
    const SORE_Math::Matrix4<float>& GetTransform() const;
		
		void SetGeometry(GeometryChunk* g);

    void Translate(float x, float y, float z);
    void Rotate(float rad, unsigned int axis);
		void SetIdentity();
		
    void UpdateCache(SORE_Math::Matrix4<float>& view);
    void AddToRenderList(render_list& list);
  protected:
    void InvalidateCache();
  private:
    GeometryChunk* geometry;
    std::vector<shared_ptr<SceneNode> > children;
    weak_ptr<SceneNode> parent;
		
    //transformations
    SORE_Math::Matrix4<float> mat;
    SORE_Math::Matrix4<float> cachedAbsoluteTransform;
    SORE_Math::Matrix4<float> cachedViewTransform;
    bool cacheUpdated;
  };
}
#endif
