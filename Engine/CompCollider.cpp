#include "CompCollider.h"
#include "GameObject.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "Scene.h"

#include "jpPhysicsRigidBody.h"

CompCollider::CompCollider(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Collider";

	body = App->physics->GetNewRigidBody(this);
}

CompCollider::CompCollider(const CompCollider& copy, GameObject* parent) : Component(Comp_Type::C_COLLIDER, parent)
{
	uid = App->random->Int();
	name_component = "Collider";
}

CompCollider::~CompCollider()
{
	if (body)
	{
		delete body;
	}
}

// -----------------------------------------------------------------
void CompCollider::ShowOptions()
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
			CopyValues(((CompCollider*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCollider::ShowInspectorInfo()
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

	// Collider type
	if (ImGui::Combo("Collider", (int*)&collider_type, "Sphere\0Plane\0Capsule\0Box\0", 4) && curr_type != collider_type) 
	{
		curr_type = collider_type;
		ChangeCollider();
	}
	// Collider Position
	if (ImGui::InputFloat3("Position", &position.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		body->SetTransform(position, local_quat);		
	}

	// Collider Angle
	if (ImGui::InputFloat3("Angle", &angle.x, 2, ImGuiInputTextFlags_EnterReturnsTrue) && body) 
	{
		// Set Good Rotation
		local_quat = Quat::FromEulerXYZ(angle.x*DEGTORAD, angle.y*DEGTORAD, angle.z*DEGTORAD);
		body->SetTransform(position, local_quat);	
	}


	ImGui::TreePop();
}

void CompCollider::CopyValues(const CompCollider * component)
{
	//more...
}

// -----------------------------------------------------------------
void CompCollider::OnTriggerEnter(Component * actor)
{
	// Call Listner OnTriggerEnter(Component* actor);
}

void CompCollider::OnTriggerLost(Component * actor)
{
	// Call Listner OnTriggerLost(Component* actor);
}

void CompCollider::ChangeCollider()
{
	if (body != nullptr)
	{
		body->SetGeometry(size, rad, curr_type);
	}
}

// -----------------------------------------------------------------
void CompCollider::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
}

void CompCollider::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
}