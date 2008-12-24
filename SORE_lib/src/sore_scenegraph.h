/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id$

#ifndef SCENEGRAPH_H_
#define SCENEGRAPH_H_

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
		
    shared_ptr<SceneNode> AddNode(GeometryChunk* gc, shared_ptr<SceneNode> parent, SORE_Math::Vector3<float> pos=defaultPos);
    shared_ptr<SceneNode> AddNode(GeometryChunk* gc, SORE_Math::Vector3<float> pos=defaultPos);
		
    render_list GetRenderList();
    SORE_Math::Matrix4<float>& GetViewMatrix();
  protected:
  private:
		static SORE_Math::Vector3<float> defaultPos;;

    shared_ptr<SceneNode> parent;
    //std::vector<GeometryChunk*> flatList;
    SORE_Math::Matrix4<float> view;
  };
}
#endif
