#include "CompInteractive.h"
#include "Application.h"
#include "ImportMaterial.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "ModuleEventSystem.h"
#include "ResourceMaterial.h"
#include "CompGraphic.h"
#include "CompImage.h"
#include "GameObject.h"
#include "CompRectTransform.h"
#include "CompButton.h"
#include "ModuleFS.h"
//Don't touch
#define HIGHLIGHTED_SPRITE 0
#define PRESSED_SPRITE 1
#define DISSABLED_SPRITE 2
//---
std::list<CompInteractive*> CompInteractive::iteractive_list;

CompInteractive::CompInteractive(Comp_Type t, GameObject * parent) :Component(t, parent)
{
	uid = App->random->Int();
	
	sprite[HIGHLIGHTED_SPRITE] = nullptr;
	sprite[PRESSED_SPRITE] = nullptr;
	sprite[DISSABLED_SPRITE] = nullptr;
	uuid_reimported_sprite[0] = 0;
	uuid_reimported_sprite[1] = 0;
	uuid_reimported_sprite[2] = 0;

	component_event.pass_component.type = EventType::EVENT_PASS_COMPONENT;
	component_event.pass_component.component = this;
	
	iteractive_list.push_back(this);

	name_component = "Interactive";
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
	//PushImmediateEvent()
	// -------------------------------------------------------------------
}

void CompInteractive::Update(float dt)
{
	PushImmediateEvent(component_event);
	if (start_transition)
	{
		UpdateTransitionColor(dt);
	}
}

void CompInteractive::Clear()
{
	target_graphic = nullptr;
	image = nullptr;
	if (!iteractive_list.empty())
		iteractive_list.remove(this);
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
	normal_color = component->normal_color;
	highlighted_color = component->highlighted_color;

	pressed_color = component->pressed_color;

	disabled_color = component->disabled_color;

	desired_color = component->desired_color;

	color_multiply = component->color_multiply;
	fade_duration = component->fade_duration;
	no_fade = component->no_fade;
	start_transition = component->start_transition;

	sprite[HIGHLIGHTED_SPRITE] = component->sprite[HIGHLIGHTED_SPRITE];
	sprite[PRESSED_SPRITE] = component->sprite[PRESSED_SPRITE];
	sprite[DISSABLED_SPRITE] = component->sprite[DISSABLED_SPRITE];

	target_graphic = component->target_graphic;
	TryConversion();


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
				json_object_dotset_string_with_std(object, "Info.Resources.Resource " + resource_count + temp + ".Name", sprite[i]->name.c_str());
			}
			json_object_dotset_number_with_std(object, name + "Resource Mesh UUID " + resource_count, sprite[i]->GetUUID());
		}
		else
		{
			json_object_dotset_number_with_std(object, name + "Resource Mesh UUID " + resource_count, 0);
		}
	}
	App->fs->json_array_dotset_float4(object, name + "Normal Color", normal_color);
	App->fs->json_array_dotset_float4(object, name + "Highlighted Color", highlighted_color);
	App->fs->json_array_dotset_float4(object, name + "Pressed Color", pressed_color);
	App->fs->json_array_dotset_float4(object, name + "Disabled Color", disabled_color);
	App->fs->json_array_dotset_float4(object, name + "Desired Color", desired_color);
	json_object_dotset_number_with_std(object, name + "Color Multiply", color_multiply);
	json_object_dotset_number_with_std(object, name + "Fade Duration", fade_duration);
	json_object_dotset_boolean_with_std(object, name + "Fade Active", no_fade);
	json_object_dotset_boolean_with_std(object, name + "Transition Start", start_transition);

	if (target_graphic != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Graphic UUID", target_graphic_uid);
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Graphic UUID", 0);

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

				// LOAD All Materials ----------------------------
				if (sprite[i]->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(sprite[i]->GetUUID()).c_str(), sprite[i]);
				}

			}
		}
	}
	normal_color=App->fs->json_array_dotget_float4_string(object, name + "Normal Color");
	highlighted_color=App->fs->json_array_dotget_float4_string(object, name + "Highlighted Color");
	pressed_color=App->fs->json_array_dotget_float4_string(object, name + "Pressed Color");
	disabled_color=App->fs->json_array_dotget_float4_string(object, name + "Disabled Color");
	desired_color=App->fs->json_array_dotget_float4_string(object, name + "Desired Color");
	color_multiply= json_object_dotget_number_with_std(object, name + "Color Multiply");
	fade_duration=json_object_dotget_number_with_std(object, name + "Fade Duration");
	no_fade=json_object_dotget_boolean_with_std(object, name + "Fade Active");
	start_transition=json_object_dotget_boolean_with_std(object, name + "Transition Start");
	target_graphic_uid=json_object_dotget_number_with_std(object, name + "Graphic UUID");

	Enable();
}
void CompInteractive::SyncComponent()
{
	if (target_graphic_uid != 0)
	{
		target_graphic = (CompGraphic*)parent->FindComponentByUUID(target_graphic_uid);
		TryConversion();
	}

	if (GetType() == Comp_Type::C_BUTTON)
	{
		static_cast<CompButton*>(this)->SyncScript();
	}

}
bool CompInteractive::IsActivate()const
{
	return disabled;
}

