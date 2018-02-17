#ifndef MODULEPHYSICS_H_
#define MODULEPHYSICS_H_

#include "Module.h"

#define DEBUG_PHYSX_DT 0.000000000001

class jpPhysicsWorld;
class jpPhysicsRigidBody;

namespace physx 
{
	class PxPhysics;
	class PxScene;
	class PxSceneDesc;
	class PxRigidActor;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxBounds3;
	class PxSimulationEventCallback;
}

class ModulePhysics : public Module
{
public:
	ModulePhysics(bool start_enabled = true);
	~ModulePhysics();

	bool Init(JSON_Object* node);
	bool Start();
	update_status PreUpdate(float dt);
	//update_status Update(float dt);
	update_status PostUpdate(float dt);
	update_status UpdateConfig(float dt);
	bool SaveConfig(JSON_Object* node);
	bool CleanUp();

	// PhysX Methods ----------------
	jpPhysicsRigidBody* GetNewRigidBody(bool dynamic = false);

	// Collision Callback -----------
	void OnTrigger(physx::PxRigidActor* trigger, physx::PxRigidActor* actor);

	// Debug Methods ----------------
	void DrawPhysics();

private:
	physx::PxPhysics* mPhysics = nullptr;

	jpPhysicsWorld * physics_world = nullptr;
	physx::PxScene* mScene = nullptr;

	// Config data ------------------
	// Use only as a debug tool -----
	bool render_physics = false;
	bool render_on_play = false;

	//jpPhysicsRigidBody* trigger_test = nullptr;
	//jpPhysicsRigidBody* collider_test = nullptr;
};


#endif // !MODULEPHYSICS_H_