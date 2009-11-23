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

#include "sore_dropdown.h"

namespace SORE_GUI
{
    Dropdown::Dropdown(SVec s, SVec p, SORE_Resource::ResourcePool& pool, Widget* par)
        : FrameWidget(s, p, SCALE_ALL, par), originalSize(s), font(0), normal(0),
          active(0), hover(0), arrow(0), menuBg(0), arrowChunk(0), curChoice(0),
          pressed(false), inArea(false)
    {
        std::string styleDir("data/");
        styleDir += GetStyle() + "/";

        font = pool.GetResource<SORE_Font::Font>(styleDir +
                                                 "LiberationSans-Regular.ttf");

        normal = pool.GetResource<SORE_Graphics::Texture2D>(styleDir +
                                                            "button_sheet_normal.tga");
        active = pool.GetResource<SORE_Graphics::Texture2D>(styleDir +
                                                            "button_sheet_active.tga");
        hover = pool.GetResource<SORE_Graphics::Texture2D>(styleDir +
                                                           "button_sheet_hover.tga");
        arrow =
            pool.GetResource<SORE_Graphics::Texture2D>(styleDir + "arrow.tga");
        menuBg =
            pool.GetResource<SORE_Graphics::Texture2D>(styleDir + "dropdown_bg.tga");

        textHeight = GetSize(VERTICAL) / 2;
        if(textHeight < 16) textHeight = 16;

        SORE_Math::Rect<float> arrowSize(0.0f,
            static_cast<float>(textHeight), 0.0f,
            static_cast<float>(textHeight));
        shad = pool.GetResource<SORE_Graphics::GLSLShader>
            ("data/Shaders/default.shad");
        arrowChunk = new SORE_Graphics::GeometryChunk(arrow, shad, arrowSize);
        UpdatePosition();
        SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
        SetTexture(normal);
    }

    Dropdown::~Dropdown()
    {
        for(std::map<unsigned int, SORE_Graphics::Text*>::iterator it=choices.begin();
            it!=choices.end();++it)
        {
            delete it->second;
        }
        delete arrowChunk;
        delete curChoice;
    }

    void Dropdown::UpdateChoice(unsigned int id, std::string text, bool selected)
    {
        if(choices.find(id) == choices.end())
        {
            SORE_Graphics::Text* t = new SORE_Graphics::Text(*font, textHeight, text);
            choices.insert(std::make_pair(id, t));
            if(selected)
                SetCurrent(t);
        }
        else
        {
            choices[id]->UpdateText(text);
            if(selected)
                SetCurrent(choices[id]);
        }
        BuildGeometry();
    }

    void Dropdown::OnGLReload()
    {
        BuildGeometry();
    }

    void Dropdown::BuildGeometry()
    {
        std::vector<std::pair<SORE_Math::Matrix4<float>,
            SORE_Graphics::GeometryChunk*> >::iterator it;
        for(it=menu.begin();it!=menu.end();++it)
        {
            SORE_Graphics::GeometryChunk* g = it->second;
            delete g;
        }
        menu.clear();
        unsigned int currHeight = GetPixels(VERTICAL, originalSize.GetVertical());
        for(std::map<unsigned int, SORE_Graphics::Text*>::iterator it=choices.begin();
            it!=choices.end();++it)
        {
            SORE_Math::Rect<float> bounds(0.0f,
                                          static_cast<float>(GetSize(HORIZONTAL)),
                                          static_cast<float>(currHeight),
                                          static_cast<float>(currHeight+textHeight*2));
            SORE_Graphics::GeometryChunk* g = new SORE_Graphics::GeometryChunk
                (menuBg, shad, bounds);
            SORE_Math::Matrix4<float> m =
                SORE_Math::Matrix4<float>::GetTranslation(0.0f, 0.0f,
                                                          GetTopLayer() + 0.0009f) *
                GetPositionMatrix();
            menu.push_back(std::make_pair(m, g));
            it->second->SetTransform(
                GetPositionMatrix() *
                SORE_Math::Matrix4<float>::GetTranslation
                (8.0f,
                 static_cast<float>(currHeight + 0.5*textHeight),
                 GetTopLayer() + 0.001f));
            it->second->TrimToWidth(GetSize(HORIZONTAL) - 16, 0);
            currHeight += textHeight*2;
        }
    }

    void Dropdown::ConnectChange(boost::function<void (unsigned int)> c)
    {
        onChange.connect(c);
    }

