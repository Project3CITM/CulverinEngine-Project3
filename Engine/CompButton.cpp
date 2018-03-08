#include "CompButton.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompScript.h"
#include "CSharpScript.h"
#include "ModuleFS.h"
#include "ResourceMaterial.h"
#include "ImportMaterial.h"

#define BUTTON_LIMIT 10
CompButton::CompButton(Comp_Type t, GameObject * parent) :CompInteractive(t, parent)
{
	uid = App->random->Int();
	name_component = "CompButton";
}


CompButton::CompButton(const CompButton & copy, GameObject * parent) : CompInteractive(Comp_Type::C_BUTTON, parent)
{
}

CompButton::~CompButton()
{
}
void CompButton::ShowOptions()
{
	//ImGui::MenuItem("CREATE", NULL, false, false);
	if (ImGui::MenuItem("Reset"))
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
			CopyValues(((CompButton*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompButton::ShowInspectorInfo()
{
	int op = ImGui::GetWindowWidth() / 4;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionButton");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("OptionButton"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	if (ImGui::Button("Sync Image..."))
	{
		SetTargetGraphic((CompGraphic*)parent->FindComponentByType(Comp_Type::C_IMAGE));
		//select_source_image = true;
	}



	int selected_opt = current_transition_mode;
	ImGui::Text("Transition"); ImGui::SameLine(op + 30);

	if (ImGui::Combo("##transition", &selected_opt, "Color tint transition\0Sprite transition\0 Animation transition"))
	{
		if (selected_opt == Transition::TRANSITION_COLOR)
			current_transition_mode = Transition::TRANSITION_COLOR;
		if (selected_opt == Transition::TRANSITION_SPRITE)
			current_transition_mode = Transition::TRANSITION_SPRITE;
		if (selected_opt == Transition::TRANSITION_ANIMATION)
			current_transition_mode = Transition::TRANSITION_ANIMATION;	
	}
	switch (selected_opt)
	{
	case 0:
		ShowInspectorColorTransition();	
		break;
	case 1:
		ShowInspectorSpriteTransition();
		break;
	case 2:
		ShowInspectorAnimationTransition();
		break;
	default:
		break;
	}
	int navigation_opt = navigation.current_navigation_mode;
	ImGui::Text("Navigation"); ImGui::SameLine(op + 30);
	if (ImGui::Combo("##navegacion", &navigation_opt, "Desactive Navigation\0Navigation Extrict\0"))
	{
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_NONE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_NONE;
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_EXTRICTE;

	}
	if (navigation.current_navigation_mode != Navigation::NavigationMode::NAVIGATION_NONE)
	{
		ShowNavigationInfo();
	}
	ImGui::Text("On Click");

	ImGui::Text("Number of Script"); 
	ImGui::SameLine(op + 60);
	ImGui::PushItemWidth(100);
	if (ImGui::InputInt("##number_script", &number_script))
	{	
		if (number_script < 0)
		{
			number_script = 0;
		}
		else if (number_script >= BUTTON_LIMIT)
		{
			number_script = BUTTON_LIMIT;
		}
		if(linked_scripts.size()<number_script)
		{
			for (int i = linked_scripts.size(); i < number_script; i++)
			{
				CompScript* sc = nullptr;
				linked_scripts.push_back(sc);
			}
		}
		else
		{
			int size = linked_scripts.size() - number_script;
			for (int i = 0; i < size; i++)
			{
				linked_scripts.pop_back();
			}
		}
	}
	ImGui::PopItemWidth();


	uint size = linked_scripts.size();
	for (uint k = 0; k < size; k++)
	{
		CompScript* item_sc = linked_scripts[k];
		if (item_sc != nullptr)
		{
			std::string name = strcmp(item_sc->GetScriptName(), "") == 0?"Empty Script": item_sc->GetScriptName();
			ImGui::Text(name.c_str());
			ImGui::SameLine();
		}
		std::string temp = std::to_string(k);
		std::string name = "Add Script" + temp;

		if (ImGui::Button(name.c_str()))
		{
			ImGui::OpenPopup(name.c_str());
		}
		if (ImGui::BeginPopup(name.c_str()))
		{
			CompScript* sc = (CompScript*)App->scene->BlitSceneComponentsAsButtons(Comp_Type::C_SCRIPT, script_name);
			if (sc != nullptr)
				linked_scripts[k] =sc;
			ImGui::EndPopup();
		}
	}

	ImGui::TreePop();
}

void CompButton::CopyValues(const CompButton* component)
{
	//more...
}

void CompButton::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
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
	json_object_dotset_number_with_std(object, name + "Linked Spites Size", linked_scripts.size());
	for (int i = 0; i < linked_scripts.size(); i++)
	{
		std::string temp = std::to_string(i);
		CompScript* sc = linked_scripts[i];
		if (sc != nullptr)
			json_object_dotset_number_with_std(object, name + "Linked Spites " + temp + " UUID", linked_scripts[i]->GetUUID());
		else
			json_object_dotset_number_with_std(object, name + "Linked Spites " + temp + " UUID", 0);
	}

	json_object_dotset_number_with_std(object, name + "Selection Mode", current_selection_state);
	json_object_dotset_number_with_std(object, name + "Transition Mode", current_transition_mode);
	json_object_dotset_number_with_std(object, name + "Navigation Mode", navigation.current_navigation_mode);

	if (navigation.interactive_up != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Interactive up", navigation.interactive_up->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Interactive up", 0);

	}
	if (navigation.interactive_down != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Interactive down", navigation.interactive_down->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Interactive down", 0);

	}
	if (navigation.interactive_right != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Interactive right", navigation.interactive_right->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Interactive right", 0);

	}
	if (navigation.interactive_left != nullptr)
	{
		json_object_dotset_number_with_std(object, name + "Interactive left", navigation.interactive_left->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Interactive left", 0);

	}
}

void CompButton::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	for (int i = 0; i < 3; i++)
	{
		std::string resource_count = std::to_string(i);

		uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Mesh UUID " + resource_count);
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
	normal_color = App->fs->json_array_dotget_float4_string(object, name + "Normal Color");
	highlighted_color = App->fs->json_array_dotget_float4_string(object, name + "Highlighted Color");
	pressed_color = App->fs->json_array_dotget_float4_string(object, name + "Pressed Color");
	disabled_color = App->fs->json_array_dotget_float4_string(object, name + "Disabled Color");
	desired_color = App->fs->json_array_dotget_float4_string(object, name + "Desired Color");
	color_multiply = json_object_dotget_number_with_std(object, name + "Color Multiply");
	fade_duration = json_object_dotget_number_with_std(object, name + "Fade Duration");
	no_fade = json_object_dotget_boolean_with_std(object, name + "Fade Active");
	start_transition = json_object_dotget_boolean_with_std(object, name + "Transition Start");
	target_graphic_uid = json_object_dotget_number_with_std(object, name + "Graphic UUID");
	number_script = json_object_dotget_number_with_std(object, name + "Linked Spites Size");

	current_selection_state = static_cast<SelectionStates>((int)json_object_dotget_number_with_std(object, name + "Selection Mode"));
	current_transition_mode = static_cast<Transition>((int)json_object_dotget_number_with_std(object, name + "Transition Mode"));
	navigation.current_navigation_mode = static_cast<Navigation::NavigationMode>((int)json_object_dotget_number_with_std(object, name + "Navigation Mode"));

	navigation.inteactive_up_uid = json_object_dotget_number_with_std(object, name + "Interactive up");
	navigation.inteactive_down_uid = json_object_dotget_number_with_std(object, name + "Interactive down");
	navigation.inteactive_right_uid = json_object_dotget_number_with_std(object, name + "Interactive right");
	navigation.inteactive_left_uid = json_object_dotget_number_with_std(object, name + "Interactive left");

	if (number_script != 0)
	{
	uid_linked_scripts = new int[number_script];
	}
	for (int i = 0; i < number_script; i++)
	{
		std::string temp = std::to_string(i);
		uid_linked_scripts[i]= json_object_dotget_number_with_std(object, name + "Linked Spites " + temp + " UUID");
	}
	Enable();
}


void CompButton::SyncScript()
{
	std::vector<Component*> script_vec;
	App->scene->root->GetComponentsByType(Comp_Type::C_SCRIPT, &script_vec, true);
	for (int i = 0; i < number_script; i++)
	{
		//Find Component with uid
		if (uid_linked_scripts[i] == 0)
		{
			CompScript* comp_script = nullptr;
			linked_scripts.push_back(comp_script);
			continue;
		}
		for (uint j = 0; j < script_vec.size(); j++)
		{
			if (script_vec[j]->GetUUID() == uid_linked_scripts[i])
			{
				linked_scripts.push_back((CompScript*)script_vec[j]);

			}
		}

	}
	RELEASE_ARRAY(uid_linked_scripts);
}

void CompButton::AddLinkedScript(const CompScript * script)
{
	linked_scripts.push_back((CompScript*)script);
}

void CompButton::OnClick()
{
	if (IsActivate() || !IsActive())
		return;
	if (linked_scripts.empty())
	{
		return;
	}

	uint size = linked_scripts.size();
	for (uint k = 0; k < size; k++)
	{
		CompScript* comp_script = linked_scripts[k];

		if (comp_script == nullptr)
			continue;
		linked_scripts[k]->csharp->DoMainFunction(CS_OnClick);
	}
}

void CompButton::OnSubmit(Event event_input)
{
	if (IsActivate() || !IsActive())
		return;
	if (linked_scripts.empty())
	{
		return;
	}

	uint size = linked_scripts.size();
	for (uint k = 0; k < size; k++)
	{
		CompScript* comp_script = linked_scripts[k];

		if (comp_script == nullptr)
			continue;
		linked_scripts[k]->csharp->DoMainFunction(CS_OnClick);
	}
}

void CompButton::OnPointDown(Event event_input)
{
	if (event_input.pointer.button != event_input.pointer.INPUT_MOUSE_LEFT)
	{
		return;
	}

	OnClick();
	point_down = true;

	UpdateSelectionState(event_input);
	PrepareHandleTransition();
}

void CompButton::ShowInspectorAnimationTransition()
{
	int op = ImGui::GetWindowWidth() / 4;

}
