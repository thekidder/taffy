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

#include "sore_screeninfo.h"

namespace SORE_Graphics
{
    ProjectionInfo SetupProjection(const ProjectionInfo& pi, const ScreenInfo& si)
    {
        ProjectionInfo returnProj = pi;
        switch(pi.type)
        {
        case NONE:
            break;
        case ORTHO2D:
            returnProj.znear = -1.0f;
            returnProj.zfar  =  1.0f;
            if(pi.useScreenCoords)
            {
                returnProj.top = 0;
                returnProj.left = 0;
                returnProj.bottom = static_cast<float>(si.height);
                returnProj.right = static_cast<float>(si.width);
                returnProj.ratio = si.ratio;
            }
            else if(pi.useScreenRatio)
            {
                returnProj.bottom = pi.left / si.ratio;
                returnProj.top = pi.right / si.ratio;
                returnProj.ratio = si.ratio;
            }
            else
            {
                returnProj.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
            }
            break;
        case ORTHO:
            //TODO: finish ortho projection
            break;
        case PERSPECTIVE:
            if(pi.useScreenRatio)
            {
                returnProj.ratio = si.ratio;
            }
            else
            {
                returnProj.ratio = (pi.right - pi.left) / (pi.top - pi.bottom);
            }
            break;
        default:
            break;
        }
        return returnProj;
    }

    SORE_Math::Vector2<float> ScreenToProjection(
        ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<int> pos)
    {
        SORE_Math::Vector2<float> projected;
        projected[0] = proj.left + (proj.right-proj.left)*
            (static_cast<float>(pos[0])/static_cast<float>(screen.width));
        projected[1] = proj.bottom + (proj.top-proj.bottom)*
            (static_cast<float>(pos[1])/static_cast<float>(screen.height));
        return projected;
    }

    SORE_Math::Vector2<int> ProjectionToScreen(
        ScreenInfo screen, ProjectionInfo proj, SORE_Math::Vector2<float> pos)
    {
        SORE_Math::Vector2<int> screenPos;
        screenPos[0] = static_cast<int>((screen.width )*
                                        (pos[0]/(proj.right-proj.left)));
        screenPos[1] = static_cast<int>((screen.height)*
                                        (pos[1]/(proj.top-proj.bottom)));
        return screenPos;
    }
}
