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
	//primary
	Vector3D<double> momentum;
	Vector3D<double> position;
	
	//secondary
	Vector3D<double> velocity;
	
	protected:
	//constant
	double mass;
	double inverseMass;
	
	public:
	void recalculate()
	{
		velocity = momentum * inverseMass;
	}
	void set_mass(double newMass)
	{
		mass = newMass;
		inverseMass = 1.0/mass;
	}
	double get_mass() const {return mass;}
};

struct ObjectDerivative
{
	Vector3D<double> force;
	Vector3D<double> velocity;
};

class PhysicsObject
{
	public:
		//virtual void Update(int elapsedTime) = 0;
		double* GetPosition() {return state.position.GetValue();}
		void MoveTo(double x, double y, double z) {state.position.Set(x,y,z);}
		
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
		PhysicsTask(SORE_Kernel::GameKernel* gk): Task(gk) {updating = false;}
		void Frame(int elapsedTime);
		void Resume();
		void Pause();
		void AddObject(PhysicsObject obj);
		
		const char* GetName() const {return "Physics Task";}
		
		ObjectDerivative Evaluate(const ObjectState& initial, double dt, const ObjectDerivative& d);
		ObjectDerivative Evaluate(const ObjectState& initial);

		Vector3Dd SumForces(ObjectState state, double dt);
		void       Integrate(ObjectState& state, int dt);
		Vector3Dd collisionGround(ObjectState state);
		Vector3Dd collisionBalls(ObjectState state, std::vector<PhysicsObject> balls);
		
		bool PhysicsCallback(SORE_Kernel::Event* event);
		
		int GetNumObjs();
		ObjectState GetState(int obj);

	protected:
		std::vector<PhysicsObject> objs;
		void Update(PhysicsObject* obj, int elapsedTime);
		bool updating;
};

#endif /*__PHYSICS_H__*/
