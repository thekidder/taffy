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

#ifndef SORE_WIDGET_H
#define SORE_WIDGET_H

#include <vector>

#include "../sore_geometrychunk.h"
#include "../sore_input.h"

#include "sore_units.h"

namespace SORE_GUI
{
	const static float LAYER_SEPARATION = 0.0001f;

	enum unit_type {HORIZONTAL, VERTICAL};

	class Widget
	{
	public:
		Widget(SVec s, SVec p, Widget* par=NULL);
		~Widget();

		SORE_Graphics::render_list GetRenderList();
		bool PropagateEvents(SORE_Kernel::Event* e);

		int GetSize(unit_type type) const;
		int GetPosition(unit_type type) const;

		bool HasFocus() const;

        static void SetStyle(std::string style);
        static std::string GetStyle();
	protected:
		//returns true if inside widget area
		bool InBounds(unsigned int x, unsigned int y);
		//returns total size of area for child widgets
 		virtual unsigned int GetClientSize(unit_type type) const {return GetSize(type); };
		//returns position of area for child widgets
		virtual unsigned int GetClientPosition(unit_type type) const {return 0; };

		const SVec& GetPosition() const;
		void SetPosition(const SVec& p);

		const SVec& GetSize() const;
		void SetSize(const SVec& s);

		int GetPixels(unit_type type, SUnit unit) const; //translate an SVec into pixels
		float GetLayer() const; //used for child widgets to determine their layer
		const SORE_Math::Matrix4<float>& GetPositionMatrix() const;

		void ClearFocus();
		float GetTopLayer();
	private:
		//these events are preprocessed: mouse coordinates are relative to the widget, 
		//not absolute
		virtual bool ProcessEvents(SORE_Kernel::Event* e) = 0; 
		//child widgets ARE responsible to transforming their geometry via GetPositionMatrix()
		virtual SORE_Graphics::render_list GetThisRenderList() = 0;
		//gets called when the parent's position is updated; overload if we need to do
		//processing on a position change
		virtual void UpdatePosition() {} 
		//gets called when the widget loses focus
		virtual void FocusLost() {}
		//overload to perform any processing on GL context reload (reload textures etc)
		virtual void OnGLReload() {}
		bool PropagateEventHelper(SORE_Kernel::Event* e, SORE_Kernel::Event* p);
		void PropagateGLReload();
		void AddChild(Widget* c);
		void RemoveChild(Widget* c);
		void UpdatePositionMatrix();

		SORE_Kernel::Event prev;

		//position matrix
		SORE_Math::Matrix4<float> mat;
		float layer;
		std::vector<Widget*> children;
		Widget* parent;
		SVec position;
		SVec size;

		const Widget* Focus() const;
		Widget*& Focus();
		Widget*& OldFocus();
		void ChangeFocus(Widget* w);
		Widget* focus;
		Widget* oldFocus;

		float& HighestLayer();
		float highestLayer;

        static std::string style;
	};
}

#endif
