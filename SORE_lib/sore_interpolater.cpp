
#include "sore_interpolater.h"

namespace SORE_Utility
{	
	InterpolaterTask::InterpolaterTask(SORE_Kernel::GameKernel* gk) : Task(gk)
	{
	}
	
	InterpolaterTask::~InterpolaterTask()
	{
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
				interpolaters.erase(temp);
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
