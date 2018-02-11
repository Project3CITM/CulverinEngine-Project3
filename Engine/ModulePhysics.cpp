#include "ModulePhysics.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	Update_enabled = true;

	have_config = false; // not yet
	name = "Physics";
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

	return true;
}
