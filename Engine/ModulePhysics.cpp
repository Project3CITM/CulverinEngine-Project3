#include "Application.h"
#include "ModuleEventSystemV2.h"
#include "ModulePhysics.h"
#include "Scene.h"

#include "Component.h"
#include "CompCollider.h"
#include "CompRigidBody.h"
#include "GameObject.h"

#include "PhysX/Include/PxPhysicsAPI.h"
#include "jpPhysicsWorld.h"
#include "GL3W/include/glew.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{
	Awake_enabled = true;
	Start_enabled = true;
	Update_enabled = true;
	preUpdate_enabled = true;

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

	//LOG("Setting up Physics");

	bool ret = true;

	Start_t = perf_timer.ReadMs();
	return ret;
}

// -----------------------------------------------------------------
update_status ModulePhysics::PreUpdate(float dt)
{
	perf_timer.Start();
	// Update Physics World
	if (dt > 0) {
		did_simulation = physics_world->Simulate(dt);
	}
	else
	{
		did_simulation = false;
	}
	preUpdate_t = perf_timer.ReadMs();
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
	AddListener(EventType::EVENT_TIME_MANAGER, this);
	return false;
}

void ModulePhysics::OnEvent(Event & event)
{
	switch (event.Get_event_data_type())
	{
	case EventType::EVENT_TRIGGER_COLLISION:
	{
		if (!did_simulation)
		{
			break;
		}

		switch (event.trigger.coll_type)
		{
		case JP_COLLISION_TYPE::TRIGGER_ENTER:
		{
			if (event.trigger.actor0->GetType() == C_COLLIDER)
			{
				static_cast<CompCollider*>(event.trigger.actor0)->OnTriggerEnter(event.trigger.actor1);
			}
			else
			{
				static_cast<CompRigidBody*>(event.trigger.actor0)->OnTriggerEnter(event.trigger.actor1);
			}

			if (event.trigger.actor1->GetType() == C_RIGIDBODY)
			{
				static_cast<CompRigidBody*>(event.trigger.actor1)->OnTriggerEnter(event.trigger.actor0);
			}
			else
			{
				static_cast<CompCollider*>(event.trigger.actor1)->OnTriggerEnter(event.trigger.actor0);
			}
			break;
		}
		case JP_COLLISION_TYPE::TRIGGER_LOST:
		{
			if (event.trigger.actor0->GetType() == C_COLLIDER)
			{
				static_cast<CompCollider*>(event.trigger.actor0)->OnTriggerLost(event.trigger.actor1);
			}
			else
			{
				static_cast<CompRigidBody*>(event.trigger.actor0)->OnTriggerLost(event.trigger.actor1);
			}

			if (event.trigger.actor1->GetType() == C_RIGIDBODY)
			{
				static_cast<CompRigidBody*>(event.trigger.actor1)->OnTriggerLost(event.trigger.actor0);
			}
			else
			{
				static_cast<CompCollider*>(event.trigger.actor1)->OnTriggerLost(event.trigger.actor0);
			}
			break;
		}
		case JP_COLLISION_TYPE::CONTACT_ENTER:
		{
			CollisionData data;
			data.is_contact = true;
			data.impact_point = event.trigger.impact_point;
			data.impact_normal = event.trigger.impact_normal;

			data.actor1 = event.trigger.actor1;
			if (event.trigger.actor0->GetType() == C_COLLIDER)
			{
				static_cast<CompCollider*>(event.trigger.actor0)->OnContact(data);
			}
			else
			{
				static_cast<CompRigidBody*>(event.trigger.actor0)->OnContact(data);
			}

			data.actor1 = event.trigger.actor0;
			if (event.trigger.actor1->GetType() == C_COLLIDER)
			{
				static_cast<CompCollider*>(event.trigger.actor1)->OnContact(data);
			}
			else
			{
				static_cast<CompRigidBody*>(event.trigger.actor1)->OnContact(data);
			}
			break;
		}
		}
		break;
	}
	case EventType::EVENT_TIME_MANAGER:
		switch (event.time.time)
		{
		case event.time.TIME_PLAY:

			for (std::map<physx::PxRigidActor*, Component*>::const_iterator item = colliders.cbegin(); item != colliders.cend(); item++)
			{
				if (item->second->GetType() != Comp_Type::C_RIGIDBODY || ((CompRigidBody*)item->second)->IsKinematic())
				{
					continue;
				}

				CompRigidBody* rbody = (CompRigidBody*)item->second;
				if (rbody->HaveBodyShape())
				{
					rbody->SetMomentumToZero();
					rbody->SetColliderPosition();
				}

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

physx::PxPhysics * ModulePhysics::GetPhysX()
{
	return mPhysics;
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

bool ModulePhysics::ShowColliderFilterOptions(uint& flags)
{
	bool show_filter_window = true;
	ImGui::SetNextWindowSize(ImVec2(200.f, 300.f));
	if (ImGui::Begin("Collision Filter", &show_filter_window, ImGuiWindowFlags_NoCollapse))
	{
		if (ImGui::Button("Set Flags") || !show_filter_window)
		{
			ImGui::End();
			return true;
		}
		ImGui::Separator();

		std::vector<std::string>* tags = App->scene->GetTagsVec();

		bool flag_state = false;
		for (uint i = 0; i < tags->size(); i++)
		{
			flag_state = (flags & (1 << i));
			if (ImGui::Checkbox(tags->at(i).c_str(), &flag_state))
			{
				if (flag_state)
				{
					flags |= (1 << i);
				}
				else
				{
					flags &= ~(1 << i);
				}
			}
		}
	}
	ImGui::End();
	return false;
}

void ModulePhysics::OnCollision(physx::PxRigidActor* actor0, physx::PxRigidActor* actor1, JP_COLLISION_TYPE type, float3 point, float3 normal)
{
	Event phys_event;
	phys_event.Set_event_data(EventType::EVENT_TRIGGER_COLLISION);
	phys_event.trigger.coll_type = type;

	std::map<physx::PxRigidActor*, Component*>::const_iterator npair;
	npair = colliders.find(actor0);
	if (npair != colliders.end())
	{
		phys_event.trigger.actor0 = npair._Ptr->_Myval.second;

		npair = colliders.find(actor1);
		if (npair != colliders.end())
		{
			phys_event.trigger.actor1 = npair._Ptr->_Myval.second;
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

	phys_event.trigger.impact_point = point;
	phys_event.trigger.impact_normal = normal;

	PushEvent(phys_event);
}

GameObject * ModulePhysics::RayCast(float3 origin, float3 direction, float distance)
{
	GameObject* ret = nullptr;

	if (origin.IsFinite() && direction.IsFinite() && distance > 0)
	{
		direction.Normalize();
		physx::PxRaycastBuffer hit;
		mScene->raycast(physx::PxVec3(origin.x, origin.y, origin.z), physx::PxVec3(direction.x, direction.y, direction.z), distance, hit);
		if (hit.hasBlock)
		{
			std::map<physx::PxRigidActor*, Component*>::const_iterator pair = colliders.find(hit.block.actor);
			ret = pair._Ptr->_Myval.second->GetParent();
		}
	}
	else
	{
		LOG("RayCast Parameters Not Valids");
	}
	return ret;
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

	App->scene->scene_buff->Bind("Scene");

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

	App->scene->scene_buff->UnBind("Scene");
}

void ModulePhysics::DebugDrawUpdate()
{
	update_debug_draw = true;
}


