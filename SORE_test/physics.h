//
// C++ Interface: physics
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef  __PHYSICS_H__
#define  __PHYSICS_H__


#include <sore.h>
#include <vector>

using namespace SORE_Math;

struct ObjectState
{
	Vector3D<double> vel;
	Vector3D<double> pos;
};

struct ObjectDerivative
{
	Vector3D<double> dVel;
	Vector3D<double> dPos;
};

ObjectDerivative Evaluate(const ObjectState& initial, double dt, const ObjectDerivative& d);
ObjectDerivative Evaluate(const ObjectState& initial);

Vector3D<double> SumForces(ObjectState state, double dt);
void             Integrate(ObjectState& state, int dt);

class PhysicsObject
{
	public:
		//virtual void Update(int elapsedTime) = 0;
		double* GetPosition() {return state.pos.GetValue();}
		void MoveTo(double x, double y, double z) {state.pos.Set(x,y,z);}
		
		ObjectState      state;
		ObjectDerivative derivative;
};

class PhysicsBall : public PhysicsObject
{
	public:
		PhysicsBall();
		PhysicsBall(double x, double y, double z);
		//void Update(int elapsedTime);
		void Zero();
};

class PhysicsTask : public SORE_Kernel::Task
{
	public:
		void Frame(int elapsedTime);
		void Resume();
		void Pause();
		void AddObject(PhysicsObject* obj);
		
		const char* GetName() const {return "Physics Task";}
	protected:
		std::vector<PhysicsObject*> objs;
		void Update(PhysicsObject* obj, int elapsedTime);
};

#endif /*__PHYSICS_H__*/
