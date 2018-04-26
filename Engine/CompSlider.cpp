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
	selective = true;
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
		float3 pos = slide_bg->GetRectTrasnform()->GetPosGlobal();
		min_pos = pos.x - slide_bar->GetRectTrasnform()->GetWidth()/2;
		max_pos = pos.x + slide_bar->GetRectTrasnform()->GetWidth()/2;
		slide_bar->SetToFilled(true);
	}
	ImGui::Text("Min pos: %f", min_pos);
	ImGui::Text("Max pos: %f", max_pos);

	if (ImGui::DragFloat("##fillQuantity", &fill, 0.01f, 0.0f, 1.0f))
	{
		slide_bar->SetToFilled(true);
		slide_bar->FillAmount(fill);
	}

	int selected_opt = current_transition_mode;
	ImGui::Text("Transition"); ImGui::SameLine((ImGui::GetWindowWidth() / 4) + 30);

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
	default:
		break;
	}
	ImGui::TreePop();
}

void CompSlider::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	json_object_dotset_number_with_std(object, name + "SlideBG uid", slide_bg->GetTextureID());
	json_object_dotset_number_with_std(object, name + "SlideBAR uid", slide_bar->GetTextureID());
	json_object_dotset_number_with_std(object, name + "Fill amount", fill);

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
	uint bg_id = json_object_dotget_number_with_std(object, name + "SlideBG uid");
	uint bar_id = json_object_dotget_number_with_std(object, name + "SlideBAR uid");
	fill = json_object_dotget_number_with_std(object, name + "Fill amount");
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
				if (sprite[i]->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iMaterial->LoadResource(std::to_string(sprite[i]->GetUUID()).c_str(), sprite[i]);
				}
			}
		}
	}
	LoadSliderMaterial(slide_bg, bg_id);
	LoadSliderMaterial(slide_bar, bar_id);
	Enable();
}



void CompSlider::OnDrag(Event event_input)
{
	float2 mous_pos = event_input.pointer.position;
	int new_x = 0;
	if (mous_pos.x > min_pos && mous_pos.x < max_pos)
	{
		image->GetRectTrasnform()->SetPos(float3(mous_pos, 0));
	}
	else
	{
		if (mous_pos.x > max_pos)
		{
			new_x = max_pos;
		}
		if (mous_pos.x < min_pos)
		{
			new_x = min_pos;
		}
		image->GetRectTrasnform()->SetPos(float3(new_x,mous_pos.y, 0));
	}
}

void CompSlider::SetSliderBg(CompImage * bg)
{
	slide_bg = bg;
}

void CompSlider::SetSliderBar(CompImage * bar)
{
	slide_bar = bar;
}

