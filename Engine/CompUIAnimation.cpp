#include "CompUIAnimation.h"
#include "AnimableComponent.h"
#include "WindowInspector.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"

CompUIAnimation::CompUIAnimation(Comp_Type t, GameObject * parent):Component(t,parent)
{
	uid = App->random->Int();
	name_component = "Comp UI Animation";
}

CompUIAnimation::~CompUIAnimation()
{
}

void CompUIAnimation::Update(float dt)
{
	if (play)
	{
		if (PlayAnimation(dt))
		{
			if (loop)
			{
				ResetAnimation();
			}
			else
			{
				play = false;
			}

		}
	}

}
void CompUIAnimation::ShowOptions()
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
			CopyValues(((CompUIAnimation*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompUIAnimation::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OptionsMesh");
	}
	ImGui::PopStyleVar();


	ImGui::TreePop();
}
void CompUIAnimation::CopyValues(const CompUIAnimation * component)
{
}
bool CompUIAnimation::PlayAnimation(float dt)
{
	if (animation_json != nullptr)
	{
		if (animation_json->animations.empty())
		{
			return true;
		}
		current_time += dt;
		current_frame = current_time*sample_rate;
	

		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = &animation_json->animations[i];

			for (int j = 0; j < item->key_frame_data.size(); j++)
			{
				
				KeyFrameData* key_frame_item = &item->key_frame_data[j];

				if (current_frame == max_frames)
				{
					AnimationData data;
					data.type = key_frame_item->parameter;
					data.value = key_frame_item->key_data.back().key_values;
					item->data->SetNewAnimationValue(data);
				}
				else
				{
					AnimationData data;
					data.type = key_frame_item->parameter;
					data.value = key_frame_item->Interpolate(dt,current_frame);
					item->data->SetNewAnimationValue(data);
				}

			}
		}
	}
	if (current_frame == max_frames)
		return true;
	return false;
}

bool CompUIAnimation::ResetAnimation()
{
	if (animation_json != nullptr)
	{
		if (animation_json->animations.empty())
		{
			return true;
		}
		current_time =0;
		current_frame = current_time*sample_rate;


		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = &animation_json->animations[i];

			for (int j = 0; j < item->key_frame_data.size(); j++)
			{

				KeyFrameData* key_frame_item = &item->key_frame_data[j];
				key_frame_item->ResetInterpolationKeys();
				AnimationData data;
				data.type = key_frame_item->parameter;
				data.value = key_frame_item->key_data.front().key_values;
				item->data->SetNewAnimationValue(data);

			}
		}
	}

	return true;
}
