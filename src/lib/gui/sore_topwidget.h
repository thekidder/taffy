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

#ifndef SORE_TOPWIDGET_H
#define SORE_TOPWIDGET_H

#include "../sore_geometryprovider.h"
#include "../sore_input.h"
#include "../sore_screeninfo.h"
#include "sore_widget.h"

namespace SORE_GUI
{
    class SORE_EXPORT TopWidget : public Widget, public SORE_Graphics::GeometryProvider
    {
    public:
        TopWidget(unsigned int width, unsigned int height);

        bool OnResize(SORE_Kernel::Event* e);

        virtual void MakeUpToDate();
        virtual std::vector<SORE_Graphics::Renderable>::iterator GeometryBegin();
        virtual std::vector<SORE_Graphics::Renderable>::iterator GeometryEnd();

        SORE_Graphics::camera_info GetCamera();
    private:
        std::vector<SORE_Graphics::Renderable> GetThisRenderList();
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdateResolution(unsigned int w, unsigned int h);

        std::vector<SORE_Graphics::Renderable> renderables;
    };
}

#endif
