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

#ifndef SORE_FRAMEWINDOW_H
#define SORE_FRAMEWINDOW_H

#include "../sore_text.h"
#include "sore_framewidget.h"

namespace SORE_GUI
{
    class SORE_EXPORT FrameWindow : public FrameWidget
    {
    public:
        FrameWindow(SVec s, SVec p, const std::string& t,
                    SORE_Resource::ResourcePool& pool, Widget* par=NULL);
        ~FrameWindow();
    protected:
        unsigned int GetClientSize(unit_type type) const;
        unsigned int GetClientPosition(unit_type type) const;
    private:
        bool ProcessEvents(SORE_Kernel::Event* e);
        virtual SORE_Graphics::render_list GetThisRenderList();
        void UpdatePosition();

        std::string title;
        boost::shared_ptr<SORE_Font::Font> font;
        SORE_Graphics::Text* titleText;
        bool dragged;
    };
}

#endif
