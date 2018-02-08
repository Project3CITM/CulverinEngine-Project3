#include "CompInteractive.h"
#include "Application.h"
#include "ImportMaterial.h"

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
		else
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
	}
	
	// -------------------------------------------------------------------
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
