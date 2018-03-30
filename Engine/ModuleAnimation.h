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
	KeyData(int key_frame, AnimationValue key_values);
	~KeyData();

	int key_frame;
	AnimationValue key_values;
	bool operator<(const KeyData& compare) const; 
};

struct KeyFrameData
{

	KeyFrameData(int key, AnimationValue value, ParameterValue parameter);
	KeyFrameData(int key, AnimationValue value);
	~KeyFrameData();
	bool ShowKeyValue(int i);
	int ReturnKeyDataPos(int& selected);

	int max_keys = 20;
	bool invalid_key = false;
	std::vector<KeyData> key_data;
	ParameterValue parameter;

};
struct AnimData
{

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
	void FindAnimComponent();
	void SaveAnimation();
	void LoadAnimation();
	int HaveAnimData(const AnimableComponent* data);
private:
	bool new_animation_window = false;
	AnimationJson* animation_json = nullptr;
};

#endif //_MODULE_ANIMATION_H_