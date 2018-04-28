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
CompButton::CompButton(Comp_Type t, GameObject * parent) :CompInteractive(t, parent), ClickAction()
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

	if (ImGui::Button("Sync Image..."))
	{
		SetTargetGraphic((CompGraphic*)parent->FindComponentByType(Comp_Type::C_IMAGE));
		//select_source_image = true;
	}



	int selected_opt = current_transition_mode;
	ImGui::Text("Transition"); ImGui::SameLine(op + 30);

	if (ImGui::Combo("##transition", &selected_opt, "Color tint transition\0Sprite transition\0 Animation transition\0"))
	{
		if (selected_opt == Transition::TRANSITION_COLOR)
			current_transition_mode = Transition::TRANSITION_COLOR;
		if (selected_opt == Transition::TRANSITION_SPRITE)
			current_transition_mode = Transition::TRANSITION_SPRITE;
		if (selected_opt == Transition::TRANSITION_ANIMATION)
			current_transition_mode = Transition::TRANSITION_ANIMATION;	
	}

	if (current_transition_mode == Transition::TRANSITION_ANIMATION)
	{
		SelectAnimationState();
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
	if (ImGui::Combo("##navegacion", &navigation_opt, "Desactive Navigation\0Navigation Extrict\0Navigation Automatic\0"))
	{
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_NONE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_NONE;
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_EXTRICTE)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_EXTRICTE;
		if (navigation_opt == Navigation::NavigationMode::NAVIGATION_AUTOMATIC)
			navigation.current_navigation_mode = Navigation::NavigationMode::NAVIGATION_AUTOMATIC;

	}
	if (navigation.current_navigation_mode != Navigation::NavigationMode::NAVIGATION_NONE)
	{
		ShowNavigationInfo();
	}
	ShowOnClickInfo();
	/*
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
	*/
	ImGui::TreePop();
}

void CompButton::SelectAnimationState()
{
	ImGui::Text("Select state to animate:");
	ImGui::SameLine();
	int selection = 1;
	if (ImGui::Combo("##State", &selection, "IDLE\0HOVER\0DISABLED"))
	{
		if (selection == SelectionStates::STATE_NORMAL)
			to_anim = SelectionStates::STATE_NORMAL;
		if (selection == SelectionStates::STATE_HIGHLIGHTED)
			to_anim = SelectionStates::STATE_HIGHLIGHTED;
		if (selection == SelectionStates::STATE_DISABLED)
			to_anim = SelectionStates::STATE_DISABLED;
	}
}

void CompButton::CopyValues(const CompButton* component)
{
	//more...
}

void CompButton::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	CompInteractive::Save(object, name, saveScene, countResources);

	SaveClickAction(object,name);
}

void CompButton::Load(const JSON_Object * object, std::string name)
{
	CompInteractive::Load(object, name);

	LoadClickAction(object, name);
	/*
	if (number_script != 0)
	{
	uid_linked_scripts = new int[number_script];
	}

	for (int i = 0; i < number_script; i++)
	{
		std::string temp = std::to_string(i);
		uid_linked_scripts[i]= json_object_dotget_number_with_std(object, name + "Linked Spites " + temp + " UUID");
	}

	*/


	

	Enable();
}
void CompButton::SyncComponent(GameObject* sync_parent)
{
	CompInteractive::SyncComponent(sync_parent);
	SyncScript();
}
void CompButton::SyncScript()
{
	SyncClickAction();
	/*
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
	*/
}

void CompButton::OnClick()
{
	if (IsActivate() || !IsActive())
		return;
	if (actions.empty())
	{
		return;
	}

	uint size = actions.size();
	for (uint k = 0; k < size; k++)
	{
		if (actions[k].script == nullptr)
			continue;

		actions[k].script->csharp->DoPublicMethod(actions[k].method, &actions[k].value);
		//linked_scripts[k]->csharp->DoMainFunction(CS_OnClick);
	}
}

void CompButton::OnSubmit(Event event_input)
{
	if (IsActivate() || !IsActive())
		return;
	if (actions.empty())
	{
		return;
	}

	uint size = actions.size();
	for (uint k = 0; k < size; k++)
	{
		if (actions[k].script == nullptr)
			continue;

		actions[k].script->csharp->DoPublicMethod(actions[k].method, &actions[k].value);
		//linked_scripts[k]->csharp->DoMainFunction(CS_OnClick);
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
