/*
  Untitled Project
  Flatland-inspired RTS project code. Created by Adam Kidder.
  Licensing currently undecided; view as proprietary code.
*/
//$Id$

#include "sore_scenegraph.h"

namespace SORE_Graphics
{
	SORE_Math::Vector3<float> SceneGraph::defaultPos = SORE_Math::Vector3<float>(0.0f, 0.0f, 0.0f);

	SceneGraph::SceneGraph()
	{
		shared_ptr<SceneNode> p(new SceneNode(NULL));
		parent = p;
	}

	SceneGraph::~SceneGraph()
	{
	}

	render_list SceneGraph::GetRenderList()
	{
		parent->UpdateCache(view);
		render_list all;
	
		parent->AddToRenderList(all);
	
		return all;
	}

	shared_ptr<SceneNode> SceneGraph::AddNode(GeometryChunk * gc, shared_ptr< SceneNode > parent, SORE_Math::Vector3< float > pos)
	{
		shared_ptr<SceneNode> newNode(new SceneNode(parent, gc));
		if(parent == NULL)
			this->parent->AddChild(newNode);
		else
			parent->AddChild(newNode);
		newNode->Translate(pos[0], pos[1], pos[2]);
		return newNode;
	}

	shared_ptr< SceneNode > SceneGraph::AddNode(GeometryChunk * gc, SORE_Math::Vector3< float > pos)
	{
		shared_ptr<SceneNode> newNode(new SceneNode(parent, gc));
		parent->AddChild(newNode);
		newNode->Translate(pos[0], pos[1], pos[2]);
		//if(gc!=NULL)
		//	flatList.push_back(gc);
		return newNode;
	}

	SORE_Math::Matrix4< float > & SceneGraph::GetViewMatrix()
	{
		return view;
	}
}
