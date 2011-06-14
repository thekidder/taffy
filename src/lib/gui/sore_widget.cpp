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

#include <sore_logger.h>
#include <sore_widget.h>

#include <stdexcept>

namespace SORE_GUI
{
    Widget::Widget(SVec size_, SVec position_, Widget* parent_)
        : fontCache(parent_->fontCache), shaderCache(parent_->shaderCache), textureCache(parent_->textureCache),
        position(position_), size(size_), parent(parent_), 
        focus(0), oldFocus(0), highestLayer(0.0f), 
        style(parent->style), styleConfig(parent->styleConfig), isVisible(true)
    {
        parent->AddChild(this);
        layer = parent->GetLayer() + LAYER_SEPARATION;
           
        if(layer > HighestLayer()) HighestLayer() = layer;

        SetPosition(position);
        prev.type = SORE_Kernel::NOEVENT;

    }

    Widget::Widget(
            SORE_Resource::Font_cache_t& fontCache_,
            SORE_Resource::Shader_cache_t& shaderCache_,
            SORE_Resource::Texture_cache_t& textureCache_,
            SVec size_, SVec position_, 
            Widget* parent_)
        : fontCache(fontCache_), shaderCache(shaderCache_), textureCache(textureCache_),
        position(position_), size(size_), parent(parent_), 
        focus(0), oldFocus(0), highestLayer(0.0f), isVisible(true)
    {
        if(parent)
        {
            parent->AddChild(this);
            layer = parent->GetLayer() + LAYER_SEPARATION;
        }
        else
        {
            layer = 0.0f;
        }

        if(layer > HighestLayer()) HighestLayer() = layer;

        SetPosition(position);
        prev.type = SORE_Kernel::NOEVENT;
    }

    Widget::~Widget()
    {
        if(HasFocus())
            ClearFocus();

        children.clear();
    }


    const SVec& Widget::GetPosition() const
    {
        return position;
    }

    void Widget::SetPosition(const SVec& p)
    {
        position = p;
        UpdatePositionMatrix();
    }

    const SVec& Widget::GetSize() const
    {
        return size;
    }

    void Widget::SetSize(const SVec& s)
    {
        size = s;
        UpdatePositionMatrix();
    }

    unsigned int Widget::GetClientSize(unit_type type) const
    {
        return GetSize(type);
    };

    void Widget::UpdatePositionMatrix()
    {
        mat = SORE_Math::Matrix4<float>::GetTranslation(
            static_cast<float>(GetPosition(HORIZONTAL)),
            static_cast<float>(GetPosition(VERTICAL)),
            layer);
        UpdatePosition();
        for(Widget_container_t::iterator it=children.begin();it!=children.end();++it)
        {
            it->UpdatePositionMatrix();
            it->UpdatePosition();
        }
    }

    const SORE_Math::Matrix4<float>& Widget::GetPositionMatrix() const
    {
        return mat;
    }

    void Widget::Frame(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        if(isVisible)
        {
            UpdateAndRender(elapsed, imm_mode);

            Widget_container_t::iterator it;
            for(it = children.begin(); it != children.end(); ++it)
            {
                it->Frame(elapsed, imm_mode);
            }
        }
    }

    bool Widget::PropagateEvents(const SORE_Kernel::Event& e)
    {
        SORE_Kernel::Event previous = prev;
        prev = e;
        
        return PropagateEventHelper(e, previous);
    }

    bool Widget::InBounds(unsigned int x, unsigned int y)
    {
        unsigned int wPos = GetPosition(HORIZONTAL);
        unsigned int hPos = GetPosition(VERTICAL);

        unsigned int wEnd = GetSize(HORIZONTAL) + wPos;
        unsigned int hEnd = GetSize(VERTICAL) + hPos;

        if(x >= wPos && x <= wEnd && y >= hPos && y <= hEnd)
            return true;
        return false;
    }

