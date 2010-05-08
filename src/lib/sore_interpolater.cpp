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

#include <sore_interpolater.h>

namespace SORE_Utility
{
	int IInterpolater::open = 0;
	
	InterpolaterTask::InterpolaterTask()
	{
	}
	
	InterpolaterTask::~InterpolaterTask()
	{
		interpolater_iterator it, temp;
		for(it=interpolaters.begin();it!=interpolaters.end();)
		{
			temp = it;
			it++;
			//delete *temp;
		}
		for(it=oldInterpolaters.begin();it!=oldInterpolaters.end();)
		{
			temp = it;
			it++;
			//delete *temp;
		}
	}
	
	void InterpolaterTask::Frame(int elapsedTime)
	{
		interpolater_iterator it, temp;
		for(it=interpolaters.begin();it!=interpolaters.end();)
		{
			temp = it;
			it++;
			(*temp)->Frame(elapsedTime);
			if((*temp)->Done())
			{
				RemoveInterpolater(temp);
			}
		}
	}
	
	void InterpolaterTask::Pause()
	{
	}
	
	void InterpolaterTask::Resume()
	{
	}
	
	interpolater_iterator InterpolaterTask::AddInterpolater(boost::shared_ptr<IInterpolater> i)
	{
		interpolaters.push_back(i);
		interpolater_iterator it = interpolaters.end();
		it--;
		return it;
	}
	
	void InterpolaterTask::RemoveInterpolater(interpolater_iterator i)
	{
		//delete *i;
		//oldInterpolaters.push_back(*i);
		interpolaters.erase(i);
	}
}
