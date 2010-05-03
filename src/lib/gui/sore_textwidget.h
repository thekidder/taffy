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

#ifndef SORE_TEXTWIDGET_H
#define SORE_TEXTWIDGET_H

#include <sore_font.h>
#include <sore_text.h>
#include <sore_widget.h>

namespace SORE_GUI
{
    class SORE_EXPORT TextWidget : public Widget
    {
    public:
        TextWidget(SVec p, SORE_Font::Font& f, unsigned int h, const std::string& t,
                   const SORE_Graphics::Color& c = SORE_Graphics::White,
                   Widget* parent=NULL);

        void UpdateText(const std::string& t);
        void SetColor(const SORE_Graphics::Color& c);
    private:
        void UpdatePosition();
        void OnGLReload();
        std::vector<SORE_Graphics::Renderable> GetThisRenderList();
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdateCache();

        SORE_Font::Font& face;
        unsigned int height;
        SORE_Graphics::Text text;
        SORE_Graphics::Color color;

        std::vector<SORE_Graphics::Renderable> all;
    };
}

#endif
