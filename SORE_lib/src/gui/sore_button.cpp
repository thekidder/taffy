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

#include <boost/lexical_cast.hpp>

#include "sore_button.h"

namespace SORE_GUI
{
	Button::Button(SVec s, SVec p, const std::string& text,
								 SORE_Resource::ResourcePool& pool, Widget* par) 
		: FrameWidget(s, p, SCALE_ALL, par), normal(0), active(0), hover(0), pressed(false), inArea(false)
	{
		normal = 
			pool.GetResource<SORE_Resource::Texture2D>("data/Textures/button_sheet_normal.tga");
		active = 
			pool.GetResource<SORE_Resource::Texture2D>("data/Textures/button_sheet_active.tga");
		hover =
			pool.GetResource<SORE_Resource::Texture2D>("data/Textures/button_sheet_hover.tga");

		unsigned int height = GetSize(VERTICAL);
		unsigned int width = GetSize(HORIZONTAL);

		unsigned int textHeight = height / 2;
		if(textHeight < 16) textHeight = 16;

		font = pool.GetResource<SORE_Font::Font>("LiberationSans-Regular.ttf");
		t = new SORE_Graphics::Text(*font, textHeight, text);
		if(t->GetWidth() >= (width-16))
		{
			float factor = static_cast<float>(t->GetWidth()) / (width - 16);
			unsigned int newHeight = static_cast<unsigned int>(static_cast<float>(textHeight) / factor);
			font = pool.GetResource<SORE_Font::Font>("LiberationSans-Regular.ttf");
			delete t;
			t = new SORE_Graphics::Text(*font, newHeight, text);
		}

		UpdatePosition();
		SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
		SetTexture(normal);
	}

	void Button::UpdatePosition()
	{
		unsigned int textStartV = (GetSize(VERTICAL) - t->GetHeight())/2;
		unsigned int textStartH = (GetSize(HORIZONTAL) - t->GetWidth())/2;
		t->SetTransform(GetPositionMatrix() * SORE_Math::Matrix4<float>::GetTranslation(static_cast<float>(textStartH), 
			static_cast<float>(textStartV), 
			0.0f));
	}

	Button::~Button()
	{
		delete t;
	}

	SORE_Graphics::render_list Button::GetThisRenderList()
	{
		SORE_Graphics::render_list list;

		std::vector<SORE_Graphics::GeometryChunk*> frame = GetChunks();
		std::vector<SORE_Graphics::GeometryChunk*>::iterator it;
		for(it = frame.begin();it!=frame.end();++it)
		{
			list.push_back(std::make_pair(&GetPositionMatrix(), *it));
		}

		SORE_Graphics::render_list text = t->GetGeometry();
		list.insert(list.begin(), text.begin(), text.end());

		return list;
	}

	void Button::SetCallback(boost::function<void ()> c)
	{
		callback = c;
	}

	bool Button::ProcessEvents(SORE_Kernel::Event* e)
	{
		if(e->type == SORE_Kernel::MOUSEENTER)
		{
			inArea = true;
			if(!pressed)
				SetTexture(hover);
			return true;
		}
		else if(e->type == SORE_Kernel::MOUSELEAVE)
		{
			inArea = false;
			if(!pressed)
				SetTexture(normal);
			return true;
		}
		else if(e->type == SORE_Kernel::MOUSEBUTTONDOWN)
		{
			pressed = true;
			t->SetTransform(t->GetTransform() * 
											SORE_Math::Matrix4<float>::GetTranslation(2.0, 2.0, 0.0f));
			SetTexture(active);
			return true;
		}
		else if(e->type == SORE_Kernel::MOUSEBUTTONUP)
		{
			pressed = false;
			t->SetTransform(t->GetTransform() * 
											SORE_Math::Matrix4<float>::GetTranslation(-2.0, -2.0, 0.0f));
			if(inArea)
				SetTexture(hover);
			else
				SetTexture(normal);
			if(callback)
				callback();
			return true;
		}
		return false;
	}
}
