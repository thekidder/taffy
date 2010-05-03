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


#ifndef  __ALLGL_H__
#define  __ALLGL_H__

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#include <GL/glew.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>

#ifdef _WIN32
    #include <GL/wglew.h>
#else /* GLX */

#include <GL/glxew.h>
    //Linux-specific to find system fonts
    #include <fontconfig/fontconfig.h>
#endif
#endif //__ALLGL_H__
