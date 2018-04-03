#include "CompUIAnimation.h"
#include "AnimableComponent.h"
#include "WindowInspector.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "Scene.h"
#include "ModuleFS.h"

CompUIAnimation::CompUIAnimation(Comp_Type t, GameObject * parent):Component(t,parent)
{
	uid = App->random->Int();
	name_component = "Comp UI Animation";
}

CompUIAnimation::~CompUIAnimation()
{
}

void CompUIAnimation::Init()
{
	play = on_execution;
}

void CompUIAnimation::Clear()
{
	RELEASE(animation_json);
}

void CompUIAnimation::Update(float dt)
{
	if (play)
	{
		if (PlayAnimation(dt))
		{
			if (loop)
			{
				LOG("ResetAnimation");
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
	int op = ImGui::GetWindowWidth() / 4;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 0));
	ImGui::SameLine(ImGui::GetWindowWidth() - 26);
	if (ImGui::ImageButton((ImTextureID*)App->scene->icon_options_transform, ImVec2(13, 13), ImVec2(-1, 1), ImVec2(0, 0)))
	{
		ImGui::OpenPopup("OpenButton");
	}
	ImGui::PopStyleVar();

	if (ImGui::BeginPopup("OptionButton"))
	{
		ShowOptions();
		ImGui::EndPopup();
	}

	if (ImGui::Button("Select AnimationJson"))
	{
		select_ui_animation = true;
	}
	ImGui::Text("Animation: ");
	if (animation_json != nullptr)
	{
		ImGui::SameLine();
		ImGui::Text("%s", animation_json->name.c_str());
	}
	if (select_ui_animation)
	{

	}
	ImGui::Text("Play"); ImGui::SameLine(op + 30);
	ImGui::Checkbox("##play", &play);

	ImGui::Text("On Execution"); ImGui::SameLine(op + 30);
	ImGui::Checkbox("##execution", &on_execution);

	ImGui::Text("Loop"); ImGui::SameLine(op + 30);
	ImGui::Checkbox("##loop", &loop);



	ImGui::TreePop();
}
void CompUIAnimation::CopyValues(const CompUIAnimation * component)
{
}
void CompUIAnimation::Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const
{
	json_object_dotset_string_with_std(object, name + "Component:", name_component);
	json_object_dotset_number_with_std(object, name + "Type", this->GetType());
	json_object_dotset_number_with_std(object, name + "UUID", uid);
	//...

	if (animation_json != nullptr)
	{
		json_object_dotset_string_with_std(object, name + "Animation UI name", animation_json->name.c_str());

	}
	else
	{
		json_object_dotset_string_with_std(object, name + "Animation UI name", "");

	}
}
void CompUIAnimation::Load(const JSON_Object * object, std::string name)
{
	uid = json_object_dotget_number_with_std(object, name + "UUID");
	std::string animation_json_file= json_object_dotget_string_with_std(object, name + "Animation UI name");
	if (!animation_json_file.empty())
	{
		if (animation_json == nullptr)
		{
			animation_json = new AnimationJson();

			LoadAnimation(&animation_json, App->fs->GetMainDirectory().c_str(), animation_json_file.c_str());
		}
	}
	//...
	Enable();
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
		current_frame = current_time*animation_json->sample_rate;

		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = animation_json->animations[i];

			for (int j = 0; j < item->key_frame_data.size(); j++)
			{
				
				KeyFrameData* key_frame_item = &item->key_frame_data[j];

				if (current_frame >= key_frame_item->max_keys)
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
	if (current_frame >= animation_json->max_keys)
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
		current_frame = current_time*animation_json->sample_rate;


		for (int i = 0; i < animation_json->animations.size(); i++)
		{
			AnimData* item = animation_json->animations[i];

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

void CompUIAnimation::LoadAnimation(AnimationJson ** animation, const char * path, const char* name)
{
	
	
		JSON_Value* config_file;
		JSON_Object* config;
		std::string nameJson = path;
		nameJson += "/";
		nameJson += name;
		config_file = json_parse_file(nameJson.c_str());
		if (config_file != nullptr)
		{
			config = json_value_get_object(config_file);

			(*animation)->name = path;//<------not
			int animation_size = json_object_dotget_number_with_std(config, "UIAnimation.Size ");
			for (uint i = 0; i < animation_size; i++)
			{
				std::string animations = std::to_string(i);
				AnimData* anim_data = new AnimData();
				AnimableComponent* item = dynamic_cast<AnimableComponent*>(parent->FindComponentByType(static_cast<Comp_Type>((int)json_object_dotget_number_with_std(config, "UIAnimation.Type " + animations))));
				if (item == nullptr)
				{
					continue;
				}
				anim_data->data = item;
				int key_frame_data = json_object_dotget_number_with_std(config, "UIAnimation.Keyframe Size " + animations);
				for (int j = 0; j < key_frame_data; j++)
				{

					std::string key_frame = std::to_string(j);
					KeyFrameData keyframe_data;
					keyframe_data.max_keys = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.Max_Key " + key_frame);
					keyframe_data.sample_rate = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.Samples " + key_frame);
					keyframe_data.invalid_key = json_object_dotget_boolean_with_std(config, "UIAnimation " + animations + ".Animations.Invalid Key " + key_frame);
					keyframe_data.parameter = static_cast<ParameterValue>((int)json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.Parameter " + key_frame));
					keyframe_data.initial = 0;
					keyframe_data.destination = 1;


					int key_data_size = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData Size " + key_frame);

					for (int k = 0; k < key_data_size; k++)
					{
						std::string key_data = std::to_string(k);
						KeyData key_data_item;
						key_data_item.key_on_time = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data);

						key_data_item.key_frame = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Keyframe " + key_data);


						switch (keyframe_data.parameter)
						{
						case ParameterValue::RECT_TRANSFORM_POSITION:

							key_data_item.key_values.f3_value = App->fs->json_array_dotget_float3_string(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data);
							break;
						case ParameterValue::RECT_TRANSFORM_ROTATION:
							key_data_item.key_values.f3_value = App->fs->json_array_dotget_float3_string(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data);
							break;
						case ParameterValue::RECT_TRANSFORM_SCALE:
							key_data_item.key_values.f3_value = App->fs->json_array_dotget_float3_string(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Value " + key_data);
							break;
						case ParameterValue::RECT_TRANSFORM_WIDTH:

							key_data_item.key_values.f_value = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data);
							break;
						case ParameterValue::RECT_TRANSFORM_HEIGHT:
							key_data_item.key_values.f_value = json_object_dotget_number_with_std(config, "UIAnimation " + animations + ".Animations.KeyData " + key_frame + "Key on time " + key_data);
							break;
						default:
							break;
						}
						keyframe_data.key_data.push_back(key_data_item);
					}
					keyframe_data.my_anim_data = anim_data;
					anim_data->key_frame_data.push_back(keyframe_data);
				}
				(*animation)->animations.push_back(anim_data);
			}
		}

	

}
