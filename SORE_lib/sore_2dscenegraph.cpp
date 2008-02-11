/***************************************************************************
 *   Copyright (C) 2007 by Adam Kidder   *
 *   thekidder@gmail.com   *
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
// $Id$

#include "sore_2dscenegraph.h"

namespace SORE_Graphics
{
	SceneGraph2D::SceneGraph2D()
	{
	}
	
	SceneGraph2D::~SceneGraph2D()
	{
	}
	
	std::vector<Sprite2D*> SceneGraph2D::GetRenderList()
	{
		std::vector<Sprite2D*> temp;
		for(sprite_ref i=nodes.begin();i!=nodes.end();i++)
		{
			temp.push_back(&(i->second));
		}
		return temp;
	}
	
	sprite_ref SceneGraph2D::AddSprite(std::string name, Sprite2D& s)
	{
		nodes.insert(std::pair<std::string, Sprite2D>(name,s));
		sprite_ref end = nodes.end();
		end--;
		return end;
	}
			
	void SceneGraph2D::ReplaceSprite(sprite_ref oldSprite, Sprite2D newSprite)
	{
		oldSprite->second = newSprite;
	}
	
	void SceneGraph2D::ReplaceSprite(std::string name, Sprite2D newSprite)
	{
		if(nodes.find(name)==nodes.end())
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "Trying to replace a sprite that doesn't exist");
		nodes[name] = newSprite;
	}
	
	Sprite2D* SceneGraph2D::GetSprite(sprite_ref id)
	{
		return &(id->second);
	}
	
	Sprite2D* SceneGraph2D::GetSprite(std::string name)
	{
		if(nodes.find(name)==nodes.end())
			ENGINE_LOG(SORE_Logging::LVL_WARNING, "Trying to return a sprite that doesn't exist");
		return &nodes[name];
	}
}
