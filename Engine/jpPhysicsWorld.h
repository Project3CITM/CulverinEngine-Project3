#ifndef JP_PHYSICS_WORLD_H__
#define JP_PHYSICS_WORLD_H__

#include "PhysX/Include/PxPhysicsAPI.h"
#include "jpPhysicsRigidBody.h"

class ModulePhysics;

class jpCollisionCallback : public physx::PxSimulationEventCallback
{
public:
	jpCollisionCallback() {};
	jpCollisionCallback(ModulePhysics* callback) : callback_module(callback) {};
	~jpCollisionCallback() {};

	void 	onConstraintBreak(physx::PxConstraintInfo *constraints, physx::PxU32 count) {};
	void 	onWake(physx::PxActor **actors, physx::PxU32 count) {};
	void 	onSleep(physx::PxActor **actors, physx::PxU32 count) {};
	void 	onContact(const physx::PxContactPairHeader &pairHeader, const physx::PxContactPair *pairs, physx::PxU32 nbPairs) {};
	void	onTrigger(physx::PxTriggerPair *pairs, physx::PxU32 count);
	void 	onAdvance(const physx::PxRigidBody *const *bodyBuffer, const physx::PxTransform *poseBuffer, const physx::PxU32 count) {};

private:
	ModulePhysics * callback_module = nullptr;
};


class jpPhysicsWorld
{
public:
	jpPhysicsWorld();
	jpPhysicsWorld(ModulePhysics* module_callback);
	~jpPhysicsWorld();

	bool CreateNewPhysicsWorld();
	bool Simulate(float dt);
	bool StopSimulation(bool priority = true);

	physx::PxScene* CreateNewScene();
	physx::PxScene* GetScene(int scene_index) const;

	// Temporal functions
	physx::PxPhysics* GetPhysicsWorld();
	physx::PxCooking* GetCooking();

	//No Scene passed creates a sceneless rigidBody
	jpPhysicsRigidBody* CreateRigidBody(physx::PxScene* curr_scene = nullptr, bool dynamic = false);


private:
	// World, only one can be created
	//physx::PxPvd* pvd = nullptr;
	physx::PxPhysics* jpWorld = nullptr;
	physx::PxFoundation* jpFoundation = nullptr;

	//physx::PxCooking* jpCooking = nullptr;
	physx::PxDefaultErrorCallback gDefaultErrorCallback;
	physx::PxDefaultAllocator gDefaultAllocatorCallback;

	jpCollisionCallback* collision_callback = nullptr;
};

#endif // !JP_PHYSICS_WORLD_H__
