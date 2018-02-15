#include "CompAnimation.h"
#include "ResourceAnimation.h"
#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleImporter.h"
#include "ImportAnimation.h"
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "Scene.h"
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

void CompAnimation::SetResource(ResourceAnimation * resource_animation, bool isImport)
{
	if (animation_resource != resource_animation)
	{
		if (animation_resource != nullptr)
		{
			if (animation_resource->num_game_objects_use_me > 0)
			{
				animation_resource->num_game_objects_use_me--;
			}
		}
		animation_resource = resource_animation;
		if (isImport)
		{
			// Fix Bug with Delete Import -----
			animation_resource->num_game_objects_use_me = 0;
		}
		else
		{
			animation_resource->num_game_objects_use_me++;
		}
	}
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
	if (animation_resource == nullptr)
	{
		ImGui::Text("Name:"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.25f, 1.00f, 0.00f, 1.00f), "None (Animation)");
		if (ImGui::Button("Select Animation..."))
		{
			select_animation = true;
		}
	}
	if (animation_resource == nullptr || select_animation)
	{
		if (select_animation)
		{
			ResourceAnimation* temp = (ResourceAnimation*)App->resource_manager->ShowResources(select_animation, Resource::Type::ANIMATION);
			if (temp != nullptr)
			{
				if (animation_resource != nullptr)
				{
					if (animation_resource->num_game_objects_use_me > 0)
					{
						animation_resource->num_game_objects_use_me--;
					}
				}
				animation_resource = temp;
				animation_resource->num_game_objects_use_me++;
				if (animation_resource->IsLoadedToMemory() == Resource::State::UNLOADED)
				{
					App->importer->iAnimation->LoadResource(animation_resource->path_assets.c_str(), animation_resource);
				}
				Enable();
			}
		}
	}
	ImGui::TreePop();
}

void CompAnimation::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", C_ANIMATION);
	json_object_dotset_number_with_std(object, name + "UUID", uid);

	if (animation_resource != nullptr)
	{
		if (saveScene == false)
		{
			// Save Info of Resource in Prefab (next we use this info for Reimport this prefab)
			std::string temp = std::to_string(countResources++);
			json_object_dotset_number_with_std(object, "Info.Resources.Resource " + temp + ".UUID Resource", animation_resource->GetUUID());
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", animation_resource->name);
		}
		json_object_dotset_number_with_std(object, name + "Resource Animation UUID", animation_resource->GetUUID());
	}
	else
	{
		json_object_dotset_number_with_std(object, name + "Resource Animation UUID", 0);
	}

	json_object_dotset_number_with_std(object, name + "NumberOfClips", animation_clips.size());

	int i = 0;
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); ++it)
	{
		json_object_dotset_string_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Name", (*it)->name.c_str());
		json_object_dotset_boolean_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Loop", (*it)->loop);
		json_object_dotset_boolean_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Finished", (*it)->finished);
		json_object_dotset_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Time", (*it)->time);
		json_object_dotset_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".StartTime", (*it)->start_frame_time);
		json_object_dotset_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".EndTime", (*it)->end_frame_time);
		json_object_dotset_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".State", (*it)->state);
		i++;
	}
}

void CompAnimation::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");

	uint resourceID = json_object_dotget_number_with_std(object, name + "Resource Animation UUID");
	if (resourceID > 0)
	{
		animation_resource = (ResourceAnimation*)App->resource_manager->GetResource(resourceID);
		if (animation_resource != nullptr)
		{
			animation_resource->num_game_objects_use_me++;

			// LOAD ANIMATION ----------------------------
			if (animation_resource->IsLoadedToMemory() == Resource::State::UNLOADED)
			{
				App->importer->iAnimation->LoadResource(std::to_string(animation_resource->GetUUID()).c_str(), animation_resource);
			}
		}
	}

	int num_clips = json_object_dotget_number_with_std(object, name + "NumberOfClips");
	for (int i = 0; i < num_clips; i++)
	{
		AnimationClip* temp = new AnimationClip();
		temp->name = json_object_dotget_string_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Name");
		temp->loop = json_object_dotget_boolean_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Loop");
		temp->finished = json_object_dotget_boolean_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Finished");
		temp->time = json_object_dotget_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".Time");
		temp->start_frame_time = json_object_dotget_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".StartTime");
		temp->end_frame_time = json_object_dotget_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".EndTime");
		temp->state = (AnimationState)(int)json_object_dotget_number_with_std(object, "Info.AnimationClips.Clip" + std::to_string(i) + ".State");
	}
}

void AnimationClip::RestartAnimationClip()
{
	time = start_frame_time;
}
