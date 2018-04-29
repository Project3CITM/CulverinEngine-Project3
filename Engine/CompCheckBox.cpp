#include "CompCheckBox.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "CSharpScript.h"
#include "CompScript.h"
#include "ResourceMaterial.h"
#include "ModuleResourceManager.h"
#include "ModuleImporter.h"
#include "ImportMaterial.h"
#include "CompImage.h"


CompCheckBox::CompCheckBox(Comp_Type t, GameObject * parent) :CompInteractive(t, parent), ClickAction()
{
	uid = App->random->Int();
	name_component = "Check Box";
}

CompCheckBox::CompCheckBox(const CompCheckBox & copy, GameObject * parent) : CompInteractive(Comp_Type::C_CHECK_BOX, parent)
{

}

CompCheckBox::~CompCheckBox()
{
}

void CompCheckBox::ShowOptions()
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
			CopyValues(((CompCheckBox*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompCheckBox::ShowInspectorInfo()
{
	int op = ImGui::GetWindowWidth() / 4;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionCheckBox");
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

	if (ImGui::Combo("##transition", &selected_opt, "Color tint transition\0Sprite transition\0"))
	{
		if (selected_opt == Transition::TRANSITION_COLOR)
			current_transition_mode = Transition::TRANSITION_COLOR;
		if (selected_opt == Transition::TRANSITION_SPRITE)
			current_transition_mode = Transition::TRANSITION_SPRITE;
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

	ImGui::TreePop();
}

void CompCheckBox::CopyValues(const CompCheckBox * component)
{
}

void CompCheckBox::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	CompInteractive::Save(object, name, saveScene, countResources);

	SaveClickAction(object, name);

}

void CompCheckBox::Load(const JSON_Object * object, std::string name)
{
	CompInteractive::Load(object, name);
	LoadClickAction(object, name);

	Enable();
}
void CompCheckBox::SyncComponent(GameObject* sync_parent)
{
	CompInteractive::SyncComponent(sync_parent);

	SyncScript();
}
void CompCheckBox::SyncScript()
{
}

void CompCheckBox::OnPointDown(Event event_input)
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

void CompCheckBox::OnClick()
{
	if (IsActivate() || !IsActive())
		return;
	//if (actions.empty())
	//{
	//	return;
	//}

	uint size = actions.size();
	for (uint k = 0; k < size; k++)
	{
		if (actions[k].script == nullptr)
			continue;

		actions[k].script->csharp->DoPublicMethod(actions[k].method, &actions[k].value);
	}

	active = !active;

	if (tick!=nullptr)
	{
		tick->SetCanDraw(active);
	}

	}
	

void CompCheckBox::ClearLinkedScripts()
{
	linked_scripts.clear();
}

void CompCheckBox::SetTick(CompImage * tick)
{
	this->tick = tick;
}

void CompCheckBox::OnSubmit(Event event_input)
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
	}
}