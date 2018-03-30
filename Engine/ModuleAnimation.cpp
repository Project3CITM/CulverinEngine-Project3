#include "ModuleAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "JSONSerialization.h"
#include "ModuleGUI.h"
#include "WindowHierarchy.h"
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
		if (!ImGui::Begin("Animation UI Window", &active, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar))
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
					animation_json = new AnimationJson();
					go->AddComponent(Comp_Type::C_ANIMATION_UI);
				}
				ImGui::End();
				return;
			}
			//Add new animation

			if (ImGui::Button("Select Component"))
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
						
						AnimationData new_value = item->ShowParameters();
						if (new_value.type != PARAMETER_NONE)
						{
							int find = HaveAnimData(item);
							if (find != -1)
							{

								if (!animation_json->animations[find].HaveKeyFrameData(new_value.type))
								{
									animation_json->animations[find].key_frame_data.push_back(KeyFrameData(0, new_value.value, new_value.type));
								}

							}
							else
							{
								AnimData tmp_anim_data;
								tmp_anim_data.data = item;
								tmp_anim_data.key_frame_data.push_back(KeyFrameData(0, new_value.value, new_value.type));
								animation_json->animations.push_back(tmp_anim_data);
							}
							new_animation_window = false;
						}
						
						ImGui::TreePop();

					}
				}
			}


			if (animation_json != nullptr)
			{
				if (animation_json->animations.empty())
				{
					ImGui::End();
					return;
				}
				for (int i = 0; i < animation_json->animations.size();i++)
				{
					AnimData* item = &animation_json->animations[i];
					Component* component = dynamic_cast<Component*>(item->data);

					bool open = ImGui::TreeNodeEx(component->GetName());
					if (open)
					{
						for (int j = 0; j < item->key_frame_data.size(); j++)
						{
							KeyFrameData* key_frame_item = &item->key_frame_data[j];

							if (key_frame_item->key_data.empty())
								continue;

							int size = key_frame_item->key_data.size();


							for (int k = 0; k < size; k++)
							{
								key_frame_item->ShowKeyValue(k);
							}

							if(ImGui::Button("Apply Changes##apply_changes"))
							{
								std::sort(key_frame_item->key_data.begin(), key_frame_item->key_data.end());
							}
							ImGui::SameLine();
							if (ImGui::Button("Create New Key##new_key"))
							{
								KeyData copy_data = key_frame_item->key_data.back();
								key_frame_item->key_data.push_back(copy_data);
							}
							ImGui::SameLine();
							if (ImGui::ImageButton((ImTextureID*)App->gui->icon_remove, ImVec2(8, 8), ImVec2(-1, 1), ImVec2(0, 0)))
							{
								item->key_frame_data.erase(item->key_frame_data.begin() + j);

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
	
	for (int i = 0; i <key_frame_data.size(); i++)
	{
		ParameterValue item = key_frame_data[i].parameter;
		if (item == data)
		{
			return true;
		}

	}
	
	return false;
}

KeyFrameData::KeyFrameData(int key, AnimationValue value, ParameterValue param) 
{
	key_data.push_back(KeyData(key, value));
	//std::sort(key_frames.begin(), key_frames.end());
	std::sort(key_data.begin(), key_data.end());

	parameter = param;
}

KeyFrameData::KeyFrameData(int key, AnimationValue value) 
{
	key_data.push_back(KeyData(key, value));
	std::sort(key_data.begin(), key_data.end());
}

KeyFrameData::~KeyFrameData()
{
}

void KeyFrameData::ShowKeyValue(int i)
{
	ImGui::PushItemWidth(150.0f);
	if(invalid_key)
		ImGui::TextColored(ImVec4(1,0,0,1),"Key frame duplicated");
	ImGui::Text("Key");
	std::string key_frame_name = "##key_frame";
	key_frame_name += std::to_string(i);
	if (ImGui::DragInt(key_frame_name.c_str(), &key_data[i].key_frame,1.0f,0, max_keys))
	{
		if (key_data[i].key_frame < 0)
		{
			key_data[i].key_frame = 0;
		}
		if (key_data[i].key_frame >= max_keys)
		{
			key_data[i].key_frame = max_keys;
		}

		for (int j = 0; j < key_data.size(); j++)
		{
			if (i == j)
				continue;
			if (key_data[j].key_frame == key_data[i].key_frame)
			{
				invalid_key = true;
				break;
			}
			else
			{
				invalid_key = false;
			}

		}
	}
	std::string paramter_name;
	switch (parameter)
	{
	case ParameterValue::RECT_TRANSFORM_POSITION:

		ImGui::Text("Position");
		paramter_name = "##transform_pos";
		paramter_name += std::to_string(i);

		ImGui::DragFloat3(paramter_name.c_str(), key_data[i].key_values.f3_value.ptr());
		break;
	case ParameterValue::RECT_TRANSFORM_ROTATION:
		ImGui::Text("Rotation");
		paramter_name = "##transform_rot";
		paramter_name += std::to_string(i);

		ImGui::DragFloat3(paramter_name.c_str(), key_data[i].key_values.f3_value.ptr());	
		break;
	case ParameterValue::RECT_TRANSFORM_SCALE:
		ImGui::Text("Scale");
		paramter_name = "##transform_sca";
		paramter_name += std::to_string(i);

		ImGui::DragFloat3(paramter_name.c_str(), key_data[i].key_values.f3_value.ptr());	
		break;
	case ParameterValue::RECT_TRANSFORM_WIDTH:
		ImGui::Text("Width");
		paramter_name = "##transform_w";
		paramter_name += std::to_string(i);
		ImGui::DragFloat(paramter_name.c_str(), &key_data[i].key_values.f_value);

		break;
	case ParameterValue::RECT_TRANSFORM_HEIGHT:
		ImGui::Text("Height");
		paramter_name = "##transform_h";
		paramter_name += std::to_string(i);
		ImGui::DragFloat(paramter_name.c_str(), &key_data[i].key_values.f_value);
		break;
	default:
		break;
	}
	ImGui::PopItemWidth();

}

AnimationJson::AnimationJson()
{
}

KeyData::KeyData(int key_frame, AnimationValue set_key_value):key_frame(key_frame) 
{
	key_values = set_key_value;
}

KeyData::~KeyData()
{
}

bool KeyData::operator<(const KeyData & compare) const
{
	
		return key_frame < compare.key_frame;
	
}
