#include "CompAnimation.h"
#include "ResourceAnimation.h"
#include "Component.h"
#include "GameObject.h"
#include "CompTransform.h"
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
#include "CompAudio.h"

CompAnimation::CompAnimation(Comp_Type t, GameObject * parent) : Component(t, parent)
{
	name_component = "CompAnimation";
}

CompAnimation::CompAnimation(const CompAnimation & copy, GameObject * parent) : Component(Comp_Type::C_ANIMATION, parent)
{

}

CompAnimation::~CompAnimation()
{
	for (std::vector<AnimationNode*>::iterator temp = animation_nodes.begin(); temp != animation_nodes.end(); temp++)
	{
		RELEASE((*temp));
	}
	for (std::vector<AnimationClip*>::iterator temp = animation_clips.begin(); temp != animation_clips.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void CompAnimation::Draw()
{
	if (debug_draw == true)
	{
		for (std::vector<std::pair<GameObject*, const AnimBone*>>::iterator it = bone_update_vector.begin(); it != bone_update_vector.end(); ++it)
		{
			if (it->first != nullptr)
			{
				it->second->DrawDebug(it->first);
			}
		}
	}
}

void CompAnimation::Clear()
{
}

void CompAnimation::PreUpdate(float dt)
{
	if (bones_placed == false)
	{
		if (animation_resource != nullptr)
		{
			for (int i = 0; i < animation_resource->bones.size(); i++)
			{
				std::vector<GameObject*> bone_vector;
				parent->GetChildDeepSearch(animation_resource->bones[i]->name.c_str(), bone_vector);
				for (std::vector<GameObject*>::iterator it = bone_vector.begin(); it != bone_vector.end(); ++it)
				{
					bone_update_vector.push_back(std::make_pair((*it), animation_resource->bones[i]));
				}
				bone_vector.clear();
			}
			bones_placed = true;
		}
	}

}

void CompAnimation::Update(float dt)
{
	ManageActualAnimationNode(dt);
	ManageAnimationClips(current_animation,dt);
	ManageAnimationClips(blending_animation, dt);
	if (active_node != nullptr)
	{
		BlendingClip* node_blending_clip = active_node->GetActiveBlendingClip();
		if (node_blending_clip != nullptr)
		{
			ManageAnimationClips(node_blending_clip->clip, dt);
		}
	}

	if (current_animation != nullptr)
	{
		for (std::vector<std::pair<GameObject*, const AnimBone*>>::iterator it = bone_update_vector.begin(); it != bone_update_vector.end(); ++it)
		{
			if (it->first != nullptr)
			{
				it->second->UpdateBone(it->first, current_animation, active_node->GetActiveBlendingClip(), blending_animation);
			}
		}
	}

	if (active_node != nullptr)
	{
		CheckNodesConditions((active_node));
	}
}

void CompAnimation::PlayAnimation(AnimationNode * node)
{
	node->active = true;
	active_node = node;

	if (current_animation != nullptr && node->clip->total_blending_time > 0.001f)
	{
		current_animation->state = AnimationState::A_PLAY;
		node->clip->state = AnimationState::A_BLENDING;
		blending_animation = (node->clip);
		node->clip->RestartAnimationClip();
		BlendingClip* node_blending_clip = active_node->GetActiveBlendingClip();
		if (node_blending_clip != nullptr)
		{
			node_blending_clip->clip->state = AnimationState::A_BLENDING_NODE;
			node_blending_clip->clip->RestartAnimationClip();
		}
	}
	else
	{
		node->clip->state = AnimationState::A_PLAY;
		current_animation = node->clip;
		node->clip->RestartAnimationClip();
		BlendingClip* node_blending_clip = active_node->GetActiveBlendingClip();
		if (node_blending_clip != nullptr)
		{
			node_blending_clip->clip->state = AnimationState::A_BLENDING_NODE;
			node_blending_clip->clip->RestartAnimationClip();
		}
	}
}

void CompAnimation::PlayClip(const char * clip_name, bool blending)
{
	AnimationClip* temp = nullptr;
	for (std::vector<AnimationClip*>::iterator it = animation_clips.begin(); it != animation_clips.end(); it++)
	{
		if ((*it)->name == clip_name)
		{
			temp = (*it);
			temp->RestartAnimationClip();
			break;
		}
	}
	if (temp == nullptr)
	{
		LOG("Clip with name %s can't be found and won't play.", clip_name);
		return;
	}
	else if (blending == true && current_animation != nullptr && current_animation != temp && blending_animation != temp)
	{	
		current_animation->state = AnimationState::A_PLAY;
		temp->state = AnimationState::A_BLENDING;
		blending_animation = temp;
	}
	else
	{
		temp->state = AnimationState::A_PLAY;
		current_animation = temp;
		if (blending_animation != nullptr)
		{
			blending_animation->state = AnimationState::A_STOP;
			blending_animation = nullptr;
		}
	}
}

void CompAnimation::SetNode(const char * node_name)
{
	AnimationNode* temp = nullptr;
	AnimationNode* prev_node = nullptr;

	for (std::vector<AnimationNode*>::iterator it = animation_nodes.begin(); it != animation_nodes.end(); it++)
	{
		if ((*it)->name == node_name)
		{
			temp = (*it);
		}
		else
		{
			if ((*it)->active == true)
			{
				prev_node = (*it);
			}
		}
	}

	if (temp == nullptr)
	{
		LOG("Node with name %s can't be found and won't be set.", node_name);
		return;
	}
	else if(temp != prev_node)
	{
		prev_node->active = false;
		PlayAnimation(temp);
	}
}

void CompAnimation::SetTransition(const char * transition_name, bool value)
{
	if (active_node != nullptr)
	{
		for (std::vector<AnimationTransition*>::iterator it = active_node->transitions.begin(); it != active_node->transitions.end(); it++)
		{
			if ((*it)->name == transition_name)
			{
				(*it)->condition = value;
				break;
			}
		}
	}
}

bool CompAnimation::IsAnimationStopped(const char * clip_name) const
{
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); it++)
	{
		if ((*it)->name == clip_name)
		{
			if ((*it)->state == AnimationState::A_STOP)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	LOG("Couldn't find animation clip with name %s!", clip_name)
	return false;
}

bool CompAnimation::IsAnimationRunning(const char * clip_name) const
{
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); it++)
	{
		if ((*it)->name == clip_name)
		{
			if ((*it)->state == AnimationState::A_PLAY || (*it)->state == AnimationState::A_BLENDING)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	LOG("Couldn't find animation clip with name %s!", clip_name)
		return false;
}

AnimationClip * CompAnimation::GetCurrentClip() const
{
	return current_animation;
}

AnimationClip * CompAnimation::GetClipFromName(std::string name) const
{
	for (std::vector<AnimationClip*>::const_iterator temp = animation_clips.begin(); temp != animation_clips.end(); temp++)
	{
		if ((*temp)->name == name)
		{
			return (*temp);
		}
	}
	return nullptr;
}

AnimationNode * CompAnimation::GetNodeFromName(std::string name) const
{
	for (std::vector<AnimationNode*>::const_iterator temp = animation_nodes.begin(); temp != animation_nodes.end(); temp++)
	{
		if ((*temp)->name == name)
		{
			return (*temp);
		}
	}
	return nullptr;
}

AnimationTransition * CompAnimation::GetTransitionFromName(std::string name, AnimationNode * node) const
{
	for (std::vector<AnimationTransition*>::const_iterator temp = node->transitions.begin(); temp != node->transitions.end(); temp++)
	{
		if ((*temp)->name == name)
		{
			return (*temp);
		}
	}
	return nullptr;
}

AnimationClip * CompAnimation::GetBlendingClip() const
{
	return blending_animation;
}

AnimationNode * CompAnimation::GetActiveNode() const
{
	return active_node;
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
	for (std::vector<AnimationClip*>::const_iterator it = component->animation_clips.begin(); it != component->animation_clips.end(); it++)
	{
		AnimationClip* temp = GetClipFromName((*it)->name);
		if(temp == nullptr)
		{
			AnimationClip* temp = new AnimationClip();
			temp->name = (*it)->name;
			temp->start_frame_time = (*it)->start_frame_time;
			temp->end_frame_time = (*it)->end_frame_time;
			temp->speed_factor = (*it)->speed_factor;
			temp->state = (*it)->state;
			temp->loop = (*it)->loop;
			temp->total_blending_time = (*it)->total_blending_time;
			if (animation_clips.size() == 0)
			{
				current_animation = temp;
			}
			animation_clips.push_back(temp);
		}
		else
		{
			temp->start_frame_time = (*it)->start_frame_time;
			temp->end_frame_time = (*it)->end_frame_time;
			temp->speed_factor = (*it)->speed_factor;
			temp->state = (*it)->state;
			temp->loop = (*it)->loop;
			temp->total_blending_time = (*it)->total_blending_time;
		}
	}

	for (std::vector<AnimationNode*>::const_iterator it = component->animation_nodes.begin(); it != component->animation_nodes.end(); it++)
	{
		AnimationNode* temp = GetNodeFromName((*it)->name);
		if (temp == nullptr)
		{
			temp = new AnimationNode();
			temp->name = (*it)->name;
			temp->clip = GetClipFromName((*it)->clip->name);
			temp->active = (*it)->active;
			if (temp->active == true)
			{
				active_node = temp;
			}
			animation_nodes.push_back(temp);
		}
		else
		{
			temp->clip = GetClipFromName((*it)->clip->name);
			temp->active = (*it)->active;
			if (temp->active == true)
			{
				active_node = temp;
			}
		}
	}
	int i = 0;
	for (std::vector<AnimationNode*>::const_iterator it = component->animation_nodes.begin(); it != component->animation_nodes.end(); it++)
	{
		AnimationNode* tempNode = animation_nodes.at(i);
		for (std::vector<AnimationTransition*>::const_iterator trans_it = (*it)->transitions.begin(); trans_it != (*it)->transitions.end(); trans_it++)
		{
			AnimationTransition* temp = GetTransitionFromName((*trans_it)->name, tempNode);
			if (temp == nullptr)
			{
				temp = new AnimationTransition();
				temp->condition = (*trans_it)->condition;
				temp->name = (*trans_it)->name;
				temp->has_exit_time = (*trans_it)->has_exit_time;
				temp->exit_time = (*trans_it)->exit_time;
				temp->destination = GetNodeFromName((*trans_it)->destination->name);
				tempNode->transitions.push_back(temp);
			}
			else
			{
				temp->condition = (*trans_it)->condition;
				temp->has_exit_time = (*trans_it)->has_exit_time;
				temp->exit_time = (*trans_it)->exit_time;
				temp->destination = GetNodeFromName((*trans_it)->destination->name);
			}
		}
		i++;
	}
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
	if (active_node != nullptr)
	{
		ImGui::Text("%s", active_node->name.c_str());
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
	if (animation_resource != nullptr)
	{
		ShowAnimationInfo();
	}
	ImGui::TreePop();
}

void CompAnimation::ShowAnimationInfo()
{
	if (animation_resource != nullptr)
	{
		ImGui::Text("Name: %s", animation_resource->name.c_str());
		ImGui::Text("Duration: %f", animation_resource->duration);
		ImGui::Text("Ticks Per Second: %f", animation_resource->ticks_per_sec);
		ImGui::Text("Number of Bones: %i", animation_resource->bones.size());
		ImGui::Checkbox("Draw Debug", &debug_draw);
	}
	if (ImGui::Button("Create Animation Clip", ImVec2(125, 25)))
	{
		CreateAnimationClip();
	}
	if (ImGui::Button("Show ASM"))
	{
		show_asm_window = true;
	}
	ImGui::SameLine(100);
	if (ImGui::Button("Bone Transformations"))
	{
		show_bone_transformations = true;
	}
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); ++it)
	{
		if (ImGui::TreeNodeEx((*it)->name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
		{
			char name_clip[50];
			strcpy_s(name_clip, 50, (*it)->name.c_str());
			ImGui::Text("Name: ");
			ImGui::SameLine();
			if (ImGui::InputText("##nameClip", name_clip, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
			{
				(*it)->name = std::string(name_clip);
			}

			ImGui::InputFloat("Start Frame:", &(*it)->start_frame_time);
			ImGui::InputFloat("End Frame:", &(*it)->end_frame_time);
			ImGui::InputFloat("Speed Factor", &(*it)->speed_factor);
			ImGui::InputFloat("Blend-in Time", &(*it)->total_blending_time);
			ImGui::Checkbox("Loop", &(*it)->loop);
			std::string state_names;
			state_names += "Play";
			state_names += '\0';
			state_names += "Stop";
			state_names += '\0';
			state_names += "Pause";
			state_names += '\0';
			state_names += "Blending";
			state_names += '\0';
			state_names += "Blending Node";
			state_names += '\0';
			int state = (*it)->state;
			if (ImGui::Combo("State", &state, state_names.c_str()))
			{
				if ((*it)->state == A_STOP && (AnimationState)state == A_PLAY)
				{
					if (current_animation != nullptr)
					{
						(*it)->state = A_BLENDING;
						blending_animation = (*it);
						blending_animation->RestartAnimationClip();
					}
					else
					{
						(*it)->state = A_PLAY;
						current_animation = (*it);
						current_animation->RestartAnimationClip();
					}
				}
				else if ((*it)->state == A_PLAY && current_animation == (*it) && (AnimationState)state == A_STOP)
				{
					(*it)->state = AnimationState::A_STOP;
					current_animation = nullptr;
					if (blending_animation != nullptr)
					{
						current_animation = blending_animation;
						blending_animation->state = AnimationState::A_PLAY;
						blending_animation = nullptr;
					}
				}
				else
				{
					(*it)->state = (AnimationState)state;
				}
			}
			ImGui::TreePop();
		}
	}
	
	if (show_bone_transformations)
	{
		if (ImGui::Begin("Bones", &show_bone_transformations))
		{
			for (std::vector<std::pair<GameObject*, const AnimBone*>>::iterator it = bone_update_vector.begin(); it != bone_update_vector.end(); ++it)
			{
				if (ImGui::TreeNodeEx(it->first->GetName(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					ImGui::Checkbox("Translations", &it->first->animation_translations);
					ImGui::Checkbox("Rotations", &it->first->animation_rotations);
					ImGui::Checkbox("Scales", &it->first->animation_scales);
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}
	if (show_asm_window)
	{
		if (ImGui::Begin("Animation State Machine", &show_asm_window))
		{
			if (ImGui::Button("Create Animation Node", ImVec2(125, 25)))
			{
				CreateAnimationNode();
			}
			for (std::vector<AnimationNode*>::const_iterator it = animation_nodes.begin(); it != animation_nodes.end(); ++it)
			{
				if (ImGui::TreeNodeEx((*it)->name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
				{
					char name_node[50];
					strcpy_s(name_node, 50, (*it)->name.c_str());
					ImGui::Text("Name: ");
					ImGui::SameLine();
					if (ImGui::InputText("##nameNode", name_node, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
					{
						(*it)->name = std::string(name_node);
					}
					if ((*it)->active == false)
					{
						if (ImGui::Checkbox("Active", &(*it)->active))
						{
							SetActiveAnimationNode((*it));
						}
					}
					if ((*it)->active == true)
					{
						ImGui::Text("I'm active!!!");
					}
			
					if (ImGui::Button("DELETE",ImVec2(50,30)))
					{
						(*it)->to_delete = true;
					}

					ImGui::Text("Audio:");
					char name_audio[50];
					strcpy_s(name_audio, 50, (*it)->anim_audio.c_str());
					if (ImGui::InputText("##Audio Name", name_audio, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
					{
						(*it)->anim_audio = std::string(name_audio);
					}
					ImGui::SameLine(200);
					if (ImGui::DragFloat("##pos", &(*it)->audio_time, 0.05f, (*it)->clip->start_frame_time, (*it)->clip->end_frame_time));
					std::string clip_names;
					int combo_pos = 0;
					int i = 0;
					for (std::vector<AnimationClip*>::const_iterator item = animation_clips.begin(); item != animation_clips.end(); ++item)
					{
						clip_names += (*item)->name;
						clip_names += '\0';
						if ((*item) == (*it)->clip)
						{
							combo_pos = i;
						}
						i++;
					}
					i = 0;
					if (ImGui::Combo("Clip", &combo_pos, clip_names.c_str()))
					{
						(*it)->clip = animation_clips.at(combo_pos);
					}
					if (ImGui::Button("Create Blending Clip", ImVec2(125, 25)))
					{
						(*it)->CreateBlendingClip();
					}
					int j = 0;
					for (std::vector<BlendingClip*>::iterator new_item = (*it)->blending_clips.begin(); new_item != (*it)->blending_clips.end(); ++new_item, ++j)
					{
						if (ImGui::TreeNodeEx((*new_item)->name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
						{
							char name_blending_node[50];
							strcpy_s(name_blending_node, 50, (*new_item)->name.c_str());
							ImGui::Text("Name: ");
							ImGui::SameLine();
							if (ImGui::InputText("##nameBlendingClip", name_blending_node, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
							{
								(*new_item)->name = std::string(name_blending_node);
							}

							for (std::vector<AnimationClip*>::const_iterator item = animation_clips.begin(); item != animation_clips.end(); ++item)
							{
								if ((*item) == (*new_item)->clip)
								{
									combo_pos = i;
								}
								i++;
							}
							i = 0;

							if (ImGui::Combo((*new_item)->name.c_str(), &combo_pos, clip_names.c_str()))
							{
								(*new_item)->clip = animation_clips.at(combo_pos);
							}

							if(ImGui::Checkbox("Active", &(*new_item)->active))
							{
								if ((*new_item)->active == true)
								{
									(*it)->SetActiveBlendingClip((*new_item));
								}
								else
								{
									(*new_item)->clip->state = AnimationState::A_STOP;
								}
							}
							ImGui::SliderFloat("Weight", &(*new_item)->weight, 0, 1);
							ImGui::TreePop();
						}
					}
					if (ImGui::Button("Create Transition", ImVec2(125, 25)))
					{
						(*it)->CreateTransition();
					}

					for (std::vector<AnimationTransition*>::const_iterator trans_it = (*it)->transitions.begin(); trans_it != (*it)->transitions.end(); ++trans_it)
					{
						if (ImGui::TreeNodeEx((*trans_it)->name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
						{
							char name_transition[50];
							strcpy_s(name_transition, 50, (*trans_it)->name.c_str());
							ImGui::Text("Name: ");
							ImGui::SameLine();
							if (ImGui::InputText("##nameTransition", name_transition, 50, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
							{
								(*trans_it)->name = std::string(name_transition);
							}

							ImGui::Checkbox("Active", &(*trans_it)->condition);

							if (ImGui::Button("DELETE", ImVec2(50, 30)))
							{
								(*trans_it)->to_delete = true;
							}

							ImGui::Checkbox("Has Exit Time", &(*trans_it)->has_exit_time);
							ImGui::InputFloat("Exit Time:", &(*trans_it)->exit_time);

							std::string node_names;
							combo_pos = 0;
							i = 0;
							for (std::vector<AnimationNode*>::const_iterator iter = animation_nodes.begin(); iter != animation_nodes.end(); ++iter)
							{
								node_names += (*iter)->name;
								node_names += '\0';
								if ((*iter) == (*trans_it)->destination)
								{
									combo_pos = i;
								}
								i++;
							}
							ImGui::Text("Destination:");
							ImGui::SameLine();
							if (ImGui::Combo("##Transition Node", &combo_pos, node_names.c_str()))
							{
								(*trans_it)->destination = animation_nodes.at(combo_pos);
							}
							ImGui::TreePop(); //Transitions
						}					
					}
					ImGui::TreePop(); //Nodes
				}
			}
		}
		ImGui::End();
	}

	for (std::vector<AnimationNode*>::const_iterator it = animation_nodes.begin(); it != animation_nodes.end(); ++it)
	{
		for (std::vector<AnimationTransition*>::const_iterator trans_it = (*it)->transitions.begin(); trans_it != (*it)->transitions.end(); ++trans_it)
		{
			if ((*it)->transitions.size() != 0 && (*trans_it)->to_delete == true)
			{
				(*it)->transitions.erase(trans_it);
			}
		}
		if (animation_nodes.size() != 0 && (*it)->to_delete == true)
		{
			animation_nodes.erase(it);
		}
	}
	
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
			json_object_dotset_string_with_std(object, "Info.Resources.Resource " + temp + ".Name", animation_resource->name.c_str());
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
		json_object_dotset_string_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Name", (*it)->name.c_str());
		json_object_dotset_boolean_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Loop", (*it)->loop);
		json_object_dotset_boolean_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Finished", (*it)->finished);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Time", (*it)->time);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".StartTime", (*it)->start_frame_time);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".EndTime", (*it)->end_frame_time);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".BlendTime", (*it)->total_blending_time);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".CurrentBlendTime", (*it)->current_blending_time);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".SpeedFactor", (*it)->speed_factor);
		json_object_dotset_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".State", (*it)->state);
		i++;
	}

	i = 0;
	json_object_dotset_number_with_std(object, name + "NumberOfNodes", animation_nodes.size());

	for (std::vector<AnimationNode*>::const_iterator it = animation_nodes.begin(); it != animation_nodes.end(); ++it)
	{
		json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Name", (*it)->name.c_str());
		json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".ClipName", (*it)->clip->name.c_str());
		json_object_dotset_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Active", (*it)->active);
		json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".AudioName", (*it)->anim_audio.c_str());
		json_object_dotset_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".AudioTime", (*it)->audio_time);
		int r = 0;

		json_object_dotset_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + "NumberOfTransitions", (*it)->transitions.size());

		for (std::vector<AnimationTransition*>::const_iterator trans_it = (*it)->transitions.begin(); trans_it != (*it)->transitions.end(); ++trans_it)
		{
			json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(r) + ".Name", (*trans_it)->name.c_str());
			json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(r) + ".DestinationName", (*trans_it)->destination->name.c_str());
			json_object_dotset_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(r) + ".Condition", (*trans_it)->condition);
			json_object_dotset_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(r) + ".HasExitTime", (*trans_it)->has_exit_time);
			json_object_dotset_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(r) + ".ExitTime", (*trans_it)->exit_time);
			r++;
		}

		r = 0;

		json_object_dotset_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + "NumberOfBlendingClips", (*it)->blending_clips.size());

		for (std::vector<BlendingClip*>::const_iterator blend_it = (*it)->blending_clips.begin(); blend_it != (*it)->blending_clips.end(); ++blend_it)
		{
			json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(r) + ".Name", (*blend_it)->name.c_str());
			json_object_dotset_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(r) + ".ClipName", (*blend_it)->clip->name.c_str());
			json_object_dotset_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(r) + ".Active", (*blend_it)->active);
			json_object_dotset_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(r) + ".Weight", (*blend_it)->weight);
			r++;
		}
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
				App->importer->iAnimation->LoadResource(animation_resource->path_assets.c_str(), animation_resource);
			}
		}
	}

	int num_clips = json_object_dotget_number_with_std(object, name + "NumberOfClips");
	for (int i = 0; i < num_clips; i++)
	{
		AnimationClip* temp = new AnimationClip();
		temp->name = json_object_dotget_string_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Name");
		temp->loop = json_object_dotget_boolean_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Loop");
		temp->finished = json_object_dotget_boolean_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Finished");
		temp->time = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".Time");
		temp->start_frame_time = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".StartTime");
		temp->end_frame_time = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".EndTime");
		temp->total_blending_time = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".BlendTime");
		temp->current_blending_time = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".CurrentBlendTime");
		temp->speed_factor = json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".SpeedFactor");
		temp->state = (AnimationState)(int)json_object_dotget_number_with_std(object, name + "Info.AnimationClips.Clip" + std::to_string(i) + ".State");
		animation_clips.push_back(temp);
		if (temp->state == AnimationState::A_PLAY)
		{
			current_animation = temp;
		}
		else if (temp->state == AnimationState::A_BLENDING)
		{
			blending_animation = temp;
		}
	}

	int num_nodes = json_object_dotget_number_with_std(object, name + "NumberOfNodes");

	for (int i = 0; i < num_nodes; i++)
	{
		AnimationNode* temp = new AnimationNode();
		temp->name = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Name");
		temp->active = json_object_dotget_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Active");
		if (temp->active == true)
		{
			active_node = temp;
		}
		std::string clip_name = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".ClipName");
		//temp->anim_audio = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".AudioName");
		//temp->audio_time = json_object_dotget_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".AudioTime");
	
		for (std::vector<AnimationClip*>::iterator temp_it = animation_clips.begin(); temp_it != animation_clips.end(); temp_it++)
		{
			if ((*temp_it)->name == clip_name)
			{
				temp->clip = (*temp_it);
				break;
			}
		}
		animation_nodes.push_back(temp);
	}

	for (int i = 0; i < num_nodes; i++)
	{
		int num_transitions = json_object_dotget_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + "NumberOfTransitions");
		for (int j = 0; j < num_transitions; j++)
		{
			AnimationTransition* temp_transition = new AnimationTransition();
			temp_transition->name = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(j) + ".Name");
			temp_transition->condition = json_object_dotget_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(j) + ".Condition");
			temp_transition->has_exit_time = json_object_dotget_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(j) + ".HasExitTime");
			temp_transition->exit_time = json_object_dotget_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(j) + ".ExitTime");

			std::string node_name = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".Transitions" + std::to_string(j) + ".DestinationName");
			for (std::vector<AnimationNode*>::iterator temp_it = animation_nodes.begin(); temp_it != animation_nodes.end(); temp_it++)
			{
				if ((*temp_it)->name == node_name)
				{
					temp_transition->destination = (*temp_it);
					break;
				}
			}
			animation_nodes.at(i)->transitions.push_back(temp_transition);
		}

		int num_blending_clips = json_object_dotget_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + "NumberOfBlendingClips");
		for (int j = 0; j < num_blending_clips; j++)
		{
			BlendingClip* temp_blending_clip = new BlendingClip();
			temp_blending_clip->name = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(j) + ".Name");
			temp_blending_clip->active = json_object_dotget_boolean_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(j) + ".Active");
			temp_blending_clip->weight = json_object_dotget_number_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(j) + ".Weight");

			std::string clip_name_blending = json_object_dotget_string_with_std(object, name + "Info.AnimationNodes.Node" + std::to_string(i) + ".BlendingClips" + std::to_string(j) + ".ClipName");
			
			AnimationClip* temp_clip = GetClipFromName(clip_name_blending);
			if (temp_clip != nullptr)
			{
				temp_blending_clip->clip = temp_clip;
			}
			animation_nodes.at(i)->blending_clips.push_back(temp_blending_clip);
		}
	}
}

