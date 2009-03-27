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

#ifndef SORE_BUTTON_H
#define SORE_BUTTON_H

#include <boost/function.hpp>

#include "../sore_resource.h"
#include "../sore_font.h"
#include "../sore_text.h"
#include "sore_framewidget.h"

namespace SORE_GUI
{
	class Button : public FrameWidget
	{
	public:
		Button(SVec s, SVec p, const std::string& text,
					 SORE_Resource::ResourcePool& pool, Widget* par=NULL);
		~Button();

		void SetCallback(boost::function<void ()> c);
	private:
		virtual SORE_Graphics::render_list GetThisRenderList();
		bool ProcessEvents(SORE_Kernel::Event* e);
		void UpdatePosition();

		SORE_Font::Font* font;
		SORE_Graphics::Text* t;
		SORE_Resource::Texture2D* normal, *active, *hover;

		boost::function<void ()> callback;

		bool pressed;
		bool inArea;
	};
}

#endif
