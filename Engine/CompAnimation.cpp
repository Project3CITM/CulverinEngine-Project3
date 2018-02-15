#include "CompAnimation.h"
#include "ResourceAnimation.h"
#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "CompCamera.h"
#include "Scene.h"
#include "ModuleConsole.h"
#include "ModuleWindow.h"
#include "WindowInspector.h"
#include "WindowSceneWorld.h"

CompAnimation::CompAnimation(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	name_component = "Animation";
}

CompAnimation::CompAnimation(const CompAnimation & copy, GameObject * parent) : Component(Comp_Type::C_ANIMATION, parent)
{
}

CompAnimation::~CompAnimation()
{
}

void CompAnimation::DrawBones()
{
}

void CompAnimation::Clear()
{
}

void CompAnimation::PreUpdate(float dt)
{
}

void CompAnimation::Update(float dt)
{
	std::vector<AnimationClip*> clips_playing;
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); ++it)
		if ((*it)->state == AnimationState::A_PLAY)
		{
			(*it)->time += dt;
			if ((*it)->time > (*it)->end_frame_time)
			{
				if ((*it)->loop == true)
				{
					(*it)->RestartAnimationClip();

					clips_playing.push_back(*it);
				}
				else
					(*it)->state = AnimationState::A_STOP;
			}
			else
				clips_playing.push_back(*it);

		}

	for (std::vector<std::pair<GameObject*, const AnimBone*>>::iterator it = bone_update_vector.begin(); it != bone_update_vector.end(); ++it)
		it->second->UpdateBone(it->first, clips_playing);
}

void CompAnimation::CopyValues(const CompAnimation* component)
{
	//more...
}


void CompAnimation::ShowOptions()
{
	if (ImGui::MenuItem("Remove Component"))
	{
		to_delete = true;
	}
	if (ImGui::MenuItem("Reset"))
	{
		ImGui::CloseCurrentPopup();
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
	if (ImGui::MenuItem("Copy Component", NULL, false))
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
			CopyValues(((CompAnimation*)((Inspector*)App->gui->win_manager[WindowName::INSPECTOR])->GetComponentCopied()));
		}
	}
}

void CompAnimation::ShowInspectorInfo()
{
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("Animation Options");
	}
	ImGui::PopStyleVar();

	// Button Options --------------------------------------
	if (ImGui::BeginPopup("Animation Options"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}
	ImGui::TreePop();
}

void CompAnimation::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_ANIMATION);
	//¿No falta aquí la UID?

}

void CompAnimation::Load(const JSON_Object * object, std::string name)
{
}

void AnimationClip::RestartAnimationClip()
{
	time = start_frame_time;
}
