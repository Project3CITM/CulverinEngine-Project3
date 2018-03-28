#ifndef COMPONENT_UI_ANIMATION_H
#define COMPONENT_UI_ANIMATION_H
#include "Component.h"
#include <vector>
#include "ModuleAnimation.h"

class CompUIAnimation:public Component
{
public:
	CompUIAnimation(Comp_Type t, GameObject* parent);
	virtual ~CompUIAnimation();


	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompUIAnimation * component);
private:
	void PlayAnimation(float dt);
public:
	std::vector<AnimData> animations;
	bool play = false;
	bool on_execution = false;
	bool loop = false;
	int max_frames = 0;

};

#endif //COMPONENT_UI_ANIMATION_H