void CompAnimation::CreateAnimationClip()
{
	AnimationClip* temp_anim_clip = new AnimationClip();
	temp_anim_clip->name += std::to_string(animation_clips.size()).c_str();
	temp_anim_clip->end_frame_time = animation_resource->duration;
	animation_clips.push_back(temp_anim_clip);

	if (animation_nodes.size() == 0)
	{
		AnimationNode* entry_node = new AnimationNode();
		entry_node->clip = temp_anim_clip;
		entry_node->active = true;
		entry_node->name = "Entry";
		animation_nodes.push_back(entry_node);
		PlayAnimation(entry_node);
	}
}

void CompAnimation::ManageAnimationClips(AnimationClip* animation_clip, float dt)
{
	if(animation_clip != nullptr && animation_clip->state != AnimationState::A_STOP)
	{
		animation_clip->time += dt * animation_clip->speed_factor;
		
		

		if (animation_clip->state == AnimationState::A_BLENDING)
		{
			animation_clip->current_blending_time -= dt;

			if (animation_clip->current_blending_time <= 0)
			{
				current_animation->state = AnimationState::A_STOP;
				current_animation = animation_clip;
				animation_clip->state = AnimationState::A_PLAY;
				blending_animation = nullptr;
			}
		}
		if (animation_clip->time > animation_clip->end_frame_time)
		{
			if (animation_clip->loop == true)
			{
				animation_clip->RestartAnimationClip();
			}
			else
				animation_clip->state = AnimationState::A_STOP;
		}
	}
}

