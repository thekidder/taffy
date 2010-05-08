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

#include <sore_screeninfo.h>

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
                returnProj.top = static_cast<float>(si.height);
                returnProj.left = 0;
                returnProj.bottom = 0;
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

    bool operator==(const ProjectionInfo& one, const ProjectionInfo& two)
    {
        switch(one.type)
        {
        case NONE:
            return two.type == NONE;
        case ORTHO2D:
            return two.type == ORTHO2D
                && one.left == two.left && one.right == two.right
                && one.top == two.top && one.bottom == two.bottom;
        case ORTHO:
            //TODO: finish ortho projection
            return two.type == ORTHO;
        case PERSPECTIVE:
            return two.type == PERSPECTIVE
                && one.fov == two.fov && one.ratio == two.ratio
                && one.znear == two.znear && one.zfar == two.zfar;
        default:
            return false;
        }
    }

    bool operator!=(const ProjectionInfo& one, const ProjectionInfo& two)
    {
        return !(one == two);
    }

    bool operator==(const camera_info& one, const camera_info& two)
    {
        return one.projection == two.projection
            && one.viewMatrix == two.viewMatrix;
    }

    bool operator!=(const camera_info& one, const camera_info& two)
    {
        return !(one == two);
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
