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

#include <sore_geometry.h>
#include <sore_dropdown.h>
#include <sore_sprite.h>

namespace SORE_GUI
{
    Dropdown::Dropdown(SVec s, SVec p, SORE_Resource::ResourcePool& pool, Widget* par)
        : FrameWidget(s, p, SCALE_ALL, par), originalSize(s),
          curChoice(0), pressed(false), inArea(false)
    {
        std::string styleDir("data/");
        styleDir += GetStyle() + "/";

        font = pool.GetResource<SORE_Font::Font>(
            styleDir + "LiberationSans-Regular.ttf");

        normal = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "button_sheet_normal.tga");
        active = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "button_sheet_active.tga");
        hover = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "button_sheet_hover.tga");
        arrow = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "arrow.tga");
        menuBg = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "dropdown_bg.tga");

        textHeight = GetSize(VERTICAL) / 2;
        if(textHeight < 16) textHeight = 16;

        SORE_Math::Rect<float> arrowSize(0.0f,
            static_cast<float>(textHeight), 0.0f,
            static_cast<float>(textHeight));
        shad = pool.GetResource<SORE_Graphics::GLSLShader>
            ("data/Shaders/default.shad");
        arrowChunk = SORE_Graphics::MakeSprite(
            arrowSize,
            SORE_Math::Rect<float>(0.0f, 1.0f, 0.0f, 1.0f),
            0.0f,
            arrow,
            shad,
            SORE_Graphics::BLEND_SUBTRACTIVE);
        UpdatePosition();
        SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
        SetTexture(normal);
        SetShader(shad);
    }

    Dropdown::~Dropdown()
    {
        for(std::map<unsigned int, SORE_Graphics::Text*>::iterator it=choices.begin();
            it!=choices.end();++it)
        {
            delete it->second;
        }
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
        menu.clear();
        unsigned int currHeight = GetPixels(VERTICAL, originalSize.GetVertical());
        for(std::map<unsigned int, SORE_Graphics::Text*>::iterator it=choices.begin();
            it!=choices.end();++it)
        {
            SORE_Math::Rect<float> bounds(
                0.0f,
                static_cast<float>(GetSize(HORIZONTAL)),
                static_cast<float>(currHeight),
                static_cast<float>(currHeight+textHeight*2));
            //TODO:fixme
            /*SORE_Graphics::GeometryChunk* g = new SORE_Graphics::GeometryChunk(
                menuBg, shad, bounds);
            */
            SORE_Graphics::Renderable g = SORE_Graphics::MakeSprite(
                bounds,
                SORE_Math::Rect<float>(0.0f, 1.0f, 0.0f, 1.0f),
                0.0f,
                menuBg,
                shad,
                SORE_Graphics::BLEND_SUBTRACTIVE);
            // SORE_Math::Matrix4<float> m =
            //     SORE_Math::Matrix4<float>::GetTranslation(
            //         0.0f, 0.0f,
            //         GetTopLayer() + 0.0009f) *
            //     GetPositionMatrix();
            SORE_Graphics::TransformationPtr m(
                new SORE_Math::Matrix4<float>(
                    SORE_Math::Matrix4<float>::GetTranslation(
                        0.0f, 0.0f,
                        GetTopLayer() + 0.0009f) *
                    GetPositionMatrix()));
            g.SetTransform(m);
            g.AddKeyword("gui");
            menu.push_back(g);
            //menu.push_back(std::make_pair(m, g));
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

    void Dropdown::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        // TODO: fixme
        /*std::vector<SORE_Graphics::Renderable> list;

        std::vector<SORE_Graphics::Renderable> frame = GetChunks();
        std::copy(frame.begin(), frame.end(), std::back_inserter(list));
        list.push_back(arrowChunk);

        if(HasFocus())
        {
            std::vector<SORE_Graphics::Renderable>::iterator mit;
            for(mit = menu.begin();mit!=menu.end();++mit)
                list.push_back(*mit);
            std::map<unsigned int, SORE_Graphics::Text*>::iterator it2;
            for(it2 = choices.begin();it2!=choices.end();++it2)
            {
                std::vector<SORE_Graphics::Renderable> text = it2->second->GetGeometry();
                list.insert(list.begin(), text.begin(), text.end());
            }
        }

        if(curChoice)
        {
            std::vector<SORE_Graphics::Renderable> text = curChoice->GetGeometry();
            std::copy(text.begin(), text.end(), std::back_inserter(list));
        }

        return list;*/
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
