#ifndef COMPONENT_ANIMATION_H
#define COMPONENT_ANIMATION_H

#include "Component.h"
#include <vector>
#include "ResourceAnimation.h"

class GameObject;
class AnimBone;

enum AnimationState
{
	A_PLAY,
	A_STOP,
	A_PAUSED,
	A_BLENDING,
	A_NONE
};
class AnimationClip
{
public:
	std::string name = "Animation Clip";
	bool loop = true;
	float time = start_frame_time;
	float start_frame_time = 0.0f;
	float end_frame_time = 0.0f;

	float total_blending_time = 3.0f;
	float current_blending_time = total_blending_time;

	bool finished = true;
	AnimationState state = A_STOP;

	void RestartAnimationClip();
};

class CompAnimation : public Component
{
public:

	CompAnimation(Comp_Type t, GameObject* parent);
	CompAnimation(const CompAnimation& copy, GameObject* parent);
	~CompAnimation();

	void Draw();
	void Clear();
	void PreUpdate(float dt);
	void Update(float dt);

	// EDITOR METHODS ---------
	void ShowOptions();
	void ShowInspectorInfo();
	void ShowAnimationInfo();
	// ------------------------



	void SetResource(ResourceAnimation * resource_animation, bool isImport = false);
	void CopyValues(const CompAnimation * component);

	// SAVE - LOAD METHODS ----------------
	void Save(JSON_Object* object, std::string name, bool saveScene, uint& countResources) const;
	void Load(const JSON_Object* object, std::string name);
	// -------------------------------------

	void CreateAnimationClip();
	void ManageAnimationClips(AnimationClip* animation_clip, float dt);

public:

	ResourceAnimation* animation_resource = nullptr;

private:

	bool select_animation = false;
	std::vector<AnimationClip*> animation_clips;

	AnimationClip* current_animation = nullptr;
	AnimationClip* blending_animation = nullptr;

	std::vector<std::pair<GameObject*, const AnimBone*>> bone_update_vector;
	bool debug_draw = false;

	bool bones_placed = false;
};

#endif
