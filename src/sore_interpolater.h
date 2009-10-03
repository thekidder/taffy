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

#ifndef  SORE_INTERPOLATER_H
#define  SORE_INTERPOLATER_H

//MSVC++ template-exporting warning
#ifdef _MSC_VER
#pragma warning( push )
#pragma warning( disable : 4251 )
#endif

#include <list>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "sore_logger.h"
#include "sore_task.h"

namespace SORE_Utility
{
	//we need to define a template-less interface so we can have a heterogeneous list
	class SORE_EXPORT IInterpolater
	{
		public:
			IInterpolater() { ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Created interpolater (%d interpolaters opened)") % open); }
			virtual ~IInterpolater() { ENGINE_LOG(SORE_Logging::LVL_DEBUG2, boost::format("Destroyed interpolater (%d interpolaters opened)") % open); }
			virtual void Frame(int elapsedTime)  = 0;
			virtual void Update(int elapsedTime) = 0;
			virtual bool Done() = 0;
		protected:
			static int open;
	};
	
	typedef std::list<boost::shared_ptr<IInterpolater> >::iterator interpolater_iterator;
	
	//Interpolater interface. Interpolaters take an input value and apply an operation on it each timestep
	template<typename T>
			class SORE_EXPORT Interpolater : public IInterpolater
	{
		public:
			Interpolater(T input, boost::function<void (T)> callback)
			{
				interpolatedValue = input;
				func = callback;
				func(input);
				death = NULL;
				open++;
			}
			
			~Interpolater()
			{
				if(death)
					death(interpolatedValue);
				open--;
			}
			
			void Frame(int elapsedTime)
			{
				Update(elapsedTime);
				func(interpolatedValue);
			}
			
			virtual bool Done() { return false; }
			
			virtual void Update(int elapsedTime) {}
			
		protected:
			T interpolatedValue;
			boost::function<void (T)> func;
			boost::function<void (T)> death;
	};
	
	template<typename T>
			class SORE_EXPORT LinearInterpolater : public Interpolater<T>
	{
		public:
			LinearInterpolater(T input, T _factor, T _limit, boost::function<void (T)> callback) : Interpolater<T>(input, callback)
			{
				factor = _factor;
				done = false;
				limit = _limit;
			}
			
			void Update(int elapsedTime)
			{
				Interpolater<T>::interpolatedValue += elapsedTime*factor;
				if(factor>0 && Interpolater<T>::interpolatedValue>limit) 
					done = true;
				else if(factor<0 && Interpolater<T>::interpolatedValue<limit) 
					done = true;
			}
			
			bool Done()
			{
				return done;
			}
			
		protected:
			double factor;
			double limit;
			bool done;
	};
	
	template<typename T>
			class SORE_EXPORT LinearTwoInterpolater : public Interpolater<T>
	{
		public:
			LinearTwoInterpolater(T input, T _factor, T _limit1, T _limit2, bool _repeat, boost::function<void (T)> callback, boost::function<void (T)> onDeath) : Interpolater<T>(input, callback)
			{
				factor = _factor;
				limit1 = _limit1;
				limit2 = _limit2;
				repeat = _repeat;
				done = 0;
				Interpolater<T>::death = onDeath;
			}
			
			void Update(int elapsedTime)
			{
				Interpolater<T>::interpolatedValue += elapsedTime*factor;
				if(factor>0 && Interpolater<T>::interpolatedValue>limit2)
				{
					factor = -factor;
					done++;
				}
				else if(factor<0 && Interpolater<T>::interpolatedValue<limit1) 
				{
					factor = -factor;
					done++;
				}
			}
			
			bool Done()
			{
				if(!repeat)
					return done>=2;
				else return false;
			}
			
		protected:
			double factor;
			double limit1,limit2;
			bool repeat;
			int done;
	};
	
	class SORE_EXPORT InterpolaterTask  : public SORE_Kernel::Task
	{
		public:
			InterpolaterTask();
			~InterpolaterTask();
			
			const char* GetName() const { return "Interpolater task"; };
			
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
			
			interpolater_iterator AddInterpolater(boost::shared_ptr<IInterpolater> i);
			void           RemoveInterpolater(interpolater_iterator i);
		protected:
			std::list<boost::shared_ptr<IInterpolater> > interpolaters;
			std::list<boost::shared_ptr<IInterpolater> > oldInterpolaters;
	};
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
