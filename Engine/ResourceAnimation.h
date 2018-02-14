#ifndef RESOURCE_ANIMATION_H
#define RESOURCE_ANIMATION_H

#include "Resource_.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include <vector>

struct PositionKey
{
	float time;
	float3 position;
};

struct RotationKey
{
	float time;
	Quat rotation;
};

struct ScaleKey
{
	float time;
	float3 scale;
};

class AnimBone
{
public:
	//void UpdateBone(GameObject* gameobject, std::vector<AnimationClip>& clip_vec) const;
	~AnimBone();

	//float3 GetPosition(AnimationClip clip_vec) const;
	//Quat GetRotation(AnimationClip* clip_vec) const;
	//float3 GetScale(AnimationClip* clip_vec) const;

public:
	std::string name;

	std::vector<PositionKey*> position_keys;
	std::vector<RotationKey*> rotation_keys;
	std::vector<ScaleKey*> scale_keys;
};

class ResourceAnimation :public Resource
{
public:
	ResourceAnimation(uint uid);
	~ResourceAnimation();

	void InitInfo(const char* name, const char* path);
	bool LoadToMemory();
	bool UnloadFromMemory();

public:

	std::vector<AnimBone*> bones;
	float duration;
	float ticks_per_sec;
};

#endif