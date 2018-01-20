#include "CompMaterial.h"
#include "Color.h"
#include "GAmeObject.h"
#include "parson.h"
#include "Application.h"
#include "ModuleFS.h"
#include "ImportMaterial.h"
#include "ResourceMaterial.h"
#include "Scene.h"

CompMaterial::CompMaterial(Comp_Type t, GameObject* parent): Component(t, parent)
{
	uid = App->random->Int();
	color = White;
	nameComponent = "Material";
}

CompMaterial::CompMaterial(const CompMaterial& copy, GameObject* parent) : Component(Comp_Type::C_MATERIAL, parent)
{
	uid = App->random->Int();
	color = copy.color;
	resourceMaterial = copy.resourceMaterial;
	if (resourceMaterial != nullptr)
	{
		resourceMaterial->NumGameObjectsUseMe++;
	}

	nameComponent = "Material";
}

CompMaterial::~CompMaterial()
{
	if (resourceMaterial != nullptr)
	{
		if (resourceMaterial->NumGameObjectsUseMe > 0)
		{
			resourceMaterial->NumGameObjectsUseMe--;
		}
	}
	resourceMaterial = nullptr;
}

void CompMaterial::preUpdate(float dt)
{
	// Before delete Resource, Set this pointer to nullptr
	if (resourceMaterial != nullptr)
	{
		if (resourceMaterial->GetState() == Resource::State::WANTDELETE)
		{
			resourceMaterial = nullptr;
		}
		else if (resourceMaterial->GetState() == Resource::State::REIMPORTED)
		{
			uuidResourceReimported = resourceMaterial->GetUUID();
			resourceMaterial = nullptr;
		}
	}
	else
	{
		if (uuidResourceReimported != 0)
		{
			resourceMaterial = (ResourceMaterial*)App->resource_manager->GetResource(uuidResourceReimported);
			if (resourceMaterial != nullptr)
			{
				resourceMaterial->NumGameObjectsUseMe++;

				// Check if loaded
				if (resourceMaterial->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(resourceMaterial->GetUUID()).c_str(), resourceMaterial);
				}
				uuidResourceReimported = 0;
			}
		}
	}
}

void CompMaterial::Clear()
{
	resourceMaterial = nullptr;
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
	if (resourceMaterial != nullptr)
	{
		return resourceMaterial->GetTextureID();
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
		toDelete = true;
	}
	if (ImGui::MenuItem("Move Up", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Move Down", NULL, false, false))
	{
		// Not implmented yet.
	}
	if (ImGui::MenuItem("Copy Component", NULL, false, false))
	{
		// Component* Copy = this;
	}
	if (ImGui::MenuItem("Paste Component", NULL, false, false))
	{
		//parent->AddComponent(App->scene->copyComponent->GetType())
		// Create contructor Component Copy or add funtion to add info
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
	ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.00f));
	if (ImGui::BeginPopup("OptionsMaterial"))
	{
		/* Reset Material */
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 2));
		if (ImGui::Button("Reset Material"))
		{
			if (resourceMaterial != nullptr)
			{
				if (resourceMaterial->NumGameObjectsUseMe > 0)
				{
					resourceMaterial->NumGameObjectsUseMe--;
				}
			}
			resourceMaterial = nullptr;
			ImGui::CloseCurrentPopup();
		}
		ImGui::Separator();
		/* Select Material */
		if (ImGui::Button("Select Material..."))
		{
			selectMaterial = true;
			ImGui::CloseCurrentPopup();
		}
		ImGui::PopStyleVar();
		ImGui::EndPopup();
	}
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::ColorEdit3("", (float*)&color);

	if (resourceMaterial != nullptr)
	{
		/* Name of the material */
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "%s", resourceMaterial->name);
		
		/* Image of the texture */
		ImGui::Image((ImTextureID*)resourceMaterial->GetTextureID(), ImVec2(170, 170), ImVec2(-1, 1), ImVec2(0, 0));
	}

	if(resourceMaterial == nullptr || selectMaterial)
	{
		if (resourceMaterial == nullptr)
		{
			if (ImGui::Button("Select Material..."))
			{
				selectMaterial = true;
			}
		}
		if (selectMaterial)
		{
			ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(selectMaterial, Resource::Type::MATERIAL);
			if (temp != nullptr)
			{
				if (resourceMaterial != nullptr)
				{
					if (resourceMaterial->NumGameObjectsUseMe > 0)
					{			
						resourceMaterial->NumGameObjectsUseMe--;
					}
				}
				resourceMaterial = temp;
				resourceMaterial->NumGameObjectsUseMe++;
				if (resourceMaterial->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(resourceMaterial->GetUUID()).c_str(), resourceMaterial);
				}
				Enable();
			}
		}
	}

	ImGui::TreePop();
}

void CompMaterial::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_number_with_std(object, name + "Type", C_MATERIAL);
	float4 tempColor = { color.r, color.g, color.b, color.a };
	App->fs->json_array_dotset_float4(object, name + "Color", tempColor);
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	if (resourceMaterial != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Resource Material UUID", resourceMaterial->GetUUID());
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
		resourceMaterial = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
		if (resourceMaterial != nullptr)
		{
			resourceMaterial->NumGameObjectsUseMe++;

			// LOAD MATERIAL -------------------------
			if (resourceMaterial->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iMaterial->LoadResource(std::to_string(resourceMaterial->GetUUID()).c_str(), resourceMaterial);
			}
		}
	}
	Enable();
}