void CompAnimation::ManageActualAnimationNode(float dt)
{
	if (active_node != nullptr)
	{
		if (active_node->anim_audio != "Null_Audio")
		{
			if (active_node->clip->time > active_node->audio_time - dt && active_node->clip->time <= active_node->audio_time)
			{
				CompAudio* temp_emiter = (CompAudio*)parent->FindComponentByType(C_AUDIO);
				if (temp_emiter != nullptr)
				{
					temp_emiter->PlayAudioEvent(active_node->anim_audio.c_str());
				}
			}
		}
	}
}

void CompAnimation::CreateAnimationNode()
{
	if (animation_clips.size() > 0)
	{
		AnimationNode* temp_anim_node = new AnimationNode();
		temp_anim_node->name += std::to_string(animation_nodes.size()).c_str();
		temp_anim_node->clip = animation_clips[0];
		if (animation_nodes.size() == 0)
		{
			temp_anim_node->active = true;
			PlayAnimation(temp_anim_node);
		}
		animation_nodes.push_back(temp_anim_node);
	}
}

void CompAnimation::CheckNodesConditions(AnimationNode * node)
{
	for (std::vector<AnimationTransition*>::iterator it = node->transitions.begin(); it != node->transitions.end(); it++)
	{
		if ((*it)->has_exit_time == true)
		{
			if (node->clip->IsAnimOverXTime((*it)->exit_time))
			{
				node->active = false;
				node->clip->state = AnimationState::A_STOP;
				PlayAnimation((*it)->destination);
				return;
			}
		}
		if ((*it)->condition == true)
		{
			(*it)->condition = false;
			node->active = false;
			node->clip->state = AnimationState::A_STOP;
			PlayAnimation((*it)->destination);
		}
	}
}

