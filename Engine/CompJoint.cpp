#include "CompJoint.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "Scene.h"
#include "ModulePhysics.h"

#include "CompRigidBody.h"
#include "jpPhysicsJoint.h"

// COMPONENT JOINT -------------------------------------------------
CompJoint::CompJoint(Comp_Type t, GameObject* parent) : Component(t,parent)
{
	uid = App->random->Int();
	name_component = "CompJoint";

	joint = new jpJoint();
}

CompJoint::CompJoint(const CompJoint & copy, GameObject * parent) : Component(Comp_Type::C_JOINT, parent)
{
	uid = App->random->Int();
	name_component = "CompJoint";
}

CompJoint::~CompJoint()
{
}

// -----------------------------------------------------------------
void CompJoint::Clear()
{
	if (joint)
	{
		if (joint->ToRelease())
		{
			CompRigidBody* first = (CompRigidBody*)parent->FindComponentByType(Comp_Type::C_RIGIDBODY);
			if (first != nullptr)
			{
				first->SetJointActor(nullptr);
			}

			if (second)
			{
				second->SetJointActor(nullptr);
				second = nullptr;
				second_uid = 0;
			}
		}

		delete joint;
		joint = nullptr;
	}
}

// -----------------------------------------------------------------
void CompJoint::ShowOptions()
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
	if (ImGui::MenuItem("Paste Component Values", NULL, false, false))
	{
		// Not implemented yet.
	}
}

void CompJoint::ShowInspectorInfo()
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

	CompRigidBody* rbody = (CompRigidBody*)App->scene->BlitSceneComponentsAsButtons(Comp_Type::C_RIGIDBODY, second_name);
	if (rbody != nullptr)
	{
		second = rbody;
		second_uid = second->GetUUID();
	}

	ImGui::Text("Distance");
	if (ImGui::InputFloat("Min", &min_dist, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue) ||
		ImGui::InputFloat("Max", &max_dist, 0.1f, 1.f, 2, ImGuiInputTextFlags_EnterReturnsTrue))
	{
		SetMinMaxDistance();
		// Maybe Should Set Second Position in min/max range;
	}
	ImGui::Separator();

	if(ImGui::Button("Create Joint"))
	{
		CreateJoint();
	}

	ImGui::TreePop();
}

void CompJoint::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	
	if (second != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Second UUID", second->GetUUID());
		json_object_dotset_string_with_std(object, name + "Second Name", second_name.c_str());
	}
	
	json_object_dotset_number_with_std(object, name + "Min Distance", min_dist);
	json_object_dotset_number_with_std(object, name + "Max Distance", max_dist);
}

void CompJoint::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	second_uid = json_object_dotget_number_with_std(object, name + "Second UUID");
	second_name = json_object_dotget_string_with_std(object, name + "Second Name");

	min_dist = json_object_dotget_number_with_std(object, name + "Min Distance");
	max_dist = json_object_dotget_number_with_std(object, name + "Max Distance");
}

void CompJoint::SyncComponent(GameObject* sync_parent)
{
	if (second_uid != 0)
	{
		std::vector<Component*> rigidbody_vec;
		if (sync_parent != nullptr)
		{
			sync_parent->GetComponentsByType(Comp_Type::C_RIGIDBODY, &rigidbody_vec, true);
		}
		else
		{
			App->scene->root->GetComponentsByType(Comp_Type::C_RIGIDBODY, &rigidbody_vec, true);
		}

		for (uint i = 0; i < rigidbody_vec.size(); i++)
		{
			if (rigidbody_vec[i]->GetUUID() == second_uid)
			{
				second = (CompRigidBody*)rigidbody_vec[i];
				CreateJoint();
			}
		}
	}
}

void CompJoint::CreateJoint()
{
	if (second != nullptr)
	{
		CompRigidBody* first = (CompRigidBody*)parent->FindComponentByType(Comp_Type::C_RIGIDBODY);
		if (first != nullptr)
		{
			first->SetJointActor(this);
			second->SetJointActor(this);
			second_uid = second->GetUUID();

			joint->CreateDistanceJoint(App->physics->GetPhysX(), first->GetPhysicsBody(), second->GetPhysicsBody(), float3(0,0,0), float3(0, 0, 0));
			joint->SetLimitDistance(min_dist, max_dist);
		}
	}
}

void CompJoint::SetMinMaxDistance()
{
	if (joint)
	{
		if (min_dist < 0)
		{
			min_dist = 1.f;
		}

		if (max_dist < 0)
		{
			max_dist = 1.f;
		}

		joint->SetLimitDistance(min_dist, max_dist);
	}
}

void CompJoint::RemoveActors(CompRigidBody * body)
{
	if (joint && joint->ToRelease())
	{
		if (second != nullptr)
		{
			if (second == body)
			{
				CompRigidBody* first = (CompRigidBody*)parent->FindComponentByType(C_RIGIDBODY);
				if (first != nullptr)
				{
					first->SetJointActor(nullptr);
				}
			}

			second->SetJointActor(nullptr);
			second = nullptr;
			second_uid = 0;
		}

		joint->Release();
	}
}

void CompJoint::RemoveActors()
{
	if (joint && joint->ToRelease())
	{
		CompRigidBody* first = (CompRigidBody*)parent->FindComponentByType(Comp_Type::C_RIGIDBODY);
		if (first != nullptr)
		{
			first->SetJointActor(nullptr);
		}

		if (second != nullptr)
		{
			second->SetJointActor(nullptr);
			second = nullptr;
			second_uid = 0;
		}

		joint->Release();
	}
	
}
