#include "ModulePhysics.h"

#include "PhysX/Include/PxPhysicsAPI.h"
#include "PhysX/Include/jpPhysicsWorld.h"
#include "PhysX/Include/jpPhysicsRigidBody.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	Update_enabled = true;

	have_config = false; // not yet
	name = "Physics";
/*
	// Create Physics World
	physics_world = new jpPhysicsWorld();
	physics_world->CreateNewPhysicsWorld();

	// Create and Get Current Scene and Physics world
	mScene = physics_world->CreateNewScene();
	mPhysics = physics_world->GetPhysicsWorld();
	*/
}

ModulePhysics::~ModulePhysics()
{
}

// -----------------------------------------------------------------
bool ModulePhysics::Init(JSON_Object * node)
{
	perf_timer.Start();

	
	Awake_t = perf_timer.ReadMs();
	return true;
}

// -----------------------------------------------------------------
bool ModulePhysics::Start()
{
	perf_timer.Start();

	LOG("Setting up Physics");

	bool ret = true;

	Start_t = perf_timer.ReadMs();
	return ret;
}

// -----------------------------------------------------------------
update_status ModulePhysics::PreUpdate(float dt)
{
	/*
	// Update Physics World
	if (dt > 0) {
		physics_world->Simulate(dt);
	}
	*/
	return UPDATE_CONTINUE;
}

//update_status ModulePhysics::Update(float dt)
//{
//	perf_timer.Start();

//	Update_t = perf_timer.ReadMs();
//	return UPDATE_CONTINUE;
//}

update_status ModulePhysics::PostUpdate(float dt)
{
	perf_timer.Start();

	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModulePhysics::UpdateConfig(float dt)
{

	return UPDATE_CONTINUE;
}

bool ModulePhysics::SaveConfig(JSON_Object * node)
{

	return true;
}


// -----------------------------------------------------------------
bool ModulePhysics::CleanUp()
{
	LOG("Cleaning Physics");

	if (physics_world)
	{
		delete physics_world;
	}
	
	return true;
}

// -----------------------------------------------------------------
jpPhysicsRigidBody * ModulePhysics::GetNewRigidBody(bool dynamic)
{
	if (physics_world)
	{
		return physics_world->CreateRigidBody(physics_world->GetScene(0), dynamic);
	}
	else
	{
		return nullptr;
	}
}

void ModulePhysics::ChangeRigidBodyToStatic(jpPhysicsRigidBody * rigidbody)
{
	if (physics_world && rigidbody)
	{
		rigidbody->ToStatic(mPhysics);
	}
}

void ModulePhysics::ChangeRigidBodyToDynamic(jpPhysicsRigidBody * rigidbody)
{
	if (physics_world && rigidbody)
	{
		rigidbody->ToDynamic(mPhysics);
	}
}
