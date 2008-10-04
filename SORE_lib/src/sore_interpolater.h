//
// C++ Interface: sore_interpolater
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
// $Id$


#ifndef  __SORE_INTERPOLATER_H__
#define  __SORE_INTERPOLATER_H__

#include "sore_task.h"
#include "sore_logger.h"
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace SORE_Utility
{
	//we need to define a template-less interface so we can have a heterogeneous list
	class IInterpolater
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
	template<class T>
			class Interpolater : public IInterpolater
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
	
	template<class T>
			class LinearInterpolater : public Interpolater<T>
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
	
	template<class T>
			class LinearTwoInterpolater : public Interpolater<T>
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
	
	class InterpolaterTask  : public SORE_Kernel::Task
	{
		public:
			InterpolaterTask(SORE_Kernel::GameKernel* gk);
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

#endif /*__SORE_INTERPOLATER_H__*/
