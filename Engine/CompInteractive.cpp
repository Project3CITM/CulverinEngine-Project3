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
#include "Scene.h"
#include "ModuleEventSystemV2.h"
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

	component_event.Set_event_data(EventType::EVENT_PASS_COMPONENT);
	component_event.pass_component.component = this;

	name_component = "Interactive";
}

CompInteractive::CompInteractive(const CompInteractive & copy, GameObject * parent) :Component(copy.GetType(), parent)
{
}

CompInteractive::~CompInteractive()
{
}
bool CompInteractive::Enable()
{
	if (!active)
	{
		active = true;
	}
	iteractive_list.push_back(this);

	return active;
}
bool CompInteractive::Disable()
{
	if (active)
	{
		active = false;
		iteractive_list.remove(this);

	}
	return active;
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
	target_graphic->SetInteractive(nullptr);
	target_graphic = nullptr;
	image = nullptr;
	if (!iteractive_list.empty())
		iteractive_list.remove(this);
	if (!navigation.assigned_to_me.empty())
	{
		for (int i = 0; i < navigation.assigned_to_me.size(); i++)
		{
			navigation.assigned_to_me[i]->NavigationRemove(this);
		}
	}
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
void CompInteractive::SyncComponent(GameObject* sync_parent)
{
	if (target_graphic_uid != 0)
	{
		SetTargetGraphic((CompGraphic*)parent->FindComponentByUUID(target_graphic_uid));
	}
	std::vector<Component*> script_vec;

	if (sync_parent != nullptr)
	{
		if (navigation.inteactive_up_uid != 0)
		{
			navigation.interactive_up = (CompInteractive*)sync_parent->GetComponentsByUID(navigation.inteactive_up_uid, true);
			navigation.inteactive_up_uid = navigation.interactive_up->GetUUID();
			navigation.interactive_up->navigation.assigned_to_me.push_back(this);
		}

		if (navigation.inteactive_down_uid != 0)
		{
			navigation.interactive_down = (CompInteractive*)sync_parent->GetComponentsByUID(navigation.inteactive_down_uid, true);
			navigation.inteactive_down_uid = navigation.interactive_down->GetUUID();

			navigation.interactive_down->navigation.assigned_to_me.push_back(this);

		}
		if (navigation.inteactive_right_uid != 0)
		{
			navigation.interactive_right = (CompInteractive*)sync_parent->GetComponentsByUID(navigation.inteactive_right_uid, true);
			navigation.inteactive_right_uid = navigation.interactive_right->GetUUID();

			navigation.interactive_right->navigation.assigned_to_me.push_back(this);

		}
		if (navigation.inteactive_left_uid != 0)
		{
			navigation.interactive_left = (CompInteractive*)sync_parent->GetComponentsByUID(navigation.inteactive_left_uid, true);
			navigation.inteactive_left_uid = navigation.interactive_left->GetUUID();

			navigation.interactive_left->navigation.assigned_to_me.push_back(this);
		}
	}
	else
	{
		if (navigation.inteactive_up_uid != 0)
		{
			navigation.interactive_up = (CompInteractive*)App->scene->root->GetComponentsByUID(navigation.inteactive_up_uid, true);
			navigation.inteactive_up_uid = navigation.interactive_up->GetUUID();
			navigation.interactive_up->navigation.assigned_to_me.push_back(this);

		}
		if (navigation.inteactive_down_uid != 0)
		{
			navigation.interactive_down = (CompInteractive*)App->scene->root->GetComponentsByUID(navigation.inteactive_down_uid, true);
			navigation.inteactive_down_uid = navigation.interactive_down->GetUUID();

			navigation.interactive_down->navigation.assigned_to_me.push_back(this);

		}
		if (navigation.inteactive_right_uid != 0)
		{
			navigation.interactive_right = (CompInteractive*)App->scene->root->GetComponentsByUID(navigation.inteactive_right_uid, true);
			navigation.inteactive_right_uid = navigation.interactive_right->GetUUID();

			navigation.interactive_right->navigation.assigned_to_me.push_back(this);

		}
		if (navigation.inteactive_left_uid != 0)
		{
			navigation.interactive_left = (CompInteractive*)App->scene->root->GetComponentsByUID(navigation.inteactive_left_uid, true);
			navigation.inteactive_left_uid = navigation.interactive_left->GetUUID();

			navigation.interactive_left->navigation.assigned_to_me.push_back(this);

		}
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

bool CompInteractive::IsSelective() const
{
	return selective;
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

void CompInteractive::NavigationRemove(CompInteractive * to_remove)
{
	if (navigation.interactive_up == to_remove)
		navigation.interactive_up = nullptr;
	if (navigation.interactive_down == to_remove)
		navigation.interactive_down = nullptr;
	if (navigation.interactive_right == to_remove)
		navigation.interactive_right = nullptr;
	if (navigation.interactive_left == to_remove)
		navigation.interactive_left = nullptr;
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
	target_graphic->SetInteractive(this);
	target_graphic_uid = target_graphic->GetUUID();
	TryConversion();
}

void CompInteractive::TryConversion()
{
	if (target_graphic->GetType() == C_IMAGE)
		image = (CompImage*)target_graphic;
}

void CompInteractive::ShowNavigationInfo()
{
	if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
	{
		ImGui::Text("Navigation Up:");
		if (navigation.interactive_up != nullptr)
		{
			ImGui::Text(navigation.interactive_up->GetParent()->GetName());
			ImGui::SameLine();
		}

		if (ImGui::Button("Select Interactive##up"))
		{
			nav_selected = 0;
			select_interactive = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear##up"))
		{
			navigation.interactive_up = nullptr;
		}
		ImGui::Text("Navigation Down:");
		if (navigation.interactive_down != nullptr)
		{
			ImGui::Text(navigation.interactive_down->GetParent()->GetName());
			ImGui::SameLine();
		}
		if (ImGui::Button("Select Interactive##down"))
		{
			nav_selected = 1;
			select_interactive = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear##down"))
		{
			navigation.interactive_down = nullptr;
		}
		ImGui::Text("Navigation Right:");
		if (navigation.interactive_right != nullptr)
		{
			ImGui::Text(navigation.interactive_right->GetParent()->GetName());
			ImGui::SameLine();
		}
		if (ImGui::Button("Select Interactive##right"))
		{
			nav_selected = 2;
			select_interactive = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear##right"))
		{
			navigation.interactive_right = nullptr;
		}
		ImGui::Text("Navigation Left:");
		if (navigation.interactive_left != nullptr)
		{
			ImGui::Text(navigation.interactive_left->GetParent()->GetName());
			ImGui::SameLine();

		}
		if (ImGui::Button("Select Interactive##left"))
		{
			nav_selected = 3;
			select_interactive = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear##left"))
		{
			navigation.interactive_left = nullptr;
		}
		
		if(select_interactive)
		{ 
			CompInteractive* temp_interactive = (CompInteractive*)ShowInteractiveWindow();
			if (temp_interactive != nullptr)
			{
				switch (nav_selected)
				{
				case 0:
					navigation.interactive_up = temp_interactive;
					navigation.interactive_up->navigation.assigned_to_me.push_back(this);
					break;
				case 1:
					navigation.interactive_down = temp_interactive;
					navigation.interactive_down->navigation.assigned_to_me.push_back(this);
					break;
				case 2:
					navigation.interactive_right = temp_interactive;
					navigation.interactive_right->navigation.assigned_to_me.push_back(this);
					break;
				case 3:
					navigation.interactive_left = temp_interactive;
					navigation.interactive_left->navigation.assigned_to_me.push_back(this);
					break;
				default:
					break;
				}
			}
		}
		//show all interactives on list x4
	}
}

Component* CompInteractive::ShowInteractiveWindow()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	Component* ret = nullptr;
	std::vector<Component*> temp_list;
	App->scene->root->GetComponentsByRangeOfType(Comp_Type::C_EDIT_TEXT, Comp_Type::C_SLIDER, &temp_list, true);
	ImGui::Begin("Interactive", &select_interactive, window_flags);

	ImVec2 WindowSize = ImGui::GetWindowSize();
	float ChildsWidth = WindowSize.x ;
	float ChildsHeight = (WindowSize.y - 50.0f);
	
		static int selected = 0;
		ImGui::BeginChild(1, ImVec2(ChildsWidth, ChildsHeight));
		for (int i = 0; i < temp_list.size(); i++)
		{
			if (ImGui::Selectable(temp_list[i]->GetParent()->GetName(), selected == i))
			{
				selected = i;
			}
		}
		ImGui::EndChild();

		if (ImGui::Button("Select##selected_interactive"))
		{
			if (selected >= 0 && selected < temp_list.size()) 
			{
				ret= temp_list[selected];
				select_interactive = false;

			}
		}
		temp_list.clear();
		ImGui::End();
		return ret;
}

CompInteractive * CompInteractive::FindNavigationOnUp()
{
	if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
	{
		return navigation.interactive_up;

	}
	else
	{
		return FindInteractive(parent->GetComponentRectTransform()->GetRot()*float3(0, 1, 0));
	}
	return nullptr;
}

void CompInteractive::OnDrag(Event event_input)
{
	image->GetRectTrasnform()->SetPos(float3(event_input.pointer.position.x, event_input.pointer.position.y, 0));
}

CompInteractive * CompInteractive::FindNavigationOnDown()
{

	if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
	{
		return navigation.interactive_down;

	}
	else
	{
		return FindInteractive(parent->GetComponentRectTransform()->GetRot()*float3(0, -1, 0));

	}
	return nullptr;
}

CompInteractive * CompInteractive::FindNavigationOnRight()
{
	if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
	{
		return navigation.interactive_right;

	}
	else
	{
		return FindInteractive(parent->GetComponentRectTransform()->GetRot()*float3(1, 0, 0));

	}
	return nullptr;
}

CompInteractive * CompInteractive::FindNavigationOnLeft()
{
	if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
	{
		return navigation.interactive_left;
	}
	else
	{
		return FindInteractive(parent->GetComponentRectTransform()->GetRot()*float3(-1, 0, 0));
	}
	return nullptr;
}

CompInteractive * CompInteractive::FindInteractive(float3 direction)
{
	float3 norm_direction = direction.Normalized();
	//float3 local_direction = parent->GetComponentRectTransform()->GetRot().Inverted() * norm_direction;
	float3 position = parent->GetComponentRectTransform()->GetGlobalPosition();
	CompInteractive* ret = nullptr;
	float best_score = -1 * INFINITY;
	for (std::list<CompInteractive*>::iterator it = iteractive_list.begin(); it != iteractive_list.end(); it++)
	{
		if (!(*it)->IsActive())
			continue;
		if ((*it) == this || (*it) == nullptr)
			continue;
		if (navigation.current_navigation_mode == Navigation::NavigationMode::NAVIGATION_NONE)
			continue;
		float3 position_it = (*it)->parent->GetComponentRectTransform()->GetGlobalPosition();
		float3 vector = position_it - position;

		float dot_value = norm_direction.Dot(vector);
		
		if (dot_value <= 0)
			continue;

		float score = dot_value / vector.Length();

		if (score > best_score)
		{
			best_score = score;
			ret = (*it);
		}		
	}
	return ret;
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

Navigation::NavigationMode CompInteractive::GetNavigationMode() const
{
	return navigation.current_navigation_mode;
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

void CompInteractive::OnMove(Event event_data)
{
	switch (event_data.gui_axis.direction)
	{
	case EGUIAxis::Direction::DIRECTION_UP:
		Navigate(event_data, FindNavigationOnUp());

		break;
	case EGUIAxis::Direction::DIRECTION_DOWN:
		Navigate(event_data, FindNavigationOnDown());

		break;
	case EGUIAxis::Direction::DIRECTION_RIGHT:
		Navigate(event_data, FindNavigationOnRight());

		break;
	case EGUIAxis::Direction::DIRECTION_LEFT:
		Navigate(event_data, FindNavigationOnLeft());

		break;
	default:
		break;
	}
}

void CompInteractive::OnSubmit(Event event_data)
{
}

void CompInteractive::OnCancel(Event event_data)
{
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
		StartAnimationSprite();
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


void CompInteractive::Navigate(Event event_data, CompInteractive * interactive)
{
	if (interactive != nullptr && interactive->IsActive())
	{
		Event pass_selected;
		pass_selected.Set_event_data(EventType::EVENT_PASS_SELECTED);
		pass_selected.pass_selected.component = interactive;
		PushEvent(pass_selected);
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

void CompInteractive::StartAnimationSprite()
{

}

