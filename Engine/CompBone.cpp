#include <stdio.h>
#include "Application.h"
#include "ResourceMesh.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleResourceManager.h"
#include "ImportMesh.h"
#include "GameObject.h"
#include "Scene.h"
#include "ModuleFS.h"
#include "CompTransform.h"
#include "CompBone.h"
#include "JSONSerialization.h"

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
{
	if (resource_mesh != nullptr)
	{
		if (resource_mesh->num_game_objects_use_me > 0)
		{
			resource_mesh->num_game_objects_use_me--;
		}
		resource_mesh = nullptr;
	}
	child_bones.clear();
	child_uids.clear();
	weights.clear();
}

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
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_BONE);
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	if (resource_mesh != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", resource_mesh->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", resource_mesh->name.c_str());
		}
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", resource_mesh->GetUUID());
		App->fs->json_array_dotset_float4(object, name + "Offset.Col0", offset.Col(0));
		App->fs->json_array_dotset_float4(object, name + "Offset.Col1", offset.Col(1));
		App->fs->json_array_dotset_float4(object, name + "Offset.Col2", offset.Col(2));
		App->fs->json_array_dotset_float4(object, name + "Offset.Col3", offset.Col(3));

		json_object_dotset_number_with_std(object, name + "Weights Size", weights.size());

		for (int i = 0; i < weights.size(); i++)
		{
			char str[255];
			sprintf(str, "Weight %i", i);
			std::string tmp(str);
			json_object_dotset_number_with_std(object, name + tmp + ".VertexID", weights[i].vertex_id);
			json_object_dotset_number_with_std(object, name + tmp + ".Weight", weights[i].weight);
		}

		json_object_dotset_number_with_std(object, name + "Childs Size", child_bones.size());

		for (int i = 0; i < child_bones.size(); i++)
		{
			char str[255];
			sprintf(str, "Child %i", i);
			json_object_dotset_number_with_std(object, name + std::string(str) + ".UUID", child_bones[i]->GetParent()->GetUUID());
		}
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Mesh UUID", 0);
	}
}

void CompBone::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Mesh UUID");

	if (resourceID > 0)
	{
		resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(resourceID);
		if (resource_mesh != nullptr)
		{
			resource_mesh->num_game_objects_use_me++;

			// LOAD MESH ----------------------------
			if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
			}

			offset.SetCol(0, App->fs->json_array_dotget_float4_string(object, name + "Offset.Col0"));
			offset.SetCol(1, App->fs->json_array_dotget_float4_string(object, name + "Offset.Col1"));
			offset.SetCol(2, App->fs->json_array_dotget_float4_string(object, name + "Offset.Col2"));
			offset.SetCol(3, App->fs->json_array_dotget_float4_string(object, name + "Offset.Col3"));

			uint weights_size = json_object_dotget_number_with_std(object, name + "Weights Size");
			weights.resize(weights_size);

			for (int i = 0; i < weights.size(); i++)
			{
				char str[255];
				sprintf(str, "Weight %i", i);
				std::string tmp(str);
				weights[i].vertex_id = json_object_dotget_number_with_std(object, name + tmp + ".VertexID");
				weights[i].weight = json_object_dotget_number_with_std(object, name + tmp + ".Weight");
			}

			uint childs_size = json_object_dotget_number_with_std(object, name + "Childs Size");
			child_bones.resize(childs_size);
			child_uids.resize(childs_size);

			for (int i = 0; i < child_bones.size(); i++)
			{
				char str[255];
				sprintf(str, "Child %i", i);
				child_uids[i] = json_object_dotget_number_with_std(object, name + std::string(str) + ".UUID");
			}
		}
	}

	
	Enable();
}

void CompBone::Link()
{
	for (int i = 0; i < child_bones.size(); i++)
		child_bones[i] = App->scene->GetGameObjectbyuid(child_uids[i])->GetComponentBone();
}

void CompBone::SyncComponent(GameObject * sync_parent)
{
	// Add bounding box ------
	parent->AddBoundingBox(resource_mesh);
}

