/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * modification, are permitted provided that the following conditions     *
 * are met:                                                               *
 *                                                                        *
 *    1. Redistributions of source code must retain the above             *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer.                                                      *
 *                                                                        *
 *    2. Redistributions in binary form must reproduce the above          *
 *       copyright notice, this list of conditions and the following      *
 *       disclaimer in the documentation and/or other materials           *
 *       provided with the distribution.                                  *
 *                                                                        *
 * THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,    *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR     *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY    *
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           *
 * (INCLUDING ENGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  *
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.   *
 *                                                                        *
 * The views and conclusions contained in the software and documentation  *
 * are those of the authors and should not be interpreted as              *
 * representing official policies, either expressed or implied, of        *
 * Adam Kidder.                                                           *
 **************************************************************************/

#ifndef SORE_WIDGET_H
#define SORE_WIDGET_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <vector>

#include <sore_matrix4x4.h>
#include <sore_dll.h>
#include <sore_geometrychunk.h>
#include <sore_input.h>
#include <sore_renderable.h>

#include <sore_units.h>

namespace SORE_GUI
{
    const static float LAYER_SEPARATION = 0.0005f;

    enum unit_type {HORIZONTAL, VERTICAL};

    class SORE_EXPORT Widget
    {
    public:
        Widget(SVec s, SVec p, Widget* par=NULL);
        ~Widget();

        std::vector<SORE_Graphics::Renderable> GetRenderList();
        bool PropagateEvents(SORE_Kernel::Event* e);

        int GetSize(unit_type type) const;
        int GetPosition(unit_type type) const;

        void SetPosition(const SVec& p);

        bool HasFocus() const;

        static void SetStyle(std::string style);
        static std::string GetStyle();

        void SetVisible(bool visible = true);
    protected:
        //returns true if inside widget area
        bool InBounds(unsigned int x, unsigned int y);
        //returns total size of area for child widgets
        virtual unsigned int GetClientSize(unit_type type) const;
        //returns position of area for child widgets
        virtual unsigned int GetClientPosition(unit_type type) const {return 0; };

        const SVec& GetPosition() const;

        const SVec& GetSize() const;
        void SetSize(const SVec& s);

        //translate an SVec into pixels
        int GetPixels(unit_type type, SUnit unit) const;
        float GetLayer() const; //used for child widgets to determine their layer
        const SORE_Math::Matrix4<float>& GetPositionMatrix() const;

        void ClearFocus();
        float GetTopLayer();
    private:
        //these events are preprocessed: mouse coordinates are relative to the widget,
        //not absolute
        virtual bool ProcessEvents(SORE_Kernel::Event* e) = 0;
        //child widgets ARE responsible to transforming their geometry
        //via GetPositionMatrix()
        virtual std::vector<SORE_Graphics::Renderable> GetThisRenderList() = 0;
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

        //render/process inputs from this widget?
        bool isVisible;
    };

    const static SORE_Graphics::geometry_layer GUI_LAYER = SORE_Graphics::LAYER3;
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
