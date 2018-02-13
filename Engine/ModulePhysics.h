#ifndef MODULEPHYSICS_H_
#define MODULEPHYSICS_H_

#include "Module.h"

class jpPhysicsWorld;
class jpPhysicsRigidBody;

namespace physx 
{
	class PxPhysics;
	class PxScene;
	class PxSceneDesc;
	class PxRigidStatic;
	class PxRigidDynamic;
	class PxBounds3;
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

	// Chenge Actor type: static/dynamic
	void ChangeRigidBodyToStatic(jpPhysicsRigidBody* actor);
	void ChangeRigidBodyToDynamic(jpPhysicsRigidBody* actor);


private:
	physx::PxPhysics* mPhysics = nullptr;

	jpPhysicsWorld * physics_world = nullptr;
	physx::PxScene* mScene = nullptr;
};

#endif // !MODULEPHYSICS_H_