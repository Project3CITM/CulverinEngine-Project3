#ifndef COMPONENT_ANIMATION_H
#define COMPONENT_ANIMATION_H

#include "Component.h"
#include <vector>
#include "ResourceAnimation.h"

class GameObject;
class AnimBone;
class AnimationTransition;

enum AnimationState
{
	A_PLAY,
	A_STOP,
	A_PAUSED,
	A_BLENDING,
	A_BLENDING_NODE,
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
	float speed_factor = 1.0f;

	float total_blending_time = 0.666f;
	float current_blending_time = total_blending_time;

	bool finished = true;
	AnimationState state = A_STOP;
	
	bool IsAnimOverXTime(float num_between_0_and_1);
	void RestartAnimationClip();
};
class BlendingClip
{
public:
	std::string name = "Blending Clip";
	AnimationClip* clip = nullptr;
	bool active = false;
	float weight = 0.0f;
};
class AnimationNode
{
public:
	~AnimationNode();

	void CreateTransition();

	void CreateBlendingClip();

	void SetActiveBlendingClip(BlendingClip * blnd_clip);
	void SetActiveBlendingClip(std::string name);
	BlendingClip* GetActiveBlendingClip();
	void SetActiveBlendingClipWeight(float weight);

	AnimationClip* clip = nullptr;
	bool active = false;
	std::string name = "Animation Node ";
	std::vector<BlendingClip*> blending_clips;
	std::string anim_audio = "Null_Audio";
	float audio_time = 0.0f;
	std::vector<AnimationTransition*> transitions;
	std::string anim_prefab_particle = "Null_Prefab";
	float prefab_particle_time = 0.0f;
	float3 prefab_pos = { 0, 0, 0 };

	bool to_delete = false;
};

class AnimationTransition
{
public:
	std::string name = "Transition ";

	AnimationNode* destination = nullptr;

	bool condition = false;

	bool has_exit_time = false;
	float exit_time = 0.0f;

	bool to_delete = false;
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

	void PlayAnimation(AnimationNode* node);
	void PlayClip(const char* clip_name, bool blending = true);
	void SetNode(const char* node_name);
	void SetTransition(const char* transition_name, bool value = true);

	bool IsAnimationStopped(const char* clip_name) const;
	bool IsAnimationRunning(const char* clip_name) const;

	AnimationClip* GetCurrentClip() const;
	AnimationClip* GetClipFromName(std::string name) const;
	AnimationNode* GetNodeFromName(std::string name) const;
	AnimationTransition* GetTransitionFromName(std::string name, AnimationNode* node) const;
	AnimationClip* GetBlendingClip() const;
	AnimationNode* GetActiveNode()const;

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
	void ManageActualAnimationNode(float dt);
	void CreateAnimationNode();
	void CheckNodesConditions(AnimationNode* node);

	void SetClipsSpeed(float speed_value);

	void SetActiveAnimationNode(AnimationNode * active);
public:

	ResourceAnimation* animation_resource = nullptr;

private:

	bool select_animation = false;
	std::vector<AnimationClip*> animation_clips;
	std::vector<AnimationNode*> animation_nodes;

	AnimationClip* current_animation = nullptr;
	AnimationClip* blending_animation = nullptr;
	AnimationNode* active_node = nullptr;

	std::vector<std::pair<GameObject*, const AnimBone*>> bone_update_vector;
	bool debug_draw = false;

	bool bones_placed = false;

	bool show_asm_window = false;

	bool show_bone_transformations = false;
	bool playing = false;
};

#endif
