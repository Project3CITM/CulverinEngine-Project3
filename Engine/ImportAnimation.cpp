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

		for (int i = 0; i < animation->mNumChannels; i++)
		{
			std::string bone_name = "Animation.Bone" + std::to_string(i);
			bone_name += ".";
			json_object_dotset_string_with_std(config, bone_name + "Name", animation->mChannels[i]->mNodeName.C_Str());
			for (int x = 0; x < animation->mChannels[i]->mNumPositionKeys; x++)
			{
				std::string position_key_name = "Position_Key" + std::to_string(x);
				position_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + position_key_name + "Time", animation->mChannels[i]->mPositionKeys[x].mTime);

				float3 pos = {animation->mChannels[i]->mPositionKeys[x].mValue.x,animation->mChannels[i]->mPositionKeys[x].mValue.y,animation->mChannels[i]->mPositionKeys[x].mValue.z};
				App->fs->json_array_dotset_float3(config, bone_name + position_key_name + "Position", pos);
			}
			for (int p = 0; p < animation->mChannels[i]->mNumRotationKeys; p++)
			{
				std::string rotation_key_name = "Rotation_Key" + std::to_string(p);
				rotation_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + rotation_key_name + "Time", animation->mChannels[i]->mRotationKeys[p].mTime);

				float4 rot = { animation->mChannels[i]->mRotationKeys[p].mValue.w ,animation->mChannels[i]->mRotationKeys[p].mValue.x ,animation->mChannels[i]->mRotationKeys[p].mValue.y ,animation->mChannels[i]->mRotationKeys[p].mValue.z};
				App->fs->json_array_dotset_float4(config, bone_name + rotation_key_name + "Rotation",rot );
			}
			for (int r = 0; r < animation->mChannels[i]->mNumScalingKeys; r++)
			{
				std::string scale_key_name = "Scale_Key" + std::to_string(r);
				scale_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + scale_key_name + "Time", animation->mChannels[i]->mScalingKeys[r].mTime);

				float3 pos = { animation->mChannels[i]->mScalingKeys[r].mValue.x,animation->mChannels[i]->mScalingKeys[r].mValue.y,animation->mChannels[i]->mScalingKeys[r].mValue.z };
				App->fs->json_array_dotset_float3(config, bone_name + scale_key_name + "Rotation", pos);
			}
		}
		json_serialize_to_file(config_file, new_directory.c_str());
	
	}

	json_value_free(config_file);

	return true;
}

bool ImportAnimation::LoadResource(const char * file, ResourceAnimation * resourceMaterial)
{
	return false;
}
