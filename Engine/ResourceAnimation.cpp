#include "ResourceAnimation.h"
#include "CompAnimation.h"
#include "Application.h"
#include "GameObject.h"
#include "CompTransform.h"

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

void AnimBone::UpdateBone(GameObject* bone, AnimationClip* playing_clip, AnimationClip* blending_clip) const
{
	if (playing_clip != nullptr)
	{
		float3 pos, blending_pos, last_pos;
		Quat rot, blending_rot, last_rot;
		float3 scale, blending_scale, last_scale;

		CompTransform* transform = bone->GetComponentTransform();

		pos = GetPosition(playing_clip);
		rot = GetRotation(playing_clip);
		scale = GetScale(playing_clip);

		if (blending_clip == nullptr)
		{
			transform->SetPos(pos);
			transform->SetRot(rot);
			transform->SetScale(scale);
		}
		else
		{
			blending_pos = GetPosition(blending_clip);
			blending_rot = GetRotation(blending_clip);
			blending_scale = GetScale(blending_clip);

			float weight = (blending_clip->total_blending_time - blending_clip->current_blending_time) / blending_clip->total_blending_time;

			last_pos = pos.Lerp(blending_pos, weight);
			last_rot = rot.Slerp(blending_rot, weight);
			last_scale = scale.Lerp(blending_scale, weight);

			transform->SetPos(last_pos);
			transform->SetRot(last_rot);
			transform->SetScale(last_scale);
		}
	}
}


float3 AnimBone::GetPosition(AnimationClip* clip_vec) const
{
	if (position_keys.size() > 1)
	{
		float3 actual_pos;
		float3 next_pos;
		float actual_time;
		float next_time;

		for (std::vector<PositionKey*>::const_iterator it = position_keys.begin(); it != position_keys.end(); ++it)
		{
			if ((*it)->time <= clip_vec->time)
			{
				if (it == position_keys.end() - 1)
				{
					return position_keys[position_keys.size() - 1]->position;
				}
				else
				{
					actual_pos = (*it)->position;
					actual_time = (*it)->time;
					next_pos = (*(it + 1))->position;
					next_time = (*(it + 1))->time;

				}
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
			if ((*it)->time <= clip_vec->time)
			{
				if (it == rotation_keys.end() - 1)
				{
					return rotation_keys[rotation_keys.size() - 1]->rotation;
				}
				else
				{
					actual_pos = (*it)->rotation;
					actual_time = (*it)->time;
					next_pos = (*(it + 1))->rotation;
					next_time = (*(it + 1))->time;

				}
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Slerp(next_pos, weight);
	}
	else
	{
		
		return rotation_keys[0]->rotation;
		
	}
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
			if ((*it)->time <= clip_vec->time)
			{
				if (it == scale_keys.end() - 1)
				{
					return scale_keys[scale_keys.size() - 1]->scale;
				}
				else
				{
					actual_pos = (*it)->scale;
					actual_time = (*it)->time;
					next_pos = (*(it + 1))->scale;
					next_time = (*(it + 1))->time;

				}
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Lerp(next_pos, weight);

	}
	return scale_keys[0]->scale;
}

void AnimBone::DrawDebug(GameObject * bone) const
{
	CompTransform* comp_transform = bone->GetComponentTransform();
	float3 trans = comp_transform->GetGlobalTransform().TranslatePart();

	for (int i = 0; i < bone->GetNumChilds(); i++)
	{
		GameObject* temp_child = bone->GetChildbyIndex(i);
		CompTransform* child_comp_transform = temp_child->GetComponentTransform();
		float3 child_trans = child_comp_transform->GetGlobalTransform().TranslatePart();

		glBegin(GL_LINES);
		glVertex3f(trans.x, trans.y, trans.z);
		glVertex3f(child_trans.x, child_trans.y, child_trans.z);
		glEnd();
	}
}
