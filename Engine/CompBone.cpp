#include "Application.h"
#include "ResourceMesh.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleResourceManager.h"
#include "ImportMesh.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompTransform.h"
#include "CompBone.h"

CompBone::CompBone(Comp_Type t, GameObject * parent): Component(t, parent)
{
	uid = App->random->Int();
	name_component = "Bone";
}

CompBone::CompBone(const CompBone & copy, GameObject * parent) : Component(Comp_Type::C_BONE, parent)
{
	resource_mesh = copy.resource_mesh;
	if (resource_mesh != nullptr)
		resource_mesh->num_game_objects_use_me++;
	name_component = "Bone";
}

CompBone::~CompBone()
{
	//RELEASE_ARRAY(name);
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->num_game_objects_use_me > 0)
		{
			resource_mesh->num_game_objects_use_me--;
		}
	}
	resource_mesh = nullptr;
}

bool CompBone::Enable()
{
	return false;
}

bool CompBone::Disable()
{
	return false;
}

void CompBone::Init()
{}

void CompBone::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->GetState() == Resource::State::WANTDELETE)
		{
			resource_mesh = nullptr;
		}
		else if (resource_mesh->GetState() == Resource::State::REIMPORTED)
		{
			uuid_resource_reimported = resource_mesh->GetUUID();
			resource_mesh = nullptr;
		}
	}
	else
	{
		if (uuid_resource_reimported != 0)
		{
			resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_mesh != nullptr)
			{
				resource_mesh->num_game_objects_use_me++;
				// Check if loaded!
				if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
				}
				uuid_resource_reimported = 0;
			}
		}
	}
	// -------------------------------------------------------------------
}

void CompBone::Update(float dt)
{}

void CompBone::Draw()
{}

void CompBone::Clear()
{}

void CompBone::ShowOptions()
{
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Reset"))
	{
		ImGui::CloseCurrentPopup();
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Move to Front", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move to Back", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component", NULL, false))
	{
		((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->SetComponentCopy(this);
	}
	if (ImGui::MenuItem("Paste Component As New", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (parent->FindComponentByType(((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType()) == nullptr
			|| ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType() > Comp_Type::C_CAMERA)
		{
			parent->AddComponentCopy(*((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied());
		}
	}
	if (ImGui::MenuItem("Paste Component Values", NULL, false, ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->AnyComponentCopied()))
	{
		if (this->GetType() == ((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()->GetType())
		{
			CopyValues(((CompBone*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompBone::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Animation Options");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("Animation Options"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	ImGui::TreePop();
}

void CompBone::CopyValues(const CompBone * comp)
{

}

void CompBone::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
}

void CompBone::Load(const JSON_Object * object, std::string name)
{
}

void CompBone::GenSkinningMatrix(const GameObject * mesh_go)
{
	skinning_matrix = parent->GetComponentTransform()->GetLocalTransform();

	GameObject* iterator = parent->GetParent();

	while (iterator != mesh_go)
	{
		float4x4 parent_transform = iterator->GetComponentTransform()->GetLocalTransform();
		skinning_matrix = parent_transform * skinning_matrix;
		iterator = iterator->GetParent();
	}

	skinning_matrix = offset.Inverted() * skinning_matrix;
}

const float4x4& CompBone::GetSkinningMatrix()
{
	return skinning_matrix;
}
