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

#include <sore_logger.h>
#include <sore_task.h>

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
