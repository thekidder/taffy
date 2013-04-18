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

#ifndef SORE_EVENT_H
#define SORE_EVENT_H

#include <sore_event_keysym.h>

namespace SORE_Kernel
{
    enum Event_type_t
    {
        NOEVENT         = 0x0000,
        MOUSEMOVE       = 0x0001,
        MOUSEBUTTONDOWN = 0x0002,
        MOUSEBUTTONUP   = 0x0004,
        MOUSECLICK      = 0x0008,
        MOUSEWHEELUP    = 0x0010,
        MOUSEWHEELDOWN  = 0x0020,
        KEYDOWN         = 0x0040,
        KEYUP           = 0x0080,
        KEYCLICK        = 0x0100,
        TEXTENTERED     = 0x0200,

        //used for SORE_GUI
        MOUSEENTER      = 0x0400,
        MOUSELEAVE      = 0x0800,

        //resize is called when the actual screen is resized by *any* method
        RESIZE          = 0x1000,
        QUIT            = 0x2000,

        INPUT_ALL       = MOUSEMOVE | MOUSEBUTTONDOWN | MOUSEBUTTONUP
        | MOUSECLICK | KEYDOWN | KEYUP | KEYCLICK | MOUSEENTER | MOUSELEAVE | MOUSEWHEELUP | MOUSEWHEELDOWN,

        INPUT_ALLMOUSE  = MOUSEMOVE | MOUSEBUTTONDOWN | MOUSEBUTTONUP | MOUSECLICK | MOUSEENTER | MOUSELEAVE | MOUSEWHEELUP | MOUSEWHEELDOWN
    };

    enum Mouse_button_t
    {
        MOUSE_BUTTON1   = 0x01,
        MOUSE_BUTTON2   = 0x02,
        MOUSE_BUTTON3   = 0x04
    };

    struct MouseInfo
    {
        unsigned int x,y;
        int          xmove,ymove;
        unsigned int buttonState;
    };

    struct KeyInfo
    {
        Key::Keysym_code_t keySym;
        unsigned int modifiers; // bitwise combination of Keysym_modifier_t
        unsigned int unicode; // used on TEXTENTERED event
    };

    struct ResizeInfo
    {
        int w,h;
    };

    struct Event
    {
        Event_type_t type;
        KeyInfo key;
        MouseInfo mouse;
        ResizeInfo resize;
    };

    // translate an SFML event to a SORE event
    Event TranslateEvent(const sf::Event& sfmlEvent);
}

#endif
