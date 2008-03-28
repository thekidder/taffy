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

#ifndef  __SORE_2DSCENEGRAPH__
#define  __SORE_2DSCENEGRAPH__

#include "sore_2dprimitives.h"
#include <list>
#include <string>

namespace SORE_Graphics
{
	typedef std::map<std::string, Sprite2D>::iterator sprite_ref;
	
	class SceneGraph2D
	{
		public:
			SceneGraph2D();
			~SceneGraph2D();
			
			std::vector<Sprite2D*> GetRenderList();
		
			sprite_ref AddSprite     (std::string name, Sprite2D& s);	
			void       ReplaceSprite (sprite_ref oldSprite, Sprite2D newSprite);
			void       ReplaceSprite (std::string name, Sprite2D newSprite);
			Sprite2D*  GetSprite     (sprite_ref id);
			Sprite2D*  GetSprite     (std::string name);
			void       RemoveSprite  (std::string name);
			void       RemoveSprite  (sprite_ref id);
			
			void       Clear         ();
			
		protected:
			std::map<std::string, Sprite2D> nodes;
	};
}

#endif /*__SORE_2DSCENEGRAPH__*/
