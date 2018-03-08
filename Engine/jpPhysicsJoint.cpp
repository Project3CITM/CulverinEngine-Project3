#include "jpPhysicsJoint.h"
#include "jpPhysicsRigidBody.h"

jpJoint::jpJoint()
{
}

jpJoint::~jpJoint()
{
	if (joint)
	{
		joint->release();
		joint = nullptr;
	}
}

void jpJoint::CreateDistanceJoint(physx::PxPhysics* px_physics, jpPhysicsRigidBody * actor0, jpPhysicsRigidBody * actor1, float3 offset0, float3 offset1)
{
	if (joint)
	{
		joint->release();
		joint = nullptr;
	}

	if (px_physics && actor0 && actor1)
	{
		joint = physx::PxDistanceJointCreate(*px_physics, actor0->GetActor(),
				physx::PxTransform(physx::PxVec3(offset0.x, offset0.y, offset0.z)),
				actor1->GetActor(), physx::PxTransform(physx::PxVec3(offset1.x, offset1.y, offset1.z)));
	}
}

bool jpJoint::ToRelease()
{
	return joint != nullptr;
}

void jpJoint::Release()
{
	if (joint)
	{
		joint->release();
		joint = nullptr;
	}
}

void jpJoint::SetLimitDistance(float min_dist, float max_dist)
{
	if (joint)
	{
		physx::PxReal min, max;
		if (min_dist < max_dist)
		{
			min = min_dist;
			max = max_dist;
		}
		else
		{
			min = max_dist;
			max = min_dist;
		}

		joint->setMinDistance(min);
		joint->setMaxDistance(max);

		joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMAX_DISTANCE_ENABLED, true);
		joint->setDistanceJointFlag(physx::PxDistanceJointFlag::eMIN_DISTANCE_ENABLED, true);
	}
}

void jpJoint::SetSecondActorPose()
{
	if (joint)
	{
		physx::PxRigidActor* first = nullptr;
		physx::PxRigidActor* second = nullptr;

		joint->getActors(first, second);
		physx::PxTransform transf = first->getGlobalPose();
		transf.p.x -= joint->getMaxDistance();
		
		second->setGlobalPose(transf);
	}
}
