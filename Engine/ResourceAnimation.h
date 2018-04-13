#ifndef RESOURCE_ANIMATION_H
#define RESOURCE_ANIMATION_H

#include "Resource_.h"
#include "Math/float3.h"
#include "Math/Quat.h"
#include <vector>

class AnimationClip;
class GameObject;
class BlendingClip;

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

	~AnimBone();

	void UpdateBone(GameObject * bone, AnimationClip * playing_clip = nullptr, BlendingClip* blending_node_clip = nullptr, AnimationClip * blending_clip = nullptr) const;

	float3 GetPosition(AnimationClip* clip_vec, bool activated) const;
	Quat GetRotation(AnimationClip* clip_vec, bool activated) const;
	float3 GetScale(AnimationClip* clip_vec, bool activated) const;

	void DrawDebug(GameObject* bone) const;

public:
	std::string name;

	std::vector<PositionKey> position_keys;
	std::vector<RotationKey> rotation_keys;
	std::vector<ScaleKey> scale_keys;
};

class ResourceAnimation :public Resource
{
public:
	ResourceAnimation(uint uid);
	~ResourceAnimation();

	void InitInfo(const char* name, const char* path_assets, const char* path_library);
	bool LoadToMemory();
	void DeleteToMemory();

public:

	std::vector<AnimBone*> bones;
	float duration;
	std::string path_library;
	float ticks_per_sec;
};

#endif