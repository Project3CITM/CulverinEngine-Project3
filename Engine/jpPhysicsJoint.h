#ifndef JP_PHYSICS_JOINT__
#define JP_PHYSICS_JOINT__

#include "PhysX/Include/PxPhysicsAPI.h"

#include "Math/float3.h"
#include "Math/Quat.h"

class jpPhysicsRigidBody;

class jpJoint
{
public:
	jpJoint();
	~jpJoint();

	void CreateDistanceJoint(physx::PxPhysics* px_physics, jpPhysicsRigidBody* actor0, jpPhysicsRigidBody* actor1, float3 offset0, float3 offset1);
	
	bool ToRelease();
	void Release();

	// Set Methods
	void SetLimitDistance(float min_dist, float max_dist);

private:
	physx::PxDistanceJoint* joint = nullptr;
};

#endif // !JP_PHYSICS_JOINT__
