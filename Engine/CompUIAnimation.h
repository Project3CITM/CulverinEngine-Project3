#ifndef COMPONENT_UI_ANIMATION_H
#define COMPONENT_UI_ANIMATION_H
#include "Component.h"
#include <vector>
#include "ModuleAnimation.h"
struct AnimationJson;
class CompUIAnimation:public Component
{
public:
	CompUIAnimation(Comp_Type t, GameObject* parent);
	virtual ~CompUIAnimation();

	void Init();
	void Clear();

	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompUIAnimation * component);

private:
	bool PlayAnimation(float dt);
	bool ResetAnimation();

public:
	AnimationJson* animation_json = nullptr;
	bool play = false;
	bool on_execution = false;
	bool loop = false;
	int max_frames = 0;
	int current_frame = 0;
	float sample_rate = 0.0f;
	float current_time = 0.0f;
private:
	bool select_ui_animation = false;

};

#endif //COMPONENT_UI_ANIMATION_H