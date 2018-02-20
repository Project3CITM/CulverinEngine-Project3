#include "CompRigidBody.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "Scene.h"
#include "ModulePhysics.h"

#include "jpPhysicsRigidBody.h"
#include "CompCollider.h"
#include "CompTransform.h"

CompRigidBody::CompRigidBody(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "RigidBody";

	if (parent)
	{
		transform = parent->GetComponentTransform();
		collider_comp = (CompCollider*)parent->FindComponentByType(Comp_Type::C_COLLIDER);
		if (collider_comp != nullptr)
		{
			LOG("Getting the physics body from the Collider");
			body = collider_comp->GivePhysicsBody(this);
			App->physics->ChangeRigidActorToDynamic(body, this);
		}
		else
		{
			LOG("Creating a new physics body for the RigidBody...");
			body = App->physics->GetNewRigidBody(this, true);
			SetColliderPosition();
		}
	}
}

CompRigidBody::CompRigidBody(const CompRigidBody & copy, GameObject * parent) : Component(Comp_Type::C_RIGIDBODY, parent)
{
	uid = App->random->Int();
	name_component = "RigidBody";

	//Same as regular constructor since this properties depend on the parent
	if (parent)
	{
		collider_comp = (CompCollider*)parent->FindComponentByType(Comp_Type::C_COLLIDER);
		if (collider_comp != nullptr)
		{
			LOG("Getting the physics body from the Collider");
			body = collider_comp->GivePhysicsBody(this);
			App->physics->ChangeRigidActorToDynamic(body, this);
		}
		else
		{
			LOG("Creating a new physics body for the RigidBody...");
			body = App->physics->GetNewRigidBody(this, true);
		}
	}

	kinematic = copy.kinematic;
	body->SetAsKinematic(kinematic);
}

CompRigidBody::~CompRigidBody()
{
}

void CompRigidBody::PreUpdate(float dt)
{
	if (dt > 0 && body)
	{
		own_update = true;
		UpdateParentPosition();
	}
}

void CompRigidBody::Update(float dt)
{
	if (transform->GetUpdated() && !own_update)
	{
		SetColliderPosition();
	}
	else
	{
		own_update = false;
	}
}

void CompRigidBody::Clear()
{
	if (collider_comp != nullptr)
	{
		App->physics->ChangeRigidActorToStatic(body, collider_comp);
		collider_comp->SetRigidBodyComp(nullptr);
		collider_comp = nullptr;
		body = nullptr;
		LOG("RigidBody deleted, physics body deletion passed to collider...");
	}
	else if(body != nullptr)
	{
		LOG("Deleted physics body form RigidBody comp...");
		App->physics->DeleteCollider(this, body);
		body = nullptr;
	}
}

void CompRigidBody::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implemented yet.
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implemented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_UNIQUE_SEPARATOR)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompRigidBody*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompRigidBody::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsCollider");
	}
	ImGui::PopStyleVar();
	if (ImGui::BeginPopup("OptionsCollider"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	if (ImGui::Checkbox("Kinematic", &kinematic))
	{
		body->SetAsKinematic(true);
	}

	ImGui::TreePop();
}

void CompRigidBody::CopyValues(const CompRigidBody * component)
{
	//more...
}

void CompRigidBody::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
}

void CompRigidBody::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
}

void CompRigidBody::SyncComponent()
{
	body->SetAsKinematic(kinematic);
}

void CompRigidBody::SetColliderPosition()
{
	Quat quat = transform->GetRotGlobal();
	float3 fpos = transform->GetPosGlobal();

	if (collider_comp)
	{
		 quat = quat*collider_comp->GetLocalQuat();
		 fpos = fpos + quat * collider_comp->GetPosition();
	}

	body->SetTransform(fpos, quat);
}

void CompRigidBody::SetColliderComp(CompCollider * new_comp)
{
	collider_comp = new_comp;
}

jpPhysicsRigidBody * CompRigidBody::GetPhysicsBody() const
{
	return body;
}

void CompRigidBody::UpdateParentPosition()
{
	// Change variables to mathgeolib
	float3 fpos;
	Quat quat;
	body->GetTransform(fpos, quat);

	Quat global_rot = transform->GetRotParent().Conjugated();

	// Global to local transform
	float3 p_pos = transform->GetPosParent();
	float3 p_scale = transform->GetParentScale();
	quat = global_rot * quat;
	fpos = global_rot * (fpos - transform->GetPosParent());
	fpos = float3(fpos.x / p_scale.x, fpos.y / p_scale.y, fpos.z / p_scale.z);

	// Collider to local transform
	if (collider_comp) {
		Quat local_rot = collider_comp->GetLocalQuat().Conjugated();
		fpos -= quat * collider_comp->GetPosition();
		quat = quat * local_rot;
	}

	// GameObject local transform and position
	transform->SetPos(fpos);
	transform->SetRot(quat);
}
