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

#ifndef SORE_DROPDOWN_H
#define SORE_DROPDOWN_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <map>

#include <boost/signals.hpp>
#include <boost/function.hpp>

#include <sore_resource.h>
#include <sore_font.h>
#include <sore_text.h>
#include <sore_framewidget.h>

namespace SORE_GUI
{
    class SORE_EXPORT Dropdown : public FrameWidget
    {
    public:
        Dropdown(SVec s, SVec p, SORE_Resource::ResourcePool& pool, Widget* par=NULL);
        ~Dropdown();

        //add a new choice or update the one specified by id
        void UpdateChoice(unsigned int id, std::string text, bool selected = false);
        void ConnectChange(boost::function<void (unsigned int)> c);
    private:
        void OnGLReload();
        virtual std::vector<SORE_Graphics::Renderable> GetThisRenderList();
        bool ProcessEvents(SORE_Kernel::Event* e);
        void UpdatePosition();
        void FocusLost();
        void BuildGeometry();
        void ChangeTexture();
        unsigned int OrderToID(unsigned int order);
        void SetCurrent(SORE_Graphics::Text* t);

        SVec originalSize;

        SORE_Font::FontPtr font;
        SORE_Graphics::GLSLShaderPtr shad;
        unsigned int textHeight;
        SORE_Graphics::Texture2DPtr
            normal, active, hover, arrow, menuBg;
        SORE_Math::Matrix4<float> arrowMat;
        SORE_Graphics::GeometryChunk* arrowChunk;

        std::map<unsigned int, SORE_Graphics::Text*> choices;
        SORE_Graphics::Text* curChoice;
        std::vector<std::pair<SORE_Math::Matrix4<float>,
            SORE_Graphics::GeometryChunk*> > menu;

        boost::signal<void (unsigned int)> onChange;

        bool pressed;
        bool inArea;
    };
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