    SORE_Graphics::render_list Dropdown::GetThisRenderList()
    {
        SORE_Graphics::render_list list;

        std::vector<SORE_Graphics::GeometryChunk*> frame = GetChunks();
        std::vector<SORE_Graphics::GeometryChunk*>::iterator it;
        for(it = frame.begin();it!=frame.end();++it)
        {
            list.push_back(std::make_pair(&GetPositionMatrix(), *it));
        }

        list.push_back(std::make_pair(&arrowMat, arrowChunk));

        if(HasFocus())
        {
            std::vector<std::pair<SORE_Math::Matrix4<float>,
                SORE_Graphics::GeometryChunk*> >::iterator mit;
            for(mit = menu.begin();mit!=menu.end();++mit)
                list.push_back(std::make_pair(&mit->first, mit->second));
            std::map<unsigned int, SORE_Graphics::Text*>::iterator it2;
            for(it2 = choices.begin();it2!=choices.end();++it2)
            {
                SORE_Graphics::render_list text = it2->second->GetGeometry();
                list.insert(list.begin(), text.begin(), text.end());
            }
        }

        if(curChoice)
        {
            SORE_Graphics::render_list text = curChoice->GetGeometry();
            list.insert(list.begin(), text.begin(), text.end());
        }

        return list;
    }

    bool Dropdown::ProcessEvents(SORE_Kernel::Event* e)
    {
        if(e->type == SORE_Kernel::MOUSEENTER)
        {
            inArea = true;
            ChangeTexture();
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSELEAVE)
        {
            inArea = false;
            ChangeTexture();
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            pressed = true;
            ChangeTexture();
            SetSize(GetSize() + SVec(SUnit(0.0, 0),
                                     SUnit(0.0, choices.size()*textHeight*2)));
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSEBUTTONUP)
        {
            pressed = false;
            unsigned int origY = GetPixels(VERTICAL, originalSize.GetVertical());

            if(e->mouse.y > origY)
            {
                unsigned int choice = e->mouse.y - origY;
                choice /= (textHeight*2);
                ClearFocus();
                SetSize(originalSize);
                inArea = false;
                SetCurrent(choices[OrderToID(choice)]);
                onChange(OrderToID(choice));
            }
            ChangeTexture();
            return true;
        }
        return false;
    }

    void Dropdown::SetCurrent(SORE_Graphics::Text* t)
    {
        if(!curChoice)
        {
            curChoice = new SORE_Graphics::Text(*font, textHeight, t->GetText());
            curChoice->TrimToWidth(
                GetPixels(HORIZONTAL,
                          originalSize.GetHorizontal()) - 16 - 2*textHeight, 0);
            UpdatePosition();
        }
        else
        {
            curChoice->UpdateText(t->GetText());
            curChoice->TrimToWidth(
                GetPixels(HORIZONTAL,
                          originalSize.GetHorizontal()) - 16 - 2*textHeight, 0);
        }
    }

    unsigned int Dropdown::OrderToID(unsigned int order)
    {
        unsigned int id = 0;
        for(std::map<unsigned int, SORE_Graphics::Text*>::iterator it=choices.begin();
            it!=choices.end();++it,++id)
        {
            if(id == order) return it->first;
        }
        ENGINE_LOG(SORE_Logging::LVL_WARNING,
                   boost::format("Could not find ID for menu item %d") % order);
        return 0;
    }

    void Dropdown::ChangeTexture()
    {
        if(HasFocus())
            SetTexture(active);
        else if(inArea)
            SetTexture(hover);
        else
            SetTexture(normal);
    }

    void Dropdown::FocusLost()
    {
        SetTexture(normal);
        SetSize(originalSize);
    }

    void Dropdown::UpdatePosition()
    {
        if(curChoice)
            curChoice->SetTransform(
                GetPositionMatrix() *
                SORE_Math::Matrix4<float>::GetTranslation
                (16.0f,
                 (GetPixels(VERTICAL, originalSize.GetVertical()) - textHeight)/2.0f,
                 GetLayer() + LAYER_SEPARATION/2.0f));
        arrowMat = SORE_Math::Matrix4<float>::GetTranslation(
            GetPixels(HORIZONTAL, originalSize.GetHorizontal()) - 16.0f - textHeight,
            (GetPixels(VERTICAL, originalSize.GetVertical()) - textHeight)/2.0f,
            GetLayer() + LAYER_SEPARATION/2.0f) * GetPositionMatrix();
        BuildGeometry();
    }
}
