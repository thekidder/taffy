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

#include <boost/lexical_cast.hpp>

#include <sore_geometry.h>
#include <sore_sprite.h>
#include <sore_timing.h>
#include <sore_textfield.h>

namespace SORE_GUI
{
    TextField::TextField(SVec s, SVec p, SORE_Resource::ResourcePool& pool,
                         Widget* par)
        : FrameWidget(s, p, SCALE_CENTER, par), text(""), displayText(0),
          pos(0), textStart(0), textEnd(0)
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
        SetShader(shad);

        float height = GetSize(VERTICAL) - 16.0f;
        caretWidth = static_cast<unsigned int>(height/16.0f);

        caret = SORE_Graphics::MakeSprite(
            SORE_Math::Rect<float>(8.0f, 8.0f+caretWidth, 8.0f, 8.0f+height),
            SORE_Math::Rect<float>(0.0f, 0.0f, 1.0f, 1.0f),
            0.0f,
            caretTex,
            shad,
            SORE_Graphics::BLEND_SUBTRACTIVE);

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

    void TextField::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
/*        std::vector<SORE_Graphics::Renderable> list;

        std::vector<SORE_Graphics::Renderable> frame = GetChunks();
        std::vector<SORE_Graphics::Renderable>::iterator it;
        for(it = frame.begin();it!=frame.end();++it)
        {
            SORE_Graphics::Renderable r(*it);
            SORE_Graphics::TransformationPtr m(
                new SORE_Math::Matrix4<float>(
                    *it->GetTransform() * GetPositionMatrix()));
            r.SetTransform(m);
            r.SetLayer(GUI_LAYER);
            r.AddKeyword("gui");
            list.push_back(r);
        }

        std::vector<SORE_Graphics::Renderable> text = displayText->GetGeometry();
        list.insert(list.begin(), text.begin(), text.end());

        if(HasFocus())
        {
            unsigned int time = SORE_Timing::GetGlobalTicks();
            if(time - caretEnd < 5000)
            {
                SORE_Graphics::Renderable r(caret);
                SORE_Graphics::TransformationPtr m(
                    new SORE_Math::Matrix4<float>(
                        *caret.GetTransform() * GetPositionMatrix()));
                r.SetTransform(m);
                r.SetLayer(GUI_LAYER);
                r.AddKeyword("gui");
                list.push_back(r);
            }
            else if(time - caretEnd > 10000)
                caretEnd = time;
        }

        return list;*/
    }

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