void CompBone::GetOwnBufferSize(uint & buffer_size)
{
	Component::GetOwnBufferSize(buffer_size);
	buffer_size += sizeof(int);					//UID

	if (resource_mesh != nullptr)
	{
		buffer_size += sizeof(int);					//resource_mesh->GetUUID()
		buffer_size += sizeof(float);				//offset.Col(0)
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//

		buffer_size += sizeof(float);				//offset.Col(1)
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//

		buffer_size += sizeof(float);				//offset.Col(2)
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//

		buffer_size += sizeof(float);				//offset.Col(3)
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//
		buffer_size += sizeof(float);				//

		buffer_size += sizeof(int);					//weights.size()
		for (int i = 0; i < weights.size(); i++)
		{
			buffer_size += sizeof(int);				//weights[i].vertex_id
			buffer_size += sizeof(float);			//weights[i].weight
		}

		buffer_size += sizeof(int);					//child_bones.size()
		for (int i = 0; i < child_bones.size(); i++)
		{
			buffer_size += sizeof(int);					//child_bones[i]->GetParent()->GetUUID()
		}
	}
	else
	{
		buffer_size += sizeof(int);				//0
	}

}

void CompBone::SaveBinary(char ** cursor, int position) const
{
	Component::SaveBinary(cursor, position);
	App->json_seria->SaveIntBinary(cursor, GetUUID());

	if (resource_mesh != nullptr)
	{
		App->json_seria->SaveIntBinary(cursor, resource_mesh->GetUUID());

		math::float4 tempOffset = offset.Col(0);
		App->json_seria->SaveFloat4Binary(cursor, tempOffset);
		tempOffset = offset.Col(1);
		App->json_seria->SaveFloat4Binary(cursor, tempOffset);
		tempOffset = offset.Col(2);
		App->json_seria->SaveFloat4Binary(cursor, tempOffset);
		tempOffset = offset.Col(3);
		App->json_seria->SaveFloat4Binary(cursor, tempOffset);

		App->json_seria->SaveIntBinary(cursor, weights.size());
		for (int i = 0; i < weights.size(); i++)
		{
			App->json_seria->SaveIntBinary(cursor, weights[i].vertex_id);
			App->json_seria->SaveFloatBinary(cursor, weights[i].weight);
		}

		App->json_seria->SaveIntBinary(cursor, child_bones.size());
		for (int i = 0; i < child_bones.size(); i++)
		{
			App->json_seria->SaveIntBinary(cursor, child_bones[i]->GetParent()->GetUUID());
		}
	}
	else
	{
		App->json_seria->SaveIntBinary(cursor, 0);
	}
}

void CompBone::LoadBinary(char ** cursor)
{
	uid = App->json_seria->LoadIntBinary(cursor);
	uint resourceID = App->json_seria->LoadIntBinary(cursor);
	if (resourceID > 0)
	{
		resource_mesh = (ResourceMesh*)App->resource_manager->GetResource(resourceID);
		if (resource_mesh != nullptr)
		{
			resource_mesh->num_game_objects_use_me++;

			// LOAD MESH ----------------------------
			if (resource_mesh->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMesh->LoadResource(std::to_string(resource_mesh->GetUUID()).c_str(), resource_mesh);
			}

			offset.SetCol(0, App->json_seria->LoadFloat4Binary(cursor));
			offset.SetCol(1, App->json_seria->LoadFloat4Binary(cursor));
			offset.SetCol(2, App->json_seria->LoadFloat4Binary(cursor));
			offset.SetCol(3, App->json_seria->LoadFloat4Binary(cursor));

			uint weights_size = App->json_seria->LoadIntBinary(cursor);
			weights.resize(weights_size);

			for (int i = 0; i < weights.size(); i++)
			{
				weights[i].vertex_id = App->json_seria->LoadIntBinary(cursor);
				weights[i].weight = App->json_seria->LoadFloatBinary(cursor);
			}

			uint childs_size = App->json_seria->LoadIntBinary(cursor);
			child_bones.resize(childs_size);
			child_uids.resize(childs_size);

			for (int i = 0; i < child_bones.size(); i++)
			{
				child_uids[i] = App->json_seria->LoadIntBinary(cursor);
			}
		}
	}
}

void CompBone::GenSkinningMatrix(const float4x4& parent_transform)
{
	float4x4 transform(parent_transform * parent->GetComponentTransform()->GetLocalTransform());

	skinning_matrix = transform * offset;

	for (std::vector<CompBone*>::iterator it = child_bones.begin(); it != child_bones.end(); ++it)
		(*it)->GenSkinningMatrix(transform);
}

const float4x4& CompBone::GetSkinningMatrix()
{
	return skinning_matrix;
}
