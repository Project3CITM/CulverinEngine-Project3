#include "ResourceAnimation.h"
#include "CompAnimation.h"
#include "Application.h"

ResourceAnimation::ResourceAnimation(uint uid):Resource(uid, Resource::Type::ANIMATION, Resource::State::UNLOADED)
{
	num_game_objects_use_me = 0;
	LOG("Resource Animation Created!");
}

ResourceAnimation::~ResourceAnimation()
{
	for (std::vector<AnimBone*>::iterator temp = bones.begin(); temp != bones.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void ResourceAnimation::InitInfo(const char * resource_name, const char * path)
{
	name = App->GetCharfromConstChar(resource_name);
	path_assets = path;
}

bool ResourceAnimation::LoadToMemory()
{
	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	return true;
}

AnimBone::~AnimBone()
{
	for (std::vector<PositionKey*>::iterator temp = position_keys.begin(); temp != position_keys.end(); temp++)
	{
		RELEASE((*temp));
	}
	for (std::vector<RotationKey*>::iterator temp = rotation_keys.begin(); temp != rotation_keys.end(); temp++)
	{
		RELEASE((*temp));
	}
	for (std::vector<ScaleKey*>::iterator temp = scale_keys.begin(); temp != scale_keys.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void AnimBone::UpdateBone(GameObject* bone, std::vector<AnimationClip*>& clip_vec) const
{
	float3 pos;
	Quat rot;
	float3 scale;

	for (std::vector<AnimationClip*>::const_iterator it = clip_vec.begin(); it != clip_vec.end(); ++it)
	{
		pos = GetPosition(*it);
		rot = GetRotation(*it);
		scale = GetScale(*it);
	}

	//TODO Blending

	/*CompTransform* trans = (CompTransform*)bone->FindComponent(Component_Transform);
	trans->SetPosition(pos);
	trans->SetRotation(rot);
	trans->SetScale(scale);*/
}

float3 AnimBone::GetPosition(AnimationClip* clip_vec) const
{
	//for(std::vector<PositionKey*>::iterator )
	if (position_keys.size() > 1)
	{
		float3 actual_pos;
		float3 next_pos;
		float actual_time;
		float next_time;

		for (std::vector<PositionKey*>::const_iterator it = position_keys.begin(); it != position_keys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == position_keys.end() - 1)
			{
				return (*position_keys.end() - 1)->position;
			}
			else
			{
				actual_pos = (*it)->position;
				actual_time = (*it)->time;
				next_pos = (*(it + 1))->position;
				next_time = (*(it + 1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Lerp(next_pos, weight);

	}
	return position_keys[0]->position;
}

Quat AnimBone::GetRotation(AnimationClip* clip_vec) const
{
	if (rotation_keys.size() > 1)
	{
		Quat actual_pos;
		Quat next_pos;
		float actual_time;
		float next_time;

		for (std::vector<RotationKey*>::const_iterator it = rotation_keys.begin(); it != rotation_keys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == rotation_keys.end() - 1)
			{
				return (*rotation_keys.end() - 1)->rotation;
			}
			else
			{
				actual_pos = (*it)->rotation;
				actual_time = (*it)->time;
				next_pos = (*(it + 1))->rotation;
				next_time = (*(it + 1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Slerp(next_pos, weight);
	}
	return rotation_keys[0]->rotation;
}

float3 AnimBone::GetScale(AnimationClip* clip_vec) const
{
	if (scale_keys.size() > 1)
	{
		float3 actual_pos;
		float3 next_pos;
		float actual_time;
		float next_time;

		for (std::vector<ScaleKey*>::const_iterator it = scale_keys.begin(); it != scale_keys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == scale_keys.end() - 1)
			{
				return (*scale_keys.end() - 1)->scale;
			}
			else
			{
				actual_pos = (*it)->scale;
				actual_time = (*it)->time;
				next_pos = (*(it + 1))->scale;
				next_time = (*(it + 1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Lerp(next_pos, weight);

	}
	return scale_keys[0]->scale;
}