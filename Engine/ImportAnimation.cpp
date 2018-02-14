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

	std::string nameJson = file;
	nameJson += ".json";
	config_file = json_value_init_object();

	if (config_file != nullptr)
	{
		config = json_value_get_object(config_file);
		json_object_clear(config);
		
		ResourceAnimation* tmpanim = new ResourceAnimation(0);

		json_object_dotset_string_with_std(config, "Animation.Name", animation->mName.C_Str());
		json_object_dotset_number_with_std(config, "Animation.Duration", animation->mDuration);
		json_object_dotset_number_with_std(config, "Animation.TicksPerSecond", animation->mTicksPerSecond);

		for (int i = 0; i < animation->mNumChannels; i++)
		{
			std::string bone_name = "Bone" + std::to_string(i);
			bone_name += ".";
			for (int x = 0; x < animation->mChannels[i]->mNumPositionKeys; x++)
			{
				std::string position_key_name = "Position_Key" + std::to_string(x);
				position_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + position_key_name + "Time", animation->mChannels[i]->mPositionKeys[x].mTime);
				//json_array_dotset_float3(config, bone_name + position_key_name + "Position", animation->mChannels[i]->mPositionKeys[x].mValue);
			}
			for (int p = 0; p < animation->mChannels[i]->mNumRotationKeys; p++)
			{
				std::string rotation_key_name = "Rotation_Key" + std::to_string(p);
				rotation_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + rotation_key_name + "Time", animation->mChannels[i]->mRotationKeys[p].mTime);
				//json_array_dotset_float3(config, bone_name + position_key_name + "Position", animation->mChannels[i]->mPositionKeys[x].mValue);
			}
			for (int r = 0; r < animation->mChannels[i]->mNumScalingKeys; r++)
			{
				std::string scale_key_name = "Scale_Key" + std::to_string(r);
				scale_key_name += ".";
				json_object_dotset_number_with_std(config, bone_name + scale_key_name + "Time", animation->mChannels[i]->mScalingKeys[r].mTime);
				//json_array_dotset_float3(config, bone_name + position_key_name + "Position", animation->mChannels[i]->mPositionKeys[x].mValue);
			}
		}

	}

	json_serialize_to_file(config_file, nameJson.c_str());

	json_value_free(config_file);
	/*

	save.AddArray("Bones");
	for (int y = 0; y < animation->mNumChannels; y++)
	{
	Configuration AnimBone;
	Bone* tempbone = new Bone();
	tempbone->name = animation->mChannels[y]->mNodeName.C_Str();
	AnimBone.SetString("Name", animation->mChannels[y]->mNodeName.C_Str());

	AnimBone.AddArray("PositionKeys");

	for (int r = 0; r < animation->mChannels[y]->mNumPositionKeys; r++)
	{
	PositionKey* tempposkey = new PositionKey();
	tempposkey->time = animation->mChannels[y]->mPositionKeys[r].mTime;
	tempposkey->position.x = animation->mChannels[y]->mPositionKeys[r].mValue.x;
	tempposkey->position.y = animation->mChannels[y]->mPositionKeys[r].mValue.y;
	tempposkey->position.z = animation->mChannsels[y]->mPositionKeys[r].mValue.z;
	tempbone->positionkeys.push_back(tempposkey);

	Configuration positionKey;
	positionKey.SetFloat("Time", animation->mChannels[y]->mPositionKeys[r].mTime);
	positionKey.AddArrayFloat("Position", &animation->mChannels[y]->mPositionKeys[r].mValue.x, 3);
	AnimBone.AddArrayEntry(positionKey);
	}
	AnimBone.AddArray("RotationKeys");
	for (int x = 0; x < animation->mChannels[y]->mNumRotationKeys; x++)
	{
	RotationKey* temprotkey = new RotationKey();
	temprotkey->time = animation->mChannels[y]->mRotationKeys[x].mTime;
	temprotkey->rotation.w = animation->mChannels[y]->mRotationKeys[x].mValue.w;
	temprotkey->rotation.x = animation->mChannels[y]->mRotationKeys[x].mValue.x;
	temprotkey->rotation.y = animation->mChannels[y]->mRotationKeys[x].mValue.y;
	temprotkey->rotation.z = animation->mChannels[y]->mRotationKeys[x].mValue.z;
	tempbone->rotationkeys.push_back(temprotkey);

	Configuration rotationKey;
	rotationKey.SetFloat("Time", animation->mChannels[y]->mRotationKeys[x].mTime);
	rotationKey.AddArrayFloat("Rotation", &animation->mChannels[y]->mRotationKeys[x].mValue.w, 4);
	AnimBone.AddArrayEntry(rotationKey);
	}
	AnimBone.AddArray("ScaleKeys");
	for (int x = 0; x < animation->mChannels[y]->mNumScalingKeys; x++)
	{
	ScaleKey* tempscakey = new ScaleKey();
	tempscakey->time = animation->mChannels[y]->mScalingKeys[x].mTime;
	tempscakey->scale.x = animation->mChannels[y]->mScalingKeys[x].mValue.x;
	tempscakey->scale.y = animation->mChannels[y]->mScalingKeys[x].mValue.y;
	tempscakey->scale.z = animation->mChannels[y]->mScalingKeys[x].mValue.z;
	tempbone->scalekeys.push_back(tempscakey);

	Configuration scalekey;
	scalekey.SetFloat("Time", animation->mChannels[y]->mScalingKeys[x].mTime);
	scalekey.AddArrayFloat("Scale", &animation->mChannels[y]->mScalingKeys[x].mValue.x, 3);
	AnimBone.AddArrayEntry(scalekey);
	}
	save.AddArrayEntry(AnimBone);
	tmpanim->bones.push_back(tempbone);
	}
	char* buffer = nullptr;
	uint fileSize = save.SaveFile(&buffer, "Animation");
	App->filesystem->SaveFile(animation->mName.C_Str(), buffer, fileSize, FileType::fileAnimation);
	RELEASE(tmpanim);
	RELEASE_ARRAY(buffer);
	*/
	return true;
}

bool ImportAnimation::LoadResource(const char * file, ResourceAnimation * resourceMaterial)
{
	return false;
}
