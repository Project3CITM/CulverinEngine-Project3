#include "Application.h"
#include "ModuleEventSystem.h"
#include "ModulePhysics.h"

#include "Component.h"
#include "CompCollider.h"
#include "CompRigidBody.h"

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

	if (!App->mode_game && render_physics)
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

update_status ModulePhysics::Update(float dt)
{
	perf_timer.Start();

	if (dt > 0) 
	{
		physics_world->StopSimulation();
	}

	Update_t = perf_timer.ReadMs();
	return UPDATE_CONTINUE;
}

update_status ModulePhysics::PostUpdate(float dt)
{
	perf_timer.Start();

	if (!App->mode_game)
	{
		DrawPhysics();
	}

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
		physics_world = nullptr;
	}

	colliders.clear();
	
	return true;
}

// -----------------------------------------------------------------
bool ModulePhysics::SetEventListenrs()
{
	AddListener(EventType::EVENT_TRIGGER_COLLISION, this);
	return false;
}

void ModulePhysics::OnEvent(Event & event)
{
	switch (event.type)
	{
	case EventType::EVENT_TRIGGER_COLLISION:
		switch (event.physics_collision.collision_type)
		{
		case JP_COLLISION_TYPE::TRIGGER_ENTER:
			{
			static_cast<CompCollider*>(event.physics_collision.trigger)->OnTriggerEnter(event.physics_collision.actor);
			static_cast<CompRigidBody*>(event.physics_collision.actor)->OnTriggerEnter(event.physics_collision.trigger);
			break;
			}
		case JP_COLLISION_TYPE::TRIGGER_LOST:
			{
			static_cast<CompCollider*>(event.physics_collision.trigger)->OnTriggerLost(event.physics_collision.actor);
			static_cast<CompRigidBody*>(event.physics_collision.actor)->OnTriggerLost(event.physics_collision.trigger);
			break;
			}
		}
		break;
	case EventType::EVENT_TIME_MANAGER:
		switch (event.time.time)
		{
		case event.time.TIME_PLAY:

			for (std::map<physx::PxRigidActor*, Component*>::const_iterator item = colliders.cbegin(); item != colliders.cend(); item++)
			{
				if (item->second->GetType() != Comp_Type::C_RIGIDBODY)
				{
					continue;
				}
				((physx::PxRigidBody*)item->first)->setLinearVelocity(physx::PxVec3(0, 0, 0));
				((physx::PxRigidBody*)item->first)->setAngularVelocity(physx::PxVec3(0, 0, 0));

			}

			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

// -----------------------------------------------------------------
jpPhysicsRigidBody * ModulePhysics::GetNewRigidBody(Component * component, bool dynamic)
{
	update_debug_draw = true;
	if (physics_world && component)
	{
		jpPhysicsRigidBody* body = physics_world->CreateRigidBody(physics_world->GetScene(0), dynamic);
		if (body->GetActor()) {
			colliders.insert(std::pair<physx::PxRigidActor*, Component*>(body->GetActor(), component));
			return body;
		}
		else
		{
			delete body;
			return nullptr;
		}
	}
	else
	{
		return nullptr;
	}
	
}

bool ModulePhysics::DeleteCollider(Component * component, jpPhysicsRigidBody * body)
{
	if (physics_world && colliders.find(body->GetActor()) != colliders.end())
	{
		colliders.erase(body->GetActor());
		update_debug_draw = true;
		delete body;
		return true;
	}
	else return false;
}

void ModulePhysics::ChangeRigidActorToStatic(jpPhysicsRigidBody * actor, Component* comp)
{
	if (actor && comp) {
		colliders.erase(actor->GetActor());
		actor->ToStatic();
		colliders.insert(std::pair<physx::PxRigidActor*, Component*>(actor->GetActor(), comp));
	}
}

void ModulePhysics::ChangeRigidActorToDynamic(jpPhysicsRigidBody * actor, Component* comp)
{
	if (actor && comp) {
		colliders.erase(actor->GetActor());
		actor->ToDynamic();
		colliders.insert(std::pair<physx::PxRigidActor*, Component*>(actor->GetActor(), comp));
	}
}

void ModulePhysics::OnTrigger(physx::PxRigidActor* trigger, physx::PxRigidActor* actor, JP_COLLISION_TYPE type)
{
	//Avoid Sending trigger events on edition mode
	if (App->engine_state != EngineState::PLAY)
	{
		return;
	}

	// Send Trigger event to execute on postupdate 
	std::map<physx::PxRigidActor*, Component*>::const_iterator npair;

	Event collision;
	collision.physics_collision.type = EventType::EVENT_TRIGGER_COLLISION;
	collision.physics_collision.collision_type = type;

	npair = colliders.find(trigger);
	if (npair != colliders.end())
	{
		collision.physics_collision.trigger = npair._Ptr->_Myval.second;

		npair = colliders.find(actor);
		if (npair != colliders.end())
		{
			collision.physics_collision.actor = npair._Ptr->_Myval.second;
		}
		else
		{
			return;
		}
	}
	else
	{
		return;
	}

	PushEvent(collision);
}

// -----------------------------------------------------------------
void ModulePhysics::DrawPhysics()
{
	if (!render_physics || (!render_on_play && App->engine_state == PLAY))
	{
		return;
	}

	if (App->engine_state == STOP && update_debug_draw)
	{
		mScene->simulate((physx::PxReal)DEBUG_PHYSX_DT);
		mScene->fetchResults(true);
		update_debug_draw = false;
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

void ModulePhysics::DebugDrawUpdate()
{
	update_debug_draw = true;
}
