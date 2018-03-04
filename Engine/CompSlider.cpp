#include "CompImage.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "WindowInspector.h"
#include "GameObject.h"
#include "Scene.h"
#include "ImportMaterial.h"
#include "ResourceMaterial.h"
#include "CompCanvasRender.h"
#include "CompRectTransform.h"
#include "CompCanvas.h"
#include "ModuleFS.h"
#include <vector>
#include "CompSlider.h"
#include "ModuleInput.h"

CompSlider::CompSlider(Comp_Type t, GameObject * parent) : CompInteractive (t, parent)
{
	uid = App->random->Int();
	name_component = "CompSlider";
}

CompSlider::CompSlider(const CompImage & copy, GameObject * parent) : CompInteractive(Comp_Type::C_SLIDER, parent)
{

}

CompSlider::~CompSlider()
{
}

void CompSlider::ShowOptions()
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
			CopyValues(((CompSlider*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompSlider::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Options Slider");
	}
	ImGui::PopStyleVar();
	if (ImGui::Button("Sync Min/Max", ImVec2(120, 0)))
	{
		
		min_pos = -slide_bar->GetRectTrasnform()->GetWidth()/2;
		max_pos = slide_bar->GetRectTrasnform()->GetWidth()/2;
		slide_bar->SetToFilled(true);
	}
	ImGui::Text("Min pos: %f", min_pos);
	ImGui::Text("Max pos: %f", max_pos);

	ImGui::TreePop();
}

void CompSlider::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
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

}

void CompSlider::Load(const JSON_Object * object, std::string name)
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
	Enable();
}

void CompSlider::OnPointDown(Event event_input)
{
	if (event_input.pointer.button != event_input.pointer.INPUT_MOUSE_LEFT)
	{
		return;
	}

	OnClick();
	point_down = true;

	//UpdateSelectionState(event_input);
	//PrepareHandleTransition();
}

void CompSlider::OnClick()
{
	//float3 new_pos =()
	int x_pos = App->input->GetMouseX();
	if (x_pos > max_pos)
	{
		x_pos = max_pos;
	}
	if (x_pos < min_pos)
	{
		x_pos = min_pos;
	}
	image->GetRectTrasnform()->SetPos(float3(x_pos, slide_bar->GetGameObjectPos().y, 0));

}
