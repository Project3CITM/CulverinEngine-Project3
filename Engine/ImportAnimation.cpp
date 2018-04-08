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

bool ImportAnimation::Import(const aiAnimation * animation, const char * name, const char * file)
{
	LOG("IMPORTING Animation %s -----", file);

	uint size = 0;

	std::string new_directory = DIRECTORY_LIBRARY_ANIMATIONS;
	new_directory += name;

	ResourceAnimation* tmp_anim = (ResourceAnimation*)App->resource_manager->CreateNewResource(Resource::Type::ANIMATION, 0);

	tmp_anim->InitInfo(name, App->fs->GetToAsstes(file).c_str(), name);
	tmp_anim->duration = animation->mDuration;
	tmp_anim->ticks_per_sec = animation->mTicksPerSecond;

	std::string assets_dir = ((Project*)App->gui->win_manager[WindowName::PROJECT])->GetDirectory();
	assets_dir += "\\";
	assets_dir += name;
	App->json_seria->SaveAnimation(tmp_anim, assets_dir.c_str(), file, new_directory.c_str());

	size += sizeof(char) * 50; //mName
	size += sizeof(double); //mDuration
	size += sizeof(double); //mTicksPerSecond
	size += sizeof(uint); //mNumChannels

	for (int i = 0; i < animation->mNumChannels; i++)
	{
		size += sizeof(char) * 50; //mNodeName
		size += sizeof(uint); //mNumPositionKeys

		size += sizeof(double) * animation->mChannels[i]->mNumPositionKeys; //mTime for each PosKey
		size += sizeof(float3) * animation->mChannels[i]->mNumPositionKeys; //posValue

		size += sizeof(uint); //mNumRotKeys
		size += sizeof(double) * animation->mChannels[i]->mNumRotationKeys; //mTime for each RotKey
		size += sizeof(float4) * animation->mChannels[i]->mNumRotationKeys; //rotValue

		size += sizeof(uint); //mNumScaKeys
		size += sizeof(double) * animation->mChannels[i]->mNumScalingKeys; //mTime for each ScaleKey
		size += sizeof(float3) * animation->mChannels[i]->mNumScalingKeys; //ScaleValue
	}

	char* data = new char[size];
	char* cursor = data;

	//Storing name
	uint bytes = sizeof(char) * 50;
	memcpy(cursor, animation->mName.C_Str(), bytes);
	cursor += bytes;

	//Storing duration
	bytes = sizeof(double);
	memcpy(cursor, &animation->mDuration, bytes);
	cursor += bytes;

	//Storing ticks per second
	bytes = sizeof(double);
	memcpy(cursor, &animation->mTicksPerSecond, bytes);
	cursor += bytes;

	//Storing NumChannels
	bytes = sizeof(uint);
	memcpy(cursor, &animation->mNumChannels, bytes);
	cursor += bytes;

	for (int i = 0; i < animation->mNumChannels; i++)
	{
		//Storing NodeName
		bytes = sizeof(char) * 50;
		memcpy(cursor, animation->mChannels[i]->mNodeName.C_Str(), bytes);
		cursor += bytes;

		//Storing NumPosKeys
		bytes = sizeof(uint);
		memcpy(cursor, &animation->mChannels[i]->mNumPositionKeys, bytes);
		cursor += bytes;
		for (int x = 0; x < animation->mChannels[i]->mNumPositionKeys; x++)
		{
			//Storing mTime
			bytes = sizeof(double);
			memcpy(cursor, &animation->mChannels[i]->mPositionKeys[x].mTime, bytes);
			cursor += bytes;

			//Storing PosValue
			bytes = sizeof(float3);
			memcpy(cursor, &animation->mChannels[i]->mPositionKeys[x].mValue, bytes);
			cursor += bytes;
		}

		//Storing NumRotKeys
		bytes = sizeof(uint);
		memcpy(cursor, &animation->mChannels[i]->mNumRotationKeys, bytes);
		cursor += bytes;

		for (int p = 0; p < animation->mChannels[i]->mNumRotationKeys; p++)
		{
			//Storing mTime
			bytes = sizeof(double);
			memcpy(cursor, &animation->mChannels[i]->mRotationKeys[p].mTime, bytes);
			cursor += bytes;

			//Storing RotValue
			bytes = sizeof(float4);
			float4 real_rot = { animation->mChannels[i]->mRotationKeys[p].mValue.x, animation->mChannels[i]->mRotationKeys[p].mValue.y, animation->mChannels[i]->mRotationKeys[p].mValue.z, animation->mChannels[i]->mRotationKeys[p].mValue.w };
			memcpy(cursor, &real_rot, bytes);
			cursor += bytes;
		}

		//Storing NumScaleKeys
		bytes = sizeof(uint);
		memcpy(cursor, &animation->mChannels[i]->mNumScalingKeys, bytes);
		cursor += bytes;

		for (int r = 0; r < animation->mChannels[i]->mNumScalingKeys; r++)
		{
			//Storing mTime
			bytes = sizeof(double);
			memcpy(cursor, &animation->mChannels[i]->mScalingKeys[r].mTime, bytes);
			cursor += bytes;

			//Storing RotValue
			bytes = sizeof(float3);
			memcpy(cursor, &animation->mChannels[i]->mScalingKeys[r].mValue, bytes);
			cursor += bytes;
		}
	}

	std::string tmp_name = name;
	App->fs->SaveFile(data, tmp_name, size, IMPORT_DIRECTORY_LIBRARY_ANIMATIONS);

	RELEASE_ARRAY(data);

	return true;
}

