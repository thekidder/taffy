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

#ifndef SORE_FRAMEWIDGET_H
#define SORE_FRAMEWIDGET_H

#include "sore_widget.h"

namespace SORE_GUI
{
    enum size_mode {SCALE_ALL, SCALE_CENTER};

    class SORE_EXPORT FrameWidget : public Widget
    {
    public:
        FrameWidget(SVec s, SVec p, size_mode m, Widget* par=NULL);
        ~FrameWidget();
    protected:
        void SetBorderSizes(float l, float r, float t, float b);
        void SetTexture(SORE_Graphics::Texture2D* tex);

        std::vector<SORE_Graphics::GeometryChunk*> GetChunks() const;
    private:
        virtual bool ProcessEvents(SORE_Kernel::Event* e) = 0;
        virtual SORE_Graphics::render_list GetThisRenderList() = 0;

        void BuildGeometry();
        void DestroyGeometry();

        size_mode mode;

        SORE_Graphics::Texture2D* texture;

        SORE_Graphics::GeometryChunk* chunk[9];
        unsigned int leftBorder, rightBorder, topBorder, bottomBorder;
    };
}

#endif
