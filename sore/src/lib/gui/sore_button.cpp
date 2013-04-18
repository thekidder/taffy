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

#include <sore_button.h>

namespace SORE_GUI
{
    Button::Button(SVec s, SVec p, const std::string& text,
                   Widget* par)
        : FrameWidget(s, p, SCALE_ALL, par), pressed(false), inArea(false)
    {
        std::string styleDir("data/");
        styleDir += StyleName() + "/";

        unsigned int height = GetSize(VERTICAL);
        // unsigned int width = GetSize(HORIZONTAL);

        unsigned int textHeight = height / 2;
        if(textHeight < 16) textHeight = 16;

        //t = new SORE_Graphics::Text(*font, textHeight, text);
        //if(t->GetWidth() >= (width-16))
        //{
        //    float factor = static_cast<float>(t->GetWidth()) / (width - 16);
        //    unsigned int newHeight = static_cast<unsigned int>(
        //        static_cast<float>(textHeight) / factor);
        //    delete t;
        //    t = new SORE_Graphics::Text(*font, newHeight, text);
        //}

        UpdatePosition();
        SetBorderSizes(16.0f, 16.0f, 16.0f, 16.0f);
        //SetTexture(normal);
        //SetShader(shad);
    }

    void Button::UpdatePosition()
    {
        /*unsigned int textStartV = (GetSize(VERTICAL) - t->GetHeight())/2;
        unsigned int textStartH = (GetSize(HORIZONTAL) - t->GetWidth())/2;
        t->SetTransform(GetPositionMatrix() *
                        SORE_Math::Matrix4<float>::GetTranslation(
                            static_cast<float>(textStartH),
            static_cast<float>(textStartV),
            0.0f));*/
    }

    Button::~Button()
    {
    }

    void Button::UpdateAndRender(int elapsed, SORE_Graphics::ImmediateModeProvider& imm_mode)
    {
        // TODO: fixme
        /*std::vector<SORE_Graphics::Renderable> list;

        std::vector<SORE_Graphics::Renderable> frame = GetChunks();
        std::vector<SORE_Graphics::Renderable>::iterator it;
        for(it = frame.begin();it!=frame.end();++it)
        {
            SORE_Graphics::Renderable r(*it);
            SORE_Graphics::TransformationPtr m(
                new SORE_Math::Matrix4<float>(
                    GetPositionMatrix()));
            r.SetTransform(m);
            r.AddKeyword("gui");
            list.push_back(r);
        }

        std::vector<SORE_Graphics::Renderable> text = t->GetGeometry();
        for(it = text.begin(); it != text.end(); ++it)
        {
            SORE_Graphics::Renderable r(*it);
            r.AddKeyword("gui");
            list.push_back(r);
        }

        return list;*/
    }

    void Button::ConnectPressed(boost::function<void ()> c)
    {
        onRelease.connect(c);
    }

    bool Button::ProcessEvents(const SORE_Kernel::Event& e)
    {
        if(e.type == SORE_Kernel::MOUSEENTER)
        {
            inArea = true;
            /*if(!pressed)
                SetTexture(hover);*/
            return true;
        }
        else if(e.type == SORE_Kernel::MOUSELEAVE)
        {
            inArea = false;
            //if(!pressed)
            //    SetTexture(normal);
            return true;
        }
        else if(e.type == SORE_Kernel::MOUSEBUTTONDOWN)
        {
            pressed = true;
            /*t->SetTransform(t->GetTransform() *
                            SORE_Math::Matrix4<float>::GetTranslation(2.0, 2.0, 0.0f));*/
            //SetTexture(active);
            return true;
        }
        else if(e.type == SORE_Kernel::MOUSEBUTTONUP)
        {
            pressed = false;
            //t->SetTransform(t->GetTransform() *
            //                SORE_Math::Matrix4<float>::GetTranslation(-2.0f,
            //                                                          -2.0f, 0.0f));
            //if(inArea)
            //    SetTexture(hover);
            //else
            //    SetTexture(normal);
            onRelease();
            return true;
        }
        return false;
    }
}
