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
	InterpolaterTask::InterpolaterTask(SORE_Kernel::GameKernel* gk) : Task(gk)
	{
	}
	
	InterpolaterTask::~InterpolaterTask()
	{
		std::vector<IInterpolater*>::iterator it, temp;
		for(it=interpolaters.begin();it<interpolaters.end();)
		{
			temp = it;
			it++;
			delete *temp;
		}
	}
	
	void InterpolaterTask::Frame(int elapsedTime)
	{
		std::vector<IInterpolater*>::iterator it, temp;
		for(it=interpolaters.begin();it<interpolaters.end();)
		{
			temp = it;
			it++;
			(*temp)->Frame(elapsedTime);
			if((*temp)->Done())
			{
				delete *temp;
				interpolaters.erase(temp);
				
			}
		}
	}
	
	void InterpolaterTask::Pause()
	{
	}
	
	void InterpolaterTask::Resume()
	{
	}
	
	void InterpolaterTask::AddInterpolater(IInterpolater* i)
	{
		interpolaters.push_back(i);
	}
}
