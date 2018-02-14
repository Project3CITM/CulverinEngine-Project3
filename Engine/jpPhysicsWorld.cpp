#include "jpPhysicsWorld.h"
//#include "Globals.h"
#include "Math.h"

//PhysX libraries for debug builds
#ifdef _DEBUG

#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3DEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PxFoundationDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3ExtensionsDEBUG.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3CommonDEBUG_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PxPvdSDKDEBUG_x86.lib")

#endif // _DEBUG

//PhysX libraries for release builds
#ifndef _DEBUG

#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PxFoundation_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3Extensions.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PhysX3Common_x86.lib")
#pragma comment (lib, "PhysX/lib/vc15win32/PxPvdSDK_x86.lib")

#endif // _DEBUG



jpPhysicsWorld::jpPhysicsWorld()
{	
}

jpPhysicsWorld::~jpPhysicsWorld()
{
	if (jpWorld) {
		physx::PxScene* scene = nullptr;
		for (unsigned int i = 0; i < jpWorld->getNbScenes(); i++) {
			jpWorld->getScenes(&scene, 1, i);
			scene->release();
			scene = nullptr;
		}
		jpWorld->release();	
		//pvd->release();
		jpFoundation->release();
	}
}

bool jpPhysicsWorld::CreateNewPhysicsWorld()
{
	if (!jpWorld) {
		jpFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gDefaultAllocatorCallback,
			gDefaultErrorCallback);

		if (jpFoundation)
		{
			jpWorld = PxCreatePhysics(PX_PHYSICS_VERSION, *jpFoundation, physx::PxTolerancesScale(), true, nullptr);
		}

		if (jpWorld) {
			//jpCooking = PxCreateCooking(PX_PHYSICS_VERSION, *jpFoundation, jpWorld->getTolerancesScale());
			return true;
		}
		else return false;
	}
	else return false;
}

bool jpPhysicsWorld::Simulate(float dt)
{
	if (jpWorld && jpWorld->getNbScenes() > 0) {
		physx::PxScene* scene;
		jpWorld->getScenes(&scene, 1, 0);
		scene->simulate(dt);
		return scene->fetchResults(true);
	}
	return false;
}

// Create Default Scene
physx::PxScene * jpPhysicsWorld::CreateNewScene()
{
	physx::PxSceneDesc sceneDesc(jpWorld->getTolerancesScale());
	sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(1);
	sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	sceneDesc.flags |= physx::PxSceneFlag::eENABLE_PCM;

	return jpWorld->createScene(sceneDesc);
}

physx::PxScene * jpPhysicsWorld::GetScene(int scene_index) const
{
	physx::PxScene* ret = nullptr;
	jpWorld->getScenes(&ret, 1, scene_index);
	return ret;
}

physx::PxPhysics * jpPhysicsWorld::GetPhysicsWorld()
{
	return jpWorld;
}

physx::PxCooking * jpPhysicsWorld::GetCooking()
{
	return nullptr;
}

jpPhysicsRigidBody * jpPhysicsWorld::CreateRigidBody(physx::PxScene * curr_scene, bool dynamic)
{
	jpPhysicsRigidBody* new_body = nullptr;
	if (jpWorld) {
		new_body = new jpPhysicsRigidBody(jpWorld, dynamic);
		if (curr_scene != nullptr)
			curr_scene->addActor(*new_body->GetActor());
	}
	
	return new_body;
}