void CompInteractive::Activate()
{
	disabled = false;
	current_selection_state = SelectionStates::STATE_NORMAL;
	PrepareHandleTransition();
}
void CompInteractive::Deactive()
{
	disabled = true;
	PrepareHandleTransition();
}

void CompInteractive::ForceClear(Event event_input)
{
	point_down = false;
	point_inside = false;
	interactive_selected = false;

	UpdateSelectionState(event_input);
	PrepareHandleTransition();

}

void CompInteractive::OnPointDown(Event event_input)
{
	if (event_input.pointer.button != event_input.pointer.INPUT_MOUSE_LEFT)
	{
		return;
	}


	point_down = true;

	UpdateSelectionState(event_input);
	PrepareHandleTransition();

}

void CompInteractive::OnPointUP(Event event_input)
{
	point_down = false;
	UpdateSelectionState(event_input);
	PrepareHandleTransition();

}

void CompInteractive::OnPointEnter(Event event_input)
{
	point_inside = true;
	UpdateSelectionState(event_input);
	PrepareHandleTransition();

}

void CompInteractive::OnPointExit(Event event_input)
{
	point_inside = false;
	UpdateSelectionState(event_input);
	PrepareHandleTransition();


}

void CompInteractive::OnInteractiveSelected(Event event_input)
{
	interactive_selected = true;
	UpdateSelectionState(event_input);
	PrepareHandleTransition();


}

void CompInteractive::OnInteractiveUnSelected(Event event_input)
{
	interactive_selected = false;
	UpdateSelectionState(event_input);
	PrepareHandleTransition();

}

bool CompInteractive::PointerInside(float2 position)
{
	float4 rect = parent->GetComponentRectTransform()->GetGlobalRect();
	ImGuiIO& io = ImGui::GetIO();
	float mouse_x = position.x;
	float mouse_y = position.y;
	if (!App->mode_game)
	{
	mouse_x = position.x - GetPositionDock("Scene").x;
	mouse_y = -(position.y - (GetPositionDock("Scene").y + GetSizeDock("Scene").y));

	mouse_x = (mouse_x*io.DisplaySize.x) / GetSizeDock("Scene").x;
	mouse_y = (mouse_y*io.DisplaySize.y)/GetSizeDock("Scene").y;

	}
	else
	{
		mouse_y = io.DisplaySize.y - position.y;
	}


	if (rect.x <= mouse_x &&
		rect.x + rect.z >= mouse_x &&
		rect.y >= mouse_y &&
		rect.y - rect.w  <= mouse_y)
	{

		return true;
	}

	return false;
}

void CompInteractive::SetTargetGraphic(CompGraphic * set_target_graphic)
{
	if (set_target_graphic == nullptr)
		return;
	target_graphic = set_target_graphic;
	target_graphic_uid = target_graphic->GetUUID();
	TryConversion();
}

