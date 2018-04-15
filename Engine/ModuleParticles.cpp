#include "Globals.h"
#include "Application.h"
#include "ModuleParticles.h"
#include "CompCamera.h"
#include "ModuleRenderer3D.h"


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

update_status ModuleParticles::PreUpdate(float dt)
{
	const CompCamera* camera = App->renderer3D->GetActiveCamera();
	for (std::vector<ParticleSystem*>::iterator item = particle_systems.begin(); item != particle_systems.cend(); )
	{
		if ((*item)->to_delete == true && (*item)->SystemIsEmpty())
		{
			delete (*item);
			item = particle_systems.erase(item);

			continue;
		}

		if (camera != nullptr)
			(*item)->SetCameraPosToFollow(camera->frustum.pos);

		float3 emitter_pos;
		((ParticleEmitter*)(*item)->GetEmitter())->GetPosition(emitter_pos);
		(*item)->distance_to_camera = camera->frustum.pos.Distance(emitter_pos);

		if ((*item)->distance_to_camera < (*item)->discard_distance)
			if (App->engine_state == EngineState::STOP)
				(*item)->PreUpdate(0.02);
			else (*item)->PreUpdate(dt);

		item++;
	}


	return UPDATE_CONTINUE;
}

update_status ModuleParticles::Update(float dt)
{
	for (std::vector<ParticleSystem*>::iterator item = particle_systems.begin(); item != particle_systems.cend(); item++)
	{
		if ((*item)->distance_to_camera < (*item)->discard_distance)
		{
			if (App->engine_state == EngineState::STOP)
				(*item)->Update(0.02, (*item)->preview);
			else (*item)->Update(dt);

			(*item)->DrawImGuiEditorWindow();

			if (App->engine_state == EngineState::STOP)
				(*item)->PostUpdate(0.02);
			else (*item)->PostUpdate(dt);
		}
	}
	return UPDATE_CONTINUE;
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
	for (std::vector<ParticleSystem*>::iterator item = particle_systems.begin(); item != particle_systems.cend(); item++)
	{
		if((*item)!=nullptr)
			delete (*item);
	}

	particle_systems.clear();

	return true;
}

ParticleSystem * ModuleParticles::CreateParticleSystem()
{
	particle_systems.push_back(new ParticleSystem());
	return particle_systems.back();
}



