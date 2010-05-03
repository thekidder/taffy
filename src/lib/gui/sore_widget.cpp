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

#include "../sore_logger.h"
#include "sore_widget.h"

namespace SORE_GUI
{
    Widget::Widget(SVec s, SVec p, Widget* par)
        : parent(par), position(p), size(s), focus(0), oldFocus(0), highestLayer(0.0f),
          isVisible(true)
    {
        if(parent)
        {
            parent->AddChild(this);
            layer = parent->GetLayer() + LAYER_SEPARATION;
        }
        else
            layer = 0.0f;

        if(layer > HighestLayer()) HighestLayer() = layer;

        SetPosition(p);

        prev.type = SORE_Kernel::NOEVENT;
    }

    Widget::~Widget()
    {
        if(HasFocus())
            ClearFocus();
        if(parent)
            parent->RemoveChild(this);
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
        for(std::vector<Widget*>::iterator it=children.begin();it!=children.end();++it)
        {
            (*it)->UpdatePositionMatrix();
            (*it)->UpdatePosition();
        }
    }

    const SORE_Math::Matrix4<float>& Widget::GetPositionMatrix() const
    {
        return mat;
    }

    std::vector<SORE_Graphics::Renderable> Widget::GetRenderList()
    {
        std::vector<SORE_Graphics::Renderable> all;
        if(isVisible)
        {
            all = GetThisRenderList();

            std::vector<Widget*>::iterator it;
            for(it = children.begin(); it != children.end(); ++it)
            {
                std::vector<SORE_Graphics::Renderable> child = (*it)->GetRenderList();
                std::copy(child.begin(), child.end(), std::back_inserter(all));
            }
        }
        return all;
    }

    bool Widget::PropagateEvents(SORE_Kernel::Event* e)
    {
        SORE_Kernel::Event previous = prev;
        prev = *e;
        if(e->type == SORE_Kernel::RESIZE)
        {
            PropagateGLReload();
            return true;
        }
        else
        {
            return PropagateEventHelper(e, &previous);
        }
    }

    void Widget::PropagateGLReload()
    {
        for(std::vector<Widget*>::iterator it=children.begin();it!=children.end();++it)
        {
            (*it)->PropagateGLReload();
        }
        OnGLReload();
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

    bool Widget::PropagateEventHelper(SORE_Kernel::Event* e, SORE_Kernel::Event* p)
    {
        if(!isVisible)
            return false;
        if(Focus() && Focus()->InBounds(e->mouse.x, e->mouse.y))
        {
            SORE_Kernel::Event relative = *e;
            relative.mouse.x -= Focus()->GetPosition(HORIZONTAL);
            relative.mouse.y -= Focus()->GetPosition(VERTICAL);
            if(Focus()->ProcessEvents(&relative))
                return true;
        }
        //if there is a mousedown, set focus accordingly
        if(e->type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            OldFocus() = Focus();
            Focus() = 0;
        }
        for(std::vector<Widget*>::iterator it=children.begin();it!=children.end();++it)
        {
            SORE_Kernel::Event relative;
            relative = *e;

            bool inWidget = (*it)->InBounds(e->mouse.x, e->mouse.y);
            bool inPrevWidget = (*it)->InBounds(p->mouse.x, p->mouse.y);

            if(inWidget && !inPrevWidget)
                relative.type = SORE_Kernel::MOUSEENTER;
            if(!inWidget && inPrevWidget)
                relative.type = SORE_Kernel::MOUSELEAVE;

            if(inWidget || relative.type == SORE_Kernel::MOUSEENTER
               || relative.type == SORE_Kernel::MOUSELEAVE || (*it)->HasFocus())
            {
                relative.mouse.x -= (*it)->GetPosition(HORIZONTAL);
                relative.mouse.y -= (*it)->GetPosition(VERTICAL);

                bool accepted = (*it)->PropagateEventHelper(&relative, p);

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

    void Widget::AddChild(Widget* c)
    {
        if(c)
        {
            if(find(children.begin(), children.end(), c) == children.end())
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
            std::vector<Widget*>::iterator it;
            if((it = find(children.begin(), children.end(), c)) != children.end())
            {
                children.erase(it);
            }
        }
    }

    std::string Widget::style = "ix_style";

    void Widget::SetStyle(std::string widgetStyle)
    {
        style = widgetStyle;
    }

    std::string Widget::GetStyle()
    {
        return style;
    }

    void Widget::SetVisible(bool visible)
    {
        isVisible = visible;
    }
}
