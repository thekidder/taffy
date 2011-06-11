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

namespace SORE_Input
{
    enum Keysym_code_t
    {
        //SSYM_UNKNOWN      = SDLK_UNKNOWN,
        //SSYM_FIRST        = SDLK_FIRST,
        SSYM_BACKSPACE,//    = SDLK_BACKSPACE,
        //SSYM_TAB          = SDLK_TAB,
        //SSYM_CLEAR        = SDLK_CLEAR,
        //SSYM_RETURN       = SDLK_RETURN,
        //SSYM_PAUSE        = SDLK_PAUSE,
        //SSYM_ESCAPE       = SDLK_ESCAPE,
        //SSYM_SPACE        = SDLK_SPACE,
        //SSYM_EXCLAIM      = SDLK_EXCLAIM,
        //SSYM_QUOTEDBL     = SDLK_QUOTEDBL,
        //SSYM_HASH         = SDLK_HASH,
        //SSYM_DOLLAR       = SDLK_DOLLAR,
        //SSYM_AMPERSAND    = SDLK_AMPERSAND,
        //SSYM_QUOTE        = SDLK_QUOTE,
        //SSYM_LEFTPAREN    = SDLK_LEFTPAREN,
        //SSYM_RIGHTPAREN   = SDLK_RIGHTPAREN,
        //SSYM_ASTERISK     = SDLK_ASTERISK,
        //SSYM_PLUS         = SDLK_PLUS,
        //SSYM_COMMA        = SDLK_COMMA,
        //SSYM_MINUS        = SDLK_MINUS,
        //SSYM_PERIOD       = SDLK_PERIOD,
        //SSYM_SLASH        = SDLK_SLASH,
        //SDLK_0            = SDLK_0,
        //SDLK_1            = SDLK_1,
        //SDLK_2            = SDLK_2,
        //SDLK_3            = SDLK_3,
        //SDLK_4            = SDLK_4,
        //SDLK_5            = SDLK_5,
        //SDLK_6            = SDLK_6,
        //SDLK_7            = SDLK_7,
        //SDLK_8            = SDLK_8,
        //SDLK_9            = SDLK_9,
        //SSYM_COLON        = SDLK_COLON,
        //SSYM_SEMICOLON    = SDLK_SEMICOLON,
        //SSYM_LESS         = SDLK_LESS,
        //SSYM_EQUALS       = SDLK_EQUALS,
        //SSYM_GREATER      = SDLK_GREATER,
        //SSYM_QUESTION     = SDLK_QUESTION,
        //SSYM_AT           = SDLK_AT,
        ///*
        //   Skip uppercase letters
        // */
        //SSYM_LEFTBRACKET  = SDLK_LEFTBRACKET,
        //SSYM_BACKSLASH    = SDLK_BACKSLASH,
        //SSYM_RIGHTBRACKET = SDLK_RIGHTBRACKET,
        //SSYM_CARET        = SDLK_CARET,
        //SSYM_UNDERSCORE   = SDLK_UNDERSCORE,
        //SSYM_BACKQUOTE    = SDLK_BACKQUOTE,
        //SSYM_a            = SDLK_a,
        //SSYM_b            = SDLK_b,
        //SSYM_c            = SDLK_c,
        //SSYM_d            = SDLK_d,
        //SSYM_e            = SDLK_e,
        //SSYM_f            = SDLK_f,
        //SSYM_g            = SDLK_g,
        //SSYM_h            = SDLK_h,
        //SSYM_i            = SDLK_i,
        //SSYM_j            = SDLK_j,
        //SSYM_k            = SDLK_k,
        //SSYM_l            = SDLK_l,
        //SSYM_m            = SDLK_m,
        //SSYM_n            = SDLK_n,
        //SSYM_o            = SDLK_o,
        //SSYM_p            = SDLK_p,
        //SSYM_q            = SDLK_q,
        //SSYM_r            = SDLK_r,
        //SSYM_s            = SDLK_s,
        //SSYM_t            = SDLK_t,
        //SSYM_u            = SDLK_u,
        //SSYM_v            = SDLK_v,
        //SSYM_w            = SDLK_w,
        //SSYM_x            = SDLK_x,
        //SSYM_y            = SDLK_y,
        //SSYM_z            = SDLK_z,
        //SSYM_DELETE       = SDLK_DELETE,
        ///* End of ASCII mapped keySSYMs */

