#include "CompInteractive.h"
#include "Application.h"
#include "ImportMaterial.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"

#include "ResourceMaterial.h"

//Don't touch
#define HIGHLIGHTED_SPRITE 0
#define PRESSED_SPRITE 1
#define DISSABLED_SPRITE 2
//---
CompInteractive::CompInteractive(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	sprite[HIGHLIGHTED_SPRITE] = nullptr;
	sprite[PRESSED_SPRITE] = nullptr;
	sprite[DISSABLED_SPRITE] = nullptr;

	uuid_reimported_sprite[0] = 0;
	uuid_reimported_sprite[1] = 0;
	uuid_reimported_sprite[2] = 0;
	name_component = "Canvas";
}

CompInteractive::CompInteractive(const CompInteractive & copy, GameObject * parent) :Component(copy.GetType(), parent)
{
}

CompInteractive::~CompInteractive()
{
}


void CompInteractive::PreUpdate(float dt)
{
	// Manage Resource -------------------------------------------------
	// Before delete Resource Set this pointer to nullptr
	for (int i = 0; i < 3; i++)
	{
		if (sprite[i] != nullptr)
		{
			if (uuid_reimported_sprite[i] != 0)
			{
				sprite[i] = (ResourceMaterial*)App->resource_manager->GetResource(uuid_reimported_sprite[i]);
				if (sprite[i] != nullptr)
				{
					sprite[i]->num_game_objects_use_me++;
					// Check if loaded!
					if (sprite[i]->IsLoadedToMemory() == Resource::State::UNLOADED)
					{
						App->importer->iMaterial->LoadResource(std::to_string(sprite[i]->GetUUID()).c_str(), sprite[i]);
					}
					uuid_reimported_sprite[i] = 0;
				}
			}
	}

	for (int i = 0; i < 3; i++)
	{
		if (sprite[i] != nullptr)
		{
			if (sprite[i]->GetState() == Resource::State::WANTDELETE)
			{
				sprite[i] = nullptr;
			}
			else if (sprite[i]->GetState() == Resource::State::REIMPORTED)
			{
				uuid_reimported_sprite[i] = sprite[i]->GetUUID();
				sprite[i] = nullptr;
			}
		}
	}
	
	}
	
	// -------------------------------------------------------------------
}

void CompInteractive::ShowOptions()
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
			CopyValues(((CompInteractive*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
	ImGui::Separator();
	if (ImGui::MenuItem("Reset Material"))
	{
		for (int i = 0; i < 3; i++)
		{
			if (sprite[i] != nullptr)
			{
				if (sprite[i]->num_game_objects_use_me > 0)
				{
					sprite[i]->num_game_objects_use_me--;
				}
			}
			sprite[i] = nullptr;

		}
		
		ImGui::CloseCurrentPopup();
	}
	/* Select Material */
	if (ImGui::MenuItem("Select Material..."))
	{
		ImGui::CloseCurrentPopup();
	}
}
void CompInteractive::CopyValues(const CompInteractive* component)
{
	//more...
}
void CompInteractive::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	for (int i = 0; i < 3; i++)
	{
		std::string resource_count = std::to_string(i);

		if (sprite[i] != nullptr)
		{
			if (saveScene == false)
			{
				// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
				std::string temp = std::to_string(countResources++);

				json_object_dotset_number_with_std(object, "Info.Resources.Resource " + resource_count + temp + ".UUID Resource", sprite[i]->GetUUID());
				json_object_dotset_string_with_std(object, "Info.Resources.Resource " + resource_count + temp + ".Name", sprite[i]->name);
			}
			json_object_dotset_number_with_std(object, name + "Resource Mesh UUID " + resource_count, sprite[i]->GetUUID());
		}
		else
		{
			json_object_dotset_number_with_std(object, name + "Resource Mesh UUID " + resource_count, 0);
		}
	}
}

void CompInteractive::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	for (int i = 0; i < 3; i++)
	{
		std::string resource_count = std::to_string(i);

		uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Mesh UUID "+ resource_count);
		if (resourceID > 0)
		{
			sprite[i] = (ResourceMaterial*)App->resource_manager->GetResource(resourceID);
			if (sprite[i] != nullptr)
			{
				sprite[i]->num_game_objects_use_me++;

				// LOAD MESH ----------------------------
				if (sprite[i]->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(sprite[i]->GetUUID()).c_str(), sprite[i]);
				}

			}
		}
	}
	Enable();
}
void CompInteractive::SetNormalColor(const float4& set_rgba)
{
	normal_color = set_rgba;

}

void CompInteractive::SetNormalColor(float set_r, float set_g, float set_b, float set_a)
{
	SetNormalColor(float4(set_r, set_g, set_b, set_a));

}

void CompInteractive::SetHighlightedColor(const float4& set_rgba)
{
	highlighted_color = set_rgba;
}

void CompInteractive::SetHighlightedColor(float set_r, float set_g, float set_b, float set_a)
{
	SetHighlightedColor(float4(set_r, set_g, set_b, set_a));
}

void CompInteractive::SetPressedColor(const float4& set_rgba)
{
	pressed_color = set_rgba;

}

void CompInteractive::SetPressedColor(float set_r, float set_g, float set_b, float set_a)
{
	SetPressedColor(float4(set_r, set_g, set_b, set_a));

}

void CompInteractive::SetDisabledColor(const float4& set_rgba)
{
	disabled_color = set_rgba;
}

void CompInteractive::SetDisabledColor(float set_r, float set_g, float set_b, float set_a)
{
	SetDisabledColor(float4(set_r, set_g, set_b, set_a));

}

void CompInteractive::SetHighlightedSprite(ResourceMaterial * set_sprite) 
{
	sprite[HIGHLIGHTED_SPRITE] = set_sprite;
}

void CompInteractive::SetPressedSprite(ResourceMaterial * set_sprite) 
{
	sprite[PRESSED_SPRITE] = set_sprite;
}

void CompInteractive::SetDisabledSprite(ResourceMaterial * set_sprite) 
{
	sprite[DISSABLED_SPRITE] = set_sprite;
}

float4 CompInteractive::GetNormalColor() const
{
	return normal_color;
}

float4 CompInteractive::GetHighlightedColor() const
{
	return highlighted_color;
}

float4 CompInteractive::GetPressedColor() const
{
	return pressed_color;
}

float4 CompInteractive::GetDisabledColor() const
{
	return disabled_color;
}

ResourceMaterial * CompInteractive::GetHighligtedSprite() const
{
	return sprite[HIGHLIGHTED_SPRITE];
}

ResourceMaterial * CompInteractive::GetPressedSprite() const
{
	return sprite[PRESSED_SPRITE];
}

ResourceMaterial * CompInteractive::GetDisabledSprite() const
{
	return sprite[DISSABLED_SPRITE];
}
