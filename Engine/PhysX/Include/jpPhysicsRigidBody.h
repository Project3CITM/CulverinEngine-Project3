#ifndef __JP_PHYSICS_RIGIDBODY_H__
#define __JP_PHYSICS_RIGIDBODY_H__

#include "PxPhysicsAPI.h"

enum JP_COLLIDER_TYPE
{
	COLL_SPHERE,
	COLL_PLANE,
	COLL_CAPSULE,
	COLL_BOX
};

//Simple abstraction of the physX rigiddynamic class for an easier implementation with component system
class jpPhysicsRigidBody
{
public:
	jpPhysicsRigidBody(physx::PxPhysics* px_physics);
	~jpPhysicsRigidBody();

	void ActivateShape();
	void DeActivateShape();

	// Set Methods
	////true == dynamic, fale == Kinematic(Static)
	void SetDynamic(bool is_dynamic);
	void SetTransform(float* trans_mat);
	void SetGeometry(physx::PxGeometry new_geometry); //DO NOT USE, does nothing, will be later on defined to use with convexmesh
	
	void SetMaterial(float &static_friction, float &dynamic_friction, float &restitution);
	void SetShape(physx::PxShape* new_shape);

	void SetGeometry(physx::PxVec3 pos, float radius, JP_COLLIDER_TYPE type);
	void SetShapeScale(physx::PxVec3 scale, float radius, JP_COLLIDER_TYPE type);

	void SetMass(float &mass);

	// Get Methods
	void GetTransform(physx::PxVec3& pos, physx::PxQuat& quat);
	physx::PxRigidBody* GetPxBody();

	// Check Methods
	bool Sleeping();

	// Simulation Methods
	void ApplyForce(physx::PxVec3 force);
	void ApplyImpulse(physx::PxVec3 impulse);
	void ApplyTorqueForce(physx::PxVec3 force);
	void ApplyTorqueImpulse(physx::PxVec3 impulse);

	// Move Kinematic
	void MoveKinematic(physx::PxTransform dest);

public:
	physx::PxRigidDynamic* px_body = nullptr;
	physx::PxShape* body_shape = nullptr;

	//Store a default material since we can't create a new one without the PxPhysics
	physx::PxMaterial* default_material = nullptr;
};

#endif // !__JP_PHYSICS_RIGIDBODY_H__

