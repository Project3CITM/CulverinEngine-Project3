#include "ModulePhysics.h"

#include "Application.h"

#include "PhysX/Include/PxPhysicsAPI.h"
#include "jpPhysicsWorld.h"
#include "GL3W/include/glew.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	Update_enabled = true;

	have_config = true; // not yet
	name = "Physics";

	// Create Physics World
	physics_world = new jpPhysicsWorld(this);
	physics_world->CreateNewPhysicsWorld();

	// Create and Get Current Scene and Physics world
	mScene = physics_world->CreateNewScene();
	mPhysics = physics_world->GetPhysicsWorld();
	
}

ModulePhysics::~ModulePhysics()
{
}

// -----------------------------------------------------------------
bool ModulePhysics::Init(JSON_Object * node)
{
	perf_timer.Start();

	//Load physics config info -------
	render_physics = json_object_get_boolean(node, "Render Physics");
	render_on_play = json_object_get_boolean(node, "Render on Play");

	if (render_physics)
	{
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, 1.0f);
		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
	}

	Awake_t = perf_timer.ReadMs();
	return true;
}

// -----------------------------------------------------------------
bool ModulePhysics::Start()
{
	perf_timer.Start();

	LOG("Setting up Physics");

	/* Simple Trigger Test
	jpPhysicsRigidBody* floor = GetNewRigidBody();
	floor->SetGeometry(float3(20.f, 1.f, 20.f), 0, JP_COLLIDER_TYPE::COLL_PLANE);

	trigger_test = GetNewRigidBody();
	trigger_test->SetAsTrigger(true);
	trigger_test->SetTransform(float3(0.5f, 2.f, 0.f), Quat::identity);

	collider_test = GetNewRigidBody(true);
	collider_test->SetGeometry(float3(.5f, .5f, .5f), 0.5, JP_COLLIDER_TYPE::COLL_BOX);
	collider_test->SetTransform(float3(0.f, 5.f, 0.f), Quat::identity);
	*/
	bool ret = true;

	Start_t = perf_timer.ReadMs();
	return ret;
}

// -----------------------------------------------------------------
update_status ModulePhysics::PreUpdate(float dt)
{
	// Update Physics World
	if (dt > 0) {
		physics_world->Simulate(dt);
	}
	
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

	DrawPhysics();

	postUpdate_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModulePhysics::UpdateConfig(float dt)
{
	if (ImGui::Checkbox("Render physics", &render_physics))
	{
		float scale = 0.f;
		(render_physics) ? scale = 1.f : scale = 0.f;

		mScene->setVisualizationParameter(physx::PxVisualizationParameter::eSCALE, scale);
	}
	ImGui::Checkbox("Render on play", &render_on_play);

	return UPDATE_CONTINUE;
}

bool ModulePhysics::SaveConfig(JSON_Object * node)
{
	json_object_set_boolean(node, "Render Physics", render_physics);
	json_object_set_boolean(node, "Render on Play", render_on_play);

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

void ModulePhysics::OnTrigger(physx::PxRigidActor* trigger, physx::PxRigidActor* actor)
{
	// Send Event
	LOG("Collision Detected");
}

// -----------------------------------------------------------------
void ModulePhysics::DrawPhysics()
{
	if ((!render_on_play && App->engine_state == PLAY) || !render_physics) return;

	if (App->engine_state == STOP)
	{
		mScene->simulate((physx::PxReal)DEBUG_PHYSX_DT);
		mScene->fetchResults(true);
	}
	const physx::PxRenderBuffer& rb = mScene->getRenderBuffer();

	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glLineWidth(2.0f);
	glBegin(GL_LINES);
	for (uint i = 0; i < rb.getNbLines(); i++)
	{
		const physx::PxDebugLine& line = rb.getLines()[i];

		glVertex3f(line.pos0.x, line.pos0.y, line.pos0.z);
		glVertex3f(line.pos1.x, line.pos1.y, line.pos1.z);
	}
	glEnd();
	glLineWidth(1.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}
