#include "Globals.h"
#include "Application.h"
#include "ModuleParticles.h"


ModuleParticles::ModuleParticles(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	have_config = true;
	name = "Particles";
}

// Destructor
ModuleParticles::~ModuleParticles()
{

}

// Called before render is available
bool ModuleParticles::Init(JSON_Object* node)
{
	perf_timer.Start();
	bool ret = true;
	


	Awake_t = perf_timer.ReadMs();
	return ret;
}




update_status ModuleParticles::PostUpdate(float dt)
{
	perf_timer.Start();
	postUpdate_t = perf_timer.ReadMs();

	return UPDATE_CONTINUE;
}

bool ModuleParticles::SaveConfig(JSON_Object * node)
{
	//Save audio config info --------------------------------

	// ------------------------------------------------------
	return true;
}

// Called before quitting
bool ModuleParticles::CleanUp()
{
	return true;
}



