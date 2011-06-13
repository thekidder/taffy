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

#include <sore_event.h>

unsigned int GetModifiers(const sf::Event& sfmlEvent)
{
    unsigned int mod = SORE_Kernel::Key::SMOD_NONE;
    if(sfmlEvent.Key.Alt)
        mod |= SORE_Kernel::Key::SMOD_ALT;
    if(sfmlEvent.Key.Control)
        mod |= SORE_Kernel::Key::SMOD_CTRL;
    if(sfmlEvent.Key.Shift)
        mod |= SORE_Kernel::Key::SMOD_SHIFT;

    return mod;
}

SORE_Kernel::Event SORE_Kernel::TranslateEvent(const sf::Event& sfmlEvent)
{
    Event event;
    switch(sfmlEvent.Type)
    {
    case sf::Event::MouseMoved:
        event.type = MOUSEMOVE;
        event.mouse.xmove = sfmlEvent.MouseMove.X;
        event.mouse.ymove = sfmlEvent.MouseMove.Y;
        event.mouse.x = sfmlEvent.MouseMove.X;
        event.mouse.y = sfmlEvent.MouseMove.Y;
        break;
    case sf::Event::MouseButtonPressed:
        event.type = MOUSEBUTTONDOWN;
        switch(sfmlEvent.MouseButton.Button)
        {
        case sf::Mouse::Left:
            event.mouse.buttonState = MOUSE_BUTTON1;
            break;
        case sf::Mouse::Middle:
            event.mouse.buttonState = MOUSE_BUTTON3;
            break;
        case sf::Mouse::Right:
            event.mouse.buttonState = MOUSE_BUTTON2;
            break;
        default:
            event.mouse.buttonState = MOUSE_BUTTON3;
            break;
        }
        break;
    case sf::Event::MouseButtonReleased:
        event.type = MOUSEBUTTONUP;
        switch(sfmlEvent.MouseButton.Button)
        {
        case sf::Mouse::Left:
            event.mouse.buttonState = MOUSE_BUTTON1;
            break;
        case sf::Mouse::Middle:
            event.mouse.buttonState = MOUSE_BUTTON3;
            break;
        case sf::Mouse::Right:
            event.mouse.buttonState = MOUSE_BUTTON2;
            break;
        default:
            event.mouse.buttonState = MOUSE_BUTTON3;
            break;
        }
        break;
    case sf::Event::MouseWheelMoved:
        event.type = MOUSEBUTTONDOWN;
        if(sfmlEvent.MouseWheel.Delta > 0)
            event.mouse.buttonState = MOUSE_WHEELUP;
        else
            event.mouse.buttonState = MOUSE_WHEELDOWN;
        break;
    case sf::Event::KeyPressed:
        // keysyms are designed to map directly to SDL keysyms
        event.type = KEYDOWN;
        event.key.keySym = static_cast<Key::Keysym_code_t>(sfmlEvent.Key.Code);
        event.key.modifiers = GetModifiers(sfmlEvent);
        break;
    case sf::Event::KeyReleased:
        event.type = KEYUP;
        event.key.keySym = static_cast<Key::Keysym_code_t>(sfmlEvent.Key.Code);
        event.key.modifiers = GetModifiers(sfmlEvent);
        break;
    case sf::Event::TextEntered:
        event.type = TEXTENTERED;
        event.key.unicode = sfmlEvent.Text.Unicode;
        break;
    case sf::Event::Resized:
        event.type = RESIZE;
        event.resize.w = sfmlEvent.Size.Width;
        event.resize.h = sfmlEvent.Size.Height;
        break;
    case sf::Event::Closed:
        event.type = QUIT;
        break;
    default: //invalid event type
        event.type = NOEVENT;
        break;
    }

    return event;
}