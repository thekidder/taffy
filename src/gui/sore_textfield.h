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

#ifndef SORE_TEXTFIELD_H
#define SORE_TEXTFIELD_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <boost/function.hpp>
#include <boost/signals.hpp>

#include "../sore_resource.h"
#include "../sore_font.h"
#include "../sore_text.h"
#include "sore_framewidget.h"

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
        virtual SORE_Graphics::render_list GetThisRenderList();
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdatePosition();
        void UpdateText(int dir);

        std::string text;
        SORE_Font::FontPtr font;
        SORE_Graphics::Text* displayText;
        SORE_Graphics::Texture2DPtr texture;

        SORE_Graphics::GeometryChunk* caret;
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