void CompAnimation::SetClipsSpeed(float speed_value)
{
	for (std::vector<AnimationClip*>::iterator it = animation_clips.begin(); it != animation_clips.end(); it++)
	{
		(*it)->speed_factor = speed_value;
	}
}

bool AnimationClip::IsAnimOverXTime(float num_between_0_and_1)
{
	if (((time - start_frame_time) / (end_frame_time - start_frame_time)) >= num_between_0_and_1)
	{
		return true;
	}
	return false;
}

void AnimationClip::RestartAnimationClip()
{
	time = start_frame_time;
	current_blending_time = total_blending_time;
}

AnimationNode::~AnimationNode()
{
	for (std::vector<AnimationTransition*>::iterator temp = transitions.begin(); temp != transitions.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void AnimationNode::CreateTransition()
{
	AnimationTransition* temp_transition = new AnimationTransition();
	temp_transition->name += std::to_string(transitions.size()).c_str();
	temp_transition->destination = this;
	transitions.push_back(temp_transition);
}

void AnimationNode::CreateBlendingClip()
{
	BlendingClip* temp_blending_clip = new BlendingClip();
	temp_blending_clip->name += std::to_string(blending_clips.size());
	temp_blending_clip->clip = this->clip;
	blending_clips.push_back(temp_blending_clip);
}

void AnimationNode::SetActiveBlendingClip(BlendingClip* blnd_clip)
{
	blnd_clip->clip->state = AnimationState::A_BLENDING_NODE;
	blnd_clip->clip->RestartAnimationClip();
	for (std::vector<BlendingClip*>::iterator new_item = blending_clips.begin(); new_item != blending_clips.end(); ++new_item)
	{
		if(blnd_clip != (*new_item))
		{
			(*new_item)->clip->state = AnimationState::A_STOP;
			(*new_item)->active = false;
		}
	}
}

void AnimationNode::SetActiveBlendingClip(std::string name)
{
	for (std::vector<BlendingClip*>::iterator new_item = blending_clips.begin(); new_item != blending_clips.end(); ++new_item)
	{
		if ((*new_item)->name == name)
		{
			SetActiveBlendingClip((*new_item));
			return;
		}
	}
}

BlendingClip * AnimationNode::GetActiveBlendingClip()
{
	for (std::vector<BlendingClip*>::const_iterator new_item = blending_clips.begin(); new_item != blending_clips.end(); ++new_item)
	{
		if ((*new_item)->active == true)
		{
			return (*new_item);
		}
	}
	return nullptr;
}

void AnimationNode::SetActiveBlendingClipWeight(float weight)
{
	BlendingClip* active = GetActiveBlendingClip();
	if (active != nullptr)
	{
		active->weight = weight;
	}
}

void CompAnimation::SetActiveAnimationNode(AnimationNode* active)
{
	for (std::vector<AnimationNode*>::const_iterator new_item = animation_nodes.begin(); new_item != animation_nodes.end(); ++new_item)
	{
		if ((*new_item) != active)
		{
			(*new_item)->active = false;
		}
	}
}
