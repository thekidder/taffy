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
* THIS SOFTWARE IS PROVIDED BY <COPYRIGHT HOLDER> ``AS IS'' AND ANY      *
* EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE      *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR     *
* PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> OR        *
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

#include <cassert>
#include <unistd.h>
#include <sys/time.h>

#include <sore_timing.h>
#include <sore_logger.h>

namespace SORE_Timing
{
    unsigned int GetGlobalTicks() //in 1/10000 second increments
    {
        static unsigned long start_s = 0;
        static long start_us = 0;

        timeval tv;
        gettimeofday(&tv, NULL);

        if(start_s == 0 && start_us==0)
        {
            start_s  = tv.tv_sec;
            start_us = tv.tv_usec;
        }
        unsigned int nano_s  = 10000*(tv.tv_sec-start_s);
        int          nano_us = (tv.tv_usec-start_us)/100;
        unsigned int time = nano_s + nano_us;
        if(time<0)
        {
            ENGINE_LOG(SORE_Logging::LVL_CRITICAL, boost::format("Global ticks count is less than zero (ticks=%s)") % time);
        }
        return time;
    }

    void Sleep(unsigned int ms)
    {
        usleep(ms*1000);
    }
}
