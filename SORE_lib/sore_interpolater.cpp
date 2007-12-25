//
// C++ Implementation: sore_interpolater
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "sore_interpolater.h"

namespace SORE_Utility
{
	int IInterpolater::open = 0;
	
	InterpolaterTask::InterpolaterTask(SORE_Kernel::GameKernel* gk) : Task(gk)
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
		interpolater_iterator it = --interpolaters.end();
		return it;
	}
	
	void InterpolaterTask::RemoveInterpolater(interpolater_iterator i)
	{
		//delete *i;
		//oldInterpolaters.push_back(*i);
		interpolaters.erase(i);
	}
}