    bool Widget::PropagateEventHelper(const SORE_Kernel::Event& e, const SORE_Kernel::Event& p)
    {
        if(!isVisible)
            return false;
        if(Focus() && Focus()->InBounds(e.mouse.x, e.mouse.y))
        {
            SORE_Kernel::Event relative = e;
            relative.mouse.x -= Focus()->GetPosition(HORIZONTAL);
            relative.mouse.y -= Focus()->GetPosition(VERTICAL);
            if(Focus()->ProcessEvents(relative))
                return true;
        }
        //if there is a mousedown, set focus accordingly
        if(e.type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            OldFocus() = Focus();
            Focus() = 0;
        }
        for(Widget_container_t::iterator it=children.begin();it!=children.end();++it)
        {
            SORE_Kernel::Event relative;
            relative = e;

            bool inWidget = it->InBounds(e.mouse.x, e.mouse.y);
            bool inPrevWidget = it->InBounds(p.mouse.x, p.mouse.y);

            if(inWidget && !inPrevWidget)
                relative.type = SORE_Kernel::MOUSEENTER;
            if(!inWidget && inPrevWidget)
                relative.type = SORE_Kernel::MOUSELEAVE;

            if(inWidget || relative.type == SORE_Kernel::MOUSEENTER
               || relative.type == SORE_Kernel::MOUSELEAVE || it->HasFocus())
            {
                relative.mouse.x -= it->GetPosition(HORIZONTAL);
                relative.mouse.y -= it->GetPosition(VERTICAL);

                bool accepted = it->PropagateEventHelper(relative, p);

                if(accepted)
                {
                    if(!Focus()) ChangeFocus(this);
                    return true;
                }
            }
        }
        if(!Focus()) ChangeFocus(this);
        bool toReturn = ProcessEvents(e);
        return toReturn;
    }

    void Widget::ChangeFocus(Widget* w)
    {
        if(OldFocus())
            OldFocus()->FocusLost();
        Focus() = w;
    }

    void Widget::ClearFocus()
    {
        if(Focus() == this) Focus() = 0;
    }

    float Widget::GetTopLayer()
    {
        return HighestLayer();
    }

    float& Widget::HighestLayer()
    {
        if(parent) return parent->HighestLayer();
        return highestLayer;
    }

    int Widget::GetSize(unit_type type) const
    {
        SUnit lSize;
        switch(type)
        {
        case HORIZONTAL:
            lSize = size.GetHorizontal();
            break;
        case VERTICAL:
            lSize = size.GetVertical();
            break;
        }
        if(!parent)
        {
            if(lSize.GetRelative())
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           "Attempting to take relative size of parentless container");
            int thisSize = lSize.GetAbsolute();
            if(thisSize < 0)
            {
                ENGINE_LOG(SORE_Logging::LVL_ERROR,
                           "Attempting to take negative size of parentless container");
                thisSize = 0;
            }
            return static_cast<unsigned int>(thisSize);
        }
        else
            return static_cast<int>(parent->GetClientSize(type)*lSize.GetRelative()) +
                lSize.GetAbsolute();
    }

    int Widget::GetPosition(unit_type type) const
    {
        if(type == HORIZONTAL)
            return GetPixels(type, position.GetHorizontal());
        else if(type == VERTICAL)
            return GetPixels(type, position.GetVertical());
        return 0;
    }

    float Widget::GetLayer() const
    {
        return layer;
    }

    int Widget::GetPixels(unit_type type, SUnit unit) const
    {
        int absSize = unit.GetAbsolute();
        unsigned int relAdd = 0;
        if(parent)
        {
            unsigned int parentSize = parent->GetSize(type);
            double relSize = unit.GetRelative();
            if(relSize < 0.0)
                relAdd = static_cast<unsigned int>(parentSize*(1-relSize));
            else
                relAdd = static_cast<unsigned int>(parentSize*relSize);

            absSize += parent->GetPosition(type);
            absSize += parent->GetClientPosition(type);
        }
        return absSize + relAdd;
    }

    Widget*& Widget::Focus()
    {
        if(parent) return parent->Focus();
        return focus;
    }

    const Widget* Widget::Focus() const
    {
        if(parent) return parent->Focus();
        return focus;
    }

    Widget*& Widget::OldFocus()
    {
        if(parent) return parent->OldFocus();
        return oldFocus;
    }

    bool Widget::HasFocus() const
    {
        return Focus() == this;
    }

    bool operator==(const SORE_GUI::Widget& one, const SORE_GUI::Widget& two)
    {
        // little bit of a hack
        return &one == &two;
    }

    void Widget::AddChild(Widget* c)
    {
        if(c)
        {
            if(std::find(children.begin(), children.end(), *c) == children.end())
            {
                children.push_back(c);
                c->parent = this;
            }
        }
    }

    void Widget::RemoveChild(Widget* c)
    {
        if(c)
        {
            Widget_container_t::iterator it;
            if((it = find(children.begin(), children.end(), *c)) != children.end())
            {
                children.erase(it);
            }
        }
    }

    void Widget::SetStyleName(const std::string& style_)
    {
        style = style_;
    }

    void Widget::LoadStyle(SORE_FileIO::InFile& file)
    {
        Json::Reader reader;
        reader.parse(file.strm(), styleConfig, false);
    }

    void Widget::SetVisible(bool visible)
    {
        isVisible = visible;
    }
}
