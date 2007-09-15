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
	state.pos = initial.pos + d.dPos*dt;
	state.vel = initial.vel + d.dVel*dt;

	ObjectDerivative output;
	output.dPos = state.vel;
	output.dVel = SumForces(state, dt);
	return output;
}

ObjectDerivative Evaluate(const ObjectState& initial)
{
	ObjectDerivative output;
	output.dPos = initial.vel;
	output.dVel = SumForces(initial, 0);
	return output;
}

void Integrate(ObjectState& state, int dt)
{
	ObjectDerivative a = Evaluate(state);                    // note: overloaded 'evaluate' just returns derivative at current time 't'
	ObjectDerivative b = Evaluate(state, dt*0.5, a);
	ObjectDerivative c = Evaluate(state, dt*0.5, b);
	ObjectDerivative d = Evaluate(state, dt, c);

	//const float dxdt = 1.0f/6.0f * (a.dx + 2.0f*(b.dx + c.dx) + d.dx);
	//const float dvdt = 1.0f/6.0f * (a.dv + 2.0f*(b.dv + c.dv) + d.dv);
	
	Vector3D<double> dxdt = 1.0/6.0 * (a.dPos + 2.0*(b.dPos + c.dPos) + d.dPos);
	Vector3D<double> dvdt = 1.0/6.0 * (a.dVel + 2.0*(b.dVel + c.dVel) + d.dVel);

	double delta = double(dt);
	
	state.pos = state.pos + dxdt * delta;
	state.vel = state.vel + dvdt * delta;
}

Vector3D<double> SumForces(ObjectState state, double dt)
{
	Vector3D<double> f (0.0, -0.00001, 0.0);
	
	if(state.vel.GetValue()[1]<0.0 && state.pos.GetValue()[1]<5.0)
		f += -(state.vel*10.0);
	
	return f;
}

PhysicsBall::PhysicsBall()
{
	derivative.dPos.Set(0.0,0.0,0.0);
	derivative.dVel.Set(0.0,0.0,0.0);
}

PhysicsBall::PhysicsBall(double x, double y, double z)
{
	state.pos.Set(x,y,z);
	derivative.dPos.Set(0.0,0.0,0.0);
	derivative.dVel.Set(0.0,0.0,0.0);
}

void PhysicsBall::Zero()
{
	state.pos.Set(0,0,0);
	state.vel.Set(0,0,0);
	derivative.dPos.Set(0.0,0.0,0.0);
	derivative.dVel.Set(0.0,0.0,0.0);
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
