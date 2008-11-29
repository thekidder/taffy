/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id: scenenode.cpp 23 2008-10-08 02:14:59Z me $

#include "sore_scenenode.h"
#include "sore_logger.h"

namespace SORE_Graphics
{

  SceneNode::SceneNode(weak_ptr<SceneNode> p, GeometryChunk* g) : geometry(g), parent(p)
  {
  }

  SceneNode::SceneNode(GeometryChunk* g) : geometry(g)
  {
  }


  SceneNode::~SceneNode()
  {
  }

  const GeometryChunk * SceneNode::GetChunk() const
  {
    return geometry;
  }

  const std::vector< shared_ptr<SceneNode> > & SceneNode::GetChildren() const
  {
    return children;
  }

  const SORE_Math::Matrix4< float > & SceneNode::GetTransform() const
  {
    return mat;
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

  void SceneNode::AddChild(shared_ptr< SceneNode > c)
  {
    children.push_back(c);
  }

  void SceneNode::InvalidateCache()
  {
    cacheUpdated = false;
    for(std::vector<shared_ptr<SceneNode> >::iterator it=children.begin();it!=children.end();++it)
      it->get()->InvalidateCache();
  }

  void SceneNode::UpdateCache(SORE_Math::Matrix4<float>& view)
  {
    if(!cacheUpdated)
      {
	if(shared_ptr<SceneNode> p = parent.lock())
	  {
	    if(!p->cacheUpdated)
	      p->UpdateCache(view);
	    cachedAbsoluteTransform = p.get()->cachedAbsoluteTransform * mat;
	    cachedViewTransform     = p.get()->cachedViewTransform     * mat;
	  }
	else
	  {
	    //ENGINE_LOG(SORE_Logging::LVL_ERROR, "Could not get parent pointer for updating cached matrix");
	    cachedViewTransform = view * mat;
	  }
	cacheUpdated = true;
      }
    for(std::vector<shared_ptr<SceneNode> >::iterator it=children.begin();it!=children.end();++it)
      it->get()->UpdateCache(view);
  }

  void SceneNode::AddToRenderList(render_list & list)
  {
    if(geometry)
      list.push_back(std::make_pair(&cachedAbsoluteTransform, geometry));
    for(std::vector<shared_ptr<SceneNode> >::iterator it=children.begin();it!=children.end();++it)
      it->get()->AddToRenderList(list);
  }
}
