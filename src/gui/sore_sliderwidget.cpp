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

#include "../sore_logger.h"
#include "sore_sliderwidget.h"

namespace SORE_GUI
{
	SliderWidget::SliderWidget(SVec s, SVec p, int min, int max, SORE_Resource::ResourcePool& pool, Widget* par)
		: FrameWidget(s, p, SCALE_ALL, par), minimum(min), maximum(max), current((max -  min)/2), dragged(false), sliderChunk(0)
	{
        std::string styleDir("data/Textures/");
        styleDir += GetStyle() + "/";

		bg = pool.GetResource<SORE_Resource::Texture2D>(styleDir + "slider_bg.tga");
		slider = pool.GetResource<SORE_Resource::Texture2D>(styleDir + "slider.tga");

		UpdatePosition();
		SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
		SetTexture(bg);
	}

	SliderWidget::~SliderWidget()
	{
		delete sliderChunk;
	}

	void SliderWidget::ConnectChange(boost::function<void (int)> c)
	{
		onChange.connect(c);
	}

	int SliderWidget::GetValue() const
	{
		return current;
	}

	void SliderWidget::SetValue(int value)
	{
		value = value < minimum ? minimum : value;
		value = value > maximum ? maximum : value;
		current = value;
		UpdateSlider();
		onChange(value);
	}

	SORE_Graphics::render_list SliderWidget::GetThisRenderList()
	{
		SORE_Graphics::render_list list;

		std::vector<SORE_Graphics::GeometryChunk*> frame = GetChunks();
		std::vector<SORE_Graphics::GeometryChunk*>::iterator it;
		for(it = frame.begin();it!=frame.end();++it)
		{
			list.push_back(std::make_pair(&GetPositionMatrix(), *it));
		}

		list.push_back(std::make_pair(&sliderMat, sliderChunk));

		return list;
	}

	bool SliderWidget::ProcessEvents(SORE_Kernel::Event* e)
	{
		float pos = ValueToX(current);
		switch(e->type)
		{
		case SORE_Kernel::MOUSEBUTTONDOWN:
			if(e->mouse.x < pos || e->mouse.x > pos+GetSize(VERTICAL)/8.0f)
			{
				SetValue(XToValue(static_cast<float>(e->mouse.x)));
			}
			dragged = true;
			return true;
			break;
		case SORE_Kernel::MOUSEBUTTONUP:
			dragged = false;
			return true;
			break;
		case SORE_Kernel::MOUSEMOVE:
			if(dragged)
			{
				SetValue(XToValue(static_cast<float>(e->mouse.x)));
				return true;
			}
			break;
		}
		return false;
	}

	void SliderWidget::UpdatePosition()
	{
		delete sliderChunk;
		float sliderHeight = static_cast<float>(GetSize(VERTICAL));
		float sliderWidth = sliderHeight / 8.0f;

		SORE_Math::Rect<float> sliderBounds(-sliderWidth, sliderWidth, 0, sliderHeight);
		sliderChunk = new SORE_Graphics::GeometryChunk(slider, sliderBounds);

		UpdateSlider();
	}

	void SliderWidget::UpdateSlider()
	{
		float pos = ValueToX(current);
		sliderMat = GetPositionMatrix() * SORE_Math::Matrix4<float>::GetTranslation(pos, 0.0f, GetLayer() + LAYER_SEPARATION/2.0f);
	}

	float SliderWidget::ValueToX(int value) const
	{
		value = value < minimum ? minimum : value;
		value = value > maximum ? maximum : value;
		return static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum) * (GetSize(HORIZONTAL) - 32.0f) + 16.0f;
	}

	int SliderWidget::XToValue(float x) const
	{
		int range = maximum - minimum;
		int value =  static_cast<int>( (x - 16.0f) / (GetSize(HORIZONTAL) - 32.0f) * range - minimum);
		value = value < minimum ? minimum : value;
		value = value > maximum ? maximum : value;
		return value;
	}
}
