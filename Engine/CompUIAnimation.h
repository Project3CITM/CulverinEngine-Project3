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
	CompUIAnimation(const CompUIAnimation & copy, GameObject * parent);
	virtual ~CompUIAnimation();

	void Init();
	void Clear();

	void Update(float dt);
	void ShowOptions();
	void ShowInspectorInfo();
	void CopyValues(const CompUIAnimation * component);

	void Save(JSON_Object * object, std::string name, bool saveScene, uint & countResources) const;
	void Load(const JSON_Object * object, std::string name);

	void GetOwnBufferSize(uint& buffer_size);
	void SaveBinary(char** cursor, int position) const;
	void LoadBinary(char** cursor);

private:
	bool PlayAnimation(float dt);
	bool ResetAnimation();
	void LoadAnimation(AnimationJson ** animation, const char * path, const char * name);

public:
	AnimationJson* animation_json = nullptr;
	bool play = false;
	bool on_execution = false;
	bool loop = false;
	int current_frame = 0;
	float current_time = 0.0f;
	float current_animation_time = 0.0f;
private:
	bool select_ui_animation = false;

};

#endif //COMPONENT_UI_ANIMATION_H