void CompInteractive::TryConversion()
{
	if (target_graphic->GetType() == C_IMAGE)
		image = (CompImage*)target_graphic;
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

NavigationMode CompInteractive::GetNavigationMode() const
{
	return NavigationMode();
}

bool CompInteractive::IsPressed()
{
	if(IsActivate())
		return false;
	return point_down && point_inside;
}

bool CompInteractive::IsHighlighted(Event event_data)
{
	if (IsActivate())
		return false;

	if (IsPressed())
		return false;

	bool selected = interactive_selected;
	selected |= (point_down && !point_inside && event_data.pointer.focus == parent)
		|| (!point_down && point_inside && event_data.pointer.focus == parent)
		|| (!point_down && point_inside && event_data.pointer.focus == nullptr);

	return selected;
}

void CompInteractive::UpdateSelectionState(Event event_data)
{
	if (IsPressed())
	{
		current_selection_state = SelectionStates::STATE_PRESSED;
		return;
	}
	if (IsHighlighted(event_data))
	{
		current_selection_state = SelectionStates::STATE_HIGHLIGHTED;
		return;
	}
	current_selection_state = SelectionStates::STATE_NORMAL;
}
void CompInteractive::PrepareHandleTransition()
{
	SelectionStates selection_state = current_selection_state;
	if (IsActivate())
	{
		selection_state = SelectionStates::STATE_DISABLED;
	}
	HandleTransition(selection_state);
}

void CompInteractive::HandleTransition(SelectionStates selection_state)
{
	if (target_graphic == nullptr)
		return;
	float4 desired_color;
	ResourceMaterial* desired_sprite = nullptr;
	switch (selection_state)
	{
	case  SelectionStates::STATE_NORMAL:
		desired_color = normal_color;
		desired_sprite = nullptr;
		break;
	case  SelectionStates::STATE_HIGHLIGHTED:
		desired_color = highlighted_color;
		desired_sprite = sprite[HIGHLIGHTED_SPRITE];
		break;
	case  SelectionStates::STATE_PRESSED:
		desired_color = pressed_color;
		desired_sprite = sprite[PRESSED_SPRITE];

		break;
	case  SelectionStates::STATE_DISABLED:
		desired_color = disabled_color;
		desired_sprite = sprite[DISSABLED_SPRITE];
		break;
	default:
		break;
	}
	switch (current_transition_mode)
	{
	case  Transition::TRANSITION_COLOR:
		StartTransitionColor(desired_color,no_fade);
		break;
	case  Transition::TRANSITION_SPRITE:
		StartTransitionSprite(desired_sprite);
		break;
	case  Transition::TRANSITION_ANIMATION:

		break;
	default:
		break;
	}
}

void CompInteractive::ShowInspectorColorTransition()
{
	int op = ImGui::GetWindowWidth() / 4;

	ImGui::Text("Normal"); ImGui::SameLine(op + 30);
	ImGui::ColorEdit4("##normal_rgba", normal_color.ptr());
	ImGui::Text("Highlighted"); ImGui::SameLine(op + 30);
	ImGui::ColorEdit4("##highlighted_rgba", highlighted_color.ptr());
	ImGui::Text("Pressed"); ImGui::SameLine(op + 30);
	ImGui::ColorEdit4("##pressed_rgba", pressed_color.ptr());
	ImGui::Text("Disabled"); ImGui::SameLine(op + 30);
	ImGui::ColorEdit4("##disabled_rgba", disabled_color.ptr());
	ImGui::Text("Color multiply"); ImGui::SameLine(op + 30);
	ImGui::DragFloat("##color_multiply", &color_multiply, 0.1f, 0.0f, 5.0f);
	ImGui::Text("Fade duration"); ImGui::SameLine(op + 30);
	if (ImGui::DragFloat("##fade_duration", &fade_duration, 0.1f, 0.0f, 2.0f))
	{
		if (fade_duration < 0.0001)
		{
			no_fade = true;
		}
		else
		{
			no_fade = false;
		}
	}
}

void CompInteractive::ShowInspectorSpriteTransition()
{
	int op = ImGui::GetWindowWidth() / 4;
	if (ImGui::Button("Select Highlighted Sprite..."))
	{
		sprite_value = HIGHLIGHTED_SPRITE;
		select_sprite = true;
	}
	if (sprite[HIGHLIGHTED_SPRITE] != nullptr)
	{
		ImGui::SameLine(op + 120);
		ImGui::Image((ImTextureID*)sprite[HIGHLIGHTED_SPRITE]->GetTextureID(), ImVec2(25, 25), ImVec2(-1, 1), ImVec2(0, 0));

	}
	if (ImGui::Button("Select Pressed Sprite..."))
	{
		sprite_value = PRESSED_SPRITE;
		select_sprite = true;
	}
	if (sprite[PRESSED_SPRITE] != nullptr)
	{
		ImGui::SameLine(op + 120);
		ImGui::Image((ImTextureID*)sprite[PRESSED_SPRITE]->GetTextureID(), ImVec2(25, 25), ImVec2(-1, 1), ImVec2(0, 0));

	}
	if (ImGui::Button("Select Disabled Sprite..."))
	{
		sprite_value = DISSABLED_SPRITE;
		select_sprite = true;
	}
	if (sprite[DISSABLED_SPRITE] != nullptr)
	{
		ImGui::SameLine(op + 120);
		ImGui::Image((ImTextureID*)sprite[DISSABLED_SPRITE]->GetTextureID(), ImVec2(25, 25), ImVec2(-1, 1), ImVec2(0, 0));
	}
// PRESSED_SPRITE 
// DISSABLED_SPRITE 
	if (sprite[sprite_value] == nullptr || select_sprite)
	{
		if (select_sprite)
		{
			ResourceMaterial* temp = (ResourceMaterial*)App->resource_manager->ShowResources(select_sprite, Resource::Type::MATERIAL);
			if (temp != nullptr)
			{
				if (sprite[sprite_value] != nullptr)
				{
					if (sprite[sprite_value]->num_game_objects_use_me > 0)
					{
						sprite[sprite_value]->num_game_objects_use_me--;
					}
				}
				sprite[sprite_value] = temp;
				sprite[sprite_value]->num_game_objects_use_me++;
				if (sprite[sprite_value]->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(sprite[sprite_value]->GetUUID()).c_str(), sprite[sprite_value]);
				}
				Enable();
			}
		}
	}
}


void CompInteractive::StartTransitionColor(float4 color_to_change, bool no_fade)
{
	if (no_fade)
	{
		target_graphic->SetColor(color_to_change);
		return;
	}
	
		start_transition = true;

	
}

void CompInteractive::UpdateTransitionColor(float dt)
{
	
}

void CompInteractive::StartTransitionSprite(ResourceMaterial * sprite_to_change)
{
	if (image == nullptr)
		return;
	image->SetOverwriteImage(sprite_to_change);
	image->UpdateSpriteId();
}
