#include "Globals.h"
#include "Application.h"
#include "ModuleParticles.h"
#include "CompCamera.h"
#include "ModuleRenderer3D.h"
#include "CompParticleSystem.h"
#include "ModuleEventSystemV2.h"


ModuleParticles::ModuleParticles(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	have_config = true;
	name = "Particles";
}

// Destructor
ModuleParticles::~ModuleParticles()
{
	glDeleteBuffers(1, &geometry_buffer);
}

// Called before render is available
bool ModuleParticles::Start()
{	
	bool ret = true;
	
	/*Generate one buffer to particle geometry*/

	
	// The VBO containing the 4 vertices of the particles.
	// Thanks to instancing, they will be shared by all particles.
	 static const GLfloat  vertex_data[] = {
		 -0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 -0.5f, 0.5f, 0.0f,
		 0.5f, 0.5f, 0.0f,
	 };
	 
	 glGenBuffers(1, &geometry_buffer);
	 glBindBuffer(GL_ARRAY_BUFFER, geometry_buffer);
	 glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);


	return ret;
}

update_status ModuleParticles::PreUpdate(float dt)
{
	BROFILER_CATEGORY("PreUpdate: ModuleParticles", Profiler::Color::Blue);
	const CompCamera* camera = App->renderer3D->GetActiveCamera();

	if (camera == nullptr)
		return UPDATE_CONTINUE;

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
		else (*item)->SetCameraPosToFollow(float3::zero);

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
	BROFILER_CATEGORY("Update: ModuleParticles", Profiler::Color::Blue);
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
	BROFILER_CATEGORY("PostUpdate: ModuleParticles", Profiler::Color::Blue);
	perf_timer.Start();

	//Send one event per system to draw particles
	for (int i = 0; i < particle_systems.size(); i++)
	{
		Event draw_event;
		draw_event.Set_event_data(EventType::EVENT_PARTICLE_DRAW);
		draw_event.particle_draw.part_system = particle_systems[i];
		PushEvent(draw_event);
	}
	
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
		if ((*item) != nullptr)
		{
			if((*item)->parent != nullptr)
				(*item)->parent->SetSystemAsNull();
			delete (*item);
		}
	}

	//Particle systems deletes 
	particle_systems.clear();

	return true;
}

ParticleSystem * ModuleParticles::CreateParticleSystem()
{
	particle_systems.push_back(new ParticleSystem());
	return particle_systems.back();
}

GLuint ModuleParticles::GetGeometryBuffer() const
{
	return geometry_buffer;
}





