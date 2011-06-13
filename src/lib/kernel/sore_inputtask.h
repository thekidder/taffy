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

#ifndef SORE_INPUTTASK_H
#define SORE_INPUTTASK_H

#include <sore_event.h>
#include <sore_task.h>

#include <queue>

namespace SORE_Kernel
{
    class Screen;

    // Polls for all SFML inputs and allows users to inject events
    // Interface for collected events is via a queue
    class SORE_EXPORT InputTask : public Task
    {
    public:
        InputTask(SORE_Kernel::Screen& screen_);

        void Frame(int elapsedTime);

        const char* GetName() const {return "Input task";}

        //add an event
        void InjectEvent(const SORE_Kernel::Event& e);

        // access the event queue
        size_t size() { return allEvents.size(); }
        Event& top() { return allEvents.front(); }
        void pop() { allEvents.pop(); }

        bool QuitEventReceived() const { return quitEvent; }
    private:
        //processes the event, either a user-injected event or one from SFML
        void HandleEvent(const Event& event);

        SORE_Kernel::Screen& screen;

        std::queue<Event> allEvents;
        bool quitEvent;
    };
}

#endif
