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
 * THIS SOFTWARE IS PROVIDED BY ADAM KIDDER ``AS IS'' AND ANY             *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL ADAM KIDDER OR               *
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

#include <boost/ptr_container/ptr_vector.hpp>
#include <json/json.h>

#include <sore_dll.h>
#include <sore_event.h>
#include <sore_fileio.h>
#include <sore_glslshader_loader.h>
#include <sore_immediatemodeprovider.h>
#include <sore_matrix4x4.h>
#include <sore_units.h>

namespace SORE_GUI
{
    const static float LAYER_SEPARATION = 0.05f;

    enum unit_type {HORIZONTAL, VERTICAL};

    class SORE_EXPORT Widget
    {
    public:
        Widget(SVec size_, SVec position_, Widget* parent_);
        Widget(
            SORE_Resource::Font_cache_t& fontCache_,
            SORE_Resource::Shader_cache_t& shaderCache_,
            SORE_Resource::Texture_cache_t& textureCache_,
            SVec size_, SVec position_, 
            Widget* parent_ = NULL);
        ~Widget();

        std::vector<SORE_Graphics::Renderable> GetRenderList();
        bool PropagateEvents(const SORE_Kernel::Event& e);

        // return absolute values - in screen units
        int GetSize(unit_type type) const;
        int GetPosition(unit_type type) const;

        int GetRelativePosition(unit_type type) const;

        void SetPosition(const SVec& p);

        bool HasFocus() const;

        void SetStyleName(const std::string& style_);
        const std::string& StyleName() { return style; }

        void LoadStyle(SORE_FileIO::InFile& file);

        void SetVisible(bool visible = true);

        void RemoveChild(Widget* c);
        void RemoveChildren();

        const SORE_Math::Matrix4<float>& GetPositionMatrix() const;
    protected:
        // perform all logic updates and render
        void Frame(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode); 

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
        

        void ClearFocus();
        float GetTopLayer();

        const Json::Value& Style() { return styleConfig[style]; }

        SORE_Resource::Font_cache_t& fontCache;
        SORE_Resource::Shader_cache_t& shaderCache;
        SORE_Resource::Texture_cache_t& textureCache;
    private:
        //these events are preprocessed: mouse coordinates are relative to the widget,
        //not absolute
        virtual bool ProcessEvents(const SORE_Kernel::Event& e) { return false; };
        //child widgets ARE responsible to transforming their geometry
        //via GetPositionMatrix()
        virtual void UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode) {};
        //gets called when the parent's position is updated; overload if we need to do
        //processing on a position change
        virtual void UpdatePosition() {}
        //gets called when the widget loses focus
        virtual void FocusLost() {}
        bool PropagateEventHelper(const SORE_Kernel::Event& e, const SORE_Kernel::Event& p);
        void AddChild(Widget* c);
        void UpdatePositionMatrix();

        SORE_Kernel::Event prev;


        //position matrix
        SORE_Math::Matrix4<float> mat;
        float layer;

        SVec position;
        SVec size;
        Widget* parent;
        typedef boost::ptr_vector<Widget> Widget_container_t;
        Widget_container_t children;

        const Widget* Focus() const;
        Widget*& Focus();
        Widget*& OldFocus();
        void ChangeFocus(Widget* w);
        Widget* focus;
        Widget* oldFocus;

        float& HighestLayer();
        float highestLayer;

        std::string style;
        Json::Value styleConfig;

        //render/process inputs from this widget?
        bool isVisible;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
