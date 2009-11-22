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

#include "sore_framewidget.h"

namespace SORE_GUI
{
    FrameWidget::FrameWidget(SVec s, SVec p, size_mode m, Widget* par)
        : Widget(s, p, par), mode(m), texture(0), leftBorder(16), rightBorder(16),
          topBorder(16), bottomBorder(16)
    {
        for(unsigned int i=0;i<9;++i)
            chunk[i] = 0;

    }

    FrameWidget::~FrameWidget()
    {
        DestroyGeometry();
    }

    void FrameWidget::SetBorderSizes(float l, float r, float t, float b)
    {
        switch(mode)
        {
        case SCALE_CENTER:
            leftBorder = static_cast<unsigned int>(l);
            rightBorder = static_cast<unsigned int>(r);
            topBorder = static_cast<unsigned int>(t);
            bottomBorder = static_cast<unsigned int>(b);
            break;
        case SCALE_ALL:
        {
            float h = static_cast<float>(GetSize(HORIZONTAL));
            float v = static_cast<float>(GetSize(VERTICAL));
            float f = h < v ? h : v;
            leftBorder = static_cast<unsigned int>(l/64.0f * f);
            rightBorder = static_cast<unsigned int>(r/64.0f * f);
            topBorder = static_cast<unsigned int>(t/64.0f * f);
            bottomBorder = static_cast<unsigned int>(b/64.0f * f);
            break;
        }
        default:
            ENGINE_LOG(SORE_Logging::LVL_ERROR,
                       "Unknown sizing mode selected for FrameWidget");
            break;
        };
        DestroyGeometry();
        BuildGeometry();
    }

    void FrameWidget::SetTexture(SORE_Graphics::Texture2D* tex)
    {
        texture = tex;
        for(unsigned int i=0;i<9;++i)
        {
            if(chunk[i])
                chunk[i]->SetTexture(tex);
        }
    }

    std::vector<SORE_Graphics::GeometryChunk*> FrameWidget::GetChunks() const
    {
        std::vector<SORE_Graphics::GeometryChunk*> all;
        for(unsigned int i=0;i<9;++i)
        {
            if(chunk[i]) all.push_back(chunk[i]);
        }
        return all;
    }

    void FrameWidget::BuildGeometry()
    {
        unsigned int width = GetSize(HORIZONTAL);
        unsigned int height = GetSize(VERTICAL);

        SORE_Math::Rect<float> bounds;
        SORE_Math::Rect<float> texCoords;

        if(leftBorder + rightBorder < width && bottomBorder + topBorder < height)
        {
            //center
            bounds = SORE_Math::Rect<float>(static_cast<float>(leftBorder),
                                            static_cast<float>(width - leftBorder),
                                            static_cast<float>(topBorder),
                                            static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.25f, 0.75f);

            chunk[0] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(leftBorder && bottomBorder + topBorder < height)
        {
            //left border
            bounds = SORE_Math::Rect<float>(0.0f,
                                            static_cast<float>(leftBorder),
                                            static_cast<float>(topBorder),
                                            static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.25f, 0.75f);

            chunk[1] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(leftBorder && topBorder)
        {
            //top left corner
            bounds = SORE_Math::Rect<float>(0.0f,
                                            static_cast<float>(leftBorder), 0.0f,
                                            static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.0f, 0.25f);

            chunk[2] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(leftBorder + rightBorder < width && topBorder)
        {
            //top border
            bounds = SORE_Math::Rect<float>(static_cast<float>(leftBorder),
                                            static_cast<float>(width - rightBorder), 0.0f,
                                            static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.0f, 0.25f);

            chunk[3] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(rightBorder && topBorder)
        {
            //top right corner
            bounds = SORE_Math::Rect<float>(static_cast<float>(width - rightBorder),
                                            static_cast<float>(width), 0.0f,
                                            static_cast<float>(topBorder));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.0f, 0.25f);

            chunk[4] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(rightBorder && topBorder + bottomBorder < height)
        {
            //right border
            bounds = SORE_Math::Rect<float>(static_cast<float>(width - rightBorder),
                                            static_cast<float>(width),
                                            static_cast<float>(topBorder),
                                            static_cast<float>(height - bottomBorder));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.25f, 0.75f);

            chunk[5] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(rightBorder && bottomBorder)
        {
            //bottom right corner
            bounds = SORE_Math::Rect<float>(
                static_cast<float>(width - rightBorder),
                static_cast<float>(width),
                static_cast<float>(height - bottomBorder),
                static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.75f, 1.0f, 0.75f, 1.0f);

            chunk[6] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(bottomBorder && rightBorder + leftBorder < width)
        {
            //bottom border
            bounds = SORE_Math::Rect<float>(static_cast<float>(leftBorder),
                                            static_cast<float>(width - rightBorder),
                                            static_cast<float>(height - bottomBorder),
                                            static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.25f, 0.75f, 0.75f, 1.0f);

            chunk[7] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }

        if(leftBorder && bottomBorder)
        {
            //bottom left corner
            bounds = SORE_Math::Rect<float>(0.0f,
                                            static_cast<float>(leftBorder),
                                            static_cast<float>(height - bottomBorder),
                                            static_cast<float>(height));
            texCoords = SORE_Math::Rect<float>(0.0f, 0.25f, 0.75f, 1.0f);

            chunk[8] = new SORE_Graphics::GeometryChunk(texture, bounds, texCoords);
        }
    }

    void FrameWidget::DestroyGeometry()
    {
        for(unsigned int i=0;i<9;++i)
        {
            delete chunk[i];
            chunk[i] = 0;
        }
    }
}
