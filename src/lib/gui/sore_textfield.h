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

#ifndef SORE_TEXTFIELD_H
#define SORE_TEXTFIELD_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <boost/function.hpp>
#include <boost/signals.hpp>

#include <sore_resource.h>
#include <sore_font.h>
#include <sore_text.h>
#include <sore_framewidget.h>

namespace SORE_GUI
{
    class SORE_EXPORT TextField : public FrameWidget
    {
    public:
        TextField(SVec s, SVec p, SORE_Resource::ResourcePool& pool, Widget* par=NULL);
        ~TextField();

        const std::string& GetText() const;
        void SetContents(const std::string& t);

        void ConnectChange(boost::function<void (std::string)> c);
    private:
        //TODO:fixme
        virtual std::vector<SORE_Graphics::Renderable> GetThisRenderList();
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdatePosition();
        void UpdateText(int dir);

        std::string text;
        SORE_Font::FontPtr font;
        SORE_Graphics::Text* displayText;
        SORE_Graphics::Texture2DPtr texture;

        SORE_Graphics::Renderable caret;
        SORE_Graphics::Texture2DPtr caretTex;
        SORE_Math::Matrix4<float> caretMat;
        unsigned int caretPos;
        unsigned int caretWidth;
        //used for caret blinking
        unsigned int caretEnd;

        unsigned int pos;

        unsigned int textStart, textEnd;

        boost::signal<void (std::string)> onChange;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
