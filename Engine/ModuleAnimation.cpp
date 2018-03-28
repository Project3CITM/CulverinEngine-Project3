#include "ModuleAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleGUI.h"
#include "WindowHierarchy.h"
#include "AnimableComponent.h"
#include "Component.h"

ModuleAnimation::ModuleAnimation()
{
}


ModuleAnimation::~ModuleAnimation()
{
}

void ModuleAnimation::ShowAnimationWindow(bool & active)
{
	
		//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.176f, 0.176f, 0.176f, 1.0f));
		if (!ImGui::Begin("Animation UI Window", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar))
		{
			ImGui::End();
		}
		if (((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected() != nullptr)
		{
			GameObject* go = ((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected();
			if (go->FindComponentByType(Comp_Type::C_ANIMATION_UI) == nullptr)
			{
				ImGui::Text("This GameObject don't have component animation create one");
				if(ImGui::Button("Create CompAnimation"))
				{
					go->AddComponent(Comp_Type::C_ANIMATION_UI);
				}
				ImGui::End();
				return;
			}
			//Add new animation

			if (ImGui::Button("Select Sprite..."))
			{
				new_animation_window = true;
			}

			if (new_animation_window)
			{
				for (int i = 0; i< go->GetNumComponents(); i++)
				{
					//(*it)
					Component* component = go->GetComponentbyIndex(i);
					AnimableComponent* item = dynamic_cast<AnimableComponent*>(component);

					if (item == nullptr)
						continue;
					bool open = ImGui::TreeNodeEx(component->GetName());

					if (open)
					{
						//item->ShowParameters();
						
						AnimationValue new_value = item->ShowParameters();
						if (new_value.type != PARAMETER_NONE)
						{
							int find = HaveAnimData(item);
							if (find != -1)
							{

								if (!animation_json->animations[find].HaveKeyFrameData(new_value.type))
								{
									animation_json->animations[find].key_data.push_back(KeyFrameData(0, new_value, new_value.type));
								}

							}
							else
							{
								AnimData tmp_anim_data;
								tmp_anim_data.data = item;
								tmp_anim_data.key_data.push_back(KeyFrameData(0, new_value, new_value.type));
								animation_json->animations.push_back(tmp_anim_data);
							}
						}
						
						ImGui::TreePop();

					}




				}
			}

			

		}
		ImGui::End();
	

}

void ModuleAnimation::FindAnimComponent()
{
}

void ModuleAnimation::SaveAnimation()
{
}

void ModuleAnimation::LoadAnimation()
{
}



int ModuleAnimation::HaveAnimData(const AnimableComponent * data)
{
	for (int i = 0; i < animation_json->animations.size(); i++)
	{
		AnimableComponent* item = animation_json->animations[i].data;
		if (item == data)
		{
			return i;
		}

	}
	return -1;
}


bool AnimData::HaveKeyFrameData(const ParameterValue & data)
{
	
	for (int i = 0; i <key_data.size(); i++)
	{
		ParameterValue item = key_data[i].parameter;
		if (item == data)
		{
			return true;
		}

	}
	
	return false;
}

KeyFrameData::KeyFrameData(int key, AnimationValue value, ParameterValue param) 
{
	key_frames.push_back(key);
	key_values.push_back(value);
	parameter = param;
}

KeyFrameData::KeyFrameData(int key, AnimationValue value) 
{
	key_frames.push_back(key);
	key_values.push_back(value);
}

KeyFrameData::~KeyFrameData()
{
}
