/**************************************************************************
 * Copyright 2010 Adam Kidder. All rights reserved.                       *
 *                                                                        *
 * Redistribution and use in source and binary forms, with or without     *
 * SMODification, are permitted provided that the following conditions     *
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

#ifndef SORE_INPUT_KEYSYM_H
#define SORE_INPUT_KEYSYM_H

#include <SFML/Window/Event.hpp>
 
namespace SORE_Input
{
    enum Keysym_code_t
    {
        SSYM_TAB          = sf::Key::Tab,
        SSYM_RETURN       = sf::Key::Return,
        SSYM_BACKSPACE    = sf::Key::Back,
        SSYM_PAUSE        = sf::Key::Pause,
        SSYM_ESCAPE       = sf::Key::Escape,
        SSYM_SPACE        = sf::Key::Space,
        SSYM_QUOTE        = sf::Key::Quote,
        SSYM_COMMA        = sf::Key::Comma,
        SSYM_MINUS        = sf::Key::Dash,
        SSYM_PERIOD       = sf::Key::Period,
        SSYM_SLASH        = sf::Key::Slash,
        SDLK_0            = sf::Key::Num0,
        SDLK_1            = sf::Key::Num1,
        SDLK_2            = sf::Key::Num2,
        SDLK_3            = sf::Key::Num3,
        SDLK_4            = sf::Key::Num4,
        SDLK_5            = sf::Key::Num5,
        SDLK_6            = sf::Key::Num6,
        SDLK_7            = sf::Key::Num7,
        SDLK_8            = sf::Key::Num8,
        SDLK_9            = sf::Key::Num9,
        SSYM_SEMICOLON    = sf::Key::SemiColon,
        SSYM_EQUALS       = sf::Key::Equal,

        SSYM_LEFTBRACKET  = sf::Key::LBracket,
        SSYM_BACKSLASH    = sf::Key::BackSlash,
        SSYM_RIGHTBRACKET = sf::Key::RBracket,
        SSYM_a            = 'a',
        SSYM_b            = 'b',
        SSYM_c            = 'c',
        SSYM_d            = 'd',
        SSYM_e            = 'e',
        SSYM_f            = 'f',
        SSYM_g            = 'g',
        SSYM_h            = 'h',
        SSYM_i            = 'i',
        SSYM_j            = 'j',
        SSYM_k            = 'k',
        SSYM_l            = 'l',
        SSYM_m            = 'm',
        SSYM_n            = 'n',
        SSYM_o            = 'o',
        SSYM_p            = 'p',
        SSYM_q            = 'q',
        SSYM_r            = 'r',
        SSYM_s            = 's',
        SSYM_t            = 't',
        SSYM_u            = 'u',
        SSYM_v            = 'v',
        SSYM_w            = 'w',
        SSYM_x            = 'x',
        SSYM_y            = 'y',
        SSYM_z            = 'z',

        SSYM_NUMPAD0      = sf::Key::Numpad0,
        SSYM_NUMPAD1      = sf::Key::Numpad1,
        SSYM_NUMPAD2      = sf::Key::Numpad2,
        SSYM_NUMPAD3      = sf::Key::Numpad3,
        SSYM_NUMPAD4      = sf::Key::Numpad4,
        SSYM_NUMPAD5      = sf::Key::Numpad5,
        SSYM_NUMPAD6      = sf::Key::Numpad6,
        SSYM_NUMPAD7      = sf::Key::Numpad7,
        SSYM_NUMPAD8      = sf::Key::Numpad8,
        SSYM_NUMPAD9      = sf::Key::Numpad9,
        SSYM_ADD          = sf::Key::Add,
        SSYM_SUBTRACT     = sf::Key::Subtract,
        SSYM_MULTIPLY     = sf::Key::Multiply,
        SSYM_DIVIDE       = sf::Key::Divide,

        SSYM_UP           = sf::Key::Up,
        SSYM_DOWN         = sf::Key::Down,
        SSYM_RIGHT        = sf::Key::Right,
        SSYM_LEFT         = sf::Key::Left,
        SSYM_INSERT       = sf::Key::Insert,
        SSYM_DELETE       = sf::Key::Delete,
        SSYM_HOME         = sf::Key::Home,
        SSYM_END          = sf::Key::End,
        SSYM_PAGEUP       = sf::Key::PageUp,
        SSYM_PAGEDOWN     = sf::Key::PageDown,

        SSYM_F1           = sf::Key::F1,
        SSYM_F2           = sf::Key::F2,
        SSYM_F3           = sf::Key::F3,
        SSYM_F4           = sf::Key::F4,
        SSYM_F5           = sf::Key::F5,
        SSYM_F6           = sf::Key::F6,
        SSYM_F7           = sf::Key::F7,
        SSYM_F8           = sf::Key::F8,
        SSYM_F9           = sf::Key::F9,
        SSYM_F10          = sf::Key::F10,
        SSYM_F11          = sf::Key::F11,
        SSYM_F12          = sf::Key::F12,
        SSYM_F13          = sf::Key::F13,
        SSYM_F14          = sf::Key::F14,
        SSYM_F15          = sf::Key::F15,

        SSYM_RSHIFT       = sf::Key::RShift,
        SSYM_LSHIFT       = sf::Key::LShift,
        SSYM_RCTRL        = sf::Key::RControl,
        SSYM_LCTRL        = sf::Key::LControl,
        SSYM_RALT         = sf::Key::RAlt,
        SSYM_LALT         = sf::Key::LAlt,
        SSYM_LSUPER       = sf::Key::LSystem,        /* Left "Windows" key */
        SSYM_RSUPER       = sf::Key::RSystem,        /* Right "Windows" key */

        SSYM_MENU         = sf::Key::Menu
    };

    enum Keysym_modifier_t
    {
        // could support these by keeping track of pressed/relased on the modifiers...
        /*SMOD_NONE   = KMOD_NONE,
        SMOD_LSHIFT = KMOD_LSHIFT,
        SMOD_RSHIFT = KMOD_RSHIFT,
        SMOD_LCTRL  = KMOD_LCTRL,
        SMOD_RCTRL  = KMOD_RCTRL,
        SMOD_LALT   = KMOD_LALT,
        SMOD_RALT   = KMOD_RALT,
        SMOD_LMETA  = KMOD_LMETA,
        SMOD_RMETA  = KMOD_RMETA,
        SMOD_NUM    = KMOD_NUM,
        SMOD_CAPS   = KMOD_CAPS,
        SMOD_MODE   = KMOD_MODE,
        
        SMOD_META*/

        SMOD_NONE  = 0x0,
        SMOD_CTRL  = 0x1,
        SMOD_SHIFT = 0x2,
        SMOD_ALT   = 0x4
    };
}

#endif
