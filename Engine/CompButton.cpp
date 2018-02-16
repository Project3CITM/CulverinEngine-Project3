#include "CompButton.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"




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

void CompButton::ShowInspectorColorTransition()
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

	//ImGui::DragFloat()
}

void CompButton::ShowInspectorSpriteTransition()
{
	int op = ImGui::GetWindowWidth() / 4;

}

void CompButton::ShowInspectorAnimationTransition()
{
	int op = ImGui::GetWindowWidth() / 4;

}
