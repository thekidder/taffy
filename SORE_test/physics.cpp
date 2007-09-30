//
// C++ Implementation: physics
//
// Description: 
//
//
// Author: Adam Kidder <thekidder@gmail.com>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "physics.h"
#include "main.h"

ObjectDerivative Evaluate(const ObjectState& initial, double dt, const ObjectDerivative& d)
{
	ObjectState state;
	state.position = initial.position + d.velocity*dt;
	state.momentum = initial.momentum + d.force*dt;

	state.recalculate();
	
	ObjectDerivative output;
	output.velocity = state.velocity;
	output.force = SumForces(state, dt);
	return output;
}

ObjectDerivative Evaluate(const ObjectState& initial)
{
	ObjectDerivative output;
	output.velocity = initial.velocity;
	output.force = SumForces(initial, 0);
	return output;
}

void Integrate(ObjectState& state, int dt)
{
	ObjectDerivative a = Evaluate(state);                    // note: overloaded 'evaluate' just returns derivative at current time 't'
	ObjectDerivative b = Evaluate(state, dt*0.5, a);
	ObjectDerivative c = Evaluate(state, dt*0.5, b);
	ObjectDerivative d = Evaluate(state, dt, c);

	Vector3D<double> dxdt = 1.0/6.0 * (a.velocity + 2.0*(b.velocity + c.velocity) + d.velocity);
	Vector3D<double> dvdt = 1.0/6.0 * (a.force + 2.0*(b.force + c.force) + d.force);
	
	state.position = state.position + dxdt * double(dt);
	state.momentum = state.momentum + dvdt * double(dt);
	state.recalculate();
	//APP_LOG(SORE_Logging::LVL_DEBUG2, "position: %f", state.position.GetValue()[1]);
}

Vector3D<double> SumForces(ObjectState state, double dt)
{
	Vector3D<double> f (0.0, -0.000001, 0.0);
	
	if(state.momentum.GetValue()[1]<0.0 && state.position.GetValue()[1]<5.0)
	{
		//f += -(state.momentum);
	}
	
	return f;
}

PhysicsBall::PhysicsBall()
{
	derivative.velocity.Set(0.0,0.0,0.0);
	derivative.force.Set(0.0,0.0,0.0);
	state.set_mass(1.0);
}

PhysicsBall::PhysicsBall(double x, double y, double z)
{
	state.position.Set(x,y,z);
	derivative.velocity.Set(0.0,0.0,0.0);
	derivative.force.Set(0.0,0.0,0.0);
	state.set_mass(1.0);
}

void PhysicsBall::Zero()
{
	state.position.Set(0,0,0);
	state.momentum.Set(0,0,0);
	derivative.velocity.Set(0.0,0.0,0.0);
	derivative.force.Set(0.0,0.0,0.0);
}

/*void PhysicsBall::Update(int elapsedTime)
{
}*/

void PhysicsTask::Frame(int elapsedTime)
{
	std::vector<PhysicsObject*>::iterator it;
	for(it=objs.begin();it<objs.end();it++)
	{
		Update(*it, elapsedTime);
	}
}

void PhysicsTask::Update(PhysicsObject* obj, int elapsedTime)
{
	Integrate(obj->state, elapsedTime);
}

void PhysicsTask::Resume()
{
}

void PhysicsTask::Pause()
{
}

void PhysicsTask::AddObject(PhysicsObject* obj)
{
	objs.push_back(obj);
}
