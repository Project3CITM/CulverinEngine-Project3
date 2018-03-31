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
				ShowNewAnimationWindow();
			}


			if (animation_json != nullptr)
			{
				ShowAnimationJsonInfo();
			}

			

		}
		ImGui::End();
	

}

void ModuleAnimation::ShowNewAnimationWindow()
{
	GameObject* go = ((Hierarchy*)App->gui->win_manager[WindowName::HIERARCHY])->GetSelected();

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

void ModuleAnimation::ShowAnimationJsonInfo()
{
	ShowAnimationKeyFrameInfo();
	ImGui::Separator();
	ShowAnimationDataInfo();
}

void ModuleAnimation::ShowAnimationDataInfo()
{

	if (animation_json->animations.empty())
	{
		return;
	}
	for (int i = 0; i < animation_json->animations.size(); i++)
	{
		AnimData* item = &animation_json->animations[i];
		Component* component = dynamic_cast<Component*>(item->data);

		bool out_open = ImGui::TreeNodeEx(component->GetName());
		if (out_open)
		{
			for (int j = 0; j < item->key_frame_data.size(); j++)
			{
				KeyFrameData* key_frame_item = &item->key_frame_data[j];

				if (key_frame_item->key_data.empty())
				{
					item->key_frame_data.erase(item->key_frame_data.begin() + j);
					continue;
				}
				int size = key_frame_item->key_data.size();

				bool in_open = ImGui::TreeNodeEx(item->data->ReturnParameterName(key_frame_item->parameter));
				if (in_open)
				{
					static int value = -1;
					Columns(3, "key");
					ImGui::Text("KeyFrames");

					value = key_frame_item->ReturnKeyDataPos(value);

					ImGui::NextColumn();
					ImGui::Text("Time");

					key_frame_item->ShowKeyOnTime();

					ImGui::NextColumn();
					ImGui::Text("KeySelected");
					if (!key_frame_item->ShowKeyValue(value))
					{
						key_frame_item->key_data.erase(key_frame_item->key_data.begin() + value);
					}
					Columns();
					ImGui::Separator();

					if (ImGui::Button("Apply Changes##apply_changes"))
					{
						std::sort(key_frame_item->key_data.begin(), key_frame_item->key_data.end());
					}
					ImGui::SameLine();

					if (ImGui::Button("Create New Key##new_key"))
					{
						KeyData copy_data = key_frame_item->key_data.back();
						key_frame_item->key_data.push_back(copy_data);
					}

					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

	}
}

void ModuleAnimation::ShowAnimationKeyFrameInfo()
{
	ImGui::Text("Max Keyframes");
	ImGui::SameLine();

	if(ImGui::DragInt("##max_keys", &max_keys, 1.0f, 1, 100))
	{

		if (animation_json->animations.empty())
		{
			return;
		}
		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = &animation_json->animations[i];
			for (int j = 0; j < item->key_frame_data.size(); j++)
			{
				KeyFrameData* key_frame_item = &item->key_frame_data[j];
				key_frame_item->max_keys = max_keys;
				key_frame_item->UpdateKeyFrame();
				key_frame_item->UpdateSampleRate();

			}
		}
	}
	ImGui::Text("Sample");
	ImGui::SameLine();
	if (ImGui::DragInt("##sample_rate", &sample_rate, 1.0f, 1, 100))
	{
		if (animation_json->animations.empty())
		{
			return;
		}
		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = &animation_json->animations[i];
			for (int j = 0; j < item->key_frame_data.size(); j++)
			{
				KeyFrameData* key_frame_item = &item->key_frame_data[j];
				key_frame_item->sample_rate = sample_rate;
				key_frame_item->UpdateSampleRate();

			}
		}
	}
}


void ModuleAnimation::FindAnimComponent()
{
}

void ModuleAnimation::SaveAnimation()
{
	if (animation_json == nullptr)
		return;

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

bool KeyFrameData::ShowKeyValue(int i)
{
	if (i == -1||i>key_data.size())
		return true;
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
		if (key_data[i].key_frame > max_keys)
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
		key_data[i].key_on_time = (float)key_data[i].key_frame / (float)sample_rate;
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
	std::string erase_name;
	erase_name = "Erase Keyframe##erase_key";
	erase_name += std::to_string(i);
	if (ImGui::Button(erase_name.c_str()))
	{
		return false;
	}
	ImGui::PopItemWidth();
	return true;

}

void KeyFrameData::ShowKeyOnTime()
{
	for (int i = 0; i < key_data.size(); i++)
	{
		ImGui::Text("%.2f", key_data[i].key_on_time);
	}
}

int KeyFrameData::ReturnKeyDataPos(int& selected)
{
	for (int i = 0; i < key_data.size(); i++)
	{
		std::string key_name = std::to_string(key_data[i].key_frame);
		key_name += "##";
		key_name += std::to_string(i);
		if (ImGui::Selectable(key_name.c_str(), selected==i))
		{
			selected = i;

			return selected;
		}
	}
	return selected;
}
void KeyFrameData::UpdateKeyFrame()
{
	for (int i = 0; i < key_data.size(); i++)
	{
		if (key_data[i].key_frame > max_keys)
		{
			key_data[i].key_frame = max_keys;
		}
	}
}

void KeyFrameData::UpdateSampleRate()
{
	for (int i = 0; i < key_data.size(); i++)
	{
			key_data[i].key_on_time = (float)key_data[i].key_frame/ (float)sample_rate;
	}
}

void KeyFrameData::UpdateInterpolationKeys()
{
	if (destination < key_data.size())
	{
		destination++;
		initial++;
	}
}

void KeyFrameData::ResetInterpolationKeys()
{
	if (key_data.size() >1)
	{
		destination = 1;
		initial=0;
	}
}

AnimationValue KeyFrameData::Interpolate(float current_time, int frame)
{
	AnimationValue ret;
	switch (parameter)
	{
	case ParameterValue::RECT_TRANSFORM_POSITION:
		if (key_data[destination].key_frame == frame)
			ret.f3_value = key_data[destination].key_values.f3_value;
		else
			ret.f3_value= key_data[initial].key_values.f3_value + current_time*(key_data[destination].key_values.f3_value - key_data[initial].key_values.f3_value);

		break;
	case ParameterValue::RECT_TRANSFORM_ROTATION:
		if (key_data[destination].key_frame == frame)
			ret.f3_value = key_data[destination].key_values.f3_value;
		else
			ret.f3_value = key_data[initial].key_values.f3_value + current_time*(key_data[destination].key_values.f3_value - key_data[initial].key_values.f3_value);

		break;
	case ParameterValue::RECT_TRANSFORM_SCALE:
		if (key_data[destination].key_frame == frame)
			ret.f3_value = key_data[destination].key_values.f3_value;
		else
			ret.f3_value = key_data[initial].key_values.f3_value + current_time*(key_data[destination].key_values.f3_value - key_data[initial].key_values.f3_value);

		break;
	case ParameterValue::RECT_TRANSFORM_WIDTH:
		if (key_data[destination].key_frame == frame)
			ret.f_value = key_data[destination].key_values.f_value;
		else
			ret.f_value = key_data[initial].key_values.f_value + current_time*(key_data[destination].key_values.f_value - key_data[initial].key_values.f_value);

	break;
	case ParameterValue::RECT_TRANSFORM_HEIGHT:
		if (key_data[destination].key_frame == frame)
			ret.f_value = key_data[destination].key_values.f_value;
		else
			ret.f_value = key_data[initial].key_values.f_value + current_time*(key_data[destination].key_values.f_value - key_data[initial].key_values.f_value);

		break;
	default:
		break;
	}
	if (key_data[destination].key_frame == frame)
		UpdateInterpolationKeys();
	return ret;
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