bool ImportAnimation::LoadResource(const char * file, ResourceAnimation * resource_animation)
{
	LOG("LOADING ANIMATION -----");

	resource_animation->bones.clear();

	char* buffer = nullptr;
	char* name = new char[50];
	double ticks_per_sec;
	double duration;
	uint num_bones;

	uint size = App->fs->LoadFile(file, &buffer, IMPORT_DIRECTORY_LIBRARY_ANIMATIONS);

	if (buffer != nullptr && size > 0)
	{
		char* cursor = buffer;

		//Name
		uint bytes = sizeof(char) * 50;
		memcpy(name, cursor, bytes);
		cursor += bytes;
		resource_animation->name = name;

		RELEASE_ARRAY(name);

		//Duration
		bytes = sizeof(double);
		memcpy(&duration, cursor, bytes);
		resource_animation->duration = duration;
		cursor += bytes;

		//Ticks per second
		bytes = sizeof(double);
		memcpy(&ticks_per_sec, cursor, bytes);
		resource_animation->ticks_per_sec = ticks_per_sec;
		cursor += bytes;

		//NumChannels
		bytes = sizeof(uint);
		memcpy(&num_bones, cursor, bytes);
		cursor += bytes;

		for (int i = 0; i < num_bones; i++)
		{
			AnimBone* temp_bone = new AnimBone();
			//NodeName
			bytes = sizeof(char) * 50;
			char* tmp_name = new char[50];
			memcpy(tmp_name, cursor, bytes);
			temp_bone->name = tmp_name;
			cursor += bytes;

			//NumPosKeys
			uint num_pos_keys;
			bytes = sizeof(uint);
			memcpy(&num_pos_keys, cursor, bytes);
			cursor += bytes;
			for (int j = 0; j < num_pos_keys; j++)
			{
				PositionKey* temp_pos_key = new PositionKey;

				//Time
				double temp_double;
				bytes = sizeof(double);
				memcpy(&temp_double, cursor, bytes);
				temp_pos_key->time = (float)temp_double;
				cursor += bytes;

				//PosValue
				bytes = sizeof(float3);
				memcpy(&temp_pos_key->position, cursor, bytes);
				cursor += bytes;
				temp_bone->position_keys.push_back(temp_pos_key);
			}

			//NumRotKeys
			uint num_rot_keys;

			bytes = sizeof(uint);
			memcpy(&num_rot_keys, cursor, bytes);
			cursor += bytes;
			for (int p = 0; p < num_rot_keys; p++)
			{
				RotationKey* temp_rot_key = new RotationKey;

				//Time
				double temp_double;
				bytes = sizeof(double);
				memcpy(&temp_double, cursor, bytes);
				temp_rot_key->time = (float)temp_double;
				cursor += bytes;

				//RotValue
				float4 temp_float4;
				bytes = sizeof(float4);
				memcpy(&temp_float4, cursor, bytes);
				cursor += bytes;
				temp_rot_key->rotation = { temp_float4.x, temp_float4.y, temp_float4.z, temp_float4.w };

				temp_bone->rotation_keys.push_back(temp_rot_key);
			}

			//NumScaKeys
			uint num_sca_keys;

			bytes = sizeof(uint);
			memcpy(&num_sca_keys, cursor, bytes);
			cursor += bytes;
			for (int p = 0; p < num_sca_keys; p++)
			{
				ScaleKey* temp_sca_key = new ScaleKey;

				//Time
				double temp_double;
				bytes = sizeof(double);
				memcpy(&temp_double, cursor, bytes);
				temp_sca_key->time = (float)temp_double;
				cursor += bytes;

				//ScaValue
				bytes = sizeof(float3);
				memcpy(&temp_sca_key->scale, cursor, bytes);
				cursor += bytes;
				temp_bone->scale_keys.push_back(temp_sca_key);
			}
			resource_animation->bones.push_back(temp_bone);
		}
	}

	RELEASE_ARRAY(buffer);

	return true;
}
