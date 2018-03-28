#ifndef _MODULE_ANIMATION_H_
#define _MODULE_ANIMATION_H_
#include "Globals.h"
#include "Module.h"


class AnimableComponent;
enum ParameterValue;
union AnimationValue;

struct KeyFrameData
{
	KeyFrameData(int key, AnimationValue value, ParameterValue parameter);
	KeyFrameData(int key, AnimationValue value);
	~KeyFrameData();

	std::vector<int> key_frames;
	std::vector<AnimationValue> key_values;
	ParameterValue parameter;
};
struct AnimData
{

	bool HaveKeyFrameData(const ParameterValue& data);
public:

	AnimableComponent* data = nullptr;
	std::vector<KeyFrameData> key_data;
};


struct AnimationJson
{
	std::string name;
	std::vector<AnimData> animations;

};


class ModuleAnimation : public Module
{
public:
	ModuleAnimation();
	virtual ~ModuleAnimation();

	void ShowAnimationWindow(bool& active);
	void FindAnimComponent();
	void SaveAnimation();
	void LoadAnimation();
	int HaveAnimData(const AnimableComponent* data);

private:
	bool new_animation_window = false;
	AnimationJson* animation_json = nullptr;
};

#endif //_MODULE_ANIMATION_H_