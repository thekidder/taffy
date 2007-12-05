
#ifndef  __SORE_INTERPOLATER_H__
#define  __SORE_INTERPOLATER_H__

#include <sore_kernel.h>

namespace SORE_Utility
{
	//lots of messy template code follows
	//the practical upshot is an easy-to-use, extensible, interpolation system
	template<class T>
			class InterpolaterFunctor
	{
		public:
			virtual void operator()(T){}
	};
	
	template<class T>
			class GlobalInterpolaterFunctor : public InterpolaterFunctor<T>
	{
		protected:
			typedef void (*INTERPOLATER_CALLBACK)(T);
			INTERPOLATER_CALLBACK func;
		public:
			GlobalInterpolaterFunctor(INTERPOLATER_CALLBACK _func)
			{
				func = _func;
			}
			bool operator()(T value)
			{
				return func(value);
			}
	};
	
	template<class T, class C>
			class ClassInterpolaterFunctor : public InterpolaterFunctor<T>
	{
		protected:
			typedef void (C::*INTERPOLATER_CALLBACK)(T);
			INTERPOLATER_CALLBACK func;
			C* obj;
		public:
			ClassInterpolaterFunctor(C* _obj, INTERPOLATER_CALLBACK _func)
			{
				obj = _obj;
				func = _func;
			}
			void operator()(T value)
			{
				(obj->*func)(value);
			}
	};
		
	template<class T>
			GlobalInterpolaterFunctor<T>* MakeFunctor(void(*func)(T))
	{
		return new GlobalInterpolaterFunctor<T>(func);
	}
	
	template<class T, class C>
			ClassInterpolaterFunctor<T,C>* MakeFunctor(C* obj, void(C::*func)(T))
	{
		return new ClassInterpolaterFunctor<T,C>(obj, func);
	}
	
	//we need to define a template-less interface so we can have a heterogeneous list
	class IInterpolater
	{
		public:
			virtual void Frame(int elapsedTime)  = 0;
			virtual void Update(int elapsedTime) = 0;
			virtual bool Done() = 0;
	};
	
	//Interpolater interface. Interpolaters take an input value and apply an operation on it each timestep
	template<class T>
			class Interpolater : public IInterpolater
	{
		public:
			Interpolater(T input, InterpolaterFunctor<T>* callback)
			{
				interpolatedValue = input;
				func = callback;
				(*func)(input);
			}
			
			void Frame(int elapsedTime)
			{
				Update(elapsedTime);
				(*func)(interpolatedValue);
			}
			
			virtual bool Done() { return false; }
			
		protected:
			T interpolatedValue;
			InterpolaterFunctor<T>* func;
	};
	
	//template<class T>
			class LinearInterpolater : public Interpolater<double>
	{
		public:
			LinearInterpolater(double input, double _factor, double _limit, InterpolaterFunctor<double>* callback) : Interpolater<double>(input, callback)
			{
				factor = _factor;
				done = false;
				limit = _limit;
			}
			
			void Update(int elapsedTime)
			{
				interpolatedValue += elapsedTime*factor;
				if(factor>0 && interpolatedValue>limit) 
					done = true;
				else if(factor<0 && interpolatedValue<limit) 
					done = true;
				//func(factor);
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
	
	class InterpolaterTask  : public SORE_Kernel::Task
	{
		public:
			InterpolaterTask(SORE_Kernel::GameKernel* gk);
			~InterpolaterTask();
			
			const char* GetName() const { return "InterpolaterTask"; };
			
			void Frame(int elapsedTime);
			void Pause();
			void Resume();
			
			void AddInterpolater(IInterpolater* i);
		protected:
			std::vector<IInterpolater*> interpolaters;
	};
}

#endif /*__SORE_INTERPOLATER_H__*/
