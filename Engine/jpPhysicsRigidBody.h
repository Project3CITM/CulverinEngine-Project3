#ifndef JP_PHYSICS_RIGIDBODY_H__
#define JP_PHYSICS_RIGIDBODY_H__

#include "PhysX/Include/PxPhysicsAPI.h"

#include "Math/float3.h"
#include "Math/Quat.h"

enum JP_COLLIDER_TYPE
{
	COLL_SPHERE,
	COLL_PLANE,
	COLL_CAPSULE,
	COLL_BOX
};

enum JP_COLLISION_TYPE
{
	TRIGGER_ENTER,
	TRIGGER_LOST,
	CONTACT_ENTER,
	CONTACT_LOST
};

//Simple abstraction of the physX rigiddynamic class for an easier implementation with component system
class jpPhysicsRigidBody
{
public:
	jpPhysicsRigidBody(physx::PxPhysics* px_physics, bool is_dynamic = false);
	~jpPhysicsRigidBody();

	void ToStatic();
	void ToDynamic();

	void ActivateShape();
	void DeActivateShape();

	// Set Methods
	void SetAsKinematic(bool kinematic);
	void SetAsTrigger(bool trigger);
	void SetSleepTime(float time);
	void SetFilterFlags(unsigned int own_flag, unsigned int collision_flags);
	void SetTransform(float* trans_mat);
	void SetTransform(float3 pos, Quat rotation, bool autoawake = true);
	void SetGeometry(physx::PxGeometry new_geometry); //DO NOT USE, does nothing, will be later on defined to use with convexmesh
	
	void SetMaterial(float &static_friction, float &dynamic_friction, float &restitution);
	void SetShape(physx::PxShape* new_shape);

	void SetGeometry(float3 new_scale, float radius, JP_COLLIDER_TYPE type);
	void SetShapeScale(float3& new_scale, float& radius, JP_COLLIDER_TYPE type);

	void SetMass(float &mass);
	void SetDynamicLockFlags(physx::PxRigidDynamicLockFlag::Enum x, bool active);
	void SetDynamicLock();

	// Get Methods
	void GetTransform(float3& pos, Quat& rotation);
	physx::PxRigidActor* GetActor();

	// Dynamic Body Methods ---------
	// Check Methods
	bool Sleeping();

	// Simulation Methods
	void ApplyForce(float3 force);
	void ApplyImpulse(float3 impulse);
	void ApplyTorqueForce(float3 force);
	void ApplyTorqueImpulse(float3 impulse);

	// Move Kinematic
	void MoveKinematic(float3 pos, Quat rotation);
	void ResetForces();

private:
	// main body
	physx::PxRigidActor* body = nullptr;
	// Shape
	physx::PxShape* body_shape = nullptr;
	// Material
	physx::PxMaterial* default_material = nullptr;
	// Static/Dynamic
	bool is_dynamic = false;
};

#endif // !JP_PHYSICS_RIGIDBODY_H__

