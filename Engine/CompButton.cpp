#include "CompButton.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CompScript.h"
#include "CSharpScript.h"

CompButton::CompButton(Comp_Type t, GameObject * parent) :CompInteractive(t, parent)
{
	uid = App->random->Int();
	name_component = "Button";
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

	if (ImGui::Combo("##transition", &selected_opt, "Color tint transition\0Sprite transition\0 Animation transition", 3))
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

	ImGui::Text("On Click");
	if (ImGui::Button("Add Script"))
	{
		ImGui::OpenPopup("Add Script");
	}

	if (ImGui::BeginPopup("Add Script"))
	{
		CompScript* sc = (CompScript*)App->scene->BlitSceneComponentsAsButtons(Comp_Type::C_SCRIPT);
		if(sc != nullptr)linked_scripts.push_back(sc);
		ImGui::EndPopup();
	}

	uint size = linked_scripts.size();
	for (uint k = 0; k < size; k++)
	{
		ImGui::Text(linked_scripts[k]->GetScriptName());
	}

	ImGui::TreePop();
}

void CompButton::CopyValues(const CompButton* component)
{
	//more...
}

void CompButton::Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	//...
}

void CompButton::Load(const JSON_Object* object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	//...
	Enable();
}

void CompButton::AddLinkedScript(const CompScript * script)
{
	linked_scripts.push_back((CompScript*)script);
}

void CompButton::OnClick()
{
	if (linked_scripts.empty())
	{
		return;
	}

	uint size = linked_scripts.size();
	for (uint k = 0; k < size; k++)
	{
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
