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

PhysicsBall::PhysicsBall()
{
}

PhysicsBall::PhysicsBall(double x, double y, double z)
{
	position.Set(x,y,z);
}

void PhysicsBall::Zero()
{
	position.Set(0,0,0);
	velocity.Set(0,0,0);
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
	if(elapsedTime>2000)
		return;
	//APP_LOG(SORE_Logging::LVL_DEBUG2, "Velocity: (%f,%f,%f)",velocity.GetValue()[0],velocity.GetValue()[1],velocity.GetValue()[2]);
	const double grav = -0.0001;
	const double terminalVelocity = 20.0;
	obj->velocity += Vector3D<double>(0.0,grav*elapsedTime,0.0);
	//if(velocity.Magnitude()>terminalVelocity)
	//	velocity = velocity.Normalize()*terminalVelocity;
	obj->position += obj->velocity*elapsedTime;
	if(obj->position.GetValue()[1]<5.0 && obj->velocity.GetValue()[1]<0.0)
		obj->velocity = -obj->velocity;
	//APP_LOG(SORE_Logging::LVL_DEBUG2, "elapsed time: %d", elapsedTime);
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
