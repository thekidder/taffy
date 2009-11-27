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

#include <boost/lexical_cast.hpp>

#include "../sore_timing.h"
#include "sore_textfield.h"

namespace SORE_GUI
{
    TextField::TextField(SVec s, SVec p, SORE_Resource::ResourcePool& pool,
                         Widget* par)
        : FrameWidget(s, p, SCALE_CENTER, par), text(""), displayText(0),
          caret(0), pos(0), textStart(0), textEnd(0)
    {
        std::string styleDir("data/");
        styleDir += GetStyle() + "/";

        SORE_Graphics::GLSLShaderPtr shad =
            pool.GetResource<SORE_Graphics::GLSLShader>("data/Shaders/default.shad");
        texture = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir+"textfield.tga");
        caretTex = pool.GetResource<SORE_Graphics::Texture2D>(
            styleDir + "caret.tga");

        SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
        SetTexture(texture);

        float height = GetSize(VERTICAL) - 16.0f;
        caretWidth = static_cast<unsigned int>(height/16.0f);

        //TODO:fixme
        /*caret = new SORE_Graphics::GeometryChunk
            (caretTex, shad, SORE_Math::Rect<float>(8.0f, 8.0f+caretWidth,
                                                    8.0f, 8.0f+height));
        */

        caretEnd = SORE_Timing::GetGlobalTicks();

        unsigned int textHeight = GetSize(VERTICAL) - 16;

        font = pool.GetResource<SORE_Font::Font>(styleDir +
                                                 "LiberationSans-Regular.ttf");
        displayText = new SORE_Graphics::Text(*font, textHeight, text);

        UpdateText(0);
        UpdatePosition();
    }

    TextField::~TextField()
    {
        delete displayText;
        delete caret;
    }

    void TextField::ConnectChange(boost::function<void (std::string)> c)
    {
        onChange.connect(c);
    }

    const std::string& TextField::GetText() const
    {
        return text;
    }

    void TextField::SetContents(const std::string& t)
    {
        text = t;
        onChange(text);
        UpdateText(1);
        UpdatePosition();
    }

    /*SORE_Graphics::render_list TextField::GetThisRenderList()
    {
        SORE_Graphics::render_list list;

        std::vector<SORE_Graphics::GeometryChunk*> frame = GetChunks();
        std::vector<SORE_Graphics::GeometryChunk*>::iterator it;
        for(it = frame.begin();it!=frame.end();++it)
        {
            list.push_back(std::make_pair(&GetPositionMatrix(), *it));
        }

        SORE_Graphics::render_list text = displayText->GetGeometry();
        list.insert(list.begin(), text.begin(), text.end());

        if(HasFocus())
        {
            unsigned int time = SORE_Timing::GetGlobalTicks();
            if(time - caretEnd < 5000)
            {
                list.push_back(std::make_pair(&caretMat, caret));
            }
            else if(time - caretEnd > 10000)
                caretEnd = time;
        }

        return list;
    }
    */

    bool TextField::ProcessEvents(SORE_Kernel::Event* e)
    {
        if(!HasFocus()) return false;
        int dir = 0;
        if(e->type == SORE_Kernel::KEYDOWN)
        {
            if(e->key.keySym == SDLK_LEFT)
            {
                if(text.length() && pos)
                {
                    pos--;
                    dir = -1;
                }
            }
            else if(e->key.keySym == SDLK_RIGHT)
            {
                if(text.length() > pos)
                {
                    pos++;
                    dir = 1;
                }
            }
            else if(e->key.keySym == SDLK_BACKSPACE)
            {
                if(pos)
                {
                    text.erase(pos - 1, 1);
                    pos--;
                    onChange(text);
                    dir = -1;
                }
            }
            else if(e->key.unicode < 128 && e->key.unicode > 31)
            {
                char insert = static_cast<char>(e->key.unicode);
                text.insert(pos, 1, insert);
                pos++;
                onChange(text);
                dir = 1;
            }
            UpdateText(dir);
            UpdatePosition();
            return true;
        }
        else if(e->type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            float x = static_cast<float>(e->mouse.x - 8);
            size_t index = displayText->GetIndex(x);
            unsigned int oldPos = pos;
            pos = index + textStart;
            dir = pos > oldPos ? 1 :
                pos < oldPos ? -1 : 0;
            UpdateText(dir);
            UpdatePosition();
        }
        return false;
    }

    void TextField::UpdateText(int dir)
    {
        displayText->UpdateText(text);
        unsigned int contentsWidth = displayText->GetWidth() + caretWidth;
        if(pos < textStart && dir < 0)
            textStart = pos;
        unsigned int numChars = text.length();
        textEnd = textStart + numChars;
        while(contentsWidth > GetSize(HORIZONTAL) -  16.0f)
        {
            numChars--;
            if( (dir > 0 && pos > textEnd) || (textEnd - textStart > numChars) )
            {
                int adjust = pos - numChars;
                if(adjust < 0) adjust = 0;
                textStart = adjust;
                textEnd = textStart + numChars;
            }
            else if(dir < 0)
            {
                textStart = pos - numChars;
                textEnd = numChars + textStart;
            }
            displayText->UpdateText(text.substr(textStart, numChars));
            contentsWidth = displayText->GetWidth() + caretWidth;
        }
        displayText->UpdateText(text.substr(textStart, pos - textStart));
        caretPos = displayText->GetWidth();
        displayText->UpdateText(text.substr(textStart, numChars));
    }

    void TextField::UpdatePosition()
    {
        displayText->SetTransform
            (GetPositionMatrix() *
             SORE_Math::Matrix4<float>::GetTranslation(8.0f, 8.0f, GetLayer() +
                                                       LAYER_SEPARATION/2.0f));
        caretMat = GetPositionMatrix() *
            SORE_Math::Matrix4<float>::GetTranslation
            (static_cast<float>(caretPos), 0.0f, GetLayer() + LAYER_SEPARATION/2.0f);
    }
}
