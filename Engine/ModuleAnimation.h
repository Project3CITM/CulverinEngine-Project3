#ifndef _MODULE_ANIMATION_H_
#define _MODULE_ANIMATION_H_
#include "Globals.h"
#include "Module.h"
#include <queue>
#include "AnimableComponent.h"
class AnimableComponent;
enum ParameterValue;
//union AnimationValue;

struct KeyData
{
	KeyData();
	KeyData(int key_frame, AnimationValue key_values);
	~KeyData();

	int key_frame=0;
	float key_on_time = 0.0f;

	AnimationValue key_values;
	bool operator<(const KeyData& compare) const; 
};

struct KeyFrameData
{
	KeyFrameData();
	KeyFrameData(int key, AnimationValue value, ParameterValue parameter);
	KeyFrameData(int key, AnimationValue value);
	~KeyFrameData();
	bool ShowKeyValue(int i);
	void ShowKeyOnTime();

	int ReturnKeyDataPos(int& selected);
	void UpdateKeyFrame();
	void UpdateSampleRate();
	void UpdateInterpolationKeys();
	void ResetInterpolationKeys();
	AnimationValue Interpolate(float time,int frame);

	int max_keys = 20;
	int sample_rate = 60;
	int initial=0;
	int destination=1;
	bool invalid_key = false;
	std::vector<KeyData> key_data;
	ParameterValue parameter;

};
struct AnimData
{
	AnimData();
	~AnimData();
	bool HaveKeyFrameData(const ParameterValue& data);
public:

	AnimableComponent* data = nullptr;

	std::vector<KeyFrameData> key_frame_data;
};


struct AnimationJson
{
	AnimationJson();
	std::string name;
	std::vector<AnimData> animations;

};


class ModuleAnimation : public Module
{
public:
	ModuleAnimation();
	virtual ~ModuleAnimation();

	void ShowAnimationWindow(bool& active);
	void ShowNewAnimationWindow();
	void ShowAnimationJsonInfo();
	void ShowAnimationDataInfo();
	void ShowAnimationKeyFrameInfo();
	AnimationJson* ShowAnimationJsonFiles(bool& active);
	void FindAnimComponent();
	void SaveAnimation();
	void LoadAnimation(AnimationJson** animation, const char* path);
	int HaveAnimData(const AnimableComponent* data);
private:

private:
	int max_keys = 20;
	int sample_rate = 60;
	bool select_animation_window = false;
	bool new_animation_window = false;
	AnimationJson* animation_json = nullptr;
};

#endif //_MODULE_ANIMATION_H_