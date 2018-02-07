#include "CompMaterial.h"
#include "Color.h"
#include "GAmeObject.h"
#include "parson.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ImportMaterial.h"
#include "ResourceMaterial.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleRenderer3D.h"
#include "Scene.h"

CompMaterial::CompMaterial(Comp_Type t, GameObject* parent): Component(t, parent)
{
	uid = App->random->Int();
	color = White;
	name_component = "Material";
}

CompMaterial::CompMaterial(const CompMaterial& copy, GameObject* parent) : Component(Comp_Type::C_MATERIAL, parent)
{
	uid = App->random->Int();
	color = copy.color;
	resource_material = copy.resource_material;
	if (resource_material != nullptr)
	{
		resource_material->num_game_objects_use_me++;
	}

	name_component = "Material";
}

CompMaterial::~CompMaterial()
{
	if (resource_material != nullptr)
	{
		if (resource_material->num_game_objects_use_me > 0)
		{
			resource_material->num_game_objects_use_me--;
		}
	}
	resource_material = nullptr;
}

void CompMaterial::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource, Set this pointer to nullptr
	if (resource_material != nullptr)
	{
		if (resource_material->GetState() == Resource::State::WANTDELETE)
		{
			resource_material = nullptr;
		}
		else if (resource_material->GetState() == Resource::State::REIMPORTED)
		{
			uuid_resource_reimported = resource_material->GetUUID();
			resource_material = nullptr;
		}
	}
	else
	{
		if (uuid_resource_reimported != 0)
		{
			resource_material = (ResourceMaterial*)App->resource_manager->GetResource(uuid_resource_reimported);
			if (resource_material != nullptr)
			{
				resource_material->num_game_objects_use_me++;

				// Check if loaded
				if (resource_material->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(resource_material->GetUUID()).c_str(), resource_material);
				}
				uuid_resource_reimported = 0;
			}
		}
	}
	// -------------------------------------------------
}

void CompMaterial::Clear()
{
	resource_material = nullptr;
}

void CompMaterial::SetColor(float r, float g, float b, float a)
{
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
}

Color CompMaterial::GetColor() const
{
	return color;
}

uint CompMaterial::GetTextureID() const
{
	if (resource_material != nullptr)
	{
		return resource_material->GetTextureID();
	}

	return 0;
}

void CompMaterial::SetUUIDMesh(uint uuid)
{
	uuid_material = uuid;
}

void CompMaterial::ShowOptions()
{
	if (ImGui::MenuItem("Reset", NULL, false, false))
	{
		// Not implmented yet.
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
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component"))
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
			CopyValues(((CompMaterial*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Material"))
	{
		if (resource_material != nullptr)
		{
			if (resource_material->num_game_objects_use_me > 0)
			{
				resource_material->num_game_objects_use_me--;
			}
		}
		resource_material = nullptr;
		ImGui::CloseCurrentPopup();
	}
	/* Select Material */
	if (ImGui::MenuItem("Select Material..."))
	{
		select_material = true;
		ImGui::CloseCurrentPopup();
	}
}

void CompMaterial::ShowInspectorInfo()
{
	// Reset Values Button -------------------------------------------
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsMaterial");
	}

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionsMaterial"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();
	ImGui::ColorEdit3("", (float*)&color);

	if (resource_material != nullptr)
	{
		/* Name of the material */
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resource_material->name);
		
		/* Image of the texture */
		ImGui::Image((ImTextureID*)resource_material->GetTextureID(), ImVec2(170, 170), ImVec2(-1, 1), ImVec2(0, 0));
	}

	if(resource_material == nullptr || select_material)
	{
		if (resource_material == nullptr)
		{
			ImGui::Text("Name:"); ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "None (Material)");
			if (ImGui::Button("Select Material..."))
			{
				select_material = true;
			}
		}
		if (select_material)
		{
			ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(select_material, Resource::Type::MATERIAL);
			if (temp != nullptr)
			{
				if (resource_material != nullptr)
				{
					if (resource_material->num_game_objects_use_me > 0)
					{			
						resource_material->num_game_objects_use_me--;
					}
				}
				resource_material = temp;
				resource_material->num_game_objects_use_me++;
				if (resource_material->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(resource_material->GetUUID()).c_str(), resource_material);
				}
				Enable();
			}
		}

		int shader_pos = 0;
		std::string shaders_names;
		for (int i = 0; i < App->renderer3D->shader_manager.programs.size(); i++) {
			shaders_names += App->renderer3D->shader_manager.programs[i]->name;
			shaders_names += '\0';
		}
		if (ImGui::Combo("Inputs Mode", &shader_pos, shaders_names.c_str())) {
			material_shader = App->renderer3D->shader_manager.programs[shader_pos];
			/*
			for (std::map<UUID, Resource*>::iterator it = App->resources->resources.begin(); it != App->resources->resources.end(); ++it) {
				if ((*it).second != nullptr && (*it).second->type == Resource::shader_program) {
					ResourceShaderProgram* res = (ResourceShaderProgram*)(*it).second;

					if (res->res_shader_program != nullptr && res->res_shader_program->shader_name == shader->shader_name) {
						this->res_uuid = res->uuid;
						break;
					}
				}
			}*/
		}
	}

	ImGui::TreePop();
}

void CompMaterial::CopyValues(const CompMaterial* component)
{
	//more...
}

void CompMaterial::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_MATERIAL);
	float4 tempColor = { color.r, color.g, color.b, color.a };
	App->fs->json_array_dotset_float4(object, name + "Color", tempColor);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	if (resource_material != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Resource Material UUID", resource_material->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Material UUID", 0);
	}
}

void CompMaterial::Load(const JSON_Object* object, std::string name)
{
	float4 tempColor = App->fs->json_array_dotget_float4_string(object, name + "Color");
	color.Set(tempColor.x, tempColor.y, tempColor.z, tempColor.w);
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Material UUID");
	if (resourceID > 0)
	{
		resource_material = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
		if (resource_material != nullptr)
		{
			resource_material->num_game_objects_use_me++;

			// LOAD MATERIAL -------------------------
			if (resource_material->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(resource_material->GetUUID()).c_str(), resource_material);
			}
		}
	}
	Enable();
}
