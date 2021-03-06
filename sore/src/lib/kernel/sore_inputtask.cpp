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

#include <sore_inputtask.h>
#include <sore_screen.h>

namespace SORE_Kernel
{
    InputTask::InputTask(SORE_Kernel::Screen& screen_) 
        : screen(screen_), quitEvent(false)
    {
        lastMouseMoveEvent.type = NOEVENT;
    }

    void InputTask::HandleEvent(const Event& event)
    {
        switch(event.type)
        {
        case NOEVENT:
            return;
        case QUIT:
            quitEvent = true;
            break;
        case RESIZE:
            // keep the GL context up to date
            screen.OnResize(event);
            break;
        default:
            break;
        }

        // emulate xmove, ymove
        if(event.type & INPUT_ALLMOUSE && lastMouseMoveEvent.type != NOEVENT)
        {
            Event copy = event;
            copy.mouse.xmove = copy.mouse.x - lastMouseMoveEvent.mouse.x;
            copy.mouse.ymove = copy.mouse.y - lastMouseMoveEvent.mouse.y;

            allEvents.push(copy);
        }
        else
        {
            allEvents.push(event);
        }
        if(event.type == MOUSEMOVE)
        {
            lastMouseMoveEvent = event;
        }
    }

    void InputTask::Frame(int elapsedTime)
    {
        sf::Event sfmlEvent;
        while(screen.window.GetEvent(sfmlEvent))
        {
            SORE_Kernel::Event event = SORE_Kernel::TranslateEvent(sfmlEvent);
            HandleEvent(event);
        }
    }

    void InputTask::InjectEvent(const SORE_Kernel::Event& e)
    {
        HandleEvent(e);
    }
}
