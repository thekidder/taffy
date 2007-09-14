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

class PhysicsObject
{
	public:
		//virtual void Update(int elapsedTime) = 0;
		double* GetPosition() {return position.GetValue();}
		void MoveTo(double x, double y, double z) {position.Set(x,y,z);}
		
		Vector3D<double> velocity;
		Vector3D<double> position;
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
