#include "ImportAnimation.h"
#include "ResourceAnimation.h"
#include "ModuleGUI.h"
#include "ModuleFS.h"
#include "JSONSerialization.h"


ImportAnimation::ImportAnimation()
{
}

ImportAnimation::~ImportAnimation()
{
}

bool ImportAnimation::Import(const aiAnimation * animation, const char * name, const char * file, uint uuid, bool isAutoImport)
{
	LOG("IMPORTING Animation %s -----", file);

	JSON_Value* config_file;
	JSON_Object* config;

	config_file = json_value_init_object();

	std::string new_directory = DIRECTORY_LIBRARY_ANIMATIONS;
	new_directory += name;
	new_directory += ".json";

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		
		ResourceAnimation* tmp_anim = (ResourceAnimation*)App->resource_manager->CreateNewResource(Resource::Type::ANIMATION, 0);

		tmp_anim->InitInfo(name, new_directory.c_str());
		tmp_anim->duration = animation->mDuration;
		tmp_anim->ticks_per_sec = animation->mTicksPerSecond;

		std::string assets_dir = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
		assets_dir += "\\";
		assets_dir += name;
		App->json_seria->SaveAnimation(tmp_anim, assets_dir.c_str(), name);

		json_object_dotset_string_with_std(config, "Animation.Name", animation->mName.C_Str());
		json_object_dotset_number_with_std(config, "Animation.Duration", animation->mDuration);
		json_object_dotset_number_with_std(config, "Animation.TicksPerSecond", animation->mTicksPerSecond);
		json_object_dotset_number_with_std(config, "Animation.NumBones", animation->mNumChannels);

		for (int i = 0; i < animation->mNumChannels; i++)
		{
			std::string bone_name = "Animation.Bone" + std::to_string(i);
			bone_name += ".";
			json_object_dotset_string_with_std(config, bone_name + "Name", animation->mChannels[i]->mNodeName.C_Str());
			json_object_dotset_number_with_std(config, bone_name + "NumPosKeys", animation->mChannels[i]->mNumPositionKeys);
			for (int x = 0; x < animation->mChannels[i]->mNumPositionKeys; x++)
			{
				std::string position_key_name = "Position_Key" + std::to_string(x);
				position_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + position_key_name + "Time", animation->mChannels[i]->mPositionKeys[x].mTime);

				float3 pos = {animation->mChannels[i]->mPositionKeys[x].mValue.x,animation->mChannels[i]->mPositionKeys[x].mValue.y,animation->mChannels[i]->mPositionKeys[x].mValue.z};
				App->fs->json_array_dotset_float3(config, bone_name + position_key_name + "Position", pos);
			}
			json_object_dotset_number_with_std(config, bone_name + "NumRotKeys", animation->mChannels[i]->mNumRotationKeys);
			for (int p = 0; p < animation->mChannels[i]->mNumRotationKeys; p++)
			{
				std::string rotation_key_name = "Rotation_Key" + std::to_string(p);
				rotation_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + rotation_key_name + "Time", animation->mChannels[i]->mRotationKeys[p].mTime);

				float4 rot = { animation->mChannels[i]->mRotationKeys[p].mValue.w ,animation->mChannels[i]->mRotationKeys[p].mValue.x ,animation->mChannels[i]->mRotationKeys[p].mValue.y ,animation->mChannels[i]->mRotationKeys[p].mValue.z};
				App->fs->json_array_dotset_float4(config, bone_name + rotation_key_name + "Rotation",rot );
			}
			json_object_dotset_number_with_std(config, bone_name + "NumScaleKeys", animation->mChannels[i]->mNumScalingKeys);
			for (int r = 0; r < animation->mChannels[i]->mNumScalingKeys; r++)
			{
				std::string scale_key_name = "Scale_Key" + std::to_string(r);
				scale_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + scale_key_name + "Time", animation->mChannels[i]->mScalingKeys[r].mTime);

				float3 pos = { animation->mChannels[i]->mScalingKeys[r].mValue.x,animation->mChannels[i]->mScalingKeys[r].mValue.y,animation->mChannels[i]->mScalingKeys[r].mValue.z };
				App->fs->json_array_dotset_float3(config, bone_name + scale_key_name + "Scale", pos);
			}
		}
		json_serialize_to_file(config_file, new_directory.c_str());
	}
	json_value_free(config_file);

	return true;
}

bool ImportAnimation::LoadResource(const char * file, ResourceAnimation * resource_animation)
{
	LOG("LOADING ANIMATION -----");

	JSON_Value* config_file;
	JSON_Object* config;
	JSON_Object* config_node;

	config_file = json_parse_file(file);
	if (file != nullptr)
	{
		config = json_value_get_object(config_file);
		config_node = json_object_get_object(config, "Animation");
		resource_animation->name = App->GetCharfromConstChar(json_object_dotget_string(config_node, "Name"));
		resource_animation->ticks_per_sec = json_object_dotget_number(config_node, "TicksPerSecond");
		resource_animation->duration = json_object_dotget_number(config_node, "Duration");
		int num_bones = json_object_dotget_number(config_node, "NumBones");
		for (int i = 0; i < num_bones; i++)
		{
			AnimBone* temp_bone = new AnimBone();
			std::string name = "Bone" + std::to_string(i);
			JSON_Object* config_bone;
			config_bone = json_object_get_object(config_node, name.c_str());

			temp_bone->name = json_object_dotget_string(config_bone, "Name");
			int num_pos_keys = json_object_dotget_number(config_bone, "NumPosKeys");
			for (int p = 0; p < num_pos_keys; p++)
			{
				PositionKey* temp_pos_key = new PositionKey();
				JSON_Object* config_pos_key;

				std::string name_pos = "Position_Key" + std::to_string(p);
				config_pos_key = json_object_get_object(config_bone, name_pos.c_str());

				temp_pos_key->time = json_object_dotget_number(config_pos_key, "Time");
				temp_pos_key->position = App->fs->json_array_dotget_float3_string(config_pos_key, "Position");

				temp_bone->position_keys.push_back(temp_pos_key);
			}
			int num_rot_keys = json_object_dotget_number(config_bone, "NumRotKeys");
			for (int j = 0; j < num_rot_keys; j++)
			{
				RotationKey* temp_rot_key = new RotationKey();
				JSON_Object* config_rot_key;

				std::string name_rot = "Rotation_Key" + std::to_string(j);
				config_rot_key = json_object_get_object(config_bone, name_rot.c_str());

				temp_rot_key->time = json_object_dotget_number(config_rot_key, "Time");
				float4 rot = App->fs->json_array_dotget_float4_string(config_rot_key, "Rotation");
				temp_rot_key->rotation = { Quat(rot.w, rot.x, rot.y, rot.z) };
				temp_bone->rotation_keys.push_back(temp_rot_key);
			}
			int num_scale_keys = json_object_dotget_number(config_bone, "NumScaleKeys");
			for (int z = 0; z < num_scale_keys; z++)
			{
				ScaleKey* temp_scale_key = new ScaleKey();
				JSON_Object* config_scale_key;

				std::string name_scale = "Scale_Key" + std::to_string(z);
				config_scale_key = json_object_get_object(config_bone, name_scale.c_str());

				temp_scale_key->time = json_object_dotget_number(config_scale_key, "Time");
				temp_scale_key->scale = App->fs->json_array_dotget_float3_string(config_scale_key, "Scale");

				temp_bone->scale_keys.push_back(temp_scale_key);
			}

			resource_animation->bones.push_back(temp_bone);
		}
	}
	return true;
}
