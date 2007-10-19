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

//SORE_Graphics::TerrainGraph* tg;

ObjectDerivative PhysicsTask::Evaluate(const ObjectState& initial, double dt, const ObjectDerivative& d)
{
	ObjectState state;
	state.set_mass(initial.get_mass());
	state.position = initial.position + d.velocity*dt;
	state.momentum = initial.momentum + d.force*dt;

	state.recalculate();
	
	ObjectDerivative output;
	output.velocity = state.velocity;
	output.force = SumForces(state, dt);
	return output;
}

ObjectDerivative PhysicsTask::Evaluate(const ObjectState& initial)
{
	ObjectDerivative output;
	output.velocity = initial.velocity;
	output.force = SumForces(initial, 0);
	return output;
}

void PhysicsTask::Integrate(ObjectState& state, int dt)
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
	if(state.position[1]>15.0)
	{
		APP_LOG_S(SORE_Logging::LVL_DEBUG2, "lol");
	}
	//APP_LOG(SORE_Logging::LVL_DEBUG2, "position: %f", state.position.GetValue()[1]);
}

Vector3D<double> PhysicsTask::SumForces(ObjectState state, double dt)
{
	Vector3D<double> f (0.0, -0.00001, 0.0);
	
	f += collisionGround(state);
	f += collisionBalls(state, objs);

	return f;
}

Vector3Dd PhysicsTask::collisionGround(ObjectState state)
{
	double depth = tg->GetHeight(state.position[0], state.position[2])-state.position[1]+0.2;
	if(depth<0.0) return Vector3Dd(0.0,0.0,0.0);
	Vector3Dd normal = tg->GetNormal(state.position[0], state.position[2]);
	
	double k = 0.5;
	double b = 0.3;
	double f = 0.001;
	
	Vector3Dd force = normal*k*depth;
	Vector3Dd damping = normal*b*(normal.dot(state.velocity));
	
	Vector3Dd friction = -((normal*state.velocity.Magnitude()) + state.velocity)*f;
	
	return force - damping + friction;
}

Vector3Dd PhysicsTask::collisionBalls(ObjectState state, std::vector<PhysicsObject> balls)
{
	Vector3Dd total;
	for(int i=0;i<balls.size();i++)
	{
		PhysicsObject ball = balls[i];
		double diff = fabs(double(ball.state.position[0]-state.position[0])+(ball.state.position[1]-state.position[1])+(ball.state.position[2]-state.position[2]));
		if(diff>0.6 || diff<0.01) continue;
		double depth = SORE_Math::distance(state.position, ball.state.position);
		if(depth>0.4) continue;
		depth = fabs(0.4-depth);
		Vector3Dd normal((state.position - ball.state.position).Normalize());
		
		double k = 0.5;
		double b = 0.3;
		double f = 0.001;
	
		Vector3Dd force = normal*k*depth;
		Vector3Dd damping = normal*b*(normal.dot(state.velocity));
	
		Vector3Dd friction = -((normal*state.velocity.Magnitude()) + state.velocity)*f;
	
		total += force - damping;
		
	}
	return total;
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
	std::vector<PhysicsObject>::iterator it;
	for(it=objs.begin();it<objs.end();it++)
	{
		Update(&(*it), elapsedTime);
	}
}

void PhysicsTask::Update(PhysicsObject* obj, int elapsedTime)
{
	if(updating)
		Integrate(obj->state, elapsedTime);
}

void PhysicsTask::Resume()
{
}

void PhysicsTask::Pause()
{
}

void PhysicsTask::AddObject(PhysicsObject obj)
{
	objs.push_back(obj);
}

int PhysicsTask::GetNumObjs()
{
	return objs.size();
}

ObjectState PhysicsTask::GetState(int obj)
{
	ObjectState temp = objs[obj].state;
	return temp;
}

bool PhysicsTask::PhysicsCallback(SORE_Kernel::Event* event)
{
	switch(event->key.keySym)
	{
		case SDLK_b:
			updating = !updating;
			break;
	}
	return false;
}