        ///* Numeric keypad */
        //SDLK_KP0          = SDLK_KP0,
        //SDLK_KP1          = SDLK_KP1,
        //SDLK_KP2          = SDLK_KP2,
        //SDLK_KP3          = SDLK_KP3,
        //SDLK_KP4          = SDLK_KP4,
        //SDLK_KP5          = SDLK_KP5,
        //SDLK_KP6          = SDLK_KP6,
        //SDLK_KP7          = SDLK_KP7,
        //SDLK_KP8          = SDLK_KP8,
        //SDLK_KP9          = SDLK_KP9,
        //SDLK_KP_PERIOD    = SDLK_KP_PERIOD,
        //SDLK_KP_DIVIDE    = SDLK_KP_DIVIDE,
        //SDLK_KP_MULTIPLY  = SDLK_KP_MULTIPLY,
        //SDLK_KP_MINUS     = SDLK_KP_MINUS,
        //SDLK_KP_PLUS      = SDLK_KP_PLUS,
        //SDLK_KP_ENTER     = SDLK_KP_ENTER,
        //SDLK_KP_EQUALS    = SDLK_KP_EQUALS,

        ///* Arrows + Home/End pad */
        //SSYM_UP           = SDLK_UP,
        //SSYM_DOWN         = SDLK_DOWN,
        SSYM_RIGHT,//        = SDLK_RIGHT,
        SSYM_LEFT,//         = SDLK_LEFT,
        //SSYM_INSERT       = SDLK_INSERT,
        //SSYM_HOME         = SDLK_HOME,
        //SSYM_END          = SDLK_END,
        //SSYM_PAGEUP       = SDLK_PAGEUP,
        //SSYM_PAGEDOWN     = SDLK_PAGEDOWN,

        ///* Function keys */
        //SDLK_F1           = SDLK_F1,
        //SDLK_F2           = SDLK_F2,
        //SDLK_F3           = SDLK_F3,
        //SDLK_F4           = SDLK_F4,
        //SDLK_F5           = SDLK_F5,
        //SDLK_F6           = SDLK_F6,
        //SDLK_F7           = SDLK_F7,
        //SDLK_F8           = SDLK_F8,
        //SDLK_F9           = SDLK_F9,
        //SDLK_F10          = SDLK_F10,
        //SDLK_F11          = SDLK_F11,
        //SDLK_F12          = SDLK_F12,
        //SDLK_F13          = SDLK_F13,
        //SDLK_F14          = SDLK_F14,
        //SDLK_F15          = SDLK_F15,

        ///* Key state SMODifier keys */
        //SSYM_NUMLOCK      = SDLK_NUMLOCK,
        //SSYM_CAPSLOCK     = SDLK_CAPSLOCK,
        //SSYM_SCROLLOCK    = SDLK_SCROLLOCK,
        //SSYM_RSHIFT       = SDLK_RSHIFT,
        //SSYM_LSHIFT       = SDLK_LSHIFT,
        //SSYM_RCTRL        = SDLK_RCTRL,
        //SSYM_LCTRL        = SDLK_LCTRL,
        //SSYM_RALT         = SDLK_RALT,
        //SSYM_LALT         = SDLK_LALT,
        //SSYM_RMETA        = SDLK_RMETA,
        //SSYM_LMETA        = SDLK_LMETA,
        //SSYM_LSUPER       = SDLK_LSUPER,        /* Left "Windows" key */
        //SSYM_RSUPER       = SDLK_RSUPER,        /* Right "Windows" key */
        //SSYM_MODE         = SDLK_MODE,        /* "Alt Gr" key */
        //SSYM_COMPOSE      = SDLK_COMPOSE,        /* Multi-key compose key */

        ///* Miscellaneous function keys */
        //SSYM_HELP         = SDLK_HELP,
        //SSYM_PRINT        = SDLK_PRINT,
        //SSYM_SYSREQ       = SDLK_SYSREQ,
        //SSYM_BREAK        = SDLK_BREAK,
        //SSYM_MENU         = SDLK_MENU,
        //SSYM_POWER        = SDLK_POWER,        /* Power Macintosh power key */
        //SSYM_EURO         = SDLK_EURO,        /* Some european keyboards */
        //SSYM_UNDO         = SDLK_UNDO,        /* Atari keyboard has Undo */
    };

    enum Keysym_modifier_t
    {
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

        SMOD_CTRL  = SMOD_LCTRL  | SMOD_RCTRL,
        SMOD_SHIFT = SMOD_LSHIFT | SMOD_RSHIFT,
        SMOD_ALT   = SMOD_LALT   | SMOD_RALT,
        SMOD_META  = SMOD_LMETA  | SMOD_RMETA*/
    };
}

#endif
