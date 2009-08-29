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
//$Id$

#include "sore_textwidget.h"

namespace SORE_GUI
{
	TextWidget::TextWidget(SVec p, SORE_Font::Font& f, unsigned int h, const std::string& t, 
												 const SORE_Graphics::Color& c, Widget* parent)
		: Widget(SVec(SUnit(), SUnit()), p, parent), face(f), text(f, h, t), color(c), height(h)
	{
		SetSize(SVec(SUnit(0.0, text.GetWidth()), SUnit(0.0, text.GetHeight())));
		UpdateCache();
	}

	void TextWidget::UpdateText(const std::string& t)
	{
		text.UpdateText(t, color);
		UpdateCache();
	}

	void TextWidget::UpdateCache()
	{
		transforms.clear();
		transforms.reserve(text.GetLength());
		all.clear();
		SORE_Graphics::render_list raw = text.GetGeometry();

		SORE_Graphics::render_list::iterator it;
		for(it = raw.begin(); it != raw.end(); ++it)
		{
			transforms.push_back(*(it->first) * GetPositionMatrix());
			all.push_back(std::make_pair(&transforms.back(), it->second));
		}
	}

	SORE_Graphics::render_list TextWidget::GetThisRenderList()
	{
		return all;
	}

	bool TextWidget::ProcessEvents(SORE_Kernel::Event* e)
	{
		return false;
	}

	void TextWidget::OnGLReload()
	{
		text.UpdateText(text.GetText(), color);
		UpdateCache();
	}
}
