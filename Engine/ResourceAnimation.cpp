#include "ResourceAnimation.h"

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
}

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

bool ResourceAnimation::LoadToMemory()
{
	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	return true;
}
