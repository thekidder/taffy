/***************************************************************************
 *   Copyright (C) 2009 by Adam Kidder                                     *
 *   thekidder@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU Lesser General Public License for more details.                   *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
// $Id$

#include "sore_interpolater.h"

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
